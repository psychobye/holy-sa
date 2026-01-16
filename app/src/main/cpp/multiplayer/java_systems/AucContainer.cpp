//
// Created on 10.12.2022.
//

#include "AucContainer.h"

#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"

#include "HUD.h"

void CNetGame::packetAucContainer(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint16_t id;
    uint8_t type;
    uint32_t price;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(id);
    bs.Read(type);
    bs.Read(price);

    CAucContainer::Show(id, type, (int)price);
}

void CAucContainer::Show(int id, int type, int price){
    auto env = CJavaWrapper::GetEnv();

    if(!CAucContainer::thiz)
        Constructor();

    auto method = env->GetMethodID(clazz, "show", "(III)V");
    env->CallVoidMethod(thiz, method, id, type, price);

    CAucContainer::bIsShow = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_AucContainer_nativeSendClick(JNIEnv *env, jobject thiz,
                                                          jint button_id) {
    CAucContainer::DeleteCppObject();

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_SHOW_CONTEINER_AUC);
    bsSend.Write((uint8_t)  button_id);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}