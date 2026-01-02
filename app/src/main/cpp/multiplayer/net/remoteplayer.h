#pragma once

#include "../vendor/raknet/rijndael.h"

#include "../game/common.h"
#include "../game/VehicleMP.h"
#include "../game/PedSamp.h"
#include "gui/gui.h"

#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						17
#define PLAYER_STATE_PASSENGER					18
#define PLAYER_STATE_DRIVER						19
#define PLAYER_STATE_WASTED						32
#define PLAYER_STATE_SPAWNED					33
#define PLAYER_STATE_EXIT_VEHICLE				4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_SPECTATING					9

#define UPDATE_TYPE_NONE		0
#define UPDATE_TYPE_ONFOOT		1
#define UPDATE_TYPE_INCAR		2
#define UPDATE_TYPE_PASSENGER	3

class CRemotePlayer
{
public:
    enum eTags : uint8
    {
        NONE = 0,
        SilverVip,
        GoldVip,
        PlatinumVip,
        Youtuber,
        Tester,
        Leader,
        Support,
        Admin,
        Developer
    };
	static inline std::string tagsName[] = {"", "VIP", "VIP", "VIP", "YouTube", "Tester", "Leader", "Support", "Admin", "DEV"};
	static inline ImColor tagsColors[] = {
			ImColor(0x0),
			ImColor(0xc2, 0x8b, 0x1e, 0xFF),
			ImColor(0xa1, 0xa1, 0xa1, 0xFF),
			ImColor(0x00, 0xd1, 0xea, 0xFF),
			ImColor(0xc4, 0x30, 0x2b, 0xFF),
			ImColor(0xfa, 0xf6, 0x8e, 0xFF),
			ImColor(0xf6, 0xb7, 0xc6, 0xFF),
			ImColor(0x0d, 0xaf, 0x6b, 0xFF),
			ImColor(0xac, 0x00, 0xf4, 0xFF),
			ImColor(0xb0, 0x2e, 0x61, 0xFF)
	};

    eTags				m_nTag { eTags::NONE };
    bool				m_bShowNameTag;
    bool				m_bKeyboardOpened{};
	uint8 				m_nKeyBoardStat{};
    float 				m_fCurrentHealth = 0;
    float 				m_fCurrentArmor = 0;
    VEHICLEID 			m_VehicleID;

private:
    CPedSamp			*m_pPlayerPed;
    CVehicleMP			*m_pCurrentVehicle;

    PLAYERID			m_PlayerID;

    bool				m_bIsNPC;
    bool				m_bIsAFK;
    uintptr 			m_dwMarkerID;
    uintptr 			m_dwGlobalMarkerID;
    uint8_t 			m_byteState;
    uint32_t			m_dwLastRecvTick;
    uint32_t			m_dwLastHeadUpdate;
    uint16_t			m_usPlayingAnimIdx;
    bool				m_bWasSettedFlag;

    ONFOOT_SYNC_DATA	m_ofSync;
    INCAR_SYNC_DATA		m_icSync;

    uint8_t 			m_byteWeaponShotID;

    CVector 			m_vecPosOffset;

    CVector				m_vecOnFootTargetPos;
    CVector 			m_vecOnFootTargetSpeed;

    CVector 			m_vecInCarTargetPos;
    CVector 			m_vecInCarTargetSpeed;
    CQuaternion			m_InCarQuaternion;

    uint8_t				m_byteSpecialAction;
    uint8_t				m_byteSeatID;

public:
	CRemotePlayer();
	~CRemotePlayer();

	void Process();
	uint32_t GetPlayerColorAsARGB();
	bool Spawn(uint8_t byteTeam, unsigned int iSkin, CVector *vecPos, float fRotation, uint32_t dwColor, uint8_t byteFightingStyle, eTags tag);
	void Remove();

	CPedSamp* GetPlayerPed() { return m_pPlayerPed; }

	void StateChange(uint8_t byteNewState, uint8_t byteOldState) const;
	void SetState(uint8_t byteState)
	{
		if(byteState != m_byteState)
		{
			StateChange(byteState, m_byteState);
			m_byteState = byteState;
		}
	}
	uint8_t GetState() { return m_byteState; };

	void StoreBulletSyncData(BULLET_SYNC* blSync);
	void SetID(PLAYERID playerId) { m_PlayerID = playerId; }
	PLAYERID GetID() { return m_PlayerID; }

	void SetNPC(bool bNPC) { m_bIsNPC = bNPC; }
	bool IsNPC() { return m_bIsNPC; }

	bool IsAFK() { return m_bIsAFK; }

	void SetPlayerColor(uint32_t dwColor);

	void Say(unsigned char* szText);

	void HandleDeath();
	void HandleAnimations();
	void UpdateAimFromSyncData(AIM_SYNC_DATA *paimSync);
	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA *psSync);
	void UpdateOnFootPositionAndSpeed(CVector *vecPos, CVector *vecMoveSpeed);
	static void StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync);
	void UpdateOnFootTargetPosition();
	void SlerpRotation();

	void UpdateInCarMatrixAndSpeed(RwMatrix* mat, CVector* pos, CVector* speed);
	void UpdateInCarTargetPosition();
	void UpdateVehicleRotation();
	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();

	uint32_t GetPlayerColor();
	void ShowGlobalMarker(short sX, short sY, short sZ);
	void HideGlobalMarker();
    void ProcessSpecialActions(BYTE byteSpecialAction);
};