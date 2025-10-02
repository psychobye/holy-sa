//
// Created on 22.07.2023.
//

#include "RadialMenu.h"
#include "util/CJavaWrapper.h"
#include "net/netgame.h"
#include "../game/Entity/Ped/Ped.h"
void CRadialMenu::Show() {
    auto env = g_pJavaWrapper->GetEnv();

    if(CRadialMenu::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CRadialMenu::clazz, "<init>","()V");
        CRadialMenu::thiz = env->NewObject(CRadialMenu::clazz, constructor);
        CRadialMenu::thiz = env->NewGlobalRef(CRadialMenu::thiz);
    }
    CRadialMenu::bIsShow = true;
    CRadialMenu::Update();
}

void CRadialMenu::Update() {
    auto env = g_pJavaWrapper->GetEnv();

    if(CRadialMenu::thiz != nullptr) {
        auto pPed = CLocalPlayer::GetPlayerPed();
        auto pVehicle = pPed->GetCurrentVehicle();
        if(!pVehicle)return;

      //  clazz = env->FindClass("com/russia/game/gui/RadialMenu");

        auto method = env->GetMethodID(clazz, "update", "(ZZZZZZZZ)V");
        env->CallVoidMethod(thiz, method,
                            pVehicle->m_pVehicle->m_nDoorLock == CARLOCK_LOCKED,
                            (bool)pVehicle->m_pVehicle->m_nVehicleFlags.bEngineOn,
                            (bool)(pVehicle->m_bIsLightOn >= eLightsState::ON_NEAR),
                            pVehicle->m_bIsLightOn == eLightsState::HIGH, // �������
                            pVehicle->m_iStrobsType != eStobsStatus::OFF, // ����������
                            pVehicle->neon.IsSet(), // neon
                            pVehicle->m_bDoorsState[eDoors::DOOR_BONNET], // �����
                            pVehicle->m_bDoorsState[eDoors::DOOR_BOOT]
                            );
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_RadialMenu_nativeOnClose(JNIEnv *env, jobject thiz) {
    env->DeleteGlobalRef(CRadialMenu::thiz);
    CRadialMenu::thiz = nullptr;

    CRadialMenu::bIsShow = false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_lit_game_gui_hud_HudManager_nativeClickMenu(JNIEnv *env, jobject thiz) {
    auto pPed = CLocalPlayer::GetPlayerPed();
   // auto pVehicle = pPed->GetCurrentVehicle();
    if(!pPed->m_pPed->IsInVehicle())return false;

    CRadialMenu::Show();
    return true;
}