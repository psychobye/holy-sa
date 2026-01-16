package com.holy.game.gui

import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.MiningStoreBinding
import com.holy.game.gui.util.Utils

class MiningStore : NativeGui<MiningStoreBinding>(MiningStoreBinding::class) {

    enum class eButtonId {
        RIGHT,
        LEFT,
        BUY,
        EXIT
    }

    private external fun nativeSendClick(buttonId: Int)
    init {
        activity.runOnUiThread {
            binding.rightButton.setOnClickListener {
                nativeSendClick(eButtonId.RIGHT.ordinal)
            }

            binding.leftButton.setOnClickListener {
                nativeSendClick(eButtonId.LEFT.ordinal)
            }

            binding.buyButton.setOnClickListener {
                nativeSendClick(eButtonId.BUY.ordinal)
            }

            binding.exitButton.setOnClickListener {
                nativeSendClick(eButtonId.EXIT.ordinal)
            }
        }
    }

    private fun show(price: String, caption: String, specs: String, description: String) {
        activity.runOnUiThread {
            binding.priceText.text = price
            binding.caption.text = caption
            binding.specsText.text = Utils.transfromColors(specs)
            binding.descriptionText.text = Utils.transfromColors(description)
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

}