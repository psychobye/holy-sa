package com.lit.launcher.ui.activity

import android.Manifest
import android.animation.AnimatorListenerAdapter
import android.app.AlertDialog
import android.content.DialogInterface
import android.content.Intent
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.firebase.crashlytics.FirebaseCrashlytics
import com.lit.game.databinding.ActivitySplashBinding

class SplashActivity : AppCompatActivity() {
    private var permissionsGranded = false

    private val REQUEST_ID = 228
    private val permissionList = arrayOf(
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.RECORD_AUDIO,
    )

    private lateinit var binding: ActivitySplashBinding

    private var askedManageSettings = false
    private var isRequestingRuntimePermissions = false

    private var currentDialog: AlertDialog? = null

    private val isOnline: Boolean
        get() {
            val cm = getSystemService(CONNECTIVITY_SERVICE) as ConnectivityManager
            return cm.activeNetworkInfo != null
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        FirebaseCrashlytics.getInstance().deleteUnsentReports()
        FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(true)

        super.onCreate(savedInstanceState)
        binding = ActivitySplashBinding.inflate(layoutInflater)
        setContentView(binding.root)

        window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_FULLSCREEN
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)

        checkPermissions()
    }

    private fun showBlockingDialog(
        title: String,
        message: String,
        positive: String,
        onPositive: (() -> Unit)? = null,
        negative: String? = null,
        onNegative: (() -> Unit)? = null,
        cancelable: Boolean = false
    ) {
        if (isFinishing || isDestroyed) return

        runOnUiThread {
            if (isFinishing || isDestroyed) return@runOnUiThread
            currentDialog?.dismiss()

            val builder = AlertDialog.Builder(this)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(cancelable)

            builder.setPositiveButton(positive) { _, _ -> onPositive?.invoke() }
            if (negative != null) builder.setNegativeButton(negative) { _, _ -> onNegative?.invoke() }

            currentDialog = builder.create()
            try {
                currentDialog?.show()
            } catch (_: Exception) { }
        }
    }

    private fun checkPermissions() {
        if (isRequestingRuntimePermissions || askedManageSettings) return

        val permissionsToRequest = mutableListOf<String>()
        for (permission in permissionList) {
            if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                permissionsToRequest.add(permission)
            }
        }

        if (permissionsToRequest.isNotEmpty()) {
            isRequestingRuntimePermissions = true
            ActivityCompat.requestPermissions(this, permissionsToRequest.toTypedArray(), REQUEST_ID)
            return
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!hasAllFilesAccess()) {
                requestManageAllFilesIfNeeded()
                return
            }
        }

        permissionsGranded = true
        startIfReady()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        isRequestingRuntimePermissions = false

        if (requestCode == REQUEST_ID) {
            var allGranted = true
            if (grantResults.isEmpty()) {
                allGranted = false
            } else {
                for (res in grantResults) {
                    if (res != PackageManager.PERMISSION_GRANTED) {
                        allGranted = false
                        break
                    }
                }
            }

            if (allGranted) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !hasAllFilesAccess()) {
                    requestManageAllFilesIfNeeded()
                } else {
                    permissionsGranded = true
                    startIfReady()
                }
            } else {
                showBlockingDialog(
                    title = "без прав работать нельзя",
                    message = "нужны права на чтение/запись и запись аудио. дай их, иначе игра не сможет работать.",
                    positive = "попробовать снова",
                    onPositive = { checkPermissions() },
                    negative = "выйти",
                    onNegative = { finishAffinity() },
                    cancelable = false
                )
            }
        }
    }

    private fun hasAllFilesAccess(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Environment.isExternalStorageManager()
        } else {
            true
        }
    }

    private fun requestManageAllFilesIfNeeded() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !hasAllFilesAccess()) {
            askedManageSettings = true
            showBlockingDialog(
                title = "нужен доступ к файлам",
                message = "игре нужен доступ ко всем файлам, чтобы работать с кешем. разреши доступ в настройках.",
                positive = "открыть настройки",
                onPositive = { openManageAllFilesSettings() },
                negative = "не",
                onNegative = { },
                cancelable = false
            )
        }
    }

    private fun openManageAllFilesSettings() {
        try {
            val intent = Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
            intent.data = Uri.parse("package:$packageName")
            startActivity(intent)
        } catch (e: Exception) {
            try {
                val intent = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
                startActivity(intent)
            } catch (ex: Exception) {
                val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
                intent.data = Uri.parse("package:$packageName")
                startActivity(intent)
            }
        }
    }

    private fun startIfReady() {
        if (permissionsGranded) {
            if (!isFinishing && !isDestroyed) {
                startActivity(Intent(this, MainActivity::class.java))
                finish()
            }
        }
    }

    private fun checkAllPermissionsAndStart() {
        val allFiles = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Environment.isExternalStorageManager()
        } else true

        val allRuntimeOk = permissionList.all {
            ContextCompat.checkSelfPermission(this, it) == PackageManager.PERMISSION_GRANTED
        }

        if (allFiles && allRuntimeOk) {
            permissionsGranded = true
            startIfReady()
        } else {
            if (!allFiles) {
                showBlockingDialog(
                    title = "нет доступа ко всем файлам",
                    message = "похоже, вы не включили «Доступ ко всем файлам». открыть настройки?",
                    positive = "да",
                    onPositive = { openManageAllFilesSettings() },
                    negative = "не",
                    onNegative = { },
                    cancelable = false
                )
            } else if (!allRuntimeOk) {
                showBlockingDialog(
                    title = "нет необходимых прав",
                    message = "не даны права READ/WRITE/RECORD_AUDIO. дать их?",
                    positive = "запросить",
                    onPositive = { checkPermissions() },
                    negative = "не",
                    onNegative = { },
                    cancelable = false
                )
            }
        }
    }

    override fun onResume() {
        super.onResume()
        checkAllPermissionsAndStart()
    }

    override fun onPause() {
        super.onPause()
        currentDialog?.dismiss()
        currentDialog = null
    }

    override fun onDestroy() {
        currentDialog?.dismiss()
        currentDialog = null
        super.onDestroy()
    }
}