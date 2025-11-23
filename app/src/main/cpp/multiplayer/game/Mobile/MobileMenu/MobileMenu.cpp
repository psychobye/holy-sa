//
// Created by traw-GG on 05.08.2025.
//

#include <jni.h>
#include "MobileMenu.h"
#include "util/patch.h"
#include "game/game.h"
#include "game/Widgets/TouchInterface.h"
#include "net/netgame.h"
#include "game/Radar.h"
#include "game/Entity/Ped/Ped.h"
#include "Mobile/MobileSettings/MobileSettings.h"

CMobileMenu* gMobileMenu;

void (*InitForPause)(uintptr* thiz);
void InitForPause_hook(uintptr* thiz)
{
    InitForPause(thiz);
}

void CMobileMenu::InjectHooks() {
    gMobileMenu = (CMobileMenu*)(g_libGTASA + (VER_x32 ? 0x006E0074 : 0x8BE780));

    CHook::InlineHook("_ZN10MobileMenu12InitForPauseEv", &InitForPause_hook, &InitForPause);
}