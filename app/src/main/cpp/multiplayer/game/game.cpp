#include "../main.h"
#include "game.h"
#include "util/CJavaWrapper.h"
#include "net/netgame.h"
#include "java_systems/HUD.h"
#include "util/patch.h"
#include "game/Widgets/WidgetGta.h"
#include "game/Models/ModelInfo.h"
#include "TxdStore.h"
#include "VisibilityPlugins.h"
#include "Scene.h"
#include "Camera.h"
#include "app/app.h"
#include "Clouds.h"
#include "java_systems/SnapShotsWrapper.h"
#include "Pickups.h"
#include "CrossHair.h"
#include "tools/DebugModules.h"
#include "CLocalisation.h"
#include "CFileMgr.h"
#include "CPad.h"
#include "game/Pipelines/CustomBuilding/CustomBuildingRenderer.h"
#include "game/Fx/CarFXRenderer.h"
#include "game/Widgets/TouchInterface.h"
#include "GrassRenderer.h"
#include "Weather.h"
#include "Clock.h"
#include "CPlayerInfoGta.h"
#include "WaterLevel.h"
#include "Draw.h"
#include "Radar.h"
#include "Coronas.h"
#include "Shadows.h"
#include "WeaponEffects.h"
#include "Skidmarks.h"
#include "Glass.h"
#include "MovingThings.h"
#include "SpecialFX.h"
#include "WaterCannons.h"
#include "Animation/AnimManager.h"
#include "World.h"

void ApplyPatches();
void ApplyInGamePatches();
void InstallHooks();
void LoadSplashTexture();
void InitScripting();

uint16_t *szGameTextMessage;
bool bUsedPlayerSlots[PLAYER_PED_SLOTS];

void CGame::Init()
{
    for (bool & statusHud : aToggleStatusHUD) {
        statusHud = true;
    }
	m_pGamePlayer = nullptr;
    m_bInit = true;

	m_bClockEnabled = true;
	m_bCheckpointsEnabled = false;
	m_dwCheckpointMarker = NULL;

	m_bRaceCheckpointsEnabled = false;
	m_dwRaceCheckpointHandle = NULL;
	m_dwRaceCheckpointMarker = NULL;

	memset(&bUsedPlayerSlots[0], 0, PLAYER_PED_SLOTS);
}

void CGame::exitGame()
{
    Log("Exiting game...");

	bIsGameExiting = true;

	g_pJavaWrapper->ExitGame();
}

// 0.3.7
uint8_t CGame::FindFirstFreePlayerPedSlot()
{
	uint8_t x = 2;
	while(x != PLAYER_PED_SLOTS)
	{
		if(!bUsedPlayerSlots[x]) return x;
		x++;
	}

	return 0;
}

// 0.3.7
CPedSamp* CGame::NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker)
{
	uint8_t bytePlayerNum = FindFirstFreePlayerPedSlot();
	if(!bytePlayerNum) return nullptr;

	CPedSamp* pPlayerNew = new CPedSamp(bytePlayerNum, iSkin, fX, fY, fZ, fRotation);
	if(pPlayerNew && pPlayerNew->m_pPed) {
        bUsedPlayerSlots[bytePlayerNum] = true;
        CLocalPlayer::m_nPlayersInRange++;

        return pPlayerNew;
    }
	return nullptr;
}

// 0.3.7
void CGame::RemovePlayer(CPedSamp* pPlayer)
{
	if(pPlayer)
	{
		bUsedPlayerSlots[pPlayer->m_bytePlayerNumber] = false;
		CLocalPlayer::m_nPlayersInRange --;
		delete pPlayer;
	}
}

CObjectSamp *CGame::NewObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance)
{
	return new CObjectSamp(iModel, fPosX, fPosY, fPosZ, vecRot, fDrawDistance);
}

uintptr CGame::CreatePickup(int iModel, int iType, CVector* pos, uint32* unk)
{
	uintptr hnd;

	auto dwModelArray = CModelInfo::ms_modelInfoPtrs;
	if(dwModelArray[iModel] == nullptr)
		iModel = 18631; // вопросик
        
	ScriptCommand(&create_pickup, iModel, iType, pos->x, pos->y, pos->z, &hnd);

	int lol = 32 * (uint16_t)hnd;
	if(lol) lol /= 32;
	if(unk) *unk = lol;

	return hnd;
}

