#include "Occlusion.h"
#include "util/patch.h"

void COcclusion::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x67843C : 0x84E8A0), &Occluders);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678164 : 0x84E2F0), &NumOccludersOnMap);
}