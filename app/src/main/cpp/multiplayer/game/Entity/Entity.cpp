//
// Created on 20.04.2023.
//
#include "../common.h"

#include "Entity.h"
#include "../RW/rwcore.h"
#include "../RwHelper.h"
#include "game/RW/rpskin.h"
#include "util/patch.h"
#include "game/Models/ModelInfo.h"
#include "game/References.h"
#include "game/Pools.h"
#include "game/game.h"
#include "chatwindow.h"
#include "game/Collision/ColStore.h"
#include "game/Camera.h"
#include "TwoDEffect/2dEffect.h"
#include "Coronas.h"
#include "TimeCycle.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "Weather.h"
#include "PointLights.h"
#include "game/Shadow/Shadows.h"
#include "World.h"

void CEntity::UpdateRwFrame()
{
    if (!m_pRwObject)
        return;

    RwFrameUpdateObjects(static_cast<RwFrame*>(rwObjectGetParent(m_pRwObject)));
}

//void CEntity::DeleteRwObject()
//{
//    if(!*(uintptr*)this) return;
//
//    (( void (*)(CEntity*))(*(void**)(*(uintptr*)this + (VER_x32 ? 0x24 : 0x24*2))))(this);
//}

void CEntity::SetInterior(int interiorId, bool needRefresh)
{
    m_nAreaCode = static_cast<eAreaCodes>(interiorId);
    
    if ( this == CGame::FindPlayerPed()->m_pPed )
    {
        CGame::currArea = interiorId;

        auto pos = GetPosition();
        CColStore::RequestCollision(&pos, m_nAreaCode);

        if(interiorId == 0) {
            CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x00420898 + 1 : 0x504194), false);
           // CTimeCycle::StopExtraColour(0);
        }

    }

}

void CEntity::UpdateRpHAnim() {
    if (const auto atomic = GetFirstAtomic(m_pRwClump)) {
        if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) && !m_bDontUpdateHierarchy) {
            RpHAnimHierarchyUpdateMatrices(GetAnimHierarchyFromSkinClump(m_pRwClump));
        }
    }
}

float CEntity::GetDistanceFromPoint(float X, float Y, float Z) const
{
    CVector vec(X, Y, Z);

    return DistanceBetweenPoints(GetPosition(), vec);
}

float CEntity::GetDistanceFromLocalPlayerPed() const
{
    auto pLocalPlayerPed = CGame::FindPlayerPed();

    return DistanceBetweenPoints(GetPosition(), pLocalPlayerPed->m_pPed->GetPosition());
}

float CEntity::GetDistanceFromCamera()
{
    if(!this)
        return 0;

    
    return DistanceBetweenPoints(GetPosition(), CCamera::Get().GetPosition());
}

bool CEntity::IsScanCodeCurrent() const {
    return m_nScanCode == CWorld::ms_nCurrentScanCode;
}

void CEntity::SetCurrentScanCode() {
    m_nScanCode = CWorld::ms_nCurrentScanCode;
}

bool CEntity::IsInCurrentArea() const
{
    return m_nAreaCode == CGame::currArea;
}

void CEntity::UpdateRW() {
    if (!m_pRwObject)
        return;

    auto parentMatrix = GetModellingMatrix();
    if (m_matrix)
        m_matrix->UpdateRwMatrix(parentMatrix);
    else
        m_placement.UpdateRwMatrix(parentMatrix);
}

bool CEntity::IsVisible()
{
    if (!m_pRwObject || !m_bIsVisible)
        return false;

    return GetIsOnScreen();
}

bool CEntity::GetIsOnScreen() {
    CVector thisVec;
    GetBoundCentre(thisVec);

    
    return CCamera::Get().IsSphereVisible(&thisVec, CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius());
}

RwMatrix* CEntity::GetModellingMatrix() {
    if (!m_pRwObject)
        return nullptr;

    return RwFrameGetMatrix(RwFrameGetParent(m_pRwObject));
}

CColModel* CEntity::GetColModel() const {
    if (IsVehicle()) {
        const auto veh = static_cast<const CVehicle*>(this);
        if (veh->m_vehicleSpecialColIndex > -1) {
            return &CVehicle::m_aSpecialColModel[veh->m_vehicleSpecialColIndex];
        }
    }

    return CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel();
}

