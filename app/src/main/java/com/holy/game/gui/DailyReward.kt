package com.holy.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.os.Handler
import android.os.Looper
import android.text.format.DateUtils
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.DailyrewardBinding
import com.rommansabbir.animationx.Zoom
import com.rommansabbir.animationx.animationXZoom

class DailyReward : NativeGui<DailyrewardBinding>(DailyrewardBinding::class) {

    private var dailyItems: MutableList<ConstraintLayout> = ArrayList()

    private external fun nativeClick()
    private external fun nativeOnDestroy()

    init {
        activity.runOnUiThread {
            binding.exitButton.setOnClickListener {
                binding.mainLayout.animationXZoom(Zoom.ZOOM_OUT, duration = 300)
                Handler(Looper.getMainLooper()).postDelayed({ destructor() }, 300)
            }
            dailyItems.add(binding.itemBg1)
            dailyItems.add(binding.itemBg2)
            dailyItems.add(binding.itemBg3)
            dailyItems.add(binding.itemBg4)
            dailyItems.add(binding.itemBg5)
            dailyItems.add(binding.itemBg6)
            dailyItems.add(binding.itemBg7)

            for (i in dailyItems.indices) {
                val parent = dailyItems[i]
                val label = parent.getChildAt(3) as ConstraintLayout

                label.setOnClickListener {
                    nativeClick()
                }
            }
            binding.mainLayout.animationXZoom(Zoom.ZOOM_IN, duration = 300)
        }
    }

    private fun destructor() {
        activity.runOnUiThread {
            destroy()

            nativeOnDestroy()
        }
    }

    private fun show(day: Int, second: Int) {
        activity.runOnUiThread {
            val idx = day - 1

            for (i in dailyItems.indices) {
                val parent = dailyItems[i]
                val label = parent.getChildAt(3) as ConstraintLayout
                val labelText = label.getChildAt(0) as TextView
                if (i == idx) {
                    parent.alpha = 1.0f
                    if (second == -1) {
                        parent.backgroundTintList =
                            ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                        label.backgroundTintList =
                            ColorStateList.valueOf(Color.parseColor("#e57373"))
                        labelText.text = "Недоступно"
                    } else if (second == 0) {
                        parent.backgroundTintList =
                            ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                        label.backgroundTintList =
                            ColorStateList.valueOf(Color.parseColor("#66bb6a"))
                        labelText.text = "Забрать"
                    } else {
                        parent.backgroundTintList =
                            ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                        label.backgroundTintList =
                            ColorStateList.valueOf(Color.parseColor("#66bb6a"))
                        labelText.text = DateUtils.formatElapsedTime(second.toLong())
                    }

                } else if (i < idx) {
                    parent.alpha = 0.4f
                    parent.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#66bb6a"))
                    label.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#66bb6a"))
                    labelText.text = "Получено"
                } else {
                    parent.alpha = 1.0f
                    parent.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#9e9e9e"))
                    label.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#9e9e9e"))
                    labelText.text = "Недоступно"
                }
            }
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}