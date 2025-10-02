package com.lit.game.gui.inventory

import android.content.res.ColorStateList
import android.graphics.Color

class InventoryItem(
    var name: String = "",
    var count: String = "",
    var sprite: String = "",
    var rareColor: ColorStateList = ColorStateList.valueOf(Color.parseColor("#00000000")),
    var caption: String = ""
){

}