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
       // val currentTextColor = currentTextColor

        paint.style = Paint.Style.STROKE
        paint.strokeWidth = 4f

        text = spannableString.toString()

        setTextColor(Color.BLACK)
        super.onDraw(canvas)

        // Revert the color back to the one initially specified
       // setTextColor(currentTextColor)
        paint.style = Paint.Style.FILL

        text = spannableString

        super.onDraw(canvas)

    }
}