package com.holy.launcher.download

import com.holy.launcher.NetworkService
import com.holy.launcher.async.dto.response.FileInfo
import com.holy.launcher.config.Config.APK_FILE_NAME
import com.holy.launcher.config.Config.ZIP_FILES_BASE_ADR
import com.holy.launcher.ui.activity.LoaderActivity
import com.holy.launcher.utils.BytesTo
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.net.URL
import java.util.zip.ZipEntry
import java.util.zip.ZipInputStream

interface DownloadListener {
    fun onDownloadComplete()
    fun onDownloadFailed()
}

class FileDownloader(
    private val loaderActivity: LoaderActivity,
    private var filesList: MutableList<FileInfo>)
{

    private var lastTime: Long = System.currentTimeMillis()
    private var lastDonwloaded: Long = 0
    private var curSpeed: Long = 0

    private var totalFilesSize: Long = 0
    private var totalDownloadedSize: Long = 0

    private var downloadListener: DownloadListener? = null

    private var leftFilesList: MutableList<FileInfo> = filesList.toMutableList()

    fun setDownloadListener(listener: DownloadListener) {
        downloadListener = listener
    }

    fun downloadAndUnzipFiles() {
        filesList = leftFilesList.toMutableList()
        totalFilesSize = filesList.sumOf { it.size }
        totalDownloadedSize = 0

        GlobalScope.launch(Dispatchers.Default) {
            for (file in filesList) {
                try {
                    downloadAndUnzipFile(file)
                    leftFilesList.remove(file)
                } catch (e: Exception) {
                    withContext(Dispatchers.Main) {
                        downloadListener?.onDownloadFailed()
                    }
                    return@launch
                }
            }
            withContext(Dispatchers.Main) {
                downloadListener?.onDownloadComplete()
            }
        }
    }

    private fun downloadFile(from: String, to: String) {
        val url = URL(from)
        val connection = url.openConnection()

        val inputStream = connection.getInputStream()
        val outputFile = File(to)

        // Создаем все необходимые родительские директории
        outputFile.parentFile?.mkdirs()

        val outputStream = FileOutputStream(outputFile)
        val buffer = ByteArray(1024)
        var bytesRead: Int
        var percentDownloaded = 0

        while (inputStream.read(buffer).also { bytesRead = it } != -1) {
            outputStream.write(buffer, 0, bytesRead)

            totalDownloadedSize += bytesRead.toLong() // Обновляем общий размер загруженных данных

            percentDownloaded = (totalDownloadedSize.toDouble() / totalFilesSize * 100).toInt() // Вычисляем общий процент завершения

            val currentTime = System.currentTimeMillis()
            val time = currentTime - lastTime
            if(time >= 1000) {
                curSpeed = totalDownloadedSize - lastDonwloaded
                lastDonwloaded = totalDownloadedSize
                lastTime = currentTime
            }

            val text = String.format("%s из %s (%s / сек.)",
                com.holy.launcher.utils.BytesTo.convert(totalDownloadedSize),
                com.holy.launcher.utils.BytesTo.convert(totalFilesSize),
                com.holy.launcher.utils.BytesTo.convert(curSpeed)
            )

            loaderActivity.updateProgress(percentDownloaded, outputFile.name, text)
        }

        inputStream.close()
        outputStream.close()
    }

    fun downloadAndInstallFile() {
        totalFilesSize = filesList.sumOf { it.size }
        GlobalScope.launch(Dispatchers.Default) {
            try {
                downloadFile(
                    NetworkService.APK_URL,
                    loaderActivity.getExternalFilesDir(null).toString() + "/" + APK_FILE_NAME
                )

                loaderActivity.installApk()

                withContext(Dispatchers.Main) {
                    downloadListener?.onDownloadComplete()
                }
            }
            catch (e: Exception) {
                downloadListener?.onDownloadFailed()
            }
        }
    }

    private suspend fun downloadAndUnzipFile(fileInfo: FileInfo) = withContext(Dispatchers.IO) {

        downloadFile(
            ZIP_FILES_BASE_ADR + fileInfo.path + ".zip",
            loaderActivity.getExternalFilesDir(null).toString() + "/" + fileInfo.path + ".zip"
        )
        println("Скачали файл ${fileInfo.path}")
        unzipFile(
            loaderActivity.getExternalFilesDir(null).toString() + "/" + fileInfo.path + ".zip",
            loaderActivity.getExternalFilesDir(null).toString() + "/" + fileInfo.path
        )

    }

    private fun unzipFile(from: String, to: String) {
        val zipFile = File(from)
        val unzipFile = File(to)

        val zipInputStream = ZipInputStream(FileInputStream(zipFile))
        var zipEntry: ZipEntry? = zipInputStream.nextEntry

        //
        loaderActivity.runOnUiThread {
            loaderActivity.speedText?.text = "Распаковка ..."
        }

        while (zipEntry != null) {
            val entryParentDir = unzipFile.parentFile

            if (zipEntry.isDirectory) {
                unzipFile.mkdirs()
            } else {
                entryParentDir?.mkdirs()
                val outputStream = FileOutputStream(unzipFile)
                val buffer = ByteArray(1024)
                var bytesRead: Int
                while (zipInputStream.read(buffer).also { bytesRead = it } != -1) {
                    outputStream.write(buffer, 0, bytesRead)
                }
                outputStream.close()
            }
            zipInputStream.closeEntry()
            zipEntry = zipInputStream.nextEntry
        }

        zipInputStream.close()
        zipFile.delete()

        println("Распаковка завершена: ${from}")
    }

}