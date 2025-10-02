package com.lit.game.gui

import android.os.CountDownTimer
import android.view.View
import com.google.gson.Gson
import com.lit.game.EasyAnimation.setOnClickListenerWithAnim
import com.lit.game.R
import com.lit.game.databinding.FishingBinding

class Fishing : NativeGui<FishingBinding>(FishingBinding::class) {
    enum class eFishingStates {
        WAITING,
        NEED_HOOK,
        NEED_SPIN
    }

    var countDownTimer: CountDownTimer? = null

    init {
        binding.button.setOnClickListenerWithAnim {
            sendPacket(ACTION_ONCLICK_BUTTON)
        }

        binding
    }

    private fun updateState(toState: Int) {
        when(toState) {
            eFishingStates.WAITING.ordinal -> {
                binding.floating.setImageResource(R.drawable.fishing_float_passive)
                binding.button.setImageResource(R.drawable.fishing_spining_button)
                binding.button.visibility = View.VISIBLE
            }
            eFishingStates.NEED_HOOK.ordinal -> {
                binding.floating.setImageResource(R.drawable.fishing_float_active)
                binding.button.setImageResource(R.drawable.fishing_hook_button)
                binding.button.visibility = View.VISIBLE
            }
            eFishingStates.NEED_SPIN.ordinal -> {
                binding.button.visibility = View.GONE

                startCountdown()
            }
        }
    }

    private fun startCountdown() {
        if (countDownTimer != null) {
            countDownTimer!!.cancel()
            countDownTimer = null
        }
        countDownTimer = object : CountDownTimer(999999999, 10) {
            override fun onTick(j: Long) {
                if (binding.progress.progress >= 998) {
                    countDownTimer!!.cancel()
                    countDownTimer!!.onFinish()
                    return
                }

                binding.progress.progress = (binding.progress.progress - 1).coerceIn(0, 1000)
            }

            override fun onFinish() {
                sendPacket(ACTION_SPIN_ENDED)
            }
        }.start()
    }

    // receive
    private val ACTION_CHANGE_STATE = 0
    data class ActionChangeStateData(
        val toState: Int = 0
    )

    private val ACTION_SHOW_CATCH = 1
    data class ActionShowCatch(
        val sprite: String,
        val caption: String,
        val description: String
    )

    // send
    private val ACTION_ONCLICK_BUTTON = 2
    private val ACTION_SPIN_ENDED = 3
    private val ACTION_CLICK_EXIT = 4

    override fun receivePacket(actionId: Int, json: String) {
        when(actionId){
            ACTION_CHANGE_STATE -> {
                val data = Gson().fromJson(json, ActionChangeStateData::class.java)

            }
        }
    }

}