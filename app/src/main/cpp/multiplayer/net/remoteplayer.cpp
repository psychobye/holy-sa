#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"
#include "../game/Entity/Ped/Ped.h"
#include "game/CarEnterExit.h"

CRemotePlayer::CRemotePlayer()
{
	m_PlayerID = INVALID_PLAYER_ID;
	m_VehicleID = INVALID_VEHICLE_ID;
	m_pPlayerPed = nullptr;
	m_pCurrentVehicle = nullptr;
	m_bIsNPC = false;
	m_bIsAFK = true;
	m_dwMarkerID = 0;
	m_dwGlobalMarkerID = 0;
	m_byteState = PLAYER_STATE_NONE;
	m_bShowNameTag = true;

	m_dwLastRecvTick = 0;

	m_byteSpecialAction = 0;
	m_byteSeatID = 0;
	m_byteWeaponShotID = 0xFF;

	m_usPlayingAnimIdx = 0;
	m_bWasSettedFlag = false;
}

CRemotePlayer::~CRemotePlayer()
{
	Remove();
}
extern bool g_uiHeadMoveEnabled;
void CRemotePlayer::ProcessSpecialActions(BYTE byteSpecialAction)
{
    // ����������
    if (m_pPlayerPed->IsCrouching() && byteSpecialAction != SPECIAL_ACTION_DUCK) {
        m_pPlayerPed->ResetCrouch();
    }
    if (byteSpecialAction == SPECIAL_ACTION_DUCK && !m_pPlayerPed->IsCrouching()) {
        m_pPlayerPed->ApplyCrouch();
    }
    ///

	m_pPlayerPed->ProcessSpecialAction(byteSpecialAction);

//	// headsync:always
	if (GetState() == PLAYER_STATE_ONFOOT)
	{
		//headmove?
		if ((GetTickCount() - m_dwLastHeadUpdate) > 500 && g_uiHeadMoveEnabled)
		{
			CVector LookAt;
			CAMERA_AIM* Aim = GameGetRemotePlayerAim(m_pPlayerPed->m_bytePlayerNumber);

            LookAt.x = Aim->pos1x + (Aim->f1x * 20.0f);
            LookAt.y = Aim->pos1y + (Aim->f1y * 20.0f);
            LookAt.z = Aim->pos1z + (Aim->f1z * 20.0f);

			ScriptCommand(&TASK_LOOK_AT_COORD, m_pPlayerPed->m_dwGTAId, LookAt.x, LookAt.y, LookAt.z, 3000);
			m_dwLastHeadUpdate = GetTickCount();
		}
	}
}

