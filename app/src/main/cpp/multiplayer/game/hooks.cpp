#include "../main.h"
#include "RW/RenderWare.h"
#include <sstream>
#include "game.h"

#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../util/CJavaWrapper.h"
#include "../java_systems/HUD.h"
#include "game/Widgets/WidgetGta.h"
#include "game/WaterCannons.h"
#include "game/Pickups.h"
#include "chatwindow.h"
#include "../util/patch.h"
#include "java_systems/Speedometr.h"
#include "Timer.h"
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include "TxdStore.h"
#include "VisibilityPlugins.h"
#include "Renderer.h"
#include "Plugins/NodeNamePlugin/NodeName.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "CLoader.h"

extern CPedSamp *g_pCurrentFiredPed;

static uint32_t dwRLEDecompressSourceSize = 0;

extern bool g_bIsTestMode;

extern CGUI *pGUI;

bool g_bPlaySAMP = false;

void InitInMenu();
void MainLoop();
void HookCPad();

struct stFile
{
	int isFileExist;
	FILE *f;
};

char lastFile[123];

stFile* NvFOpen(const char* r0, const char* r1, int r2, int r3)
{
    strcpy(lastFile, r1);

	static char path[255]{};
	memset(path, 0, sizeof(path));

	sprintf(path, "%s%s", g_pszStorage, r1);

	if(!strncmp(r1+12, "mainV1.scm", 10))
	{
		sprintf(path, "%sSAMP/main.scm", g_pszStorage);
		Log("Loading %s", path);
	}
	if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sSAMP/script.img", g_pszStorage);
		Log("Loading script.img..");
	}
	if(!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%sSAMP/peds.ide", g_pszStorage);
		Log("Loading peds.ide..");
	}
	if(!strncmp(r1, "DATA/VEHICLES.IDE", 17))
	{
		sprintf(path, "%sSAMP/vehicles.ide", g_pszStorage);
		Log("Loading vehicles.ide..");
	}
	if (!strncmp(r1, "DATA/GTA.DAT", 12))
	{
		sprintf(path, "%sSAMP/gta.dat", g_pszStorage);
		Log("Loading gta.dat..");
	}
	if (!strncmp(r1, "DATA/HANDLING.CFG", 17))
	{
		sprintf(path, "%sSAMP/handling.cfg", g_pszStorage);
		Log("Loading handling.cfg..");
	}
	if (!strncmp(r1, "DATA/WEAPON.DAT", 15))
	{
		sprintf(path, "%sSAMP/weapon.dat", g_pszStorage);
		Log("Loading weapon.dat..");
	}

#if VER_x32
	auto *st = (stFile*)malloc(8);
#else
	auto *st = (stFile*)malloc(0x10);
#endif
	st->isFileExist = false;

	// Log("%s", path);
	FILE *f  = fopen(path, "rb");

	if(f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		return nullptr;
	}
}

#include "keyboard.h"
#include "voice/SpeakerList.h"
#include "voice/include/util/Render.h"
#include "game/Widgets/TouchInterface.h"
#include "Clouds.h"
#include "gps.h"

void MainMenu_OnStartSAMP()
{

	if(g_bPlaySAMP) return;

	InitInMenu();

	// StartGameScreen::OnNewGameCheck()
	(( void (*)())(g_libGTASA + (VER_x32 ? 0x002A7270 + 1 : 0x365EA0)))();

	g_bPlaySAMP = true;
}

#include "..//keyboard.h"
#include "game/Widgets/TouchInterface.h"
#include "World.h"
#include "../java_systems/SelectEntity.h"

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	if (CTimer::m_UserPause)
	{
		return TouchEvent(type, num, posX, posY);
	}

	if(!CGUI::OnTouchEvent(type, num, posX, posY))
		return;

	CTouchInterface::lastPosX = posX;
	CTouchInterface::lastPosY = posY;

	TouchEvent(type, num, posX, posY);
}

#if VER_x32
uint32_t CHudColours__GetIntColour(uint32 colour_id)
{
	return TranslateColorCodeToRGBA(colour_id);
}
#else
uint32_t CHudColours__GetIntColour(uintptr* thiz, uint32 colour_id)
{
	return TranslateColorCodeToRGBA(colour_id);
}
#endif

// fire weapon hooks
uint32_t (*CWeapon__FireInstantHit)(CWeapon * thiz, CPed * pFiringEntity, CVector * vecOrigin, CVector * muzzlePosn, CEntity * targetEntity, CVector * target, CVector * originForDriveBy, int arg6, int muzzle);
uint32_t CWeapon__FireInstantHit_hook(CWeapon * thiz, CPed * pFiringEntity, CVector * vecOrigin, CVector * muzzlePosn, CEntity * targetEntity, CVector * target, CVector * originForDriveBy, int arg6, int muzzle) __attribute__((optimize("O0")))
{
    auto &pLocalPed = CLocalPlayer::GetPlayerPed()->m_pPed;
    if(pLocalPed) {
        if (pFiringEntity != pLocalPed)
            return muzzle;

        if (pNetGame) {
            CPlayerPool::ApplyCollisionChecking();
        }

        if (CGame::GetGameInit()) {
            CPedSamp *pPlayerPed = CGame::FindPlayerPed();
            if (pPlayerPed)
                pPlayerPed->FireInstant();
        }

        if (pNetGame) {
            CPlayerPool::ResetCollisionChecking();
        }

        return muzzle;
    }

	return CWeapon__FireInstantHit(thiz, pFiringEntity, vecOrigin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}

unsigned int (*MainMenuScreen__Update)(uintptr_t thiz, float a2);
unsigned int MainMenuScreen__Update_hook(uintptr_t thiz, float a2)
{
	unsigned int ret = MainMenuScreen__Update(thiz, a2);
	MainMenu_OnStartSAMP();
	return ret;
}

int lastNvEvent;
#include "..//nv_event.h"
#include "ResourceCrypt/ResourceCrypt.h"

int32_t(*NVEventGetNextEvent_hooked)(NVEvent* ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent* ev, int waitMSecs)
{
	if(!ev)
		return 0;

	int32_t ret = NVEventGetNextEvent_hooked(ev, waitMSecs);

	lastNvEvent =  ev->m_type;

	NVEvent event;
	if(NVEventGetNextEvent(&event))
	{
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		if (type == NV_MULTITOUCH_CANCEL)
		{
			type = NV_MULTITOUCH_UP;
		}

		if ((x1 || y1) || num == 0)
		{
			if (num == 0 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(type, 0, x1, y1); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
			}
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(type, 1, x2, y2); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
			}
		}
		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(type, 2, x3, y3); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
			}
		}
	}

	return ret;
}

signed int (*OS_FileOpen)(unsigned int a1, OSFile *intoFile, const char *filename, int a4);
signed int OS_FileOpen_hook(unsigned int a1, OSFile *intoFile, const char *filename, int a4)
{
    return OS_FileOpen(a1, intoFile, filename, a4);
}

size_t (*OS_FileRead)(OSFile a1, void *buffer, size_t numBytes);
size_t OS_FileRead_hook(OSFile a1, void *buffer, size_t numBytes)
{
    dwRLEDecompressSourceSize = numBytes;

    if (!numBytes) {
        return 0;
    }

    size_t result = OS_FileRead(a1, buffer, numBytes);
    if (CResourceCrypt::IsEncryptedFile((uint8 *) buffer)) {
        CResourceCrypt::DecryptStream(static_cast<char *>(buffer));
    }
    return result;
}

extern char g_iLastBlock[123];
int *(*LoadFullTexture)(TextureDatabaseRuntime *thiz, unsigned int a2);
int *LoadFullTexture_hook(TextureDatabaseRuntime *thiz, unsigned int a2)
{
	strcpy(g_iLastBlock, thiz->name);

    return LoadFullTexture(thiz, a2);
}

