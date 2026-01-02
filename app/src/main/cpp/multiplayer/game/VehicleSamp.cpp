#include "../main.h"
#include "game.h"

#include "..//CDebugInfo.h"
#include "..//net/netgame.h"
#include "game/Core/Vector.h"
#include "game/Models/VehicleModelInfo.h"
#include "game/Models/ModelInfo.h"
#include "game/Entity/Ped/Ped.h"
#include "chatwindow.h"
#include "util/patch.h"
#include "game/Enums/eCarNodes.h"
#include "DamageManager.h"
#include "VehicleSamp.h"
#include "game/Shadow/Shadows.h"
#include "Entity/Vehicle/Automobile.h"
#include "util/TextRasterizer/TextRasterizer.h"
#include "CHandlingDataMgr.h"
#include "Entity/Vehicle/Bike.h"
#include "Coronas.h"

CVehicleSamp::CVehicleSamp(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bSiren)
{
    fPosZ += 0.25f;

    m_pDamageManager = nullptr;
	m_pVehicle = nullptr;
	m_dwGTAId = 0;

	if ((iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM))
	{
		// normal vehicle
		if (!CStreaming::TryLoadModel(iType))
			throw std::runtime_error("Model not loaded");

		ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &m_dwGTAId);
		ScriptCommand(&set_car_z_angle, m_dwGTAId, fRotation);
		ScriptCommand(&car_gas_tank_explosion, m_dwGTAId, 0);
		ScriptCommand(&set_car_hydraulics, m_dwGTAId, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
		ScriptCommand(&set_car_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
		m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

        auto* automobile = reinterpret_cast<CAutomobile*>(m_pVehicle);
        m_pDamageManager = &automobile->m_damageManager;

		if (m_pVehicle)
		{
			//m_pVehicle->m_nOverrideLights = eVehicleOverrideLightsState::NO_CAR_LIGHT_OVERRIDE;
			m_pVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
			m_pVehicle->fHealth = 1000.0;

            // motorcycles fall underground in interiors
            RwMatrix mat;
            m_pVehicle->GetMatrix(&mat);
            mat.pos.x = fPosX;
            mat.pos.y = fPosY;
            mat.pos.z = fPosZ;
            m_pVehicle->SetMatrix((CMatrix&)mat);
		}
	}

	uint8_t defComp = 0;
	BIT_SET(defComp, 0);
	for (int i = 0; i < E_CUSTOM_COMPONENTS::ccMax; i++)
	{
		if (i == E_CUSTOM_COMPONENTS::ccExtra)
		{
			uint16_t defComp_extra = 0;
			BIT_SET(defComp_extra, EXTRA_COMPONENT_BOOT);
			BIT_SET(defComp_extra, EXTRA_COMPONENT_BONNET);
			BIT_SET(defComp_extra, EXTRA_COMPONENT_DEFAULT_DOOR);
			BIT_SET(defComp_extra, EXTRA_COMPONENT_WHEEL);
			BIT_SET(defComp_extra, EXTRA_COMPONENT_BUMP_REAR);
			BIT_SET(defComp_extra, EXTRA_COMPONENT_BUMP_FRONT);
			SetComponentVisible(i, defComp_extra);
		}
		else
		{
			SetComponentVisible(i, (uint16_t)defComp);
		}
	}

	bHasSuspensionLines = false;
	m_pSuspensionLines = nullptr;

	m_fWheelOffsetX = 0.0f;
	m_fWheelOffsetY = 0.0f;
	m_bWasWheelOffsetProcessedX = true;
	m_bWasWheelOffsetProcessedY = true;
	m_uiLastProcessedWheelOffset = 0;

	auto pWheelLF = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_lf_dummy");
	auto pWheelRF = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_rf_dummy");
	auto pWheelRB = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_rb_dummy");
	auto pWheelLB = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_lb_dummy");

	if (pWheelLF && pWheelRF && pWheelRB && pWheelLB)
	{
		memcpy(&m_vInitialWheelMatrix[0], (const void*)&(pWheelLF->modelling), sizeof(RwMatrix));
		memcpy(&m_vInitialWheelMatrix[1], (const void*)&(pWheelRF->modelling), sizeof(RwMatrix));
		memcpy(&m_vInitialWheelMatrix[2], (const void*)&(pWheelRB->modelling), sizeof(RwMatrix));
		memcpy(&m_vInitialWheelMatrix[3], (const void*)&(pWheelLB->modelling), sizeof(RwMatrix));
	}

    SetHandlingData();
}

void CVehicleSamp::ChangeDummyColor(const char* dummy, RwRGBA color) {
    auto flashFrame = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, dummy);
    if (!flashFrame)
        return;

    auto firstAtomic = reinterpret_cast<RpAtomic*>(GetFirstObject(flashFrame));
    if (!firstAtomic)
        return;

    auto geometry = RpAtomicGetGeometry(firstAtomic);
    RpGeometryGetMaterial(geometry, 0)->color = color;
}


CVehicleSamp::~CVehicleSamp()
{
	if(!m_dwGTAId)return;

	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if(!m_pVehicle)return;

	auto modelId = m_pVehicle->m_nModelIndex;

	if(m_pVehicle->IsTrailer()){
		CVehicleSamp *tmpVeh = CVehiclePool::GetVehicleFromTrailer(this);
		if(tmpVeh)
		{
			ScriptCommand(&detach_trailer_from_cab, m_dwGTAId, tmpVeh->m_dwGTAId);
			tmpVeh->m_pTrailer = nullptr;
		}
	}

	if (m_dwMarkerID) {
		ScriptCommand(&disable_marker, m_dwMarkerID);
		m_dwMarkerID = 0;
	}
	RemoveEveryoneFromVehicle();

	if(m_pTrailer) {
		ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
		m_pTrailer = nullptr;
	}

	if (m_pVehicle->m_nModelIndex == TRAIN_PASSENGER_LOCO ||
		m_pVehicle->m_nModelIndex == TRAIN_FREIGHT_LOCO) {
		ScriptCommand(&destroy_train, m_dwGTAId);
	}
	else {
		ScriptCommand(&destroy_car, m_dwGTAId);
	}

	if(pPlateTexture) {
		RwTextureDestroy(pPlateTexture);
		pPlateTexture = nullptr;
	}

	if(m_pVinylTex) {
		RwTextureDestroy(m_pVinylTex);
		m_pVinylTex = nullptr;
	}
	
	if (m_pCustomHandling) {
		delete m_pCustomHandling;
		m_pCustomHandling = nullptr;
	}

    if (m_pCustomBikeHandling) {
        delete m_pCustomBikeHandling;
        m_pCustomBikeHandling = nullptr;
    }

	if (bHasSuspensionLines && m_pSuspensionLines) {
		delete[] m_pSuspensionLines;
		m_pSuspensionLines = nullptr;
		bHasSuspensionLines = false;
	}

	//
	if(m_pLeftFrontTurnLighter != nullptr)
	{
		delete m_pLeftFrontTurnLighter;
		m_pLeftFrontTurnLighter = nullptr;
	}
	if(m_pLeftRearTurnLighter != nullptr)
	{
		delete m_pLeftRearTurnLighter;
		m_pLeftRearTurnLighter = nullptr;
	}
	if(m_pRightFrontTurnLighter != nullptr)
	{
		delete m_pRightFrontTurnLighter;
		m_pRightFrontTurnLighter = nullptr;
	}
	if(m_pRightRearTurnLighter != nullptr)
	{
		delete m_pRightRearTurnLighter;
		m_pRightRearTurnLighter = nullptr;
	}

	CStreaming::RemoveModelIfNoRefs(modelId);
}

