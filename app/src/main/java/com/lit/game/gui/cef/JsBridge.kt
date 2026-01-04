package com.lit.game.gui.cef

import android.util.Log
import android.webkit.JavascriptInterface
import com.lit.game.core.Samp.Companion.activity

/**
 * Bridge for JavaScript to call Kotlin functions.
 */
class JsBridge(private val manager: CefManager) {

    companion object {
        private const val TAG = "CEF_JS"
    }

    @JavascriptInterface
    fun cefReady() {
        activity.runOnUiThread {
            Log.d(TAG, "CEF Ready")
            manager.isCefReady = true
            while (manager.jsEventQueue.isNotEmpty()) {
                val (event, json) = manager.jsEventQueue.removeFirst()
                manager.evaluateJs(event, json)
            }
        }
    }

    @JavascriptInterface
    fun sendClientEvent(event: String, json: String) {
        Log.d(TAG, "sendClientEvent: $event | $json")
        activity.runOnUiThread { manager.onClientEvent(event, json) }
    }

    @JavascriptInterface
    fun sendServerEvent(event: String, json: String) {
        Log.d(TAG, "sendServerEvent: $event | $json")
        activity.runOnUiThread { manager.sendServerEventToJavaScript(event, json) }
    }

    @JavascriptInterface
    fun updateInteractiveAreas(jsonRects: String) {
        Log.d(TAG, "updateInteractiveAreas: $jsonRects")
        activity.runOnUiThread {
            (manager.webBinding.root as? CefFrameLayout)?.updateInteractiveRects(jsonRects)
        }
    }

    @JavascriptInterface
    fun log(message: String) {
        Log.d(TAG, message)
    }

    @JavascriptInterface
    fun error(message: String) {
        Log.e(TAG, message)
    }
}