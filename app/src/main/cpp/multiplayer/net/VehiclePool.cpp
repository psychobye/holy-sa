#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "game/Coronas.h"
#include "Speedometr.h"

void CVehiclePool::Init()
{
//	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++)
//	{
//		m_bVehicleSlotState[VehicleID] = false;
//		m_pVehicles[VehicleID] = nullptr;
//	}
}

void CVehiclePool::Free()
{
    auto ids = GetAllIds();
    for (auto& id : ids) {
        Delete(id);
    }
}

CVehicleMP* CVehiclePool::GetVehicleFromTrailer(CVehicleMP *pTrailer) {

    if (!pTrailer) return nullptr;

    for(auto &pair : CVehiclePool::list) {
        auto pVehicle = pair.second;
        if(reinterpret_cast<CVehicle *>(pVehicle->m_pVehicle->m_pTrailer) == pTrailer->m_pVehicle) {
            return pVehicle;
        }
    }

	return nullptr;
}

void CVehiclePool::Process()
{
	for(auto& pair : list) {
		auto pVehicle = pair.second;

        if (!pVehicle->m_pVehicle->IsAdded())
            continue;

        if (pVehicle->GetHealth() < 300.0f) {
			pVehicle->SetHealth(300.0f);
		}

		pVehicle->m_pVehicle->m_nVehicleFlags.bLightsOn = (pVehicle->m_bIsLightOn >= eLightsState::ON_NEAR);
		pVehicle->m_pVehicle->m_nVehicleFlags.bEngineOn = pVehicle->m_bIsEngineOn;

		pVehicle->ProcessDamage();
		pVehicle->ProcessStrobs();
		pVehicle->neon.Process();

        CSpeedometr::UpdateSpeed();
        CSpeedometr::UpdateInfo();

		if (pVehicle->IsDriverLocalPlayer())
			pVehicle->SetInvulnerable(false);
		else
			pVehicle->SetInvulnerable(true);

		pVehicle->ProcessMarkers();
	}

}
#include "..//game/CCustomPlateManager.h"
#include "chatwindow.h"

bool CVehiclePool::New(NewVehiclePacket *pNewVehicle) {
	auto vehicleId = pNewVehicle->VehicleID;

    if (GetAt(vehicleId)) {
        CChatWindow::DebugMessage("Warning: vehicle %u was not deleted", vehicleId);
        Delete(vehicleId);
    }

	CVehicleMP* pVeh;
    try {
        pVeh = new CVehicleMP(pNewVehicle->iVehicleType,
							pNewVehicle->vecPos.x,
							pNewVehicle->vecPos.y,
							pNewVehicle->vecPos.z,
							pNewVehicle->fRotation,
							pNewVehicle->byteAddSiren);

		list[vehicleId] = pVeh;

        entityToIdMap[pVeh->m_pVehicle] = vehicleId;
        rwObjectToIdMap[pVeh->m_pVehicle->m_pRwObject] = vehicleId;
    } catch (const std::exception &e) {
        CChatWindow::DebugMessage("Warning: vehicle %u not created", vehicleId);
        return false;
    }


	pVeh->SetHealth(pNewVehicle->fHealth);

    // interior
    if (pNewVehicle->byteInterior > 0)
		pVeh->m_pVehicle->SetInterior(pNewVehicle->byteInterior);

//    // damage status
//    if (pNewVehicle->dwPanelDamageStatus ||
//        pNewVehicle->dwDoorDamageStatus ||
//        pNewVehicle->byteLightDamageStatus) {
//        m_pVehicles[pNewVehicle->VehicleID]->UpdateDamageStatus(
//                pNewVehicle->dwPanelDamageStatus,
//                pNewVehicle->dwDoorDamageStatus,
//                pNewVehicle->byteLightDamageStatus, pNewVehicle->byteTireDamageStatus);
//    }

//    m_bIsActive[vehicleId] = true;
//    m_bDeathSended[vehicleId] = false;

    return true;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
    if (auto pVehicle = GetAt(VehicleID)) {
        const auto vehicle = pVehicle->m_pVehicle;
        entityToIdMap.erase(vehicle);
        rwObjectToIdMap.erase(vehicle->m_pRwObject);

        delete list[VehicleID];
        list.erase(VehicleID);
        return true;
    }
    return false;
}

VEHICLEID CVehiclePool::FindIDFromGtaPtr(CEntity *pGtaVehicle)
{
    if (!pGtaVehicle) return INVALID_VEHICLE_ID;
    return GetEntity(pGtaVehicle);
}

CVehicleMP *CVehiclePool::FindVehicle(CVehicle *pGtaVehicle)
{
    if (!pGtaVehicle) return nullptr;

    uint32_t vehicleId = GetEntity(pGtaVehicle);
    if (vehicleId != INVALID_VEHICLE_ID) {
        return GetAt(vehicleId);
    }
    return nullptr;
}

VEHICLEID CVehiclePool::FindIDFromRwObject(RwObject* pRWObject)
{
    if (!pRWObject) return INVALID_VEHICLE_ID;
    return GetRwObject(pRWObject);
}

int CVehiclePool::FindGtaIDFromID(VEHICLEID id)
{
    auto pVehicle = GetAt(id);
    if(pVehicle && pVehicle->m_pVehicle)
        return pVehicle->m_dwGTAId;

    return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance = 10000.0f;
	float fThisDistance = 0.0f;
	VEHICLEID ClosetSoFar = INVALID_VEHICLE_ID;

	for(auto &pair : list) {
		auto pVehicle = pair.second;

		fThisDistance = pVehicle->m_pVehicle->GetDistanceFromLocalPlayerPed();
		if(fThisDistance < fLeastDistance)
		{
			fLeastDistance = fThisDistance;
			ClosetSoFar = pair.first;
		}
	}

	return ClosetSoFar;
}

void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked)
{
	CVehicleMP *pVehicle = list[VehicleID];

	if(pVehicle)
	{
		pVehicle->m_byteObjectiveVehicle = byteObjective;

		pVehicle->SetDoorState(byteDoorsLocked);
	}
}