#include "../main.h"
#include "game/Enums/eWeaponType.h"
#include "util.h"
#include "game/Enums/OSDeviceForm.h"
#include "patch.h"

#include <vector>
#include <dirent.h>

OSDeviceForm CUtil::OS_SystemForm()
{
	return CHook::CallFunction<OSDeviceForm>(g_libGTASA + (VER_x32 ? 0x0026BAD4 + 1 : 0x32155C));
}

int CUtil::GameGetWeaponModelIDFromWeaponID(int iWeaponID)
{
	switch (iWeaponID)
	{
		case WEAPON_BRASSKNUCKLE:
			return WEAPON_MODEL_BRASSKNUCKLE;

		case WEAPON_GOLFCLUB:
			return WEAPON_MODEL_GOLFCLUB;

		case WEAPON_NIGHTSTICK:
			return WEAPON_MODEL_NITESTICK;

		case WEAPON_KNIFE:
			return WEAPON_MODEL_KNIFE;

		case WEAPON_BASEBALLBAT:
			return WEAPON_MODEL_BAT;

		case WEAPON_SHOVEL:
			return WEAPON_MODEL_SHOVEL;

		case WEAPON_POOL_CUE:
			return WEAPON_MODEL_POOLSTICK;

		case WEAPON_KATANA:
			return WEAPON_MODEL_KATANA;

		case WEAPON_CHAINSAW:
			return WEAPON_MODEL_CHAINSAW;

		case WEAPON_DILDO1:
			return WEAPON_MODEL_DILDO;

		case WEAPON_DILDO2:
			return WEAPON_MODEL_DILDO2;

		case WEAPON_VIBE1:
			return WEAPON_MODEL_VIBRATOR;

		case WEAPON_VIBE2:
			return WEAPON_MODEL_VIBRATOR2;

		case WEAPON_FLOWERS:
			return WEAPON_MODEL_FLOWER;

		case WEAPON_CANE:
			return WEAPON_MODEL_CANE;

		case WEAPON_GRENADE:
			return WEAPON_MODEL_GRENADE;

		case WEAPON_TEARGAS:
			return WEAPON_MODEL_TEARGAS;

		case WEAPON_MOLOTOV:
			return -1;

		case WEAPON_PISTOL:
			return WEAPON_MODEL_COLT45;

		case WEAPON_PISTOL_SILENCED:
			return WEAPON_MODEL_SILENCED;

		case WEAPON_DESERT_EAGLE:
			return WEAPON_MODEL_DEAGLE;

		case WEAPON_SHOTGUN:
			return WEAPON_MODEL_SHOTGUN;

		case WEAPON_SAWNOFF_SHOTGUN:
			return WEAPON_MODEL_SAWEDOFF;

		case WEAPON_SPAS12_SHOTGUN:
			return WEAPON_MODEL_SHOTGSPA;

		case WEAPON_MICRO_UZI:
			return WEAPON_MODEL_UZI;

		case WEAPON_MP5:
			return WEAPON_MODEL_MP5;

		case WEAPON_AK47:
			return WEAPON_MODEL_AK47;

		case WEAPON_M4:
			return WEAPON_MODEL_M4;

		case WEAPON_TEC9:
			return WEAPON_MODEL_TEC9;

		case WEAPON_COUNTRYRIFLE:
			return WEAPON_MODEL_RIFLE;

		case WEAPON_SNIPERRIFLE:
			return WEAPON_MODEL_SNIPER;

		case WEAPON_RLAUNCHER:
			return WEAPON_MODEL_ROCKETLAUNCHER;

		case WEAPON_RLAUNCHER_HS:
			return WEAPON_MODEL_HEATSEEKER;

		case WEAPON_FLAMETHROWER:
			return WEAPON_MODEL_FLAMETHROWER;

		case WEAPON_MINIGUN:
			return WEAPON_MODEL_MINIGUN;

		case WEAPON_REMOTE_SATCHEL_CHARGE:
			return WEAPON_MODEL_SATCHEL;

		case WEAPON_DETONATOR:
			return WEAPON_MODEL_BOMB;

		case WEAPON_SPRAYCAN:
			return WEAPON_MODEL_SPRAYCAN;

		case WEAPON_EXTINGUISHER:
			return WEAPON_MODEL_FIREEXTINGUISHER;

		case WEAPON_CAMERA:
			return WEAPON_MODEL_CAMERA;

		case -1:
			return WEAPON_MODEL_NIGHTVISION;

		case -2:
			return WEAPON_MODEL_INFRARED;

		case WEAPON_PARACHUTE:
			return WEAPON_MODEL_PARACHUTE;

	}

	return -1;
}

