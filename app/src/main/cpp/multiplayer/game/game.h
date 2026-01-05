#pragma once

#include <thread>
#include "rgba.h"
#include "common.h"
#include "pad.h"
#include "Quaternion.h"
#include "PedSamp.h"
#include "VehicleMP.h"
#include "ObjectSamp.h"
#include "font.h"
#include "scripting.h"
#include "radarcolors.h"
#include "util.h"
#include "sprite2d.h"
#include "RW/RenderWare.h"
#include "widget.h"
#include "widgetmanager.h"
#include "CHandlingDefault.h"
#include "CActorPed.h"
#include "CWeaponsOutFit.h"
#include "CFirstPersonCamera.h"
#include "Streaming.h"
#include "game/Textures/TextureDatabaseRuntime.h"

class CGame
{
public:
    static bool Initialise(const uint8_t* pDatFile);
    static bool InitialiseEssentialsAfterRW();
    static void InitialiseOnceBeforeRW();
	static bool InitialiseRenderWare();

    static void LoadingScreen(const char *pMsg, const char *pMsg2, const char *pSplashName);

    static void Process();
    static bool Init1(const uint8_t* pDatFile);
    static bool Init2(const uint8_t* pDatFile);
    static bool Init3(const uint8_t* pDatFile);

public:
	static void InjectHooks();

    static void Init();

	static int GetScreenWidth() { return RsGlobal->maximumWidth; };
    static int GetScreenHeight() { return RsGlobal->maximumHeight; };

	static void InitInMenu();
	static void InitInGame();

    static void HandleChangedHUDStatus();
    static bool IsToggledHUDElement(int iID);

    static void ToggleHUDElement(int iID, bool bToggle);

	// 0.3.7
    static CPedSamp* FindPlayerPed() { if(!m_pGamePlayer) m_pGamePlayer = new CPedSamp(); return m_pGamePlayer; }
	// 0.3.7
	static uint8_t FindFirstFreePlayerPedSlot();
	// 0.3.7
	static CPedSamp* NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker = 0);
	// 0.3.7
	static void RemovePlayer(CPedSamp* pPlayer);
	// 0.3.7
	static CObjectSamp* NewObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistastatic);
	static uintptr CreatePickup(int iModel, int iType, CVector* pos, uint32* unk);

	static float FindGroundZForCoord(float x, float y, float z);
	// 0.3.7
	static void SetWorldTime(int iHour, int iMinute);
	// 0.3.7
    static void SetWorldWeather(unsigned char byteWeatherID);
	// 0.3.7
    static void EnableClock(bool bEnable);
	static void ToggleThePassingOfTime(bool bOnOff);
	// 0.3.7
    static void EnableZoneNames(bool bEnable);
    static void DisplayWidgets(bool bDisp);
    static void PlaySound(int iSound, float fX, float fY, float fZ);

	// 0.3.7
    static void ToggleRaceCheckpoints(bool bEnabled) { m_bRaceCheckpointsEnabled = bEnabled; }
	// 0.3.7
    static void SetCheckpointInformation(CVector *pos, CVector *extent);
	// 0.3.7
    static void SetRaceCheckpointInformation(uint8_t byteType, CVector *pos, CVector *next, float fSize);
	// 0.3.7
    static void MakeRaceCheckpoint();
	//
    static void DisableCheckpoint();
	// 0.3.7
	static void DisableRaceCheckpoint();
	// 0.3.7
	static uintptr CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle);
	// 0.3.7
	static void DisableMarker(uint32_t dwMarkerID);
	// 0.3.7
	static void RefreshStreamingAt(float x, float y);
	// 0.3.7
	static void DisableTrainTraffic();
	// 0.3.7
	static void SetMaxStats();

	static void SetWantedLevel(uint8_t byteLevel);
	static uint8_t GetWantedLevel();

	static bool IsAnimationLoaded(const char szAnimFile[]);

	static void RequestAnimation(const char szAnimFile[]) {
		ScriptCommand(&request_animation, szAnimFile);
	}
	// 0.3.7
	static void DisplayGameText(char* szStr, int iTime, int iType);
	// 0.3.7
	static void SetGravity(float fGravity);
	static void ToggleCJWalk(bool bUseCJWalk);
	// 0.3.7
	static void DisableInteriorEnterExits();
	static bool CanSeeOutSideFromCurrArea();

	// 0.3.7
	static void AddToLocalMoney(int iAmmount);

	static void DrawGangZone(const CRect* rect, uint32_t dwColor);

	// checkpoint
    static inline bool			    m_bCheckpointsEnabled;

    static void CreateCheckPoint();

    static inline uint32_t		    m_dwCheckpointMarker;
    static inline CVector			m_vecCheckpointPos;
    static inline CVector			m_vecCheckpointExtent;
    static inline bool              isRegistrationActive = false;
    static inline bool              isBanJump = false;
	static inline bool              bIsGameExiting = false;

    static inline bool 			    m_bDl_enabled = false;

    static void exitGame();

	inline static int32 currArea;
    inline static RwMatrix* m_pWorkingMatrix1;
    inline static RwMatrix* m_pWorkingMatrix2;

    static bool GetGameInit() {
        return m_bInit;
    }

private:
    static inline bool              m_bInit = false;
    static inline bool              aToggleStatusHUD[HUD_MAX];
    static inline CPedSamp*		    m_pGamePlayer;

public:
    static void PostToMainThread(std::function<void()> task);
    static void ProcessMainThreadTasks();
    static inline std::queue<std::function<void()>> tasks;
    static inline std::mutex mtx;

	// race checkpoint
	static inline bool		m_bRaceCheckpointsEnabled{};
	static inline float		m_fRaceCheckpointSize{};
	static inline uint8_t	m_byteRaceType{};
	static inline CVector 	m_vecRaceCheckpointPos{};
	static inline CVector 	m_vecRaceCheckpointNext{};
	static inline uint32_t	m_dwRaceCheckpointHandle{};
	static inline uint32_t	m_dwRaceCheckpointMarker{};

	static void RaceCheckpointPicked();

    static inline bool 		m_bClockEnabled;
};