//
// Created on 11.02.2023.
//

#include "Speedometr.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "../game/Entity/Ped/Ped.h"

void CSpeedometr::UpdateSpeed()
{
    if( CSpeedometr::thiz == nullptr )
        return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    CPedSamp *pPed = CLocalPlayer::GetPlayerPed();
    if(!pPed) return;
    if(!pPed->m_pPed->IsInVehicle()) return;

    CVehicleMP* pVehicle = pPed->GetCurrentVehicle();

    auto vecSpeed = pVehicle->m_pVehicle->GetMoveSpeed();
    auto speed = std::sqrt(vecSpeed.x * vecSpeed.x + vecSpeed.y * vecSpeed.y + vecSpeed.z * vecSpeed.z) * 179.1f;

    auto method = env->GetMethodID(clazz, "updateSpeed", "(I)V");
    env->CallVoidMethod(CSpeedometr::thiz,method, (int) speed);
}

void CSpeedometr::UpdateInfo()
{
    if( CSpeedometr::thiz == nullptr ) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    CPedSamp *pPed = CLocalPlayer::GetPlayerPed();
    if(!pPed) return;

    if(!pPed->m_pPed->IsInVehicle()) return;

    CVehicleMP* pVehicle = pPed->GetCurrentVehicle();

    auto method = env->GetMethodID(clazz, "updateInfo", "(IIIIIII)V");

    env->CallVoidMethod(CSpeedometr::thiz,method,
            (int) CSpeedometr::fFuel,
            (int) pVehicle->GetHealth(),
            CSpeedometr::iMilliage,
            pVehicle->m_pVehicle->m_nVehicleFlags.bEngineOn,
            (int) pVehicle->m_pVehicle->GetLightsStatus(),
            (int) pVehicle->m_pVehicle->m_nDoorLock == CARLOCK_LOCKED,
            pVehicle->m_iTurnState
    );
}



extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Speedometer_sendClick(JNIEnv *env, jobject thiz, jint click_id) {
    switch(click_id)
    {
        case CSpeedometr::BUTTON_ENGINE:
        {
            pNetGame->SendChatCommand("/e");
            break;
        }
        case CSpeedometr::BUTTON_LIGHT:
        {
            pNetGame->SendChatCommand("/light");
            break;
        }
        case CSpeedometr::BUTTON_TURN_LEFT:
        {
            CPedSamp *pPlayerPed = CLocalPlayer::m_pPlayerPed;
            CVehicleMP* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == eTurnState::TURN_LEFT)
                pVehicle->m_iTurnState = eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = eTurnState::TURN_LEFT;

            break;
        }
        case CSpeedometr::BUTTON_TURN_RIGHT:
        {
            CPedSamp *pPlayerPed = CLocalPlayer::m_pPlayerPed;
            CVehicleMP* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == eTurnState::TURN_RIGHT)
                pVehicle->m_iTurnState = eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = eTurnState::TURN_RIGHT;

            break;
        }
        case CSpeedometr::BUTTON_TURN_ALL:
        {
            CPedSamp *pPlayerPed = CLocalPlayer::m_pPlayerPed;
            CVehicleMP* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == eTurnState::TURN_ALL)
                pVehicle->m_iTurnState = eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = eTurnState::TURN_ALL;

            break;
        }
    }
}