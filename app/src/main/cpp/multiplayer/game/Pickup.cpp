//
// Created on 08.11.2023.
//

#include "Pickup.h"
#include "World.h"
#include "util/patch.h"
#include "Camera.h"
#include "net/netgame.h"


// Delete pickup's object (CObjectSamp)
void CPickup::GetRidOfObjects() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0031D790 + 1 : 0x3E4A20), this);

    // Need destructor :c
//    if (m_pObject) {
//        CWorld::Remove(m_pObject);
//        delete m_pObject;
//        m_pObject = nullptr;
//    }
}

// Creates an object (CObjectSamp) for pickup. slotIndex - object to replace; use -1 (or any negative value) to create a new object
void CPickup::GiveUsAPickUpObject(CObject** obj, int32 slotIndex) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0031D510 + 1 : 0x3E46D4), this, obj, slotIndex);
}

// Is pickup visible (checks if distance between pickup and camera is shorter than 100 units)
bool CPickup::IsVisible() {
    

    return DistanceBetweenPoints2D(GetPosn2D(), CCamera::Get().GetPosition()) < 100.0f;
}

bool CPickup::Update() {
    auto &ped = CLocalPlayer::GetPlayerPed()->m_pPed;

    auto oldState = m_bIsPicked;

    auto checkPickup = [&]() {
        if (m_nPickupType == PICKUP_2P) {
            if (!ped->IsInVehicle())
                return false;

            if (DistanceBetweenPoints(GetPosn(), ped->pVehicle->GetPosition()) < 1.0f) {
                Remove();
                return true;
            }
            else
                return false;
        }
        else {
            return DistanceBetweenPoints(GetPosn(), ped->GetPosition()) < 0.9f;
        }
    };

    m_bIsPicked = checkPickup();

    if (oldState == false && m_bIsPicked == true)
        return true;

    return false;
}


void CPickup::Remove() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x002D7DC8 + 1 : 0x3E427C), this);
}
