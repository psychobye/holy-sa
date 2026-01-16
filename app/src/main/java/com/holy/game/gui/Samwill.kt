package com.holy.game.gui

import android.os.CountDownTimer
import android.view.View
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.SawmillBinding

class Samwill : NativeGui<SawmillBinding>(SawmillBinding::class) {

    private var countDownTimer: CountDownTimer? = null
    private external fun nativeSendExit(samwillpacket: Int)
    private var samwill1 = -1
    private var samwill2 = -1
    private var samwill3 = -1
    private var samwill4 = -1
    private var samwill5 = -1
    private var samwillpacket = 0
    private var sawSound = 0
    private var tick: Long = 0

    init {
        activity.runOnUiThread {
            binding.button.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (binding.progressBar.progress in 2501..4049 && samwill1 != 0) {
                    Samp.soundPool.play(sawSound, 0.1f, 0.1f, 1, 0, 1.2f)
                    samwillpacket++
                    samwill1 = 1
                    binding.cell1.setImageResource(R.drawable.samwill_green)
                    tick = 4050
                    return@setOnClickListener
                }
                else if (binding.progressBar.progress < 4050) {
                    if (Samp.vibrator.hasVibrator()) {
                        Samp.vibrator.vibrate(200)
                    }
                    tick = 4050
                    samwill1 = 0
                    binding.cell1.setImageResource(R.drawable.samwill_red)
                    return@setOnClickListener
                }
                if (binding.progressBar.progress in 7101..8649 && samwill2 != 0) {
                    Samp.soundPool.play(sawSound, 0.1f, 0.1f, 1, 0, 1.2f)
                    samwillpacket++
                    samwill2 = 1
                    binding.cell2.setImageResource(R.drawable.samwill_green)
                    tick = 8650
                    return@setOnClickListener
                }
                else if (binding.progressBar.progress < 8650) {
                    if (Samp.vibrator.hasVibrator()) {
                        Samp.vibrator.vibrate(200)
                    }
                    tick = 8650
                    samwill2 = 0
                    binding.cell2.setImageResource(R.drawable.samwill_red)
                    return@setOnClickListener
                }
                if (binding.progressBar.progress in 11721..13249 && samwill3 != 0) {
                    Samp.soundPool.play(sawSound, 0.1f, 0.1f, 1, 0, 1.2f)
                    samwillpacket++
                    samwill3 = 1
                    binding.cell3.setImageResource(R.drawable.samwill_green)
                    tick = 13250
                    return@setOnClickListener
                }
                else if (binding.progressBar.progress < 13250) {
                    if (Samp.vibrator.hasVibrator()) {
                        Samp.vibrator.vibrate(200)
                    }
                    tick = 13250
                    samwill3 = 0
                    binding.cell3.setImageResource(R.drawable.samwill_red)
                    return@setOnClickListener
                }
                if (binding.progressBar.progress in 16331..17869 && samwill4 != 0) {
                    Samp.soundPool.play(sawSound, 0.1f, 0.1f, 1, 0, 1.2f)
                    samwillpacket++
                    samwill4 = 1
                    binding.cell4.setImageResource(R.drawable.samwill_green)
                    tick = 17870
                    return@setOnClickListener
                }
                else if (binding.progressBar.progress < 17870) {
                    if (Samp.vibrator.hasVibrator()) {
                        Samp.vibrator.vibrate(200)
                    }
                    tick = 17870
                    samwill4 = 0
                    binding.cell4.setImageResource(R.drawable.samwill_red)
                    return@setOnClickListener
                }
                else if (binding.progressBar.progress in 20921..22479 && samwill5 != 0) {
                    Samp.soundPool.play(sawSound, 0.1f, 0.1f, 1, 0, 1.2f)
                    samwillpacket++
                    samwill5 = 1
                    binding.cell5.setImageResource(R.drawable.samwill_green)
                    tick = 22480
                    destroy()
                    countDownTimer!!.cancel()
                    return@setOnClickListener
                }
                else if (binding.progressBar.progress < 22480) {
                    if (Samp.vibrator.hasVibrator()) {
                        Samp.vibrator.vibrate(200)
                    }
                    tick = 22480
                    samwill5 = 0
                    binding.cell5.setImageResource(R.drawable.samwill_red)
                    return@setOnClickListener
                }
            }

            sawSound = Samp.soundPool.load(activity, R.raw.saw, 0)

            binding.cell1.setImageResource(R.drawable.samwill_gray)
            binding.cell2.setImageResource(R.drawable.samwill_gray)
            binding.cell3.setImageResource(R.drawable.samwill_gray)
            binding.cell4.setImageResource(R.drawable.samwill_gray)
            binding.cell5.setImageResource(R.drawable.samwill_gray)
            binding.progressBar.progress = 25000

            startCountdown()
        }
    }

    override fun destroy() {
        super.destroy()

        Samp.soundPool.unload(sawSound)
        nativeSendExit(samwillpacket)
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    private fun startCountdown() {
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            countDownTimer = null
            tick = 0
        }
        countDownTimer = object : CountDownTimer(999999999, 17) {
            override fun onTick(j: Long) {
                tick += 40
                binding.progressBar.progress = tick.toInt()
                val progresstext = binding.progressBar.progress / 25 / 10
                binding.progressText.text = "$progresstext%"
                if (binding.progressBar.progress > 4050 && samwill1 != 1) {
                    samwill1 = 0
                    binding.cell1.setImageResource(R.drawable.samwill_red)
                }
                if (binding.progressBar.progress > 8650 && samwill2 != 1) {
                    samwill2 = 0
                    binding.cell2.setImageResource(R.drawable.samwill_red)
                }
                if (binding.progressBar.progress > 13250 && samwill3 != 1) {
                    samwill3 = 0
                    binding.cell3.setImageResource(R.drawable.samwill_red)
                }
                if (binding.progressBar.progress > 17870 && samwill4 != 1) {
                    samwill4 = 0
                    binding.cell4.setImageResource(R.drawable.samwill_red)
                }
                if (binding.progressBar.progress > 22480 && samwill5 != 1) {
                    samwill5 = 0
                    binding.cell5.setImageResource(R.drawable.samwill_red)
                }
                if (binding.progressBar.progress >= binding.progressBar.max) {
                    destroy()
                    countDownTimer!!.cancel()
                }
            }

            override fun onFinish() {
                // Hide();
            }
        }.start()
    }
}