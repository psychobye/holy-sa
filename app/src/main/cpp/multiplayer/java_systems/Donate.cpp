//
// Created on 16.02.2023.
//

#include "Donate.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "Treasure.h"

void CDonate::updateBalance(int balance)
{
    if(!CDonate::thiz)
        return;

    JNIEnv *env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(CDonate::clazz, "updateBalance", "(I)V");
    env->CallVoidMethod(CDonate::thiz, method, balance);
}

void CNetGame::packetShowDonateStash(Packet* p)
{
    if(!CDonate::thiz)
        CDonate::Constructor();

    CDonate::resetStash();

    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t count;
    bs.Read(count);

    uint8_t type;
    uint32_t value;
    uint32_t id;
    for(int i = 0; i < count; i++) {
        bs.Read(type);
        bs.Read(value);
        bs.Read(id);

        CDonate::addToMyItem(type, value, id);
    }
}

void CDonate::resetStash()
{
    if(CDonate::thiz == nullptr)return;

    JNIEnv *env = CJavaWrapper::GetEnv();

    jmethodID method = env->GetMethodID(CDonate::clazz, "resetStash", "()V");
    env->CallVoidMethod(CDonate::thiz, method);
}

void CDonate::addToMyItem(int type, int value, int id)
{
    if(CDonate::thiz == nullptr)return;

    JNIEnv *env = CJavaWrapper::GetEnv();

    jmethodID method = env->GetMethodID(CDonate::clazz, "addToMyItems", "(III)V");
    env->CallVoidMethod(CDonate::thiz, method, type, value, id);
}

void CNetGame::packetShowDonat(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t type;
    bs.Read(type);

    uint32_t balance;
    bs.Read(balance);

    if(type == 4)
    {// show
//        uint8_t myItems;
//        bs.Read(myItems);

        CDonate::Constructor();
        CDonate::updateBalance(balance);
    }
    else if (type == 5){
        CDonate::updateBalance(balance);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_donate_Donate_sendAction(JNIEnv *env, jobject thiz, jint action_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_DONATE;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write((uint8_t) action_id);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_donate_Donate_sendClickItem(JNIEnv *env, jobject thiz, jint action_type,
                                                       jint button_id, jint item_type,
                                                       jint item_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_DONATE;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write((uint8_t) action_type);
    bsSend.Write((uint8_t) button_id);
    bsSend.Write((uint16_t) item_type);
    bsSend.Write((uint16_t) item_id);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_donate_Donate_native_1clickStashItem(JNIEnv *env, jobject thiz,
                                                                jint click_id, jint sql_id) {

    if(click_id == CDonate::CLICK_ID_TREASURE) {
        CTreasure::Show(sql_id, true);
        return;
    }
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)      ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)      RPC_DONATE_STASH);
    bsSend.Write((uint8_t)      click_id);
    bsSend.Write((uint32_t)     sql_id);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_donate_Donate_native_1loadStash(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)      ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)      RPC_DONATE_STASH);
    bsSend.Write((uint8_t)      11);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_donate_Donate_nativeOnExit(JNIEnv *env, jobject thiz) {
    CDonate::DeleteCppObject();
}