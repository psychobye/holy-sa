package com.lit.game

import android.graphics.Bitmap
import android.widget.ImageView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.nio.ByteBuffer

// просто struct чтоб удобнее было передавать в параметрах
class SnapShot(var type: Int,
               var modelId: Int,
               var rotX: Float,
               var rotY: Float,
               var rotZ: Float,
               var xOffset: Float,
               var yOffset: Float,
               var zOffset: Float,


)

class PlateSnapShot(var type: Int,
                    var number: String,
                    var region: String
)


object EntitySnaps {

    private external fun nativeEntitySnapAddToQueue(imageView: ImageView, type: Int, modelId: Int, rotX: Float, rotY: Float, rotZ: Float, xOffset: Float, yOffset: Float, zOffset: Float)
    private external fun nativePlateSnapAddToQueue(imageView: ImageView, type: Int, number: String, region: String)

    fun loadEntitySnapToImageView(snap: SnapShot, imageView: ImageView) {
        nativeEntitySnapAddToQueue(imageView, snap.type, snap.modelId, snap.rotX, snap.rotY, snap.rotZ, snap.xOffset, snap.yOffset, snap.zOffset)
    }

    fun loadPlateSnapToImageView(snap: PlateSnapShot, imageView: ImageView) {
        nativePlateSnapAddToQueue(imageView, snap.type, snap.number, snap.region)
    }

    @JvmStatic
    fun setToImageView(imageView: ImageView, bytes: ByteArray, width: Int, height: Int) {
        GlobalScope.launch {

            if(bytes.isNotEmpty()) {
                val bmp = bitmapFromRgba(width, height, bytes)
                bmp.setHasAlpha(true)

                withContext(Dispatchers.Main) {
                    imageView.setImageBitmap(bmp)
                }
            }
        }
    }

    @JvmStatic
    fun bitmapFromRgba(width: Int, height: Int, bytes: ByteArray): Bitmap {
        val buffer = ByteBuffer.wrap(bytes)
        val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        bitmap.copyPixelsFromBuffer(buffer)
        return bitmap
    }


    const val OBJECT = 0
    const val TYPE_PED = 1
    const val TYPE_VEHICLE = 2
}