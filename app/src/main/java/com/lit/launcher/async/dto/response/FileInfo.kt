package com.lit.launcher.async.dto.response

import java.io.Serializable

class FileInfo : Serializable {
  //  var link: String? = null
    var path: String = ""
    var size: Long = 0
    var hash: Long = 0
}