void CGame::InitInMenu()
{
	Log("CGame: InitInMenu");

	ApplyPatches();
	InstallHooks();

	GameAimSyncInit();
	LoadSplashTexture();

	szGameTextMessage = new uint16_t[1076];
}

void CGame::InitInGame()
{
	Log("CGame: InitInGame");
	ApplyInGamePatches();
	GameResetRadarColors();

	std::time_t currentTime = std::time(nullptr);

	std::tm* timeInfo = std::localtime(&currentTime);
	CGame::SetWorldTime(timeInfo->tm_hour, 0);
    CLocalPlayer::Init();
}

void CGame::ToggleHUDElement(int iID, bool bToggle)
{
	if (iID < 0 || iID >= HUD_MAX)
	{
		return;
	}
	aToggleStatusHUD[iID] = bToggle;
}

bool CGame::IsToggledHUDElement(int iID)
{
	if (iID < 0 || iID >= HUD_MAX)
	{
		return 1;
	}
	return aToggleStatusHUD[iID];
}

float CGame::FindGroundZForCoord(float x, float y, float z)
{
	static float fGroundZ;
	ScriptCommand(&get_ground_z, x, y, z, &fGroundZ);
	return fGroundZ;
}

// 0.3.7
void CGame::SetCheckpointInformation(CVector *pos, CVector *extent)
{
	memcpy(&m_vecCheckpointPos,pos,sizeof(CVector));
	memcpy(&m_vecCheckpointExtent,extent,sizeof(CVector));
}

// 0.3.7
void CGame::SetRaceCheckpointInformation(uint8_t byteType, CVector *pos, CVector *next, float fSize)
{
	memcpy(&m_vecRaceCheckpointPos,pos,sizeof(CVector));
	memcpy(&m_vecRaceCheckpointNext,next,sizeof(CVector));
	m_fRaceCheckpointSize = fSize;
	m_byteRaceType = byteType;

	MakeRaceCheckpoint();
}

void CGame::RaceCheckpointPicked() {
    if(!m_bRaceCheckpointsEnabled)
        return;

    RakNet::BitStream bs;
    bs.Write((uint8_t)  ID_CUSTOM_RPC);
    bs.Write((uint8_t)  RPC_PICKED_RACE_CP);

    pNetGame->GetRakClient()->Send(&bs, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

// 0.3.7
void CGame::MakeRaceCheckpoint()
{
	if(m_bRaceCheckpointsEnabled)
	{
		DisableRaceCheckpoint();
	}

	ScriptCommand(&create_racing_checkpoint, (int)m_byteRaceType,
				m_vecRaceCheckpointPos.x, m_vecRaceCheckpointPos.y, m_vecRaceCheckpointPos.z,
				m_vecRaceCheckpointNext.x, m_vecRaceCheckpointNext.y, m_vecRaceCheckpointNext.z,
				m_fRaceCheckpointSize, &m_dwRaceCheckpointHandle);

	m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.x,
													 m_vecRaceCheckpointPos.y, m_vecRaceCheckpointPos.z, 1005, 0);

	m_bRaceCheckpointsEnabled = true;
}

// 0.3.7
void CGame::DisableRaceCheckpoint()
{
	if (m_dwRaceCheckpointHandle != NULL)
	{
		ScriptCommand(&destroy_racing_checkpoint, m_dwRaceCheckpointHandle);
		m_dwRaceCheckpointHandle = NULL;
	}
	if(m_dwRaceCheckpointMarker != NULL)
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		m_dwRaceCheckpointMarker = NULL;
	}
	m_bRaceCheckpointsEnabled = false;
}
void CGame::DisableCheckpoint() {
	if(m_dwCheckpointMarker != NULL)
	{
		DisableMarker(m_dwCheckpointMarker);
		m_dwCheckpointMarker = NULL;
	}
	m_bCheckpointsEnabled = false;
}

