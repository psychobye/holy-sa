//
// Created on 04.05.2023.
//

#include "World.h"
#include "util/patch.h"
#include "game.h"

// inlined
CPlayerInfoGta& FindPlayerInfo(int32 playerId) {
    return CWorld::Players[playerId < 0 ? CWorld::PlayerInFocus : playerId];
}

// Returns player ped
// 0x56E210
CPlayerPed* FindPlayerPed(int32 playerId) {
    if(playerId == -1)
        playerId = CWorld::PlayerInFocus;

    return FindPlayerInfo(playerId).m_pPed;
}

bool PlayerIsEnteringCar()
{
    // FIXME: �� ��������. ������� ���������� ������ ��� ��� �� �������� ����. �� ��������, ����� ���� ��� ������� ������ ������� ��� ����� � ����?
//    auto pPed = pGame.FindPlayerPed()->m_pPed;
//    if ( pPed->GetTaskManager().CTaskManager::FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER) )
//        return true;
//
//    return pPed->GetTaskManager().CTaskManager::FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) != nullptr;
    return false;
}

// Returns player vehicle
// 0x56E0D0
CVehicle* FindPlayerVehicle(int32 playerId, bool bIncludeRemote) {
    CPlayerPed* player = FindPlayerPed(playerId);
    if (!player || !player->bInVehicle)
        return nullptr;

    return player->pVehicle;
}

CEntity* FindPlayerEntity(int32 playerId) {
    auto player = FindPlayerPed(playerId);
    if (!player)
        return nullptr;

    if (player->IsInVehicle())
        return player->pVehicle;

    return player;
}

CVector FindPlayerCentreOfWorld_NoInteriorShift(int32 playerId) {
    return CHook::CallFunction<CVector>("_Z29FindPlayerCentreOfWorldForMapi", playerId);
}