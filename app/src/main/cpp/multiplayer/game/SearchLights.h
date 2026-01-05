#include <vector>
#include <map>
#include "rgba.h"
#include "Vector.h"
#include "common.h"

class CLamppostInfo
{
public:
    CVector         vecPos;
    CRGBA           colour;
    float           fCustomSizeMult;
    int             nNoDistance;
    int             nDrawSearchlight;
    float           fHeading;
    int             nCoronaShowMode;
    float           fObjectDrawDistance;

    CLamppostInfo(const CVector& pos, const CRGBA& col, float fCustomMult, int CoronaShowMode, int nNoDistance, int nDrawSearchlight, float heading, float ObjectDrawDistance = 0.0f)
            : vecPos(pos), colour(col), fCustomSizeMult(fCustomMult), nCoronaShowMode(CoronaShowMode), nNoDistance(nNoDistance), nDrawSearchlight(nDrawSearchlight), fHeading(heading), fObjectDrawDistance(ObjectDrawDistance)
    {}
};

extern std::vector<CLamppostInfo>* m_pLampposts;
extern std::map<unsigned int, CLamppostInfo>* pFileContent;

inline uint32_t PackKey(unsigned short nModel, unsigned short nIndex)
{
    return nModel << 16 | nIndex;
}

class CSearchLights {
public:
    static bool bCatchLamppostsNow;

    static bool IsModelALamppost(unsigned short nModelId);

    static void RegisterLamppost(CEntity *pEntity);
};