void CVehicleSamp::RenderTurnLights()
{
    static bool blinkState = false;
    static uint32_t lastBlinkTime = 0;

    uint32_t now = CTimer::m_snTimeInMilliseconds;
    if (now - lastBlinkTime >= 501) {
        blinkState = !blinkState;
        lastBlinkTime = now;
    }

    if (!blinkState) return;

    switch (m_iTurnState) {
        case TURN_OFF: break;
        case TURN_LEFT:
            DrawTurnlight(false);
            break;
        case TURN_RIGHT:
            DrawTurnlight(true);
            break;
        case TURN_ALL:
            DrawTurnlight(true);
            DrawTurnlight(false);
            break;
    }
}

bool CVehicleSamp::DrawTurnlight(int isRight)
{
    auto modelInfo = CModelInfo::GetVehicleModelInfo(m_pVehicle->m_nModelIndex);
    if (!modelInfo || !modelInfo->m_pVehicleStruct)
        return false;

    CVector* dummies = modelInfo->m_pVehicleStruct->m_avDummyPos;
    if (!dummies)
        return false;

    const int indices[2] = { 0, 1 };

    const uint32_t baseId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(m_pVehicle));

    for (int i = 0; i < 2; ++i) {
        CVector v = dummies[indices[i]];
        if (!isRight)
            v.x = -v.x;

        uint32_t coronaId = baseId + (isRight ? 1u : 0u) + (i ? 2u : 0u);

        CCoronas::RegisterCorona(
                coronaId,
                m_pVehicle,
                255, 160, 0, 255,
                &v,
                0.5f,
                50.f,
                eCoronaType::CORONATYPE_HEADLIGHT,
                eCoronaFlareType::FLARETYPE_NONE,
                false,
                false,
                0,
                0.0f,
                false,
                0,
                0,
                15.0f,
                false,
                false
        );
    }

    return true;
}

VEHICLEID CVehicleSamp::getSampId()
{
	return CVehiclePool::FindIDFromGtaPtr(m_pVehicle);
}

void CVehicleSamp::AttachTrailer()
{
	if (m_pTrailer && GamePool_Vehicle_GetAt(m_pTrailer->m_dwGTAId) )
	{
		ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
	}
}

//-----------------------------------------------------------

void CVehicleSamp::DetachTrailer()
{
	if (m_pTrailer && GamePool_Vehicle_GetAt(m_pTrailer->m_dwGTAId))
	{
		ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
	}
	m_pTrailer = nullptr;
}

//-----------------------------------------------------------

void CVehicleSamp::SetTrailer(CVehicleSamp* pTrailer)
{
	m_pTrailer = pTrailer;
}

//-----------------------------------------------------------

void CVehicleSamp::SetHealth(float fHealth)
{
	if (m_pVehicle)
	{
		m_pVehicle->fHealth = fHealth;
	}
}

float CVehicleSamp::GetHealth()
{
	if (m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

// 0.3.7
void CVehicleSamp::SetInvulnerable(bool bInv)
{
	if (!m_pVehicle) return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;
	if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_2_1 ? 0x00667D14 : 0x5C7358)) return;

	if (bInv)
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 1, 1, 1, 1, 1);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
		m_bIsInvulnerable = true;
	}
	else
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 0, 0, 0, 0, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
		m_bIsInvulnerable = false;
	}
}

