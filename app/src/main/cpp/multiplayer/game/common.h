#pragma once

#include <cinttypes>

#define VER_2_1 	    true
#define VER_SAMP 	    true
#define VER_LR		    true
#define USE_FILE_LOG    true

// vc
#define USE_CUTSCENE_SHADOW_FOR_PED

#define ASSERT assert
#define nil nullptr
#define SQR(x) ((x) * (x))

typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;
typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;
typedef intptr_t  intptr;
typedef uintptr_t uintptr;

typedef uint8     bool8;
typedef uint16    bool16;
typedef uint32    bool32;

#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)

VALIDATE_SIZE(bool, 1);
VALIDATE_SIZE(char, 1);
VALIDATE_SIZE(short, 2);
VALIDATE_SIZE(int, 4);
VALIDATE_SIZE(float, 4);
VALIDATE_SIZE(long long, 8);

#include <string>
#include <span>
#include <array>
#include <cassert>
#include "game/RW/RenderWare.h"
#include "Core/Matrix.h"

#include "rgba.h"
#include "tHandlingData.h"
#include "SimpleTransform.h"
#include "General.h"
#include "MemoryMgr.h"
#include "game/Enums/eBulletSyncHitType.h"
#include <chrono>
#include "gui/gui.h"
#include "tools/TristateCheckbox.h"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24



#include <android/log.h>

/*!
* @brief Used for static variable references
*
* @tparam T    The type of the variable
* @param Addr  The address of it
*/
template<typename T>
T& StaticRef(uintptr_t addr) {
	return *reinterpret_cast<T*>(addr);
}

// TODO: Replace this with the one above
template<typename T, uintptr_t Addr>
T& StaticRef() {
	return StaticRef<T>(Addr);
}

constexpr auto DEFAULT_SCREEN_WIDTH       = 640;
constexpr auto DEFAULT_SCREEN_HEIGHT      = 448;
constexpr auto DEFAULT_SCREEN_HEIGHT_PAL  = 512.0f;
constexpr auto DEFAULT_SCREEN_HEIGHT_NTSC = 448.0f;
constexpr auto DEFAULT_ASPECT_RATIO       = 4.0f / 3.0f;
constexpr auto DEFAULT_VIEW_WINDOW        = 0.7f;

namespace notsa {
	namespace detail {
		static void VerifyMacroImpl(bool result) {
			assert(result); // In release mode this won't do anything
		}
	};

    /*!
    * @arg value The value to search for in the range
    *
    * @brief Check if a range contains a value, uses `rng::find`. NOTE: If you plan on using the iterator, just use `rng::find` instead..
    */
    template<std::ranges::input_range R, class T = std::ranges::range_value_t<R>, class Proj = std::identity>
    requires std::indirect_binary_predicate<std::ranges::equal_to, std::projected<std::ranges::iterator_t<R>, Proj>, const T*>
    bool contains(R&& r, const T& value, Proj proj = {}) {
        return std::ranges::find(r, value, proj) != std::ranges::end(r);
    }


    /*!
    * Helper (Of your fingers) - Reduces typing needed for Python style `value in {}`
    */
    template<typename Y>
    bool contains(std::initializer_list<Y> r, const Y& value) {
        return contains(r, value, {});
    }
};
//! Macro for passing a string var to *scanf_s function.
#define VERIFY notsa::detail::VerifyMacroImpl
#define VERIFY_TODO_FIX(_expr) (_expr) // Macro used to mark shit that uses `VERIFY and sometimes fails


#ifdef NDEBUG
#define DLOG(...)
#else
#define DLOG(...) __android_log_write(ANDROID_LOG_DEBUG,"LOG_TAG",__VA_ARGS__)
#endif

typedef unsigned char byte;
// Macro for unused function arguments - Use it to avoid compiler warnings of unused arguments
#define UNUSED(x) (void)(x);

