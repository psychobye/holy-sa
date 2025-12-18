//
// Created on 26.07.2023.
//

#include "Camera.h"
#include "util/patch.h"
#include "scripting.h"
#include "Scene.h"
#include "Timer.h"
#include "World.h"
#include "Models/ModelInfo.h"

// externs
SCamColVars *gpCamColVars;

int CCamera::gCurCamColVars = 5;
float CCamera::gRadiusScalarForLengthToVehicle = 0.2939f;
float CCamera::gCurDistForCam = 1.0f;
float CCamera::gLastRadiusUsedInCollisionPreventionOfCamera = 2.0f;

CVector CCamera::gCamPosCached = {0.0f, 0.0f, 0.0f};
bool CCamera::gCamPosCachedInit = false;

CCam& CCamera::GetActiveCamera() {
    return CCamera::Get().m_aCams[CCamera::Get().m_nActiveCam];
}

void CCamera::Init() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0046F8C0 + 1 : 0x55BA30), this);
}

void CCamera::SetRwCamera(RwCamera *pCamera) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x003E161C + 1 : 0x4BF318), this, pCamera);
}

void CCamera::TakeControl(CEntity *target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x003E1714 + 1 : 0x4BF474), this, target, modeToGoTo, switchType, whoIsInControlOfTheCamera);
}

float CCamera::CalculateGroundHeight(eGroundHeightType type) {
    return CHook::CallFunction<float>(g_libGTASA + (VER_x32 ? 0x3DC5C8 + 1 : 0x4BA958), this, type);
}

void CCamera::RestoreWithJumpCut() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x3DB154 + 1 : 0x4B94B4), this);
}

void CCamera::SetBehindPlayer()
{
    ScriptCommand(&lock_camera_position, 0);
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&set_camera_behind_player);
    ScriptCommand(&restore_camera_jumpcut);
}

// 0.3.7
void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&set_camera_position, fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}

// 0.3.7
void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&point_camera, fX, fY, fZ, iType);
}

// 0.3.7
void CCamera::InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode)
{
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&lock_camera_position1, 1);
    ScriptCommand(&set_camera_pos_time_smooth, posFrom->x, posFrom->y, posFrom->z, posTo->x, posTo->y, posTo->z, time, mode);
}

// 0.3.7
void CCamera::InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode)
{
    ScriptCommand(&lock_camera_position, 1);
    ScriptCommand(&point_camera_transverse, posFrom->x, posFrom->y, posFrom->z, posTo->x, posTo->y, posTo->z, time, mode);
}

bool CCamera::IsSphereVisible(const CVector* origin, float radius) {
    return CHook::CallFunction<bool>("_ZN7CCamera15IsSphereVisibleERK7CVectorf", this, origin, radius);
}

void CCamera::SetCameraUpForMirror() {
    preMirrorMat = m_mCameraMatrix;
    m_mCameraMatrix = m_matMirror;
    CHook::CallFunction<void>("_ZN7CCamera23CopyCameraMatrixToRWCamEb", this, true);
    CHook::CallFunction<void>("_ZN7CCamera22CalculateDerivedValuesEbb", this, true, false);
}

void CCamera::RestoreCameraAfterMirror() {
    SetMatrix(preMirrorMat);
    CHook::CallFunction<void>("_ZN7CCamera23CopyCameraMatrixToRWCamEb", this, true);
    CHook::CallFunction<void>("_ZN7CCamera22CalculateDerivedValuesEbb", this, false, false);
}

bool CCamera::ConeCastCollisionResolve(CCamera *cam, CVector *pPos, CVector *pLookAt, CVector *pDest, float rad, float minDist, float *pDist) {
    return CHook::CallFunction<bool>("_ZN7CCamera24ConeCastCollisionResolveEP7CVectorS1_S1_ffPf", cam, pPos, pLookAt, pDest, rad, minDist, pDist);
}

