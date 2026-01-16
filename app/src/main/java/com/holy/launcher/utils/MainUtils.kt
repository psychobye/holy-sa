package com.holy.launcher.utils

import com.lit.launcher.async.dto.response.FileInfo
import com.lit.launcher.async.dto.response.LatestVersionInfoDto
import com.holy.launcher.domain.enums.DownloadType

class MainUtils {
    companion object {
        @JvmStatic
        var usselesTex = mutableListOf(/*".dxt",*/ ".pvr", /*".etc"*/)

        @JvmStatic
        var type = com.holy.launcher.domain.enums.DownloadType.RELOAD_GAME_FILES

        @JvmField
        var FILES_TO_RELOAD: MutableList<FileInfo> = mutableListOf()

        @JvmField
        var LATEST_APK_INFO: LatestVersionInfoDto? = null
    }
}