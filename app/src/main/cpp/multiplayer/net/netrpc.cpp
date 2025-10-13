#include <sstream>
#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"
#include "../CSettings.h"
#include "../util/CJavaWrapper.h"
#include "java_systems/HUD.h"
#include "java_systems/ObjectEditor.h"
#include "../game/Entity/Ped/Ped.h"
#include "java_systems/GameFilesCheck.h"

extern CNetGame *pNetGame;

int iNetModeNormalOnfootSendRate	= NETMODE_ONFOOT_SENDRATE;
int iNetModeNormalInCarSendRate		= NETMODE_INCAR_SENDRATE;
int iNetModeFiringSendRate			= NETMODE_FIRING_SENDRATE;
int iNetModeSendMultiplier 			= NETMODE_SEND_MULTIPLIER;

void EditObject(RPCParameters *rpcParams) {
	auto Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	bool isPlayerObject;
	uint16_t Id;

	bsData.Read(isPlayerObject);
	bsData.Read(Id);

	CObjectEditor::Start(Id, isPlayerObject);
    LOGRPC("RPC: EditObject %d", Id);
}

void EditAttachedObject(RPCParameters *rpcParams) {
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	uint32_t index;

	bsData.Read(index);

	CObjectEditor::Start(index);

    LOGRPC("RPC: EditAttachedObject %d", index);
}
void InitGame(RPCParameters *rpcParams)
{
    LOGRPC("RPC: InitGame");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID MyPlayerID;
	bool bLanMode, bStuntBonus;

	RakNet::BitStream bsInitGame(Data,(iBitLength/8)+1,false);

	bsInitGame.ReadCompressed(pNetGame->m_bZoneNames);							// unknown
	bsInitGame.ReadCompressed(pNetGame->m_bUseCJWalk);							// native UsePlayerPedAnims(); +
	bsInitGame.ReadCompressed(pNetGame->m_bAllowWeapons);						// native AllowInteriorWeapons(allow); +
	bsInitGame.ReadCompressed(pNetGame->m_bLimitGlobalChatRadius);				// native LimitGlobalChatRadius(Float:chat_radius); +
	bsInitGame.Read(pNetGame->m_fGlobalChatRadius);								// +
	bsInitGame.ReadCompressed(bStuntBonus);										// native EnableStuntBonusForAll(enable); +
	bsInitGame.Read(pNetGame->m_fNameTagDrawDistance);							// native SetNameTagDrawDistance(Float:distance); +
	bsInitGame.ReadCompressed(pNetGame->m_bDisableEnterExits);					// native DisableInteriorEnterExits(); +
	bsInitGame.ReadCompressed(pNetGame->m_bNameTagLOS);							// native DisableNameTagLOS(); +
	bsInitGame.ReadCompressed(pNetGame->m_bManualVehicleEngineAndLight);		// native ManualVehicleEngineAndLights(); +
	bsInitGame.Read(pNetGame->m_iSpawnsAvailable);								// +
	bsInitGame.Read(MyPlayerID);												// 
	bsInitGame.ReadCompressed(pNetGame->m_bShowPlayerTags);						// native ShowNameTags(show); +
	bsInitGame.Read(pNetGame->m_iShowPlayerMarkers);							// native ShowPlayerMarkers(mode); +
	bsInitGame.Read(pNetGame->m_byteWorldTime);									// native SetWorldTime(hour); +
	bsInitGame.Read(pNetGame->m_byteWeather);									// native SetWeather(weatherid); +
	bsInitGame.Read(pNetGame->m_fGravity);										// native SetGravity(Float:gravity); +
	bsInitGame.ReadCompressed(bLanMode);										// 
	bsInitGame.Read(pNetGame->m_iDeathDropMoney);								// native SetDeathDropAmount(amount); +
	bsInitGame.ReadCompressed(pNetGame->m_bInstagib);							// always 0

	bsInitGame.Read(iNetModeNormalOnfootSendRate);
	bsInitGame.Read(iNetModeNormalInCarSendRate);
	bsInitGame.Read(iNetModeFiringSendRate);
	bsInitGame.Read(iNetModeSendMultiplier);

	bsInitGame.Read(pNetGame->m_iLagCompensation);								// lagcomp +

	uint8_t byteStrLen;
	bsInitGame.Read(byteStrLen);
	if(byteStrLen)																// SetGameModeText(); +
	{
		memset(pNetGame->m_szHostName, 0, sizeof(pNetGame->m_szHostName));
		bsInitGame.Read(pNetGame->m_szHostName, byteStrLen);
	}
	pNetGame->m_szHostName[byteStrLen] = '\0';

	uint8_t byteVehicleModels[212];
	bsInitGame.Read((char*)&byteVehicleModels[0], 212);							// don't use?
	bsInitGame.Read(pNetGame->m_iVehicleFriendlyFire);							// native EnableVehicleFriendlyFire(); +

	CGame::SetGravity(pNetGame->m_fGravity);

	if(pNetGame->m_bDisableEnterExits)
		CGame::DisableInteriorEnterExits();

	pNetGame->SetGameState(eNetworkState::CONNECTED);

	CGame::SetWorldWeather(pNetGame->m_byteWeather);
	CGame::SetWorldTime(pNetGame->m_byteWorldTime, 0);
	CGame::ToggleCJWalk(pNetGame->m_bUseCJWalk);
	CPlayerPool::SetLocalPlayerID(MyPlayerID);

	CGameFilesCheck::RequestChecked();
}

