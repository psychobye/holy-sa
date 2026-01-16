package com.holy.launcher.async.dto.response

import android.app.Activity
import android.content.Context
import com.holy.launcher.NetworkService
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import java.io.ObjectInputStream
import java.io.ObjectOutputStream
import java.io.Serializable

interface MonitoringDataLoaderListener {
    fun monitoringDataLoadedSuccess()
}


data class MonitoringData(
    var news: List<com.holy.launcher.async.dto.response.News> = listOf(),
    var servers: List<Servers> = listOf()
) : Serializable

object ServersList {
    val file = "servers.dat"

    var news: List<com.holy.launcher.async.dto.response.News> = listOf()
    var servers: List<Servers> = listOf()

    fun load(activity: Activity, net: NetworkService, listener: MonitoringDataLoaderListener) {
        val call = net.monitoringData

        call?.enqueue(object : Callback<MonitoringData?> {
            override fun onResponse(call: Call<MonitoringData?>, response: Response<MonitoringData?>) {
                if (response.isSuccessful) {
                    val monitoringData = response.body()

                    news = monitoringData?.news!!
                    servers = monitoringData.servers

                    saveToFile(activity, monitoringData)

                    listener.monitoringDataLoadedSuccess()
                    return
                }
                loadFromFile(activity, listener)
            }

            override fun onFailure(call: Call<MonitoringData?>, t: Throwable) {
                loadFromFile(activity, listener)
            }
        })
    }

    fun saveToFile(context: Context, data: MonitoringData) {
        try {
            val fileOutputStream = context.openFileOutput(file, Context.MODE_PRIVATE)
            val objectOutputStream = ObjectOutputStream(fileOutputStream)
            objectOutputStream.writeObject(data)
            objectOutputStream.close()
            fileOutputStream.close()
        } catch (ex: Exception) {
            ex.printStackTrace()
        }
    }


    fun loadFromFile(context: Context, listener: MonitoringDataLoaderListener) {
        try {
            val fileInputStream = context.openFileInput(file)
            val objectInputStream = ObjectInputStream(fileInputStream)
            val data = objectInputStream.readObject() as MonitoringData
            objectInputStream.close()
            fileInputStream.close()

            news = data.news
            servers = data.servers

            listener.monitoringDataLoadedSuccess()
        } catch (ex: Exception) {
            ex.printStackTrace()
        }
    }
}
