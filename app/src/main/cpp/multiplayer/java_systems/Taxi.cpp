//
// Created on 03.10.2023.
//

#include "Taxi.h"
#include "../net/netgame.h"

void CTaxi::ReceivePacket(Packet *p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40); // skip packet and rpc id

    RPC_TYPE type;
    bs.Read(type);

    if(type == RPC_TYPE::TOGGLE) {
        uint8 toggle;
        bs.Read(toggle);

        if(toggle)
            Constructor();
        else
            Destroy();

        return;
    }
    if(type == RPC_TYPE::NOTIFICATION) {
        float distance;
        bs.Read(m_nOrderId);
        bs.Read(distance);

        CTaxi::NewOrder(distance);
        return;
    }
    if(type == RPC_TYPE::INFO) {
        uint8 fareType;
        uint16 ordersCount;
        uint32 salary;

        bs.Read(fareType);
        bs.Read(ordersCount);
        bs.Read(salary);

        CTaxi::UpdateInfo(fareType, ordersCount, salary);
        return;
    }
}

void CTaxi::NewOrder(float distance) {
    if(!thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();

    auto method = env->GetMethodID(clazz, "newOrder", "(F)V");
    env->CallVoidMethod(thiz, method, distance);
}

void CTaxi::UpdateInfo(int fare, int orderCounts, int salary) {
    if(!thiz)
        Constructor();

    auto env = CJavaWrapper::GetEnv();

    jstring jnick = env->NewStringUTF( CPlayerPool::GetLocalPlayerName() );

    auto method = env->GetMethodID(clazz, "updateInfo", "(Ljava/lang/String;III)V");
    env->CallVoidMethod(thiz, method, jnick, fare, orderCounts, salary);

    env->DeleteLocalRef(jnick);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_Taxi_nativeAcceptOrder(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bs;
    bs.Write((uint8_t)  ID_CUSTOM_RPC);
    bs.Write((uint8_t)  RPC_TAXI);
    bs.Write((uint8_t)  CTaxi::RPC_TYPE::APPLY_ORDER);
    bs.Write((uint16_t) CTaxi::m_nOrderId);

    pNetGame->GetRakClient()->Send(&bs, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}