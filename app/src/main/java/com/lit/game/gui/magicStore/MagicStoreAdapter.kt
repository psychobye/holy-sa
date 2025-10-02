package com.lit.game.gui.magicStore

import android.content.res.ColorStateList
import android.graphics.Color
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.EntitySnaps
import com.lit.game.R
import com.lit.game.core.Samp

interface MagicStoreListener {
    fun onToggleButtons(toggle: Boolean)
}

class MagicStoreAdapter (var list: MutableList<MagicStoreItem>, val listener: MagicStoreListener) : RecyclerView.Adapter<MagicStoreAdapter.ViewHolder>() {
    var selectedItem = -1

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.magicstore_item, parent, false)
        return ViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
        val snap = list[pos].snap
        if(snap != null)
            EntitySnaps.loadEntitySnapToImageView(snap, holder.itemImg)
        else
            holder.itemImg.setImageResource(list[pos].res)

        holder.itemText.text = list[pos].name
        holder.priceText.text = String.format("%s", Samp.formatter.format(list[pos].price))

        if(list[pos].priceType == MagicStoreItems.PRICE_BRONZE) {
            holder.priceLayout.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#b87333"))
            holder.priceIcon.setImageResource(R.drawable.magicstore_bronze_icon)
        }

        if(list[pos].priceType == MagicStoreItems.PRICE_SILVER) {
            holder.priceLayout.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#c0c0c0"))
            holder.priceIcon.setImageResource(R.drawable.magicstore_silver_icon)
        }

        if(list[pos].priceType == MagicStoreItems.PRICE_GOLD) {
            holder.priceLayout.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#FFD700"))
            holder.priceIcon.setImageResource(R.drawable.magicstore_gold_icon)
        }

        if(pos == selectedItem) {
           // holder.priceLayout.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#DD2828"))
            holder.cornersLayout.setBackgroundResource(R.drawable.magicstore_item_bg_corner_active)
        }
        else {
            //holder.priceLayout.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#BDBDBD"))
            holder.cornersLayout.setBackgroundResource(R.drawable.magicstore_item_bg_corner)
        }
    }

    override fun getItemCount(): Int {
        return list.size
    }

    inner class ViewHolder (itemView: View) : RecyclerView.ViewHolder(itemView) {
        val itemImg         = itemView.findViewById<ImageView>(R.id.itemImg)
        val itemText        = itemView.findViewById<TextView>(R.id.itemText)
        val priceText       = itemView.findViewById<TextView>(R.id.priceText)
        val cornersLayout   = itemView.findViewById<ImageView>(R.id.cornersLayout)
        val priceLayout     = itemView.findViewById<ConstraintLayout>(R.id.priceLayout)
        val priceIcon       = itemView.findViewById<ImageView>(R.id.priceIcon)

        init {
            itemView.setOnClickListener {
                val pos = this.bindingAdapterPosition
                val oldPos = selectedItem

                notifyItemChanged(oldPos)
                notifyItemChanged(pos)
                selectedItem = pos

                if(pos == oldPos) {
                    selectedItem = -1
                    listener.onToggleButtons(false)
                    return@setOnClickListener
                }
                listener.onToggleButtons(true)

            }
        }
    }
}