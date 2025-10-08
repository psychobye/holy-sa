package com.lit.game.gui.hud

import android.graphics.Color
import android.graphics.PorterDuff
import android.text.Html
import android.view.View
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.HudBinding
import com.lit.game.gui.Menu
import com.lit.game.gui.util.ConvertViewCoordsToGta
import com.lit.game.gui.util.Utils
import com.lit.launcher.domain.enums.StorageElements
import com.lit.launcher.storage.Storage
import com.rommansabbir.animationx.Attention
import com.rommansabbir.animationx.animationXAttention
import java.util.Timer
import java.util.TimerTask

class HudManager : Chat() {
    private val binding = HudBinding.bind(activity.findViewById(R.id.hudView))

    private val moneyText = AnimatedMoneyText(binding.hudMoneyTextview)

    private var salaryNotify = SalaryNotify()

    private val hud_main: ConstraintLayout
    private val target_notify: ConstraintLayout

    private val target_notify_exit: ImageView
    private val target_notify_status: ImageView
    private val target_notify_text: TextView

    private val hud_weapon: ImageView
    private val hud_ammo: TextView
    private val levelinfo: TextView
    private val hud_greenzone: ImageView
    private val hud_gpsactive: ImageView
    private val hud_serverlogo: ConstraintLayout
    private val hud_wanted: ArrayList<ImageView>
    var camera_mode_butt: ImageView
    private val lock_vehicle: ImageView
    private val hud_bg: ImageView


    // damage inf
    var take_damage_timer: Timer? = null
    var give_damage_timer: Timer? = null
    var damage_informer_give_text: TextView
    var damage_informer_give_layout: ConstraintLayout
    var damage_informer_take: TextView
    var buttonLockCD: Long = 0
    private var isHudSetPos = false

    private external fun HudInit()
    private external fun ClickLockVehicleButton()
    private external fun SetRadarBgPos(x1: Float, y1: Float, x2: Float, y2: Float)
    private external fun nativeSetRadarPos(x1: Float, y1: Float, width: Float, height: Float)
    private external fun clickCameraMode()
    private external fun clickMultText()
    private external fun sendTorpedo()
    private external fun changeWeapon()
    private external fun nativeClickMenu(): Boolean
    var damageSound = 0
    fun toggleTorpedoButt(toggle: Boolean) {
        activity.runOnUiThread {
            val butt_torpedo = activity.findViewById<ImageView>(R.id.butt_torpedo)
            if (toggle) butt_torpedo.visibility = View.VISIBLE else butt_torpedo.visibility = View.GONE
        }
    }

    fun addGiveDamageNotify(nick: String?, weapon: String?, damage: Float, bodypart: String?) {
        Samp.soundPool.play(damageSound, 0.1f, 0.1f, 1, 0, 1.2f)
        val task: TimerTask = object : TimerTask() {
            override fun run() {
                activity.runOnUiThread { damage_informer_give_layout.visibility = View.GONE }
                give_damage_timer = null
            }
        }
        if (give_damage_timer != null) give_damage_timer!!.cancel()
        give_damage_timer = Timer("Timer")
        give_damage_timer!!.schedule(task, 2000L)
        activity.runOnUiThread {
            damage_informer_give_text.text = String.format("%s (%s) - %s +%.2f", nick, bodypart, weapon, damage)
            if (damage_informer_give_layout.visibility == View.GONE) {
                damage_informer_give_layout.visibility = View.VISIBLE
                damage_informer_give_layout.animationXAttention(Attention.ATTENTION_BOUNCE, 500)
            }
        }
    }

    fun addTakeDamageNotify(nick: String?, weapon: String?, damage: Float) {
        val task: TimerTask = object : TimerTask() {
            override fun run() {
                activity.runOnUiThread { damage_informer_take.visibility = View.GONE }
                take_damage_timer = null
            }
        }
        if (take_damage_timer != null) take_damage_timer!!.cancel()
        take_damage_timer = Timer("Timer")
        take_damage_timer!!.schedule(task, 2000L)
        activity.runOnUiThread {
            damage_informer_take.text = String.format("%s - %s -%.2f", nick, weapon, damage)
            damage_informer_take.visibility = View.VISIBLE
        }
    }

    fun toggleProgressTexts(toggle: Boolean) {
        activity.runOnUiThread {
            val newState = if (toggle) View.VISIBLE else View.GONE

            binding.armorText.visibility    = newState
            binding.healthText.visibility   = newState
            binding.satietyText.visibility  = newState
        }
    }

    fun toggleLockButton(toggle: Boolean) {
        if (toggle) {
            activity.runOnUiThread { Utils.ShowLayout(lock_vehicle, true) }
        } else {
            activity.runOnUiThread { Utils.HideLayout(lock_vehicle, true) }
        }
    }

