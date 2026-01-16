package com.lit.game.gui.hud

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.factor.bouncy.BouncyRecyclerView
import com.lit.game.R
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.BinderBinding
import com.lit.game.gui.NativeGui
import com.lit.game.gui.styling.ColorPicker
import com.lit.game.gui.styling.ColorPickerListener
import java.io.ObjectInputStream
import java.io.ObjectOutputStream
import java.io.UnsupportedEncodingException


interface BinderAdapterListener {
    fun onSelectedItem(text: String)
}

class Binder : NativeGui<BinderBinding>(BinderBinding::class), BinderAdapterListener{
    val adapter = BinderAdapter(this)

    init {

        val mLayoutManager: LinearLayoutManager = GridLayoutManager(activity, 3)
        binding.recycler.layoutManager = mLayoutManager
        binding.recycler.orientation = LinearLayoutManager.VERTICAL
        binding.recycler.adapter = adapter

        binding.addButton.setOnClickListener {
            activity.runOnUiThread {
                binding.addInput.setText("")
                binding.addLayout.visibility = View.VISIBLE
            }
        }

        binding.bindingAddSucces.setOnClickListener { view: View? ->
            val input = binding.addInput

            if (input.text.length > 400) {
                Toast.makeText(activity, "Не больше 400 символов!", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            var count = 0
            for (i in 0 until input.text.length) {
                if (input.text[i] == ';') {
                    count++
                }
            }
            if (count > 4) {
                Toast.makeText(activity, "Не более 4-х сообщений!", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            adapter.addItem(BinderItem(input.text.toString(), -0xff6978))
            binding.addLayout.visibility = View.GONE
        }

        binding.exitButton.setOnClickListener {
            destroy()
        }
    }

    class BinderAdapter(val listener: BinderAdapterListener) : BouncyRecyclerView.Adapter<BinderAdapter.ViewHolder>() {
        var lastItemId = 0

        val BINDER_FILE_NAME = "binder.dat"
        private val inflater: LayoutInflater
        var list: MutableList<BinderItem> = ArrayList()

        init {
            loadData()
            // this.list = list;
            inflater = LayoutInflater.from(activity)
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            val view = inflater.inflate(R.layout.binder_item, parent, false)
            return ViewHolder(view)
        }

        override fun getItemCount(): Int {
            return list.size
        }

        override fun onBindViewHolder(holder: ViewHolder, pos: Int) {
            holder.binder_text.text = list[pos].text
            holder.binder_bubble.setColorFilter(list[pos].color)
        }

        override fun onItemMoved(fromPosition: Int, toPosition: Int) {
            //example of handling reorder
            val item = list.removeAt(fromPosition)
            list.add(toPosition, item)
            notifyItemMoved(fromPosition, toPosition)
            Toast.makeText(activity, "Перемещено", Toast.LENGTH_SHORT).show()
            saveData()
        }

        override fun onItemReleased(viewHolder: RecyclerView.ViewHolder) {}
        override fun onItemSelected(viewHolder: RecyclerView.ViewHolder?) {}
        fun changeColor(color: Int) {
            try {
                list[lastItemId].color = color
                notifyItemChanged(lastItemId)
                saveData()
            }
            catch (_: Exception){}
        }

        override fun onItemSwipedToEnd(viewHolder: RecyclerView.ViewHolder, i: Int) {
            lastItemId = i

            ColorPicker(object : ColorPickerListener {
                override fun onColorPickerSelected(color: Int) {
                    changeColor(ColorPicker.rgbaToArgb(color))
                }

                override fun onColorPickerChange(color: Int) {
                    changeColor(ColorPicker.rgbaToArgb(color))
                }

                override fun onColorPickerClose() {
                    // обработка закрытия
                }
            }).show(false, false, ColorPicker.argbToRgba(list[lastItemId].color))
        }

        override fun onItemSwipedToStart(viewHolder: RecyclerView.ViewHolder, i: Int) {
            list.removeAt(i)
            notifyItemRemoved(i)
            Toast.makeText(activity, "Удалено", Toast.LENGTH_SHORT).show()
            saveData()
        }

        inner class ViewHolder internal constructor(view: View) : RecyclerView.ViewHolder(view) {
            val binder_text: TextView
            val binder_bubble: ImageView

            init {
                binder_text = view.findViewById(R.id.binder_text)
                binder_bubble = view.findViewById(R.id.binder_bubble)
                view.setOnClickListener { view1: View? ->
                    val pos = bindingAdapterPosition
                    listener.onSelectedItem(list[pos].text)

                }
            }
        }

        private fun loadData() {
            try {
                ObjectInputStream(activity.openFileInput(BINDER_FILE_NAME)).use { ois ->
                    list = ois.readObject() as ArrayList<BinderItem>
                }
            } catch (ex: Exception) {
                println(ex.message)
            }
        }

        private fun saveData() {
            try {
                ObjectOutputStream(activity.openFileOutput(BINDER_FILE_NAME, Context.MODE_PRIVATE)).use { oos ->
                    oos.writeObject(list)
                }
            } catch (ex: Exception) {
                println(ex.message)
            }
        }

        fun addItem(item: BinderItem) {
            list.add(item)
            notifyItemInserted(itemCount)
            saveData()
        }
    }

    override fun onSelectedItem(text: String) {
        destroy()

        val messages = text.split(";".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
        var messageCount = 0
        for (message in messages) {
            try {
                messageCount++
                if (messageCount > 4)
                    return

                Chat.SendChatMessage(message.toByteArray(charset("windows-1251")))
            } catch (e: UnsupportedEncodingException) {
                throw RuntimeException(e)
            }
        }
        Chat.chat_input.text.clear()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }
}