void ServerJoin(RPCParameters *rpcParams)
{
    LOGRPC("RPC: ServerJoin");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szPlayerName[MAX_PLAYER_NAME+1];
	PLAYERID playerId;
	unsigned char byteNameLen = 0;
	uint8_t bIsNPC = 0;
	int pading;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(pading);
	bsData.Read(bIsNPC);
	bsData.Read(byteNameLen);
	bsData.Read(szPlayerName, byteNameLen);
	szPlayerName[byteNameLen] = '\0';

    CPlayerPool::New(playerId, szPlayerName, bIsNPC);
}

void ServerQuit(RPCParameters *rpcParams)
{
    LOGRPC("ServerQuit");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteReason;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(byteReason);

    CPlayerPool::Delete(playerId, byteReason);
}

void ClientMessage(RPCParameters *rpcParams)
{
    LOGRPC("ClientMessage");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint32_t dwStrLen;
	uint32_t dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);
	char* szMsg = (char*)malloc(dwStrLen+1);
	bsData.Read(szMsg, dwStrLen);
	szMsg[dwStrLen] = 0;

	//std::string msg = szMsg;

	auto red = (dwColor >> 24) & 0xFF;
	auto green = (dwColor >> 16) & 0xFF;
	auto blue = (dwColor >> 8) & 0xFF;

	char buf[dwStrLen + 55];
	snprintf(buf, sizeof(buf), "{%02X%02X%02X}%s", red, green, blue, szMsg);

	CHUD::AddChatMessage(buf);

	free(szMsg);
}

void Chat(RPCParameters *rpcParams)
{
    LOGRPC("Chat");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	uint8_t byteTextLen;

	if(pNetGame->GetGameState() != eNetworkState::CONNECTED)	return;

	unsigned char szText[256];
	memset(szText, 0, 256);

	bsData.Read(playerId);
	bsData.Read(byteTextLen);
	bsData.Read((char*)szText,byteTextLen);

	szText[byteTextLen] = '\0';

	if (playerId == CPlayerPool::GetLocalPlayerID())
	{
        CChatWindow::AddChatMessage(CPlayerPool::GetLocalPlayerName(),
                                    CLocalPlayer::GetPlayerColor(), (char*)szText);
	}
	else
	{
		CRemotePlayer *pRemotePlayer = CPlayerPool::GetAt(playerId);
		if(pRemotePlayer)
			pRemotePlayer->Say(szText);
	}
}

void Weather(RPCParameters *rpcParams)
{
    LOGRPC("Weather");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteWeather;
	bsData.Read(byteWeather);

    pNetGame->m_byteWeather = byteWeather;
	CGame::SetWorldWeather(byteWeather);
}

