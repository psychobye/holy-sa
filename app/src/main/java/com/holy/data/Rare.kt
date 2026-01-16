package com.holy.data

import android.content.res.ColorStateList
import android.graphics.Color

object Rare {
    const val NONE        = 0
    const val UNCOMMON    = 1
    const val COMMON      = 2
    const val IMMORTAL    = 3
    const val MYTHICAL    = 4
    const val LEGENDARY   = 5

    fun getRareFromPrice(priceLc: Int) : Int{
        if(priceLc < 200)
            return UNCOMMON

        if(priceLc < 500)
            return COMMON

        if(priceLc < 1100)
            return IMMORTAL

        else
            return MYTHICAL

    }
    fun getName(rare: Int): String {
        if(rare == UNCOMMON)
            return "Обычный"

        if(rare == COMMON)
            return "Необычный"

        if(rare == IMMORTAL)
            return "Редкий"

        if(rare == MYTHICAL)
            return "Мифический"

        if(rare == LEGENDARY)
            return "Легендарный"

        return "Неизвестно"
    }

    fun getColorAsStateList(rare: Int): ColorStateList {
        if(rare == UNCOMMON)
            return ColorStateList.valueOf(Color.parseColor("#5e98d9"))

        if(rare == COMMON)
            return ColorStateList.valueOf(Color.parseColor("#4b69ff"))

        if(rare == IMMORTAL)
            return ColorStateList.valueOf(Color.parseColor("#b28a33"))

        if(rare == MYTHICAL)
            return ColorStateList.valueOf(Color.parseColor("#8847ff"))

        if(rare == LEGENDARY)
            return ColorStateList.valueOf(Color.parseColor("#eb4b4b"))

        return ColorStateList.valueOf(Color.parseColor("#00000000"))
    }

    fun getColorAsInt(rare: Int): Int {
        if(rare == UNCOMMON)
            return Color.parseColor("#5e98d9")

        if(rare == COMMON)
            return Color.parseColor("#4b69ff")

        if(rare == IMMORTAL)
            return Color.parseColor("#b28a33")

        if(rare == MYTHICAL)
            return Color.parseColor("#8847ff")

        if(rare == LEGENDARY)
            return Color.parseColor("#eb4b4b")

        return Color.parseColor("#000000")
    }
}