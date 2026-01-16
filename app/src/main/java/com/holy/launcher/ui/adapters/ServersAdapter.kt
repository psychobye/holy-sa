package com.holy.launcher.ui.adapters

import android.app.Activity
import android.graphics.Color
import android.graphics.PorterDuff
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.launcher.async.dto.response.Servers
import com.holy.launcher.domain.enums.StorageElements
import com.holy.launcher.service.ActivityService
import com.holy.launcher.service.impl.ActivityServiceImpl
import com.holy.launcher.storage.NativeStorage
import com.holy.launcher.storage.Storage

class ServersAdapter(context: Activity, servers: List<Servers>) : RecyclerView.Adapter<ServersAdapter.ServersViewHolder>() {
    private val context: Activity
    private val servers: List<Servers>
    private var selectedItem = 0
    private var activityService: com.holy.launcher.service.ActivityService? = null

    init {
        activityService = com.holy.launcher.service.impl.ActivityServiceImpl()
    }

    init {
        this.context = context
        this.servers = servers
        try {
            selectedItem = com.holy.launcher.storage.NativeStorage.getClientProperty("server", context).toInt()
            com.holy.launcher.storage.NativeStorage.addClientProperty("ip", servers[selectedItem].ip, context)
            com.holy.launcher.storage.NativeStorage.addClientProperty("port", servers[selectedItem].port.toString(), context)
        } catch (e: Exception) {
            // e.printStackTrace();
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ServersViewHolder {
        val v = LayoutInflater.from(context).inflate(R.layout.item_server, parent, false)
        return ServersViewHolder(v)
    }

    override fun onBindViewHolder(holder: ServersViewHolder, position: Int) {
        val server = servers[position]
        val MainColor = Color.parseColor(server.color)
        if (server.lock == 1) {
            holder.bearPaw.setImageResource(R.drawable.server_lock_icon)
        }
        else {
            holder.bearPaw.setImageResource(R.drawable.bear_paw)
        }
        //	int LightColor = Color.parseColor(servers.getColor());
//        when(server.online) {
//            in 0..400 -> {
//                holder.people.setColorFilter(Color.parseColor("#ffffff"), PorterDuff.Mode.SRC_ATOP)
//                holder.textonline.text = "Низкая загруженность"
//            }
//            in 400 .. 700 -> {
//                holder.people.setColorFilter(Color.parseColor("#f59b33"), PorterDuff.Mode.SRC_ATOP)
//                holder.textonline.text = "Средняя загруженность"
//            }
//            else -> {
//                holder.people.setColorFilter(Color.parseColor("#f53333"), PorterDuff.Mode.SRC_ATOP)
//                holder.textonline.text = "Высокая загруженность"
//            }
//        }
        holder.bearPaw.setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP)
        holder.people.setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP)
        holder.backColor.background.setColorFilter(MainColor, PorterDuff.Mode.SRC_ATOP)
        holder.name.text = server.name
        holder.name.setTextColor(MainColor)
        holder.mult.text = server.mult
        holder.textonline.text = Integer.toString(server.online)
        holder.textmaxonline.text = "/1200"
        if (selectedItem == position) {
            holder.container.scaleX = 1.05f
            holder.container.scaleY = 1.05f
            holder.backColor.alpha = 0.60f

            //todo данная штука сделана для обратной совместимости, удалить через месяц - два
            saveServerInfoToStorage(server)
        }
        else {
            holder.container.scaleX = 1.0f
            holder.container.scaleY = 1.0f
            holder.backColor.alpha = 0.35f
        }
        holder.container.setOnClickListener {
            selectedItem = position
            notifyDataSetChanged()
            com.holy.launcher.storage.NativeStorage.addClientProperty("server", server.serverID, context)
            com.holy.launcher.storage.NativeStorage.addClientProperty("ip", server.ip, context)
            com.holy.launcher.storage.NativeStorage.addClientProperty("port", server.port.toString(), context)
            saveServerInfoToStorage(server)
            activityService!!.showInfoMessage("Сервер выбран! Для начала игры нажмите жёлтую кнопку", context)
        }
    }

    private fun saveServerInfoToStorage(servers: Servers) {
        com.holy.launcher.storage.Storage.addProperty(com.holy.launcher.domain.enums.StorageElements.SERVER_MULTI, servers.mult, context)
        com.holy.launcher.storage.Storage.addProperty(com.holy.launcher.domain.enums.StorageElements.SERVER_COLOR, servers.color, context)
        com.holy.launcher.storage.Storage.addProperty(com.holy.launcher.domain.enums.StorageElements.SERVER_NAME, servers.name, context)
        com.holy.launcher.storage.Storage.addProperty(com.holy.launcher.domain.enums.StorageElements.SERVER_LOCKED, servers.lock.toString(), context)
    }

    override fun getItemCount(): Int {
        return servers.size
    }

    class ServersViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        @JvmField
		var container: FrameLayout
        @JvmField
		var bearPaw: ImageView
        @JvmField
		var people: ImageView
        @JvmField
		var name: TextView
        @JvmField
		var mult: TextView
        @JvmField
		var textonline: TextView
        @JvmField
		var textmaxonline: TextView
        @JvmField
		var backColor: LinearLayout

        init {
            name = itemView.findViewById(R.id.firstName)
            mult = itemView.findViewById(R.id.secondName)
            bearPaw = itemView.findViewById(R.id.bearPaw)
            people = itemView.findViewById(R.id.people)
            textonline = itemView.findViewById(R.id.online)
            textmaxonline = itemView.findViewById(R.id.onlineTotal)
            backColor = itemView.findViewById(R.id.backColor)
            container = itemView.findViewById(R.id.container)
        }
    }
}