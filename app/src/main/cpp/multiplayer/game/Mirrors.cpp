//
// Created by traw-GG on 14.07.2025.
//
#include <GLES3/gl32.h>
#include "Mirrors.h"
#include "game.h"
#include "net/netgame.h"
#include "Camera.h"
#include "Scene.h"
#include "TimeCycle.h"
#include "app/app.h"
#include "World.h"
#include "Mobile/MobileSettings/MobileSettings.h"
#include "VisibilityPlugins.h"
#include "graphics/RQShader.h"
#include "app/app_game.h"

void emu_SetRenderingSphere(float* direction, GLboolean with) {
    if (with) {
        RQShader::curShaderStateFlags |= FLAG_SPHERE_XFORM;
    } else {
        RQShader::curShaderStateFlags &= ~FLAG_SPHERE_XFORM;
    }
}

void CMirrors::CreateBuffer()
{
    if (!pBuffer) {
        const RwInt32 depth = RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera));
        const RwInt32 width = 512;
        const RwInt32 height = 256;

        pBuffer = RwRasterCreate(width, height, depth, rwRASTERTYPECAMERATEXTURE);
        if (pBuffer && !(pZBuffer = RwRasterCreate(width, height, depth, rwRASTERTYPEZBUFFER))) {
            RwRasterDestroy(pBuffer);
            pBuffer = nullptr;
        }
    }

    const int reflectionQuality = CMobileSettings::ms_MobileSettings[MS_CarReflections].value;
    const RwInt32 newReflectionSize =
            (reflectionQuality == 3) ? 256 :
            (reflectionQuality == 2) ? 128 :
            0;

    if (reflBuffer[0]) {
        const bool needResize = (newReflectionSize > 0) &&
                                (reflBuffer[0]->height != newReflectionSize);

        if (newReflectionSize == 0 || needResize) {
            RwRasterDestroy(reflBuffer[0]);
            RwRasterDestroy(reflBuffer[1]);
            reflBuffer[0] = nullptr;
            reflBuffer[1] = nullptr;
        }
    }

    // Создание с проверкой ошибок
    if (newReflectionSize > 0 && !reflBuffer[0]) {
        const RwInt32 depth = RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera));
        reflBuffer[0] = RwRasterCreate(newReflectionSize, newReflectionSize,
                                       depth, rwRASTERTYPECAMERATEXTURE);
        reflBuffer[1] = reflBuffer[0] ?
                        RwRasterCreate(newReflectionSize, newReflectionSize, depth, rwRASTERTYPEZBUFFER) :
                        nullptr;

        if (!reflBuffer[1]) {
            if (reflBuffer[0]) RwRasterDestroy(reflBuffer[0]);
            reflBuffer[0] = nullptr;
        }
    }
}

void CMirrors::RenderReflections()
{
    auto carReflections = CMobileSettings::ms_MobileSettings[MS_CarReflections].value;
    if (carReflections < 2)
        return;

    CHook::CallFunction<void>("_ZN8CMirrors12CreateBufferEv");
    if (!FindPlayerPed(-1) || !reflBuffer[0]) {
        return;
    }

    static float reflectionRadius = 0.0f;
    static bool radiusInitialized = false;

    if (!radiusInitialized) {
        reflectionRadius = 60.0f;
        radiusInitialized = true;
    }

    
    CCamera::Get().m_sphereMapRadius = reflectionRadius * reflectionRadius;

    auto originalMirrorType = TypeOfMirror;
    TypeOfMirror = MIRROR_TYPE_SPHERE_MAP;

    RwRaster* originalFrameBuffer = Scene.m_pRwCamera->frameBuffer;
    RwRaster* originalZBuffer = Scene.m_pRwCamera->zBuffer;
    const float originalFarPlane = Scene.m_pRwCamera->farPlane;
    const float originalFogPlane = Scene.m_pRwCamera->fogPlane;

    memcpy(&Scene.m_pRwCamera->frameBuffer, reflBuffer, sizeof(RwRaster*)*2);
    emu_SetRenderingSphere(nullptr, 1);

    RwRGBA skyColor(
            std::max<uint16_t>(CTimeCycle::m_CurrentColours.m_nSkyTopRed, 64u),
            std::max<uint16_t>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen, 64u),
            std::max<uint16_t>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 64u),
            255
    );

    RwCameraClear(Scene.m_pRwCamera, &skyColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

    if (CHook::CallFunction<RwBool>("_Z19RsCameraBeginUpdateP8RwCamera", Scene.m_pRwCamera))
    {
        Scene.m_pRwCamera->farPlane = reflectionRadius;
        Scene.m_pRwCamera->fogPlane = reflectionRadius * 0.75f;

        bRenderingReflection = true;
        DefinedState();

        CHook::CallFunction<void>("_ZN9CRenderer23ConstructReflectionListEv");
        RenderScene();

        bRenderingReflection = false;
        RwCameraEndUpdate(Scene.m_pRwCamera);
    }

    emu_SetRenderingSphere(nullptr, 0);
    Scene.m_pRwCamera->frameBuffer = originalFrameBuffer;
    Scene.m_pRwCamera->zBuffer = originalZBuffer;
    TypeOfMirror = originalMirrorType;

    CCamera::Get().m_sphereMapRadius = 0.0f;
    Scene.m_pRwCamera->farPlane = originalFarPlane;
    Scene.m_pRwCamera->fogPlane = originalFogPlane;
}

void CMirrors::BeforeMainRender() {
    if (TypeOfMirror == MIRROR_TYPE_NONE)
        return;

    RwRaster* prevCamRaster  = RwCameraGetRaster(Scene.m_pRwCamera);
    RwRaster* prevCamZRaster = RwCameraGetZRaster(Scene.m_pRwCamera);

    RwCameraSetRaster(Scene.m_pRwCamera, pBuffer);
    RwCameraSetZRaster(Scene.m_pRwCamera, pZBuffer);

    
    CCamera::Get().SetCameraUpForMirror();

    RwRGBA color{ 0, 0, 0, 255 };
    RwCameraClear(Scene.m_pRwCamera, &color, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

    Scene.m_pRwCamera->viewWindow.y = -Scene.m_pRwCamera->viewWindow.y;

    if (RsCameraBeginUpdate(Scene.m_pRwCamera)) {
        bRenderingReflection = true;

        DefinedState();
        RenderScene();
        CVisibilityPlugins::RenderWeaponPedsForPC();
        CVisibilityPlugins::ms_weaponPedsForPC.Clear();

        bRenderingReflection = false;
        RwCameraEndUpdate(Scene.m_pRwCamera);

        RwCameraSetRaster(Scene.m_pRwCamera, prevCamRaster);
        RwCameraSetZRaster(Scene.m_pRwCamera, prevCamZRaster);

        CCamera::Get().RestoreCameraAfterMirror();
    }
}

void CMirrors::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679504 : 0x850A28), &reflBuffer);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678B40 : 0x84F6B0), &pBuffer);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6784DC : 0x84E9E0), &pZBuffer);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677EB8 : 0x84DDA0), &TypeOfMirror);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6763E8 : 0x84A848), &bRenderingReflection);

    CHook::Redirect("_ZN8CMirrors12CreateBufferEv", &CreateBuffer);
    CHook::Redirect("_ZN8CMirrors17RenderReflectionsEv", &RenderReflections);
    CHook::Redirect("_ZN8CMirrors16BeforeMainRenderEv", &BeforeMainRender);
}