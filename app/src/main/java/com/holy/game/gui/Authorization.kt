package com.holy.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.text.Editable
import android.text.TextWatcher
import android.view.View
import android.widget.CompoundButton
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.AuthorizationBinding
import com.holy.game.gui.util.Utils

class Authorization : NativeGui<AuthorizationBinding>(AuthorizationBinding::class) {
    private external fun nativeToggleAutoLogin(toggle: Boolean)
    private external fun nativeClickRecoveryPass()
    private external fun nativeClickLogin(password: String?)

    init {
        activity.runOnUiThread {
            binding.autoLoginInfo.setOnClickListener {
                binding.autologinMainHelp.visibility = View.VISIBLE
            }
            binding.autologinHelpClose.setOnClickListener {
                binding.autologinMainHelp.visibility = View.GONE
            }
            binding.switcherAutologin.setOnCheckedChangeListener { _: CompoundButton?, b: Boolean ->
                nativeToggleAutoLogin(b)
            }
            binding.recoveryPassword.setOnClickListener {
                nativeClickRecoveryPass()
            }
            binding.authPlay.setOnClickListener {
                // view.startAnimation(Samp.clickAnim);
                if (binding.authPassword.text?.length!! > 5) {
                    nativeClickLogin(binding.authPassword.text.toString())
                }
            }
            binding.authBack.setOnClickListener {
                Utils.HideLayout(binding.authRight2, false)
                Utils.ShowLayout(binding.authRight1, false)
                binding.authInfoTitle.text = "Выберите способ авторизации!"
                binding.authInfo.text = "Информация: если этот аккаунт является не вашим, то выйдите из игры и смените игровое имя на новое!"
            }
            binding.authButt.setOnClickListener {
                Utils.HideLayout(binding.authRight1, false)
                Utils.ShowLayout(binding.authRight2, false)
                binding.authInfoTitle.text = "Введите пароль, чтобы войти в игру."
            }
            binding.authPassword.addTextChangedListener(object : TextWatcher {
                override fun afterTextChanged(s: Editable) {}
                override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
                override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {
                    if (binding.authPassword.text?.length!! > 5) {
                        binding.authPlay.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                    }
                    else {
                        binding.authPlay.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#004261"))
                    }
                }
            })

        }
    }

    fun update(nick: String, id: Int, toggle_autologin: Boolean, email_acvive: Boolean) {
        activity.runOnUiThread {
            if (email_acvive) {
                binding.recoveryPassword.visibility = View.VISIBLE
            }
            else {
                binding.recoveryPassword.visibility = View.GONE
            }
            binding.switcherAutologin.isChecked = toggle_autologin

            binding.authNick.text = String.format("%s [%d]", nick, id)
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}