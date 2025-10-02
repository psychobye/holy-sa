package com.lit.launcher.ui.fragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.launcher.async.dto.response.News
import com.lit.launcher.async.dto.response.ServersList
import com.lit.launcher.ui.adapters.NewsAdapter
import com.lit.launcher.ui.adapters.ServersAdapter

class MonitoringFragment : Fragment() {
    private var userPer30Min: TextView? = null
    private var recyclerNews: RecyclerView? = null
    private lateinit var newsAdapter: NewsAdapter
    private var news: List<News>? = null
    private var recyclerServers: RecyclerView? = null
    private var serversAdapter: ServersAdapter? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val inflate = inflater.inflate(R.layout.fragment_monitoring, container, false)
        recyclerNews = inflate.findViewById(R.id.newsRV)
        userPer30Min = inflate.findViewById(R.id.userPer30Min)
        recyclerNews?.setHasFixedSize(true) // Используйте ?. для безопасного вызова
        recyclerServers = inflate.findViewById(R.id.ourServersRV)
        recyclerServers?.setHasFixedSize(true) // Используйте ?. для безопасного вызова
        attachNews()
        attachServers()
        return inflate
    }

    private fun attachNews() {
        val layoutManager = LinearLayoutManager(requireActivity(), LinearLayoutManager.HORIZONTAL, false)
        recyclerNews?.layoutManager = layoutManager // Используйте ?. для безопасного вызова
        news = ServersList.news
        newsAdapter = NewsAdapter(requireContext(), news)
        recyclerNews?.adapter = newsAdapter // Используйте ?. для безопасного вызова
    }

    private fun attachServers() {
        val layoutManager = LinearLayoutManager(requireActivity())
        recyclerServers?.layoutManager = layoutManager

        serversAdapter = ServersAdapter(requireActivity(), ServersList.servers)
        recyclerServers?.adapter = serversAdapter
        if (ServersList.servers.isNotEmpty()) {
            val players = ServersList.servers[0].onlinePer30Min;
            userPer30Min?.setText(String.format("%d игроков", players));
        } else {
            userPer30Min?.setText("Нет данных о игроках");
        }

    }
}