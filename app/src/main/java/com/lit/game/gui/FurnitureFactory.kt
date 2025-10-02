package com.lit.game.gui

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Color
import android.graphics.Point
import android.graphics.PorterDuff
import android.view.MotionEvent
import android.view.View
import android.view.View.OnTouchListener
import android.view.ViewStub
import android.widget.FrameLayout
import android.widget.ImageView
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.gui.util.Utils
import java.util.Random
import kotlin.math.abs

class FurnitureFactory {
    private lateinit var furniture_factory_main_layout: FrameLayout
    private lateinit var furniture_factory_button_continue: MaterialButton

    private var furniturePart: ArrayList<ImageView> = ArrayList()
    private var furnitureReadyPart: ArrayList<ImageView> = ArrayList()
    private var totalscores = 0

    private val furnitureFactoryStub = activity.findViewById<ViewStub>(R.id.furnitureFactoryStub)
    private lateinit var furnitureFactoryInflated: View

    external fun nativeSendAction(buttonID: Int)

    init {
        activity.runOnUiThread {
            furnitureFactoryInflated = furnitureFactoryStub.inflate()

            furniture_factory_main_layout = activity.findViewById(R.id.furniture_factory_main_layout)
            furniture_factory_button_continue = activity.findViewById(R.id.furniture_factory_button_continue)

            furniture_factory_button_continue.setOnClickListener {
                activity.runOnUiThread {
                    Samp.deInflatedViewStud(furnitureFactoryInflated, R.id.furnitureFactoryStub, R.layout.furniture_factory)
                }
                nativeSendAction(0)
            }
            furniturePart.add(activity.findViewById(R.id.furniture_part1))
            furniturePart.add(activity.findViewById(R.id.furniture_part2))
            furniturePart.add(activity.findViewById(R.id.furniture_part3))
            furniturePart.add(activity.findViewById(R.id.furniture_part4))
            furnitureReadyPart.add(activity.findViewById(R.id.furniture_readypart1))
            furnitureReadyPart.add(activity.findViewById(R.id.furniture_readypart2))
            furnitureReadyPart.add(activity.findViewById(R.id.furniture_readypart3))
            furnitureReadyPart.add(activity.findViewById(R.id.furniture_readypart4))
            for (i in furniturePart.indices) {
                furniturePart.get(i).setOnTouchListener(touchListener)
            }
        }
    }

    fun show(furnitureType: Int) {
        activity.runOnUiThread {
            totalscores = 0
            val bitmap: Bitmap = when (furnitureType) {
                0 -> {
                    BitmapFactory.decodeResource(activity.resources, R.drawable.furniture_chair)
                }

                1 -> {
                    BitmapFactory.decodeResource(activity.resources, R.drawable.furniture_closet)
                }

                else -> {
                    BitmapFactory.decodeResource(activity.resources, R.drawable.furniture_door)
                }
            }
            val ggg = breakBitmap(bitmap)
            for (i in furniturePart.indices) {
                val layoutParams = furniturePart[i].layoutParams as FrameLayout.LayoutParams
                furniturePart[i].setImageBitmap(ggg[i])
                layoutParams.leftMargin = Random().nextInt(1000)
                layoutParams.topMargin = Random().nextInt(500)
                furniturePart[i].layoutParams = layoutParams
                furniturePart[i].visibility = View.VISIBLE
                furnitureReadyPart[i].setImageBitmap(ggg[i])
                furnitureReadyPart[i].setColorFilter(Color.parseColor("#cfd8dc"), PorterDuff.Mode.SRC_ATOP)
            }
            Utils.ShowLayout(furniture_factory_main_layout, true)
        }
    }

    private var xDelta = 0
    private var yDelta = 0
    private val touchListener = OnTouchListener { view, event ->
        val x = event.rawX.toInt()
        val y = event.rawY.toInt()
        val layoutParams = view.layoutParams as FrameLayout.LayoutParams
        val needIdx = getFurnitureIndex(view)
        when (event.action and MotionEvent.ACTION_MASK) {
            MotionEvent.ACTION_DOWN -> {
                xDelta = x - layoutParams.leftMargin
                yDelta = y - layoutParams.topMargin
            }

            MotionEvent.ACTION_UP -> {
                if (abs(getLocationView(furnitureReadyPart[needIdx]).x - layoutParams.leftMargin) < 10) {
                    furnitureReadyPart[needIdx].clearColorFilter()
                    activity.runOnUiThread { view.visibility = View.GONE }
                    totalscores += 25
                }
                if (totalscores >= 100) {
                    activity.runOnUiThread {
                        Utils.HideLayout(furniture_factory_main_layout, true)
                    }
                    Samp.deInflatedViewStud(furnitureFactoryInflated, R.id.furnitureFactoryStub, R.layout.furniture_factory)
                    nativeSendAction(1)
                }
            }

            MotionEvent.ACTION_MOVE -> {
                layoutParams.leftMargin = x - xDelta
                layoutParams.topMargin = y - yDelta
                layoutParams.rightMargin = 0
                layoutParams.bottomMargin = 0
                view.layoutParams = layoutParams
            }
        }
        view.performClick()

        true
    }

    fun breakBitmap(bitmap: Bitmap): Array<Bitmap> {
        // Разрезает картинку на 4 куска
        val halfWidth = bitmap.width / 2
        val halfHeight = bitmap.height / 2
        val topLeft = Bitmap.createBitmap(bitmap, 0, 0, halfWidth, halfHeight)
        val topRight = Bitmap.createBitmap(bitmap, halfWidth, 0, halfWidth, halfHeight)
        val bottomLeft = Bitmap.createBitmap(bitmap, 0, halfHeight, halfWidth, halfHeight)
        val bottomRight = Bitmap.createBitmap(bitmap, halfWidth, halfHeight, halfWidth, halfHeight)
        return arrayOf(topLeft, topRight, bottomLeft, bottomRight)
    }

    fun getFurnitureIndex(view: View): Int {
        for (i in furniturePart.indices) {
            if (furniturePart[i] === view) {
                return i
            }
        }
        return -1
    }

    companion object {
        @JvmStatic
        fun getLocationView(view: View): Point {
            val location = IntArray(2)
            view.getLocationOnScreen(location)
            return Point(location[0], location[1])
        }
    }
}