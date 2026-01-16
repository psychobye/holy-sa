package com.holy.game.gui.magicStore

import com.lit.game.SnapShot

class MagicStoreItem(val name: String, val category: Int, val price: Int, val priceType: Int, val res: Int, val snap: SnapShot?) {

    constructor(name: String, category: Int, price: Int, priceType: Int, res: Int)
        :this(name, category, price, priceType, res, null)

    constructor(name: String, category: Int, price: Int, priceType: Int, snap: SnapShot?)
            :this(name, category, price, priceType, 0, snap)
}