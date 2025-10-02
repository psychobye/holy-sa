//
// Created on 30.07.2023.
//

#include "MineGame1.h"
#include "net/netgame.h"


void CMineGame1::Show() {
    if(thiz == nullptr)
        CMineGame1::Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    auto method = env->GetMethodID(clazz, "show", "()V");
    env->CallVoidMethod(thiz, method);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_MineGame1_nativeExit(JNIEnv *env, jobject thiz, jint exittype) {
    CMineGame1::DeleteCppObject();

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) RPC_SHOW_MINING_GAME);
    bsSend.Write((uint8_t) exittype);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}