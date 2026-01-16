package com.holy.game.gui.flexMenu

data class FlexMenuItem(
    val id: Int,
    val title: String,
    val iconRes: Int? = null,
    val payload: Any? = null,
    var isActive: Boolean = false,
    val isTab: Boolean = false
)
