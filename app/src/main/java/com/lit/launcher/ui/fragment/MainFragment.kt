package com.lit.launcher.ui.fragment

import android.content.Context
import android.content.Intent
import android.graphics.drawable.Drawable
import android.net.Uri
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputMethodManager
import android.widget.EditText
import android.widget.FrameLayout
import android.widget.ImageView
import androidx.core.content.ContextCompat.getDrawable
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.launcher.config.Config
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.storage.NativeStorage
import java.io.File

class MainFragment : Fragment() {
    private var playBtn: ImageView? = null
    private var downloadBtn: ImageView? = null
    private var serverBtn: FrameLayout? = null
    private var newsBtn: FrameLayout? = null
    private var storeBtn: ImageView? = null
    private var notifyBtn: ImageView? = null
    private var settingsBtn: ImageView? = null
    private var tgBtn: ImageView? = null
    private var ytBtn: ImageView? = null
    private var supportBtn: ImageView? = null
    private var forumBtn: ImageView? = null
    private var nicknameField: EditText? = null
    private var notification: LauncherNotification? = null

    private var playState: Drawable? = null
    private var canPlay: Boolean = false

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        val view = inflater.inflate(R.layout.fragment_main, container, false)

        playBtn = view.findViewById(R.id.play_btn)
        downloadBtn = view.findViewById(R.id.download_btn)
        serverBtn = view.findViewById(R.id.server_btn)
        newsBtn = view.findViewById(R.id.news)
        storeBtn = view.findViewById(R.id.store_btn)
        notifyBtn = view.findViewById(R.id.notify_btn)
        settingsBtn = view.findViewById(R.id.settings_btn)

        tgBtn = view.findViewById(R.id.tg_btn)
        ytBtn = view.findViewById(R.id.yt_btn)
        supportBtn = view.findViewById(R.id.support_btn)
        forumBtn = view.findViewById(R.id.forum_btn)

        nicknameField = view.findViewById(R.id.nick_edit_text)

        val notifRoot = view.findViewById<View>(R.id.notification_root)
        notification = LauncherNotification(notifRoot, requireActivity())

        onClickInit()
        // InitPlayState()
        setPlayState(true, false)
        initUserData()
        initNicknameListener()
        return view
    }

    private fun onClickInit() {
        playBtn?.setOnClickListener {
            onClickPlay()
        }
        downloadBtn?.setOnClickListener {
            onClickDownload()
        }
        serverBtn?.setOnClickListener {
            onClickServer()
        }
        newsBtn?.setOnClickListener {
            openLink(Config.TELEGRAM_URI)
        }
        storeBtn?.setOnClickListener {
            onClickStore()
        }
        notifyBtn?.setOnClickListener {
            // replaceFragment(NotifyFragment)
        }
        settingsBtn?.setOnClickListener {
            replaceFragment(SettingsFragment())
        }
        tgBtn?.setOnClickListener {
            openLink(Config.TELEGRAM_URI)
        }
        ytBtn?.setOnClickListener {
            openLink(Config.YOUTUBE_URI)
        }
        supportBtn?.setOnClickListener {
            openLink(Config.SUPPORT_URI)
        }
        forumBtn?.setOnClickListener {
            openLink(Config.FORUM_URL)
        }
    }

    private fun openLink(url: String) {
        val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
        startActivity(intent)
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

    private fun initUserData() {
        val nickname = NativeStorage.getClientProperty("name", activity)
        nicknameField?.setText(nickname)
    }

    private fun initNicknameListener() {
        nicknameField?.setOnFocusChangeListener { _, hasFocus ->
            if (!hasFocus) {
                saveNickname()
            }
        }

        nicknameField?.setOnEditorActionListener { v, actionId, event ->
            val isDone = actionId == EditorInfo.IME_ACTION_DONE
            val isEnterKey = event?.keyCode == android.view.KeyEvent.KEYCODE_ENTER && event.action == android.view.KeyEvent.ACTION_DOWN

            if (isDone || isEnterKey) {
                saveNickname()
                nicknameField?.clearFocus()
                hideKeyboard()
                true
            } else {
                false
            }
        }
    }

    private fun saveNickname() {
        val nickname = nicknameField?.text?.toString() ?: ""
        NativeStorage.addClientProperty("name", nickname, activity)
    }

    private fun hideKeyboard() {
        val imm = requireContext().getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
        imm.hideSoftInputFromWindow(nicknameField?.windowToken, 0)
    }

    private fun InitPlayState() {
        playState = getDrawable(requireContext(), R.drawable.play_btn_off)
        playBtn?.setImageDrawable(playState)
        canPlay = false
    }

    private fun setPlayState(enable: Boolean, download: Boolean) {
        val drawable = when {
            enable -> R.drawable.play_btn
            !enable && !download -> R.drawable.play_btn_off
            else -> null
        }?.let { getDrawable(requireContext(), it) }

        playState = drawable
        canPlay = enable

        playBtn?.apply {
            visibility = if (enable || !download) View.VISIBLE else View.INVISIBLE
            setImageDrawable(playState)
        }
        downloadBtn?.visibility = if (download && !enable) View.VISIBLE else View.GONE
    }

    private fun startGame() {
        val log = File(requireContext().getExternalFilesDir(null), "log.txt")
        log.delete()

        val cinfo = File(requireContext().getExternalFilesDir(null), "CINFO.BIN")
        cinfo.delete()

        val minfo = File(File(requireContext().getExternalFilesDir(null), "models"), "MINFO.BIN")
        minfo.delete()

        val nickname = NativeStorage.getClientProperty("name", requireContext())
        if (nickname.isNullOrBlank()) {
            ActivityServiceImpl.showErrorMessage("Укажите ник!", requireActivity())
            return
        }

        val intent = Intent(requireContext(), Samp::class.java)
        intent.flags = Intent.FLAG_ACTIVITY_CLEAR_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP
        startActivity(intent)

        requireActivity().overridePendingTransition(android.R.anim.fade_in, android.R.anim.fade_out)

        requireActivity().finish()
    }

    private fun onClickPlay() {
        if(!canPlay) return

        startGame()
        /*if (isCheckSkipping) {
            startGame()
        } else {
            val progressDialog = findViewById<ConstraintLayout>(R.id.progressDialog)
            progressDialog.visibility = View.VISIBLE

            GlobalScope.launch {
                val filesList = CacheChecker.getInvalidFilesList(this@MainActivity)
                withContext(Dispatchers.Main) {
                    doAfterCacheChecked(filesList)

                    progressDialog.visibility = View.GONE
                }
            }
        }*/
    }

    private fun onClickServer() {
        notification?.showNotification(
            type = 0,
            text = "СЕРВЕРОВ НЕТ",
            duration = 5,
            actionId = 0,
            butt1 = "",
            butt2 = ""
        )
    }

    private fun onClickDownload() {
        notification?.showNotification(
            type = 0,
            text = "СЕРВЕРОВ НЕТ",
            duration = 5,
            actionId = 0,
            butt1 = "",
            butt2 = ""
        )
    }

    private fun onClickStore() {
        notification?.showNotification(
            type = 4,
            text = "СКОРО",
            duration = 5,
            actionId = 0,
            butt1 = "OK",
            butt2 = ""
        )
    }
}