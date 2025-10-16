package com.lit.launcher.ui.adapters

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Switch
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.launcher.domain.data.SettingsItem
import com.lit.launcher.storage.NativeStorage

class SettingsAdapter(
    private val items: List<SettingsItem>,
    private val context: Context
) : RecyclerView.Adapter<SettingsAdapter.ViewHolder>() {

    inner class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val title: TextView = view.findViewById(R.id.itemText)
        val switch: Switch = view.findViewById(R.id.itemSwitch)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_settings, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val item = items[position]
        holder.title.text = item.title

        val value = NativeStorage.getClientProperty(item.key, context)
        holder.switch.isChecked = value == "1"

        holder.switch.setOnCheckedChangeListener { _, isChecked ->
            NativeStorage.addClientProperty(item.key, if (isChecked) "1" else "0", context)
        }
    }

    override fun getItemCount(): Int = items.size
}