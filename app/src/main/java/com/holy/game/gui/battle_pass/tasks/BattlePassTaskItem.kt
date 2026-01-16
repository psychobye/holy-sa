package com.holy.game.gui.battle_pass.tasks

import android.text.Spanned
import androidx.core.text.toSpanned

data class BattlePassTaskItem(
    var caption: Spanned = "".toSpanned(),
    var description: Spanned = "".toSpanned(),
    var curProgress: Int = 0,
    var maxProgress: Int = 0,
    var reward: Int = 0
)
