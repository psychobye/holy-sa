package com.lit.game.core

import android.annotation.SuppressLint
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Intent
import android.content.pm.PackageManager
import android.media.AudioAttributes
import android.media.SoundPool
import android.net.Uri
import android.os.Bundle
import android.os.Vibrator
import android.util.Base64
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.view.ViewParent
import android.view.ViewStub
import android.view.WindowManager
import android.view.animation.AnimationUtils
import android.webkit.WebView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import com.google.firebase.crashlytics.FirebaseCrashlytics
import com.lit.game.R
import com.lit.game.gui.hud.HudManager
import com.lit.launcher.async.task.CacheChecker.isGameCacheValid
import java.io.File
import java.io.IOException
import java.security.MessageDigest
import java.text.DecimalFormat
import java.text.DecimalFormatSymbols
import java.util.Locale
import java.util.Timer
import java.util.TimerTask

class Samp : GTASA() {
    private var mDialogClientSettings: DialogClientSettings? = null

    private external fun initSAMP(maxFps: Float, directory: String)

    override fun onCreate(bundle: Bundle?) {

        activity = this

        val display = Companion.windowManager.defaultDisplay
        maxFps = display.refreshRate

        val internalDir = File(filesDir, "AudioConfig")
        clearDir(internalDir)
        copyFromAssets(internalDir)

        initSAMP(maxFps, filesDir.toString())
        super.onCreate(bundle)
        init()
    }
    private fun clearDir(dir: File) {
        try {
            if (dir.exists()) {
                dir.deleteRecursively()
                Log.d("AudioConfig", "Audio directory cleared: ${dir.absolutePath}")
            }
        } catch (e: IOException) {
            Log.e("AudioConfig", "Failed to clear audio dir: ${e.message}")
        }
    }

    private fun copyFromAssets(dir: File) {
        try {
            if (!dir.exists()) {
                dir.mkdirs()
            }

            val assetFiles = assets.list("AudioConfig") ?: return

            assetFiles.forEach { filename ->
                assets.open("AudioConfig/$filename").use { inputStream ->
                    File(dir, filename).outputStream().use { outputStream ->
                        inputStream.copyTo(outputStream)
                        Log.d("AudioConfig", "Copied audio: $filename → ${dir.absolutePath}")
                    }
                }
            }

        } catch (e: IOException) {
            Log.e("ShaderManager", "Failed to copy shaders: ${e.message}")
        }
    }

    fun init() {
        HudManager()
        preloadCef()
    }

    @SuppressLint("SetJavaScriptEnabled")
    private fun preloadCef() {
        val webView = WebView(activity).apply {
            settings.javaScriptEnabled = true
            settings.domStorageEnabled = true
            settings.allowFileAccess = true

            setBackgroundColor(android.graphics.Color.TRANSPARENT)
            setLayerType(View.LAYER_TYPE_HARDWARE, null)
        }

        webView.loadUrl("about:blank")

        webView.postDelayed({
            webView.destroy()
        }, 500)
    }

    fun countAllChildren(viewParent: ViewParent?): Int {
        if (viewParent == null || viewParent !is ViewGroup) {
            return 0
        }
        val childCount = viewParent.childCount
        var totalCount = childCount
        for (i in 0 until childCount) {
            val childView = viewParent.getChildAt(i)
            if (childView is ViewGroup) {
                totalCount += countAllChildren(childView)
            }
        }
        return totalCount
    }

    fun hideLoadingScreen() {
        val task: TimerTask = object : TimerTask() {
            override fun run() {
                activity.runOnUiThread {
                    val loadscreen_main_layout = activity.findViewById<ConstraintLayout>(R.id.loadscreen_main_layout)
                    val parentContainer = loadscreen_main_layout.parent as ViewGroup
                    parentContainer.removeView(loadscreen_main_layout)
                }
            }
        }
        val timer = Timer("Timer")
        timer.schedule(task, 900L)
    }

    fun exitGame() {
        FirebaseCrashlytics.getInstance().deleteUnsentReports()
        FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false)
        finishAndRemoveTask()
        System.exit(0)
    }

    fun goVibrate(milliseconds: Int) {
        if (vibrator.hasVibrator()) {
            vibrator.vibrate(milliseconds.toLong())
        }
    }

    private fun openUrl(url: String) {
        runOnUiThread {
            val address = Uri.parse(url)
            val openlink = Intent(Intent.ACTION_VIEW, address)
            startActivity(openlink)
        }
    }

    private fun copyTextToBuffer(string: String) {
        runOnUiThread {
            val clipboardManager = this.getSystemService(CLIPBOARD_SERVICE) as ClipboardManager
            val clipData = ClipData.newPlainText("text", string)
            clipboardManager.setPrimaryClip(clipData)

            Toast.makeText(this, "Скопированно в буфер обмена ", Toast.LENGTH_SHORT).show()
        }
    }

    companion object {
        @JvmStatic
        lateinit var activity: AppCompatActivity

        @JvmStatic
        val vibrator by lazy { activity.getSystemService(VIBRATOR_SERVICE) as Vibrator }

        @JvmStatic
        var maxFps = 0f

        val windowManager by lazy { activity.getSystemService(WINDOW_SERVICE) as WindowManager }

        val formatter = DecimalFormatSymbols(Locale.getDefault()).run {
            groupingSeparator = '.'
            DecimalFormat("###,###.###", this)
        }

        const val INVALID_PLAYER_ID = 65535

        val soundPool = SoundPool.Builder()
            .setAudioAttributes(
                AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_GAME)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                    .build()
            )
            .build()

        @JvmStatic
        val clickAnim by lazy { AnimationUtils.loadAnimation(activity, R.anim.button_click) }

        // func
        external fun sendCommand(command: String?)
        external fun playUrlSound(url: String?)
        external fun gameFilesChecked(ok: Boolean)

        fun deInflatedViewStud(inflated: View?, id: Int, layout: Int) {
            activity.runOnUiThread {
                if (inflated == null) {
                    return@runOnUiThread
                }
                val parentContainer = inflated.parent as ViewGroup ?: return@runOnUiThread
                val index = parentContainer.indexOfChild(inflated)
                if (index == -1) {
                    return@runOnUiThread
                }
                parentContainer.removeViewAt(index)
                val viewStub = ViewStub(activity)
                viewStub.layoutResource = layout
                viewStub.id = id
                parentContainer.addView(viewStub, index)
            }
        }

        @JvmStatic
        private fun requestGameFilesCheck() {
            Thread {
                if (isGameCacheValid(activity)) {
                    activity.runOnUiThread { gameFilesChecked(true) }
                } else {
                    activity.runOnUiThread { gameFilesChecked(false) }
                }
            }.start()
        }

        @JvmStatic
        private fun getSignature(): String {
            var apkSignature: String? = null
            try {
                val packageInfo = activity.packageManager.getPackageInfo(
                    activity.packageName,
                    PackageManager.GET_SIGNATURES
                )
                for (signature in packageInfo.signatures!!) {
                    val md = MessageDigest.getInstance("SHA")
                    md.update(signature.toByteArray())
                    apkSignature = Base64.encodeToString(md.digest(), Base64.DEFAULT)
                }
            } catch (ignored: Exception) {
            }
            return apkSignature.toString()
        }

        val isTablet: Boolean
            get() = activity.resources.getBoolean(R.bool.is_tablet)
    }

    fun showClientSettings() {
        runOnUiThread {
            if (mDialogClientSettings != null) {
                mDialogClientSettings = null
            }
            mDialogClientSettings = DialogClientSettings()
            mDialogClientSettings?.show(supportFragmentManager, "test")
        }
    }
}
