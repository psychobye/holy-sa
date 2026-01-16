package com.holy.game.gui.donate

import android.annotation.SuppressLint
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.RecyclerView
import com.google.android.material.button.MaterialButton
import com.holy.game.EntitySnaps
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity

class DonateAdapter internal constructor(var donate: Donate) : RecyclerView.Adapter<DonateAdapter.ViewHolder>() {
    var selected_item = -1
    var items: MutableList<DonateItem> = mutableListOf()
    val mInflater = LayoutInflater.from(activity)

    // inflates the cell layout from xml when needed
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = mInflater.inflate(R.layout.donate_cell, parent, false)
        return ViewHolder(view)
    }

    // binds the data to the TextView in each cell
    @SuppressLint("DefaultLocale")
    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        if (selected_item == position) {
            if (items[position].sqlId != 0) {
                holder.donate_cell_my_buttons_layout.visibility = View.VISIBLE
                holder.donate_cell_buttons_layout.visibility = View.GONE

            }
            else {
                holder.donate_cell_buttons_layout.visibility = View.VISIBLE
                holder.donate_cell_my_buttons_layout.visibility = View.GONE
            }
        }
        else {
            holder.donate_cell_my_buttons_layout.visibility     = View.GONE
            holder.donate_cell_buttons_layout.visibility        = View.GONE
        }
        holder.donate_cell_price_text.text = String.format("%s", Samp.formatter.format(items[position].price.toLong()))

        if(items[position].snap != null)
            EntitySnaps.loadEntitySnapToImageView(items[position].snap!!, holder.donate_cell_img)
        else
            holder.donate_cell_img.setImageResource(items[position].imgRecourse)

      //  holder.donate_cell_img.setImageBitmap(EntitySnaps.getDrawable(ENTITY_TYPE_VEHICLE, 411))
      //  EntitySnaps.loadEntitySnapToImageView(TYPE_VEHICLE, 411, holder.donate_cell_img)
        //Vehicles.getSnap(holder.donate_cell_img)
        holder.donate_cell_name.text = items[position].name
    }

    // total number of cells
    override fun getItemCount(): Int {
        return items.size
    }

    // stores and recycles views as they are scrolled off screen
    inner class ViewHolder internal constructor(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var donate_cell_price_text: TextView
        var donate_cell_name: TextView
        var donate_cell_img: ImageView

        // donat butt
        var donate_cell_buttons_layout: ConstraintLayout

        // my butt
        var donate_cell_my_buttons_layout: ConstraintLayout

        init {
            donate_cell_my_buttons_layout = itemView.findViewById(R.id.donate_cell_my_buttons_layout)

            // Отмена
            val donate_cell_my_cancel_butt = itemView.findViewById<MaterialButton>(R.id.donate_cell_my_cancel_butt)
            donate_cell_my_cancel_butt.setOnClickListener {
                val pos = this.bindingAdapterPosition
                selected_item = -1
                notifyItemChanged(pos)
            }

            // Использовать
            val donate_cell_my_use_butt = itemView.findViewById<MaterialButton>(R.id.donate_cell_my_use_butt)
            donate_cell_my_use_butt.setOnClickListener {
                val pos = this.bindingAdapterPosition
                if (items[pos].category == Donate.CATEGORY_TREASURES)
                    donate.native_clickStashItem(Donate.CLICK_ID_TREASURE, items[pos].value)
                else
                    donate.native_clickStashItem(0, items[pos].sqlId)
            }

            // Продать
            val donate_cell_my_sell_butt = itemView.findViewById<MaterialButton>(R.id.donate_cell_my_sell_butt)
            donate_cell_my_sell_butt.setOnClickListener {
                val pos = this.bindingAdapterPosition
                donate.native_clickStashItem(2, items[pos].sqlId)
            }
            /**
             *
             */

            // Купитьт
            val donate_cell_buy_butt = itemView.findViewById<MaterialButton>(R.id.donate_cell_buy_butt)
            donate_cell_buy_butt.setOnClickListener {
                val pos = this.bindingAdapterPosition
                donate.clickBuy(items[pos])
            }

            // Info
            val donate_cell_info_butt = itemView.findViewById<MaterialButton>(R.id.donate_cell_info_butt)
            donate_cell_info_butt.setOnClickListener {
                val pos = this.bindingAdapterPosition
                donate.clickInfo(items[pos])
            }

            // Cancel
            val donate_cell_cancel_butt = itemView.findViewById<MaterialButton>(R.id.donate_cell_cancel_butt)
            donate_cell_cancel_butt.setOnClickListener {
                val pos = selected_item
                selected_item = -1
                notifyItemChanged(pos)
            }
            donate_cell_price_text = itemView.findViewById(R.id.donate_cell_price_text)
            donate_cell_img = itemView.findViewById(R.id.donate_cell_img)
            donate_cell_name = itemView.findViewById(R.id.donate_cell_name)
            donate_cell_buttons_layout = itemView.findViewById(R.id.donate_cell_buttons_layout)


            // Cell select
            itemView.setOnClickListener {
                val pos = this.bindingAdapterPosition
                val olspos = selected_item

                if(selected_item == pos)
                    selected_item = -1
                else
                    selected_item = pos

                notifyItemChanged(olspos)
                notifyItemChanged(pos)
            }
        }
    }
}