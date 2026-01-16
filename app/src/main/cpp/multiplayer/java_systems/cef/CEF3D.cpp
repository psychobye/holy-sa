#include "CEF3D.h"
#include "util/TextureUploadManager.h"
#include "util/util.h"
#include "game/game.h"
#include "Models/ModelInfo.h"
#include "Camera.h"
#include "util/CJavaWrapper.h"

// TODO:
// - DestroyObject
// - setTexture("url.mp4", "screen")
// - Update (stream zone)
// - States: NOT_INITED, INITED, etc.

CObjectSamp* CCEF3D::CreateObjectScaled(CVector pos, int iModel, float fScale)
{
    if (!CModelInfo::GetModelInfo(iModel)) return nullptr;
    CModelInfo::GetModelInfo(iModel)->m_nRefCount = 899;

    CVector vecRot(0.f, 0.f, 0.f);
    CVector vecScale(fScale);

    auto *object = new CObjectSamp(iModel, pos.x, pos.y, pos.z, vecRot, 0.0f);

    CEntity* entity = object->m_pEntity;
    if (!entity) return object;

    entity->Remove();

    RwMatrix matrix;
    entity->GetMatrix(&matrix);
    RwMatrixScale(&matrix, &vecScale);

    entity->SetMatrix(reinterpret_cast<CMatrix&>(matrix));
    entity->UpdateRW();
    entity->UpdateRwFrame();

    entity->Add();
    entity->m_bUsesCollision = true;

    return object;
}

RwObject* CCEF3D::RwFrameForAllObjectsCallback(RwObject* object, void* data)
{
    auto* pData = reinterpret_cast<CEFObjectData*>(data);
    if (!object || object->type != rpATOMIC || !pData || !pData->pTexture)
        return object;

    auto* atomic = reinterpret_cast<RpAtomic*>(object);
    RpGeometry* geometry = atomic->geometry;

    for (int i = 0; i < geometry->matList.numMaterials; i++) {
        RpMaterial* material = geometry->matList.materials[i];
        if (!material) continue;

        bool isTarget = false;

        if (pData->targetMaterialIndex != -1) {
            if (i == pData->targetMaterialIndex) isTarget = true;
        }
        else if (!pData->targetTexName.empty() && material->texture) {
            if (strstr(material->texture->name, pData->targetTexName.c_str())) {
                isTarget = true;
            }
        }
        else if (i == 1) {
            isTarget = true;
        }

        if (isTarget) {
            material->texture = pData->pTexture;
            material->color = { 255, 255, 255, 255 };
        }
    }
    return object;
}

void CCEF3D::SetTexture(int id, const std::string& url, const std::string& texName) {
    if (m_objects.count(id)) {
        m_objects[id].targetTexName = texName;

        if (!texName.empty() && std::all_of(texName.begin(), texName.end(), ::isdigit)) {
            m_objects[id].targetMaterialIndex = std::stoi(texName);
        } else {
            m_objects[id].targetMaterialIndex = -1;
        }
    }

    auto env = CJavaWrapper::GetEnv();
    if (!env) return;

    if (!CCEF::thiz) CCEF::Constructor();

    jstring jUrl = env->NewStringUTF(url.c_str());
    jstring jTexName = env->NewStringUTF(texName.c_str());

    jmethodID method = env->GetMethodID(
            CCEF::clazz,
            "fetchTexture",
            "(ILjava/lang/String;Ljava/lang/String;)V"
    );

    if (method) {
        env->CallVoidMethod(
                CCEF::thiz,
                method,
                (jint)id,
                jUrl,
                jTexName
        );
    }

    env->DeleteLocalRef(jUrl);
    env->DeleteLocalRef(jTexName);
}

// !TODO: vec rotate
void CCEF3D::CreateObject(int id, int iModel, CVector pos, float fScale) {
    if (m_objects.count(id)) return;

    CObjectSamp* pObj = CreateObjectScaled(pos, iModel, fScale);
    if (pObj) {
        m_objects[id] = { pObj, nullptr, "" };
    }
}

