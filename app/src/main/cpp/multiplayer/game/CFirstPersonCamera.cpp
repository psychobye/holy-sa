#include "../main.h"
#include "game.h"
#include "CFirstPersonCamera.h"
#include "..//chatwindow.h"
#include "game/Entity/Ped/Ped.h"
#include "..//net/netgame.h"
#include "Scene.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"

bool CFirstPersonCamera::m_bEnabled = false;
float CFirstPersonCamera::g_fTonerLerp = 1.0f;

#include "../main.h"
#include "game.h"
#include "CFirstPersonCamera.h"
#include "..//chatwindow.h"
#include "game/Entity/Ped/Ped.h"
#include "..//net/netgame.h"
#include "Scene.h"
#include "game/CPad.h"

void CFirstPersonCamera::MakePlayerFaceCameraDirection(CCam* pCam, CPedSamp* player) {
    CVector playerPos = player->m_pPed->GetPosition();

    CVector LookAt = {
            pCam->Source.x + (pCam->Front.x * 20.0f),
            pCam->Source.y + (pCam->Front.y * 20.0f),
            pCam->Source.z + (pCam->Front.z * 20.0f)
    };

    CVector direction = {
            LookAt.x - playerPos.x,
            LookAt.y - playerPos.y,
            0.0f
    };

    double fZ = atan2(-direction.x, direction.y) * 57.295776;
    if (fZ > 360.0f) fZ -= 360.0f;
    if (fZ < 0.0f) fZ += 360.0f;
    player->SetRotation((float)fZ);
}

void CFirstPersonCamera::UpdateFirstPersonMoveAnims(CCam* pCam, CPedSamp* pPedSamp)
{
    if (!pCam || !pPedSamp) return;
    CPed* pPed = pPedSamp->m_pPed;
    if (!pPed) return;
    RpClump* clump = pPed->m_pRwClump;
    if (!clump) return;
    CPad* pad = CPad::GetPad(0);
    if (!pad) return;

    auto clampf = [](float v, float lo, float hi) { return (v < lo) ? lo : (v > hi) ? hi : v; };

    short walkX = CPad::GetPedWalkLeftRight(pad);
    short walkY = CPad::GetPedWalkUpDown(pad);

    float moveX = clampf(((float)walkX / 128.0f), -1.0f, 1.0f);
    float moveY = clampf(((float)walkY / 128.0f), -1.0f, 1.0f);

    const float deadZone = 0.001f;
    float moveMag = sqrtf(moveX * moveX + moveY * moveY);

    if (moveMag < deadZone)
    {
        pPed->SetMoveState(PEDMOVE_NONE);
        pPed->m_fMoveAnim = 0.0f;
        pPed->m_vecAnimMovingShiftLocal = CVector2D(0.0f, 0.0f);

        CAnimManager::BlendAnimation(clump, ANIM_GROUP_DEFAULT, ANIM_ID_IDLE, 4.0f);

        for (AnimationId id : {ANIM_ID_WALK, ANIM_ID_RUN, ANIM_ID_FIGHTSH_BWD, ANIM_ID_FIGHTSH_LEFT, ANIM_ID_FIGHTSH_RIGHT}) {
            auto a = (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, id);
            if (a) a->SetBlendDelta(-8.0f);
        }

        pPed->UpdateRpHAnim();
        return;
    }

    if (moveMag > 1.0f) moveMag = 1.0f;
    float globalSpeed = sqrtf(moveMag);
    const float runThreshold = 0.6f;

    pPed->m_fCurrentRotation = pCam->m_fHorizontalAngle + 1.5707f;

    auto animWalk = (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, ANIM_ID_WALK);
    auto animRun  = (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, ANIM_ID_RUN);
    auto animBwd  = (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, ANIM_ID_FIGHTSH_BWD);
    auto animLeft = (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, ANIM_ID_FIGHTSH_LEFT);
    auto animRight= (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, ANIM_ID_FIGHTSH_RIGHT);

    if (moveY < -0.05f) {
        float fwdMag = fabsf(moveY);
        float runWeight = (moveMag > runThreshold) ? (moveMag - runThreshold) / (1.0f - runThreshold) : 0.0f;
        float walkWeight = 1.0f - runWeight;

        if (!animWalk) animWalk = CAnimManager::BlendAnimation(clump, ANIM_GROUP_DEFAULT, ANIM_ID_WALK, 8.0f);
        if (animWalk) {
            animWalk->SetBlendAmount(walkWeight * fwdMag);
            animWalk->SetSpeed(globalSpeed);
        }

        if (!animRun) animRun = CAnimManager::BlendAnimation(clump, ANIM_GROUP_DEFAULT, ANIM_ID_RUN, 8.0f);
        if (animRun) {
            animRun->SetBlendAmount(runWeight * fwdMag);
            animRun->SetSpeed(globalSpeed * 1.2f);
        }
        if (animBwd) animBwd->SetBlendAmount(0.0f);
    }
    else if (moveY > 0.05f) {
        if (!animBwd) animBwd = CAnimManager::BlendAnimation(clump, ANIM_GROUP_DEFAULT, ANIM_ID_FIGHTSH_BWD, 8.0f);
        if (animBwd) {
            animBwd->SetBlendAmount(fabsf(moveY));
            animBwd->SetSpeed(globalSpeed);
        }
        if (animWalk) animWalk->SetBlendAmount(0.0f);
        if (animRun) animRun->SetBlendAmount(0.0f);
    }

    auto handleStrafe = [&](CAnimBlendAssociation*& anim, AnimationId id, float val) {
        if (val > 0.05f) {
            if (!anim) anim = CAnimManager::BlendAnimation(clump, ANIM_GROUP_DEFAULT, id, 8.0f);
            if (anim) {
                anim->SetBlendAmount(val);
                anim->SetSpeed(globalSpeed);
            }
        } else if (anim) {
            anim->SetBlendAmount(0.0f);
        }
    };

    handleStrafe(animLeft, ANIM_ID_FIGHTSH_LEFT, moveX < 0 ? fabsf(moveX) : 0.0f);
    handleStrafe(animRight, ANIM_ID_FIGHTSH_RIGHT, moveX > 0 ? fabsf(moveX) : 0.0f);

    pPed->m_vecAnimMovingShiftLocal = CVector2D(0.0f, 0.0f);
    pPed->UpdateRpHAnim();
}

