#pragma once
#include "aimstuff.h"
#include "VehicleMP.h"
#include "Weapon.h"
#include "game/Enums/ePedState.h"

class CPedSamp
{
public:
    bool 		m_bIsSpawned{false};
    uint8_t		m_bytePlayerNumber;
    uintptr 	m_dwArrow;
    bool        lToggle;
    CPed*	m_pPed;

    uint8_t m_iCurrentSpecialAction = 0;
    int drunk_level = 0;
    RwMatrix m_HeadBoneMatrix;
    std::unordered_map<int, CAttachedPlayerObject> m_aAttachedObject;

    bool            m_bHaveBulletData;
    BULLET_DATA 	m_bulletData;
    bool            IsActionCarry = false;
    bool            m_iPissingState = false;
    uintptr         m_dwPissParticlesHandle = 0;
    uintptr		    m_dwGTAId;

public:
	CPedSamp();	// local
	CPedSamp(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation); // remote
	~CPedSamp();

	CAMERA_AIM * GetCurrentAim();
	void SetCurrentAim(CAMERA_AIM *pAim);

	uint16_t GetCameraMode();

	void SetCameraMode(uint16_t byteCamMode);

	float GetCameraExtendedZoom();
	uint8_t GetExtendedKeys();

	void ApplyCrouch();
	void ResetCrouch();
	bool IsCrouching();

	void SetDead();
	// 0.3.7
	void ApplyCommandTask(char* a2, int a4, int a5, int a6, CVector* a7, char a8, float a9, int a10, int a11, char a12);
	CVehicle* GetGtaVehicle();
	// 0.3.7
	void SetInitialState() const;
	// 0.3.7
	void SetHealth(float fHealth) const;
	void SetArmour(float fArmour);
	// 0.3.7
	float GetHealth();
	float GetArmour();
	// 0.3.7
	void TogglePlayerControllable(bool bToggle, bool isTemp = false);
	// 0.3.7
	void SetModelIndex(uint32 uiModel);

	bool IsAnimationPlaying(char* szAnimName);
	void ClearAllTasks();

	void EnterVehicle(int iVehicleID, bool bPassenger);
	// 0.3.7
	void ExitCurrentVehicle();

	uint8_t GetCurrentWeapon();
	void SetArmedWeapon(int iWeaponID) const;
	void ClearPlayerAimState();
	void SetAimZ(float fAimZ);
	float GetAimZ();
	CWeapon * GetCurrentWeaponSlot();
	//CAMERA_AIM* GetCurrentAim();

	void ClearAllWeapons();
	void DestroyFollowPedTask();
	void ResetDamageEntity();

	// 0.3.7
	void RestartIfWastedAt(const CVector vecRestart, float fRotation);
	// 0.3.7
	void ForceTargetRotation(float fRotation);
	// 0.3.7
	uint8_t GetActionTrigger();
	// 0.3.7
	bool IsDead() const;
	uint16_t GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	void SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	// 0.3.7
	void ShowMarker(uint32_t iMarkerColorID);
	// 0.3.7
	void HideMarker();
	// 0.3.7
	void SetFightingStyle(int iStyle);
	// 0.3.7
	void SetRotation(float fRotation);
	// 0.3.7
	void ApplyAnimation( char *szAnimName, char *szAnimFile, float fT, bool opt1, bool opt2, bool opt3, bool opt4, int iUnk );

	void SetActionTrigger(ePedState action);
	CPed * GetGtaActor() { return m_pPed; };

	void AttachObject(ATTACHED_OBJECT_INFO* pInfo, int iSlot);
	void SetAttachOffset(int iSlot, CVector pos, CVector rot);
	void DeattachObject(int iSlot);
	bool IsHasAttach();
	void FlushAttach();
	void ProcessAttach();

	void ProcessHeadMatrix();

	bool IsPlayingAnim(int idx);
	int GetCurrentAnimationIndex(float& blendData);
	void PlayAnimByIdx(int idx, float BlendData, bool loop = false, bool freeze = false, uint8_t time = 0);

	void SetMoveAnim(int iAnimGroup);
    CAttachedPlayerObject* GetAttachedObject(uint32 id)
    {
        auto it = m_aAttachedObject.find(id);
        return it != m_aAttachedObject.end() ? &it->second : nullptr;
    }



    bool IsValidAttach(int iSlot);
	void SetCameraExtendedZoom(float fZoom, float fAspectRatio);
    void FireInstant();
    void ProcessBulletData(BULLET_DATA *btData);
	void GetWeaponInfoForFire(int bLeft, CVector *vecBone, CVector *vecOut);
    void ProcessSpecialAction(uint8_t byteSpecialAction);
    void ClearAnimations();
    void SetWeaponAmmo(int iWeaponID, int iAmmo);

	CVector *GetCurrentWeaponFireOffset();
	CVehicleMP *GetCurrentVehicle();
	CVehicle *GetCurrentGtaVehicle();
	uint32_t GetCurrentGTAVehicleID();
	VEHICLEID GetCurrentSampVehicleID();
};