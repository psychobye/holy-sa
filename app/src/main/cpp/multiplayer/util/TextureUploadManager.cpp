#include "TextureUploadManager.h"
#include "main.h"
#include "Models/ModelInfo.h"

RwTexture* TextureUploadManager::CreateRwTextureFromRGBA(const char* name, uint8_t* rgba, int width, int height) {
    if (!rgba || width <= 0 || height <= 0) return nullptr;

    int rwWidth  = 1; while(rwWidth < width) rwWidth <<= 1;
    int rwHeight = 1; while(rwHeight < height) rwHeight <<= 1;

    RwRaster* raster = RwRasterCreate(rwWidth, rwHeight, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
    void* dst = RwRasterLock(raster, 0, rwRASTERLOCKWRITE);

    for (int y = 0; y < height; ++y) {
        uint8_t* srcLine = rgba + y * width * 4;
        uint8_t* dstLine = (uint8_t*)dst + y * rwWidth * 4;

        for (int x = 0; x < width; ++x) {
            uint8_t r = srcLine[x*4 + 0];
            uint8_t g = srcLine[x*4 + 1];
            uint8_t b = srcLine[x*4 + 2];
            uint8_t a = srcLine[x*4 + 3];

            dstLine[x*4 + 0] = b;
            dstLine[x*4 + 1] = g;
            dstLine[x*4 + 2] = r;
            dstLine[x*4 + 3] = a;
        }
    }

    RwRasterUnlock(raster);

    RwTexture* tex = RwTextureCreate(raster);

    if (name) RwTextureSetName(tex, name);

    return tex;
}