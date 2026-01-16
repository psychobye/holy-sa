package com.holy.game.gui

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

class MineGame2 {

    private lateinit var mine_2_example: ConstraintLayout
    private lateinit var mine_2_conv_del: ImageView
    private lateinit var mine_2_item_1: ImageView
    private lateinit var mine_2_progress: ProgressView
    private lateinit var mine_2_score: TextView
    private lateinit var mine_2_exit_butt: View

    private val mineGame2Stub = activity.findViewById<ViewStub>(R.id.mineGame2Stub)
    private lateinit var mineGame2Inflated: View

    private var score = 0
    private val diamond_items = ArrayList<ImageView>()
    private val dimonds = ArrayList<Int>()

    private external fun nativeExit(type: Int)

    init {
        activity.runOnUiThread {
            mineGame2Inflated = mineGame2Stub.inflate()

            mine_2_example = activity.findViewById(R.id.mine_2_example)
            mine_2_conv_del = activity.findViewById(R.id.mine_2_conv_del)
            mine_2_progress = activity.findViewById(R.id.mine_2_progress)
            mine_2_score = activity.findViewById(R.id.mine_2_score)
            mine_2_exit_butt = activity.findViewById(R.id.mine_2_exit_butt)
            mine_2_item_1 = activity.findViewById(R.id.mine_2_item_1)

            diamond_items.add(activity.findViewById(R.id.mine_2_item_1))
            diamond_items.add(activity.findViewById(R.id.mine_2_item_2))
            diamond_items.add(activity.findViewById(R.id.mine_2_item_3))
            diamond_items.add(activity.findViewById(R.id.mine_2_item_4))
            diamond_items.add(activity.findViewById(R.id.mine_2_item_5))
            diamond_items.add(activity.findViewById(R.id.mine_2_item_6))
            diamond_items.add(activity.findViewById(R.id.mine_2_item_7))
            dimonds.add(R.drawable.mine_diamond_1)
            dimonds.add(R.drawable.mine_diamond_2)
            dimonds.add(R.drawable.mine_diamond_3)
            dimonds.add(R.drawable.mine_diamond_4)
            dimonds.add(R.drawable.mine_diamond_5)
            dimonds.add(R.drawable.mine_diamond_6)

            for (i in diamond_items.indices) {
                diamond_items[i].setOnClickListener { view: View ->
                    if (view.tag === mine_2_example.getChildAt(0).tag) { // верно
                        score += 20
                        mine_2_progress.borderColor = Color.parseColor("#66bb6a")
                        mine_2_score.setTextColor(Color.parseColor("#66bb6a"))
                    }
                    else {
                        mine_2_progress.borderColor = Color.parseColor("#ef5350")
                        mine_2_score.setTextColor(Color.parseColor("#ef5350"))
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
                                    mine_2_progress.borderColor = Color.parseColor("#ffffff")
                                    mine_2_score.setTextColor(Color.parseColor("#ffffff"))
                                }
                            }
                        }
                        val timer = Timer("Timer")
                        timer.schedule(task, 1500L)
                    }
                    reshuffle()
                    mine_2_score.text = String.format("00%d", score)
                    mine_2_progress.progress = score.toFloat()
                }
            }
        }
    }
    private fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(mineGame2Inflated, R.id.mineGame2Stub, R.layout.mine_2)
        }
    }
    fun show() {
        activity.runOnUiThread {
            reshuffle()
            mine_2_progress.progress = 0F
            score = 0

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
        val example = mine_2_example.getChildAt(0) as ImageView
        example.setImageResource(dimonds[tmp])
        example.tag = tmp

        //чтобы нужный элемент полюбому был среди рандомных
        val rnd_item = random.nextInt(diamond_items.size)
        diamond_items[rnd_item].setImageResource(dimonds[tmp])
        diamond_items[rnd_item].tag = tmp
    }
}