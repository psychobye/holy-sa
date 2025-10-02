//
// Created by �������� ���� on 26.04.2023.
//
#include "TheftAuto.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "Notification.h"

void CTheftAuto::startRendering() {
    if(!CTheftAuto::thiz)
        Constructor();

    CTheftAuto::show();

}

void CTheftAuto::show() {
    if(CTheftAuto::thiz == nullptr)return;

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(clazz, "showRendering", "()V");
    env->CallVoidMethod(CTheftAuto::thiz, method);

    CTheftAuto::bIsShow = true;
}

void CNetGame::packetTheftAuto(Packet* p)
{
    CTheftAuto::startRendering();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_theft_1auto_TheftAuto_finishRendering(JNIEnv *env, jobject thiz,
                                                                 jint status) {
    if (status == 1) {
        uint8_t packet = ID_CUSTOM_RPC;
        uint8_t RPC = RPC_THEFT_AUTO;

        RakNet::BitStream bsSend;
        bsSend.Write(packet);
        bsSend.Write(RPC);

        pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);

//        CNotification::show(type, (char *) utf_str, time, actionId);
    } else {
        char str[256] = "������� ���������! ���������� ���";
        CNotification::show(0, (char *) str, 5, -1, "-", "-");
    }

    CTheftAuto::DeleteCppObject();
}