constexpr float E              = 2.71828f;          // e
constexpr float E_CONST        = 0.577f;            // Euler-Mascheroni constant
constexpr float FRAC_1_TAU     = 0.159154f;         // 1 / τ
constexpr float FRAC_1_PI      = 0.318309f;         // 1 / π
constexpr float FRAC_2_TAU     = 0.318309f;         // 2 / τ
constexpr float FRAC_2_PI      = 0.636619f;         // 2 / π
constexpr float FRAC_2_SQRT_PI = 1.12837f;          // 2 / √π
constexpr float FRAC_4_TAU     = 0.636619f;         // 4 / τ
constexpr float FRAC_1_SQRT_2  = 0.707106f;         // 1 / √2
constexpr float FRAC_PI_2      = 1.57079f;          // π / 2
constexpr float FRAC_PI_3      = 1.04719f;          // π / 3
constexpr float FRAC_PI_4      = 0.785398f;         // π / 4
constexpr float FRAC_PI_6      = 0.523598f;         // π / 6
constexpr float FRAC_PI_8      = 0.392699f;         // π / 8
constexpr float FRAC_TAU_2     = 3.14159f;          // τ / 2 = π
constexpr float FRAC_TAU_3     = 2.09439f;          // τ / 3
constexpr float FRAC_TAU_4     = 1.57079f;          // τ / 4
constexpr float FRAC_TAU_6     = 1.04719f;          // τ / 6
constexpr float FRAC_TAU_8     = 0.785398f;         // τ / 8
constexpr float FRAC_TAU_12    = 0.523598f;         // τ / 12
constexpr float LN_2           = 0.693147f;         // ln(2)
constexpr float LN_10          = 2.30258f;          // ln(10)
constexpr float LOG2_E         = 1.44269f;          // log2(e)
constexpr float LOG10_E        = 0.434294f;         // log10(e)
constexpr float LOG10_2        = 0.301029f;         // log10(2)
constexpr float LOG2_10        = 3.32192f;          // log2(10)
constexpr float PI             = 3.14159f;          // π
constexpr float HALF_PI        = PI / 2.0f;         // π / 2
constexpr float PI_6           = PI / 6.0f;         // π / 6
constexpr float SQRT_2         = 1.41421f;          // √2
constexpr float SQRT_3         = 1.73205f;          // √3
constexpr float TWO_PI         = 6.28318f;          // τ (TAU)

constexpr float COS_45 = SQRT_2; // cos(45deg)

constexpr float sq(float x) { return x * x; }

#define HUD_ELEMENT_MAP     1
#define HUD_ELEMENT_TAGS    2
#define HUD_ELEMENT_BUTTONS 3
#define HUD_ELEMENT_HUD     4
#define HUD_ELEMENT_VOICE	5
#define HUD_ELEMENT_TEXTLABELS	6
#define HUD_ELEMENT_FPS		7


#define HUD_MAX	8

#define PLAYER_PED_SLOTS	120
#define MAX_TEXT_DRAWS	3072
#define MAX_STREAMS		1000
#define MAX_MATERIALS		16
#define MAX_REMOVE_MODELS	1200
#define MAX_WEAPON_MODELS 47
#define MAX_WEAPONS_SLOT 13

typedef uint16 VEHICLEID;
typedef uint16 PLAYERID;
typedef uint16 ActorId;

#define PADDING(x,y) uint8_t x[y]

#define BIT_SET(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define BIT_CLEAR(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define BIT_CHECK(byte,nbit) ((byte) &   (1<<(nbit)))

// game uses maximumWidth/Height, but this probably won't work
// with RW windowed mode
#define SCREEN_WIDTH ((float)RsGlobal->maximumWidth)
#define SCREEN_HEIGHT ((float)RsGlobal->maximumHeight)
#define SCREEN_ASPECT_RATIO (CDraw::ms_fAspectRatio)
#define SCREEN_VIEW_WINDOW (std::tan(DegreesToRadians(CDraw::GetFOV() / (2.0f)))) // todo: GetScaledFov

