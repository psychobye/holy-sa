package com.holy.game.gui.achivments

import androidx.recyclerview.widget.GridLayoutManager
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.AchivmentsBinding
import com.holy.game.gui.NativeGui
import com.holy.data.AchivmentsList


class Achivments : AchivmentsListener, NativeGui<AchivmentsBinding>(AchivmentsBinding::class) {

    private var achivmentsAdapter = AchivmentsAdapter(AchivmentsList.list, this)

    private external fun nativeSendGive(index: Int)
    private external fun nativeExit()

    init {
        activity.runOnUiThread {
            binding.achivmentsExitButt.setOnClickListener {
                destroy()
            }

            if(Samp.isTablet)
                binding.achivmentsRecycle.layoutManager = GridLayoutManager(activity, 1)
            else
                binding.achivmentsRecycle.layoutManager = GridLayoutManager(activity, 2)

            binding.achivmentsRecycle.adapter = achivmentsAdapter
        }
    }

    fun updateItem(index: Int, progress: Int, step: Int) {
        activity.runOnUiThread {
            achivmentsAdapter.list[index].progress = progress
            achivmentsAdapter.list[index].curStep  = step

            achivmentsAdapter.notifyItemChanged(index)

            updateProgress()
            updateAvailableReward()
        }
    }

    private fun updateProgress() {
        activity.runOnUiThread {
            val totalAchiv = achivmentsAdapter.list.size
            var completeAchiv = 0

            for (achiv in achivmentsAdapter.list) {
                if (achiv.isAchivmentComplete()) completeAchiv++
            }
            binding.achivmentsProgress.text = String.format("Выполнено: %d из %d", completeAchiv, totalAchiv)
        }
    }

    private fun updateAvailableReward() {
        activity.runOnUiThread {
            var total = 0

            for (achiv in achivmentsAdapter.list) {
                total += achiv.getAvailableReward()
            }

            binding.achivmentsAvailableLc.text = String.format("Доступно: %s LC", Samp.formatter.format(total))
        }
    }

    override fun onGive(index: Int) {
        nativeSendGive(index)
    }

    override fun destroy() {
        super.destroy()

        nativeExit()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}