void RequestClass(RPCParameters *rpcParams)
{
    unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
    uint8_t byteRequestOutcome = 0;
    PLAYER_SPAWN_INFO SpawnInfo;

    bsData.Read(byteRequestOutcome);
    bsData.Read((char*)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

    if (byteRequestOutcome) {
        CLocalPlayer::SetSpawnInfo(&SpawnInfo);
        CLocalPlayer::HandleClassSelectionOutcome(true);
    }
    else {
        CLocalPlayer::HandleClassSelectionOutcome(false);
    }
}

// TODO: RequestClass
// TODO: CLocalPlayer::m_bWaitingForSpawnRequestReply
void RequestSpawn(RPCParameters *rpcParams)
{
    unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
    uint8_t byteRequestOutcome = false;

    bsData.Read(byteRequestOutcome);

    if (byteRequestOutcome == 2 || (byteRequestOutcome && CLocalPlayer::m_bWaitingForSpawnRequestReply)) {
        CLocalPlayer::Spawn();
    }
    else {
        CLocalPlayer::m_bWaitingForSpawnRequestReply = false;
    }
}

void WorldTime(RPCParameters *rpcParams)
{
    LOGRPC("WorldTime");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteWorldTime;
	bsData.Read(byteWorldTime);
	CGame::SetWorldTime(byteWorldTime, 0);
}

void SetTimeEx(RPCParameters *rpcParams)
{
    LOGRPC("SetTimeEx");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteHour;
	uint8_t byteMinute;
	bsData.Read(byteHour);
	bsData.Read(byteMinute);

	CGame::SetWorldTime(byteHour, byteMinute);
}

void WorldPlayerAdd(RPCParameters *rpcParams)
{
    LOGRPC("WorldPlayerAdd");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	PLAYERID playerId;
	uint8_t byteFightingStyle=4;
	uint8_t byteTeam=0;
    uint8_t iSkin = 0;
	CVector vecPos;
	float fRotation=0;
	uint32_t dwColor=0;
	uint8_t tagType;

	bsData.Read(playerId);
	bsData.Read(byteTeam);
	bsData.Read(iSkin);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fRotation);
	bsData.Read(dwColor);
	bsData.Read(byteFightingStyle);

	#if VER_LR
		bsData.Read(tagType);
	#endif
	//CChatWindow::AddMessage("WorldPlayerAdd. vip lvl = %d", vipLvl);
	//bsData.Read(bVisible);

    // TODO: block check
    /*const char* rawName = CPlayerPool::GetPlayerName(playerId);
    if (rawName == nullptr)
        return;

    std::string nameCopy = rawName;*/

	CRemotePlayer* pRemotePlayer = CPlayerPool::GetAt(playerId);
	if(pRemotePlayer)
		pRemotePlayer->Spawn(byteTeam,
							 iSkin,
							 &vecPos,
							 fRotation,
							 dwColor,
							 byteFightingStyle,
							 (CRemotePlayer::eTags)tagType
							 );
}

void WorldPlayerRemove(RPCParameters *rpcParams)
{
    LOGRPC("WorldPlayerRemove");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	bsData.Read(playerId);

	auto pPlayer = CPlayerPool::GetSpawnedPlayer(playerId);
	if (pPlayer)
	{
        pPlayer->Remove();
	}
}

void SetCheckpoint(RPCParameters *rpcParams)
{
    LOGRPC("SetCheckpoint");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	float fX, fY, fZ, fSize;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fSize);

	CVector pos, Extent;

	pos.x = fX;
	pos.y = fY;
	pos.z = fZ;
	Extent.x = fSize;
	Extent.y = fSize;
	Extent.z = fSize;

	CGame::SetCheckpointInformation(&pos, &Extent);
	CGame::CreateCheckPoint();
}

void DisableCheckpoint(RPCParameters *rpcParams)
{
    LOGRPC("DisableCheckpoint");
	CGame::DisableCheckpoint();
}

void SetRaceCheckpoint(RPCParameters *rpcParams)
{
    LOGRPC("SetRaceCheckpoint");
	unsigned char *Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	float fX, fY, fZ;
	uint8_t byteType;
	CVector pos, next;

	bsData.Read(byteType);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	pos.x = fX;
	pos.y = fY;
	pos.z = fZ;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	next.x = fX;
	next.y = fY;
	next.z = fZ;

	bsData.Read(fX);

	CGame::SetRaceCheckpointInformation(byteType, &pos, &next, fX);
}

