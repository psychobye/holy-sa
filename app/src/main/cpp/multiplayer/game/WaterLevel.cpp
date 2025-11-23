//
// Created on 23.09.2023.
//

#include "WaterLevel.h"
#include "../util/patch.h"

void CWaterLevel::RenderWaterFog() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0059BF84 + 1 : 0x6C0268));
}

void CWaterLevel::RenderWater() {
    CHook::CallFunction<void>("_ZN11CWaterLevel11RenderWaterEv");
}

bool CWaterLevel::GetWaterLevelNoWaves(CVector pos, float* pOutWaterLevel, float* pOutBigWaves, float* pOutSmallWaves) {
    return CHook::CallFunction<bool>("_ZN11CWaterLevel20GetWaterLevelNoWavesEfffPfS0_S0_", pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves);
}

void CWaterLevel::CalculateWavesOnlyForCoordinate2( // TODO: Original name didn't have a 2 in it... I'm just lazy!
        int32 x, int32 y,
        float* pResultHeight,
        float bigWavesAmpl,
        float smallWavesAmpl
) {
    CHook::CallFunction<void>("_ZN11CWaterLevel31CalculateWavesOnlyForCoordinateEiiffPf", x, y, bigWavesAmpl, smallWavesAmpl, pResultHeight);
}