package com.lit.launcher.ui.fragment

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.animation.Animation
import android.view.animation.AnimationUtils
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.launcher.async.dto.response.FileInfo
import com.lit.launcher.async.task.CacheChecker
import com.lit.launcher.config.Config
import com.lit.launcher.domain.enums.DownloadType
import com.lit.launcher.service.ActivityService
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.storage.NativeStorage
import com.lit.launcher.ui.activity.LoaderActivity
import com.lit.launcher.ui.dialogs.ConfirmDialog
import com.lit.launcher.ui.dialogs.EnterNicknameDialog
import com.lit.launcher.utils.FileUtils
import com.lit.launcher.utils.MainUtils
import com.lit.launcher.utils.MainUtils.Companion.type
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File

class SettingsFragment : Fragment(), View.OnClickListener {
    private var animation: Animation? = null
    private var nicknameField: TextView? = null
    private var activityService: ActivityService? = null
    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        activityService = ActivityServiceImpl()
        val inflate = inflater.inflate(R.layout.fragment_settings, container, false)
        animation = AnimationUtils.loadAnimation(context, R.anim.button_click)
        nicknameField = inflate.findViewById(R.id.nick_edit)
        nicknameField?.setOnClickListener(this)
        inflate.findViewById<View>(R.id.youtubeButton).setOnClickListener(this)
        inflate.findViewById<View>(R.id.vkButton).setOnClickListener(this)
        inflate.findViewById<View>(R.id.discordButton).setOnClickListener(this)
        inflate.findViewById<View>(R.id.resetSettings).setOnClickListener(this)
        inflate.findViewById<View>(R.id.reinstallGame).setOnClickListener(this)
        inflate.findViewById<View>(R.id.validateCache).setOnClickListener(this)
        inflate.findViewById<View>(R.id.telegramButton).setOnClickListener(this)
        initUserData()
        return inflate
    }

    override fun onClick(view: View) {
        when (view.id) {
            R.id.youtubeButton -> {
                view.startAnimation(animation)
                performYouTubeButtonAction()
            }

            R.id.vkButton -> {
                view.startAnimation(animation)
                performVkButtonAction()
            }

            R.id.discordButton -> {
                view.startAnimation(animation)
                performDiscordButtonAction()
            }

            R.id.telegramButton -> {
                view.startAnimation(animation)
                performTelegramButtonAction()
            }

            R.id.resetSettings -> {
                view.startAnimation(animation)
                performResetSettingsButtonAction()
            }

            R.id.reinstallGame -> {
                view.startAnimation(animation)
                performReinstallGameButtonAction()
            }

            R.id.validateCache -> {
                view.startAnimation(animation)
                performValidateCacheButtonAction()
            }

            R.id.nick_edit -> performNickEditFieldOnClickAction()
            else -> {}
        }
    }

    private fun performNickEditFieldOnClickAction() {
        EnterNicknameDialog(this)
    }

    private fun performReinstallGameButtonAction() {
        val confirmDialog = ConfirmDialog(activity, "Переустановить игру?")
        confirmDialog.setOnDialogCloseListener { isConfirm: Boolean -> onConfirmReinstallFinished(isConfirm) }
        confirmDialog.createDialog()
    }

    private fun onConfirmReinstallFinished(isConfirm: Boolean) {
        if (isConfirm) {
            val gameDirectory = File(requireActivity().getExternalFilesDir(null).toString())
            FileUtils.delete(gameDirectory)
            type = DownloadType.RELOAD_GAME_FILES
            startActivity(Intent(activity, LoaderActivity::class.java))
        }
    }

    private fun performValidateCacheButtonAction() {
        val progressDialog = requireActivity().findViewById<ConstraintLayout>(R.id.progressDialog)
        progressDialog.visibility = View.VISIBLE

        GlobalScope.launch {
            val filesList = CacheChecker.getInvalidFilesList(requireActivity())
            withContext(Dispatchers.Main) {
                doAfterCacheChecked(filesList)

                progressDialog.visibility = View.GONE
            }
        }
    }

    private fun doAfterCacheChecked(fileToReloadArray: MutableList<FileInfo>) {
        if (fileToReloadArray.isEmpty()) {
            activityService?.showInfoMessage("Файлы в порядке!", this.activity)
        } else {
            validateCache(fileToReloadArray)
        }
    }

    private fun validateCache(filesToReloadList: MutableList<FileInfo>) {
        MainUtils.FILES_TO_RELOAD = filesToReloadList
        type = DownloadType.RELOAD_GAME_FILES
        startActivity(Intent(activity, LoaderActivity::class.java))
    }

    private fun performResetSettingsButtonAction() {
        val confirmDialog = ConfirmDialog(activity, "Сбросить настройки игры?")
        confirmDialog.setOnDialogCloseListener { isConfirm: Boolean -> onConfirmResetSettingsFinished(isConfirm) }
        confirmDialog.createDialog()
    }

    private fun onConfirmResetSettingsFinished(isConfirm: Boolean) {
        if (!isConfirm) {
            return
        }
        if (!activityService!!.isGameFileInstall(activity, Config.SETTINGS_FILE_PATH)) {
            activityService!!.showInfoMessage("Сначала установите игру", activity)
            return
        }
        val settingsFile = File(requireActivity().getExternalFilesDir(null).toString() + Config.SETTINGS_FILE_PATH)
        if (settingsFile.exists()) {
            settingsFile.delete()
            activityService!!.showInfoMessage("Вы успешно сбросили настройки!", activity)
        } else {
            activityService!!.showInfoMessage("Настройки по умолчанию уже установлены", activity)
        }
    }

    private fun performDiscordButtonAction() {
        startActivity(Intent(CONTACTS_VIEW_ACTION, Uri.parse(Config.DISCORD_URI)))
    }

    private fun performVkButtonAction() {
        startActivity(Intent(CONTACTS_VIEW_ACTION, Uri.parse(Config.VK_URI)))
    }

    private fun performYouTubeButtonAction() {
        startActivity(Intent(CONTACTS_VIEW_ACTION, Uri.parse(Config.YOU_TUBE_URI)))
    }

    private fun performTelegramButtonAction() {
        startActivity(Intent(CONTACTS_VIEW_ACTION, Uri.parse(Config.TELEGRAM_URI)))
    }

    private fun initUserData() {
        val nickname = NativeStorage.getClientProperty("name", this.activity)
        nicknameField!!.text = nickname
    }

    fun updateNicknameField(nickname: String?) {
        nicknameField!!.text = nickname
    }

    companion object {
        private const val GAME_DIRECTORY_EMPTY_SIZE = 0
        private const val CONTACTS_VIEW_ACTION = "android.intent.action.VIEW"
    }
}