void DisableRaceCheckpoint(RPCParameters *rpcParams)
{
    LOGRPC("DisableRaceCheckpoint");
	if(CGame::m_bRaceCheckpointsEnabled)
	{
		CGame::DisableRaceCheckpoint();
		CHUD::toggleGps(false);
	}
}
void WorldVehicleAdd(RPCParameters *rpcParams)
{
    LOGRPC("WorldVehicleAdd");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	NewVehiclePacket NewVehicle;
	bsData.Read((char *)&NewVehicle,sizeof(NewVehiclePacket));

	if(NewVehicle.iVehicleType < 400 || NewVehicle.iVehicleType > 611) return;
	if (!CVehiclePool::New(&NewVehicle))
		return;
	int iVehicle = CVehiclePool::FindGtaIDFromID(NewVehicle.VehicleID);
	if (iVehicle)
	{
		for (int i = 0; i < 14; i++)
		{
			int data = (int)NewVehicle.byteModSlots[i];
			uint32_t v = 0;

			if (data == 0)
				continue;

			data += 999;

			if(CStreaming::TryLoadModel(data))
				ScriptCommand(&add_car_component, iVehicle, data, &v);
		}
	}
}

void WorldVehicleRemove(RPCParameters *rpcParams)
{
    LOGRPC("WorldVehicleRemove");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPedSamp *pPlayerPed = CGame::FindPlayerPed();
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleID;

	bsData.Read(VehicleID);

	CVehiclePool::Delete(VehicleID);
}

void EnterVehicle(RPCParameters *rpcParams)
{
    LOGRPC("EnterVehicle");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	PLAYERID playerId = INVALID_PLAYER_ID;
	VEHICLEID VehicleID = 0;
	uint8_t bytePassenger = 0;

	bsData.Read(playerId);
	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	CRemotePlayer *pPlayer = CPlayerPool::GetSpawnedPlayer(playerId);
	if(pPlayer) {
		DLOG("RPC_EnterVehicle VehicleID = %d, playerId = %d,  bytePassenger = %d", VehicleID, playerId, bytePassenger);
		pPlayer->EnterVehicle(VehicleID, (bool) bytePassenger);
	}

}

void ExitVehicle(RPCParameters *rpcParams)
{
    LOGRPC("ExitVehicle");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	VEHICLEID VehicleID = 0;

	bsData.Read(playerId);
	bsData.Read(VehicleID);

	CRemotePlayer *pPlayer = CPlayerPool::GetSpawnedPlayer(playerId);
	if(pPlayer)
		pPlayer->ExitVehicle();
}

void GameModeRestart(RPCParameters *rpcParams)
{
    LOGRPC("GameModeRestart");
    CChatWindow::AddMessage("{ff0000}Идёт перезагрузка сервера...");
    pNetGame->SetGameState(eNetworkState::RESTARTING);
}

#define REJECT_REASON_BAD_VERSION   1
#define REJECT_REASON_BAD_NICKNAME  2
#define REJECT_REASON_BAD_MOD       3
#define REJECT_REASON_BAD_PLAYERID  4

void ConnectionRejected(RPCParameters *rpcParams)
{
    LOGRPC("ConnectionRejected");
    unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
    uint8_t byteRejectReason;

    bsData.Read(byteRejectReason);

    if(byteRejectReason == REJECT_REASON_BAD_VERSION)
        CChatWindow::AddMessage("{D35D5D}Отказ: неверная версия клиента. Обновите клиент до последней версии.");
    else if(byteRejectReason == REJECT_REASON_BAD_NICKNAME)
    {
        CChatWindow::AddMessage("{D35D5D}Отказ: недопустимый никнейм!");
        CChatWindow::AddMessage("{D35D5D}Ник должен быть длиной 5-20 символов.");
        CChatWindow::AddMessage("{D35D5D}Допустимые символы: A-Z a-z 0-9 [ ] и _");
    }
    else if(byteRejectReason == REJECT_REASON_BAD_MOD)
    {
        CChatWindow::AddMessage("{D35D5D}Отказ: обнаружены неподдерживаемые модификации.");
        CChatWindow::AddMessage("{D35D5D}Отключите сторонние моды и попробуйте снова.");
    }
    else if(byteRejectReason == REJECT_REASON_BAD_PLAYERID)
    {
        CChatWindow::AddMessage("{D35D5D}Отказ: неверный или занятый PlayerID.");
        CChatWindow::AddMessage("{D35D5D}Возможно, этот ID уже используется — попробуйте подключиться позже.");
    }

    pNetGame->GetRakClient()->Disconnect(500);
}

