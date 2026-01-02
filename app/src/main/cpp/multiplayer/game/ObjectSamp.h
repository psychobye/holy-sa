#pragma once

#include "game/Core/Vector.h"

enum class eObjectAttachType {
    NONE,
    TO_PLAYER,
    TO_VEHICLE,
};

class CObjectSamp
{
public:
	RwMatrix	        m_matTarget;
	RwMatrix	        m_matCurrent;
	bool 		        m_bIsMoving {false};
	float		        m_fMoveSpeed {0.f};
	bool		        m_bIsPlayerSurfing;
	bool		        m_bNeedRotate;

	CQuaternion         m_quatTarget;
	CQuaternion         m_quatStart;

	CVector             m_vecAttachedOffset;
	CVector             m_vecAttachedRotation;
	uint16_t            m_usAttachedVehicle;
    eObjectAttachType   m_bAttachedType;

	CVector 	        m_vecRot;
	CVector		        m_vecRotationTarget;
	CVector		        m_vecSubRotationTarget;
	float		        m_fDistanceToTargetPoint;
	uint32_t	        m_iStartMoveTick;
	bool 		        bNeedReAttach = false;

	CPhysical		    *m_pEntity;
	uint32_t		    m_dwGTAId;

    static inline       std::vector<CEntity*> objectToIdMap {};

	CObjectSamp(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance);
	~CObjectSamp();

	void Process(float fElapsedTime);
	float DistanceRemaining(RwMatrix *matPos);

	void SetPos(float x, float y, float z);
	void MoveTo(float x, float y, float z, float speed, float rX, float rY, float rZ);

	void AttachToVehicle(uint16_t usVehID, CVector* pVecOffset, CVector* pVecRot);
	void ProcessAttachToVehicle(CVehicleMP* pVehicle);

	void InstantRotate(float x, float y, float z);
	void StopMoving();

	void GetRotation(float* pfX, float* pfY, float* pfZ);
    void SetRot(float &radX, float &radY, float &radZ);
};