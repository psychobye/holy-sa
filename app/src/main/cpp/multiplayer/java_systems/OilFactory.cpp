//
// Created on 22.07.2023.
//

#include "net/netgame.h"
#include <jni.h>

#include "OilFactory.h"
#include "util/CJavaWrapper.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_fillingGames_OilFactory_nativeSendExit(JNIEnv *env, jobject thiz,
                                                     jboolean ok) {

    RakNet::BitStream bsSend;
    bsSend.Write((uint8)  ID_CUSTOM_RPC);
    bsSend.Write((uint8)  RPC_SHOW_OILGAME);
    bsSend.Write((uint8)  ok);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);


    COilFactory::DeleteCppObject();
}

void COilFactory::Show() {
    if(!COilFactory::thiz)
        Constructor();
}
