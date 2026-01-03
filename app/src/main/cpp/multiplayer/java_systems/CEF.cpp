#include "CEF.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

std::unordered_map<std::string, std::function<void(const std::string&)>> CCEF::callbacks;

void CCEF::Init(const std::string& url) {
    Log("INIT");
    auto env = CJavaWrapper::GetEnv();
    if (!thiz) Constructor();

    jstring jUrl = env->NewStringUTF(url.c_str());
    jmethodID method = env->GetMethodID(clazz, "initBrowser", "(Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jUrl);
    env->DeleteLocalRef(jUrl);

    bIsShow = true;
}

void CCEF::Show() {
    Log("SHOW");
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(clazz, "showBrowser", "()V");
    env->CallVoidMethod(thiz, method);
    bIsShow = true;
}

void CCEF::Hide() {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(clazz, "hideBrowser", "()V");
    env->CallVoidMethod(thiz, method);
    bIsShow = false;
}

void CCEF::SetSize(float size) {
    if (!thiz) return;

    auto env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(clazz, "setSize", "(F)V");
    if (!method) return;

    env->CallVoidMethod(thiz, method, size);
}

void CCEF::SetUrl(const std::string& url) {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jstring jUrl = env->NewStringUTF(url.c_str());
    jmethodID method = env->GetMethodID(clazz, "setBrowserUrl", "(Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jUrl);
    env->DeleteLocalRef(jUrl);
}

void CCEF::SendEvent(const std::string& event, const std::string& json) {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jstring jEvent = env->NewStringUTF(event.c_str());
    jstring jData  = env->NewStringUTF(json.c_str());
    jmethodID method = env->GetMethodID(clazz, "sendEvent", "(Ljava/lang/String;Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jEvent, jData);
    env->DeleteLocalRef(jEvent);
    env->DeleteLocalRef(jData);
}

void CCEF::RegisterCallback(const std::string& event, std::function<void(const std::string&)> cb) {
    callbacks[event] = cb;
}

void CCEF::OnJsEvent(const std::string& event, const std::string& data) {
    if (callbacks.count(event)) callbacks[event](data);
}

// pkt
void CCEF::pktInit(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    std::string url;
    bs.ReadStr8(url);

    CCEF::Init(cp1251_to_utf8(url));
}

void CCEF::pktShow(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    CCEF::Show();
}

void CCEF::pktHide(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    CCEF::Hide();
}

void CCEF::pktSetSize(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    float size;
    bs.Read(size);

    CCEF::SetSize(size);
}

void CCEF::pktSetUrl(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    std::string url;
    bs.ReadStr8(url);

    CCEF::SetUrl(cp1251_to_utf8(url));
}