package com.holy.game.gui.donate

import android.content.Intent
import android.content.res.ColorStateList
import android.graphics.Color
import android.net.Uri
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.ImageView
import android.widget.Spinner
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.recyclerview.widget.GridLayoutManager
import com.factor.bouncy.BouncyRecyclerView
import com.google.android.material.button.MaterialButton
import com.holy.game.R
import com.holy.game.core.Samp
import com.holy.game.core.Samp.Companion.activity
import com.holy.game.databinding.DonateBinding
import com.holy.game.gui.NativeGui
import com.holy.data.TreasuresList
import com.holy.data.acs.Accessories
import com.holy.data.skins.Skins
import com.holy.data.vehicles.Vehicles
import com.holy.launcher.config.Config
import java.util.Collections
import kotlin.math.ceil


class Donate : NativeGui<DonateBinding>(DonateBinding::class) {
    private lateinit var donate_category_buy_all: ConstraintLayout
    private lateinit var donate_category_my_item: ConstraintLayout
    private lateinit var donate_category_buy_treasure: ConstraintLayout
    private lateinit var donate_category_buy_cars: ConstraintLayout
    private lateinit var donate_category_buy_skins: ConstraintLayout
    private lateinit var donate_category_buy_acs: ConstraintLayout
    private lateinit var donate_category_buy_other: ConstraintLayout
    private lateinit var donate_category_buy_vip:ConstraintLayout

    private lateinit var donateRecycle: BouncyRecyclerView

    private lateinit var donate_balance_text: TextView
    private lateinit var donate_exit_button: ImageView
    private lateinit var donate_check_butt: MaterialButton
    private lateinit var donate_deposit_butt: MaterialButton
    private lateinit var donate_history_butt: MaterialButton

    private var active_category_type = -1
    private lateinit var active_caterory_view: View

    private val donateAdapter = DonateAdapter(this)

    private external fun nativeOnExit()
    external fun native_clickStashItem(clickId: Int, sqlId: Int)

    private external fun native_loadStash()
    private external fun sendAction(type: Int)

    private external fun sendClickItem(actionType: Int, buttonId: Int, itemType: Int, itemId: Int)
    private external fun nativeShowTreasurePreview(treasureId: Int)

    init {
        activity.runOnUiThread {
            donate_category_buy_all = activity.findViewById(R.id.donate_category_buy_all)
            donate_category_my_item = activity.findViewById(R.id.donate_category_my_item)
            donate_category_buy_treasure = activity.findViewById(R.id.donate_category_buy_treasure)
            donate_category_buy_cars = activity.findViewById(R.id.donate_category_buy_cars)
            donate_category_buy_skins = activity.findViewById(R.id.donate_category_buy_skins)
            donate_category_buy_acs = activity.findViewById(R.id.donate_category_buy_acs)
            donate_category_buy_other = activity.findViewById(R.id.donate_category_buy_other)
            donate_category_buy_vip = activity.findViewById(R.id.donate_category_buy_vip)

            donateRecycle = activity.findViewById(R.id.donateRecycle)

            donate_balance_text = activity.findViewById(R.id.donate_balance_text)
            donate_exit_button = activity.findViewById(R.id.donate_exit_button)
            donate_check_butt = activity.findViewById(R.id.donate_check_butt)
            donate_deposit_butt = activity.findViewById(R.id.donate_deposit_butt)
            donate_deposit_butt.setOnClickListener {
                val address = Uri.parse(com.holy.launcher.config.Config.DONATE_URL)
                val openlink = Intent(Intent.ACTION_VIEW, address)
                activity.startActivity(openlink)
            }
            donate_history_butt = activity.findViewById(R.id.donate_history_butt)

            activity.findViewById<MaterialButton>(R.id.donate_change_butt).setOnClickListener {
                sendAction(ACTION_TYPE_CLICK_CONVERT)
            }

            donate_history_butt.setOnClickListener {
                sendAction(ACTION_TYPE_CLICK_HISTORY)
            }

            donate_check_butt.setOnClickListener {
                sendAction(ACTION_TYPE_CLICK_CHECK)
            }

            // Сортировка
            val sort = arrayOf("Сначала новые", "Сначала дорогие", "Сначала дешевые")
            val spinner = activity.findViewById<Spinner>(R.id.donate_sort)


            val adapter: ArrayAdapter<String> = ArrayAdapter<String>(activity, android.R.layout.simple_spinner_item, sort)

            // Определяем разметку для использования при выборе элемента
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)

            // Применяем адаптер к элементу spinner
            spinner.adapter = adapter
            spinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
                override fun onItemSelected(adapterView: AdapterView<*>?, view: View?, i: Int, l: Long) {
                    if (active_category_type != CATEGORY_STASH) {
                        if (i == 2) {
                            Collections.sort(donateAdapter.items, com.holy.game.gui.donate.DonateComparators.DonateComparatorByPrice())
                        }
                        if (i == 1) {
                            Collections.sort(donateAdapter.items, com.holy.game.gui.donate.DonateComparators.DonateComparatorByPrice().reversed())
                        }
                        if (i == 0) {
                            setActiveCategory(active_category_type, active_caterory_view)
                        }
                    }
                    donateAdapter.notifyDataSetChanged()
                }

                override fun onNothingSelected(adapterView: AdapterView<*>?) {
                }
            }

