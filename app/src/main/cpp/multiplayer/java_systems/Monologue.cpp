//
// Created on 19.11.2023.
//

#include "Monologue.h"
#include "game/common.h"
#include "net/netgame.h"
#include "util/util.h"

void CMonologue::ReceivePacket(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip packet and rpc id

    eRpcType type;
    bs.Read(type);

    if(type == eRpcType::HIDE) {
        CMonologue::Destroy();
        return;
    }

    int32 skinId;

    bs.Read(monologueId); // uint16
    bs.Read(skinId);

    uint32 textLen;
    bs.Read(textLen);
    char text[textLen + 1];
    bs.Read(text, textLen);
    text[textLen] = '\0';

    uint8 authorLen;
    bs.Read(authorLen);
    char author[authorLen + 1];
    bs.Read(author, authorLen);
    author[authorLen] = '\0';

    uint8 buttonLen;
    bs.Read(buttonLen);
    char button[buttonLen + 1];
    bs.Read(button, buttonLen);
    button[buttonLen] = '\0';

    CMonologue::Show(skinId, text, author, button);
}

void CMonologue::Show(int skinId, const char* text, const char* author, const char* button) {
    if(!CMonologue::thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();

    jstring jtext   = env->NewStringUTF( cp1251_to_utf8(text).c_str() );
    jstring jauthor = env->NewStringUTF( cp1251_to_utf8(author).c_str() );
    jstring jbutton = env->NewStringUTF( cp1251_to_utf8(button).c_str() );

    auto method = env->GetMethodID(clazz, "show", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, skinId, jtext, jauthor, jbutton);

    env->DeleteLocalRef(jtext);
    env->DeleteLocalRef(jauthor);
    env->DeleteLocalRef(jbutton);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Monologue_nativeSendClick(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_MONOLOGY);
    bsSend.Write((uint16_t) CMonologue::monologueId);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}