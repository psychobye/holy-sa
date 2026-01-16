package com.holy.game.gui.battle_pass.tasks

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.holy.game.databinding.BattlepassTaskItemBinding
import com.holy.game.gui.util.Utils

interface BattlePassTasksAdapterListener {
    fun BattlePassTasksAdapter_OnClickRefresh(pos: Int)
}

class BattlePassTasksAdapter(var listener: BattlePassTasksAdapterListener)
    : RecyclerView.Adapter<BattlePassTasksAdapter.ViewHolder>() {

    var list: HashMap<Int, BattlePassTaskItem> = hashMapOf()

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val binding = BattlepassTaskItemBinding.inflate(LayoutInflater.from(parent.context), parent, false)

        binding.root.layoutParams.width = (Utils.screenSize.first / 2.15 ).toInt()
        binding.root.layoutParams.height = Utils.screenSize.second / 4

        return ViewHolder(binding)
    }

    override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
        val item = list[pos]!!
        val binding = holder.binding

        binding.caption.text = item.caption
        binding.description.text = item.description

        binding.progress.progress = item.curProgress.toFloat()
        binding.progress.max = item.maxProgress.toFloat()

        if(item.curProgress >= item.maxProgress)
            binding.completedLayout.visibility = View.VISIBLE
        else
            binding.completedLayout.visibility = View.GONE

        binding.progressText.text = String.format("%d из %d", item.curProgress, item.maxProgress)
    }

    override fun getItemCount(): Int {
        return list.size
    }


    inner class ViewHolder(val binding: BattlepassTaskItemBinding) : RecyclerView.ViewHolder(binding.root) {

        init {
            binding.refreshButton.setOnClickListener {
                val pos = this.bindingAdapterPosition
                listener.BattlePassTasksAdapter_OnClickRefresh(pos)
            }
        }
    }
}