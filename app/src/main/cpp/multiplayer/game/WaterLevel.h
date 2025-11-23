//
// Created on 23.09.2023.
//

#ifndef RUSSIA_WATERLEVEL_H
#define RUSSIA_WATERLEVEL_H

#include "main.h"

class CWaterLevel {

public:
    static void RenderWaterFog();
    static void RenderWater();
    static bool GetWaterLevelNoWaves(CVector pos, float* pOutWaterLevel, float* pOutBigWaves = nullptr, float* pOutSmallWaves = nullptr);

    static void CalculateWavesOnlyForCoordinate2(
            int32 x, int32 y,
            float* pResultHeight, // in/out variable => in is the "water level" from `GetWaterLevelNoWaves`/out is the Z coordinate of the wave
            float bigWavesAmpl,
            float smallWavesAmpl
    );

    static float CalculateWavesOnlyForCoordinate2_Direct( // TODO: Once the OG function is reversed, we should use this instead of it (once we've verified that the reversed version works as expected)
            int32 x, int32 y,
            float waterLevel, // "water level" from `GetWaterLevelNoWaves`
            float bigWavesAmpl,
            float smallWavesAmpl
    ) {
        CalculateWavesOnlyForCoordinate2(x, y, &waterLevel, bigWavesAmpl, smallWavesAmpl);
        return waterLevel; // Result of the above function is stored in this variable.
    }
};


#endif //RUSSIA_WATERLEVEL_H
