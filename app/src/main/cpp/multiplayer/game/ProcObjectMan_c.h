#include "main.h"
#include "EntityItem_c.h"
#include "List_c.h"
#include "SurfaceInfos_c.h"

class ProcObjectMan_c {
public:
    int m_numAllocatedMatrices;        // 0x00
    int m_numProcSurfaceInfos;         // 0x04
    int m_procSurfaceInfos[128];       // 0x08
    EntityItem_c m_entityItems[512];   // 0x2C08
    List_c m_entityPool;               // 0x7C08

public:
    static void InjectHooks();
    static bool Init(ProcObjectMan_c *thisPtr);
    static void LoadDataFile(ProcObjectMan_c *thisPtr);

    static ProcObjectMan_c& Get() {
        static ProcObjectMan_c* pProcObjectMan_c = nullptr;
        if (!pProcObjectMan_c) {
            pProcObjectMan_c = reinterpret_cast<ProcObjectMan_c*>(g_libGTASA + (VER_x32 ? 0x0 : 0xC36618)); // TODO: x32
        }
        return *pProcObjectMan_c;
    }
};
VALIDATE_SIZE(ProcObjectMan_c, (VER_x32 ? 0xD00 : 0x5220));