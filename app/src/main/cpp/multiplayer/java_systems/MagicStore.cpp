//
// Created on 04.10.2023.
//

#include "MagicStore.h"
#include "net/netgame.h"
#include "game/common.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_magicStore_MagicStore_nativeOnClose(JNIEnv *env, jobject thiz) {
    CMagicStore::DeleteCppObject();
}

void CMagicStore::UpdateBalance(int bronze, int silver, int gold) {
    auto env = CJavaWrapper::GetEnv();

    auto method = env->GetMethodID(clazz, "updateBalance", "(III)V");
    env->CallVoidMethod(thiz, method, bronze, silver, gold);
}

void CMagicStore::ReceivePacket(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip packet and rpc id

    PacketType type;
    uint8_t toggle;

    bs.Read(type);
    if(type == PacketType::TOGGLE) {
        Constructor();
        return;
    }
    if(type == PacketType::UPDATE_BALANCE) {
        if(!thiz)
            return;
        
        uint32_t bronze, silver, gold;
        bs.Read(bronze);
        bs.Read(silver);
        bs.Read(gold);

        UpdateBalance(bronze, silver, gold);
        return;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_magicStore_MagicStore_nativeSendClick(JNIEnv *env, jobject thiz, jint click_type, jint index, jint category) {
    RakNet::BitStream bs;
    bs.Write((uint8)  ID_CUSTOM_RPC);
    bs.Write((uint8)  RPC_MAGIC_STORE);
    bs.Write((uint8)  CMagicStore::PacketType::CLICK);
    bs.Write((uint8)  click_type);
    bs.Write((uint8)  category);
    bs.Write((uint32) index);

    pNetGame->GetRakClient()->Send(&bs, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}