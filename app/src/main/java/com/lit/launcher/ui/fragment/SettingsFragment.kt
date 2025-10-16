package com.lit.launcher.ui.fragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.lit.launcher.domain.data.SettingsItem
import com.lit.launcher.ui.adapters.SettingsAdapter

class SettingsFragment : Fragment() {

    private var closeBtn: ImageView? = null
    private var version: TextView? = null
    private lateinit var recyclerView: RecyclerView
    private lateinit var adapter: SettingsAdapter

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_settings, container, false)

        closeBtn = view.findViewById(R.id.close_btn)
        version = view.findViewById(R.id.version)
        recyclerView = view.findViewById(R.id.settingsRecyclerView)

        val versionName = requireContext().packageManager
            .getPackageInfo(requireContext().packageName, 0).versionName
        version?.text = getString(R.string.version, versionName) + " "

        initRecycler()
        onClickInit()
        return view
    }

    private fun initRecycler() {
        val items = listOf(
            SettingsItem("ОТОБРАЖАТЬ ФПС", "debug"),
            SettingsItem("УВЕДОМЛЕНИЯ", "androidKeyboard")
        )

        adapter = SettingsAdapter(items, requireContext())
        recyclerView.layoutManager = LinearLayoutManager(requireContext())
        recyclerView.adapter = adapter
    }

    private fun onClickInit() {
        closeBtn?.setOnClickListener {
            replaceFragment(MainFragment())
        }
    }

    private fun Fragment.replaceFragment(fragment: Fragment, addToBackStack: Boolean = true) {
        if (!isAdded) return
        parentFragmentManager.beginTransaction().apply {
            setCustomAnimations(
                android.R.anim.fade_in,
                android.R.anim.fade_out,
                android.R.anim.fade_in,
                android.R.anim.fade_out
            )
            replace(R.id.fragment_container, fragment)
            if (addToBackStack) addToBackStack(null)
            commit()
        }
    }
}