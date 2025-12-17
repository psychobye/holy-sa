package com.lit.game.gui.quest

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class QuestViewModel : ViewModel() {
    val questid = MutableLiveData<Int>()
    val name = MutableLiveData<String>()
    val description = MutableLiveData<String>()
    val reward = MutableLiveData<Int>()
}