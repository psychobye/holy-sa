//
// Created on 21.01.2023.
//

#include "DuelsGui.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

void CDuelsGui::clearKillList()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "clearKillList", "()V");

    env->CallVoidMethod(CDuelsGui::thiz, method);
}

void CDuelsGui::addMessage(PLAYERID killer, PLAYERID killee, int reason, int team)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;
    if(!pNetGame)return;

    jstring jKillername = env->NewStringUTF( CPlayerPool::GetPlayerName(killer) );
    jstring jKilleename = env->NewStringUTF( CPlayerPool::GetPlayerName(killee) );

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "addItem", "(Ljava/lang/String;Ljava/lang/String;II)V");

    env->CallVoidMethod(CDuelsGui::thiz, method, jKillername, jKilleename, reason, team);
    env->DeleteLocalRef(jKilleename);
    env->DeleteLocalRef(jKillername);
}

void CDuelsGui::addTop(PLAYERID top1, PLAYERID top2, PLAYERID top3, bool show)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;
    if(!pNetGame)return;

    jstring top1name = env->NewStringUTF( CPlayerPool::GetPlayerName(top1) );
    jstring top2name = env->NewStringUTF( CPlayerPool::GetPlayerName(top2) );
    jstring top3name = env->NewStringUTF( CPlayerPool::GetPlayerName(top3) );

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "addTop", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");

    env->CallVoidMethod(CDuelsGui::thiz, method, top1name, top2name, top3name, show);
    env->DeleteLocalRef(top1name);
    env->DeleteLocalRef(top2name);
    env->DeleteLocalRef(top3name);


}

void CDuelsGui::addStatistic(PLAYERID kills, PLAYERID deaths, bool show)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;
    if(!pNetGame)return;

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "addStatistic", "(IIZ)V");

    env->CallVoidMethod(CDuelsGui::thiz, method, kills, deaths, show);
}

void CDuelsGui::showKillsLeft(bool show, int kills, int needKills)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "showKillsLeft", "(ZII)V");

    env->CallVoidMethod(CDuelsGui::thiz, method, show, kills, needKills);
}

void CNetGame::packetDuelsKillsLeft(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t show;
    uint8_t kills;
    uint8_t needKiils;

    bs.Read(show);
    bs.Read(kills);
    bs.Read(needKiils);

    CDuelsGui::showKillsLeft(show, kills, needKiils);
}

void CNetGame::packetDuelsTop(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t show;
    uint16_t top1;
    uint16_t top2;
    uint16_t top3;

    bs.Read(show);
    bs.Read(top1);
    bs.Read(top2);
    bs.Read(top3);

    CDuelsGui::addTop(top1, top2, top3, show);
}

void CNetGame::packetDuelsStatistic(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t show;
    uint16_t kills;
    uint16_t deaths;

    bs.Read(show);
    bs.Read(kills);
    bs.Read(deaths);

    CDuelsGui::addStatistic(kills, deaths, show);
}

void CNetGame::packetKillList(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t killer;
    uint16_t killee;
    uint16_t reason;
    uint8_t team;

    bs.Read(killer);
    bs.Read(killee);
    bs.Read(reason);
    bs.Read(team);

    CDuelsGui::addMessage(killer, killee, reason, team);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_DuelsHud_init(JNIEnv *env, jobject thiz) {
    CDuelsGui::thiz = env->NewGlobalRef(thiz);
}