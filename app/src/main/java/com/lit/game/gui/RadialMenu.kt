package com.lit.game.gui

import com.lit.data.vehicles.Vehicles
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.gui.flexMenu.PopAction
import com.lit.game.gui.flexMenu.PopItem
import com.lit.game.gui.flexMenu.FlexMenu
import com.lit.game.gui.flexMenu.FlexMenuItem
import java.lang.ref.WeakReference

object RadialMenu {
    private var menuRef: WeakReference<FlexMenu>? = null
    private val stateCache = HashMap<Int, Boolean>()
    private var lastVehicleId: Int = 0
    private var lastTitle: String = ""

    private external fun nativeOnClose()
    private external fun nativeRequestUpdate()

    @JvmStatic
    fun show() {
        menuRef?.get()?.destroy()
        menuRef = null

        val newMenu = FlexMenu()
        menuRef = WeakReference(newMenu)

        if (lastTitle.isNotEmpty()) newMenu.setTitle(lastTitle)

        newMenu.onPopItemClicked = { pop ->
            when (pop.action) {
                PopAction.TOGGLE -> {
                    when (pop.id) {
                        101 -> Samp.sendCommand("/light")
                        102 -> Samp.sendCommand("/farlight")
                        103 -> Samp.sendCommand("/strobes")
                        104 -> Samp.sendCommand("/neon")
                        201 -> Samp.sendCommand("/e")
                        301 -> Samp.sendCommand("/lock")
                    }
                }
                PopAction.APPLY -> {
                    when (pop.id) {
                        401 -> Samp.sendCommand("/music")
                    }
                }
                PopAction.CLOSE -> menuRef?.get()?.hidePopup()
                PopAction.CUSTOM -> TODO()
                null -> TODO()
            }
            nativeRequestUpdate()
        }

        newMenu.onMenuClose = {
            stateCache.clear()
            nativeOnClose()
            menuRef = null
        }

        setupMenu(newMenu)
        newMenu.show(FlexMenu.Type.SCROLL_CONTENT_ITEM.id, lastTitle)
        nativeRequestUpdate()
    }

    @JvmStatic
    fun update(
        modelId: Int,
        lock: Boolean, engine: Boolean, light: Boolean,
        nightLight: Boolean, strob: Boolean, neon: Boolean,
        bonnet: Boolean, boot: Boolean
    ) {
        val currentMenu = menuRef?.get() ?: return

        if (lastVehicleId != modelId) {
            lastVehicleId = modelId
            val name = Vehicles.getName(modelId)
            lastTitle = name

            activity.runOnUiThread {
                currentMenu.setTitle(name)
            }
        }

        fun sync(parentId: Int, id: Int, state: Boolean) {
            if (stateCache[id] != state) {
                stateCache[id] = state
                currentMenu.updatePopState(parentId, id, state)
            }
        }

        sync(1, 101, light)
        sync(1, 102, nightLight)
        sync(1, 103, strob)
        sync(1, 104, neon)
        sync(2, 201, engine)
        sync(3, 301, lock)
    }

    private fun setupMenu(m: FlexMenu) {
        m.addItem(FlexMenuItem(1, "Lights", R.drawable.ic_lamp, true, true), listOf(
            PopItem(101, "Light", R.drawable.ic_light, null, PopAction.TOGGLE, false),
            PopItem(102, "NightLights", R.drawable.ic_farlight, null, PopAction.TOGGLE, false),
            PopItem(103, "Strob", R.drawable.ic_strobe, null, PopAction.TOGGLE, false),
            PopItem(104, "Neon", R.drawable.ic_neon, null, PopAction.TOGGLE, false)
        ))
        m.addItem(FlexMenuItem(2, "Engine", R.drawable.ic_engine, false, true), listOf(
            PopItem(201, "Engine", R.drawable.ic_key, null, PopAction.TOGGLE, false),
        ))
        m.addItem(FlexMenuItem(3, "Doors", R.drawable.ic_car_door, false, true), listOf(
            PopItem(301, "Lock", R.drawable.ic_lock, null, PopAction.TOGGLE, false),
        ))
        m.addItem(FlexMenuItem(4, "Music", R.drawable.ic_boombox, false, true), listOf(
            PopItem(401, "Radio", R.drawable.ic_live, null, PopAction.APPLY, false)
        ))
    }
}