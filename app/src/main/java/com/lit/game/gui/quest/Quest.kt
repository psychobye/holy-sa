package com.lit.game.gui.quest

import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import com.lit.game.core.Samp.Companion.activity

class Quest {
    fun show(questid: Int, name: String, description: String, reward: Int) {
        activity.runOnUiThread {
            val act = activity as? AppCompatActivity ?: return@runOnUiThread
            val vm = ViewModelProvider(act).get(QuestViewModel::class.java)

            vm.questid.postValue(questid)
            vm.name.postValue(name)
            vm.description.postValue(description)
            vm.reward.postValue(reward)
        }
    }
}