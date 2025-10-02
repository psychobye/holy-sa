package com.lit.game.gui

import android.content.Context
import android.content.res.ColorStateList
import android.graphics.Color
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity

class KillListItem(
    var killertext: String,
    var deathtext: String,
    var gun: Int,
    var teamColor: Int
) {

}


class DuelsHud {
    private var adapter: KillListAdapter
    var kill_list: RecyclerView
    var duels_kills_left_layout: ConstraintLayout
    var duels_kills_left_text: TextView
    private val topPanel: ConstraintLayout
    private val statePanel: ConstraintLayout
    private val top1NameTextView: TextView
    private val top2NameTextView: TextView
    private val top3NameTextView: TextView
    private val stateKillsCountTextView: TextView
    private val stateDeathsCountTextView: TextView
    external fun init()

    init {
        init()
        kill_list = Samp.activity.findViewById(R.id.kill_list)
        adapter = KillListAdapter(Samp.activity, mutableListOf())
        kill_list.adapter = adapter

        //
        duels_kills_left_layout = Samp.activity.findViewById(R.id.duels_kills_left_layout)
        duels_kills_left_text = Samp.activity.findViewById(R.id.duels_kills_left_text)
        topPanel = Samp.activity.findViewById(R.id.top_panel)
        statePanel = Samp.activity.findViewById(R.id.state_panel)
        top1NameTextView = Samp.activity.findViewById(R.id.top1_name_text)
        top2NameTextView = Samp.activity.findViewById(R.id.top2_name_text)
        top3NameTextView = Samp.activity.findViewById(R.id.top3_name_text)
        stateKillsCountTextView = Samp.activity.findViewById(R.id.state_kills_count_text)
        stateDeathsCountTextView = Samp.activity.findViewById(R.id.state_deaths_count_text)
    }

    fun showKillsLeft(show: Boolean, kills: Int, needKills: Int) {
        if (show) {
            activity.runOnUiThread {
                duels_kills_left_layout.visibility = View.VISIBLE
                duels_kills_left_text.text = String.format("%d/%d", kills, needKills)
            }
        } else {
            activity.runOnUiThread { duels_kills_left_layout.visibility = View.GONE }
        }
    }

    fun addItem(killertext: String, deathtext: String, gun: Int, team: Int) {
        activity.runOnUiThread {
            if (kill_list.visibility == View.GONE) {
                kill_list.visibility = View.VISIBLE
            }
            adapter.addItem(killertext, deathtext, gun, team)
        }
    }

    fun addTop(top1Name: String?, top2Name: String?, top3Name: String?, show: Boolean) {
        if (show) {
            activity.runOnUiThread {
                topPanel.visibility = View.VISIBLE
                top1NameTextView.text = String.format("1. %s", top1Name)
                top2NameTextView.text = String.format("2. %s", top2Name)
                top3NameTextView.text = String.format("3. %s", top3Name)
            }
        } else {
            activity.runOnUiThread { topPanel.visibility = View.GONE }
        }
    }

    fun addStatistic(killsCount: Int, deathsCount: Int, show: Boolean) {
        if (show) {
            activity.runOnUiThread {
                statePanel.visibility = View.VISIBLE
                stateKillsCountTextView.text = killsCount.toString()
                stateDeathsCountTextView.text = deathsCount.toString()
            }
        } else {
            activity.runOnUiThread { statePanel.visibility = View.GONE }
        }
    }

    fun clearKillList() {
        activity.runOnUiThread {
            kill_list.visibility = View.GONE
            adapter.clearItems()
        }
    }

    inner class KillListAdapter internal constructor(context: Context?, var list: MutableList<KillListItem>) : RecyclerView.Adapter<KillListAdapter.ViewHolder>() {
        val inflater: LayoutInflater


        init {
            inflater = LayoutInflater.from(context)
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            val view = inflater.inflate(R.layout.itemkilllist, parent, false)
            return ViewHolder(view)
        }

        override fun onBindViewHolder(holder: ViewHolder, position: Int) {

            holder.kill_list_killer_textview.text = list[position].killertext
            holder.kill_list_death_textview.text = list[position].deathtext
            holder.kill_list_bg.backgroundTintList = ColorStateList.valueOf(list[position].teamColor)

            val gunId = list[position].gun.toString()
            val resourceName = KILL_LIST_ICON_RESOURCE_NAME + gunId
            holder.kill_list_gun_icon.setBackgroundResource(getResId(resourceName))
        }

        fun getResId(res: String?): Int {
            return Samp.activity.resources.getIdentifier(res, "drawable", Samp.activity.packageName)
        }

        override fun getItemCount(): Int {
            return list.size
        }

        inner class ViewHolder internal constructor(view: View) : RecyclerView.ViewHolder(view) {
            val kill_list_killer_textview: TextView
            val kill_list_death_textview: TextView
            val kill_list_gun_icon: ImageView
            val kill_list_bg: ConstraintLayout

            init {
                kill_list_bg = view.findViewById(R.id.kill_list_bg)
                kill_list_killer_textview = view.findViewById(R.id.kill_list_killer_textview)
                kill_list_death_textview = view.findViewById(R.id.kill_list_death_textview)
                kill_list_gun_icon = view.findViewById(R.id.kill_list_gun_icon)
            }
        }

        fun clearItems() {
            list.clear()
            notifyDataSetChanged()
        }

        fun addItem(killer: String, death: String, gun: Int, team: Int) {
            activity.runOnUiThread {

                val parsedColor = when (team) {
                    1 -> Color.parseColor("#e53935")
                    2 -> Color.parseColor("#165A7A")
                    else -> Color.parseColor("#FF282A2C")
                }

                list.add(KillListItem(killer, death, gun, parsedColor))
                notifyItemInserted(list.size - 1)

                if (list.size > 3) {
                    list.removeAt(0)
                    notifyItemRemoved(0)
                }
            }
        }
    }

    companion object {
        private const val KILL_LIST_ICON_RESOURCE_NAME = "kill_list_icon_weapon_"
    }
}