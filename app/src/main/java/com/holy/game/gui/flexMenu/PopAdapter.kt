package com.holy.game.gui.flexMenu

import android.annotation.SuppressLint
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.game.databinding.PopItemButtonBinding

class PopAdapter(
    private val onClick: (PopItem) -> Unit
) : RecyclerView.Adapter<PopAdapter.VH>() {

    private val items = ArrayList<PopItem>()

    @SuppressLint("NotifyDataSetChanged")
    fun setItems(newItems: List<PopItem>) {
        items.clear()
        items.addAll(newItems)
        notifyDataSetChanged()
    }

    @SuppressLint("NotifyDataSetChanged")
    fun clear() {
        items.clear()
        notifyDataSetChanged()
    }

    inner class VH(private val b: PopItemButtonBinding) : RecyclerView.ViewHolder(b.root) {
        fun bind(position: Int) {
            val item = items[position]

            val bgRes = if (item.isActive) R.drawable.btn_pop_on else R.drawable.btn_pop_off
            b.btnBg.setImageResource(bgRes)

            val activeColor = ContextCompat.getColor(b.root.context, R.color.white)
            val inactiveColor = ContextCompat.getColor(b.root.context, R.color.gray_BF)
            val color = if (item.isActive) activeColor else inactiveColor

            if (item.iconRes != null) {
                b.btnIcon.visibility = View.VISIBLE
                b.btnIcon.setImageResource(item.iconRes)
                b.btnIcon.setColorFilter(color)
                b.btnIconText.visibility = View.GONE
            } else if (!item.textInstead.isNullOrEmpty()) {
                b.btnIcon.visibility = View.GONE
                b.btnIconText.visibility = View.VISIBLE
                b.btnIconText.text = item.textInstead
                b.btnIconText.setTextColor(color)
            } else {
                b.btnIcon.visibility = View.GONE
                b.btnIconText.visibility = View.GONE
            }

            b.root.setOnClickListener setOnClick@{
                val pos = bindingAdapterPosition
                if (pos == RecyclerView.NO_POSITION) return@setOnClick
                val current = items[pos]
                onClick(current)
            }
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): VH {
        val inflater = LayoutInflater.from(parent.context)
        val binding = PopItemButtonBinding.inflate(inflater, parent, false)
        return VH(binding)
    }

    override fun onBindViewHolder(holder: VH, position: Int) = holder.bind(position)

    override fun getItemCount() = items.size
}