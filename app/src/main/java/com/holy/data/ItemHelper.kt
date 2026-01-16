package com.holy.data

import android.content.res.ColorStateList
import android.graphics.Color
import android.widget.ImageView
import com.lit.game.EntitySnaps
import com.lit.game.PlateSnapShot
import com.lit.game.SnapShot
import com.lit.game.core.Samp
import com.lit.data.acs.Accessories
import com.lit.data.skins.Skins
import com.lit.data.vehicles.Vehicles
import java.lang.Exception

object ItemHelper {

    fun loadSpriteToImageView(sprite: String, imageView: ImageView) {
        val entitySnap = getSnapEntityFromName(sprite)
        if(entitySnap != null) {
            imageView.setImageResource(0)
            EntitySnaps.loadEntitySnapToImageView(entitySnap, imageView)
            return
        }

        val plateSnapShot = getPlateSnapFromName(sprite)
        if(plateSnapShot != null) {
            imageView.setImageResource(0)
            EntitySnaps.loadPlateSnapToImageView(plateSnapShot, imageView)
            return
        }

        val resId = Samp.activity.resources.getIdentifier(sprite, "drawable", Samp.activity.packageName)
        //if(resId != 0) {
            imageView.setImageResource(Samp.activity.resources.getIdentifier(sprite, "drawable", Samp.activity.packageName))
            return
        //}

    }

    fun loadSpriteToImageView_new(sprite: String, imageView: ImageView) {
        val entitySnap = getSnapEntityFromName_new(sprite)
        if(entitySnap != null) {
            imageView.setImageResource(0)
            EntitySnaps.loadEntitySnapToImageView(entitySnap, imageView)
            return
        }

        val plateSnapShot = getPlateSnapFromName(sprite)
        if(plateSnapShot != null) {
            imageView.setImageResource(0)
            EntitySnaps.loadPlateSnapToImageView(plateSnapShot, imageView)
            return
        }

        //val resId = Samp.activity.resources.getIdentifier(sprite, "drawable", Samp.activity.packageName)
        //if(resId != 0) {
        imageView.setImageResource(Samp.activity.resources.getIdentifier(sprite, "drawable", Samp.activity.packageName))
        return
        //}

    }

    fun getSnapEntityFromName_new(str: String): SnapShot? {
        // "veh_411_0.5_0.1_0.0_0.0_0.0_0.0"

        val parts = str.split("_")

        val type = when(parts[0]) {
            "skin" -> {
                EntitySnaps.TYPE_PED
            }
            "veh" -> {
                EntitySnaps.TYPE_VEHICLE
            }
            "obj" -> {
                EntitySnaps.OBJECT
            }
            else -> -1
        }
        if(type == -1)
            return null

        return SnapShot(
            type,
            parts[1].toInt(),

            parts[2].toFloat(), parts[3].toFloat(), parts[4].toFloat(),
            parts[5].toFloat(), parts[6].toFloat(), parts[7].toFloat(),
        )
    }

    private fun getSnapEntityFromName(sprite: String): SnapShot? {
        if (sprite.isNotEmpty()) {
            if(sprite.contains("skin_")) {
                val parts = sprite.split("_")
                val skinId = parts.lastOrNull()?.toIntOrNull() ?: 0

                return Skins.getSnap(skinId)
            }
            if(sprite.contains("acs_")) {
                val parts = sprite.split("_")
                val acsId = parts.lastOrNull()?.toIntOrNull() ?: 0

                return Accessories.getSnap(acsId)
            }
            if(sprite.contains("veh_")) {
                val parts = sprite.split("_")
                val vehId = parts.lastOrNull()?.toIntOrNull() ?: 0

                return Vehicles.getSnap(vehId)
            }
        }
        return null
    }

    private fun getPlateSnapFromName(sprite: String): PlateSnapShot? {
        if (sprite.isNotEmpty()) {
            if (sprite.contains("plate_")) {
                val parts = sprite.split("_")

                return try {
                    PlateSnapShot(parts[1].toInt(), parts[2], parts[3])
                } catch (e: Exception) {
                    null
                }
            }
        }
        return null
    }

    fun getColorAsStateList(sprite: String): ColorStateList {
        if (sprite.isNotEmpty()) {
            if(sprite.contains("skin_")) {
                val parts = sprite.split("_")
                val skinId = parts.lastOrNull()?.toIntOrNull() ?: 0

                return Rare.getColorAsStateList(Skins.getRare(skinId))
            }
            if(sprite.contains("acs_")) {
                val parts = sprite.split("_")
                val acsId = parts.lastOrNull()?.toIntOrNull() ?: 0

                return Rare.getColorAsStateList(Skins.getRare(Accessories.getRare(acsId)))
            }
        }

        return ColorStateList.valueOf(Color.parseColor("#00000000"))
    }
}