int (*CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
    if(!resEntry || !flags)
        return 0;

    uint16_t size = *(uint16_t *)(resEntry+26);
    if(size)
    {
        RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ*)(resEntry+28);
        if(arr)
        {
            uint32_t validFlag = flags & 0x84;
            if(validFlag)
            {
                for(int i = 0; i < size; i++)
                {
                    if(!arr[i].validate) break;

                    uintptr_t *v4 = *(uintptr_t**)(arr[i].validate);
                    if(v4)
                    {
                        if(!*v4 || v4 > (uintptr_t*)0xFFFFFF00)
                            return 0;
                    }
                }
            }
        }
    }

    return CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry)
		return 0;
	uint16_t size = *(uint16_t *)(resEntry + 26);
	if (size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
		if (!arr)
			return 0;
		uint32_t validFlag = flags & 0x84;
		for (int i = 0; i < size; i++)
		{
			if (!arr[i].validate)
				break;
			if (validFlag)
			{
				uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
				if (!v4)
					;
				else
				{
					if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
						return 0;
					else
					{
						if (!*(uintptr_t **)v4)
							return 0;
					}
				}
			}
		}
	}
	return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

int (*GetMeshPriority)(const RpMesh*);
int GetMeshPriority_hook(const RpMesh* rpMesh)
{
	if (rpMesh)
	{
		RpMaterial *rpMeshMat = rpMesh->material;
		if (rpMeshMat)
		{
			if (rpMeshMat->texture)
			{
				if (!rpMeshMat->texture->raster)
					return 0;
			}
		}
	}
	return GetMeshPriority(rpMesh);
}

void (*StartGameScreen__OnNewGameCheck)();
void StartGameScreen__OnNewGameCheck_hook()
{
    // отключить кнопку начать игру
    if(g_bPlaySAMP)
        return;

    StartGameScreen__OnNewGameCheck();
}

int (*CTextureDatabaseRuntime__GetEntry)(TextureDatabaseRuntime *a1,const char *name, bool *hasSibling);
int CTextureDatabaseRuntime__GetEntry_hook(TextureDatabaseRuntime *a1, const char *name, bool *hasSibling)
{
	//Log("GetEntry = %s", (char*)a1);
	int result; // r0

	if (a1)
		result = CTextureDatabaseRuntime__GetEntry(a1, name, hasSibling);
	else
		result = -1;
	return result;
}

#include "common.h"
#include "game/Models/ModelInfo.h"
#include "cHandlingDataMgr.h"
#include "Pools.h"
#include "game/Core/MatrixLinkList.h"
#include "Collision/Collision.h"
#include "IdleCam.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimManager.h"
#include "References.h"
#include "CarEnterExit.h"
#include "game/Entity/Ped/PlayerPed.h"
#include "World.h"
#include "graphics/ES2VertexBuffer.h"
#include "graphics/RQ_Commands.h"
#include "Camera.h"
#include "Scene.h"
#include "game/Widgets/AdjustableHUD.h"
#include "game/Clouds.h"
#include "game/Weather.h"
#include "game/RenderBuffer.h"
#include "game/TimeCycle.h"
#include "game/Coronas.h"
#include "game/Draw.h"
#include "game/Clock.h"
#include "game/Birds.h"
#include "game/PathFind.h"
#include "game/FileLoader.h"
#include "game/Shadow/RealTimeShadowManager.h"
#include "game/Shadow/Shadows.h"
#include "Widgets/WidgetRadar.h"
#include "graphics/RQShader.h"
#include "Pipelines/CustomCar/CustomCarEnvMapPipeline.h"
#include "Collision/ColStore.h"
#include "Core/QuadTreeNode.h"
#include "IplStore.h"
#include "game/Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Audio/entities/AEVehicleAudioEntity.h"
#include "Audio/managers/AESoundManager.h"
#include "Audio/hardware/AEAudioHardware.h"
#include "Mirrors.h"
#include "Mobile/MobileSettings/MobileSettings.h"
#include "EntryExitManager.h"
#include "Occlusion.h"
#include "Radar.h"
#include "CPad.h"
#include "Mobile/MobileMenu/MobileMenu.h"
#include "BulletTraces.h"

void InjectHooks()
{
	Log("InjectHooks");

    //COcclusion::InjectHooks();
    CEntryExitManager::InjectHooks();
    CTaskSimpleUseGun::InjectHooks();
    CIplStore::InjectHooks();
    CQuadTreeNode::InjectHooks();
    CColStore::InjectHooks();
    CCustomCarEnvMapPipeline::InjectHooks();
    RQShader::InjectHooks();
    CAEAudioEntity::InjectHooks();
    CAEVehicleAudioEntity::InjectHooks();
    CMirrors::InjectHooks();
    CMobileMenu::InjectHooks();
    CMobileSettings::InjectHooks();
    // CPad::InjectHooks();

    CWeapon::InjectHooks();
    CWeaponInfo::InjectHooks();
    CBulletTraces::InjectHooks();

	CHook::Write(g_libGTASA + (VER_x32 ? 0x678954 : 0x84F2D0), &Scene);

#if !VER_x32 // mb all.. wtf crash x64?
    CHook::RET("_ZN11CPlayerInfo14LoadPlayerSkinEv");
    // CHook::RET("_ZN11CPopulation10InitialiseEv");
#endif
	CCamera::InjectHooks(); //
    CReferences::InjectHooks(); //
	CModelInfo::injectHooks(); //
	CTimer::InjectHooks(); //
	cTransmission::InjectHooks(); //
	CAnimBlendAssociation::InjectHooks(); //
	cHandlingDataMgr::InjectHooks(); //
	CPools::InjectHooks(); //
	CVehicle::InjectHooks(); //
	CMatrixLink::InjectHooks(); //
	CMatrixLinkList::InjectHooks(); //
	CStreaming::InjectHooks();
	CPlaceable::InjectHooks(); //
	CMatrix::InjectHooks(); //
    CCollision::InjectHooks(); //
	CIdleCam::InjectHooks(); //
	CTouchInterface::InjectHooks(); //
	CWidgetGta::InjectHooks();
	CEntity::InjectHooks(); //
	CPhysical::InjectHooks(); //
	CAnimManager::InjectHooks(); //
	//CCarEnterExit::InjectHooks();
	CPlayerPed::InjectHooks(); //
	CTaskManager::InjectHooks(); //
	CPedIntelligence::InjectHooks(); //
	CWorld::InjectHooks(); //
	CGame::InjectHooks();
    ES2VertexBuffer::InjectHooks();
	CRQ_Commands::InjectHooks();
	CTxdStore::InjectHooks();
	CVisibilityPlugins::InjectHooks();
	// CAdjustableHUD::InjectHooks();

	// new
	CClouds::InjectHooks();
	CWeather::InjectHooks();
	RenderBuffer::InjectHooks();
    CTimeCycle::InjectHooks();
	CCoronas::InjectHooks();
	CDraw::InjectHooks();
	CClock::InjectHooks();
	CBirds::Init();
	CVehicleModelInfo::InjectHooks();
	//CPathFind::InjectHooks();
	CSprite2d::InjectHooks();
	//CFileLoader::InjectHooks();
	CShadows::InjectHooks();
    //CPickups::InjectHooks();
	CRenderer::InjectHooks();
	CStreamingInfo::InjectHooks();
	TextureDatabase::InjectHooks();
	TextureDatabaseEntry::InjectHooks();
	TextureDatabaseRuntime::InjectHooks();
    CCustomBuildingDNPipeline::InjectHooks();
    CWidgetRadar::InjectHooks();
    CRadar::InjectHooks();

    // CRealTimeShadowManager::InjectHooks();
}

void (*NvUtilInit)();
void NvUtilInit_hook()
{
    DLOG("NvUtilInit");

    NvUtilInit();

    static char sStoragePath[] = "/storage/emulated/0/TESTLIT/";
    uintptr_t addrs[] = {
            g_libGTASA + (VER_x32 ? 0x679698 : 0x850D50),
            g_libGTASA + (VER_x32 ? 0x679984 : 0x851328)
    };

    for (auto addr : addrs) {
        CHook::UnFuck(addr);
        *(const char**)addr = sStoragePath;
    }

    g_pszStorage = sStoragePath;
    // g_pszStorage = (char*)(g_libGTASA + (VER_x32 ? 0x6D687C : 0x8B46A8)); // StorageRootBuffer

    CLoader::loadSetting();

    CLoader::initCrashLytics();

    CWeaponsOutFit::ParseDatFile();
}

