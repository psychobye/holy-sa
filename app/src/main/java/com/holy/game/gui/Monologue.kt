package com.holy.game.gui

import android.view.View
import com.lit.game.EntitySnaps
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.core.TypewriterAnimation
import com.lit.game.core.TypewriterAnimationListener
import com.lit.game.databinding.MonologBinding
import com.lit.data.skins.Skins

class Monologue : NativeGui<MonologBinding>(MonologBinding::class) {
    private lateinit var typewriterAnimation: TypewriterAnimation

    private external fun nativeSendClick()

    init {
        activity.runOnUiThread {
            typewriterAnimation = TypewriterAnimation(binding.text)

            typewriterAnimation.setOnEndListener(object : TypewriterAnimationListener {
                override fun onEnd() {
                    binding.button.visibility = View.VISIBLE
                }
            })

            binding.button.setOnClickListener {
                nativeSendClick()
            }

            binding.textBg.setOnClickListener {
                typewriterAnimation.stopAnimation()
            }

            binding.personImage.translationX = 3000f
            binding.personImage.animate().translationX(0f)
                .setDuration(400)
                .withEndAction {
                    binding.textBg.alpha = 0f
                    binding.textBg.visibility = View.VISIBLE
                    binding.textBg.animate()
                        .alpha(1f)
                        .setDuration(400)
                        .start()
                }
                .start()

        }
    }

    private fun show(skinId: Int, text: String, author: String, button: String) {
        activity.runOnUiThread {
            EntitySnaps.loadEntitySnapToImageView(Skins.getSnap(skinId), binding.personImage)

            typewriterAnimation.animateText(text)

            binding.button.text = button
            binding.author.text = author
            binding.button.visibility = View.INVISIBLE

        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}