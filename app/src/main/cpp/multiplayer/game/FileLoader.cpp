//
// Created on 15.04.2023.
//

#include "FileLoader.h"
#include "../main.h"
#include "CFileMgr.h"
#include "game/Models/ModelInfo.h"
#include "game/Enums/eItemDefinitionFlags.h"
#include "util/patch.h"
#include "game/Models/AtomicModelInfo.h"
#include "Streaming.h"
#include "cHandlingDataMgr.h"

// Load line into static buffer (`ms_line`)
char* CFileLoader::LoadLine(FILE* file) {
   // if(!fgets(ms_line, sizeof(ms_line), file))
     //   return nullptr;

  //  return ms_line;

    if (!CFileMgr::ReadLine(file, ms_line, sizeof(ms_line)))
        return nullptr;

    // Sanitize it (otherwise random crashes appear)
    for (char* it = ms_line; *it; it++) {
        // Have to cast to uint8, because signed ASCII is retarded
        if ((uint8)*it < (uint8)' ' || *it == ',')
            *it = ' ';
    }

    return FindFirstNonNullOrWS(ms_line);
}

bool CFileMgr::ReadLine(FILESTREAM file, char *str, int32 num)
{
    return fgets(str, num, file) != nullptr;
}

/*!
* Load line from a text buffer with sanitization (replaces chars < 32 (space) with a space)
* @param bufferIt Iterator into buffer. It is modified by this function to point after the last character of this line
* @param buffSize Size of buffer. It is modified to represent the size of the buffer remaining after the end of this line
* @returns The beginning of the line - Note, this isn't a pointer into the passed in buffer!
* @addr 0x536FE0
*/
char* CFileLoader::LoadLine(char*& bufferIt, int32& buffSize) {
    return ( ( char*(*)(char*, int32) )(g_libGTASA + (VER_x32 ? 0x003EEFD8 + 1 : 0x4D0108)) )(bufferIt, buffSize);
}

char* CFileLoader::FindFirstNonNullOrWS(char* it) {
    // Have to cast to uint8, because signed ASCII is retarded
    for (; *it && (uint8)*it <= (uint8)' '; it++);
    return it;
}

void CFileLoader::LoadVehicleObject() {
    int32              modelId{ MODEL_INVALID };
    char               modelName[24]{};
    char               texName[24]{};
    char               type[8]{};
    char               handlingName[16]{};
    char               gameName[32]{};
    char               anims[16]{};
    char               vehCls[16]{};
    uint32             frq{}, flags{};
    tVehicleCompsUnion vehComps{};
    int32              misc{ -1 };
    float              wheelSizeFront{ 0.7f }, wheelSizeRear{ 0.7f };
    int32              wheelUpgradeCls{ -1 };

    const auto pFile = CFileMgr::OpenFile("SAMP/vehicles.ide", "rb");
    while (CFileLoader::LoadLine(pFile)) {
        if (strlen(CFileLoader::ms_line) == 0 || CFileLoader::ms_line[0] == ';' || CFileLoader::ms_line[0] == '#' || CFileLoader::ms_line[0] == '\r') {
            // Пропустить комментарии и пустые строки
            continue;
        }

        std::istringstream iss(CFileLoader::ms_line);

        iss >> modelId;
        iss >> modelName;
        iss >> texName;
        iss >> type;
        iss >> handlingName;
        iss >> gameName;
        iss >> anims;
        iss >> vehCls;
        iss >> frq;
        iss >> flags;

        if (iss >> std::hex >> vehComps.m_nComps) {
            if (iss >> misc) {
                if (iss >> wheelSizeFront) {
                    if (iss >> wheelSizeRear) {
                        iss >> wheelUpgradeCls;
                    }
                }
            }
        }

        if (!CVehicleNames::VehicleNames.contains(handlingName)) {
            CVehicleNames::VehicleNames[handlingName] = modelId;
        }
    }
}

int32 CFileLoader::LoadObject(const char* line) {
    int32  modelId{ MODEL_INVALID};
    char   modelName[24];
    char   texName[24];
    float  fDrawDist;
    uint32 nFlags;

    auto iNumRead = sscanf(line, "%d %s %s %f %d", &modelId, modelName, texName, &fDrawDist, &nFlags);
    if (iNumRead != 5 || fDrawDist < 4.0f)
    {
        int32 objType;
        float fDrawDist2_unused, fDrawDist3_unused;
        iNumRead = sscanf(line, "%d %s %s %d", &modelId, modelName, texName, &objType);
        if (iNumRead != 4)
            return -1;

        switch (objType)
        {
            case 1:
                VERIFY(sscanf(line, "%d %s %s %d %f %d", &modelId, (modelName), (texName), &objType, &fDrawDist, &nFlags) >= 5);
                break;
            case 2:
                VERIFY(sscanf(line, "%d %s %s %d %f %f %d", &modelId, (modelName), (texName), &objType, &fDrawDist, &fDrawDist2_unused, &nFlags) == 7);
                break;
            case 3:
                VERIFY(sscanf(line, "%d %s %s %d %f %f %f %d", &modelId, (modelName), (texName), &objType, &fDrawDist, &fDrawDist2_unused, &fDrawDist3_unused, &nFlags) == 8);
                break;
        }
    }

    sItemDefinitionFlags flags(nFlags);
    const auto mi = flags.bIsDamageable ? reinterpret_cast<CAtomicModelInfo *>(CModelInfo::AddDamageAtomicModel(modelId)) : CModelInfo::AddAtomicModel(modelId);
    mi->m_fDrawDistance = fDrawDist;

    mi->SetModelName(modelName);

    auto db = CStreaming::GetModelCDName(modelId);
    mi->SetTexDictionary(texName, db);

    SetAtomicModelInfoFlags(mi, nFlags);

    return modelId;
}

CEntity* CFileLoader::LoadObjectInstance1(const char* line) {
    char modelName[24];
    CFileObjectInstance instance;
    VERIFY(sscanf(
            line,
            "%d %s %d %f %f %f %f %f %f %f %d",
            &instance.m_nModelId,
            &modelName,
            &instance.m_nInstanceType,
            &instance.m_vecPosition.x,
            &instance.m_vecPosition.y,
            &instance.m_vecPosition.z,
            &instance.m_qRotation.x,
            &instance.m_qRotation.y,
            &instance.m_qRotation.z,
            &instance.m_qRotation.w,
            &instance.m_nLodInstanceIndex
    ) == 11);
    return LoadObjectInstance(&instance, modelName);
}

void CFileLoader::InjectHooks() {
    CHook::Redirect("_ZN11CFileLoader10LoadObjectEPKc", &CFileLoader::LoadObject);
    CHook::Redirect("_ZN11CFileLoader18LoadObjectInstanceEPKc", &CFileLoader::LoadObjectInstance1);
}

CEntity *CFileLoader::LoadObjectInstance(CFileObjectInstance *objInstance, const char *modelName) {
    return CHook::CallFunction<CEntity*>(g_libGTASA + (VER_x32 ? 0x003F059C + 1: 0x4D20FC), objInstance, modelName);
}