void Pickup(RPCParameters *rpcParams)
{
    LOGRPC("Pickup");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

    NEW_PICKUP_DATA Pickup{};
	int pickupId;

	bsData.Read(pickupId);
	bsData.Read((char*)&Pickup, sizeof (NEW_PICKUP_DATA));

	CPickupPool::New(pickupId, &Pickup);
}

void DestroyPickup(RPCParameters *rpcParams)
{
    LOGRPC("DestroyPickup");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	int iIndex;
	bsData.Read(iIndex);

    CPickupPool::Destroy(iIndex);
}

void Create3DTextLabel(RPCParameters *rpcParams)
{
    LOGRPC("Create3DTextLabel");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t LabelID;
	uint32_t color;
	CVector pos;
	float dist;
	uint8_t testLOS;
	PLAYERID PlayerID;
	VEHICLEID VehicleID;
	char szBuff[4096+1];

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	bsData.Read(LabelID);
	bsData.Read(color);
	bsData.Read(pos.x);
	bsData.Read(pos.y);
	bsData.Read(pos.z);
	bsData.Read(dist);
	bsData.Read(testLOS);
	bsData.Read(PlayerID);
	bsData.Read(VehicleID);

	stringCompressor->DecodeString(szBuff, 4096, &bsData);

	CText3DLabelsPool::CreateTextLabel((int)LabelID, szBuff, color, pos, dist, testLOS, PlayerID, VehicleID);
}

void Delete3DTextLabel(RPCParameters *rpcParams)
{
    LOGRPC("Delete3DTextLabel");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t LabelID;
	bsData.Read(LabelID);

    CText3DLabelsPool::Delete((int)LabelID);
}

void Update3DTextLabel(RPCParameters *rpcParams)
{
    LOGRPC("Update3DTextLabel");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t LabelID;
	bsData.Read(LabelID);


}
#define EVENT_TYPE_PAINTJOB			1
#define EVENT_TYPE_CARCOMPONENT		2
#define EVENT_TYPE_CARCOLOR			3
#define EVENT_ENTEREXIT_MODSHOP		4
void ProcessIncommingEvent(BYTE bytePlayerID, int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
    LOGRPC("ProcessIncommingEvent");
	uint32_t v;
	int iVehicleID;
	int iPaintJob;
	int iComponent;
	int iWait;
	CVehicleSamp* pVehicle;
	CRemotePlayer* pRemote;

	switch (iEventType) 
	{

	case EVENT_TYPE_CARCOMPONENT:
		Log("RPC: EVENT_TYPE_CARCOMPONENT");
		iVehicleID = CVehiclePool::FindGtaIDFromID(dwParam1);
		iComponent = (int)dwParam2;

		if(CStreaming::TryLoadModel(iComponent)) {
            CVehicle* pVeh = GamePool_Vehicle_GetAt(iVehicleID);
            if (pVeh)
                CHook::CallFunction<int32_t>(g_libGTASA + (VER_x32 ? 0x0058C66C + 1 : 0x6AFF4C), pVeh, iComponent);
        }
		break;

	/*case EVENT_ENTEREXIT_MODSHOP:
		if (bytePlayerID > MAX_PLAYERS) return;
		pVehicle = pVehiclePool->GetAt((BYTE)dwParam1);
		pRemote = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
		if (pRemote && pVehicle) 
		{
			//pVehicle->SetLockedState((int)dwParam2); // Results in a crash at 0x48CFC9 under certain conditions.
			pRemote->m_iIsInAModShop = (int)dwParam2;
		}
		break;*/
	}
}

void ScmEvent(RPCParameters* rpcParams)
{
    LOGRPC("ScmEvent");
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID bytePlayerID;
	int iEvent;
	uint32_t dwParam1, dwParam2, dwParam3;

	bsData.Read(bytePlayerID);
	bsData.Read(iEvent);
	bsData.Read(dwParam1);
	bsData.Read(dwParam2);
	bsData.Read(dwParam3);
	ProcessIncommingEvent(bytePlayerID, iEvent, dwParam1, dwParam2, dwParam3);
}

