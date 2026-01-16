package com.lit.game.gui.tire_shop

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

class TireShop : NativeGui<StylingCenterBinding>(StylingCenterBinding::class), CustomSeekListener, TuningAdapterListener, TuningSelectMenuListener {

    private val TYPE_BUY = 1
    private val TYPE_SUSPENSION = 2
    private val TYPE_WHEEL_OFFSET = 3
    private val TYPE_TIRE_WIDTH = 4
    private val TYPE_DISK_RAD   = 5
    private val TYPE_DISK   = 6
    private val TYPE_EXIT_BUTTON = 7;
    private val TYPE_RAZVAL_FRONT = 11
    private val TYPE_RAZVAL_BACK = 12

    private val items = arrayListOf(
        TuningItem("Высота подвески", R.drawable.suspension_icon, TYPE_SUSPENSION),
        TuningItem("Проставки", R.drawable.wheel_offset_icon, TYPE_WHEEL_OFFSET),
        TuningItem("Сход-развал (перед.)", R.drawable.razval_icon, TYPE_RAZVAL_FRONT),
        TuningItem("Сход-развал (зад.)", R.drawable.razval_icon, TYPE_RAZVAL_BACK),
        TuningItem("Ширина шин", R.drawable.tire_width_icon, TYPE_TIRE_WIDTH),
        TuningItem("Радиус диска", R.drawable.disk_rad_icon, TYPE_DISK_RAD),
        TuningItem("Смена дисков", R.drawable.disk_group, TYPE_DISK),
    )
    private val adapter = TuningAdapter(items, this)

    private var valueType: Int = 0

    external fun native_getCurrentValue(type: Int): Float
    external fun native_onChange(type: Int, value: Float)
    external fun nativeSendClick(type: Int)

    init {
        activity.runOnUiThread {
            binding.recycle.layoutManager = LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false)
            binding.recycle.adapter = adapter

            // buy
            binding.backButton.setOnClickListener {
                nativeSendClick(TYPE_BUY)
            }

            // exit
            binding.exitButt.setOnClickListener {
                destroy()
            }
        }
    }

    private fun showCustomSeekBar(caption: String, valueType: Int, min: Float, max: Float, step: Float) {
        this.valueType = valueType

        CustomSeekBar(this).showSeek(
            caption,
            native_getCurrentValue(valueType),
            min,
            max,
            step
        )

        binding.mainLayout.visibility = View.GONE
    }
    override fun onChangeProgress(value: Float) {
        native_onChange(valueType, value);
    }

    override fun onCancel() {
        binding.mainLayout.visibility = View.VISIBLE
    }

    override fun onSucces(value: Float) {
        binding.mainLayout.visibility = View.VISIBLE
        nativeSendClick(valueType)
    }

    fun update(to_pay: Int, balance: Int) {
        activity.runOnUiThread {
            binding.balanceText.text = String.format("%s руб.", Samp.formatter.format(balance))
            binding.priceText.text = String.format("%s руб.", Samp.formatter.format(to_pay))
        }
    }

    private fun showSelectMenu(type: Int, list: List<TuningSelectMenuItem>) {
        activity.runOnUiThread {
            binding.mainLayout.visibility = View.GONE

            TuningSelectMenu(native_getCurrentValue(type).toInt(), list, this);
        }

        valueType = type
    }

    override fun onChangeSelectMenu(newValue: Int) {
        native_onChange(TYPE_DISK, newValue.toFloat())
    }

    override fun onExitSelectMenu() {
        binding.mainLayout.visibility = View.VISIBLE
    }

    override fun onSelectedSelectMenu() {
        nativeSendClick(valueType)
        binding.mainLayout.visibility = View.VISIBLE
    }

    override fun onClickItem(pos: Int) {
        when(pos) {
            TYPE_DISK -> {
                val diskList = listOf(
                    TuningSelectMenuItem("Стоковые", 0, 0),
                    TuningSelectMenuItem("Диски 1", 0, 1025),
                    TuningSelectMenuItem("Диски 2", 0, 1073),
                    TuningSelectMenuItem("Диски 3", 0, 1074),
                    TuningSelectMenuItem("Диски 4", 0, 1075),
                    TuningSelectMenuItem("Диски 5", 0, 1076),
                    TuningSelectMenuItem("Диски 6", 0, 1077),
                    TuningSelectMenuItem("Диски 7", 0, 1078),
                    TuningSelectMenuItem("Диски 8", 0, 1079),
                    TuningSelectMenuItem("Диски 9", 0, 1080),
                    TuningSelectMenuItem("Диски 10", 0, 1081),
                    TuningSelectMenuItem("Диски 11", 0, 1082),
                    TuningSelectMenuItem("Диски 12", 0, 1083),
                    TuningSelectMenuItem("Диски 13", 0, 1084),
                    TuningSelectMenuItem("Диски 14", 0, 1085),
                    TuningSelectMenuItem("Диски 15", 0, 1096),
                    TuningSelectMenuItem("Диски 16", 0, 1097),
                    TuningSelectMenuItem("Диски 17", 0, 1098)

                )
                showSelectMenu(pos, diskList)
                return
            }
            TYPE_SUSPENSION -> {
                showCustomSeekBar(
                    "Высота подвески",
                    TYPE_SUSPENSION,
                    -0.35f,
                    0.15f,
                    0.01f
                )
                return
            }

            TYPE_RAZVAL_FRONT -> {
                showCustomSeekBar(
                    "Развал-схождение (перед)",
                    TYPE_RAZVAL_FRONT,
                    -20.0f,
                    20.0f,
                    1.0f
                )
            }

            TYPE_RAZVAL_BACK -> {
                showCustomSeekBar(
                    "Развал-схождение (зад)",
                    TYPE_RAZVAL_BACK,
                    -20.0f,
                    20.0f,
                    1.0f
                )
            }

            TYPE_TIRE_WIDTH -> {
                showCustomSeekBar(
                    "Ширина резины",
                    TYPE_TIRE_WIDTH,
                    0.0f,
                    200.0f,
                    1.0f
                )
            }

            TYPE_DISK_RAD -> {
                showCustomSeekBar(
                    "Радиус диска",
                    TYPE_DISK_RAD,
                    0.50f,
                    1.4f,
                    0.05f
                )
            }

            TYPE_WHEEL_OFFSET -> {
                showCustomSeekBar(
                    "Проставки",
                    TYPE_WHEEL_OFFSET,
                    -20.0f,
                    10.0f,
                    1.0f
                )
            }
        }
    }

    override fun destroy() {
        super.destroy()
        nativeSendClick(TYPE_EXIT_BUTTON)
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}