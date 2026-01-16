package com.holy.launcher.ui.activity

import android.content.Intent
import android.os.Bundle
import android.os.CountDownTimer
import android.os.Handler
import android.view.View
import android.view.animation.AnimationUtils
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.akexorcist.roundcornerprogressbar.RoundCornerProgressBar
import com.holy.game.R

class StoryActivity : AppCompatActivity() {
    private var progressStory: RoundCornerProgressBar? = null
    private var countDownTimer: CountDownTimer? = null
    private var progress: Long = 0
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_story)
        window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_FULLSCREEN
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)
        progressStory = findViewById<View>(R.id.progressStory) as RoundCornerProgressBar
        (findViewById<View>(R.id.closeStory) as ImageView).setOnClickListener { view -> `lambda$onCreate$0$StoryActivity`(view) }
        progressStory!!.progress = 0.0f
        startTimer()
    }

    fun `lambda$onCreate$0$StoryActivity`(view: View) {
        view.startAnimation(AnimationUtils.loadAnimation(this, R.anim.button_click))
        Handler().postDelayed({ closeStory() }, 200)
    }

    fun closeStory() {
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            countDownTimer = null
        }
        progress = 0
        startActivity(Intent(applicationContext, MainActivity::class.java))
    }

    private fun startTimer() {
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            progress = 0
            countDownTimer = null
        }
        countDownTimer = object : CountDownTimer(5000, 1) {
            override fun onTick(j: Long) {
                progress = 5000 - j
                progressStory!!.progress = progress.toFloat()
            }

            override fun onFinish() {

            }
        }.start()
    }

    public override fun onDestroy() {
        super.onDestroy()
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
        }
        progress = 0
        countDownTimer = null
    }

    public override fun onPause() {
        super.onPause()
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            progress = 0
            countDownTimer = null
        }
    }
}