CVector CEntity::TransformFromObjectSpace(const CVector& offset)
{
    auto result = CVector();
    if (m_matrix) {
        result = *m_matrix * offset;
        return result;
    }

    CUtil::TransformPoint(result, m_placement, offset);
    return result;
}

// 0x533560
CVector* CEntity::TransformFromObjectSpace(CVector& outPos, const CVector& offset)
{
    auto result = TransformFromObjectSpace(offset);
    outPos = result;
    return &outPos;
}

void CEntity::SetCollisionChecking(bool bCheck)
{
    m_bCollisionProcessed = bCheck;
}


CBaseModelInfo* CEntity::GetModelInfo() const {
    return CModelInfo::GetModelInfo(m_nModelIndex);
}

CVector* CEntity::GetBoundCentre(CVector* pOutCentre)
{
    auto mi = CModelInfo::GetModelInfo(m_nModelIndex);
    const auto& colCenter = mi->GetColModel()->GetBoundCenter();
    return TransformFromObjectSpace(*pOutCentre, colCenter);
}

// 0x534290
void CEntity::GetBoundCentre(CVector& outCentre) {
    TransformFromObjectSpace(outCentre, GetColModel()->GetBoundCenter());
}

CVector CEntity::GetBoundCentre()
{
    CVector v;
    GetBoundCentre(v);
    return v;
}

bool CEntity::GetIsTouching(CEntity* entity)
{
    CVector thisVec;
    GetBoundCentre(thisVec);

    CVector otherVec;
    entity->GetBoundCentre(otherVec);

    auto fThisRadius = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundRadius();
    auto fOtherRadius = CModelInfo::GetModelInfo(entity->m_nModelIndex)->GetColModel()->GetBoundRadius();

    return (thisVec - otherVec).Magnitude() <= (fThisRadius + fOtherRadius);
}

void CEntity::RegisterReference(CEntity** entity)
{
    if (IsBuilding() && !m_bIsTempBuilding && !m_bIsProcObject && !m_nIplIndex)
        return;

    auto refs = m_pReferences;
    while (refs) {
        if (refs->m_ppEntity == entity) {
            return;
        }
        refs = refs->m_pNext;
    }

    if (!m_pReferences && !CReferences::pEmptyList) {
        auto iPedsSize = GetPedPoolGta()->GetSize();
        for (int32 i = 0; i < iPedsSize; ++i) {
            auto ped = GetPedPoolGta()->GetAt(i);
            if (ped) {
                ped->PruneReferences();
                if (CReferences::pEmptyList)
                    break;
            }

        }

        if (!CReferences::pEmptyList) {
            auto iVehsSize = GetVehiclePoolGta()->GetSize();
            for (int32 i = 0; i < iVehsSize; ++i) {
                auto vehicle = GetVehiclePoolGta()->GetAt(i);
                if (vehicle) {
                    vehicle->PruneReferences();
                    if (CReferences::pEmptyList)
                        break;
                }

            }
        }

        if (!CReferences::pEmptyList) {
            auto iObjectsSize = GetObjectPoolGta()->GetSize();
            for (int32 i = 0; i < iObjectsSize; ++i) {
                auto obj = GetObjectPoolGta()->GetAt(i);
                if (obj) {
                    obj->PruneReferences();
                    if (CReferences::pEmptyList)
                        break;
                }
            }
        }
    }

    if (CReferences::pEmptyList) {
        auto pEmptyRef = CReferences::pEmptyList;
        CReferences::pEmptyList = pEmptyRef->m_pNext;
        pEmptyRef->m_pNext = m_pReferences;
        m_pReferences = pEmptyRef;
        pEmptyRef->m_ppEntity = entity;
    }
}

void CEntity::ResolveReferences()
{
    auto refs = m_pReferences;
    while (refs) {
        if (*refs->m_ppEntity == this)
            *refs->m_ppEntity = nullptr;

        refs = refs->m_pNext;
    }

    refs = m_pReferences;
    if (!refs)
        return;

    refs->m_ppEntity = nullptr;
    while (refs->m_pNext)
        refs = refs->m_pNext;

    refs->m_pNext = CReferences::pEmptyList;
    CReferences::pEmptyList = m_pReferences;
    m_pReferences = nullptr;
}

