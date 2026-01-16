package com.holy.game.gui.tab

import android.content.Context
import android.text.Editable
import android.text.TextWatcher
import android.view.View
import android.view.inputmethod.InputMethodManager
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.TabBinding
import com.lit.game.gui.NativeGui
import com.holy.game.gui.util.LinearLayoutManagerWrapper

class Tab : NativeGui<TabBinding>(TabBinding::class) {

    private external fun nativeDeleteCppObj()

    private val mTabAdapter: com.holy.game.gui.tab.TabAdapter = com.holy.game.gui.tab.TabAdapter()

    init {
        activity.runOnUiThread {
            binding.clearSearchButton.setOnClickListener {
                clearEditText()
            }

            binding.goSearchButton.setOnClickListener {
                clearEditText()
            }

            binding.recycle.layoutManager =
                com.holy.game.gui.util.LinearLayoutManagerWrapper(activity)
            binding.recycle.adapter = mTabAdapter

            binding.searchInput.setText("")
            setVisibleIconInSearchView("")

            binding.closeButton.setOnClickListener {
                destroy()
            }

            binding.searchInput.addTextChangedListener(object : TextWatcher {
                override fun beforeTextChanged(charSequence: CharSequence, i: Int, i1: Int, i2: Int) {}
                override fun onTextChanged(charSequence: CharSequence, i: Int, i1: Int, i2: Int) {}
                override fun afterTextChanged(editable: Editable) {
                    mTabAdapter.updateSearch(editable.toString())
                    setVisibleIconInSearchView(editable.toString())
                }
            })
        }
    }

    private fun clearEditText() {
        binding.searchInput.setText("")
        setVisibleIconInSearchView("")
    }

    override fun destroy() {
        super.destroy()

        activity.runOnUiThread {
            (activity.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager).hideSoftInputFromWindow(binding.searchInput.windowToken, 0)
            nativeDeleteCppObj()
        }
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    fun setStat(id: Int, color: Int, name: String?, score: Int, ping: Int) {
        activity.runOnUiThread {
            mTabAdapter.addItem(com.holy.game.gui.tab.PlayerData(id, color, name, score, ping))
        }
    }

    fun setVisibleIconInSearchView(str: String) {
        activity.runOnUiThread {
            if (str.isEmpty()) {
                binding.goSearchButton.visibility = View.VISIBLE
                binding.clearSearchButton.visibility = View.INVISIBLE
                return@runOnUiThread
            }
            binding.goSearchButton.visibility = View.INVISIBLE
            binding.clearSearchButton.visibility = View.VISIBLE
        }
    }
}