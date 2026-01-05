#include "ColAccel.h"
#include "util/patch.h"

// TODO: need to reverse end optimize
void CColAccel::startCache() {
    return CHook::CallFunction<void>("_ZN9CColAccel10startCacheEv");
}

void CColAccel::endCache() {
    return CHook::CallFunction<void>("_ZN9CColAccel8endCacheEv");
}