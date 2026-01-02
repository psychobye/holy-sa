#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

#include "../game/scripting.h"

#include "../game/common.h"
#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "../CSettings.h"
#include "../util/CJavaWrapper.h"
#include "java_systems/HUD.h"
#include "java_systems/Inventory.h"
#include "../game/Weapon.h"
#include "../game/Entity/Ped/Ped.h"
#include "../java_systems/SkinShop.h"
#include "../java_systems/Race.h"
#include "Monologue.h"
#include "../game/Camera.h"

extern int iNetModeNormalOnfootSendRate;
extern int iNetModeNormalInCarSendRate;
extern bool bUsedPlayerSlots[];

#define IS_TARGETING(x) ((x) & 128)
#define IS_FIRING(x) ((x) & 4)

#define NETMODE_HEADSYNC_SENDRATE		1000
#define NETMODE_AIM_SENDRATE			100
#define NETMODE_FIRING_SENDRATE			30


#define STATS_UPDATE_TICKS				1000

void CLocalPlayer::Init() {
    m_pPlayerPed = CGame::FindPlayerPed();
    m_bDeathSended = false;
    m_bInRCMode = false;

    m_dwLastSendSpecTick = GetTickCount();
    m_dwLastAimSendTick = GetTickCount();
    m_dwPassengerEnterExit = GetTickCount();

    ResetAllSyncAttributes();

    m_bIsSpectating = false;
    m_byteSpectateType = SPECTATE_TYPE_NONE;
    m_SpectateID = 0xFFFFFFFF;
    m_bSpawnDialogShowed = false;
}

void CLocalPlayer::ProcessClassSelection()
{
    if (!m_bSpawnDialogShowed)
    {
        RequestClass(m_iSelectedClass);
        m_bSpawnDialogShowed = true;
    }
}

void CLocalPlayer::ResetAllSyncAttributes()
{
    m_bWaitingForSpawnRequestReply = false;
	m_byteCurInterior = 0;
    m_iSelectedClass = 0;
	m_bInRCMode = false;
}

void CLocalPlayer::SendStatsUpdate()
{
	RakNet::BitStream bsStats;

//	uint32_t wAmmo = m_pPlayerPed->GetCurrentWeaponSlot()->dwAmmo;

	bsStats.Write((BYTE)ID_STATS_UPDATE);
	bsStats.Write(CHUD::iLocalMoney);
	//bsStats.Write(wAmmo);
	bsStats.Write(m_pPlayerPed->drunk_level);
	pNetGame->GetRakClient()->Send(&bsStats, HIGH_PRIORITY, UNRELIABLE, 0);
}

void CLocalPlayer::CheckWeapons()
{
    if (!ammoUpdated)
        return;

    ammoUpdated = false;

    RakNet::BitStream bs;
    bs.Write((uint8_t) ID_WEAPONS_UPDATE);
    bs.Write((uint16_t) INVALID_PLAYER_ID);
    bs.Write((uint16_t) INVALID_PLAYER_ID);

    for (int i = 0; i < MAX_WEAPONS_SLOT; i++)
    {
        bs.Write((uint8)    i);
        bs.Write((uint8)    m_pPlayerPed->m_pPed->m_aWeapons[i].m_nType);
        bs.Write((uint16)   m_pPlayerPed->m_pPed->m_aWeapons[i].m_nTotalAmmo);
    }
    pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, RELIABLE, 0);
}
extern bool g_uiHeadMoveEnabled;

#include "..//game/CWeaponsOutFit.h"
#include "java_systems/ObjectEditor.h"
#include "java_systems/casino/Chip.h"
#include "java_systems/AucContainer.h"
#include "util/patch.h"
#include "java_systems/AdminRecon.h"
#include "java_systems/Medic.h"
#include "java_systems/Tab.h"
#include "java_systems/DailyReward.h"
#include "java_systems/Styling.h"
#include "java_systems/Dialog.h"
#include "java_systems/TechInspect.h"
#include "java_systems/casino/Baccarat.h"
#include "java_systems/TireShop.h"
#include "java_systems/casino/Dice.h"
#include "java_systems/TheftAuto.h"
#include "java_systems/AutoShop.h"
#include "java_systems/casino/LuckyWheel.h"
#include "java_systems/RadialMenu.h"
#include "java_systems/GuiWrapper.h"
#include "game/World.h"
#include "voice/SpeakerList.h"
#include "voice/Record.h"
#include "voice/Playback.h"
#include "GuiWrapper.h"
#include "game/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h"

