package com.lit.game.gui.util

import android.animation.Animator
import android.animation.AnimatorListenerAdapter
import android.annotation.SuppressLint
import android.content.Context
import android.text.Html
import android.text.Spanned
import android.util.DisplayMetrics
import android.view.MotionEvent
import android.view.View
import android.view.WindowManager
import android.view.animation.DecelerateInterpolator
import android.view.animation.OvershootInterpolator
import com.lit.game.core.Samp
import java.util.regex.Pattern

object Utils {

    private val screenSizeInternal: Pair<Int, Int> by lazy { calculateScreenSize() }

    val screenSize: Pair<Int, Int>
        get() = screenSizeInternal

    private fun calculateScreenSize(): Pair<Int, Int> {
        val displayMetrics = DisplayMetrics()
        val windowManager = Samp.activity.getSystemService(Context.WINDOW_SERVICE) as WindowManager
        windowManager.defaultDisplay.getRealMetrics(displayMetrics)

        return Pair(displayMetrics.widthPixels, displayMetrics.heightPixels)
    }

    fun pxFromDp(dp: Float): Float {
        return dp * Samp.activity.resources.displayMetrics.density
    }

    @JvmStatic
    fun ShowLayout(view: View?, isAnim: Boolean) {
        if (view != null) {
            if (isAnim) {
                view.alpha = 0.0f
                fadeIn(view)
            } else {
                view.alpha = 1.0f
            }
            view.visibility = View.VISIBLE
            view.invalidate()
        }
    }

    @JvmStatic
    fun HideLayout(view: View?, isAnim: Boolean) {
        if (view != null) {
            if (isAnim) {
                fadeOut(view)
                return
            }
            view.alpha = 0.0f
            view.visibility = View.GONE
            view.invalidate()
        }
    }

    private fun fadeIn(view: View?) {
        view?.animate()?.setDuration(150)?.setListener(object : AnimatorListenerAdapter() {
            override fun onAnimationEnd(animation: Animator) {
                super.onAnimationEnd(animation)
            }
        })?.alpha(1.0f)
    }

    private fun fadeOut(view: View?) {
        view?.animate()?.setDuration(150)?.setListener(object : AnimatorListenerAdapter() {
            override fun onAnimationEnd(animation: Animator) {
                view.visibility = View.GONE
                super.onAnimationEnd(animation)
            }
        })?.alpha(0.0f)
    }

    @JvmStatic
    fun transfromColors(inputText: String): Spanned {
        val pattern = Pattern.compile("\\{(.{6})\\}([^\\{]*)")
        val matcher = pattern.matcher(inputText)
        val spannableStringBuilder = StringBuilder()
        var currentIndex = 0

        while (matcher.find()) {
            val colorHex = matcher.group(1)
            val textToColor = matcher.group(2)

            val startIndex = matcher.start()
            val endIndex = matcher.end()

            spannableStringBuilder.append(inputText.substring(currentIndex, startIndex))
            spannableStringBuilder.append("<font color='#$colorHex'>$textToColor</font>")
            currentIndex = endIndex
        }

        spannableStringBuilder.append(inputText.substring(currentIndex))

        return Html.fromHtml(spannableStringBuilder.toString().replace("\n", "<br>"))
    }

    @SuppressLint("ClickableViewAccessibility")
    fun addPressScaleAnimation(
        v: View,
        scaleDown: Float = 0.9f,
        duration: Long = 90L,
        onClick: (() -> Unit)? = null
    ) {
        v.isClickable = true
        v.setOnTouchListener { view, event ->
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    view.animate().cancel()
                    view.animate()
                        .scaleX(scaleDown)
                        .scaleY(scaleDown)
                        .setDuration(duration)
                        .setInterpolator(DecelerateInterpolator())
                        .start()
                    false
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                    view.animate().cancel()
                    view.animate()
                        .scaleX(1f)
                        .scaleY(1f)
                        .setDuration(120)
                        .setInterpolator(OvershootInterpolator(1.2f))
                        .withEndAction {
                            if (event.action == MotionEvent.ACTION_UP) {
                                onClick?.invoke()
                            }
                        }
                        .start()
                    false
                }
                else -> false
            }
        }
    }
}