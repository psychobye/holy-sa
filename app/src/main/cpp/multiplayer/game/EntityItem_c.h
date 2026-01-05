#pragma once
#include "List_c.h"

struct EntityItem_c : public ListItem_c {
    void* pData;                // 0x10
    void* pLocTri;              // 0x18
    uint8_t m_allocatedMatrix;  // 0x20
    uint8_t _pad[7];            // 0x21-0x27
};
// VALIDATE_SIZE(EntityItem_c, 0x28); // TODO: x32