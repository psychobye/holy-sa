package com.holy.game.core

import android.os.Handler
import android.os.Looper
import android.widget.TextView
import com.holy.game.gui.util.Utils
import kotlin.random.Random

interface TypewriterAnimationListener {
    fun onEnd()
}

class TypewriterAnimation(private val textView: TextView) {
    private var text: CharSequence = ""
    private var index: Int = 0
    private val handler = Handler(Looper.getMainLooper())
    private var listener: TypewriterAnimationListener? = null

    private val characterAdder = object : Runnable {
        override fun run() {
            textView.text = text.subSequence(0, index++)
            if (index <= text.length) {
                handler.postDelayed(this, (35 + Random.nextInt(40)).toLong())
            } else {
                listener?.onEnd()
            }
        }
    }

    fun animateText(text: CharSequence) {
        textView.text = ""
        this.text = Utils.transfromColors(text.toString())
        handler.removeCallbacks(characterAdder)
        index = 0
        handler.postDelayed(characterAdder, 500)
    }

    fun stopAnimation() {
        handler.removeCallbacks(characterAdder)
        textView.text = text

        listener?.onEnd()
    }

    fun setOnEndListener(listener: TypewriterAnimationListener) {
        this.listener = listener
    }
}