    fun updateBars(health: Int, armour: Int, hunger: Int) {
        activity.runOnUiThread {
            binding.healthProgress.progress     = health
            binding.healthText.text             = String.format("%d", health)

            binding.armorProgress.progress      = armour
            binding.armorText.text              = String.format("%d", armour)

            binding.satietyProgress.progress    = hunger
            binding.satietyText.text            = String.format("%d", hunger)
        }
    }

    fun updateAmmo(weaponid: Int, ammo: Int, ammoclip: Int) {
        activity.runOnUiThread {
            val id = activity.resources.getIdentifier(String.format("weapon_%d", weaponid), "drawable", activity.packageName)
            if (id > 0) hud_weapon.setImageResource(id)
            if ((weaponid > 15) and (weaponid < 44) and (weaponid != 21)) {
                Utils.ShowLayout(hud_ammo, false)
                val ss = String.format("%d<font color='#B0B0B0'>/%d</font>", ammoclip, ammo - ammoclip)
                hud_ammo.text = Html.fromHtml(ss)
            } else {
                hud_ammo.visibility = View.GONE
            }
        }
    }

    fun updateMoney(money: Int) {
        moneyText.updateMoney(money)
    }

    fun UpdateWanted(wantedLVL: Int) {
        activity.runOnUiThread {
            for (i in hud_wanted.indices) {
                if (i < wantedLVL) {
                    hud_wanted[i].visibility = View.VISIBLE
                } else {
                    hud_wanted[i].visibility = View.GONE
                }
            }
        }
    }

    fun toggleAll(toggle: Boolean, isWithChat: Boolean) {
        if (!toggle) {
            activity.runOnUiThread { hud_main.visibility = View.GONE }
        } else {
            activity.runOnUiThread { hud_main.visibility = View.VISIBLE }
        }
        /*if (!isWithChat) {
            activity.runOnUiThread {
                chat_box.visibility = View.GONE
                hide_chat.visibility = View.GONE
            }
        } else {
            activity.runOnUiThread {
                chat_box.visibility = View.VISIBLE
                hide_chat.visibility = View.VISIBLE
            }
        }*/
    }

    fun toggleGreenZone(toggle: Boolean) {
        if (toggle) {
            activity.runOnUiThread { Utils.ShowLayout(hud_greenzone, true) }
        } else {
            activity.runOnUiThread { Utils.HideLayout(hud_greenzone, true) }
        }
    }

    fun toggleGps(toggle: Boolean) {
        if (toggle) {
            activity.runOnUiThread { Utils.ShowLayout(hud_gpsactive, true) }
        } else {
            activity.runOnUiThread { Utils.HideLayout(hud_gpsactive, true) }
        }
    }

    fun initServerLogo() {
        activity.runOnUiThread {
            val serverColor = Storage.getProperty(StorageElements.SERVER_COLOR, activity)
            val serverName = Storage.getProperty(StorageElements.SERVER_NAME, activity)
            val serverMulti = Storage.getProperty(StorageElements.SERVER_MULTI, activity)

            val img = activity.findViewById<ImageView>(R.id.hud_logo_img)
            val text = activity.findViewById<TextView>(R.id.hud_logo_text)

            // если serverColor null или пустой — дефолт белый
            val safeColor = try {
                Color.parseColor(serverColor ?: "#FFFFFF")
            } catch (e: IllegalArgumentException) {
                Color.WHITE
            }

            img.setColorFilter(safeColor, PorterDuff.Mode.SRC_ATOP)
            text.setTextColor(safeColor)
            text.text = "$serverName "

            if (!serverMulti.isNullOrBlank()) {
                val multiText = activity.findViewById<TextView>(R.id.hud_logo_multi_text)
                multiText.visibility = View.VISIBLE
                multiText.text = "$serverMulti "
            }
        }
    }

    fun toggleServerLogo(toggle: Boolean) {
        // if (toggle) activity.runOnUiThread { Utils.ShowLayout(hud_serverlogo, false) } else activity.runOnUiThread { Utils.HideLayout(hud_serverlogo, false) }
    }

    fun updateLevelInfo(level: Int, currentexp: Int, maxexp: Int) {
        activity.runOnUiThread {
            val strlevelinfo = String.format("LVL %d [EXP %d/%d]", level, currentexp, maxexp)
            levelinfo.text = strlevelinfo
        }
    }

