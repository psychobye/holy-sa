package com.holy.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.graphics.PorterDuff
import android.view.View
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.SpeedometrBinding

class Speedometer : NativeGui<SpeedometrBinding>(SpeedometrBinding::class) {
    var timer_turnlight_left: Thread? = null
    var timer_turnlight_right: Thread? = null
    var timer_turnlight_all: Thread? = null
    var turnlight_left_state = false
    var turnlight_right_state = false
    var turnlight_all_state = false

    private val turnlight_tick_sound_1 = Samp.soundPool.load(activity, R.raw.turnlight_tick_1, 0)
    private val turnlight_tick_sound_2 = Samp.soundPool.load(activity, R.raw.turnlight_tick_2, 0)

    private val timestep: Long = 501 // ms

    external fun ClickSpedometr(turnID: Int, toggle: Boolean)
    external fun sendClick(clickId: Int)

    init {
        activity.runOnUiThread {
            binding.blinkerIcon.setOnClickListener {
                sendClick(BUTTON_TURN_ALL)
            }
            binding.turnLeftIcon.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                sendClick(BUTTON_TURN_LEFT)
            }
            binding.turnRightIcon.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                sendClick(BUTTON_TURN_RIGHT)
            }
            binding.enginenIcon.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                sendClick(BUTTON_ENGINE)
            }
            binding.lightIcon.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                sendClick(BUTTON_LIGHT)
            }
        }
    }

    private fun deleteThreads() {
        timer_turnlight_left?.interrupt()
        timer_turnlight_right?.interrupt()
        timer_turnlight_all?.interrupt()

        timer_turnlight_left = null
        timer_turnlight_right = null
        timer_turnlight_all = null

        turnlight_left_state = false
        turnlight_right_state = false
        turnlight_all_state = false

        activity.runOnUiThread {
            binding.turnLeftIcon.setImageResource(R.drawable.speedometr_turn_off)
            binding.turnRightIcon.setImageResource(R.drawable.speedometr_turn_off)
            binding.blinkerIcon.imageTintList = null
        }
    }

    override fun destroy() {
        activity.runOnUiThread {
            Samp.soundPool.unload(turnlight_tick_sound_1)
            Samp.soundPool.unload(turnlight_tick_sound_2)

            deleteThreads()

            super.destroy()
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    fun updateSpeed(speed: Int) {
        activity.runOnUiThread {
            binding.speedLine.progress = speed
            binding.speedText.text = "$speed"
        }
    }

    fun updateInfo(
        fuel: Int,
        hp: Int,
        mileage: Int,
        engine: Int,
        light: Int,
        lock: Int,
        turnlight: Int
    ) {
        activity.runOnUiThread {
            binding.fuelText.text = String.format("%03d", fuel)
            binding.milliageText.text = String.format("%06d", mileage)
            binding.hpText.text = String.format("%d%%", hp / 10)

            binding.enginenIcon.imageTintList =
                if (engine == 1) ColorStateList.valueOf(Color.parseColor("#01FE48")) else null

            binding.lightIcon.imageTintList =
                if (light == 1) ColorStateList.valueOf(Color.parseColor("#FAFF00")) else null

            binding.lockIcon.setColorFilter(
                if (lock == 1) Color.parseColor("#FF0000") else Color.parseColor(
                    "#00FF00"
                ), PorterDuff.Mode.SRC_IN
            )
            when (turnlight) {
                TURN_SIGNAL_LEFT -> {
                    if (timer_turnlight_left == null || !timer_turnlight_left!!.isAlive) {
                        deleteThreads()
                        timer_turnlight_left = Thread(runnable_turnlight_left)
                        timer_turnlight_left!!.start()
                    }
                }

                TURN_SIGNAL_RIGHT -> {
                    if (timer_turnlight_right == null || !timer_turnlight_right!!.isAlive) {
                        deleteThreads()
                        timer_turnlight_right = Thread(runnable_turnlight_right)
                        timer_turnlight_right!!.start()
                    }
                }

                TURN_SIGNAL_ALL -> {
                    if (timer_turnlight_all == null || !timer_turnlight_all!!.isAlive) {
                        deleteThreads()
                        timer_turnlight_all = Thread(runnable_turnlight_all)
                        timer_turnlight_all!!.start()
                    }
                }

                else -> {
                    deleteThreads()
                    binding.blinkerIcon.imageTintList = null
                    binding.turnLeftIcon.setImageResource(R.drawable.speedometr_turn_off)
                    binding.turnRightIcon.setImageResource(R.drawable.speedometr_turn_off)
                }
            }
        }
    }

    private var runnable_turnlight_all = Runnable {
        while (!Thread.currentThread().isInterrupted) {
            activity.runOnUiThread {
                if (turnlight_all_state) {
                    binding.blinkerIcon.imageTintList = null
                    binding.turnLeftIcon.setImageResource(R.drawable.speedometr_turn_off)
                    binding.turnRightIcon.setImageResource(R.drawable.speedometr_turn_off)
                    turnlight_all_state = false

                    Samp.soundPool.play(turnlight_tick_sound_1, 1.0f, 1.0f, 1, 0, 1.0f)
                } else {
                    binding.blinkerIcon.imageTintList = ColorStateList.valueOf(Color.parseColor("#f44336"))
                    binding.turnLeftIcon.setImageResource(R.drawable.speedometr_turn_on)
                    binding.turnRightIcon.setImageResource(R.drawable.speedometr_turn_on)
                    Samp.soundPool.play(turnlight_tick_sound_2, 1.0f, 1.0f, 1, 0, 1.0f)
                    turnlight_all_state = true
                }
            }
            try {
                Thread.sleep(timestep)
            } catch (e: InterruptedException) {
                break
            }
        }
    }
    private var runnable_turnlight_left = Runnable {
        while (!Thread.currentThread().isInterrupted) {
            activity.runOnUiThread {
                turnlight_left_state = if (turnlight_left_state) {
                    binding.turnLeftIcon.setImageResource(R.drawable.speedometr_turn_off)
                    Samp.soundPool.play(turnlight_tick_sound_1, 1.0f, 1.0f, 1, 0, 1.0f)
                    false
                } else {
                    binding.turnLeftIcon.setImageResource(R.drawable.speedometr_turn_on)
                    Samp.soundPool.play(turnlight_tick_sound_2, 1.0f, 1.0f, 1, 0, 1.0f)
                    true
                }
            }
            try {
                Thread.sleep(timestep)
            } catch (e: InterruptedException) {
                break
            }
        }
    }
    private var runnable_turnlight_right = Runnable {
        while (!Thread.currentThread().isInterrupted) {
            activity.runOnUiThread {
                turnlight_right_state = if (turnlight_right_state) {
                    binding.turnRightIcon.setImageResource(R.drawable.speedometr_turn_off)
                    Samp.soundPool.play(turnlight_tick_sound_1, 1.0f, 1.0f, 1, 0, 1.0f)
                    false
                } else {
                    binding.turnRightIcon.setImageResource(R.drawable.speedometr_turn_on)
                    Samp.soundPool.play(turnlight_tick_sound_2, 1.0f, 1.0f, 1, 0, 1.0f)
                    true
                }
            }
            try {
                Thread.sleep(timestep)
            } catch (e: InterruptedException) {
                break
            }
        }
    }

    companion object {
        const val BUTTON_ENGINE = 0
        const val BUTTON_LIGHT = 1
        const val BUTTON_TURN_LEFT = 2
        const val BUTTON_TURN_RIGHT = 3
        const val BUTTON_TURN_ALL = 4
        const val TURN_SIGNAL_LEFT = 1
        const val TURN_SIGNAL_RIGHT = 2
        const val TURN_SIGNAL_ALL = 3
    }
}