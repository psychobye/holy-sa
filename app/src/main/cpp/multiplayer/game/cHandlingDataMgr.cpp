//
// Created on Traw-GG 27.08.2025.
//

#include "cHandlingDataMgr.h"
#include "CFileMgr.h"
#include "main.h"
#include "FileLoader.h"
#include "util/patch.h"
#include "Models/ModelInfo.h"
#include "CHandlingDefault.h"
#include <vector>
#include <fstream>

bool isHandlingLoaded = false;

void (*LoadHandlingData_orig)(defHandlingDataMgr *thiz);
void cHandlingDataMgr::LoadHandlingData(defHandlingDataMgr *thiz)
{
    CFileLoader::LoadVehicleObject();

    isHandlingLoaded = true;
    LoadHandlingData_orig(thiz);
    isHandlingLoaded = false;

    char filePath[256];
    snprintf(filePath, sizeof(filePath), "%sSAMP/handling.cfg", g_pszStorage);
    FILE* file = fopen(filePath, "rb");

    char line[300];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, ";the end", 8) == 0)
            break;

        if (strlen(line) == 0 || line[0] == ';') {
            // ���������� ����������� � ������ ������
            continue;
        }

        switch (line[0]) {
            case '!': {
                std::istringstream iss(line);
                std::string firstSym;
                std::string name;

                iss >> firstSym;
                if (!(iss >> name)) {
                    continue;
                }

                auto id = FindExactWord(name.c_str());
                if (id == -1) {
                    continue;
                }

                // bike
                tBikeHandlingData d{};
                d.InitFromData(id, line);

                m_aBikeHandling[id] = d;
                //if (id >= 0 && id < thiz->m_aBikeHandling.size()) {
                //    memcpy(&thiz->m_aBikeHandling[id], &d, sizeof(tHandlingData));
                //}
                CHandlingDefault::FillDefaultBikeHandling(id, &d);
                cHandlingDataMgr::ConvertBikeDataToGameUnits(&d);
                break;
            }
            case '$': {
                std::istringstream iss(line);
                std::string firstSym;
                std::string name;

                iss >> firstSym;
                if (!(iss >> name)) {
                    continue;
                }

                auto id = FindExactWord(name.c_str());
                if (id == -1) {
                    continue;
                }

                // flying
                tFlyingHandlingData d{};
                d.InitFromData(id, line);

                m_aFlyingHandling[id] = d;
                //if (id >= 0 && id < thiz->m_aFlyingHandling.size()) {
                //    memcpy(&thiz->m_aFlyingHandling[id], &d, sizeof(tHandlingData));
                //}
                continue;
            }
            case '%': {
                std::istringstream iss(line);
                std::string firstSym;
                std::string name;

                iss >> firstSym;
                if (!(iss >> name)) {
                    continue;
                }

                auto id = FindExactWord(name.c_str());
                if (id == -1) {
                    continue;
                }

                // boat
                tBoatHandlingData d{};
                d.InitFromData(id, line);

                m_aBoatHandling[id] = d;
                //if (id >= 0 && id < thiz->m_aBoatHandling.size()) {
                //    memcpy(&thiz->m_aBoatHandling[id], &d, sizeof(tHandlingData));
                //}
                break;
            }
                /*case '^': {
                    CVehicleAnimGroupData::LoadAGroupFromData(line);
                    break;
                }*/
            default: {
                std::istringstream iss(line);
                std::string name;

                if (!(iss >> name)) {
                    continue;
                }

                auto id = FindExactWord(name.c_str());
                if (id == -1) {
                    continue;
                }

                tHandlingData d{};
                d.InitFromData(id, line);

                m_aVehicleHandling[id] = d;
                //if (id >= 0 && id < thiz->m_aVehicleHandling.size()) {
                //    memcpy(&thiz->m_aVehicleHandling[id], &d, sizeof(tHandlingData));
                //}
                CHandlingDefault::FillDefaultHandling(id, &d);
                cHandlingDataMgr::ConvertDataToGameUnits(&d);
                break;
            }
        }
    }
    fclose(file);
}

