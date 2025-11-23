#pragma once

#include "rgba.h"
#include "CCustomPlateManager.h"
#include "game/Entity/Vehicle/Vehicle.h"
#include "game/Enums/eCarNodes.h"
#include "DamageManager.h"
#include "Fx/VehicleNeon.h"

enum E_CUSTOM_COMPONENTS
{
	ccBumperF = 0,
	ccBumperR,
	ccFenderF,
	ccFenderR,
	ccSpoiler,
	ccExhaust,
	ccRoof,
	ccTaillights,
	ccHeadlights,
	ccDiffuser,
	ccSplitter,
	ccExtra,
	ccMax
};

enum E_HANDLING_PARAMS
{
	hpMaxSpeed,
	hpAcceleration,
	hpGear,
	hpEngineInertion,
	hpMass,
	hpMassTurn,

	hpBrakeDeceleration,
	hpTractionMultiplier,
	hpTractionLoss,
	hpTractionBias,
	hpSuspensionLowerLimit,
	hpSuspensionBias,

	hpWheelSize,

	hpMax,
	hpCount
};

#define EXTRA_COMPONENT_BOOT			10
#define EXTRA_COMPONENT_BONNET			11
#define EXTRA_COMPONENT_BUMP_REAR		12
#define EXTRA_COMPONENT_DEFAULT_DOOR 	13
#define EXTRA_COMPONENT_WHEEL			14
#define EXTRA_COMPONENT_BUMP_FRONT		15

struct SHandlingData
{
	uint8_t flag;
	float fValue;
	SHandlingData(uint8_t uFlag, float fvalue) : flag(uFlag), fValue(fvalue) {}
};

enum class eLightsState {
    OFF,
    ON_NEAR,
    HIGH,
};

enum class eStobsStatus {
	OFF = 0,
	ON_TYPE_1, 	// ����� ����� �����
	ON_TYPE_2,	// ����� 3 ����, ������ 3 ���� � ����������� ������
	ON_TYPE_3, 	// ��� ������� 4 ����
	ON_TYPE_4,  // ���� 2 ������ 2
};

enum eTurnState
{
	TURN_OFF,
	TURN_LEFT,
	TURN_RIGHT,
	TURN_ALL
};

void* GetSuspensionLinesFromModel(int nModelIndex, int& numWheels);
CCollisionData* GetCollisionDataFromModel(int nModelIndex);

class CVehicleSamp
{
public:
	CVehicleSamp(int iType, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f, bool bSiren = false);
	~CVehicleSamp();

	void SetHealth(float fHealth);
	float GetHealth();

	// 0.3.7
	bool IsOccupied();

	// 0.3.7
	void SetInvulnerable(bool bInv);
	// 0.3.7
	bool IsDriverLocalPlayer();

	void ProcessMarkers();

	void RemoveEveryoneFromVehicle();

	void SetDoorState(int iState) const;

	void UpdateDamageStatus(uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage, uint8_t byteTireDamage);

	void AttachTrailer();
	void DetachTrailer();
	void SetTrailer(CVehicleSamp* pTrailer);

	unsigned int GetVehicleSubtype() const;

	void SetEngineState(bool bEnable);
	void RemoveComponent(uint16_t uiComponent) const;

	void SetComponentVisible(uint8_t group, uint16_t components);
    void SetBikeHandlingData();
    void SetHeliHandlingData();
    void SetVehicleHandlingData();
	void SetHandlingData();

	void ProcessHeadlightsColor(uint8_t& r, uint8_t& g, uint8_t& b);
	void SetWheelAngle(bool isFront, float angle);
	void SetWheelOffset(bool isFront, float offset);
	void SetWheelWidth(float fValue);
	void ProcessWheelsOffset();

private:
	void ProcessWheelOffset(RwFrame* pFrame, bool bLeft, float fValue, int iID);
	void SetComponentVisibleInternal(const char* szComponent, bool bVisible) const;
	static std::string GetComponentNameByIDs(uint8_t group, int subgroup);

	void CopyGlobalSuspensionLinesToPrivate();

public:
	CRGBA 	        tonerColor		{255, 255, 255, 110};
	CRGBA 	        mainColor		{0, 0, 0, 255};
    CRGBA 	        wheelColor		{0, 0, 0, 255};
	CRGBA 	        secondColor		{0, 0, 0, 255};

	CRGBA 	        lightColor		{255, 255, 255, 0};
	CRGBA 	        lightColorStrob	{255, 255, 255, 0};
	eStobsStatus 	m_iStrobsType = eStobsStatus::OFF;
	uint32_t        dwStrobStep{};
    uint32_t        lastStrobTime{};

