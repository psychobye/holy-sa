#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "util/patch.h"
#include "World.h"
#include "../java_systems/HUD.h"
#include <jni.h>

extern CNetGame *pNetGame;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uintptr_t dwCurPlayerActor = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteCurDriver = 0;
int16_t(*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
int16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return (int16_t)RemotePlayerKeys[byteCurPlayer].wKeyLR;;
	}
	else
	{
		int16_t dwResult = CPad__GetPedWalkLeftRight(thiz);
		LocalPlayerKeys.wKeyLR = (uint16_t)dwResult;
		return dwResult;
	}
}

int16_t(*CPad__GetPedWalkUpDown)(uintptr_t thiz);
int16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return (int16_t)RemotePlayerKeys[byteCurPlayer].wKeyUD;;
	}
	else
	{
		// Local player
		int16_t dwResult = CPad__GetPedWalkUpDown(thiz);
		LocalPlayerKeys.wKeyUD = (uint16_t)dwResult;
		return dwResult;
	}
}

int (*CPad__GetLookLeft)(uintptr_t *thiz, bool a2);
int CPad__GetLookLeft_hook(uintptr_t *thiz, bool a2)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__GetLookLeft(thiz, a2);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
}

int (*CPad__GetLookRight)(uintptr_t *thiz, bool a2);
int CPad__GetLookRight_hook(uintptr_t *thiz, bool a2)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__GetLookRight(thiz, a2);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
}

bool (*CPad__GetTurretRight)(uintptr_t *thiz);
bool CPad__GetTurretRight_hook(uintptr_t *thiz)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__GetTurretRight(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
}

bool (*CPad__GetTurretLeft)(uintptr_t *thiz);
bool CPad__GetTurretLeft_hook(uintptr_t *thiz)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__GetTurretLeft(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
}

//int (*CPad__ShiftTargetLeftJustDown)(uintptr_t *thiz);
//int CPad__ShiftTargetLeftJustDown_hook(uintptr_t *thiz)
//{
//	Log("shitft L = %d", CPad__ShiftTargetLeftJustDown(thiz));
//	if(dwCurPlayerActor && (byteCurPlayer != 0))
//	{
//		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
//	}
//	else
//	{
//		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__ShiftTargetLeftJustDown(thiz);
//		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
//	}
//}
//
//int (*CPad__ShiftTargetRightJustDown)(uintptr_t *thiz);
//int CPad__ShiftTargetRightJustDown_hook(uintptr_t *thiz)
//{
//	Log("shitft R = %d", CPad__ShiftTargetRightJustDown(thiz));
//	if(dwCurPlayerActor && (byteCurPlayer != 0))
//	{
//		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
//	}
//	else
//	{
//		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__ShiftTargetRightJustDown(thiz);
//		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
//	}
//}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyLR;
	}
	else
	{
		int16_t dwResult = CPad__GetSteeringLeftRight(thiz);
		LocalPlayerKeys.wKeyLR = (uint16_t)dwResult;
		return dwResult;
	}
}

int16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
int16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyUD;
	}
	else
	{
		// local player
		int16_t dwResult = CPad__GetSteeringUpDown(thiz);
		LocalPlayerKeys.wKeyUD = (uint16_t)dwResult;
		return dwResult;
	}
}

uint32_t (*CPad__SprintJustDown)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__SprintJustDown_hook(uintptr_t thiz, uint32_t unk)
{
	return 0;
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}
//
bool (*CPad__JumpJustDown)(uintptr_t* thiz);
bool CPad__JumpJustDown_hook(uintptr_t* thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return false;
	}
	else
	{
		if(CGame::isBanJump)
		{
			return false;
		}
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}
//
bool (*CPad__GetJump)(uintptr_t *thiz);
bool CPad__GetJump_hook(uintptr_t *thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		//if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return false;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
        if(CGame::isBanJump)
        {
            return false;
        }

		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

int16 (*CPad__GetCarGunFired)(uintptr_t *thiz, bool bCanShootFwd, bool bAir);
int16 CPad__GetCarGunFired_hook(uintptr_t *thiz, bool bCanShootFwd, bool bAir)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		auto res = CPad__GetCarGunFired(thiz, bCanShootFwd, bAir);

		if(res)
			LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;

		return res;
	}
}

