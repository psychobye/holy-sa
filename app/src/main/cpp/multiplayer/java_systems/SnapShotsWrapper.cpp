//
// Created on 01.10.2023.
//

#include "SnapShotsWrapper.h"
#include "game/CCustomPlateManager.h"
#include "main.h"
#include "util/CJavaWrapper.h"
#include "util/patch.h"
#include "game/SnapShots.h"
#include "net/VehiclePool.h"
#include "util/TextureUploadManager.h"
#include <jni.h>
#include <GLES3/gl32.h>

void SnapShotsWrapper::Process() {
    if(queue.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(queueMutex);

    bool needCrop = false;
    auto item = queue.front();

    RwTexture* tex = nullptr;

    switch (item.type) {
        case Types::PED:
            tex = CSnapShots::CreatePedSnapShot(item.modelId, 0, &item.rot, &item.offset);
            needCrop = true;
            break;
        case Types::VEHICLE:
            tex = CSnapShots::CreateVehicleSnapShot(item.modelId, 0, &item.rot, &item.offset);
            needCrop = true;
            break;
        case Types::OBJECT:
            tex = CSnapShots::CreateObjectSnapShot(item.modelId, &item.rot, &item.offset);
            break;

        case Types::PLATE:
            needCrop = true;
            tex = CCustomPlateManager::createTexture(item.plateType, item.number, item.region);
            break;

        default:
            assert("SnapShotsWrapper type");
            break;
    }


    auto env = CJavaWrapper::GetEnv();

    auto bytes = ConvertTexToBitMapBytes(tex, needCrop);

    SetToImageView(
            bytes,
            (int)tex->raster->width,
            (int)tex->raster->height,
            item.imageview
            );

    env->DeleteLocalRef(bytes);
    env->DeleteGlobalRef(item.imageview);
    queue.pop();

    tex->refCount = 0;
    RwTextureDestroy(tex);
}

void cropImage(uint8_t* image, int& width, int& height) {
    int left = 0, right = width - 1, top = 0, bottom = height - 1;

    // ������� ����� �������
    while (left < width) {
        bool columnTransparent = true;
        for (int y = 0; y < height; ++y) {
            int pixelIndex = (y * width + left) * 4; // RGBA

            if (image[pixelIndex + 3] != 0) {
                columnTransparent = false;
                break;
            }
        }
        if (!columnTransparent) {
            break;
        }
        ++left;
    }

    // ������� ������ �������
    while (right >= 0) {
        bool columnTransparent = true;
        for (int y = 0; y < height; ++y) {
            int pixelIndex = (y * width + right) * 4; // RGBA

            if (image[pixelIndex + 3] != 0) {
                columnTransparent = false;
                break;
            }
        }
        if (!columnTransparent) {
            break;
        }
        --right;
    }

    // ������� ������� �������
    while (top < height) {
        bool rowTransparent = true;
        for (int x = left; x <= right; ++x) {
            int pixelIndex = (top * width + x) * 4; // RGBA

            if (image[pixelIndex + 3] != 0) {
                rowTransparent = false;
                break;
            }
        }
        if (!rowTransparent) {
            break;
        }
        ++top;
    }

    // ������� ������ �������
    while (bottom >= 0) {
        bool rowTransparent = true;
        for (int x = left; x <= right; ++x) {
            int pixelIndex = (bottom * width + x) * 4; // RGBA

            if (image[pixelIndex + 3] != 0) {
                rowTransparent = false;
                break;
            }
        }
        if (!rowTransparent) {
            break;
        }
        --bottom;
    }

    // ��������� ������� �����������
    int newWidth = right - left + 1;
    int newHeight = bottom - top + 1;

    // �������� �� ��������� ���������� �����������
    if (newWidth <= 0 || newHeight <= 0) {
        width = 0;
        height = 0;
        return;
    }

    // ��������� �����������, ������� ���
    for (int y = 0; y < newHeight; ++y) {
        int srcRowIndex = (top + y) * width + left;
        int dstRowIndex = y * newWidth;
        std::copy(image + srcRowIndex * 4, image + (srcRowIndex + newWidth) * 4, image + dstRowIndex * 4);
    }

    // ��������� ������� �����������
    width = newWidth;
    height = newHeight;
}


jbyteArray SnapShotsWrapper::ConvertTexToBitMapBytes(RwTexture* tex, bool needCrop) {
    auto env = CJavaWrapper::GetEnv();
    auto &raster = tex->raster;

    auto size = raster->width * raster->height * 4;
    auto buffer = new uint8[size];

    RwInt32 RasterExtOffset = *(RwInt32*)(g_libGTASA + (VER_x32 ? 0x006B31FC : 0x890108));
    uintptr_t renderTarget = *(uintptr_t*)((char*)(tex->raster) + RasterExtOffset + (VER_x32 ? 0x18 : 0x20));

    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x001D0958 + 1 : 0x266024), renderTarget, true);
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x001B4C20 + 1 : 0x247E98), 0, 0, raster->width, raster->height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    if(needCrop) {
        cropImage(buffer, raster->width, raster->height);
        size = raster->width * raster->height * 4;
    }

    jbyteArray imageBytes = env->NewByteArray(size);
    env->SetByteArrayRegion(imageBytes, 0, size, reinterpret_cast<const jbyte *>(buffer));

    delete[] buffer;
    return imageBytes;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_EntitySnaps_nativeEntitySnapAddToQueue(JNIEnv *env, jobject thiz, jobject imageview, jint type, jint model_id, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat x_offset, jfloat y_offset, jfloat z_offset) {
    std::lock_guard<std::mutex> lock(SnapShotsWrapper::queueMutex);

    SnapShotsWrapper::queue.push({
        env->NewGlobalRef(imageview),
        model_id,
        (SnapShotsWrapper::Types)type,
        CVector(rot_x, rot_y, rot_z),
        CVector(x_offset, y_offset, z_offset)
    });

    env->DeleteLocalRef(imageview);
}

void SnapShotsWrapper::SetToImageView(jbyteArray bytes, int width, int height, jobject imageview) {
    auto env = CJavaWrapper::GetEnv();

    auto method = env->GetStaticMethodID(clazz, "setToImageView", "(Landroid/widget/ImageView;[BII)V");
    env->CallStaticVoidMethod(clazz, method, imageview, bytes, width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_EntitySnaps_nativePlateSnapAddToQueue(JNIEnv *env, jobject thiz, jobject image_view, jint type, jstring jnumber, jstring jregion) {

    auto item = SnapShotsWrapper::QueueItem(env->NewGlobalRef(image_view), SnapShotsWrapper::Types::PLATE, static_cast<ePlateType>(type));

    const char* number = env->GetStringUTFChars(jnumber, nullptr);
    const char* region = env->GetStringUTFChars(jregion, nullptr);

    item.number.assign(number);
    item.region.assign(region);

    env->ReleaseStringUTFChars(jnumber, number);
    env->ReleaseStringUTFChars(jregion, region);

    std::lock_guard<std::mutex> lock(SnapShotsWrapper::queueMutex);

    SnapShotsWrapper::queue.push(item);

    env->DeleteLocalRef(image_view);
}