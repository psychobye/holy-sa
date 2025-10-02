//
// Created on 06.12.2022.
//

#include "Chip.h"
#include <jni.h>

#include "main.h"

#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

#include "java_systems/HUD.h"

void hide();

void CNetGame::packetCasinoChip(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint8_t type;
    uint32_t balance;

    bs.Read(type);
    bs.Read(balance);

    if(!type) {
        balance = CHUD::iLocalMoney;
    }
    CChip::show(type, balance);
}

void CChip::show(bool isSell, int balance){
    auto env = g_pJavaWrapper->GetEnv();

    if(CChip::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CChip::clazz, "<init>", "(ZI)V");
        CChip::thiz = env->NewObject(CChip::clazz, constructor, isSell, balance);
        CChip::thiz = env->NewGlobalRef(CChip::thiz);
    }

    CChip::bIsShow = true;
}

void CChip::hide() {
    CChip::bIsShow = false;

    auto env = g_pJavaWrapper->GetEnv();
    env->DeleteGlobalRef(CChip::thiz);
    CChip::thiz = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_casino_BuySellChip_native_1sendClick(JNIEnv *env, jobject thiz,
                                                                jint button_id, jlong input,
                                                                jboolean isSell) {
    CChip::hide();

    if(button_id == 2) // exit
        return;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_CASINO_BUY_CHIP);
    bsSend.Write((uint8_t)isSell);
    bsSend.Write((uint8_t)button_id);
    bsSend.Write((uint32_t)input);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}