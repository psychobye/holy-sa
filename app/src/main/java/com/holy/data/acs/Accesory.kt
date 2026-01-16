package com.holy.data.acs

import com.lit.data.Rare

class Accesory(
    var name: String,
    var priceRub: Int,
    var priceLc: Int = 0,
    var rare: Int = Rare.NONE,

    var rotX: Float = 0.0f,
    var rotY: Float = 180.0f,
    var rotZ: Float = 0.0f,

    var offsetX: Float = 0.0f,
    var offsetY: Float = 0.0f,
    var offsetZ: Float = 0.0f,
){

}