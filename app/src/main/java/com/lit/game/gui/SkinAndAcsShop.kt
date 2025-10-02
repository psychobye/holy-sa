package com.lit.game.gui

import android.view.View
import android.view.ViewStub
import android.widget.ImageView
import android.widget.TextView
import com.google.android.material.button.MaterialButton
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity

class SkinAndAcsShop : NativeGuiWrapper {
    private var shopstore_buyinfo: TextView? = null
    private var shopstore_left: ImageView? = null
    private var shopstore_right: ImageView? = null
    private var shopstore_buy: MaterialButton? = null
    private var shopstore_exit: MaterialButton? = null
    private var shopstore_camera: ImageView? = null

    external fun nativeSendClick(clickId: Int)

    private val skinShopStub = activity.findViewById<ViewStub>(R.id.skinShopStub)
    private var skinShopInflated: View? = null

    init {
        activity.runOnUiThread {
            skinShopInflated = skinShopStub.inflate()

            shopstore_buyinfo = activity.findViewById(R.id.shopstore_buyinfo)
            shopstore_left = activity.findViewById(R.id.shopstore_left)
            shopstore_right = activity.findViewById(R.id.shopstore_right)
            shopstore_buy = activity.findViewById(R.id.shopstore_buy)
            shopstore_exit = activity.findViewById(R.id.shopstore_exit)
            shopstore_camera = activity.findViewById(R.id.shopstore_camera)

            shopstore_left?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                nativeSendClick(0)
            }
            shopstore_right?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                nativeSendClick(3)
            }
            shopstore_buy?.setOnClickListener { view: View ->
                view.startAnimation(Samp.clickAnim)
                nativeSendClick(1)
            }
            shopstore_exit?.setOnClickListener { nativeSendClick(2) }
            shopstore_camera?.setOnClickListener { nativeSendClick(4) }
        }
    }

    fun update(type: Int, price: Int) {
        activity.runOnUiThread {
            val price = String.format("%s руб.", Samp.formatter.format(price.toLong()))
            shopstore_buyinfo?.text = price
            if (type == 0) {
                shopstore_camera?.visibility = View.VISIBLE
            }
            else {
                shopstore_camera?.visibility = View.GONE
            }
        }
    }

    override fun destroy() {
        activity.runOnUiThread {
            shopstore_left?.setOnClickListener(null)
            shopstore_right?.setOnClickListener(null)
            shopstore_buy?.setOnClickListener(null)
            shopstore_exit?.setOnClickListener(null)
            shopstore_camera?.setOnClickListener(null)

            shopstore_buyinfo = null
            shopstore_left = null
            shopstore_right = null
            shopstore_buy = null
            shopstore_exit = null
            shopstore_camera = null

            Samp.deInflatedViewStud(skinShopInflated, R.id.skinShopStub, R.layout.shopstoreselect)

            //System.gc()
        }
    }
}