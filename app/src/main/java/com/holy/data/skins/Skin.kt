package com.holy.data.skins

class Skin(val name: String, val priceRub: Int, val priceLC: Int, val xOffset: Float = 0.0f, val yOffset: Float = 0.0f, val zOffset: Float = 0.0f) {

    constructor(name: String, priceRub: Int)
     : this(name, priceRub, 0)

//    constructor(name: String, priceRub: Int, zOffset: Float)
//            : this(name, priceRub, 0, zOffset)
}