// This scales from PS2 pixel coordinates to the real resolution
inline float SCREEN_STRETCH_X(float a)           { return a * SCREEN_WIDTH  / (float)DEFAULT_SCREEN_WIDTH; } // RsGlobal.maximumWidth * 0.0015625 * value
inline float SCREEN_STRETCH_Y(float a)           { return a * SCREEN_HEIGHT / (float)DEFAULT_SCREEN_HEIGHT; }
inline float SCREEN_STRETCH_FROM_RIGHT(float a)  { return SCREEN_WIDTH  - SCREEN_STRETCH_X(a); }
inline float SCREEN_STRETCH_FROM_BOTTOM(float a) { return SCREEN_HEIGHT - SCREEN_STRETCH_Y(a); }

#define START_PROFILE	auto start_time = std::chrono::steady_clock::now();
#define END_PROFILE auto end_time = std::chrono::steady_clock::now(); \
auto elapsed_time_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time); \
auto elapsed_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); \
Log("PROFILE time micro sec = %lld, milli = %lld", elapsed_time_microseconds.count(), elapsed_time_milliseconds.count());

// Use this to add const that wasn't there in the original code
#define Const const


enum eLights :	uint8_t
{
	// these have to correspond to their respective panels
	LEFT_HEADLIGHT = 0,
	RIGHT_HEADLIGHT,
	LEFT_TAIL_LIGHT,
	RIGHT_TAIL_LIGHT,
	/*  LEFT_BRAKE_LIGHT,
		RIGHT_BRAKE_LIGHT,
		FRONT_LEFT_INDICATOR,
		FRONT_RIGHT_INDICATOR,
		REAR_LEFT_INDICATOR,
		REAR_RIGHT_INDICATOR,*/

	MAX_LIGHTS            // MUST BE 16 OR LESS
};

#define ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED     2

struct CAttachedPlayerObject
{
	uint32_t 	    dwModelId;
	uint32_t 	    dwBone;
	CVector 	    vecOffset;
	CVector 	    vecRotation;
	CVector 	    vecScale;
	CRGBA 	        dwColor[2];

	class CObjectSamp*  pObject;
	uint32_t 	    dwSampBone;
};

#pragma pack(push, 1)
struct ATTACHED_OBJECT_INFO
{
	uint32_t 	dwModelId;
	uint32_t 	dwBoneId_MP;
	CVector 	vecOffset;
	CVector 	vecRotation;
	CVector 	vecScale;
	uint32_t 	dwColor[2];
};
#pragma pack(pop)
VALIDATE_SIZE(ATTACHED_OBJECT_INFO, 52);
//-----------------------------------------------------------

typedef struct _RES_ENTRY_OBJ
{
	PADDING(_pad0, 48); 	// 0-48
	uintptr_t validate; 	//48-52
	PADDING(_pad1, 4); 		//52-56
} RES_ENTRY_OBJ;
static_assert(sizeof(_RES_ENTRY_OBJ) == (VER_x32 ? 56 : 64)); // FIXME?

#pragma pack(push, 1)
struct AIM_SYNC_DATA
{
	uint8_t		byteCamMode;
	CVector 	vecAimf;
	CVector 	vecAimPos;
	float 		fAimZ;
	uint8_t 	byteCamExtZoom : 6;
	uint8_t 	byteWeaponState : 2;
	uint8_t 	aspect_ratio;
#if VER_LR
	uint8 		m_bKeyboardOpened;
#endif
};
#pragma pack(pop)
VALIDATE_SIZE(AIM_SYNC_DATA, 32);

#pragma pack(push, 1)
struct BULLET_SYNC
{
	eBulletSyncHitType 	byteHitType;			// +0
	uint16_t 			hitId;				// +1
	CVector 			vecOrigin;				// +3
	CVector 			vecHit;					// +12
	CVector 			vecOffset;				// +20
	uint8_t 			byteWeaponID;			// +28
};
#pragma pack(pop)
VALIDATE_SIZE(BULLET_SYNC, 40);

struct CEntity;

struct BULLET_DATA {
	CVector 	vecOrigin;
	CVector 	vecPos;
	CVector 	vecOffset;
	CEntity* 	pEntity;
};

//-----------------------------------------------------------

