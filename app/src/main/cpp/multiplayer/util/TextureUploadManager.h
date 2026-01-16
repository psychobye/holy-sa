#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <cstdint>
#include "rwcore.h"

class TextureUploadManager {
public:
    static RwTexture* CreateRwTextureFromBytes(uint8_t* src, int width, int height, bool srcIsRGBA);
};