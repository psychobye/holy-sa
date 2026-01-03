#pragma once
#include <jni.h>

class CRadialMenu {
public:
    static inline jclass clazz = nullptr;
    static inline bool bIsShow = false;

public:
    static void Show();
    static void Update();
};