void CEntity::PruneReferences()
{
    if (!m_pReferences)
        return;

    auto refs = m_pReferences;
    auto ppPrev = &m_pReferences;
    while (refs) {
        if (*refs->m_ppEntity == this) {
            ppPrev = &refs->m_pNext;
            refs = refs->m_pNext;
        }
        else {
            auto refTemp = refs->m_pNext;
            *ppPrev = refs->m_pNext;
            refs->m_pNext = CReferences::pEmptyList;
            CReferences::pEmptyList = refs;
            refs->m_ppEntity = nullptr;
            refs = refTemp;
        }
    }
}

void CEntity::CleanUpOldReference(CEntity** entity)
{
    if (!m_pReferences)
        return;

    auto refs = m_pReferences;
    auto ppPrev = &m_pReferences;
    while (refs->m_ppEntity != entity) {
        ppPrev = &refs->m_pNext;
        refs = refs->m_pNext;
        if (!refs)
            return;
    }

    *ppPrev = refs->m_pNext;
    refs->m_pNext = CReferences::pEmptyList;
    refs->m_ppEntity = nullptr;
    CReferences::pEmptyList = refs;
}

bool CEntity::DoesNotCollideWithFlyers()
{
    auto mi = CModelInfo::GetModelInfo(m_nModelIndex);
    return mi->SwaysInWind() || mi->bDontCollideWithFlyer;
}

CEntity::CEntity() : CPlaceable() {
    m_nStatus = STATUS_ABANDONED;
    m_nType = ENTITY_TYPE_NOTHING;

    m_nFlags = 0;
    m_bIsVisible = true;
    m_bBackfaceCulled = true;

    m_nScanCode = 0;
    m_nAreaCode = eAreaCodes::AREA_CODE_NORMAL_WORLD;
    m_nModelIndex = 0xFFFF;
    m_pRwObject = nullptr;
    m_nIplIndex = 0;
    m_nRandomSeed = CGeneral::GetRandomNumber();
    m_pReferences = nullptr;
    m_pStreamingLink = nullptr;
    m_nNumLodChildren = 0;
    m_nNumLodChildrenRendered = 0;
    m_pLod = nullptr;
}

CEntity::~CEntity()
{
    if (m_pLod)
        m_pLod->m_nNumLodChildren--;

    CEntity::DeleteRwObject();
    CEntity::ResolveReferences();
}

void CEntity::Add()
{
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x3ED8D8 + 1 : 0x4CD574), this);
}

// ------------- hooks

inline void CleanUpOldReference_hook(CEntity *thiz, CEntity** entity) {
    thiz->CleanUpOldReference(entity);
}

inline void ResolveReferences_hook(CEntity *thiz) {
    thiz->ResolveReferences();
}

inline void PruneReferences_hook(CEntity* thiz) {
    thiz->PruneReferences();
}

inline void RegisterReference_hook(CEntity* thiz, CEntity** entity) {
    thiz->RegisterReference(entity);
}

void CEntity__ProcessLightsForEntity(CEntity* entity) {
    entity->ProcessLightsForEntity();
}

void CEntity::InjectHooks() {
    CHook::Redirect("_ZN7CEntity22ProcessLightsForEntityEv", &CEntity__ProcessLightsForEntity);
}

void CEntity::Add(const CRect* rect) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x3ED8FC + 1 : 0x4CD5C0), this, rect);
}

void CEntity::Remove() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x3EDBE8 + 1 : 0x4CD888), this);
}

void CEntity::SetModelIndex(uint32 index) {

}

void CEntity::SetModelIndexNoCreate(uint32 index) {

}

void CEntity::SetIsStatic(bool isStatic) {

}

void CEntity::CreateRwObject() {

}

CRect CEntity::GetBoundRect() {
    return CRect();
}

void CEntity::ProcessControl() {

}

void CEntity::ProcessCollision() {

}

void CEntity::DeleteRwObject() {

}

void CEntity::ProcessShift() {

}

bool CEntity::TestCollision(bool bApplySpeed) {
    return false;
}

void CEntity::Teleport(CVector destination, bool resetRotation) {

}

void CEntity::SpecialEntityPreCollisionStuff(struct CPhysical *colPhysical, bool bIgnoreStuckCheck, bool &bCollisionDisabled, bool &bCollidedEntityCollisionIgnored, bool &bCollidedEntityUnableToMove, bool &bThisOrCollidedEntityStuck) {

}

uint8 CEntity::SpecialEntityCalcCollisionSteps(bool &bProcessCollisionBeforeSettingTimeStep, bool &unk2) {
    return 0;
}

