package com.lit.game.gui.battle_pass.rate

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.databinding.BattlepassRateItemBinding
import com.lit.game.gui.util.Utils


class BattlePassRateAdapter : RecyclerView.Adapter<BattlePassRateAdapter.ViewHolder>() {

    var list: HashMap<Int, BattlePassRateItem> = hashMapOf()

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val binding = BattlepassRateItemBinding.inflate(LayoutInflater.from(parent.context), parent, false)

        binding.root.layoutParams.width = (Utils.screenSize.first * 0.69 ).toInt()

        return ViewHolder(binding)
    }

    override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
        val item = list[pos]!!
        val binding = holder.binding

        when(pos + 1){
            1 -> {
                binding.placeIcon.setImageResource(R.drawable.battlepass_rate_item_place_1_icon)
            }
            2 -> {
                binding.placeIcon.setImageResource(R.drawable.battlepass_rate_item_place_2_icon)
            }
            3 -> {
                binding.placeIcon.setImageResource(R.drawable.battlepass_rate_item_place_3_icon)
            }
            else -> {
                binding.placeIcon.setImageResource(R.drawable.battlepass_rate_item_place_4_icon)
            }
        }

        binding.placeText.text = String.format("%d", pos + 1)
        binding.nick.text = item.nick
        binding.pointsText.text = String.format("%s", Samp.formatter.format(item.points))
    }

    override fun getItemCount(): Int {
        return list.size
    }


    inner class ViewHolder(val binding: BattlepassRateItemBinding) : RecyclerView.ViewHolder(binding.root) {

    }
}