package com.holy.game.gui.casino

import android.annotation.SuppressLint
import android.graphics.Color
import android.text.Editable
import android.text.TextWatcher
import android.view.View
import android.view.ViewStub
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.content.res.AppCompatResources
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity

@SuppressLint("SetTextI18n")
class BuySellChip(isSell: Boolean, balance: Int) {
    // chip
    private lateinit var casino_chip_action_caption: TextView
    private lateinit var casino_chip_balance_text: TextView
    private lateinit var casino_chip_currency_text: TextView
    private lateinit var casino_chip_bg_2: ConstraintLayout
    private lateinit var casino_chip_get_count: TextView
    private lateinit var casino_chip_input: EditText
    private lateinit var casino_chip_buy_button: MaterialButton
    private lateinit var casino_chip_exit_button: View
    private lateinit var casino_chip_back_button: ImageView

    private val casinoChipStub = activity.findViewById<ViewStub>(R.id.casinoChipStub)
    private lateinit var casinoChipInflated: View

    external fun native_sendClick(buttonID: Int, input: Long, isSell: Boolean)

    init {
        activity.runOnUiThread {
            casinoChipInflated = casinoChipStub.inflate()

            casino_chip_action_caption  = activity.findViewById(R.id.casino_chip_action_caption)
            casino_chip_balance_text    = activity.findViewById(R.id.casino_chip_balance_text)
            casino_chip_currency_text   = activity.findViewById(R.id.casino_chip_currency_text)
            casino_chip_bg_2            = activity.findViewById(R.id.casino_chip_bg_2)
            casino_chip_get_count       = activity.findViewById(R.id.casino_chip_get_count)
            casino_chip_input           = activity.findViewById(R.id.casino_chip_input)
            casino_chip_buy_button      = activity.findViewById(R.id.casino_chip_buy_button)
            casino_chip_exit_button     = activity.findViewById(R.id.casino_chip_exit_button)
            casino_chip_back_button     = activity.findViewById(R.id.casino_chip_back_button)

            casino_chip_back_button.setOnClickListener {
                native_sendClick(0, 0, isSell)

                hide()
            }
            casino_chip_input.addTextChangedListener(object : TextWatcher {
                override fun beforeTextChanged(
                    charSequence: CharSequence,
                    i: Int,
                    i1: Int,
                    i2: Int
                ) {
                }

                override fun onTextChanged(charSequence: CharSequence, i: Int, i1: Int, i2: Int) {
                    chipUpdateGetting(isSell)
                }

                override fun afterTextChanged(editable: Editable) {}
            })
            casino_chip_buy_button.setOnClickListener {
                native_sendClick(1, chipGetCount, isSell)

                hide()
            }
            casino_chip_exit_button.setOnClickListener {
                native_sendClick(2, chipGetCount, isSell)

                hide()
            }

            if (isSell) {
                casino_chip_action_caption.text = "Продать фишки"
                casino_chip_balance_text.text = String.format("%d фишек", balance)
                casino_chip_currency_text.text = "1 = 950 руб."
                casino_chip_bg_2.background =
                    AppCompatResources.getDrawable(activity, R.drawable.casino_chip_bg_2_sell)
                casino_chip_buy_button.text = "продать"
                casino_chip_buy_button.setTextColor(Color.parseColor("#A01618"))
            } else {
                casino_chip_action_caption.text = "Купить фишки"
                casino_chip_balance_text.text = String.format("%d руб.", balance)
                casino_chip_currency_text.text = "1 = 1.000 руб."
                casino_chip_bg_2.background =
                    AppCompatResources.getDrawable(activity, R.drawable.casino_chip_bg_2_buy)
                casino_chip_buy_button.text = "купить"
                casino_chip_buy_button.setTextColor(Color.parseColor("#017088"))

            }
            casino_chip_input.text = null
            chipUpdateGetting(isSell)

        }
    }

    private fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(casinoChipInflated, R.id.casinoChipStub, R.layout.casino_chip)
        }
    }

    private val chipGetCount: Long
        get() {
            if (casino_chip_input.text.toString().isEmpty()) return 0L
            val tmp: Long = try {
                casino_chip_input.text.toString().toLong()
            } catch (e: Exception) {
                return 0L
            }
            return if (tmp < 0 || tmp > 2000000000) 0L else tmp
        }

    fun chipUpdateGetting(isSell: Boolean) {
        activity.runOnUiThread {
            if (isSell) {
                casino_chip_get_count.text = String.format(
                    "К получению: %s руб.", Samp.formatter.format(
                        chipGetCount * 950L
                    )
                )
            } else {
                casino_chip_get_count.text = String.format(
                    "К оплате: %s руб.", Samp.formatter.format(
                        chipGetCount * 1000L
                    )
                )
            }
        }
    }
}