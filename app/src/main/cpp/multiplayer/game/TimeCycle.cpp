//
// Created on 20.09.2023.
//

#include "TimeCycle.h"
#include "util/patch.h"

void CTimeCycle::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678B94 : 0x84F758), &m_bExtraColourOn);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679A9C : 0x851558), &m_ExtraColourInter);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678194 : 0x84E350), &m_ExtraColour);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678234 : 0x84E490), &m_ExtraColourWeatherType);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676BB8 : 0x84B7D0), &m_CurrentColours);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006770C0 : 0x84C1D0), &m_BelowHorizonGrey);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006778B8 : 0x84D1A8), &m_FogReduction);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x006793E4 : 0x8507F0), &m_VectorToSun);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677F84 : 0x84DF38), &m_fShadowDisplacementX);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679B64 : 0x8516E8), &m_fShadowDisplacementY);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678E30 : 0x84FC90), &m_fShadowFrontX);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006772B4 : 0x84C5B0), &m_fShadowFrontY);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006794CC : 0x8509C0), &m_fShadowSideX);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679244 : 0x8504B0), &m_fShadowSideY);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676404 : 0x84A880), &m_CurrentStoredValue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679DF0 : 0x851BF8), &m_aBoxes);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679628 : 0x850C70), &m_NumBoxes);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677ED4 : 0x84DDD8), &m_nAmbientRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678E7C : 0x84FD28), &m_nAmbientGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676AF0 : 0x84B640), &m_nAmbientBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x006791A4 : 0x850378), &m_nAmbientRed_Obj);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00675FE8 : 0x84A058), &m_nAmbientGreen_Obj);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676960 : 0x84B328), &m_nAmbientBlue_Obj);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679E8C : 0x851D30), &m_nSkyTopRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679E34 : 0x851C80), &m_nSkyTopGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677D40 : 0x84DAB0), &m_nSkyTopBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677D20 : 0x84DA70), &m_nSkyBottomRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678F6C : 0x84FF08), &m_nSkyBottomGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006799A4 : 0x851368), &m_nSkyBottomBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678584 : 0x84EB30), &m_nSunCoreRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679010 : 0x850050), &m_nSunCoreGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067620C : 0x84A498), &m_nSunCoreBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067772C : 0x84CE90), &m_nSunCoronaRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006785EC : 0x84EC00), &m_nSunCoronaGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678510 : 0x84EA48), &m_nSunCoronaBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067973C : 0x850E90), &m_fSunSize);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676F44 : 0x84BEE0), &m_fSpriteSize);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679278 : 0x850518), &m_fSpriteBrightness);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676424 : 0x84A8C0), &m_nShadowStrength);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678290 : 0x84E548), &m_nLightShadowStrength);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006760EC : 0x84A258), &m_nPoleShadowStrength);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676194 : 0x84A3A8), &m_fFarClip);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679AD8 : 0x8515D0), &m_fFogStart);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679794 : 0x850F40), &m_fLightsOnGroundBrightness);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x006796B0 : 0x850D80), &m_nLowCloudsRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678A78 : 0x84F520), &m_nLowCloudsGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006784B0 : 0x84E988), &m_nLowCloudsBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679E4C : 0x851CB0), &m_nFluffyCloudsBottomRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678BD0 : 0x84F7D0), &m_nFluffyCloudsBottomGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067828C : 0x84E540), &m_nFluffyCloudsBottomBlue);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067930C : 0x850640), &m_fWaterRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006788F8 : 0x84F218), &m_fWaterGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676A8C : 0x84B578), &m_fWaterBlue);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006766B4 : 0x84ADD8), &m_fWaterAlpha);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676568 : 0x84AB40), &m_fPostFx1Red);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676738 : 0x84AEE0), &m_fPostFx1Green);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679418 : 0x850858), &m_fPostFx1Blue);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006763D4 : 0x84A820), &m_fPostFx1Alpha);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678D7C : 0x84FB28), &m_fPostFx2Red);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006769C8 : 0x84B3F0), &m_fPostFx2Green);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679888 : 0x851130), &m_fPostFx2Blue);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679BE8 : 0x8517E8), &m_fPostFx2Alpha);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676CEC : 0x84BA38), &m_fCloudAlpha);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067618C : 0x84A398), &m_nHighLightMinIntensity);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678F70 : 0x84FF10), &m_nWaterFogAlpha);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678CE0 : 0x84F9F0), &m_nDirectionalMult);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678390 : 0x84E748), &m_BrightnessAddedToAmbientRed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677468 : 0x84C918), &m_BrightnessAddedToAmbientGreen);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676EE8 : 0x84BE28), &m_BrightnessAddedToAmbientBlue);
}
