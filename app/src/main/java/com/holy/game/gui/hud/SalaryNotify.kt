package com.holy.game.gui.hud

import android.view.View
import android.view.animation.AnimationUtils
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.skydoves.progressview.ProgressView
import java.util.Timer
import java.util.TimerTask

class SalaryNotify {
    val salary_job_layout = activity.findViewById<ConstraintLayout>(R.id.salary_job_layout)
    val salary_job_salary_text = activity.findViewById<TextView>(R.id.salary_job_salary_text)
    val salary_job_lvl_text = activity.findViewById<TextView>(R.id.salary_job_lvl_text)
    val salary_job_progress = activity.findViewById<ProgressView>(R.id.salary_job_progress)
    val salary_job_exp_text = activity.findViewById<TextView>(R.id.salary_job_exp_text)

    val old_salary_exp = 0
    var current_real_salary = 0
    var current_visual_salary = 0
    var thread_update_salary: Thread? = null


    init {
        salary_job_layout.visibility = View.GONE
    }
    private var updateSalaryRunnable = Runnable()
    {
        while (current_visual_salary < current_real_salary && !Thread.currentThread().isInterrupted) {
            current_visual_salary++
            activity.runOnUiThread {
                salary_job_salary_text.text = String.format("Заработано: %s рублей", Samp.formatter.format(current_visual_salary.toLong()))
            }
            try {
                Thread.sleep(5)
            } catch (e: InterruptedException) {
                activity.runOnUiThread {
                    salary_job_salary_text.text = String.format("Заработано: %s рублей", Samp.formatter.format(current_real_salary.toLong()))
                }
                break
            }
        }
    }

    fun update(salary: Int, lvl: Int, exp: Float) {
        if (current_visual_salary > salary) {
            current_visual_salary = 0
        }
        //current_visual_salary = 0;
        // current_visual_salary = current_real_salary;
        current_real_salary = salary
        activity.runOnUiThread {
            salary_job_exp_text.text = String.format("%.2f / 100", exp)
            salary_job_lvl_text.text = String.format("Ваш уровень работника: %d", lvl)
            if (old_salary_exp > salary) {
                salary_job_progress.progress = 0F
            }
            salary_job_progress.progress = exp
            salary_job_layout.visibility = View.VISIBLE
        }
        if (thread_update_salary != null && thread_update_salary!!.isAlive) {
            thread_update_salary!!.interrupt()
        }
        if (salary == 0) {
            current_visual_salary = 0
            activity.runOnUiThread { salary_job_salary_text.text = "Заработано: 0 рублей" }
        } else {
            thread_update_salary = Thread(updateSalaryRunnable)
            thread_update_salary!!.start()
        }
        val task: TimerTask = object : TimerTask() {
            override fun run() {
                activity.runOnUiThread {
                    salary_job_layout.startAnimation(AnimationUtils.loadAnimation(activity, R.anim.popup_hide_to_top))
                    salary_job_layout.visibility = View.GONE
                }
            }
        }
        val timer = Timer("Timer")
        timer.schedule(task, 5000L)
    }
}