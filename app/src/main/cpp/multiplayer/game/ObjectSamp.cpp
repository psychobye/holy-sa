#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "util/patch.h"
#include "Timer.h"
#include "game/Models/ModelInfo.h"
#include <cmath>

float fixAngle(float angle)
{
	if (angle > 180.0f) angle -= 360.0f;
	if (angle < -180.0f) angle += 360.0f;
	return angle;
}

float subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

CObjectSamp::CObjectSamp(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance)
{
	if(!CModelInfo::GetModelInfo(iModel))
		iModel = 18631; // ��������

	m_pEntity 			= 0;
	m_dwGTAId 			= 0;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &m_dwGTAId);
	ScriptCommand(&put_object_at, m_dwGTAId, fPosX, fPosY, fPosZ);

	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);

	m_bIsPlayerSurfing = false;
	m_bNeedRotate = false;

	m_bAttachedType = eObjectAttachType::NONE;
	m_usAttachedVehicle = 0xFFFF;

    if (m_pEntity) {
        auto it = std::find(objectToIdMap.begin(), objectToIdMap.end(), m_pEntity);
        if (it == objectToIdMap.end()) {
            objectToIdMap.push_back(m_pEntity);
        }
    }
	InstantRotate(vecRot.x, vecRot.y, vecRot.z);
}

CObjectSamp::~CObjectSamp()
{
	auto modelId = m_pEntity->m_nModelIndex;

	if(m_pEntity)
		ScriptCommand(&destroy_object, m_dwGTAId);

    if (m_pEntity) {
        auto it = std::find(objectToIdMap.begin(), objectToIdMap.end(), m_pEntity);
        if (it != objectToIdMap.end()) {
            objectToIdMap.erase(it);
        }
        ScriptCommand(&destroy_object, m_dwGTAId);
    }
	CStreaming::RemoveModelIfNoRefs(modelId);
}

