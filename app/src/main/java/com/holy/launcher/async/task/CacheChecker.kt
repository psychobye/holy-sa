package com.holy.launcher.async.task

import android.app.Activity
import android.content.Context
import com.lit.launcher.async.dto.response.FileInfo
import com.lit.launcher.async.dto.response.GameFileInfoDto
import com.lit.launcher.utils.MainUtils
import java.io.File
import java.io.FileInputStream
import java.io.ObjectInputStream
import java.io.ObjectOutputStream
import java.util.zip.Adler32
import java.util.zip.CheckedInputStream

object CacheChecker {
    private val NOT_CHECK_BY_HASH_FILES = setOf(
        "gta_sa.set",
        "GTASAMP10.b",
        "SAMP/settings.ini",
        "Adjustable.cfg",
        "gtasatelem.set"
    )

    private const val CACHE_FILE_NAME = "last_files.dat"

    fun setFilesList(context: Context, gameFileInfoDto: GameFileInfoDto) {
        val iterator = gameFileInfoDto.files.iterator()

        while (iterator.hasNext()) {
            val file = iterator.next()
            for (ext in MainUtils.usselesTex) {
                if (file.path.contains(ext)) {
                    println("Удаление файла: ${file.path}")
                    iterator.remove() // Удаляем файл из списка
                }
            }
        }

        try {
            val fileOutputStream = context.openFileOutput(CACHE_FILE_NAME, Context.MODE_PRIVATE)
            val objectOutputStream = ObjectOutputStream(fileOutputStream)
            objectOutputStream.writeObject(gameFileInfoDto)
            objectOutputStream.close()
            fileOutputStream.close()
        } catch (ex: Exception) {
            ex.printStackTrace()
        }
    }


    @JvmStatic
    fun getFilesList(context: Context): GameFileInfoDto {
        try {
            val fileInputStream = context.openFileInput(CACHE_FILE_NAME)
            val objectInputStream = ObjectInputStream(fileInputStream)
            val files = objectInputStream.readObject() as GameFileInfoDto
            objectInputStream.close()
            fileInputStream.close()
            return files
        } catch (ex: Exception) {
            ex.printStackTrace()
        }
        return GameFileInfoDto()
    }

    @JvmStatic
    fun isGameCacheValid(activity: Activity): Boolean {
        return getInvalidFilesList(activity).isEmpty()
    }

    @JvmStatic
    fun getInvalidFilesList(activity: Activity): MutableList<FileInfo> {
        val filesToReload: MutableList<FileInfo> = mutableListOf()
        val externalFilesDir = activity.getExternalFilesDir(null)
        val allFiles = getFilesList(activity).files

        for (file in allFiles) {
            val localFile = File(externalFilesDir, file.path)

            if(!localFile.exists()) {
                filesToReload.add(file)
                continue
            }

            if(file.path !in NOT_CHECK_BY_HASH_FILES) {
                val hash = calculateAdler32Checksum(localFile)

                if(hash != file.hash)
                    filesToReload.add(file)
            }
        }

        return filesToReload
    }

    private fun calculateAdler32Checksum(file: File): Long {
        try {
            val adler32 = Adler32()
            val buffer = ByteArray(4096)
            var checksum: Long = -1

            FileInputStream(file).use { inputStream ->
                val checkedInputStream = CheckedInputStream(inputStream, adler32)
                while (checkedInputStream.read(buffer) != -1) {
                    // Пропустите эту строку, так как Adler32 уже обновлен автоматически
                }
                checksum = checkedInputStream.checksum.value
            }

            return checksum
        }
        catch (e: Exception){
            return -1
        }

    }

}