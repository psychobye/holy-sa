package com.holy.game.gui

import com.holy.game.EntitySnaps
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.AucContainerBinding
import com.holy.data.skins.Skins
import com.holy.data.vehicles.Vehicles

class AucContainer : NativeGui<AucContainerBinding>(AucContainerBinding::class){

    private external fun nativeSendClick(buttonID: Int)


    init {
        activity.runOnUiThread {
            binding.giveButton.setOnClickListener {
                nativeSendClick(1)

                destroy()
            }
            binding.sellButton.setOnClickListener {
                nativeSendClick(2)

                destroy()
            }

            binding.exitButton.setOnClickListener {
                nativeSendClick(0)

                destroy()
            }
        }
    }

    private fun show(id: Int, type: Int, price: Int) {
        activity.runOnUiThread {
            binding.priceText.text = String.format("%s руб.", Samp.formatter.format(price))
            if (type == 0) {
                binding.caption.text = Vehicles.getName(id)
                EntitySnaps.loadEntitySnapToImageView(Vehicles.getSnap(id), binding.image)
            } else {
                binding.caption.text = Skins.getName(id)
                EntitySnaps.loadEntitySnapToImageView(Skins.getSnap(id), binding.image)
            }
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}