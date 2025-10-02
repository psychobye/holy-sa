//
// Created on 14.07.2023.
//

#include "GunStore.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

void CGunStore::Show() {
    auto env = g_pJavaWrapper->GetEnv();

    if(CGunStore::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CGunStore::clazz, "<init>","()V");
        CGunStore::thiz = env->NewObject(CGunStore::clazz, constructor);
        CGunStore::thiz = env->NewGlobalRef(CGunStore::thiz);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_GunShop_nativeSendButt(JNIEnv *env, jobject thiz, jint butt_id) {
    if(butt_id == 0) { // exit
        env->DeleteGlobalRef(CGunStore::thiz);
        CGunStore::thiz = nullptr;
    }
    pNetGame->SendCustomPacket(251, 44, butt_id);
}