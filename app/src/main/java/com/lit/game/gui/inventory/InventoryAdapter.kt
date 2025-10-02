package com.lit.game.gui.inventory

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.game.databinding.InventoryItemBinding
import com.lit.data.ItemHelper

interface InventoryListener {
    fun onSelectedItem(matrixId: Int, itemId: Int)
}

class InventoryAdapter (
    private val matrixId: Int,
    val list: List<InventoryItem>,
    private val listener: InventoryListener
) : RecyclerView.Adapter<InventoryAdapter.ViewHolder>() {

    var selectedItemId: Int = -1

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val binding = InventoryItemBinding.inflate(LayoutInflater.from(parent.context), parent, false)
        return ViewHolder(binding)
    }

    override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
        val item = list[pos]
        val binding = holder.binding

        binding.image.backgroundTintList = item.rareColor
        binding.caption.text = item.caption

        if (selectedItemId == pos)
            binding.itemLayout.setBackgroundResource(R.drawable.inv_bg_shape_active)
        else
            binding.itemLayout.setBackgroundResource(R.drawable.inv_bg_shape)

        if(item.count.isNotEmpty()) {
            binding.valueBg.visibility = View.VISIBLE
            binding.valueText.text = item.count
        }
        else {
            binding.valueBg.visibility = View.GONE
        }

        ItemHelper.loadSpriteToImageView_new(item.sprite, binding.image)
    }

    override fun getItemCount(): Int {
        return list.size
    }

    inner class ViewHolder (val binding: InventoryItemBinding) : RecyclerView.ViewHolder(binding.root) {
        init {
            itemView.setOnClickListener {
                val pos = this.bindingAdapterPosition

                listener.onSelectedItem(matrixId, pos)
            }
        }
    }
}