int16 (*CPad__GetCarGunUpDown)(uintptr *thiz, int bCheckHydraulics, uintptr *pAutomobile, float fLimit, int bPoop);
int16 CPad__GetCarGunUpDown_hook(uintptr *thiz, int bCheckHydraulics, uintptr *pAutomobile, float fLimit, int bPoop)
{
	if(byteCurDriver != 0)
	{
		if(RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_ANALOG_UP])
            return 0x80;

        if(RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_ANALOG_DOWN])
            return 0xffffff80;

        return 0;
	}
	else
	{
        auto res = CPad__GetCarGunUpDown(thiz, bCheckHydraulics, pAutomobile, fLimit, bPoop);

        if(res == 0x80)
            LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_UP] = true;

        if(res == 0xffffff80)
            LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_DOWN] = true;

        return res;
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAbortClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return 0;
	}
	else
	{
		return CPad__DuckJustDown(thiz, unk);
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{

	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK])
			return 2;

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;

		//if(dwResult == 2)
		//{
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		//}
		//else if(dwResult == 1)
		//{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = false;
		//}

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		CPedSamp* pPlayerPed = CGame::FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->m_pPed->IsInVehicle() || pPlayerPed->m_pPed->IsAPassenger())
				return 0;
		}

		// local player
		uint16_t wAccelerate = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
		if (wAccelerate == 0xFF)
		{
			if (pPlayerPed)
			{
				CVehicle* pGtaVehicle = pPlayerPed->GetGtaVehicle();
				if (pGtaVehicle)
				{
					if (pGtaVehicle->m_nVehicleFlags.bEngineOn == 0)
					{
						wAccelerate = 0;
					}
				}
			}
		}

		return wAccelerate;
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint32_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

bool (*CPad__GetHorn)(uintptr_t* thiz);
bool CPad__GetHorn_hook(uintptr_t* thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		auto ret = CPad__GetHorn(thiz);
		// local player
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = ret;
		return ret;
	}
}

uint32_t (*CVehicle__UsesSiren)(uintptr_t* thiz);
uint32_t CVehicle__UsesSiren_hook(uintptr_t* thiz)
{
	return 0;
}

extern float * pfCameraExtZoom;
void (*CPed__ProcessControl)(CPlayerPed* thiz);
void CPed__ProcessControl_hook(CPlayerPed* thiz)
{
	dwCurPlayerActor = reinterpret_cast<uintptr_t>(thiz);
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// REMOTE PLAYER
		uint16_t byteSavedCameraMode;
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);


		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4)
			*wCameraMode2 = 0;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		CHook::NOP(g_libGTASA + (VER_x32 ? 0x004A2A92 : 0x598D1C), 2);

		CWorld::PlayerInFocus = byteCurPlayer;
		// call original
		
		CPed__ProcessControl(thiz);
		// restore

#if VER_x32
		CHook::WriteMemory(g_libGTASA + 0x004A2A92, "\xFE\xF7\x81\xFF", 4);
#else
        CHook::WriteMemory(g_libGTASA + 0x598D1C, "\x7A\xFB\xFF\x97", 4);