void RemoveBuilding(RPCParameters* rpcParams)
{
	//
}
#include "..//gui/gui.h"
#include "../playertags.h"
#include "java_systems/Tab.h"
#include "java_systems/Dialog.h"

void SetPlayerChatBubble(RPCParameters* rpcParams)
{
    LOGRPC("SetPlayerChatBubble");
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	//PLAYERID playerId;
	uint16_t playerId;
	//if (playerId < 0 || playerId > MAX_PLAYERS) return;
	uint32_t color;
	float drawDistance;
	uint32_t expireTime;
	uint8_t texLength;

	bsData.Read(playerId);
	bsData.Read(color);
	bsData.Read(drawDistance);
	bsData.Read(expireTime);
	bsData.Read(texLength);
	if (texLength >= 255) return;
	char pText[256];
	bsData.Read(pText, texLength);
	pText[texLength] = '\0';

	CPlayerTags::AddChatBubble(playerId, pText, color, drawDistance, expireTime);
}

void WorldPlayerDeath(RPCParameters* rpcParams)
{
    LOGRPC("WorldPlayerDeath");
//	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
//	int iBitLength = rpcParams->numberOfBitsOfData;
//
//	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
//	PLAYERID playerId;
//	bsData.Read(playerId);
//	if (pNetGame->GetPlayerPool())
//	{
//		CRemotePlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt(playerId);
//		if (pPlayer)
//		{
//			pPlayer->HandleDeath();
//		}
//	}
}

void VehicleDamage(RPCParameters* rpcParams)
{
    LOGRPC("VehicleDamage");
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	VEHICLEID vehId;
	bsData.Read(vehId);

	if (CVehiclePool::GetAt(vehId))
	{
		uint32_t dwPanelStatus, dwDoorStatus;
		uint8_t byteLightStatus, byteTireStatus;

		bsData.Read(dwPanelStatus);
		bsData.Read(dwDoorStatus);
		bsData.Read(byteLightStatus);
		bsData.Read(byteTireStatus);

		CVehicleSamp* pVehicle = CVehiclePool::GetAt(vehId);
		if (pVehicle)
			pVehicle->UpdateDamageStatus(dwPanelStatus, dwDoorStatus, byteLightStatus, byteTireStatus);
	}
}

void WorldActorAdd(RPCParameters* rpcParams)
{
    LOGRPC("WorldActorAdd");
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	uint32_t iSkinId;
	CVector vecPos;
	float fRotation;
	float fHealth;
	uint8 bInvulnerable;

#if VER_LR
	uint8 name_len;
	char name[255];
	float armour;
	uint32 weapon;
#endif

	bsData.Read(actorId);
	bsData.Read(iSkinId);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fRotation);
	bsData.Read(fHealth);
	bsData.Read(bInvulnerable);

#if VER_LR
	if(!bsData.Read(name_len))
		return;

	bsData.Read(name, name_len);
	name[name_len] = '\0';
	bsData.Read(armour);
	bsData.Read(weapon);

    CActorPool::Spawn(actorId, iSkinId, vecPos, fRotation, fHealth, bInvulnerable, name, armour, weapon);
#else
    CActorPool::Spawn(actorId, iSkinId, vecPos, fRotation, fHealth, bInvulnerable);
#endif


}

void WorldActorRemove(RPCParameters* rpcParams)
{
    LOGRPC("WorldActorRemove");
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;

	bsData.Read(actorId);

	CActorPool::Delete(actorId);
}

void SetActorHealth(RPCParameters* rpcParams)
{
    LOGRPC("SetActorHealth");
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	float fHealth;
	bsData.Read(actorId);
	bsData.Read(fHealth);

	auto pActor = CActorPool::GetAt(actorId);
	if (pActor && pActor->m_pPed)
	{
		pActor->m_pPed->m_fHealth = fHealth;
	}
}

void SetActorPos(RPCParameters* rpcParams)
{
    LOGRPC("SetActorPos");
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	CVector pos;
	bsData.Read(actorId);
	bsData.Read((char*)& pos, sizeof(CVector));

	if (CActorPool::GetAt(actorId))
	{
		CActorPool::GetAt(actorId)->m_pPed->Teleport(pos, false);
	}
}

