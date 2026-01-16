package com.holy.game.gui.theft_auto

import android.os.CountDownTimer
import android.util.TypedValue
import android.view.View
import android.widget.LinearLayout
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.TheftAutoBinding
import com.holy.game.gui.NativeGui
import com.holy.game.gui.adapters.TheftAutoLevelsAdapter

class TheftAuto : NativeGui<TheftAutoBinding>(TheftAutoBinding::class){

    private var levelsAdapter =
        com.holy.game.gui.adapters.TheftAutoLevelsAdapter(activity, initLevelsColor)
    private var failCount = 0
    private var currentLevel = 0
    private var countDownTimer: CountDownTimer? = null
    private var tick: Long = 0
    private var success = 0
    private var shiftInDp = 0

    init {
        activity.runOnUiThread {

            binding.mainButt.setOnClickListener {
                it.startAnimation(Samp.clickAnim)
                doMainButtonAction()
            }

            binding.levelsView.adapter = levelsAdapter
            failCount = 0
            currentLevel = 1
        }
    }

    private external fun finishRendering(status: Int)

    private val initLevelsColor: List<Int>
        get() {
            val initLevelsColor: MutableList<Int> = ArrayList()
            for (i in 0..9) {
                initLevelsColor.add(R.color.theft_auto_init_level_color)
            }
            return initLevelsColor
        }

    private fun showRendering() {
        activity.runOnUiThread {
            binding.mainLayout.visibility = View.VISIBLE
            binding.point.setImageResource(R.drawable.samwill_gray)
            val lp = LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.MATCH_PARENT)
            shiftInDp = getRandomMargin(MIN_POINT_MARGIN, MAX_POINT_MARGIN)
            val r = Samp.activity.resources
            val shiftInPx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, shiftInDp.toFloat(), r.displayMetrics).toInt()
            lp.setMargins(shiftInPx, 0, 0, 0)
            binding.point.layoutParams = lp
            success = -1
            binding.progressBar.progress = PROGRESS_BAR_MAX_VALUE
            startCountdown()
        }
    }

    private fun getRandomMargin(min: Int, max: Int): Int {
        return (Math.random() * (max - min) + min).toInt()
    }

    private fun doMainButtonAction() {
        val leftPointWithShift = shiftInDp * SHIFT_FACTOR
        val rightPointWithShift = shiftInDp * SHIFT_FACTOR + POINT_SIZE
        if (binding.progressBar.progress in (leftPointWithShift + 1)..<rightPointWithShift && success != 0) {

            success = 1
            binding.point.setImageResource(R.drawable.samwill_green)
            levelsAdapter.updateItem(currentLevel - 1, R.color.theft_auto_passed_level_color)
            tick = rightPointWithShift.toLong()
            return
        }
        else if (binding.progressBar.progress < rightPointWithShift) {
            if (Samp.vibrator.hasVibrator()) {
                Samp.vibrator.vibrate(200)
            }
            tick = rightPointWithShift.toLong()
            success = 0
            binding.point.setImageResource(R.drawable.samwill_red)
            levelsAdapter.updateItem(currentLevel - 1, R.color.theft_auto_not_passed_level_color)
            return
        }
    }

    private fun startCountdown() {
        val rightPointWithShift = shiftInDp * SHIFT_FACTOR + POINT_SIZE
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            countDownTimer = null
            tick = 0
        }
        countDownTimer = object : CountDownTimer(999999999, 17) {
            override fun onTick(j: Long) {
                tick += PROGRESS_SPEED.toLong()
                binding.progressBar.progress = tick.toInt()
                if (binding.progressBar.progress > rightPointWithShift && success != 1) {
                    success = 0
                    binding.point.setImageResource(R.drawable.samwill_red)
                    levelsAdapter.updateItem(currentLevel - 1, R.color.theft_auto_not_passed_level_color)
                }
                if (binding.progressBar.progress >= binding.progressBar.max) {
                    countDownTimer!!.cancel()
                    countDownTimer!!.onFinish()
                }
            }

            override fun onFinish() {
                goToNextLevelOrHide()
            }
        }.start()
    }

    private fun goToNextLevelOrHide() {
        if (R.color.theft_auto_not_passed_level_color == levelsAdapter!!.getItem(currentLevel - 1)) {
            failCount++
        }
        if (failCount == MAX_FAIL_COUNT) {
            destroy()
            finishRendering(GAME_NOT_PASSED_STATUS)
            return
        }
        if (currentLevel == MAX_LEVEL) {
            destroy()
            finishRendering(GAME_PASSED_STATUS)
            return
        }
        currentLevel++
        startNewLevel()
    }

    private fun startNewLevel() {
        showRendering()
    }

    companion object {
        private const val MIN_POINT_MARGIN = 100
        private const val MAX_POINT_MARGIN = 500
        private const val POINT_SIZE = 2000
        private const val SHIFT_FACTOR = 50
        private const val PROGRESS_BAR_MAX_VALUE = 30000
        private const val PROGRESS_SPEED = 250
        private const val GAME_PASSED_STATUS = 1
        private const val GAME_NOT_PASSED_STATUS = 0
        private const val MAX_FAIL_COUNT = 3
        private const val MAX_LEVEL = 10
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}