	int 		m_nVinylIndex 	    = -1;
	RwTexture* 	m_pVinylTex 		= nullptr;

	int         m_dwChangeWheelTo = -1;
    int         m_dwCurrentWheelModel = -1;
	RwTexture*  pPlateTexture = nullptr;

	CVehicle* 	m_pVehicle;
    CDamageManager* m_pDamageManager;

	// �����������
	CObjectSamp*		m_pLeftFrontTurnLighter = nullptr;
	CObjectSamp*		m_pRightFrontTurnLighter = nullptr;
	CObjectSamp*		m_pLeftRearTurnLighter = nullptr;
	CObjectSamp*		m_pRightRearTurnLighter = nullptr;
	eTurnState 		m_iTurnState = TURN_OFF;
	bool 			m_bIsOnRightTurnLight = false;
	bool 			m_bIsOnLeftTurnLight = false;
	bool 			m_bIsOnAllTurnLight = false;
    bool            m_bDoorsState[eDoors::MAX_DOORS] {};
	float 			m_fDefaultWheelSize = 0.0f;

	CVehicleSamp* 		m_pTrailer = nullptr;
    uintptr		    m_dwMarkerID = 0;
	bool 			m_bIsInvulnerable = false;
	uint8_t			m_byteObjectiveVehicle = 0; // Is this a special objective vehicle? 0/1
	bool			m_bSpecialMarkerEnabled = false;
	bool 			m_bIsEngineOn = false;
    eLightsState 	m_bIsLightOn = eLightsState::OFF;

	tHandlingData* m_pCustomHandling = nullptr;
    tBikeHandlingData* m_pCustomBikeHandling = nullptr;

    std::list<SHandlingData> m_msLastSyncHandling {};
    CColLine* m_pSuspensionLines;
	bool bHasSuspensionLines;

	float m_fWheelAngleFront    = DEGTORAD(0.0f);
	float m_fWheelAngleBack     = DEGTORAD(0.0f);

	float m_fWheelSize      = 0.0f;
	float m_fWheelWidth     = 0.0f;
	float m_fWheelOffsetX   = 0.0f;
	float m_fWheelOffsetY   = 0.0f;

	bool m_bWasWheelOffsetProcessedX;
	bool m_bWasWheelOffsetProcessedY;
	uint32_t m_uiLastProcessedWheelOffset;

	RwMatrix m_vInitialWheelMatrix[4]{};

	// Damage status
	uint8_t			m_byteTyreStatus = 0;
	uint8_t			m_byteLightStatus = 0;
	uint32_t		m_dwDoorStatus = 0;
	uint32_t		m_dwPanelStatus = 0;
	uintptr 		m_dwGTAId;

    CVehicleNeon    neon;

public:

    bool HasDamageModel() const;

    void SetPanelStatus(ePanels bPanel, ePanelDamageState bPanelStatus) const;

    void ChangeVinylTo(int vinylIdx);
    void SetRGBATexture(CRGBA crgba, CRGBA crgba2);

    void SetDoorStatus(uint32_t dwDoorStatus, bool spawnFlyingComponen);

    void SetPanelStatus(ePanelDamageState ulPanelStatus) const;

    uint8_t GetLightStatus(eLights bLight) const;

    uint8_t GetBikeWheelStatus(uint8_t bWheel) const;

    void SetDoorStatus(eDoors bDoor, eDoorStatus bDoorStatus, bool spawnFlyingComponen);

    uint8_t GetDoorStatus(eDoors bDoor);

    void ProcessDamage();

    void GetDamageStatusEncoded(uint8_t *byteTyreFlags, uint8_t *byteLightFlags, uint32_t *dwDoorFlags, uint32_t *dwPanelFlags);

    uint8_t GetWheelStatus(eCarWheel bWheel) const;

	VEHICLEID getSampId();

	void toggleRightTurnLight(bool toggle);
	void toggleLeftTurnLight(bool toggle);

	void setPlate(ePlateType type, std::string& szNumber, std::string& szRegion);

public:
    void addComponent(uint16_t compId);

	void OpenDoor(eCarNodes index, eDoors doorId, bool state);

	void SetLightState(int iLight, eLightsDamageState iState) const;
	void ProcessStrobs();
    void SetStrob(eStobsStatus type);

    void ChangeDummyColor(const char *dummy, RwRGBA color);
};