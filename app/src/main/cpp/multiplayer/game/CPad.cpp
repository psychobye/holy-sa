#include "CPad.h"
#include "util/patch.h"
#include "net/localplayer.h"
#include "World.h"
#include "Widgets/TouchInterface.h"
#include "Entity/Vehicle/Vehicle.h"

CPad CPad::Pads[2] = {};

void CPad::Initialise() {
    CHook::CallFunction<void>("_ZN4CPad10InitialiseEv");
}

int16_t CPad::GetPedWalkLeftRight(CPad *thiz) {
    return CHook::CallFunction<int16_t>("_ZN4CPad19GetPedWalkLeftRightEv", thiz);
}

int16_t CPad::GetPedWalkUpDown(CPad *thiz) {
    return CHook::CallFunction<int16_t>("_ZN4CPad16GetPedWalkUpDownEv", thiz);
}

void CPad::Clear(CPad *thiz, bool bOkToClearTheDisableFlag, bool bReinit) {
    return CHook::CallFunction<void>("_ZN4CPad5ClearEbb", thiz, bOkToClearTheDisableFlag, bReinit);
}

bool CPad::HornJustDown(CPad *thiz) {
    return CHook::CallFunction<bool>("_ZN4CPad12HornJustDownEv", thiz);
}

bool CPad::GetHorn(CPad *thiz, bool bEnableTouch) {
    return CHook::CallFunction<bool>("_ZN4CPad7GetHornEb", thiz, bEnableTouch);
}

void CPad::InjectHooks() { }