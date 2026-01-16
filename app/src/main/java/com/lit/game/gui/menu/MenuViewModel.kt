package com.lit.game.gui.menu

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class MenuViewModel : ViewModel() {
    val time = MutableLiveData<Float>()
    val level = MutableLiveData<Int>()
    val exp = MutableLiveData<Int>()
    val expMax = MutableLiveData<Int>()
    val family = MutableLiveData<String>()
    val familyColor = MutableLiveData<Triple<Int, Int, Int>>()
}