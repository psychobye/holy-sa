package com.lit.game.gui

import android.graphics.Color
import android.view.View
import android.view.ViewStub
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.skydoves.progressview.ProgressView
import java.util.Random
import java.util.Timer
import java.util.TimerTask

class MineGame1 {
    private val diamond_items = ArrayList<ImageView>()
    private val dimonds = ArrayList<Int>()

    private lateinit var mine_1_example: ConstraintLayout
    private lateinit var mine_1_progress: ProgressView
    private lateinit var mine_1_score: TextView
    private lateinit var mine_1_exit_butt: View

    private val mineGame1Stub = activity.findViewById<ViewStub>(R.id.mineGame1Stub)
    private lateinit var mineGame1Inflated: View

    private var score = 0

    private external fun nativeExit(exittype: Int)

    init {
       activity.runOnUiThread {
           mineGame1Inflated = mineGame1Stub.inflate()

           mine_1_exit_butt = activity.findViewById(R.id.mine_1_exit_butt)
           mine_1_exit_butt.setOnClickListener {
               nativeExit(0)
               hide();
           }

           mine_1_example = activity.findViewById(R.id.mine_1_example)
           mine_1_progress = activity.findViewById(R.id.mine_1_progress)
           mine_1_score = activity.findViewById(R.id.mine_1_score)

           dimonds.add(R.drawable.mine_diamond_1)
           dimonds.add(R.drawable.mine_diamond_2)
           dimonds.add(R.drawable.mine_diamond_3)
           dimonds.add(R.drawable.mine_diamond_4)
           dimonds.add(R.drawable.mine_diamond_5)
           dimonds.add(R.drawable.mine_diamond_6)
           diamond_items.add(activity.findViewById(R.id.mine_1_item_1))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_2))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_3))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_4))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_5))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_6))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_7))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_8))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_9))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_10))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_11))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_12))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_13))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_14))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_15))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_16))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_17))
           diamond_items.add(activity.findViewById(R.id.mine_1_item_18))
           for (i in diamond_items.indices) {
               diamond_items[i].setOnClickListener { view: View ->
                   if (view.tag === mine_1_example.getChildAt(0).tag) { // верно
                       score += 20
                       mine_1_progress.borderColor = Color.parseColor("#66bb6a")
                       mine_1_score.setTextColor(Color.parseColor("#66bb6a"))
                   }
                   else {
                       mine_1_progress.borderColor = Color.parseColor("#ef5350")
                       mine_1_score.setTextColor(Color.parseColor("#ef5350"))
                       score -= 20
                       if (score < 0) score = 0
                   }
                   if (score >= 100) {
                       nativeExit(1)
                       hide()
                       return@setOnClickListener
                   }
                   else {
                       val task: TimerTask = object : TimerTask() {
                           override fun run() {
                               activity.runOnUiThread {
                                   mine_1_progress.borderColor = Color.parseColor("#ffffff")
                                   mine_1_score.setTextColor(Color.parseColor("#ffffff"))
                               }
                           }
                       }
                       val timer = Timer("Timer")
                       timer.schedule(task, 1500L)
                   }
                   reshuffle()
                   mine_1_score.text = String.format("00%d", score)
                   mine_1_progress.progress = score.toFloat()
               }
           }
       }
    }

    fun show() {
        activity.runOnUiThread {
            score = 0
            mine_1_progress.progress = 0F
            reshuffle()
        }
    }
    private fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(mineGame1Inflated, R.id.mineGame1Stub, R.layout.mine_1)
        }
    }
    private fun reshuffle() {
        val random = Random()
        for (i in diamond_items.indices) {
            val tmp = random.nextInt(dimonds.size)
            diamond_items[i].setImageResource(dimonds[tmp])
            diamond_items[i].tag = tmp
        }
        val tmp = random.nextInt(dimonds.size)
        val example = mine_1_example.getChildAt(0) as ImageView
        example.setImageResource(dimonds[tmp])
        example.tag = tmp

        //чтобы нужный элемент полюбому был среди рандомных
        val rnd_item = random.nextInt(diamond_items.size)
        diamond_items[rnd_item].setImageResource(dimonds[tmp])
        diamond_items[rnd_item].tag = tmp
    }

}