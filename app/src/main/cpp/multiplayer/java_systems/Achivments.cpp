//
// Created on 15.07.2023.
//

#include "raknet/NetworkTypes.h"
#include "Achivments.h"
#include "raknet/BitStream.h"
#include "util/CJavaWrapper.h"
#include "raknet/PacketEnumerations.h"
#include "net/netgame.h"

void CAchivments::packetAchivments(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

   uint16_t len;
   bs.Read(len);

   uint32_t progress;
   uint8_t step;
   for(int i = 0; i < len; i ++) {
        bs.Read(progress);
        bs.Read(step);

       updateItem(i, progress, step);
   }
}

void CAchivments::updateItem(int index, int progress, int step) {
    if(CAchivments::thiz == nullptr)
        CAchivments::Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    auto method = env->GetMethodID(CAchivments::clazz, "updateItem", "(III)V");
    env->CallVoidMethod(CAchivments::thiz, method, index, progress, step);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_achivments_Achivments_nativeSendGive(JNIEnv *env, jobject thiz,
                                                                jint index) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_ACHIVMENTS);
    bsSend.Write((uint16_t) index);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_achivments_Achivments_nativeExit(JNIEnv *env, jobject thiz) {
    CAchivments::DeleteCppObject();
}