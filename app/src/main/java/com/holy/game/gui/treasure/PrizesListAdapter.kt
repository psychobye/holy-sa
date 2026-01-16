package com.holy.game.gui.treasure

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.RecyclerView
import com.holy.game.EntitySnaps
import com.holy.game.R
import com.holy.data.Rare

class PrizesListAdapter (private val list: List<TreasureItem>) : RecyclerView.Adapter<PrizesListAdapter.ViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.treasure_prizelist_item, parent, false)
        return ViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {

        if(list[position].snap != null) {
            EntitySnaps.loadEntitySnapToImageView(list[position].snap!!, holder.spinBoxItemImage)
        }
        else {
            holder.spinBoxItemImage.setImageResource(list[position].imgResource)
        }

        holder.spinBoxItemName.text = list[position].name
        holder.spinBoxItemNameBg.backgroundTintList = Rare.getColorAsStateList(list[position].rare)
    }

    override fun getItemCount(): Int {
        return list.size
    }

    inner class ViewHolder (itemView: View) : RecyclerView.ViewHolder(itemView) {
        var spinBoxItemName     = itemView.findViewById<TextView>(R.id.spinBoxItemName)
        var spinBoxItemImage    = itemView.findViewById<ImageView>(R.id.spinBoxItemImage)
        val spinBoxItemNameBg   = itemView.findViewById<ConstraintLayout>(R.id.spinBoxItemNameBg)
    }
}