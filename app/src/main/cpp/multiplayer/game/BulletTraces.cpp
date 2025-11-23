//
// Created by psychobye on 23.11.2025.
//

#include "BulletTraces.h"
#include "Camera.h"
#include "util/patch.h"
#include "World.h"

#include "json.hpp"
#include <fstream>
#include <iostream>

void CBulletTraces::Render() {
    RxObjSpace3DVertex verts[6];
    for (CBulletTrace& trace : aTraces) {
        if (!trace.m_bExists) {
            continue;
        }

        // custom trace logic
        /*const size_t traceId = GetTraceIndex(&trace);
        const eWeaponType weaponType = aTracesInfo[traceId];
        if (auto* pBullet = GetBulletPreset(weaponType)) {

        }*/

        const float t = 1.0f - (float)trace.GetRemainingLifetime() / (float)trace.m_nLifeTime;

        {
            const auto camToOriginDir = (trace.m_vecStart - CCamera::Get().GetPosition()).Normalized();

            const auto up = camToOriginDir.Cross(trace.GetDirection().Normalized()).Normalized();
            const auto sizeVec = up * (trace.m_fRadius * t);
            const CVector currPosOnTrace = trace.m_vecEnd - trace.GetDirection() * t;

            const CVector vertPositions[std::size(verts)] = {
                    currPosOnTrace,
                    currPosOnTrace + sizeVec,
                    currPosOnTrace - sizeVec,

                    trace.m_vecEnd,
                    trace.m_vecEnd + sizeVec,
                    trace.m_vecEnd - sizeVec,
            };

            for (size_t idx = 0; idx < std::size(vertPositions); ++idx) {
                const auto& pos = vertPositions[idx];
                RwV3dAssign(RwIm3DVertexGetPos(&verts[idx]), &pos);
            }
        }

        for (auto& vert : verts) {
            RwIm3DVertexSetRGBA(&vert, 255, 255, 128, 0);
        }
        RwIm3DVertexSetRGBA(&verts[3], 255, 255, 128, (char)((float)trace.m_nTransparency * t)); // Only vertex 3 has non-zero alpha

        if (RwIm3DTransform(verts, std::size(verts), nullptr, rwIM3D_VERTEXRGBA)) {
            RwImVertexIndex indices[] = {
                    // Each row represents a triangle
                    4, 1, 3,
                    1, 0, 3,
                    0, 2, 3,
                    3, 2, 5
            };
            RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, indices, std::size(indices));
            RwIm3DEnd();
        }
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATECULLMODE,     RWRSTATE(rwCULLMODECULLBACK));
}

void PlayFrontEndSoundForTrace(CVector fromWorldSpace, CVector toWorldSpace) {
    CMatrix camMat = CCamera::Get().GetMatrix();
    const CVector& camPos = camMat.GetPosition();

    const auto fromRelToCam = fromWorldSpace - camPos;
    const auto toRelToCam = toWorldSpace - camPos;

    const float fromCSY = DotProduct(fromRelToCam, camMat.GetForward());
    const float toCSY = DotProduct(toRelToCam, camMat.GetForward());

    if (std::signbit(toCSY) == std::signbit(fromCSY)) {
        return;
    }

    const float fromCSX = DotProduct(fromRelToCam, camMat.GetRight());
    const float fromCSZ = DotProduct(fromRelToCam, camMat.GetUp());

    const float toCSX = DotProduct(toRelToCam, camMat.GetRight());
    const float toCSZ = DotProduct(toRelToCam, camMat.GetUp());

    const float t = fabs(fromCSY) / (fabs(fromCSY) + fabs(toCSY));

    const float pointOnLineZ = fromCSZ + (toCSZ - fromCSZ) * t;
    const float pointOnLineX = fromCSX + (toCSX - fromCSX) * t;

    const float camToLineDist = std::hypotf(pointOnLineZ, pointOnLineX);

    if (camToLineDist >= 2.0f) {
        return;
    }

    const auto ReportBulletAudio = [&](auto event) {
        const float volDistFactor = 1.0f - camToLineDist * 0.5f;
        const float volumeChange  = volDistFactor == 0.0f ? -100.0f : std::log10(volDistFactor);

        auto* audioEngine = (uintptr*)(g_libGTASA + (VER_x32 ? 0x94EC9C : 0xBB65F8));
        CHook::CallFunction<void>("_ZN12CAudioEngine24ReportFrontendAudioEventEiff", audioEngine, event, volumeChange, 1.0f);
    };

    const bool isComingFromBehind = fromCSY <= 0.0f;
    if (0.f <= pointOnLineX) {
        ReportBulletAudio(isComingFromBehind ? AE_FRONTEND_BULLET_PASS_RIGHT_REAR : AE_FRONTEND_BULLET_PASS_RIGHT_FRONT);
    } else {
        ReportBulletAudio(isComingFromBehind ? AE_FRONTEND_BULLET_PASS_LEFT_REAR : AE_FRONTEND_BULLET_PASS_LEFT_FRONT);
    }
}

