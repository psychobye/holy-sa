package com.holy.game.gui.fillingGames

import com.lit.game.core.Samp
import com.lit.game.databinding.MilkBinding
import com.lit.game.gui.NativeGui

class Milk : NativeGui<MilkBinding>(MilkBinding::class) {

    private external fun nativeOnClosed(completed: Boolean)

    init {
        Samp.activity.runOnUiThread {

            ParentClass(
                binding.progress1,
                binding.progress2,

                binding.button1,
                binding.button2,

                binding.progressText1,
                binding.progressText2,

                object : FillingGameListener {
                    override fun onEnded() {
                        destroy()

                        nativeOnClosed(true)
                    }
                }
            )

            binding.exitButton.setOnClickListener {
                destroy()
                nativeOnClosed(false)
            }
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}