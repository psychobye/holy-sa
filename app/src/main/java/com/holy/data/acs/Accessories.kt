package com.holy.data.acs

import com.lit.game.EntitySnaps
import com.lit.game.SnapShot
import com.lit.game.gui.donate.Donate
import com.lit.game.gui.donate.DonateItem
import com.lit.game.gui.treasure.TreasureItem
import com.lit.data.Rare

object Accessories {

    fun getPriceLc(modelId: Int) : Int {
        return list[modelId]?.priceLc ?: 0
    }

    fun getSnap(modelId: Int): SnapShot {
        val acs = list[modelId]
        if(acs == null)
            return SnapShot(EntitySnaps.OBJECT, 18631, 0.0f, 0.0f,0.0f, 0.0f, 0.0f, 0.0f)

        return SnapShot(
            EntitySnaps.OBJECT,
            modelId,
            acs.rotX, acs.rotY, acs.rotZ,
            acs.offsetX, acs.offsetY, acs.offsetZ
        )
    }

    fun createDonateItem(modelId: Int): DonateItem {
        return DonateItem(getName(modelId), Donate.CATEGORY_ACS, getPriceLc(modelId), modelId, getSnap(modelId))
    }

    fun createTreasureItem(acsId: Int): TreasureItem {
        val acs = list[acsId]

        if(acs == null)
            return TreasureItem(0, "pizda", 0)

        return TreasureItem(
            SnapShot(
                EntitySnaps.OBJECT,
                acsId,
                acs.rotX, acs.rotY, acs.rotZ,
                acs.offsetX, acs.offsetY, acs.offsetZ
            ),
            acs.name,
            acs.rare
        )
    }

    fun getPriceRub(modelId: Int) : Int {
        return list[modelId]?.priceRub ?: 0
    }

    fun getRare(modelId: Int) : Int {
        return if((list[modelId]?.rare ?: 0) == Rare.NONE)
            Rare.getRareFromPrice(getPriceLc(modelId))
        else {
            list[modelId]?.rare ?: 0
        }
    }

    fun getName(modelId: Int) : String {
        return list[modelId]?.name ?: "Invalid Id"
    }