void __fastcall CUtil::TransformPoint(RwV3d &result, const CSimpleTransform &t, const RwV3d &v)
{
	float cos_heading = cosf(t.m_fHeading);
	float sin_heading = sinf(t.m_fHeading);

	result = {
			t.m_vPosn.x + cos_heading * v.x - sin_heading * v.y,
			t.m_vPosn.y + sin_heading * v.x + cos_heading * v.y,
			v.z + t.m_vPosn.z
	};
}

const char* CUtil::GetWeaponName(int iWeaponID)
{
	switch (iWeaponID)
	{
		case WEAPON_UNARMED:
			return "Fist";
		case WEAPON_BRASSKNUCKLE:
			return "Brass Knuckles";
		case WEAPON_GOLFCLUB:
			return "Golf Club";
		case WEAPON_NIGHTSTICK:
			return "Nite Stick";
		case WEAPON_KNIFE:
			return "Knife";
		case WEAPON_BASEBALLBAT:
			return "Baseball Bat";
		case WEAPON_SHOVEL:
			return "Shovel";
		case WEAPON_POOL_CUE:
			return "Pool Cue";
		case WEAPON_KATANA:
			return "Katana";
		case WEAPON_CHAINSAW:
			return "Chainsaw";
		case WEAPON_DILDO1:
			return "Dildo";
		case WEAPON_DILDO2:
			return "Dildo";
		case WEAPON_VIBE1:
			return "Vibrator";
		case WEAPON_VIBE2:
			return "Vibrator";
		case WEAPON_FLOWERS:
			return "Flowers";
		case WEAPON_CANE:
			return "Cane";
		case WEAPON_GRENADE:
			return "Grenade";
		case WEAPON_TEARGAS:
			return "Teargas";
		case WEAPON_MOLOTOV:
			return "Molotov";
		case WEAPON_PISTOL:
			return "Colt 45";
		case WEAPON_PISTOL_SILENCED:
			return "Silenced Pistol";
		case WEAPON_DESERT_EAGLE:
			return "Desert Eagle";
		case WEAPON_SHOTGUN:
			return "Shotgun";
		case WEAPON_SAWNOFF_SHOTGUN:
			return "Sawn-off Shotgun";
		case WEAPON_SPAS12_SHOTGUN: // wtf?
			return "Combat Shotgun";
		case WEAPON_MICRO_UZI:
			return "UZI";
		case WEAPON_MP5:
			return "MP5";
		case WEAPON_AK47:
			return "AK47";
		case WEAPON_M4:
			return "M4";
		case WEAPON_TEC9:
			return "TEC9";
		case WEAPON_COUNTRYRIFLE:
			return "Rifle";
		case WEAPON_SNIPERRIFLE:
			return "Sniper Rifle";
		case WEAPON_RLAUNCHER:
			return "Rocket Launcher";
		case WEAPON_RLAUNCHER_HS:
			return "Heat Seaker";
		case WEAPON_FLAMETHROWER:
			return "Flamethrower";
		case WEAPON_MINIGUN:
			return "Minigun";
		case WEAPON_REMOTE_SATCHEL_CHARGE:
			return "Satchel Explosives";
		case WEAPON_DETONATOR:
			return "Bomb";
		case WEAPON_SPRAYCAN:
			return "Spray Can";
		case WEAPON_EXTINGUISHER:
			return "Fire Extinguisher";
		case WEAPON_CAMERA:
			return "Camera";
		case WEAPON_PARACHUTE:
			return "Parachute";
		case WEAPON_RAMMEDBYCAR:
			return "Vehicle";

		case WEAPON_EXPLOSION:
			return "Explosion";
		case WEAPON_DROWNING:
			return "Drowned";
		case WEAPON_FALL:
			return "Collision";
		default:
			return "unknown";
	}
}

#include <dlfcn.h>
#include <link.h>
#include "../str_obfuscator.hpp"

uintptr_t CUtil::FindLib(const char* libname)
{
	void* handle = dlopen(libname, RTLD_LAZY);
	if (handle) {
		void* symbol = dlsym(handle, "JNI_OnLoad");
		if (symbol) {
			Dl_info info;
			if (dladdr(symbol, &info) != 0) {
				return reinterpret_cast<uintptr_t>(info.dli_fbase);
			}
		}
		dlclose(handle);
	}

	return 0;
}

void CrashLog(const char* fmt, ...);

#include <algorithm>

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