// 0.3.7
bool CVehicleSamp::IsDriverLocalPlayer()
{
	if (m_pVehicle)
	{
		if ((CPed*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
			return true;
	}

	return false;
}

bool IsValidGamePed(CPed* pPed);

void CVehicleSamp::RemoveEveryoneFromVehicle()
{
	DLOG("RemoveEveryoneFromVehicle");
	if (!m_pVehicle) return;
	if(!m_dwGTAId)return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	if (m_pVehicle->pDriver)
	{
		m_pVehicle->pDriver->RemoveFromVehicle();
	}

	for (int i = 0; i < 7; i++)
	{
		if (m_pVehicle->m_apPassengers[i] != nullptr)
		{
			m_pVehicle->m_apPassengers[i]->RemoveFromVehicle();
		}
	}
}

// 0.3.7
bool CVehicleSamp::IsOccupied()
{
	if (m_pVehicle)
	{
		if (m_pVehicle->pDriver) return true;
		if (m_pVehicle->m_apPassengers[0]) return true;
		if (m_pVehicle->m_apPassengers[1]) return true;
		if (m_pVehicle->m_apPassengers[2]) return true;
		if (m_pVehicle->m_apPassengers[3]) return true;
		if (m_pVehicle->m_apPassengers[4]) return true;
		if (m_pVehicle->m_apPassengers[5]) return true;
		if (m_pVehicle->m_apPassengers[6]) return true;
	}

	return false;
}

void CVehicleSamp::ProcessMarkers()
{
	if (!m_pVehicle) return;

	if (m_byteObjectiveVehicle)
	{
		if (!m_bSpecialMarkerEnabled)
		{
			if (m_dwMarkerID)
			{
				ScriptCommand(&disable_marker, m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}

		return;
	}

	if (!m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
	{
		if (m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_bSpecialMarkerEnabled = false;
			m_dwMarkerID = 0;
		}
	}

	if (m_pVehicle->GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied())
	{
		if (!m_dwMarkerID)
		{
			// show
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
		}
	}

	else if (IsOccupied() || m_pVehicle->GetDistanceFromLocalPlayerPed() >= 200.0f)
	{
		// remove
		if (m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}

void CVehicleSamp::SetDoorState(int iState) const
{
	if (!m_pVehicle) return;
	if (iState)
	{
		m_pVehicle->m_nDoorLock = CARLOCK_LOCKED;
	}
	else
	{
		m_pVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
	}
}

void CVehicleSamp::addComponent(uint16_t compId)  {
	if (!CStreaming::TryLoadModel(compId))
		return;

	m_pVehicle->AddVehicleUpgrade(compId);
	m_uiLastProcessedWheelOffset = GetTickCount();

//	SetHandlingData();
//
//	m_bWasWheelOffsetProcessedX = false;
//	m_bWasWheelOffsetProcessedY = false;
//	//m_uiLastProcessedWheelOffset = GetTickCount();
//	ProcessWheelsOffset();

}

void CVehicleSamp::RemoveComponent(uint16_t uiComponent) const
{

	int component = (uint16_t)uiComponent;

	if (!m_dwGTAId || !m_pVehicle)
	{
		return;
	}

	if (GamePool_Vehicle_GetAt(m_dwGTAId))
	{
		ScriptCommand(&remove_component, m_dwGTAId, component);
	}
}

void CVehicleSamp::SetComponentVisible(uint8_t group, uint16_t components)
{

	if (group == E_CUSTOM_COMPONENTS::ccExtra)
	{
		for (int i = 0; i < 16; i++)
		{
			std::string szName = GetComponentNameByIDs(group, i);
			SetComponentVisibleInternal(szName.c_str(), false);

			if (BIT_CHECK(components, i))
			{
				SetComponentVisibleInternal(szName.c_str(), true);
			}
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			std::string szName = GetComponentNameByIDs(group, i);

			SetComponentVisibleInternal(szName.c_str(), false);
			if (BIT_CHECK(components, i))
			{
				SetComponentVisibleInternal(szName.c_str(), true);
			}
		}
	}
}

void* GetSuspensionLinesFromModel(int nModelIndex, int& numWheels)
{
    auto pCollisionData = GetCollisionDataFromModel(nModelIndex);
    if (!pCollisionData) return nullptr;

    numWheels = pCollisionData->m_nNumLines;
    return pCollisionData->m_pLines;
}


CCollisionData* GetCollisionDataFromModel(int nModelIndex)
{
    auto dwModelarray = CModelInfo::ms_modelInfoPtrs;
    uint8_t* pModelInfoStart = (uint8_t*)dwModelarray[nModelIndex];

    if (!pModelInfoStart) return nullptr;

    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(nModelIndex);
    if (!modelInfo->m_pColModel) return nullptr;

    return modelInfo->m_pColModel->m_pColData;
}

void CVehicleSamp::SetBikeHandlingData() {
    const auto& mi = CModelInfo::GetVehicleModelInfo(m_pVehicle->m_nModelIndex);
    if (!mi) {
        return;
    }

    delete m_pCustomHandling;
    m_pCustomHandling = CHandlingDefault::GetCopyDefaultHandling(mi->m_nHandlingId);

    cHandlingDataMgr::ConvertDataToGameUnits(m_pCustomHandling);
    m_pVehicle->m_pHandlingData = m_pCustomHandling;

    delete m_pCustomBikeHandling;
    m_pCustomBikeHandling = CHandlingDefault::GetCopyBikeDefaultHandling(mi->m_nHandlingId);

    cHandlingDataMgr::ConvertBikeDataToGameUnits(m_pCustomBikeHandling);
    reinterpret_cast<CBike*>(m_pVehicle)->m_BikeHandling = m_pCustomBikeHandling;

    CopyGlobalSuspensionLinesToPrivate();

    CHook::CallFunction<void>("_ZN5CBike20SetupSuspensionLinesEv", reinterpret_cast<CBike*>(m_pVehicle));
}

void CVehicleSamp::SetHeliHandlingData() {
    const auto& mi = CModelInfo::GetVehicleModelInfo(m_pVehicle->m_nModelIndex);
    if (!mi) {
        return;
    }

    delete m_pCustomHandling;
    m_pCustomHandling = CHandlingDefault::GetCopyDefaultHandling(mi->m_nHandlingId);

    cHandlingDataMgr::ConvertDataToGameUnits(m_pCustomHandling);
    m_pVehicle->m_pHandlingData = m_pCustomHandling;

    m_pVehicle->pFlyingHandling = cHandlingDataMgr::GetFlyingPointer(mi->m_nHandlingId);

    CopyGlobalSuspensionLinesToPrivate();

    ((void (*)(CVehicle*))(g_libGTASA + (VER_x32 ? 0x0055F430 + 1 : 0x68036C)))(m_pVehicle);
}

void CVehicleSamp::SetVehicleHandlingData() {
    const auto& mi = CModelInfo::GetVehicleModelInfo(m_pVehicle->m_nModelIndex);
    if (!mi) {
        return;
    }

    delete m_pCustomHandling;
    m_pCustomHandling = CHandlingDefault::GetCopyDefaultHandling(mi->m_nHandlingId);

    for (auto& i : m_msLastSyncHandling)
    {
        if(i.fValue == 0.0f && i.flag != E_HANDLING_PARAMS::hpWheelSize)
            continue;

        switch (i.flag)
        {
            case E_HANDLING_PARAMS::hpMaxSpeed:
                m_pCustomHandling->m_transmissionData.m_fMaxGearVelocity = i.fValue * 0.84;
                break;
            case E_HANDLING_PARAMS::hpAcceleration: {
                m_pCustomHandling->m_transmissionData.m_fEngineAcceleration =  i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpEngineInertion:
                m_pCustomHandling->m_transmissionData.m_fEngineInertia = i.fValue;
                break;
            case E_HANDLING_PARAMS::hpGear:

                if (i.fValue == 1)
                {
                    m_pCustomHandling->m_transmissionData.m_nDriveType = 'R';
                }

                if (i.fValue == 2)
                {
                    m_pCustomHandling->m_transmissionData.m_nDriveType = 'F';
                }

                if (i.fValue == 3)
                {
                    m_pCustomHandling->m_transmissionData.m_nDriveType = '4';
                }

                break;
            case E_HANDLING_PARAMS::hpMass:
                m_pCustomHandling->m_fMass = i.fValue;
                break;
            case E_HANDLING_PARAMS::hpMassTurn:
                m_pCustomHandling->m_fTurnMass = i.fValue;
                break;
            case E_HANDLING_PARAMS::hpBrakeDeceleration:
            {
                m_pCustomHandling->m_fBrakeDeceleration = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpTractionMultiplier:
            {
                m_pCustomHandling->m_fTractionMultiplier = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpTractionLoss:
            {
                m_pCustomHandling->m_fTractionLoss = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpTractionBias:
            {
                m_pCustomHandling->m_fTractionBias = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpSuspensionLowerLimit:
            {
                m_pCustomHandling->m_fSuspensionLowerLimit = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpSuspensionBias:
            {
                m_pCustomHandling->m_fSuspensionBiasBetweenFrontAndRear = i.fValue;
                break;
            }
            case E_HANDLING_PARAMS::hpWheelSize:
            {
                m_fWheelSize = i.fValue;
                break;
            }
        }
    }

    auto fDefaultFrontWheelSize = mi->m_fWheelSizeFront;
    auto fDefaultRearWheelSize = mi->m_fWheelSizeRear;

    if(m_fWheelSize != 0.0f) {
        mi->m_fWheelSizeFront = m_fWheelSize;
        mi->m_fWheelSizeRear = m_fWheelSize;
    } else {
        m_fWheelSize = mi->m_fWheelSizeFront;
    }

    m_fDefaultWheelSize = std::max(fDefaultFrontWheelSize, fDefaultRearWheelSize);

    cHandlingDataMgr::ConvertDataToGameUnits(m_pCustomHandling);
    m_pVehicle->m_pHandlingData = m_pCustomHandling;

    ((void (*)(CVehicle*))(g_libGTASA + (VER_x32 ? 0x0054EC38 + 1 : 0x66EE5C)))(m_pVehicle); // CAutomobile::SetupSuspensionLines
    CopyGlobalSuspensionLinesToPrivate();

    mi->m_fWheelSizeFront = fDefaultFrontWheelSize;
    mi->m_fWheelSizeRear = fDefaultRearWheelSize;

    ((void (*)(CVehicle*))(g_libGTASA + (VER_x32 ? 0x0055F430 + 1 : 0x68036C)))(m_pVehicle);
}

void CVehicleSamp::SetHandlingData() {
    switch (GetVehicleSubtype()) {
        case VEHICLE_SUBTYPE_CAR:
            SetVehicleHandlingData();
            break;

        case VEHICLE_SUBTYPE_BIKE:
        case VEHICLE_SUBTYPE_PUSHBIKE:
            SetBikeHandlingData();
            break;

        case VEHICLE_SUBTYPE_HELI:
        case VEHICLE_SUBTYPE_PLANE:
            SetHeliHandlingData();
            break;
    }
}

void CVehicleSamp::setPlate(ePlateType type, std::string& szNumber, std::string& szRegion)
{
	if(pPlateTexture) {
		RwTextureDestroy(pPlateTexture);
		pPlateTexture = nullptr;
	}
	//pPlateTexture = CCustomPlateManager::createTexture(NUMBERPLATE_TYPE_RU_POLICE, "B330OP", "122");
	pPlateTexture = CCustomPlateManager::createTexture(type, szNumber, szRegion);
}

RwObject* GetAllAtomicObjectCB(RwObject* object, void* data)
{
	auto result = *((std::vector<RwObject*>*) data);
	result.push_back(object);
	return object;
}

// Get all atomics for this frame (even if they are invisible)
void GetAllAtomicObjects(RwFrame* frame, std::vector<RwObject*>& result)
{

	((uintptr_t(*)(RwFrame*, void*, uintptr_t))(g_libGTASA + (VER_x32 ? 0x001D8858 + 1 : 0x2703BC)))(frame, (void*)GetAllAtomicObjectCB, (uintptr_t)& result);
}

void CVehicleSamp::ProcessHeadlightsColor(uint8_t& r, uint8_t& g, uint8_t& b)
{
	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}
	if(m_iStrobsType != eStobsStatus::OFF) {
		r = lightColorStrob.r;
		g = lightColorStrob.g;
		b = lightColorStrob.b;
		return;
	}

	r = lightColor.r;
	g = lightColor.g;
	b = lightColor.b;
}

void CVehicleSamp::SetWheelAngle(bool isFront, float angle)
{
	if (!m_pVehicle || !m_dwGTAId)
	{
		return;
	}

	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}

	if (isFront)
	{
		m_fWheelAngleFront = DEGTORAD(angle);
	}
	else
	{
		m_fWheelAngleBack = DEGTORAD(angle);
	}
}

void CVehicleSamp::SetWheelOffset(bool isFront, float offset)
{
	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}

	if (isFront)
	{
		m_fWheelOffsetX = offset / 100.f;
		m_bWasWheelOffsetProcessedX = false;
	}
	else
	{
		m_fWheelOffsetY = offset / 100.f;
		m_bWasWheelOffsetProcessedY = false;
	}

	m_uiLastProcessedWheelOffset = GetTickCount();
}

void CVehicleSamp::SetWheelWidth(float fValue)
{
	m_fWheelWidth = fValue / 100.f;
}

void CVehicleSamp::ProcessWheelsOffset()
{
	if (GetTickCount() - m_uiLastProcessedWheelOffset <= 30)
	{
		return;
	}
	if(m_dwChangeWheelTo != -1) {
        if(m_dwChangeWheelTo == 0)
            m_pVehicle->RemoveVehicleUpgrade(m_dwCurrentWheelModel);
        else
		    addComponent(m_dwChangeWheelTo);

        m_dwCurrentWheelModel = m_dwChangeWheelTo;

		m_bWasWheelOffsetProcessedX = false;
		m_bWasWheelOffsetProcessedY = false;
		m_dwChangeWheelTo = -1;
	}
	if (!m_bWasWheelOffsetProcessedX)
	{
		auto pWheelLF = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_lf_dummy");
		auto pWheelRF = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_rf_dummy");

		ProcessWheelOffset(pWheelLF, true, m_fWheelOffsetX, 0);
		ProcessWheelOffset(pWheelRF, false, m_fWheelOffsetX, 1);

		m_bWasWheelOffsetProcessedX = true;
	}
	if (!m_bWasWheelOffsetProcessedY)
	{
		auto pWheelRB = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_rb_dummy");
		auto pWheelLB = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, "wheel_lb_dummy");

		ProcessWheelOffset(pWheelRB, false, m_fWheelOffsetY, 2);
		ProcessWheelOffset(pWheelLB, true, m_fWheelOffsetY, 3);
		
		m_bWasWheelOffsetProcessedY = true;
	}
}

void CVehicleSamp::ProcessWheelOffset(RwFrame* pFrame, bool bLeft, float fValue, int iID)
{
	CVector vecOffset;
	vecOffset.x = 0.0f - fValue;
	vecOffset.y = 0.0f;
	vecOffset.z = 0.0f;
	if (bLeft)
	{
		vecOffset.x *= -1.0f;
	}

	CVector vecOut;
	ProjectMatrix(&vecOut, (CMatrix*)&m_vInitialWheelMatrix[iID], &vecOffset);

	pFrame->modelling.pos = vecOut;
}

void CVehicleSamp::OpenDoor(eCarNodes index, eDoors doorId, bool state) {
    if(!m_pVehicle)
        return;

    if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
    {
        m_bDoorsState[doorId] = state;

        if(state)
            CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005507F4 + 1 : 0x670B10), m_pVehicle, 0, index, doorId, 1.0f, 1);
        else
            CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005507F4 + 1 : 0x670B10), m_pVehicle, 0, index, doorId, 0.0f, 1);
    }
}


void CVehicleSamp::SetComponentVisibleInternal(const char* szComponent, bool bVisible) const
{
    if (!m_pVehicle || !m_dwGTAId)
    {
        return;
    }

    if (!GamePool_Vehicle_GetAt(m_dwGTAId))
    {
        return;
    }

    if (!m_pVehicle->m_pRwObject)
    {
        return;
    }


    auto pFrame = CClumpModelInfo::GetFrameFromName(m_pVehicle->m_pRwClump, szComponent);
    if (pFrame != nullptr)
    {
        // Get all atomics for this component - Usually one, or two if there is a damaged version
        std::vector<RwObject*> atomicList;
        GetAllAtomicObjects(pFrame, atomicList);

        // Count number currently visible
        uint uiNumAtomicsCurrentlyVisible = 0;
        for (auto & i : atomicList)
        {
            if (!i)
            {
                continue;
            }
            if (i->flags & 0x04)
            {
                uiNumAtomicsCurrentlyVisible++;
            }
        }

        if (bVisible && uiNumAtomicsCurrentlyVisible == 0)
        {
            // Make atomic (undamaged version) visible. TODO - Check if damaged version should be made visible instead
            for (auto pAtomic : atomicList)
            {
                if (!pAtomic)
                {
                    continue;
                }
                int AtomicId = ((int(*)(RwObject*))(g_libGTASA + (VER_x32 ? 0x005D4B54 + 1 : 0x6F9D68)))(pAtomic); // CVisibilityPlugins::GetAtomicId

                if (!(AtomicId & ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED))
                {
                    // Either only one version, or two versions and this is the undamaged one
                    pAtomic->flags |= 0x04;
                }
            }
        }
        else if (!bVisible && uiNumAtomicsCurrentlyVisible > 0)
        {
            // Make all atomics invisible
            for (auto & i : atomicList)
            {
                if (!i)
                {
                    continue;
                }
                i->flags &= ~0x05;            // Mimic what GTA seems to do - Not sure what the bottom bit is for
            }
        }
    }
}


std::string CVehicleSamp::GetComponentNameByIDs(uint8_t group, int subgroup)
{

	if (group == E_CUSTOM_COMPONENTS::ccExtra && subgroup >= EXTRA_COMPONENT_BOOT)
	{
		switch (subgroup)
		{
			case EXTRA_COMPONENT_BOOT:
				return {"boot_dummy"};
			case EXTRA_COMPONENT_BONNET:
				return {"bonnet_dummy"};
			case EXTRA_COMPONENT_BUMP_REAR:
				return {"bump_rear_dummy"};
			case EXTRA_COMPONENT_DEFAULT_DOOR:
				return {"door_lf_dummy"};
			case EXTRA_COMPONENT_WHEEL:
				return {"wheel_lf_dummy"};
			case EXTRA_COMPONENT_BUMP_FRONT:
				return {"bump_front_dummy"};
		}
	}

	std::string retn;

	switch (group)
	{
		case E_CUSTOM_COMPONENTS::ccBumperF:
			retn += "bumberF_";
			break;
		case E_CUSTOM_COMPONENTS::ccBumperR:
			retn += "bumberR_";
			break;
		case E_CUSTOM_COMPONENTS::ccFenderF:
			retn += "fenderF_";
			break;
		case E_CUSTOM_COMPONENTS::ccFenderR:
			retn += "fenderR_";
			break;
		case E_CUSTOM_COMPONENTS::ccSpoiler:
			retn += "spoiler_";
			break;
		case E_CUSTOM_COMPONENTS::ccExhaust:
			retn += "exhaust_";
			break;
		case E_CUSTOM_COMPONENTS::ccRoof:
			retn += "roof_";
			break;
		case E_CUSTOM_COMPONENTS::ccTaillights:
			retn += "taillights_";
			break;
		case E_CUSTOM_COMPONENTS::ccHeadlights:
			retn += "headlights_";
			break;
		case E_CUSTOM_COMPONENTS::ccDiffuser:
			retn += "diffuser_";
			break;
		case E_CUSTOM_COMPONENTS::ccSplitter:
			retn += "splitter_";
			break;
		case E_CUSTOM_COMPONENTS::ccExtra:
			retn += "ext_";
			break;
		default:
			retn = std::string("err");
			break;
	}

	retn += ('0' + (char)subgroup);

	return retn;
}

void CVehicleSamp::CopyGlobalSuspensionLinesToPrivate()
{
	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR)
	{
		return;
	}

	if (!bHasSuspensionLines)
	{
		int numWheels;
		void* pOrigSuspension = GetSuspensionLinesFromModel(m_pVehicle->m_nModelIndex, numWheels);

		if (pOrigSuspension && numWheels)
		{
			bHasSuspensionLines = true;
			m_pSuspensionLines = static_cast<CColLine*>(CMemoryMgr::Malloc(numWheels * sizeof(CColLine)));
		}
	}

	int numWheels;
	void* pOrigSuspension = GetSuspensionLinesFromModel(m_pVehicle->m_nModelIndex, numWheels);

	if (pOrigSuspension && numWheels)
	{
		memcpy(m_pSuspensionLines, pOrigSuspension, numWheels * sizeof(CColLine));
	}
}

void CVehicleSamp::SetEngineState(bool bEnable) {
	if(!m_dwGTAId)return;
	if(!m_pVehicle)return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) {
		return;
	}

	m_bIsEngineOn = bEnable;
}

bool CVehicleSamp::HasDamageModel() const
{
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		return true;
	return false;
}

void CVehicleSamp::SetPanelStatus(ePanels bPanel, ePanelDamageState bPanelStatus) const
{
    if (m_pDamageManager->GetPanelStatus(bPanel) != bPanelStatus)
    {
        if (m_pDamageManager->GetPanelStatus(bPanel) != bPanelStatus)
        {
            m_pDamageManager->SetPanelStatus(bPanel, bPanelStatus);
            if (bPanelStatus == DAMSTATE_OK) {
                // Grab the car node index for the given panel
                static int s_iCarNodeIndexes[7] = { 0x0F, 0x0E, 0x00 /*?*/, 0x00 /*?*/, 0x12, 0x0C, 0x0D };
                int iCarNodeIndex = s_iCarNodeIndexes[bPanel];

                // CAutomobile::FixPanel
                CHook::CallFunction<void>("_ZN11CAutomobile8FixPanelEi7ePanels", m_pVehicle, iCarNodeIndex, bPanel);
            }
            else {
                CHook::CallFunction<void>("_ZN11CAutomobile14SetPanelDamageE7ePanelsb", m_pVehicle,
                                          bPanel, false);
            }
        }
    }
}

uint8_t CVehicleSamp::GetDoorStatus(eDoors bDoor) {
    if (m_pVehicle && bDoor < MAX_DOORS) {
        return m_pDamageManager->m_aDoorsStatus[bDoor];
    }
    return 0;
}

void CVehicleSamp::SetDoorStatus(eDoors bDoor, eDoorStatus bDoorStatus, bool spawnFlyingComponen)
{
    if (GetDoorStatus(bDoor) != bDoorStatus)
    {
        m_pDamageManager->SetDoorStatus(bDoor, bDoorStatus);
        if (bDoorStatus == DAMSTATE_OK || bDoorStatus == DAMSTATE_OPENED) {
            // Grab the car node index for the given door id
            static int s_iCarNodeIndexes[6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };
            int iCarNodeIndex = s_iCarNodeIndexes[bDoor];

            // CAutomobile::FixDoor
            CHook::CallFunction<void>("_ZN11CAutomobile7FixDoorEi6eDoors", m_pVehicle, iCarNodeIndex, bDoor);
        }
        else {
            bool bQuiet = !spawnFlyingComponen;
            CHook::CallFunction<void>("_ZN11CAutomobile13SetDoorDamageE6eDoorsb", m_pVehicle, bDoor, bQuiet);
        }
    }
}

void CVehicleSamp::SetDoorStatus(uint32_t dwDoorStatus, bool spawnFlyingComponen) {
    if (m_pVehicle) {
        for (uint8_t uiIndex = 0; uiIndex < MAX_DOORS; uiIndex++) {
            SetDoorStatus(static_cast<eDoors>(uiIndex), static_cast<eDoorStatus>(dwDoorStatus), spawnFlyingComponen);
            dwDoorStatus >>= 8;
        }
    }
}

void CVehicleSamp::SetPanelStatus(ePanelDamageState ulPanelStatus) const {
    if (m_pVehicle) {
        for (uint8_t uiIndex = 0; uiIndex < MAX_PANELS; uiIndex++) {
            SetPanelStatus((ePanels)uiIndex, ulPanelStatus);
            ulPanelStatus = static_cast<ePanelDamageState>(static_cast<uint32_t>(ulPanelStatus) >> 4);
        }
    }
}

uint8_t CVehicleSamp::GetLightStatus(eLights bLight) const {
    if (m_pVehicle && bLight < MAX_LIGHTS) {
        return m_pDamageManager->GetLightStatus(bLight);
    }
    return 0;
}

uint8_t CVehicleSamp::GetWheelStatus(eCarWheel bWheel) const {
    if (m_pVehicle && bWheel < MAX_CARWHEELS) {
        return m_pDamageManager->GetWheelStatus(bWheel);
    }
    return 0;
}

uint8_t CVehicleSamp::GetBikeWheelStatus(uint8_t bWheel) const {
    if (m_pVehicle && bWheel < 2) {
        if (bWheel == 0) {
            return m_pDamageManager->m_anWheelsStatus[CAR_WHEEL_FRONT_LEFT];
        }
        else {
            return m_pDamageManager->m_anWheelsStatus[CAR_WHEEL_REAR_LEFT];
        }
    }
    return 0;
}

void CVehicleSamp::UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage, uint8_t byteTireDamage) {
    if (HasDamageModel()) {
        SetPanelStatus((ePanelDamageState) dwPanelDamage);
        SetDoorStatus(dwDoorDamage, false);

        m_pDamageManager->SetLightStatus(eLights::LEFT_HEADLIGHT, static_cast<eLightsDamageState>(byteLightDamage & 1));
        m_pDamageManager->SetLightStatus(eLights::RIGHT_HEADLIGHT, static_cast<eLightsDamageState>((byteLightDamage >> 2) & 1));
        if ((byteLightDamage >> 6) & 1) {
            m_pDamageManager->SetLightStatus(eLights::LEFT_TAIL_LIGHT, (eLightsDamageState)1);
            m_pDamageManager->SetLightStatus(eLights::RIGHT_TAIL_LIGHT, (eLightsDamageState)1);
        }

        m_pDamageManager->SetWheelStatus(eCarWheel::CAR_WHEEL_REAR_RIGHT, static_cast<eCarWheelStatus>(byteTireDamage & 1));
        m_pDamageManager->SetWheelStatus(eCarWheel::CAR_WHEEL_FRONT_RIGHT, static_cast<eCarWheelStatus>((byteTireDamage >> 1) & 1));
        m_pDamageManager->SetWheelStatus(eCarWheel::CAR_WHEEL_REAR_LEFT, static_cast<eCarWheelStatus>((byteTireDamage >> 2) & 1));
        m_pDamageManager->SetWheelStatus(eCarWheel::CAR_WHEEL_FRONT_LEFT, static_cast<eCarWheelStatus>((byteTireDamage >> 3) & 1));

    }
    else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE) {
        m_pDamageManager->m_anWheelsStatus[CAR_WHEEL_REAR_LEFT] = static_cast<eCarWheelStatus>(byteTireDamage & 1);
        m_pDamageManager->m_anWheelsStatus[CAR_WHEEL_FRONT_LEFT] = static_cast<eCarWheelStatus>((byteTireDamage >> 1) & 1);
    }
}

unsigned int CVehicleSamp::GetVehicleSubtype() const
{
    if (m_pVehicle)
    {
        if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066D678 : 0x83BB50))
        {
            return VEHICLE_SUBTYPE_CAR;
        }
        else if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066DA20 : 0x83C2A0))
        {
            return VEHICLE_SUBTYPE_BOAT;
        }
        else if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066D7F0 : 0x83BE40))
        {
            return VEHICLE_SUBTYPE_BIKE;
        }
        else if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066DD84 : 0x83C968))
        {
            return VEHICLE_SUBTYPE_PLANE;
        }
        else if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066DB34 : 0x83C4C8))
        {
            return VEHICLE_SUBTYPE_HELI;
        }
        else if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066D908 : 0x83C070))
        {
            return VEHICLE_SUBTYPE_PUSHBIKE;
        }
        else if (*(uintptr*)m_pVehicle == g_libGTASA + (VER_x32 ? 0x0066E0FC : 0x83D058))
        {
            return VEHICLE_SUBTYPE_TRAIN;
        }
    }

    return 0;
}

