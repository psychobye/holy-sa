package com.holy.game.gui

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import androidx.viewbinding.ViewBinding
import com.lit.game.NewUiList
import com.lit.game.R
import com.lit.game.core.Samp.Companion.activity
import kotlin.reflect.KClass


abstract class NativeGui<T : ViewBinding>(private var bindingClass: KClass<T>) {

    private val rootView by lazy { activity.findViewById<FrameLayout>(R.id.ui_layout) }
    private val inflater by lazy { LayoutInflater.from(activity) }

    protected val binding: T by lazy {
        val inflateMethod = bindingClass.java.getMethod("inflate", LayoutInflater::class.java, ViewGroup::class.java, Boolean::class.java)
        inflateMethod.invoke(null, inflater, rootView, false) as T
    }
    private val viewToAdd by lazy { binding.root }

    private val uiId by lazy { NewUiList.getUiId(this) }

    init {
        activity.runOnUiThread {
            rootView.addView(viewToAdd)
        }
    }

    open fun destroy() {
        activity.runOnUiThread {
            rootView.removeView(viewToAdd)

            sendPacket(NewUiList.ACTION_ONCLOSED, "")
        }
    }

    abstract fun receivePacket(actionId: Int, json: String)

    fun sendPacket(actionId: Int, json: String = "") {
        nativeSendPacket(uiId, actionId, json)
    }

    open fun toggle(show: Boolean) {
        activity.runOnUiThread {
            if(show)
                viewToAdd.visibility = View.VISIBLE
            else
                viewToAdd.visibility = View.GONE
        }
    }

    companion object {
        external fun nativeSendPacket(uiId: Int, actionId: Int, data: String)
    }
}

