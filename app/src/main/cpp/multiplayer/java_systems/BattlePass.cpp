//
// Created on 10.01.2024.
//

#include "BattlePass.h"
#include "../net/netgame.h"
#include "BattlePassBuy.h"

void CBattlePass::UpdateMainItem(int id, const std::string& name, const std::string& sprite, int rare, int state) {
    if(!thiz)
        return;

    auto env = CJavaWrapper::GetEnv();

    jstring jname   = env->NewStringUTF( cp1251_to_utf8(name).c_str() );
    jstring jsprite = env->NewStringUTF( cp1251_to_utf8(sprite).c_str() );

   // clazz = env->FindClass("com/russia/game/gui/battle_pass/BattlePass");
    auto method = env->GetMethodID(clazz, "updateMainItem", "(ILjava/lang/String;Ljava/lang/String;II)V");
    env->CallVoidMethod(thiz, method, id, jname, jsprite, rare, state);

    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jsprite);
}

void CBattlePass::UpdateTaskItem(int id, const std::string& caption, const std::string& description, int curProgess, int maxProgress, int reward) {
    if(!thiz)
        return;

    auto env = CJavaWrapper::GetEnv();

    jstring jcaption        = env->NewStringUTF( cp1251_to_utf8(caption).c_str() );
    jstring jdescription    = env->NewStringUTF( cp1251_to_utf8(description).c_str() );

     //clazz = env->FindClass("com/russia/game/gui/battle_pass/BattlePass");
    auto method = env->GetMethodID(clazz, "updateTaskItem", "(ILjava/lang/String;Ljava/lang/String;III)V");
    env->CallVoidMethod(thiz, method, id, jcaption, jdescription, curProgess, maxProgress, reward);

    env->DeleteLocalRef(jcaption);
    env->DeleteLocalRef(jdescription);
}

void CBattlePass::UpdateRateItem(bool isGlobal, int id, const std::string& nick, int points) {
    if(!thiz)
        return;

    auto env = CJavaWrapper::GetEnv();

    jstring jnick = env->NewStringUTF( cp1251_to_utf8(nick).c_str() );

   // clazz = env->FindClass("com/russia/game/gui/battle_pass/BattlePass");
    auto method = env->GetMethodID(clazz, "updateRateItem", "(ZILjava/lang/String;I)V");
    env->CallVoidMethod(thiz, method, isGlobal, id, jnick, points);

    env->DeleteLocalRef(jnick);
}

void CBattlePass::UpdateData(int myPlaceByLvl, int myPlaceByPoints, int myLvl, int pointsForUp, int myChips, jlong endTime, jlong updateTasksTime) {
    if(!thiz)
        return;

    auto env = CJavaWrapper::GetEnv();

    //clazz = env->FindClass("com/russia/game/gui/battle_pass/BattlePass");
    auto method = env->GetMethodID(clazz, "updateData", "(IIIIIJJ)V");
    env->CallVoidMethod(thiz, method, myPlaceByLvl, myPlaceByPoints, myLvl, pointsForUp, myChips, endTime, updateTasksTime);
}

void CBattlePass::ReceivePacket(Packet* p)
{
    RakNet::BitStream bs((unsigned char *) p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    ePacketType packetType;
    bs.Read(packetType);

    if(packetType == ePacketType::SHOW) {
        Constructor();
        return;
    }
    if(packetType == ePacketType::UPDATE_DATA) {
        uint32 myPlaceByLvl;
        bs.Read(myPlaceByLvl);

        uint32 myPlaceByPoints;
        bs.Read(myPlaceByPoints);

        uint16 myLvl;
        bs.Read(myLvl);

        uint16 pointsForUp;
        bs.Read(pointsForUp);

        uint16 myChips;
        bs.Read(myChips);

        uint32 endTime;
        bs.Read(endTime);

        uint32 updateTasksTime;
        bs.Read(updateTasksTime);

        UpdateData(myPlaceByLvl, myPlaceByPoints, myLvl, pointsForUp, myChips, endTime, updateTasksTime);
        return;
    }
    if(packetType == ePacketType::UPDATE_MAIN_ITEM) {
        uint16 count;
        bs.Read(count);

        while (count) {
            uint16 id;
            bs.Read(id);

            std::string name;
            bs.Read(name);

            std::string sprite;
            bs.Read(sprite);

            uint8 rare;
            bs.Read(rare);

            uint8 state;
            bs.Read(state);

            UpdateMainItem(id, name, sprite, rare, state);

            count --;
        }
        return;
    }
    if(packetType == ePacketType::UPDATE_TASK_ITEM) {
        uint16 count;
        bs.Read(count);

        while (count) {
            uint16 id;
            bs.Read(id);

            std::string caption;
            bs.Read(caption);

            std::string description;
            bs.Read(description);

            uint16 curProgress;
            bs.Read(curProgress);

            uint16 maxProgress;
            bs.Read(maxProgress);

            uint8 reward;
            bs.Read(reward);

            UpdateTaskItem(id, caption, description, curProgress, maxProgress, reward);

            count --;
        }
        return;
    }
    if(packetType == ePacketType::UPDATE_RATE_ITEM) {
        uint8_t isGlobal;
        bs.Read(isGlobal);

        uint16 count;
        bs.Read(count);

        while (count) {
            uint16 id;
            bs.Read(id);

            std::string nick;
            bs.Read(nick);

            uint32 points;
            bs.Read(points);

            UpdateRateItem(isGlobal, id, nick, points);

            count --;
        }
        return;
    }
    if(packetType == ePacketType::SHOW_BUY_PAGE) {
        uint32 endTime;
        bs.Read(endTime);

        CBattlePassBuy::Show(endTime);
        return;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_battle_1pass_BattlePass_nativeOnClose(JNIEnv *env, jobject thiz) {
    CBattlePass::DeleteCppObject();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_battle_1pass_BattlePassBuy_nativeOnClick(JNIEnv *env, jobject thiz, jint click_type) {
    RakNet::BitStream bs;
    bs.Write((uint8)  ID_CUSTOM_RPC);
    bs.Write((uint8)  PACKET_BATTLEPASS);
    bs.Write((uint8)  CBattlePass::ePacketType::CLICK_BUY_PAGE);
    bs.Write((uint8)  click_type);

    pNetGame->GetRakClient()->Send(&bs, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_battle_1pass_BattlePass_nativeOnClick(JNIEnv *env, jobject thiz, jint click_type, jint index) {
    RakNet::BitStream bs;
    bs.Write((uint8)  ID_CUSTOM_RPC);
    bs.Write((uint8)  PACKET_BATTLEPASS);
    bs.Write((uint8)  click_type);
    bs.Write((uint16) index);

    pNetGame->GetRakClient()->Send(&bs, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}