void CObjectSamp::Process(float fElapsedTime)
{
	if (m_bAttachedType == eObjectAttachType::TO_VEHICLE)
	{
		CVehicleMP* pVehicle = CVehiclePool::GetAt(m_usAttachedVehicle);
		if (pVehicle)
		{
			if (pVehicle->m_pVehicle->IsAdded())
			{
				ProcessAttachToVehicle(pVehicle);
			}
		}
	}
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if (!m_pEntity) return;
	if (!(m_pEntity->m_matrix)) return;
	if (m_bIsMoving)
	{
		CVector vecSpeed = { 0.0f, 0.0f, 0.0f };
		RwMatrix matEnt;
		matEnt = m_pEntity->GetMatrix().ToRwMatrix();
		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matEnt);
		uint32_t dwThisTick = GetTickCount();

		float posX = matEnt.pos.x;
		float posY = matEnt.pos.y;
		float posZ = matEnt.pos.z;

		float f1 = ((float)(dwThisTick - m_iStartMoveTick)) * 0.001f * m_fMoveSpeed;
		float f2 = m_fDistanceToTargetPoint - remaining;

		if (distance >= remaining)
		{
			m_pEntity->SetVelocity(vecSpeed);
			m_pEntity->SetTurnSpeed(vecSpeed);

			matEnt.pos = m_matTarget.pos;

			if (m_bNeedRotate) {
				m_quatTarget.GetMatrix(&matEnt);
			}

            m_pEntity->Remove();

			m_pEntity->SetMatrix((CMatrix&)matEnt);
			m_pEntity->UpdateRW();
			m_pEntity->UpdateRwFrame();

            m_pEntity->Add();

			StopMoving();
			return;
		}

		if (fElapsedTime <= 0.0f)
			return;

		float delta = 1.0f / (remaining / distance);
		matEnt.pos.x += ((m_matTarget.pos.x - matEnt.pos.x) * delta);
		matEnt.pos.y += ((m_matTarget.pos.y - matEnt.pos.y) * delta);
		matEnt.pos.z += ((m_matTarget.pos.z - matEnt.pos.z) * delta);

		distance = remaining / m_fDistanceToTargetPoint;
		float slerpDelta = 1.0f - distance;

		delta = 1.0f / fElapsedTime;
		vecSpeed.x = (matEnt.pos.x - posX) * delta * 0.02f;
		vecSpeed.y = (matEnt.pos.y - posY) * delta * 0.02f;
		vecSpeed.z = (matEnt.pos.z - posZ) * delta * 0.02f;

		if (FloatOffset(f1, f2) > 0.1f)
		{
			if (f1 > f2)
			{
				delta = (f1 - f2) * 0.1f + 1.0f;
				vecSpeed *= delta;
			}

			if (f2 > f1)
			{
				delta = 1.0f - (f2 - f1) * 0.1f;
				vecSpeed *= delta;
			}
		}

		m_pEntity->SetVelocity(vecSpeed);
		m_pEntity->ApplyMoveSpeed();

		if (m_bNeedRotate)
		{
			float fx, fy, fz;
			GetRotation(&fx, &fy, &fz);
			distance = m_vecRotationTarget.z - distance * m_vecSubRotationTarget.z;
			vecSpeed.x = 0.0f;
			vecSpeed.y = 0.0f;
			vecSpeed.z = subAngle(remaining, distance) * 0.01f;
			if (vecSpeed.z <= 0.001f)
			{
				if (vecSpeed.z < -0.001f)
					vecSpeed.z = -0.001f;
			}
			else
			{
				vecSpeed.z = 0.001f;
			}

			m_pEntity->SetTurnSpeed(vecSpeed);

			m_pEntity->GetMatrix(&matEnt);
			CQuaternion quat;
			quat.Slerp(&m_quatStart, &m_quatTarget, slerpDelta);
			quat.Normalize();
			quat.GetMatrix(&matEnt);
		}
		else
		{
			m_pEntity->GetMatrix(&matEnt);
		}

        m_pEntity->Remove();

		m_pEntity->SetMatrix((CMatrix&)matEnt);
		m_pEntity->UpdateRW();
		m_pEntity->UpdateRwFrame();

        m_pEntity->Add();
	}
}

float CObjectSamp::DistanceRemaining(RwMatrix *matPos)
{

	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.x - m_matTarget.pos.x) * (matPos->pos.x - m_matTarget.pos.x);
	fSY = (matPos->pos.y - m_matTarget.pos.y) * (matPos->pos.y - m_matTarget.pos.y);
	fSZ = (matPos->pos.z - m_matTarget.pos.z) * (matPos->pos.z - m_matTarget.pos.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObjectSamp::SetPos(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		ScriptCommand(&put_object_at, m_dwGTAId, x, y, z);
	}
}



