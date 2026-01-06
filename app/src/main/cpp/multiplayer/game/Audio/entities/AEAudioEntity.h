//
// Created by Traw-GG on 13.07.2025.
//
#pragma once

#include "game/Audio/AESound.h"
#include "game/Enums/eAudioEvents.h"

class CEntity;

class CAEAudioEntity {
public:
    CEntity* m_pEntity{};
    CAESound m_tempSound{};

private:
    static inline int8* m_pAudioEventVolumes;

public:
    static void InjectHooks();

    virtual void UpdateParameters(CAESound* sound, int16 curPlayPos) { /* Empty on purpose */ };

    static float GetDefaultVolume(eAudioEvents event) { return static_cast<float>(m_pAudioEventVolumes[event]); }
};

VALIDATE_SIZE(CAEAudioEntity, (VER_x32 ? 0x7C : 0x90));
