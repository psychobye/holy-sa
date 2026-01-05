#include "common.h"

class SurfaceInfos_c {
public:
    static uint32 GetSurfaceIdFromName(SurfaceInfos_c *thiz, const char *surfaceName);

    static SurfaceInfos_c& Get() {
        static SurfaceInfos_c* pSurfaceInfos_c = nullptr;
        if (!pSurfaceInfos_c) {
            pSurfaceInfos_c = reinterpret_cast<SurfaceInfos_c*>(g_libGTASA + (VER_x32 ? 0x0 : 0xBD6B6C)); // TODO: x32
        }
        return *pSurfaceInfos_c;
    }
};