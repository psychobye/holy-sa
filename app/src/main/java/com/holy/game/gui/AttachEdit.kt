package com.holy.game.gui

import android.annotation.SuppressLint
import android.content.res.ColorStateList
import android.graphics.Color
import android.view.MotionEvent
import android.view.View
import android.view.View.OnTouchListener
import android.view.ViewStub
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import java.lang.Thread.sleep

class AttachEdit @SuppressLint("ClickableViewAccessibility") constructor() {
    private val BUTTON_LEFT_RIGHT = 0
    private val BUTTON_UP_DOWN = 1
    private val BUTTON_PUSH_PULL = 2
    private val BUTTON_SCALE = 3
    private val BUTTON_ROT_X = 4
    private val BUTTON_ROT_Y = 5
    private val BUTTON_ROT_Z = 6
    private var active_button = -1

    private lateinit var attach_exit_butt: MaterialButton
    private lateinit var attach_save_butt: MaterialButton
    private lateinit var attach_leftright: ConstraintLayout
    private lateinit var attach_topbott: ConstraintLayout
    private lateinit var attach_pushpull: ConstraintLayout
    private lateinit var attach_scale: ConstraintLayout
    private lateinit var attach_rotX: ConstraintLayout
    private lateinit var attach_rotY: ConstraintLayout
    private lateinit var attach_rotZ: ConstraintLayout
    private lateinit var attach_text_description: TextView
    private lateinit var attach_left_butt: ConstraintLayout
    private lateinit var attach_right_button: ConstraintLayout

    private val objectEditorStub = activity.findViewById<ViewStub>(R.id.objectEditorStub)
    private lateinit var objectEditorInflated: View

    external fun Exit()
    external fun AttachClick(buttonType: Int, buttonID: Boolean)
    external fun Save()

    init {
        activity.runOnUiThread {
            objectEditorInflated = objectEditorStub.inflate()

            attach_exit_butt = activity.findViewById(R.id.attach_exit_butt)
            attach_save_butt = activity.findViewById(R.id.attach_save_butt)
            attach_leftright = activity.findViewById(R.id.attach_leftright)
            attach_topbott = activity.findViewById(R.id.attach_topbott)
            attach_pushpull = activity.findViewById(R.id.attach_pushpull)
            attach_scale = activity.findViewById(R.id.attach_scale)
            attach_rotX = activity.findViewById(R.id.attach_rotX)
            attach_rotY = activity.findViewById(R.id.attach_rotY)
            attach_rotZ = activity.findViewById(R.id.attach_rotZ)
            attach_text_description = activity.findViewById(R.id.attach_text_description)
            attach_left_butt = activity.findViewById(R.id.attach_left_butt)
            attach_right_button = activity.findViewById(R.id.attach_right_button)

            attach_left_butt.setOnTouchListener(touchListener)
            attach_right_button.setOnTouchListener(touchListener)

            attach_save_butt.setOnClickListener {
                Samp.deInflatedViewStud(objectEditorInflated, R.id.objectEditorStub, R.layout.object_editor)

                Save()
            }

            attach_exit_butt.setOnClickListener {
                Samp.deInflatedViewStud(objectEditorInflated, R.id.objectEditorStub, R.layout.object_editor)

                Exit()
            }

            attach_leftright.setOnClickListener { view: View ->
                active_button = BUTTON_LEFT_RIGHT
                attach_text_description.text = "Установите смещение по оси Y"
                SelectetItem(view)
            }

            attach_topbott.setOnClickListener { view: View ->
                active_button = BUTTON_UP_DOWN
                attach_text_description.text = "Установите смещение по оси Z"
                SelectetItem(view)
            }

            attach_pushpull.setOnClickListener { view: View ->
                active_button = BUTTON_PUSH_PULL
                attach_text_description.text = "Установите смещение по оси X"
                SelectetItem(view)
            }

            attach_scale.setOnClickListener { view: View ->
                attach_text_description.text = "Установите размер"
                active_button = BUTTON_SCALE
                SelectetItem(view)
            }

            attach_rotX.setOnClickListener { view: View ->
                attach_text_description.text = "Установите поворот"
                active_button = BUTTON_ROT_X
                SelectetItem(view)
            }

            attach_rotY.setOnClickListener { view: View ->
                attach_text_description.text = "Установите поворот"
                active_button = BUTTON_ROT_Y
                SelectetItem(view)
            }

            attach_rotZ.setOnClickListener { view: View ->
                attach_text_description.text = "Установите поворот"
                active_button = BUTTON_ROT_Z
                SelectetItem(view)
            }
        }
    }

    fun SelectetItem(view: View) {
        attach_leftright.getChildAt(0).backgroundTintList = null
        attach_leftright.getChildAt(2).backgroundTintList = null

        attach_topbott.getChildAt(0).backgroundTintList = null
        attach_topbott.getChildAt(2).backgroundTintList = null

        attach_pushpull.getChildAt(0).backgroundTintList = null
        attach_pushpull.getChildAt(2).backgroundTintList = null

        attach_scale.getChildAt(0).backgroundTintList = null
        attach_scale.getChildAt(2).backgroundTintList = null

        attach_rotX.getChildAt(0).backgroundTintList = null
        attach_rotX.getChildAt(2).backgroundTintList = null

        attach_rotY.getChildAt(0).backgroundTintList = null
        attach_rotY.getChildAt(2).backgroundTintList = null

        attach_rotZ.getChildAt(0).backgroundTintList = null
        attach_rotZ.getChildAt(2).backgroundTintList = null

        val tmp = view as ConstraintLayout
        tmp.getChildAt(0).backgroundTintList =
            ColorStateList.valueOf(Samp.activity.resources.getColor(R.color.yellow))
        tmp.getChildAt(2).backgroundTintList = ColorStateList.valueOf(Color.parseColor("#000000"))
    }

    //
    private var isLongPressRunnableActive = false

    private val touchListener = OnTouchListener { view, event ->
        when (event.action) {
            MotionEvent.ACTION_DOWN -> {
                // Запуск потока при нажатии
                isLongPressRunnableActive = true
                Thread {
                    while (isLongPressRunnableActive) {
                        if (view === attach_left_butt) {
                            AttachClick(active_button, false)
                        } else {
                            AttachClick(active_button, true)
                        }
                        sleep(100)
                    }
                }.start()
                view.performClick()
            }
            MotionEvent.ACTION_UP -> {
                // Остановка потока при отпускании
                isLongPressRunnableActive = false
            }
        }
        true // Возвращаем true, чтобы показать, что событие обработано
    }

}