#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <sys/resource.h>
#include <cstdlib>

typedef pthread_t OSThreadHandle;
typedef void* (*OSThreadFunction)(void*);

typedef enum {
    THREAD_RENDER_QUEUE,
    THREAD_CD_STREAM,
    THREAD_MAIN,
    THREAD_BANK_LOADER,
    THREAD_STREAM_RADIO,
    THREAD_DEFAULT
} CustomThreadType;

typedef struct {
    const char* threadNamePattern;
    int priority;
    size_t stackSizeKB;
} ThreadConfig;

static const ThreadConfig threadConfigs[] = {
        [THREAD_RENDER_QUEUE]  = { "RenderQueue", -20, 512 },
        [THREAD_CD_STREAM]     = { "CdStream",    -20, 512 },
        [THREAD_MAIN]          = { "MainThread",  -15, 256 },
        [THREAD_BANK_LOADER]   = { "BankLoader",   -5, 128 },
        [THREAD_STREAM_RADIO]  = { "StreamThread",  0, 128 },
        [THREAD_DEFAULT]       = { NULL,            0, 256 }
};

static CustomThreadType getThreadType(const char* threadName) {
    if (!threadName) return THREAD_DEFAULT;

    for (int i = 0; i < sizeof(threadConfigs)/sizeof(ThreadConfig); i++) {
        if (threadConfigs[i].threadNamePattern &&
            strstr(threadName, threadConfigs[i].threadNamePattern)) {
            return (CustomThreadType)i;
        }
    }
    return THREAD_DEFAULT;
}

struct ThreadLaunchData {
    void* thread_struct;
    OSThreadFunction func;
    char thread_name[32];
};

OSThreadHandle (*orig_OS_ThreadLaunch)(
        OSThreadFunction function,
        void* functionData,
        unsigned int processorAffinity,
        const char* threadName,
        int joinable,
        uintptr_t priority);
OSThreadHandle custom_OS_ThreadLaunch(
        OSThreadFunction function,
        void* functionData,
        unsigned int processorAffinity,
        const char* threadName,
        int joinable,
        uintptr_t priority) {

    CustomThreadType threadType = getThreadType(threadName);
    const ThreadConfig* config = &threadConfigs[threadType];

#if VER_x32
    if (threadType == THREAD_MAIN) {
        Log("ThreadMain return to original");
        return orig_OS_ThreadLaunch(function, functionData, processorAffinity, threadName, joinable, priority);
    }
#endif

    auto* threadData = (ThreadLaunchData*)malloc(sizeof(ThreadLaunchData));
    if (!threadData) return static_cast<OSThreadHandle>(NULL);

    threadData->func = function;
    threadData->thread_struct = functionData;

    if (threadName) {
        strncpy(threadData->thread_name, threadName, sizeof(threadData->thread_name)-1);
        threadData->thread_name[sizeof(threadData->thread_name)-1] = '\0';
    } else {
        strcpy(threadData->thread_name, "AppThread");
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    struct rlimit rlim{};
    getrlimit(RLIMIT_STACK, &rlim);

    size_t safe_stack = (rlim.rlim_cur == RLIM_INFINITY)
                        ? config->stackSizeKB * 1024
                        : std::min(config->stackSizeKB * 1024, (size_t)rlim.rlim_cur);

    if (pthread_attr_setstacksize(&attr, safe_stack) != 0) {
        Log("Stack size set failed, using default");
    }

    struct sched_param param = {0};
    pthread_attr_getschedparam(&attr, &param);

    int priority_min = sched_get_priority_min(SCHED_OTHER);
    int priority_max = sched_get_priority_max(SCHED_OTHER);
    int range = priority_max - priority_min;

    switch (priority) {
        case 0:
            param.sched_priority = priority_min;
            break;
        case 1:
            param.sched_priority = priority_min + (2 * range) / 3;
            break;
        case 2:
            param.sched_priority = priority_min + (4 * range) / 5;
            break;
        case 3:
            param.sched_priority = priority_max;
            break;
        default:
            param.sched_priority = 0;
            break;
    }

    if (pthread_attr_setschedparam(&attr, &param) != 0) {
        Log("Failed to set priority %d (range %d-%d)",
            param.sched_priority, priority_min, priority_max);
    }

    pthread_t thread;
    if (pthread_create(&thread, &attr, CJavaWrapper::NVThreadSpawnProc, threadData) != 0) {
        free(threadData);
        pthread_attr_destroy(&attr);
        return static_cast<OSThreadHandle>(NULL);
    }

    if (threadName) {
        pthread_setname_np(thread, threadName);
    }

    if (!joinable) {
        pthread_detach(thread);
    }

    pthread_attr_destroy(&attr);
    return thread;
}

void InstallSpecialHooks()
{
	InjectHooks();

	Log("InstallSpecialHooks");

    CHook::InlineHook("_Z10NvUtilInitv", &NvUtilInit_hook, &NvUtilInit);
	// fix но очень дропает фпс
	//CHook::InstallPLT(g_libGTASA + 0x677B64, &rqSetAlphaTest_hook, &rqSetAlphaTest);

	CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x6785FC : 0x84EC20), &StartGameScreen__OnNewGameCheck_hook, &StartGameScreen__OnNewGameCheck);

	// texture crash
	CHook::InlineHook("_ZN22TextureDatabaseRuntime8GetEntryEPKcRb", &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);

	//CHook::InlineHook("_Z15GetMeshPriorityPK6RpMesh", &GetMeshPriority_hook, &GetMeshPriority);
	//new fix
//#if VER_x32
//	CHook::InlineHook("_Z32_rxOpenGLDefaultAllInOneRenderCBP10RwResEntryPvhj", &rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);
//	CHook::InlineHook("_ZN25CCustomBuildingDNPipeline18CustomPipeRenderCBEP10RwResEntryPvhj", &CustomPipeRenderCB_hook, &CustomPipeRenderCB);
//#endif

   	//CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x6701D4 : 0x840708), &RLEDecompress_hook, &RLEDecompress);
	//
	CHook::InlineHook("_ZN22TextureDatabaseRuntime15LoadFullTextureEj", &LoadFullTexture_hook, &LoadFullTexture);

	CHook::RET("_ZN12CCutsceneMgr16LoadCutsceneDataEPKc"); // LoadCutsceneData
	CHook::RET("_ZN12CCutsceneMgr10InitialiseEv");			// CCutsceneMgr::Initialise

	CHook::InlineHook("_Z11OS_FileReadPvS_i", &OS_FileRead_hook, &OS_FileRead);
	CHook::Redirect("_Z7NvFOpenPKcS0_bb", &NvFOpen);

	CHook::Redirect("_ZN5CGame20InitialiseRenderWareEv", &CGame::InitialiseRenderWare);
	CHook::InlineHook("_ZN14MainMenuScreen6UpdateEf", &MainMenuScreen__Update_hook, &MainMenuScreen__Update);
	CHook::InlineHook("_Z11OS_FileOpen14OSFileDataAreaPPvPKc16OSFileAccessType", &OS_FileOpen_hook, &OS_FileOpen);

	CHook::InlineHook("_Z19NVEventGetNextEventP7NVEventi", NVEventGetNextEvent_hook, &NVEventGetNextEvent_hooked);

    CHook::InlineHook("_Z15OS_ThreadLaunchPFjPvES_jPKci16OSThreadPriority", &custom_OS_ThreadLaunch, &orig_OS_ThreadLaunch);
}

// thanks Codeesar for dont work damage calculator :(
struct stPedDamageResponse
{
	CEntity* pEntity;
	float fDamage;
	ePedPieceTypes iBodyPart;
	eWeaponType iWeaponType;
	bool bSpeak;
};

extern float m_fWeaponDamages[43 + 1];

