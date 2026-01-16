//
// Created on 17.02.2024.
//

#include "BattlePassBuy.h"
#include "net/netgame.h"

void CBattlePassBuy::Show(jlong endDate) {
    if(!thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();

   // clazz = env->FindClass("com/russia/game/gui/battle_pass/BattlePassBuy");
    auto method = env->GetMethodID(clazz, "show", "(J)V");
    env->CallVoidMethod(thiz, method, endDate);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_battle_1pass_BattlePassBuy_nativeOnClosed(JNIEnv *env, jobject thiz) {
    CBattlePassBuy::DeleteCppObject();
}