#include "CPad.h"
#include "util/patch.h"

CPad CPad::Pads[2] = {};

void CPad::Initialise() {
    CHook::CallFunction<void>("_ZN4CPad10InitialiseEv");
}

void CPad::InjectHooks() {
}