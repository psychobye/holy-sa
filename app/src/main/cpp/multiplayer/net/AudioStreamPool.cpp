#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../game/Entity/Ped/Ped.h"

// ����������� ������ ������ ���� repeat = false
void CAudioStreamPool::Init()
{
	bShutdownThread = false;

	new std::thread([] {
        while (!bShutdownThread)
        {
            Process();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void CAudioStreamPool::Free() {
    auto ids = GetAllIds();
    for (auto& id : ids) {
        DeleteStreamByID(id);
    }

    StopIndividualStream();
}

void CAudioStreamPool::AddStaticStream(float x, float y, float z, int iInterior, float fDistance, const char* szUrl, bool needRepeat)
{
    static uint32 totalStaticStream = 0;
    totalStaticStream ++;

    auto streamId = totalStaticStream * -1;

    CVector pos = CVector(x, y, z);
    auto pStream = new CAudioStream(&pos, iInterior, fDistance, szUrl, needRepeat, 0.7, 0);
    pStream->m_bNeedSync = false;

    PostToAudioThread([=]{
        list[streamId] = pStream;
    });
}

void CAudioStreamPool::AddStream(int id, CVector* pPos, int iInterior, float fDistance, const char* szUrl, bool needRepeat, uint32 creationTime)
{
    auto pStream = new CAudioStream(pPos, iInterior, fDistance, szUrl, needRepeat, 0.7, creationTime);

    PostToAudioThread([=]{
        DeleteStreamNoThreadSafe(id);
        list[id] = pStream;
    });
}

void CAudioStreamPool::DeleteStreamByID(int streamId)
{
    PostToAudioThread([=]{
        DeleteStreamNoThreadSafe(streamId);
    });
}

void CAudioStreamPool::DeleteStreamNoThreadSafe(int streamId)
{
    auto pStream = GetAt(streamId);
    if(pStream) {
        delete pStream;
        list.erase(streamId);
    }
}

void CAudioStreamPool::PlayIndividualStream(std::string szUrl, bool needReplay)
{
    auto pos = CVector();
    auto stream = new CAudioStream(&pos, 0, 0, szUrl, needReplay, DEFAULT_VOLUME, 0);

    PostToAudioThread([=]{
        delete m_pIndividual;

        m_pIndividual = stream;
        m_pIndividual->PlayLocalStream(false);
    });
}

void CAudioStreamPool::PlayIndividualStreamWithPos(std::string szUrl, CVector* pos, float &dist, bool needReplay)
{
    auto stream = new CAudioStream(pos, 0, 0, szUrl, needReplay, DEFAULT_VOLUME, 0);

    PostToAudioThread([=]{
        delete m_pIndividual;

        m_pIndividual = stream;
        m_pIndividual->PlayLocalStream(true);
    });
}

void CAudioStreamPool::StopIndividualStream()
{
    PostToAudioThread([]{
        delete m_pIndividual;
        m_pIndividual = nullptr;
    });
}

void CAudioStreamPool::AttachTo(int streamId, eSoundAttachedTo attachType, int toId)
{
    PostToAudioThread([=]{
        auto stream = GetAt(streamId);
        if(stream)
            stream->Attach(attachType, toId);
    });
}

void CAudioStreamPool::SetVolume(int streamId, float volume) {
    PostToAudioThread([=]{
        auto stream = GetAt(streamId);
        if(stream) {
            stream->SetVolume(volume);
        }
    });
}

#include "game/Widgets/TouchInterface.h"
#include "game/Camera.h"

void CAudioStreamPool::PostToAudioThread(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(mtx);
    tasks.push(std::move(task));
}

void CAudioStreamPool::ProcessAudioThreadTasks()
{
    std::queue<std::function<void()>> local_tasks;

    {
        std::lock_guard<std::mutex> lock(mtx);

        std::swap(tasks, local_tasks);
    }

    while (!local_tasks.empty()) {
        auto task = std::move(local_tasks.front());
        local_tasks.pop();
        task();
    }
}


void CAudioStreamPool::Process()
{
    ProcessAudioThreadTasks();

	if (CTimer::m_UserPause)
	{
		BASS_SetConfig(5, 0);
	}
	else if(CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_PHONE]->IsTouched(nullptr)){
		BASS_SetConfig(5, 1000);
	}
	else {
		BASS_SetConfig(5, 10000);
	}

	auto pLocalPed = CGame::FindPlayerPed();

	if(!pLocalPed || !pLocalPed->m_pPed)
		return;

    for (auto & pair : list)
    {
        auto pStream = pair.second;
        pStream->Process(reinterpret_cast<RwMatrix *>(&pLocalPed->m_pPed->GetMatrix()));
	}

	

	BASS_Set3DPosition(
			reinterpret_cast<const BASS_3DVECTOR*>(&CCamera::Get().GetPosition()), nullptr,
			reinterpret_cast<const BASS_3DVECTOR*>(&CCamera::Get().GetMatrix().GetUp()),
			reinterpret_cast<const BASS_3DVECTOR*>(&CCamera::Get().GetMatrix().GetForward())
	);

	BASS_Apply3D();
}