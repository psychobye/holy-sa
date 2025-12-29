/*
by plaka
*/

#include "../main.h"
#include "game.h"
#include "font.h"

#include <vector>
#include <string>
#include <thread>
#include <sstream>

#include "..//net/netgame.h"
#include "game/Entity/Ped/Ped.h"

void CAudioStream::PlayLocalStream(bool use3d) {
    if(use3d) {
        m_hStream = BASS_StreamCreateURL(m_szUrl.c_str(), 0, (m_bReplay ? BASS_SAMPLE_LOOP : BASS_STREAM_AUTOFREE | BASS_STREAM_BLOCK) | BASS_STREAM_RESTRATE| BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX, 0, 0);

        BASS_3DVECTOR vec(m_vPos.x, m_vPos.y, m_vPos.z);
        BASS_3DVECTOR orient(0.0f, 0.0f, 0.0f);
        BASS_3DVECTOR vel(0.0f, 0.0f, 0.0f);
        BASS_ChannelSet3DPosition(m_hStream, &vec, &orient, &vel);
        BASS_ChannelSet3DAttributes(m_hStream, BASS_3DMODE_NORMAL, std::max(m_fDistance * 0.1, 5.0), m_fDistance, -1, -1, 0);
        BASS_Apply3D();
    }
	else {
        m_hStream = BASS_StreamCreateURL(m_szUrl.c_str(), 0, (m_bReplay ? BASS_SAMPLE_LOOP : BASS_STREAM_AUTOFREE | BASS_STREAM_BLOCK) | BASS_STREAM_RESTRATE, 0, 0);
	}

    BASS_ChannelPlay(m_hStream, false);
}

void CAudioStream::PlayStream()
{
	m_hStream = BASS_StreamCreateURL(m_szUrl.c_str(), 0, (m_bReplay ? BASS_SAMPLE_LOOP : BASS_STREAM_AUTOFREE ) | BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX, 0, 0);

    BASS_3DVECTOR vec(m_vPos.x, m_vPos.y, m_vPos.z);
    BASS_3DVECTOR orient(0.0f, 0.0f, 0.0f);
    BASS_3DVECTOR vel(0.0f, 0.0f, 0.0f);
    BASS_ChannelSet3DPosition(m_hStream, &vec, &orient, &vel);
    BASS_ChannelSet3DAttributes(m_hStream, BASS_3DMODE_NORMAL, std::max(m_fDistance * 0.1, 5.0), m_fDistance, -1, -1, 0);
    BASS_Apply3D();

    if(!m_bNeedSync) {
        BASS_ChannelPlay(m_hStream, false);
        return;
    }

    // hueta mb
    uint32 dif = CTimer::GetCurrentUnixTimeMoscow() - m_qwCreatedTimeInSecond;

    QWORD lengthBytes = BASS_ChannelGetLength(m_hStream, BASS_POS_BYTE);
    double lengthSeconds = BASS_ChannelBytes2Seconds(m_hStream, lengthBytes);

    if(dif < 5) {
        BASS_ChannelPlay(m_hStream, false);
        return;
    }
    //


    BASS_ChannelSetSync(m_hStream, BASS_SYNC_DOWNLOAD | BASS_SYNC_ONETIME, 0, [](HSYNC handle, DWORD channel, DWORD data, void* user) {
        if(!user)
            return;

        auto stream = (CAudioStream*)user;

        uint32 dif = CTimer::GetCurrentUnixTimeMoscow() - stream->m_qwCreatedTimeInSecond;

        QWORD lengthBytes = BASS_ChannelGetLength(stream->m_hStream, BASS_POS_BYTE);
        double lengthSeconds = BASS_ChannelBytes2Seconds(stream->m_hStream, lengthBytes);

        if(dif > lengthSeconds && !stream->m_bReplay)
            return;

        uint32 offsetInSecond = dif % (uint32)lengthSeconds;
        float percent = (offsetInSecond / lengthSeconds) * 100.f;
        uint64_t posInBytes = lengthBytes * (percent / 100);

        BASS_ChannelSetPosition(channel, posInBytes, BASS_POS_BYTE);
        BASS_ChannelPlay(channel, false);

    }, this);
}

void CAudioStream::StopStream()
{
	if (m_hStream)
	{
		BASS_StreamFree(m_hStream);
	}
	m_hStream = 0;
}

void CAudioStream::ProcessAttached()
{
	if(m_iAttachType == NONE) {
		m_numCurrInterior = m_nBaseInterior;
		return;
	}

	if (m_iAttachType == TOVEHICLE)
	{
		CVehicleSamp* pVeh = CVehiclePool::GetAt(m_iAttachedTo);
		if (!pVeh || !pVeh->m_pVehicle)
		{
            if(m_hStream)
                StopStream();

			return;
		}
		m_vPos = pVeh->m_pVehicle->GetPosition();
		m_numCurrInterior = pVeh->m_pVehicle->m_nAreaCode;
	}
	if (m_iAttachType == TOPLAYER) // player
	{
		CPedSamp *pPed = nullptr;
		if(m_iAttachedTo == CPlayerPool::GetLocalPlayerID()) {
			pPed = CLocalPlayer::GetPlayerPed();
		}
		else {
			if(CPlayerPool::GetSpawnedPlayer(m_iAttachedTo)) {
                pPed = CPlayerPool::GetSpawnedPlayer(m_iAttachedTo)->GetPlayerPed();
			}
		}
		if (!pPed || !pPed->m_pPed)
		{
            if(m_hStream)
                StopStream();

			return;
		}
		m_vPos = pPed->m_pPed->GetPosition();
		m_numCurrInterior = pPed->m_pPed->m_nAreaCode;
	}

    if(m_hStream){
        BASS_3DVECTOR vec(m_vPos.x, m_vPos.y, m_vPos.z);
        BASS_3DVECTOR orient(0.0f, 0.0f, 0.0f);
        BASS_3DVECTOR vel(0.0f, 0.0f, 0.0f);

        BASS_ChannelSet3DPosition(m_hStream, &vec, &orient, &vel);
    }
}

CAudioStream::CAudioStream(CVector* pPos, int iInterior, float fDistance, std::string szUrl, bool needReplay, float volume, uint32 creationTime)
{
	m_iAttachType = NONE;
	m_iAttachedTo = 0;
    m_bReplay = needReplay;
	m_fVolume = volume;
	m_hStream = 0;

	m_szUrl = szUrl;
	m_vPos = *pPos;

	m_numCurrInterior = m_nBaseInterior = iInterior;
	m_fDistance = fDistance;
    m_qwCreatedTimeInSecond = creationTime;
}

CAudioStream::~CAudioStream()
{
    StopStream();
}

void CAudioStream::Attach(eSoundAttachedTo toType, int toId)
{
	m_iAttachType = toType;
	m_iAttachedTo = toId;
}

void CAudioStream::SetVolume(float fValue)
{
	m_fVolume = fValue;
	BASS_ChannelSetAttribute(m_hStream, BASS_ATTRIB_VOL, fValue);
}

void CAudioStream::Process(RwMatrix* pMatListener)
{
    ProcessAttached();

	float fDistance = CUtil::GetDistanceBetween3DPoints(pMatListener->pos, m_vPos);
	if (fDistance <= m_fDistance && !m_hStream && (m_numCurrInterior == CGame::currArea || m_numCurrInterior == -1))
	{
		PlayStream();
		return;
	}
	if ((fDistance > m_fDistance || (m_numCurrInterior != CGame::currArea && m_numCurrInterior != -1) ) && m_hStream)
	{
		StopStream();
		return;
	}
}