void CRemotePlayer::Process() {
	m_bIsAFK = false;
	if ((GetTickCount() - m_dwLastRecvTick) > 2000)
		m_bIsAFK = true;


	if (GetState() == PLAYER_STATE_ONFOOT && !m_pPlayerPed->m_pPed->IsInVehicle() && !m_bIsAFK) {
		if ((GetTickCount() - m_dwLastHeadUpdate) > 500 /*&& pSettings->GetReadOnly().szHeadMove*/) {
			CVector LookAt;
			CAMERA_AIM *Aim = GameGetRemotePlayerAim(m_pPlayerPed->m_bytePlayerNumber);

            LookAt.x = Aim->pos1x + (Aim->f1x * 20.0f);
            LookAt.y = Aim->pos1y + (Aim->f1y * 20.0f);
            LookAt.z = Aim->pos1z + (Aim->f1z * 20.0f);

			m_pPlayerPed->ApplyCommandTask("FollowPedSA", 0, 2000, -1, &LookAt, 0, 0.1f, 500, 3, 0);
			m_dwLastHeadUpdate = GetTickCount();
		}
		ProcessSpecialActions(m_ofSync.byteSpecialAction);
		SlerpRotation();
		//	HandleAnimations();

		m_pPlayerPed->SetArmedWeapon((int) m_ofSync.byteCurrentWeapon);
		if (m_byteWeaponShotID != 0xFF) {
			m_ofSync.byteCurrentWeapon = m_byteWeaponShotID;
			m_pPlayerPed->SetArmedWeapon((int) m_byteWeaponShotID);
			//ScriptCommand(&task_shoot_at_coord, m_pPlayerPed->m_dwGTAId, localMat.pos.x, localMat.pos.y, localMat.pos.z, 10);
			m_pPlayerPed->SetCurrentAim(CLocalPlayer::GetPlayerPed()->GetCurrentAim());
			m_pPlayerPed->SetKeys((uint16_t) 4, m_ofSync.lrAnalog, m_ofSync.udAnalog);

			//unknown weapon
			m_byteWeaponShotID = 0xFF;
		} else
			m_pPlayerPed->SetKeys(m_ofSync.wKeys, m_ofSync.lrAnalog, m_ofSync.udAnalog);

		if (m_ofSync.vecMoveSpeed == 0.0f) {
			m_pPlayerPed->m_pPed->SetVelocity(m_ofSync.vecMoveSpeed);
		}

		if (m_bIsAFK) {
			m_ofSync.lrAnalog = 0;
			m_ofSync.udAnalog = 0;

			m_pPlayerPed->m_pPed->ResetMoveSpeed();
			m_pPlayerPed->m_pPed->ResetTurnSpeed();
			m_pPlayerPed->m_pPed->Teleport(m_ofSync.vecPos, true);
		}
	}
	if (GetState() == PLAYER_STATE_DRIVER && m_pPlayerPed->m_pPed->IsInVehicle()) {
		if (!m_pCurrentVehicle) {
			return;
		}
		if (!GamePool_Vehicle_GetAt(m_pCurrentVehicle->m_dwGTAId)) {
			return;
		}
		UpdateVehicleRotation();

		if (m_icSync.vecMoveSpeed.x == 0.0f &&
			m_icSync.vecMoveSpeed.y == 0.0f &&
			m_icSync.vecMoveSpeed.z == 0.0f) {
			m_pCurrentVehicle->m_pVehicle->SetVelocity(m_icSync.vecMoveSpeed);
		}

		m_pPlayerPed->SetKeys(m_icSync.wKeys, m_icSync.lrAnalog, m_icSync.udAnalog);

	}

	if (m_byteState != PLAYER_STATE_WASTED)
		m_pPlayerPed->SetHealth(1000.0f);

}

void CRemotePlayer::SlerpRotation()
{
	RwMatrix mat;
	CQuaternion quatPlayer, quatResult;

	if(m_pPlayerPed)
	{
		m_pPlayerPed->m_pPed->GetMatrix(&mat);

		quatPlayer.SetFromMatrix(&mat);

		quatResult.Slerp(&m_ofSync.quat, &quatPlayer, 0.75f);
		quatResult.GetMatrix(&mat);
		m_pPlayerPed->m_pPed->SetMatrix((CMatrix&)mat);

		double fZ = atan2(-mat.up.x, mat.up.y) * 57.295776; /* rad to deg */
		if(fZ > 360.0f) fZ -= 360.0f;
		if(fZ < 0.0f) fZ += 360.0f;
		m_pPlayerPed->SetRotation((float)fZ);
	}
}


void CRemotePlayer::Remove()
{
	if(CPlayerPool::GetSpawnedPlayer(GetID())) {
		CPlayerPool::spawnedPlayers.erase(GetID());
	}
	if(m_dwMarkerID)
	{
		CGame::DisableMarker(m_dwMarkerID);
		m_dwMarkerID = 0;
	}

	if(m_dwGlobalMarkerID)
	{
		CGame::DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}

	if(m_pPlayerPed)
	{
		CGame::RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}
}

void CRemotePlayer::UpdateInCarMatrixAndSpeed(RwMatrix* mat, CVector* pos, CVector* speed)
{
	m_InCarQuaternion.SetFromMatrix(mat);

	m_vecInCarTargetPos = *pos;
	m_vecInCarTargetSpeed = *speed;

	m_pCurrentVehicle->m_pVehicle->SetVelocity(speed);
}

