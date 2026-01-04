package com.lit.game.gui.cef

import android.annotation.SuppressLint
import android.graphics.Color
import android.util.Log
import android.view.View
import android.webkit.WebViewClient
import com.google.gson.Gson
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.LayoutCefRootBinding
import com.lit.game.gui.NativeGui

/**
 * Manager for CEF-like WebView with Gson-based JS ↔ Kotlin communication.
 */
class CefManager : NativeGui<LayoutCefRootBinding>(LayoutCefRootBinding::class) {

    /** Gson instance for JSON serialization */
    private val gson = Gson()

    /** Native function to send events to the server */
    private external fun nativeSendEvent(event: String, json: String)

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

    /** Init WebView with optional URL */
    fun initBrowser(url: String) {
        activity.runOnUiThread {
            if (!isWebViewReady) setupWebView()
            try { binding.webView.loadUrl(url) } catch (_: Throwable) { pendingUrl = url }
        }
    }

    private val TAG = "CEF"

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
}