static const int table[128] =
		{
				// 80
				0x82D0,     0x83D0,     0x9A80E2,   0x93D1,     0x9E80E2,   0xA680E2,   0xA080E2,   0xA180E2,
				0xAC82E2,   0xB080E2,   0x89D0,     0xB980E2,   0x8AD0,     0x8CD0,     0x8BD0,     0x8FD0,
				// 90
				0x92D1,     0x9880E2,   0x9980E2,   0x9C80E2,   0x9D80E2,   0xA280E2,   0x9380E2,   0x9480E2,
				0,          0xA284E2,   0x99D1,     0xBA80E2,   0x9AD1,     0x9CD1,     0x9BD1,     0x9FD1,
				// A0
				0xA0C2,     0x8ED0,     0x9ED1,     0x88D0,     0xA4C2,     0x90D2,     0xA6C2,     0xA7C2,
				0x81D0,     0xA9C2,     0x84D0,     0xABC2,     0xACC2,     0xADC2,     0xAEC2,     0x87D0,
				// B0
				0xB0C2,     0xB1C2,     0x86D0,     0x96D1,     0x91D2,     0xB5C2,     0xB6C2,     0xB7C2,
				0x91D1,     0x9684E2,   0x94D1,     0xBBC2,     0x98D1,     0x85D0,     0x95D1,     0x97D1,
				// C0
				0x90D0,     0x91D0,     0x92D0,     0x93D0,     0x94D0,     0x95D0,     0x96D0,     0x97D0,
				0x98D0,     0x99D0,     0x9AD0,     0x9BD0,     0x9CD0,     0x9DD0,     0x9ED0,     0x9FD0,
				// D0
				0xA0D0,     0xA1D0,     0xA2D0,     0xA3D0,     0xA4D0,     0xA5D0,     0xA6D0,     0xA7D0,
				0xA8D0,     0xA9D0,     0xAAD0,     0xABD0,     0xACD0,     0xADD0,     0xAED0,     0xAFD0,
				// E0
				0xB0D0,     0xB1D0,     0xB2D0,     0xB3D0,     0xB4D0,     0xB5D0,     0xB6D0,     0xB7D0,
				0xB8D0,     0xB9D0,     0xBAD0,     0xBBD0,     0xBCD0,     0xBDD0,     0xBED0,     0xBFD0,
				// F0
				0x80D1,     0x81D1,     0x82D1,     0x83D1,     0x84D1,     0x85D1,     0x86D1,     0x87D1,
				0x88D1,     0x89D1,     0x8AD1,     0x8BD1,     0x8CD1,     0x8DD1,     0x8ED1,     0x8FD1
		};

std::string cp1251_to_utf8(const std::string& in) {
	return cp1251_to_utf8(in.c_str());
}

std::string cp1251_to_utf8(const char* in) {
	std::string result;

	while (*in) {
		if (*in & 0x80) {
			int v = table[(int)(0x7f & *in++)];
			if (!v)
				continue;
			result += (char)v;
			result += (char)(v >> 8);
			if (v >>= 16)
				result += (char)v;
		} else { // ASCII
			result += *in++;
		}
	}

	return result;
}

void cp1251_to_utf8(char* out, const char* in, unsigned int len)
{
	int count = 0;

	while (*in)
	{
		if (len && (count >= len)) break;

		if (*in & 0x80)
		{
			int v = table[(int)(0x7f & *in++)];
			if (!v)
				continue;
			*out++ = (char)v;
			*out++ = (char)(v >> 8);
			if (v >>= 16)
				* out++ = (char)v;
		}
		else // ASCII
			*out++ = *in++;

		count++;
	}

	*out = 0;
}

std::string ConvertColorToHtml(const std::string& format) {
	std::string result;
	bool first_time_flag = true;

	for (size_t i = 0; i < format.length(); i++) {
		if (format[i] == '{') {
			if (first_time_flag) {
				result += "<font color='#";
			} else {
				result += "</font><font color='#";
			}
		}
		if (format[i] == '}') {
			result += "'>";
			first_time_flag = false;
		}
		if (format[i] != '{' && format[i] != '}') {
			result += format[i];
		}
	}

	result += "</font>";

	for (size_t i = 0; i < result.size(); i++) {
		if (result[i] == '\n') {
			result[i] = '<';
			result.insert(i + 1, "br>");
			i += 3;
		}
	}

	return result;
}

typedef struct ConvLetter {
	char    win1251;
	int     unicode;
} Letter;


