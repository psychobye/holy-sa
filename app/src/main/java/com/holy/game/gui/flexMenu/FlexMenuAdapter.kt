package com.holy.game.gui.flexMenu

import android.annotation.SuppressLint
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.holy.game.R
import com.holy.game.databinding.FlexTabItemBinding

class FlexMenuAdapter(
    private val onClick: (FlexMenuItem) -> Unit
) : RecyclerView.Adapter<FlexMenuAdapter.VH>() {

    private val items = ArrayList<FlexMenuItem>()

    @SuppressLint("NotifyDataSetChanged")
    fun setItems(newItems: List<FlexMenuItem>) {
        items.clear()
        items.addAll(newItems)
        notifyDataSetChanged()
    }

    fun add(item: FlexMenuItem) {
        items.add(item)
        notifyItemInserted(items.lastIndex)
    }

    @SuppressLint("NotifyDataSetChanged")
    fun clear() {
        items.clear()
        notifyDataSetChanged()
    }

    fun setActiveById(id: Int) {
        val changed = mutableListOf<Int>()
        for ((i, it) in items.withIndex()) {
            val shouldBeActive = it.id == id
            if (it.isActive != shouldBeActive) {
                it.isActive = shouldBeActive
                changed.add(i)
            }
        }
        changed.forEach { notifyItemChanged(it) }
    }

    inner class VH(private val b: FlexTabItemBinding) : RecyclerView.ViewHolder(b.root) {
        fun bind(position: Int) {
            val itm = items[position]

            b.btnTitle.text = itm.title

            val bgRes = if (itm.isActive) R.drawable.btn_flex_on else R.drawable.btn_flex_off
            b.btnBg.setImageResource(bgRes)

            if (itm.iconRes != null && itm.iconRes != 0) {
                b.btnIcon.visibility = View.VISIBLE
                b.btnIcon.setImageResource(itm.iconRes)
            } else {
                b.btnIcon.visibility = View.GONE
            }

            b.root.setOnClickListener setOnClick@{
                val pos = bindingAdapterPosition
                if (pos == RecyclerView.NO_POSITION) return@setOnClick
                val current = items[pos]

                setActiveById(current.id)
                onClick(current)
            }
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): VH {
        val inflater = LayoutInflater.from(parent.context)
        val binding = FlexTabItemBinding.inflate(inflater, parent, false)
        return VH(binding)
    }

    override fun onBindViewHolder(holder: VH, position: Int) = holder.bind(position)

    override fun getItemCount() = items.size
}