void SetActorFacingAngle(RPCParameters* rpcParams)
{
    LOGRPC("SetActorFacingAngle");

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	float angle;
	bsData.Read(actorId);
	bsData.Read(angle);

	if (CActorPool::GetAt(actorId))
	{
		CActorPool::GetAt(actorId)->ForceTargetRotation(angle);
	}
}

void SetActorAnimation(RPCParameters* rpcParams)
{
    LOGRPC("SetActorAnimation");

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	PLAYERID actorId;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	char szAnimLib[256];
	char szAnimName[256];
	float fS;
	bool opt1, opt2, opt3, opt4;
	int opt5;

	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	bsData.Read(actorId);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fS);
	bsData.Read(opt1);
	bsData.Read(opt2);
	bsData.Read(opt3);
	bsData.Read(opt4);
	bsData.Read(opt5);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	if (CActorPool::GetAt(actorId))
	{
		CActorPool::GetAt(actorId)->ApplyAnimation(szAnimName, szAnimLib, fS, (int)opt1, (int)opt2, (int)opt3, (int)opt4, opt5);
	}
}

void ClearActorAnimations(RPCParameters* rpcParams)
{
    LOGRPC("ClearActorAnimations");
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	float angle;
	bsData.Read(actorId);

	if (CActorPool::GetAt(actorId))
	{
		RwMatrix mat;
		CActorPool::GetAt(actorId)->m_pPed->GetMatrix(&mat);

		CActorPool::GetAt(actorId)->m_pPed->Teleport(CVector(mat.pos), false);
	}
}

void UpdateScoresPingsIPs(RPCParameters* rpcParams)
{
    LOGRPC("UpdateScoresPingsIPs");
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID bytePlayerId;
	int iPlayerScore;
	uint32_t dwPlayerPing;

	for (PLAYERID i = 0; i < (iBitLength / 8) / 9; i++)
	{
		bsData.Read(bytePlayerId);
		bsData.Read(iPlayerScore);
		bsData.Read(dwPlayerPing);

		CPlayerPool::UpdateScore(bytePlayerId, iPlayerScore);
		CPlayerPool::UpdatePing(bytePlayerId, dwPlayerPing);
	}
	CTab::update();
}

void RegisterRPCs(RakClientInterface* pRakClient)
{
    LOGRPC("Registering RPC's..");
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditObject, EditObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditAttachedObject, EditAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_InitGame, InitGame);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ServerJoin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ServerQuit);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ClientMessage);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Chat, Chat);
    pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RequestClass);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RequestSpawn);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Weather, Weather);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldTime, WorldTime);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, SetTimeEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, WorldPlayerAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, WorldPlayerRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, SetCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, DisableCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, SetRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, DisableRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, WorldVehicleAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, WorldVehicleRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, EnterVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ExitVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, CDialog::rpcShowPlayerDialog);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_GameModeRestart, GameModeRestart);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ConnectionRejected);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Pickup, Pickup);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, DestroyPickup);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, Create3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDestroy3DTextLabel, Delete3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ScmEvent);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RemoveBuilding, RemoveBuilding);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetPlayerChatBubble, SetPlayerChatBubble);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, WorldPlayerDeath);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_VehicleDamage, VehicleDamage);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ShowActor, WorldActorAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_HideActor, WorldActorRemove);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorAnimation, SetActorAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, SetActorFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, SetActorPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, SetActorHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClearActorAnimations, ClearActorAnimations);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, UpdateScoresPingsIPs);
}

void UnRegisterRPCs(RakClientInterface* pRakClient)
{
    LOGRPC("UnRegistering RPC's..");
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerTeam);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_InitGame);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerJoin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerQuit);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClientMessage);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Chat);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Weather);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldTime);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetTimeEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDialogBox);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_GameModeRestart);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ConnectionRejected);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Pickup);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DestroyPickup);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDestroy3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScmEvent);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RemoveBuilding);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetPlayerChatBubble);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_VehicleDamage);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ShowActor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_HideActor);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorAnimation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClearActorAnimations);
}