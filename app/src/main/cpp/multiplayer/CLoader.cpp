#include <jni.h>
#include "CLoader.h"
#include "util/patch.h"
#include "crashlytics.h"
#include "CSettings.h"
#include "net/netgame.h"
#include "java_systems/Speedometr.h"
#include "java_systems/Donate.h"
#include "java_systems/ObjectEditor.h"
#include "java_systems/Styling.h"
#include "java_systems/TireShop.h"
#include "java_systems/casino/Dice.h"
#include "java_systems/TheftAuto.h"
#include "java_systems/casino/Baccarat.h"
#include "java_systems/Tab.h"
#include "java_systems/SelectEntity.h"
#include "java_systems/AdminRecon.h"
#include "java_systems/DailyReward.h"
#include "java_systems/AutoShop.h"
#include "java_systems/TechInspect.h"
#include "java_systems/AucContainer.h"
#include "java_systems/ChooseSpawn.h"
#include "java_systems/casino/LuckyWheel.h"
#include "java_systems/casino/Chip.h"
#include "java_systems/Treasure.h"
#include "gui/GiftNotify.h"
#include "java_systems/FurnitureFactory.h"
#include "java_systems/Samwill.h"
#include "java_systems/GunStore.h"
#include "java_systems/Achivments.h"
#include "java_systems/OilFactory.h"
#include "java_systems/FuelStation.h"
#include "java_systems/RadialMenu.h"
#include "java_systems/Authorization.h"
#include "java_systems/ArmyGame.h"
#include "java_systems/Medic.h"
#include "java_systems/Inventory.h"
#include "java_systems/GuiWrapper.h"
#include "java_systems/mine/MineGame1.h"
#include "java_systems/mine/MineGame2.h"
#include "java_systems/mine/MineGame3.h"
#include "java_systems/SkinShop.h"
#include "java_systems/Registration.h"
#include "java_systems/Taxi.h"
#include "java_systems/Race.h"
#include "java_systems/BattlePass.h"
#include "java_systems/Milk.h"
#include "java_systems/Menu.h"
#include "java_systems/Quest.h"
#include "java_systems/cef/CEF.h"
#include "GuiWrapper.h"
#include "MagicStore.h"
#include "SnapShotsWrapper.h"
#include "Monologue.h"
#include "MiningStore.h"
#include "BuyPlate.h"
#include "BusStation.h"
#include "WarPoints.h"
#include "BattlePassBuy.h"
#include "JavaGui.h"

// ok?
void CLoader::initCrashLytics()
{
    firebase::crashlytics::SetCustomKey("build data", __DATE__);
    firebase::crashlytics::SetCustomKey("build time", __TIME__);

    firebase::crashlytics::SetUserId(CSettings::m_Settings.szNickName);
    firebase::crashlytics::SetCustomKey("Nick", CSettings::m_Settings.szNickName);

    char str[100];

    sprintf(str, "0x%x", g_libGTASA);
    firebase::crashlytics::SetCustomKey("libGTASA.so", str);

    sprintf(str, "0x%x", g_libSAMP);
    firebase::crashlytics::SetCustomKey("libsamp.so", str);
}

void CLoader::loadSetting()
{
    CSettings::LoadSettings(nullptr);
}

jclass LinkJavaClass(jclass localObj) {
    auto env = CJavaWrapper::GetEnv();
    auto globalRef = (jclass)env->NewGlobalRef(localObj);
    env->DeleteLocalRef(localObj);
    return globalRef;
}

void CLoader::initJavaClasses(JavaVM* pjvm) {
    JNIEnv* env = nullptr;
    (void)pjvm->GetEnv((void**)&env, JNI_VERSION_1_6);

    CJavaGui::clazz = LinkJavaClass(env->FindClass("com/holy/game/NewUiList"));
    
    CBattlePassBuy::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/battle_pass/BattlePassBuy"));
    CWarPoints::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/WarPoints"));
    CBusStation::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/BusStation"));
    CBattlePass::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/battle_pass/BattlePass"));
    CBuyPlate::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/BuyPlate"));
    CMiningStore::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/MiningStore"));
    CMonologue::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Monologue"));
    SnapShotsWrapper::clazz = LinkJavaClass(env->FindClass("com/holy/game/EntitySnaps"));
    CRace::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Race"));
    CMagicStore::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/magicStore/MagicStore"));
    CTaxi::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Taxi"));
    CRegistration::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Registration"));
    CSkinShop::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/SkinAndAcsShop"));
    CMineGame3::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/MineGame3"));
    CMineGame2::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/MineGame2"));
    CMineGame1::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/MineGame1"));
    CInventory::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/inventory/Inventory"));
    CMedic::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/PreDeath"));
    CArmyGame::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/ArmyGame"));
    CAuthorization::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Authorization"));
    CRadialMenu::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/RadialMenu"));
    CFuelStation::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/FuelStation"));
    COilFactory::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/fillingGames/OilFactory"));
    CMilk::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/fillingGames/Milk"));
    CAchivments::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/achivments/Achivments"));
    CGunStore::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/GunShop"));
    CSamwill::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Samwill"));
    CFurnitureFactory::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/FurnitureFactory"));
    CGiftNotify::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/GiftNotify"));
    CTreasure::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/treasure/Treasure"));
    CChip::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/casino/BuySellChip"));
    CLuckyWheel::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/casino/LuckyWheel"));
    CChooseSpawn::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/ChooseSpawn"));
    CAucContainer::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/AucContainer"));
    CTechInspect::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/TechIspect"));
    CAutoShop::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/AutoShop"));
    CDailyReward::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/DailyReward"));
    CAdminRecon::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/AdminRecon"));
    CTab::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/tab/Tab"));
    CSpeedometr::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/Speedometer"));
    CDonate::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/donate/Donate"));
    CObjectEditor::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/AttachEdit"));
    CStyling::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/styling/Styling"));
    CTireShop::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/tire_shop/TireShop"));
    CDice::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/casino/Dice"));
    CTheftAuto::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/theft_auto/TheftAuto"));
    CBaccarat::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/CasinoBaccarat"));
    CActionsPed::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/ActionsPed"));
    CMenu::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/menu/Menu"));
    CQuest::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/quest/Quest"));
    CCEF::clazz = LinkJavaClass(env->FindClass("com/holy/game/gui/cef/CefManager"));
}