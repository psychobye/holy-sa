package com.lit.game.gui

import android.content.res.ColorStateList
import android.graphics.Color
import android.text.Editable
import android.text.Html
import android.text.TextWatcher
import android.view.View
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.BuyPlateBinding
import com.lit.data.ItemHelper
import com.rommansabbir.animationx.Attention.ATTENTION_BOUNCE
import com.rommansabbir.animationx.animationXAttention

class BuyPlate : NativeGui<BuyPlateBinding>(BuyPlateBinding::class) {

    enum class PlateType{
        NONE,
        RU,
        UA,
        BY,
        KZ
    }

    enum class ClickType {
        SELECT_RU,
        SELECT_UA,
        SELECT_BY,
        SELECT_KZ,

        CLICK_RANDOM,
        CLICK_CHECK,
        CLICK_BUY
    }

    private external fun nativeSendClick(clickId: Int, text: ByteArray = ByteArray(0))
    private external fun nativeOnExit()

    var regex = Regex("")

    init {
        activity.runOnUiThread {
            binding.input.addTextChangedListener(object : TextWatcher {
                override fun beforeTextChanged(charSequence: CharSequence, i: Int, i1: Int, i2: Int) {}
                override fun onTextChanged(charSequence: CharSequence, i: Int, i1: Int, i2: Int) {}
                override fun afterTextChanged(editable: Editable) {
                    binding.warning.visibility = View.GONE
                    binding.checkButton.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#2196F3"))

                    if(!regex.matches(binding.input.text)) {
                        binding.warning.visibility = View.VISIBLE
                        binding.checkButton.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#FFC6C6C6"))
                    }
                }
            })
        }

        binding.refreshButton.setOnClickListener {
            nativeSendClick(ClickType.CLICK_RANDOM.ordinal)
        }

        binding.checkButton.setOnClickListener {
            if(binding.warning.visibility != View.GONE) {
                binding.warning.animationXAttention(ATTENTION_BOUNCE)
                return@setOnClickListener
            }
            val strWithoutSpaces =  binding.input.text.toString().replace("\\s".toRegex(), "")
            nativeSendClick(ClickType.CLICK_CHECK.ordinal, strWithoutSpaces.toByteArray(charset("windows-1251")))
        }

        binding.buyButton.setOnClickListener {
            val strWithoutSpaces =  binding.input.text.toString().replace("\\s".toRegex(), "")
            nativeSendClick(ClickType.CLICK_BUY.ordinal, strWithoutSpaces.toByteArray(charset("windows-1251")))
        }

        binding.typeRu.setOnClickListener {
            setActiveCategory(PlateType.RU)
        }

        binding.typeUa.setOnClickListener {
            setActiveCategory(PlateType.UA)
        }

        binding.typeBy.setOnClickListener {
            setActiveCategory(PlateType.BY)
        }

        binding.typeKz.setOnClickListener {
            setActiveCategory(PlateType.KZ)
        }

        binding.exitButton.setOnClickListener {
            destroy()
        }

        setActiveCategory(PlateType.RU)
    }

    private fun show(sprite:String, randomPrice: Int, finalPrice: String) {
        activity.runOnUiThread {
            ItemHelper.loadSpriteToImageView(sprite, binding.plateImage)

            binding.randomPriceText.text = String.format("Случайный вариант \nза %s руб.", Samp.formatter.format(randomPrice))
            binding.buyPrice.text = finalPrice
        }
    }

    private fun resetAllSelects() {
        binding.typeRu.backgroundTintList   = null
        binding.typeUa.backgroundTintList  = null
        binding.typeBy.backgroundTintList   = null
        binding.typeKz.backgroundTintList   = null
    }

    private fun setActiveCategory(id: PlateType) {
        resetAllSelects()

        when(id) {
            PlateType.RU -> {
                binding.format.text = Html.fromHtml("<b>Формат номера:</b><br>А123ВС 45")

                regex = Regex("^[ABEKMHOPCTYX]{1}\\d{3}[ABEKMHOPCTYX]{2} \\d{2,3}$")
                binding.typeRu.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#61000000"))
                nativeSendClick(ClickType.SELECT_RU.ordinal)
            }
            PlateType.UA -> {
                binding.format.text = Html.fromHtml("<b>Формат номера:</b><br>АВ 1234 СТ")
                regex = Regex("^[ABEKMHOPCTYX]{2}\\s\\d{4} [ABEKMHOPCTYX]{2}$")
                binding.typeUa.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#61000000"))
                nativeSendClick(ClickType.SELECT_UA.ordinal)
            }
            PlateType.BY -> {
                binding.format.text = Html.fromHtml("<b>Формат номера:</b><br>1234 АВ-5")
                regex = Regex("^\\d{4}\\s[ABEKMHOPCTYX]{2}-\\d$")
                binding.typeBy.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#61000000"))
                nativeSendClick(ClickType.SELECT_BY.ordinal)
            }
            PlateType.KZ -> {
                binding.format.text = Html.fromHtml("<b>Формат номера:</b><br>123АВС 12")
                regex = Regex("^\\d{3}[ABEKMHOPCTYX]{2} \\d{2,3}$")
                binding.typeKz.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#61000000"))
                nativeSendClick(ClickType.SELECT_KZ.ordinal)
            }

            else -> {}
        }
    }

    override fun destroy() {
        super.destroy()

        nativeOnExit()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}