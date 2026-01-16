package com.lit.game.gui.achivments

class Achivment(val caption: String, val desription: String, val steps: Array<Int>, val rewards: Array<Int>)
{
    var progress    = 0
    var curStep     = 0

    fun isAchivmentComplete(): Boolean {
        return curStep == 3
    }

    fun getAvailableReward(): Int {
        var totalSum = 0
        for (i in curStep until rewards.size) {
            totalSum += rewards[i]
        }
        return totalSum
    }

    fun isNeedGiveButt(): Boolean {
        if(curStep >= 3)
            return false

        if(progress >= steps[curStep])
            return true

        return false
    }

}
