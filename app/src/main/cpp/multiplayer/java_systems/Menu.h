#pragma once

#include <jni.h>
#include "GuiWrapper.h"

class CMenu : public CGuiWrapper<CMenu>{
public:
    enum class ePacketType : uint8_t {
        EXIT,
        SHOW
    };

    static void Show(int donate, int money, float totalHours, int level, int exp, int expMax, const std::string& familyName, uint8_t r, uint8_t g, uint8_t b);
};
