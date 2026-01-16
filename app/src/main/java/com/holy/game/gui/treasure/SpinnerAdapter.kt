package com.holy.game.gui.treasure

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.EntitySnaps
import com.lit.game.R

class SpinnerAdapter (private val items: List<TreasureItem>) :
    RecyclerView.Adapter<SpinnerAdapter.ViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.spin_box_item, parent, false)
        return ViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val actualPosition = position % items.size
        if(items[actualPosition].snap != null) {
            EntitySnaps.loadEntitySnapToImageView(items[actualPosition].snap!!, holder.spinBoxItemImage)
        }
        else {
            holder.spinBoxItemImage.setImageResource(items[actualPosition].imgResource)
        }
        holder.spinBoxItemName.text = items[actualPosition].name
    }

    // total number of cells
    override fun getItemCount(): Int {
        return Integer.MAX_VALUE;
    }

    inner class ViewHolder (itemView: View) : RecyclerView.ViewHolder(itemView) {
        var spinBoxItemName: TextView = itemView.findViewById(R.id.spinBoxItemName)
        var spinBoxItemImage: ImageView = itemView.findViewById(R.id.spinBoxItemImage)

    }
}