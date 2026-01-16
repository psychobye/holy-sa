package com.lit.game.gui.battle_pass

import android.os.Handler
import android.os.Looper
import android.view.View
import android.view.ViewTreeObserver
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.LinearLayoutManager
import com.lit.game.R
import com.lit.game.core.Samp
import com.lit.game.core.Samp.Companion.activity
import com.lit.game.databinding.BattlepassBinding
import com.lit.game.databinding.BattlepassMainBinding
import com.lit.game.databinding.BattlepassRateBinding
import com.lit.game.databinding.BattlepassTasksBinding
import com.lit.game.gui.NativeGui
import com.lit.game.gui.battle_pass.rate.BattlePassRateAdapter
import com.lit.game.gui.battle_pass.rate.BattlePassRateItem
import com.lit.game.gui.battle_pass.tasks.BattlePassTaskItem
import com.lit.game.gui.battle_pass.tasks.BattlePassTasksAdapter
import com.lit.game.gui.battle_pass.tasks.BattlePassTasksAdapterListener
import com.lit.game.gui.util.Utils
import com.lit.data.Rare
import java.text.SimpleDateFormat
import java.util.Calendar
import java.util.Date
import java.util.Locale
import java.util.TimeZone

class BattlePass
    : NativeGui<BattlepassBinding>(BattlepassBinding::class), BattlePassMainAdapterListener, BattlePassTasksAdapterListener {

    enum class ePacketType {
        // receive
        SHOW,
        UPDATE_MAIN_ITEM,
        UPDATE_TASK_ITEM,
        UPDATE_RATE_ITEM,
        UPDATE_DATA,
        SHOW_BUY_PAGE,

        // send
        CLICK_BUY_LEVELS,
        CLICK_GIFT,
        CLICK_BUY_PAGE,
        CLICK_MAIN_ITEM,
        CLICK_TASK_ITEM
    };

    enum class Pages {
        MAIN,
        TASKS,
        RATE
    }

    // отсчет времени обновления задач
    private lateinit var handler: Handler
    private lateinit var runnable: Runnable
    private var tasksUpdateTime = 0L

    private external fun nativeOnClose()
    private external fun nativeOnClick(clickType: Int, index: Int = -1)

    private val mainAdapter = BattlePassMainAdapter(this)
    private val tasksAdapter = BattlePassTasksAdapter(this)
    private val rateAdapter = BattlePassRateAdapter()

    private val rateGlobalList: HashMap<Int, BattlePassRateItem> = hashMapOf()
    private val rateNoDonateList: HashMap<Int, BattlePassRateItem> = hashMapOf()
    private var myPlaceByLvl = 0
    private var myPlaceByPoints = 0

    private val mainPageBinding: BattlepassMainBinding by lazy { BattlepassMainBinding.bind(binding.root) }
    private val tasksPageBinding: BattlepassTasksBinding by lazy { BattlepassTasksBinding.bind(binding.root) }
    private val ratePageBinding: BattlepassRateBinding by lazy { BattlepassRateBinding.bind(binding.root) }

    init {
        activity.runOnUiThread {
            mainPageBinding.lvlBg.viewTreeObserver.addOnPreDrawListener(object : ViewTreeObserver.OnPreDrawListener {
                override fun onPreDraw(): Boolean {
                    mainPageBinding.lvlBg.viewTreeObserver.removeOnPreDrawListener(this)

                    mainAdapter.setItemHeight(mainPageBinding.lvlBg.height)
                    mainAdapter.notifyDataSetChanged()

                    return true
                }
            })


            mainPageBinding.recycler.layoutManager = LinearLayoutManager(activity, LinearLayoutManager.HORIZONTAL, false)
            mainPageBinding.recycler.adapter = mainAdapter

            tasksPageBinding.tasksRecycler.layoutManager = GridLayoutManager(activity, 2)
            tasksPageBinding.tasksRecycler.adapter = tasksAdapter

            ratePageBinding.rateRecycler.layoutManager = LinearLayoutManager(activity, LinearLayoutManager.VERTICAL, false)
            ratePageBinding.rateRecycler.adapter = rateAdapter

            binding.exitButt.setOnClickListener {
                destroy()
            }

            binding.mainPageButton.setOnClickListener {
                setActivePage(Pages.MAIN)
            }

            binding.tasksPageButton.setOnClickListener {
                setActivePage(Pages.TASKS)
            }

            binding.ratePageButton.setOnClickListener {
                setActivePage(Pages.RATE)
            }

            mainPageBinding.buyLvlButton.setOnClickListener {
                nativeOnClick(ePacketType.CLICK_BUY_LEVELS.ordinal)
            }

            mainPageBinding.giftButton.setOnClickListener {
                nativeOnClick(ePacketType.CLICK_GIFT.ordinal)
            }

            handler = Handler(Looper.getMainLooper())

            runnable = object : Runnable {
                override fun run() {
                    updateTasksUpdateTime()
                    handler.postDelayed(this, 1000)
                }
            }
            handler.post(runnable)

            ratePageBinding.rateCatByLvl.setOnClickListener {
                setRateByLvl()
            }

            ratePageBinding.rateNoDonateCat.setOnClickListener {
                setRateByPoint()
            }

            setActivePage(Pages.MAIN)
        }
    }

    private fun setRateByPoint() {
        ratePageBinding.rateYouPlaceText.text = "$myPlaceByPoints место"

        ratePageBinding.rateCatByLvl.setBackgroundResource(0)
        ratePageBinding.rateNoDonateCat.setBackgroundResource(R.drawable.magicstore_cat_active_bg)

        rateAdapter.list = rateNoDonateList
        rateAdapter.notifyDataSetChanged()
    }

    private fun setRateByLvl() {
        ratePageBinding.rateYouPlaceText.text = "$myPlaceByLvl место"

        ratePageBinding.rateNoDonateCat.setBackgroundResource(0)
        ratePageBinding.rateCatByLvl.setBackgroundResource(R.drawable.magicstore_cat_active_bg)

        rateAdapter.list = rateGlobalList
        rateAdapter.notifyDataSetChanged()
    }


    private fun updateMainItem(
        id: Int,
        name: String,
        sprite: String,
        rare: Int,
        state: Int,
    ) {
        activity.runOnUiThread {
            val item: BattlePassMainItem = mainAdapter.list[id] ?: BattlePassMainItem()

            item.name = Utils.transfromColors(name)
            item.sprite = sprite
            item.status = state
            item.rareColor = Rare.getColorAsStateList(rare)

            if (mainAdapter.list.containsKey(id)) {
                mainAdapter.notifyItemChanged(id)
            } else {
                mainAdapter.list[id] = item
                mainAdapter.notifyItemInserted(id)
            }
        }
    }

    private fun updateRateItem(
        isGlobal: Boolean,
        id: Int,
        nick: String,
        points: Int
    ) {
        activity.runOnUiThread {

            if (isGlobal) {
                val item: BattlePassRateItem = rateGlobalList[id] ?: BattlePassRateItem()

                item.nick = nick
                item.points = points

                if (!rateGlobalList.containsKey(id)) {
                    rateGlobalList[id] = item
                }
            } else {
                val item: BattlePassRateItem = rateNoDonateList[id] ?: BattlePassRateItem()

                item.nick = nick
                item.points = points

                if (!rateNoDonateList.containsKey(id)) {
                    rateNoDonateList[id] = item
                }
            }
        }
    }

    private fun updateTaskItem(
        id: Int,
        caption: String,
        description: String,
        curProgress: Int,
        maxProgress: Int,
        reward: Int
    ) {
        activity.runOnUiThread {
            val item = tasksAdapter.list[id] ?: BattlePassTaskItem()

            item.caption = Utils.transfromColors(caption)
            item.description = Utils.transfromColors(description)
            item.curProgress = curProgress
            item.maxProgress = maxProgress
            item.reward = reward

            if (tasksAdapter.list.containsKey(id))
                tasksAdapter.notifyItemChanged(id)
            else {
                tasksAdapter.list[id] = item
                tasksAdapter.notifyItemInserted(id)
            }
        }
    }

    private fun resetAllPages() {
        binding.mainPageButton.setBackgroundResource(0)
        binding.tasksPageButton.setBackgroundResource(0)
        binding.ratePageButton.setBackgroundResource(0)

        binding.mainPageLayout.visibility = View.GONE
        binding.tasksPageLayout.visibility = View.GONE
        binding.ratePageLayout.visibility = View.GONE
    }

    private fun setActivePage(page: Pages) {
        resetAllPages()

        when (page) {
            Pages.MAIN -> {
                binding.mainPageButton.setBackgroundResource(R.drawable.magicstore_cat_active_bg)
                binding.mainPageLayout.visibility = View.VISIBLE
            }

            Pages.TASKS -> {
                binding.tasksPageButton.setBackgroundResource(R.drawable.magicstore_cat_active_bg)
                binding.tasksPageLayout.visibility = View.VISIBLE
            }

            Pages.RATE -> {
                binding.ratePageButton.setBackgroundResource(R.drawable.magicstore_cat_active_bg)
                binding.ratePageLayout.visibility = View.VISIBLE
                setRateByLvl()
            }

            else -> {}
        }
    }

    private fun updateData(myPlaceByLvl: Int, myPlaceByPoints: Int, myLvl: Int, pointsForUp: Int, myChips: Int, endTime: Long, updateTasksTime: Long) {
        activity.runOnUiThread {
            this.myPlaceByLvl = myPlaceByLvl
            this.myPlaceByPoints = myPlaceByPoints

            mainPageBinding.currentLvlText.text = "Уровень $myLvl"
            mainPageBinding.pointForUp.text = pointsForUp.toString()
            mainPageBinding.chipsCount.text = Samp.formatter.format(myChips)

            setEndDate(endTime)
            tasksUpdateTime = updateTasksTime
        }
    }

    private fun updateTasksUpdateTime() {
        activity.runOnUiThread {
            val calendar = Calendar.getInstance(TimeZone.getTimeZone("Europe/Moscow"))
            val currentTimestamp = calendar.timeInMillis / 1000

            val differenceSeconds = tasksUpdateTime - currentTimestamp

            if (differenceSeconds > 0) {
                val hours = differenceSeconds / 3600
                val minutes = (differenceSeconds % 3600) / 60
                val seconds = differenceSeconds % 60

                val formattedTime = String.format("%02d:%02d:%02d", hours, minutes, seconds)

                tasksPageBinding.updateTasksTime.text = "До обновления заданий: $formattedTime"
            }
        }
    }

    private fun setEndDate(unixTime: Long) {
        activity.runOnUiThread {
            val date = Date(unixTime * 1000L)
            val sdf = SimpleDateFormat("d MMMM yyyy", Locale("ru", "RU"))
            sdf.timeZone = TimeZone.getTimeZone("Europe/Moscow")

            val formattedDate = sdf.format(date)

            binding.endDate.text = "Дата окончания: $formattedDate"
        }
    }

    override fun destroy() {
        handler.removeCallbacks(runnable)

        super.destroy()

        nativeOnClose()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    override fun BattlePassMainAdapter_OnClickGive(pos: Int) {
        nativeOnClick(ePacketType.CLICK_MAIN_ITEM.ordinal, pos)
    }

    override fun BattlePassTasksAdapter_OnClickRefresh(pos: Int) {
        nativeOnClick(ePacketType.CLICK_TASK_ITEM.ordinal, pos)
    }

}