package com.lit.launcher.ui.activity

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.view.animation.AnimationUtils
import android.widget.ImageView
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.FileProvider
import androidx.viewpager2.widget.ViewPager2
import androidx.viewpager2.widget.ViewPager2.OnPageChangeCallback
import com.google.firebase.crashlytics.FirebaseCrashlytics
import com.lit.game.BuildConfig
import com.lit.game.R
import com.lit.launcher.NetworkService
import com.lit.launcher.async.dto.response.FileInfo
import com.lit.launcher.async.dto.response.LoaderSliderInfoResponseDto
import com.lit.launcher.config.Config
import com.lit.launcher.domain.enums.DownloadType
import com.lit.launcher.download.DownloadListener
import com.lit.launcher.download.FileDownloader
import com.lit.launcher.service.ActivityService
import com.lit.launcher.service.impl.ActivityServiceImpl
import com.lit.launcher.ui.adapters.LoaderSliderAdapter
import com.lit.launcher.utils.MainUtils
import com.lit.launcher.utils.MainUtils.Companion.type
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import java.io.File
import kotlin.system.exitProcess

class LoaderActivity : AppCompatActivity() {
    private var fileDownloader: FileDownloader? = null
    private var activityService: ActivityService? = null
    @JvmField
    var loading: TextView? = null
    @JvmField
    var repeatLoadButton: TextView? = null
    @JvmField
    var loadingPercent: TextView? = null
    @JvmField
    var speedText: TextView? = null
    var fileName: TextView? = null
    var progressBar: ProgressBar? = null
    var leftButton: ImageView? = null
    var rightButton: ImageView? = null
    var sliderView: ViewPager2? = null