    init {

        camera_mode_butt = activity.findViewById(R.id.camera_mode_butt)
        camera_mode_butt.setOnClickListener { view: View? -> clickCameraMode() }
        // === damage informer
        damage_informer_give_layout = activity.findViewById(R.id.damage_informer_give_layout)
        damage_informer_give_text = activity.findViewById(R.id.damage_informer_give_text)
        damage_informer_take = activity.findViewById(R.id.damage_informer_take)
        damageSound = Samp.soundPool.load(activity.application, R.raw.hit, 0)

        initServerLogo()

        HudInit()
        hud_bg = activity.findViewById(R.id.hud_bg)

        // кнопка закрыть/открыть тачку
        lock_vehicle = activity.findViewById(R.id.vehicle_lock_butt)
        lock_vehicle.visibility = View.GONE
        lock_vehicle.setOnClickListener { view: View? ->
            val currTime = System.currentTimeMillis() / 1000
            if (buttonLockCD > currTime) {
                return@setOnClickListener
            }
            buttonLockCD = currTime + 2
            ClickLockVehicleButton()
        }

        ///
        hud_main = activity.findViewById(R.id.hud_main)
        target_notify = activity.findViewById(R.id.target_notify)
        target_notify.visibility = View.GONE
        target_notify_exit = activity.findViewById(R.id.target_notify_exit)
        target_notify_status = activity.findViewById(R.id.target_notify_status)
        target_notify_text = activity.findViewById(R.id.target_notify_text)
        hud_greenzone = activity.findViewById(R.id.hud_greenzone)
        hud_greenzone.visibility = View.GONE
        hud_gpsactive = activity.findViewById(R.id.hud_gpsactive)
        hud_gpsactive.visibility = View.GONE
        hud_serverlogo = activity.findViewById(R.id.hud_logo)
        hud_serverlogo.visibility = View.GONE
        levelinfo = activity.findViewById(R.id.levelinfo)
        hud_ammo = activity.findViewById(R.id.hud_ammo)
        hud_ammo.visibility = View.GONE
        hud_weapon = activity.findViewById(R.id.hud_weapon)
        hud_wanted = ArrayList()
        hud_wanted.add(activity.findViewById(R.id.hud_star_1))
        hud_wanted.add(activity.findViewById(R.id.hud_star_2))
        hud_wanted.add(activity.findViewById(R.id.hud_star_3))
        hud_wanted.add(activity.findViewById(R.id.hud_star_4))
        hud_wanted.add(activity.findViewById(R.id.hud_star_5))
        hud_wanted.add(activity.findViewById(R.id.hud_star_6))
        binding.healthProgress.progress     = 100
        binding.healthText.text             = String.format("%d", 100)
        binding.armorProgress.progress      = 0
        binding.armorText.text              = String.format("%d", 0)
        binding.satietyProgress.progress    = 0
        binding.satietyText.text            = String.format("%d", 0)
        for (i in hud_wanted.indices) {
            hud_wanted[i].visibility = View.GONE
        }

        binding.menuButton.setOnClickListener {
            if (!nativeClickMenu()) {
                Menu()
            }
        }

        // X2 click
        val multiText = activity.findViewById<TextView>(R.id.hud_logo_multi_text)
        multiText.setOnClickListener { view: View? -> clickMultText() }
        hud_weapon.setOnClickListener { v: View? -> changeWeapon() }

        hud_bg.post {
            if (isHudSetPos) return@post
            SetRadarBgPos(hud_bg.x, hud_bg.y, hud_bg.x + hud_bg.width, hud_bg.y + hud_bg.height)

            val xxx = Utils.screenSize
            println("screenSize = ${xxx.first}, ${xxx.second}")
            println("parentSize = ${hud_main.width}, ${hud_main.height}")

            val data = ConvertViewCoordsToGta.convertCoordsToGta(
                ConvertViewCoordsToGta.Data(
                    binding.hudBg.x,
                    binding.hudBg.y,
                    binding.hudBg.width.toFloat(),
                    binding.hudBg.height.toFloat()
                )
            )
            nativeSetRadarPos(data.x, data.y, data.width * 0.96f, data.width * 0.96f)

            activity.runOnUiThread {
                hud_main.visibility = View.GONE
                hud_bg.visibility = View.GONE
            }
            isHudSetPos = true
        }
        Utils.HideLayout(hud_gpsactive, false)
        val butt_torpedo = activity.findViewById<ImageView>(R.id.butt_torpedo)
        butt_torpedo.setOnClickListener { view: View? -> sendTorpedo() }
    }

    fun showUpdateTargetNotify(type: Int, text: String?) {
        activity.runOnUiThread {
            Utils.ShowLayout(target_notify, true)
            if (type == 0) {
                target_notify_text.text = text
                target_notify_text.setTextColor(-0x1)
                target_notify_status.setImageResource(R.drawable.target_notify_none)
            } else if (type == 1) {
                target_notify_text.text = text
                target_notify_text.setTextColor(-0xcc3bb1)
                target_notify_status.setImageResource(R.drawable.target_notify_success)
            }
            target_notify_exit.setOnClickListener { view: View? -> hideTargetNotify() }
        }
    }

    fun hideTargetNotify() {
        activity.runOnUiThread { Utils.HideLayout(target_notify, true) }
    }

    fun updateSalary(salary: Int, lvl: Int, exp: Float) {
        activity.runOnUiThread {
            salaryNotify.update(salary, lvl, exp)
        }
    }
}