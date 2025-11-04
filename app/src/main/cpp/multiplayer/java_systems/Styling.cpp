//
// Created by admin on 05.04.2023.
//

#include "Styling.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "Notification.h"
#include "../game/Entity/Ped/Ped.h"

constexpr int STYLING_ITEMS_COUNT = 9;

void CStyling::Show(int money, int total, const uint32_t* prices) {
    auto env = CJavaWrapper::GetEnv();

    if(!thiz)
        Constructor();

    jmethodID method = env->GetMethodID(CStyling::clazz, "show", "(II[I)V");

    jint arr[STYLING_ITEMS_COUNT];
    for (int i = 0; i < STYLING_ITEMS_COUNT ; i++) {
        arr[i] = prices[i];
    }

    jintArray array = env->NewIntArray(STYLING_ITEMS_COUNT);
    env->SetIntArrayRegion(array, 0, STYLING_ITEMS_COUNT, arr);

    env->CallVoidMethod(thiz, method, money, total, array);

    env->DeleteLocalRef(array);
    bIsShow = true;
}

uint32 CStyling::GetValueFromType(eValueType type) {
    auto pPed = CGame::FindPlayerPed();

    if(!pPed->m_pPed->IsInVehicle()) return 0;

    auto pVehicle = pPed->GetCurrentVehicle();

    switch(type) {
        case eValueType::VALUE_TYPE_NEON_TYPE: {
            return static_cast<uint32>(pVehicle->neon.neonType);
        }
        case eValueType::VALUE_TYPE_NEON_COLOR: {
            return pVehicle->neon.neonColor.ToInt();
        }
        case eValueType::VALUE_TYPE_LIGHT_TYPE: {
            return static_cast<uint32>(pVehicle->m_bIsLightOn);
        }
        case eValueType::VALUE_TYPE_LIGHT_COLOR: {
            return pVehicle->lightColor.ToInt();
        }
        case eValueType::VALUE_TYPE_TONER_COLOR: {
            return pVehicle->tonerColor.ToInt();
        }
        case eValueType::VALUE_TYPE_BODY1_COLOR: {
            return pVehicle->mainColor.ToInt();
        }
        case eValueType::VALUE_TYPE_BODY2_COLOR: {
            return pVehicle->secondColor.ToInt();
        }
        case eValueType::VALUE_TYPE_WHEEL_COLOR: {
            return pVehicle->wheelColor.ToInt();
        }
        case eValueType::VALUE_TYPE_VINYL: {
            return pVehicle->m_nVinylIndex;
        }
        case eValueType::VALUE_TYPE_STROB: {
            return static_cast<uint32>(pVehicle->m_iStrobsType);
        }
    }
    assert("invalid type");
}

void CNetGame::packetStylingCenter(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    bool toggle;
    uint32_t balance, total;
    uint32_t prices[STYLING_ITEMS_COUNT];

    bs.Read(toggle);
    bs.Read(balance);
    bs.Read(total);

    for(int i = 0; i < STYLING_ITEMS_COUNT; i++) {
        bs.Read(prices[i]);
    }


    if(toggle) {
        CStyling::Show(balance, total, prices);
    }
    else
        CStyling::Destroy();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_styling_Styling_nativeOnExit(JNIEnv *env, jobject thiz) {
    CStyling::DeleteCppObject();

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_STYLING_CENTER);
    bsSend.Write((uint8_t)  CStyling::ePacketType::EXIT);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_styling_Styling_nativeSendBuy(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_STYLING_CENTER);
    bsSend.Write((uint8_t)  CStyling::ePacketType::BUY);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_styling_Styling_nativeToggleCam(JNIEnv *env, jobject thiz, jboolean cam) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t) ID_CUSTOM_RPC);
    bsSend.Write((uint8_t) RPC_STYLING_CENTER);
    bsSend.Write((uint8_t) CStyling::ePacketType::TOGGLE_CAM);
    bsSend.Write((uint8_t)(cam ? 1 : 0));

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_lit_game_gui_styling_Styling_nativeIsAvailable(JNIEnv *env, jobject thiz, jint type) {
    auto valueType = (CStyling::eValueType)(type);

    auto pPed = CGame::FindPlayerPed();
    if(!pPed->m_pPed->IsInVehicle()) return 0;
    auto pVehicle = pPed->GetCurrentVehicle();

    switch(valueType) {
        case CStyling::eValueType::VALUE_TYPE_NEON_TYPE: {
           return true;
        }
        case CStyling::eValueType::VALUE_TYPE_NEON_COLOR: {
           if(pVehicle->neon.neonType != eNeonTypes::ON_TYPE_STATIC && pVehicle->neon.neonType != eNeonTypes::ON_TYPE_FLASH) {
                CNotification::show(0, "���������� � ������ ����� �����", 5, 0, "", "");
                return false;
            }
        }
        case CStyling::eValueType::VALUE_TYPE_LIGHT_TYPE: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_LIGHT_COLOR: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_TONER_COLOR: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_BODY1_COLOR: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_BODY2_COLOR: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_WHEEL_COLOR: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_VINYL: {
            return true;
        }
        case CStyling::eValueType::VALUE_TYPE_STROB: {
            return true;
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_styling_Styling_nativeSendChangeValue(JNIEnv *env, jobject thiz, jint value_type) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_STYLING_CENTER);
    bsSend.Write((uint8_t)  CStyling::ePacketType::UPDATE_VALUE);
    bsSend.Write((uint8_t)  value_type);
    bsSend.Write((uint32)   CStyling::GetValueFromType((CStyling::eValueType)value_type));

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_styling_Styling_nativeResetTuning(JNIEnv *env, jobject thiz, jint type) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_STYLING_CENTER);
    bsSend.Write((uint8_t)  CStyling::ePacketType::RESET_VALUE);
    bsSend.Write((uint8_t)  type);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_styling_Styling_nativeChangeValue(JNIEnv *env, jobject thiz, jint type, jint value) {
    auto pPed = CGame::FindPlayerPed();
    if(!pPed->m_pPed->IsInVehicle()) return;
    auto pVehicle = pPed->GetCurrentVehicle();

    auto valueType = (CStyling::eValueType)(type);

    switch(valueType) {
        case CStyling::eValueType::VALUE_TYPE_LIGHT_TYPE: {
            pVehicle->m_bIsLightOn = static_cast<eLightsState>(value);
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_LIGHT_COLOR: {
            pVehicle->lightColor.Set(value);
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_TONER_COLOR: {
            pVehicle->tonerColor.Set(value);
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_BODY1_COLOR: {
            pVehicle->mainColor.Set(value);
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_BODY2_COLOR: {
            pVehicle->secondColor.Set(value);
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_WHEEL_COLOR: {
            pVehicle->wheelColor.Set(value);
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_NEON_COLOR: {
            pVehicle->neon.neonColor.Set(value); // y - ??????
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_NEON_TYPE: {
            pVehicle->neon.neonType = static_cast<eNeonTypes>(value);
            pVehicle->neon.neonColor = 0xFFFFFFFF;
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_VINYL: {
            CGame::PostToMainThread([=] {
                pVehicle->ChangeVinylTo(value);
            });
            break;
        }
        case CStyling::eValueType::VALUE_TYPE_STROB: {
            pVehicle->m_iStrobsType = static_cast<eStobsStatus>(value);
            pVehicle->SetStrob(pVehicle->m_iStrobsType);
            break;
        }
    }
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_lit_game_gui_styling_Styling_nativeGetCurrentValue(JNIEnv *env, jobject thiz, jint type) {
    return CStyling::GetValueFromType(static_cast<CStyling::eValueType>(type));
}