    val list = hashMapOf(
        5767 to Accesory("Ворон", 500000, 500, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.2f, 0.0f),
        5770 to Accesory("Тыква", 500000, 500, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.2f, 0.0f),
        5778 to Accesory("Маска Гиены", 500000, 50),
        5768 to Accesory("Маска Грызуна", 500000, 50),
        5769 to Accesory("Маска Смайла", 500000, 50, Rare.NONE, 0.0f, 180.0f, 90.0f, 0.0f, 0.0f, 0.0f),
        5809 to Accesory("Маска Оленя", 500000, 50),
        5811 to Accesory("Маска Мишки", 500000, 500, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.7f, 0.07f),
        5869 to Accesory("Маска Кабана", 500000, 50),
        5873 to Accesory("Маска Пакет", 500000, 500, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.0f, 0.0f),
        5874 to Accesory("Гуманоид", 700000, 700, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.2f, 0.0f),
        5916 to Accesory("Пёсель", 500000, 500, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.8f, 0.1f),
        5918 to Accesory("Скейтборд", 1000000, 100, Rare.NONE, 0.0f, 180.0f, 70.0f, 0.0f, 0.8f, 0.1f),
        6605 to Accesory("Жёлтый рюкзак Nike", 750000, 75),
        17976 to Accesory("Фиолетовый рюкзак PUMA", 650000, 65),
        10971 to Accesory("Серая широкая маска", 40000, 4),
        10973 to Accesory("Тканевая маска", 40000, 4, Rare.NONE, 7.0f, 180.0f, 90.0f, 0.0f, 0.0f, -1.0f),
        10974 to Accesory("Маска", 40000, 4, Rare.NONE, 0.0f, 180.0f, 90.0f, 0.0f, 0.0f, -0.91f),
        10976 to Accesory("Маска Кабуки", 120000, 4, Rare.NONE, 0.0f, 180.0f, 90.0f, -0.05f, 0.0f, -1.0f),
        10977 to Accesory("Пейнтбольная маска", 70000, 7, Rare.NONE, 0.07f, 180.0f, 90.0f, 0.0f, 0.0f, -0.1f),
        10978 to Accesory("Зёлная маска 2", 90000, 9),
        10979 to Accesory("Морталкомбат маска", 150000, 15, Rare.NONE, 0.00f, 180.0f, 90.0f, 0.02f, 0.0f, -1.0f),
        10980 to Accesory("Респиратор", 70000, 7, Rare.NONE, 0.0f, 180.0f, 90.0f, 0.0f, 0.0f, -1.0f),
        10981 to Accesory("Белая маска", 40000, 4, 0),
        10982 to Accesory("Маска Payday", 200000, 20, Rare.NONE, 0.0f, 180.0f, 90.0f, 0.10f, 0.3f, -1.0f),
        10998 to Accesory("Лазурная кепка", 60000, 6, Rare.NONE, -175.0f, 0.0f, -55.0f, 0.0f, 0.0f, 0.0f),
        10999 to Accesory("Коричневая кепка", 60000, 6),
        11000 to Accesory("Зелёная кепка", 60000, 6),
        11001 to Accesory("Камуфляжная кепка", 60000, 6),
        11002 to Accesory("Белый рюкзак Adidas", 250000, 25),
        11003 to Accesory("Красный рюкзак Adidas", 280000, 28),
        11004 to Accesory("Красный рюкзак Louis Vuitton", 280000, 28),
        11005 to Accesory("Зелёный рюкзак Louis Vuitton", 280000, 28),
        11006 to Accesory("Лазурный рюкзак с розами", 300000, 30),
        11007 to Accesory("Чёрный рюкзак с розами", 320000, 32),
        17995 to Accesory("Чёрный рюкзак", 420000, 42),
        17571 to Accesory("Красный рюкзак", 350000, 35),
        6603 to Accesory("Синий рюкзак", 350000, 35),
        6602 to Accesory("Розовый рюкзак", 350000, 35),
        14011 to Accesory("Сумка Рич", 600000, 60),
        14037 to Accesory("Сумка Луи", 600000, 60),
        14283 to Accesory("Скейт", 1000000, 1000, Rare.NONE, 50.0f, 180.0f, 70.0f, 0.0f, 0.1f, -0.05f),
        17598 to Accesory("Скейтборд Tony Hawk", 1000000, 100),
        6159 to Accesory("Скейтборд обычный", 600000, 60),
        17570 to Accesory("Скейтборд Hoopla", 1000000, 100),
        17451 to Accesory("Скейтборд Rise & Shine", 1000000, 100),
        14289 to Accesory("Молот Тора", 2500000, 2500, Rare.NONE, 90.0f, 180.0f, 0.0f, 0.0f, 0.0f, 0.0f),
        14393 to Accesory("Посох", 1500000, 1500, Rare.NONE, 230.0f, 180.0f, 250.0f, 0.0f, 0.1f, 0.0f),
        14394 to Accesory("Кирка", 1500000, 1500),
        16757 to Accesory("Гитара", 500000, 50, Rare.NONE, -180.0f, -310.0f, 84.0f, 0.0f, 0.0f, 0.0f),
        17445 to Accesory("Чёрная гитара", 500000, 50, Rare.NONE, -180.0f, -310.0f, 84.0f, 0.0f, 0.0f, 0.0f),
        17444 to Accesory("Коричневая гитара", 500000, 50, Rare.NONE, -180.0f, -310.0f, 84.0f, 0.0f, 0.0f, 0.0f),
        16794 to Accesory("Лазурно-розовая сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16803 to Accesory("Розово-леопардовая сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16804 to Accesory("Сине-розовая сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16805 to Accesory("Красно-голубая сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16807 to Accesory("Красно-чёрная сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16808 to Accesory("Сине-чёрная сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16810 to Accesory("Зелёно-синяя сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16819 to Accesory("Жёлто-красная сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        16831 to Accesory("Чёрная сумка", 600000, 60, Rare.NONE, 9.0f, -178.0f, -48.0f, 0.0f, 0.0f, -0.03f),
        17991 to Accesory("Зелёная дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        17443 to Accesory("Красная дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        17442 to Accesory("Оранжевая дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        6009 to Accesory("Розовая дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        6608 to Accesory("Чёрная дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        6607 to Accesory("Синяя дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        6606 to Accesory("Белая дамская сумка", 600000, 60, Rare.NONE, 0.0f, 150.0f, 58.0f, 0.0f, 0.0f, 0.0f),
        6604 to Accesory("Солнцезащитные очки 3", 90000, 9, Rare.NONE, 4.0f, 84.0f, 0.0f, 0.0f, 0.0f, -0.09f),
        17979 to Accesory("Крылья", 5000000, 5000, Rare.NONE, -35.0f, 63.0f, 15.0f, 0.0f, 0.0f, -0.26f),
        17992 to Accesory("Чёрные очки", 90000, 9, Rare.NONE, 4.0f, 84.0f, 0.0f, 0.0f, 0.0f, -0.09f),
        6012 to Accesory("Металоискатель", 200000, 20),
        17581 to Accesory("JBL Колонка", 2000000, 200, Rare.NONE, 0.0f, 160.0f, 0.0f, 0.0f, 0.0f, 0.0f),
        6013 to Accesory("Солнцезащитные очки", 90000, 9, Rare.NONE, 4.0f, 84.0f, 0.0f, 0.0f, 0.0f, -0.09f),
        6010 to Accesory("Солнцезащитные очки 2", 90000, 9, Rare.NONE, 4.0f, 84.0f, 0.0f, 0.0f, 0.0f, -0.09f),
        6075  to Accesory("Божественный Нимб", 5_000_000, 5_000, Rare.LEGENDARY),
        5872  to Accesory("Прян", 1_000_000, 1_000, Rare.LEGENDARY, 0.0f, 180.0f, 90.0f, 0.0f, 0.0f, 0.0f),

        //magicstore only?
        17342  to Accesory("Инфернал", 999999999, 999999999, Rare.LEGENDARY, 180.0f, 0.0f, -115.0f, 0.0f, 3.7f, 0.0f),
        17346  to Accesory("Сильфида", 999999999, 999999999, Rare.LEGENDARY, 180.0f, 0.0f, -90.0f, 0.052f, 3.5f, 0.0f),
        17350  to Accesory("Трезубец Сулимо", 999999999, 999999999, Rare.LEGENDARY),
        17351  to Accesory("Коса Баланара", 999999999, 999999999, Rare.LEGENDARY),
        16849  to Accesory("Щит Капитана Америки", 999999999, 999999999, Rare.LEGENDARY, -90.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
        16583  to Accesory("Топор Этригана", 999999999, 999999999, Rare.LEGENDARY),
        17359  to Accesory("Меч Саурона", 999999999, 999999999, Rare.LEGENDARY)
    )
}