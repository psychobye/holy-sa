package com.holy.game.gui.util

import android.content.Context
import android.graphics.*
import android.text.TextPaint
import android.util.AttributeSet
import android.util.TypedValue
import androidx.appcompat.widget.AppCompatTextView
import com.holy.game.R
import kotlin.math.ceil
import kotlin.math.max

class GradientStrokeTextView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = android.R.attr.textViewStyle
) : AppCompatTextView(context, attrs, defStyleAttr) {

    private var strokeColor: Int = Color.WHITE
    private var strokeWidthDp: Float = 2.05f
    private var topColor: Int = Color.WHITE
    private var bottomColor: Int = Color.parseColor("#323232")
    private var topStop: Float = 0.52f
    private var bottomStop: Float = 1.0f
    private var letterSpacingCustom: Float = -0.5f

    private val strokePaint = TextPaint()
    private val fillPaint = TextPaint()

    init {
        attrs?.let {
            val a = context.obtainStyledAttributes(it, R.styleable.GradientStrokeTextView)
            strokeColor = a.getColor(R.styleable.GradientStrokeTextView_strokeColor, strokeColor)
            strokeWidthDp = a.getFloat(R.styleable.GradientStrokeTextView_strokeWidthDp, strokeWidthDp)
            topColor = a.getColor(R.styleable.GradientStrokeTextView_topColor, topColor)
            bottomColor = a.getColor(R.styleable.GradientStrokeTextView_bottomColor, bottomColor)
            topStop = a.getFloat(R.styleable.GradientStrokeTextView_topStop, topStop)
            bottomStop = a.getFloat(R.styleable.GradientStrokeTextView_bottomStop, bottomStop)
            letterSpacingCustom = a.getFloat(R.styleable.GradientStrokeTextView_letterSpacingCustom, letterSpacingCustom)
            a.recycle()
        }
        this.letterSpacing = letterSpacingCustom

        includeFontPadding = true

        val base = paint
        fillPaint.set(base)
        strokePaint.set(base)

        fillPaint.isAntiAlias = true
        strokePaint.isAntiAlias = true

        strokePaint.strokeWidth = dpToPx(strokeWidthDp)
        strokePaint.style = Paint.Style.STROKE
        strokePaint.color = strokeColor

        fillPaint.style = Paint.Style.FILL
    }

    private fun dpToPx(dp: Float): Float =
        TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dp, resources.displayMetrics)

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)

        val contentHeight = max(1, h - paddingTop - paddingBottom)
        val colors = intArrayOf(topColor, bottomColor)
        val positions = floatArrayOf(topStop.coerceIn(0f,1f), bottomStop.coerceIn(0f,1f))

        fillPaint.shader = LinearGradient(
            0f,
            0f,
            0f,
            contentHeight.toFloat(),
            colors,
            positions,
            Shader.TileMode.CLAMP
        )
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val extra = ceil(strokePaint.strokeWidth.toDouble()).toInt()
        val newHeight = measuredHeight + extra * 2
        setMeasuredDimension(measuredWidth, newHeight)
    }

    override fun onDraw(canvas: Canvas) {
        val tvLayout = layout ?: run {
            super.onDraw(canvas)
            return
        }

        val textStr = text?.toString() ?: ""
        val lineCount = tvLayout.lineCount

        canvas.save()
        canvas.translate(scrollX.toFloat(), scrollY.toFloat())

        for (i in 0 until lineCount) {
            val lineStart = tvLayout.getLineStart(i)
            val lineEnd = tvLayout.getLineEnd(i)
            val lineText = textStr.substring(lineStart, lineEnd)

            val x = paddingLeft + tvLayout.getLineLeft(i)
            val y = tvLayout.getLineBaseline(i).toFloat() + paddingTop

            canvas.drawText(lineText, x, y, strokePaint)
            canvas.drawText(lineText, x, y, fillPaint)
        }

        canvas.restore()
    }

    fun setStrokeColor(color: Int) {
        strokeColor = color
        strokePaint.color = color
        invalidate()
    }

    fun setStrokeWidthDp(dp: Float) {
        strokeWidthDp = dp
        strokePaint.strokeWidth = dpToPx(dp)
        requestLayout()
        invalidate()
    }

    fun setGradientColors(top: Int, bottom: Int, topStop: Float = 0.52f, bottomStop: Float = 1f) {
        topColor = top
        bottomColor = bottom
        this.topStop = topStop.coerceIn(0f, 1f)
        this.bottomStop = bottomStop.coerceIn(0f, 1f)

        if (height > 0 && width > 0) {
            createGradientShader(height - paddingTop - paddingBottom)
            invalidate()
        } else {
            post {
                createGradientShader(height - paddingTop - paddingBottom)
                invalidate()
            }
        }
    }

    fun setLetterSpacingCustom(ls: Float) {
        letterSpacingCustom = ls
        letterSpacing = ls
    }

    private fun createGradientShader(viewContentHeight: Int) {
        val h = max(1, viewContentHeight)
        val colors = intArrayOf(topColor, bottomColor)
        val positions = floatArrayOf(topStop, bottomStop)

        fillPaint.shader = LinearGradient(
            0f, 0f, 0f, h.toFloat(),
            colors, positions, Shader.TileMode.CLAMP
        )
    }
}