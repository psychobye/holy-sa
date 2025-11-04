//
// Created on 03.02.2023.
//

#include "WidgetGta.h"
#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "util/patch.h"
#include "java_systems/HUD.h"
#include "WidgetRegionLook.h"
#include "java_systems/SelectEntity.h"
#include "TouchInterface.h"
#include "WidgetButton.h"

void CWidgetGta::SetEnabled(bool bEnabled) {
    m_bEnabled = bEnabled;
}

void updateWidgetVisibility(CWidgetGta* widget, bool shouldShow) {
    if (widget) {
        if (shouldShow) {
            if (widget->m_fOriginY < 0) {
                widget->m_fOriginY += 10000.0f;
            }
        } else {
            if (widget->m_fOriginY >= 0) {
                widget->m_fOriginY -= 10000.0f;
            }
        }
    }
}

bool (*CWidget__IsTouched)(uintptr_t *thiz, CVector2D *pVecOut);
bool CWidget__IsTouched_hook(uintptr_t *thiz, CVector2D *pVecOut) {
//    if(*thiz == CWidgetGta::pWidgets[WIDGET_POSITION_HORN]) {
//        return true;
//    }
//    if(!CHUD::bIsShow)
//        return false;


    return CWidget__IsTouched(thiz, pVecOut);
}

void (*CWidgetButton__Update)(CWidgetButton* thiz);
void CWidgetButton__Update_hook(CWidgetButton* thiz) {
    CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR]->m_fTapHoldTime = 1.0;
    CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_PHONE]->m_bEnabled = false; // voice
    CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_VEHICLE_SHOOT_LEFT]->m_bEnabled = false;
    CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_VEHICLE_SHOOT_RIGHT]->m_bEnabled = false;

    if (pNetGame) {
        const auto pPlayerPed = CLocalPlayer::GetPlayerPed();
        if (pPlayerPed) {
            bool bSwimming = pPlayerPed->m_pPed->physicalFlags.bSubmergedInWater;
            bool isInVehicle = pPlayerPed->m_pPed->IsInVehicle();
            bool isDriver = pPlayerPed->m_pPed->IsADriver();
            const auto pVehicle = pPlayerPed->GetCurrentVehicle();
            bool isEngine = false;
            if (pVehicle) {
                isEngine = pVehicle->m_bIsEngineOn;
            }

            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_BUTTON_SWIM], bSwimming);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_BUTTON_DIVE], bSwimming);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_SPRINT], (bSwimming || isInVehicle) ? false : true);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_PED_MOVE], isInVehicle ? false : true);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_ACCELERATE], (!bSwimming && isDriver && isEngine) ? true : false);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_BRAKE], (!bSwimming && isDriver && isEngine) ? true : false);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_HANDBRAKE], (!bSwimming && isDriver && isEngine) ? true : false);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_VEHICLE_STEER_ANALOG], (!bSwimming && isDriver && isEngine) ? true : false);
            updateWidgetVisibility(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_HORN], (!bSwimming && isDriver && isEngine) ? true : false);
        }
    }

    CWidgetButton__Update(thiz);
}

void (*CWidgetRegionLook__Update)(CWidgetRegionLook *thiz);

void CWidgetRegionLook__Update_hook(CWidgetRegionLook *thiz) {
    CWidgetRegionLook__Update(thiz);

    if (thiz->m_bLookBack) {
        thiz->m_bLookBack = false;
        CActionsPed::bPressed = true;
        return;
    }
    CActionsPed::bPressed = false;
}

void (*CWidgetButtonEnterCar__Draw)(uintptr_t *thiz);
void CWidgetButtonEnterCar__Draw_hook(uintptr_t *thiz) {


    if(!CHUD::bIsShowEnterExitButt)
        return;

    CWidgetButtonEnterCar__Draw(thiz);
}

void (*CWidgetButton__Enabled)(CWidgetButton* thiz, bool bEnabled);
void CWidgetButton__Enabled_hook(CWidgetButton* thiz, bool bEnabled) {
    int widgetId = -1;
    for (int i = 0; i < WidgetIDs::NUM_WIDGETS; ++i) {
        if (CTouchInterface::m_pWidgets[i] == thiz) {
            widgetId = i;
            break;
        }
    }

    if (widgetId >= 0 && CTouchInterface::hiddenWidgets[widgetId]) {
        bEnabled = false;
        CWidgetButton__Enabled(thiz, bEnabled);
        return;
    }

    if (pNetGame) {
        const auto& pPlayerPed = CLocalPlayer::GetPlayerPed();
        if (pPlayerPed) {
            bool bSwimming = pPlayerPed->m_pPed->physicalFlags.bSubmergedInWater;
            bool isInVehicle = pPlayerPed->m_pPed->IsInVehicle();
            bool isInDriver = pPlayerPed->m_pPed->IsADriver();

            struct WidgetCondition {
                CWidgetGta* widget;
                bool (*condition)(bool, bool, bool);
            };

            WidgetCondition widgetConditions[] = {
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_BUTTON_SWIM], [](bool swimming, bool, bool) { return swimming; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_BUTTON_DIVE], [](bool swimming, bool, bool) { return swimming; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_SPRINT], [](bool swimming, bool inVehicle, bool) { return (swimming || inVehicle) ? false : true; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_PED_MOVE], [](bool, bool inVehicle, bool) { return inVehicle ? false : true; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_ACCELERATE], [](bool swimming, bool, bool isDriver) { return (!swimming && isDriver) ? true : false; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_BRAKE], [](bool swimming, bool, bool isDriver) { return (!swimming && isDriver) ? true : false; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_HANDBRAKE], [](bool swimming, bool, bool isDriver) { return (!swimming && isDriver) ? true : false; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_VEHICLE_STEER_ANALOG], [](bool swimming, bool, bool isDriver) { return (!swimming && isDriver) ? true : false; }},
                    { CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_HORN], [](bool swimming, bool, bool isDriver) { return (!swimming && isDriver) ? true : false; }},
            };

            for (const auto& widgetCondition : widgetConditions) {
                if (thiz == widgetCondition.widget) {
                    bEnabled = widgetCondition.condition(bSwimming, isInVehicle, isInDriver);
                    break;
                }
            }
        }
    }

    CWidgetButton__Enabled(thiz, bEnabled);
}

void CWidgetGta::InjectHooks() {
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66486C : 0x82A150), &CWidgetRegionLook__Update_hook, &CWidgetRegionLook__Update);
    //CHook::InlineHook("_ZN21CWidgetButtonEnterCar4DrawEv", &CWidgetButtonEnterCar__Draw_hook, &CWidgetButtonEnterCar__Draw); // LOL, for what? xD
    CHook::InlineHook("_ZN7CWidget10SetEnabledEb", &CWidgetButton__Enabled_hook, &CWidgetButton__Enabled);
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x00671424 : 0x842408), &CWidgetButton__Update_hook, &CWidgetButton__Update);
}

void CWidgetGta::SetTexture(const char *name) {
    m_Sprite.m_pTexture = CUtil::LoadTextureFromDB("mobile", name);
}

bool CWidgetGta::IsReleased(CVector2D *pVecOut) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x002B3484 + 1 : 0x372794), this, pVecOut);
}

bool CWidgetGta::IsTouched(CVector2D *pVecOut) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x002B3324 + 1 : 0x3725D0), this, pVecOut);
}


