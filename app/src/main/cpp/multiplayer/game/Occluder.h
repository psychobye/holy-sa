//
// Created by Traw-GG on 10.08.2024.
//
#pragma once

#include "Vector2D.h"
#include "Vector.h"
#include "FixedVector.h"

class COccluder {
public:
    FixedVector<int16_t, 4.f> m_Center{};

    // As usual, some R* dev from 2003 decided to swap length and width
    // that is, in the structure, it's `m_Length, m_Width, m_Height`
    // so converting it all into one FixedVector would be fucking confusing
    // (as `size.x` would not be `m_Width` but rather `m_Length`
    // TODO(In standalone): Replace this with a vector and fix the abovementioend issue.
    FixedFloat<int16_t, 4.f> m_Length;
    FixedFloat<int16_t, 4.f> m_Width;
    FixedFloat<int16_t, 4.f> m_Height;

    FixedVector<uint8, 256.f / TWO_PI> m_Rot{};

    struct {
        int16_t m_NextIndex : 15;
        int16_t m_DontStream : 1;
    };
};
VALIDATE_SIZE(COccluder, 0x12);