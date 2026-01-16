package com.holy.game.gui.treasure

import android.content.res.ColorStateList
import android.graphics.Color
import android.util.DisplayMetrics
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.LinearSmoothScroller
import androidx.recyclerview.widget.RecyclerView
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.TreasureBinding
import com.holy.game.gui.NativeGui
import com.holy.data.TreasuresList


class Treasure : NativeGui<TreasureBinding>(TreasureBinding::class) {
    private var spinnerAdapter: SpinnerAdapter?             = null
    private var prizesListAdapter: PrizesListAdapter?       = null

    private var targetPosition          = 0
    private var treasureId              = 0
    private var available               = false

    private external fun nativeSendOpenTreasure(treasureId: Int)
    private external fun nativeDestructor()
    private external fun nativeShowGiftNotify(name: String, rare: Int)

    init {
        activity.runOnUiThread {
            binding.treasureExitButt.setOnClickListener {
                activity.runOnUiThread {
                    destroy()
                }
            }

            binding.treasureOpenButt.setOnClickListener {
                if(available) {
                    nativeSendOpenTreasure(treasureId)

                    available = false
                    updateButt()
                }
            }
        }
    }

    private fun show(treasureId: Int, available: Boolean) {
        activity.runOnUiThread {
            this.treasureId = treasureId
            this.available = available

            updateButt()

            spinnerAdapter = SpinnerAdapter(TreasuresList.list[treasureId])
            prizesListAdapter = PrizesListAdapter(TreasuresList.list[treasureId].shuffled())

            binding.treasureSpinnerRecycle.layoutManager = LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false)

            // Сам спинер
            binding.treasureSpinnerRecycle.adapter = spinnerAdapter

            // Список призов
            binding.spinPrizesListRecycle.layoutManager = GridLayoutManager(activity, 6)
            binding.spinPrizesListRecycle.adapter = prizesListAdapter
        }
    }

    private fun updateButt() {
        activity.runOnUiThread {
            if(available) {
                binding.treasureOpenButt.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#fbc02d"))
                binding.treasureOpenButt.setTextColor(Color.parseColor("#ffffff"))
            }
            else {
                binding.treasureOpenButt.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#bbbdbb"))
                binding.treasureOpenButt.setTextColor(Color.parseColor("#7d7d7d"))
            }
        }
    }

    private fun openTreasure(winnerId: Int) {

        val targetPosition = winnerId +  TreasuresList.list[treasureId].size * 3

        this.targetPosition = targetPosition;

        val smoothScroller = object : LinearSmoothScroller(activity) {

            override fun getHorizontalSnapPreference(): Int {
                return SNAP_TO_ANY
            }
            override fun getVerticalSnapPreference(): Int {
                return SNAP_TO_START // Выберите предпочтительное положение для прокрутки
            }

            override fun calculateDtToFit(
                viewStart: Int,
                viewEnd: Int,
                boxStart: Int,
                boxEnd: Int,
                snapPreference: Int
            ): Int {
                return boxStart + (boxEnd - boxStart) / 2 - (viewStart + (viewEnd - viewStart) / 2) - 15
            }

            override fun calculateSpeedPerPixel(displayMetrics: DisplayMetrics): Float {
                return super.calculateSpeedPerPixel(displayMetrics) * 10 // Установите желаемую скорость прокрутки
            }
        }

        smoothScroller.targetPosition = targetPosition
        binding.treasureSpinnerRecycle.layoutManager?.startSmoothScroll(smoothScroller)

        binding.treasureSpinnerRecycle.addOnScrollListener(object : RecyclerView.OnScrollListener() {
            override fun onScrollStateChanged(recyclerView: RecyclerView, newState: Int) {
                super.onScrollStateChanged(recyclerView, newState)
                // Конец прокрутки
                if (newState == RecyclerView.SCROLL_STATE_IDLE) {
                    val realPos = targetPosition - TreasuresList.list[treasureId].size * 3
                    val data = TreasuresList.list[treasureId][realPos]
                    nativeShowGiftNotify(data.name, data.rare)
                }
            }
        })
    }

    override fun destroy() {
        super.destroy()

        nativeDestructor()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

}