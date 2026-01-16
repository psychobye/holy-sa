package com.lit.game.gui.tuning

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.game.core.Samp

interface TuningAdapterListener {
    fun onClickItem(pos: Int)
}

class TuningAdapter(val list: List<TuningItem>, val listener: TuningAdapterListener) : RecyclerView.Adapter<TuningAdapter.ViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.tuning_item, parent, false)
        return ViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
        holder.caption.text = list[pos].caption
        holder.priceText.text = String.format("от %s руб.", Samp.formatter.format(list[pos].price))
        holder.icon.setImageResource(list[pos].icon)
    }

    fun updatePrices(prices: IntArray) {
        for(i in prices.indices)
            list[i].price = prices[i]
    }

    override fun getItemCount(): Int {
        return list.size
    }

    inner class ViewHolder (itemView: View) : RecyclerView.ViewHolder(itemView) {
        var caption     = itemView.findViewById<TextView>(R.id.caption)
        var priceText   = itemView.findViewById<TextView>(R.id.priceText)
        var icon        = itemView.findViewById<ImageView>(R.id.icon)

        init {
            itemView.findViewById<ConstraintLayout>(R.id.tuningItem).setOnClickListener {
                val pos = this.bindingAdapterPosition

                if(list[pos].customId != -1)
                    listener.onClickItem(list[pos].customId)
                else
                    listener.onClickItem(pos)
            }
        }
    }
}