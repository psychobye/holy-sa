package com.holy.game.gui

object PlayerInfo {
    @JvmStatic private external fun nativeGetId(): Int
    @JvmStatic private external fun nativeGetName(): String
    @JvmStatic private external fun nativeGetModelId(): Int
    @JvmStatic private external fun nativeGetHealth(): Float
    @JvmStatic private external fun nativeGetArmour(): Float
    @JvmStatic private external fun nativeGetX(): Double
    @JvmStatic private external fun nativeGetY(): Double
    @JvmStatic private external fun nativeGetZ(): Double

    val id: Int
        get() = nativeGetId()

    val name: String
        get() = nativeGetName()

    val modelId: Int
        get() = nativeGetModelId()

    val health: Float
        get() = nativeGetHealth()

    val armour: Float
        get() = nativeGetArmour()

    val position: Triple<Double, Double, Double>
        get() = Triple(nativeGetX(), nativeGetY(), nativeGetZ())

    val basic: Map<String, Any>
        get() = mapOf(
            "id" to id,
            "name" to name,
            "modelId" to modelId
        )

    val stats: Map<String, Any>
        get() = mapOf(
            "health" to health,
            "armour" to armour,
        )
}