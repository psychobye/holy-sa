package com.holy.game.gui.cef

import android.annotation.SuppressLint
import android.graphics.Color
import android.util.Log
import android.view.View
import android.webkit.WebViewClient
import com.google.gson.Gson
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.LayoutCefRootBinding
import com.holy.game.gui.NativeGui
import com.holy.game.gui.util.Utils
import com.holy.game.gui.util.Utils.bitmapToByteBuffer

/**
 * Manager for CEF-like WebView with Gson-based JS <-> Kotlin communication.
 */
class CefManager : NativeGui<LayoutCefRootBinding>(LayoutCefRootBinding::class) {

    /** Gson instance for JSON serialization */
    private val gson = Gson()

    /** Native function to send events to the server */
    private external fun nativeSendEvent(event: String, json: String)
    /** Native function to send bitmap bytes to rwtexture */
    private external fun nativeUploadBytes(id: Int, texName: String, buffer: java.nio.ByteBuffer, width: Int, height: Int)

    private val inflightTargets = mutableMapOf<String, com.bumptech.glide.request.target.CustomTarget<android.graphics.Bitmap>>()
    private val loadingLock = java.util.concurrent.ConcurrentHashMap<String, java.util.concurrent.atomic.AtomicBoolean>()

    /** JS callbacks map */
    internal val jsCallbacks = mutableMapOf<String, (String) -> Unit>()

    /** Queue of JS events waiting for CEF readiness */
    internal val jsEventQueue = ArrayDeque<Pair<String, String>>()

    /** True if JS environment is ready */
    internal var isCefReady = false

    /** WebView state */
    private var isWebViewReady = false
    private var pendingUrl: String? = null

    /** JS bridge instance */
    val jsBridge = JsBridge(this)

    /** Access to binding for JSBridge */
    internal val webBinding: LayoutCefRootBinding get() = binding

    /** Send event to server */
    internal fun sendNativeEvent(event: String, json: String) {
        nativeSendEvent(event, json)
    }

    /** LogCat Tag */
    private val TAG = "CEF"

    /** Init WebView with optional URL */
    fun initBrowser(url: String) {
        activity.runOnUiThread {
            if (!isWebViewReady) setupWebView()
            try { binding.webView.loadUrl(url) } catch (_: Throwable) { pendingUrl = url }
        }
    }

    /** WebView setup with JS bridge */
    @SuppressLint("SetJavaScriptEnabled")
    fun setupWebView() {
        if (isWebViewReady) return
        val web = binding.webView
        activity.runOnUiThread {
            web.webViewClient = WebViewClient()
            web.settings.javaScriptEnabled = true
            web.settings.domStorageEnabled = true
            web.settings.allowFileAccess = true
            web.setBackgroundColor(Color.TRANSPARENT)
            web.setLayerType(View.LAYER_TYPE_HARDWARE, null)
            web.addJavascriptInterface(jsBridge, "CefBridge")
            isWebViewReady = true
            pendingUrl?.let { web.loadUrl(it); pendingUrl = null }
        }
    }

    fun showBrowser() = activity.runOnUiThread { binding.root.visibility = View.VISIBLE }
    fun hideBrowser() = activity.runOnUiThread { binding.root.visibility = View.GONE }
    fun setBrowserUrl(url: String) = activity.runOnUiThread { binding.webView.loadUrl(url) }

    fun setSize(scale: Float) {
        activity.runOnUiThread {
            val root = binding.root
            val web = binding.webView
            val clampedScale = scale.coerceIn(0f, 1f)
            root.post {
                val parentWidth = root.width
                val parentHeight = root.height
                val newWidth = (parentWidth * clampedScale).toInt()
                val newHeight = (parentHeight * clampedScale).toInt()
                val marginLeft = (parentWidth - newWidth) / 2
                val marginTop = (parentHeight - newHeight) / 2
                val params = web.layoutParams as? android.widget.FrameLayout.LayoutParams
                    ?: android.widget.FrameLayout.LayoutParams(newWidth, newHeight)
                params.width = newWidth
                params.height = newHeight
                params.leftMargin = marginLeft
                params.topMargin = marginTop
                web.layoutParams = params
            }
        }
    }

    /** Called from JS */
    fun onClientEvent(event: String, json: String) {
        try {
            val wrapped = gson.toJson(JsEvent(event, gson.fromJson(json, Any::class.java)))
            sendNativeEvent(event, wrapped)
        } catch (e: Exception) {
            Log.e(TAG, "FAILED JSON: '$json'")
        }
        jsCallbacks[event]?.invoke(json)
    }

    /** Send event to JS */
    public fun sendServerEventToJavaScript(event: String, json: String) {
        activity.runOnUiThread {
            if (isCefReady) evaluateJs(event, json)
            else jsEventQueue.add(event to json)
        }
    }

    fun evaluateJs(event: String, json: String) {
        val escapedJson = json.replace("'", "\\'")
        val escapedEvent = event.replace("'", "\\'")
        val js = "window.Cef && window.Cef._trigger('$escapedEvent', '$escapedJson');"
        try { binding.webView.evaluateJavascript(js, null) }
        catch (e: Throwable) { Log.e("CEF", "JS Eval Error: ${e.message}") }
    }

    override fun receivePacket(actionId: Int, json: String) { }

    /** Helper data class for wrapping JS events */
    data class JsEvent(val event: String, val data: Any)

    /** CEF3D */
    @SuppressLint("CheckResult")
    fun fetchTexture(id: Int, url: String, texName: String) {
        val lockKey = "${id}_$texName"

        val flag = loadingLock.getOrPut(lockKey) { java.util.concurrent.atomic.AtomicBoolean(false) }
        if (!flag.compareAndSet(false, true)) return

        activity.runOnUiThread {
            val target = object : com.bumptech.glide.request.target.CustomTarget<android.graphics.Bitmap>() {
                override fun onResourceReady(
                    resource: android.graphics.Bitmap,
                    transition: com.bumptech.glide.request.transition.Transition<in android.graphics.Bitmap>?
                ) {
                    try {
                        val safeBmp = resource.copy(android.graphics.Bitmap.Config.ARGB_8888, false)
                        val w = safeBmp.width
                        val h = safeBmp.height

                        val buf = bitmapToByteBuffer(safeBmp, Utils.PixelOrder.BGRA)
                        buf.rewind()

                        nativeUploadBytes(id, texName, buf, w, h)

                        safeBmp.recycle()
                        Log.d(TAG, "fetchTexture: sent ID:$id Name:$texName to native (W:$w H:$h)")
                    } catch (t: Throwable) {
                        Log.e(TAG, "fetchTexture error: ${t.message}")
                    } finally {
                        flag.set(false)
                        inflightTargets.remove(lockKey)
                    }
                }

                override fun onLoadCleared(placeholder: android.graphics.drawable.Drawable?) {
                    flag.set(false)
                    inflightTargets.remove(lockKey)
                }

                override fun onLoadFailed(errorDrawable: android.graphics.drawable.Drawable?) {
                    flag.set(false)
                    inflightTargets.remove(lockKey)
                    Log.e(TAG, "fetchTexture: Glide failed for $url")
                }
            }

            inflightTargets[lockKey] = target

            com.bumptech.glide.Glide.with(activity)
                .asBitmap()
                .load(url)
                .centerCrop()
                .override(1024, 1024)
                .into(target)
        }
    }
}