            // категории
            donate_category_buy_all.setOnClickListener {
                setActiveCategory(CATEGORY_ALL, it)
            }

            donate_category_buy_treasure.setOnClickListener {
                setActiveCategory(CATEGORY_TREASURES, it)
            }

            donate_category_my_item.setOnClickListener {
                setActiveCategory(CATEGORY_STASH, it)
                native_loadStash()
            }

            donate_category_buy_cars.setOnClickListener {
                setActiveCategory(CATEGORY_CARS, it)
            }

            donate_category_buy_skins.setOnClickListener {
                setActiveCategory(CATEGORY_SKINS, it)
            }

            donate_category_buy_acs.setOnClickListener {
                setActiveCategory(CATEGORY_ACS, it)
            }

            donate_category_buy_vip.setOnClickListener {
                setActiveCategory(CATEGORY_VIP, it)
            }

            donate_category_buy_other.setOnClickListener {
                setActiveCategory(CATEGORY_OTHER, it)
            }

            donate_exit_button.setOnClickListener {
                destroy()
            }

            if(Samp.isTablet)
                donateRecycle.layoutManager = GridLayoutManager(activity, 2)
            else
                donateRecycle.layoutManager = GridLayoutManager(activity, 3)

            donateRecycle.adapter = donateAdapter
            donateRecycle.setHasFixedSize(true)
            donateRecycle.setItemViewCacheSize(10)

