//
// Created by psychobye on 23.11.2025.
//
#pragma once

#include "BulletTrace.h"
#include "Enums/eWeaponType.h"

class CVector;
class CEntity;

struct CBulletPreset {
    RwRGBA  rgba;
    bool    isLaserGun;
    bool    isLongTracers;
    float   time;
    float   radius;
};

class CBulletTraces {
public:
    static inline std::array<CBulletTrace, 16> aTraces;
    static inline std::array<eWeaponType, 16> aTracesInfo;

    static inline std::unordered_map<int, CBulletPreset> bulletPresets;

public:
    static void InjectHooks();
    static void LoadTraceInfo();
    static CBulletPreset* GetBulletPreset(eWeaponType weaponType);

    static void Init();
    static void Update();
    static void AddTrace(const CVector& from, const CVector& to, eWeaponType weaponType, float radius, uint32 disappearTime, uint8 alpha);
    static void AddTrace(const CVector& from, const CVector& to, eWeaponType weaponType, CEntity* fromEntity);
    static void Render();

    static CBulletTrace* GetFree();
    static size_t        GetTraceIndex(CBulletTrace* pTrace) { return std::distance(std::begin(aTraces), pTrace); }
};