void CGame::CreateCheckPoint()
{
	if(m_bCheckpointsEnabled)
	{
		DisableCheckpoint();
	}

	m_dwCheckpointMarker =
			CreateRadarMarkerIcon(0,
								  m_vecCheckpointPos.x,
								  m_vecCheckpointPos.y,
								  m_vecCheckpointPos.z, 1005, 0);

	m_bCheckpointsEnabled = true;
}

// 0.3.7
uintptr CGame::CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle)
{
	uintptr dwMarkerID = 0;

	if(iStyle == 1) 
		ScriptCommand(&create_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if(iStyle == 2) 
		ScriptCommand(&create_radar_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if(iStyle == 3) 
		ScriptCommand(&create_icon_marker_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else 
		ScriptCommand(&create_radar_marker_without_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);

	if(iMarkerType == 0)
	{
		if(iColor >= 1004)
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 3);
		}
		else
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 2);
		}
	}

	return dwMarkerID;
}

void CGame::SetWorldTime(int iHour, int iMinute) {
    CClock::ms_nGameClockHours = iHour;
    CClock::ms_nGameClockMinutes = iMinute;
}

void CGame::SetWorldWeather(unsigned char byteWeatherID) {
    CWeather::ForcedWeatherType = byteWeatherID;
    CWeather::OldWeatherType = byteWeatherID;
    CWeather::NewWeatherType = byteWeatherID;
}

void CGame::ToggleThePassingOfTime(bool bOnOff)
{

}

// 0.3.7
void CGame::EnableClock(bool bEnable)
{
	
}

// 0.3.7
void CGame::EnableZoneNames(bool bEnable)
{
	ScriptCommand(&enable_zone_names, bEnable);
}

void CGame::DisplayWidgets(bool bDisp)
{

}

// ��������
void CGame::PlaySound(int iSound, float fX, float fY, float fZ)
{
	ScriptCommand(&play_sound, fX, fY, fZ, iSound);
}

// 0.3.7
void CGame::RefreshStreamingAt(float x, float y)
{
	ScriptCommand(&refresh_streaming_at, x, y);
}

// 0.3.7
void CGame::DisableTrainTraffic()
{
	ScriptCommand(&enable_train_traffic,0);
}

// 0.3.7
void CGame::SetMaxStats()
{
    CHook::CallFunction<void>("_ZN6CCheat18VehicleSkillsCheatEv");
    CHook::CallFunction<void>("_ZN6CCheat17WeaponSkillsCheatEv");

	// CStats::SetStatValue nop
	CHook::RET("_ZN6CStats12SetStatValueEtf");
}

void CGame::SetWantedLevel(uint8_t byteLevel)
{
	CHUD::iWantedLevel = byteLevel;
	CHUD::UpdateWanted();
}

bool CGame::IsAnimationLoaded(const char szAnimFile[])
{
	return ScriptCommand(&is_animation_loaded, szAnimFile);
}
// 0.3.7
void CGame::DisplayGameText(char* szStr, int iTime, int iType)
{
	ScriptCommand(&text_clear_all);
	CFont::AsciiToGxtChar(szStr, szGameTextMessage);

	// CMessages::AddBigMesssage
	(( void (*)(uint16_t*, int, int))(g_libGTASA + (VER_x32 ? 0x0054C62C + 1 : 0x66C150)))(szGameTextMessage, iTime, iType);
}

// 0.3.7
void CGame::SetGravity(float fGravity)
{
#if VER_x32
	CHook::UnFuck(g_libGTASA + (VER_2_1 ? 0x003FE810 : 0x3A0B64));
	*(float*)(g_libGTASA + (VER_2_1 ? 0x003FE810 : 0x3A0B64)) = fGravity;
#endif
}

void CGame::ToggleCJWalk(bool bUseCJWalk)
{
#if !VER_x32
	CHook::NOP(g_libGTASA + 0x5C3970, 2);
#else
	CHook::NOP(g_libGTASA + 0x4C5F6A, 2);
#endif
}

void CGame::DisableMarker(uint32_t dwMarkerID)
{
	ScriptCommand(&disable_marker, dwMarkerID);
}

// 0.3.7

