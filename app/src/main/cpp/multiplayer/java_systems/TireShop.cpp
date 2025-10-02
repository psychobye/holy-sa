//
// Created by �������� ���� on 20.03.2023.
//
#include "TireShop.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "Speedometr.h"
#include "../game/Entity/Ped/Ped.h"

#include "HUD.h"

void CTireShop::show(uint32_t to_pay) {
    auto env = g_pJavaWrapper->GetEnv();

    if(!CTireShop::thiz)
        Constructor();

    auto method = env->GetMethodID(CTireShop::clazz, "update", "(II)V");
    env->CallVoidMethod(CTireShop::thiz, method, to_pay, CHUD::iLocalMoney);

    CTireShop::bIsShow = true;
}

void CNetGame::packetTireShop(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    bool toggle;
    uint32_t price;

    bs.Read(toggle);
    bs.Read(price);

    if(toggle)
        CTireShop::show(price);
    else
        CTireShop::Destroy();
}

float CTireShop::GetCurrentValue(int valueType) {
    auto pPed = CGame::FindPlayerPed();

    if(!pPed->m_pPed->IsInVehicle()) 0;

    auto pVehicle = pPed->GetCurrentVehicle();

    switch(valueType) {
        // ��������
        case 2: {
            for (auto & i : pVehicle->m_msLastSyncHandling) {
                if (i.flag == E_HANDLING_PARAMS::hpSuspensionLowerLimit)
                    return i.fValue;
            }
            return 0;
        }

            // ���������
        case 3: {
            return pVehicle->m_fWheelOffsetX;
        }

            // ������ ������
        case 4: {
            return pVehicle->m_fWheelWidth * 100.f;
        }

            // ������ �����
        case 5: {
            return pVehicle->m_fWheelSize;
        }

        case 6: {
            return pVehicle->m_dwCurrentWheelModel;
        }
            // ����-������ �����
        case 11: {
            return RADTODEG(pVehicle->m_fWheelAngleFront);
        }

            // ����-������ ���
        case 12: {
            return RADTODEG(pVehicle->m_fWheelAngleBack);
        }

        default: return 0;
    }
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_lit_game_gui_tire_1shop_TireShop_native_1getCurrentValue(JNIEnv *env, jobject thiz,
                                                                       jint type) {
    return CTireShop::GetCurrentValue(type);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_tire_1shop_TireShop_native_1onChange(JNIEnv *env, jobject thiz,
                                                                jint type, jfloat value) {

    auto pPed = CGame::FindPlayerPed();

    if(!pPed->m_pPed->IsInVehicle()) return;

    auto pVehicle = pPed->GetCurrentVehicle();
 //   auto pGtaVehicle = pVehicle->m_pVehicle;

    switch(type) {
        // ��������
        case 2: {
            auto& handling = pVehicle->m_msLastSyncHandling;

            handling.erase(
                    std::remove_if(
                            handling.begin(),
                            handling.end(),
                            [](const auto& entry) {
                                return entry.flag == E_HANDLING_PARAMS::hpSuspensionLowerLimit;
                            }),
                    handling.end()
            );

            handling.emplace_back(E_HANDLING_PARAMS::hpSuspensionLowerLimit, value);
            pVehicle->SetHandlingData();

            break;
        }

        // ���������
        case 3: {
            pVehicle->SetWheelOffset(true, value);
            pVehicle->SetWheelOffset(false, value);
         //   pVehicle->SetHandlingData();
            break;
        }

        // ������ ������
        case 4: {
            pVehicle->SetWheelWidth(value);
            break;
        }

        // ������ �����
        case 5: {
            auto& handling = pVehicle->m_msLastSyncHandling;

            handling.erase(
                    std::remove_if(
                            handling.begin(),
                            handling.end(),
                            [](const auto& entry) {
                                return entry.flag == E_HANDLING_PARAMS::hpWheelSize;
                            }),
                    handling.end()
            );

            handling.emplace_back(E_HANDLING_PARAMS::hpWheelSize, value);

            pVehicle->SetHandlingData();
            break;
        }

        case 6: {
            pVehicle->m_dwChangeWheelTo = (int)value;
            break;
        }

        // ����-������ �����
        case 11: {
            pVehicle->SetWheelAngle(true, value);
            break;
        }

        // ����-������ ���
        case 12: {
            pVehicle->SetWheelAngle(false, value);
            break;
        }
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_tire_1shop_TireShop_nativeSendClick(JNIEnv *env, jobject thiz, jint type) {

    if(type == 7) {// exit
        CTireShop::DeleteCppObject();
    }

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)  ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)  RPC_TUNING_WHEELS);
    bsSend.Write((int8_t)   type);
    bsSend.Write((float)    CTireShop::GetCurrentValue(type));

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}