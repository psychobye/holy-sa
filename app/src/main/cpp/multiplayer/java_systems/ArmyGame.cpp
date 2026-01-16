//
// Created on 28.07.2023.
//

#include "ArmyGame.h"
#include "util/CJavaWrapper.h"
#include "net/netgame.h"

void CArmyGame::Show() {
    auto env = g_pJavaWrapper->GetEnv();

    if(thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(clazz, "<init>","()V");
        thiz = env->NewObject(clazz, constructor);
        thiz = env->NewGlobalRef(thiz);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_ArmyGame_native_1onClose(JNIEnv *env, jobject thiz) {
    pNetGame->SendCustomPacket(251, 45, 1);

    env->DeleteGlobalRef(CArmyGame::thiz);
    CArmyGame::thiz = nullptr;
}