void CEntity::PreRender() {

}

void CEntity::Render() {

}

bool CEntity::SetupLighting() {
    return false;
}

void CEntity::RemoveLighting(bool bRemove) {

}

void CEntity::FlagToDestroyWhenNextProcessed() {

}

void CEntity::ProcessLightsForEntity() {
    auto fBalance = CCustomBuildingDNPipeline::m_fDNBalanceParam;
    if (m_bRenderDamaged || !m_bIsVisible || GetDistanceFromLocalPlayerPed() > 100.f)
        return;

    if (IsVehicle()) {
        if (AsPhysical()->physicalFlags.bDestroyed)
            return;
    }
    else {
        if (m_matrix && m_matrix->GetUp().z < 0.96F)
            return;
    }

    auto mi = CModelInfo::GetModelInfo(m_nModelIndex);
    if (!mi->m_n2dfxCount)
        return;

    
    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = CHook::CallFunction<C2dEffect*>("_ZN14CBaseModelInfo11Get2dEffectEi", mi, iFxInd);
        auto fIntensity = 1.0F;
        auto uiRand = m_nRandomSeed ^ (iFxInd & 0x7);

        if (effect->m_type == e2dEffectType::EFFECT_SUN_GLARE && CWeather::SunGlare >= 0.0F) {
            auto vecEffPos = TransformFromObjectSpace(effect->m_pos);

            auto vecDir = vecEffPos - GetPosition();
            vecDir.Normalise();

            auto vecCamDir = CCamera::Get().GetPosition() - vecEffPos;
            auto fCamDist = vecCamDir.Magnitude();
            auto fScale = 2.0F / fCamDist;
            auto vecScaledCam = (vecCamDir * fScale);
            vecDir += vecScaledCam;
            vecDir.Normalise();

            auto fDot = -DotProduct(vecDir, CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue]);
            if (fDot <= 0.0F)
                continue;

            auto fGlare = sqrt(fDot) * CWeather::SunGlare;
            auto fRadius = sqrt(fCamDist) * CWeather::SunGlare * 0.5F;
            vecEffPos += vecScaledCam;

            auto ucRed = static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreRed + 510) * fGlare / 3.0F);
            auto ucGreen = static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreGreen + 510) * fGlare / 3.0F);
            auto ucBlue = static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreBlue + 510) * fGlare / 3.0F);

            CCoronas::RegisterCorona(
                    reinterpret_cast<uintptr_t>(this->m_pRwObject) + 1,
                    nullptr,
                    ucRed,
                    ucGreen,
                    ucBlue,
                    255,
                    &vecEffPos,
                    fRadius,
                    120.0F,
                    CCoronas::gpCoronaTexture[0],
                    eCoronaFlareType::FLARETYPE_NONE,
                    false,
                    false,
                    0,
                    0.0F,
                    false,
                    1.5F,
                    0,
                    15.0F,
                    false,
                    false
            );

            continue;
        }

        if (effect->m_type != e2dEffectType::EFFECT_LIGHT)
            continue;

        auto vecEffPos = TransformFromObjectSpace(effect->m_pos);
        auto bDoColorLight = false;
        auto bDoNoColorLight = false;
        auto bCoronaVisible = false;
        bool bUpdateCoronaCoors = false;
        auto fDayNight = 1.0F;
        if (effect->light.m_bAtDay && effect->light.m_bAtNight) {
            bCoronaVisible = true;
        }
        else if (effect->light.m_bAtDay && fBalance < 1.0F) {
            bCoronaVisible = true;
            fDayNight = 1.0F - fBalance;
        }
        else if (effect->light.m_bAtNight && fBalance > 0.0F) {
            bCoronaVisible = true;
            fDayNight = fBalance;
        }

        const auto& vecPos = GetPosition();
        auto iFlashType = effect->light.m_nCoronaFlashType;
        float fBalance; // todo: shadow var
        uint32 uiMode, uiOffset;
        if (iFlashType == e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET && CWeather::WetRoads > 0.5F || bCoronaVisible) {
            switch (iFlashType) {
                case e2dCoronaFlashType::FLASH_DEFAULT:
                    bDoColorLight = true;
                    break;

                case e2dCoronaFlashType::FLASH_RANDOM:
                case e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET:
                    if ((CTimer::GetTimeInMS() ^ uiRand) & 0x60)
                        bDoColorLight = true;
                    else
                        bDoNoColorLight = true;

                    if ((uiRand ^ (CTimer::GetTimeInMS() / 4096)) & 0x3)
                        bDoColorLight = true;

                    break;

                case e2dCoronaFlashType::FLASH_ANIM_SPEED_4X:
                    if (((CTimer::GetTimeInMS() + iFxInd * 256) & 0x200) == 0)
                        bUpdateCoronaCoors = true;
                    else
                        bDoColorLight = true;

                    break;

                case e2dCoronaFlashType::FLASH_ANIM_SPEED_2X:
                    if (((CTimer::GetTimeInMS() + iFxInd * 512) & 0x400) == 0)
                        bUpdateCoronaCoors = true;
                    else
                        bDoColorLight = true;

                    break;

                case e2dCoronaFlashType::FLASH_ANIM_SPEED_1X:
                    if (((CTimer::GetTimeInMS() + iFxInd * 1024) & 0x800) == 0)
                        bUpdateCoronaCoors = true;
                    else
                        bDoColorLight = true;

                    break;

                case e2dCoronaFlashType::FLASH_UNKN:
                    if (static_cast<uint8>(uiRand) > 0x10) {
                        bDoColorLight = true;
                        break;
                    }

                    if ((CTimer::GetTimeInMS() ^ (uiRand * 8)) & 0x60)
                        bDoColorLight = true;
                    else
                        bDoNoColorLight = true;

                    if ((uiRand ^ (CTimer::GetTimeInMS() / 4096)) & 0x3)
                        bDoColorLight = true;

                    break;

                case e2dCoronaFlashType::FLASH_TRAINCROSSING:
                    if (IsObject()) { // && entity->objectFlags.bTrainCrossEnabled) {
                        if (CTimer::GetTimeInMS() & 0x400)
                            bDoColorLight = true;

                        if (iFxInd & 1)
                            bDoColorLight = !bDoColorLight;
                    }

                    if (iFxInd >= 4)
                        bDoColorLight = false;

                    break;

                case e2dCoronaFlashType::FLASH_UNUSED:
                    //if (CBridge::ShouldLightsBeFlashing() && (CTimer::GetTimeInMS() & 0x1FF) < 0x3C)
                    //    bDoColorLight = true;

                    break;

                case e2dCoronaFlashType::FLASH_ONLY_RAIN:
                    if (CWeather::Rain > 0.0001F) {
                        fIntensity = CWeather::Rain;
                        bDoColorLight = true;
                    }
                    break;

                case e2dCoronaFlashType::FLASH_5ON_5OFF:
                case e2dCoronaFlashType::FLASH_6ON_4OFF:
                case e2dCoronaFlashType::FLASH_4ON_6OFF:

                    bDoColorLight = true;

                    uiOffset = CTimer::GetTimeInMS() + 3333 * (iFlashType - 11);
                    uiOffset += static_cast<uint32>(vecPos.x * 20.0F);
                    uiOffset += static_cast<uint32>(vecPos.y * 10.0F);

                    uiMode = 9 * ((uiOffset % 10000) / 10000);
                    fBalance = ((uiOffset % 10000) - (1111 * uiMode)) * 0.0009F;
                    switch (uiMode) {
                        case 0:
                            fIntensity = fBalance;
                            break;
                        case 1:
                        case 2:
                            fIntensity = 1.0F;
                            break;
                        case 3:
                            fIntensity = 1.0F - fBalance;
                            break;
                        default:
                            bDoColorLight = false;
                    }

                    break;

                default:
                    break;
            }
        }

