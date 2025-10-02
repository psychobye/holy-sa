//
// Created on 03.07.2023.
//

#include "LuckyWheel.h"
#include "util/CJavaWrapper.h"
#include "raknet/PacketEnumerations.h"
#include "net/netgame.h"

void CLuckyWheel::Show(int count, int time) {
    if(!thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();

    auto method = env->GetMethodID(clazz, "show","(II)V");
    env->CallVoidMethod(thiz, method, count, time);

    CLuckyWheel::bIsShow = true;
}

void CLuckyWheel::packetShow(Packet *p) {
    RakNet::BitStream bs((unsigned char *) p->data, p->length, false);
    bs.IgnoreBits(40);

    uint32_t count;
    uint32_t time;

    bs.Read(count);
    bs.Read(time);

    CLuckyWheel::Show(count, time);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_casino_LuckyWheel_nativeSendClick(JNIEnv *env, jobject thiz, jint button_id) {
    CLuckyWheel::Destroy();

    if (button_id == 228) {// ������
        return;
    }
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_CASINO_LUCKY_WHEEL_MENU;
    uint8_t button = button_id;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}