CAMERA_AIM* caAim = new CAMERA_AIM();

CVector lastPos;

uint32_t CLocalPlayer::CalculateAimSendRate(uint16_t wKeys) {
	uint32_t baseRate = NETMODE_HEADSYNC_SENDRATE;

	if (IS_TARGETING(wKeys)) {
        if (IS_FIRING(wKeys)) {
			baseRate = NETMODE_FIRING_SENDRATE + m_nPlayersInRange;
		} else {
			baseRate = NETMODE_AIM_SENDRATE + m_nPlayersInRange;
		}
	} else if (IS_FIRING(wKeys)) {
		baseRate = GetOptimumOnFootSendRate();
	}

	return static_cast<uint32_t>(baseRate);
}


bool CLocalPlayer::Process()
{
	if(CTimer::GetIsUserPaused())
		return false;

	lastPos = m_pPlayerPed->m_pPed->GetPosition();

	m_pPlayerPed->SetCurrentAim(caAim);
	caAim = m_pPlayerPed->GetCurrentAim();

	uint32_t dwThisTick = GetTickCount();

	if(m_pPlayerPed) {

		if(CGame::m_bRaceCheckpointsEnabled) {
			if(DistanceBetweenPoints2D(CGame::m_vecRaceCheckpointPos, m_pPlayerPed->m_pPed->GetPosition()) <= CGame::m_fRaceCheckpointSize)
			{
				CGame::RaceCheckpointPicked();
			}
		}
		if (m_pPlayerPed->drunk_level) {
			m_pPlayerPed->drunk_level--;
			ScriptCommand(&SET_PLAYER_DRUNKENNESS, m_pPlayerPed->m_bytePlayerNumber,
						  m_pPlayerPed->drunk_level / 100);
		}
		// handle dead
		if (!m_bDeathSended && m_pPlayerPed->IsDead()) {
			ToggleSpectating(false);
			m_pPlayerPed->FlushAttach();
			// reset tasks/anims
			m_pPlayerPed->TogglePlayerControllable(true);

			if (m_pPlayerPed->m_pPed->IsAPassenger()) {

				SendInCarFullSyncData();
			}

			//m_pPlayerPed->SetDead();
			SendDeath();

			m_bDeathSended = true;

			return true;
		}

		if ((dwThisTick - m_dwLastStatsUpdateTick) > STATS_UPDATE_TICKS) {
			SendStatsUpdate();
			m_dwLastStatsUpdateTick = dwThisTick;
		}

		CheckWeapons();
		CWeaponsOutFit::ProcessLocalPlayer(m_pPlayerPed);

		m_pPlayerPed->ProcessSpecialAction(m_pPlayerPed->m_iCurrentSpecialAction);

		// handle interior changing
		if (CGame::currArea != m_byteCurInterior)
			UpdateRemoteInterior(CGame::currArea);

		// SPECTATING
		if (m_bIsSpectating) {
            if (m_bSpawnDialogShowed)
            {
                m_bSpawnDialogShowed = false;
            }

			ProcessSpectating();
            return true;
		}

            // INCAR
        else if (m_pPlayerPed->m_pPed->IsInVehicle()) {
            MaybeSendExitVehicle();

            if (!m_pPlayerPed->m_pPed->IsAPassenger()) // DRIVER
                SendInCarFullSyncData();
            else // PASSENGER
                SendPassengerFullSyncData();
        }
        else {
            MaybeSendEnterVehicle();

            SendOnFootFullSyncData();
            UpdateSurfing();
        }


		// aim. onfoot and incar?
//		if ((dwThisTick - m_dwLastHeadUpdate) > 1000 && g_uiHeadMoveEnabled) {
//			CVector LookAt;
//			CAMERA_AIM *Aim = GameGetInternalAim();
//			LookAt = Aim->vecSource + (Aim->vecFront * 20.0f);
//
//			CGame::FindPlayerPed()->ApplyCommandTask("FollowPedSA", 0, 2000, -1, &LookAt, 0,
//														 0.1f, 500, 3, 0);
//			m_dwLastHeadUpdate = dwThisTick;
//		}

		uint16_t lrAnalog, udAnalog;
		uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

		if ((dwThisTick - m_dwLastAimSendTick) > CalculateAimSendRate(wKeys)) {
			m_dwLastAimSendTick = dwThisTick;
			SendAimSyncData();
		}

        if ((dwThisTick - m_dwLastUpdateHudButtons) > 100) {
            m_dwLastUpdateHudButtons = GetTickCount();

            CVehicleMP* m_pNearestVehicle = nullptr;
            auto nearestId = CVehiclePool::FindNearestToLocalPlayerPed();
            if (nearestId != INVALID_VEHICLE_ID) {
                auto veh = CVehiclePool::GetAt(nearestId);
                if (veh && veh->m_pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f)
                    m_pNearestVehicle = veh;
            }

            if (!m_pPlayerPed->lToggle || m_pPlayerPed->m_iCurrentSpecialAction == SPECIAL_ACTION_CARRY) {
                CHUD::bIsShowEnterExitButt = false;
            }
            else {
                if (m_pPlayerPed->m_pPed->IsInVehicle())
                    CHUD::bIsShowEnterExitButt = false;
                else {
                    if (m_pNearestVehicle)
                        CHUD::bIsShowEnterExitButt = true;
                }
            }
        }
	}
	////////////////////////////
	bool needDrawableHud = true;
	bool needDrawableChat = true;

	if(CDialog::bIsShow || CDice::bIsShow || CTab::bIsShow || CAutoShop::bIsShow || CRadialMenu::bIsShow
	   || CLuckyWheel::bIsShow || !m_pPlayerPed || CSkinShop::bIsShow ||
	   CMedic::bIsShow || CInventory::bIsShow || !m_pPlayerPed->m_bIsSpawned || CObjectEditor::bIsToggle || CChip::bIsShow
	   || CAucContainer::bIsShow || CAdminRecon::bIsShow || CHUD::bIsCamEditGui || CDailyReward::bIsShow ||
	   CTechInspect::bIsShow || CBaccarat::bIsShow || m_pPlayerPed->IsDead() || CStyling::bIsShow || CTireShop::bIsShow || CTheftAuto::bIsShow)
	{
		needDrawableHud = false;
	}

	if (CMonologue::bIsShow || CTireShop::bIsShow || CTheftAuto::bIsShow || CStyling::bIsShow || CRadialMenu::bIsShow || CRace::bIsShow) {
		needDrawableChat = false;
	}

	CHUD::toggleAll(needDrawableHud, needDrawableChat);

    if (m_pPlayerPed->GetActionTrigger() != ACTION_WASTED &&
        m_pPlayerPed->GetActionTrigger() != ACTION_DEATH &&
        pNetGame->GetGameState() == eNetworkState::CONNECTED &&
        !m_bIsSpectating)
    {
        CLocalPlayer::ProcessClassSelection();
    }

    return true;
}

