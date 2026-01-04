#include "CEF.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

// Stores JS callbacks by event name
std::unordered_map<std::string, std::function<void(const std::string&)>> CCEF::callbacks;

/**
 * Initializes CEF browser on Java/Kotlin side with initial URL
 * - Creates Java object if not exists
 * - Calls initBrowser(String url)
 */
void CCEF::Init(const std::string& url) {
    auto env = CJavaWrapper::GetEnv();
    if (!thiz) Constructor();

    jstring jUrl = env->NewStringUTF(url.c_str());
    jmethodID method = env->GetMethodID(clazz, "initBrowser", "(Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jUrl);
    env->DeleteLocalRef(jUrl);

    bIsShow = true;
}

/**
 * Shows CEF browser view
 */
void CCEF::Show() {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(clazz, "showBrowser", "()V");
    env->CallVoidMethod(thiz, method);
    bIsShow = true;
}

/**
 * Hides CEF browser view
 */
void CCEF::Hide() {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(clazz, "hideBrowser", "()V");
    env->CallVoidMethod(thiz, method);
    bIsShow = false;
}

/**
 * Sets browser UI scale / size (float value passed to Java)
 */
void CCEF::SetSize(float size) {
    if (!thiz) return;

    auto env = CJavaWrapper::GetEnv();
    jmethodID method = env->GetMethodID(clazz, "setSize", "(F)V");
    if (!method) return;

    env->CallVoidMethod(thiz, method, size);
}

/**
 * Updates browser URL at runtime
 */
void CCEF::SetUrl(const std::string& url) {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();
    jstring jUrl = env->NewStringUTF(url.c_str());
    jmethodID method = env->GetMethodID(clazz, "setBrowserUrl", "(Ljava/lang/String;)V");
    env->CallVoidMethod(thiz, method, jUrl);
    env->DeleteLocalRef(jUrl);
}

/**
 * Packet: initialize CEF with URL from server
 */
void CCEF::pktInit(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip RPC header

    std::string url;
    bs.ReadStr8(url);

    CCEF::Init(cp1251_to_utf8(url));
}

/**
 * Packet: show browser
 */
void CCEF::pktShow(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    CCEF::Show();
}

/**
 * Packet: hide browser
 */
void CCEF::pktHide(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    CCEF::Hide();
}

/**
 * Packet: set browser size
 */
void CCEF::pktSetSize(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    float size;
    bs.Read(size);

    CCEF::SetSize(size);
}

/**
 * Packet: update browser URL
 */
void CCEF::pktSetUrl(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    std::string url;
    bs.ReadStr8(url);

    CCEF::SetUrl(cp1251_to_utf8(url));
}

/**
 * Sends server event into JavaScript context
 * (C++ -> Java -> JS)
 */
void CCEF::GetEvent(const std::string& event, const std::string& json) {
    if (!thiz) return;
    auto env = CJavaWrapper::GetEnv();

    jstring jEvent = env->NewStringUTF(event.c_str());
    jstring jJson = env->NewStringUTF(json.c_str());

    jmethodID method = env->GetMethodID(
            clazz,
            "sendServerEventToJavaScript",
            "(Ljava/lang/String;Ljava/lang/String;)V"
    );

    env->CallVoidMethod(thiz, method, jEvent, jJson);

    env->DeleteLocalRef(jEvent);
    env->DeleteLocalRef(jJson);
}

/**
 * Sends event from client to server
 * (CEF / JS -> C++ -> server)
 */
void CCEF::SendEvent(const std::string& event, const std::string& json) {
    RakNet::BitStream bs;
    bs.Write((uint8_t)ID_CUSTOM_RPC);
    bs.Write((uint8_t)RPC_CLIENT_CEF_EVENT);

    bs.Write((uint8_t)event.size());
    bs.Write(event.c_str(), event.size());

    bs.Write((uint32_t)json.size());
    bs.Write(json.c_str(), json.size());

    pNetGame->GetRakClient()->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
}

/**
 * Handles incoming server -> client CEF event
 */
void CCEF::OnServerEvent(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    std::string event;
    bs.ReadStr8(event);

    uint32_t remainingBits = bs.GetNumberOfUnreadBits();
    uint32_t jsonBytes = remainingBits / 8;

    if (jsonBytes > 0 && jsonBytes < 100000) {
        std::string jsonStr;
        jsonStr.resize(jsonBytes);
        bs.Read(&jsonStr[0], jsonBytes);
        CCEF::GetEvent(event, jsonStr);
    }
}

// ======================
// JNI bridge (Java → C++)
// ======================

/**
 * Called from Java when JS sends event to native layer
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_cef_CefManager_nativeSendEvent(
        JNIEnv *env,
        jobject /*thiz*/,
        jstring jevent,
        jstring jjson)
{
    if (!jevent) return;

    const char *event_str = env->GetStringUTFChars(jevent, nullptr);
    std::string event = event_str ? std::string(event_str) : "";
    if (event_str) env->ReleaseStringUTFChars(jevent, event_str);

    std::string json;
    if (jjson) {
        const char *json_str = env->GetStringUTFChars(jjson, nullptr);
        if (json_str) {
            json = std::string(json_str);
            env->ReleaseStringUTFChars(jjson, json_str);
        }
    }

    CCEF::SendEvent(event, json);
}