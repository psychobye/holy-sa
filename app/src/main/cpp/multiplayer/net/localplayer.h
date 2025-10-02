#pragma once

#include "../game/common.h"
#include "game/Core/Quaternion.h"
#include "../game/PedSamp.h"
#include "game/Enums/eWeaponType.h"

// spectate
#define SPECTATE_TYPE_NONE				0
#define SPECTATE_TYPE_PLAYER			1
#define SPECTATE_TYPE_VEHICLE			2

// special action's
#define SPECIAL_ACTION_NONE				0
#define SPECIAL_ACTION_USEJETPACK		2
#define SPECIAL_ACTION_DANCE1			5
#define SPECIAL_ACTION_DANCE2			6
#define SPECIAL_ACTION_DANCE3			7
#define SPECIAL_ACTION_DANCE4			8
#define SPECIAL_ACTION_HANDSUP			10
#define SPECIAL_ACTION_USECELLPHONE		11
#define SPECIAL_ACTION_SITTING			12
#define SPECIAL_ACTION_STOPUSECELLPHONE 13
#define SPECIAL_ACTION_NIGHTVISION		14
#define SPECIAL_ACTION_THERMALVISION	15
// added in 0.3
#define SPECIAL_ACTION_DUCK 			1
#define SPECIAL_ACTION_ENTER_VEHICLE 	3
#define SPECIAL_ACTION_EXIT_VEHICLE 	4
#define SPECIAL_ACTION_DRINK_BEER		20
#define SPECIAL_ACTION_SMOKE_CIGGY		21
#define SPECIAL_ACTION_DRINK_WINE		22
#define SPECIAL_ACTION_DRINK_SPRUNK		23
#define SPECIAL_ACTION_PISSING			68
// added in 0.3e
#define SPECIAL_ACTION_CUFFED			24
// added in 0.3x
#define SPECIAL_ACTION_CARRY			25

#pragma pack(push, 1)
struct PLAYER_SPAWN_INFO
{
    uint8_t byteTeam;
    int iSkin;
    uint8_t unk;
    CVector vecPos;
    float fRotation;
    int iSpawnWeapons[3];
    int iSpawnWeaponsAmmo[3];
};
#pragma pack(pop)
VALIDATE_SIZE(PLAYER_SPAWN_INFO, 46);

#pragma pack(push, 1)
struct ONFOOT_SYNC_DATA
{
	uint16_t lrAnalog;				// +0
	uint16_t udAnalog;				// +2
	uint16_t wKeys;					// +4
	CVector vecPos;					// +6
	CQuaternion quat;				// +18
	uint8_t byteHealth;				// +34
	uint8_t byteArmour;				// +35
	uint8_t byteCurrentWeapon;		// +36
	uint8_t byteSpecialAction;		// +37
	CVector vecMoveSpeed;			// +38
	CVector vecSurfOffsets;			// +50
	uint16_t wSurfInfo;				// +62
	union {
		struct {
			uint16_t id;
			uint8_t  frameDelta;
			union {
				struct {
					bool    loop : 1;
					bool    lockX : 1;
					bool    lockY : 1;
					bool    freeze : 1;
					uint8_t time : 2;
					uint8_t _unused : 1;
					bool    regular : 1;
				};
				uint8_t value;
			} flags;
		} animation;
		struct {
			uint32_t  dwAnimation;
			//uint16_t  dwAnimationFlags;
		};
	};
};
#pragma pack(pop)
VALIDATE_SIZE(ONFOOT_SYNC_DATA, 68);


#pragma pack(push, 1)
struct TRAILER_SYNC_DATA
{
	VEHICLEID trailerID;
	CVector vecPos;
	CQuaternion quat;
	CVector vecMoveSpeed;
	CVector vecTurnSpeed;
};
#pragma pack(pop)
VALIDATE_SIZE(TRAILER_SYNC_DATA, 54);


#pragma pack(push, 1)
struct INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;			// +0
	uint16_t lrAnalog;				// +2
	uint16_t udAnalog;				// +4
	uint16_t wKeys;					// +6
	CQuaternion quat;				// +8
	CVector vecPos;					// +24
	CVector vecMoveSpeed;			// +36
	float fCarHealth;				// +48
	uint8_t bytePlayerHealth;		// +52
	uint8_t bytePlayerArmour;		// +53
	uint8_t byteCurrentWeapon;		// +54
	uint8_t byteSirenOn;			// +55
	uint8_t byteLandingGearState;	// +56
	VEHICLEID TrailerID;			// +57

	union
	{
		uint32_t HydraThrustAngle;
		float fTrainSpeed;
	};
};
#pragma pack(pop)
VALIDATE_SIZE(INCAR_SYNC_DATA, 63);


