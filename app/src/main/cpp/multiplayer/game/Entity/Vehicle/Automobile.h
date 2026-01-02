//
// Created on 16.09.2023.
//

#pragma once

#include "DamageManager.h"
#include "Enums/eSkidmarkType.h"
#include "Enums/eCarNodes.h"
#include "Door.h"
#include "BouncingPanel.h"
#include "Enums/eCarWheel.h"
#include "../Ped/Ped.h"

class CAutomobile : public CVehicle {
public:
    CDamageManager                      m_damageManager;
    std::array<CDoor, 6>                m_doors;
    std::array<RwFrame*, CAR_NUM_NODES> m_aCarNodes;
    std::array<CBouncingPanel, 3>       m_panels;
    CDoor                               m_swingingChassis;
    std::array<CColPoint, 4>            m_wheelColPoint;                    // 0x724
    std::array<float, 4>                m_fWheelsSuspensionCompression;     // 0x7D4 - [0-1] with 0 being suspension fully compressed, and 1 being completely relaxed - Filled with 1.f in the ctor
    std::array<float, 4>                m_fWheelsSuspensionCompressionPrev; // 0x7E4 - Filled with 1.f in the ctor
    std::array<float, 4>                m_aWheelTimer;

    float field_804;
    float m_fIntertiaValue1; //  m_anWheelSurfaceType[2]
    float m_fIntertiaValue2;

    std::array<eSkidmarkType, 4> m_wheelSkidmarkType;       // 0x810
    std::array<bool,          4> m_wheelSkidmarkBloodState; // 0x820
    std::array<bool,          4> m_wheelSkidmarkMuddy;      // 0x824
    std::array<float,         4> m_wheelRotation;           // 0x828
    std::array<float,         4> m_wheelPosition;           // 0x838
    union {                                                 // 0x848
        std::array<float, 4> m_wheelSpeed;
        struct {
            float m_fHeliWheelSpeed1;
            float m_fHeliRotorSpeed;
            float m_fHeliWheelSpeed3;
            float m_fHeliWheelSpeed4;
        };
    };
    std::array<float, 4> m_fWheelBurnoutSpeed; // 0x858 - Passed to CVehicleMP::ProcessWheel as last 3rd parameter, but it's not used
    struct {
        bool bTaxiLight : 1 { true }; // AKA `bTaxiLightOn`
        bool bShouldNotChangeColour : 1 {}; // AKA `ucNPCFlagPad2`
        bool bWaterTight : 1 {}; // AKA `bIgnoreWater`
        bool bDoesNotGetDamagedUpsideDown : 1 {}; // AKA `bDontDamageOnRoof`
        bool bCanBeVisiblyDamaged : 1 { true }; // AKA `bTakePanelDamage`
        bool bTankExplodesCars : 1 { true }; // AKA `ucTaxiUnkn6`
        bool bIsBoggedDownInSand : 1 {}; // AKA `bLostTraction`
        bool bIsMonsterTruck : 1 {}; // AKA `bSoftSuspension`
    } autoFlags;
    int8   _align;
    bool   m_bDoingBurnout;                         // 0x86A
    uint16 m_wMiscComponentAngle;                   // 0x86C
    uint16 m_wMiscComponentAnglePrev;               // 0x86E
    uint32 m_nBusDoorTimerEnd;                      // 0x870
    uint32 m_nBusDoorTimerStart;                    // 0x874
    std::array<float, 4> m_aSuspensionSpringLength; // 0x878 // By default SuspensionUpperLimit - SuspensionLowerLimit
    std::array<float, 4> m_aSuspensionLineLength;   // 0x888 // By default SuspensionUpperLimit - SuspensionLowerLimit + mi.GetSizeOfWheel(<corresponding wheel>) / 2.f - So I assume line is always longer than the spring
    float m_fFrontHeightAboveRoad;
    float m_fRearHeightAboveRoad;
    float m_fCarTraction;
    float m_fTireTemperature;
    float m_fAircraftGoToHeading;
    float m_fRotationBalance; // Controls destroyed helicopter rotation
    float m_fMoveDirection;
    CVector m_moveForce;
    CVector m_turnForce;
    std::array<float, 6> DoorRotation; // Inited in ctor with random values, but seemingly unused.

    float m_fBurnTimer;

    std::array<CPhysical*, 4> m_apWheelCollisionEntity;
    std::array<CVector, 4>    m_vWheelCollisionPos; // Bike::m_avTouchPointsLocalSpace

    CPed* m_pExplosionVictim;
    std::array<char, 24> field_928;

    float LeftDoorOpenForDriveBys;
    float RightDoorOpenForDriveBys;
    float m_fDoomVerticalRotation;
    float m_fDoomHorizontalRotation;
    float m_fForcedOrientation;
    std::array<float, 2> m_fUpDownLightAngle;
    uint8 m_nNumContactWheels;
    uint8 m_nWheelsOnGround;
    uint8 m_wheelsOnGrounPrev;
    float m_fGasPedalAudio; // [0; 1] adjusts the speed of playback of the skiding sound

    std::array<tWheelState, 4> m_aWheelState;
    std::array<void*, 2> m_exhaustNitroFxSystem;

    uint8 m_harvesterParticleCounter;
    uint8 m_fireParticleCounter;
    int16 __pad_982;
    float m_heliDustFxTimeConst;

    // variables
    static constexpr float PACKER_COL_ANGLE_MULT   = -0.0001f;
    static constexpr float FORKLIFT_COL_ANGLE_MULT = 0.0006f;
    static constexpr float DOZER_COL_ANGLE_MULT    = 0.0002f;
    static constexpr float ROLL_ONTO_WHEELS_FORCE  = 0.0025f;
    static bool&           m_sAllTaxiLights;
    static CVector&        vecHunterGunPos; // { 0.0f, 4.8f, -1.3f }
    static CMatrix*        matW2B;

    static constexpr auto Type = VEHICLE_TYPE_AUTOMOBILE;

public:
    bool BreakTowLink();
};

VALIDATE_SIZE(CAutomobile, (VER_x32 ? 0x99C : 0xBC8));