int32 cHandlingDataMgr::FindExactWord(const char* name) {
    auto it = CVehicleNames::VehicleNames.find(name);
    if (it != CVehicleNames::VehicleNames.end()) {
        return it->second - 400;
    }

    Log("Can't find handling %s", name);
    return -1;
}

int32 cHandlingDataMgr::FindExactWord(const char* name, const char* nameTable, uint32 entrySize, uint32 entryCount) {
    for (auto i = 0u; i < entryCount; i++) {
        const auto entry = &nameTable[entrySize * i];
        if (!strncmp(name, entry, strlen(entry))) {
            return i;
        }
    }
    Log("Vehicle name not found in table: %s", name);
    return -1;
}

int32 tHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const char* p = line;
    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;

    char driveTypeChar, engineTypeChar;
    int absTemp;
    unsigned int modelFlags, handlingFlags;
    unsigned int frontLights, rearLights, animGroup;

    int scanned = sscanf(p, "%f %f %f %f %f %f %d %f %f %f %d %f %f %f %c %c %f %f %d %f %f %f %f %f %f %f %f %f %f %d %x %x %d %d %d",
                         &m_fMass,
                         &m_fTurnMass,
                         &m_fDragMult,
                         &m_vecCentreOfMass.x,
                         &m_vecCentreOfMass.y,
                         &m_vecCentreOfMass.z,
                         &m_nPercentSubmerged,
                         &m_fTractionMultiplier,
                         &m_fTractionLoss,
                         &m_fTractionBias,
                         &m_transmissionData.m_nNumberOfGears,
                         &m_transmissionData.m_fMaxGearVelocity,
                         &m_transmissionData.m_fEngineAcceleration,
                         &m_transmissionData.m_fEngineInertia,
                         &driveTypeChar,
                         &engineTypeChar,
                         &m_fBrakeDeceleration,
                         &m_fBrakeBias,
                         &absTemp,
                         &m_fSteeringLock,
                         &m_fSuspensionForceLevel,
                         &m_fSuspensionDampingLevel,
                         &m_fSuspensionHighSpdComDamp,
                         &m_fSuspensionUpperLimit,
                         &m_fSuspensionLowerLimit,
                         &m_fSuspensionBiasBetweenFrontAndRear,
                         &m_fSuspensionAntiDiveMultiplier,
                         &m_fSeatOffsetDistance,
                         &m_fCollisionDamageMultiplier,
                         &m_nMonetaryValue,
                         &modelFlags,
                         &handlingFlags,
                         &frontLights,
                         &rearLights,
                         &animGroup);

    m_transmissionData.m_nDriveType = driveTypeChar;
    m_transmissionData.m_nEngineType = engineTypeChar;
    m_bABS = (absTemp != 0);

    m_nModelFlags = static_cast<eVehicleHandlingModelFlags>(modelFlags);
    m_nHandlingFlags = static_cast<eVehicleHandlingFlags>(handlingFlags);
    m_nFrontLights = static_cast<eVehicleLightsSize>(frontLights);
    m_nRearLights = static_cast<eVehicleLightsSize>(rearLights);
    m_nAnimGroup = animGroup;

    m_transmissionData.m_handlingFlags = m_nHandlingFlags;
    m_transmissionData.m_fEngineAcceleration *= 0.4f;

    return (scanned == 35) ? 0 : -1;
}

int32 tBoatHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const char* p = line;
    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;

    int scanned = sscanf(p, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                         &m_fThrustY,
                         &m_fThrustZ,
                         &m_fThrustAppZ,
                         &m_fAqPlaneForce,
                         &m_fAqPlaneLimit,
                         &m_fAqPlaneOffset,
                         &m_fWaveAudioMult,
                         &m_vecMoveRes.x,
                         &m_vecMoveRes.y,
                         &m_vecMoveRes.z,
                         &m_vecTurnRes.x,
                         &m_vecTurnRes.y,
                         &m_vecTurnRes.z,
                         &m_fLookLRBehindCamHeight);

    return (scanned == 14) ? 0 : -1;
}

