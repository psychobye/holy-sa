package com.holy.launcher.ui.activity

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.view.View
import android.view.animation.Animation
import android.view.animation.AnimationUtils
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.launcher.async.dto.response.FileInfo
import com.holy.launcher.config.Config.DONATE_URL
import com.holy.launcher.domain.enums.DownloadType
import com.holy.launcher.domain.enums.StorageElements
import com.holy.launcher.service.impl.ActivityServiceImpl
import com.holy.launcher.storage.NativeStorage
import com.holy.launcher.storage.Storage
import com.holy.launcher.ui.dialogs.EnterLockedServerPasswordDialog
import com.holy.launcher.ui.fragment.MainFragment
import com.holy.launcher.utils.MainUtils
import org.apache.commons.lang3.StringUtils
import java.io.File

class MainActivity : AppCompatActivity() {
    private var animation: Animation? = null
    private var mainFragment: MainFragment? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setTheme(R.style.AppBaseTheme)

        window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_FULLSCREEN
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)

        animation = AnimationUtils.loadAnimation(this, R.anim.button_click)

        mainFragment = MainFragment()

        replaceFragment(mainFragment)
    }

    private fun onClickPlay() {
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

    private val isCheckSkipping: Boolean
        get() {
            val isTestMode = com.holy.launcher.storage.NativeStorage.getClientProperty("test", this)

            //todo брать из Storage тк static стирается
            return TEST_MODE_ON_VALUE == isTestMode
    //        return true
        }

    private fun doAfterCacheChecked(fileToReload: MutableList<FileInfo>) {

        for (file in fileToReload) {
            println("invalid file = ${file.path}")
        }
        if (fileToReload.isEmpty()) {
            startGame()
        } else {
            MainUtils.FILES_TO_RELOAD = fileToReload

            MainUtils.type = com.holy.launcher.domain.enums.DownloadType.RELOAD_GAME_FILES
            startActivity(Intent(this, LoaderActivity::class.java))
        }
    }

    private fun startGame() {
        val log = File(getExternalFilesDir(null).toString() + "/log.txt")
        log.delete()

        // FIXME
        val aaaaaaaaaa = File(getExternalFilesDir(null).toString() + "/CINFO.BIN")
        // aaaaaaaaaa.delete()

        val bbbbbbbb = File(getExternalFilesDir(null).toString() + "/models/MINFO.BIN")
        bbbbbbbb.delete()

        val nickname = com.holy.launcher.storage.NativeStorage.getClientProperty("name", this)
        val selectedServer = com.holy.launcher.storage.NativeStorage.getClientProperty("server", this)
        if (StringUtils.isBlank(nickname)) {
            com.holy.launcher.service.impl.ActivityServiceImpl.showErrorMessage("Укажите ник!", this)
            onClickSettings()
            return
        }
        if (StringUtils.isBlank(selectedServer)) {
            com.holy.launcher.service.impl.ActivityServiceImpl.showErrorMessage("Выберите сервер", this)
            onClickMonitoring()
            return
        }
        val tmp = com.holy.launcher.storage.Storage.getProperty(com.holy.launcher.domain.enums.StorageElements.SERVER_LOCKED, this)
        var serverLockedValue = 0
        if (tmp != null) serverLockedValue = tmp.toInt()
        if (SERVER_LOCKED_VALUE == serverLockedValue) {
            val dialog = com.holy.launcher.ui.dialogs.EnterLockedServerPasswordDialog(this)
            dialog.setOnDialogCloseListener { password: String -> saveServerPassword(password) }
            dialog.createDialog()
            return
        }
        if (SERVER_LOCKED_VALUE != serverLockedValue) {
            com.holy.launcher.storage.NativeStorage.addClientProperty("password", StringUtils.EMPTY, this)
        }
        val intent = Intent(this, Samp::class.java)
        intent.flags = Intent.FLAG_ACTIVITY_CLEAR_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP
        startActivity(intent)

         this.finish();
    }

    private fun saveServerPassword(password: String) {
        com.holy.launcher.storage.NativeStorage.addClientProperty("password", password, this)
        startActivity(Intent(this, Samp::class.java))
        finish()
    }

    private fun onClickSettings() {
        // setTextColor(settingsButton, settingsTV, settingsImage)
        // replaceFragment(settingsFragment)
    }

    private fun onClickDonate() {
        val address = Uri.parse(DONATE_URL)
        val openlink = Intent(Intent.ACTION_VIEW, address)
        startActivity(openlink)
    }

    private fun onClickMonitoring() {
        // setTextColor(monitoringButton, monitoringTV, monitoringImage)
        // replaceFragment(monitoringFragment)
    }

    private fun replaceFragment(fragment: Fragment?) {
        supportFragmentManager.beginTransaction().replace(R.id.fragment_container, fragment!!).commitAllowingStateLoss()
    }

    public override fun onDestroy() {
        super.onDestroy()
    }

    public override fun onRestart() {
        super.onRestart()
    }

    public override fun onStop() {
        super.onStop()
    }

    companion object {
        private const val IS_AFTER_LOADING_KEY = "isAfterLoading"
        private const val GAME_DIRECTORY_EMPTY_SIZE = 0
        private const val SERVER_LOCKED_VALUE = 1
        private const val TEST_MODE_ON_VALUE = "1"
    }
}