// CORONAS
        auto bSkipCoronaChecks = false;
        auto laRiotsActiveHere = CHook::CallFunction<bool>("_ZN10CGameLogic17LaRiotsActiveHereEv");
        if (laRiotsActiveHere) {
            bool bLightsOn = bDoColorLight;
            bLightsOn &= !IsVehicle();
            bLightsOn &= ((uiRand & 3) == 0 || (uiRand & 3) == 1 && (CTimer::GetTimeInMS() ^ (uiRand * 8)) & 0x60);

            if (bLightsOn) {
                bDoColorLight = false;
                bDoNoColorLight = true;
                bSkipCoronaChecks = true;

                CCoronas::RegisterCorona(
                        reinterpret_cast<uintptr_t>(this) + iFxInd,
                        nullptr,
                        0,
                        0,
                        0,
                        255,
                        &vecEffPos,
                        effect->light.m_fCoronaSize,
                        effect->light.m_fCoronaFarClip,
                        effect->light.m_pCoronaTex,
                        static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                        effect->light.m_bCoronaEnableReflection,
                        effect->light.m_bCheckObstacles,
                        0,
                        0.0F,
                        effect->light.m_bOnlyLongDistance,
                        1.5F,
                        0,
                        15.0F,
                        false,
                        false
                );
            }
        }

        if (!bSkipCoronaChecks && bDoColorLight) {
            auto bCanCreateLight = true;
            if (effect->light.m_bCheckDirection) {
                const auto& camPos = CCamera::Get().GetPosition();
                CVector lightOffset{
                        static_cast<float>(effect->light.offsetX),
                        static_cast<float>(effect->light.offsetY),
                        static_cast<float>(effect->light.offsetZ)
                };
                auto vecLightPos = GetMatrix().TransformVector(lightOffset);

                auto fDot = DotProduct(vecLightPos, (camPos - vecEffPos));
                bCanCreateLight = fDot >= 0.0F;
            }

            if (bCanCreateLight) {
                bSkipCoronaChecks = true;
                auto fBrightness = fIntensity;
                if (effect->light.m_bBlinking1)
                    fBrightness = (1.0F - (CGeneral::GetRandomNumber() % 32) * 0.012F) * fIntensity;

                if (effect->light.m_bBlinking2 && (CTimer::GetFrameCounter() + uiRand) & 3)
                    fBrightness = 0.0F;

                if (effect->light.m_bBlinking3 && (CTimer::GetFrameCounter() + uiRand) & 0x3F) {
                    if (((CTimer::GetFrameCounter() + uiRand) & 0x3F) == 1)
                        fBrightness *= 0.5F;
                    else
                        fBrightness = 0.0F;
                }

                auto fSizeMult = 2.0F; // 1.0F
                if (m_nModelIndex == MODEL_RCBARON) {
                    fBrightness *= 1.9F;
                    fSizeMult = 2.0F;
                }

                fIntensity = CTimeCycle::m_CurrentColours.m_fSpriteBrightness * fBrightness * 0.1F;
                auto fSize = effect->light.m_fCoronaSize * fSizeMult;

                auto ucRed = static_cast<uint8>(static_cast<float>(effect->light.m_color.red) * fIntensity);
                auto ucGreen = static_cast<uint8>(static_cast<float>(effect->light.m_color.green) * fIntensity);
                auto ucBlue = static_cast<uint8>(static_cast<float>(effect->light.m_color.blue) * fIntensity);

                CCoronas::RegisterCorona(
                        reinterpret_cast<uintptr_t>(this) + iFxInd,
                        nullptr,
                        ucRed,
                        ucGreen,
                        ucBlue,
                        static_cast<uint8>(fDayNight * 255.0F),
                        &vecEffPos,
                        fSize,
                        effect->light.m_fCoronaFarClip,
                        effect->light.m_pCoronaTex,
                        static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                        effect->light.m_bCoronaEnableReflection,
                        effect->light.m_bCheckObstacles,
                        0,
                        0.0F,
                        effect->light.m_bOnlyLongDistance,
                        0.8F,
                        0,
                        15.0F,
                        effect->light.m_bOnlyFromBelow,
                        effect->light.m_bUpdateHeightAboveGround
                );
            }
            else {
                bDoColorLight = false;
                bUpdateCoronaCoors = true;
            }
        }

        if (!bSkipCoronaChecks && bDoNoColorLight) {
            bSkipCoronaChecks = true;
            CCoronas::RegisterCorona(
                    reinterpret_cast<uintptr_t>(this) + iFxInd,
                    nullptr,
                    0,
                    0,
                    0,
                    255,
                    &vecEffPos,
                    effect->light.m_fCoronaSize,
                    effect->light.m_fCoronaFarClip,
                    effect->light.m_pCoronaTex,
                    static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                    effect->light.m_bCoronaEnableReflection,
                    effect->light.m_bCheckObstacles,
                    0,
                    0.0F,
                    effect->light.m_bOnlyLongDistance,
                    1.5F,
                    0,
                    15.0F,
                    false,
                    false
            );
        }

        if (!bSkipCoronaChecks && bUpdateCoronaCoors) {
            CCoronas::UpdateCoronaCoors(
                    reinterpret_cast<uintptr>(this) + iFxInd,
                    &vecEffPos,
                    effect->light.m_fCoronaFarClip,
                    0.0F
            );
        }

