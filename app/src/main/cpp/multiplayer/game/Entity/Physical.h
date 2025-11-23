//
// Created on 09.04.2023.
//

#pragma once

#include "game/Enums/eSurfaceType.h"
#include "Entity.h"
#include "game/Core/PtrNodeDoubleLink.h"
#include "game/Core/EntryInfoList.h"
#include "game/Shadow/RealTimeShadow.h"

//#pragma pack(push, 1)
struct CPhysical : public CEntity {
    float       m_fPrevDistFromCam;
    uint32_t    m_LastCollisionTime;
    union {
        struct {
            uint32 bMakeMassTwiceAsBig : 1;
            uint32 bApplyGravity : 1;
            uint32 bDisableCollisionForce : 1;
            uint32 bCollidable : 1;
            uint32 bDisableTurnForce : 1;
            uint32 bDisableMoveForce : 1;
            uint32 bInfiniteMass : 1;
            uint32 bDisableZ : 1;

            uint32 bSubmergedInWater : 1;
            uint32 bOnSolidSurface : 1;
            uint32 bBroken : 1;
            uint32 bProcessCollisionEvenIfStationary : 1; // ref @ 0x6F5CF0
            uint32 bSkipLineCol : 1;                               // only used for peds
            uint32 bDontApplySpeed : 1;
            uint32 b15 : 1;
            uint32 bProcessingShift : 1;

            uint32 b17 : 1;
            uint32 bDisableSimpleCollision : 1; // ref @ CPhysical::ProcessCollision
            uint32 bBulletProof : 1;
            uint32 bFireProof : 1;
            uint32 bCollisionProof : 1;
            uint32 bMeleeProof : 1;
            uint32 bInvulnerable : 1;
            uint32 bExplosionProof : 1;

            uint32 bDontCollideWithFlyers : 1;
            uint32 bAttachedToEntity : 1;
            uint32 bAddMovingCollisionSpeed : 1;
            uint32 bTouchingWater : 1;
            uint32 bCanBeCollidedWith : 1;
            uint32 bDestroyed : 1;
            uint32 b31 : 1;
            uint32 b32 : 1;
        } physicalFlags;
        uint32 m_nPhysicalFlags;
    };
    CVector         m_vecMoveSpeed;
    CVector         m_vecTurnSpeed;
    CVector         m_vecFrictionMoveSpeed;
    CVector         m_vecFrictionTurnSpeed;
    CVector         m_vecForce;
    CVector         m_vecTorque;
    float           m_fMass;
    float           m_fTurnMass;
    float           m_fVelocityFrequency;
    float           m_fAirResistance;
    float           m_fElasticity;
    float           m_fBuoyancyConstant;
    CVector         m_vecCentreOfMass;
    CEntryInfoList  m_pCollisionList;

    CPtrNodeDoubleLink *m_pMovingList;

    uint8_t         m_nFakePhysics;
    uint8_t         m_nNumEntitiesCollided;
    eSurfaceType    m_nContactSurface;
    uint8_t         _pad4;

    CEntity*        m_apCollidedEntities[6];
    float           m_fMovingSpeed; // m_fTrueDistanceTravelled
    float           m_fDamageIntensity; // m_fDamageImpulseMagnitude
    CEntity         *m_pDamageEntity;
    CVector         m_vecLastCollisionImpactVelocity;
    CVector         m_vecLastCollisionPosn;
    uint16_t        m_nPieceType;
    uint8_t         _pad5[2];
    CEntity         *m_pAttachedTo;
    CVector         m_vecAttachPosnOffset;
    CVector         m_vecAttachTiltOffset;
    CQuaternion     m_qAttachedEntityRotation;
    CEntity         *m_pEntityIgnoredCollision;
    float           m_fContactSurfaceBrightness;
    float           m_fDynamicLighting;
    CRealTimeShadow *m_pShadowData;

public:
    static inline float DAMPING_LIMIT_IN_FRAME = 0.25;
    static inline float DAMPING_LIMIT_OF_SPRING_FORCE = 0.999;
    static inline float PHYSICAL_SHIFT_SPEED_DAMP = 0.707;
    static inline float SOFTCOL_SPEED_MULT = 0.95;
    static inline float SOFTCOL_SPEED_MULT2 = 1.05;
    static inline float SOFTCOL_DEPTH_MIN = 0.5;
    static inline float SOFTCOL_DEPTH_MULT = 2.0;
    static inline float SOFTCOL_CARLINE_SPEED_MULT = 0.4;
    static inline float TEST_ADD_AMBIENT_LIGHT_FRAC = 0.5;
    static inline float HIGHSPEED_ELASTICITY_MULT_COPCAR = 2.0;
  //  static CVector& fxDirection;

public:
    CPhysical();
    ~CPhysical() override;

    // originally virtual functions
    void Add() override;
    void Remove() override;

    static void InjectHooks();

    void RemoveAndAdd();
    void AddToMovingList();

    void RemoveFromMovingList();
    //void SetDamagedPieceRecord(float fDamageIntensity, CEntity* entity, CColPoint& colPoint, float fDistanceMult);
    void ApplyMoveForce(float x, float y, float z);
    void ApplyMoveForce(CVector force);
    void ApplyTurnForce(CVector force, CVector point);
    void ApplyForce(CVector vecMoveSpeed, CVector point, bool bUpdateTurnSpeed);
    bool ApplySpringDampening(float fDampingForce, float fSpringForceDampingLimit, CVector& direction, CVector& collisionPoint, CVector& collisionPos);
    bool ApplySpringDampeningOld(float arg0, float arg1, CVector& arg2, CVector& arg3, CVector& arg4);
    bool ApplySpringCollisionAlt(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, CVector& normal, float& fSpringForceDampingLimit);

    CVector GetSpeed( CVector point);
    void ApplyMoveSpeed();
    void ApplyTurnSpeed();

    CVector& GetMoveSpeed()                 { return m_vecMoveSpeed; }
    void     SetVelocity(CVector velocity)  { m_vecMoveSpeed = velocity; } // 0x441130

    CVector& GetTurnSpeed()         { return m_vecTurnSpeed; }
    void ResetTurnSpeed()           { m_vecTurnSpeed = CVector(); }
    void SetTurnSpeed(CVector vec)  { m_vecTurnSpeed = vec; }

    void ResetMoveSpeed()           { SetVelocity(CVector{}); }
    void ResetFrictionMoveSpeed()   { m_vecFrictionMoveSpeed = CVector(); }
    void ResetFrictionTurnSpeed()   { m_vecFrictionTurnSpeed = CVector(); }

    float GetMass(const CVector& pos, const CVector& dir) {
        return 1.0f / (CrossProduct(pos, dir).SquaredMagnitude() / m_fTurnMass +
                       1.0f / m_fMass);
    }

    bool IsAdded();

    void ApplyFrictionForce(CVector vecMoveForce, CVector point);

    void ApplyFrictionMoveForce(CVector moveForce);

    bool ApplyFriction(CPhysical *entity, float fFriction, CColPoint &colPoint);
};
//#pragma pack(pop)

static_assert(sizeof(CPhysical) == (VER_x32 ? 0x13C : 0x198), "Invalid size CPhysical");