void (*CPedDamageResponseCalculator__ComputeDamageResponse)(stPedDamageResponse* thiz, CEntity* pEntity, uintptr_t pDamageResponse, bool bSpeak);
void CPedDamageResponseCalculator__ComputeDamageResponse_hook(stPedDamageResponse* thiz, CEntity* pEntity, uintptr_t pDamageResponse, bool bSpeak)
{
	if(!thiz || thiz->fDamage == 0)
		return CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pEntity, pDamageResponse, bSpeak);

	if( thiz->iWeaponType < 0 || thiz->iWeaponType > std::size(m_fWeaponDamages) )
	{
		thiz->fDamage /= 3.0303030303;
	}
	else {
		thiz->fDamage = m_fWeaponDamages[thiz->iWeaponType];
	}

	float fDamage = thiz->fDamage;

	int bodypart = thiz->iBodyPart;

    auto senderId = CPlayerPool::FindRemotePlayerIDFromGtaPtr((CPed *) thiz->pEntity);
	auto receiverId = CPlayerPool::FindRemotePlayerIDFromGtaPtr((CPed *) pEntity);

	auto pSender = (CPed *)thiz->pEntity;
	auto pReceiver = (CPed *)pEntity;

	auto pLocalPed = CLocalPlayer::GetPlayerPed();

	if(!pLocalPed->m_bIsSpawned || pLocalPed->IsDead())
		return;

	if (pSender == pLocalPed->m_pPed)
	{
		if(receiverId == INVALID_PLAYER_ID) {
			receiverId = CActorPool::FindActorIdFromGta(pReceiver);
			CNetGame::ActorTakeDamage(receiverId, thiz->iWeaponType, fDamage, bodypart);
		}
		else {
			CNetGame::sendGiveDamage(receiverId, thiz->iWeaponType, fDamage, bodypart);
		}
		// CHUD::addGiveDamageNotify(receiverId, thiz->iWeaponType, fDamage, thiz->iBodyPart);
	}

	// player take damage
	else if (pReceiver == pLocalPed->m_pPed)
	{
		char nick[MAX_PLAYER_NAME];
		strcpy(nick, CPlayerPool::GetPlayerName(senderId));
		// CHUD::addTakeDamageNotify(CPlayerPool::GetPlayerName(senderId), thiz->iWeaponType, fDamage);

		CNetGame::sendTakeDamage(senderId, thiz->iWeaponType, fDamage, bodypart);
	}
//	CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pEntity, pDamageResponse, bSpeak);
}


RpMaterialList* (*_rpMaterialListDeinitialize)(RpMaterialList* matList);
RpMaterialList* _rpMaterialListDeinitialize_hook(RpMaterialList* matList)
{
    if(!matList || !matList->materials)
        return nullptr;

    return _rpMaterialListDeinitialize(matList);
}

int (*RwFrameAddChild)(int a1, int a2);
int RwFrameAddChild_hook(int a1, int a2)
{
    if(a1 == 0 || a2 == 0) return 0;

	return RwFrameAddChild(a1, a2);
}

int(*CUpsideDownCarCheck__IsCarUpsideDown)(uintptr* thiz, const CVehicleSamp* pVehicleToCheck);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(uintptr* thiz, const CVehicleSamp* pVehicleToCheck)
{
    /* Passengers leave the vehicle out of fear if it overturns */

//	if (*(uintptr_t*)(a2 + 20))
//	{
//		return CUpsideDownCarCheck__IsCarUpsideDown(a1, a2);
//	}
	return 0;
}

#include "..//keyboard.h"

#include "util.h"
#include <list>

RwFrame* CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, RpClump* clump, int32 modelId) {
    if (pFrameResult)
        return pFrameResult;

    uintptr_t calledFrom = 0;
    GET_LR(calledFrom);

    if (calledFrom == (VER_x32 ? 0x0058C61C : 0x6AFEE4)                // CVehicleSamp::SetWindowOpenFlag
        || calledFrom == (VER_x32 ? 0x0058C644 : 0x6AFF18)             // CVehicleSamp::ClearWindowOpenFlag
        || calledFrom == (VER_x32 ? 0x003885EC : 0x45FC40)             // CVehicleModelInfo::GetOriginalCompPosition
        || calledFrom == (VER_x32 ? 0x00387A28 : 0x45ECD0))            // CVehicleModelInfo::CreateInstance
        return nullptr;

    for (uint i = 2; i < 40; i++) {
        RwFrame* pNewFrameResult;
        pNewFrameResult = CHook::CallFunction<RwFrame*>("_ZN15CClumpModelInfo14GetFrameFromIdEP7RpClumpi", clump, i);
        if (pNewFrameResult) {
            return pNewFrameResult;
        }
    }
    return nullptr;
}

RwFrame* (*CClumpModelInfo_GetFrameFromId)(RpClump*, int32);
RwFrame* CClumpModelInfo_GetFrameFromId_hook(RpClump* clump, int32 modelId) {
    return CClumpModelInfo_GetFrameFromId_Post(CClumpModelInfo_GetFrameFromId(clump, modelId), clump, modelId);
}

#include "crashlytics.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;
//
//int (*_rwFreeListFreeReal)(uintptr* freeList, void *entry);
//int _rwFreeListFreeReal_hook(uintptr* freeList, void *entry)
//{
//    assert(freeList != nullptr);
//
//	if (freeList)
//	{
//		return _rwFreeListFreeReal(freeList, entry);
//	}
//	else
//	{
//		return 0;
//	}
//}

#include "CRenderTarget.h"
#include "CCustomPlateManager.h"
#include "Entity/Vehicle/Bike.h"

extern bool bDisableVehicleCollision;

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
int32 (*CAutomobile__ProcessEntityCollision)(CVehicle* thiz, CEntity* ent, CColPoint* aColPoints);
int32 CAutomobile__ProcessEntityCollision_hook(CVehicle* thiz, CEntity* ent, CColPoint* aColPoints) {

    g_usLastProcessedModelIndexAutomobile = thiz->m_nModelIndex;
    g_iLastProcessedModelIndexAutoEnt = thiz->m_nModelIndex;

    if (bDisableVehicleCollision) {
        if (ent->IsVehicle() || ent->m_nType == ENTITY_TYPE_PED) {
            return 0;
        }
    }

	bool bReplace = false;
    CColLine* pOld = nullptr;
    CCollisionData* pColData = nullptr;

	if (pNetGame)
	{
		uint16_t vehId = CVehiclePool::FindIDFromGtaPtr(thiz);
		CVehicleSamp* pVeh = CVehiclePool::GetAt(vehId);
		if (pVeh) {
			if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
				pColData = CModelInfo::GetModelInfo(thiz->m_nModelIndex)->AsVehicleModelInfoPtr()->m_pColModel->m_pColData;
				if (pColData && pVeh->m_pSuspensionLines) {
					if (pColData->m_pLines) {
						pOld = pColData->m_pLines;
                        pColData->m_pLines = pVeh->m_pSuspensionLines;
						bReplace = true;
					}
				}
			}
		}
	}
	auto result = CAutomobile__ProcessEntityCollision(thiz, ent, aColPoints);

	if (bReplace) {
        pColData->m_pLines = pOld;
	}

    return result;
}

int32 (*CBike__ProcessEntityCollision)(CBike* thiz, CEntity* ent, CColPoint* aColPoints);
int32 CBike__ProcessEntityCollision_hook(CBike* thiz, CEntity* ent, CColPoint* aColPoints) {

    if (bDisableVehicleCollision) {
        if (ent->IsVehicle() || ent->m_nType == ENTITY_TYPE_PED) {
            return 0;
        }
    }

    return CBike__ProcessEntityCollision(thiz, ent, aColPoints);
}

int32 (*CMonsterTruck__ProcessEntityCollision)(void* thiz, CEntity* ent, CColPoint* aColPoints);
int32 CMonsterTruck__ProcessEntityCollision_hook(void* thiz, CEntity* ent, CColPoint* aColPoints) {

    if (bDisableVehicleCollision) {
        if (ent->IsVehicle() || ent->m_nType == ENTITY_TYPE_PED) {
            return 0;
        }
    }

    return CMonsterTruck__ProcessEntityCollision(thiz, ent, aColPoints);
}