#endif

		CWorld::PlayerInFocus = 0;
		*pbyteCameraMode = byteSavedCameraMode;

		GameSetLocalPlayerCameraExtZoom();
		GameSetLocalPlayerAim();
		*wCameraMode2 = wSavedCameraMode2;
	}
	else
	{
		CPed__ProcessControl(thiz);
	}
}

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	CVehicle *pVehicle = (CVehicle*)thiz;
	uintptr_t this_vtable = *(uintptr_t*)pVehicle;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch(this_vtable)
	{
#if VER_x32
		// CAutomobile
		case 0x0066D678:
		call_addr = 0x00553E44;
		break;

		// CBoat
		case 0x0066DA20:
		call_addr = 0x0056BEC0;
		break;

		// CBike
		case 0x0066D7F0:
		call_addr = 0x00561A90;
		break;

		// CPlane
		case 0x0066DD84:
		call_addr = 0x00575CF8;
		break;

		// CHeli
		case 0x0066DB34:
		call_addr = 0x005712A8;
		break;

		// CBmx
		case 0x0066D908:
		call_addr = 0x00568B84;
		break;

		// CMonsterTruck
		case 0x0066DC5C:
		call_addr = 0x00574864;
		break;

		// CQuadBike
		case 0x0066DEAC:
		call_addr = 0x0057A2F0;
		break;

		// CTrain
		case 0x0066E0FC:
		call_addr = 0x0057D0A0;
		break;
#else
        // CAutomobile
		case 0x83BB50:
		call_addr = 0x67459C;
		break;

		// CBoat
		case 0x83C2A0:
		call_addr = 0x68DCE8;
		break;

		// CBike
		case 0x83BE40:
		call_addr = 0x682BC8;
		break;

		// CPlane
		case 0x83C968:
		call_addr = 0x6993B8;
		break;

		// CHeli
		case 0x83C4C8:
		call_addr = 0x693978;
		break;

		// CBmx
		case 0x83C070:
        call_addr = 0x68A9AC;
		break;

		// CMonsterTruck
		case 0x83C718:
		call_addr = 0x698090;
		break;

		// CQuadBike
		case 0x83CBB8:
		call_addr = 0x69DB44;
		break;

		// CTrain
		case 0x83D058:
		call_addr = 0x6A0A20;
		break;
#endif
	}

	if(pVehicle && pVehicle->pDriver)
	{
		byteCurDriver = FindPlayerNumFromPedPtr((uintptr_t)pVehicle->pDriver);
	}

	if(pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 &&
		pVehicle->pDriver != GamePool_FindPlayerPed() &&
			CWorld::PlayerInFocus == 0) // CWorld::PlayerInFocus
	{
		CWorld::PlayerInFocus = 0;

		pVehicle->pDriver->m_nPedType = static_cast<ePedType>(4);
		//CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_libGTASA + (VER_x32 ? 0x003ACE04 + 1: 0x489B78)))(reinterpret_cast<uintptr_t>(&pVehicle->m_VehicleAudioEntity));
		pVehicle->pDriver->m_nPedType = static_cast<ePedType>(0);
	}
	else
	{
		(( void (*)(uintptr_t))(g_libGTASA + (VER_x32 ? 0x003ACE04 + 1 : 0x489B78)))(reinterpret_cast<uintptr_t>(&pVehicle->m_VehicleAudioEntity));
	}

	// Tyre burst fix
	if (pVehicle->pDriver)
	{
		if (pVehicle->m_nVehicleFlags.bTyresDontBurst)
		{
			pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
		}
		if(!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
	}
	else
	{
		if (!pVehicle->m_nVehicleFlags.bTyresDontBurst)
		{
			pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
		}
		if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
	}

	// VEHTYPE::ProcessControl()
    (( void (*)(CVehicle*))(g_libGTASA + call_addr + (VER_x32 ? 1 : 0)))(pVehicle);
}


uint32_t(*CPad__GetWeapon)(uintptr_t thiz, uintptr_t ped, bool unk);
uint32_t CPad__GetWeapon_hook(uintptr_t thiz, uintptr_t ped, bool unk)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint8_t old = CWorld::PlayerInFocus;
		CWorld::PlayerInFocus = byteCurPlayer;
		uintptr_t value = CPad__GetWeapon(thiz, ped, unk);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = value;
		CWorld::PlayerInFocus = old;
		return value;
	}
}