void CRemotePlayer::UpdateInCarTargetPosition()
{
    if(!m_pCurrentVehicle) return;

	RwMatrix matEnt;
	CVector vec;

	float delta;

	m_pCurrentVehicle->m_pVehicle->GetMatrix(&matEnt);

	if(m_pCurrentVehicle->m_pVehicle->IsAdded())
	{
		m_vecPosOffset.x = FloatOffset(m_vecInCarTargetPos.x, matEnt.pos.x);
		m_vecPosOffset.y = FloatOffset(m_vecInCarTargetPos.y, matEnt.pos.y);
		m_vecPosOffset.z = FloatOffset(m_vecInCarTargetPos.z, matEnt.pos.z);

		if(m_vecPosOffset.x > 0.05f || m_vecPosOffset.y > 0.05f || m_vecPosOffset.z > 0.05f)
		{
			delta = 0.5f;
			if( m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_HELI)
			{
				delta = 2.0f;
			}

			if(m_vecPosOffset.x > 8.0f || m_vecPosOffset.y > 8.0f || m_vecPosOffset.z > delta)
			{
				matEnt.pos = m_vecInCarTargetPos;

				m_pCurrentVehicle->m_pVehicle->SetMatrix((CMatrix&)matEnt);
				m_pCurrentVehicle->m_pVehicle->SetVelocity(m_vecInCarTargetSpeed);
			}
			else
			{
				vec = m_pCurrentVehicle->m_pVehicle->GetMoveSpeed();

				if(m_vecPosOffset.x > 0.05f)
					vec.x += (m_vecInCarTargetPos.x - matEnt.pos.x) * 0.06f;
				if(m_vecPosOffset.y > 0.05f)
					vec.y += (m_vecInCarTargetPos.y - matEnt.pos.y) * 0.06f;
				if(m_vecPosOffset.z > 0.05f)
					vec.z += (m_vecInCarTargetPos.z - matEnt.pos.z) * 0.06f;

				if( FloatOffset(vec.x, 0.0f) > 0.01f ||
					FloatOffset(vec.y, 0.0f) > 0.01f ||
					FloatOffset(vec.z, 0.0f) > 0.01f)
				{
					m_pCurrentVehicle->m_pVehicle->SetVelocity(vec);
				}
			}
		}
	}
	else
	{
		matEnt.pos = m_vecInCarTargetPos;

		m_pCurrentVehicle->m_pVehicle->SetMatrix((CMatrix&)matEnt);
	}
}

void CRemotePlayer::UpdateVehicleRotation() {
	if (!m_pCurrentVehicle)
		return;

	CQuaternion quat, qresult;
	RwMatrix matEnt;
	CVector vec = m_pCurrentVehicle->m_pVehicle->GetTurnSpeed();

    const float maxTurn = 0.02f;
	vec.x = std::clamp(vec.x, -maxTurn, maxTurn);
	vec.y = std::clamp(vec.y, -maxTurn, maxTurn);
	vec.z = std::clamp(vec.z, -maxTurn, 0.02f);

	m_pCurrentVehicle->m_pVehicle->SetTurnSpeed(vec);

	matEnt = m_pCurrentVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
	quat.SetFromMatrix(&matEnt);
	qresult.Slerp(&m_InCarQuaternion, &quat, 0.75f);
	qresult.Normalize();
	qresult.GetMatrix(&matEnt);
	m_pCurrentVehicle->m_pVehicle->SetMatrix((CMatrix &) matEnt);
}

