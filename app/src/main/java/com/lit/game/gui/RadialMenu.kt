package com.lit.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.view.View
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.CircularCarMenuBinding
import com.rommansabbir.animationx.Fade
import com.rommansabbir.animationx.animationXFade

class RadialMenu : NativeGui<CircularCarMenuBinding>(CircularCarMenuBinding::class){
    private external fun nativeOnClose()

    init {
        activity.runOnUiThread {
            binding.buttonMusic.setOnClickListener{
                Samp.sendCommand("/music")
                destroy()
            }
            binding.radialStrob.setOnClickListener {
                Samp.sendCommand("/strobes")
            }
            binding.radialNeon.setOnClickListener {
                Samp.sendCommand("/neon")
            }
            binding.radialLock.setOnClickListener {
                Samp.sendCommand("/lock")
            }
            binding.radialEngine.setOnClickListener {
                Samp.sendCommand("/e")
            }
            binding.radialLight.setOnClickListener {
                Samp.sendCommand("/light")
            }
            binding.radialNightLights.setOnClickListener {
                Samp.sendCommand("/far")
            }
            binding.radialBonnet.setOnClickListener {
                Samp.sendCommand("/bonnet")
            }
            binding.radialBoot.setOnClickListener {
                Samp.sendCommand("/boot")
            }
            binding.radialCloseButt.setOnClickListener {
                it.startAnimation(Samp.clickAnim)
                destroy()
            }

            toggleLightCategory(false)

            binding.radialLightsCategoty.setOnClickListener {
                if(binding.radialLightsCategotyBg.visibility == View.VISIBLE)
                    toggleLightCategory(false)
                else
                    toggleLightCategory(true)
            }

            binding.radialMainLayout.animationXFade(Fade.FADE_IN, 100)
        }
    }

    override fun destroy() {
        super.destroy()
        nativeOnClose()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    fun update(lock: Boolean, engine: Boolean, light: Boolean, nightLight: Boolean, strob: Boolean, neon: Boolean, bonnet: Boolean, boot: Boolean) {
        activity.runOnUiThread {
            binding.radialEngine.imageTintList = if (engine) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialLock.imageTintList = if (lock) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialLight.imageTintList = if (light)      ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialNightLights.imageTintList = if (nightLight) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialNeon.imageTintList = if (neon) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialStrob.imageTintList = if (strob) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialBonnet.imageTintList = if (bonnet) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
            binding.radialBoot.imageTintList = if (boot) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null
        }
    }

    private fun toggleLightCategory(toggle: Boolean) {
        activity.runOnUiThread {
            binding.radialLightsCategoty.imageTintList = if (toggle) ColorStateList.valueOf(Color.parseColor("#FF8BC34A")) else null

            binding.radialLightsCategotyBg.visibility = if(toggle) View.VISIBLE else View.GONE
            binding.radialNeon.visibility = if(toggle) View.VISIBLE else View.GONE
            binding.radialNightLights.visibility = if(toggle) View.VISIBLE else View.GONE
            binding.radialLight.visibility = if(toggle) View.VISIBLE else View.GONE
            binding.radialStrob.visibility = if(toggle) View.VISIBLE else View.GONE
        }
    }
}