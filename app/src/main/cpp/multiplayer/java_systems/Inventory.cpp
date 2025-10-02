#include "HUD.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "Inventory.h"
#include "chatwindow.h"

void CInventory::ToggleShow(bool toggle)
{
    auto env = CJavaWrapper::GetEnv();

    if(!thiz)
        Constructor();

    auto method = env->GetMethodID(clazz, "toggleShow", "(ZLjava/lang/String;IFFII)V");

    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    if (!pPlayer) return;

    jstring jnick = env->NewStringUTF(CPlayerPool::GetLocalPlayerName());

    env->CallVoidMethod(CInventory::thiz, method,
                        toggle,
                        jnick,
                        CPlayerPool::GetLocalPlayerID(),
                        pPlayer->GetHealth(),
                        pPlayer->GetArmour(),
                        CHUD::iSatiety,
                        pPlayer->m_pPed->m_nModelIndex);

    env->DeleteLocalRef(jnick);

    bIsShow = toggle;
}

void CInventory::UpdateItem(int matrixId, int pos, const std::string& sprite, const std::string& caption, const std::string &count, int rare, bool active) {
    if(!thiz)
        return;

    auto env = CJavaWrapper::GetEnv();

    jstring jsprite = env->NewStringUTF( cp1251_to_utf8(sprite).c_str() );
    jstring jcaption = env->NewStringUTF( cp1251_to_utf8(caption).c_str() );
    jstring jcount = env->NewStringUTF( cp1251_to_utf8(count).c_str() );

    //clazz = env->FindClass("com/russia/game/gui/inventory/Inventory");
    jmethodID method = env->GetMethodID(clazz, "updateItem", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;IZ)V");

    env->CallVoidMethod(CInventory::thiz, method, matrixId, pos, jsprite, jcaption, jcount, rare, active);
    env->DeleteLocalRef(jsprite);
    env->DeleteLocalRef(jcaption);
}

void CInventory::UpdateCarryng(int matrixindex, std::string& carryng) {
    if(thiz == nullptr)
        return;

    auto env = CJavaWrapper::GetEnv();

    jstring jcarryng = env->NewStringUTF( cp1251_to_utf8(carryng).c_str() );

    jmethodID method = env->GetMethodID(clazz, "updateCarryng", "(ILjava/lang/String;)V");
    env->CallVoidMethod(CInventory::thiz, method, matrixindex, jcarryng);

    env->DeleteLocalRef(jcarryng);
}

void CInventory::ToggleAdditionalMatrix(bool toggle, const char* caption = "") {
    if(thiz == nullptr)
        return;

    auto env = CJavaWrapper::GetEnv();

    jstring jcaption = env->NewStringUTF( caption );

    jmethodID method = env->GetMethodID(clazz, "toggleAdditional", "(ZLjava/lang/String;)V");
    env->CallVoidMethod(CInventory::thiz, method, toggle, jcaption);

    env->DeleteLocalRef(jcaption);
}

void CInventory::updateItem(int matrixindex, int pos, bool active) {
    if(thiz == nullptr)
        return;

    auto env = CJavaWrapper::GetEnv();

    jmethodID method = env->GetMethodID(clazz, "itemToggleActive", "(IIZ)V");
    env->CallVoidMethod(CInventory::thiz, method, matrixindex, pos, active);
}

void CInventory::ReceivePacket(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip packet and rpc id

    RpcTypes type;
    bs.Read(type);

    if(type == RpcTypes::TOGGLE) {
        uint8 toggle;
        bs.Read(toggle);

        if(toggle)
            ToggleShow(toggle);
        else
            Destroy();

        return;
    }
    if(type == RpcTypes::ITEM_SELECT) {
        uint8_t matrixId;
        uint8_t pos;
        uint8_t active;

        bs.Read(matrixId);
        bs.Read(pos);
        bs.Read(active);

        CInventory::updateItem(matrixId, pos, active);
        return;
    }
    if(type == RpcTypes::UPDATE_CARRYING) {
        uint8 matrixId;
        std::string carryng;

        bs.Read(matrixId);
        bs.Read(carryng);

        CInventory::UpdateCarryng(matrixId, carryng);
        return;
    }
    if(type == RpcTypes::UPDATE_ITEM) {
        uint8 count;
        bs.Read(count);
       
        while (count) {
            uint8 matrixId, pos;
            bs.Read(matrixId);
            bs.Read(pos);

            std::string sprite;
            bs.ReadStr8(sprite);

            std::string item_count;
            bs.Read(item_count);

            std::string itemName;
            bs.ReadStr8(itemName);

            uint8 selected;
            bs.Read(selected);

            uint8 rareColor;
            bs.Read(rareColor);

            CInventory::UpdateItem(matrixId, pos, sprite, itemName, item_count, rareColor, selected);

            count--;
        }
        return;
    }
    if(type == RpcTypes::SHOW_ADDITIONAL) {
        uint8_t toggle;

        bs.Read(toggle);

        if(!toggle) {
            ToggleAdditionalMatrix(toggle);
            return;
        }

        uint8 len;
        bs.Read(len);
        char caption[len + 1];
        bs.Read(caption, len);
        caption[len] = '\0';

        char caption_utf[len + 1];
        cp1251_to_utf8(caption_utf, caption, len);

        ToggleAdditionalMatrix(toggle, caption_utf);
        return;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_inventory_Inventory_sendSelectItem(JNIEnv *env, jobject thiz, jint matrixId,
                                                    jint pos) {

    RakNet::BitStream bs;
    bs.Write((uint8_t)  ID_CUSTOM_RPC);
    bs.Write((uint8_t)  RPC_INVENTORY);
    bs.Write((uint8_t)  CInventory::RpcTypes::CLICK_CELL);

    bs.Write((uint8_t)  matrixId);
    bs.Write((uint16_t) pos);

    pNetGame->GetRakClient()->Send(&bs, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_inventory_Inventory_sendClickButton(JNIEnv *env, jobject thiz, jint buttonid) {
    RakNet::BitStream bs;
    bs.Write((uint8_t)  ID_CUSTOM_RPC);
    bs.Write((uint8_t)  RPC_INVENTORY);
    bs.Write((uint8_t)  CInventory::RpcTypes::CLICK_BUTTON);
    bs.Write((uint8_t)  buttonid);

    pNetGame->GetRakClient()->Send(&bs, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}