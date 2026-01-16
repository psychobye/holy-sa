//
// Created on 03.07.2023.
//

#include "ChooseSpawn.h"
#include "raknet/BitStream.h"
#include "util/CJavaWrapper.h"
#include "net/netgame.h"

void CChooseSpawn::Update(int organization, int station, int exit, int garage, int house){
    if(!thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();
    auto method = env->GetMethodID(clazz, "update", "(ZZZZZ)V");
    env->CallVoidMethod(thiz, method, organization, station, exit, garage, house);
}

void CChooseSpawn::packetToggle(Packet *p) {
    RakNet::BitStream bs((unsigned char *) p->data, p->length, false);
    bs.IgnoreBits(40);

    uint8_t toggle;
    uint8_t organization;
    uint8_t station;
    uint8_t exit;
    uint8_t garage;
    uint8_t house;

    bs.Read(toggle);
    bs.Read(organization);
    bs.Read(station);
    bs.Read(exit);
    bs.Read(garage);
    bs.Read(house);

   // Log("RPC_TOGGLE_CHOOSE_SPAWN");

    if (toggle == 1) {
        CChooseSpawn::Update(organization, station, exit, garage, house);
    } else if (toggle == 0) {
        CChooseSpawn::Destroy();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_ChooseSpawn_nativeSendClick(JNIEnv *env, jobject thiz, jint spawn_id) {
    pNetGame->SendCustomPacket(253, RPC_TOGGLE_CHOOSE_SPAWN, spawn_id);
}