package com.holy.game.gui.battle_pass

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.holy.game.R
import com.holy.game.databinding.BattlepassMainItemBinding
import com.holy.game.gui.util.Utils
import com.holy.data.ItemHelper

interface BattlePassMainAdapterListener {
    fun BattlePassMainAdapter_OnClickGive(pos: Int)
}

class BattlePassMainAdapter(val listener: BattlePassMainAdapterListener)
    : RecyclerView.Adapter<BattlePassMainAdapter.ViewHolder>() {

    private var itemWidth = 0
    private var itemHeight = 0
    var list: HashMap<Int, BattlePassMainItem> = hashMapOf()

    fun setItemHeight(height: Int) {
        println("setItemsSize $height")

        itemHeight  = height
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val binding = BattlepassMainItemBinding.inflate(LayoutInflater.from(parent.context), parent, false)

        binding.root.layoutParams.width = (Utils.screenSize.first * 0.165).toInt()
//        binding.root.layoutParams.height = (Utils.screenSize.second * 0.541).toInt()

//        binding.root.layoutParams.width = itemWidth
//        binding.root.layoutParams.height = itemHeight

        return ViewHolder(binding)
    }

    override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
        val item = list[pos]!!
        val binding = holder.binding

        when(item.status) {
            BattlePassItemStatus.AVAILABLE_COMPLETE.ordinal -> {
                binding.itemFull.alpha = 1f
                binding.giveButton.visibility = View.VISIBLE
                binding.statusIcon.setImageResource(0)
            }

            BattlePassItemStatus.COMPLETED.ordinal -> {
                binding.itemFull.alpha = 1f
                binding.giveButton.visibility = View.GONE
                binding.statusIcon.setImageResource(R.drawable.battlepass_main_item_completed_icon)
            }

            BattlePassItemStatus.LOCKED.ordinal -> {
                binding.itemFull.alpha = 0.5f
                binding.giveButton.visibility = View.GONE
                binding.statusIcon.setImageResource(R.drawable.battlepass_main_item_lock_icon)
            }
        }

        binding.root.layoutParams.height = itemHeight

        binding.level.text = String.format("%d", pos + 1)
        binding.name.text = item.name
        binding.image.backgroundTintList = item.rareColor

        ItemHelper.loadSpriteToImageView_new(item.sprite, binding.image)
    }

    override fun getItemCount(): Int {
        return list.size
    }


    inner class ViewHolder(val binding: BattlepassMainItemBinding) : RecyclerView.ViewHolder(binding.root) {

        init {
            binding.giveButton.setOnClickListener {
                val pos = this.bindingAdapterPosition
                listener.BattlePassMainAdapter_OnClickGive(pos)
            }
//
//            binding.giveButtonPrem.setOnClickListener {
//                val pos = this.bindingAdapterPosition
//                listener.BattlePassMainListener_OnClickGivePrem(pos)
//            }
//
//            binding.buyLvlButton.setOnClickListener {
//                listener.BattlePassMainListener_OnClickBuyIncLvl()
//            }
        }
    }
}