void CLocalPlayer::SendDeath()
{
	RakNet::BitStream bsPlayerDeath;

	bsPlayerDeath.Write((uint16)	CLocalPlayer::lastDamageId);
	bsPlayerDeath.Write((uint8)		CLocalPlayer::lastDamageWeap);

	pNetGame->GetRakClient()->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::GoEnterVehicle(bool passenger)
{
	if (GetTickCount() - m_dwPassengerEnterExit < 1000)
		return;

	VEHICLEID ClosetVehicleID = CVehiclePool::FindNearestToLocalPlayerPed();
	if (ClosetVehicleID != INVALID_VEHICLE_ID)
	{
		CVehicleMP* pVehicle = CVehiclePool::GetAt(ClosetVehicleID);

		if (pVehicle != nullptr && pVehicle->m_pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
		{
			m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, passenger);
			SendEnterVehicleNotification(ClosetVehicleID, passenger);
			m_dwPassengerEnterExit = GetTickCount();
		}
	}
}

void CLocalPlayer::UpdateSurfing() {}

void CLocalPlayer::SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
    DLOG("SendEnterVehicleNotification");

	RakNet::BitStream bsSend;

	bsSend.Write(VehicleID);
	bsSend.Write((uint8_t)bPassenger);

	pNetGame->GetRakClient()->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0,false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::MaybeSendExitVehicle() {
    static bool oldExitVehicleState = false;
    bool exitVehicleState = m_pPlayerPed->m_pPed->IsExitingVehicle();

    if(exitVehicleState && !oldExitVehicleState) {
        auto vehicleId = CVehiclePool::FindIDFromGtaPtr(m_pPlayerPed->m_pPed->pVehicle);

        if(vehicleId != INVALID_VEHICLE_ID) {
            RakNet::BitStream bsSend;

            bsSend.Write(vehicleId);
            pNetGame->GetRakClient()->RPC(&RPC_ExitVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
        }

    }
    oldExitVehicleState = exitVehicleState;
}

void CLocalPlayer::MaybeSendEnterVehicle() {
    static bool oldEnterVehicleState = false;

    auto task
        = static_cast<CTaskComplexEnterCarAsDriver*>(m_pPlayerPed->m_pPed->GetTaskManager().CTaskManager::FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER));

    bool enterVehicleState = task != nullptr;

    if(enterVehicleState && !oldEnterVehicleState) {
        auto vehicleId = CVehiclePool::FindIDFromGtaPtr(task->GetTarget());

        if(vehicleId != INVALID_VEHICLE_ID)
            SendEnterVehicleNotification(vehicleId, false);
    }
    oldEnterVehicleState = enterVehicleState;
}


void CLocalPlayer::UpdateRemoteInterior(uint8_t byteInterior)
{
	Log("CLocalPlayer::UpdateRemoteInterior %d", byteInterior);

	m_byteCurInterior = byteInterior;
	RakNet::BitStream bsUpdateInterior;
	bsUpdateInterior.Write(byteInterior);
	pNetGame->GetRakClient()->RPC(&RPC_SetInteriorId, &bsUpdateInterior, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

bool CLocalPlayer::Spawn()
{
    if (!m_bHasSpawnInfo) {
        return false;
    }
	Log("CLocalPlayer::Spawn");

	Voice::Playback::SetSoundEnable(true);
	Voice::Record::SetMicroEnable(true);
	Voice::SpeakerList::Show();

    if (m_bSpawnDialogShowed)
    {
        m_bSpawnDialogShowed = false;
    }


	CCamera::Get().RestoreWithJumpCut();
	CCamera::SetBehindPlayer();

	//CGame::DisplayWidgets(true);
	//CGame::DisplayHUD(true);
	m_pPlayerPed->TogglePlayerControllable(true);
	
	m_pPlayerPed->SetInitialState();

	m_pPlayerPed->m_bIsSpawned = true;

    CGame::RefreshStreamingAt(m_SpawnInfo.vecPos.x, m_SpawnInfo.vecPos.y);

    m_pPlayerPed->RestartIfWastedAt(&m_SpawnInfo.vecPos, m_SpawnInfo.fRotation);
    m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	m_pPlayerPed->ClearAllWeapons();
	m_pPlayerPed->ResetDamageEntity();

    CHUD::updateAmmo();

	//CGame::DisableTrainTraffic();

    m_pPlayerPed->m_pPed->SetPosn(m_SpawnInfo.vecPos.x,
                                  m_SpawnInfo.vecPos.y, m_SpawnInfo.vecPos.z + 0.5f);

	m_pPlayerPed->ForceTargetRotation(m_SpawnInfo.fRotation);

	m_bDeathSended = false;
    m_bWaitingForSpawnRequestReply = false;

	RakNet::BitStream bsSendSpawn;
	pNetGame->GetRakClient()->RPC(&RPC_Spawn, &bsSendSpawn, SYSTEM_PRIORITY,
		RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

	return true;
}

void CLocalPlayer::HandleClassSelection()
{
    m_bClearedToSpawn = false;
    if (m_pPlayerPed) {
        m_pPlayerPed->SetInitialState();
        m_pPlayerPed->SetHealth(100.0f);
        m_pPlayerPed->TogglePlayerControllable(false);
    }
}

uint32_t CLocalPlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(CPlayerPool::GetLocalPlayerID());
}

void CLocalPlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(CPlayerPool::GetLocalPlayerID(), dwColor);
}

int CLocalPlayer::GetOptimumOnFootSendRate() {
	if (!m_pPlayerPed) return 1000;
	return (iNetModeNormalOnfootSendRate + m_nPlayersInRange * 2);
}

int CLocalPlayer::GetOptimumInCarSendRate() {
    if (!m_pPlayerPed) return 1000;
    return (iNetModeNormalInCarSendRate + m_nPlayersInRange * 2);
}

// 0.3.7
void CLocalPlayer::SendSpawn()
{
    if (!m_bSpawnDialogShowed) return;

    RequestSpawn();
    m_bWaitingForSpawnRequestReply = true;
}
// 0.3.7
void CLocalPlayer::RequestClass(int iClass)
{
    RakNet::BitStream bsClassRequest;
    bsClassRequest.Write(iClass);
    pNetGame->GetRakClient()->RPC(&RPC_RequestClass, &bsClassRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7
void CLocalPlayer::RequestSpawn()
{
    RakNet::BitStream bsSpawnRequest;
    pNetGame->GetRakClient()->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
// 0.3.7
void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO* pSpawnInfo)
{
    memcpy(&m_SpawnInfo, pSpawnInfo, sizeof(PLAYER_SPAWN_INFO));
    m_bHasSpawnInfo = true;
}
// 0.3.7
void CLocalPlayer::HandleClassSelectionOutcome(bool bOutcome)
{
    if (bOutcome)
    {
        if (m_pPlayerPed)
        {
            m_pPlayerPed->ClearAllWeapons();
            m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
        }

        m_bClearedToSpawn = true;
    }
}

uint8_t CLocalPlayer::GetSpecialAction()
{
	if(!m_pPlayerPed) return SPECIAL_ACTION_NONE;

	if(m_pPlayerPed->IsCrouching())
		return SPECIAL_ACTION_DUCK;

	if(m_pPlayerPed->IsActionCarry)
		return SPECIAL_ACTION_CARRY;

	if(m_pPlayerPed->m_pPed->IsEnteringCar())
		return SPECIAL_ACTION_ENTER_VEHICLE;

    if(m_pPlayerPed->m_pPed->IsExitingVehicle())
        return SPECIAL_ACTION_EXIT_VEHICLE;


	return SPECIAL_ACTION_NONE;
}

#include "..//chatwindow.h"
#include "game/CarEnterExit.h"
#include "localplayer.h"
#include "game/Camera.h"

void CLocalPlayer::SendOnFootFullSyncData()
{
    static ONFOOT_SYNC_DATA lastPacket;
    static auto lastTime = GetTickCount();

    auto passed = GetTickCount() - lastTime;
    if (passed < GetOptimumOnFootSendRate())
        return;

    uint16_t lrAnalog = 0, udAnalog = 0;
    const uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
    RwMatrix matPlayer = m_pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();

    CQuaternion quaternion{};
    quaternion.SetFromMatrix(&matPlayer);
    quaternion.Normalize();

    constexpr float QUATERNION_EPSILON = 0.00001f;
    if (FloatOffset(quaternion.w, lastPacket.quat.w) < QUATERNION_EPSILON &&
        FloatOffset(quaternion.x, lastPacket.quat.x) < QUATERNION_EPSILON &&
        FloatOffset(quaternion.y, lastPacket.quat.y) < QUATERNION_EPSILON &&
        FloatOffset(quaternion.z, lastPacket.quat.z) < QUATERNION_EPSILON)
    {
        quaternion = lastPacket.quat;
    }

    ONFOOT_SYNC_DATA packet = {
            .lrAnalog = lrAnalog,
            .udAnalog = udAnalog,
            .wKeys = wKeys,
            .vecPos = matPlayer.pos,
            .quat = quaternion,
            .byteHealth = static_cast<uint8_t>(m_pPlayerPed->GetHealth()),
            .byteArmour = static_cast<uint8_t>(m_pPlayerPed->GetArmour()),
            .byteCurrentWeapon = static_cast<uint8_t>((GetPlayerPed()->GetExtendedKeys() << 6) |
                                                      (GetPlayerPed()->GetCurrentWeapon() & 0x3F)),
            .byteSpecialAction = GetSpecialAction(),
            .vecMoveSpeed = m_pPlayerPed->m_pPed->GetMoveSpeed(),
            .vecSurfOffsets = CVector(0.0f, 0.0f, 0.0f),
            .wSurfInfo = 0,
            .dwAnimation = 0
    };

    const bool shouldSend = passed > timeNoSendedDataWithoutAfk ||
                            memcmp(&lastPacket, &packet, sizeof(ONFOOT_SYNC_DATA)) != 0;

    if (shouldSend) {
        lastTime = GetTickCount();

        RakNet::BitStream bsPlayerSync;
        bsPlayerSync.Write(static_cast<uint8_t>(ID_PLAYER_SYNC));
        bsPlayerSync.Write(reinterpret_cast<const char*>(&packet), sizeof(ONFOOT_SYNC_DATA));
        pNetGame->GetRakClient()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

        lastPacket = std::move(packet);
    }
}

void CLocalPlayer::SendInCarFullSyncData()
{
    static auto lastTime = GetTickCount();
    static INCAR_SYNC_DATA lastPacket;

    auto passed = GetTickCount() - lastTime;
    if (passed < GetOptimumInCarSendRate())
        return;

    CVehicleMP* pVehicle = m_pPlayerPed->GetCurrentVehicle();
    if (!pVehicle || !pVehicle->m_pVehicle) {
        return;
    }

    const VEHICLEID vehicleId = m_pPlayerPed->GetCurrentSampVehicleID();
    if (vehicleId == INVALID_VEHICLE_ID) {
        return;
    }

    uint16_t lrAnalog = 0, udAnalog = 0;
    const uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

    RwMatrix matVehicle;
    pVehicle->m_pVehicle->GetMatrix(&matVehicle);
    const CVector vecMoveSpeed = pVehicle->m_pVehicle->GetMoveSpeed();

    CQuaternion quat;
    quat.SetFromMatrix(&matVehicle);
    quat.Normalize();

    constexpr float QUATERNION_EPSILON = 0.00001f;
    if (FloatOffset(quat.w, lastPacket.quat.w) < QUATERNION_EPSILON &&
        FloatOffset(quat.x, lastPacket.quat.x) < QUATERNION_EPSILON &&
        FloatOffset(quat.y, lastPacket.quat.y) < QUATERNION_EPSILON &&
        FloatOffset(quat.z, lastPacket.quat.z) < QUATERNION_EPSILON)
    {
        quat = lastPacket.quat;
    }

    uint16_t trailerId = 0;
    if (auto pTrailer = pVehicle->m_pVehicle->m_pTrailer) {
        if (pTrailer->m_pTractor == pVehicle->m_pVehicle) {
            trailerId = CVehiclePool::FindIDFromGtaPtr(pTrailer);
        }
    }

    INCAR_SYNC_DATA packet = {
            .VehicleID = vehicleId,
            .lrAnalog = lrAnalog,
            .udAnalog = udAnalog,
            .wKeys = wKeys,
            .quat = quat,
            .vecPos = matVehicle.pos,
            .vecMoveSpeed = vecMoveSpeed,
            .fCarHealth = pVehicle->GetHealth(),
            .bytePlayerHealth = static_cast<uint8_t>(m_pPlayerPed->GetHealth()),
            .bytePlayerArmour = static_cast<uint8_t>(m_pPlayerPed->GetArmour()),
            .HydraThrustAngle = pVehicle->m_iTurnState,
            .byteSirenOn = static_cast<uint8_t>(pVehicle->m_pVehicle->m_nVehicleFlags.bSirenOrAlarm),
            .TrailerID = trailerId
    };

    if (passed > timeNoSendedDataWithoutAfk ||
        memcmp(&lastPacket, &packet, sizeof(INCAR_SYNC_DATA)) != 0)
    {
        lastTime = GetTickCount();

        RakNet::BitStream bsVehicleSync;
        bsVehicleSync.Write(static_cast<uint8_t>(ID_VEHICLE_SYNC));
        bsVehicleSync.Write(reinterpret_cast<const char*>(&packet), sizeof(INCAR_SYNC_DATA));
        pNetGame->GetRakClient()->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

        if (packet.TrailerID != 0) {
            SendTrailerSync(packet.TrailerID, pVehicle->m_pVehicle->m_pTrailer);
        }

        lastPacket = std::move(packet);
    }
}

void CLocalPlayer::SendTrailerSync(uint16 trailerId, CVehicle *trailer) {
    TRAILER_SYNC_DATA packet;

    RwMatrix matTrailer;
    trailer->GetMatrix(&matTrailer);

    CQuaternion syncQuat;
    syncQuat.SetFromMatrix(&matTrailer);
    syncQuat.Normalize();

    const TRAILER_SYNC_DATA trSync = {
            .trailerID = trailerId,
            .vecPos = matTrailer.pos,
            .quat = syncQuat,
            .vecMoveSpeed = trailer->GetMoveSpeed(),
            .vecTurnSpeed = trailer->GetTurnSpeed()
    };

    RakNet::BitStream bs;
    bs.Write((uint8) ID_TRAILER_SYNC);
    bs.Write(packet);
    pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CLocalPlayer::SendPassengerFullSyncData()
{
    static auto lastTime = GetTickCount();
    static PASSENGER_SYNC_DATA lastPacket;

    auto passed = GetTickCount() - lastTime;
    if (passed < GetOptimumInCarSendRate())
        return;

    uint16_t lrAnalog = 0, udAnalog = 0;
    const uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

    const uint16_t vehicleId = m_pPlayerPed->GetCurrentSampVehicleID();
    if (vehicleId == INVALID_VEHICLE_ID) {
        return;
    }

    PASSENGER_SYNC_DATA packet = {
            .VehicleID = vehicleId,
            .lrAnalog = lrAnalog,
            .udAnalog = udAnalog,
            .wKeys = wKeys,
            .bytePlayerHealth = static_cast<uint8_t>(m_pPlayerPed->GetHealth()),
            .bytePlayerArmour = static_cast<uint8_t>(m_pPlayerPed->GetArmour()),
            .byteSeatFlags = static_cast<uint8_t>(m_pPlayerPed->m_pPed->GetSampSeatId()),
            .byteDriveBy = 0, // m_bPassengerDriveByMode если нужно
            .byteCurrentWeapon = static_cast<uint8_t>(m_pPlayerPed->GetCurrentWeapon()),
            .vecPos = m_pPlayerPed->m_pPed->GetPosition()
    };

    const bool shouldSend = passed > timeNoSendedDataWithoutAfk ||
                            memcmp(&lastPacket, &packet, sizeof(PASSENGER_SYNC_DATA)) != 0;

    if (shouldSend)
    {
        lastTime = GetTickCount();

        RakNet::BitStream bsPassengerSync;
        bsPassengerSync.Write(static_cast<uint8_t>(ID_PASSENGER_SYNC));
        bsPassengerSync.Write(reinterpret_cast<const char*>(&packet), sizeof(PASSENGER_SYNC_DATA));
        pNetGame->GetRakClient()->Send(&bsPassengerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

        lastPacket = std::move(packet);
    }
}

void CLocalPlayer::SendAimSyncData()
{
    const CAMERA_AIM* pCameraAim = m_pPlayerPed->GetCurrentAim();

    uint8_t weaponState = 0;
    if (const CWeapon* pWeapon = m_pPlayerPed->GetCurrentWeaponSlot())
    {
        weaponState =
                (pWeapon->m_nState == WEAPONSTATE_RELOADING) ? 3 :
                ((pWeapon->dwAmmoInClip > 1) ? 2 : pWeapon->dwAmmoInClip);
    }

    AIM_SYNC_DATA aimSync = {
            .byteCamMode = static_cast<uint8_t>(m_pPlayerPed->GetCameraMode()),

            .vecAimf1 = {
                    pCameraAim->f1x,
                    pCameraAim->f1y,
                    pCameraAim->f1z
            },

            .vecAimPos = {
                    pCameraAim->pos1x,
                    pCameraAim->pos1y,
                    pCameraAim->pos1z
            },

            .fAimZ = m_pPlayerPed->GetAimZ(),

            .byteCamExtZoom = static_cast<uint8_t>(
                    static_cast<unsigned char>(
                            m_pPlayerPed->GetCameraExtendedZoom() * 63.0f
                    ) & 0x3F
            ),

            .byteWeaponState = weaponState,
            .aspect_ratio = 255
    };

    RakNet::BitStream bsAimSync;
    bsAimSync.Write((uint8_t)ID_AIM_SYNC);
    bsAimSync.Write(reinterpret_cast<char*>(&aimSync), sizeof(AIM_SYNC_DATA));

    pNetGame->GetRakClient()->Send(
            &bsAimSync,
            HIGH_PRIORITY,
            UNRELIABLE_SEQUENCED,
            0
    );
}

void CLocalPlayer::ProcessSpectating()
{
	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

    SPECTATOR_SYNC_DATA packet = {
            .vecPos = CCamera::Get().m_mCameraMatrix.GetPosition(),
            .lrAnalog = lrAnalog,
            .udAnalog = udAnalog,
            .wKeys = wKeys
    };

	if((GetTickCount() - m_dwLastSendSpecTick) > GetOptimumOnFootSendRate())
	{
		m_dwLastSendSpecTick = GetTickCount();

        RakNet::BitStream bsSpectatorSync;
		bsSpectatorSync.Write((uint8_t)ID_SPECTATOR_SYNC);
		bsSpectatorSync.Write((char*)&packet, sizeof(SPECTATOR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsSpectatorSync, HIGH_PRIORITY, UNRELIABLE, 0);
	}

    m_pPlayerPed->SetHealth(100.0f);
    m_pPlayerPed->m_pPed->Teleport(
            CVector(packet.vecPos.x, packet.vecPos.y, packet.vecPos.z + 20.0f),
            false
    );

	// handle spectate player left the server
	if(m_byteSpectateType == SPECTATE_TYPE_PLAYER)
	{
		if(!CPlayerPool::GetAt(m_SpectateID)) {
			m_byteSpectateType = SPECTATE_TYPE_NONE;
			m_bSpectateProcessed = false;
			return;
		}
	}

	// handle spectate player is no longer active (ie Died)
	if(m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
			CPlayerPool::GetAt(m_SpectateID) &&
		(!CPlayerPool::GetSpawnedPlayer(m_SpectateID) ||
				CPlayerPool::GetAt(m_SpectateID)->GetState() == PLAYER_STATE_WASTED))
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	if(m_bSpectateProcessed) return;

	if(m_byteSpectateType == SPECTATE_TYPE_NONE)
	{
		GetPlayerPed()->m_pPed->RemoveFromVehicle();
        CCamera::SetPosition(50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);
        CCamera::LookAtPoint(60.0f, 60.0f, 50.0f, 2);
		m_bSpectateProcessed = true;
	}
	else if(m_byteSpectateType == SPECTATE_TYPE_PLAYER) {
		if (CPlayerPool::GetSpawnedPlayer(m_SpectateID)) {
			if (auto* pPlayerPed = CPlayerPool::GetAt(m_SpectateID)->GetPlayerPed()) {
                
                CCamera::Get().TakeControl(pPlayerPed->m_pPed, static_cast<eCamMode>(m_byteSpectateMode), eSwitchType::JUMPCUT, 1);
				m_bSpectateProcessed = true;
			}
		}
	}
	else if(m_byteSpectateType == SPECTATE_TYPE_VEHICLE) {
        if (auto* pVehicle = CVehiclePool::GetAt((VEHICLEID) m_SpectateID)) {
            CCamera::Get().TakeControl(pVehicle->m_pVehicle, static_cast<eCamMode>(m_byteSpectateMode), eSwitchType::JUMPCUT, 1);
            m_bSpectateProcessed = true;
        }
    }
}

void CLocalPlayer::ToggleSpectating(bool bToggle)
{
	m_bIsSpectating = bToggle;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;
	m_bSpectateProcessed = false;
}

void CLocalPlayer::SpectatePlayer(PLAYERID playerId)
{
	if(CPlayerPool::GetSpawnedPlayer(playerId))
	{
		if(CPlayerPool::GetAt(playerId)->GetState() != PLAYER_STATE_NONE &&
				CPlayerPool::GetAt(playerId)->GetState() != PLAYER_STATE_WASTED)
		{
			m_byteSpectateType = SPECTATE_TYPE_PLAYER;
			m_SpectateID = playerId;
			m_bSpectateProcessed = false;
		}
	}
}

void CLocalPlayer::SpectateVehicle(VEHICLEID VehicleID)
{
	if (CVehiclePool::GetAt(VehicleID))
	{
		m_byteSpectateType = SPECTATE_TYPE_VEHICLE;
		m_SpectateID = VehicleID;
		m_bSpectateProcessed = false;
	}
}

