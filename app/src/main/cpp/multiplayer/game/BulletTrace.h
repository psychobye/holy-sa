//
// Created by psychobye on 23.11.2025.
//
#pragma once

#include "common.h"
#include "Timer.h"
#include "Vector.h"

class CBulletTrace {
public:
    CVector     m_vecStart;
    CVector     m_vecEnd;
    bool        m_bExists;
    uint32_t    m_nCreationTime;
    uint32_t    m_nLifeTime;
    float       m_fRadius;
    uint32_t    m_nTransparency;

public:
    void Update() {
        if (CTimer::GetTimeInMS() - m_nCreationTime >= m_nLifeTime) {
            m_bExists = false;
        }
    }

    CVector GetDirection() const noexcept { return m_vecEnd - m_vecStart; }
    uint32_t GetRemainingLifetime() const noexcept { return CTimer::GetTimeInMS() - m_nCreationTime; }
};

VALIDATE_SIZE(CBulletTrace, 0x2C);