void CCEF3D::pktCreateObject(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint32_t id;
    bs.Read(id);

    uint32_t iModel;
    bs.Read(iModel);

    CVector pos;
    bs.Read(pos.x);
    bs.Read(pos.y);
    bs.Read(pos.z);

    float fScale;
    bs.Read(fScale);

    CCEF3D::CreateObject(id, iModel, pos, fScale);
}

void CCEF3D::pktSetTexture(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint32_t id;
    bs.Read(id);

    std::string url;
    bs.ReadStr8(url);

    std::string texName;
    bs.ReadStr8(texName);

    CCEF3D::SetTexture(id, cp1251_to_utf8(url), cp1251_to_utf8(texName));
}

/**
 * apply pending texture created from JNI buffer.
 */
void CCEF3D::ApplyPendingTexture()
{
    std::lock_guard<std::mutex> lock(m_pendingMutex);
    if (!m_pendingReady || !m_pendingBuffer) return;

    RwTexture* newTex = TextureUploadManager::CreateRwTextureFromBytes(m_pendingBuffer, m_pendingWidth, m_pendingHeight, false);

    delete[] m_pendingBuffer;
    m_pendingBuffer = nullptr;

    int id = m_pendingId;
    if (m_objects.count(id)) {
        m_objects[id].pTexture = newTex;

        auto pObject = m_objects[id].pObject;
        if (pObject && pObject->m_pEntity && pObject->m_pEntity->m_pRwObject) {
            RwFrameForAllObjects((RwFrame*)pObject->m_pEntity->m_pRwObject->parent,
                                 RwFrameForAllObjectsCallback, &m_objects[id]);
        }
    }

    m_pendingReady = false;
}

void CCEF3D::EnqueuePendingTexture(int id, uint8_t* buffer, size_t pxCount, int width, int height, const std::string& texName) {
    if (!buffer || pxCount == 0 || width <= 0 || height <= 0) return;

    std::lock_guard<std::mutex> lock(m_pendingMutex);

    if (m_pendingBuffer) {
        delete[] m_pendingBuffer;
        m_pendingBuffer = nullptr;
    }

    size_t bytes = pxCount * 4;
    m_pendingBuffer = new uint8_t[bytes];
    memcpy(m_pendingBuffer, buffer, bytes);

    m_pendingId = id;
    m_pendingPxCount = pxCount;
    m_pendingWidth = width;
    m_pendingHeight = height;
    m_pendingTexName = texName;
    m_pendingReady = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_holy_game_gui_cef_CefManager_nativeUploadBytes(
        JNIEnv* env, jobject /*thiz*/,
        jint id, jstring jTexName, jobject buffer,
        jint width, jint height
) {
    if (!buffer || !jTexName || width <= 0 || height <= 0)
        return;

    jobject gBuffer = env->NewGlobalRef(buffer);
    if (!gBuffer) return;

    const char* texNameCStr = env->GetStringUTFChars(jTexName, nullptr);
    if (!texNameCStr) {
        env->DeleteGlobalRef(gBuffer);
        return;
    }
    std::string texNameStr(texNameCStr);
    env->ReleaseStringUTFChars(jTexName, texNameCStr);

    void* ptr = env->GetDirectBufferAddress(gBuffer);
    if (!ptr) {
        env->DeleteGlobalRef(gBuffer);
        return;
    }

    size_t pxCount = static_cast<size_t>(width) * static_cast<size_t>(height);

    CGame::PostToMainThread([gBuffer, ptr, id, pxCount, width, height, texNameStr]() {
        CCEF3D::EnqueuePendingTexture(
                id,
                static_cast<uint8_t*>(ptr),
                pxCount,
                width,
                height,
                texNameStr
        );

        JNIEnv* env = CJavaWrapper::GetEnv();
        if (env && gBuffer) {
            env->DeleteGlobalRef(gBuffer);
        }
    });
}