int32 (*CTrailer__ProcessEntityCollision)(void* thiz, CEntity* ent, CColPoint* aColPoints);
int32 CTrailer__ProcessEntityCollision_hook(void* thiz, CEntity* ent, CColPoint* aColPoints) {

    if (bDisableVehicleCollision) {
        if (ent->IsVehicle() || ent->m_nType == ENTITY_TYPE_PED) {
            return 0;
        }
    }

    return CTrailer__ProcessEntityCollision(thiz, ent, aColPoints);
}

void (*MainMenuScreen__OnExit)();
void MainMenuScreen__OnExit_hook()
{
	CGame::exitGame();

	//g_pJavaWrapper->ExitGame();

	//return CGame__Shutdown();
}

static CVehicleSamp* g_pLastProcessedVehicleMatrix = nullptr;
static int g_iLastProcessedWheelVehicle = -1;

void (*CMatrix__Rotate)(CMatrix* thiz, float a1, float a2, float a3);
void CMatrix__Rotate_hook(CMatrix* thiz, float a1, float a2, float a3)
{
	uintptr_t dwRetAddr = 0;
    GET_LR(dwRetAddr);

	CMatrix__Rotate(thiz, a1, a2, a3);

	if (dwRetAddr == 0x003A9D76 + 1)
		return;

	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1) {
		if (g_iLastProcessedWheelVehicle == 2) {
			thiz->RotateY(-g_pLastProcessedVehicleMatrix->m_fWheelAngleFront);
		}
		if (g_iLastProcessedWheelVehicle == 4) {
			thiz->RotateY(-g_pLastProcessedVehicleMatrix->m_fWheelAngleBack);
		}
		if (g_iLastProcessedWheelVehicle == 5) {
			thiz->RotateY(g_pLastProcessedVehicleMatrix->m_fWheelAngleFront);
		}
		if (g_iLastProcessedWheelVehicle == 7) {
			thiz->RotateY(g_pLastProcessedVehicleMatrix->m_fWheelAngleBack);
		}
	}
}

void (*CMatrix__SetScale)(void* thiz, float x, float y, float z);
void CMatrix__SetScale_hook(void* thiz, float x, float y, float z)
{
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_iLastProcessedWheelVehicle >= 2 || g_iLastProcessedWheelVehicle <= 7)
		{
			auto pModel = CModelInfo::GetVehicleModelInfo(g_pLastProcessedVehicleMatrix->m_pVehicle->m_nModelIndex);

			if(g_pLastProcessedVehicleMatrix->m_fWheelSize != g_pLastProcessedVehicleMatrix->m_fDefaultWheelSize) {
				y *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3;
				z *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3;

			}

			if (g_pLastProcessedVehicleMatrix->m_fWheelWidth != 0)
			{
				x = g_pLastProcessedVehicleMatrix->m_fWheelWidth;
			}
		}
	}

	CMatrix__SetScale(thiz, x, y, z);
}

void (*CAutomobile__UpdateWheelMatrix)(CVehicle* thiz, int, int);
void CAutomobile__UpdateWheelMatrix_hook(CVehicle* thiz, int nodeIndex, int flags)
{
	if (g_pLastProcessedVehicleMatrix)
	{
		g_iLastProcessedWheelVehicle = nodeIndex;
	}

	CAutomobile__UpdateWheelMatrix(thiz, nodeIndex, flags);
}

void (*CAutomobile__PreRender)(CVehicle* thiz);
void CAutomobile__PreRender_hook(CVehicle* thiz)
{
	if(!thiz->IsSubAutomobile()) {
		// bug trailer, monster and etc
		CAutomobile__PreRender(thiz);
		return;
	}


	auto pModelInfoStart = CModelInfo::GetVehicleModelInfo(thiz->m_nModelIndex);

	float fOldFront = pModelInfoStart->m_fWheelSizeFront;
	float fOldRear = pModelInfoStart->m_fWheelSizeRear;

	uint16_t vehid = CVehiclePool::FindIDFromGtaPtr(thiz);
    auto pVeh = CVehiclePool::GetAt(vehid);
    if (pVeh) {
        pVeh->ProcessWheelsOffset();
        g_pLastProcessedVehicleMatrix = pVeh;

        if (pVeh->neon.IsSet()) {
			pVeh->neon.Render(pVeh->m_pVehicle);
        }
    }

	CAutomobile__PreRender(thiz);

	pModelInfoStart->m_fWheelSizeFront = fOldFront;
	pModelInfoStart->m_fWheelSizeRear = fOldRear;

	g_pLastProcessedVehicleMatrix = nullptr;
	g_iLastProcessedWheelVehicle = -1;
}

void (*CTaskSimpleUseGun__RemoveStanceAnims)(uintptr* thiz, void* ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(uintptr* thiz, void* ped, float a3)
{
	if(!thiz)
		return;

	uintptr* m_pAnim = (uintptr*)(thiz + 0x2c);
	if(m_pAnim) {
		if (!((uintptr *)(m_pAnim + 0x14))) {
            assert("suka");
            return;
        }
	}
	CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
}

void (*CCam__Process)(CCam*);
void CCam__Process_hook(CCam* thiz)
{
    CFirstPersonCamera::Update();

    if (!CFirstPersonCamera::IsEnabled()) {
        CCam__Process(thiz);
        return;
    }
    CVector vecSpeed;
    CVehicleSamp* veh = nullptr;

    float& CAR_FOV_START_SPEED = *(float*)(g_libGTASA + (VER_x32 ? 0x006A9FD0 : 0x8855D4));
    float old = CAR_FOV_START_SPEED;

    if (pNetGame && (thiz->m_nMode == MODE_CAM_ON_A_STRING || thiz->m_nMode == MODE_1STPERSON))
    {
        auto ped = CLocalPlayer::GetPlayerPed()->m_pPed;
        if (ped->IsInVehicle() && ped->pVehicle)
        {
            veh = CVehiclePool::FindVehicle(ped->pVehicle);
            vecSpeed = veh->m_pVehicle->m_vecMoveSpeed;
            veh->m_pVehicle->m_vecMoveSpeed *= 6.0f;

            CAR_FOV_START_SPEED = 200.0f;
        }
    }

    CCam__Process(thiz);
    if (veh)
    {
        veh->m_pVehicle->m_vecMoveSpeed = vecSpeed;
        CAR_FOV_START_SPEED = old;
    }
    if(thiz->m_nMode == MODE_FOLLOWPED || thiz->m_nMode == MODE_AIMWEAPON)
    {
        if (pNetGame)
        {
            if (auto pPed = CLocalPlayer::GetPlayerPed())
            {
                CCamera::Get().m_uiTransitionDuration = 0xFFFFFFFF;
                CCamera::Get().m_uiTransitionDurationTargetCoors = 0xFFFFFFFF;
                CCamera::Get().m_bJust_Switched = false;

                CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
            }
        }
    }
    if((thiz->m_nMode == MODE_CAM_ON_A_STRING || thiz->m_nMode == MODE_1STPERSON))
    {
        if (pNetGame)
        {
            CPedSamp* pPed = CLocalPlayer::GetPlayerPed();
            if (pPed)
            {
                CCamera::Get().m_uiTransitionDuration = 0xFFFFFFFF;
                CCamera::Get().m_uiTransitionDurationTargetCoors = 0xFFFFFFFF;
                CCamera::Get().m_bJust_Switched = false;

                CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, veh);
            }
        }
    }
}

int g_iCounterVehicleCamera = 0;
int (*CPad__CycleCameraModeDownJustDown)(void*);
int CPad__CycleCameraModeDownJustDown_hook(void* thiz)
{
	if (!g_pWidgetManager)
	{
		return 0;
	}

	CPed* pPed = GamePool_FindPlayerPed();
	if (!pPed)
	{
		return 0;
	}

	static uint32_t lastTick = CTimer::m_snTimeInMillisecondsNonClipped;
	bool bPressed = false;
	if (CHUD::bIsTouchCameraButt && CTimer::m_snTimeInMillisecondsNonClipped - lastTick >= 250)
	{
		CHUD::bIsTouchCameraButt = false;
		bPressed = true;
		lastTick = CTimer::m_snTimeInMillisecondsNonClipped;
	}

	if (pPed->bInVehicle)
	{
		if (bPressed)
		{
			g_iCounterVehicleCamera++;
		}
		if (g_iCounterVehicleCamera == 6)
		{
			CFirstPersonCamera::SetEnabled(true);
			return 0;
		}
		else if (g_iCounterVehicleCamera >= 7)
		{
			g_iCounterVehicleCamera = 0;
			CFirstPersonCamera::SetEnabled(false);
			return 1;

		}
		else
		{
			CFirstPersonCamera::SetEnabled(false);
		}

		return bPressed;
	}
	return 0;
}

