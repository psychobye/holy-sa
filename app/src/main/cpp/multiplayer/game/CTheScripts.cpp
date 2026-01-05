#include "CTheScripts.h"
#include "util/patch.h"

void CTheScripts::StartTestScript() {
    return CHook::CallFunction<void>("_ZN11CTheScripts15StartTestScriptEv");
}

void CTheScripts::Process() {
    return CHook::CallFunction<void>("_ZN11CTheScripts7ProcessEv");
}