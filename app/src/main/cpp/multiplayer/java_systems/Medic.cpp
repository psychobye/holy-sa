//
// Created on 14.01.2023.
//

#include "Medic.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "util/CJavaWrapper.h"

void CMedic::ConstructIfNeed() {
    auto env = g_pJavaWrapper->GetEnv();
    if(thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(clazz, "<init>","()V");
        thiz = env->NewObject(clazz, constructor);
        thiz = env->NewGlobalRef(thiz);
    }
}

void CMedic::showPreDeath(char* nick, int id)
{
    CMedic::ConstructIfNeed();

    JNIEnv* env = CJavaWrapper::GetEnv();

    jstring jNick = env->NewStringUTF(nick);

    jmethodID method = env->GetMethodID(clazz, "showPreDeath", "(Ljava/lang/String;I)V");

    env->CallVoidMethod(CMedic::thiz, method, jNick, id);
    env->DeleteLocalRef(jNick);

    CMedic::bIsShow = true;
}

void CMedic::showMedGame(char* nick)
{
    CMedic::ConstructIfNeed();

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jstring jNick = env->NewStringUTF(nick);

    jmethodID method = env->GetMethodID(clazz, "showMiniGame", "(Ljava/lang/String;)V");

    env->CallVoidMethod(CMedic::thiz, method, jNick);
    env->DeleteLocalRef(jNick);

    CMedic::bIsShow = true;
}

void CMedic::hide()
{
    auto env = g_pJavaWrapper->GetEnv();

    if(thiz == nullptr)
        return;

    jmethodID method = env->GetMethodID(clazz, "hide", "()V");
    env->CallVoidMethod(CMedic::thiz, method);

    CMedic::bIsShow = false;

    env->DeleteGlobalRef(thiz);
    thiz = nullptr;
}



void CNetGame::packetPreDeath(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t toggle;
    uint16_t killerId;
    char* killername;

    bs.Read(toggle);
    bs.Read(killerId);

    if (toggle == 1) {
        if(CActorPool::GetAt(killerId))
            killername = CActorPool::GetName(killerId);
        else
            killername =  CPlayerPool::GetPlayerName(killerId);

        CMedic::showPreDeath(killername, killerId);
    }
    else {
        CMedic::hide();
    }
}

void CNetGame::packetMedGame(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t strLen;
    char nick[MAX_PLAYER_NAME];

    bs.Read(strLen);
    bs.Read(nick, strLen);
    nick[strLen] = '\0';

    char utf8_nick[MAX_PLAYER_NAME];
    cp1251_to_utf8(utf8_nick, nick);

    CMedic::showMedGame(utf8_nick);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_PreDeath_medicMiniGameExit(JNIEnv *env, jobject thiz, jint type_id) {
    CMedic::hide();

    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_MED_GAME;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write((uint8_t)type_id);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_PreDeath_medicPreDeathExit(JNIEnv *env, jobject thiz, jint button_id) {
    CMedic::hide();

    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_PRE_DEATH;
    uint8_t button = button_id;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}