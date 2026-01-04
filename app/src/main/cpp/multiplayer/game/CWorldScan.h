#pragma once

#include "types.h"

class CWorldScan {
public:
    static void InjectHooks();

    static void ScanWorld(RwV2d* Points, int32_t NumPoints, void (*HitFunc)(int32_t, int32_t));
};