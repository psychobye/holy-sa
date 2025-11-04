//
// Created by admin on 05.04.2023.
//

#pragma once

#include <jni.h>
#include "GuiWrapper.h"


class CStyling : public CGuiWrapper<CStyling>
{
public:
    enum class ePacketType : uint8_t {
        EXIT,
        UPDATE_VALUE,
        RESET_VALUE,
        RESET_ALL,
        BUY,
        TOGGLE_CAM
    };

    enum class eValueType : int {
        VALUE_TYPE_NEON_TYPE,
        VALUE_TYPE_NEON_COLOR,
        VALUE_TYPE_LIGHT_TYPE,
        VALUE_TYPE_LIGHT_COLOR,
        VALUE_TYPE_TONER_COLOR,
        VALUE_TYPE_BODY1_COLOR,
        VALUE_TYPE_BODY2_COLOR,
        VALUE_TYPE_WHEEL_COLOR,
        VALUE_TYPE_VINYL,
        VALUE_TYPE_STROB
    };

public:
    static void Show(int money, int total, const uint32_t arr[]);
    static void update(int money, int total);

    static uint32_t GetValueFromType(eValueType type);
};
