package com.lit.game.gui.quest

enum class QuestStatus(val value: Int) {
    NOT_TAKEN(0),
    IN_PROGRESS(1),
    DONE(2),
    COMPLETED(3);

    companion object {
        fun from(value: Int): QuestStatus =
            entries.firstOrNull { it.value == value } ?: NOT_TAKEN
    }
}