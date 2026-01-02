#include "../main.h"
#include "gui.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../keyboard.h"
#include "../CSettings.h"
#include "../util/CJavaWrapper.h"
#include "../util/util.h"
#include "../game/VehicleSamp.h"
#include "../game/Widgets/WidgetGta.h"
#include "../game/Entity/Ped/Ped.h"
#include "../java_systems/HUD.h"
#include "game/Widgets/TouchInterface.h"

extern CNetGame *pNetGame;
extern CJavaWrapper *g_pJavaWrapper;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

CGUI::CGUI()
{
	Log("Initializing GUI..");

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;

	m_vecScreenCenter.x = io.DisplaySize.x / 2;
	m_vecScreenCenter.y = io.DisplaySize.y / 2;
	// font Size
	m_fFontSize = ScaleY( CSettings::Get().fFontSize );

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsLiveRussia();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, CSettings::m_Settings.szFont);
	// cp1251 ranges
	static const ImWchar ranges[] = 
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};
	Log("GUI | Loading font: %s", CSettings::m_Settings.szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log("GUI | ImFont pointer = 0x%X", m_pFont);
}

void CGUI::RenderMechanicHook() {
	// Тягач механников
	auto pPed = CGame::FindPlayerPed();
	if(!pPed)return;
	auto pVeh = pPed->GetCurrentGtaVehicle();

	auto needDrawTruckButt = pPed->m_pPed->IsADriver() && pVeh->m_nModelIndex == 525;
	if(!needDrawTruckButt) return;

	ImVec2 butSize = ImVec2(150, 150);
	ImVec2 butPos = GetPercent(85, 64, butSize);

	ImGui::SetNextWindowPos(butPos);
	ImGui::SetNextWindowSize(butSize);

	ImGui::Begin("extra_keys", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground );
	{
		ImGui::SetCursorPos(butPos);

		if (ImGui::ImageButton((ImTextureID) m_pIconTruckTex->raster, butSize)) {
			RakNet::BitStream bsSend;
			bsSend.Write((uint8_t) ID_CUSTOM_RPC);
			bsSend.Write((uint8_t) RPC_MECHANICK_HOOK);
			pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE, 0);
		}

	}
	ImGui::End();
}

void CGUI::RenderPassengerButton() {
	if(!CHUD::NeededRenderPassengerButton())
		return;

	auto& driverButton = CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_ENTER_CAR];
	float driverButtonSize = driverButton->m_RectScreen.right - driverButton->m_RectScreen.left;

	ImVec2 butSize = ImVec2(driverButtonSize, driverButtonSize);
	ImVec2 butPos = ImVec2(driverButton->m_RectScreen.left - driverButtonSize - 10, driverButton->m_RectScreen.bottom);

	ImGui::SetNextWindowPos(butPos);
	ImGui::SetNextWindowSize(butSize);

	ImGui::Begin("extra_keys", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground );
	{
		ImGui::SetCursorPos(butPos);

		if (ImGui::ImageButton((ImTextureID) m_pPassengerButtonTex->raster, butSize)) {
            CLocalPlayer::GoEnterVehicle(true);
		}

	}
	ImGui::End();
}

ImFont* CGUI::LoadFont(char *font, float fontsize)
{
	ImGuiIO &io = ImGui::GetIO();

	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/%s", g_pszStorage, font);

	// ranges
	static const ImWchar ranges[] = 
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E7F, // Thai
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
		0
	};
	
	ImFont* pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	return pFont;
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}
#include "../java_systems/HUD.h"

#include "..//CDebugInfo.h"
#include "java_systems/Dialog.h"
#include "java_systems/Speedometr.h"
#include "game/Timer.h"
#include "imgui/imgui_internal.h"
#include "java_systems/SelectEntity.h"
#include "GiftNotify.h"
#include "VehiclePipeMenu.h"
#include "voice/SpeakerList.h"
#include "voice/include/util/Render.h"
#include "tools/DebugModules.h"
#include "GPS.h"

void RenderBackgroundHud();
void RenderCloudWindow();
extern bool g_bIsTestMode;

void CGUI::Render()
{
    if(!pNetGame)
        return;

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime   = CTimer::GetTimeStepInSeconds();

	DebugModules::PreRenderUpdate();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	if(CSettings::Get().szDebug == 1)
		DebugModules::Render2D();

    // TODO: VoiceChat
	// Voice::SpeakerList::Show();

	/*for (const auto& renderCallback : CVoiceRender::renderCallbacks) {
		if (renderCallback != nullptr) {
			renderCallback();
		}
	}*/
	//RenderCloudWindow();
	//ImGui::ShowDemoWindow(&sfadfklasdjflkasdjf);
//	RenderVersion();
	//RenderRakNetStatistics();
	CDebugInfo::Draw();
	RenderMechanicHook();
	RenderPassengerButton();
	RenderBackgroundHud();
//	CVehiclePipeMenu::Render();
  //  CGiftNotify::Render();
  
	CPlayerTags::Render();
	CText3DLabelsPool::Draw();
    GPS::DoPathDraw();

    // TODO: Speedometer
	if(CGame::FindPlayerPed()->m_pPed->IsInVehicle() && !CGame::FindPlayerPed()->m_pPed->IsAPassenger()
	&& !CKeyBoard::IsOpen() && !CDialog::bIsShow && CHUD::bIsShow && !CTimer::m_UserPause)
	{
		if(CSpeedometr::thiz == nullptr)
		{
			CSpeedometr::Constructor();
		}
	}
	else
	{
		if(CSpeedometr::thiz)
		{
			CSpeedometr::Destroy();
		}
	}

	CKeyBoard::Render();
    CActionsPed::drawProgress(); // TODO: func

	m_bAnyItemHovered = ImGui::IsAnyItemHovered();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}

