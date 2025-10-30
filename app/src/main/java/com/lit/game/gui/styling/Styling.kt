package com.lit.game.gui.styling

import android.view.View
import androidx.recyclerview.widget.LinearLayoutManager
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.StylingCenterBinding
import com.lit.game.gui.NativeGui
import com.lit.game.gui.tuning.TuningAdapter
import com.lit.game.gui.tuning.TuningAdapterListener
import com.lit.game.gui.tuning.TuningItem
import com.lit.game.gui.tuning.TuningSelectMenu
import com.lit.game.gui.tuning.TuningSelectMenuItem
import com.lit.game.gui.tuning.TuningSelectMenuListener

class Styling : NativeGui<StylingCenterBinding>(StylingCenterBinding::class), ColorPickerListener, TuningAdapterListener, TuningSelectMenuListener {

    companion object {
        enum class ValueType {
            VALUE_TYPE_NEON_TYPE,
            VALUE_TYPE_NEON_COLOR,
            VALUE_TYPE_LIGHT_COLOR,
            VALUE_TYPE_TONER_COLOR,
            VALUE_TYPE_BODY1_COLOR,
            VALUE_TYPE_BODY2_COLOR,
            VALUE_TYPE_WHEEL_COLOR,
            VALUE_TYPE_VINYL,
            VALUE_TYPE_STROB
        }
    }

    private val items = arrayListOf(
        TuningItem("Тип неона", R.drawable.ic_styling_neon),
        TuningItem("Цвет неона", R.drawable.ic_styling_neon),
        TuningItem("Цвет фар", R.drawable.ic_styling_lights),
        TuningItem("Цвет тонера", R.drawable.ic_styling_toner),
        TuningItem("Кузов (1)", R.drawable.ic_styling_color),
        TuningItem("Кузов (2)", R.drawable.ic_styling_color),
        TuningItem("Цвет дисков", R.drawable.ic_styling_wheel),
        TuningItem("Винилы", R.drawable.ic_styling_vinil),
        TuningItem("Стробоскопы", R.drawable.ic_styling_strob),
    )
    private val adapter = TuningAdapter(items, this)

    private var valueType: ValueType = ValueType.VALUE_TYPE_NEON_TYPE

    private external fun nativeOnExit()
    private external fun nativeGetCurrentValue(type: Int): Int
    private external fun nativeSendChangeValue(valueType: Int)
    private external fun nativeChangeValue(type: Int, value: Int)
    private external fun nativeIsAvailable(type: Int): Boolean
    private external fun nativeResetTuning(type: Int)

    private external fun nativeSendBuy()

