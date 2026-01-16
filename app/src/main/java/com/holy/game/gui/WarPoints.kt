package com.holy.game.gui

import com.holy.game.core.Samp
import com.holy.game.databinding.WarPointsBinding

class WarPoints : NativeGui<WarPointsBinding>(WarPointsBinding::class) {


    fun update(time: Int, myPoints: Int, enemyScore: Int) {
        Samp.activity.runOnUiThread {
            binding.myPointsText.text       = String.format("Ваши очки: %d", myPoints)
            binding.enemyPointsText.text    = String.format("Вражеские очки: %d", enemyScore)

            binding.timeText.text = String.format("%d секунд", time)
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}