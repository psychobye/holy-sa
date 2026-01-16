package com.holy.game.gui.tuning

data class TuningItem(
    var caption: String = "",
    var icon: Int = 0,
    var customId: Int = -1,
    var price: Int = 0
    ) {
//    constructor(caption: String, icon: Int):
//            this(caption, 0, icon)
//
//    constructor(caption: String, icon: Int, customId: Int):
//            this(caption, 0, icon, customId)
}