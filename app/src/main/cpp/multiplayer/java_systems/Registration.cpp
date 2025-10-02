//
// Created on 30.08.2023.
//

#include "Registration.h"

#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"
#include "util/CJavaWrapper.h"
#include "../game/Entity/Ped/Ped.h"
#include "game/Camera.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Registration_nativeChooseSkinClick(JNIEnv *env, jobject thiz, jint choosesex) {
    g_pJavaWrapper->RegisterSexMale = choosesex;
    CGame::ToggleHUDElement(0, false);
    CPedSamp *pPlayer = CGame::FindPlayerPed();

    pNetGame->SendRegisterSkinPacket(
            CRegistration::ChangeSkin(g_pJavaWrapper->RegisterSkinValue));

    if (pPlayer->m_pPed->IsInVehicle())
        pPlayer->m_pPed->RemoveFromVehicleAndPutAt({-82.9753, 966.7605, 1597.9788});
    else
        pPlayer->m_pPed->Teleport(CVector(-82.9753, 966.7605, 1597.9788), false);

    pPlayer->ForceTargetRotation(90.0f);

    if (pPlayer) {
        CCamera::SetPosition(-85.068267, 966.699584, 1598.421997, 0.0f, 0.0f, 0.0f);
        CCamera::LookAtPoint(-80.124114, 967.120971, 1597.807373, 2);
        pPlayer->m_pPed->SetInterior(2, true);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Registration_nativeSkinBackClick(JNIEnv *env, jobject thiz) {
    g_pJavaWrapper->RegisterSkinValue--;
    if (g_pJavaWrapper->RegisterSexMale == 1) // man
    {
        if (g_pJavaWrapper->RegisterSkinValue < 1) {
            g_pJavaWrapper->RegisterSkinValue = 9;
        }
    } else if (g_pJavaWrapper->RegisterSexMale == 2) // woman
    {
        if (g_pJavaWrapper->RegisterSkinValue < 1) {
            g_pJavaWrapper->RegisterSkinValue = 4;
        }
    }
    //CChatWindow::AddMessage("chooseskinvalue: %d, chooseskinid: %d", g_pJavaWrapper->RegisterSkinValue, g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
    pNetGame->SendRegisterSkinPacket(
            CRegistration::ChangeSkin(g_pJavaWrapper->RegisterSkinValue));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Registration_nativeSkinNextClick(JNIEnv *env, jobject thiz) {
    g_pJavaWrapper->RegisterSkinValue++;
    if (g_pJavaWrapper->RegisterSexMale == 1) // man
    {
        if (g_pJavaWrapper->RegisterSkinValue > 9) {
            g_pJavaWrapper->RegisterSkinValue = 1;
        }
    } else if (g_pJavaWrapper->RegisterSexMale == 2) // woman
    {
        if (g_pJavaWrapper->RegisterSkinValue > 4) {
            g_pJavaWrapper->RegisterSkinValue = 1;
        }
    }
    //CChatWindow::AddMessage("chooseskinvalue: %d, chooseskinid: %d", g_pJavaWrapper->RegisterSkinValue, g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
    pNetGame->SendRegisterSkinPacket(
            CRegistration::ChangeSkin(g_pJavaWrapper->RegisterSkinValue));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Registration_nativeClick(JNIEnv *pEnv, jobject thiz, jstring password, jstring mail, jint sex) {
    const char *inputPassword = pEnv->GetStringUTFChars(password, nullptr);
    const char *inputMail = pEnv->GetStringUTFChars(mail, nullptr);

    if (pNetGame) {
        pNetGame->SendRegisterPacket((char *) inputPassword, (char *) inputMail, sex,
                                     g_pJavaWrapper->RegisterSkinId);
    }

    CGame::ToggleHUDElement(0, false);

    pEnv->ReleaseStringUTFChars(password, inputPassword);
    pEnv->ReleaseStringUTFChars(mail, inputMail);
}

void CRegistration::Show(char *nick, int id) {
    if(!thiz)
        Constructor();

    CGame::isRegistrationActive = true;
    auto env = g_pJavaWrapper->GetEnv();

    jstring jNick = env->NewStringUTF(nick);

    auto method = env->GetMethodID(clazz, "show", "(Ljava/lang/String;I)V");
    env->CallVoidMethod(thiz, method, jNick, id);

    env->DeleteLocalRef(jNick);

    g_pJavaWrapper->RegisterSkinValue = 1;
}

const uint32_t cRegisterSkin[2][10] = {
        {9,  195, 231, 232, 1,   1,   1,   1,   1, 1}, // female
        {16, 79,  134, 135, 200, 234, 235, 236, 239} // male
};

uint32_t CRegistration::ChangeSkin(int skin) {
    uint32_t uiSkin = 16;
    bool bIsMan = g_pJavaWrapper->RegisterSexMale == 1;
    uint32_t uiMaxSkins = bIsMan ? 9 : 4;

    if (0 < skin > uiMaxSkins) {
        g_pJavaWrapper->RegisterSkinId = uiSkin;
        return uiSkin;
    }

    uiSkin = cRegisterSkin[(int) bIsMan][skin - 1];
    g_pJavaWrapper->RegisterSkinId = uiSkin;

    return uiSkin;
}
