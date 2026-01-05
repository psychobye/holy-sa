#pragma once
#include "common.h"
#include <cstdint>

class CModelInfoAccelerator {
public:
    static uint16_t* m_pArrayModelInfoIds; // 0x00 (DCQ)
    static uint16_t  m_nModelInfosAdded;   // 0x08 (DCW)
    static char      m_FileName[20];       // 0x0A
    static uint8_t   m_bHasRun;            // 0x1E
    static uint8_t   m_bFileFound;         // 0x1F

public:
    static void InjectHooks();

    static void Begin(const char* fileName);

    static CModelInfoAccelerator& Get() {
        return *reinterpret_cast<CModelInfoAccelerator*>(g_libGTASA + (VER_x32 ? 0x0 : 0xB8D7E8));
    }
};