#include "../main.h"
#include "game.h"
#include "CFirstPersonCamera.h"
#include "..//chatwindow.h"
#include "game/Entity/Ped/Ped.h"
#include "..//net/netgame.h"
#include "Scene.h"

bool CFirstPersonCamera::m_bEnabled = false;
float CFirstPersonCamera::g_fTonerLerp = 1.0f;

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

void CFirstPersonCamera::ProcessCameraOnFoot(CCam* pCam, CPedSamp* pPed)
{
    if (!m_bEnabled)
    {
        return;
    }

    CVector* pVec = &pCam->Source;

    CVector vecOffset;
    vecOffset.x = 0.35f;
    vecOffset.y = 0.1f;
    vecOffset.z = 0.1f;


    CVector vecOut;
    ProjectMatrix(&vecOut, (CMatrix*)&pPed->m_HeadBoneMatrix, &vecOffset);

    if (vecOut.x != vecOut.x || vecOut.y != vecOut.y || vecOut.z != vecOut.z)
    {
        pPed->m_pPed->GetBonePosition(&vecOut, BONE_NECK, false);
    }
    if (vecOut.x != vecOut.x || vecOut.y != vecOut.y || vecOut.z != vecOut.z)
    {
        return;
    }

    *pVec = vecOut;
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.2f);
    MakePlayerFaceCameraDirection(pCam, pPed);
}

void CFirstPersonCamera::ProcessCameraInVeh(CCam* pCam, CPedSamp* pPed, CVehicleSamp* pVeh)
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
    m_bEnabled = bEnabled;
}

void CFirstPersonCamera::Toggle()
{
    m_bEnabled ^= 1;
}

bool CFirstPersonCamera::IsEnabled()
{
    return m_bEnabled;
}
