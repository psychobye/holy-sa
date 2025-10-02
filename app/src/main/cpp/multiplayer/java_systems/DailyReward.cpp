//
// Created on 31.01.2023.
//

#include "DailyReward.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

void CDailyReward::Show(int day, int second)
{
    auto env = CJavaWrapper::GetEnv();

    if(!CDailyReward::thiz)
        Constructor();

    jmethodID method = env->GetMethodID(clazz, "show", "(II)V");
    env->CallVoidMethod(CDailyReward::thiz, method, day, second);
}

void CDailyReward::ReceivePacket(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t day;
    int16_t second;

    bs.Read(day);
    bs.Read(second);

    CDailyReward::Show(day, second);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_DailyReward_nativeClick(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_DAILY_REWARDS);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_DailyReward_nativeOnDestroy(JNIEnv *env, jobject thiz) {
    CDailyReward::DeleteCppObject();
}