//
// Created on 18.09.2023.
//

#include "GameFilesCheck.h"
#include "../util/CJavaWrapper.h"
#include "raknet/BitStream.h"
#include "net/netgame.h"

void CGameFilesCheck::RequestChecked() {
    auto env = g_pJavaWrapper->GetEnv();
    auto clazz = env->GetObjectClass(g_pJavaWrapper->activity);

    auto method = env->GetStaticMethodID(clazz, "requestGameFilesCheck", "()V");
    env->CallStaticVoidMethod(clazz, method);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_core_Samp_00024Companion_gameFilesChecked(JNIEnv *env, jobject clazz, jboolean ok) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_REQUEST_CHECK_FILES);
    bsSend.Write((uint8_t)  ok);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}