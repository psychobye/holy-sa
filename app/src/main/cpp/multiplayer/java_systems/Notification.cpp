//
// Created on 01.02.2023.
//

#include "Notification.h"
#include <jni.h>
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

void CNotification::hide() {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    jclass clazz = env->GetObjectClass(CNotification::thiz);
    jmethodID method = env->GetMethodID(clazz, "hide", "(Z)V");

    env->CallVoidMethod(CNotification::thiz, method, true);
}

void CNotification::show(int type, const char* text, int duration, int actionId, const char* butt1, const char* butt2) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    char utfchar[255];
    cp1251_to_utf8(utfchar, text);
    jstring jtext = env->NewStringUTF(utfchar);

    char utfbutt1[123];
    cp1251_to_utf8(utfbutt1, butt1);
    jstring jbutt1 = env->NewStringUTF(utfbutt1);

    char utfbutt2[123];
    cp1251_to_utf8(utfbutt2, butt2);
    jstring jbutt2 = env->NewStringUTF(utfbutt2);

    jclass clazz = env->GetObjectClass(CNotification::thiz);
    jmethodID method = env->GetMethodID(clazz, "ShowNotification",
                                        "(ILjava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");

    env->CallVoidMethod(CNotification::thiz, method, type, jtext, duration, actionId, jbutt1, jbutt2);
    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(jbutt1);
    env->DeleteLocalRef(jbutt2);
}

void CNetGame::packetNotification(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t type;
    uint8_t len;

    uint8_t time;
    uint16_t actionId;
    bs.Read(type);

    if (type == 65535){
        CNotification::hide();
        return;
    }
    bs.Read(len);

    char str[len + 1];
    bs.Read(str, len);
    str[len] = '\0';

    bs.Read(time);
    bs.Read(actionId);

    bs.Read(len);
    char butt1[len + 1];
    bs.Read(butt1, len);
    butt1[len] = '\0';

    bs.Read(len);
    char butt2[len + 1];
    bs.Read(butt2, len);
    butt2[len] = '\0';


    CNotification::show(type, str, time, actionId, butt1, butt2);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Notification_sendClick(JNIEnv *env, jobject thiz, jint action_id,
                                                  jint button_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_ACTION_LABEL;
    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write( (uint16_t)action_id);
    bsSend.Write( (uint8_t)button_id);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Notification_init(JNIEnv *env, jobject thiz) {
    CNotification::thiz = env->NewGlobalRef(thiz);
}