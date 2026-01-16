#include "PlayerInfo.h"
#include <jni.h>
#include "main.h"
#include "HUD.h"
#include "../game/game.h"
#include "net/netgame.h"

int CPlayerInfo::GetId() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer) return 0;
    return CPlayerPool::GetLocalPlayerID();
}

std::string CPlayerInfo::GetName() {
    const char* raw = CPlayerPool::GetLocalPlayerName();
    if (!raw) return std::string();
    return std::string(raw);
}

int CPlayerInfo::GetModelId() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer || !pPlayer->m_pPed) return 0;
    return pPlayer->m_pPed->m_nModelIndex;
}

float CPlayerInfo::GetHealth() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer) return 0.f;
    return pPlayer->GetHealth();
}

float CPlayerInfo::GetArmour() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer) return 0.f;
    return pPlayer->GetArmour();
}

double CPlayerInfo::GetX() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer || !pPlayer->m_pPed) return 0.0;
    return pPlayer->m_pPed->GetPosition().x;
}

double CPlayerInfo::GetY() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer || !pPlayer->m_pPed) return 0.0;
    return pPlayer->m_pPed->GetPosition().y;
}

double CPlayerInfo::GetZ() {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer || !pPlayer->m_pPed) return 0.0;
    return pPlayer->m_pPed->GetPosition().z;
}

extern "C" {
    JNIEXPORT jint JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetId(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jint)CPlayerInfo::GetId();
    }

    JNIEXPORT jstring JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetName(JNIEnv* env, jclass /*cls*/) {
        std::string s = CPlayerInfo::GetName();
        return env->NewStringUTF(s.c_str());
    }

    JNIEXPORT jint JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetModelId(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jint)CPlayerInfo::GetModelId();
    }

    JNIEXPORT jfloat JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetHealth(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jfloat)CPlayerInfo::GetHealth();
    }

    JNIEXPORT jfloat JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetArmour(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jfloat)CPlayerInfo::GetArmour();
    }

    JNIEXPORT jdouble JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetX(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jdouble)CPlayerInfo::GetX();
    }

    JNIEXPORT jdouble JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetY(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jdouble)CPlayerInfo::GetY();
    }

    JNIEXPORT jdouble JNICALL
    Java_com_holy_game_gui_PlayerInfo_nativeGetZ(JNIEnv* /*env*/, jclass /*cls*/) {
        return (jdouble)CPlayerInfo::GetZ();
    }
}