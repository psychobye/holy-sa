package com.holy.game.gui.fillingGames

import android.os.CountDownTimer
import android.widget.ImageView
import android.widget.TextView
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.skydoves.progressview.ProgressView

interface FillingGameListener {
    fun onEnded()
}

class ParentClass(
    private val progressBar1: ProgressView,
    private val progressBar2: ProgressView,

    private val button1: ImageView,
    private val button2: ImageView,

    private val percentText1: TextView,
    private val percentText2: TextView,

    private val listener: FillingGameListener
) {

    var state1 = 0
    var state2 = 0

    var countDownTimer: CountDownTimer? = null

    init {
        activity.runOnUiThread {
            button1.setOnClickListener {
                it.startAnimation(Samp.clickAnim)
                update1(+100)
            }

            button2.setOnClickListener {
                it.startAnimation(Samp.clickAnim)
                update2(+100)
            }

            startCountdown()
        }
    }

    private fun startCountdown() {
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            countDownTimer = null
        }
        countDownTimer = object : CountDownTimer(999999999, 10) {
            override fun onTick(j: Long) {
                if (state1 >= 998 && state2 >= 998) {
                    countDownTimer!!.cancel()
                    countDownTimer!!.onFinish()
                    return
                }

                update1(-1)
                update2(-1)
            }

            override fun onFinish() {
               listener.onEnded()
            }
        }.start()
    }

    fun update1(value: Int) {
        state1 = (state1 + value).coerceIn(0, 1000)

        percentText1.text = String.format("%d%%", state1 / 10)
        progressBar1.progress = state1.toFloat()
    }

    fun update2(value: Int) {
        state2 = (state2 + value).coerceIn(0, 1000)

        percentText2.text = String.format("%d%%", state2 / 10)
        progressBar2.progress = state2.toFloat()
    }
}