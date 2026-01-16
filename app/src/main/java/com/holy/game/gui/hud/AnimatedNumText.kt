package com.holy.game.gui.hud

import android.widget.TextView
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import kotlin.math.abs
import kotlin.math.ceil

class AnimatedMoneyText(
    private val textView: TextView
) {
    private var currentRealValue = 0
    private var currentVisualValue = 0
    private var thread: Thread? = null
    
    fun updateMoney(money: Int) {
        currentRealValue = money
       
        if (thread?.isAlive == true) {
            thread?.interrupt()
        }
        try {
            thread = Thread(runnable)
            thread?.start()
        } catch (e: Exception) {
            updateText(currentRealValue)
        }
    }

    private var runnable = Runnable() 
    {
        while (currentVisualValue < currentRealValue && !Thread.currentThread().isInterrupted) {
            currentVisualValue += ceil(abs(currentRealValue - currentVisualValue) * 0.005).toInt()
            if (currentVisualValue > currentRealValue) {
                currentVisualValue = currentRealValue
            }
            updateText(currentVisualValue)
            
            try {
                Thread.sleep(10)
            } catch (e: InterruptedException) {
                updateText(currentRealValue)
                break
            }
        }
        while (currentVisualValue > currentRealValue && !Thread.currentThread().isInterrupted) {
            currentVisualValue -= ceil(abs(currentRealValue - currentVisualValue) * 0.005).toInt()
            if (currentVisualValue < currentRealValue) {
                currentVisualValue = currentRealValue
            }
            updateText(currentVisualValue)
            
            try {
                Thread.sleep(10)
            } catch (e: InterruptedException) {
                updateText(currentRealValue)
                break
            }
        }
    }

    private fun updateText(toValue: Int) {
        activity.runOnUiThread {
            textView.text = String.format("%s", Samp.formatter.format(toValue))
        }
    }
}