static Letter g_letters[] = {
		{0x82, 0x201A}, // SINGLE LOW-9 QUOTATION MARK
		{0x83, 0x0453}, // CYRILLIC SMALL LETTER GJE
		{0x84, 0x201E}, // DOUBLE LOW-9 QUOTATION MARK
		{0x85, 0x2026}, // HORIZONTAL ELLIPSIS
		{0x86, 0x2020}, // DAGGER
		{0x87, 0x2021}, // DOUBLE DAGGER
		{0x88, 0x20AC}, // EURO SIGN
		{0x89, 0x2030}, // PER MILLE SIGN
		{0x8A, 0x0409}, // CYRILLIC CAPITAL LETTER LJE
		{0x8B, 0x2039}, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
		{0x8C, 0x040A}, // CYRILLIC CAPITAL LETTER NJE
		{0x8D, 0x040C}, // CYRILLIC CAPITAL LETTER KJE
		{0x8E, 0x040B}, // CYRILLIC CAPITAL LETTER TSHE
		{0x8F, 0x040F}, // CYRILLIC CAPITAL LETTER DZHE
		{0x90, 0x0452}, // CYRILLIC SMALL LETTER DJE
		{0x91, 0x2018}, // LEFT SINGLE QUOTATION MARK
		{0x92, 0x2019}, // RIGHT SINGLE QUOTATION MARK
		{0x93, 0x201C}, // LEFT DOUBLE QUOTATION MARK
		{0x94, 0x201D}, // RIGHT DOUBLE QUOTATION MARK
		{0x95, 0x2022}, // BULLET
		{0x96, 0x2013}, // EN DASH
		{0x97, 0x2014}, // EM DASH
		{0x99, 0x2122}, // TRADE MARK SIGN
		{0x9A, 0x0459}, // CYRILLIC SMALL LETTER LJE
		{0x9B, 0x203A}, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
		{0x9C, 0x045A}, // CYRILLIC SMALL LETTER NJE
		{0x9D, 0x045C}, // CYRILLIC SMALL LETTER KJE
		{0x9E, 0x045B}, // CYRILLIC SMALL LETTER TSHE
		{0x9F, 0x045F}, // CYRILLIC SMALL LETTER DZHE
		{0xA0, 0x00A0}, // NO-BREAK SPACE
		{0xA1, 0x040E}, // CYRILLIC CAPITAL LETTER SHORT U
		{0xA2, 0x045E}, // CYRILLIC SMALL LETTER SHORT U
		{0xA3, 0x0408}, // CYRILLIC CAPITAL LETTER JE
		{0xA4, 0x00A4}, // CURRENCY SIGN
		{0xA5, 0x0490}, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
		{0xA6, 0x00A6}, // BROKEN BAR
		{0xA7, 0x00A7}, // SECTION SIGN
		{0xA8, 0x0401}, // CYRILLIC CAPITAL LETTER IO
		{0xA9, 0x00A9}, // COPYRIGHT SIGN
		{0xAA, 0x0404}, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
		{0xAB, 0x00AB}, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		{0xAC, 0x00AC}, // NOT SIGN
		{0xAD, 0x00AD}, // SOFT HYPHEN
		{0xAE, 0x00AE}, // REGISTERED SIGN
		{0xAF, 0x0407}, // CYRILLIC CAPITAL LETTER YI
		{0xB0, 0x00B0}, // DEGREE SIGN
		{0xB1, 0x00B1}, // PLUS-MINUS SIGN
		{0xB2, 0x0406}, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
		{0xB3, 0x0456}, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
		{0xB4, 0x0491}, // CYRILLIC SMALL LETTER GHE WITH UPTURN
		{0xB5, 0x00B5}, // MICRO SIGN
		{0xB6, 0x00B6}, // PILCROW SIGN
		{0xB7, 0x00B7}, // MIDDLE DOT
		{0xB8, 0x0451}, // CYRILLIC SMALL LETTER IO
		{0xB9, 0x2116}, // NUMERO SIGN
		{0xBA, 0x0454}, // CYRILLIC SMALL LETTER UKRAINIAN IE
		{0xBB, 0x00BB}, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		{0xBC, 0x0458}, // CYRILLIC SMALL LETTER JE
		{0xBD, 0x0405}, // CYRILLIC CAPITAL LETTER DZE
		{0xBE, 0x0455}, // CYRILLIC SMALL LETTER DZE
		{0xBF, 0x0457} // CYRILLIC SMALL LETTER YI
};

void CUtil::ConvertARGBtoRGBA(const uint32_t* src, uint8_t* dst, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        CRGBA col;
        col.FromARGB(src[i]);
        size_t off = i * 4;
        dst[off + 0] = col.r;
        dst[off + 1] = col.g;
        dst[off + 2] = col.b;
        dst[off + 3] = col.a;
    }
}