void CObjectSamp::MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ)
{
	RwMatrix mat;
	m_pEntity->GetMatrix(&mat);

	if (m_bIsMoving) {
		this->StopMoving();
		mat.pos = m_matTarget.pos;

		if (m_bNeedRotate) {
			m_quatTarget.GetMatrix(&mat);
		}

        m_pEntity->Remove();

		m_pEntity->SetMatrix((CMatrix&)mat);
		m_pEntity->UpdateRW();
		m_pEntity->UpdateRwFrame();

        m_pEntity->Add();
	}

	m_iStartMoveTick = GetTickCount();
	m_fMoveSpeed = fSpeed;
	m_matTarget.pos = {fX, fY, fZ};

	m_bIsMoving = true;

	if (fRotX <= -999.0f || fRotY <= -999.0f || fRotZ <= -999.0f) {
		m_bNeedRotate = false;
	}
	else
	{
		m_bNeedRotate = true;

		CVector vecRot;
		RwMatrix matrix;
		this->GetRotation(&vecRot.x, &vecRot.y, &vecRot.z);

		m_vecRotationTarget.x = fixAngle(fRotX);
		m_vecRotationTarget.y = fixAngle(fRotY);
		m_vecRotationTarget.z = fixAngle(fRotZ);

		m_vecSubRotationTarget.x = subAngle(vecRot.x, fRotX);
		m_vecSubRotationTarget.y = subAngle(vecRot.y, fRotY);
		m_vecSubRotationTarget.z = subAngle(vecRot.z, fRotZ);

		this->InstantRotate(fRotX, fRotY, fRotZ);
		m_pEntity->GetMatrix(&matrix);

		m_matTarget.right = matrix.right;
		m_matTarget.at = matrix.at;
		m_matTarget.up = matrix.up;

		this->InstantRotate(vecRot.x, vecRot.y, vecRot.z);
		m_pEntity->GetMatrix(&matrix);

		m_quatStart.SetFromMatrix(&matrix);
		m_quatTarget.SetFromMatrix(&m_matTarget);
		m_quatStart.Normalize();
		m_quatTarget.Normalize();
	}

	m_fDistanceToTargetPoint = m_pEntity->GetDistanceFromPoint(m_matTarget.pos.x, m_matTarget.pos.y, m_matTarget.pos.z);

	if (pNetGame) {
		CLocalPlayer::UpdateSurfing();
	}
}

void CObjectSamp::AttachToVehicle(uint16_t usVehID, CVector* pVecOffset, CVector* pVecRot)
{
	m_bAttachedType = eObjectAttachType::TO_VEHICLE;
	m_usAttachedVehicle = usVehID;
	m_vecAttachedOffset.x = pVecOffset->x;
	m_vecAttachedOffset.y = pVecOffset->y;
	m_vecAttachedOffset.z = pVecOffset->z;

	m_vecAttachedRotation.x = pVecRot->x;
	m_vecAttachedRotation.y = pVecRot->y;
	m_vecAttachedRotation.z = pVecRot->z;
}

void CObjectSamp::ProcessAttachToVehicle(CVehicleMP* pVehicle)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		if (!ScriptCommand(&is_object_attached, m_dwGTAId) || bNeedReAttach)
		{
			ScriptCommand(&attach_object_to_car, m_dwGTAId, pVehicle->m_dwGTAId, m_vecAttachedOffset.x,
				m_vecAttachedOffset.y, m_vecAttachedOffset.z, m_vecAttachedRotation.x, m_vecAttachedRotation.y, m_vecAttachedRotation.z);
		}
	}
}

void CObjectSamp::InstantRotate(float x, float y, float z)
{
	if(!m_pEntity)return;
	x = DegreesToRadians(x);
	y = DegreesToRadians(y);
	z = DegreesToRadians(z);

    m_pEntity->Remove();

	m_pEntity->SetOrientation(x, y, z);

	m_pEntity->UpdateRW();
	m_pEntity->UpdateRwFrame();

    m_pEntity->Add();
}

void CObjectSamp::StopMoving()
{
	CVector vec = { 0.0f, 0.0f, 0.0f };
	this->m_pEntity->ResetMoveSpeed();
	this->m_pEntity->SetTurnSpeed(vec);
	m_bIsMoving = false;
}

void CObjectSamp::SetRot(float &radX, float &radY, float &radZ)
{
    m_pEntity->Remove();

	m_pEntity->SetOrientation(radX, radY, radZ);

	m_pEntity->UpdateRW();
	m_pEntity->UpdateRwFrame();

    m_pEntity->Add();
}

void CObjectSamp::GetRotation(float* pfX,float* pfY,float* pfZ)
{
	if (!m_pEntity) return;

	m_pEntity->m_matrix->ConvertToEulerAngles(pfX, pfY, pfZ, 21);

	*pfX = *pfX * 57.295776 * -1.0;
	*pfY = *pfY * 57.295776 * -1.0;
	*pfZ = *pfZ * 57.295776 * -1.0;

}