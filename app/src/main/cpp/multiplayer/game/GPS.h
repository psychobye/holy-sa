#include "jni.h"
#include "main.h"
#include "Vector.h"
#include "NodeAddress.h"
#include "PathFind.h"

#define MAX_NODE_POINTS 2000
#define GPS_LINE_WIDTH  6.0f
#define GPS_LINE_R  200
#define GPS_LINE_G  0
#define GPS_LINE_B  200
#define GPS_LINE_A  255

class GPS {
public:
    static inline CVector to;
    static void DoPathDraw();
    static void Set(CVector pos, bool toggle);
    static bool enabled;

    static inline CNodeAddress resultNodes[MAX_NODE_POINTS];
    static inline CVector2D nodePoints[MAX_NODE_POINTS];
    static inline RwIm2DVertex lineVerts[MAX_NODE_POINTS * 4];

    static inline CNodeAddress aNodesToBeCleared_NEW[MAX_NODE_POINTS];

    static void Setup2DVertex(RwIm2DVertex &vertex, float x, float y);
};