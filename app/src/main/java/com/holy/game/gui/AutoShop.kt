package com.holy.game.gui

import android.annotation.SuppressLint
import android.view.View
import android.view.ViewStub
import android.widget.Button
import android.widget.TextView
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity

class AutoShop {
    private val AUTOSHOP_BUTTON_COLOR_LEFT = 1
    private val AUTOSHOP_BUTTON_COLOR_RIGHT = 2
    private val AUTOSHOP_BUTTON_LEFT = 3
    private val AUTOSHOP_BUTTON_TESTDRIVE = 4
    private val AUTOSHOP_BUTTON_BUY = 5
    private val AUTOSHOP_BUTTON_EXIT = 6
    private val AUTOSHOP_BUTTON_RIGHT = 7
    private val AUTOSHOP_BUTTON_CAMERA = 8

    private lateinit var autoshop_buybutt: Button
    private lateinit var autoshop_colorleft: Button
    private lateinit var autoshop_colorright: Button
    private lateinit var autoshop_rightbutt: Button
    private lateinit var autoshop_leftbutt: Button
    private lateinit var autoshop_camerabutt: Button
    private lateinit var testdrive_butt: Button
    private lateinit var autoshop_exitbutt: Button
    private lateinit var autoshop_modelname: TextView
    private lateinit var autoshop_pricevalue: TextView
    private lateinit var autoshop_maxspeed_value: TextView
    private lateinit var autoshop_acceleration_value: TextView
    private lateinit var autoshop_available_value: TextView
    private lateinit var autoshop_gear_value: TextView

    private val autoShopStub = activity.findViewById<ViewStub>(R.id.autoShopStub)
    private lateinit var autoShopInflated: View

    private external fun native_SendAutoShopButton(buttonID: Int)

    init {
        activity.runOnUiThread {
            if(autoShopStub == null)
                return@runOnUiThread

            autoShopInflated = autoShopStub.inflate()

            autoshop_buybutt = activity.findViewById(R.id.autoshop_buybutt)
            autoshop_colorleft = activity.findViewById(R.id.autoshop_colorleft)
            autoshop_colorright = activity.findViewById(R.id.autoshop_colorright)
            autoshop_rightbutt = activity.findViewById(R.id.autoshop_rightbutt)
            autoshop_leftbutt = activity.findViewById(R.id.autoshop_leftbutt)
            autoshop_camerabutt = activity.findViewById(R.id.autoshop_camerabutt)
            testdrive_butt = activity.findViewById(R.id.testdrive_butt)
            autoshop_exitbutt = activity.findViewById(R.id.autoshop_exitbutt)
            autoshop_modelname = activity.findViewById(R.id.autoshop_modelname)
            autoshop_pricevalue = activity.findViewById(R.id.autoshop_pricevalue)
            autoshop_maxspeed_value = activity.findViewById(R.id.autoshop_maxspeed_value)
            autoshop_acceleration_value = activity.findViewById(R.id.autoshop_acceleration_value)
            autoshop_available_value = activity.findViewById(R.id.autoshop_available_value)
            autoshop_gear_value = activity.findViewById(R.id.autoshop_gear_value)

            autoshop_colorleft.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_COLOR_LEFT)
            }
            autoshop_colorright.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_COLOR_RIGHT)
            }
            autoshop_leftbutt.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_LEFT)
            }
            testdrive_butt.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_TESTDRIVE)
            }
            autoshop_buybutt.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_BUY)
            }
            autoshop_exitbutt.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_EXIT)
            }
            autoshop_rightbutt.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_RIGHT)
            }
            autoshop_camerabutt.setOnClickListener {
                native_SendAutoShopButton(AUTOSHOP_BUTTON_CAMERA)
            }
        }
    }

    @SuppressLint("SetTextI18n", "DefaultLocale")
    private fun update(
        name: String?,
        price: Int,
        count: Int,
        maxspeed: Float,
        acceleration: Float,
        gear: Int
    ) {
        activity.runOnUiThread {
            when (gear) {
                1 -> {
                    autoshop_gear_value.text = "Задний"
                }

                2 -> {
                    autoshop_gear_value.text = "Передний"
                }

                3 -> {
                    autoshop_gear_value.text = "Полный"
                }

                4 -> {
                    autoshop_gear_value.text = "Цепной"
                }

                else -> {
                    autoshop_gear_value.text = "Не указан"
                }
            }
            autoshop_pricevalue.text = Samp.formatter.format(price.toLong()) + " руб."
            autoshop_available_value.text = String.format("%d", count)
            autoshop_acceleration_value.text = String.format("%.1f", acceleration) + " с."
            autoshop_maxspeed_value.text = String.format("%.0f", maxspeed) + " км/ч"
            autoshop_modelname.text = name
        }
    }

    private fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(autoShopInflated, R.id.autoShopStub, R.layout.autoshop)
        }
    }
}