void (*FxEmitterBP_c__Render)(uintptr_t* a1, int a2, int a3, float a4, char a5);
void FxEmitterBP_c__Render_hook(uintptr_t* a1, int a2, int a3, float a4, char a5)
{
	if(!a1 || !a2) return;
	uintptr_t* temp = *((uintptr_t**)a1 + 3);
	if (!temp)
	{
        assert("sukax2");
		return;
	}
	FxEmitterBP_c__Render(a1, a2, a3, a4, a5);
}

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

int32 (*CPed__ProcessEntityCollision)(CPed* thiz, CEntity* ent, CColPoint* colPoint);
int32 CPed__ProcessEntityCollision_hook(CPed* thiz, CEntity* ent, CColPoint* colPoint) {
	g_iLastProcessedSkinCollision = thiz->m_nModelIndex;
	g_iLastProcessedEntityCollision = ent->m_nModelIndex;

    if (bDisableVehicleCollision) {
        if (ent->IsVehicle() || ent->m_nType == ENTITY_TYPE_PED) {
            return 0;
        }
    }

	return CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

int (*CTaskSimpleGetUp__ProcessPed)(uintptr_t* thiz, CPed* ped);
int CTaskSimpleGetUp__ProcessPed_hook(uintptr_t* thiz, CPed* ped)
{
    //return false;
	if(!ped)return 0;
	int res = 0;
	try {
		res = CTaskSimpleGetUp__ProcessPed(thiz, ped);
	}
	catch(...) {
		return 0;
	}

	return res;
}

int (*RwResourcesFreeResEntry)(int);
int RwResourcesFreeResEntry_hook(int a1)
{
	int result; // r0

	if (a1)
		result = RwResourcesFreeResEntry(a1);
	else
		result = 0;
	return result;
}

RpMaterial* (*SetCompAlphaCB)(RpMaterial *pMaterial, void *pData);
RpMaterial* SetCompAlphaCB_hook(RpMaterial *pMaterial, void *pData)
{
    RpMaterial* result; // r0

	if (pMaterial)
		result = SetCompAlphaCB(pMaterial, pData);
	else
		result = nullptr;

	return result;
}

bool (*CCollision__ProcessVerticalLine)(const CColLine *Line, const CMatrix *mat, CColModel *colModel, CColPoint *colPoint, float *fCollisionRatio, bool bSeeThroughStuff, bool bShootThroughStuff, CStoredCollPoly *pStoredPoly);
bool CCollision__ProcessVerticalLine_hook(const CColLine *Line, const CMatrix *mat, CColModel *colModel, CColPoint *colPoint, float *fCollisionRatio, bool bSeeThroughStuff, bool bShootThroughStuff, CStoredCollPoly *pStoredPoly)
{
	bool result;

	if (colModel)
		result = CCollision__ProcessVerticalLine(Line, mat, colModel, colPoint, fCollisionRatio, bSeeThroughStuff, bShootThroughStuff, pStoredPoly);
	else
		result = 0;
	return result;
}


int (*CWeapon__GenerateDamageEvent)(CPed *victim, CEntity *creator, eWeaponType weaponType, int32 damageFactor, ePedPieceTypes pedPiece, int32 direction);
int CWeapon__GenerateDamageEvent_hook(CPed *victim, CEntity *creator, eWeaponType weaponType, int32 damageFactor, ePedPieceTypes pedPiece, int32 direction)

{
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Disable ComputeDamageAnim when it's not a head!
		CHook::NOP(g_libGTASA + (VER_x32 ? 0x005E14E4 : 0x706A58), 2);
		CHook::NOP(g_libGTASA + (VER_x32 ? 0x005E15FC : 0x706BD8), 2);
	}
	int result = CWeapon__GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction);
	if (pedPiece != PED_PIECE_HEAD)
	{
#if VER_x32
		CHook::WriteMemory(g_libGTASA + 0x005E14E4, (uintptr_t) "\x46\xF5\xFE\xFC", 4);
		CHook::WriteMemory(g_libGTASA + 0x005E15FC, (uintptr_t) "\x8D\xF5\xAE\xFF", 4);
#else
        CHook::WriteMemory(g_libGTASA + 0x706A58, (uintptr_t) "\xDB\xFB\xF4\x17", 4);
        CHook::WriteMemory(g_libGTASA + 0x706BD8, (uintptr_t) "\x73\xE7\xF4\x17", 4);
#endif
	}
	return result;
}



uint32_t (*CWeapon__FireSniper)(CWeapon *pWeaponSlot, CPed *pFiringEntity, CEntity *a3, CVector *vecOrigin);
uint32_t CWeapon__FireSniper_hook(CWeapon *pWeaponSlot, CPed *pFiringEntity, CEntity *a3, CVector *vecOrigin)
{
	if(GamePool_FindPlayerPed() == pFiringEntity)
	{
		if(CGame::GetGameInit())
		{
			CPedSamp* pPlayerPed = CGame::FindPlayerPed();
			if(pPlayerPed)
				pPlayerPed->FireInstant();
		}
	}

	return 1;
}

void SendBulletSync(CVector *vecOrigin, CVector *vecEnd, CColPoint *colPoint, CEntity *pEntity)
{
    uint8_t byteHitType = BULLET_HIT_TYPE_NONE;
    uint16  InstanceID  = INVALID_PLAYER_ID;

    if (pEntity) {
        InstanceID = CPlayerPool::FindRemotePlayerIDFromGtaPtr((CPed*)pEntity);
        if (InstanceID == INVALID_PLAYER_ID)
        {
            InstanceID = CVehiclePool::FindIDFromGtaPtr((CVehicle*)pEntity);
            if (InstanceID == INVALID_VEHICLE_ID)
            {
                InstanceID = CObjectPool::FindIDFromGtaPtr(pEntity);
                if (InstanceID == INVALID_OBJECT_ID)
                {

                }
                else
                {
                    byteHitType = BULLET_HIT_TYPE_OBJECT;
                }
            }
            else
            {
                byteHitType = BULLET_HIT_TYPE_VEHICLE;
            }
        }
        else
        {
            byteHitType = BULLET_HIT_TYPE_PLAYER;
        }
    }

    const BULLET_SYNC data = {
            .byteHitType    = static_cast<eBulletSyncHitType>(byteHitType),
            .hitId          = InstanceID,
            .vecOrigin 	    = *vecOrigin,
            .vecHit 	    = colPoint->m_vecPoint,
            .vecOffset      = *vecEnd,
            .byteWeaponID   = static_cast<uint8_t>(CGame::FindPlayerPed()->GetCurrentWeapon())
    };

    RakNet::BitStream bsBullet;
    bsBullet.Write((char)ID_BULLET_SYNC);
    bsBullet.Write((char*)&data, sizeof(BULLET_SYNC));

    pNetGame->GetRakClient()->Send(&bsBullet, HIGH_PRIORITY, RELIABLE, 0);
}

bool g_bForceWorldProcessLineOfSight = false;
uint32_t (*CWeapon__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CColPoint *colPoint, CPed **ppEntity, CWeapon *pWeaponSlot, CPed **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
uint32_t CWeapon__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CColPoint *colPoint, CPed **ppEntity, CWeapon *pWeaponSlot, CPed **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7)
{
	uintptr_t dwRetAddr = 0;
    GET_LR(dwRetAddr);

#if VER_x32
	if(dwRetAddr >= 0x005DC178 && dwRetAddr <= 0x005DD684)
		g_bForceWorldProcessLineOfSight = true;
#else
    if(dwRetAddr >= 0x701494 && dwRetAddr <= 0x702B18)
        g_bForceWorldProcessLineOfSight = true;
#endif

	return CWeapon__ProcessLineOfSight(vecOrigin, vecEnd, colPoint, ppEntity, pWeaponSlot, ppEntity2, b1, b2, b3, b4, b5, b6, b7);
}

