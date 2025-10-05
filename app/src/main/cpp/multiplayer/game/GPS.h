#ifndef LR64_GPS_H
#define LR64_GPS_H
#include "jni.h"
#include "main.h"
#include "Vector.h"

class GPS {
public:
    static inline CVector to;
    static void DoPathDraw();
    static void Set(CVector pos, bool toggle);
    static bool enabled;

};


#endif //LR64_GPS_H
