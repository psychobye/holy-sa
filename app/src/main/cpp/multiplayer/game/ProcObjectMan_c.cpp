#include "ProcObjectMan_c.h"
#include "util/patch.h"
#include "CFileMgr.h"
#include "FileLoader.h"
#include "Models/ModelInfo.h"
#include <cstdio>
#include <cmath>
#include <cstdint>

bool ProcObjectMan_c::Init(ProcObjectMan_c *thisPtr)
{
    List_c *p_m_entityPool = &thisPtr->m_entityPool;

    for (auto & m_entityItem : thisPtr->m_entityItems) {
        p_m_entityPool->AddItem(&m_entityItem);
    }

    thisPtr->m_numProcSurfaceInfos = 0;

    ProcObjectMan_c::LoadDataFile(thisPtr);

    thisPtr->m_numAllocatedMatrices = 0;

    return true;
}

void ProcObjectMan_c::LoadDataFile(ProcObjectMan_c *thisPtr)
{
    auto fileHandle = CFileMgr::OpenFile("data/procobj.dat", "r");
    if (!fileHandle) return;

    auto line = CFileLoader::LoadLine(fileHandle);
    if (!line) {;
        CFileMgr::CloseFile(fileHandle);
        return;
    }

    int lineNum = 0;
    while (line) {
        ++lineNum;

        if (*line && *line != '#') {
            char surfaceName[64] = {0};
            char modelName[64] = {0};

            float sizeX = 0.0f, sizeY = 0.0f;
            int unknown1 = 0, unknown2 = 0;
            float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f, rotW = 0.0f;
            float zOffMin = 0.0f, zOffMax = 0.0f;
            int align = 0, useGrid = 0;

            int itemsRead = sscanf(
                    (const char *)line,
                    "%63s %63s %f %f %d %d %f %f %f %f %f %f %d %d",
                    surfaceName,
                    modelName,
                    &sizeX,
                    &sizeY,
                    &unknown1,
                    &unknown2,
                    &rotX,
                    &rotY,
                    &rotZ,
                    &rotW,
                    &zOffMin,
                    &zOffMax,
                    &align,
                    &useGrid
            );

            if (itemsRead < 14) {
                line = CFileLoader::LoadLine(fileHandle);
                continue;
            }

            if (thisPtr->m_numProcSurfaceInfos >= (int)(sizeof(thisPtr->m_procSurfaceInfos) / sizeof(thisPtr->m_procSurfaceInfos[0]))) {
                break;
            }

            auto& g_surfaceInfos = SurfaceInfos_c::Get();
            auto surfaceId = (unsigned char)SurfaceInfos_c::GetSurfaceIdFromName(&g_surfaceInfos, surfaceName);

            auto* procData = reinterpret_cast<float*>(&(thisPtr->m_procSurfaceInfos[thisPtr->m_numProcSurfaceInfos]));

            procData[0] = sizeX;
            procData[1] = (sizeX != 0.0f) ? (1.0f / (sizeX * sizeX)) : 0.0f;
            procData[2] = sizeY;
            procData[6] = fminf(sizeY, 80.0f);
            procData[7] = (float)unknown1 * (3.14159265f / 180.0f);
            procData[8] = (float)unknown2 * (3.14159265f / 180.0f);

            {
                using byte = unsigned char;
                byte* pBytes = reinterpret_cast<byte*>(procData);
                pBytes[8] = surfaceId;
            }

            bool modelOk = CModelInfo::GetModelInfo(modelName, reinterpret_cast<int*>(procData + 3));
            if (!modelOk) {
                Log("ProcObjectMan_c::LoadDataFile - line %d: model '%s' not found", lineNum, modelName);
            } else {
                procData[4] = sizeX;
                procData[5] = (sizeX != 0.0f) ? (1.0f / (sizeX * sizeX)) : 0.0f;

                procData[9]  = zOffMin;
                procData[10] = zOffMax;
                procData[11] = (float)align;
                procData[14] = rotX;
                procData[16] = (float)useGrid;
            }

            Log("ProcObjectMan_c::LoadDataFile - line %d: model='%s' surf='%s'(id=%u) pos(sizeX=%.3f,sizeY=%.3f) modelOk=%d",
                lineNum, modelName, surfaceName, (unsigned)surfaceId, sizeX, sizeY, modelOk ? 1 : 0);

            ++thisPtr->m_numProcSurfaceInfos;
        }

        line = CFileLoader::LoadLine(fileHandle);
    }

    CFileMgr::CloseFile(fileHandle);
}

void ProcObjectMan_c::InjectHooks() {
    CHook::Redirect("_ZN15ProcObjectMan_c12LoadDataFileEv", &LoadDataFile);
}