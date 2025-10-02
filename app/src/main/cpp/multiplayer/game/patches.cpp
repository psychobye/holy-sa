#include "../main.h"
#include "common.h"

#include "../CSettings.h"
#include "..//CDebugInfo.h"
#include "game.h"
#include "util/patch.h"
#include "chatwindow.h"
#include "CPlayerInfoGta.h"
#include "util/CJavaWrapper.h"
#include "World.h"

void InitInGame();

void DisableAutoAim()
{
	CHook::RET("_ZN10CPlayerPed22FindWeaponLockOnTargetEv"); // CPedSamp::FindWeaponLockOnTarget
	CHook::RET("_ZN10CPlayerPed26FindNextWeaponLockOnTargetEP7CEntityb"); // CPedSamp::FindNextWeaponLockOnTarget
	CHook::RET("_ZN4CPed21SetWeaponLockOnTargetEP7CEntity"); // CPed::SetWeaponLockOnTarget
}

void ApplyFPSPatch(uint8_t fps)
{
    if(fps > CSettings::maxFps)
        fps = CSettings::maxFps;

#if VER_x32
    CHook::NOP(g_libGTASA + 0x005E49F4, 1);
    CHook::NOP(g_libGTASA + 0x005E4934, 1);
#else
    CHook::NOP(g_libGTASA + 0x70A474, 1);
    CHook::NOP(g_libGTASA + 0x70A398, 1);
#endif
    CGame::PostToMainThread([=]{
        auto RsGlobal = (RsGlobalType*)(g_libGTASA + (VER_x32 ? 0x009FC8FC : 0xC9B320));
        RsGlobal->maxFPS = 60;
    });

    Log("New fps limit = %d", fps);
}

int test_pointsArray[1000];
int test_pointersLibArray[1000];

void ApplyPatches_level0()
{
	Log("ApplyPatches_level0");
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006783C0 : 0x84E7A8), &CWorld::Players);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679B5C : 0x8516D8), &CWorld::PlayerInFocus);

	CHook::Redirect("_ZN6CWorld28FindPlayerSlotWithPedPointerEPv", &CUtil::FindPlayerSlotWithPedPointer);

	// 3 touch begin
	memset(test_pointsArray, 0, 999 * sizeof(int));
	CHook::Write(g_libGTASA + (VER_x32 ? 0x00679E90 : 0x851D38), &test_pointsArray);

	memset(test_pointersLibArray, 0, 999 * sizeof(int));
	CHook::Write(g_libGTASA + (VER_x32 ? 0x006D7178 : 0x8B5028), test_pointersLibArray);

    // 3 touch end
#if VER_x32
	CHook::WriteMemory(g_libGTASA + 0x0026B03C, (uintptr_t)"\x03\x20", 2); // OS_PointerGetNumber
	CHook::WriteMemory(g_libGTASA + 0x0026B046, (uintptr_t) "\x03\x28", 2); // OS_PointerGetType
	CHook::WriteMemory(g_libGTASA + 0x002700AC, (uintptr_t) "\x03\x28", 2); // LIB_PointerGetCoordinates
	CHook::WriteMemory(g_libGTASA + 0x00270118, (uintptr_t) "\x03\x28", 2); // LIB_PointerGetWheel
	CHook::WriteMemory(g_libGTASA + 0x00270164, (uintptr_t) "\x03\x28", 2); // LIB_PointerDoubleClicked
	CHook::WriteMemory(g_libGTASA + 0x002700F2, (uintptr_t) "\x03\x2A", 2); // LIB_PointerGetButton
