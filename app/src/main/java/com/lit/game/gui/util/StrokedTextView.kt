package com.lit.game.gui.util

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import androidx.appcompat.widget.AppCompatTextView

class StrokedTextView @JvmOverloads constructor(context: Context?, attrs: AttributeSet? = null, defStyle: Int = 0) : AppCompatTextView(context!!, attrs, defStyle) {

    override fun onDraw(canvas: Canvas) {
        val spannableString = text

        paint.style = Paint.Style.STROKE
        paint.strokeWidth = 3f

        text = spannableString.toString()

        setTextColor(Color.BLACK)
        super.onDraw(canvas)

        paint.style = Paint.Style.FILL

        text = spannableString

        super.onDraw(canvas)
    }
}