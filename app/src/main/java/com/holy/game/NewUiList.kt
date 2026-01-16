package com.holy.game

import com.holy.game.core.Samp.Companion.activity
import com.holy.game.gui.AdminRecon
import com.holy.game.gui.ArmyGame
import com.holy.game.gui.NativeGui
import kotlin.reflect.KClass


object NewUiList {
    const val ACTION_CREATE     = -1
    const val ACTION_DESTROY    = -2

    // если нужно временно скрыть не удаляя
    const val ACTION_SHOW       = -3
    const val ACTION_HIDE       = -4

    const val ACTION_ONCLOSED   = -5
    const val ACTION_GOTOP      = -6

    private val list: HashMap<Int, KClass<out Any>> = hashMapOf(
        0 to AdminRecon::class,
        1 to ArmyGame::class,
    )

    private val instances: HashMap<Int, Any> = hashMapOf()

    private fun <T : Any> getInstance(id: Int): T? {
        val instance = instances[id]
        if (instance != null) {
            println(" instance ready. return")
            return instance as T
        }

        val clazz = list[id]
        if(clazz == null) {
            println("Class not found for ID: $id")
            return null
        }
        val newInst = clazz.java.newInstance() as T
        instances[id] = newInst
        return newInst
    }

    private fun deleteIfExists(id: Int) {
        instances[id]?.let {
            (it as NativeGui<*>).destroy()
            instances.remove(id)
        }
    }

    @JvmStatic
    fun getUiId(targetClass: Any): Int {
        return instances.entries.find { it.value == targetClass }?.key ?: -999
    }

    @JvmStatic
    fun receiveUiPacket(uiId: Int, actionId: Int, json: String = "") {
        activity.runOnUiThread {
            if (actionId == ACTION_CREATE) {
                getInstance<NativeGui<*>>(uiId) ?: return@runOnUiThread // constructor
                return@runOnUiThread
            }
            if (actionId == ACTION_DESTROY) {
                deleteIfExists(uiId)
            }
            if(actionId == ACTION_HIDE){
                val instance = getInstance<NativeGui<*>>(uiId)
                instance?.toggle(false)
            }
            if(actionId == ACTION_SHOW){
                val instance = getInstance<NativeGui<*>>(uiId)
                instance?.toggle(true)
            }

            else {
                val instance = getInstance<NativeGui<*>>(uiId) ?: return@runOnUiThread

                instance.receivePacket(actionId, json)
            }
        }
    }

}