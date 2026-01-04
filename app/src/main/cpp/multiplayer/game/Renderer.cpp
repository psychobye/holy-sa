#include "Renderer.h"
#include "../util/patch.h"
#include "app/app_light.h"
#include "VisibilityPlugins.h"
#include "Camera.h"
#include "World.h"
#include "CWorldScan.h"
#include "Streaming.h"

void CRenderer::ScanWorld() {
    static CVector oldCameraPosn;
    static CVector oldCameraView;

    CCamera& cam = CCamera::Get();
    RwCamera* m_pRwCamera = cam.m_pRwCamera;
    if (!m_pRwCamera) return;

    float farPlane = m_pRwCamera->farPlane;

    RwV3d points[13];
    points[0] = { 0.0f, 0.0f, 0.0f };

    float viewX = m_pRwCamera->viewWindow.x;
    float viewY = m_pRwCamera->viewWindow.y;
    float farX = farPlane * viewX;
    float farY = farPlane * viewY;

    points[1] = { -farX,  farY, farPlane };
    points[2] = {  farX,  farY, farPlane };
    points[3] = {  farX, -farY, farPlane };
    points[4] = { -farX, -farY, farPlane };

    float lodScale = 300.0f / farPlane;
    for (int i = 1; i <= 4; ++i) {
        points[i + 4] = { points[i].x * lodScale, points[i].y * lodScale, points[i].z * lodScale };
    }

    for (int i = 5; i <= 8; ++i) {
        points[i + 4] = { points[i].x * 0.2f, points[i].y * 0.2f, points[i].z * 0.2f };
    }

    // CRenderer::m_pFirstPersonVehicle = nullptr;
    CVisibilityPlugins::InitAlphaEntityList(); // crash here? need to reverse

    if (CWorld::ms_nCurrentScanCode == 0xFFFF) {
        CWorld::ClearScanCodes();
        CWorld::ms_nCurrentScanCode = 1;
    } else {
        CWorld::ms_nCurrentScanCode++;
    }

    CVector* p_pos;
    if (&cam.m_mCameraMatrix) {
        p_pos = &cam.m_mCameraMatrix.GetPosition();
    } else {
        p_pos = &cam.m_placement.m_vPosn;
    }
    oldCameraPosn = *p_pos;

    auto* camFrame = static_cast<RwFrame*>(m_pRwCamera->object.object.parent);
    RwMatrix* camMat = RwFrameGetLTM(camFrame);
    RwV3dTransformPoints(points, points, 13, camMat);

    RwV2d sectorPoints[5];
    for (int i = 0; i < 5; ++i) {
        sectorPoints[i].x = (points[i].x / 50.0f) + 60.0f;
        sectorPoints[i].y = (points[i].y / 50.0f) + 60.0f;
    }

    CRenderer::m_loadingPriority = false;
    CWorldScan::ScanWorld(sectorPoints, 5, CRenderer::ScanSectorList);

    RwV2d lodPoints[5];
    lodPoints[0].x = (points[0].x / 200.0f) + 15.0f;
    lodPoints[0].y = (points[0].y / 200.0f) + 15.0f;
    for (int i = 1; i < 5; ++i) {
        lodPoints[i].x = (points[4 + i].x / 200.0f) + 15.0f;
        lodPoints[i].y = (points[4 + i].y / 200.0f) + 15.0f;
    }

    CWorldScan::ScanWorld(lodPoints, 5, CRenderer::ScanBigBuildingList);
}

void CRenderer::RenderFadingInEntities() {
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));
    DeActivateDirectional();
    SetAmbientColours();
    CVisibilityPlugins::RenderFadingEntities();
}

void CRenderer::RenderFadingInUnderwaterEntities() {
    CHook::CallFunction<void>("_ZN9CRenderer32RenderFadingInUnderwaterEntitiesEv");
}

void CRenderer::RenderRoads() {
    CHook::CallFunction<void>("_ZN9CRenderer11RenderRoadsEv");
}

void CRenderer::RenderEverythingBarRoads() {
    CHook::CallFunction<void>("_ZN9CRenderer24RenderEverythingBarRoadsEv");
}

void CRenderer::ScanSectorList(int32 BlockX, int32 BlockY) {
    CHook::CallFunction<void>("_ZN9CRenderer14ScanSectorListEii", BlockX, BlockY);
}

void CRenderer::ScanBigBuildingList(int32 BlockX, int32 BlockY) {
    CHook::CallFunction<void>("_ZN9CRenderer19ScanBigBuildingListEii", BlockX, BlockY);
}

void CRenderer::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6764D0 : 0x84AA10), &ms_bRenderOutsideTunnels);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x67914C : 0x8502C8), &m_loadingPriority);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x6778EC : 0x84D210), &ms_aVisibleEntityPtrs);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6771F0 : 0x84C428), &ms_nNoOfVisibleEntities);

    // CHook::Redirect("_ZN9CRenderer9ScanWorldEv", &ScanWorld); // WARNING: FPS drop
}