bool CRemotePlayer::Spawn(uint8_t byteTeam, unsigned int iSkin, CVector *vecPos, float fRotation,
	uint32_t dwColor, uint8_t byteFightingStyle, eTags tag)
{
	if(m_pPlayerPed)
	{
		CGame::RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}

    Log("CRemotePlayer::Spawn skin: %d", iSkin);
	m_pPlayerPed = CGame::NewPlayer(iSkin, vecPos->x, vecPos->y, vecPos->z, fRotation);

	if(m_pPlayerPed && m_pPlayerPed->m_pPed)
	{
		SetPlayerColor(dwColor);

		if(m_dwMarkerID)
		{
			CGame::DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		if(pNetGame->m_iShowPlayerMarkers)
			m_pPlayerPed->ShowMarker(m_PlayerID);

		if(byteFightingStyle != 4)
			m_pPlayerPed->SetFightingStyle(byteFightingStyle);

		m_pPlayerPed->SetMoveAnim(byteTeam);

		SetState(PLAYER_STATE_SPAWNED);

        CPlayerPool::spawnedPlayers[GetID()] = this;

        m_nTag = tag;
		return true;
	}

	SetState(PLAYER_STATE_NONE);
	return false;
}

void CRemotePlayer::EnterVehicle(VEHICLEID VehicleID, bool bPassenger)
{
	if( m_pPlayerPed && CVehiclePool::GetAt(VehicleID) &&
		!m_pPlayerPed->m_pPed->IsInVehicle())
	{
		int iGtaVehicleID = CVehiclePool::FindGtaIDFromID(VehicleID);
		if(iGtaVehicleID && iGtaVehicleID != INVALID_VEHICLE_ID)
		{
			m_pPlayerPed->SetKeys(0, 0, 0);
			m_pPlayerPed->EnterVehicle(iGtaVehicleID, bPassenger);
		}
	}
}

void CRemotePlayer::ExitVehicle()
{
	if(m_pPlayerPed && m_pPlayerPed->m_pPed->IsInVehicle())
	{
		m_pPlayerPed->SetKeys(0, 0, 0);
		m_pPlayerPed->ExitCurrentVehicle();
	}
}

void CRemotePlayer::StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync)
{
	VEHICLEID TrailerID = trSync->trailerID;
	if (!TrailerID) return;

	CVehicleMP* pVehicle = CVehiclePool::GetAt(TrailerID);

	if (pVehicle)
	{
		RwMatrix matWorld;

		trSync->quat.GetMatrix(&matWorld);
		matWorld.pos = trSync->vecPos;

		pVehicle->m_pVehicle->SetMatrix((CMatrix&)matWorld);
		pVehicle->m_pVehicle->SetVelocity(trSync->vecMoveSpeed);
		pVehicle->m_pVehicle->SetTurnSpeed(trSync->vecTurnSpeed);
	}
}

void CRemotePlayer::UpdateOnFootTargetPosition()
{
    auto pos = m_pPlayerPed->m_pPed->GetPosition();
    m_vecPosOffset = m_ofSync.vecPos - pos;

    if (m_vecPosOffset.Magnitude() > 0.00001f)
    {
        if (m_vecPosOffset.x > 2.0f || m_vecPosOffset.y > 2.0f || m_vecPosOffset.z > 1.0f)
        {
            m_pPlayerPed->m_pPed->Teleport(m_ofSync.vecPos, false);
            return;
        }

        CVector speed = m_ofSync.vecMoveSpeed;
        speed += m_vecPosOffset * 0.1f;
        m_pPlayerPed->m_pPed->SetVelocity(speed);
    }
}
float m_fWeaponDamages[43 + 1]
= {
5.0f, /* Fist */
5.0f, /* Brass Knuckles */
5.0f, /* Golf Club */
5.0f, /* Nightstick */
5.0f, /* Knife */
5.0f, /* Baseball Bat */
5.0f, /* Shovel */
5.0f, /* Pool Cue */
5.0f, /* Katana */
5.0f, /* Chainsaw */
5.0f, /* Purple Dildo */
5.0f, /* Dildo */
5.0f, /* Vibrator */
5.0f, /* Silver Vibrator */
5.0f, /* Flowers */
5.0f, /* Cane */
75.0f, /* Grenade */
0.0f, /* Tear Gas */
75.0f, /* Molotov Cocktail */
0.0f, /* +EMPTY+ */
0.0f, /* +EMPTY+ */
0.0f, /* +EMPTY+ */
25.0f, /* 9mm (pistol) */
10.0f, /* Silenced 9mm */
45.0f, /* Desert Eagle */
40.0f, /* Shotgun */
10.0f, /* Sawnoff Shotgun */
10.0f, /* Combat Shotgun */
10.0f, /* Micro SMG/Uzi */
10.0f, /* MP5 */
10.0f, /* AK-47 */
10.0f, /* M4 */
20.0f, /* Tec-9 */
45.0f, /* Country Rifle */
45.0f, /* Sniper Rifle */
75.0f, /* RPG */
75.0f, /* HS Rocket */
5.0f, /* Flamethrower */
70.0f, /* Minigun */
75.0f, /* Satchel Charge */
0.0f, /* Detonator */
1.0f, /* Spraycan */
5.0f, /* Fire Extinguisher */
0.0f /* Camera */
};
void CRemotePlayer::StoreBulletSyncData(BULLET_SYNC* pBulletSync)
{
	auto btData = &m_pPlayerPed->m_bulletData;
	memset(btData, 0, sizeof(BULLET_DATA));

	btData->vecOrigin 	= pBulletSync->vecOrigin;
	btData->vecPos 		= pBulletSync->vecHit;

	if(pBulletSync->byteHitType == BULLET_HIT_TYPE_PLAYER) {
        if (pBulletSync->hitId == CPlayerPool::GetLocalPlayerID()) {
            btData->pEntity = CGame::FindPlayerPed()->m_pPed;
        } else {
            auto pRemotePlayer = CPlayerPool::GetSpawnedPlayer(pBulletSync->hitId);
            if(pRemotePlayer) {
                btData->pEntity = pRemotePlayer->GetPlayerPed()->m_pPed;
            }
        }
    }
    if(pBulletSync->byteHitType == BULLET_HIT_TYPE_VEHICLE) {
        CVehicleMP *pVehicle = CVehiclePool::GetAt(pBulletSync->hitId);
        if(pVehicle)
        {
            btData->pEntity = pVehicle->m_pVehicle;
        }
	}

	m_byteWeaponShotID = pBulletSync->byteWeaponID;
    m_pPlayerPed->m_bHaveBulletData = true;
	m_pPlayerPed->FireInstant();
}

void CRemotePlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(m_PlayerID, dwColor);
}

void CRemotePlayer::Say(unsigned char* szText)
{
	char * szPlayerName = CPlayerPool::GetPlayerName(m_PlayerID);
	CChatWindow::AddChatMessage(szPlayerName,GetPlayerColor(), (char*)szText);
}

void calculateAimVector(CVector* vec1, CVector* vec2)
{
	float f1;
	float f2;
	float f3;

	f1 = atan2(vec1->x, vec1->y) - 1.570796370506287;
	f2 = sin(f1);
	f3 = cos(f1);
	vec2->x = vec1->y * 0.0 - f3 * vec1->z;
	vec2->y = f2 * vec1->z - vec1->x * 0.0;
	vec2->z = f3 * vec1->x - f2 * vec1->y;
}

void CRemotePlayer::UpdateAimFromSyncData(AIM_SYNC_DATA * pAimSync)
{
    if(!m_pPlayerPed) return;
    m_pPlayerPed->SetCameraMode(pAimSync->byteCamMode);

    CAMERA_AIM Aim;

    Aim.f1x = pAimSync->vecAimf1[0];
    Aim.f1y = pAimSync->vecAimf1[1];
    Aim.f1z = pAimSync->vecAimf1[2];

    Aim.f2x = pAimSync->vecAimf1[0];
    Aim.f2y = pAimSync->vecAimf1[1];
    Aim.f2z = pAimSync->vecAimf1[2];

    Aim.pos1x = pAimSync->vecAimPos[0];
    Aim.pos1y = pAimSync->vecAimPos[1];
    Aim.pos1z = pAimSync->vecAimPos[2];

    Aim.pos2x = pAimSync->vecAimPos[0];
    Aim.pos2y = pAimSync->vecAimPos[1];
    Aim.pos2z = pAimSync->vecAimPos[2];

    m_pPlayerPed->SetCurrentAim(&Aim);
    m_pPlayerPed->SetAimZ(pAimSync->fAimZ);

    float fAspect = pAimSync->aspect_ratio * 0.0039215689f;
    // float fExtZoom = (float)(paimSync->byteCamExtZoom)/63.0f;
    // m_pPlayerPed->SetCameraExtendedZoom(fExtZoom, 0);

    CWeapon* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();

    if (pAimSync->byteWeaponState == 3)
        pwstWeapon->m_nState = static_cast<eWeaponState>(2);		// Reloading
    else
    if (pAimSync->byteWeaponState != 2)
        pwstWeapon->dwAmmoInClip = (uint32_t)pAimSync->byteWeaponState;
    else
    if (pwstWeapon->dwAmmoInClip < 2)
        pwstWeapon->dwAmmoInClip = 2;
}

void CRemotePlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync)
{
    m_dwLastRecvTick = GetTickCount();
    SetState(PLAYER_STATE_ONFOOT);

    memcpy(&m_ofSync, pofSync, sizeof(ONFOOT_SYNC_DATA));

	m_byteSpecialAction = pofSync->byteSpecialAction;

	m_fCurrentHealth = pofSync->byteHealth;
    m_fCurrentArmor = pofSync->byteArmour;

    if(m_pPlayerPed->m_pPed->IsInVehicle())
    {
        m_pPlayerPed->m_pPed->RemoveFromVehicleAndPutAt(pofSync->vecPos);
    }

    if (GetPlayerPed()->GetCurrentWeapon() != m_ofSync.byteCurrentWeapon) {
        GetPlayerPed()->m_pPed->GiveWeapon(m_ofSync.byteCurrentWeapon, 9999);
    }
    UpdateOnFootTargetPosition();
}
void CRemotePlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync)
{
	memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));
	m_VehicleID = picSync->VehicleID;

	m_pCurrentVehicle = CVehiclePool::GetAt(m_VehicleID);
	if (!m_pCurrentVehicle)return;

	if(!m_pPlayerPed->m_pPed->IsInVehicle())
	{
		ScriptCommand(&put_actor_in_car, m_pPlayerPed->m_dwGTAId, m_pCurrentVehicle->m_dwGTAId);
	}
	if (m_pPlayerPed->GetCurrentVehicle() != m_pCurrentVehicle) {
		m_pPlayerPed->m_pPed->RemoveFromVehicleAndPutAt(picSync->vecPos);
	}

	// -------------- TRAILER
	if(m_icSync.TrailerID  == INVALID_VEHICLE_ID || m_icSync.TrailerID  == 0 )
	{//
		if(m_pCurrentVehicle->m_pTrailer) {
			m_pCurrentVehicle->DetachTrailer();
		}
	}
	else {
		CVehicleMP *pTrailer = CVehiclePool::GetAt(m_icSync.TrailerID);
		if(pTrailer) {
			if (m_pCurrentVehicle->m_pTrailer) {
				if ((CVehicle*)(m_pCurrentVehicle->m_pVehicle->m_pTrailer) != pTrailer->m_pVehicle) {
					m_pCurrentVehicle->DetachTrailer();
				}
			} else {
				if (GamePool_Vehicle_GetAt(pTrailer->m_dwGTAId)) {
					m_pCurrentVehicle->SetTrailer(pTrailer);
					m_pCurrentVehicle->AttachTrailer();
				}
			}
		}
	}
	// -----------------------------------------------------------

	m_byteSeatID = 0;
	m_fCurrentHealth = picSync->bytePlayerHealth;
	m_fCurrentArmor = picSync->bytePlayerArmour;

	m_dwLastRecvTick = GetTickCount();

	m_byteSpecialAction = 0;
	m_pCurrentVehicle->SetHealth(picSync->fCarHealth);

	m_pCurrentVehicle->m_iTurnState = static_cast<eTurnState>(picSync->HydraThrustAngle);

	SetState(PLAYER_STATE_DRIVER);

    RwMatrix matVehicle;
    m_icSync.quat.Normalize();
    m_icSync.quat.GetMatrix(&matVehicle);
    matVehicle.pos = m_icSync.vecPos;

    UpdateInCarMatrixAndSpeed(&matVehicle, &m_icSync.vecPos, &m_icSync.vecMoveSpeed);
    UpdateInCarTargetPosition();
}

