#pragma once
#include <jni.h>
#include <string>
#include <mutex>
#include <cstdint>
#include "CEF.h"
#include "common.h"
#include "rwcore.h"
#include "rpworld.h"
#include "game/Entity/Object/Object.h"
#include "net/netgame.h"

class CCEF3D {
public:
    struct CEFObjectData {
        CObjectSamp* pObject;
        RwTexture* pTexture;
        std::string targetTexName;
        int targetMaterialIndex = -1;
    };

    static void ApplyPendingTexture();
    static CObjectSamp* CreateObjectScaled(CVector pos, int iModel, float fScale);
    static RwObject* RwFrameForAllObjectsCallback(RwObject* object, void* data);

    static void CreateObject(int id, int iModel, CVector pos, float fScale);
    static void SetTexture(int id, const std::string& url, const std::string& texName);

    static void EnqueuePendingTexture(int id, uint8_t* buffer, size_t pxCount, int width, int height, const std::string& texName);

    static void pktCreateObject(Packet *p);
    static void pktSetTexture(Packet *p);

private:
    static inline std::unordered_map<int, CEFObjectData> m_objects{};

    static inline int              m_pendingId{-1};
    static inline std::mutex       m_pendingMutex{};
    static inline uint8_t*         m_pendingBuffer{nullptr};
    static inline size_t           m_pendingPxCount{0};
    static inline int              m_pendingWidth{0};
    static inline int              m_pendingHeight{0};
    static inline std::string      m_pendingTexName{};
    static inline bool             m_pendingReady{false};
};