// POINT LIGHTS
        bool bSkipLights = false;
        if (effect->light.m_fPointlightRange != 0.0F && bDoColorLight) {
            auto color = effect->light.m_color;
            if (color.red || color.green || color.blue) {
                auto fColorMult = fDayNight * fIntensity / 256.0F;

                bSkipLights = true;
                CPointLights::AddLight(
                        ePointLightType::PLTYPE_POINTLIGHT,
                        vecEffPos,
                        CVector(0.0F, 0.0F, 0.0F),
                        effect->light.m_fPointlightRange,
                        static_cast<float>(color.red) * fColorMult,
                        static_cast<float>(color.green) * fColorMult,
                        static_cast<float>(color.blue) * fColorMult,
                        effect->light.m_nFogType,
                        true,
                        nullptr
                );
            }
            else {
                CPointLights::AddLight(
                        ePointLightType::PLTYPE_DARKLIGHT,
                        vecEffPos,
                        CVector(0.0F, 0.0F, 0.0F),
                        effect->light.m_fPointlightRange,
                        0.0F,
                        0.0F,
                        0.0F,
                        RwFogType::rwFOGTYPENAFOGTYPE,
                        true,
                        nullptr
                );
            }
        }

        if (!bSkipLights) {
            if (effect->light.m_nFogType & RwFogType::rwFOGTYPEEXPONENTIAL) {
                auto color = effect->light.m_color;
                CPointLights::AddLight(
                        (ePointLightType)3u, // todo: Enum doesn't contain all types?
                        vecEffPos,
                        CVector(0.0F, 0.0F, 0.0F),
                        0.0F,
                        color.red / 256.0F,
                        color.green / 256.0F,
                        color.blue / 256.0F,
                        RwFogType::rwFOGTYPEEXPONENTIAL,
                        true,
                        nullptr
                );
            }
            else if (effect->light.m_nFogType & RwFogType::rwFOGTYPELINEAR && bDoColorLight && effect->light.m_fPointlightRange == 0.0F) {
                auto color = effect->light.m_color;
                CPointLights::AddLight(
                        (ePointLightType)4u, // todo: Enum doesn't contain all types?
                        vecEffPos,
                        CVector(0.0F, 0.0F, 0.0F),
                        0.0F,
                        color.red / 256.0F,
                        color.green / 256.0F,
                        color.blue / 256.0F,
                        RwFogType::rwFOGTYPELINEAR,
                        true,
                        nullptr
                );
            }
        }

