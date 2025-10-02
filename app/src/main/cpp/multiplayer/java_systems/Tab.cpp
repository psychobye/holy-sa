#include "main.h"
#include "Tab.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"
#include "HUD.h"

extern CGUI* pGUI;

void CTab::Show() {
    if(!CTab::thiz)
        Constructor();

    pNetGame->UpdatePlayerScoresAndPings();
}

void CTab::update() {

    CTab::setStat(CPlayerPool::GetLocalPlayerID(),
                  CPlayerPool::GetLocalPlayerName(),
                               CPlayerPool::GetLocalPlayerScore(),
                               CPlayerPool::GetLocalPlayerPing());

    PLAYERID i = 1, x;
    for (x = 0; x < MAX_PLAYERS; x++)
    {
        if (!CPlayerPool::GetAt(x)) continue;

        CTab::setStat(x,
                      CPlayerPool::GetPlayerName(x),
                      CPlayerPool::GetRemotePlayerScore(x),
                      CPlayerPool::GetRemotePlayerPing(x) );

    }


    //Show Window
}

void CTab::setStat(int id, char name[], int score, int ping) {
    if(!CTab::thiz)
        return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jmethodID setStat = env->GetMethodID(clazz, "setStat", "(IILjava/lang/String;II)V");


    jstring jPlayerName = env->NewStringUTF( name );

    uint32_t color;
    if(id == CPlayerPool::GetLocalPlayerID())
        color = CLocalPlayer::GetPlayerColor();
    else
        color = CPlayerPool::GetAt(id)->GetPlayerColor();

    if(color == 0) color = 0xffffffff;
    CRGBA gg;
    gg.Set(color);
    gg = gg.ToRGB();

    env->CallVoidMethod(CTab::thiz, setStat, id, (jint)gg.ToIntARGB(), jPlayerName, score, ping);

    env->DeleteLocalRef(jPlayerName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_tab_Tab_nativeDeleteCppObj(JNIEnv *env, jobject thiz) {
    CTab::DeleteCppObject();
}