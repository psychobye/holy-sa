#pragma once
#include "font.h"
#include "Cam.h"
#include <string>

class CFirstPersonCamera
{
    static bool m_bEnabled;
public:
    static void MakePlayerFaceCameraDirection(CCam* pCam, CPedSamp* player);
    static void ProcessCameraOnFoot(CCam* pCam, CPedSamp* pPed);
    static void ProcessCameraInVeh(CCam* pCam, CPedSamp* pPed, CVehicleSamp* pVeh);

    static void SetEnabled(bool bEnabled);
    static void Toggle();
    static bool IsEnabled();

    static void Update();
    static float g_fTonerLerp;
};
