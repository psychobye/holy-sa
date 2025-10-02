package com.lit.launcher.ui.activity

import android.content.Intent
import android.graphics.PorterDuff
import android.net.Uri
import android.os.Bundle
import android.view.View
import android.view.animation.Animation
import android.view.animation.AnimationUtils
import android.widget.FrameLayout
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.launcher.async.dto.response.FileInfo
import com.lit.launcher.config.Config.DONATE_URL
import com.lit.launcher.config.Config.FORUM_URL
import com.lit.launcher.domain.enums.DownloadType
import com.lit.launcher.domain.enums.StorageElements
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.storage.NativeStorage
import com.lit.launcher.storage.Storage
import com.lit.launcher.ui.dialogs.EnterLockedServerPasswordDialog
import com.lit.launcher.ui.fragment.MonitoringFragment
import com.lit.launcher.ui.fragment.SettingsFragment
import com.lit.launcher.utils.MainUtils
import org.apache.commons.lang3.StringUtils
import java.io.File

class MainActivity : AppCompatActivity() {
    private var animation: Animation? = null
    private var donateButton: LinearLayout? = null
    private var donateImage: ImageView? = null
    private var donateTV: TextView? = null
    private var monitoringButton: LinearLayout? = null
    private var monitoringFragment: MonitoringFragment? = null
    private var monitoringImage: ImageView? = null
    private var monitoringTV: TextView? = null
    private var playButton: LinearLayout? = null
    private var playImage: ImageView? = null
    private var rouletteButton: LinearLayout? = null
    private var rouletteImage: ImageView? = null
    private var rouletteTV: TextView? = null
    private var settingsButton: LinearLayout? = null
    private var settingsFragment: SettingsFragment? = null
    private var settingsImage: ImageView? = null
    private var settingsTV: TextView? = null
    private var container_layout: FrameLayout? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setTheme(R.style.AppBaseTheme)

//        setContentView(R.layout.spin_box);
//        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
//        gg = new SpinBox(this);
        window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_FULLSCREEN
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)

        container_layout = findViewById(R.id.container)
        animation = AnimationUtils.loadAnimation(this, R.anim.button_click)
        monitoringTV = findViewById(R.id.monitoringTV)
        settingsTV = findViewById(R.id.settingsTV)
        rouletteTV = findViewById(R.id.forumTV) 
        donateTV = findViewById(R.id.donateTV)
        monitoringImage = findViewById(R.id.monitoringImage)
        settingsImage = findViewById(R.id.settingsImage)
        rouletteImage = findViewById(R.id.forumImage)
        donateImage = findViewById(R.id.donateImage)
        playImage = findViewById(R.id.playImage)
        monitoringButton = findViewById(R.id.monitoringButton)
        settingsButton = findViewById(R.id.settingsButton)
        rouletteButton = findViewById(R.id.rouletteButton)
        donateButton = findViewById(R.id.donateButton)
        playButton = findViewById(R.id.playButton)
        monitoringFragment = MonitoringFragment()
        settingsFragment = SettingsFragment()
        if (savedInstanceState != null && savedInstanceState.getBoolean(IS_AFTER_LOADING_KEY)) {
            replaceFragment(settingsFragment)
        } else if (savedInstanceState == null && intent.extras != null && intent.extras!!.getBoolean(IS_AFTER_LOADING_KEY)) {
            onClickSettings()
        } else {
            replaceFragment(monitoringFragment)
        }
        monitoringButton!!.setOnClickListener {
            onClickMonitoring()
        }

        settingsButton!!.setOnClickListener {
            onClickSettings()
        }

        rouletteButton!!.setOnClickListener {
            val address = Uri.parse(FORUM_URL)
            val openlink = Intent(Intent.ACTION_VIEW, address)
            startActivity(openlink)
        }
        donateButton!!.setOnClickListener {
            onClickDonate()
        }

        playButton!!.setOnClickListener {
            onClickPlay()
        }

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
            val isTestMode = NativeStorage.getClientProperty("test", this)

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

            MainUtils.type = DownloadType.RELOAD_GAME_FILES
            startActivity(Intent(this, LoaderActivity::class.java))
        }
    }

    private fun startGame() {
        val log = File(getExternalFilesDir(null).toString() + "/log.txt")
        log.delete()

        // FIXME
        val aaaaaaaaaa = File(getExternalFilesDir(null).toString() + "/CINFO.BIN")
        aaaaaaaaaa.delete()

        val bbbbbbbb = File(getExternalFilesDir(null).toString() + "/models/MINFO.BIN")
        bbbbbbbb.delete()

        val nickname = NativeStorage.getClientProperty("name", this)
        val selectedServer = NativeStorage.getClientProperty("server", this)
        if (StringUtils.isBlank(nickname)) {
            ActivityServiceImpl.showErrorMessage("Укажите ник!", this)
            onClickSettings()
            return
        }
        if (StringUtils.isBlank(selectedServer)) {
            ActivityServiceImpl.showErrorMessage("Выберите сервер", this)
            onClickMonitoring()
            return
        }
        val tmp = Storage.getProperty(StorageElements.SERVER_LOCKED, this)
        var serverLockedValue = 0
        if (tmp != null) serverLockedValue = tmp.toInt()
        if (SERVER_LOCKED_VALUE == serverLockedValue) {
            val dialog = EnterLockedServerPasswordDialog(this)
            dialog.setOnDialogCloseListener { password: String -> saveServerPassword(password) }
            dialog.createDialog()
            return
        }
        if (SERVER_LOCKED_VALUE != serverLockedValue) {
            NativeStorage.addClientProperty("password", StringUtils.EMPTY, this)
        }
        val intent = Intent(this, Samp::class.java)
        intent.flags = Intent.FLAG_ACTIVITY_CLEAR_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP
        startActivity(intent)

         this.finish();
    }

    private fun saveServerPassword(password: String) {
        NativeStorage.addClientProperty("password", password, this)
        startActivity(Intent(this, Samp::class.java))
        finish()
    }

    private fun onClickSettings() {
        setTextColor(settingsButton, settingsTV, settingsImage)
        replaceFragment(settingsFragment)
    }

    private fun onClickDonate() {
        val address = Uri.parse(DONATE_URL)
        val openlink = Intent(Intent.ACTION_VIEW, address)
        startActivity(openlink)
    }

    private fun onClickMonitoring() {
        setTextColor(monitoringButton, monitoringTV, monitoringImage)
        replaceFragment(monitoringFragment)
    }

    fun setTextColor(linearLayout: LinearLayout?, textView: TextView?, imageView: ImageView?) {
        monitoringButton!!.alpha = 0.45f
        settingsButton!!.alpha = 0.45f
        rouletteButton!!.alpha = 0.45f
        donateButton!!.alpha = 0.45f
        monitoringTV!!.setTextColor(resources.getColor(R.color.menuTextDisable))
        settingsTV!!.setTextColor(resources.getColor(R.color.menuTextDisable))
        rouletteTV!!.setTextColor(resources.getColor(R.color.menuTextDisable))
        donateTV!!.setTextColor(resources.getColor(R.color.menuTextDisable))
        monitoringImage!!.setColorFilter(resources.getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN)
        settingsImage!!.setColorFilter(resources.getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN)
        rouletteImage!!.setColorFilter(resources.getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN)
        donateImage!!.setColorFilter(resources.getColor(R.color.menuTextDisable), PorterDuff.Mode.SRC_IN)
        linearLayout!!.alpha = 1.0f
        textView!!.setTextColor(resources.getColor(R.color.menuTextEnable))
        imageView!!.setColorFilter(resources.getColor(R.color.menuTextEnable), PorterDuff.Mode.SRC_IN)
    }

    private fun replaceFragment(fragment: Fragment?) {
        supportFragmentManager.beginTransaction().replace(R.id.container, fragment!!).commitAllowingStateLoss()
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