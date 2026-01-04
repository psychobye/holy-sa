#include "CWorldScan.h"
#include "util/patch.h"

void CWorldScan::ScanWorld(RwV2d* Points, int32_t NumPoints, void (*HitFunc)(int32_t, int32_t)) {
    return CHook::CallFunction<void>("_ZN10CWorldScan9ScanWorldEP5RwV2diPFviiE", Points, NumPoints, HitFunc);
}

void CWorldScan::InjectHooks() { }