    init {
        activity.runOnUiThread {
            binding.recycle.layoutManager = LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false)
            binding.recycle.adapter = adapter

            binding.exitButt.setOnClickListener {
                destroy()
                nativeOnExit()
            }

            binding.buyButton.setOnClickListener {
                nativeSendBuy()
            }

        }
    }

    private fun show(money: Int, total: Int, prices: IntArray) {
        activity.runOnUiThread {
            binding.balanceText.text    = Samp.formatter.format(money.toLong())
            binding.priceText.text      = Samp.formatter.format(total.toLong())
            adapter.updatePrices(prices)
        }
    }

    override fun onColorPickerSelected(color: Int) {
        activity.runOnUiThread { binding.mainLayout.visibility = View.VISIBLE }

        nativeSendChangeValue(valueType.ordinal)

        if (valueType == ValueType.VALUE_TYPE_BODY1_COLOR ||
            valueType == ValueType.VALUE_TYPE_BODY2_COLOR ||
            valueType == ValueType.VALUE_TYPE_WHEEL_COLOR) {
                Samp.playUrlSound("http://files.russia.online/sounds/styling/spray.mp3")
        }
    }

    override fun onColorPickerChange(color: Int) {
        nativeChangeValue(valueType.ordinal, color)
    }

    override fun onColorPickerClose() {
        activity.runOnUiThread { binding.mainLayout.visibility = View.VISIBLE }
        nativeResetTuning(valueType.ordinal)
    }

    private fun showColorPicker(type: ValueType, withAlpha: Boolean, withBrithness: Boolean) {
        if (!nativeIsAvailable(type.ordinal)) return

        activity.runOnUiThread {
            binding.mainLayout.visibility = View.GONE
            ColorPicker(this).show(withAlpha, withBrithness, nativeGetCurrentValue(type.ordinal))
        }
        valueType = type
    }
    private fun showSelectMenu(type: ValueType, list: List<TuningSelectMenuItem>) {
        if (!nativeIsAvailable(type.ordinal)) return

        activity.runOnUiThread {
            binding.mainLayout.visibility = View.GONE

            TuningSelectMenu(nativeGetCurrentValue(type.ordinal), list, this);
        }

        valueType = type
    }

    override fun onClickItem(pos: Int) {
        when(pos) {
            ValueType.VALUE_TYPE_NEON_TYPE.ordinal -> {
                val neonList = listOf(
                    TuningSelectMenuItem("Нет неона", 0, 0),
                    TuningSelectMenuItem("Статичный цвет", 500_000, 1),
                    TuningSelectMenuItem("Переливающийся", 600_000, 2),
                    TuningSelectMenuItem("Моргающий", 600_000, 3),
                    TuningSelectMenuItem("Резкий", 600_000, 4)
                )
                showSelectMenu(ValueType.VALUE_TYPE_NEON_TYPE, neonList)
            }
            ValueType.VALUE_TYPE_NEON_COLOR.ordinal -> {
                showColorPicker(ValueType.VALUE_TYPE_NEON_COLOR, false, false)
            }
            ValueType.VALUE_TYPE_LIGHT_COLOR.ordinal -> {
                showColorPicker(ValueType.VALUE_TYPE_LIGHT_COLOR, false, false)
            }
            ValueType.VALUE_TYPE_TONER_COLOR.ordinal -> {
                showColorPicker(ValueType.VALUE_TYPE_TONER_COLOR, true, true)
            }
            ValueType.VALUE_TYPE_BODY1_COLOR.ordinal -> {
                showColorPicker(ValueType.VALUE_TYPE_BODY1_COLOR, false, true)
                Samp.playUrlSound("http://files.russia.online/sounds/styling/shake_ballon.mp3")
            }
            ValueType.VALUE_TYPE_BODY2_COLOR.ordinal -> {
                showColorPicker(ValueType.VALUE_TYPE_BODY2_COLOR, false, true)
                Samp.playUrlSound("http://files.russia.online/sounds/styling/shake_ballon.mp3")
            }
            ValueType.VALUE_TYPE_WHEEL_COLOR.ordinal -> {
                showColorPicker(ValueType.VALUE_TYPE_WHEEL_COLOR, false, true)
                Samp.playUrlSound("http://files.russia.online/sounds/styling/shake_ballon.mp3")
            }
            ValueType.VALUE_TYPE_STROB.ordinal -> {
                val strobList = listOf(
                    TuningSelectMenuItem("Нет стробоскопа", 0, 0),
                    TuningSelectMenuItem("Полицейский", 500_000, 1),
                    TuningSelectMenuItem("Режим 2", 500_000, 2),
                    TuningSelectMenuItem("Режим 3", 500_000, 3),
                    TuningSelectMenuItem("Режим 4", 500_000, 4)
                )
                showSelectMenu(ValueType.VALUE_TYPE_STROB, strobList);
            }
            ValueType.VALUE_TYPE_VINYL.ordinal -> {
                val MAX_VINYLS_INDEX = 65
                val vinylsList = mutableListOf<TuningSelectMenuItem>()

                vinylsList.add(
                    TuningSelectMenuItem("Нет винила", 0, -1)
                )
                for(index in 0..MAX_VINYLS_INDEX) {
                    vinylsList.add(
                        TuningSelectMenuItem("Винил #$index", 1_000_000, index)
                    )
                }

                showSelectMenu(ValueType.VALUE_TYPE_VINYL, vinylsList)
            }
        }
    }

    // ---------------------------------------------------------------------



    override fun onChangeSelectMenu(newValue: Int) {
        nativeChangeValue(valueType.ordinal, newValue)
    }

    override fun onExitSelectMenu() {
        binding.mainLayout.visibility = View.VISIBLE
    }

    override fun onSelectedSelectMenu() {
        nativeSendChangeValue(valueType.ordinal)

        binding.mainLayout.visibility = View.VISIBLE
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}