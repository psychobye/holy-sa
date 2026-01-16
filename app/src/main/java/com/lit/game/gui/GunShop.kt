package com.lit.game.gui

import android.view.View
import android.view.ViewStub
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity

class GunShop {
    private val gunShopStub = activity.findViewById<ViewStub>(R.id.gunShopStub)
    private lateinit var gunShopInflated: View

    private external fun nativeSendButt(buttId: Int)

    var weaponselectid = 0

    init {
        activity.runOnUiThread {
            gunShopInflated = gunShopStub.inflate()

            val gunshop_golfclub: ConstraintLayout = activity.findViewById(R.id.gunshop_golfclub)
            val gunshop_mp5: ConstraintLayout = activity.findViewById(R.id.gunshop_mp5)
            val gunshop_m4: ConstraintLayout = activity.findViewById(R.id.gunshop_m4)
            val gunshop_deagle: ConstraintLayout = activity.findViewById(R.id.gunshop_deagle)
            val gunshop_rifle: ConstraintLayout = activity.findViewById(R.id.gunshop_rifle)
            val gunshop_smoke: ConstraintLayout = activity.findViewById(R.id.gunshop_smoke)
            val gunshop_ak47: ConstraintLayout = activity.findViewById(R.id.gunshop_ak47)
            val gunshop_bat: ConstraintLayout = activity.findViewById(R.id.gunshop_bat)
            val gunshop_shotgun: ConstraintLayout = activity.findViewById(R.id.gunshop_shotgun)
            val gunshop_armor: ConstraintLayout = activity.findViewById(R.id.gunshop_armor)
            val gunshop_katana: ConstraintLayout = activity.findViewById(R.id.gunshop_katana)
            val gunshop_knucles: ConstraintLayout = activity.findViewById(R.id.gunshop_knucles)
            val gunshop_weapontext: TextView = activity.findViewById(R.id.gunshop_weapontext)
            val gunshop_weaponimage: ImageView = activity.findViewById(R.id.gunshop_weaponimage)
            val gunshop_weaponbuy: MaterialButton = activity.findViewById(R.id.gunshop_weaponbuy)
            val gunshop_exit: ImageView = activity.findViewById(R.id.gunshop_exit)

            gunshop_exit.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                hide()
            }
            gunshop_weaponbuy.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                nativeSendButt(weaponselectid)
            }
            gunshop_golfclub.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 1
                gunshop_weapontext.text = "ГОЛЬФ КЛЮШКА"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_2)
            }
            gunshop_mp5.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 2
                gunshop_weapontext.text = "MP5"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_29)
            }
            gunshop_m4.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 3
                gunshop_weapontext.text = "M4"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_31)
            }
            gunshop_deagle.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 4
                gunshop_weapontext.text = "DESERT EAGLE"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_24)
            }
            gunshop_rifle.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 5
                gunshop_weapontext.text = "RIFLE"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_33)
            }
            gunshop_smoke.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 6
                gunshop_weapontext.text = "ДЫМОВАЯ ШАШКА"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_17)
            }
            gunshop_ak47.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 7
                gunshop_weapontext.text = "AK-47"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_30)
            }
            gunshop_bat.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 8
                gunshop_weapontext.text = "БИТА"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_5)
            }
            gunshop_shotgun.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 9
                gunshop_weapontext.text = "SHOTGUN"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_25)
            }
            gunshop_armor.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 10
                gunshop_weapontext.text = "БРОНЕЖИЛЕТ"
                gunshop_weaponimage.setImageResource(R.drawable.gunshop_armor)
            }
            gunshop_katana.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 11
                gunshop_weapontext.text = "КАТАНА"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_8)
            }
            gunshop_knucles.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                weaponselectid = 12
                gunshop_weapontext.text = "КАСТЕТ"
                gunshop_weaponimage.setImageResource(R.drawable.weapon_1)
            }

            weaponselectid = 12
            gunshop_weapontext.text = "КАСТЕТ"
            gunshop_weaponimage.setImageResource(R.drawable.weapon_1)

        }
    }

    fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(gunShopInflated, R.id.gunShopStub, R.layout.gunshop)

            nativeSendButt(0)
        }
    }
}