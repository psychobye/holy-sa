package com.holy.game.gui

import android.graphics.Color
import android.view.MotionEvent
import android.view.View
import android.view.View.OnTouchListener
import android.view.ViewStub
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.gui.FurnitureFactory.Companion.getLocationView
import com.skydoves.progressview.ProgressView
import java.util.Random
import java.util.Timer
import java.util.TimerTask
import kotlin.math.abs

class MineGame3 {
    private var score = 0
    private val dimonds = ArrayList<Int>()
    private val items = ArrayList<ImageView>()
    private val dimonds_box_captions_color = IntArray(7)
    private val dimonds_box_captions = arrayOfNulls<String>(7)
    private val box = ArrayList<ConstraintLayout>()

    private lateinit var mine_3_main_layout: ConstraintLayout
    private lateinit var mine_3_progress: ProgressView
    private lateinit var mine_3_score: TextView
    private lateinit var mine_3_exit_butt: View

    private val mineGame3Stub = activity.findViewById<ViewStub>(R.id.mineGame3Stub)
    private lateinit var mineGame3Inflated: View

    private external fun nativeExit(type: Int)

    init {
        activity.runOnUiThread {
            mineGame3Inflated = mineGame3Stub.inflate()

            mine_3_exit_butt = activity.findViewById(R.id.mine_3_exit_butt)
            mine_3_exit_butt.setOnClickListener {
                nativeExit(0)
                hide()
            }
            mine_3_progress = activity.findViewById(R.id.mine_3_progress)
            mine_3_score = activity.findViewById(R.id.mine_3_score)
            dimonds_box_captions[2] = "Золото"
            dimonds_box_captions_color[2] = Color.parseColor("#fdd835")
            dimonds_box_captions[3] = "Кристалл"
            dimonds_box_captions_color[3] = Color.parseColor("#00e5ff")
            dimonds_box_captions[0] = "Кварц"
            dimonds_box_captions_color[0] = Color.parseColor("#f06292")
            dimonds_box_captions[4] = "Руда"
            dimonds_box_captions_color[4] = Color.parseColor("#8391AC")
            dimonds_box_captions[1] = "Медь"
            dimonds_box_captions_color[1] = Color.parseColor("#C1620A")
            dimonds_box_captions[5] = "Уголь"
            dimonds_box_captions_color[5] = Color.parseColor("#c7c7c7")
            box.add(activity.findViewById(R.id.mine_3_box_1))
            box.add(activity.findViewById(R.id.mine_3_box_2))
            box.add(activity.findViewById(R.id.mine_3_box_3))
            items.add(activity.findViewById(R.id.mine_3_item_1))
            items.add(activity.findViewById(R.id.mine_3_item_2))
            items.add(activity.findViewById(R.id.mine_3_item_3))
            for (i in items.indices) {
                items[i].setOnTouchListener(touchListener)
            }

            //  Init();
            mine_3_main_layout = activity.findViewById(R.id.mine_3_main_layout)

            dimonds.add(R.drawable.mine_diamond_1)
            dimonds.add(R.drawable.mine_diamond_2)
            dimonds.add(R.drawable.mine_diamond_3)
            dimonds.add(R.drawable.mine_diamond_4)
            dimonds.add(R.drawable.mine_diamond_5)
            dimonds.add(R.drawable.mine_diamond_6)
        }
    }

    fun show() {
        activity.runOnUiThread {
            reshuffle()
            score = 0
            mine_3_progress.progress = 0F
            mine_3_main_layout.post { UpdateSizes() }

        }
    }

    fun UpdateSizes() {
        //position
        var layoutParams: ConstraintLayout.LayoutParams
        val screenwidth = activity.resources.displayMetrics.widthPixels
        val screenheight = activity.resources.displayMetrics.heightPixels
        for (i in items.indices) {
            layoutParams = items[i].layoutParams as ConstraintLayout.LayoutParams

            //  items.get(i).setTop((int) mine_3_score.getY()- mine_3_score.getHeight());
            layoutParams.topMargin = (screenheight - items[i].height * 1.8).toInt()
            when (i) {
                0 -> {
                    layoutParams.leftMargin = items[1].left - items[0].width * 2
                }

                1 -> {
                    layoutParams.leftMargin = screenwidth / 2
                }

                2 -> {
                    layoutParams.leftMargin = items[1].left + items[0].width * 2
                }
            }
            items[i].layoutParams = layoutParams
        }
    }

