#pragma once

#include "game/Entity/Vehicle/Vehicle.h"
#include "game/RW/rwcore.h"
#include "game/Enums/OSDeviceForm.h"

class CVehicle;
class CPed;

class CUtil {
public:
    static bool IsValidGameVehicle(CVehicle *pVehicle);
    static bool IsValidGamePed(CPed * pPed);
    static void WorldRemoveEntity(CEntity* pEnt);

    static const char *GetWeaponName(int iWeaponID);

    static int32_t FindPlayerSlotWithPedPointer(CPed *pPlayersPed);

    static RwTexture* GetTexture(const char* name);

    static RwTexture *LoadTextureFromDB(const char *dbname, const char *texture);

    // Converts degrees to radians
    // keywords: 0.017453292 flt_8595EC
    constexpr static float DegreesToRadians(float angleInDegrees) {
        return angleInDegrees * PI / 180.0F;
    }

    // Converts radians to degrees
    // 57.295826
    constexpr static float RadiansToDegrees(float angleInRadians) {
        return angleInRadians * 180.0F / PI;
    }
    static float GetDistanceBetween3DPoints(const RwV3d f, const RwV3d s);
    static float GetDistanceBetween2DPoints(const RwV2d f, const RwV2d s) {
        return sqrt( (s.x-f.x)*(s.x-f.x) + (s.y-f.y)*(s.y-f.y) );
    }

    static OSDeviceForm OS_SystemForm();

    static void TransformPoint(RwV3d &result, const CSimpleTransform &t, const RwV3d &v);
    static int GameGetWeaponModelIDFromWeaponID(int iWeaponID);

    static void RenderEntity(CEntity *entity);

    static uintptr_t FindLib(const char *libname);

    static void ConvertARGBtoRGBA(const uint32_t *src, uint8_t *dst, size_t pixelCount);

    static int NextPow2(int v);
};

std::string cp1251_to_utf8(const std::string& in);
void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);
std::string cp1251_to_utf8(const char* in);

std::string ConvertColorToHtml(const std::string& format);
float fixAngle(float angle);