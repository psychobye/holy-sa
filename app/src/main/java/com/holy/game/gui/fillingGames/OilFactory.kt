package com.holy.game.gui.fillingGames

import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.OilfactoryBinding
import com.holy.game.gui.NativeGui

class OilFactory
    : NativeGui<OilfactoryBinding>(OilfactoryBinding::class)
{

    private external fun nativeSendExit(ok: Boolean)

    init {
        activity.runOnUiThread {

            ParentClass(
                binding.waterProgress,
                binding.oilProgress,

                binding.waterButton,
                binding.oilButton,

                binding.waterProgressText,
                binding.oilProgressText,

                object : FillingGameListener {
                    override fun onEnded() {
                        destroy()

                        nativeSendExit(true)
                    }
                }
            )

            binding.exitButton.setOnClickListener {
                destroy()
                nativeSendExit(false)
            }
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

}