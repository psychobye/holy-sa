package com.holy.game.gui.magicStore

import android.view.View
import android.widget.TextView
import androidx.recyclerview.widget.LinearLayoutManager
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.MagicStoreBinding
import com.lit.game.gui.NativeGui

class MagicStore : NativeGui<MagicStoreBinding>(MagicStoreBinding::class), MagicStoreListener {
    val CLICK_BUY       = 0
    val CLICK_INFO      = 1
    val CLICK_BRONZE    = 2
    val CLICK_SILVER    = 3
    val CLICK_GOLD      = 4

    private var activeCategory = 0
    private lateinit var magicStoreAdapter: MagicStoreAdapter

    private external fun nativeOnClose()
    private external fun nativeSendClick(clickType: Int, index: Int = 0, category: Int = 0)

    init {
        activity.runOnUiThread {
            binding.catItems.setOnClickListener {
                setActiveCategory(MagicStoreItems.CATEGORY_ITEMS, binding.catItems)
            }

            binding.catAnimals.setOnClickListener {
                setActiveCategory(MagicStoreItems.CATEGORY_ANIMALS, binding.catAnimals)
            }

            binding.catPotions.setOnClickListener {
                setActiveCategory(MagicStoreItems.CATEGORY_POTIONS, binding.catPotions)
            }

            binding.catConsumables.setOnClickListener {
                setActiveCategory(MagicStoreItems.CATEGORY_CONSUMABLES, binding.catConsumables)
            }

            binding.catSkins.setOnClickListener {
                setActiveCategory(MagicStoreItems.CATEGORY_SKINS, binding.catSkins)
            }

            binding.exitButt.setOnClickListener {
                destroy()
            }

            binding.buyButton.setOnClickListener {
                val id = magicStoreAdapter.selectedItem
                if(id < 0) // crash
                    return@setOnClickListener
                nativeSendClick(CLICK_BUY, id, magicStoreAdapter.list[id].category)
            }

            binding.infoButt.setOnClickListener {
                val id = magicStoreAdapter.selectedItem
                if(id < 0) // crash
                    return@setOnClickListener
                nativeSendClick(CLICK_INFO, id, magicStoreAdapter.list[id].category)
            }

            binding.bronzeLayout.setOnClickListener {
                nativeSendClick(CLICK_BRONZE)
            }

            binding.silverLayout.setOnClickListener {
                nativeSendClick(CLICK_SILVER)
            }

            binding.goldLayout.setOnClickListener {
                nativeSendClick(CLICK_GOLD)
            }

            magicStoreAdapter = MagicStoreAdapter(mutableListOf(), this)

            binding.recycle.layoutManager = LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false)
            binding.recycle.adapter = magicStoreAdapter

            setActiveCategory(MagicStoreItems.CATEGORY_ITEMS, binding.catItems)
        }
    }
    private fun setActiveCategory(category: Int, view: TextView) {
        activity.runOnUiThread {
            activeCategory = category

            resetAllSelected()

            view.setBackgroundResource(R.drawable.magicstore_cat_active_bg)

            magicStoreAdapter.list.clear()

            magicStoreAdapter.selectedItem = -1
            onToggleButtons(false)

            for (item in MagicStoreItems.list) {
                if (item.category == category) {
                    magicStoreAdapter.list.add(item)
                }
            }
            magicStoreAdapter.notifyDataSetChanged()
        }
    }

    private fun resetAllSelected() {
        activity.runOnUiThread {
            binding.catItems.setBackgroundResource(0)
            binding.catAnimals.setBackgroundResource(0)
            binding.catConsumables.setBackgroundResource(0)
            binding.catPotions.setBackgroundResource(0)
            binding.catSkins.setBackgroundResource(0)
        }
    }

    override fun destroy() {
        super.destroy()

        nativeOnClose()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    override fun onToggleButtons(toggle: Boolean) {
        if(toggle)
            binding.buttonsLayout.visibility = View.VISIBLE
        else
            binding.buttonsLayout.visibility = View.GONE
    }

    private fun updateBalance(bronze: Int, silver: Int, gold:Int) {
        activity.runOnUiThread {
            binding.bronzeText.text = String.format("%s", Samp.formatter.format(bronze))
            binding.silverText.text = String.format("%s", Samp.formatter.format(silver))
            binding.goldText.text = String.format("%s", Samp.formatter.format(gold))
        }
    }
}