#define	VEHICLE_SUBTYPE_CAR				1
#define	VEHICLE_SUBTYPE_BIKE			2
#define	VEHICLE_SUBTYPE_HELI			3
#define	VEHICLE_SUBTYPE_BOAT			4
#define	VEHICLE_SUBTYPE_PLANE			5
#define	VEHICLE_SUBTYPE_PUSHBIKE		6
#define	VEHICLE_SUBTYPE_TRAIN			7

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO			538
#define TRAIN_FREIGHT_LOCO				537
#define TRAIN_PASSENGER					570
#define TRAIN_FREIGHT					569
#define TRAIN_TRAM						449
#define HYDRA							520

//-----------------------------------------------------------

#define ACTION_WASTED					55
#define ACTION_DEATH					54
#define ACTION_INCAR					50
#define ACTION_NORMAL					1
#define ACTION_SCOPE					12
#define ACTION_NONE						0 

//-----------------------------------------------------------

#define WEAPON_MODEL_BRASSKNUCKLE		331 // was 332
#define WEAPON_MODEL_GOLFCLUB			333
#define WEAPON_MODEL_NITESTICK			334
#define WEAPON_MODEL_KNIFE				335
#define WEAPON_MODEL_BAT				336
#define WEAPON_MODEL_SHOVEL				337
#define WEAPON_MODEL_POOLSTICK			338
#define WEAPON_MODEL_KATANA				339
#define WEAPON_MODEL_CHAINSAW			341
#define WEAPON_MODEL_DILDO				321
#define WEAPON_MODEL_DILDO2				322
#define WEAPON_MODEL_VIBRATOR			323
#define WEAPON_MODEL_VIBRATOR2			324
#define WEAPON_MODEL_FLOWER				325
#define WEAPON_MODEL_CANE				326
#define WEAPON_MODEL_GRENADE			342 // was 327
#define WEAPON_MODEL_TEARGAS			343 // was 328
#define WEAPON_MODEL_MOLOTOV			344 // was 329
#define WEAPON_MODEL_COLT45				346
#define WEAPON_MODEL_SILENCED			347
#define WEAPON_MODEL_DEAGLE				348
#define WEAPON_MODEL_SHOTGUN			349
#define WEAPON_MODEL_SAWEDOFF			350
#define WEAPON_MODEL_SHOTGSPA			351
#define WEAPON_MODEL_UZI				352
#define WEAPON_MODEL_MP5				353
#define WEAPON_MODEL_AK47				355
#define WEAPON_MODEL_M4					356
#define WEAPON_MODEL_TEC9				372
#define WEAPON_MODEL_RIFLE				357
#define WEAPON_MODEL_SNIPER				358
#define WEAPON_MODEL_ROCKETLAUNCHER		359
#define WEAPON_MODEL_HEATSEEKER			360
#define WEAPON_MODEL_FLAMETHROWER		361
#define WEAPON_MODEL_MINIGUN			362
#define WEAPON_MODEL_SATCHEL			363
#define WEAPON_MODEL_BOMB				364
#define WEAPON_MODEL_SPRAYCAN			365
#define WEAPON_MODEL_FIREEXTINGUISHER	366
#define WEAPON_MODEL_CAMERA				367
#define WEAPON_MODEL_NIGHTVISION		368	// newly added
#define WEAPON_MODEL_INFRARED			369	// newly added
#define WEAPON_MODEL_JETPACK			370	// newly added
#define WEAPON_MODEL_PARACHUTE			371
#define WEAPON_MODEL_PARACHUTE			371

#define DEGTORAD(x) ((x) * PI / 180.0f)
#define RADTODEG(x) ((x) * 180.0f / PI)


template <typename T>
T clamp(T value, T low, T high)
{
	return std::min(std::max(value, low), high);
}

template<typename T>
auto lerp(const T& from, const T& to, float t) {
	return to * t + from * (1.f - t);
}

inline const float invLerp(float fMin, float fMax, float fVal) {
	return (fVal - fMin) / (fMax - fMin);
}

// 0x4EEA80 - And inlined helluvalot
inline bool approxEqual(float f1, float f2, float epsilon) {
	return fabs(f1 - f2) < epsilon;
}

