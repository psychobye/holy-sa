package com.holy.game.gui.quest

import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import com.holy.game.core.Samp.Companion.activity

class Quest {

    fun add(
        questid: Int,
        name: String,
        description: String,
        reward: Int,
        status: Int,
        progress: Int,
        reset_at: Int
    ) {
        activity.runOnUiThread {
            val act = activity as? AppCompatActivity ?: return@runOnUiThread
            val vm = ViewModelProvider(act).get(QuestViewModel::class.java)

            vm.questid.postValue(questid)
            vm.name.postValue(name)
            vm.description.postValue(description)
            vm.reward.postValue(reward)
            vm.status.postValue(status)
            vm.progress.postValue(progress)
            vm.reset_at.postValue(reset_at)
        }
    }

    fun updateActiveCount(count: Int) {
        activity.runOnUiThread {
            val act = activity as? AppCompatActivity ?: return@runOnUiThread
            val vm = ViewModelProvider(act).get(QuestViewModel::class.java)
            vm.activeQuestCount.postValue(count)
        }
    }
}