#pragma pack(push, 1)
struct PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;			// +0
	union {
		uint16_t DriveBySeatAdditionalKeyWeapon;
		struct
		{
			uint8_t byteSeatFlags : 2;
			uint8_t byteDriveBy : 2;
			uint8_t byteCurrentWeapon : 6;
			uint8_t AdditionalKey : 2;
		};
	};
	uint8_t bytePlayerHealth;		// +4
	uint8_t bytePlayerArmour;		// +5
	uint16_t lrAnalog;				// +6
	uint16_t udAnalog;				// +8
	uint16_t wKeys;					// +10
	CVector vecPos;					// +12
};
#pragma pack(pop)
VALIDATE_SIZE(PASSENGER_SYNC_DATA, 24);

#pragma pack(push, 1)
struct SPECTATOR_SYNC_DATA
{
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	CVector vecPos;
};
#pragma pack(pop)
VALIDATE_SIZE(SPECTATOR_SYNC_DATA, 18);

class CLocalPlayer
{
public:
    static void Init();

    static void ResetAllSyncAttributes();
    static bool Process();
    static void SendDeath();
    static void GoEnterVehicle(bool passenger);
    static uint32_t GetPlayerColor();
    static void SetPlayerColor(uint32_t dwColor);
    static void UpdateSurfing();
    static void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger);
    static void UpdateRemoteInterior(uint8_t byteInterior);
    static bool Spawn();
    static int GetOptimumOnFootSendRate();
    static int GetOptimumInCarSendRate();
    static void ProcessSpectating();
    static void ToggleSpectating(bool bToggle);
    static void SpectatePlayer(PLAYERID playerId);
    static void SpectateVehicle(VEHICLEID VehicleID);
    static bool IsSpectating() { return m_bIsSpectating; }
    static CPedSamp* GetPlayerPed() { return m_pPlayerPed; };

    static void SendSpawn();
    static void RequestClass(int iClass);
    static void RequestSpawn();
    static void ProcessClassSelection();
    static void SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawnInfo);
    static void HandleClassSelection();
    static void HandleClassSelectionOutcome(bool bOutcome);

    static void SendOnFootFullSyncData();
    static void SendInCarFullSyncData();
    static void SendTrailerSync(uint16 trailerId, CVehicle *trailer);
    static void SendPassengerFullSyncData();
    static void SendAimSyncData();
    static void SendStatsUpdate();
    static void CheckWeapons();
    static uint8_t GetSpecialAction();
    static uint32_t CalculateAimSendRate(uint16_t wKeys);
    static void MaybeSendExitVehicle();
    static void MaybeSendEnterVehicle();

public:
    static constexpr int                timeNoSendedDataWithoutAfk = 500;

    static inline CPedSamp*             m_pPlayerPed;
    static inline int 	                m_nPlayersInRange{};

    static inline bool m_bWaitingForSpawnRequestReply;

    static inline bool		            m_bIsSpectating;
    static inline uint8_t	            m_byteSpectateMode;
    static inline uint8_t	            m_byteSpectateType;
    static inline uint32_t	            m_SpectateID; // Vehicle or player id
    static inline bool		            m_bSpectateProcessed;

    static inline PLAYERID 	            lastDamageId;
    static inline eWeaponType 	        lastDamageWeap;
    static inline bool			        ammoUpdated{};
    static inline int	m_iSelectedClass;

private:
    static inline bool				    m_bDeathSended;
    static inline uint8_t				m_byteCurInterior;
    static inline bool				    m_bInRCMode;
    static inline bool				m_bClearedToSpawn;
    static inline bool				m_bHasSpawnInfo;
    static inline bool				m_bSpawnDialogShowed;

    static inline PLAYER_SPAWN_INFO	m_SpawnInfo;

    static inline uint32_t			    m_dwPassengerEnterExit;

    static inline uint32_t			    m_dwLastSendSpecTick;
    static inline uint32_t			    m_dwLastAimSendTick;
    static inline uint32_t			    m_dwLastStatsUpdateTick;

    static inline uint32_t 			    m_dwLastUpdateHudButtons;
};