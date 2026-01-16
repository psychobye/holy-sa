package com.holy.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.text.Editable
import android.text.TextWatcher
import android.view.View
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.RegistrationBinding
import com.lit.game.gui.util.Utils

class Registration : NativeGui<RegistrationBinding>(RegistrationBinding::class) {
    private var reg_right1: ConstraintLayout? = null
    private var reg_right2: ConstraintLayout? = null
    private var reg_back: ConstraintLayout? = null
    private var registration_layout: ConstraintLayout? = null
    private var sex_layout: ConstraintLayout? = null
    private var skin_layout: ConstraintLayout? = null
    private var choose_skin_notf: ImageView? = null
    private var choose_skin_left: ImageView? = null
    private var choose_skin_right: ImageView? = null
    private var choose_skin_btn: MaterialButton? = null
    private var choose_skin_notf_close: ImageView? = null
    private var choosesex_male: ImageView? = null
    private var choosesex_female: ImageView? = null
    private var choosesex_btn: MaterialButton? = null
    private var reg_play: MaterialButton? = null
    private var reg_btn: ImageView? = null
    private var reg_skin: ImageView? = null
    private var reg_password: EditText? = null
    private var reg_passwordtwo: EditText? = null
    private var reg_mail: EditText? = null
    private var reg_infotitle: TextView? = null
    private var reg_info: TextView? = null
    private var reg_nick: TextView? = null
    private var MailString = ""
    private var choosesex = 0

    external fun nativeChooseSkinClick(choosesex: Int)
    external fun nativeSkinBackClick()
    external fun nativeSkinNextClick()
    external fun nativeClick(password: String?, mail: String?, sex: Int)

    init {
        activity.runOnUiThread {

            registration_layout = activity.findViewById(R.id.registration_layout)
            skin_layout = activity.findViewById(R.id.skin_layout)
            skin_layout?.visibility = View.GONE
            choose_skin_notf = activity.findViewById(R.id.choose_skin_notf)
            choose_skin_left = activity.findViewById(R.id.choose_skin_left)
            choose_skin_right = activity.findViewById(R.id.choose_skin_right)
            choose_skin_btn = activity.findViewById(R.id.choose_skin_btn)
            choose_skin_notf_close = activity.findViewById(R.id.choose_skin_notf_close)
            sex_layout = activity.findViewById(R.id.sex_layout)
            sex_layout?.visibility = View.GONE
            choosesex_male = activity.findViewById(R.id.choosesex_male)
            choosesex_female = activity.findViewById(R.id.choosesex_female)
            choosesex_btn = activity.findViewById(R.id.choosesex_btn)
            reg_right1 = activity.findViewById(R.id.reg_right1)
            reg_right2 = activity.findViewById(R.id.reg_right2)
            reg_back = activity.findViewById(R.id.reg_back)
            reg_play = activity.findViewById(R.id.reg_play)
            reg_btn = activity.findViewById(R.id.reg_btn)
            reg_skin = activity.findViewById(R.id.reg_skin)
            reg_password = activity.findViewById(R.id.reg_password)
            reg_passwordtwo = activity.findViewById(R.id.reg_passwordtwo)
            reg_mail = activity.findViewById(R.id.reg_mail)
            reg_infotitle = activity.findViewById(R.id.reg_infotitle)
            reg_info = activity.findViewById(R.id.reg_info)
            reg_nick = activity.findViewById(R.id.reg_nick)
            choose_skin_left?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                nativeSkinBackClick()
            }
            choose_skin_right?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                nativeSkinNextClick()
            }
            choose_skin_notf_close?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                Utils.HideLayout(choose_skin_notf_close, true)
                Utils.HideLayout(choose_skin_notf, true)
            }
            choose_skin_btn?.setOnClickListener {
                nativeClick(reg_password?.text.toString(), MailString, choosesex)
            }

            choosesex_btn?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                if (choosesex > 0) {
                    nativeChooseSkinClick(choosesex)
                    Utils.ShowLayout(skin_layout, true)
                    Utils.HideLayout(sex_layout, true)
                }
            }
            choosesex_male?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                choosesex = 1
                choosesex_male?.setBackgroundResource(R.drawable.choosesex_male_active)
                choosesex_female?.setBackgroundResource(R.drawable.choosesex_female)
            }

            choosesex_female?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                choosesex = 2
                choosesex_male?.setBackgroundResource(R.drawable.choosesex_male)
                choosesex_female?.setBackgroundResource(R.drawable.choosesex_female_active)
            }
            reg_play?.setOnClickListener { view: View? ->
                //view.startAnimation(Samp.clickAnim);
                if ((reg_password?.text?.length ?: 0) > 5 && reg_password?.text.toString() == reg_passwordtwo?.text.toString()) {
                    Utils.ShowLayout(sex_layout, true)
                    Utils.HideLayout(registration_layout, true)
                }
            }
            reg_back?.setOnClickListener {
                Utils.HideLayout(reg_right2, false)
                Utils.ShowLayout(reg_right1, false)
                reg_skin?.setBackgroundResource(R.drawable.reg_skin)
                reg_infotitle?.text = "Выберите способ авторизации!"
                reg_info?.visibility = View.GONE
            }
            reg_btn?.setOnClickListener {
                Utils.HideLayout(reg_right1, false)
                Utils.ShowLayout(reg_right2, false)
                reg_skin?.setBackgroundResource(R.drawable.reg_skin1)
                reg_infotitle?.text = "Для начала игры заполните все поля"
                reg_info?.visibility = View.VISIBLE
            }
            reg_password?.addTextChangedListener(object : TextWatcher {
                override fun afterTextChanged(s: Editable) {}
                override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
                override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {
                    if ((reg_password?.getText()?.length ?: 0) > 5 && reg_password?.text.toString() == reg_passwordtwo?.text.toString()) {
                        reg_play?.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                    } else {
                        reg_play?.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#004261"))
                    }
                }
            })
            reg_passwordtwo?.addTextChangedListener(object : TextWatcher {
                override fun afterTextChanged(s: Editable) {}
                override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
                override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {
                    if ((reg_password?.text?.length ?: 0) > 5 && reg_password?.text.toString() == reg_passwordtwo?.text.toString()) {
                        reg_play?.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                    } else {
                        reg_play?.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#004261"))
                    }
                }
            })
            reg_mail?.addTextChangedListener(object : TextWatcher {
                override fun afterTextChanged(s: Editable) {}
                override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
                override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {
                    MailString = if ((reg_mail?.text?.length ?: 0) > 5) {
                        reg_mail?.text.toString()
                    } else {
                        ""
                    }
                }
            })
        }
    }

    fun show(nick: String?, id: Int) {
        activity.runOnUiThread {
            choosesex = 0
            val strnickid = String.format("%s [%d]", nick, id)
            reg_nick!!.text = strnickid
            Utils.ShowLayout(registration_layout, false)
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}