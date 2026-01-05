#include "SearchLights.h"
#include "Streaming.h"
#include "CFileMgr.h"
#include "FileLoader.h"
#include "Models/ModelInfo.h"

// TODO: implement functional

bool CSearchLights::bCatchLamppostsNow = false;

std::map<unsigned int, CLamppostInfo>  FileContent;
std::map<unsigned int, CLamppostInfo>* pFileContent = &FileContent;

bool CSearchLights::IsModelALamppost(uint16_t nModelId)
{
    auto it = pFileContent->lower_bound(PackKey(nModelId, 0));
    return it != pFileContent->end() && (it->first >> 16) == nModelId;
}

void CSearchLights::RegisterLamppost(CEntity *pEntity) {

}

void LoadDatFile()
{
    auto* fd = CFileMgr::OpenFile("data/SALodLights.dat", "r");
    if(fd)
    {
        unsigned short nModel = 0xFFFF, nCurIndexForModel = 0;
        const char* pLine;
        while((pLine = CFileLoader::LoadLine(fd)) != nullptr)
        {
            if (pLine[0] && pLine[0] != '#')
            {
                if (pLine[0] == '%')
                {
                    nCurIndexForModel = 0;
                    if (strcmp(pLine, "%additional_coronas") != 0)
                    {
                        CModelInfo::GetModelInfo(pLine + 1, reinterpret_cast<int32 *>(&nModel));
                    }
                    else
                    {
                        nModel = 0xFFFE;
                    }
                }
                else
                {
                    float fOffsetX, fOffsetY, fOffsetZ;
                    unsigned int nRed, nGreen, nBlue, nAlpha;
                    float fCustomSize = 1.0f;
                    float fDrawDistance = 0.0f;
                    int nNoDistance = 0;
                    int nDrawSearchlight = 0;
                    int nCoronaShowMode = 0;

                    if (sscanf(pLine, "%3d %3d %3d %3d %f %f %f %f %f %2d %1d %1d", &nRed, &nGreen, &nBlue, &nAlpha, &fOffsetX, &fOffsetY, &fOffsetZ, &fCustomSize, &fDrawDistance, &nCoronaShowMode, &nNoDistance, &nDrawSearchlight) != 12)
                    {
                        sscanf(pLine, "%3d %3d %3d %3d %f %f %f %f %2d %1d %1d", &nRed, &nGreen, &nBlue, &nAlpha, &fOffsetX, &fOffsetY, &fOffsetZ, &fCustomSize, &nCoronaShowMode, &nNoDistance, &nDrawSearchlight);
                    }
                    pFileContent->insert(std::make_pair(PackKey(nModel, nCurIndexForModel++), CLamppostInfo(CVector(fOffsetX, fOffsetY, fOffsetZ), CRGBA((uint8_t)nRed, (uint8_t)nGreen, (uint8_t)nBlue, (uint8_t)nAlpha), fCustomSize, nCoronaShowMode, nNoDistance, nDrawSearchlight, 0.0f, fDrawDistance)));
                }
            }
        }
        CSearchLights::bCatchLamppostsNow = true;
        Log("SALodLights.dat been processed...");
        CFileMgr::CloseFile(fd);
    }
    else
    {
        // CSearchLights::bRenderLodLights = false;
        // bRenderSearchlightEffects = false;
        Log("Failed to open SALodLights.dat!");
    }
}