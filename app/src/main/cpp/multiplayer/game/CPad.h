#include <cstdint>

struct CControllerState {
    uint8_t data[0x60];
};

class CPad {
public:
    static CPad Pads[2];

    static void InjectHooks();
    static void Initialise();

    static CPad* GetPad(int index) { return &Pads[index]; }

    static int16_t GetPedWalkLeftRight(CPad *thiz);
    static int16_t GetPedWalkUpDown(CPad *thiz);

    static void Clear(CPad *thiz, bool bOkToClearTheDisableFlag, bool bReinit);

    static bool HornJustDown(CPad *thiz);
    static bool GetHorn(CPad *thiz, bool bEnableTouch);
    static bool GetSprint(CPad *thiz, int nSprintType);

    CControllerState NewState;               // 0x00
    CControllerState OldState;               // 0x30
    int32_t currentPadId;                    // 0x60
    uint16_t SteeringLeftRightBuffer[10];    // 0x64
    int32_t DrunkDrivingBufferUsed;          // 0x78
    CControllerState PCTempKeyState;         // 0x7C
    CControllerState PCTempJoyState;         // 0xAC
    CControllerState PCTempMouseState;       // 0xDC
    uint8_t Phase;                           // 0x10C
    uint8_t _pad1;                           // 0x10D
    uint16_t ShakeDur;                       // 0x10E
    uint16_t DisablePlayerControls;          // 0x110
    uint8_t ShakeFreq;                        // 0x112
    uint8_t JustOutOfFrontEnd;                // 0x113
    float fCruisingSpeed;                     // 0x114
    uint8_t bRhythm;                          // 0x118
    uint8_t bWheelie;                         // 0x119
    uint8_t bStoppie;                         // 0x11A
    uint8_t bApplyGas;                        // 0x11B
    uint8_t bApplyBrake;                      // 0x11C
    uint8_t bLaneCorrection;                  // 0x11D
    uint8_t bUsingDebugCamera;                // 0x11E
    uint8_t bUsingDebugPlayerFreeze;          // 0x11F
    uint8_t bHasCheated;                      // 0x120
    uint8_t bDisableForbiddenTerr;            // 0x121
    uint8_t bStopRhythmSprites;               // 0x122
    uint8_t bDoorsLocked;                     // 0x123
    uint8_t bRegainControl;                   // 0x124
    uint8_t _pad2[3];                         // 0x125-0x127
    float fBikeStickY;                        // 0x128
    uint8_t bApplyBrakes;                     // 0x12C
    uint8_t bDisablePlayerEnterCar;           // 0x12D
    uint8_t bDisablePlayerDuck;               // 0x12E
    uint8_t bDisablePlayerFireWeapon;         // 0x12F
    uint8_t bDisablePlayerFireWeaponWithL1;  // 0x130
    uint8_t bDisablePlayerCycleWeapon;        // 0x131
    uint8_t bDisablePlayerJump;               // 0x132
    uint8_t bDisablePlayerDisplayVitalStats;  // 0x133
    int32_t LastTimeTouched;                  // 0x134
    int32_t AverageWeapon;                    // 0x138
    int32_t AverageEntries;                   // 0x13C
    int32_t NoShakeBeforeThis;                // 0x140
    uint8_t NoShakeFreq;                      // 0x144
    uint8_t bHasJetPack;                      // 0x145
    uint8_t bRocketLocked;                    // 0x146
    uint8_t bTrainPassenger;                  // 0x147
    uint8_t bSavedForTrain;                   // 0x148
    uint8_t bSetSteeringMode;                 // 0x149
    uint8_t bSetTouchLayout;                  // 0x14A
    uint8_t _pad3[1];                         // 0x14B
    float m_fAccelX;                          // 0x14C
    float m_fAccelY;                          // 0x150
    float m_fAccelZ;                          // 0x154
};