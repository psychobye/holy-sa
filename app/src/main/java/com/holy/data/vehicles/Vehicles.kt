package com.holy.data.vehicles

import com.holy.game.EntitySnaps
import com.holy.game.SnapShot
import com.holy.game.gui.donate.Donate
import com.holy.game.gui.donate.DonateItem
import com.holy.game.gui.treasure.TreasureItem
import com.holy.data.Rare


object Vehicles {
    fun getPriceRub(modelId: Int) : Int {
        return list[modelId]?.price ?: 0
    }

    fun getPriceLc(modelId: Int) : Int {
        return list[modelId]?.donate_price ?: 0
    }

    fun getRare(modelId: Int) : Int {
        return Rare.getRareFromPrice(getPriceLc(modelId))
    }

    fun getName(modelId: Int) : String {
        return list[modelId]?.name ?: "Invalid Id"
    }

    fun getData(modelId: Int) : Vehicle? {
        return list[modelId]
    }

    fun getSnap(modelId: Int): SnapShot {
        return SnapShot(
            EntitySnaps.TYPE_VEHICLE,
            modelId,
            20f, 180f, 30f,
            0.0f, 0.0f, 0.0f
        )
    }

//    @JvmStatic
//    @SuppressLint("DefaultLocale")
//    fun getResource(modelId: Int): Int {
//        val str = String.format("auc_veh_%d", modelId)
//        return Samp.activity.resources.getIdentifier(str, "drawable", Samp.activity.packageName)
//    }

//    fun createDonateItem(modelId: Int): DonateItem {
//        return DonateItem(getName(modelId), Donate.CATEGORY_CARS, getPriceLc(modelId), getResource(modelId), modelId)
//       // return DonateItem(getName(modelId), Donate.CATEGORY_CARS, getPriceLc(modelId), modelId, getSnap(modelId))
//    }
    fun createDonateItem(modelId: Int): DonateItem {
        return DonateItem(getName(modelId), Donate.CATEGORY_CARS, getPriceLc(modelId), modelId, getSnap(modelId))
    }

    fun createTreasureItem(modelId: Int): TreasureItem {
        return TreasureItem(getSnap(modelId), getName(modelId), getRare(modelId))
    }