void CGame::AddToLocalMoney(int iAmmount)
{
	CHUD::iLocalMoney = iAmmount;

	CHUD::UpdateMoney();
}

// 0.3.7
void CGame::DisableInteriorEnterExits()
{
//#if VER_x32
//	uintptr_t addr = *(uintptr_t*)(g_libGTASA + (VER_2_1 ? 0x007A1E20 : 0x700120));
//	int count = *(uint32_t*)(addr+8);
//
//	addr = *(uintptr_t*)addr;
//
//	for(int i=0; i<count; i++)
//	{
//		*(uint16_t*)(addr+0x30) = 0;
//		addr += 0x3C;
//	}
//#endif
}

extern bool bFullMap;
void CGame::DrawGangZone(const CRect* rect, uint32_t dwColor)
{
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x00443C60 + 1 : 0x528EC4), rect, &dwColor, bFullMap);
}

void CGame::InitialiseOnceBeforeRW() {
    CMemoryMgr::Init();
    CHook::CallFunction<void>("_ZN14MobileSettings10InitializeEv"); // впадлу реверсить т.к. меню надо вообще удалить
    CLocalisation::Initialise();
    CFileMgr::Initialise();
    CdStreamInit(TOTAL_IMG_ARCHIVES); // mb use TOTAL_IMG_ARCHIVES?
    CPad::Initialise();
}

int CGame::Init2()
{
    // CWaterLevel::WaterLevelInitialise();
    CDraw::SetFOV(120.0f, false);
    // CDraw::ms_fLODDistance = 1140457472;
    // CHook::CallFunction<bool>("_ZN18CCustomCarPlateMgr10InitialiseEv");
    //CStreaming::LoadInitialPeds(v6);
    // AllRequestedModels = CStreaming::LoadAllRequestedModels(0);
    // AnimFiles = (CStreaming *)CAnimManager::LoadAnimFiles(AllRequestedModels);
    // CStreaming::LoadInitialWeapons(AnimFiles);
    CStreaming::LoadAllRequestedModels(0);
    /*CPed::Initialise(v12);
    CRadar::Initialise(v13);
    CRadar::LoadTextures(v14);
    CWeapon::InitialiseWeapons(Textures);*/
    // CWorld::PlayerInFocus = 0;
    // CCoronas::Init();
    // CShadows::Init();
    // CWeaponEffects::Init();
    // CSkidmarks::Init();
    // CGlass::Init();
    CHook::CallFunction<void>("_ZN11CTheScripts4InitEv");
    // CClock::Initialise((CClock *)(unsigned int)&stru_3E8);
    // CMovingThings::Init();
    CHook::CallFunction<void>("_ZN6CStats4InitEv");
    // CClouds::Init();
    // CSpecialFX::Init();
    // CWaterCannons::Init();
}

void CGame::InjectHooks() {
    CHook::Redirect("_ZN5CGame27InitialiseEssentialsAfterRWEv", &CGame::InitialiseEssentialsAfterRW);
    CHook::Redirect("_ZN5CGame22InitialiseOnceBeforeRWEv", &CGame::InitialiseOnceBeforeRW);
	CHook::Redirect("_ZN5CGame7ProcessEv", &CGame::Process);
    // CHook::Redirect("_ZN5CGame5Init2EPKc", &CGame::Init2);

	CHook::Write(g_libGTASA + (VER_x32 ? 0x00678C38 : 0x84F8A0), &CGame::currArea);

	CHook::Write(g_libGTASA + (VER_x32 ? 0x006796E8 : 0x850DF0), &CGame::m_pWorkingMatrix1);
	CHook::Write(g_libGTASA + (VER_x32 ? 0x00677B38 : 0x84D6A0), &CGame::m_pWorkingMatrix2);
}

