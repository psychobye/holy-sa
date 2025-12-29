#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include "game/common.h"
#include "advobfuscator/string.h"

using namespace andrivet::advobfuscator;

#define SAMP_VERSION	"0.3.7"
#define PORT_VERSION	"0.69"

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/raknet/SAMP/samp_netencr.h"
#include "vendor/raknet/SAMP/SAMPRPC.h"

#include "util/util.h"

extern uintptr_t g_libGTASA;
extern char* g_pszStorage;
extern char* g_pszRootStorage;

void LogLong(const char* message);
void Log(const char *fmt, ...);

#ifdef NDEBUG
#define DLOG(...)
#else
#define DLOG(...) __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", __VA_ARGS__)
#endif

#define LOGRPC true

#if LOGRPC and !NDEBUG
#define LOGRPC(...) __android_log_print(ANDROID_LOG_INFO, "RPC called", __VA_ARGS__)
#else
#define LOGRPC(...)
#endif

uint32_t GetTickCount();