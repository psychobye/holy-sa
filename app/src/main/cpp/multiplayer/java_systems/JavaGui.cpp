//
// Created on 02.04.2024.
//
#include "JavaGui.h"
#include "game.h"

void CJavaGui::ReceivePacket(Packet *p) {
    //
    RakNet::BitStream bs(p->data, p->length, false);
    bs.IgnoreBits(8);

    uint32 uiId;
    int32 actionId;
    std::string data;

    bs.Read(uiId);
    bs.Read(actionId);
    bs.Read(data);
    //

    auto env = CJavaWrapper::GetEnv();
    jstring jdata = env->NewStringUTF( cp1251_to_utf8(data).c_str() );

    auto method = env->GetStaticMethodID(clazz, "receiveUiPacket", "(IILjava/lang/String;)V");
    env->CallStaticVoidMethod(clazz, method, uiId, actionId, jdata);

    env->DeleteLocalRef(jdata);
}

void CJavaGui::ExecuteAction(int uiId, eActionId actionId)
{
    auto env = CJavaWrapper::GetEnv();

    auto method = env->GetStaticMethodID(clazz, "receiveUiPacket", "(IILjava/lang/String;)V");
    env->CallStaticVoidMethod(clazz, method, uiId, actionId, "");
}

void CJavaGui::Create(int uiId)
{
    ExecuteAction(uiId, eActionId::ACTION_CREATE);
}

void CJavaGui::Destroy(int uiId)
{
    ExecuteAction(uiId, eActionId::ACTION_DESTROY);
}

void CJavaGui::TempToggle(int uiId, bool toggle)
{
    auto actionId = (toggle ? eActionId::ACTION_SHOW : eActionId::ACTION_HIDE);

    ExecuteAction(uiId, actionId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_NativeGui_00024Companion_nativeSendPacket(JNIEnv *env, jobject thiz, jint ui_id, jint action_id, jstring json) {

    const char* data = env->GetStringUTFChars(json, nullptr);
    std::string sendData;
    sendData.assign(data);
    env->ReleaseStringUTFChars(json, data);

    // maybe push code in main thread?
    RakNet::BitStream bs;
    bs.Write((uint8_t)      PACKET_ANDROID_GUI);
    bs.Write((uint32_t)     ui_id);
    bs.Write((uint32_t)     action_id);
    bs.Write(               sendData);

    pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
}