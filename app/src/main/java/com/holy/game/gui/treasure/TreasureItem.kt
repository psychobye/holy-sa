package com.holy.game.gui.treasure

import com.holy.game.SnapShot

class TreasureItem(val imgResource: Int, val name: String, val rare: Int, val snap: SnapShot? = null) {

    constructor(res: Int, name: String, rare: Int)
            : this(res, name, rare, null)

    constructor(snap: SnapShot, name: String, rare: Int)
            : this(0, name, rare, snap)
}