#include "gui/gui.h"
#include "java_systems/HUD.h"
#include "WaterCannons.h"
#include "Camera.h"


uintptr_t g_playerPed;

uintptr_t(*ProcessPlayerWeapon)(uintptr_t thiz, uintptr_t playerPed);
uintptr_t ProcessPlayerWeapon_hook(uintptr_t thiz, uintptr_t playerPed)
{
	g_playerPed = playerPed;
	uintptr_t toRetn = ProcessPlayerWeapon(thiz, playerPed);

	g_playerPed = 0;
	return toRetn;
}

void (*CAutomobile__FireTruckControl)(uintptr_t* thiz, uintptr_t* pFire);
void CAutomobile__FireTruckControl_hook(uintptr_t* thiz, uintptr_t* pFire)
{
	if(byteCurDriver != 0) {
		uint8_t old = CWorld::PlayerInFocus;
		CWorld::PlayerInFocus = byteCurDriver;
		CAutomobile__FireTruckControl(thiz, pFire);
		CWorld::PlayerInFocus = old;

		CWaterCannons::bFromLocalPlayer = false;
	}
	else {
		CWaterCannons::bFromLocalPlayer = true;
	}
	return CAutomobile__FireTruckControl(thiz, pFire);
	//return res;
}

uint32_t(*CCamera_IsTargetingActive)(CCamera* thiz, CPed* pPed);
uint32_t CCamera_IsTargetingActive_hook(CCamera* thiz, CPed* pPed)
{
	if (pPed != GamePool_FindPlayerPed())
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 1 : 0;
	}
	else
	{
		/* CCamera::IsTargetingActive */
		static CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
		uint32_t bIsTargeting = CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x3D9F54 + 1 : 0x4B8154), &TheCamera);

		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = bIsTargeting;
		return bIsTargeting;
	}
}

uint32_t(*CPad__GetEnterTargeting)(uintptr_t thiz);
uint32_t CPad__GetEnterTargeting_hook(uintptr_t thiz)
{

	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];
	}
	else
	{
		if(pNetGame && pNetGame->m_GreenZoneState )
		{
			return 0;
		}
		uint8_t old = CWorld::PlayerInFocus;
		CWorld::PlayerInFocus = byteCurPlayer;
		uintptr_t result = CPad__GetEnterTargeting(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result;


		CWorld::PlayerInFocus = old;
		return result;
	}
}

uint32_t TaskUseGun(uintptr_t thiz, uintptr_t ped)
{
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	uint32_t result = 0;

	if (dwCurPlayerActor &&
		(byteCurPlayer != 0)) // not local player and local player's keys set.
	{
		uint16_t byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4)* wCameraMode2 = 0;

		// save the camera zoom factor, apply the context
		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);

		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		CWorld::PlayerInFocus = byteCurPlayer;

		result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + (VER_x32 ? 0x004DDB70 + 1 : 0x5DFCC4)))(thiz, ped);

		// restore the camera modes, internal id and local player's aim
		*pbyteCameraMode = byteSavedCameraMode;

		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoom();

		CWorld::PlayerInFocus = 0;
		GameSetLocalPlayerAim();
		*wCameraMode2 = wSavedCameraMode2;
	}
	else
	{
		result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + (VER_x32 ? 0x004DDB70 + 1 : 0x5DFCC4)))(thiz, ped);
	}

	return result;
}


uint32_t CPad__TaskProcess(uintptr_t thiz, uintptr_t ped, int unk, int unk1)
{
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	uint8_t old = CWorld::PlayerInFocus;
	CWorld::PlayerInFocus = byteCurPlayer;

	uint32_t result =  ((uint32_t(*)(uintptr_t, uintptr_t, int, int))(g_libGTASA + (VER_x32 ? 0x00539F9C + 1 : 0x655E28)))(thiz, ped, unk, unk1);
	CWorld::PlayerInFocus = old;
	return result;
}

