package com.holy.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.os.CountDownTimer
import android.view.GestureDetector
import android.view.GestureDetector.SimpleOnGestureListener
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup.MarginLayoutParams
import android.widget.Button
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.ProgressBar
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.rommansabbir.animationx.Bounce
import com.rommansabbir.animationx.animationXBounce
import kotlin.math.abs

class Notification {
    var constraintLayout: ConstraintLayout
    var br_not_bg: FrameLayout
    var br_not_fl: FrameLayout
    var br_not_view: View
    var br_not_icon: ImageView
    var br_title_text: FrameLayout
    var br_title_text_title: TextView
    var br_title_text_text: TextView
    var br_not_text: TextView
    var br_not_text2: TextView
    var br_not_firstbutton: Button
    var br_not_secondbutton: Button
    var br_not_progress: ProgressBar

    external fun sendClick(actionId: Int, buttonId: Int)
    external fun init()
    var countDownTimer: CountDownTimer? = null
    fun ShowNotification(type: Int, text: String?, duration: Int, actionId: Int, butt1: String?, butt2: String?) {
        activity.runOnUiThread {

            if (duration != 0) {
                br_not_progress.max = duration * 990
                br_not_progress.progress = duration * 990
            } else {
                br_not_progress.max = 100
                br_not_progress.progress = 100
            }
            when (type) {
                0 -> {
                    val progressbar = activity.getDrawable(R.drawable.notify_progressbar_red)
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#9F0A0A"))
                    //   br_not_bg.setBackgroundResource(R.drawable.notify_background_red);
                    br_not_progress.progressDrawable = progressbar
                    br_not_firstbutton.visibility = View.GONE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.VISIBLE
                    setMargins(br_title_text, 0, 2, 25, 12)
                    br_title_text.visibility = View.GONE
                    br_not_text2.visibility = View.VISIBLE
                    br_not_text.visibility = View.GONE
                    br_not_view.setBackgroundResource(R.drawable.notify_error_bg)
                    br_not_icon.setImageResource(R.drawable.notify_error)
                    br_not_text2.text = text
                }

                1 -> {
                    val progressbar1 = activity.getDrawable(R.drawable.notify_progressbar_green)
                    // br_not_bg.setBackgroundResource(R.drawable.notify_background_green);
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#025200"))
                    br_not_progress.progressDrawable = progressbar1
                    br_not_firstbutton.visibility = View.GONE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.VISIBLE
                    setMargins(br_not_text, 0, 2, 25, 12)
                    br_title_text.visibility = View.GONE
                    br_not_text2.visibility = View.GONE
                    br_not_text.visibility = View.VISIBLE
                    br_not_view.setBackgroundResource(R.drawable.notify_ruble_bg)
                    br_not_icon.setImageResource(R.drawable.notify_ruble)
                    br_not_text.text = text
                }

                2 -> {
                    val progressbar2 = activity.getDrawable(R.drawable.notify_progressbar_red)
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#9F0A0A"))
                    //  br_not_bg.setBackgroundResource(R.drawable.notify_background_red);
                    br_not_progress.progressDrawable = progressbar2
                    br_not_firstbutton.visibility = View.GONE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.VISIBLE
                    setMargins(br_not_text, 0, 2, 25, 12)
                    br_title_text.visibility = View.GONE
                    br_not_text2.visibility = View.GONE
                    br_not_text.visibility = View.VISIBLE
                    br_not_view.setBackgroundResource(R.drawable.notify_ruble_bg)
                    br_not_icon.setImageResource(R.drawable.notify_ruble)
                    br_not_text.text = text
                }

                3 -> {
                    val progressbar3 = activity.getDrawable(R.drawable.notify_progressbar_green)
                    //  br_not_bg.setBackgroundResource(R.drawable.notify_background_green);
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#025200"))
                    br_not_progress.progressDrawable = progressbar3
                    br_not_firstbutton.visibility = View.GONE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.VISIBLE
                    setMargins(br_title_text, 0, 2, 25, 12)
                    br_title_text.visibility = View.GONE
                    br_not_text2.visibility = View.VISIBLE
                    br_not_text.visibility = View.GONE
                    br_not_view.setBackgroundResource(R.drawable.notify_success_bg)
                    br_not_icon.setImageResource(R.drawable.notify_success)
                    br_not_text2.text = text
                }

                4 -> {
                    val progressbar4 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
                    // br_not_bg.setBackgroundResource(R.drawable.notify_background_blue);
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#003752"))
                    br_not_progress.progressDrawable = progressbar4
                    br_not_firstbutton.visibility = View.VISIBLE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.GONE
                    setMargins(br_not_text, 25, 2, 25, 12)
                    br_title_text.visibility = View.GONE
                    br_not_text2.visibility = View.GONE
                    br_not_text.visibility = View.VISIBLE
                    br_not_text.text = text
                    br_not_firstbutton.text = ">>"
                }

                5 -> {
                    val progressbar5 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
                    // br_not_bg.setBackgroundResource(R.drawable.notify_background_blue);
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#003752"))
                    br_not_progress.progressDrawable = progressbar5
                    br_not_firstbutton.visibility = View.VISIBLE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.GONE
                    setMargins(br_title_text, 25, 2, 25, 12)
                    br_title_text.visibility = View.VISIBLE
                    br_not_text2.visibility = View.GONE
                    br_not_text.visibility = View.GONE
                    br_title_text_title.text = text
                    br_title_text_text.text = "Нажмите, чтобы войти"
                    br_not_firstbutton.text = "Войти"
                }

                6 -> {
                    val progressbar6 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#003752"))
                    //  br_not_bg.setBackgroundResource(R.drawable.notify_background_blue);
                    br_not_progress.progressDrawable = progressbar6
                    br_not_firstbutton.visibility = View.VISIBLE
                    br_not_secondbutton.visibility = View.VISIBLE
                    br_not_fl.visibility = View.GONE
                    setMargins(br_title_text, 25, 2, 25, 12)
                    br_title_text.visibility = View.VISIBLE
                    br_not_text2.visibility = View.GONE
                    br_not_text.visibility = View.GONE
                    br_title_text_title.text = "Поступило предложение"
                    br_title_text_text.text = text
                    br_not_firstbutton.text = butt1
                    br_not_secondbutton.text = butt2
                }

                7 -> {
                    val progressbar7 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
                    //  br_not_bg.setBackgroundResource(R.drawable.notify_background_green);
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#705802"))
                    br_not_progress.progressDrawable = progressbar7
                    br_not_firstbutton.visibility = View.GONE
                    br_not_secondbutton.visibility = View.GONE
                    br_not_fl.visibility = View.VISIBLE
                    setMargins(br_title_text, 0, 2, 25, 12)
                    br_title_text.visibility = View.GONE
                    br_not_text2.visibility = View.VISIBLE
                    br_not_text.visibility = View.GONE
                    br_not_view.setBackgroundResource(R.drawable.notify_info_bg)
                    br_not_icon.setImageResource(R.drawable.notify_info)
                    br_not_text2.text = text
                }
            }
            br_not_firstbutton.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                sendClick(actionId, POSSITIVE)
                hide(true)
            }
            br_not_secondbutton.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                sendClick(actionId, NEGATIVE)
                hide(true)
            }
            if (duration != 0) {
                startCountdown()
            }
            constraintLayout.animate().cancel()
            constraintLayout.visibility = View.GONE
            constraintLayout.translationX = 0f
            constraintLayout.alpha = 1f
            constraintLayout.visibility = View.VISIBLE
            constraintLayout.animationXBounce(Bounce.BOUNCE_IN_UP, duration = 500)
        }
    }

    private fun setMargins(view: View, left: Int, top: Int, right: Int, bottom: Int) {
        if (view.layoutParams is MarginLayoutParams) {
            val p = view.layoutParams as MarginLayoutParams
            p.setMargins(left, top, right, bottom)
            view.requestLayout()
        }
    }

    fun startCountdown() {
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            countDownTimer = null
        }
        countDownTimer = object : CountDownTimer(br_not_progress.progress.toLong(), 1) {
            override fun onTick(j: Long) {
                br_not_progress.progress = j.toInt()
            }

            override fun onFinish() {
                hide(true)
            }
        }.start()
    }

    fun hide(right: Boolean) {
        activity.runOnUiThread {
            if (constraintLayout.visibility == View.VISIBLE) {
                if (countDownTimer != null) {
                    countDownTimer!!.cancel()
                    countDownTimer = null
                }
                if (right) {
                    constraintLayout.animate().translationXBy(300.0f)
                        .alpha(0f)
                        .setDuration(500)
                        .withEndAction {
                            constraintLayout.visibility = View.GONE
                            constraintLayout.translationX = 0f
                            constraintLayout.alpha = 1f
                        }
                        .start()
                } else {
                    constraintLayout.animate().translationXBy(-300.0f)
                        .alpha(0f)
                        .setDuration(500)
                        .withEndAction {
                            constraintLayout.visibility = View.GONE
                            constraintLayout.translationX = 0f
                            constraintLayout.alpha = 1f
                        }
                        .start()
                }
                
            }
        }
    }

    init {
        init()

        constraintLayout = activity.findViewById(R.id.constraintLayout_notif)
        br_not_bg = activity.findViewById(R.id.br_not_bg)
        br_not_fl = activity.findViewById(R.id.br_not_fl)
        br_not_view = activity.findViewById(R.id.br_not_view)
        br_not_icon = activity.findViewById(R.id.br_not_icon)
        br_title_text = activity.findViewById(R.id.br_title_text)
        br_title_text_title = activity.findViewById(R.id.br_title_text_title)
        br_title_text_text = activity.findViewById(R.id.br_title_text_text)
        br_not_text = activity.findViewById(R.id.br_not_text)
        br_not_text2 = activity.findViewById(R.id.br_not_text2)
        br_not_firstbutton = activity.findViewById(R.id.br_not_firstbutton)
        br_not_secondbutton = activity.findViewById(R.id.br_not_secondbutton)
        br_not_progress = activity.findViewById(R.id.br_not_progress)
        constraintLayout.visibility = View.GONE
        val gdt = GestureDetector(GestureListener())
        br_not_bg.setOnTouchListener { view: View?, event: MotionEvent? ->
            gdt.onTouchEvent(event!!)
            true
        }
    }

    private inner class GestureListener : SimpleOnGestureListener() {
        override fun onSingleTapUp(e: MotionEvent): Boolean {
            hide(false)
            return false
        }

        override fun onFling(e1: MotionEvent?, e2: MotionEvent, velocityX: Float, velocityY: Float): Boolean {
            if (e1!!.x - e2.x > SWIPE_MIN_DISTANCE && abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                hide(false)
            } else if (e2.x - e1.x > SWIPE_MIN_DISTANCE && abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                hide(true)
            }
            return false
        }
    }

    companion object {
        // end static //
        const val POSSITIVE = 1
        const val NEGATIVE = 0
        private const val SWIPE_MIN_DISTANCE = 120
        private const val SWIPE_THRESHOLD_VELOCITY = 200
    }
}