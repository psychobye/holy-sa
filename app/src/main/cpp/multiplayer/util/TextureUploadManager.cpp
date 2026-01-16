#include "TextureUploadManager.h"
#include "main.h"
#include "Models/ModelInfo.h"

RwTexture* TextureUploadManager::CreateRwTextureFromBytes(uint8_t* src, int width, int height, bool srcIsRGBA) {
    if (!src || width <= 0 || height <= 0) return nullptr;

    int rwWidth  = 1; while(rwWidth < width) rwWidth <<= 1;
    int rwHeight = 1; while(rwHeight < height) rwHeight <<= 1;

    RwRaster* raster = RwRasterCreate(rwWidth, rwHeight, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
    if (!raster) return nullptr;

    void* dst = RwRasterLock(raster, 0, rwRASTERLOCKWRITE);
    if (!dst) { RwRasterDestroy(raster); return nullptr; }

    memset(dst, 0, rwWidth * rwHeight * 4);

    for (int y = 0; y < height; ++y) {
        uint8_t* srcLine = src + y * width * 4;
        uint8_t* dstLine = reinterpret_cast<uint8_t*>(dst) + y * rwWidth * 4;

        for (int x = 0; x < width; ++x) {
            uint8_t s0 = srcLine[x*4 + 0];
            uint8_t s1 = srcLine[x*4 + 1];
            uint8_t s2 = srcLine[x*4 + 2];
            uint8_t s3 = srcLine[x*4 + 3];

            uint8_t r,g,b,a;
            if (srcIsRGBA) {
                b = s0; g = s1; r = s2; a = s3;
            } else {
                r = s0; g = s1; b = s2; a = s3;
            }

            dstLine[x*4 + 0] = b;
            dstLine[x*4 + 1] = g;
            dstLine[x*4 + 2] = r;
            dstLine[x*4 + 3] = a;
        }
    }

    RwRasterUnlock(raster);
    RwTexture* tex = RwTextureCreate(raster);
    if (!tex) { RwRasterDestroy(raster); return nullptr; }
    return tex;
}