package com.holy.game.gui.battle_pass

import android.content.res.ColorStateList
import android.graphics.Color
import android.text.Spanned
import androidx.core.text.toSpanned

enum class BattlePassItemStatus {
    LOCKED,
    AVAILABLE_COMPLETE,
    COMPLETED
}

class BattlePassMainItem (
    var name: Spanned = "".toSpanned(),
    var sprite: String = "",
    var status: Int = 0,
    var rareColor: ColorStateList = ColorStateList.valueOf(Color.parseColor("#00000000"))
)