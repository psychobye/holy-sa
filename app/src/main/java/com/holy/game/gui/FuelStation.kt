package com.holy.game.gui

import android.view.View
import android.view.ViewStub
import android.widget.ImageView
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity

class FuelStation(type: Int, price1: Int, price2: Int, price3: Int, price4: Int, price5: Int, maxCount: Int) {
    private lateinit var fuelstation_1: ConstraintLayout
    private lateinit var fuelstation_2: ConstraintLayout
    private lateinit var fuelstation_3: ConstraintLayout
    private lateinit var fuelstation_4: ConstraintLayout
    private lateinit var fuelstation_5: ConstraintLayout
    private lateinit var fuelstation_exit: ImageView
    private lateinit var fuelstationBuyButt: MaterialButton
    private lateinit var fuelstation_info: TextView
    private lateinit var fuelstation_1_info: TextView
    private lateinit var fuelstation_2_info: TextView
    private lateinit var fuelstation_3_info: TextView
    private lateinit var fuelstation_4_info: TextView
    private lateinit var fuelstation_5_info: TextView
    private lateinit var fuelstation_buyinfo: TextView
    private lateinit var fuelstation_literinfo: TextView
    private lateinit var fuelstation_bar: SeekBar

    private val fuelStationStub = activity.findViewById<ViewStub>(R.id.fuelStationStub)
    private lateinit var fuelStationInflated: View

    var fuelstation_active = 0
    var fuelprice = 0

    private external fun nativeSendClick(fueltype: Int, fuelliters: Int)

