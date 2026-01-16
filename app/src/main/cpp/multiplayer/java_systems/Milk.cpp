//
// Created on 17.03.2024.
//

#include "Milk.h"
#include "net/netgame.h"

void CMilk::ReceivePacket(Packet *p) {

    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip packet and rpc id

    eRpcId type;
    bs.Read(type);

    if(type == eRpcId::SHOW)
        Constructor();

}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_fillingGames_Milk_nativeOnClosed(JNIEnv *env, jobject thiz, jboolean completed) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8)  ID_CUSTOM_RPC);
    bsSend.Write((uint8)  PACKET_MILK);
    bsSend.Write((uint8)  CMilk::eRpcId::ON_CLOSE);
    bsSend.Write((uint8)  completed);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);


    CMilk::DeleteCppObject();
}