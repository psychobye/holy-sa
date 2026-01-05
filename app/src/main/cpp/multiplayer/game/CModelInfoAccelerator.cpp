#include "CModelInfoAccelerator.h"
#include "CFileMgr.h"
#include "util/patch.h"

uint16_t* CModelInfoAccelerator::m_pArrayModelInfoIds = nullptr;
uint16_t  CModelInfoAccelerator::m_nModelInfosAdded = 0;
char      CModelInfoAccelerator::m_FileName[20] = {};
uint8_t   CModelInfoAccelerator::m_bHasRun = 0;
uint8_t   CModelInfoAccelerator::m_bFileFound = 0;

constexpr size_t MINFO_BIN_SIZE = 41100;

void CModelInfoAccelerator::Begin(const char* fileName)
{
    strcpy(CModelInfoAccelerator::m_FileName, fileName);

    if (CModelInfoAccelerator::m_bHasRun)
        return;

    auto* file = CFileMgr::OpenFile(CModelInfoAccelerator::m_FileName, "rb");
    CModelInfoAccelerator::m_bFileFound = false; // (file != nullptr);

    CModelInfoAccelerator::m_pArrayModelInfoIds = new uint16_t[MINFO_BIN_SIZE / sizeof(uint16_t)]();

    if (file)
    {
        // OS_FileRead(file, m_pArrayModelInfoIds, MINFO_BIN_SIZE);
        CFileMgr::CloseFile(file);
    }
}

void CModelInfoAccelerator::InjectHooks() {
    CHook::Redirect("_ZN21CModelInfoAccelerator5BeginEPc", &Begin);

    auto base = g_libGTASA + 0xB8D7E8;

    CHook::Write(base + 0x00, &m_pArrayModelInfoIds);
    CHook::Write(base + 0x08, &m_nModelInfosAdded);
    CHook::Write(base + 0x0A, &m_FileName);
    CHook::Write(base + 0x1E, &m_bHasRun);
    CHook::Write(base + 0x1F, &m_bFileFound);
}