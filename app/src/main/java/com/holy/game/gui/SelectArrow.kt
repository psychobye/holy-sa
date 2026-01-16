package com.holy.game.gui

import android.view.View
import android.widget.ImageView
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp.Companion.activity

interface SelectArrowClickListener {
    fun selectArrowClickTo(clickType: SelectArrow.ClickType)
}

class SelectArrow(listener: SelectArrowClickListener) {

    enum class ClickType {
        LEFT,
        RIGHT,
        OK,
        CANCEL
    }

    private val select_arrow_main_layout = activity.findViewById<ConstraintLayout>(R.id.select_arrow_main_layout)
    private val select_arrow_right = activity.findViewById<ImageView>(R.id.select_arrow_right)
    private val select_arrow_left = activity.findViewById<ImageView>(R.id.select_arrow_left)
    private val select_arrow_ok_butt = activity.findViewById<MaterialButton>(R.id.select_arrow_ok_butt)
    private val select_arrow_cancel_butt = activity.findViewById<MaterialButton>(R.id.select_arrow_cancel_butt)

    init {
        activity.runOnUiThread {
            select_arrow_right.setOnClickListener { listener.selectArrowClickTo(ClickType.RIGHT) }
            select_arrow_left.setOnClickListener { listener.selectArrowClickTo(ClickType.LEFT) }
            select_arrow_ok_butt.setOnClickListener {
                hide()
                listener.selectArrowClickTo(ClickType.OK)
            }
            select_arrow_cancel_butt.setOnClickListener {
                hide()
                listener.selectArrowClickTo(ClickType.CANCEL)
            }
        }
    }

    fun hide() {
        activity.runOnUiThread {
            select_arrow_main_layout.visibility = View.GONE
        }
    }

    fun show() {
        activity.runOnUiThread {
            select_arrow_main_layout.visibility = View.VISIBLE
        }
    }
}