//
// Created on 25.09.2023.
//

#pragma once

#include "game/Entity/Object/Object.h"

class CSkyBox {
public:
    static void Init();
    static void Process();
    static void SetRotSpeed(float mFRotSpeed);
    static void SetTexture(const char *texName);
    static bool IsNeedRender();
    static CObjectSamp *GetSkyObject();

private:
    static CObjectSamp* CreateObjectScaled(int iModel, float fScale);
    static void ReTexture();
    static RwObject* RwFrameForAllObjectsCallback(RwObject* object, void* data);

private:
    static constexpr auto SKYBOX_OBJECT_ID = 18659;

    static inline CObjectSamp*      m_pSkyObject{};
    static inline bool          m_bNeedRender = true;
    static inline float         m_fRotSpeed = 0.004f;
    static inline RwTexture*    m_pTex{};
    static inline int           m_dwChangeTime = -1;
};
