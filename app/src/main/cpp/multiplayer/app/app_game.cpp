//
// Created on 23.09.2023.
//

#include "app_game.h"
#include "../game/Birds.h"
#include "../game/Skidmarks.h"
#include "../main.h"
#include "../game/Coronas.h"
#include "../game/Clouds.h"
#include "../game/Glass.h"
#include "../game/MovingThings.h"
#include "../game/VisibilityPlugins.h"
#include "../game/WaterLevel.h"
#include "../game/WaterCannons.h"
#include "../game/WeaponEffects.h"
#include "../game/SpecialFX.h"
#include "../game/PointLights.h"
#include "../game/PostEffects.h"
#include "game/Camera.h"
#include "util/patch.h"
#include "keyboard.h"
#include "HUD.h"
#include "game/Widgets/TouchInterface.h"
#include "game/CrossHair.h"
#include "tools/DebugModules.h"
#include "Mirrors.h"
#include "Mobile/MobileSettings/MobileSettings.h"
#include "Weather.h"
#include "Renderer.h"

void RenderScene()
{
    const bool underWater = CWeather::UnderWaterness <= 0.0f;

    // Установка состояний рендеринга
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    if (!CMirrors::TypeOfMirror) {
        CMovingThings::Render_BeforeClouds();
        CClouds::Render();
    }

    // Восстановление состояний рендеринга
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEGOURAUD));

    if (CMirrors::TypeOfMirror != MIRROR_TYPE_SPHERE_MAP)
        CHook::CallFunction<void>("_ZN14CCarFXRenderer15PreRenderUpdateEv");

    CRenderer::RenderRoads();

    if (CMirrors::TypeOfMirror != MIRROR_TYPE_SPHERE_MAP)
        CCoronas::RenderReflections();

    CRenderer::RenderEverythingBarRoads();

    if (CMirrors::TypeOfMirror != MIRROR_TYPE_SPHERE_MAP) {
        CRenderer::RenderFadingInUnderwaterEntities();
        if (underWater) {
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
            CWaterLevel::RenderWater();
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
        }
    }

    if (CMirrors::TypeOfMirror != MIRROR_TYPE_SPHERE_MAP || CMobileSettings::ms_MobileSettings[MS_CarReflections].value == 3)
        CRenderer::RenderFadingInEntities();

    //BreakManager_c::Render(&g_breakMan, 0);

    /*if (!CMirrors::bRenderingReflection) {
        float nearClipPlaneOld = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
        float farPlane = RwCameraGetFarClipPlane(Scene.m_pRwCamera);

        float v3;
        float z = CCamera::GetActiveCamera().Front.z;
        if (z <= 0.0f)
            v3 = -z;
        else
            v3 = 0.0f;

        constexpr float flt_8CD4F0 = 2.0f;
        constexpr float flt_8CD4EC = 5.9604645e-8f; // Same as 1 / (float)(1 << 24) => 1 / 16777216.f ; Not sure if that means anything..

        float unknown = ((flt_8CD4F0 * flt_8CD4EC * 0.25f - flt_8CD4F0 * flt_8CD4EC) * v3 + flt_8CD4F0 * flt_8CD4EC) * (farPlane - nearClipPlaneOld);

        RwCameraEndUpdate(Scene.m_pRwCamera);

        RwCameraSetNearClipPlane(Scene.m_pRwCamera, unknown + nearClipPlaneOld);
        RwCameraBeginUpdate(Scene.m_pRwCamera);
        CShadows::UpdateStaticShadows();
        CShadows::RenderStaticShadows();
        CShadows::RenderStoredShadows();
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClipPlaneOld);

        RwCameraBeginUpdate(Scene.m_pRwCamera);
    }*/

    //BreakManager_c::Render(&g_breakMan, 1);

    if (CMirrors::TypeOfMirror != MIRROR_TYPE_SPHERE_MAP)
        CHook::CallFunction<void>("_ZN9CPlantMgr6RenderEv");

    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    if (!CMirrors::TypeOfMirror) {
        CClouds::RenderBottomFromHeight();
        CWeather::RenderRainStreaks();

        if (CMobileSettings::ms_MobileSettings[MS_Visuals].value == 3)
            CCoronas::RenderSunReflection();
    }

    if (!underWater && CMirrors::TypeOfMirror != MIRROR_TYPE_SPHERE_MAP) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        CWaterLevel::RenderWater();
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    }
}

void RenderEffects() {
//	RenderEffects();
    CBirds::Render();
    CSkidmarks::Render();
//    CRopes::Render();
//    CGlass::Render();
    CMovingThings::Render();
    CVisibilityPlugins::RenderReallyDrawLastObjects();
    CCoronas::Render();

    // FIXME
    auto g_fx = *(uintptr_t *) (g_libGTASA + (VER_x32 ? 0x00820520 : 0xA062A8));
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x00363DF0 + 1 : 0x433F54), &g_fx, CCamera::Get().m_pRwCamera, false);

    CWaterCannons::Render();
    CWaterLevel::RenderWaterFog();
    CClouds::MovingFogRender();
 //   CClouds::VolumetricCloudsRender();
////    if (CHeli::NumberOfSearchLights || CTheScripts::NumberOfScriptSearchLights) {
////        CHeli::Pre_SearchLightCone();
////        CHeli::RenderAllHeliSearchLights();
////        CTheScripts::RenderAllSearchLights();
////        CHeli::Post_SearchLightCone();
////    }
    CWeaponEffects::Render();
////    if (CReplay::Mode != MODE_PLAYBACK && !CPad::GetPad(0)->DisablePlayerControls) {
////        FindPlayerPed()->DrawTriangleForMouseRecruitPed();
////    }
    CSpecialFX::Render();
//    //CVehicleRecording::Render();
    CPointLights::RenderFogEffect();
//    //CRenderer::RenderFirstPersonVehicle();
    CPostEffects::MobileRender();

    DebugModules::Render3D();
}

void Render2dStuff()
{
    if( CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x001BB7F4 + 1 : 0x24EA90)) ) // emu_IsAltRenderTarget()
        CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x001BC20C + 1 : 0x24F5B8)); // emu_FlushAltRenderTarget()

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));

    CGUI::Render();

    CCrossHair::Render();

    if (CHUD::bIsShow) {

        // radar
        auto radar = CTouchInterface::m_pWidgets[WIDGET_RADAR];

        if (radar)
        {

            radar->m_fOriginX = CHUD::radarPos.x;
            radar->m_fOriginY = CHUD::radarPos.y;

            radar->m_fScaleX = CHUD::radarSize.x;
            radar->m_fScaleY = CHUD::radarSize.y;
        }

        ((void (*)()) (g_libGTASA + (VER_x32 ? 0x00437B0C + 1 : 0x51CFF0)))(); // CHud::DrawRadar

        if(!CKeyBoard::m_bEnable)
            ( ( void(*)(bool) )(g_libGTASA + (VER_x32 ? 0x002B0BD8 + 1 : 0x36FB00)) )(false); // CTouchInterface::DrawAll
    }

    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x1C0750 + 1 : 0x252CE4), 1); // textdraw text
    ((void (*)(bool)) (g_libGTASA + (VER_x32 ? 0x0054BDD4 + 1 : 0x66B678)))(1u); // CMessages::Display - gametext
    ((void (*)(bool)) (g_libGTASA + (VER_x32 ? 0x005A9120 + 1 : 0x6CCEA0)))(1u); // CFont::RenderFontBuffer
}