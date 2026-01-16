package com.holy.game.gui

import android.annotation.SuppressLint
import android.content.res.ColorStateList
import android.graphics.Color
import android.text.Html
import android.view.View
import android.view.ViewStub
import android.widget.Button
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import java.util.Random

class PreDeath {
    private lateinit var toHospitalButton: Button
    private lateinit var waitHelpButton: Button
    private lateinit var pre_death_killer_text: TextView
    private lateinit var pre_death_deathbuttons_layout: ConstraintLayout
    private lateinit var pde_death_game_buttons: ConstraintLayout
    private lateinit var pre_death_adr_button: ConstraintLayout
    private lateinit var pre_death_df_button: ConstraintLayout
    private lateinit var pre_death_pulse_count: TextView
    private lateinit var pre_death_health_count: TextView
    private lateinit var pre_death_caption_textview: TextView

    private var timeRemaining = 0
    private var pulse = 0
    private var health = 0

    private val preDeathStub = activity.findViewById<ViewStub>(R.id.preDeathStub)
    private lateinit var preDeathInflated: View

    private external fun medicPreDeathExit(buttonID: Int)
    private external fun medicMiniGameExit(typeId: Int)

    init {
        activity.runOnUiThread {
            preDeathInflated = preDeathStub.inflate()

            pre_death_caption_textview = activity.findViewById(R.id.pre_death_caption_textview)
            pre_death_adr_button = activity.findViewById(R.id.pre_death_adr_button)
            pde_death_game_buttons = activity.findViewById(R.id.pde_death_game_buttons)
            pre_death_deathbuttons_layout = activity.findViewById(R.id.pre_death_deathbuttons_layout)

            pre_death_killer_text = activity.findViewById(R.id.pre_death_killer_text)
            waitHelpButton = activity.findViewById(R.id.preDeath_wait_help_butt)
            toHospitalButton = activity.findViewById(R.id.preDeath_to_hospital_button)
            pre_death_pulse_count = activity.findViewById(R.id.pre_death_pulse_count)
            pre_death_health_count = activity.findViewById(R.id.pre_death_health_count)

            waitHelpButton.setOnClickListener {
                medicPreDeathExit(0)
            }
            toHospitalButton.setOnClickListener {
                if (timeRemaining > 0) return@setOnClickListener
                medicPreDeathExit(1)
            }
            pre_death_adr_button.setOnClickListener {
                val random = Random()
                pulse += 5 + random.nextInt(5)
                if (pulse > 80) pulse = 70 + random.nextInt(9)
                if (health >= 100 && pulse >= 70) {
                    medicMiniGameExit(1)
                    return@setOnClickListener
                }
                updatePulseAndHealth()
            }
            pre_death_df_button = activity.findViewById(R.id.pre_death_df_button)
            pre_death_df_button.setOnClickListener { view: View? ->
                val random = Random()
                health += 10 + random.nextInt(5)
                if (health > 100) health = 100
                if (health >= 100 && pulse >= 70) {
                    medicMiniGameExit(1)
                    return@setOnClickListener
                }
                updatePulseAndHealth()
            }

        }
    }

    fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(preDeathInflated, R.id.preDeathStub, R.layout.pre_death)
        }
    }

    fun setButtonActive() {
        activity.runOnUiThread { toHospitalButton.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#ffffff")) }
    }

    fun setButtonText(str: String?) {
        activity.runOnUiThread { toHospitalButton.text = str }
    }

    fun showPreDeath(killerName: String?, killerID: Int) { // death
        activity.runOnUiThread {
            if(killerID == Samp.INVALID_PLAYER_ID)
                pre_death_caption_textview.text = Html.fromHtml("Вы были <font color='#fbc02d'>ранены</font>")
            else
                pre_death_caption_textview.text = Html.fromHtml("Вы были <font color='#fbc02d'>ранены</font> игроком")

            timeRemaining = 15
            pde_death_game_buttons.visibility = View.GONE
            pre_death_deathbuttons_layout.visibility = View.VISIBLE
            pre_death_killer_text.text = String.format("%s (%d)", killerName, killerID)

            val thread = Thread(runTimeRemaining)
            thread.start()
        }
    }

    fun updatePulseAndHealth() {
        activity.runOnUiThread {
            pre_death_pulse_count.text = String.format("%d", pulse)
            pre_death_health_count.text = String.format("%d", health)
        }
    }

    fun showMiniGame(playerName: String?) {
        health = 0
        pulse = health
        activity.runOnUiThread {
            pre_death_caption_textview.text = Html.fromHtml("<font color='#fbc02d'>Пострадавший</font>")
            pde_death_game_buttons.visibility = View.VISIBLE
            pre_death_deathbuttons_layout.visibility = View.GONE
            pre_death_killer_text.text = playerName
            updatePulseAndHealth()
        }
    }

    @SuppressLint("SetTextI18n")
    private val runTimeRemaining = Runnable {
        for (i in 0..19) {
            if (Thread.currentThread().isInterrupted) {
                timeRemaining = 0
                setButtonText("В БОЛЬНИЦУ")
                break
            }
            timeRemaining--
            if (timeRemaining == 0) {
                //toHospitalButton.setBackgroundTintList(ContextCompat.getColor(this));
                setButtonText("В БОЛЬНИЦУ")
                break
            }
            else {
                setButtonText("Доступно через $timeRemaining")
            }
            try {
                Thread.sleep(1000)
            } catch (e: InterruptedException) {
                timeRemaining = 0
                setButtonText("В БОЛЬНИЦУ")
                break
            }
        }
    }
}