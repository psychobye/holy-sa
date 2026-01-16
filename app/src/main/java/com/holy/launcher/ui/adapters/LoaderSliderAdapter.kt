package com.holy.launcher.ui.adapters

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R

class LoaderSliderAdapter(private val sliderItems: List<String>) : RecyclerView.Adapter<LoaderSliderViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): LoaderSliderViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.loader_slider_item_data, parent, false)

        return LoaderSliderViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: LoaderSliderViewHolder, position: Int) {
        holder.text.text = sliderItems[position]
    }

    override fun getItemCount(): Int {
        return sliderItems.size
    }
}

class LoaderSliderViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
    val text: TextView = itemView.findViewById(R.id.loader_slider_item_data_text)

}