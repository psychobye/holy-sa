//
// Created on 07.11.2022.
//

#include "FurnitureFactory.h"
#include "HUD.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

//extern CNetGame* pNetGame;

void CFurnitureFactory::Show(int furnitureType) {
    auto env = g_pJavaWrapper->GetEnv();

    if(CFurnitureFactory::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
        CFurnitureFactory::thiz = env->NewObject(clazz, constructor);
        CFurnitureFactory::thiz = env->NewGlobalRef(CFurnitureFactory::thiz);
    }

    jmethodID method = env->GetMethodID(clazz, "show", "(I)V");
    env->CallVoidMethod(CFurnitureFactory::thiz, method, (int)furnitureType);
}

void CFurnitureFactory::Destroy() {
    auto env = g_pJavaWrapper->GetEnv();
    env->DeleteGlobalRef(CFurnitureFactory::thiz);
    CFurnitureFactory::thiz = nullptr;
}

void CNetGame::Packet_FurnitureFactory(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40);

    uint8_t show;
    uint32_t furniture;

    bs.Read(show);
    bs.Read(furniture);

    if(show)
    {
        CFurnitureFactory::Show((int)furniture);
    } else {
       // CFurnitureFactory::Destroy();
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_FurnitureFactory_nativeSendAction(JNIEnv *env, jobject thiz,
                                                             jint button_id) {

    CFurnitureFactory::Destroy();

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_SHOW_FACTORY_GAME);
    bsSend.Write((uint8_t)  button_id);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}