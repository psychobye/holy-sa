//
// Created on 24.11.2023.
//

#include "MiningStore.h"
#include "net/netgame.h"

void CMiningStore::Show(const std::string& price, const std::string& caption, const std::string& specs, const std::string& description) {
    if(!thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();

    jstring jprice          = env->NewStringUTF(cp1251_to_utf8(price).c_str());
    jstring jcaption        = env->NewStringUTF(cp1251_to_utf8(caption).c_str());
    jstring jspecs          = env->NewStringUTF(cp1251_to_utf8(specs).c_str());
    jstring jdescription    = env->NewStringUTF(cp1251_to_utf8(description).c_str());

    auto method = env->GetMethodID(clazz, "show", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jprice, jcaption, jspecs, jdescription);

    env->DeleteLocalRef(jprice);
    env->DeleteLocalRef(jcaption);
    env->DeleteLocalRef(jspecs);
    env->DeleteLocalRef(jdescription);
}

void CMiningStore::ReceivePacket(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    ePacketType type;
    bs.Read(type);

    if(type == ePacketType::SHOW) {
        std::string price;
        bs.Read(price);

        std::string caption;
        bs.Read(caption);

        std::string specs;
        bs.Read(specs);

        std::string description;
        bs.Read(description);

        Show(price, caption, specs, description);
        return;
    }
    if(type == ePacketType::HIDE){
        CMiningStore::Destroy();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_MiningStore_nativeSendClick(JNIEnv *env, jobject thiz, jint button_id) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_MINING_STORE);
    bsSend.Write((uint8_t)  button_id);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}