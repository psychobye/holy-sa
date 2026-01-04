//
// Created on 15.11.2023.
//

#ifndef RUSSIA_RENDERER_H
#define RUSSIA_RENDERER_H

#include "game/common.h"

constexpr auto MAX_INVISIBLE_ENTITY_PTRS = 150u;
constexpr auto MAX_VISIBLE_ENTITY_PTRS   = 1000u;
constexpr auto MAX_VISIBLE_LOD_PTRS      = 1000u;
constexpr auto MAX_VISIBLE_SUPERLOD_PTRS = 50u;

class CRenderer {
public:
    static inline bool ms_bRenderOutsideTunnels;
    static inline bool m_loadingPriority;

    static inline CEntity* ms_aVisibleEntityPtrs[MAX_VISIBLE_ENTITY_PTRS];
    static inline int32 ms_nNoOfVisibleEntities;

    enum class VisibilityState : uint8_t {
        ENTITY_NOT_VISIBLE = 0,
        ENTITY_VISIBLE = 1,
        ENTITY_STREAM_MODEL = 2,
    };

public:
    static void InjectHooks();

    static void ScanWorld();
    static void RenderFadingInEntities();
    static void RenderFadingInUnderwaterEntities();
    static void RenderRoads();
    static void RenderEverythingBarRoads();

    static void ScanSectorList(int32 BlockX, int32 BlockY);
    static void ScanBigBuildingList(int32 BlockX, int32 BlockY);
};


#endif //RUSSIA_RENDERER_H