    init {
        activity.runOnUiThread {
            fuelStationInflated = fuelStationStub.inflate()

            fuelstation_1           = activity.findViewById(R.id.fuelstation_1)
            fuelstation_2           = activity.findViewById(R.id.fuelstation_2)
            fuelstation_3           = activity.findViewById(R.id.fuelstation_3)
            fuelstation_4           = activity.findViewById(R.id.fuelstation_4)
            fuelstation_5           = activity.findViewById(R.id.fuelstation_5)
            fuelstation_exit        = activity.findViewById(R.id.fuelstation_exit)
            fuelstationBuyButt      = activity.findViewById(R.id.fuelstationBuyButt)
            fuelstation_info        = activity.findViewById(R.id.fuelstation_info)
            fuelstation_1_info      = activity.findViewById(R.id.fuelstation_1_info)
            fuelstation_2_info      = activity.findViewById(R.id.fuelstation_2_info)
            fuelstation_3_info      = activity.findViewById(R.id.fuelstation_3_info)
            fuelstation_4_info      = activity.findViewById(R.id.fuelstation_4_info)
            fuelstation_5_info      = activity.findViewById(R.id.fuelstation_5_info)
            fuelstation_buyinfo     = activity.findViewById(R.id.fuelstation_buyinfo)
            fuelstation_literinfo   = activity.findViewById(R.id.fuelstation_literinfo)
            fuelstation_bar         = activity.findViewById(R.id.fuelstation_bar)

            fuelstation_bar.setOnSeekBarChangeListener(object : OnSeekBarChangeListener {
                override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                    if (fuelstation_active == 1) {
                        fuelprice = price1 * progress
                    }
                    else if (fuelstation_active == 2) {
                        fuelprice = price2 * progress
                    }
                    else if (fuelstation_active == 3) {
                        fuelprice = price3 * progress
                    }
                    else if (fuelstation_active == 4) {
                        fuelprice = price4 * progress
                    }
                    else if (fuelstation_active == 5) {
                        fuelprice = price5 * progress
                    }
                    fuelstation_buyinfo.text = String.format("%s руб.", Samp.formatter.format(fuelprice))
                    fuelstation_literinfo.text = String.format("%s л", progress)
                }

                override fun onStartTrackingTouch(seekBar: SeekBar) {}
                override fun onStopTrackingTouch(seekBar: SeekBar) {}
            })
            fuelstationBuyButt.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                hide(fuelstation_active, fuelstation_bar.progress)
            }
            fuelstation_exit.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                hide(0, 0)
            }
            fuelstation_1.setOnClickListener {
                setActiveItem(it)

                fuelstation_active = 1
                fuelprice = price1 * fuelstation_bar.progress
                val strpriceinfo1 = String.format("%s руб.", Samp.formatter.format(fuelprice))
                fuelstation_buyinfo.text = strpriceinfo1
            }
            fuelstation_2.setOnClickListener {
                setActiveItem(it)

                fuelstation_active = 2
                fuelprice = price2 * fuelstation_bar.progress
                val strpriceinfo2 = String.format("%s руб.", Samp.formatter.format(fuelprice))
                fuelstation_buyinfo.text = strpriceinfo2
            }
            fuelstation_3.setOnClickListener {
                setActiveItem(it)

                fuelstation_active = 3
                fuelprice = price3 * fuelstation_bar.progress
                val strpriceinfo3 = String.format("%s руб.", Samp.formatter.format(fuelprice))
                fuelstation_buyinfo.text = strpriceinfo3
            }
            fuelstation_4.setOnClickListener {
                setActiveItem(it)

                fuelstation_active = 4
                fuelprice = price4 * fuelstation_bar.progress
                val strpriceinfo4 = String.format("%s руб.", Samp.formatter.format(fuelprice))
                fuelstation_buyinfo.text = strpriceinfo4
            }
            fuelstation_5.setOnClickListener {
                setActiveItem(it)

                fuelstation_active = 5
                fuelprice = price5 * fuelstation_bar.progress
                val strpriceinfo5 = String.format("%s руб.", Samp.formatter.format(fuelprice))
                fuelstation_buyinfo.text = strpriceinfo5
            }

            fuelstation_1_info.text = String.format("%sр/литр", price1)
            fuelstation_2_info.text = String.format("%sр/литр", price2)
            fuelstation_3_info.text = String.format("%sр/литр", price3)
            fuelstation_4_info.text = String.format("%sр/литр", price5)
            fuelstation_5_info.text = String.format("%sр/литр", price4)

            fuelstation_buyinfo.text = "0 р"
            fuelstation_literinfo.text = "0 л"
            fuelstation_bar.max = maxCount
            fuelstation_bar.progress = 0
            fuelstation_active = type
            if (type == 1) {
                fuelstation_info.text = "Рекомендуемый тип топлива: АИ-92"
                setActiveItem(fuelstation_1)
            }
            else if (type == 2) {
                fuelstation_info.text = "Рекомендуемый тип топлива: АИ-95"
                setActiveItem(fuelstation_2)
            }
            else if (type == 3) {
                fuelstation_info.text = "Рекомендуемый тип топлива: АИ-98"
                setActiveItem(fuelstation_3)
            }
            else if (type == 4) {
                fuelstation_info.text = "Рекомендуемый тип топлива: ДТ"
                setActiveItem(fuelstation_4)
            }
            else if (type == 5) {
                fuelstation_info.text = "Рекомендуемый тип топлива: АИ-100"
                setActiveItem(fuelstation_5)
            }
        }
    }

    fun setActiveItem(view: View) {
        activity.runOnUiThread {
            fuelstation_1.setBackgroundResource(R.drawable.fuelstation_item_bg)
            fuelstation_2.setBackgroundResource(R.drawable.fuelstation_item_bg)
            fuelstation_3.setBackgroundResource(R.drawable.fuelstation_item_bg)
            fuelstation_4.setBackgroundResource(R.drawable.fuelstation_item_bg)
            fuelstation_5.setBackgroundResource(R.drawable.fuelstation_item_bg)

            view.startAnimation(Samp.clickAnim)
            view.setBackgroundResource(R.drawable.fuelstation_item_active_bg)
        }
    }

    fun hide(typefuel: Int, literfuel: Int) {
        activity.runOnUiThread {
            nativeSendClick(typefuel, literfuel)

            Samp.deInflatedViewStud(fuelStationInflated, R.id.fuelStationStub, R.layout.fuelstation)
        }
    }
}