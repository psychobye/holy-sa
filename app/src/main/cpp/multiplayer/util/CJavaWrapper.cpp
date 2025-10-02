#include "CJavaWrapper.h"
#include "../main.h"

extern "C" JavaVM *javaVM;

#include "..//keyboard.h"
#include "..//CSettings.h"
#include "..//net/netgame.h"
#include "../game/game.h"
#include "java_systems/Tab.h"
#include "java_systems/HUD.h"
#include "../game/Entity/Ped/Ped.h"
#include "java_systems/SkinShop.h"
#include "java_systems/Registration.h"
#include "java_systems/BusStation.h"

extern CNetGame *pNetGame;

JNIEnv *CJavaWrapper::GetEnv() {
    JNIEnv *env = nullptr;
    int getEnvStat = javaVM->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) {
        Log("GetEnv: not attached");
        if (javaVM->AttachCurrentThread(&env, NULL) != 0) {
            Log("Failed to attach");
            return nullptr;
        }
    }
    if (getEnvStat == JNI_EVERSION) {
        Log("GetEnv: version not supported");
        return nullptr;
    }

    if (getEnvStat == JNI_ERR) {
        Log("GetEnv: JNI_ERR");
        return nullptr;
    }

    return env;
}
typedef void* (*OSThreadFunction)(void*);
struct ThreadLaunchData {
    void* thread_struct;
    OSThreadFunction func;
    char thread_name[32];
};

void* CJavaWrapper::NVThreadSpawnProc(void* arg) {
    std::unique_ptr<ThreadLaunchData> data(static_cast<ThreadLaunchData*>(arg));

    bool attached = false;
    JNIEnv* env = nullptr;
    jint status = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (status == JNI_EDETACHED) {
        if (javaVM->AttachCurrentThread(&env, nullptr) == 0) {
            attached = true;
        }
    }

    if (env && data->thread_name[0] != '\0') {
        jclass threadClass = env->FindClass("java/lang/Thread");
        if (threadClass) {
            jmethodID currentThread = env->GetStaticMethodID(threadClass, "currentThread", "()Ljava/lang/Thread;");
            jmethodID setName = env->GetMethodID(threadClass, "setName", "(Ljava/lang/String;)V");

            if (currentThread && setName) {
                jobject threadObj = env->CallStaticObjectMethod(threadClass, currentThread);
                jstring nameStr = env->NewStringUTF(data->thread_name);
                env->CallVoidMethod(threadObj, setName, nameStr);

                env->DeleteLocalRef(nameStr);
                env->DeleteLocalRef(threadObj);
            }
            env->DeleteLocalRef(threadClass);
        }
    }

    void* result = data->func(data->thread_struct);

    if (attached) {
        javaVM->DetachCurrentThread();
    }
    return result;
}

void CJavaWrapper::ShowClientSettings() {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(activity, s_ShowClientSettings);

    EXCEPTION_CHECK(env);
}

#include "..//CDebugInfo.h"
#include "chatwindow.h"
#include "java_systems/Medic.h"
#include "java_systems/Speedometr.h"
#include "java_systems/casino/Dice.h"
#include "java_systems/AutoShop.h"
#include "java_systems/ChooseSpawn.h"
#include "java_systems/Authorization.h"
#include "java_systems/GuiWrapper.h"
#include "GuiWrapper.h"

