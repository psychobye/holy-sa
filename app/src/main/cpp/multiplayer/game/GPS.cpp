#include "GPS.h"
#include "gui/gui.h"
#include "voice/SpeakerList.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "net/localplayer.h"
#include "Entity/Ped/PlayerPed.h"

RwTexture* marker;

bool GPS::enabled = false;

extern CGUI *pGUI;
void GPS::DoPathDraw() {
    if(!GPS::enabled) return;
    if(!marker) return;
    if(!pNetGame) return;

    CPedSamp *pActor = CLocalPlayer::GetPlayerPed();
    auto &pPed = pActor->m_pPed;

    float distFromCam = pPed->GetDistanceFromCamera();

    CVector TagPos;

    TagPos = GPS::to;

    TagPos.z += 0.25f + distFromCam * 0.0475f;

    CVector Out;
    // CSprite::CalcScreenCoors
    ((void (*)(CVector *, CVector *, float *, float *, bool, bool)) (g_libGTASA + (VER_x32 ? 0x005C57E8 + 1 : 0x6E9DF8)))(
            &TagPos, &Out, 0, 0, 0, 0);

    if (Out.z < 1.0f)
        return;

    ImVec2 pos = ImVec2(Out.x, Out.y);
    pos.x -= 46.0f / 2;
    pos.y -= pGUI->GetFontSize();

    ImVec2 a = ImVec2(pos.x, pos.y);
    ImVec2 b = ImVec2(pos.x + 46.0f ,
                      pos.y + 46.0f);
    ImGui::GetForegroundDrawList()->AddImage((ImTextureID) marker->raster, a, b);

    char szStr[30];

    snprintf(&szStr[0], 30, "%d m.", (int)pPed->GetDistanceFromPoint(GPS::to.x, GPS::to.y, GPS::to.z));
    ImVec2 vpos = ImVec2(Out.x - 30, Out.y + 40);
    pGUI->RenderText(vpos, (ImU32)0xFFFFFFFF, true, &szStr[0]);

}

void GPS::Set(CVector pos, bool toggle) {
    Log("GPS::Set");
    GPS::to = pos;
    GPS::enabled = toggle;

    if(!marker)
        marker = CUtil::LoadTextureFromDB("txd", "radar_waypoint");
}