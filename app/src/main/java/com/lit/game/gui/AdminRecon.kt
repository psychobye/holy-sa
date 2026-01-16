package com.lit.game.gui

import android.view.View
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.AdminReconBinding

class AdminRecon : NativeGui<AdminReconBinding>(AdminReconBinding::class) {

    private external fun clickButton(buttonID: Int)

    init {
        activity.runOnUiThread {
            binding.flipButton.setOnClickListener { clickButton(FLIP_BUTTON) }
            binding.reExitButt.setOnClickListener { clickButton(EXIT_BUTTON) }
            binding.statsButton.setOnClickListener { clickButton(STATS_BUTTON) }
            binding.freezeButton.setOnClickListener { clickButton(FREEZE_BUTTON) }
            binding.unfreezeButton.setOnClickListener { clickButton(UNFREEZE_BUTTON) }
            binding.spawnButton.setOnClickListener { clickButton(SPAWN_BUTTON) }
            binding.slapButton.setOnClickListener { clickButton(SLAP_BUTTON) }
            binding.refreshButton.setOnClickListener { clickButton(REFRESH_BUTTON) }
            binding.muteButton.setOnClickListener { clickButton(MUTE_BUTTON) }
            binding.jailButton.setOnClickListener { clickButton(JAIL_BUTTON) }
            binding.kickButton.setOnClickListener { clickButton(KICK_BUTTON) }
            binding.reBanButt.setOnClickListener { clickButton(BAN_BUTTON) }
            binding.warnButton.setOnClickListener { clickButton(WARN_BUTTON) }
            binding.prevButton.setOnClickListener { clickButton(PREV_BUTTON) }
            binding.nextButton.setOnClickListener { clickButton(NEXT_BUTTON) }
        }
    }

    fun update(nick: String?, id: Int) {
        activity.runOnUiThread {
            binding.nicknameText.text = nick
            binding.idText.text = String.format("%d", id)
            binding.mainLayout.visibility = View.VISIBLE
        }
    }

    fun tempToggle(toggle: Boolean) {
        activity.runOnUiThread {
            if (toggle) {
                binding.mainLayout.visibility = View.VISIBLE
            } else {
                binding.mainLayout.visibility = View.GONE
            }
        }
    }

    companion object {
        private const val EXIT_BUTTON = 0
        private const val STATS_BUTTON = 1
        private const val FREEZE_BUTTON = 2
        private const val UNFREEZE_BUTTON = 3
        private const val SPAWN_BUTTON = 4
        private const val SLAP_BUTTON = 5
        private const val REFRESH_BUTTON = 6
        private const val MUTE_BUTTON = 7
        private const val JAIL_BUTTON = 8
        private const val KICK_BUTTON = 9
        private const val BAN_BUTTON = 10
        private const val WARN_BUTTON = 11
        private const val NEXT_BUTTON = 12
        private const val PREV_BUTTON = 13
        private const val FLIP_BUTTON = 14
    }

    override fun receivePacket(actionId: Int, data: String) {
        println("admin recon receivePacket")
    }
}
