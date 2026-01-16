//
// Created on 30.07.2023.
//

#include "MineGame2.h"
#include "net/netgame.h"

void CMineGame2::Show() {
    if(thiz == nullptr)
        CMineGame2::Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    auto method = env->GetMethodID(clazz, "show", "()V");
    env->CallVoidMethod(thiz, method);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_MineGame2_nativeExit(JNIEnv *env, jobject thiz, jint type) {
    CMineGame2::DeleteCppObject();

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) RPC_SHOW_MINING_GAME);
    bsSend.Write((uint8_t) type);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}