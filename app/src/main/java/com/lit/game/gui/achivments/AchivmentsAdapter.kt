package com.lit.game.gui.achivments

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.RecyclerView
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.skydoves.progressview.ProgressView

interface AchivmentsListener {
    fun onGive(index: Int)
}

class AchivmentsAdapter (val list: List<Achivment>, val listener: AchivmentsListener) : RecyclerView.Adapter<AchivmentsAdapter.ViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val itemView = LayoutInflater.from(parent.context).inflate(R.layout.achivments_item, parent, false)
        return ViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val curStep = list[position].curStep
        var index = curStep
        if(index >= 3) index = 2

        holder.achivmentsItemCaption.text   = list[position].caption
        holder.achivmentsItemText.text      = list[position].desription

        holder.achivmentsItemReward.text = String.format("%d LC", list[position].rewards[index])

        if(list[position].isNeedGiveButt()) {
            holder.achivmentsItemProgress.visibility        = View.GONE
            holder.achivmentsItemProgressText.visibility    = View.GONE

            holder.achivmentsItemGiveButt.visibility = View.VISIBLE
        }
        else {
            holder.achivmentsItemGiveButt.visibility = View.GONE

            holder.achivmentsItemProgress.visibility        = View.VISIBLE
            holder.achivmentsItemProgressText.visibility    = View.VISIBLE

            holder.achivmentsItemProgress.max = list[position].steps[index].toFloat()
            holder.achivmentsItemProgress.progress = list[position].progress.toFloat()

            holder.achivmentsItemProgressText.text = String.format("%d/%d", list[position].progress, list[position].steps[index])
        }

        // звездочки
        if(curStep >= 1)
            holder.achivmentsItemStar1.setImageResource(R.drawable.achivments_item_star_active)
        else
            holder.achivmentsItemStar1.setImageResource(R.drawable.achivments_item_star)

        if(curStep >= 2)
            holder.achivmentsItemStar2.setImageResource(R.drawable.achivments_item_star_active)
        else
            holder.achivmentsItemStar2.setImageResource(R.drawable.achivments_item_star)

        if(curStep >= 3) {
            holder.achivmentsItemStar3.setImageResource(R.drawable.achivments_item_star_active)
            holder.achivmentsItemFullLayout.visibility = View.VISIBLE
        }
        else {
            holder.achivmentsItemStar3.setImageResource(R.drawable.achivments_item_star)
            holder.achivmentsItemFullLayout.visibility = View.GONE
        }
    }

    override fun getItemCount(): Int {
        return list.size
    }

    inner class ViewHolder (itemView: View) : RecyclerView.ViewHolder(itemView) {
        var achivmentsItemCaption       = itemView.findViewById<TextView>(R.id.achivmentsItemCaption)
        var achivmentsItemText          = itemView.findViewById<TextView>(R.id.achivmentsItemText)
        var achivmentsItemProgress      = itemView.findViewById<ProgressView>(R.id.achivmentsItemProgress)
        var achivmentsItemProgressText  = itemView.findViewById<TextView>(R.id.achivmentsItemProgressText)
        var achivmentsItemReward        = itemView.findViewById<TextView>(R.id.achivmentsItemReward)
        var achivmentsItemGiveButt      = itemView.findViewById<MaterialButton>(R.id.achivmentsItemGiveButt)
        var achivmentsItemFullLayout    = itemView.findViewById<ConstraintLayout>(R.id.achivmentsItemFullLayout)

        var achivmentsItemStar1         = itemView.findViewById<ImageView>(R.id.achivmentsItemStar1)
        var achivmentsItemStar2         = itemView.findViewById<ImageView>(R.id.achivmentsItemStar2)
        var achivmentsItemStar3         = itemView.findViewById<ImageView>(R.id.achivmentsItemStar3)

        init {
            itemView.findViewById<MaterialButton>(R.id.achivmentsItemGiveButt).setOnClickListener {
                val pos = this.bindingAdapterPosition
                listener.onGive(pos)
            }
        }
    }
}