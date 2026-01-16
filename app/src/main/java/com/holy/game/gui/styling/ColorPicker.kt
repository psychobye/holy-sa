package com.holy.game.gui.styling

import android.view.View
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.ColorpickerBinding
import com.lit.game.gui.NativeGui
import ir.kotlin.kavehcolorpicker.KavehColorPicker

interface ColorPickerListener {
    fun onColorPickerSelected(color: Int)
    fun onColorPickerChange(color: Int)
    fun onColorPickerClose()
}

class ColorPicker(private val listener: ColorPickerListener)
    : NativeGui<ColorpickerBinding>(ColorpickerBinding::class) {

    private var startColorArgb: Int = 0xFF000000.toInt()
    private var lastColorArgb: Int = startColorArgb

    private var suppressEvents = false
    private var kavehListener: KavehColorPicker.OnColorChangedListener? = null

    init {
        activity.runOnUiThread {
            binding.selectButton.setOnClickListener {
                val colorArgb = binding.colorPickerView.color
                lastColorArgb = colorArgb
                listener.onColorPickerSelected(argbToRgba(colorArgb))
                destroy()
            }

            binding.cancelButton.setOnClickListener {
                listener.onColorPickerChange(argbToRgba(startColorArgb))
                listener.onColorPickerClose()
                destroy()
            }
        }
    }

    fun show(withAlpha: Boolean, withBrightness: Boolean, startColor: Int) {
        activity.runOnUiThread {
            activePicker?.takeIf { it !== this }?.destroy()
            activePicker = this

            val alphaByte = (startColor ushr 24) and 0xFF
            val usedStartArgb = if (alphaByte == 0) rgbaToArgb(startColor) else startColor
            startColorArgb = usedStartArgb
            lastColorArgb = usedStartArgb

            suppressEvents = true

            binding.colorPickerView.hueSliderView = if (withBrightness) binding.hueSlider else null
            binding.colorPickerView.alphaSliderView = if (withAlpha) binding.colorAlphaSlider else null

            binding.hueSlider.visibility = if (withBrightness) View.VISIBLE else View.GONE
            binding.colorAlphaSlider.visibility = if (withAlpha) View.VISIBLE else View.GONE

            binding.colorPickerView.color = usedStartArgb

            binding.colorPickerView.post {
                val listenerImpl = object : KavehColorPicker.OnColorChangedListener {
                    override fun onColorChanged(color: Int) {
                        if (suppressEvents) return
                        lastColorArgb = color
                        listener.onColorPickerChange(argbToRgba(color))
                    }
                }
                kavehListener = listenerImpl
                binding.colorPickerView.setOnColorChangedListener(listenerImpl)
                suppressEvents = false
            }
        }
    }

    override fun destroy() {
        activity.runOnUiThread {
            kavehListener = null
            binding.colorPickerView.hueSliderView = null
            binding.colorPickerView.alphaSliderView = null
            if (activePicker === this) activePicker = null
            super.destroy()
            suppressEvents = false
        }
    }

    override fun receivePacket(actionId: Int, json: String) { /* no-op */ }

    companion object {
        private var activePicker: ColorPicker? = null

        fun argbToRgba(argb: Int): Int {
            val a = (argb ushr 24) and 0xFF
            val r = (argb ushr 16) and 0xFF
            val g = (argb ushr 8) and 0xFF
            val b = argb and 0xFF
            return (r shl 24) or (g shl 16) or (b shl 8) or a
        }

        fun rgbaToArgb(rgba: Int): Int {
            val r = (rgba ushr 24) and 0xFF
            val g = (rgba ushr 16) and 0xFF
            val b = (rgba ushr 8) and 0xFF
            val a = rgba and 0xFF
            return (a shl 24) or (r shl 16) or (g shl 8) or b
        }
    }
}