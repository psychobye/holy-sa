package com.holy.game.gui.menu

import android.content.Intent
import android.graphics.Color
import android.net.Uri
import android.view.View
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.google.android.material.transition.MaterialSharedAxis
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.MenuActionDialogBinding
import com.holy.game.gui.NativeGui
import com.holy.game.gui.menu.fragment.MenuMainFragment
import com.holy.game.gui.menu.fragment.MenuShopFragment
import com.holy.game.gui.menu.fragment.MenuQuestFragment
import com.holy.game.gui.quest.QuestViewModel
import com.holy.game.gui.util.Utils.addPressScaleAnimation

// TODO: Google Avatar
class Menu : NativeGui<MenuActionDialogBinding>(MenuActionDialogBinding::class) {
    private external fun nativeOnExit()

    private lateinit var holyDonateText: TextView
    private lateinit var moneyText: TextView

    private val TAG_MAIN = "menu_main"
    private val TAG_SHOP = "menu_shop"
    private val TAG_QUEST = "menu_quest"

    init {
        binding.mainLayout.visibility = View.GONE

        showMainFragment()

        holyDonateText = binding.holyDonate
        moneyText = binding.money

        activity.runOnUiThread {
            addPressScaleAnimation(binding.logo)
            addPressScaleAnimation(binding.closeButton)
            addPressScaleAnimation(binding.avatar)
            addPressScaleAnimation(binding.balance)
            addPressScaleAnimation(binding.mainBtn)
            addPressScaleAnimation(binding.shopBtn)
            addPressScaleAnimation(binding.questBtn)

            listOf(binding.mainBtn, binding.shopBtn, binding.questBtn).forEach {
                it.isClickable = true
                it.isEnabled = true
            }

            binding.logo.setOnClickListener {
                val youtubeIntent = Intent(Intent.ACTION_VIEW, Uri.parse("vnd.youtube:RUsUQ9tRHHQ"))
                if (youtubeIntent.resolveActivity(activity.packageManager) != null) {
                    activity.startActivity(youtubeIntent)
                } else {
                    val webIntent = Intent(Intent.ACTION_VIEW, Uri.parse("https://www.youtube.com/watch?v=RUsUQ9tRHHQ"))
                    activity.startActivity(webIntent)
                }
            }

            binding.closeButton.setOnClickListener { close(true) }

            binding.mainBtn.setOnClickListener {
                showMainFragment()
                updateTabVisuals(binding.mainBtn, binding.shopBtn, binding.questBtn)
            }

            binding.shopBtn.setOnClickListener {
                // TODO: ShopAcitivity (donate)
                // showShopFragment()
                // updateTabVisuals(binding.shopBtn, binding.mainBtn, binding.questBtn)
            }

            binding.questBtn.setOnClickListener {
                // TODO: QuestAcitivity
                // showQuestFragment()
                // updateTabVisuals(binding.questBtn, binding.mainBtn, binding.shopBtn)
            }

            updateTabVisuals(binding.mainBtn, binding.shopBtn, binding.questBtn)
        }
    }

    private fun updateTabVisuals(selected: TextView, vararg others: TextView) {
        val ctx = (activity as? AppCompatActivity) ?: return
        val colorWhite = ContextCompat.getColor(ctx, R.color.white)
        val colorGray = ContextCompat.getColor(ctx, R.color.gray)

        selected.isSelected = true
        selected.setTextColor(colorWhite)

        for (t in others) {
            t.isSelected = false
            t.setTextColor(colorGray)
        }
    }

    private fun transactFragment(fragment: Fragment, tag: String) {
        val act = activity as? AppCompatActivity ?: return
        act.runOnUiThread {
            val fm = act.supportFragmentManager

            val transaction = fm.beginTransaction().setReorderingAllowed(true)

            listOf(TAG_MAIN, TAG_SHOP, TAG_QUEST).forEach { t ->
                fm.findFragmentByTag(t)?.let { transaction.hide(it) }
            }

            val existing = fm.findFragmentByTag(tag)
            if (existing != null) {
                transaction.show(existing)
            } else {
                transaction.add(R.id.fragment_container, fragment, tag)
            }

            transaction.commitAllowingStateLoss()
        }
    }

    fun showMainFragment() {
        val fragment = MenuMainFragment()
        fragment.controller = object : MenuController {
            override fun destroyMenu() {
                destroy()
            }
        }
        transactFragment(fragment, TAG_MAIN)
    }

    fun show(donate: Int, money: Int, totalHours: Float, level: Int, exp: Int, expMax: Int, familyName: String, r: Int, g: Int, b: Int) {
        activity.runOnUiThread {
            binding.holyDonate.text = Samp.formatter.format(donate.toLong())
            binding.money.text = Samp.formatter.format(money.toLong())
            binding.mainLayout.visibility = View.VISIBLE

            val act = activity as? AppCompatActivity ?: return@runOnUiThread
            val vm = ViewModelProvider(act).get(MenuViewModel::class.java)

            vm.time.postValue(totalHours)
            vm.level.postValue(level)
            vm.exp.postValue(exp)
            vm.expMax.postValue(expMax)
            vm.family.postValue(familyName)
            vm.familyColor.postValue(Triple(r, g, b))
        }
    }

    fun close(animated: Boolean) {
        if (!animated) {
            binding.mainLayout.visibility = View.GONE
            destroy()
            return
        }

        binding.mainLayout.apply {
            animate()
                .alpha(0f)
                .setDuration(150)
                .withEndAction {
                    alpha = 1f
                    visibility = View.GONE
                    destroy()
                }
                .start()
        }
    }

    private fun clearMenuFragments() {
        val act = activity as? AppCompatActivity ?: return
        act.runOnUiThread {
            val fm = act.supportFragmentManager
            listOf(TAG_MAIN, TAG_SHOP, TAG_QUEST).forEach { tag ->
                fm.findFragmentByTag(tag)?.let { fm.beginTransaction().remove(it).commitAllowingStateLoss() }
            }
            fm.executePendingTransactions()
        }
    }

    override fun destroy() {
        activity.runOnUiThread {
            binding.mainLayout.visibility = View.GONE
        }

        clearMenuFragments()

        nativeOnExit()
        super.destroy()
    }

    override fun receivePacket(actionId: Int, json: String) {
        TODO("Not yet implemented")
    }
}