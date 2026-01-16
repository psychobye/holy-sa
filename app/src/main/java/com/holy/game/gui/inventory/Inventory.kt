package com.holy.game.gui.inventory

import android.view.View
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.GridLayoutManager
import com.holy.game.EntitySnaps
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.InventoryBinding
import com.holy.game.gui.NativeGui
import com.holy.data.Rare
import com.holy.data.skins.Skins

class Inventory : InventoryListener, NativeGui<InventoryBinding>(InventoryBinding::class) {
    private val MATRIX_NONE         = 0
    private val MATRIX_PLAYER       = 1
    private val MATRIX_MAIN         = 2
    private val MATRIX_ADDITIONAL   = 3

    private var currSelectedMat  = -1
    private var currSelectedPos  = -1

    private val MAX_ITEMS_IN_MAIN_MATRIX = 4 * 12
    private val MAX_ITEMS_IN_PLAYER_MATRIX = 2 * 6
    private val MAX_ITEMS_IN_ADDITIONAL_MATRIX = 3 * 49


    private val matrices: ArrayList<InventoryAdapter> = arrayListOf(
        InventoryAdapter(MATRIX_NONE,       arrayListOf(), this),
        InventoryAdapter(MATRIX_PLAYER,     List(MAX_ITEMS_IN_PLAYER_MATRIX)        { InventoryItem() }, this),
        InventoryAdapter(MATRIX_MAIN,       List(MAX_ITEMS_IN_MAIN_MATRIX)          { InventoryItem() }, this),
        InventoryAdapter(MATRIX_ADDITIONAL, List(MAX_ITEMS_IN_ADDITIONAL_MATRIX)    { InventoryItem() }, this)
    )

    private external fun sendSelectItem(matrixId: Int, pos: Int)
    private external fun sendClickButton(buttonId: Int)

    init {
        activity.runOnUiThread {
            // init PLayer
            binding.playerMatrixRecycle.layoutManager = GridLayoutManager(activity, 2)
            binding.playerMatrixRecycle.adapter = matrices[MATRIX_PLAYER]

            // init Main
            binding.myMatrixRecycle.layoutManager = GridLayoutManager(activity, 4)
            binding.myMatrixRecycle.adapter = matrices[MATRIX_MAIN]

            // init Additional
            binding.additionalMatrixRecycle.layoutManager = GridLayoutManager(activity, 3)
            binding.additionalMatrixRecycle.adapter = matrices[MATRIX_ADDITIONAL]

            // кнопка удалить
            binding.invDelButt.setOnClickListener {
                sendClickButton(INVENTAR_BUTTON_DELETE)
            }

            //кнопка продать
            binding.invSellButt.setOnClickListener {
                sendClickButton(INVENTAR_BUTTON_TRATE)
            }

            // кнопка Использовать
            binding.invUseButt.setOnClickListener {
                sendClickButton(INVENTAR_BUTTON_USE)
            }

            // Кнопка инфо
            binding.invInfButt.setOnClickListener {
                sendClickButton(INVENTAR_BUTTON_INFO)
            }

            binding.playerMainIcon.setOnClickListener {
                sendClickButton(INVENTAR_BUTTON_MY_SKIN)
            }

            binding.exitButt.setOnClickListener {
                sendClickButton(INVENTAR_BUTTON_EXIT)
            }

        }
    }

    private fun itemToggleActive(matrixId: Int, pos: Int, active: Boolean){
        activity.runOnUiThread {
            //if(currSelectedMat == matrixId && currSelectedPos == pos && !active)
                resetSelected()

            if(active)
                setSelected(matrixId, pos)
        }
    }

    private fun resetSelected() {
        if(currSelectedMat != -1 && currSelectedPos != -1) {
            matrices[currSelectedMat].selectedItemId = -1
            matrices[currSelectedMat].notifyItemChanged(currSelectedPos)

            currSelectedMat = -1
            currSelectedPos = -1

            binding.myButtonsLayout.visibility = View.GONE
        }
    }

    private fun setSelected(matrixId: Int, pos: Int) {
        val matrix = matrices[matrixId]

        matrix.selectedItemId = pos
        currSelectedMat = matrixId
        currSelectedPos = pos
        matrix.notifyItemChanged(pos)

        binding.myButtonsLayout.visibility = View.VISIBLE
    }

    private fun updateItem(matrixId: Int, pos: Int, sprite: String, caption: String, count: String, rare: Int, active: Boolean) {
        activity.runOnUiThread {
            val matrix = matrices[matrixId]
            val item = matrix.list[pos]

            item.sprite = sprite

            item.caption = caption
            item.count = count
            item.rareColor = Rare.getColorAsStateList(rare)

            if(currSelectedMat == matrixId && currSelectedPos == pos && !active)
                resetSelected()

            if(active) {
                setSelected(matrixId, pos)
            }
            matrix.notifyItemChanged(pos)
            return@runOnUiThread
        }

    }

    private fun updateCarryng(matrixId: Int, carryng: String) {
        activity.runOnUiThread {
            if(matrixId == MATRIX_MAIN)
                binding.myMassText.text = carryng

            if(matrixId == MATRIX_ADDITIONAL)
                binding.additionalMassText.text = carryng
        }
    }

    private fun toggleShow(toggle: Boolean, nick: String, id: Int, health: Float, armour: Float, satiety: Int, skinId: Int) {
        activity.runOnUiThread {
            if (toggle) {
                EntitySnaps.loadEntitySnapToImageView(
                    Skins.getSnap(skinId),
                    binding.playerMainIcon
                )
                binding.healthText.text = "%.0f".format(health)
                binding.armourText.text = "%.0f".format(armour)

                binding.satietyText.text = "$satiety"
              //  binding.playerMatCaption.text = "$nick (ID: $id)"
            }
        }
    }

    private fun toggleAdditional(toggle: Boolean, caption: String) {
        activity.runOnUiThread {
            if(toggle) {
                binding.additionalLayout.visibility = View.VISIBLE
                binding.additionalTopCaption.text = caption

                val layoutParams = binding.exitButt.layoutParams as ConstraintLayout.LayoutParams
                layoutParams.startToEnd = binding.additionalLayout.id
                binding.exitButt.layoutParams = layoutParams
            }
            else {
                binding.additionalLayout.visibility = View.GONE

                val layoutParams = binding.exitButt.layoutParams as ConstraintLayout.LayoutParams
                layoutParams.startToEnd = binding.myLayout.id
                binding.exitButt.layoutParams = layoutParams
            }
        }
    }

    companion object {
        const val INVENTAR_BUTTON_TRATE = 1
        const val INVENTAR_BUTTON_DELETE = 2
        const val INVENTAR_BUTTON_USE = 3
        const val INVENTAR_BUTTON_INFO = 4
        const val INVENTAR_BUTTON_MY_SKIN = 5
        const val INVENTAR_BUTTON_EXIT = 6
    }

    override fun onSelectedItem(matrixId: Int, itemId: Int) {
        sendSelectItem(matrixId, itemId)
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}