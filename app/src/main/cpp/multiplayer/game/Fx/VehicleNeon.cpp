//
// Created on 29.05.2024.
//

#include "VehicleNeon.h"
#include "Timer.h"
#include "Models/ModelInfo.h"
#include "game/Shadow/Shadows.h"

void CVehicleNeon::Process() {
    if( neonType <= eNeonTypes::ON_TYPE_STATIC )
        return;

    uint32 dif = CTimer::m_snTimeInMillisecondsNonClipped - lastNeonTime;

    if(neonType == eNeonTypes::ON_TYPE_RAINDOW) {
        neonColor.RainbowCycle(CTimer::m_snTimeInMillisecondsNonClipped);
        return;
    }

    if (neonType == eNeonTypes::ON_TYPE_FLASH) {
        if(dif >= 200) {
            if (neonState == 0) {
                neonColor.a = 0;
                neonState = 1;
            } else if (neonState == 1) {
                neonColor.a = 255;
                neonState = 2;
            } else if (neonState == 2) {
                neonColor.a = 0;
                neonState = 3;
            } else if (neonState == 3) {
                neonColor.a = 255;
                neonState = 4;
            } else if (neonState == 4) {
                neonState = 5;
            } else if (neonState == 5) {
                neonState = 0;
            }
            lastNeonTime = CTimer::m_snTimeInMillisecondsNonClipped;
        }
        return;
    }
    if (neonType == eNeonTypes::ON_TYPE_SHARP) {
        if(dif >= 150) {
            neonColor = CRGBA::RandomColor();
            lastNeonTime = CTimer::m_snTimeInMillisecondsNonClipped;
        }
        return;
    }
}

void CVehicleNeon::Render(CVehicle* veh) const
{
    if(neonColor.a == 0)
        return;

    CVector Pos = CModelInfo::ms_modelInfoPtrs[veh->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMin;
    CVector center  = veh->TransformFromObjectSpace(CVector(0.0f, 0.0f, 0.0f));
    CVector up      = veh->TransformFromObjectSpace(CVector(0.0f, -Pos.y - 0.5f, 0.0f)) - center;
    CVector right   = veh->TransformFromObjectSpace(CVector(Pos.x + 0.2f, 0.0f, 0.0f)) - center;

    CShadows::StoreShadowToBeRendered(SHADOW_ADDITIVE,
                                      pNeonTex,
                                      &center, up.x * 1.8f, up.y * 1.8f,
                                      right.x * 2.f, right.y * 2.f,
                                      -100, neonColor.r, neonColor.g, neonColor.b,
                                      4.0f,
                                      false,
                                      1.0f,
                                      nullptr,
                                      true
    );
}
