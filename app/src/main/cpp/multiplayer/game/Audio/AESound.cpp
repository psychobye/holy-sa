//
// Created by Traw-GG on 13.07.2025.
//

#include "AESound.h"
#include "util/patch.h"

void CAESound::Initialise(
        int16 bankSlotId, int16 soundID, CAEAudioEntity* audioEntity, CVector pos, float volume, float rollOffFactor, float relativeFrequency, float doppler,
        uint8 frameDelay, uint32 flags, float frequencyVariance, int16 playTime)
{
    CHook::CallFunction<void>("_ZN8CAESound10InitialiseEssP14CAEAudioEntity7CVectorffffhtf", this, bankSlotId, soundID, audioEntity, pos, volume, rollOffFactor, relativeFrequency, doppler, frameDelay, flags, frequencyVariance, playTime);
}

void CAESound::UnregisterWithPhysicalEntity() {
    CEntity::ClearReference(m_pPhysicalEntity);
}

void CAESound::RegisterWithPhysicalEntity(CEntity* entity) {
    UnregisterWithPhysicalEntity();
    if (entity) {
        m_pPhysicalEntity = entity;
        entity->RegisterReference(&m_pPhysicalEntity);
    }
}

void CAESound::StopSoundAndForget() {
    CHook::CallFunction<void>("_ZN8CAESound18StopSoundAndForgetEv", this);
}