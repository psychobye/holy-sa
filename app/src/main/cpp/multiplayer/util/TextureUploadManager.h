#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <cstdint>
#include "rwcore.h"

class TextureUploadManager {
public:
    static RwTexture* CreateRwTextureFromRGBA(const char* name, uint8_t* rgba, int width, int height);
};