void CVehicleSamp::GetDamageStatusEncoded(uint8_t* byteTyreFlags, uint8_t* byteLightFlags, uint32_t* dwDoorFlags, uint32_t* dwPanelFlags)
{
    if (byteTyreFlags) *byteTyreFlags = GetWheelStatus(eCarWheel::CAR_WHEEL_REAR_RIGHT) |
                                        (GetWheelStatus(eCarWheel::CAR_WHEEL_FRONT_RIGHT) << 1) |
                                        (GetWheelStatus(eCarWheel::CAR_WHEEL_REAR_LEFT) << 2) |
                                        (GetWheelStatus(eCarWheel::CAR_WHEEL_FRONT_LEFT) << 3);

    if (byteLightFlags) *byteLightFlags = GetLightStatus(eLights::LEFT_HEADLIGHT) |
                                          (GetLightStatus(eLights::RIGHT_HEADLIGHT) << 2);

    if (GetLightStatus(eLights::LEFT_TAIL_LIGHT) && GetLightStatus(eLights::RIGHT_TAIL_LIGHT))
        *byteLightFlags |= (1 << 6);

    if (dwDoorFlags) *dwDoorFlags = GetDoorStatus(eDoors::DOOR_BONNET) |
                                    (GetDoorStatus(eDoors::DOOR_BOOT) << 8) |
                                    (GetDoorStatus(eDoors::DOOR_LEFT_FRONT) << 16) |
                                    (GetDoorStatus(eDoors::DOOR_RIGHT_FRONT) << 24);

    if (dwDoorFlags) *dwPanelFlags = m_pDamageManager->GetPanelStatus(FRONT_LEFT_PANEL) |
                                     m_pDamageManager->GetPanelStatus(FRONT_RIGHT_PANEL) << 4 |
                                     m_pDamageManager->GetPanelStatus(REAR_LEFT_PANEL) << 8 |
                                     m_pDamageManager->GetPanelStatus(REAR_RIGHT_PANEL) << 12 |
                                     m_pDamageManager->GetPanelStatus(WINDSCREEN_PANEL) << 16 |
                                     m_pDamageManager->GetPanelStatus(FRONT_BUMPER) << 20 |
                                     m_pDamageManager->GetPanelStatus(REAR_BUMPER) << 24;
}