extern "C"
{
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_togglePlayer(JNIEnv *pEnv, jobject thiz, jint toggle) {
    if (toggle)
        CLocalPlayer::GetPlayerPed()->TogglePlayerControllable(false,
                                                                                              true);
    else
        CLocalPlayer::GetPlayerPed()->TogglePlayerControllable(true, true);
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnRightClick(JNIEnv *pEnv, jobject thiz,
                                                                   jint state) {
    pNetGame->SendSpeedTurnPacket(2, state);
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnCenterClick(JNIEnv *pEnv, jobject thiz,
                                                                    jint state) {
    pNetGame->SendSpeedTurnPacket(1, state);
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnLeftClick(JNIEnv *pEnv, jobject thiz,
                                                                  jint state) {
    pNetGame->SendSpeedTurnPacket(0, state);
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_onDeathInfoWait(JNIEnv *pEnv, jobject thiz) {
    pNetGame->SendCustomPacket(251, 48, 0);
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_onDeathInfoClick(JNIEnv *pEnv, jobject thiz) {
    pNetGame->SendCustomPacket(251, 48, 1);
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_onAuctionButtonClick(JNIEnv *pEnv, jobject thiz,
                                                                  jint btnid) {
    pNetGame->SendCustomPacket(251, 52, btnid);
}
}

void CJavaWrapper::Vibrate(int milliseconds) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    env->CallVoidMethod(this->activity, this->j_Vibrate, milliseconds);
}

void CJavaWrapper::SetPauseState(bool a1) {
    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    env->CallVoidMethod(this->activity, this->s_setPauseState, a1);
}

void CJavaWrapper::hideLoadingScreen() {

    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(this->activity);
    jmethodID method = env->GetMethodID(clazz, "hideLoadingScreen", "()V");

    env->CallVoidMethod(this->activity, method);
}

void CJavaWrapper::ExitGame() {

    JNIEnv *env = GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    env->CallVoidMethod(this->activity, this->s_ExitGame);
}

void CJavaWrapper::ClearScreen() {
    Log("ClearScreen");

    CSkinShop::Destroy();
    CHUD::hideTargetNotify();
    CAuthorization::Destroy();
    CChooseSpawn::Destroy();
    CRegistration::Destroy();
    CSpeedometr::Destroy();
    CAutoShop::toggle(false);
    CBusStation::Destroy();
    CHUD::toggleGps(false);
    CHUD::toggleGreenZone(false);
    CMedic::hide();
    CDice::Destroy();
}

CJavaWrapper::CJavaWrapper(JNIEnv *env, jobject activity) {
    this->activity = env->NewGlobalRef(activity);

    jclass nvEventClass = env->GetObjectClass(activity);

    if (!nvEventClass) {
        Log("nvEventClass null");
        return;
    }

//    s_GetClipboardText = env->GetMethodID(nvEventClass, "getClipboardText", "()[B");

    s_ShowClientSettings = env->GetMethodID(nvEventClass, "showClientSettings", "()V");

    j_Vibrate = env->GetMethodID(nvEventClass, "goVibrate", "(I)V");

    s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

    s_ExitGame = env->GetMethodID(nvEventClass, "exitGame", "()V");

    //s_showSplash = env->GetMethodID(nvEventClass, "showSplash", "()V");

    env->DeleteLocalRef(nvEventClass);
    env->DeleteLocalRef(activity);
}

CJavaWrapper::~CJavaWrapper() {
    JNIEnv *pEnv = GetEnv();
    if (pEnv) {
        pEnv->DeleteGlobalRef(this->activity);
    }
}

void CJavaWrapper::SendBuffer(const std::string& text) const {
    JNIEnv *env = GetEnv();

    jstring jstring = env->NewStringUTF( text.c_str() );

    auto clazz = env->GetObjectClass(activity);
    auto method = env->GetMethodID(clazz, "copyTextToBuffer", "(Ljava/lang/String;)V");

    env->CallVoidMethod(activity, method, jstring);
    env->DeleteLocalRef(jstring);
}

void CJavaWrapper::OpenUrl(const std::string& url) const {
    JNIEnv *env = GetEnv();

    jstring jstring = env->NewStringUTF( url.c_str() );

    auto clazz = env->GetObjectClass(activity);
    auto method = env->GetMethodID(clazz, "openUrl","(Ljava/lang/String;)V");

    env->CallVoidMethod(activity, method, jstring);
    env->DeleteLocalRef(jstring);
}

CJavaWrapper *g_pJavaWrapper = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_core_Samp_00024Companion_playUrlSound(JNIEnv *env, jobject clazz, jstring jurl) {
    auto url = env->GetStringUTFChars(jurl, nullptr);

    char temp[888];
    strcpy(temp, url);
    //CAudioStreamPool::PlayIndividualStream(temp, false);

    CAudioStreamPool::PostToAudioThread([=] {
        auto stream = BASS_StreamCreateURL(temp, 0, BASS_STREAM_AUTOFREE | BASS_STREAM_BLOCK | BASS_STREAM_RESTRATE, nullptr, 0);
        BASS_ChannelPlay(stream, false);
    });

    env->ReleaseStringUTFChars(jurl, url);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Menu_nativeSendMenuButt(JNIEnv *env, jobject thiz, jint butt_id) {

    switch (butt_id) {
        case 1:
            pNetGame->SendChatCommand("/gps");
            break;
        case 2:
            pNetGame->SendChatCommand("/mm");
            break;
        case 3:
            pNetGame->SendChatCommand("/inv");
            break;
        case 4: {
            //pNetGame->SendChatCommand("/anim");
            pNetGame->SendChatCommand("/anim");
            break;
        }
        case 5:
            pNetGame->SendChatCommand("/donat");
            break;
        case 6:
            pNetGame->SendChatCommand("/car");
            break;
        case 7: {
            pNetGame->SendChatCommand("/report");
            break;
        }
        case 8: {
            pNetGame->SendChatCommand("/promo");
            break;
        }
        case 9: {
            CTab::Show();
            break;
        }
        case 10: {
            pNetGame->SendChatCommand("/fammenu");
            break;
        }
        case 11: {
            pNetGame->SendChatCommand("/achievements");
            break;
        }
        case 12: {
            pNetGame->SendChatCommand("/livepass");
            break;
        }
    }
}