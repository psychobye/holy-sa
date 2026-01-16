package com.holy.game.gui.tire_shop

import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.CustomSliderBinding
import com.holy.game.gui.NativeGui
import kotlin.math.roundToInt

interface CustomSeekListener {
    fun onChangeProgress(value: Float)
    fun onCancel()
    fun onSucces(value: Float)
}

class CustomSeekBar(listener: CustomSeekListener) : NativeGui<CustomSliderBinding>(CustomSliderBinding::class) {
    private var startValue = 0f

    init {
        activity.runOnUiThread {
            binding.slider.clearOnChangeListeners() // WTF? android bug. listener not deleted
            binding.slider.addOnChangeListener { _, value: Float, _ ->
                listener.onChangeProgress(value)
            }
            binding.cancelButton.setOnClickListener {
                listener.onChangeProgress(startValue)
                listener.onCancel()

                destroy()
            }
            binding.okButton.setOnClickListener {
                listener.onSucces(binding.slider.value)

                destroy()
            }
        }
    }

    fun showSeek(caption: String?, current: Float, min: Float, max: Float, step: Float) {
        activity.runOnUiThread {
            binding.slider.valueFrom = min
            binding.slider.valueTo = max
            binding.slider.stepSize = step
            val alignedValue = min + ((current - min) / step).roundToInt() * step
            if (alignedValue > max || alignedValue < min)
                binding.slider.value = min
            else
                binding.slider.value = alignedValue

            startValue = alignedValue
            binding.caption.text = caption
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}