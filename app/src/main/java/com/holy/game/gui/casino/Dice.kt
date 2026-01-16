package com.holy.game.gui.casino

import android.annotation.SuppressLint
import android.view.View
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.CasinoDiceBinding
import com.lit.game.gui.NativeGui

class Dice : NativeGui<CasinoDiceBinding>(CasinoDiceBinding::class) {
    private var isTempHiden = false
    private val CASINO_DICE_BUTTON_BET = 0
    private val CASINO_DICE_BUTTON_DICE = 1
    private val CASINO_DICE_BUTTON_EXIT = 2

    private external fun nativeSendButton(buttId: Int)

    init {
        activity.runOnUiThread {

            binding.makeBetButton.setOnClickListener{
                nativeSendButton(CASINO_DICE_BUTTON_BET)
            }
            binding.diceButton.setOnClickListener {
                nativeSendButton(CASINO_DICE_BUTTON_DICE)
            }
            binding.exitButton.setOnClickListener {
                nativeSendButton(CASINO_DICE_BUTTON_EXIT)
            }

            binding.mainLayout.visibility = View.VISIBLE
        }
    }

    fun tempToggle(toggle: Boolean) {
        isTempHiden = !toggle
        if (toggle) {
            activity.runOnUiThread { binding.mainLayout.visibility = View.VISIBLE }
        }
        else {
            activity.runOnUiThread { binding.mainLayout.visibility = View.GONE }
        }
    }

    @SuppressLint("DefaultLocale", "SetTextI18n")
    fun update(tableID: Int, tableBet: Int, tableBank: Int, money: Int, player1name: String?, player1stat: Int, player2name: String?, player2stat: Int, player3name: String?, player3stat: Int, player4name: String?, player4stat: Int, player5name: String?, player5stat: Int, time: Int, crupName: String?, crupId: Int) {
        activity.runOnUiThread {
            if (time > 1) {
                binding.timeText.text = String.format("До конца раунда: %d", time)
                binding.timeText.visibility = View.VISIBLE
            }
            else {
                binding.timeText.visibility = View.GONE
            }
            if (crupId != Samp.INVALID_PLAYER_ID)
                binding.dealerText.text = String.format("Крупье: %s[%d]", crupName, crupId)
            else
                binding.dealerText.text = "Крупье: --"

            binding.tableCaption.text = "Стол $tableID"
            binding.betText.text = Samp.formatter.format(tableBet.toLong()) + " руб."
            binding.bankText.text = Samp.formatter.format(tableBank.toLong()) + " руб."
            binding.myBalanceText.text = Samp.formatter.format(money.toLong()) + " руб."

            binding.playerName1.text = player1name
            if (player1stat == 0) {
                binding.playerStatus1.text = "--"
            }
            else if (player1stat == 1) {
                binding.playerStatus1.text = "++"
            }
            else {
                binding.playerStatus1.text = String.format("%d", player1stat)
            }

            binding.playerName2.text = player2name
            if (player2stat == 0) {
                binding.playerStatus2.text = "--"
            }
            else if (player2stat == 1) {
                binding.playerStatus2.text = "++"
            }
            else {
                binding.playerStatus2.text = String.format("%d", player2stat)
            }

            binding.playerName3.text = player3name
            if (player3stat == 0) {
                binding.playerStatus3.text = "--"
            }
            else if (player3stat == 1) {
                binding.playerStatus3.text = "++"
            }
            else {
                binding.playerStatus3.text = String.format("%d", player3stat)
            }

            binding.playerName4.text = player4name
            if (player4stat == 0) {
                binding.playerStatus4.text = "--"
            }
            else if (player4stat == 1) {
                binding.playerStatus4.text = "++"
            }
            else {
                binding.playerStatus4.text = String.format("%d", player4stat)
            }

            binding.playerName5.text = player5name
            if (player5stat == 0) {
                binding.playerStatus5.text = "--"
            }
            else if (player5stat == 1) {
                binding.playerStatus5.text = "++"
            }
            else {
                binding.playerStatus5.text = String.format("%d", player5stat)
            }

            if (!isTempHiden) {
                binding.mainLayout.visibility = View.VISIBLE
            }
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}