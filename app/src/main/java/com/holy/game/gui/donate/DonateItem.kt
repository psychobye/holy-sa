package com.holy.game.gui.donate

import com.holy.game.SnapShot

data class DonateItem(
    var name: String,
    var category: Int = 0,
    var price: Int = 0,
    var imgRecourse: Int = 0,
    var value: Int = 0,
    var snap: SnapShot? = null
    ) {

    constructor(name: String, category: Int, price: Int, imgRecourse: Int, value: Int)
            : this(name, category, price, imgRecourse, value, null)

    constructor(name: String, category: Int, price: Int, value: Int, snap: SnapShot)
            : this(name, category, price, 0, value, snap)

    var sqlId = 0
}