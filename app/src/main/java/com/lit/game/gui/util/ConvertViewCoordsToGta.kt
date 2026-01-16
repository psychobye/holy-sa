package com.lit.game.gui.util

object ConvertViewCoordsToGta {

    class Data (
        var x: Float = 0.0f,
        var y: Float = 0.0f,
        var width: Float = 0.0f,
        var height: Float = width
    )

    fun convertCoordsToGta(data: Data) : Data {
        val returnData = Data()

        val screenWidth = Utils.screenSize.first.toFloat()
        val screenHeight = Utils.screenSize.second.toFloat()

        // calc view cords in percent
        val viewPercentX = (data.x + (data.width / 2f) ) / screenWidth
        val viewPercentY = (data.y + (data.height / 2.2f) ) / screenHeight

        returnData.x = 640f * viewPercentX
        returnData.y = 480f * viewPercentY

        val viewPercentWidth    = data.width / 2 / screenWidth
        val viewPercentHeight   = data.height / 3.333f / screenHeight

        returnData.width = 640f * viewPercentWidth /* 0.96f*/
        returnData.height = 480f * viewPercentHeight

        return returnData
    }
}