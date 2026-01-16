//
// Created on 17.12.2023.
//

#include "BuyPlate.h"
#include "../net/netgame.h"
#include <string>

void CBuyPlate::ReceivePacket(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip packet and rpc id

    std::string sprite;
    int32 randomPrice;
    std::string buyPrice;

    bs.Read(sprite);
    bs.Read(randomPrice);
    bs.Read(buyPrice);

    Show(sprite, randomPrice, buyPrice);
}

void CBuyPlate::Show(std::string& sprite, int randomPrice, std::string& buyPrice) {
    if(!thiz)
        Constructor();

    auto env = g_pJavaWrapper->GetEnv();

    jstring jsprite = env->NewStringUTF(cp1251_to_utf8(sprite).c_str() );
    jstring jbuyPrice = env->NewStringUTF(cp1251_to_utf8(buyPrice).c_str() );

    auto method = env->GetMethodID(clazz, "show", "(Ljava/lang/String;ILjava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jsprite, randomPrice, jbuyPrice);

    env->DeleteLocalRef(jsprite);
    env->DeleteLocalRef(jbuyPrice);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_BuyPlate_nativeSendClick(JNIEnv *env, jobject thiz, jint click_id, jbyteArray text) {
    jbyte* pMsg = env->GetByteArrayElements(text, nullptr);
    jsize length = env->GetArrayLength(text);

    std::string szStr((char*)pMsg, length);

    RakNet::BitStream bs;
    bs.Write((uint8_t)  ID_CUSTOM_RPC);
    bs.Write((uint8_t)  RPC_BUY_PLATE);
    bs.Write((uint8_t)  click_id);
    bs.Write(szStr);

    pNetGame->GetRakClient()->Send(&bs, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);

    env->ReleaseByteArrayElements(text, pMsg, JNI_ABORT);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_BuyPlate_nativeOnExit(JNIEnv *env, jobject thiz) {
    CBuyPlate::DeleteCppObject();
}