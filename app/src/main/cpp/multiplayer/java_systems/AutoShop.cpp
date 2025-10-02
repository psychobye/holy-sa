//
// Created on 29.06.2023.
//

#include "AutoShop.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include <jni.h>

void CAutoShop::toggle(bool toggle) {
    CAutoShop::Constructor();

    if(!toggle && thiz != nullptr) {
        auto env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(CAutoShop::clazz, "hide", "()V");
        env->CallVoidMethod(CAutoShop::thiz, method);

        env->DeleteGlobalRef(CAutoShop::thiz);
        CAutoShop::thiz = nullptr;
    }

    bIsShow = toggle;
}

void CAutoShop::updateInfo(char* name, int price, int count, float maxspeed, float acceleration, int gear) {

    CAutoShop::Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    jstring j_name = env->NewStringUTF(name);

    jmethodID method = env->GetMethodID(CAutoShop::clazz, "update", "(Ljava/lang/String;IIFFI)V");
    env->CallVoidMethod(CAutoShop::thiz, method, j_name, price, count, maxspeed, acceleration, gear);

    env->DeleteLocalRef(j_name);
}

void CAutoShop::Packet_UpdateAutoShop(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40);

    uint32_t price;
    uint32_t count;
    float maxspeed;
    float acceleration;
    uint8_t len;
    uint8_t gear;

    bs.Read(len);
    char name[len + 1];
    bs.Read(name, len);
    name[len] = '\0';

    bs.Read(price);
    bs.Read(count);
    bs.Read(maxspeed);
    bs.Read(acceleration);
    bs.Read(gear);

    char utf8[len + 1];
    cp1251_to_utf8(utf8, name);


    CAutoShop::updateInfo(utf8, price, count, maxspeed, acceleration, gear);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_AutoShop_native_1SendAutoShopButton(JNIEnv *env, jobject thiz,
                                                               jint button_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_CLICK_AUTOSHOP;
    uint8_t button = button_id;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}