int32 tFlyingHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const char* p = line;

    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;

    int scanned = sscanf(p, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                         &m_fThrust,
                         &m_fThrustFallOff,
                         &m_fYaw,
                         &m_fYawStab,
                         &m_fSideSlip,
                         &m_fRoll,
                         &m_fRollStab,
                         &m_fPitch,
                         &m_fPitchStab,
                         &m_fFormLift,
                         &m_fAttackLift,
                         &m_fGearUpR,
                         &m_fGearDownL,
                         &m_fWindMult,
                         &m_fMoveRes,
                         &m_vecTurnRes.x,
                         &m_vecTurnRes.y,
                         &m_vecTurnRes.z,
                         &m_vecSpeedRes.x,
                         &m_vecSpeedRes.y,
                         &m_vecSpeedRes.z);

    return (scanned == 21) ? 0 : -1;
}

int32 tBikeHandlingData::InitFromData(int32 id, const char* line) {
    m_nVehicleId = id;

    const char* p = line;
    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    while (*p && isspace(*p)) p++;

    int scanned = sscanf(p, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                         &m_fLeanFwdCOM,
                         &m_fLeanFwdForce,
                         &m_fLeanBakCOM,
                         &m_fLeanBakForce,
                         &m_fMaxLean,
                         &m_fFullAnimLean,
                         &m_fDesLean,
                         &m_fSpeedSteer,
                         &m_fSlipSteer,
                         &m_fNoPlayerCOMz,
                         &m_fWheelieAng,
                         &m_fStoppieAng,
                         &m_fWheelieSteer,
                         &m_fWheelieStabMult,
                         &m_fStoppieStabMult);

    return (scanned == 15) ? 0 : -1;
}

void cHandlingDataMgr::ConvertDataToGameUnits(tHandlingData* h) {
    const auto t = &h->GetTransmission();

    t->m_fEngineAcceleration *= ACCEL_CONST;
    t->m_fMaxGearVelocity *= VELOCITY_CONST;
    h->m_fBrakeDeceleration *= ACCEL_CONST;
    h->m_fMassRecpr = 1.f / h->m_fMass;
    h->m_fBuoyancyConstant = h->m_fMass * 0.8f / (float)h->m_nPercentSubmerged;
    h->m_fCollisionDamageMultiplier = (h->m_fCollisionDamageMultiplier * 2000.f) / h->m_fMass;

    auto engineAccelLimit = t->m_fEngineAcceleration / 6.f;
    auto maxVelocity      = t->m_fMaxGearVelocity;
    while (maxVelocity > 0.f) {
        maxVelocity -= 0.01f;
        if (h->m_fDragMult >= 0.01f) {
            if ((sq(maxVelocity) * (h->m_fDragMult / 2.f) / 1000.f) <= engineAccelLimit) {
                break;
            }
            continue;
        }

        auto fRecip = 1.f / (sq(maxVelocity) * h->m_fDragMult + 1.f);
        if ((fRecip - 1.f) * -maxVelocity <= engineAccelLimit) {
            break;
        }
    }

    if (h->m_nVehicleId == VT_RCBANDIT) {
        t->m_fMaxVelocity    = maxVelocity;
        t->m_maxReverseGearVelocity = -maxVelocity;
    } else if (h->m_bUseMaxspLimit) {
        t->m_fMaxVelocity    = maxVelocity / 1.2f;
        t->m_maxReverseGearVelocity = std::min(-t->m_fMaxVelocity / 4.f, -0.2f);
    } else {
        t->m_fMaxGearVelocity     = maxVelocity * 1.2f;
        t->m_fMaxVelocity = maxVelocity;
        if (GetBikeHandlingPointer(h->m_nVehicleId)) {
            // 2 wheelers
            t->m_maxReverseGearVelocity = -0.05f;
        } else {
            t->m_maxReverseGearVelocity = std::min(-maxVelocity * 0.3f, -0.2f);
        }
    }

    t->m_fEngineAcceleration /= (t->m_nDriveType == '4') ? 4.f : 2.f;
    t->InitGearRatios();
}

