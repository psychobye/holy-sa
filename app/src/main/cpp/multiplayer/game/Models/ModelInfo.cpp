//
// Created on 07.03.2023.
//

#include "ModelInfo.h"
#include "util/patch.h"

CVehicleModelInfo* CModelInfo::AddVehicleModel(int index)
{
    // DLOG("AddVehicleModel %d", index);
    auto& pInfo = CModelInfo::ms_vehicleModelInfoStore.AddItem();

    ((void(*)(CVehicleModelInfo*))(g_libGTASA + (VER_x32 ? 0x386E98 + 1 : 0x45DF50)))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    CHook::SetVTable(&pInfo, g_libGTASA + (VER_x32 ? 0x006676A8 : 0x82F3B0));
    CHook::CallVTableFunctionByNum<void>(&pInfo, 7);
   // ((void(*)(CVehicleModelInfo*))(*(uintptr_t*)(pInfo.vtable + (VER_x32 ? 0x1C : 0x1C*2))))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CPedModelInfo* CModelInfo::AddPedModel(int index)
{
    // DLOG("CModelInfo_AddPedModel_hook %d", index);

    auto& pInfo = CModelInfo::ms_pedModelInfoStore.AddItem();

    ((void(*)(CPedModelInfo*))(g_libGTASA + (VER_x32 ? 0x00384FD8 + 1 : 0x45B3BC)))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    CHook::SetVTable(&pInfo, g_libGTASA + (VER_x32 ? 0x00667658 : 0x82F310));
    CHook::CallVTableFunctionByNum<void>(&pInfo, 7);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CDamageAtomicModelInfo* CModelInfo::AddDamageAtomicModel(int32 index)
{
    // DLOG("AddDamageAtomicModel %d", index);
    return CHook::CallFunction<CDamageAtomicModelInfo*>(g_libGTASA + (VER_x32 ? 0x00385F94 + 1 : 0x49BAE8), index);
}

CAtomicModelInfo* CModelInfo::AddAtomicModel(int index)
{
    // DLOG("AddAtomicModel %d", index);
    auto& pInfo = ms_atomicModelInfoStore.AddItem();

    ((void(*)(CAtomicModelInfo*))(g_libGTASA + (VER_x32 ? 0x00384FD8 + 1 : 0x45B3BC)))(&pInfo);

    CHook::SetVTable(&pInfo, g_libGTASA + (VER_x32 ? 0x00667444 : 0x82EEE8));
    CHook::CallVTableFunctionByNum<void>(&pInfo, 7);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CClumpModelInfo* CModelInfo::AddClumpModel(int32 index)
{
    // DLOG("AddClumpModel %d", index);
    auto& pInfo = ms_clumpModelInfoStore.AddItem();

    ((void(*)(CClumpModelInfo*))(g_libGTASA + (VER_x32 ? 0x00384FD8 + 1 : 0x45B3BC)))(&pInfo);

    CHook::SetVTable(&pInfo, g_libGTASA + (VER_x32 ? 0x667524 : 0x82F0A8));
    CHook::CallVTableFunctionByNum<void>(&pInfo, 7);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CWeaponModelInfo* CModelInfo::AddWeaponModel(int32 index)
{
    auto& pInfo = ms_weaponModelInfoStore.AddItem();

    ((void(*)(CWeaponModelInfo*))(g_libGTASA + (VER_x32 ? 0x00384FD8 + 1 : 0x45B3BC)))(&pInfo);

    CHook::SetVTable(&pInfo, g_libGTASA + (VER_x32 ? 0x6676F8 : 0x82F450));
    CHook::CallVTableFunctionByNum<void>(&pInfo, 7);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32* index)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    auto iCurInd = ms_lastPositionSearched;

    while (iCurInd < NUM_MODEL_INFOS) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            if (index) {
                *index = iCurInd;
            }
            return mi;
        }

        ++iCurInd;
    }

    iCurInd = ms_lastPositionSearched;
    if (iCurInd < 0)
        return nullptr;

    while (iCurInd >= 0) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            if (index) {
                *index = iCurInd;
            }
            return mi;
        }

        --iCurInd;
    }

    return nullptr;
}

CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32 minIndex, int32 maxIndex)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    if (minIndex > maxIndex)
        return nullptr;

    for (int32 i = minIndex; i <= maxIndex; ++i) {
        auto mi = GetModelInfo(i);
        if (mi && mi->m_nKey == iKey)
            return mi;
    }

    return nullptr;
}

int32 CModelInfo::GetModelInfoIndex(const char* name)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    auto iCurInd = ms_lastPositionSearched;

    while (iCurInd < NUM_MODEL_INFOS) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            return iCurInd;
        }

        ++iCurInd;
    }

    iCurInd = ms_lastPositionSearched;
    if (iCurInd < 0)
        return 0;

    while (iCurInd >= 0) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            return iCurInd;
        }

        --iCurInd;
    }

    return 0;
}

void CModelInfo::Initialise() {
    memset(ms_modelInfoPtrs, 0, sizeof(ms_modelInfoPtrs));
}

void CModelInfo::injectHooks()
{
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676B58 : 0x84B710), &ms_lastPositionSearched);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x676A34 : 0x84B4C8), &ms_atomicModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6773CC : 0x84C7E0), &ms_pedModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678C8C : 0x84F948), &ms_vehicleModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6796CC : 0x850DB8), &ms_modelInfoPtrs);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678538 : 0x84EA98), &ms_clumpModelInfoStore);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676F64 : 0x84BF20), &ms_weaponModelInfoStore);

    CHook::Redirect("_ZN10CModelInfo13AddClumpModelEi", &AddClumpModel);
    CHook::Redirect("_ZN10CModelInfo11AddPedModelEi", &AddPedModel);
    CHook::Redirect("_ZN10CModelInfo15AddVehicleModelEi", &AddVehicleModel);
    CHook::Redirect("_ZN10CModelInfo14AddAtomicModelEi", &AddAtomicModel);
    CHook::Redirect("_ZN10CModelInfo14AddWeaponModelEi", &AddWeaponModel);
}
