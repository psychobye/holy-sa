//
// Created on 22.07.2023.
//

#include "FuelStation.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_FuelStation_nativeSendClick(JNIEnv *env, jobject thiz,
                                                       jint fueltype, jint fuelliters) {
    pNetGame->SendCustomPacketFuelData(251, 39, fueltype, fuelliters);

    env->DeleteGlobalRef(CFuelStation::thiz);
    CFuelStation::thiz = nullptr;
}

void CFuelStation::Show(int type, int price1, int price2, int price3, int price4, int price5,
                        int maxCount) {
    auto env = g_pJavaWrapper->GetEnv();

    if(thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(clazz, "<init>", "(IIIIIII)V");
        thiz = env->NewObject(clazz, constructor, type, price1, price2, price3, price4, price5, maxCount);
        thiz = env->NewGlobalRef(thiz);
    }

}
