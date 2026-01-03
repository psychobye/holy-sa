package com.lit.game.gui.cef

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.RectF
import android.util.AttributeSet
import android.view.MotionEvent
import android.widget.FrameLayout

class CefFrameLayout @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : FrameLayout(context, attrs, defStyleAttr) {

    private val interactiveRects = mutableListOf<RectF>()

    private val density = context.resources.displayMetrics.density

    fun updateInteractiveRects(rectsJson: String) {
        // TODO: Gson
        interactiveRects.clear()
        try {
            val clean = rectsJson.replace("[", "").replace("]", "")
            if (clean.isBlank()) return

            val numbers = clean.split(",").map { it.trim().toFloatOrNull() ?: 0f }

            for (i in numbers.indices step 4) {
                if (i + 3 < numbers.size) {
                    val x = numbers[i] * density
                    val y = numbers[i+1] * density
                    val w = numbers[i+2] * density
                    val h = numbers[i+3] * density
                    interactiveRects.add(RectF(x, y, x + w, y + h))
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun onInterceptTouchEvent(ev: MotionEvent?): Boolean {
        if (ev == null) return false

        var isHitWebView = false
        for (rect in interactiveRects) {
            if (rect.contains(ev.x, ev.y)) {
                isHitWebView = true
                break
            }
        }

        return !isHitWebView
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        return false
    }
}