void cHandlingDataMgr::ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling) {
    bikeHandling->m_fMaxLean = sinf(DegreesToRadians(bikeHandling->m_fMaxLean));
    bikeHandling->m_fFullAnimLean = DegreesToRadians(bikeHandling->m_fFullAnimLean);
    bikeHandling->m_fWheelieAng = sinf(DegreesToRadians(bikeHandling->m_fWheelieAng));
    bikeHandling->m_fStoppieAng = sinf(DegreesToRadians(bikeHandling->m_fStoppieAng));
}

// get handling id by name
int32 cHandlingDataMgr::GetHandlingId(const char* nameToFind) {
    auto it = CVehicleNames::VehicleNames.find(nameToFind);
    if (it != CVehicleNames::VehicleNames.end()) {
        return it->second - 400;
    }

    DLOG("Can't find handling %s", nameToFind);
    return 0;
}

tFlyingHandlingData* cHandlingDataMgr::GetFlyingPointer(uint8 handlingId) {
    auto it = m_aFlyingHandling.find(handlingId);
    return it != m_aFlyingHandling.end() ? &it->second : nullptr;
}

// 0x6F5300
tBoatHandlingData* cHandlingDataMgr::GetBoatPointer(uint8 handlingId) {
    auto it = m_aBoatHandling.find(handlingId);
    return it != m_aBoatHandling.end() ? &it->second : nullptr;
}

tHandlingData* cHandlingDataMgr::GetVehiclePointer(uint32 handlingId) {
    auto it = m_aVehicleHandling.find(handlingId);
    return it != m_aVehicleHandling.end() ? &it->second : nullptr;
}

tBikeHandlingData* cHandlingDataMgr::GetBikeHandlingPointer(uint32 handlingId) {
    auto it = m_aBikeHandling.find(handlingId);
    return it != m_aBikeHandling.end() ? &it->second : nullptr;
}

int32 GetHandlingId_hooked(uintptr* thiz, const char* nameToFind) {
    return cHandlingDataMgr::GetHandlingId(nameToFind);
}

tFlyingHandlingData* (*defHandlingDataMgr__GetFlyingPointer_orig)(defHandlingDataMgr* thiz, uint8 plane);
tFlyingHandlingData* defHandlingDataMgr__GetFlyingPointer(defHandlingDataMgr* thiz, uint8 plane) {
    if (isHandlingLoaded) {
        return defHandlingDataMgr__GetFlyingPointer_orig(thiz, plane);
    }
    return cHandlingDataMgr::GetFlyingPointer(plane);
}

tBoatHandlingData* (*defHandlingDataMgr__GetBoatPointer_orig)(defHandlingDataMgr* thiz, uint8 boat);
tBoatHandlingData* defHandlingDataMgr__GetBoatPointer(defHandlingDataMgr* thiz, uint8 boat) {
    if (isHandlingLoaded) {
        return defHandlingDataMgr__GetBoatPointer_orig(thiz, boat);
    }
    return cHandlingDataMgr::GetBoatPointer(boat);
}

void cHandlingDataMgr::InjectHooks() {
    CHook::Redirect("_ZN16cHandlingDataMgr13GetHandlingIdEPKc", &GetHandlingId_hooked);
    CHook::InlineHook("_ZN16cHandlingDataMgr16LoadHandlingDataEv", &LoadHandlingData, &LoadHandlingData_orig);
    CHook::InlineHook("_ZN16cHandlingDataMgr14GetBoatPointerEh", &defHandlingDataMgr__GetBoatPointer, &defHandlingDataMgr__GetBoatPointer_orig);
    CHook::InlineHook("_ZN16cHandlingDataMgr16GetFlyingPointerEh", &defHandlingDataMgr__GetFlyingPointer, &defHandlingDataMgr__GetFlyingPointer_orig);
}