CBulletTrace* CBulletTraces::GetFree() {
    for (CBulletTrace& trace : aTraces) {
        if (!trace.m_bExists) {
            return &trace;
        }
    }
    return nullptr;
}

void CBulletTraces::AddTrace(const CVector& from, const CVector& to, eWeaponType weaponType, float radius, uint32 disappearTime, uint8 alpha)
{
    if (CBulletTrace* pTrace = GetFree()) {
        pTrace->m_vecStart = from;
        pTrace->m_vecEnd = to;
        pTrace->m_nCreationTime = CTimer::GetTimeInMS();
        pTrace->m_nTransparency = alpha;
        pTrace->m_bExists = true;
        pTrace->m_fRadius = radius;

        const auto traceIdx = GetTraceIndex(pTrace);
        if (traceIdx < 10) {
            pTrace->m_nLifeTime = (uint32)(traceIdx < 5 ? disappearTime : disappearTime / 2.0f);
        } else {
            pTrace->m_nLifeTime = (uint32)(disappearTime / 4.0f);
        }
        aTracesInfo[traceIdx] = weaponType;
    }
    PlayFrontEndSoundForTrace(from, to);
}

void CBulletTraces::AddTrace(const CVector& posMuzzle, const CVector& posBulletHit, eWeaponType weaponType, CEntity* fromEntity)
{
    if (FindPlayerPed() == fromEntity || FindPlayerVehicle() && FindPlayerVehicle() == fromEntity) {
        switch (CCamera::GetActiveCamera().m_nMode) {
            case MODE_M16_1STPERSON:
            case MODE_SNIPER:
            case MODE_CAMERA:
            case MODE_ROCKETLAUNCHER:
            case MODE_ROCKETLAUNCHER_HS:
            case MODE_M16_1STPERSON_RUNABOUT:
            case MODE_SNIPER_RUNABOUT:
            case MODE_ROCKETLAUNCHER_RUNABOUT:
            case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
            case MODE_HELICANNON_1STPERSON: {
                if (FindPlayerEntity()->AsPhysical()->m_vecMoveSpeed.Magnitude() < 0.05f) {
                    return;
                }
            }
        }
    }

    CVector dir = posBulletHit - posMuzzle;
    const float traceLengthOriginal = dir.Magnitude();
    dir.Normalise();

    const float traceLengthNew = CGeneral::GetRandomNumberInRange(0.0f, traceLengthOriginal);
    const float fRadius = std::min(CGeneral::GetRandomNumberInRange(2.0f, 5.0f), traceLengthOriginal - traceLengthNew);

    CVector from = posMuzzle + dir * traceLengthNew;
    CVector to = from + dir * fRadius;

    AddTrace(
            from,
            to,
            weaponType,
            0.01f,
            300,
            70
    );
}

CBulletPreset* CBulletTraces::GetBulletPreset(eWeaponType weaponType) {
    auto it = bulletPresets.find(weaponType);
    if (it != bulletPresets.end()) {
        return &it->second;
    }

    DLOG("Can't find BulletPreset %d", weaponType);
    return nullptr;
}

void CBulletTraces::LoadTraceInfo() {
    Log("Loading BulletsPreset.json..");

    using json = nlohmann::json;

    std::string filename = std::string(g_pszStorage) + "SAMP/BulletsPreset.json";
    std::ifstream inputFile(filename.c_str());
    if (!inputFile.is_open()) {
        DLOG("not load filename: %s", filename.c_str());
        return;
    }

    try {
        json jsonData;
        inputFile >> jsonData;

        for (auto& [key, value] : jsonData.items()) {
            int weaponId = std::stoi(key);

            auto& color = value["color"];
            bulletPresets[weaponId].rgba.alpha = color["alpha"];
            bulletPresets[weaponId].rgba.red = color["red"];
            bulletPresets[weaponId].rgba.green = color["green"];
            bulletPresets[weaponId].rgba.blue = color["blue"];

            bulletPresets[weaponId].isLaserGun = value["laser_gun"];
            bulletPresets[weaponId].isLongTracers = value["long_tracers"];

            bulletPresets[weaponId].time = value.value("time", 0.0f);
            bulletPresets[weaponId].radius = value["radius"];
        }

    } catch (const std::exception& e) {
        DLOG("error parsing JSON: %s", e.what());
    }

    inputFile.close();
}

void AddTrace_hooked(CVector* posMuzzle, CVector* posBulletHit, eWeaponType weaponType, CEntity* fromEntity) {
    CBulletTraces::AddTrace(posMuzzle, posBulletHit, weaponType, fromEntity);
}

void CBulletTraces::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679ABC : 0x851598), &aTraces);

    CHook::Redirect("_ZN13CBulletTraces6RenderEv", &Render);
    CHook::Redirect("_ZN13CBulletTraces8AddTraceEP7CVectorS1_iP7CEntity", &AddTrace_hooked);
}