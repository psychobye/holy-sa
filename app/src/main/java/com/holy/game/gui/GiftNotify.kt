package com.holy.game.gui

import android.view.View
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.GiftNotifyBinding
import com.holy.data.Rare
import nl.dionsegijn.konfetti.core.Party
import nl.dionsegijn.konfetti.core.Position
import nl.dionsegijn.konfetti.core.emitter.Emitter
import java.util.concurrent.TimeUnit

class GiftNotify(namePrize: String, rare: Int = 0) : NativeGui<GiftNotifyBinding>(GiftNotifyBinding::class) {
    external fun nativeHide()

    init {
        activity.runOnUiThread {
            binding.giftNotifyNameText.text = namePrize

            binding.giftNotifyButt.setOnClickListener {
                activity.runOnUiThread {
                    destroy()
                }
            }

            if(rare != Rare.NONE) {
                binding.giftNotifyRareText.setTextColor(Rare.getColorAsStateList(rare))
                binding.giftNotifyRareText.text = String.format("Редкость: %s", Rare.getName(rare))
                binding.giftNotifyRareText.setShadowLayer(10f, 0f, 0f, Rare.getColorAsInt(rare))
                binding.giftNotifyRareText.visibility = View.VISIBLE
            } else {
                binding.giftNotifyRareText.visibility = View.GONE
            }

            val party = Party(
                speed = 0f,
                maxSpeed = 30f,
                damping = 0.9f,
                spread = 360,
                colors = listOf(0xfce18a, 0xff726d, 0xf4306d, 0xb48def),
                emitter = Emitter(duration = 100, TimeUnit.MILLISECONDS).max(100),
                position = Position.Relative(0.5, 0.3)
            )
            binding.konfettiView.start(party)

        }
    }

    override fun destroy() {
        super.destroy()
        nativeHide()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}