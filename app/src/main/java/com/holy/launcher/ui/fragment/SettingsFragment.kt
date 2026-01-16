package com.holy.launcher.ui.fragment

import android.content.ActivityNotFoundException
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.lit.game.R
import com.holy.launcher.config.Config
import com.lit.launcher.domain.data.SettingsItem
import com.lit.launcher.ui.adapters.SettingsAdapter
import java.io.File

class SettingsFragment : Fragment() {

    private var closeBtn: ImageView? = null
    private var logBtn: FrameLayout? = null
    private var version: TextView? = null
    private lateinit var recyclerView: RecyclerView
    private lateinit var adapter: SettingsAdapter
    private var notification: LauncherNotification? = null

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_settings, container, false)

        closeBtn = view.findViewById(R.id.close_btn)
        logBtn = view.findViewById(R.id.logBtn)
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
        logBtn?.setOnClickListener {
            val logFile = File(com.holy.launcher.config.Config.GAME_PATH, com.holy.launcher.config.Config.LOG_FILE_PATH)
            val crashLogFile = File(com.holy.launcher.config.Config.GAME_PATH, com.holy.launcher.config.Config.CRASH_LOG_FILE_PATH)

            if (!logFile.exists() && !crashLogFile.exists()) {
                notification?.showNotification(
                    type = 0,
                    text = "Логи не найдены",
                    duration = 5,
                    actionId = 0,
                    butt1 = "",
                    butt2 = ""
                )
                return@setOnClickListener
            }

            val filesToSend = mutableListOf<File>()
            if (logFile.exists()) filesToSend.add(logFile)
            if (crashLogFile.exists()) filesToSend.add(crashLogFile)

            val uris = ArrayList<android.net.Uri>()
            filesToSend.forEach { file ->
                uris.add(
                    androidx.core.content.FileProvider.getUriForFile(
                        requireContext(),
                        requireContext().packageName + ".provider",
                        file
                    )
                )
            }

            val emailIntent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
                type = "message/rfc822"
                putExtra(Intent.EXTRA_EMAIL, arrayOf(com.holy.launcher.config.Config.SUPPORT_MAIL))
                putExtra(Intent.EXTRA_SUBJECT, "logs")
                putExtra(Intent.EXTRA_TEXT, getString(R.string.logShare))
                putParcelableArrayListExtra(Intent.EXTRA_STREAM, uris)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                setPackage("com.google.android.gm")
            }

            try {
                startActivity(emailIntent)
            } catch (e: ActivityNotFoundException) {
                notification?.showNotification(
                    type = 0,
                    text = "Gmail не установлен",
                    duration = 5,
                    actionId = 0,
                    butt1 = "",
                    butt2 = ""
                )
            }
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