// SHADOWS
        if (effect->light.m_fShadowSize != 0.0F) {
            auto fShadowZ = 15.0F;
            if (effect->light.m_nShadowZDistance)
                fShadowZ = static_cast<float>(effect->light.m_nShadowZDistance);

            if (bDoColorLight) {
                auto color = effect->light.m_color;
                auto fColorMult = effect->light.m_nShadowColorMultiplier * fIntensity / 256.0F;
                color.red    = static_cast<uint8>(static_cast<float>(color.red) * fColorMult);
                color.green  = static_cast<uint8>(static_cast<float>(color.green) * fColorMult);
                color.blue   = static_cast<uint8>(static_cast<float>(color.blue) * fColorMult);

                CShadows::StoreShadowToBeRendered(
                        SHADOW_ADDITIVE,
                        effect->light.m_pShadowTex,
                        &vecEffPos,
                        effect->light.m_fShadowSize, 0.0F,
                        0.0F, -effect->light.m_fShadowSize,
                        128,
                        color.red,
                        color.green,
                        color.blue,
                        fShadowZ,
                        false,
                        1.f,
                        nullptr,
                        false
                );
            }
            else if (bDoNoColorLight) {
                CShadows::StoreShadowToBeRendered(
                        SHADOW_ADDITIVE,
                        effect->light.m_pShadowTex,
                        &vecEffPos,
                        effect->light.m_fShadowSize, 0.0F,
                        0.0F, -effect->light.m_fShadowSize,
                        0,
                        0,
                        0,
                        0,
                        fShadowZ,
                        false,
                        1.f,
                        nullptr,
                        false
                );
            }
        }
    }
}
