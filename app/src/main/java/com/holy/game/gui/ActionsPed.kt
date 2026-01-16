package com.holy.game.gui

import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.EntityActionBinding

class ActionsPed : NativeGui<EntityActionBinding> (EntityActionBinding::class){

    private var selectId = 0

    private external fun nativeDelCppObject()

    init {
        activity.runOnUiThread {
            binding.playerMute.setOnClickListener {
                Samp.sendCommand(String.format("/vmute %d", selectId))
                destroy()
            }

            binding.playerPass.setOnClickListener {
                Samp.sendCommand(String.format("/showpass %d", selectId))
                destroy()
            }

            binding.playerHandShake.setOnClickListener {
                Samp.sendCommand(String.format("/hi %d", selectId))
                destroy()
            }

            binding.playerLic.setOnClickListener {
                Samp.sendCommand(String.format("/lic %d", selectId))
                destroy()
            }

            binding.playerVoen.setOnClickListener {
                Samp.sendCommand(String.format("/showvb %d", selectId))
                destroy()
            }

            binding.playerMed.setOnClickListener {
                Samp.sendCommand(String.format("/med %d", selectId))
                destroy()
            }

            binding.playerPts.setOnClickListener {
                Samp.sendCommand(String.format("/pts %d", selectId))
                destroy()
            }

            binding.playerExit.setOnClickListener {
                destroy()
            }
        }
    }

    fun show(name: String?, selectId: Int) {
        activity.runOnUiThread {
            this.selectId = selectId

            binding.playerNick.text = String.format("%s (%d)", name, selectId)
        }
    }

    override fun destroy() {
        super.destroy()
        nativeDelCppObject()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}