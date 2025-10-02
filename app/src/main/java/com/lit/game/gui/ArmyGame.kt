package com.lit.game.gui

import android.view.View
import android.view.ViewStub
import android.widget.ImageView
import android.widget.ProgressBar
import android.widget.TextView
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity

class ArmyGame {
    private lateinit var army_water_progress: ProgressBar
    private lateinit var army_soap_progress: ProgressBar
    private lateinit var army_mop_btn: ImageView
    private lateinit var army_bucket_btn: ImageView
    private lateinit var army_water_procent: TextView
    private lateinit var army_soap_procent: TextView

    private val armyGameStub = activity.findViewById<ViewStub>(R.id.armyGameStub)
    private lateinit var armyGameInflated: View

    var armystate = 0f
    var armywaterstate = 0f
    var armysoapstate = 0f

    private external fun native_onClose()

    init {
        activity.runOnUiThread {
            armyGameInflated = armyGameStub.inflate()

            army_water_progress = activity.findViewById(R.id.army_water_progress)
            army_soap_progress = activity.findViewById(R.id.army_soap_progress)
            army_mop_btn = activity.findViewById(R.id.army_mop_btn)
            army_bucket_btn = activity.findViewById(R.id.army_bucket_btn)
            army_water_procent = activity.findViewById(R.id.army_water_procent)
            army_soap_procent = activity.findViewById(R.id.army_soap_procent)

            army_mop_btn.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                armywaterstate += 100f
                army_water_progress.progress = armywaterstate.toInt()
                val stroilwaterprocent = String.format("%d%%", armywaterstate.toInt() / 10)
                army_water_procent.text = stroilwaterprocent
            }
            army_bucket_btn.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                armysoapstate += 100f
                army_soap_progress.progress = armysoapstate.toInt()
                val stroiloilprocent = String.format("%d%%", armysoapstate.toInt() / 10)
                army_soap_procent.text = stroiloilprocent
            }

            armystate = 0f
            armywaterstate = 0f
            armysoapstate = 0f

            val thread = Thread(runnableoilgame)
            thread.start()
        }
    }

    var runnableoilgame = Runnable {
        while (armystate < 2000) {
            armywaterstate -= 0.0001.toFloat()
            armysoapstate -= 0.0001.toFloat()
            if (armywaterstate < 0) {
                armywaterstate = 0f
            }
            if (armywaterstate > 1000) {
                armywaterstate = 1000f
            }
            if (armysoapstate < 0) {
                armysoapstate = 0f
            }
            if (armysoapstate > 1000) {
                armysoapstate = 1000f
            }
            army_water_progress.progress = armywaterstate.toInt()
            army_soap_progress.progress = armysoapstate.toInt()
            armystate = armywaterstate + armysoapstate
        }
        hide()
    }

    fun hide() {
        native_onClose()

        Samp.deInflatedViewStud(armyGameInflated, R.id.armyGameStub, R.layout.armygame)
    }
}