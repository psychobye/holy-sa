package com.lit.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.view.View
import android.widget.ImageView
import android.widget.TextView
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.CasinoBcBinding
import com.rommansabbir.animationx.Slide
import com.rommansabbir.animationx.animationXSlide
import java.util.Random

class CasinoBaccarat : NativeGui<CasinoBcBinding>(CasinoBcBinding::class) {
    private var youWin = false
    private var bIsTempToggle = true
    private var currTime = 0
    private var lastCurrBet = 0
    private var lastBetType = BET_TYPE_NONE
    private var currentBet = 0
    private var currentBetType = BET_TYPE_NONE

    private enum class ChipType {
        CHIP_TYPE_NONE, CHIP_TYPE_1, CHIP_TYPE_5, CHIP_TYPE_25, CHIP_TYPE_100, CHIP_TYPE_500, CHIP_TYPE_1000
    }

    private var selectedChipType = ChipType.CHIP_TYPE_NONE
    private var bOffSound = false

    // last wins
    private var casinoBcLastWins = ArrayList<ImageView>()
    private var lastBets = ArrayList<Int>()
    private var betsound = 0

    private external fun sendAddBet(sum: Int, bettype: Int)
    private external fun exit()

    private var cardsValues = arrayOf("0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K")
    private var cardsSuitRes = arrayOf("card_clubs", "card_diamonds", "card_hearts", "card_spades")
    private var cardColors = arrayOf("#000000", "#F24E1E", "#F24E1E", "#000000")

