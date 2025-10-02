//
// Created on 10.01.2023.
//

#include <jni.h>
#include "net/netgame.h"
#include "AdminRecon.h"

#include "../game/game.h"
#include "gui/gui.h"

#include "HUD.h"

void CAdminRecon::tempToggle(bool toggle){
    if(CAdminRecon::thiz == nullptr)
        return;

    JNIEnv* env = CJavaWrapper::GetEnv();

    jmethodID tempToggle = env->GetMethodID(clazz, "tempToggle", "(Z)V");
    env->CallVoidMethod(thiz, tempToggle, toggle);
}

void CAdminRecon::Update(int targetId){
    JNIEnv* env = CJavaWrapper::GetEnv();

    if(!thiz)
        CAdminRecon::Constructor();

    jstring jName = env->NewStringUTF( CPlayerPool::GetPlayerName(targetId) );

    jmethodID Show = env->GetMethodID(CAdminRecon::clazz, "update", "(Ljava/lang/String;I)V");

    env->CallVoidMethod(CAdminRecon::thiz, Show, jName, targetId);
    env->DeleteLocalRef(jName);

    CAdminRecon::bIsShow = true;
    CAdminRecon::iPlayerID = targetId;
}

void CNetGame::packetAdminRecon(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t toggle;
    uint32_t targetID;

    bs.Read(toggle);
    bs.Read(targetID);

    if(toggle == 1){
        CAdminRecon::Update((int)targetID);
    } else {
        CAdminRecon::Destroy();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_AdminRecon_clickButton(JNIEnv *env, jobject thiz, jint button_id) {
    switch (button_id) {
        case CAdminRecon::eButtons::EXIT_BUTTON:{
            pNetGame->SendChatCommand("/reoff");
//            CAdminRecon::bIsToggle = false;
            break;
        }
        case CAdminRecon::eButtons::STATS_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/getstats %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::FREEZE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/freeze %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::UNFREEZE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/unfreeze %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::SPAWN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/spawn %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::SLAP_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/slap %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::REFRESH_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::MUTE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/mute %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::JAIL_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/jail %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::KICK_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/kick %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::BAN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/ban %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::WARN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/warn %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::NEXT_BUTTON:{

            PLAYERID playerid = CAdminRecon::iPlayerID + 1;
            while(!CPlayerPool::GetAt(playerid))
            {
                playerid++;
                if(playerid > MAX_PLAYERS) playerid = 0;
            }
            CAdminRecon::iPlayerID = playerid;

            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::PREV_BUTTON:{

            PLAYERID playerid = CAdminRecon::iPlayerID - 1;
            while(!CPlayerPool::GetAt(playerid))
            {
                playerid--;
                if(playerid < 0) playerid = MAX_PLAYERS;
            }
            CAdminRecon::iPlayerID = playerid;

            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::eButtons::FLIP_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/flip %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        default: return;
    }
}