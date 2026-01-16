//
// Created on 25.07.2023.
//

#include "Authorization.h"
#include "util/CJavaWrapper.h"
#include "game/common.h"
#include "CSettings.h"
#include "net/netgame.h"

void
CAuthorization::Update(char *nick, int id, bool toggleAutoLogin, bool email_acvive) {
    if(!CAuthorization::thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();
    jstring jnick = env->NewStringUTF(nick);

    auto method = env->GetMethodID(CAuthorization::clazz, "update","(Ljava/lang/String;IZZ)V");
    env->CallVoidMethod(thiz, method, jnick, id, toggleAutoLogin, email_acvive);

    env->DeleteLocalRef(jnick);
}

void CAuthorization::SendLoginPacket(const char* password) {
    uint8_t packet = PACKET_AUTH;
    uint8_t RPC = RPC_TOGGLE_LOGIN;
    uint8_t bytePasswordLen = strlen(password);
    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(bytePasswordLen);
    bsSend.Write(password, bytePasswordLen);
    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);

    strcpy(CSettings::m_Settings.player_password, password);
    CSettings::save();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_Authorization_nativeToggleAutoLogin(JNIEnv *env, jobject thiz,
                                                               jboolean toggle) {
    CSettings::m_Settings.szAutoLogin = toggle;
    CSettings::save();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_Authorization_nativeClickRecoveryPass(JNIEnv *env, jobject thiz) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_RESTORE_PASS;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_Authorization_nativeClickLogin(JNIEnv *env, jobject thiz,
                                                          jstring password) {
    const char *inputPassword = env->GetStringUTFChars(password, nullptr);

    if (pNetGame) {
        CAuthorization::SendLoginPacket(inputPassword);
    }

    env->ReleaseStringUTFChars(password, inputPassword);
}