void CFirstPersonCamera::ResetPedAnims()
{
    CPedSamp* pPedSamp = CLocalPlayer::GetPlayerPed();
    if (!pPedSamp) return;
    CPed* pPed = pPedSamp->m_pPed;
    if (!pPed || !pPed->m_pRwClump) return;

    RpClump* clump = pPed->m_pRwClump;

    pPed->SetMoveState(PEDMOVE_NONE);
    pPed->m_fMoveAnim = 0.0f;
    pPed->m_vecAnimMovingShiftLocal = CVector2D(0.0f, 0.0f);

    CAnimManager::BlendAnimation(clump, ANIM_GROUP_DEFAULT, ANIM_ID_IDLE, 4.0f);

    AnimationId IDs[] = {
            ANIM_ID_WALK, ANIM_ID_RUN, ANIM_ID_FIGHTSH_BWD,
            ANIM_ID_FIGHTSH_LEFT, ANIM_ID_FIGHTSH_RIGHT
    };

    for (AnimationId id : IDs) {
        auto a = (CAnimBlendAssociation*)RpAnimBlendClumpGetAssociation(clump, id);
        if (a) {
            a->SetBlendDelta(-8.0f);
        }
    }
    pPed->UpdateRpHAnim();
}

void CFirstPersonCamera::ProcessCameraOnFoot(CCam* pCam, CPedSamp* pPed)
{
    if (!m_bEnabled) return;

    CVector* pVec = &pCam->Source;

    CVector vecOffset;
    vecOffset.x = 0.35f;
    vecOffset.y = 0.1f;
    vecOffset.z = 0.1f;

    CVector vecOut;
    ProjectMatrix(&vecOut, (CMatrix*)&pPed->m_HeadBoneMatrix, &vecOffset);

    if (vecOut.x != vecOut.x || vecOut.y != vecOut.y || vecOut.z != vecOut.z)
        pPed->m_pPed->GetBonePosition(&vecOut, BONE_NECK, false);
    if (vecOut.x != vecOut.x || vecOut.y != vecOut.y || vecOut.z != vecOut.z)
        return;

    *pVec = vecOut;
    UpdateFirstPersonMoveAnims(pCam, pPed);
    MakePlayerFaceCameraDirection(pCam, pPed);
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.2f);
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.2f);
}

void CFirstPersonCamera::ProcessCameraInVeh(CCam* pCam, CPedSamp* pPed, CVehicleMP* pVeh)
{
    if (!m_bEnabled || !pPed->GetGtaVehicle())
    {
        return;
    }

    CVector* pVec = &pCam->Source;

    CVector vecOffset;
    vecOffset.x = 0.0f;
    vecOffset.y = 0.0f;
    vecOffset.z = 0.6f;

    uint16_t modelIndex = pPed->GetGtaVehicle()->m_nModelIndex;

    if (modelIndex == 581 || modelIndex == 509 || modelIndex == 481 || modelIndex == 462 || modelIndex == 521 || modelIndex == 463 || modelIndex == 510 ||
        modelIndex == 522 || modelIndex == 461 || modelIndex == 468 || modelIndex == 448 || modelIndex == 586)
    {
        vecOffset.x = 0.05f;
        vecOffset.y = 0.3f;
        vecOffset.z = 0.45f;
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.3f);
    }
    else
    {
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.01f);
    }

    CVector vecOut;
    RwMatrix mat;

    memcpy(&mat, pPed->m_pPed->m_matrix, sizeof(RwMatrix));

    ProjectMatrix(&vecOut, (CMatrix*)&mat, &vecOffset);

    if (vecOut.x != vecOut.x || vecOut.y != vecOut.y || vecOut.z != vecOut.z)
    {
        pPed->m_pPed->GetBonePosition(&vecOut, BONE_NECK, false);
    }
    if (vecOut.x != vecOut.x || vecOut.y != vecOut.y || vecOut.z != vecOut.z)
    {
        return;
    }

    *pVec = vecOut;

    if (!pVeh) {
        if (!pPed->m_pPed->IsAPassenger()) {
            pCam->m_nMode = MODE_1STPERSON;
        }
        return;
    }
}

void CFirstPersonCamera::Update()
{
    // toner alpha lerp
    const float target = m_bEnabled ? 0.5f : 1.0f;
    if (m_bEnabled) {
        const float speed = 0.02f;
        g_fTonerLerp -= speed;
        if (g_fTonerLerp < target)
            g_fTonerLerp = target;
    }
    else {
        g_fTonerLerp = 1.0f;
    }
}

void CFirstPersonCamera::SetEnabled(bool bEnabled)
{
    if (m_bEnabled && !bEnabled) {
        ResetPedAnims();
    }
    m_bEnabled = bEnabled;
}

void CFirstPersonCamera::Toggle()
{
    if (m_bEnabled) {
        ResetPedAnims();
    }
    m_bEnabled ^= 1;
}

bool CFirstPersonCamera::IsEnabled()
{
    return m_bEnabled;
}