            setActiveCategory(CATEGORY_ALL, donate_category_buy_all)
        }
    }

    private fun updateBalance(balance: Int) {
        activity.runOnUiThread {
            donate_balance_text.text = String.format("Баланс: %s LC", Samp.formatter.format(balance.toLong()))
        }
    }

    override fun destroy() {
        super.destroy()

        nativeOnExit()
    }

    override fun receivePacket(actionId: Int, data: String) {
        TODO("Not yet implemented")
    }

    private fun setActiveCategory(category: Int, view: View) {
        activity.runOnUiThread {
            active_category_type = category
            active_caterory_view = view
            resetAllCategoryTint()
            view.backgroundTintList = ColorStateList.valueOf(Color.parseColor("#ffff00"))
            donateAdapter.items.clear()
            donateAdapter.selected_item = -1

            for (item in allItems) {
                if (item.category == category || category == CATEGORY_ALL) {
                    donateAdapter.items.add(item)
                }
            }
            donateAdapter.notifyDataSetChanged()
        }
    }

    private fun resetStash() {
        activity.runOnUiThread {
            donateAdapter.selected_item = -1
            donateAdapter.items.clear()

            donateAdapter.notifyDataSetChanged()
        }
    }

    private fun resetAllCategoryTint() {
        donate_category_buy_other.backgroundTintList = null
        donate_category_buy_vip.backgroundTintList = null
        donate_category_my_item.backgroundTintList = null
        donate_category_buy_all.backgroundTintList = null
        donate_category_buy_cars.backgroundTintList = null
        donate_category_buy_skins.backgroundTintList = null
        donate_category_buy_acs.backgroundTintList = null
        donate_category_buy_treasure.backgroundTintList = null
    }

    fun clickInfo(item: DonateItem) {
        if(item.category == CATEGORY_TREASURES) {
            nativeShowTreasurePreview(item.value)
            return
        }
        sendClickItem(ACTION_TYPE_CLICK_ITEM, BUTTTON_INFO, item.category, item.value)
    }

    fun clickBuy(item: DonateItem) {
        sendClickItem(ACTION_TYPE_CLICK_ITEM, BUTTTON_BUY, item.category, item.value)
    }

    /** мои предметы  */
    private fun addToMyItems(category: Int, value: Int, sqlId: Int) {

        activity.runOnUiThread {
            if (active_category_type != CATEGORY_STASH) {
                setActiveCategory(CATEGORY_STASH, donate_category_my_item)
            }
            var temp = DonateItem("", category, 0, 0, value)
            temp = convertOldItem(temp)
            temp.sqlId = sqlId
            donateAdapter.items.add(temp)
            donateAdapter.notifyItemInserted( donateAdapter.items.size - 1 )
        }
    }


    internal enum class eOldCategory {
        ROULETTE_PRIZE_TYPE_MONEY,  // need del
        ROULETTE_PRIZE_TYPE_EXP,
        ROULETTE_PRIZE_TYPE_CAR,
        ROULETTE_PRIZE_TYPE_CHIP,  // need del
        ROULETTE_PRIZE_TYPE_GUN,
        ROULETTE_PRIZE_TYPE_SKIN,
        ROULETTE_PRIZE_TYPE_ACS,
        ROULETTE_PRIZE_TYPE_LC,  // need del
        ROULETTE_PRIZE_TYPE_VIP,
        ROULETTE_PRIZE_TYPE_ARMY,  // военный билет
        ROULETTE_PRIZE_TYPE_LIC,
        ROULETTE_PRIZE_TYPE_MED,
        ROULETTE_PRIZE_TYPE_CARSLOT,
        ROULETTE_PRIZE_TYPE_WARN,  // снятие варна
        ROULETTE_PRIZE_TYPE_VIP_WEEK,
        ROULETTE_PRIZE_TYPE_VIP_TWOWEEK,
        ROULETTE_PRIZE_TYPE_VIP_MONTH,
        ROULETTE_PRIZE_TYPE_LAW,  //законопослушность
        ROULETTE_PRIZE_TYPE_FAM_CARSLOT,  //семейный карслот
        ROULETTE_PRIZE_TYPE_CASE, //кейс
        ROULETTE_PRIZE_TYPE_HOUSESLOT,
        ROULETTE_PRIZE_TYPE_BIZSLOT,
    }

    private fun convertOldItem(old: DonateItem): DonateItem {
        var old = old
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_CASE.ordinal) {
            old = TreasuresList.listForDonate[old.value].copy()
            return old
        }
        if(old.category == eOldCategory.ROULETTE_PRIZE_TYPE_HOUSESLOT.ordinal) {
            old.price = ceil((1000 / 3.5f).toDouble()).toInt()
            old.name = "Слот для дома"
            old.imgRecourse = getResId("house_slot")
            return old
        }
        if(old.category == eOldCategory.ROULETTE_PRIZE_TYPE_BIZSLOT.ordinal) {
            old.price = ceil((5000 / 3.5f).toDouble()).toInt()
            old.name = "Слот для бизнеса"
            old.imgRecourse = getResId("biz_slot")
            return old
        }
        if(old.category == eOldCategory.ROULETTE_PRIZE_TYPE_LAW.ordinal) {
            old.price = 0
            old.name = String.format("Законопослушность +%d", old.value)
            old.imgRecourse = getResId("donate_zakon")
            return old
        }
        if(old.category == eOldCategory.ROULETTE_PRIZE_TYPE_FAM_CARSLOT.ordinal) {
            old.price = ceil((200 / 3.5f).toDouble()).toInt()
            old.name = "Слот для семейного авто"
            old.imgRecourse = getResId("donate_family_car_slot")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_VIP_MONTH.ordinal) {
            if (old.value == 1) {
                old.price = ceil((300 / 3.5f).toDouble()).toInt()
                old.name = "Silver VIP\n( 30 дней )"
                old.imgRecourse = getResId("silver_vip")
                return old
            }
            if (old.value == 2) {
                old.price = ceil((650 / 3.5f).toDouble()).toInt()
                old.name = "Gold VIP\n( 30 дней )"
                old.imgRecourse = getResId("gold_vip")
                return old
            }
            if (old.value == 3) {
                old.price = ceil((1000 / 3.5f).toDouble()).toInt()
                old.name = "Platinum VIP\n( 30 дней )"
                old.imgRecourse = getResId("platinum_vip")
                return old
            }
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_VIP_TWOWEEK.ordinal) {
            if (old.value == 1) {
                old.price = ceil((170 / 3.5f).toDouble()).toInt()
                old.name = "Silver VIP\n( 15 дней )"
                old.imgRecourse = getResId("silver_vip")
                return old
            }
            if (old.value == 2) {
                old.price = ceil((370 / 3.5f).toDouble()).toInt()
                old.name = "Gold VIP\n( 15 дней )"
                old.imgRecourse = getResId("gold_vip")
                return old
            }
            if (old.value == 3) {
                old.price = ceil((570 / 3.5f).toDouble()).toInt()
                old.name = "Platinum VIP\n( 15 дней )"
                old.imgRecourse = getResId("platinum_vip")
                return old
            }
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_VIP_WEEK.ordinal) {
            if (old.value == 1) {
                old.price = ceil((100 / 3.5f).toDouble()).toInt()
                old.name = "Silver VIP\n( 7 дней )"
                old.imgRecourse = getResId("silver_vip")
                return old
            }
            if (old.value == 2) {
                old.price = ceil((215 / 3.5f).toDouble()).toInt()
                old.name = "Gold VIP\n( 7 дней )"
                old.imgRecourse = getResId("gold_vip")
                return old
            }
            if (old.value == 3) {
                old.price = ceil((335 / 3.5f).toDouble()).toInt()
                old.name = "Platinum VIP\n( 7 дней )"
                old.imgRecourse = getResId("platinum_vip")
                return old
            }
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_VIP.ordinal) {
            if (old.value == 1) {
                old.price = 0
                old.name = "Silver VIP\n( 1 день )"
                old.imgRecourse = getResId("silver_vip")
                return old
            }
            if (old.value == 2) {
                old.price = 0
                old.name = "Gold VIP\n( 1 день )"
                old.imgRecourse = getResId("gold_vip")
                return old
            }
            if (old.value == 3) {
                old.price = 0
                old.name = "Platinum VIP\n( 1 день )"
                old.imgRecourse = getResId("platinum_vip")
                return old
            }
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_GUN.ordinal) {
            old.price = 0
            old.name = "Случайное оружие ( 1.000 пт)"
            old.imgRecourse = getResId("donate_gun")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_WARN.ordinal) {
            old.price = ceil((100 / 3.5f).toDouble()).toInt()
            old.name = "Снятие варна"
            old.imgRecourse = getResId("donate_warn")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_CARSLOT.ordinal) {
            old.price = ceil((100 / 3.5f).toDouble()).toInt()
            old.name = "Слот для авто"
            old.imgRecourse = getResId("donate_veh_slot")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_ARMY.ordinal) {
            old.price = ceil((100 / 3.5f).toDouble()).toInt()
            old.name = "Военный билет"
            old.imgRecourse = getResId("donate_voen")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_LIC.ordinal) {
            old.price = ceil((150 / 3.5f).toDouble()).toInt()
            old.name = "Комплект лицензий"
            old.imgRecourse = getResId("licenses_pack")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_MED.ordinal) {
            old.price = ceil((100 / 3.5f).toDouble()).toInt()
            old.name = "Мед. карта"
            old.imgRecourse = getResId("donate_medcard")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_MONEY.ordinal) {
            old.price = 0
            old.name = String.format("%s руб.", Samp.formatter.format(old.value.toLong()))
            old.imgRecourse = getResId("donate_money")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_EXP.ordinal) {
            old.price = 0
            old.name = String.format("%d exp", old.value)
            old.imgRecourse = getResId("donate_exp")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_LC.ordinal) {
            old.price = 0
            old.name = String.format("%d LC", old.value)
            old.imgRecourse = getResId("donate_lc")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_CHIP.ordinal) {
            old.price = 0
            old.name = String.format("%d фишек", old.value)
            old.imgRecourse = getResId("donate_chips")
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_CAR.ordinal) {
            val vehicleId = old.value
            old.snap = Vehicles.getSnap(vehicleId)
            old.price = Vehicles.getPriceLc(vehicleId) / 2
            old.name = Vehicles.getName(vehicleId)
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_SKIN.ordinal) {
            val skinId = old.value
            old.snap = Skins.getSnap(skinId)
            old.price = ceil(Skins.getPriceLC(skinId) / 3.5f).toInt()
            old.name = Skins.getName(skinId)
            return old
        }
        if (old.category == eOldCategory.ROULETTE_PRIZE_TYPE_ACS.ordinal) {
            val acsId = old.value
            old.category = CATEGORY_ACS

            if(acsId == -1) {// FIXME: rollers, magic and stupid
                old.imgRecourse = getResId("inv_rollers")
                old.price = ceil(2_000 / 3.5f).toInt()
                old.name = "Ролики"
                return old;
            }

            old.snap = Accessories.getSnap(acsId)
            old.price = ceil(Accessories.getPriceLc(acsId) / 3.5f).toInt()
            old.name = Accessories.getName(acsId)
            return old
        }
        return old
    }

    companion object {
        const val CATEGORY_STASH = -1
        const val CATEGORY_ALL = 0
        const val CATEGORY_CARS = 1
        const val CATEGORY_SKINS = 2
        const val CATEGORY_ACS = 3
        const val CATEGORY_VIP = 4
        const val CATEGORY_OTHER = 5
        const val CATEGORY_TREASURES = 6

        const val CLICK_ID_TREASURE = 11
        const val ACTION_TYPE_CLICK_CHECK = 0
        const val ACTION_TYPE_CLICK_HISTORY = 1
        const val ACTION_TYPE_CLICK_CONVERT = 2
        const val ACTION_TYPE_CLICK_ITEM = 3
        const val DONATE_OTHER_HOUSE_SLOT = 0
        const val DONATE_OTHER_BIZ_SLOT = 1
        const val DONATE_OTHER_WARN = 2
        const val DONATE_OTHER_LICENSES = 3
        const val DONATE_OTHER_CHANGE_FAM_NAME = 4
        const val DONATE_OTHER_CHANGE_SIM = 5
        const val DONATE_OTHER_VOEN = 6
        const val DONATE_OTHER_ZAKON = 7
        const val DONATE_OTHER_MED = 8
        const val DONATE_OTHER_CHANGE_NICK = 9
        const val DONATE_OTHER_CHANGE_SEX = 10
        const val DONATE_OTHER_VEH_SLOT = 11
        const val DONATE_OTHER_VEH_FAMILY_SLOT = 12
        const val DONATE_OTHER_GIVE_FAMILY_SLOT = 13
        const val DONATE_OTHER_ROLLERS = 14
        const val DONATE_OTHER_EXP = 15
        const val DONATE_OTHER_MUSIC = 16
        const val CONTACTS_VIEW_ACTION = "android.intent.action.VIEW"
        const val BUTTTON_BUY = 0
        const val BUTTTON_INFO = 1

        fun getResId(res: String?): Int {
            return activity.resources.getIdentifier(res, "drawable", activity.packageName)
        }

        val allItems = arrayOf( // ДОБАВЛЯТЬ В НАЧАЛО КАТЕГОРИИ ЧТОБЫ ВОРКАЛА СОРТИРОВКА ПО НОВИЗНЕ!!!!!!!!!111111111111111111
            // Сундуки
            TreasuresList.listForDonate[10],
            TreasuresList.listForDonate[0],
            TreasuresList.listForDonate[1],
            TreasuresList.listForDonate[2],
            TreasuresList.listForDonate[3],
            TreasuresList.listForDonate[4],
            TreasuresList.listForDonate[5],
            TreasuresList.listForDonate[6],
            TreasuresList.listForDonate[7],
            TreasuresList.listForDonate[8],
            TreasuresList.listForDonate[9],

            // skins
            Skins.createDonateItem(12),
            Skins.createDonateItem(183),
            Skins.createDonateItem(219),
            Skins.createDonateItem(45),
            Skins.createDonateItem(186),
            Skins.createDonateItem(206),
            Skins.createDonateItem(221),
            Skins.createDonateItem(299),
            Skins.createDonateItem(138),
            Skins.createDonateItem(158),
            Skins.createDonateItem(95),
            Skins.createDonateItem(170),
            Skins.createDonateItem(168),
            Skins.createDonateItem(77),
            Skins.createDonateItem(114),
            Skins.createDonateItem(115),
            Skins.createDonateItem(119),
            Skins.createDonateItem(121),
            Skins.createDonateItem(1),
            Skins.createDonateItem(154),
            Skins.createDonateItem(40),
            Skins.createDonateItem(159),
            Skins.createDonateItem(94),
            Skins.createDonateItem(87),
            Skins.createDonateItem(227),
            Skins.createDonateItem(228),
            Skins.createDonateItem(267),
            Skins.createDonateItem(294),
            Skins.createDonateItem(296),
            Skins.createDonateItem(139),
            Skins.createDonateItem(264),
            Skins.createDonateItem(226),
            Skins.createDonateItem(137),
            Skins.createDonateItem(171),
            Skins.createDonateItem(297),
            Skins.createDonateItem(298),
            Skins.createDonateItem(272),
            Skins.createDonateItem(97),
            Skins.createDonateItem(184),
            Skins.createDonateItem(273),
            Skins.createDonateItem(220),
            Skins.createDonateItem(208),
            Skins.createDonateItem(238),
            Skins.createDonateItem(161),
            Skins.createDonateItem(14),
            
            // cars
            Vehicles.createDonateItem(442),
            Vehicles.createDonateItem(554),
            Vehicles.createDonateItem(535),
            Vehicles.createDonateItem(575),
            Vehicles.createDonateItem(580),
            Vehicles.createDonateItem(496),
            Vehicles.createDonateItem(503),
            Vehicles.createDonateItem(504),
            Vehicles.createDonateItem(506),
            Vehicles.createDonateItem(536),
            Vehicles.createDonateItem(550),
            Vehicles.createDonateItem(565),
            Vehicles.createDonateItem(576),
            Vehicles.createDonateItem(585),
            Vehicles.createDonateItem(600),
            Vehicles.createDonateItem(466),
            Vehicles.createDonateItem(461),
            Vehicles.createDonateItem(581),
            Vehicles.createDonateItem(521),
            Vehicles.createDonateItem(463),
            Vehicles.createDonateItem(468),
            Vehicles.createDonateItem(411),
            Vehicles.createDonateItem(579),
            Vehicles.createDonateItem(451),
            Vehicles.createDonateItem(477),
            Vehicles.createDonateItem(489),
            Vehicles.createDonateItem(589),
            Vehicles.createDonateItem(517),
            Vehicles.createDonateItem(474),
            Vehicles.createDonateItem(420),
            Vehicles.createDonateItem(438),
            Vehicles.createDonateItem(475),
            Vehicles.createDonateItem(526),
            Vehicles.createDonateItem(527),
            Vehicles.createDonateItem(401),
            Vehicles.createDonateItem(421),
            Vehicles.createDonateItem(604),
            Vehicles.createDonateItem(494),
            Vehicles.createDonateItem(412),
            Vehicles.createDonateItem(562),
            Vehicles.createDonateItem(479),
            Vehicles.createDonateItem(492),
            Vehicles.createDonateItem(400),
            Vehicles.createDonateItem(549),
            Vehicles.createDonateItem(404),
            Vehicles.createDonateItem(402),
            Vehicles.createDonateItem(436),
            Vehicles.createDonateItem(405),
            Vehicles.createDonateItem(560),
            Vehicles.createDonateItem(507),
            Vehicles.createDonateItem(426),
            Vehicles.createDonateItem(502),
            Vehicles.createDonateItem(458),
            Vehicles.createDonateItem(542),
            Vehicles.createDonateItem(480),
            Vehicles.createDonateItem(470),
            Vehicles.createDonateItem(566),
            Vehicles.createDonateItem(505),
            Vehicles.createDonateItem(410),
            Vehicles.createDonateItem(559),
            Vehicles.createDonateItem(534),
            Vehicles.createDonateItem(419),
            Vehicles.createDonateItem(587),
            Vehicles.createDonateItem(429),
            Vehicles.createDonateItem(567),
            Vehicles.createDonateItem(543),
            Vehicles.createDonateItem(546),
            Vehicles.createDonateItem(551),
            Vehicles.createDonateItem(547),
            Vehicles.createDonateItem(415),
            Vehicles.createDonateItem(541),
            Vehicles.createDonateItem(603),
            Vehicles.createDonateItem(424),
            Vehicles.createDonateItem(586),
            Vehicles.createDonateItem(522),
            Vehicles.createDonateItem(471),
            Vehicles.createDonateItem(568),
            Vehicles.createDonateItem(571),
            Vehicles.createDonateItem(444),
            Vehicles.createDonateItem(540),
            Vehicles.createDonateItem(518),
            Vehicles.createDonateItem(467),
            Vehicles.createDonateItem(558),

            //vip
            DonateItem("Silver VIP\n( 7 дней )", CATEGORY_VIP, 100, getResId("silver_vip"), 0),
            DonateItem("Silver VIP\n( 15 дней )", CATEGORY_VIP, 170, getResId("silver_vip"), 1),
            DonateItem("Silver VIP\n( 30 дней )", CATEGORY_VIP, 300, getResId("silver_vip"), 2),
            DonateItem("Gold VIP\n( 7 дней )", CATEGORY_VIP, 215, getResId("gold_vip"), 3),
            DonateItem("Gold VIP\n( 15 дней )", CATEGORY_VIP, 370, getResId("gold_vip"), 4),
            DonateItem("Gold VIP\n( 30 дней )", CATEGORY_VIP, 650, getResId("gold_vip"), 5),
            DonateItem("Platinum VIP\n( 7 дней )", CATEGORY_VIP, 335, getResId("platinum_vip"), 6),
            DonateItem("Platinum VIP\n( 15 дней )", CATEGORY_VIP, 570, getResId("platinum_vip"), 7),
            DonateItem("Platinum VIP\n( 30 дней )", CATEGORY_VIP, 1000, getResId("platinum_vip"), 8),

            //acs
            Accessories.createDonateItem(5767),
            Accessories.createDonateItem(5770),
            Accessories.createDonateItem(5811),
            Accessories.createDonateItem(5916),
            Accessories.createDonateItem(5873),
            Accessories.createDonateItem(5874),
            Accessories.createDonateItem(14283),
            Accessories.createDonateItem(14393),
            Accessories.createDonateItem(14394),
            Accessories.createDonateItem(14289),
            Accessories.createDonateItem(17979),

            // other
            DonateItem("Колонка JBL", CATEGORY_OTHER, 1000, DONATE_OTHER_MUSIC, Accessories.getSnap(17581)),
            DonateItem("EXP", CATEGORY_OTHER, 15, getResId("donate_exp"), DONATE_OTHER_EXP),
            DonateItem("Ролики ( анимация ходьбы )", CATEGORY_OTHER, 2000, getResId("inv_rollers"), DONATE_OTHER_ROLLERS),
            DonateItem("Слот для дома\n(От 1000)", CATEGORY_OTHER, 1000, getResId("house_slot"), DONATE_OTHER_HOUSE_SLOT),
            DonateItem("Слот для Бизнеса", CATEGORY_OTHER, 5000, getResId("biz_slot"), DONATE_OTHER_BIZ_SLOT),
            DonateItem("Снять варн", CATEGORY_OTHER, 100, getResId("donate_warn"), DONATE_OTHER_WARN),
            DonateItem("Все лицензии", CATEGORY_OTHER, 150, getResId("licenses_pack"), DONATE_OTHER_LICENSES),
            DonateItem("Изменить имя семьи", CATEGORY_OTHER, 100, getResId("donate_change_family_name"), DONATE_OTHER_CHANGE_FAM_NAME),
            DonateItem("4-х значный номер", CATEGORY_OTHER, 250, getResId("donate_sim"), DONATE_OTHER_CHANGE_SIM),
            DonateItem("Военный билет", CATEGORY_OTHER, 100, getResId("donate_voen"), DONATE_OTHER_VOEN),
            DonateItem("Законопослушность\n( + 10 )", CATEGORY_OTHER, 5, getResId("donate_zakon"), DONATE_OTHER_ZAKON),
            DonateItem("Мед. карта", CATEGORY_OTHER, 100, getResId("donate_medcard"), DONATE_OTHER_MED),
            DonateItem("Смена ника", CATEGORY_OTHER, 250, getResId("donate_changenick"), DONATE_OTHER_CHANGE_NICK),
            DonateItem("Смена пола", CATEGORY_OTHER, 100, getResId("donate_change_sex"), DONATE_OTHER_CHANGE_SEX),
            DonateItem("Слот для авто", CATEGORY_OTHER, 100, getResId("donate_veh_slot"), DONATE_OTHER_VEH_SLOT),
            DonateItem("Слот для семейного авто", CATEGORY_OTHER, 200, getResId("donate_family_car_slot"), DONATE_OTHER_VEH_FAMILY_SLOT),
            DonateItem("Передача семьи", CATEGORY_OTHER, 1000, getResId("donate_give_family"), DONATE_OTHER_GIVE_FAMILY_SLOT))
    }
}