bool CGame::InitialiseRenderWare() {
	DLOG("InitialiseRenderWare ..");

	CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

	CTxdStore::Initialise();
	CVisibilityPlugins::Initialise();

#if VER_SAMP
    TextureDatabaseRuntime::Load("samp", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("mobile", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("txd", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("gta3", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("gta_int", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("cutscene", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("player", false, TextureDatabaseFormat::DF_DXT);
    TextureDatabaseRuntime::Load("menu", false, TextureDatabaseFormat::DF_DXT);
#else
	TextureDatabaseRuntime::Load("samp", false, TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("gui", false, TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("mobile", false, TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("txd", false, TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("gta3", false, TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("gta_int", false, TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("menu", false, TextureDatabaseFormat::DF_DXT);

	TextureDatabaseRuntime* radar = TextureDatabaseRuntime::Load("radar", false, TextureDatabaseFormat::DF_ETC);
	TextureDatabaseRuntime::Register(radar);

	//skins
	TextureDatabaseRuntime* skins = TextureDatabaseRuntime::Load("skins", false, TextureDatabaseFormat::DF_ETC);
	TextureDatabaseRuntime::Register(skins);

	// cars
	TextureDatabaseRuntime* cars = TextureDatabaseRuntime::Load("cars", false, TextureDatabaseFormat::DF_ETC);
	TextureDatabaseRuntime::Register(cars);
#endif


	const auto camera = RwCameraCreate();
	if (!camera) {
		CameraDestroy(camera);
		return false;
	}

	const auto frame = RwFrameCreate();
	rwObjectHasFrameSetFrame(&camera->object.object, frame);
	camera->frameBuffer = RwRasterCreate(RsGlobal->maximumWidth, RsGlobal->maximumHeight, 0, rwRASTERTYPECAMERA);
	camera->zBuffer = RwRasterCreate(RsGlobal->maximumWidth, RsGlobal->maximumHeight, 0, rwRASTERTYPEZBUFFER);
	if (!camera->object.object.parent) {
		CameraDestroy(camera);
		return false;
	}
	Scene.m_pRwCamera = camera;
	TheCamera.Init();
	TheCamera.SetRwCamera(Scene.m_pRwCamera);
	RwCameraSetFarClipPlane(Scene.m_pRwCamera, 2000.0f);
	RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.9f);
	CameraSize(Scene.m_pRwCamera, nullptr, 0.7f, DEFAULT_ASPECT_RATIO);

	RwBBox bb;
	bb.sup = { 10'000.0f,  10'000.0f,  10'000.0f};
	bb.inf = {-10'000.0f, -10'000.0f, -10'000.0f};

	if (Scene.m_pRpWorld = RpWorldCreate(&bb); !Scene.m_pRpWorld) {
		CameraDestroy(Scene.m_pRwCamera);
		Scene.m_pRwCamera = nullptr;

		return false;
	}
	RpWorldAddCamera(Scene.m_pRpWorld, Scene.m_pRwCamera);
	LightsCreate(Scene.m_pRpWorld);
//	CreateDebugFont();
	CFont::Initialise();
	CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0046FF38 + 1 : 0x55C1C8)); // CHud::Initialise();
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005B1188 + 1 : 0x6D5970)); // CPlayerSkin::Initialise();
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005B28D4 + 1 : 0x6D6E30)); // CPostEffects::Initialise();
	CGame::m_pWorkingMatrix1 = RwMatrixCreate();
	CGame::m_pWorkingMatrix2 = RwMatrixCreate();

	return true;
}

void MainLoop();

void CGame::PostToMainThread(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(mtx);
    tasks.push(std::move(task));
}

void CGame::ProcessMainThreadTasks()
{
    if (tasks.empty())
        return;

    std::function<void()> task;
    {
        std::lock_guard<std::mutex> lock(mtx);

        task = std::move(tasks.front());
        tasks.pop();
    }
    task();
}

void CGame::Process() {
	if(bIsGameExiting)return;

	MainLoop();
    ProcessMainThreadTasks();

	uint32_t CurrentTimeInCycles;
	uint32_t v1; // r4
	uint32_t v2; // r5
	uint32_t v3; // r5

	//FIXME
	((void(*)())(g_libGTASA + (VER_x32 ? 0x003F8B50 + 1 : 0x4DB464)))(); // CPad::UpdatePads()
	((void(*)())(g_libGTASA + (VER_x32 ? 0x002B03F8 + 1 : 0x36F374)))(); // CTouchInterface::Clear()
	((void(*)())(g_libGTASA + (VER_x32 ? 0x0028C178 + 1 : 0x3467BC)))(); // CHID::Update()

//	CLoadMonitor::BeginFrame(&g_LoadMonitor);
	CurrentTimeInCycles = CTimer::GetCurrentTimeInCycles();
	v1 = CurrentTimeInCycles / CTimer::GetCyclesPerMillisecond();

	CStreaming::Update();

	v2 = CTimer::GetCurrentTimeInCycles();
	v3 = v2 / CTimer::GetCyclesPerMillisecond();

    //	CCutsceneMgr::Update();

	if ( !(CTimer::m_CodePause << 0x18) )
	{
		auto gMobileMenu = (uintptr_t *) (g_libGTASA + (VER_x32 ? 0x006E0074 : 0x8BE780));
		((void(*)(uintptr_t*))(g_libGTASA + (VER_x32 ? 0x0029A730 + 1 : 0x356A7C)))(gMobileMenu); // MobileMenu::Update
	}

	// CTheZones::Update()

	// CCover::Update()

	CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

//	auto p_tx = (CSimpleTransform *)&TheCamera + 0x14 + 0x30;
//	if ( !TheCamera.m_pMat )
//		p_tx = *TheCamera + 0x4;

	//CAudioZones::Update(0, p_tx->m_translate);

	*(int32_t*)(g_libGTASA + (VER_x32 ? 0x00A7D22C: 0xD217F8)) = 0; // CWindModifiers::Number

	if ( !CTimer::m_CodePause && !CTimer::m_UserPause )
	{
		CSprite2d::SetRecipNearClip();
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005C89F8 + 1 : 0x6ECF00)))(); // CSprite2d::InitPerFrame();
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005A8A74 + 1 : 0x6CC898)))(); // CFont::InitPerFrame()
        // CCheat::DoCheats();
        // CClock::Update()

		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005CC2E8 + 1 : 0x6F0BD8)))(); // CWeather::Update()
		((void(*)())(g_libGTASA + (VER_x32 ? 0x0032AED8 + 1 : 0x3F3AD8)))(); // CTheScripts::Process()
	    // CCollision::Update()

        // CPathFind::UpdateStreaming

        // CTrain::UpdateTrains();
        // CHeli::UpdateHelis();
        // CDarkel::Update()
		((void(*)())(g_libGTASA + (VER_x32 ? 0x005BE838 + 1 : 0x6E2F08)))(); // CSkidmarks::Update();
		((void(*)())(g_libGTASA + (VER_x32 ? 0x005AB4C8 + 1 : 0x6D032C)))(); // CGlass::Update()
        // CWanted::UpdateEachFrame();
        // CCreepingFire::Update();
		// CSetPieces::Update();

		auto gFireManager = (uintptr_t *) (g_libGTASA + (VER_x32 ? 0x00958800 : 0xBC12D8));
		((void (*)(uintptr_t *)) (g_libGTASA + (VER_x32 ? 0x003F1628 + 1 : 0x4D361C)))(gFireManager); // CFireManager::Update

		// FIXME: add if
		((void(*)(bool))(g_libGTASA + (VER_x32 ? 0x004CC380 + 1 : 0x5CB5E0)))(false); // CPopulation::Update нужно (

		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005DB8E8 + 1 : 0x700AF4)))(); // CWeapon::UpdateWeapons()
