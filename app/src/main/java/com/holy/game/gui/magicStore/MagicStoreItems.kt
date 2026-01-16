package com.holy.game.gui.magicStore

import com.holy.game.core.Samp
import com.holy.data.acs.Accessories
import com.holy.data.skins.Skins

object MagicStoreItems {
    const val PRICE_BRONZE = 0
    const val PRICE_SILVER = 1
    const val PRICE_GOLD = 2

    const val CATEGORY_ITEMS = 0
    const val CATEGORY_ANIMALS = 1
    const val CATEGORY_POTIONS = 2
    const val CATEGORY_CONSUMABLES = 3
    const val CATEGORY_SKINS = 4

    val list = listOf(
        MagicStoreItem("Инфернал", CATEGORY_ANIMALS, 7_500, PRICE_GOLD, Accessories.getSnap(17342)),
        MagicStoreItem("Сильфида", CATEGORY_ANIMALS, 7_500, PRICE_GOLD, Accessories.getSnap(17346)),

        MagicStoreItem("Щит Капитана Америки", CATEGORY_ITEMS, 7_500, PRICE_GOLD, Accessories.getSnap(16849)),
        MagicStoreItem("Меч Саурона", CATEGORY_ITEMS, 5_000, PRICE_GOLD, Accessories.getSnap(17359)),
        MagicStoreItem("Топор Этригана", CATEGORY_ITEMS, 5_000, PRICE_GOLD, Accessories.getSnap(16583)),
        MagicStoreItem("Трезубец Сулимо", CATEGORY_ITEMS, 5_000, PRICE_GOLD, Accessories.getSnap(17350)),
        MagicStoreItem("Коса Баланара", CATEGORY_ITEMS, 5_000, PRICE_GOLD, Accessories.getSnap(17351)),

        MagicStoreItem("Ключ в подземелье", CATEGORY_CONSUMABLES, 100, PRICE_BRONZE, getRecourse("inv_key_dungeon")),
        MagicStoreItem("Ключ от сундука", CATEGORY_CONSUMABLES, 100, PRICE_SILVER, getRecourse("inv_key_chest")),
        MagicStoreItem("Колокольчик", CATEGORY_CONSUMABLES, 5, PRICE_GOLD, getRecourse("inv_bell")),

        MagicStoreItem("Зелье исцеления", CATEGORY_POTIONS, 1, PRICE_GOLD, getRecourse("inv_potion_health")),
        MagicStoreItem("Зелье мудрости", CATEGORY_POTIONS, 150, PRICE_SILVER, getRecourse("inv_potion_wisdom")),
        MagicStoreItem("Зелье сытости", CATEGORY_POTIONS, 100, PRICE_BRONZE, getRecourse("inv_potion_satiety")),
        MagicStoreItem("Зелье Божества", CATEGORY_POTIONS, 5, PRICE_GOLD, getRecourse("inv_potion_deities")),
        MagicStoreItem("Зелье инфернала", CATEGORY_POTIONS, 2, PRICE_GOLD, getRecourse("inv_potion_infernal")),
        MagicStoreItem("Зелье сильфиды", CATEGORY_POTIONS, 2, PRICE_GOLD, getRecourse("inv_potion_sylphs")),
        MagicStoreItem("Зелье репутации", CATEGORY_POTIONS, 1, PRICE_GOLD, getRecourse("inv_potion_reputation")),

        MagicStoreItem(Skins.getName(153), CATEGORY_SKINS, 5000, PRICE_GOLD, Skins.getSnap(153)),
        MagicStoreItem(Skins.getName(38), CATEGORY_SKINS, 5000, PRICE_GOLD, Skins.getSnap(38)),
        MagicStoreItem(Skins.getName(241), CATEGORY_SKINS, 5000, PRICE_GOLD, Skins.getSnap(241)),
        MagicStoreItem(Skins.getName(242), CATEGORY_SKINS, 5000, PRICE_GOLD, Skins.getSnap(242))
    )


    private fun getRecourse(name: String): Int {
        val findRes = Samp.activity.resources.getIdentifier(name, "drawable", Samp.activity.packageName)
        if(findRes == 0)
            return Samp.activity.resources.getIdentifier("acs_10998", "drawable", Samp.activity.packageName)
        else
            return findRes
    }

}