bool CGUI::OnTouchEvent(int &type, int &num, int &x, int &y)
{
	if(!CKeyBoard::OnTouchEvent(type, num, x, y)) return false;

	ImGuiIO& io = ImGui::GetIO();

//	io.MousePos.x = (float)x;
//	io.MousePos.y = (float)y;
	io.AddMousePosEvent((float)x, (float)y);

    if (pNetGame) {
        if (!CTextDrawPool::OnTouchEvent(type, num, x, y))
            return true;
    }

	switch(type)
	{
		case TOUCH_PUSH:
		{
		//	io.AddMousePosEvent((float)x, (float)y);
			io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
//			if(m_bAnyItemHovered) {
//				return false;
//			}
			return true;
		}

		case TOUCH_POP:
		{
			//io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
			io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
			return true;
		}
		default:
		{
		//	io.AddMousePosEvent((float)x, (float)y);
//			if(m_bAnyItemHovered) {
//				return false;
//			}
			return true;
		}
	}
}

extern uint32_t g_uiBorderedText;

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	const int iOffset = CSettings::m_Settings.iFontOutline;
	const auto drawList = ImGui::GetBackgroundDrawList();
	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			drawList->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right
			posCur.x += iOffset;
			drawList->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			drawList->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			drawList->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			const ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());

			static const ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
			drawList->AddRectFilled(posCur, b, co);

		}
	}

	drawList->AddText(posCur, col, text_begin, text_end);
}

void CGUI::CreateCircleProgressBar(ImVec2 pos, ImColor color, int value, float radius, float thickness)
{
	float a_max = (radius / (thickness / 2) / 100.f) * (float)value;

	ImGui::GetForegroundDrawList()->PathClear();
	ImGui::GetForegroundDrawList()->PathArcTo(pos, radius, 0, a_max);
	ImGui::GetForegroundDrawList()->PathStroke(color, false, ImMax(2.0f, thickness));
}

void CGUI::RainbowText(const char* text, int fontSize)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto posCur = ImGui::GetCursorPos();
	int iOffset = CSettings::m_Settings.iFontOutline;

    float time = ImGui::GetTime();

	if(iOffset > 0) {
		//
		posCur.x -= iOffset;
		drawList->AddText(nullptr, fontSize, posCur, ImColor(IM_COL32_BLACK), text);
		posCur.x += iOffset;
		// right
		posCur.x += iOffset;
		drawList->AddText(nullptr, fontSize, posCur, ImColor(IM_COL32_BLACK), text);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		drawList->AddText(nullptr, fontSize, posCur, ImColor(IM_COL32_BLACK), text);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		drawList->AddText(nullptr, fontSize, posCur, ImColor(IM_COL32_BLACK), text);
		posCur.y -= iOffset;
	}

	float hue = fmod(time * 0.2f, 1.0f);
	ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(ImColor::HSV(hue, 1.0f, 1.0f)));

	drawList->AddText(nullptr, fontSize, posCur, color, text);
}

void ImGui::StyleColorsLiveRussia(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    style->FramePadding = ImVec2(0, 0);
    style->DisplayWindowPadding = ImVec2(0, 0);
    style->WindowPadding = ImVec2(0, 0);

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = (new ImColor(0xFF, 0xCA, 0x28, 0xFF))->Value;
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = (new ImColor(0xFF, 0xC1, 0x07, 0xFF))->Value;
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

bool ImGui::ButtonLr(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, nullptr, true);
	ImDrawList* draw = GetWindowDrawList();

	ImVec2 pos = ImGui::GetCursorPos();
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = (held || pressed ? ImColor(46, 164, 79, 225) : ImColor(46, 164, 79, 255)); // if (button held or hovered then ImColor(46, 164, 79, 225) if not hovered or helded ImColor(46, 164, 79, 255)
	draw->AddRectFilled(bb.Min, bb.Max, col, 5, ImDrawFlags_RoundCornersAll/*this will be used almost everywhere*/);

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");

	RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}

bool ImGui::ImageButtonLr(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 bb_max = ImVec2(pos.x + size.x, pos.y + size.y);

    ImGuiContext& g = *GImGui;
    ImGuiID id = window->GetID("##image");
    if (frame_padding >= 0)
    {
        bb_max += ImVec2(frame_padding, frame_padding);
    }

    ImGui::ItemSize(bb_max, frame_padding);
    if (!ImGui::ItemAdd(ImRect(pos, bb_max), id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ImRect(pos, bb_max), id, &hovered, &held);

    if (bg_col.w > 0.0f)
        ImGui::GetWindowDrawList()->AddRectFilled(pos, bb_max, ImGui::GetColorU32(bg_col));

    ImGui::GetWindowDrawList()->AddImage(user_texture_id, pos, bb_max, uv0, uv1, ImGui::GetColorU32(tint_col));

    return pressed;
}

DataStructures::SingleProducerConsumer<BUFFERED_COMMAND_TEXTDRAW> CGUI::m_BufferedCommandTextdraws;
void CGUI::PushToBufferedQueueTextDrawPressed(uint16_t textdrawId)
{
    BUFFERED_COMMAND_TEXTDRAW* pCmd = m_BufferedCommandTextdraws.WriteLock();

    pCmd->textdrawId = textdrawId;

    m_BufferedCommandTextdraws.WriteUnlock();
}

void CGUI::ProcessPushedTextdraws()
{
    BUFFERED_COMMAND_TEXTDRAW* pCmd = nullptr;
    while (pCmd = m_BufferedCommandTextdraws.ReadLock())
    {
        RakNet::BitStream bs;
        bs.Write(pCmd->textdrawId);
        pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
        m_BufferedCommandTextdraws.ReadUnlock();
    }
}