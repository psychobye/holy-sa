package com.lit.game.gui.cef

import android.annotation.SuppressLint
import android.graphics.Color
import android.util.Log
import android.view.View
import android.webkit.WebView
import android.webkit.WebViewClient
import android.webkit.JavascriptInterface
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.LayoutCefRootBinding
import com.lit.game.gui.NativeGui

// !TODO: SetFocus, sendServerEventToWebView
class CEFManager : NativeGui<LayoutCefRootBinding>(LayoutCefRootBinding::class) {
    private val jsCallbacks = mutableMapOf<String, (String) -> Unit>()
    private var isWebViewReady = false
    private var pendingUrl: String? = null

    fun initBrowser(url: String) {
        activity.runOnUiThread {
            try {
                if (!isWebViewReady) setupWebView()
                binding.webView.loadUrl(url)
            } catch (_: Throwable) {
                pendingUrl = url
            }
        }
    }

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
            web.addJavascriptInterface(JsBridge(), "CefBridge")
            isWebViewReady = true

            pendingUrl?.let {
                web.loadUrl(it)
                pendingUrl = null
            }
        }
    }

    fun showBrowser() {
        activity.runOnUiThread { binding.root.visibility = View.VISIBLE }
    }

    fun hideBrowser() {
        activity.runOnUiThread { binding.root.visibility = View.GONE }
    }

    fun setBrowserUrl(url: String) {
        activity.runOnUiThread { binding.webView.loadUrl(url) }
    }

    fun setSize(scale: Float) {
        activity.runOnUiThread {
            val root = binding.root
            val web = binding.webView

            val clampedScale = scale.coerceIn(0.0f, 1.0f)

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

    fun sendEvent(event: String, json: String) {
        binding.webView.post {
            val js = "window.Cef && window.Cef._trigger('${event.replace("'", "\\'")}', ${json});"
            try { binding.webView.evaluateJavascript(js, null) } catch (_: Throwable) {}
        }
    }

    fun registerCallback(event: String, callback: (String) -> Unit) {
        jsCallbacks[event] = callback
    }

    inner class JsBridge {
        @JavascriptInterface
        fun sendEvent(event: String, json: String) {
            jsCallbacks[event]?.invoke(json)
        }

        @JavascriptInterface
        fun updateInteractiveAreas(jsonRects: String) {
            activity.runOnUiThread {
                (binding.root as? CefFrameLayout)?.updateInteractiveRects(jsonRects)
            }
        }
    }

    override fun receivePacket(actionId: Int, json: String) { }
}