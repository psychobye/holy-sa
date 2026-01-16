package com.holy.game.gui.quest

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class QuestViewModel : ViewModel() {
    companion object {
        const val QUEST_NOT_TAKEN = 0
        const val QUEST_IN_PROGRESS = 1
        const val QUEST_DONE = 2
        const val QUEST_COMPLETED = 3
    }

    val questid = MutableLiveData<Int>()
    val name = MutableLiveData<String>()
    val description = MutableLiveData<String>()
    val reward = MutableLiveData<Int>()
    val status = MutableLiveData<Int>()
    val progress = MutableLiveData<Int>()
    val reset_at = MutableLiveData<Int>()
    val activeQuestCount = MutableLiveData<Int>()
}