bool bWeaponClicked;

extern CGUI *pGUI;

uint32_t (*CPad__CycleWeaponRightJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponRightJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;

	if(!bWeaponClicked) {
		return 0;
	}
	else {
		bWeaponClicked = false;
		return 1;
	}
	return CPad__CycleWeaponRightJustDown(thiz);
}


uint32_t(*CPad__GetNitroFired)(uintptr_t thiz);
uint32_t CPad__GetNitroFired_hook(uintptr_t thiz)
{
	if (byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		CPedSamp* pPlayerPed = CGame::FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->m_pPed->IsInVehicle() || pPlayerPed->m_pPed->IsAPassenger())
				return 0;
		}

		// local player
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__GetNitroFired(thiz);
		//return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
		return CPad__GetNitroFired(thiz);
	}
}

uint32_t(*CPad__GetHydraulicJump)(uintptr_t thiz);
uint32_t CPad__GetHydraulicJump_hook(uintptr_t thiz)
{
	if (byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		CPedSamp* pPlayerPed = CGame::FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->m_pPed->IsInVehicle() || pPlayerPed->m_pPed->IsAPassenger())
				return 0;
		}

		// local player
		uint32_t result = CPad__GetHydraulicJump(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = result;
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	// CPedSamp::ProcessControl
	CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x6692A4 : 0x833150), &CPed__ProcessControl_hook, &CPed__ProcessControl);

    // lr/ud (onfoot)
    CHook::InlineHook("_ZN4CPad19GetPedWalkLeftRightEv", &CPad__GetPedWalkLeftRight_hook, &CPad__GetPedWalkLeftRight);
    CHook::InlineHook("_ZN4CPad16GetPedWalkUpDownEv", &CPad__GetPedWalkUpDown_hook, &CPad__GetPedWalkUpDown);

    // steering lr/ud (incar)
    CHook::InlineHook("_ZN4CPad20GetSteeringLeftRightEv", &CPad__GetSteeringLeftRight_hook, &CPad__GetSteeringLeftRight);
    CHook::InlineHook("_ZN4CPad17GetSteeringUpDownEv", &CPad__GetSteeringUpDown_hook, &CPad__GetSteeringUpDown);

    CHook::InlineHook("_ZN4CPad13GetAccelerateEv", &CPad__GetAccelerate_hook, &CPad__GetAccelerate);
    CHook::InlineHook("_ZN4CPad8GetBrakeEv", &CPad__GetBrake_hook, &CPad__GetBrake);
    CHook::InlineHook("_ZN4CPad12GetHandBrakeEv", &CPad__GetHandBrake_hook, &CPad__GetHandBrake);
    CHook::InlineHook("_ZN4CPad13GetNitroFiredEv", &CPad__GetNitroFired_hook, &CPad__GetNitroFired);
    CHook::InlineHook("_ZN4CPad16GetHydraulicJumpEv", &CPad__GetHydraulicJump_hook, &CPad__GetHydraulicJump);

    CHook::InlineHook("_ZN4CPad9GetSprintEi", &CPad__GetSprint_hook, &CPad__GetSprint);
    CHook::InlineHook("_ZN4CPad12JumpJustDownEv", &CPad__JumpJustDown_hook, &CPad__JumpJustDown);
    CHook::InlineHook("_ZN4CPad7GetJumpEv", &CPad__GetJump_hook, &CPad__GetJump);
    CHook::InlineHook("_ZN4CPad15GetCarGunUpDownEbP11CAutomobilefb", &CPad__GetCarGunUpDown_hook, &CPad__GetCarGunUpDown);
    CHook::InlineHook("_ZN4CPad12GetAutoClimbEv", &CPad__GetAutoClimb_hook, &CPad__GetAutoClimb);
    CHook::InlineHook("_ZN4CPad13GetAbortClimbEv", &CPad__GetAbortClimb_hook, &CPad__GetAbortClimb);
    // swimm
    CHook::InlineHook("_ZN4CPad12DiveJustDownEv", &CPad__DiveJustDown_hook, &CPad__DiveJustDown);
    CHook::InlineHook("_ZN4CPad16SwimJumpJustDownEv", &CPad__SwimJumpJustDown_hook, &CPad__SwimJumpJustDown);

    //
    CHook::InlineHook("_ZN4CPad13GetTurretLeftEv", &CPad__GetTurretLeft_hook, &CPad__GetTurretLeft);
    CHook::InlineHook("_ZN4CPad14GetTurretRightEv", &CPad__GetTurretRight_hook, &CPad__GetTurretRight);

    // all vehicles ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66D6A4 : 0x83BBA8), &AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66DA4C : 0x83C2F8), &AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66D81C : 0x83BE98), &AllVehicles__ProcessControl_hook); // CBike::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66DDB0 : 0x83C9C0), &AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66DB60 : 0x83C520), &AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66D934 : 0x83C0C8), &AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66DC88 : 0x83C770), &AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66DED8 : 0x83CC10), &AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66E128 : 0x83D0B0), &AllVehicles__ProcessControl_hook); // CTrain::ProcessControl

    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66968C : 0x833928), &TaskUseGun);
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66CF4C : 0x83ACF8), &CPad__TaskProcess);

	// sprint/jump stuff
	CHook::InlineHook("_ZN4CPad14SprintJustDownEv", &CPad__SprintJustDown_hook, &CPad__SprintJustDown);// �������������

	// �������� �� ���� �� ���� / �������
	CHook::InlineHook("_ZN4CPad14GetCarGunFiredEbb", &CPad__GetCarGunFired_hook, &CPad__GetCarGunFired);
	//CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x6703F4 : 0xFF), &CAutomobile__FireTruckControl_hook, &CAutomobile__FireTruckControl);

	CHook::InlineHook("_ZN4CPad19MeleeAttackJustDownEv", &CPad__MeleeAttackJustDown_hook, &CPad__MeleeAttackJustDown);

	CHook::InlineHook("_ZN4CPad9GetWeaponEP4CPedb", &CPad__GetWeapon_hook, &CPad__GetWeapon);
	CHook::InlineHook("_ZN4CPad17GetEnterTargetingEv", &CPad__GetEnterTargeting_hook, &CPad__GetEnterTargeting);

	CHook::InlineHook("_ZN7CCamera17IsTargetingActiveEP10CPlayerPed", &CCamera_IsTargetingActive_hook, &CCamera_IsTargetingActive);

	CHook::InlineHook("_ZN23CTaskSimplePlayerOnFoot19ProcessPlayerWeaponEP10CPlayerPed", &ProcessPlayerWeapon_hook, &ProcessPlayerWeapon);

	CHook::InlineHook("_ZN4CPad12DuckJustDownEP4CPed", &CPad__DuckJustDown_hook, &CPad__DuckJustDown);
	CHook::InlineHook("_ZN4CPad8GetBlockEv", &CPad__GetBlock_hook, &CPad__GetBlock);

	CHook::InlineHook("_ZN4CPad7GetHornEb", &CPad__GetHorn_hook, &CPad__GetHorn);
	CHook::InlineHook("_ZNK8CVehicle9UsesSirenEv", &CVehicle__UsesSiren_hook, &CVehicle__UsesSiren);

	CHook::InlineHook("_ZN4CPad24CycleWeaponRightJustDownEv", &CPad__CycleWeaponRightJustDown_hook, &CPad__CycleWeaponRightJustDown);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_changeWeapon(JNIEnv *env, jobject thiz) {
	if(!CGame::FindPlayerPed()) return;

	if(!bWeaponClicked) {
		bWeaponClicked = true;
	}
	else {
		bWeaponClicked = false;
	}
}