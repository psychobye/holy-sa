#include "RadialMenu.h"
#include "util/CJavaWrapper.h"
#include "net/netgame.h"
#include "../game/Entity/Ped/Ped.h"

void CRadialMenu::Show() {
    JNIEnv* env = CJavaWrapper::GetEnv();

    if (!CRadialMenu::clazz) return;

    CRadialMenu::bIsShow = true;

    jmethodID showMethod = env->GetStaticMethodID(CRadialMenu::clazz, "show", "()V");
    env->CallStaticVoidMethod(CRadialMenu::clazz, showMethod);
}

void CRadialMenu::Update() {
    JNIEnv* env = CJavaWrapper::GetEnv();

    if (!CRadialMenu::clazz && !CRadialMenu::bIsShow) return;

    auto pPed = CLocalPlayer::GetPlayerPed();
    auto pVehicle = (pPed) ? pPed->GetCurrentVehicle() : nullptr;
    if (!pVehicle || !pVehicle->m_pVehicle) return;

    jmethodID updateMethod = env->GetStaticMethodID(CRadialMenu::clazz, "update", "(IZZZZZZZZ)V");
    if (!updateMethod) return;

    env->CallStaticVoidMethod(CRadialMenu::clazz, updateMethod,
                              pVehicle->m_pVehicle->m_nModelIndex,
                              pVehicle->m_pVehicle->m_nDoorLock == CARLOCK_LOCKED,
                              (bool)pVehicle->m_pVehicle->m_nVehicleFlags.bEngineOn,
                              (bool)(pVehicle->m_bIsLightOn >= eLightsState::ON_NEAR),
                              pVehicle->m_bIsLightOn == eLightsState::HIGH,
                              pVehicle->m_iStrobsType != eStobsStatus::OFF,
                              pVehicle->neon.IsSet(),
                              pVehicle->m_bDoorsState[eDoors::DOOR_BONNET],
                              pVehicle->m_bDoorsState[eDoors::DOOR_BOOT]
    );
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_RadialMenu_nativeOnClose(JNIEnv *env, jobject thiz) {
    CRadialMenu::bIsShow = false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_lit_game_gui_hud_HudManager_nativeClickMenu(JNIEnv *env, jobject thiz) {
    auto pPed = CLocalPlayer::GetPlayerPed();
    if (!pPed || !pPed->m_pPed->IsInVehicle()) return false;

    CRadialMenu::Show();
    CRadialMenu::Update();
    return true;
}

extern "C" JNIEXPORT void JNICALL
Java_com_lit_game_gui_RadialMenu_nativeRequestUpdate(JNIEnv *env, jobject thiz) {
    CRadialMenu::Update();
}