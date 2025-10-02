//
// Created on 13.07.2023.
//

#include "Samwill.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

void CSamwill::Show() {
    auto env = g_pJavaWrapper->GetEnv();

    if(CSamwill::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CSamwill::clazz, "<init>", "()V");
        CSamwill::thiz = env->NewObject(CSamwill::clazz, constructor);
        CSamwill::thiz = env->NewGlobalRef(CSamwill::thiz);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Samwill_nativeSendExit(JNIEnv *env, jobject thiz,
                                                         jint samwillpacket) {
    pNetGame->SendCustomPacket(251, 20, samwillpacket);

    env->DeleteGlobalRef(CSamwill::thiz);
    CSamwill::thiz = nullptr;
}