bool (*CWorld__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CColPoint *colPoint, CEntity **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8);
bool CWorld__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CColPoint *colPoint, CEntity **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
    GET_LR(dwRetAddr);

	if(dwRetAddr == (VER_x32 ? 0x005dd0b0 + 1 : 0x70253C) || g_bForceWorldProcessLineOfSight)
	{
		g_bForceWorldProcessLineOfSight = false;

		if(g_pCurrentFiredPed != CLocalPlayer::GetPlayerPed())
		{

			if(g_pCurrentFiredPed->m_bHaveBulletData) {

				g_pCurrentFiredPed->m_bHaveBulletData = false;

				auto bulletData = &g_pCurrentFiredPed->m_bulletData;

				*vecOrigin 	= bulletData->vecOrigin;

				if(bulletData->pEntity) {
					ProjectMatrix(vecEnd, &bulletData->pEntity->GetMatrix(), &bulletData->vecOffset);
				}
                else {
                    *vecEnd 	= bulletData->vecPos;
                }

				vecEnd->x = std::clamp(vecEnd->x, -10000.0f, 10000.0f);
				vecEnd->y = std::clamp(vecEnd->y, -10000.0f, 10000.0f);
				vecEnd->z = std::clamp(vecEnd->z, -10000.0f, 10000.0f);

                return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, colPoint, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
			}
		}

		else
		{
			auto result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, colPoint, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

			SendBulletSync(vecOrigin, vecEnd, colPoint, *ppEntity);

			return result;
		}

	}

	return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, colPoint, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}

bool (*CEventKnockOffBike__AffectsPed)(uintptr_t *thiz, CPed *a2);
bool CEventKnockOffBike__AffectsPed_hook(uintptr_t *thiz, CPed *a2)
{
	return false;
}

void (*CPed__SetCurrentWeapon)(CPed* thiz, eWeaponType weaponType);
void CPed__SetCurrentWeapon_hook(CPed* thiz, eWeaponType weaponType) {
	CPed__SetCurrentWeapon(thiz, weaponType);

	CHUD::updateAmmo();
}

#include "../game/Coronas.h"
#include "app/app_game.h"

long long (*CAnimBlendNode__FindKeyFrame)(CAnimBlendNode *thiz, float fCurrentTime);
long long CAnimBlendNode__FindKeyFrame_hook(CAnimBlendNode *thiz, float fCurrentTime)
{
	if (thiz->m_pAnimSequence)
	{
		return CAnimBlendNode__FindKeyFrame(thiz, fCurrentTime);
	}
	else return 0;
}

#include "../SkyBox.h"
#include "Widgets/WidgetButton.h"

int g_iLastRenderedObject;

void(*CEntity__Render)(CEntity*);
void CEntity__Render_hook(CEntity* entity) {
	if (CSkyBox::GetSkyObject()) {
		//Log("thiz = %x, ent = %x", thiz, CSkyBox::GetSkyObject()->m_pEntity);
		if (CSkyBox::GetSkyObject()->m_pEntity == entity && !CSkyBox::IsNeedRender()) {
            return;
        }
	}

    if ((entity->IsPed() || entity->IsObject()) && CMirrors::bRenderingReflection)
        return;

    g_iLastRenderedObject = entity->m_nModelIndex;
    CEntity__Render(entity);
}

void(*CFireManager__ExtinguishPointWithWater)(uintptr* thiz, CVector point, float fRadius, float fWaterStrength);
void CFireManager__ExtinguishPointWithWater_hook(uintptr* thiz, CVector point, float fRadius, float fWaterStrength) {
	//if (CWaterCannons::bFromLocalPlayer) {
		CNetGame::SendExtinguishPointWithWater(&point, &fRadius, &fWaterStrength);
	//}
    CFireManager__ExtinguishPointWithWater(thiz, point, fRadius, fWaterStrength);
}

