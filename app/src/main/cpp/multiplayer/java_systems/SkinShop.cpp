//
// Created on 18.08.2023.
//

#include "SkinShop.h"
#include "net/netgame.h"

void CSkinShop::Update(int type, int price) {
    if(thiz == nullptr)
        Constructor();

    auto env = g_pJavaWrapper->GetEnv();
    auto method = env->GetMethodID(clazz, "update", "(II)V");
    env->CallVoidMethod(thiz, method, type, price);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_SkinAndAcsShop_nativeSendClick(JNIEnv *env, jobject thiz, jint click_id) {
    pNetGame->SendCustomPacket(251, 42, click_id);
}