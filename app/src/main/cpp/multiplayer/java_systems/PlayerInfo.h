#pragma once

#include <string>

class CPlayerInfo {
public:
    static int    GetId();
    static std::string GetName();
    static int    GetModelId();
    static float  GetHealth();
    static float  GetArmour();
    static double GetX();
    static double GetY();
    static double GetZ();
    static int    GetLevel();
    static int    GetMoney();
};