package com.lit.game.gui

import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.RaceBinding

class Race : NativeGui<RaceBinding>(RaceBinding::class) {

    private fun update(curPos: Int, maxPos: Int, curChecks: Int, maxChecks: Int, time: Long) {
        activity.runOnUiThread {
            binding.checkValue.text = String.format("%d/%d", curChecks, maxChecks)
            binding.posValue.text   = String.format("%d/%d", curPos, maxPos)

            val minutes = time / 60
            val remainingSeconds = time % 60
            binding.timeValue.text = String.format("%02d:%02d", minutes, remainingSeconds)
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}