int64 TextureListing_GetMipCount() {
    return 1;
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char* name)
{
    if(pNetGame && !strncmp(name, "CODEWAY", 7))
    {
        float fFindZ = CWorld::FindGroundZForCoord(X, Y) + 1.5f;

        if(pNetGame->GetGameState() != eNetworkState::CONNECTED) return 0;

        RakNet::BitStream bsSend;
        bsSend.Write(X);
        bsSend.Write(Y);
        bsSend.Write(fFindZ);
        pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }

    return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint32_t(*HashString)(const char* s);
uint32_t HashString_hook(const char* s)
{
    const char* p = &s[0];
    uint32_t hashPart = 0;
    while(*p != 0)
    {
    hashPart = 33 * hashPart + *p;
    ++p;
    }
    return (hashPart + (hashPart >> 5));
}

// Fixes farclip glitch with wall (wardumb be like)
void(*emu_DistanceFogSetup)(float minDistance, float maxDistance, float red, float green, float blue);
void emu_DistanceFogSetup_hook(float minDistance, float maxDistance, float red, float green, float blue)
{
    emu_DistanceFogSetup(0.8f * minDistance, 0.95f * maxDistance, red, green, blue);
}

void (*CWidgetButtonSprint_CWidgetButtonSprint)(uintptr_t *thiz, uint8_t *pszSprite, CTouchInterface::WidgetPositionIDs *WidgetPos);
void CWidgetButtonSprint_CWidgetButtonSprint_hook(uintptr_t *thiz, uint8_t *pszSprite, CTouchInterface::WidgetPositionIDs *WidgetPos)
{
    CWidgetButtonSprint_CWidgetButtonSprint(thiz, (uint8_t*)"", WidgetPos);
}

RwImage* (*RsGrabScreen)(RwCamera* camera);
RwImage* RsGrabScreen_hook(RwCamera* camera)
{
    if(!camera || !camera->frameBuffer) return NULL;

    RwImage* newImage = RwImageCreate(camera->frameBuffer->width, camera->frameBuffer->height, 32);
    if(newImage)
    {
        RwImageAllocatePixels(newImage);
        RwImageSetFromRaster(newImage, camera->frameBuffer);
    }
    return newImage;
}

void JPegCompressScreenToFile(RwCamera* pCamera, const char* pFilename){
    return CHook::CallFunction<void>("_Z24JPegCompressScreenToFileP8RwCameraPKc", pCamera, pFilename);
}

// TODO: move to CWeapon::Fire
RwCamera* (*CWeapon_TakePhotograph)(CWeapon *thiz, CEntity *pEntity, CVector *cameraPos);
RwCamera* CWeapon_TakePhotograph_hook(CWeapon *thiz, CEntity *pEntity, CVector *cameraPos)
{
    static uint32_t nextAllowedPic = 0;

    if(nextAllowedPic < CTimer::m_snTimeInMilliseconds)
    {
        nextAllowedPic = CTimer::m_snTimeInMilliseconds + 5000;

        CFileMgr::SetDirMyDocuments();

        std::string path = "screenshot_test.jpg";
        std::vector<char> filename(path.begin(), path.end());
        filename.push_back('\0');

        JPegCompressScreenToFile(Scene.m_pRwCamera, filename.data());

        CFileMgr::SetDir("");

        Log("Screenshot saved!");
    }

    return CWeapon_TakePhotograph(thiz, pEntity, cameraPos);
}

void (*jcopy_sample_rows)(uint8_t **input_array, int source_row, uint8_t **output_array, int dest_row, int num_rows, uint num_cols);
void jcopy_sample_rows_hook(uint8_t **input_array, int source_row, uint8_t **output_array, int dest_row, int num_rows, uint num_cols)
{
    jcopy_sample_rows(input_array, source_row, output_array, dest_row, num_rows, num_cols);
    Log("jpeg_samplecopy: dest_row (i) = %d, num_rows (height) = %d, num_cols (width) = %d", source_row, dest_row, num_cols);
}

void InstallHooks()
{
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66F91C : 0x83F8A0), &CFireManager__ExtinguishPointWithWater_hook, &CFireManager__ExtinguishPointWithWater);

	// retexture
	CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66F76C : 0x83F610), &CEntity__Render_hook, &CEntity__Render);

	CHook::Redirect("_Z13RenderEffectsv", &RenderEffects);

    // CHook::InlineHook("_ZN6CRadar12SetCoordBlipE9eBlipType7CVectorj12eBlipDisplayPc", &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);

    // ---------- JPATCH ----------
    // CHook::InlineHook("_Z10HashStringPKc", &HashString_hook, &HashString);
    CHook::InlineHook("_Z20emu_DistanceFogSetupfffff", &emu_DistanceFogSetup_hook, &emu_DistanceFogSetup);
    CHook::InlineHook("_ZN19CWidgetButtonSprintC2EPKcRK14WidgetPosition", &CWidgetButtonSprint_CWidgetButtonSprint_hook, &CWidgetButtonSprint_CWidgetButtonSprint);
    // CHook::InlineHook("_Z12psGrabScreenP8RwCamera", &RsGrabScreen_hook, &RsGrabScreen);
    // CHook::InlineHook("_ZN7CWeapon14TakePhotographEP7CEntityP7CVector", &CWeapon_TakePhotograph_hook, &CWeapon_TakePhotograph);
    // CHook::InlineHook("_Z17jcopy_sample_rowsPPhiS0_iij", &jcopy_sample_rows_hook, &jcopy_sample_rows);
    // ---------- JPATCH END ----------

    // WTFBUG lol
    CHook::InlineHook("_ZN4CPed16SetCurrentWeaponE11eWeaponType", &CPed__SetCurrentWeapon_hook, &CPed__SetCurrentWeapon);

	// CHook::Redirect("_Z19PlayerIsEnteringCarv", &PlayerIsEnteringCar); // crash

	// no fall bike
	CHook::InlineHook("_ZNK18CEventKnockOffBike10AffectsPedEP4CPed", &CEventKnockOffBike__AffectsPed_hook, &CEventKnockOffBike__AffectsPed);

    // TODO: FIX
	// Bullet sync
    CHook::InlineHook("_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb", &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
	CHook::InlineHook("_ZN7CWeapon10FireSniperEP4CPedP7CEntityP7CVector", &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
	CHook::InlineHook("_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb", &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);
	CHook::InlineHook("_ZN28CPedDamageResponseCalculator21ComputeDamageResponseEP4CPedR18CPedDamageResponseb", &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
	CHook::InlineHook("_ZN7CWeapon18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntity11eWeaponTypeS6_bbbbbbb", &CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);

	// audio
//	CHook::InlineHook(g_libGTASA, 0x368850, &CAudioEngine__Service_hook, &CAudioEngine__Service);
	//CHook::InlineHook(g_libGTASA, 0x35AC44, &CAEVehicleAudioEntity__GetAccelAndBrake_hook, &CAEVehicleAudioEntity__GetAccelAndBrake);

	CHook::InlineHook("_ZN10CCollision19ProcessVerticalLineERK8CColLineRK7CMatrixR9CColModelR9CColPointRfbbP15CStoredCollPoly", &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);
    //CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x66E964 : 0x83DF78), &CWeapon__GenerateDamageEvent_hook, &CWeapon__GenerateDamageEvent);

    // Fire extingusher fix
	//CHook::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);

	CHook::Redirect("_Z13Render2dStuffv", &Render2dStuff);
	CHook::InlineHook("_Z14AND_TouchEventiiii", &TouchEvent_hook, &TouchEvent);
    //
	CHook::Redirect("_ZN11CHudColours12GetIntColourEh", &CHudColours__GetIntColour); // dangerous

	//CHook::InlineHook("_Z15RwFrameAddChildP7RwFrameS0_", &RwFrameAddChild_hook, &RwFrameAddChild);
    //CHook::InlineHook("_Z27_rpMaterialListDeinitializeP14RpMaterialList", &_rpMaterialListDeinitialize_hook, &_rpMaterialListDeinitialize);
	CHook::InlineHook("_ZN19CUpsideDownCarCheck15IsCarUpsideDownEPK8CVehicle", &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
    //CHook::InlineHook("_ZN14CAnimBlendNode12FindKeyFrameEf", &CAnimBlendNode__FindKeyFrame_hook, &CAnimBlendNode__FindKeyFrame);

	// gettexture fix crash
	CHook::Redirect("_Z10GetTexturePKc", &CUtil::GetTexture);

    // GetFrameFromID fix
    CHook::InlineHook("_ZN15CClumpModelInfo14GetFrameFromIdEP7RpClumpi", &CClumpModelInfo_GetFrameFromId_hook, &CClumpModelInfo_GetFrameFromId);
	// fix
	//CHook::InlineHook("_Z19_rwFreeListFreeRealP10RwFreeListPv", &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);

	CHook::InlineHook("_ZN11CAutomobile22ProcessEntityCollisionEP7CEntityP9CColPoint", &CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);
    CHook::InlineHook("_ZN5CBike22ProcessEntityCollisionEP7CEntityP9CColPoint", &CBike__ProcessEntityCollision_hook, &CBike__ProcessEntityCollision);
    CHook::InlineHook("_ZN13CMonsterTruck22ProcessEntityCollisionEP7CEntityP9CColPoint", &CMonsterTruck__ProcessEntityCollision_hook, &CMonsterTruck__ProcessEntityCollision);
    CHook::InlineHook("_ZN8CTrailer22ProcessEntityCollisionEP7CEntityP9CColPoint", &CTrailer__ProcessEntityCollision_hook, &CTrailer__ProcessEntityCollision);

    CHook::InlineHook("_ZN14MainMenuScreen6OnExitEv", &MainMenuScreen__OnExit_hook, &MainMenuScreen__OnExit);

    // TODO: FIX
	CHook::InlineHook("_ZN11CAutomobile9PreRenderEv", &CAutomobile__PreRender_hook, &CAutomobile__PreRender);
	CHook::InlineHook("_ZN11CAutomobile17UpdateWheelMatrixEii", &CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
	CHook::InlineHook("_ZN7CMatrix6RotateEfff", &CMatrix__Rotate_hook, &CMatrix__Rotate);
	CHook::InlineHook("_ZN7CMatrix8SetScaleEfff", &CMatrix__SetScale_hook, &CMatrix__SetScale);

	//CHook::InlineHook("_ZN17CTaskSimpleUseGun17RemoveStanceAnimsEP4CPedf", &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);

	CHook::InlineHook("_ZN4CCam7ProcessEv", &CCam__Process_hook, &CCam__Process);

	CHook::InlineHook("_ZN4CPad27CycleCameraModeDownJustDownEv", &CPad__CycleCameraModeDownJustDown_hook, &CPad__CycleCameraModeDownJustDown);

	CHook::InlineHook("_ZN13FxEmitterBP_c6RenderEP8RwCamerajfh", &FxEmitterBP_c__Render_hook, &FxEmitterBP_c__Render);
	CHook::InlineHook("_ZN4CPed22ProcessEntityCollisionEP7CEntityP9CColPoint", &CPed__ProcessEntityCollision_hook, &CPed__ProcessEntityCollision);

	CHook::InlineHook("_ZN16CTaskSimpleGetUp10ProcessPedEP4CPed", &CTaskSimpleGetUp__ProcessPed_hook, &CTaskSimpleGetUp__ProcessPed); // CTaskSimpleGetUp::ProcessPed

	CHook::InlineHook("_Z23RwResourcesFreeResEntryP10RwResEntry", &RwResourcesFreeResEntry_hook, &RwResourcesFreeResEntry);

    auto* RQCaps = (RQCapabilities*)(g_libGTASA + (VER_x32 ? 0x6B8B9C : 0x896130));
    if (RQCaps->hasTextureCompressionPVRTCCap) {
        CHook::Redirect("_ZNK14TextureListing11GetMipCountEv", &TextureListing_GetMipCount);
    }

	HookCPad();
}