    private fun reshuffle() {
        val random = Random()
        var rnd_cell = random.nextInt(dimonds.size)
        var rnd_diamond = random.nextInt(dimonds.size)
        for (i in rnd_cell until rnd_cell + 3) {
            val j = i % 3
            items[j].visibility = View.VISIBLE
            items[j].setImageResource(dimonds[rnd_diamond % dimonds.size])
            items[j].tag = rnd_diamond % dimonds.size
            rnd_diamond++
        }

        //
        rnd_cell = random.nextInt(dimonds.size)
        var d = 0
        for (i in rnd_cell until rnd_cell + 3) {
            val j = i % 3
            box[j].tag = items[d].tag
            val boxcaption = box[j].getChildAt(0) as TextView
            boxcaption.text = dimonds_box_captions[items[d].tag as Int]
            boxcaption.setTextColor(dimonds_box_captions_color[items[d].tag as Int])
            boxcaption.setShadowLayer(10f, 0f, 0f, dimonds_box_captions_color[items[d].tag as Int])
            d++
        }
    }
    private fun hide() {
        activity.runOnUiThread {
            Samp.deInflatedViewStud(mineGame3Inflated, R.id.mineGame3Stub, R.layout.mine_3)
        }
    }
    private fun getIndexByTag(view: View): Int {
        for (i in box.indices) {
            if (box[i].tag === view.tag) {
                return i
            }
        }
        return -1
    }

    private var xDelta = 0
    private var yDelta = 0
    private val touchListener: OnTouchListener = object : OnTouchListener {
        override fun onTouch(view: View, event: MotionEvent): Boolean {
            val x = event.rawX.toInt()
            val y = event.rawY.toInt()
            val layoutParams = view.layoutParams as ConstraintLayout.LayoutParams
            val idx = getIndexByTag(view)
            val xbox = abs(getLocationView(box[idx]).x)
            val ybox = abs(getLocationView(box[idx]).y)
            when (event.action and MotionEvent.ACTION_MASK) {
                MotionEvent.ACTION_DOWN -> {
                    xDelta = x - layoutParams.leftMargin
                    yDelta = y - layoutParams.topMargin
                }

                MotionEvent.ACTION_UP -> {
                    run {
                        if (x > xbox && x < xbox + box[idx].width && y > ybox && y < ybox + box[idx].height) {
                            activity.runOnUiThread {
                                view.visibility = View.GONE
                                score += 100
                                if (score >= 300) {
                                    nativeExit(1)
                                    hide()
                                }
                                mine_3_progress.borderColor = Color.parseColor("#66bb6a")
                                mine_3_score.setTextColor(Color.parseColor("#66bb6a"))
                                mine_3_score.text = String.format("00%d", score)
                                mine_3_progress.progress = score.toFloat()
                            }
                            val task: TimerTask = object : TimerTask() {
                                override fun run() {
                                    activity.runOnUiThread {
                                        mine_3_progress.borderColor = Color.parseColor("#ffffff")
                                        mine_3_score.setTextColor(Color.parseColor("#ffffff"))
                                    }
                                }
                            }
                            val timer = Timer("Timer")
                            timer.schedule(task, 1500L)
                        }
                        box[idx].scaleX = 1.0.toFloat()
                        box[idx].scaleY = 1.0.toFloat()
                    }
                    run {
                        layoutParams.leftMargin = x - xDelta
                        if (x > xbox && x < xbox + box[idx].width && y > ybox && y < ybox + box[idx].height) {
                            box[idx].scaleX = 1.1.toFloat()
                            box[idx].scaleY = 1.1.toFloat()
                        }
                        else {
                            box[idx].scaleX = 1.0.toFloat()
                            box[idx].scaleY = 1.0.toFloat()
                        }
                        layoutParams.topMargin = y - yDelta
                        layoutParams.rightMargin = 0
                        layoutParams.bottomMargin = 0
                        view.layoutParams = layoutParams
                    }
                }

                MotionEvent.ACTION_MOVE -> {
                    layoutParams.leftMargin = x - xDelta
                    if (x > xbox && x < xbox + box[idx].width && y > ybox && y < ybox + box[idx].height) {
                        box[idx].scaleX = 1.1.toFloat()
                        box[idx].scaleY = 1.1.toFloat()
                    }
                    else {
                        box[idx].scaleX = 1.0.toFloat()
                        box[idx].scaleY = 1.0.toFloat()
                    }
                    layoutParams.topMargin = y - yDelta
                    layoutParams.rightMargin = 0
                    layoutParams.bottomMargin = 0
                    view.layoutParams = layoutParams
                }
            }
            return true
        }
    }
}