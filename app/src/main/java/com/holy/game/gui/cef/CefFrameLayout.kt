package com.holy.game.gui.cef

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.RectF
import android.util.AttributeSet
import android.view.MotionEvent
import android.widget.FrameLayout
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken

class CefFrameLayout @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : FrameLayout(context, attrs, defStyleAttr) {

    private val interactiveRects = mutableListOf<RectF>()
    private val density = context.resources.displayMetrics.density
    private val gson = Gson()

    /**
     * Updates the list of interactive rectangles from JSON.
     * Expected format: [[x, y, width, height], ...]
     */
    fun updateInteractiveRects(rectsJson: String) {
        interactiveRects.clear()
        try {
            val type = object : TypeToken<List<List<Float>>>() {}.type
            val rects: List<List<Float>> = gson.fromJson(rectsJson, type)

            for (r in rects) {
                if (r.size >= 4) {
                    val x = r[0] * density
                    val y = r[1] * density
                    val w = r[2] * density
                    val h = r[3] * density
                    interactiveRects.add(RectF(x, y, x + w, y + h))
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun onInterceptTouchEvent(ev: MotionEvent?): Boolean {
        if (ev == null) return false

        for (rect in interactiveRects) {
            if (rect.contains(ev.x, ev.y)) return false
        }
        return true
    }

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean = false
}