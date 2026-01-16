#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"

#include "StreamAtVehicle.h"

#include "StreamInfo.h"

extern CNetGame *pNetGame;

namespace Voice
{
StreamAtVehicle::StreamAtVehicle(const uint16_t color, std::string name,
                                 const float distance, const VEHICLEID vehicleId) noexcept
    : LocalStream(StreamType::LocalStreamAtVehicle, color, std::move(name), distance)
    , vehicleId(vehicleId)
{}

void StreamAtVehicle::Tick() noexcept
{
    this->LocalStream::Tick();


    CVehicleMP *pVehicle = CVehiclePool::GetAt(this->vehicleId);
    if(!pVehicle) return;

    for(const auto& channel : this->GetChannels())
    {
        if(channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pVehicle->m_pVehicle->GetPosition()),
                nullptr, nullptr);
        }
    }
}

void StreamAtVehicle::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);


    CVehicleMP *pVehicle = CVehiclePool::GetAt(this->vehicleId);
    if(!pVehicle) return;

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&pVehicle->m_pVehicle->GetPosition()),
        &kZeroVector, &kZeroVector);
}
}