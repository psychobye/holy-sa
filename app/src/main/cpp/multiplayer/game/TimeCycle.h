//
// Created on 20.09.2023.
//

#ifndef RUSSIA_TIMECYCLE_H
#define RUSSIA_TIMECYCLE_H

#include "ColourSet.h"
#include "Enums/eWeatherType.h"
#include "Collision/Box.h"

class CTimeCycleBox {
public:
    CBox  m_Box;
    int16 m_FarClip;
    uint8 m_LodDistMult;
    int32 m_ExtraColor;
    float m_Strength;
    float m_Falloff;
};
VALIDATE_SIZE(CTimeCycleBox, 0x28);

class CTimeCycle {
public:
    static constexpr auto NUM_HOURS = 8;
    static inline float gfLaRiotsLightMult = 1.0f;

    static inline CVector m_vecDirnLightToSun;
    static inline int32 m_FogReduction;
    static inline uint32 m_bExtraColourOn;
    static inline int32 m_ExtraColour;
    static inline int32 m_ExtraColourWeatherType;
    static inline float m_ExtraColourInter;

    static inline CColourSet m_CurrentColours;
    static inline RwRGBA m_BelowHorizonGrey;

    static inline CVector m_VectorToSun[16];
    static inline float m_fShadowDisplacementX[16];
    static inline float m_fShadowDisplacementY[16];
    static inline float m_fShadowFrontX[16];
    static inline float m_fShadowFrontY[16];
    static inline float m_fShadowSideX[16];
    static inline float m_fShadowSideY[16];
    static inline uint32 m_CurrentStoredValue;

    static inline CTimeCycleBox m_aBoxes[20];
    static inline uint32 m_NumBoxes;

    static inline uint8 m_nAmbientRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nAmbientGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nAmbientBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nAmbientRed_Obj[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nAmbientGreen_Obj[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nAmbientBlue_Obj[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nSkyTopRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSkyTopGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSkyTopBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nSkyBottomRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSkyBottomGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSkyBottomBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_fSunSize[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nSunCoronaRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSunCoronaGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSunCoronaBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nSunCoreRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSunCoreGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nSunCoreBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_fSpriteSize[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fSpriteBrightness[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nShadowStrength[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nLightShadowStrength[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nPoleShadowStrength[NUM_HOURS][NUM_WEATHERS];

    static inline uint16 m_fFarClip[NUM_HOURS][NUM_WEATHERS];
    static inline uint16 m_fFogStart[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fLightsOnGroundBrightness[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nLowCloudsRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nLowCloudsGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nLowCloudsBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_nFluffyCloudsBottomRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nFluffyCloudsBottomGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nFluffyCloudsBottomBlue[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_fWaterRed[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fWaterGreen[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fWaterBlue[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fWaterAlpha[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_fPostFx1Red[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fPostFx1Green[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fPostFx1Blue[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fPostFx1Alpha[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_fPostFx2Red[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fPostFx2Green[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fPostFx2Blue[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_fPostFx2Alpha[NUM_HOURS][NUM_WEATHERS];

    static inline uint8 m_fCloudAlpha[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nHighLightMinIntensity[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nWaterFogAlpha[NUM_HOURS][NUM_WEATHERS];
    static inline uint8 m_nDirectionalMult[NUM_HOURS][NUM_WEATHERS];

    static inline float m_BrightnessAddedToAmbientRed;
    static inline float m_BrightnessAddedToAmbientGreen;
    static inline float m_BrightnessAddedToAmbientBlue;

    static inline float m_DirectionalLightColourFromDayRed;
    static inline float m_DirectionalLightColourFromDayGreen;
    static inline float m_DirectionalLightColourFromDayBlue;

public:
    static void InjectHooks();

    static int32 GetShadowStrength(void) { return m_CurrentColours.m_nShadowStrength; }

    static auto GetVectorToSun() { return m_VectorToSun[m_CurrentStoredValue]; }
    static auto GetShadowSide() { return CVector2D{ m_fShadowSideX[m_CurrentStoredValue], m_fShadowSideY[m_CurrentStoredValue] }; }
    static bool ShouldIgnoreSky() {
        return (
                m_nSkyTopRed[m_ExtraColour][m_ExtraColourWeatherType]   == 0 &&
                m_nSkyTopGreen[m_ExtraColour][m_ExtraColourWeatherType] == 0 &&
                m_nSkyTopBlue[m_ExtraColour][m_ExtraColourWeatherType]  == 0
        );
    }

    static float GetAmbientRed()                    { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientRed; }
    static float GetAmbientGreen()                  { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientGreen; }
    static float GetAmbientBlue()                   { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBlue; }
    static float GetAmbientRed_BeforeBrightness()   { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessRed; }
    static float GetAmbientGreen_BeforeBrightness() { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessGreen; }
    static float GetAmbientBlue_BeforeBrightness()  { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessBlue; }
    static float GetAmbientRed_Obj()                { return m_CurrentColours.m_fAmbientRed_Obj; }
    static float GetAmbientGreen_Obj()              { return m_CurrentColours.m_fAmbientGreen_Obj; }
    static float GetAmbientBlue_Obj()               { return m_CurrentColours.m_fAmbientBlue_Obj; }

    static float SumOfCurrentRGB1() { return m_CurrentColours.m_fPostFx1Blue + m_CurrentColours.m_fPostFx1Green + m_CurrentColours.m_fPostFx1Red; }
    static float SumOfCurrentRGB2() { return m_CurrentColours.m_fPostFx2Blue + m_CurrentColours.m_fPostFx2Green + m_CurrentColours.m_fPostFx2Red; }
};


#endif //RUSSIA_TIMECYCLE_H
