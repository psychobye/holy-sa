package com.holy.game.gui

import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.BusStopBinding

class BusStation : com.holy.game.gui.NativeGui<BusStopBinding>(BusStopBinding::class) {

    fun update(time: Int) {
        activity.runOnUiThread {
            binding.timeText.text = String.format("%d", time)
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}