package com.lit.game.gui.flexMenu

data class PopItem(
    val id: Int,
    val title: String,
    val iconRes: Int? = null,
    val textInstead: String? = null,
    val action: PopAction? = null,
    var isActive: Boolean = false
)

enum class PopAction {
    TOGGLE, APPLY, CLOSE, CUSTOM
}
