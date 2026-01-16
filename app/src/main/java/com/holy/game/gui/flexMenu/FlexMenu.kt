package com.holy.game.gui.flexMenu

import android.annotation.SuppressLint
import android.view.View
import android.widget.TextView
import androidx.recyclerview.widget.LinearLayoutManager
import com.lit.game.R
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.FlexMenuRootBinding
import com.lit.game.gui.NativeGui
import com.lit.game.gui.util.VerticalSpaceItemDecoration

class FlexMenu : NativeGui<FlexMenuRootBinding>(FlexMenuRootBinding::class) {
    var onItemClicked: ((FlexMenuItem) -> Unit)? = null
    var onPopItemClicked: ((PopItem) -> Unit)? = null
    var onMenuClose: (() -> Unit)? = null

    private val menuTitle: TextView = binding.menuTitle
    private val adapter = FlexMenuAdapter { clickItem(it.id) }
    private val popAdapter = PopAdapter { clickPop(currentActiveItemId ?: -1, it.id) }

    private val items = mutableListOf<FlexMenuItem>()
    val popMap = mutableMapOf<Int, MutableList<PopItem>>()
    var currentActiveItemId: Int? = null

    enum class Type(val id: Int) {
        OFF(0), SCROLL_CONTENT_ITEM(1), TEXT(2), TOGGLE_CONTENT_ITEM(3);
        companion object { fun from(id: Int) = entries.firstOrNull { it.id == id } }
    }

    init {
        binding.mainLayout.visibility = View.GONE
        binding.mainLayout.alpha = 1f

        binding.btnsList.apply {
            layoutManager = LinearLayoutManager(activity)
            adapter = this@FlexMenu.adapter
            setHasFixedSize(true)
            addItemDecoration(VerticalSpaceItemDecoration(activity.resources.getDimensionPixelSize(R.dimen._3sdp)))
        }

        binding.popList.apply {
            layoutManager = LinearLayoutManager(activity)
            adapter = popAdapter
            setHasFixedSize(true)
        }

        binding.exitButt.setOnClickListener { close(true) }
    }

    fun setTitle(title: String) {
        activity.runOnUiThread {
            binding.menuTitle.text = title
        }
    }

    fun addItem(item: FlexMenuItem, popItems: List<PopItem>? = null) {
        items.add(item)
        popItems?.let { popMap[item.id] = it.toMutableList() }
    }

    fun show(type: Int, title: String) {
        activity.runOnUiThread {
            menuTitle.text = title
            when (Type.from(type)) {
                Type.SCROLL_CONTENT_ITEM, Type.TOGGLE_CONTENT_ITEM -> {
                    adapter.setItems(items)
                    val firstTab = items.firstOrNull { it.isTab } ?: items.firstOrNull()
                    firstTab?.let {
                        adapter.setActiveById(it.id)
                        popMap[it.id]?.let { showPopup(it) }
                        currentActiveItemId = it.id
                    }
                }
                Type.TEXT -> adapter.setItems(emptyList())
                else -> {}
            }
            binding.mainLayout.visibility = View.VISIBLE
            binding.mainLayout.alpha = 1f
        }
    }

    fun showPopup(items: List<PopItem>?) {
        activity.runOnUiThread {
            if (items.isNullOrEmpty()) {
                binding.popupBgFrame.visibility = View.GONE
            } else {
                popAdapter.setItems(items)
                binding.popupBgFrame.visibility = View.VISIBLE
            }
        }
    }

    fun hidePopup() { activity.runOnUiThread { binding.popupBgFrame.visibility = View.GONE } }

    fun clickItem(id: Int) {
        val item = items.firstOrNull { it.id == id } ?: return
        onItemClicked?.invoke(item)
        popMap[item.id]?.let { showPopup(it) }
        currentActiveItemId = item.id
        adapter.setActiveById(id)
    }

    @SuppressLint("NotifyDataSetChanged")
    fun clickPop(parentId: Int, popId: Int) {
        val popList = popMap[parentId] ?: return
        val popItem = popList.firstOrNull { it.id == popId } ?: return
        val oldState = popItem.isActive
        onPopItemClicked?.invoke(popItem)
        popItem.isActive = oldState
        activity.runOnUiThread {
            popAdapter.notifyDataSetChanged()
        }
        if (popItem.action == PopAction.CLOSE) hidePopup()
        else if (popItem.action == PopAction.APPLY) close(false)
    }

    @SuppressLint("NotifyDataSetChanged")
    fun updatePopState(parentId: Int, popId: Int, state: Boolean) {
        val popList = popMap[parentId] ?: return
        val popItem = popList.firstOrNull { it.id == popId } ?: return
        if (popItem.isActive != state) {
            popItem.isActive = state
            activity.runOnUiThread {
                if (currentActiveItemId == parentId && binding.popupBgFrame.visibility == View.VISIBLE) {
                    popAdapter.notifyDataSetChanged()
                }
            }
        }
    }

    fun close(animated: Boolean) {
        if (!animated) {
            destroy()
            return
        }
        activity.runOnUiThread {
            binding.mainLayout.animate()
                .alpha(0f)
                .setDuration(150)
                .withEndAction {
                    destroy()
                }.start()
        }
    }

    override fun destroy() {
        activity.runOnUiThread {
            binding.mainLayout.visibility = View.GONE
            binding.mainLayout.alpha = 1f
            binding.popupBgFrame.visibility = View.GONE
            binding.btnsList.adapter = null
            binding.popList.adapter = null
            binding.exitButt.setOnClickListener(null)
        }

        items.clear()
        popMap.clear()
        currentActiveItemId = null
        onItemClicked = null
        onPopItemClicked = null
        try { onMenuClose?.invoke() } catch (_: Throwable) {}
        onMenuClose = null
        super.destroy()
    }

    override fun receivePacket(actionId: Int, json: String) {}
}