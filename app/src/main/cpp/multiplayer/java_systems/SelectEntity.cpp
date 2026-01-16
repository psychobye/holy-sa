//
// Created on 01.05.2023.
//
#include "game/Core/Vector.h"
#include "net/netgame.h"
#include "game/Render/Sprite.h"
#include "../game/Widgets/TouchInterface.h"
#include "game/util.h"
#include "gui/gui.h"
#include "SelectEntity.h"
#include "util/CJavaWrapper.h"
#include "../game/Entity/Ped/Ped.h"
#include "util/patch.h"
#include "game/Widgets/WidgetGta.h"

uint16_t CActionsPed::findSelectEntity() {
    if (!pNetGame) return INVALID_PLAYER_ID;

    CVector vecOut;

    m_nSelectedId   = INVALID_PLAYER_ID;

    float minDist = 1000.f;

    for (auto & pair: CPlayerPool::spawnedPlayers) {
        auto pPlayer = pair.second;

        if(pPlayer->GetPlayerPed()->m_pPed->GetDistanceFromLocalPlayerPed() > 5)
            continue;

        CVector vec = pPlayer->GetPlayerPed()->m_pPed->GetPosition();

        CSprite::CalcScreenCoors(vec, &vecOut, nullptr, nullptr, false, false);

        float dist = CUtil::GetDistanceBetween2DPoints(
                {(RwReal) CTouchInterface::lastPosX, (RwReal) CTouchInterface::lastPosY },
                {vecOut.x, vecOut.y}
        );
        dist = abs(dist);
        
        if(minDist > dist && dist < 150) {
            minDist = dist;
            m_nSelectedId = pPlayer->GetID();
        }
    }
    return m_nSelectedId;
}

void CActionsPed::drawProgress()
{
    if(bPressed && !bIsShow) {
//        if(CGUI::m_bAnyItemHovered) {
//            bPressed = false;
//            m_dwProgress = 0;
//            return;
//        }
        if(m_dwProgress == 0) {
            if (findSelectEntity() != INVALID_PLAYER_ID)
                m_dwProgress ++;

            return;
        }
        else if(m_dwProgress > 0){
                m_dwProgress++;
                CGUI::CreateCircleProgressBar(
                        ImVec2(CTouchInterface::lastPosX, CTouchInterface::lastPosY - 300),
                        0xff0000ff,
                        m_dwProgress,
                        60,
                        18
                );

                if(m_dwProgress == 100) {
                    showActions();
                }
        }
        return;
    }
    m_dwProgress = 0;
}

void CActionsPed::showActions() {
    if(!thiz)
        Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    auto method = env->GetMethodID(clazz, "show", "(Ljava/lang/String;I)V");

    auto name = env->NewStringUTF( CPlayerPool::GetPlayerName(m_nSelectedId) );

    env->CallVoidMethod(thiz, method, name, m_nSelectedId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_ActionsPed_nativeDelCppObject(JNIEnv *env, jobject thiz) {
    CActionsPed::DeleteCppObject();
}