    val list = hashMapOf(
        400 to Vehicle("Nissan Sentra", 900_000, 450),
        401 to Vehicle("BMW 3 Series E30", 490_000, 245),
        402 to Vehicle("Honda Civic Type R", 1_225_000, 612),
        403 to Vehicle("МАЗ", 7_000_000, 0),
        404 to Vehicle("Mitsubishi Lancer IX", 1_140_000, 570),
        405 to Vehicle("BMW E60", 1_350_000, 675),
        406 to Vehicle("Газон", 7_000_000, 0),
        407 to Vehicle("MAN Пожарный", 7_000_000, 0),
        408 to Vehicle("Mercedes-Benz Мусоровоз", 7_000_000, 0),
        409 to Vehicle("Газ Чайка", 7_000_000, 0),
        410 to Vehicle("Nissan GT-R", 5_100_000, 2550),
        411 to Vehicle("Bugatti Veyron", 110_000_000, 55000),
        412 to Vehicle("BMW E39", 660_000, 330),
        413 to Vehicle("Ford Инкассация", 7_000_000, 0),
        414 to Vehicle("Mercedes-Benz Бензовоз", 7_000_000, 0),
        415 to Vehicle("Lamborghini Huracan", 14_000_000, 7000),
        416 to Vehicle("Ford Transit Скорая", 7_000_000, 0),
        417 to Vehicle("Leviathan", 7_000_000, 0),
        418 to Vehicle("GAZ Vector Next", 7_000_000, 0),
        419 to Vehicle("Bentley Continental GT", 7_500_000, 3750),
        420 to Vehicle("VAZ 2109", 175_000, 87),
        421 to Vehicle("BMW 5 Series E34", 530_000, 265),
        422 to Vehicle("Буханка", 7_000_000, 0),
        423 to Vehicle("Mr Whoopee", 7_000_000, 0),
        424 to Vehicle("Квадроцикл", 740_000, 370),
        425 to Vehicle("Hunter", 7_000_000, 0),
        426 to Vehicle("Mercedes-Benz s600", 1_740_000, 870),
        427 to Vehicle("Камаз ОМОН", 7_000_000, 0),
        428 to Vehicle("Газель ВТБ24", 7_000_000, 0),
        429 to Vehicle("Porshe Panamera S", 8_400_000, 4200),
        430 to Vehicle("Predator", 7_000_000, 0),
        431 to Vehicle("Ikarus 250", 7_000_000, 0),
        432 to Vehicle("Танк", 7_000_000, 0),
        433 to Vehicle("Урал 4320", 7_000_000, 0),
        434 to Vehicle("Hotknife", 7_000_000, 0),
        435 to Vehicle("Прицеп 1", 7_000_000, 0),
        436 to Vehicle("Volkswagen Golf", 1_270_000, 635),
        437 to Vehicle("Ikarus 260", 7_000_000, 0),
        438 to Vehicle("LADA 2114", 187_000, 93),
        439 to Vehicle("Волга 3102", 450_000, 0),
        440 to Vehicle("УАЗ-452 Военный", 7_000_000, 0),
        441 to Vehicle("RC Bandit", 7_000_000, 0),
        442 to Vehicle("Volkswagen Polo", 7_000_000, 800),
        443 to Vehicle("Packer", 7_000_000, 0),
        444 to Vehicle("Monster", 20_000_000, 10_000),
        445 to Vehicle("Admiral", 1_500_000, 0),
        446 to Vehicle("Squallo", 7_000_000, 0),
        447 to Vehicle("Seasparrow", 7_000_000, 0),
        448 to Vehicle("Pizzaboy", 7_000_000, 0),
        449 to Vehicle("Tram", 7_000_000, 0),
        450 to Vehicle("Прицеп_2", 7_000_000, 0),
        451 to Vehicle("Lamborghini Aventador", 17_000_000, 8500),
        452 to Vehicle("Speeder", 7_000_000, 0),
        453 to Vehicle("Reefer", 7_000_000, 0),
        454 to Vehicle("Reefer", 7_000_000, 0),
        455 to Vehicle("Снегоуборочная машина", 7_000_000, 0),
        456 to Vehicle("MAN", 7_000_000, 0),
        457 to Vehicle("Caddy", 7_000_000, 0),
        458 to Vehicle("BMW X5M E70", 2_500_000, 1250),
        459 to Vehicle("Topfun Van", 7_000_000, 0),
        460 to Vehicle("Skimmer", 7_000_000, 0),
        461 to Vehicle("Ducati SuperSport S", 1_500_000, 750),
        462 to Vehicle("Мопед Вятка", 7_000_000, 0),
        463 to Vehicle("Ducati XDiavel S", 3_500_000, 1750),
        464 to Vehicle("RC Baron", 7_000_000, 0),
        465 to Vehicle("RC Raider", 7_000_000, 0),
        466 to Vehicle("Mercedes-Benz GLE 350", 18_000_000, 2250),
        467 to Vehicle("LADA Priora", 600_000, 300),
        468 to Vehicle("Aprilia-SXV", 1_000_000, 500),
        469 to Vehicle("Sparrow", 7_000_000, 0),
        470 to Vehicle("Volvo XC90", 4_200_000, 2100),
        471 to Vehicle("Снегоход", 5_000_000, 2500),
        472 to Vehicle("Coastguard", 7_000_000, 0),
        473 to Vehicle("Dinghy", 7_000_000, 0),
        474 to Vehicle("NIVA 2121", 130_000, 65),
        475 to Vehicle("Mercedes-Benz E230", 210_000, 105),
        476 to Vehicle("Rustler", 7_000_000, 0),
        477 to Vehicle("Rolls-Royce Wraith", 25_000_000, 12_500),
        478 to Vehicle("Тигр Военный", 7_000_000, 0),
        479 to Vehicle("Skoda Octavia", 790_000, 395),
        480 to Vehicle("Lexus LFA", 3_650_000, 1825),
        481 to Vehicle("Аист", 7_000_000, 0),
        482 to Vehicle("Burrito", 7_000_000, 0),
        483 to Vehicle("ПАЗ", 7_000_000, 0),
        484 to Vehicle("Marquis", 7_000_000, 0),
        485 to Vehicle("Волга", 7_000_000, 0),
        486 to Vehicle("Dozer", 7_000_000, 0),
        487 to Vehicle("Maverick", 7_000_000, 0),
        488 to Vehicle("SAN News Maverick", 7_000_000, 0),
        489 to Vehicle("Mercedes-Benz G63 6x6", 25_000_000, 12_500),
        490 to Vehicle("FBI Rancher", 7_000_000, 0),
        491 to Vehicle("Mercedes-Maybach S650", 9_000_000, 4_500),
        492 to Vehicle("Opel Insignia", 880_000, 440),
        493 to Vehicle("Jetmax", 7_000_000, 0),
        494 to Vehicle("Nissan Silvia S14", 600_000, 300),
        495 to Vehicle("Ford Raptor 6x6", 20_000_000, 750),
        496 to Vehicle("BMW M5 E39", 1_500_000, 750),
        497 to Vehicle("Police Maverick", 7_000_000, 0),
        498 to Vehicle("Ikarus", 7_000_000, 0),
        499 to Vehicle("ГАЗ Хлеб", 7_000_000, 0),
        500 to Vehicle("Mesa", 7_000_000, 0),
        501 to Vehicle("RC Goblin", 7_000_000, 0),
        502 to Vehicle("Subaru Impreza WRX STI", 1_900_000, 950),
        503 to Vehicle("Dodge Demon SRT", 3_500_000, 1750),
        504 to Vehicle("Cadillac Escalade", 10_000_000, 5000),
        505 to Vehicle("Jeep Grand Cherokee", 4_500_000, 2_250),
        506 to Vehicle("Porsche 911 Carrera S", 10_200_000, 5100),
        507 to Vehicle("Audi A6", 1_600_000, 800),
        508 to Vehicle("УАЗ-452", 7_000_000, 0),
        509 to Vehicle("Bike", 7_000_000, 0),
        510 to Vehicle("Mountain Bike", 7_000_000, 0),
        511 to Vehicle("Beagle", 7_000_000, 0),
        512 to Vehicle("Cropduster", 7_000_000, 0),
        513 to Vehicle("Stuntplane", 7_000_000, 0),
        514 to Vehicle("DAF FX", 7_000_000, 0),
        515 to Vehicle("КАЗ", 7_000_000, 0),
        516 to Vehicle("Audi A6 Universal", 1_450_000, 725),
        517 to Vehicle("Lada 2107", 95_000, 47),
        518 to Vehicle("LADA 2101", 100_000, 50),
        519 to Vehicle("Shamal", 7_000_000, 0),
        520 to Vehicle("Hydra", 7_000_000, 0),
        521 to Vehicle("BMW S1000 RR", 2_500_000, 1_250),
        522 to Vehicle("Kawasaki Ninja", 4_700_000, 2_350),
        523 to Vehicle("Yamaha FZR-8", 7_000_000, 0),
        524 to Vehicle("Cement Truck", 7_000_000, 0),
        525 to Vehicle("Mercedes-Benz Эвакуатор", 7_000_000, 0),
        526 to Vehicle("Subaru Legacy", 250_000, 125),
        527 to Vehicle("Lada Vesta", 340_000, 170),
        528 to Vehicle("SWAT", 7_000_000, 175),
        530 to Vehicle("Forklift", 7_000_000, 0),
        531 to Vehicle("Трактор", 7_000_000, 0),
        532 to Vehicle("Combine Harvester", 7_000_000, 0),
        533 to Vehicle("Mercedes-Benz c63 ФСБ", 7_000_000, 0),
        534 to Vehicle("Range Rover Sport", 6_200_000, 3_100),
        535 to Vehicle("Nissan Skyline R34", 7_000_000, 1_750),
        536 to Vehicle("Volvo 242 DL", 200_000, 100),
        537 to Vehicle("Freight (Train)", 7_000_000, 0),
        538 to Vehicle("Brownstreak (Train)", 7_000_000, 0),
        539 to Vehicle("Vortex", 7_000_000, 0),
        540 to Vehicle("УАЗ - Патриот", 1_000_000, 500),
        541 to Vehicle("Ferrari La Ferrari", 15000000, 7500),
        542 to Vehicle("Toyota Camry 3.5", 3300000, 1650),
        543 to Vehicle("Mercedes-Benz g63", 10000000, 5000),
        544 to Vehicle("Firetruck LA", 7000000, 25000),
        545 to Vehicle("Москвич 149", 7000000, 25000),
        546 to Vehicle("Mercedes-Benz GT63s", 10500000, 4250),
        547 to Vehicle("Audi RS7", 11700000, 5850),
        548 to Vehicle("Cargobob", 7000000, 25000),
        549 to Vehicle("Volkswagen Scirocco", 960000, 480),
        550 to Vehicle("Alfa Romeo Giulia", 1800000, 900),
        551 to Vehicle("BMW M5", 11450000, 5725),
        552 to Vehicle("Камаз ФСБ", 7000000, 25000),
        553 to Vehicle("Nevada", 7000000, 25000),
        554 to Vehicle("Daewoo Matiz", 350_000, 175),
        555 to Vehicle("ЗАЗ 968", 7000000, 50000),
        556 to Vehicle("Monster A", 7000000, 25000),
        557 to Vehicle("Monster B", 7000000, 25000),
        558 to Vehicle("Rolls-Royce Cullinan", 25000000, 12500),
        559 to Vehicle("Audi RS6", 5700000, 2850),
        560 to Vehicle("Subaru Impreza", 1500000, 750),
        561 to Vehicle("Toyota Land Cruiser Prado", 2700000, 1350),
        562 to Vehicle("Toyota Mark 2", 730000, 365),
        563 to Vehicle("Raindance", 7000000, 25000),
        564 to Vehicle("RC Tiger", 7000000, 25000),
        565 to Vehicle("Mercedes-Benz a45 AMG", 2650000, 1325),
        566 to Vehicle("Mercedes-Benz C63 AMG", 4200000, 2100),
        567 to Vehicle("Maybach", 7_000_000, 4_500),
        568 to Vehicle("Bandito", 5000000, 2500),
        569 to Vehicle("Freight Flat Trailer (Train)", 7000000, 25000),
        570 to Vehicle("Streak Trailer (Train)", 7000000, 25000),
        571 to Vehicle("Kart", 5000000, 2500),
        572 to Vehicle("Mower", 7000000, 25000),
        573 to Vehicle("Dune", 7000000, 25000),
        574 to Vehicle("Sweeper", 7000000, 25000),
        575 to Vehicle("Chevrolette Camaro ZL1", 7000000, 7_500),
        576 to Vehicle("Tesla Cubertruck", 7000000, 3500),
        577 to Vehicle("AT400", 7000000, 25000),
        578 to Vehicle("Снегоуборочная машина", 7000000, 25000),
        579 to Vehicle("Lamborghini Urus", 17000000, 8500),
        580 to Vehicle("Mercedes-Benz G65 AMG", 7000000, 5_000),
        581 to Vehicle("Yamaha FZ-10", 2000000, 1000),
        582 to Vehicle("Ford Transit СМИ ", 7000000, 25000),
        583 to Vehicle("Tug", 7000000, 25000),
        584 to Vehicle("Petrol Trailer", 7000000, 25000),
        585 to Vehicle("VAZ 2115", 118000, 60),
        586 to Vehicle("Путник", 1500000, 750),
        587 to Vehicle("Audi R8", 8200000, 4100),
        588 to Vehicle("Ikarus M", 7000000, 25000),
        589 to Vehicle("VAZ-1111 (OKA)", 25_000, 12),
        590 to Vehicle("Freight Box Trailer (Train)", 7_000_000, 0),
        591 to Vehicle("Article Trailer 3", 7_000_000, 0),
        592 to Vehicle("Andromada", 7_000_000, 0),
        593 to Vehicle("Dodo", 7_000_000, 0),
        594 to Vehicle("RC Cam", 7_000_000, 0),
        595 to Vehicle("Launch", 7_000_000, 0),
        596 to Vehicle("Skoda Octavia МВД", 7_000_000, 0),
        597 to Vehicle("LADA Priora МВД", 7_000_000, 0),
        598 to Vehicle("Mercedes-Benz c63 МВД", 7_000_000, 0),
        599 to Vehicle("Police Ranger", 7_000_000, 0),
        600 to Vehicle("Ford Mustang GT", 7_280_000, 3_640),
        602 to Vehicle("Alpha", 7_000_000, 0),
        603 to Vehicle("BMW M8", 16_500_000, 8250),
        604 to Vehicle("Mercedes-Benz E420", 550_000, 275),
        605 to Vehicle("Ракета", 7_000_000, 0)
    )
}
