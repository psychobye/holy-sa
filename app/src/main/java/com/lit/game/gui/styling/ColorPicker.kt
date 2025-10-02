package com.lit.game.gui.styling

import android.view.View
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.ColorpickerBinding
import com.lit.game.gui.NativeGui
import com.skydoves.colorpickerview.ColorEnvelope
import com.skydoves.colorpickerview.listeners.ColorEnvelopeListener

interface ColorPickerListener {
    fun onColorPickerSelected(color: Int)
    fun onColorPickerChange(color: Int)
    fun onColorPickerClose()
}


class ColorPicker(listener: ColorPickerListener)
    : NativeGui<ColorpickerBinding>(ColorpickerBinding::class) {
    private var startColor = 0

    init {
        activity.runOnUiThread {

            binding.selectButton.setOnClickListener {
                val color = binding.colorPicker.color
                listener.onColorPickerSelected( argbToRgba(color) )
                destroy()
            }

            binding.cancelButton.setOnClickListener {
                listener.onColorPickerChange( startColor )
                listener.onColorPickerClose()

                destroy()
            }

            binding.colorPicker.setColorListener(ColorEnvelopeListener { envelope: ColorEnvelope, _: Boolean ->
                listener.onColorPickerChange( argbToRgba(envelope.color) )
            })
        }
    }

    fun show(withAlpha: Boolean, withBrithness: Boolean, startColor: Int) {
        activity.runOnUiThread {
            this.startColor = startColor
            binding.colorPicker.setInitialColor( rgbaToArgb(startColor) )

            if (withBrithness) {
                binding.brightnessSlide.visibility = View.VISIBLE
                binding.colorPicker.attachBrightnessSlider(binding.brightnessSlide)
            }
            else {
                binding.brightnessSlide.visibility = View.GONE
            }
            if (withAlpha) {
                binding.alphaSlideBar.visibility = View.VISIBLE
                binding.colorPicker.attachAlphaSlider(binding.alphaSlideBar)
            }
            else {
                binding.alphaSlideBar.visibility = View.GONE
            }
        }
    }

    companion object {
        fun argbToRgba(argb: Int): Int {
            val alpha = argb ushr 24 and 0xFF
            val red = argb shr 16 and 0xFF
            val green = argb shr 8 and 0xFF
            val blue = argb and 0xFF

            return (red shl 24) or (green shl 16) or (blue shl 8) or alpha
        }

        fun rgbaToArgb(argb: Int): Int {
            val red = argb ushr 24 and 0xFF
            val green = argb shr 16 and 0xFF
            val blue = argb shr 8 and 0xFF
            val alpha = argb and 0xFF

            return (alpha shl 24) or (red shl 16) or (green shl 8) or blue
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}