#include "CPad.h"
#include "util/patch.h"

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

void CPad::InjectHooks() { }