void CVehicleSamp::ProcessDamage() {
    VEHICLEID vehId = CVehiclePool::FindIDFromGtaPtr(m_pVehicle);
    if (vehId != INVALID_VEHICLE_ID) {
        if (HasDamageModel()) {
            uint8_t byteTyreFlags, byteLightFlags;
            uint32_t dwDoorFlags, dwPanelFlags;

            GetDamageStatusEncoded(&byteTyreFlags, &byteLightFlags, &dwDoorFlags, &dwPanelFlags);
            bool bDamageChanged = (byteTyreFlags != m_byteTyreStatus) ||
                                  (dwDoorFlags != m_dwDoorStatus) ||
                                  (dwPanelFlags != m_dwPanelStatus);
            if (bDamageChanged) {
                m_byteLightStatus = byteLightFlags;
                m_byteTyreStatus = byteTyreFlags;
                m_dwDoorStatus = dwDoorFlags;
                m_dwPanelStatus = dwPanelFlags;

                RakNet::BitStream bsDamage;

                bsDamage.Write(vehId);
                bsDamage.Write(dwPanelFlags);
                bsDamage.Write(dwDoorFlags);
                bsDamage.Write(byteLightFlags);
                bsDamage.Write(byteTyreFlags);

                pNetGame->GetRakClient()->RPC(&RPC_VehicleDamage, &bsDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
            }
        }
        else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE) {
            uint8_t byteTyreFlags = GetBikeWheelStatus(1) | (GetBikeWheelStatus(0) << 1);
            if (m_byteTyreStatus != byteTyreFlags) {
                m_byteTyreStatus = byteTyreFlags;

                RakNet::BitStream bsDamage;
                bsDamage.Write(             vehId);
                bsDamage.Write((uint32_t)   0);
                bsDamage.Write((uint32_t)   0);
                bsDamage.Write((uint8_t)    0);
                bsDamage.Write(             byteTyreFlags);

                pNetGame->GetRakClient()->RPC(&RPC_VehicleDamage, &bsDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
            }
        }
    }
}

void CVehicleSamp::SetStrob(eStobsStatus type) {
	if (!m_pVehicle || GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR) {
		return;
	}

    SetLightState(1, VEHICLE_LIGHT_OK);
    SetLightState(0, VEHICLE_LIGHT_OK);

    m_iStrobsType = type;
    dwStrobStep = 0;

    if(type == eStobsStatus::OFF) {
        return;
    }

}

void CVehicleSamp::ProcessStrobs() {
	if (!m_pVehicle || GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR) {
		return;
	}

	if(m_iStrobsType == eStobsStatus::OFF) {
        return;
    }
    uint32 dif = CTimer::m_snTimeInMillisecondsNonClipped - lastStrobTime;
	if(dif >= 40) {
        lastStrobTime = CTimer::m_snTimeInMillisecondsNonClipped;
		dwStrobStep ++;
        if (m_iStrobsType == eStobsStatus::ON_TYPE_4) {
            lightColorStrob = lightColor;
            switch (dwStrobStep) {
                case 1: {
                    SetLightState(0, VEHICLE_LIGHT_OK);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 2: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 3: {
                    SetLightState(0, VEHICLE_LIGHT_OK);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 4: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 5: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_OK);
                    break;
                }
                case 6: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 7: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_OK);
                    break;
                }
                case 8: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 11: {
                    dwStrobStep = 0;
                    break;
                }
            }
            return;
        }
        if (m_iStrobsType == eStobsStatus::ON_TYPE_3) {
            lightColorStrob = lightColor;
            switch (dwStrobStep) {
                case 1: {
                    SetLightState(0, VEHICLE_LIGHT_OK);
                    SetLightState(1, VEHICLE_LIGHT_OK);
                    break;
                }
                case 2: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 3: {
                    SetLightState(0, VEHICLE_LIGHT_OK);
                    SetLightState(1, VEHICLE_LIGHT_OK);
                    break;
                }
                case 4: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 5: {
                    SetLightState(0, VEHICLE_LIGHT_OK);
                    SetLightState(1, VEHICLE_LIGHT_OK);
                    break;
                }
                case 6: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 7: {
                    SetLightState(0, VEHICLE_LIGHT_OK);
                    SetLightState(1, VEHICLE_LIGHT_OK);
                    break;
                }
                case 8: {
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 13: {
                    dwStrobStep = 0;
                    break;
                }
            }
            return;
        }
		if (m_iStrobsType == eStobsStatus::ON_TYPE_2) {
            lightColorStrob = lightColor;
			switch (dwStrobStep) {
				case 1: {
					SetLightState(0, VEHICLE_LIGHT_OK);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 2: {
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_OK);
					break;
				}
				case 3: {
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 6: {
                    dwStrobStep = 0;
					break;
				}
			}
			return;
		}
		if (m_iStrobsType == eStobsStatus::ON_TYPE_1) {
			switch (dwStrobStep) {
				case 1: {
					lightColorStrob = 0xff000000;
					SetLightState(0, VEHICLE_LIGHT_OK);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 2: {
					lightColorStrob = 0xff000000;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 3: {
					lightColorStrob = 0xff000000;
					SetLightState(0, VEHICLE_LIGHT_OK);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 4: {
					lightColorStrob = 0xff000000;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 5: {
					lightColorStrob = 0xff000000;
					SetLightState(0, VEHICLE_LIGHT_OK);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 6: {
					lightColorStrob = 0xff000000;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 7: {
					lightColorStrob = 0x0000ff00;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_OK);
					break;
				}
				case 8: {
					lightColorStrob = 0x0000ff00;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 9: {
					lightColorStrob = 0x0000ff00;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_OK);
					break;
				}
				case 10: {
					lightColorStrob = 0x0000ff00;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_SMASHED);
					break;
				}
				case 11: {
					lightColorStrob = 0x0000ff00;
					SetLightState(0, VEHICLE_LIGHT_SMASHED);
					SetLightState(1, VEHICLE_LIGHT_OK);
					break;
				}
                case 12: {
                    lightColorStrob = 0x0000ff00;
                    SetLightState(0, VEHICLE_LIGHT_SMASHED);
                    SetLightState(1, VEHICLE_LIGHT_SMASHED);
                    break;
                }
                case 15: {
                    dwStrobStep = 0;
                    break;
                }
			}
		}
	}
}

void CVehicleSamp::SetLightState(int iLight, eLightsDamageState state) const
{
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0056E748 + 1 : 0x690948), (uintptr_t)m_pVehicle + sizeof(CVehicle), iLight, state);
}


void CVehicleSamp::ChangeVinylTo(int vinylIdx) {
    if(m_pVinylTex) {
        RwTextureDestroy(m_pVinylTex);
        m_pVinylTex = nullptr;
    }

    m_nVinylIndex = vinylIdx;

    if(vinylIdx == -1)
        return;

	char name[55];
	sprintf(name, "remapbody%d", vinylIdx + 1);
	m_pVinylTex = CUtil::LoadTextureFromDB("cars", name);
}

/*void CVehicleSamp::SetRGBATexture(CRGBA crgba, CRGBA crgba2) {
    static constexpr int TEXTURE_WIDTH = 256;
    static constexpr int TEXTURE_HEIGHT = 128;
    static constexpr int PIXEL_SIZE = 4;

    if (m_pVinylTex) {
        RwTextureDestroy(m_pVinylTex);
        m_pVinylTex = nullptr;
    }

    auto textRasterizer = std::make_shared<CTextRasterizer>(TEXTURE_WIDTH, TEXTURE_HEIGHT);
    uint8_t* pBitmap = textRasterizer->GetBitmap();

    for (int x = 0; x < TEXTURE_WIDTH; x++) {
        float t = x / static_cast<float>(TEXTURE_WIDTH - 1);
        CColor colGradient(
                static_cast<uint8_t>(crgba.r + (crgba2.r - crgba.r) * t),
                static_cast<uint8_t>(crgba.g + (crgba2.g - crgba.g) * t),
                static_cast<uint8_t>(crgba.b + (crgba2.b - crgba.b) * t),
                255
        );

        uint32_t pixelValue = colGradient.Get(CColor::COLOR_ENDIAN_ABGR);
        for (int y = 0; y < TEXTURE_HEIGHT; y++) {
            *reinterpret_cast<uint32_t*>(pBitmap + (y * TEXTURE_WIDTH + x) * PIXEL_SIZE) = pixelValue;
        }
    }

    RwImage* pRwImage = RwImageCreate(TEXTURE_WIDTH, TEXTURE_HEIGHT, PIXEL_SIZE * 8);
    if (!pRwImage) {
        return;
    }

    RwImageAllocatePixels(pRwImage);
    for (int y = 0; y < TEXTURE_HEIGHT; y++) {
        memcpy(
                pRwImage->cpPixels + pRwImage->stride * y,
                pBitmap + TEXTURE_WIDTH * PIXEL_SIZE * y,
                TEXTURE_WIDTH * PIXEL_SIZE
        );
    }

    int width, height, depth, flags;
    RwImageFindRasterFormat(pRwImage, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);

    RwRaster* pRaster = RwRasterCreate(width, height, depth, flags);
    if (!pRaster) {
        RwImageDestroy(pRwImage);
        return;
    }

    RwRasterSetFromImage(pRaster, pRwImage);
    RwImageDestroy(pRwImage);

    m_pVinylTex = RwTextureCreate(pRaster);
    if (m_pVinylTex) {
        m_pVinylTex->refCount++;
    } else {
        RwRasterDestroy(pRaster);
    }
}*/

void CVehicleSamp::SetRGBATexture(CRGBA crgba, CRGBA crgba2) {
    static constexpr int TEXTURE_WIDTH = 256;
    static constexpr int TEXTURE_HEIGHT = 128;
    static constexpr int PIXEL_SIZE = 4;

    if (m_pVinylTex) {
        RwTextureDestroy(m_pVinylTex);
        m_pVinylTex = nullptr;
    }

    std::vector<uint8_t> pixelData(TEXTURE_WIDTH * TEXTURE_HEIGHT * PIXEL_SIZE);

    for (int x = 0; x < TEXTURE_WIDTH; x++) {
        float t = x / static_cast<float>(TEXTURE_WIDTH - 1);
        auto r = static_cast<uint8_t>(crgba.r + (crgba2.r - crgba.r) * t);
        auto g = static_cast<uint8_t>(crgba.g + (crgba2.g - crgba.g) * t);
        auto b = static_cast<uint8_t>(crgba.b + (crgba2.b - crgba.b) * t);
        uint8_t a = 255;

        uint32_t pixelValue = (a << 24) | (b << 16) | (g << 8) | r;

        for (int y = 0; y < TEXTURE_HEIGHT; y++) {
            size_t offset = (y * TEXTURE_WIDTH + x) * PIXEL_SIZE;
            *reinterpret_cast<uint32_t*>(&pixelData[offset]) = pixelValue;
        }
    }

    RwImage* pRwImage = RwImageCreate(TEXTURE_WIDTH, TEXTURE_HEIGHT, PIXEL_SIZE * 8);
    if (!pRwImage) {
        return;
    }

    RwImageAllocatePixels(pRwImage);
    for (int y = 0; y < TEXTURE_HEIGHT; y++) {
        memcpy(
                pRwImage->cpPixels + pRwImage->stride * y,
                pixelData.data() + TEXTURE_WIDTH * PIXEL_SIZE * y,
                TEXTURE_WIDTH * PIXEL_SIZE
        );
    }

    int width, height, depth, flags;
    RwImageFindRasterFormat(pRwImage, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);

    RwRaster* pRaster = RwRasterCreate(width, height, depth, flags);
    if (!pRaster) {
        RwImageDestroy(pRwImage);
        return;
    }

    RwRasterSetFromImage(pRaster, pRwImage);
    RwImageDestroy(pRwImage);

    m_pVinylTex = RwTextureCreate(pRaster);
    if (m_pVinylTex) {
        m_pVinylTex->refCount++;
    } else {
        RwRasterDestroy(pRaster);
    }
}