bool CCamera::CameraColDetect(CCamera *cam, CVector *camPos, CVector *targetPos)
{
    if (!camPos || !targetPos) return false;

    CEntity* ent = CWorld::GetToIgnoreEntity();

    float dx = camPos->x - targetPos->x;
    float dy = camPos->y - targetPos->y;
    float dz = camPos->z - targetPos->z;
    float desiredDist = sqrtf(dx*dx + dy*dy + dz*dz);
    if (desiredDist < 1e-6f) desiredDist = 1e-6f;

    SCamColVars defaults = { 1.0f, 50.0f, 0.5f, 0.3f, 0.1f, 0.02f };
    SCamColVars *cv = gpCamColVars ? gpCamColVars : &defaults;

    float computedRad = cv->camRad * (gRadiusScalarForLengthToVehicle * desiredDist);
    float maxCamRad = cv->maxCamRad;

    if (ent) {
        if (ent->GetType() == ENTITY_TYPE_VEHICLE) {
            CColModel* cm = ent->GetColModel();
            if (cm) {
                float boxX = cm->m_boundBox.m_vecMax.x - cm->m_boundBox.m_vecMin.x;
                float boxY = cm->m_boundBox.m_vecMax.y - cm->m_boundBox.m_vecMin.y;
                float boxZ = cm->m_boundBox.m_vecMax.z - cm->m_boundBox.m_vecMin.z;

                float size2d = fmaxf(boxX, boxY);
                float distLowestZ = boxZ * 0.5f;
                float candidate = fmaxf(size2d - distLowestZ, 0.2f);

                maxCamRad = fminf(maxCamRad, candidate);
            }
        } else {
            CColModel* cm = ent->GetColModel();
            if (cm) {
                float boxX = cm->m_boundBox.m_vecMax.x - cm->m_boundBox.m_vecMin.x;
                float boxY = cm->m_boundBox.m_vecMax.y - cm->m_boundBox.m_vecMin.y;
                float boxZ = cm->m_boundBox.m_vecMax.z - cm->m_boundBox.m_vecMin.z;

                float minXY = fminf(boxX, boxY);
                float halfZ = boxZ * 0.5f;
                float candidate = minXY;
                if (halfZ < candidate) candidate = halfZ;
                maxCamRad = fminf(maxCamRad, candidate);
            }
        }
    }

    float usedRadius = fminf(maxCamRad, computedRad);
    usedRadius = fmaxf(usedRadius, 0.65f);
    gLastRadiusUsedInCollisionPreventionOfCamera = usedRadius;

    float camMinDist;
    if (gCurCamColVars > 9) {
        camMinDist = cv->camMinDist;
        if (ent && ent->GetType() == ENTITY_TYPE_VEHICLE) {
            camMinDist = fminf(camMinDist, 0.05f);
        }
    } else {
        // flt_74F478[...] / distance
        float dy_local = camPos->y - targetPos->y;
        float denom = sqrtf(dy_local*dy_local + (camPos->x - targetPos->x)*(camPos->x - targetPos->x) + (camPos->z - targetPos->z)*(camPos->z - targetPos->z));
        camMinDist = (denom > 1e-5f) ? (cv->camMinDist / denom) : cv->camMinDist;
    }

    CVector colPos{};
    float pDist = desiredDist;
    bool collided = CCamera::ConeCastCollisionResolve(cam, camPos, targetPos, &colPos, usedRadius, camMinDist, &pDist);

    // near clip tweak
    if (pDist <= cv->distToModClipping) {
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, cv->clippingDistance);
    }

    float pDistFrac = (pDist <= 1.01f) ? pDist : (pDist / desiredDist);
    pDistFrac = fmaxf(0.0f, fminf(pDistFrac, 1.0f));

    static CVector gCamPosCached = {0.0f, 0.0f, 0.0f};
    static bool gCamPosCachedInit = false;
    float v31;
    if (pDistFrac >= gCurDistForCam) {
        if (!gCamPosCachedInit) {
            gCamPosCached = *camPos;
            gCamPosCachedInit = true;
        }
        float dxC = camPos->x - gCamPosCached.x;
        float dyC = camPos->y - gCamPosCached.y;
        float dzC = camPos->z - gCamPosCached.z;
        float movedSqr = dxC*dxC + dyC*dyC + dzC*dzC;
        if (movedSqr <= 0.0001f) {
            v31 = gCurDistForCam;
        } else {
            float step = (cv->speedZoomOut * CTimer::ms_fTimeStep) * (pDistFrac - gCurDistForCam);
            if (step > 0.05f) step = 0.05f;
            v31 = gCurDistForCam + step;
            gCurDistForCam = v31;
        }
        gCamPosCached = *camPos;
    } else {
        gCurDistForCam = pDistFrac;
        v31 = pDistFrac;
    }

    if (v31 > 1.0f) { v31 = 1.0f; gCurDistForCam = 1.0f; }
    if (v31 < 0.0f) { v31 = 0.0f; gCurDistForCam = 0.0f; }

    camPos->x = targetPos->x + dx * v31;
    camPos->y = targetPos->y + dy * v31;
    camPos->z = targetPos->z + dz * v31;

    if (ent && ent->GetType() == ENTITY_TYPE_VEHICLE && gCurDistForCam < 0.5f) {
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);
    }

    return collided;
}

void CCamera::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678DD8 : 0x84FBE0), &preMirrorMat);

    // CHook::Redirect("_ZN7CCamera20CameraColDetAndReactEP7CVectorS1_", &CCamera::CameraColDetect); // idk for what im cryna :=(
}