    init {
        activityService = ActivityServiceImpl()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_loader)
        window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_FULLSCREEN
                or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)
        initialize()
        installGame(type)
    }

    private fun redirectToMonitoring() {
        val intent = Intent(this, SplashActivity::class.java)
        intent.putExtras(getIntent())
        startActivity(intent)
        finish()
    }

    private fun installGame(type: DownloadType) {
        when (type) {
            DownloadType.RELOAD_GAME_FILES -> {
                fileDownloader = MainUtils.FILES_TO_RELOAD?.let { FileDownloader(this, it) }

                fileDownloader?.setDownloadListener(object : DownloadListener {
                    override fun onDownloadComplete() {
                        redirectToMonitoring()
                    }

                    override fun onDownloadFailed() {
                        repeatLoadButton?.visibility = View.VISIBLE
                    }
                })
                fileDownloader?.downloadAndUnzipFiles()
            }

            DownloadType.UPDATE_APK -> {
                val fileInfo = FileInfo()
                val apkInfo = MainUtils.LATEST_APK_INFO ?: return

                fileInfo.size = apkInfo.size
                fileInfo.path = apkInfo.path

                fileDownloader = FileDownloader(this, mutableListOf(fileInfo))

                fileDownloader?.setDownloadListener(object : DownloadListener {
                    override fun onDownloadComplete() {
                    }
                    override fun onDownloadFailed() {
                        finish()
                        exitProcess(EXIT_SUCCESS_STATUS)
                    }
                })
                fileDownloader?.downloadAndInstallFile()
            }

        }
    }
    fun updateProgress(percent: Int, file: String, text: String) {
        runOnUiThread {
            progressBar?.progress = percent
            loadingPercent?.text = "$percent%"
            fileName?.text = file
            speedText?.text = text
        }
    }

    private fun initialize() {
        val animation = AnimationUtils.loadAnimation(this, R.anim.button_click)
        loading = findViewById(R.id.loadingText)
        loadingPercent = findViewById(R.id.loadingPercent)
        progressBar = findViewById(R.id.progressBar)
        speedText = findViewById(R.id.speedText)
        fileName = findViewById<View>(R.id.fileName) as TextView
        leftButton = findViewById<View>(R.id.leftButton) as ImageView
        rightButton = findViewById<View>(R.id.rightButton) as ImageView
        sliderView = findViewById(R.id.loaderSliderView)

        // repeat
        repeatLoadButton = findViewById<View>(R.id.repeatLoad) as TextView
        repeatLoadButton?.setOnClickListener {
            repeatLoadButton?.visibility = View.GONE
            fileDownloader?.downloadAndUnzipFiles()
        }

        leftButton!!.setOnClickListener { v: View ->
            v.startAnimation(animation)
            performLeftButton()
        }
        rightButton!!.setOnClickListener { v: View ->
            v.startAnimation(animation)
            performRightButton()
        }
        loadResources()
    }

    private fun loadResources() {
        val retrofit = Retrofit.Builder()
            .baseUrl(Config.LIVE_RUSSIA_RESOURCE_SERVER_URL)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
        val networkService = retrofit.create(NetworkService::class.java)
        val call = networkService.loaderSliderInfo
        call.enqueue(object : Callback<LoaderSliderInfoResponseDto?> {
            override fun onResponse(call: Call<LoaderSliderInfoResponseDto?>, response: Response<LoaderSliderInfoResponseDto?>) {
                if (response.isSuccessful) {
                    configureSlider(response.body())
                } else {
                    configureSliderWithoutData()
                }
            }

            override fun onFailure(call: Call<LoaderSliderInfoResponseDto?>, t: Throwable) {
                configureSliderWithoutData()
            }
        })
    }

    private fun configureSliderWithoutData() {

        initSlider(ArrayList())
    }

    private fun configureSlider(loaderSliderInfoResponseDto: LoaderSliderInfoResponseDto?) {
        println("configureSlider")

        initSlider(loaderSliderInfoResponseDto?.texts!!)
    }

    private fun initSlider(sliderDataArrayList: List<String>) {
        val adapter = LoaderSliderAdapter(sliderDataArrayList)
        sliderView!!.adapter = adapter
        sliderView!!.offscreenPageLimit = 1
        sliderView!!.clipToPadding = false
        sliderView!!.clipChildren = false
        sliderView!!.setCurrentItem(1, false)
        sliderView!!.registerOnPageChangeCallback(object : OnPageChangeCallback() {
            override fun onPageSelected(position: Int) {
                super.onPageSelected(position)
            }

            override fun onPageScrollStateChanged(state: Int) {
                if (state == ViewPager2.SCROLL_STATE_IDLE) {
                    if (sliderView!!.currentItem == sliderDataArrayList.size + 2 - 1) {
                        sliderView!!.setCurrentItem(1, false)
                    } else if (sliderView!!.currentItem == 0) {
                        sliderView!!.setCurrentItem(sliderDataArrayList.size + 2 - 2, false)
                    }
                }
                super.onPageScrollStateChanged(state)
            }
        })
    }

    private fun performRightButton() {
        sliderView!!.currentItem = sliderView!!.currentItem + 1
    }

    private fun performLeftButton() {
        sliderView!!.currentItem = sliderView!!.currentItem - 1
    }

    fun showMessage(_s: String?) {
        Toast.makeText(applicationContext, _s, Toast.LENGTH_LONG).show()
    }

    override fun onBackPressed() {
        finish()
        System.exit(EXIT_SUCCESS_STATUS)
    }

    fun installApk() {
        try {
            val file = File(getExternalFilesDir(null).toString(), Config.APK_FILE_NAME)
            var intent: Intent
            if (file.exists()) {
                run {
                    val apkUri = FileProvider.getUriForFile(this, BuildConfig.APPLICATION_ID + FILE_PROVIDER_EXTENSION, file)
                    //                    intent = new Intent(Intent.ACTION_INSTALL_PACKAGE);
                    intent = Intent(Intent.ACTION_VIEW)
                    intent.data = apkUri
                    intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                }
                startActivity(intent)
                finish()
            } else {
                ActivityServiceImpl.showErrorMessage("Ошибка установки: файл не найден", this)
                finish()
                exitProcess(EXIT_SUCCESS_STATUS)
            }
        } catch (e: Exception) {
            FirebaseCrashlytics.getInstance().recordException(e)
            finish()
            exitProcess(EXIT_SUCCESS_STATUS)
        }
    }

    companion object {
        private const val FILE_PROVIDER_EXTENSION = ".provider"
        private const val EXIT_SUCCESS_STATUS = 0
    }
}