//		if ( !CCutsceneMgr::ms_running )
//			CTheCarGenerators::Process();
//		CCranes::UpdateCranes();
//		CClouds::Update();
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005A6720 + 1 : 0x6CA130)))(); // CMovingThings::Update();
		((void(*)())(g_libGTASA + (VER_x32 ? 0x005CBB20 + 1 : 0x6F04CC)))(); // CWaterCannons::Update()
//		CUserDisplay::Process();
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x00427744 + 1 : 0x50BE40)))(); // CWorld::Process()

//		CLoadMonitor::EndFrame(&g_LoadMonitor);

		if ( !CTimer::bSkipProcessThisFrame )
		{
			CPickups::Update();
//			CCarCtrl::PruneVehiclesOfInterest();
//			CGarages::Update();
// 			CEntryExitManager::Update();
//			CStuntJumpManager::Update();
			((void (*)()) (g_libGTASA + (VER_x32 ? 0x0059CFC0 + 1 : 0x6C13F0)))(); // CBirds::Update()
			((void (*)()) (g_libGTASA + (VER_x32 ? 0x005C03E4 + 1 : 0x6E4A7C)))(); // CSpecialFX::Update()
			// CRopes::Update();
		}
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005B28D8 + 1 : 0x6D6E34)))(); // CPostEffects::Update()
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x0041EF78 + 1 : 0x502ADC)))(); // CTimeCycle::Update() crash without
		// CPopCycle::Update()

        // CInterestingEvents::ScanForNearbyEntities

		((void (*)(CCamera*)) (g_libGTASA + (VER_x32 ? 0x003DC7D0 + 1 : 0x4BAB78)))(&TheCamera); // CCamera::Process()

		// CCullZones::Update() менты не могут найти?
		// CGameLogic::Update() // FIXME: TEST
		// CGangWars::Update();
        // CConversations::Update()
        // CPedToPlayerConversations::Update()
        // CBridge::Update()

		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005A3E40 + 1 : 0x6C75E4)))(); // CCoronas::DoSunAndMoon()
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005A22C8 + 1 : 0x6C5BE0)))(); // CCoronas::Update()
		((void (*)()) (g_libGTASA + (VER_x32 ? 0x005BD370 + 1 : 0x6E1BC4)))(); // CShadows::UpdatePermanentShadows()

		// CPlantMgr::Update

        CCustomBuildingRenderer::Update();