#else
    CHook::WriteMemory(g_libGTASA + 0x320844, (uintptr_t)"\x60\x00\x80\x52", 4); // OS_PointerGetNumber
	CHook::WriteMemory(g_libGTASA + 0x320850, (uintptr_t) "\x1F\x0C\x00\x71", 4); // OS_PointerGetType
	CHook::WriteMemory(g_libGTASA + 0x326FEC, (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerGetCoordinates
	CHook::WriteMemory(g_libGTASA + 0x32706C, (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerGetWheel
	CHook::WriteMemory(g_libGTASA + 0x3270AC, (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerDoubleClicked
	CHook::WriteMemory(g_libGTASA + 0x327040, (uintptr_t) "\x1F\x0D\x00\x71", 4); // LIB_PointerGetButton
#endif

// fix aplha raster
#if VER_x32
    CHook::WriteMemory(g_libGTASA + 0x001AE8DE, (uintptr_t)"\x01\x22", 2);
#else
    CHook::WriteMemory(g_libGTASA + 0x23FDE0, (uintptr_t)"\x22\x00\x80\x52", 4);
#endif
     DisableAutoAim();

	CHook::RET("_ZN6CTrain10InitTrainsEv"); // CTrain::InitTrains

    CHook::RET("_ZN8CClothes4InitEv"); // CClothes::Init()
	CHook::RET("_ZN8CClothes13RebuildPlayerEP10CPlayerPedb"); // CClothes::RebuildPlayer

	CHook::RET("_ZNK35CPedGroupDefaultTaskAllocatorRandom20AllocateDefaultTasksEP9CPedGroupP4CPed"); // AllocateDefaultTasks
	CHook::RET("_ZN6CGlass4InitEv"); // CGlass::Init
    CHook::RET("_ZN8CGarages17Init_AfterRestartEv"); // CGarages::Init_AfterRestart
    CHook::RET("_ZN6CGangs10InitialiseEv"); // CGangs::Initialise
    CHook::RET("_ZN5CHeli9InitHelisEv"); // CHeli::InitHelis(void)
	CHook::RET("_ZN11CFileLoader10LoadPickupEPKc"); // CFileLoader::LoadPickup

    // entryexit
    CHook::RET("_ZN17CEntryExitManager22PostEntryExitsCreationEv");
}

void ApplyShadowPatch()
{
	CHook::RET("_ZN22CRealTimeShadowManager20ReturnRealTimeShadowEP15CRealTimeShadow"); // CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
	CHook::RET("_ZN22CRealTimeShadowManager6UpdateEv"); //shadow CRealTimeShadowManager::Update
    // CHook::RET(g_libGTASA + 0x543B04); // CShadows::RenderStaticShadows
    // CHook::RET("_ZN8CMirrors16BeforeMainRenderEv"); // CMirrors::BeforeMainRender(void)
}

void ApplyPatches()
{
	Log("Installing patches..");

	CHook::RET("_ZN17CVehicleModelInfo17SetCarCustomPlateEv"); // default plate

	CHook::RET("_Z16SaveGameForPause10eSaveTypesPc"); // �� ��������� ��� ������������. ������ �����

#if VER_x32
	// ������ ������
	CHook::WriteMemory(g_libGTASA + 0x00442120, (uintptr_t)"\x2C\xE0", 2); // B 0x44217c
	CHook::WriteMemory(g_libGTASA + 0x0044217C, (uintptr_t)"\x30\x46", 2); // mov r0, r6
	// CRadar::DrawEntityBlip (translate color)
	CHook::WriteMemory(g_libGTASA + 0x004404C0, (uintptr_t)"\x3A\xE0", 2); // B 0x440538
	CHook::WriteMemory(g_libGTASA + 0x00440538, (uintptr_t)"\x30\x46", 2); // mov r0, r6

	// CRadar::DrawCoordBlip (translate color)
	CHook::WriteMemory(g_libGTASA + 0x0043FB5E, (uintptr_t)"\x12\xE0", 2); // B 0x43fb86
	CHook::WriteMemory(g_libGTASA + 0x0043FB86, (uintptr_t)"\x48\x46", 2); // mov r0, r9
	CHook::WriteMemory(g_libGTASA + 0x002AB5C6, (uintptr_t)"\x00\x21", 2);
#else
    // ������ ������
	CHook::WriteMemory(g_libGTASA + 0x52737C, (uintptr_t)"\x1E\x00\x00\x14", 4); // B 0x5273F4
	CHook::WriteMemory(g_libGTASA + 0x5273F4, (uintptr_t)"\xE1\x03\x14\x2A", 4); // mov w1, w20

	// CRadar::DrawEntityBlip (translate color)
	CHook::WriteMemory(g_libGTASA + 0x5258D8, (uintptr_t)"\x22\x00\x00\x14", 4); // B 0x525960
	CHook::WriteMemory(g_libGTASA + 0x525960, (uintptr_t)"\xE1\x03\x16\x2A", 4); // mov w1, W22

	// CRadar::DrawCoordBlip (translate color)
	CHook::WriteMemory(g_libGTASA + 0x524F58, (uintptr_t)"\xCC\xFF\xFF\x17", 4); // B 0x524E88
	CHook::WriteMemory(g_libGTASA + 0x524E88, (uintptr_t)"\xE1\x03\x16\x2A", 4); // mov w1, W22
	//CHook::WriteMemory(g_libGTASA + 0x002AB5C6, (uintptr_t)"\x00\x21", 2);

    CHook::NOP(g_libGTASA + 0x5240C4, 2); // CRadar::DrawRadarSprite(4, ..); (North Icon)

    // crash legend
    CHook::NOP(g_libGTASA + 0x36A690, 1);
#endif

	ApplyShadowPatch();

	CDebugInfo::ApplyDebugPatches();

	CHook::RET("_ZN12CAudioEngine16StartLoadingTuneEv"); // ���� ������������ ������

	// DefaultPCSaveFileName
	char* DefaultPCSaveFileName = (char*)(g_libGTASA + (VER_x32 ? 0x006B012C : 0x88CB08));
	memcpy(DefaultPCSaveFileName, "GTASAMP", 8);

#if VER_x32
	CHook::NOP(g_libGTASA + 0x003F61B6, 2);	// CCoronas::RenderSunReflection crash
	CHook::NOP(g_libGTASA + 0x00584884, 2);	// �� ������ ��� ��� ������ �� ����� 	( ������, �������� and etc )
	CHook::NOP(g_libGTASA + 0x00584850, 2);	// �� ������ ��� ��� ������ �� �����	( ������, �������� and etc )
#else
    CHook::NOP(g_libGTASA + 0x004D8700, 1);  // CCoronas::RenderSunReflection crash
    CHook::NOP(g_libGTASA + 0x006A852C, 1);  // �� ������ ��� ��� ������ �� �����   ( ������, �������� and etc )
    CHook::NOP(g_libGTASA + 0x006A84E0, 1);  // �� ������ ��� ��� ������ �� �����  ( ������, �������� and etc )

#endif

	CHook::RET("_ZN17CVehicleRecording4LoadEP8RwStreamii"); // CVehicleRecording::Load

    CHook::RET("_ZN18CMotionBlurStreaks6UpdateEv");
    CHook::RET("_ZN7CCamera16RenderMotionBlurEv");

	CHook::RET("_ZN11CPlayerInfo17FindObjectToStealEP4CPed"); // Crash
	CHook::RET("_ZN26CAEGlobalWeaponAudioEntity21ServiceAmbientGunFireEv");	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
	CHook::RET("_ZN30CWidgetRegionSteeringSelection4DrawEv"); // CWidgetRegionSteeringSelection::Draw
	CHook::RET("_ZN23CTaskSimplePlayerOnFoot18PlayIdleAnimationsEP10CPlayerPed");	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
	CHook::RET("_ZN13CCarEnterExit17SetPedInCarDirectEP4CPedP8CVehicleib");	// CCarEnterExit::SetPedInCarDirect
	CHook::RET("_ZN6CRadar10DrawLegendEiii"); // CRadar::DrawLgegend
	CHook::RET("_ZN6CRadar19AddBlipToLegendListEhi"); // CRadar::AddBlipToLegendList

	CHook::RET("_ZN11CAutomobile35CustomCarPlate_BeforeRenderingStartEP17CVehicleModelInfo"); // CAutomobile::CustomCarPlate_BeforeRenderingStart
	CHook::RET("_ZN11CAutomobile33CustomCarPlate_AfterRenderingStopEP17CVehicleModelInfo"); // CAutomobile::CustomCarPlate_AfterRenderingStop
	CHook::RET("_ZN7CCamera8CamShakeEffff"); // CCamera::CamShake
	CHook::RET("_ZN7CEntity23PreRenderForGlassWindowEv"); // CEntity::PreRenderForGlassWindow
	CHook::RET("_ZN8CMirrors16RenderReflBufferEb"); // CMirrors::RenderReflBuffer
	CHook::RET("_ZN4CHud23DrawBustedWastedMessageEv"); // CHud::DrawBustedWastedMessage // ���������
	CHook::RET("_ZN4CHud14SetHelpMessageEPKcPtbbbj"); // CHud::SetHelpMessage
	CHook::RET("_ZN4CHud24SetHelpMessageStatUpdateEhtff"); // CHud::SetHelpMessageStatUpdate
	CHook::RET("_ZN6CCheat16ProcessCheatMenuEv"); // CCheat::ProcessCheatMenu
	CHook::RET("_ZN6CCheat13ProcessCheatsEv"); // CCheat::ProcessCheats
	CHook::RET("_ZN6CCheat16AddToCheatStringEc"); // CCheat::AddToCheatString
	CHook::RET("_ZN6CCheat12WeaponCheat1Ev"); // CCheat::WeaponCheat1
	CHook::RET("_ZN6CCheat12WeaponCheat2Ev"); // CCheat::WeaponCheat2
	CHook::RET("_ZN6CCheat12WeaponCheat3Ev"); // CCheat::WeaponCheat3
	CHook::RET("_ZN6CCheat12WeaponCheat4Ev"); // CCheat::WeaponCheat4
	CHook::RET("_ZN8CGarages14TriggerMessageEPcsts"); // CGarages::TriggerMessage

	CHook::RET("_ZN11CPopulation6AddPedE8ePedTypejRK7CVectorb"); // CPopulation::AddPed
	CHook::RET("_ZN6CPlane27DoPlaneGenerationAndRemovalEv"); // CPlane::DoPlaneGenerationAndRemoval

	CHook::RET("_ZN10CEntryExit19GenerateAmbientPedsERK7CVector"); // CEntryExit::GenerateAmbientPeds
	CHook::RET("_ZN8CCarCtrl31GenerateOneEmergencyServicesCarEj7CVector"); // CCarCtrl::GenerateOneEmergencyServicesCar
	CHook::RET("_ZN11CPopulation17AddPedAtAttractorEiP9C2dEffect7CVectorP7CEntityi"); // CPopulation::AddPedAtAttractor crash. wtf stuff?

	CHook::RET("_ZN7CDarkel26RegisterCarBlownUpByPlayerEP8CVehiclei"); // CDarkel__RegisterCarBlownUpByPlayer_hook
	CHook::RET("_ZN7CDarkel25ResetModelsKilledByPlayerEi"); // CDarkel__ResetModelsKilledByPlayer_hook
	CHook::RET("_ZN7CDarkel25QueryModelsKilledByPlayerEii"); // CDarkel__QueryModelsKilledByPlayer_hook
	CHook::RET("_ZN7CDarkel27FindTotalPedsKilledByPlayerEi"); // CDarkel__FindTotalPedsKilledByPlayer_hook
	CHook::RET("_ZN7CDarkel20RegisterKillByPlayerEPK4CPed11eWeaponTypebi"); // CDarkel__RegisterKillByPlayer_hook

    // CHook::NOP(g_libGTASA + (VER_x32 ? 0x0046BE88 : 0x55774C), 1);	// CStreaming::ms_memoryAvailable = (int)v24 // DANGEROUS
#if VER_x32
	CHook::NOP(g_libGTASA + (VER_2_1 ? 0x0040BF26 : 0x3AC8B2), 2); 	// CMessages::AddBigMessage from CPlayerInfo::KillPlayer

	CHook::NOP(g_libGTASA + (VER_2_1 ? 0x004C5902 : 0x454A88), 2);  // CCamera::ClearPlayerWeaponMode from CPedSamp::ClearWeaponTarget
	//CHook::NOP(g_libGTASA + 0x2FEE76, 2);	// CGarages::RespraysAreFree = true in CRunningScript::ProcessCommands800To899
	CHook::NOP(g_libGTASA + (VER_2_1 ? 0x003F395E : 0x39840A), 2);	// CStreaming::Shutdown from CGame::Shutdown

	//	CHook::WriteMemory(g_libGTASA + 0x2C3868, "\x00\x20\x70\x47", 4); 					// CGameLogic::IsCoopGameGoingOn
	CHook::WriteMemory(g_libGTASA + 0x001D16EA, "\x4F\xF4\x00\x10\x4F\xF4\x80\x06", 8); 	// RenderQueue::RenderQueue
	CHook::WriteMemory(g_libGTASA + 0x001D193A, "\x4F\xF4\x00\x16", 4); 	// RenderQueue::RenderQueue
#endif
	CHook::RET("_ZN10CPlayerPed14AnnoyPlayerPedEb"); // CPedSamp::AnnoyPlayerPed
	CHook::RET("_ZN11CPopulation15AddToPopulationEffff");	// CPopulation::AddToPopulation

	CHook::RET("_ZN23CAEPedSpeechAudioEntity11AddSayEventEisjfhhh"); // CPed::Say

}
void ApplyInGamePatches()
{
	Log("Installing patches (ingame)..");

	/* ������������� ����� */
	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + (VER_x32 ? 0x0098D252 : 0xC1BF92)), 1, 100);
	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA + (VER_x32 ? 0x0098D2B8 : 0xC1BFF8)) = 100;

	// CPlayerPed::CPlayerPed task fix
#if VER_x32
	CHook::WriteMemory(g_libGTASA + 0x004C36E2, (uintptr_t)"\xE0", 1);
#else
    CHook::WriteMemory(g_libGTASA + 0x5C0BC4, (uintptr_t)"\x34\x00\x80\x52", 4);
#endif
	// radar draw blips
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x0043FE5A : 0x52522C), 2);
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x004409AE : 0x525E14), 2);

//	CHook::WriteMemory(g_libGTASA + 0x00341F84, (uintptr_t)"\x00\xF0\x21\xBE", 4);

	CHook::RET("_ZN4CPed31RemoveWeaponWhenEnteringVehicleEi"); // CPed::RemoveWeaponWhenEnteringVehicle

	// no vehicle audio processing
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x00553E96 : 0x674610), 2);
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x00561AC2 : 0x682C1C), 2);
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x0056BED4 : 0x68DD0C), 2);

	// Disable in-game radio
	CHook::RET("_ZN20CAERadioTrackManager7ServiceEi");

	// ����� � ����
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x2ABA08 : 0x36A6E8), 2); // ����� ������� �����
	CHook::NOP(g_libGTASA + (VER_x32 ? 0x2ABA14 : 0x36A6F8), 2); // ������ �������
    CHook::NOP(g_libGTASA + (VER_x32 ? 0x2AB4A6 : 0x36A190), 2); // �������� ���������
}