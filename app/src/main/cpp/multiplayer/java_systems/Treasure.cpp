//
// Created on 09.07.2023.
//

#include "Treasure.h"
#include "util/CJavaWrapper.h"
#include "raknet/BitStream.h"
#include "raknet/PacketEnumerations.h"
#include "net/netgame.h"

void CTreasure::Show(int treasureId, bool available) {
    if(!thiz)
        Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    auto method = env->GetMethodID(clazz, "show", "(IZ)V");
    env->CallVoidMethod(CTreasure::thiz, method, treasureId, available);
}

void CTreasure::open(int prizeId) {
    if(!thiz)
        Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(clazz, "openTreasure", "(I)V");
    env->CallVoidMethod(CTreasure::thiz, method, prizeId);
}

void CNetGame::packetTreasure(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t prizeId;
    bs.Read(prizeId);

    CTreasure::open(prizeId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_treasure_Treasure_nativeSendOpenTreasure(JNIEnv *env, jobject thiz,
                                                                    jint treasure_id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_TREASURE);
    bsSend.Write((uint8_t)  treasure_id);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_donate_Donate_nativeShowTreasurePreview(JNIEnv *env, jobject thiz,
                                                                   jint treasure_id) {
    CTreasure::Show(treasure_id, false);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_treasure_Treasure_nativeDestructor(JNIEnv *env, jobject thiz) {
    CTreasure::DeleteCppObject();
}