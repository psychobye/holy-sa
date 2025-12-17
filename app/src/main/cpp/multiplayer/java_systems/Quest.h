#pragma once

#include <jni.h>
#include "GuiWrapper.h"

class CQuest : public CGuiWrapper<CQuest>{
public:
    enum class ePacketType : uint8_t {
        EXIT,
        SHOW
    };

    static void Show(uint8_t questid, const std::string& name, const std::string& description, uint32_t reward);
};
