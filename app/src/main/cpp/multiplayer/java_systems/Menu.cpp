#include "Menu.h"
#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "Tab.h"

void CMenu::Show(
    int donate, int money, float totalHours, int level, int exp, int expMax, const std::string& familyName, uint8_t r, uint8_t g, uint8_t b) {
    auto env = CJavaWrapper::GetEnv();

    if (!CMenu::thiz)
        Constructor();

    jstring jFamily = env->NewStringUTF(familyName.c_str());

    jmethodID method = env->GetMethodID(
            CMenu::clazz,
            "show",
            "(IIFIIILjava/lang/String;III)V"
    );

    env->CallVoidMethod(
            CMenu::thiz,
            method,
            static_cast<jint>(donate),
            static_cast<jint>(money),
            static_cast<jfloat>(totalHours),
            static_cast<jint>(level),
            static_cast<jint>(exp),
            static_cast<jint>(expMax),
            jFamily,
            static_cast<jint>(r),
            static_cast<jint>(g),
            static_cast<jint>(b)
    );

    env->DeleteLocalRef(jFamily);

    bIsShow = true;
}

void CNetGame::packetShowMenu(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint32_t type;
    bs.Read(type);

    uint32_t donate;
    bs.Read(donate);

    uint32_t money;
    bs.Read(money);

    float totalHours;
    bs.Read(totalHours);

    uint32_t level;
    bs.Read(level);

    uint32_t exp;
    bs.Read(exp);

    uint32_t expMax;
    bs.Read(expMax);

    std::string familyName;
    bs.ReadStr8(familyName);

    uint8_t r, g, b;
    bs.Read(r);
    bs.Read(g);
    bs.Read(b);

    if(type == 1) {
        CMenu::Show(
                donate,
                money,
                totalHours,
                level,
                exp,
                expMax,
                cp1251_to_utf8(familyName),
                r, g, b
        );
    }
    else if(type == 0) {
        CMenu::Destroy();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_menu_fragment_MenuMainFragment_nativeSendMenuButt(JNIEnv *env, jobject thiz, jint butt_id) {
    switch (butt_id) {
        case 1:
            pNetGame->SendChatCommand("/car");
            break;
        case 2:
            pNetGame->SendChatCommand("/skin");
            break;
        case 3:
            pNetGame->SendChatCommand("/tp");
            break;
        case 4: {
            CTab::Show();
            break;
        }
        case 5: {
            pNetGame->SendChatCommand("/report");
            break;
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_nativeShowMenu(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_MENU);
    bsSend.Write((uint8_t)CMenu::ePacketType::SHOW);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_menu_Menu_nativeOnExit(JNIEnv *env, jobject thiz) {
    CMenu::DeleteCppObject();

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_MENU);
    bsSend.Write((uint8_t)CMenu::ePacketType::EXIT);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}