package com.lit.game.gui.tuning

import com.lit.game.core.Samp
import com.lit.game.databinding.TuningArrowsBinding
import com.lit.game.gui.NativeGui


interface TuningSelectMenuListener {
    fun onChangeSelectMenu(newValue: Int)
    fun onExitSelectMenu()
    fun onSelectedSelectMenu()
}


class TuningSelectMenu(val startValue: Int, val list: List<TuningSelectMenuItem>, val listener: TuningSelectMenuListener)
    : NativeGui<TuningArrowsBinding>(TuningArrowsBinding::class) {

    private var curSelectedIndex = 0

    init {
        for(index in list.indices) {
            if(list[index].value == startValue) {
                curSelectedIndex = index
                break
            }
        }
        updateInfo()

        binding.leftButton.setOnClickListener {
            curSelectedIndex --

            if(curSelectedIndex < 0)
                curSelectedIndex = list.size - 1

            updateInfo()
            listener.onChangeSelectMenu(list[curSelectedIndex].value)
        }

        binding.rightButton.setOnClickListener {
            curSelectedIndex ++

            if (curSelectedIndex > list.size - 1)
                curSelectedIndex = 0

            updateInfo()
            listener.onChangeSelectMenu(list[curSelectedIndex].value)
        }

        binding.exitButton.setOnClickListener {
            listener.onChangeSelectMenu(startValue) // reset to start value

            destroy()
            listener.onExitSelectMenu()
        }

        binding.buyButton.setOnClickListener {
            destroy()
            listener.onSelectedSelectMenu()
        }
    }

    private fun updateInfo() {
        binding.priceText.text = String.format("%s руб.", Samp.formatter.format( list[curSelectedIndex].price ))
        binding.caption.text = list[curSelectedIndex].caption
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}