// Used in some audio functions, mostly CAESmoothFadeThread
inline bool approxEqual2(float f1, float f2, float epsilon = 0.01F)
{
	return f1 == f2 || fabs(f1 - f2) < epsilon;
}

// Converts radians to degrees
// 57.295826
constexpr float RadiansToDegrees(float angleInRadians) {
	return angleInRadians * 180.0F / PI;
}

// Converts degrees to radians
// keywords: 0.017453292 flt_8595EC
constexpr float DegreesToRadians(float angleInDegrees) {
	return angleInDegrees * PI / 180.0F;
}


namespace notsa {
namespace ui {

struct ScopedWindow {
			ScopedWindow(const char* name, ImVec2 defaultSize, bool& open, ImGuiWindowFlags flags = 0) :
					m_needsEnd{open}
			{
				if (open) {
					ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
					ImGui::Begin(name, &open, flags);
				}
			}

			~ScopedWindow() {
				if (m_needsEnd) {
					ImGui::End();
				}
			}

		private:
			bool m_needsEnd{};
		};

		struct ScopedChild {
			template<typename... Ts>
			ScopedChild(Ts&&... args) {
				ImGui::BeginChild(std::forward<Ts>(args)...);
			}

			~ScopedChild() { ImGui::EndChild(); }
		};

		template<typename T>
		struct ScopedID {
			ScopedID(T id) { ImGui::PushID(id); }
			~ScopedID() { ImGui::PopID(); }
		};

		struct ScopedDisable {
			ScopedDisable(bool disable) { ImGui::BeginDisabled(disable); }
			~ScopedDisable()            { ImGui::EndDisabled(); }
		};

	//! Render a nested menu (A series of `BeginMenu` calls). If all `BeginMenu` calls return `true` the provided `OnAllVisibleFn` is called.
	template<typename R, typename T>
	inline void DoNestedMenu(R&& menuPath, T OnAllVisibleFn) {
		assert(std::size(menuPath) > 0); // Empty makes no sense

		int32 nopen{};
		for (auto name : menuPath) {
			if (!ImGui::BeginMenu(name)) {
				break;
			}
			nopen++;
		}
		if (nopen == std::size(menuPath)) {
			std::invoke(OnAllVisibleFn);
		}
		while (nopen--) {
			ImGui::EndMenu();
		}
	}



	//! Initializer list version of `DoNestedMenu` (So no ugly `std::to_array` has to be used)
template<typename T>
inline void DoNestedMenuIL(std::initializer_list<const char*> menuPath, T OnAllVisibleFn) {
	DoNestedMenu(menuPath, OnAllVisibleFn);
}

/*!
* A tree-node with a tri-state checkbox on the left.
* Make sure to always call `TreePop();` after it!
* @param label      Label of the tree node
* @param cbTriState CheckBox's TriState
* @param cbDisabled Whenever the checkbox of the tree-node is cbDisabled [To make treenode disabled too, use `ScopedDisable`]
* @returns Tuple: [tree node open, checkbox state changed, checkbox state]
*/
inline auto TreeNodeWithTriStateCheckBox(const char* label, ImGui::ImTristate cbTriState, bool cbDisabled, ImGuiTreeNodeFlags treeNodeFlags = 0) {
	// TODO/NOTE: The Tree's label is a workaround for when the label is shorter than the visual checkbox (otherwise the checkbox can't be clic
	const auto open = ImGui::TreeNodeEx("##         ", treeNodeFlags | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth);

	ImGui::SameLine();

	// Tree is never cbDisabled, otherwise it couldn't be opened
	ScopedDisable disabler{ cbDisabled };

	// Checkbox + it's label will be the tree name
	bool cbState{};
	const auto stateChanged = ImGui::CheckboxTristate(label, cbTriState, cbState);

	return std::make_tuple(open, stateChanged, cbState);
};
}; // namespace ui
}; // namespace notsa

inline void LittleTest() {
	((void(__cdecl *)())(g_libGTASA + (VER_x32 ? 0x3FCAF0 + 1 : 0x4E0224)))();
}