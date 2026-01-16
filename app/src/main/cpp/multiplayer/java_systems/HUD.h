//
// Created on 28.10.2022.
//
#pragma once
#include "../util/CJavaWrapper.h"
#include "game/common.h"
#include "game/Entity/Ped/Ped.h"


class CHUD {
public:

    static void toggleAll(bool toggle, bool chat = true, bool onlyVisual = true);
    static void updateAmmo();
    static void toggleLockButton(bool toggle);

    static bool bIsShow;
    static int  iLocalMoney;
    static bool bIsShowEnterExitButt;
    static bool bIsShowLockButt;
    static bool bIsShowChat;
    static bool bIsTouchCameraButt;
    static bool bIsCamEditGui;
    static bool bIsOnlyVisualOff;
    static jobject thiz;

    static void UpdateWanted();

    static void UpdateMoney();
    static bool bIsShowMafiaWar;

    static CVector2D radarBgPos1;
    static CVector2D radarBgPos2;

    static inline CVector2D radarPos;
    static inline CVector2D radarSize;

    static void AddChatMessage(const char msg[]);

    static void SetChatInput(const char ch[]);

    static void ToggleChatInput(bool toggle);

    static void ToggleProgressTexts(bool toggle);

    void ToggleChat(bool toggle);

    static void ChangeChatTextSize(int size);

    static void UpdateSalary(int salary, int lvl, float exp);

    static int iWantedLevel;

    static void addGiveDamageNotify(PLAYERID Id, int weaponId, float damage, ePedPieceTypes i);
    static PLAYERID lastGiveDamagePlayerId;
    static float fLastGiveDamage;

    static void addTakeDamageNotify(char *nick, int weaponId, float damage);

    static void toggleServerLogo(bool toggle);

    static void toggleGreenZone(bool toggle);

    static void toggleGps(bool toggle);

    static void updateLevelInfo(int level, int currentexp, int maxexp);

    static void showUpdateTargetNotify(int type, char *text);

    static void hideTargetNotify();
    static int iSatiety;

    static void updateBars();

    static bool NeededRenderPassengerButton();

    static void RenderBackgroundRadar();
};

