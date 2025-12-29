
#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>

#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "chatwindow.h"
#include "playertags.h"
#include "keyboard.h"
#include "CSettings.h"
#include "java_systems/HUD.h"
#include "CLoader.h"
#include "crashlytics.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "str_obfuscator.hpp"

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
char* g_pszStorage = nullptr;
char* g_pszRootStorage = nullptr;

#include "CLocalisation.h"
#include "java_systems/HUD.h"
#include "java_systems/Inventory.h"
#include "util/CStackTrace.h"

void CrashLog(const char* fmt, ...);
bool g_bIsTestMode = false;
CNetGame *pNetGame = nullptr;

CGUI *pGUI = nullptr;

void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();

extern uint16_t g_usLastProcessedModelIndexAutomobile;
extern int g_iLastProcessedModelIndexAutoEnt;

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;

extern char g_bufRenderQueueCommand[200];
extern uintptr_t g_dwRenderQueueOffset;
extern char lastFile[123];
extern int g_iLastRenderedObject;
extern int lastNvEvent;
extern CVector lastPos;
char g_iLastBlock[123];
char streamimgState[123];

void PrintBuildCrashInfo()
{
	std::time_t currentTime = std::time(nullptr);
	std::tm* timeInfo = std::localtime(&currentTime);

	CrashLog("Crash time: %d:%d:%d %d:%d:%d", timeInfo->tm_mday, timeInfo->tm_mon, timeInfo->tm_year, timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
	CrashLog("Build times: %s %s. Type: %s", __TIME__, __DATE__, (VER_x32 ? "x32" : "x64"));
	CrashLog("Last processed auto and entity: %d %d", g_usLastProcessedModelIndexAutomobile, g_iLastProcessedModelIndexAutoEnt);
	CrashLog("Last processed skin and entity: %d %d", g_iLastProcessedSkinCollision, g_iLastProcessedEntityCollision);
	CrashLog("Last rendered object: %d", g_iLastRenderedObject);
	CrashLog("Last texture: %s", g_iLastBlock);
	CrashLog("Last file: %s", lastFile);
	CrashLog("Last pos: %.2f, %.2f, %.2f", lastPos.x, lastPos.y, lastPos.z);
	CrashLog("Last nvEvent: %d", lastNvEvent);
    CrashLog("%s", streamimgState);
}

#include <sstream>
#include "vendor/bass/bass.h"
#include "util/CJavaWrapper.h"
#include "CDebugInfo.h"
#include "voice/Plugin.h"

void InitInMenu()
{
    CGame::Init();
    CGame::InitInMenu();

	pGUI = new CGUI();
	CKeyBoard::init();
	CPlayerTags::Init();
}

#include <unistd.h> // system api
#include <sys/mman.h>
#include <cassert> // assert()
#include <dlfcn.h> // dlopen
#include "voice/Playback.h"

void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;
	if (!bGameInited)
	{
		CGame::InitInGame();
		CGame::SetMaxStats();

        Voice::Playback::Init(); // bass init

        // TODO: VoiceChat
		/*Voice::CVoicePlugin::OnPluginLoad();
		Voice::CVoicePlugin::OnSampLoad();

		// voice
		Log("[dbg:samp:load] : module loading...");

		for (const auto& loadCallback : Samp::loadCallbacks) {
			if (loadCallback != nullptr) {
				loadCallback();
			}
		}

		Samp::loadStatus = true;

		// voice
		for (const auto& deviceInitCallback : CVoiceRender::deviceInitCallbacks) {
			if (deviceInitCallback != nullptr) {
				deviceInitCallback();
			}
		}

		Log("[dbg:samp:load] : module loaded");*/

		g_pJavaWrapper->hideLoadingScreen();

		CHUD::toggleServerLogo(true);
		bGameInited = true;

		return;
	}

	if (!bNetworkInited)
	{
		// TODO: obfuscate ip & port
        pNetGame = new CNetGame(
                "185.189.255.97",
                2630,
                CSettings::Get().szNickName,
                CSettings::Get().szPassword
        );

        /*pNetGame = new CNetGame(
                "94.23.168.153",
                1275,
                CSettings::Get().szNickName,
                CSettings::Get().szPassword
        );*/

		bNetworkInited = true;
		Log("InitInGame() end");
		return;
	}
}

#include "CDebugInfo.h"
#include "java_systems/SnapShotsWrapper.h"

void MainLoop()
{
	if(CGame::bIsGameExiting)return;

	InitInGame();

	if(pNetGame) pNetGame->Process();
}

struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;

void handler3(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
	{
		act3_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGBUS)
	{
		PrintBuildCrashInfo();

		CrashLog("SIGBUS | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}

}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
	{
		act_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGSEGV)
	{
		CrashLog(" ");
		PrintBuildCrashInfo();

		CrashLog("SIGSEGV | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}
}

void handler2(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
	{
		act2_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGFPE)
	{
		PrintBuildCrashInfo();

		CrashLog("SIGFPE | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();

	}

}

void handler1(int signum, siginfo_t* info, void* contextPtr)
{
	auto context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
	{
		act1_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGABRT)
	{
		CrashLog(" ");
		PrintBuildCrashInfo();

		CrashLog("SIGABRT | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}

}

extern "C"
{
	JavaVM* javaVM = nullptr;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}

#include "util/patch.h"
#include "CLoader.h"

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	javaVM = vm;

	g_libGTASA = CUtil::FindLib("libGTASA.so");

	if(g_libGTASA == 0)
	{
		Log("ERROR: libGTASA.so address not found!");
		return 0;
	}
	Log("libGTASA.so image base address: 0x%X", g_libGTASA);

	g_libSAMP = CUtil::FindLib("libmultiplayer.so");
	if(g_libSAMP == 0)
	{
		Log("ERROR: libsamp.so address not found!");
		return 0;
	}
	Log("libsamp.so image base address: 0x%X", g_libSAMP);

    // anti-cheat by plaka penka xD
    CRYPTEDSTRING(libstr, "libgvraudio.so");
	CRYPTEDSTRING(testtest1111, "libradio.so");
    if(dlopen(libstr.decode().c_str(), RTLD_LAZY) || dlopen(testtest1111.decode().c_str(), RTLD_LAZY))
    {
        exit(0);
    }

	CLoader::initJavaClasses(vm);

	CHook::InitHookStuff();

	InstallSpecialHooks();
	InitRenderWareFunctions();

	ApplyPatches_level0();

	struct sigaction act{};
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, &act_old);

	struct sigaction act1{};
	act1.sa_sigaction = handler1;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &act1, &act1_old);

	struct sigaction act2{};
	act2.sa_sigaction = handler2;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGFPE, &act2, &act2_old);

	struct sigaction act3{};
	act3.sa_sigaction = handler3;
	sigemptyset(&act3.sa_mask);
	act3.sa_flags = SA_SIGINFO;
	sigaction(SIGBUS, &act3, &act3_old);

	return JNI_VERSION_1_6;
}

void Log(const char *fmt, ...)
{
	static char buffer[512] {};

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	firebase::crashlytics::Log(buffer);
	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

#if USE_FILE_LOG
	//if(pDebug) pDebug->AddMessage(buffer);
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%slog.txt", g_pszStorage);
		flLog = fopen(buffer, "ab");
	}

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);
#endif
}

void CrashLog(const char* fmt, ...)
{
	static char buffer[512] {};
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

#ifdef NDEBUG
	firebase::crashlytics::Log(buffer);
#endif
	__android_log_write(ANDROID_LOG_FATAL, "AXL", buffer);

#if USE_FILE_LOG
	static FILE* flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%scrash_log.txt", g_pszStorage);
		flLog = fopen(buffer, "ab");
	}

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);
#endif
}

uint32_t GetTickCount()
{
    return CTimer::m_snTimeInMillisecondsNonClipped;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_core_Samp_initSAMP(JNIEnv *env, jobject thiz, jfloat maxFps, jstring directory) {
    Log("Initializing SAMP..");

    const char *dirChars = env->GetStringUTFChars(directory, nullptr);
    if (g_pszRootStorage != nullptr) {
        delete[] g_pszRootStorage;
    }
    g_pszRootStorage = new char[strlen(dirChars) + 1];
    strcpy(g_pszRootStorage, dirChars);

    env->ReleaseStringUTFChars(directory, dirChars);

	CSettings::maxFps = (int)maxFps;
    g_pJavaWrapper = new CJavaWrapper(env, thiz);
}
extern bool ProcessLocalCommands(const char str[]);

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_core_Samp_00024Companion_sendCommand(JNIEnv *env, jobject clazz, jstring command) {
	const char *_command = env->GetStringUTFChars(command, nullptr);

	if(!ProcessLocalCommands(_command))
		pNetGame->SendChatCommand(_command);

	env->ReleaseStringUTFChars(command, _command);
}