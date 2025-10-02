//
// Created on 16.04.2023.
//

#include "Dice.h"
#include "net/netgame.h"
#include "java_systems/HUD.h"

void CDice::TempToggle(bool toggle)
{
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(thiz != nullptr) {
        jmethodID method = env->GetMethodID(clazz, "tempToggle", "(Z)V");
        env->CallVoidMethod(thiz, method, toggle);
    }
}

void CDice::Update(DicePacket *data) {

    bIsShow = true;

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(!thiz)
        Constructor();

    jstring jPlayerName[MAX_PLAYERS_CASINO_DICE];

    for(int i = 0; i < MAX_PLAYERS_CASINO_DICE; i++) {
        if(data->playerID[i] == INVALID_PLAYER_ID) {
            jPlayerName[i] = env->NewStringUTF("--");
        } else {
            jPlayerName[i] = env->NewStringUTF( CPlayerPool::GetPlayerName(data->playerID[i]) );
        }
    }

    jstring jCrupName;
    jCrupName = env->NewStringUTF( CPlayerPool::GetPlayerName(data->crupId) );

    jmethodID method = env->GetMethodID(clazz, "update",
                                             "(IIIILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;IILjava/lang/String;I)V");


    env->CallVoidMethod(thiz, method, data->tableId, data->bet, data->bank, CHUD::iLocalMoney,
                        jPlayerName[0], data->playerStat[0],
                        jPlayerName[1], data->playerStat[1],
                        jPlayerName[2], data->playerStat[2],
                        jPlayerName[3], data->playerStat[3],
                        jPlayerName[4], data->playerStat[4],
                        data->time, jCrupName, data->crupId);

    for(auto & i : jPlayerName)
        env->DeleteLocalRef(i);

    env->DeleteLocalRef(jCrupName);
}

void CNetGame::packetDice(Packet *p) {
    RakNet::BitStream bs((unsigned char *) p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    CDice::DicePacket data{};
    bs.Read((char*)&data, sizeof(CDice::DicePacket));

    if(data.bIsShow) {
        CDice::Update(&data);
    } else {
        CDice::Destroy();
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_casino_Dice_nativeSendButton(JNIEnv *env, jobject thiz, jint butt_id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8)    ID_CUSTOM_RPC);
    bsSend.Write((uint8)    RPC_SHOW_DICE_TABLE);
    bsSend.Write((uint8)    butt_id);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}