    init {
        activity.runOnUiThread {
            binding.offSoundButt.setOnClickListener {
                activity.runOnUiThread {
                    it.startAnimation(Samp.clickAnim)
                    bOffSound = !bOffSound

                    if (bOffSound)
                        binding.offSoundButt.imageTintList = ColorStateList.valueOf(Color.parseColor("#f44336"))
                    else
                        binding.offSoundButt.imageTintList = ColorStateList.valueOf(Color.parseColor("#9e9e9e"))
                }
            }
            binding.cancelButt.setOnClickListener { view: View ->
                if (lastBets.size == 0) return@setOnClickListener
                if (currTime < TIME_TO_BET) {
                    playNoMoreBets()
                    return@setOnClickListener
                }
                sendAddBet(lastBets[lastBets.size - 1] * -1, BET_TYPE_NONE)
                lastBets.removeAt(lastBets.size - 1)
                view.startAnimation(Samp.clickAnim)
            }
            binding.repeatButt.setOnClickListener { view: View ->
                if (currTime < TIME_TO_BET) {
                    playNoMoreBets()
                    return@setOnClickListener
                }
                if (currentBet != 0) { // x2
                    sendAddBet(currentBet, currentBetType)
                    lastBets.add(currentBet)
                }
                else {
                    if (lastCurrBet <= 0) return@setOnClickListener
                    sendAddBet(lastCurrBet, lastBetType)
                    lastBets.add(lastCurrBet)
                }
                view.startAnimation(Samp.clickAnim)
            }

            // last wins
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_1))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_2))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_3))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_4))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_5))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_6))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_7))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_8))
            casinoBcLastWins.add(activity.findViewById(R.id.casino_bc_last_win_9))
            binding.exitButt.setOnClickListener {
                exit()
            }

            // ==== выделение фишек
            binding.chip1.setOnClickListener {
                changeChipSelect(binding.chip1)
                selectedChipType = ChipType.CHIP_TYPE_1
            }

            binding.chip5.setOnClickListener {
                changeChipSelect(binding.chip5)
                selectedChipType = ChipType.CHIP_TYPE_5
            }
            binding.chip25.setOnClickListener {
                changeChipSelect(binding.chip25)
                selectedChipType = ChipType.CHIP_TYPE_25
            }
            binding.chip100.setOnClickListener {
                changeChipSelect(binding.chip100)
                selectedChipType = ChipType.CHIP_TYPE_100
            }
            binding.chip500.setOnClickListener {
                changeChipSelect(binding.chip500)
                selectedChipType = ChipType.CHIP_TYPE_500
            }
            binding.chip1000.setOnClickListener {
                changeChipSelect(binding.chip1000)
                selectedChipType = ChipType.CHIP_TYPE_1000
            }

            // ==============
            binding.redArea.setOnClickListener {
                if (selectedChipType == ChipType.CHIP_TYPE_NONE) return@setOnClickListener
                if (currTime < TIME_TO_BET) {
                    playNoMoreBets()
                    return@setOnClickListener
                }
                sendAddBet(getChipValue(selectedChipType), BET_TYPE_RED)
                lastBets.add(getChipValue(selectedChipType))
            }

            binding.greenArea.setOnClickListener {
                if (selectedChipType == ChipType.CHIP_TYPE_NONE) return@setOnClickListener
                if (currTime < TIME_TO_BET) {
                    playNoMoreBets()
                    return@setOnClickListener
                }
                sendAddBet(getChipValue(selectedChipType), BET_TYPE_GREEN)
                lastBets.add(getChipValue(selectedChipType))
            }

            binding.yellowArea.setOnClickListener {
                if (selectedChipType == ChipType.CHIP_TYPE_NONE) return@setOnClickListener
                if (currTime < TIME_TO_BET) {
                    playNoMoreBets()
                    return@setOnClickListener
                }
                sendAddBet(getChipValue(selectedChipType), BET_TYPE_YELLOW)
                lastBets.add(getChipValue(selectedChipType))
            }
            betsound = Samp.soundPool.load(activity, R.raw.betchip, 0)
        }
    }

    private fun update(redCard: Int, yellowCard: Int, totalBets: Int, totalRed: Int, totalYellow: Int, totalGreen: Int, time: Int, betType: Int, betSum: Int, winner: Int, balance: Int) {
        currTime = time
        activity.runOnUiThread {
            val timeText = binding.timerBg.getChildAt(0) as TextView
            if (bIsTempToggle)
                binding.mainLayout.visibility = View.VISIBLE

            binding.balanceText.text = String.format("%s", Samp.formatter.format(balance.toLong()))
            if (time == 4) {
                showWinner(winner)
            }
            if (time == ROUND_TIME) {
                if (!bOffSound) {
                    when (Random().nextInt(2)) {
                        0 -> {
                            Samp.playUrlSound("http://files.russia.online/sounds/casino/go_bet_1.wav")
                        }

                        else -> {
                            Samp.playUrlSound("http://files.russia.online/sounds/casino/go_bet_2.wav")
                        }
                    }
                }
                reset()
            }
            if (time == TIME_TO_BET) {
                playNoMoreBets()
            }
            if (time > TIME_TO_BET) timeText.text = String.format("%d", time - TIME_TO_BET) else timeText.text = "-"

            // percents
            if (totalBets > 0) {
                binding.redPercent.text = String.format("%.0f%%", totalRed.toFloat() / totalBets * 100f)
                binding.yellowPercent.text = String.format("%.0f%%", totalYellow.toFloat() / totalBets * 100f)
                binding.greenPercent.text = String.format("%.0f%%", totalGreen.toFloat() / totalBets * 100f)
            }
            if (redCard != 0) {
                if (binding.redCard.visibility == View.GONE) {
                    updateRedCard(redCard)
                }
            }
            if (yellowCard != 0) {
                if (binding.yellowCard.visibility == View.GONE) {
                    updateYellowCard(yellowCard)
                }
            }

            // bets
            if (betSum != 0) {
                binding.repeatButt.setImageResource(R.drawable.casino_bc_x2_button)
            }
            else {
                binding.repeatButt.setImageResource(R.drawable.casino_bc_repeat_button)
            }
            currentBetType = betType
            if (betSum != 0) {
                if (currentBet != betSum) {
                    currentBet = betSum
                    val chipCount: TextView
                    if (betType == BET_TYPE_RED) {
                        chipCount = binding.redChip.getChildAt(0) as TextView
                        if (binding.redChip.visibility == View.GONE) {
                            binding.redChip.visibility = View.VISIBLE
                            binding.redChip.animationXSlide(Slide.SLIDE_IN_UP, 400)
                        }
                        if (!bOffSound) Samp.soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f)
                    }
                    else if (betType == BET_TYPE_YELLOW) {
                        chipCount = binding.yellowChip.getChildAt(0) as TextView
                        if (binding.yellowChip.visibility == View.GONE) {
                            binding.yellowChip.visibility = View.VISIBLE
                            binding.yellowChip.animationXSlide(Slide.SLIDE_IN_UP, 400)
                        }
                        if (!bOffSound) Samp.soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f)
                    }
                    else if (betType == BET_TYPE_GREEN) {
                        chipCount = binding.greenChip.getChildAt(0) as TextView
                        if (binding.greenChip.visibility == View.GONE) {
                            binding.greenChip.visibility = View.VISIBLE
                            binding.greenChip.animationXSlide(Slide.SLIDE_IN_UP, 400)
                        }
                        if (!bOffSound) Samp.soundPool.play(betsound, 0.2f, 0.2f, 1, 0, 1.0f)
                    }
                    else return@runOnUiThread
                    chipCount.text = String.format("%d", betSum)
                }
            }
            else {
                if (binding.yellowChip.visibility == View.VISIBLE || binding.redChip.visibility == View.VISIBLE || binding.greenChip.visibility == View.VISIBLE) {
                    currentBet = betSum
                    binding.yellowChip.visibility = View.GONE
                    binding.redChip.visibility = View.GONE
                    binding.greenChip.visibility = View.GONE
                }
            }
        }
    }

    private fun playNoMoreBets() {
        if (bOffSound) return
        when (Random().nextInt(3)) {
            0 -> {
                Samp.playUrlSound("http://files.russia.online/sounds/casino/no_bet_1.wav")
            }

            1 -> {
                Samp.playUrlSound("http://files.russia.online/sounds/casino/no_bet_2.wav")
            }

            else -> {
                Samp.playUrlSound("http://files.russia.online/sounds/casino/no_bet_3.wav")
            }
        }
    }

    private fun reset() {
        if (!bOffSound) {
            if (youWin) {
                Samp.playUrlSound("http://files.russia.online/sounds/casino/chip_win.mp3")
            }
            else {
                Samp.playUrlSound("http://files.russia.online/sounds/casino/cards_clear.mp3")
            }
        }
        lastBets.clear()
        lastCurrBet = currentBet
        lastBetType = currentBetType
        currentBet = 0
        currentBetType = BET_TYPE_NONE
        youWin = false
        binding.yellowCard.visibility = View.GONE
        binding.redCard.visibility = View.GONE
        binding.winLayout.visibility = View.GONE
    }

    private fun tempToggle(toggle: Boolean) {
        activity.runOnUiThread {
            if (toggle)
                binding.mainLayout.visibility = View.VISIBLE
            else
                binding.mainLayout.visibility = View.GONE
        }
        bIsTempToggle = toggle
    }


    private fun updateYellowCard(cardNum: Int) {
        if (!bOffSound)
            Samp.playUrlSound("http://files.russia.online/sounds/casino/cards.mp3")

        val rnd = Random().nextInt(4)
        val textTop = binding.yellowCard.getChildAt(0) as TextView
        val textBottom = binding.yellowCard.getChildAt(1) as TextView
        val suit = binding.yellowCard.getChildAt(2) as ImageView
        textTop.text = cardsValues[cardNum]
        textTop.setTextColor(Color.parseColor(cardColors[rnd]))
        textBottom.text = cardsValues[cardNum]
        textBottom.setTextColor(Color.parseColor(cardColors[rnd]))
        suit.setImageResource(activity.resources.getIdentifier(cardsSuitRes[rnd], "drawable", activity.packageName))
        binding.yellowCard.visibility = View.VISIBLE
        binding.yellowCard.animationXSlide(Slide.SLIDE_IN_UP, 400)
    }

    private fun updateRedCard(cardNum: Int) {
        if (!bOffSound) Samp.playUrlSound("http://files.russia.online/sounds/casino/cards.mp3")
        val rnd = Random().nextInt(4)
        val textTop = binding.redCard.getChildAt(0) as TextView
        val textBottom = binding.redCard.getChildAt(1) as TextView
        val suit = binding.redCard.getChildAt(2) as ImageView
        textTop.text = cardsValues[cardNum]
        textTop.setTextColor(Color.parseColor(cardColors[rnd]))
        textBottom.text = cardsValues[cardNum]
        textBottom.setTextColor(Color.parseColor(cardColors[rnd]))
        suit.setImageResource(activity.resources.getIdentifier(cardsSuitRes[rnd], "drawable", activity.packageName))
        binding.redCard.visibility = View.VISIBLE
        binding.redCard.animationXSlide(Slide.SLIDE_IN_UP, 400)
    }

    private fun showWinner(winner: Int) {
        if (currentBet <= 0) return
        activity.runOnUiThread {
            val casino_bc_win_value = activity.findViewById<TextView>(R.id.casino_bc_win_value)
            val casino_bc_win_icon = activity.findViewById<ImageView>(R.id.casino_bc_win_icon)
            val casino_bc_win_text = activity.findViewById<TextView>(R.id.casino_bc_win_text)
            val casino_bc_win_header = activity.findViewById<View>(R.id.casino_bc_win_header)
            if (currentBetType == winner) {
                if (!bOffSound) {
                    when (Random().nextInt(3)) {
                        0 -> {
                            Samp.playUrlSound("http://files.russia.online/sounds/casino/win_1.wav")
                        }

                        1 -> {
                            Samp.playUrlSound("http://files.russia.online/sounds/casino/win_2.wav")
                        }

                        else -> {
                            Samp.playUrlSound("http://files.russia.online/sounds/casino/win_3.wav")
                        }
                    }
                }
                youWin = true
                casino_bc_win_text.text = "Вы выиграли"
                if (currentBetType != BET_TYPE_GREEN) {
                    casino_bc_win_value.text = String.format("%d", currentBet * 2)
                }
                else {
                    casino_bc_win_value.text = String.format("%d", currentBet * 11)
                }
                casino_bc_win_value.visibility = View.VISIBLE
                casino_bc_win_icon.visibility = View.VISIBLE
            }
            else {
                casino_bc_win_text.text = "Вы проиграли"
                casino_bc_win_value.visibility = View.GONE
                casino_bc_win_icon.visibility = View.GONE
            }
            when (winner) {
                BET_TYPE_RED -> {
                    binding.winCaption.text = "Black Casha"
                    casino_bc_win_header.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#FF3131"))
                }

                BET_TYPE_YELLOW -> {
                    binding.winCaption.text = "Live Russia"
                    casino_bc_win_header.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#FFC223"))
                }

                BET_TYPE_GREEN -> {
                    binding.winCaption.text = "Ничья"
                    casino_bc_win_header.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#205537"))
                }

                else -> {}
            }
            binding.winLayout.visibility = View.VISIBLE
        }
    }

    private fun changeChipSelect(selected: ImageView?) {
        binding.chip1.backgroundTintList = null
        binding.chip5.backgroundTintList = null
        binding.chip25.backgroundTintList = null
        binding.chip100.backgroundTintList = null
        binding.chip500.backgroundTintList = null
        binding.chip1000.backgroundTintList = null
        selected!!.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#b4004e"))
    }

    private fun getChipValue(chipType: ChipType?): Int {
        return when (chipType) {
            ChipType.CHIP_TYPE_1 -> 1
            ChipType.CHIP_TYPE_5 -> 5
            ChipType.CHIP_TYPE_25 -> 25
            ChipType.CHIP_TYPE_100 -> 100
            ChipType.CHIP_TYPE_500 -> 500
            ChipType.CHIP_TYPE_1000 -> 1000
            else -> 0
        }
    }

    private fun updateLastWins(lastwins: IntArray) {
        activity.runOnUiThread {
            for (i in lastwins.indices) {
                if (lastwins[i] == BET_TYPE_RED) {
                    casinoBcLastWins[i].imageTintList = ColorStateList.valueOf(Color.parseColor("#5C1C1D"))
                }
                else if (lastwins[i] == BET_TYPE_YELLOW) {
                    casinoBcLastWins[i].imageTintList = ColorStateList.valueOf(Color.parseColor("#9F8731"))
                }
                else {
                    casinoBcLastWins[i].imageTintList = ColorStateList.valueOf(Color.parseColor("#205537"))
                }
            }
        }
    }

    override fun destroy() {
        if (!bOffSound) {
            when (Random().nextInt(2)) {
                0 -> {
                    Samp.playUrlSound("http://files.russia.online/sounds/casino/exit_2.wav")
                }

                else -> {
                    Samp.playUrlSound("http://files.russia.online/sounds/casino/exit_1.wav")
                }
            }
        }
        activity.runOnUiThread {
            reset()
            Samp.soundPool.unload(betsound)

            super.destroy()
        }

    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    companion object {
        const val ROUND_TIME = 25
        const val TIME_TO_BET = 10
        const val INVALID_CARD = -1
        const val BET_TYPE_NONE = 0
        const val BET_TYPE_RED = 1
        const val BET_TYPE_YELLOW = 2
        const val BET_TYPE_GREEN = 3
    }
}