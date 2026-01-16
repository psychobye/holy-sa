package com.holy.launcher.ui.fragment

import android.app.Activity
import android.content.res.ColorStateList
import android.graphics.Color
import android.os.CountDownTimer
import android.util.Log
import android.view.View
import android.view.ViewGroup.MarginLayoutParams
import android.widget.Button
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.ProgressBar
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R

class LauncherNotification(private val rootView: View, private val activity: Activity) {
    private val constraintLayout: ConstraintLayout =
        (rootView.findViewById<ConstraintLayout>(R.id.constraintLayout_notif)
            ?: (rootView as? ConstraintLayout)
            ?: throw IllegalArgumentException("rootView is not ConstraintLayout and doesn't contain constraintLayout_notif"))

    private val br_not_bg: FrameLayout = rootView.findViewById(R.id.br_not_bg)
    private val br_not_fl: FrameLayout = rootView.findViewById(R.id.br_not_fl)
    private val br_not_view: View = rootView.findViewById(R.id.br_not_view)
    private val br_not_icon: ImageView = rootView.findViewById(R.id.br_not_icon)
    private val br_title_text: FrameLayout = rootView.findViewById(R.id.br_title_text)
    private val br_title_text_title: TextView = rootView.findViewById(R.id.br_title_text_title)
    private val br_title_text_text: TextView = rootView.findViewById(R.id.br_title_text_text)
    private val br_not_text: TextView = rootView.findViewById(R.id.br_not_text)
    private val br_not_text2: TextView = rootView.findViewById(R.id.br_not_text2)
    private val br_not_firstbutton: Button = rootView.findViewById(R.id.br_not_firstbutton)
    private val br_not_secondbutton: Button = rootView.findViewById(R.id.br_not_secondbutton)
    private val br_not_progress: ProgressBar = rootView.findViewById(R.id.br_not_progress)

    private var countDownTimer: CountDownTimer? = null

    init {
        constraintLayout.visibility = View.GONE
        br_not_firstbutton.setOnClickListener {
            Log.d("LauncherNotification", "first button clicked")
            hide(true)
        }
        br_not_secondbutton.setOnClickListener {
            Log.d("LauncherNotification", "second button clicked")
            hide(true)
        }
        br_not_bg.setOnTouchListener { _, _ ->
            hide(false)
            true
        }
    }

    fun showNotification(type: Int, text: String?, duration: Int, actionId: Int, butt1: String?, butt2: String?) {
        activity.runOnUiThread {
            if (duration != 0) {
                br_not_progress.max = duration * 990
                br_not_progress.progress = duration * 990
            } else {
                br_not_progress.max = 100
                br_not_progress.progress = 100
            }

            when (type) {
                0 -> { // error
                    val progressbar = activity.getDrawable(R.drawable.notify_progressbar_red)
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#9F0A0A"))
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

                1 -> { // green / ruble-like in original
                    val progressbar1 = activity.getDrawable(R.drawable.notify_progressbar_green)
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

                2 -> { // red ruble-like (same as 1 but red theme)
                    val progressbar2 = activity.getDrawable(R.drawable.notify_progressbar_red)
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#9F0A0A"))
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

                3 -> { // success
                    val progressbar3 = activity.getDrawable(R.drawable.notify_progressbar_green)
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

                4 -> { // simple button >>
                    val progressbar4 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
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

                5 -> { // "login" prompt
                    val progressbar5 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
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

                6 -> { // two buttons (offer)
                    val progressbar6 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
                    br_not_bg.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#003752"))
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
                    br_not_firstbutton.text = butt1 ?: "Принять"
                    br_not_secondbutton.text = butt2 ?: "Отказ"
                }

                7 -> { // info
                    val progressbar7 = activity.getDrawable(R.drawable.notify_progressbar_yellow)
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

                else -> { // fallback
                    br_not_bg.setBackgroundColor(0xFF003752.toInt())
                    br_not_fl.visibility = View.GONE
                    br_title_text.visibility = View.VISIBLE
                    br_title_text_title.text = text ?: ""
                    br_title_text_text.text = butt2 ?: ""
                    br_not_firstbutton.text = butt1 ?: "OK"
                }
            }
            if (butt1 != null) br_not_firstbutton.text = butt1
            if (butt2 != null) br_not_secondbutton.text = butt2

            if (duration != 0) startCountdown()
            // show with simple rise animation
            constraintLayout.animate().cancel()
            constraintLayout.translationY = 300f
            constraintLayout.alpha = 0f
            constraintLayout.visibility = View.VISIBLE
            constraintLayout.animate().translationY(0f).alpha(1f).setDuration(500).start()
        }
    }

    private fun startCountdown() {
        countDownTimer?.cancel()
        countDownTimer = object : CountDownTimer(br_not_progress.progress.toLong(), 50L) {
            override fun onTick(millisUntilFinished: Long) {
                br_not_progress.progress = millisUntilFinished.toInt()
            }
            override fun onFinish() { hide(true) }
        }.start()
    }

    fun hide(right: Boolean) {
        activity.runOnUiThread {
            if (constraintLayout.visibility == View.VISIBLE) {
                countDownTimer?.cancel()
                countDownTimer = null
                constraintLayout.animate().translationXBy(if (right) 300f else -300f)
                    .alpha(0f)
                    .setDuration(300)
                    .withEndAction {
                        constraintLayout.visibility = View.GONE
                        constraintLayout.translationX = 0f
                        constraintLayout.alpha = 1f
                    }.start()
            }
        }
    }

    private fun setMargins(view: View, left: Int, top: Int, right: Int, bottom: Int) {
        if (view.layoutParams is MarginLayoutParams) {
            val p = view.layoutParams as MarginLayoutParams
            p.setMargins(left, top, right, bottom)
            view.requestLayout()
        }
    }

    companion object {
        const val POSSITIVE = 1
        const val NEGATIVE = 0
    }
}