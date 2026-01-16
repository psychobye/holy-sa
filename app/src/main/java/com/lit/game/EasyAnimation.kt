package com.lit.game

import android.view.View
import android.view.animation.Animation
import com.lit.game.core.Samp

object EasyAnimation {

    fun View.setOnClickListenerWithAnim(
        withEndAction: (() -> Unit)? = null
    ) {
        this@setOnClickListenerWithAnim.setOnClickListener {
            val animation = Samp.clickAnim
            animation.setAnimationListener(object : Animation.AnimationListener {
                override fun onAnimationStart(animation: Animation?) {}
                override fun onAnimationEnd(animation: Animation?) {
                    if (withEndAction != null) {
                        withEndAction()
                    }

                    animation?.setAnimationListener(null)
                }

                override fun onAnimationRepeat(animation: Animation?) {}
            })

            it.startAnimation(animation)
        }
    }
}