//		if ( v6 <= 3 )
//			CCarCtrl::GenerateRandomCars();
//		CRoadBlocks::GenerateRoadBlocks();
//		CCarCtrl::RemoveDistantCars();
//		CCarCtrl::RemoveCarsIfThePoolGetsFull();
		auto temp = TheCamera.m_pRwCamera;

		auto g_fx = *(uintptr_t *) (g_libGTASA + (VER_x32 ? 0x00820520 : 0xA062A8));
		((void (*)(uintptr_t*, RwCamera*, float )) (g_libGTASA + (VER_x32 ? 0x00363DE0 + 1 : 0x433F48)))(&g_fx, temp, CTimer::ms_fTimeStep / 50.0f); // Fx_c::Update

		auto g_breakMan = (uintptr_t *) (g_libGTASA + (VER_x32 ? 0x0099DD14 : 0xC31CF0));
		((void (*)(uintptr_t*, float )) (g_libGTASA + (VER_x32 ? 0x0045267C + 1 : 0x53AFDC)))(g_breakMan, CTimer::ms_fTimeStep); // BreakManager_c::Update

        // InteriorManager_c::Update(&g_interiorMan);
        // ProcObjectMan_c::Update

        // WaterCreatureManager_c::Update

		((void (*)()) (g_libGTASA + (VER_x32 ? 0x00596540 + 1 : 0x6BB3A8)))(); // CWaterLevel::PreRenderWater()
	}

//	CHeli::UpdateHelis();
//	CCheat::ProcessAllCheats();
	static bool once = false;
	if (!once)
	{
		DebugModules::CreateModules(GImGui);
		CCrossHair::Init();
		CCustomPlateManager::Initialise();
		CRenderTarget::Initialise();

		once = true;
		return;
	}
    CTextDrawPool::Draw();
	SnapShotsWrapper::Process();
}

bool CGame::CanSeeOutSideFromCurrArea() {
	return currArea == AREA_CODE_NORMAL_WORLD;
}

bool CGame::InitialiseEssentialsAfterRW() {
    auto TheText = (uintptr_t *) (g_libGTASA + (VER_x32 ? 0xA00768 : 0xCA0888));
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x54D834 + 1 : 0x66D5F4), TheText);

    if (!CCarFXRenderer::Initialise() || !CGrassRenderer::Initialise() || !CCustomBuildingRenderer::Initialise()) {
        return false;
    }

    CTimer::Initialise();
    CTouchInterface::LoadTouchControls();
   // CWidgetListShop::LoadFromFile();
    return true;
}