void CRemotePlayer::StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync)
{
	m_VehicleID = ppsSync->VehicleID;

	m_byteSeatID = ppsSync->byteSeatFlags;
	m_pCurrentVehicle = CVehiclePool::GetAt(m_VehicleID);

	if(!m_pCurrentVehicle)return;
	if (m_pPlayerPed->GetCurrentVehicle() != m_pCurrentVehicle) {
		m_pPlayerPed->m_pPed->RemoveFromVehicleAndPutAt(ppsSync->vecPos);
	}

	if(!m_pPlayerPed->m_pPed->IsInVehicle()){
		m_byteSeatID--;
		ScriptCommand(&put_actor_in_car2, m_pPlayerPed->m_dwGTAId, m_pCurrentVehicle->m_dwGTAId, m_byteSeatID);
//		m_byteSeatID = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(m_pCurrentVehicle->m_pVehicle, m_byteSeatID);
//		CCarEnterExit::SetPedInCarDirect(m_pPlayerPed->m_pPed, m_pCurrentVehicle->m_pVehicle, m_byteSeatID);
	}


	m_fCurrentHealth = ppsSync->bytePlayerHealth;
	m_fCurrentArmor = ppsSync->bytePlayerArmour;

	m_dwLastRecvTick = GetTickCount();

	SetState(PLAYER_STATE_PASSENGER);
}

void CRemotePlayer::HandleDeath()
{
	if (GetPlayerPed())
	{
		GetPlayerPed()->SetKeys(0, 0, 0);
		GetPlayerPed()->SetDead();
		m_fCurrentHealth = 0;
	}

	SetState(PLAYER_STATE_WASTED);

	memset(&m_ofSync, 0, sizeof(m_ofSync));
	memset(&m_icSync, 0, sizeof(m_icSync));

	HideGlobalMarker();
}

void CRemotePlayer::HandleAnimations()
{
	if (!m_pPlayerPed)
	{
		return;
	}

	int flag = m_ofSync.dwAnimation >> 31;
	//pChatWindow->AddMessage("FLAG REMOTE %d", flag);

	if (!flag)
	{
		uint16_t newAnim = (uint16_t)m_ofSync.dwAnimation;
		if (newAnim != m_usPlayingAnimIdx)
		{
			//pChatWindow->AddMessage("applying %hu anium", newAnim);
			m_usPlayingAnimIdx = newAnim;
			m_pPlayerPed->PlayAnimByIdx(newAnim, 4.0f);
		}
		if (m_usPlayingAnimIdx)
		{
			if (!m_pPlayerPed->IsPlayingAnim(m_usPlayingAnimIdx))
			{
				m_pPlayerPed->PlayAnimByIdx(newAnim, 4.0f);
			}
		}
		m_bWasSettedFlag = true;
	}
	if (flag && m_bWasSettedFlag)
	{
		m_usPlayingAnimIdx = 0;
		// we have to reset anim
		m_pPlayerPed->PlayAnimByIdx(0, 4.0f);
		m_bWasSettedFlag = false;
	}
}


uint32_t CRemotePlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(m_PlayerID);
}

void CRemotePlayer::ShowGlobalMarker(short sX, short sY, short sZ)
{
	if(m_dwGlobalMarkerID)
	{
		CGame::DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}

	if(!m_pPlayerPed)
	{
		m_dwGlobalMarkerID = CGame::CreateRadarMarkerIcon(0, (float)sX, (float)sY, (float)sZ, m_PlayerID, 0);
	}
}

void CRemotePlayer::HideGlobalMarker()
{
	if(m_dwGlobalMarkerID)
	{
		CGame::DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}
}


void CRemotePlayer::StateChange(BYTE byteNewState, BYTE byteOldState) const
{
	if(byteNewState == PLAYER_STATE_DRIVER && byteOldState == PLAYER_STATE_ONFOOT)
	{
		// If their new vehicle is the one the local player
		// is driving, we'll have to kick the local player out
		CPedSamp *pLocalPlayerPed = CGame::FindPlayerPed();
		VEHICLEID LocalVehicle=0xFFFF;

		if(pLocalPlayerPed && pLocalPlayerPed->m_pPed->IsInVehicle() && !pLocalPlayerPed->m_pPed->IsAPassenger())
		{
			LocalVehicle = CVehiclePool::FindIDFromGtaPtr(pLocalPlayerPed->GetGtaVehicle());
			if(LocalVehicle == m_VehicleID) {
				pLocalPlayerPed->m_pPed->RemoveFromVehicle();
				CGame::DisplayGameText("~r~Car Jacked~w~!",1000,5);
			}
		}
	}
}