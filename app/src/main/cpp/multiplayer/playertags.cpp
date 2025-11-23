#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "playertags.h"
#include "CSettings.h"
#include "game/Render/Sprite.h"
#include "game/Entity/Ped/Ped.h"
#include "voice/BlackList.h"
#include "voice/PluginConfig.h"
#include "voice/SpeakerList.h"
#include "game/World.h"
#include "game/Camera.h"

extern CNetGame *pNetGame;
extern CGUI *pGUI;

void CPlayerTags::Init()
{
	Log("Loading afk_icon..");
	pMuteIconTex = CUtil::LoadTextureFromDB("gui", "mute_icon");
    pSpeakIconTex = CUtil::LoadTextureFromDB("gui", "speaker_icon");
	m_pAfk_icon = CUtil::LoadTextureFromDB("samp", "afk_icon");
	m_pKeyboardIconTex = CUtil::LoadTextureFromDB("gui", "keyboard_icon");

	HealthBarBDRColor = ImColor( 0x00, 0x00, 0x00, 0xFF );

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_bChatBubbleStatus[i] = false;
		m_pSzText[i] = nullptr;
		m_pSzTextWithoutColors[i] = nullptr;
	}
}

#include <algorithm>
void CPlayerTags::Render() {
	if (!pNetGame->m_bShowPlayerTags || CSettings::m_Settings.i3dTextsDisable)
		return;

	RenderActors();

	static CVector vecPos;
    

	for(auto &pair : CPlayerPool::spawnedPlayers) {
		auto &pPlayer = pair.second;
		auto playerId = pair.first;

		if (!pPlayer->m_bShowNameTag || pPlayer->GetPlayerPed()->m_pPed->m_bOffscreen)
			continue;

		auto pPlayerPed = pPlayer->GetPlayerPed();

		float distFromCam = pPlayerPed->m_pPed->GetDistanceFromCamera();

		if (m_bChatBubbleStatus[playerId]) {
			if (distFromCam > m_fDistance[playerId]) {
				ResetChatBubble(playerId);
			} else {
				pPlayerPed->m_pPed->GetBonePosition(&vecPos, BONE_JAW, false);
				DrawChatBubble(playerId, &vecPos, distFromCam);
			}

			if (GetTickCount() - m_dwStartTime[playerId] >= m_dwTime[playerId]) {
				ResetChatBubble(playerId);
			}
		}

		if (distFromCam <= pNetGame->m_fNameTagDrawDistance) {
			pPlayerPed->m_pPed->GetBonePosition(&vecPos, BONE_JAW, false);

			if (!pNetGame->m_bNameTagLOS || CWorld::GetIsLineOfSightClear(vecPos, CCamera::Get().GetPosition(), true, false, false, true, false, false, false)) {
				static char szNickBuf[50]{};
				memset(szNickBuf, 0, sizeof(szNickBuf));

				sprintf(szNickBuf, "%s (%d)", CPlayerPool::GetPlayerName(playerId), playerId);
				Draw(&vecPos, szNickBuf, &distFromCam, pPlayer);
			}
		}
	}
}

void CPlayerTags::RenderActors() {

	static CVector vecPos;
	

	for (auto & pair : CActorPool::list) {

		auto &pActor = pair.second;
		auto &pPed = pActor->m_pPed;

		float distFromCam = pPed->GetDistanceFromCamera();

        if(pPed->m_bOffscreen)
            continue;

		if (distFromCam <= pNetGame->m_fNameTagDrawDistance) {
			pPed->GetBonePosition(&vecPos, BONE_JAW, false);

			if (!pNetGame->m_bNameTagLOS || CWorld::GetIsLineOfSightClear(vecPos, CCamera::Get().GetPosition(), false, false, false, false, false, false, false)) {
				vecPos.z += 0.25f + (distFromCam * 0.0475f);

                CVector sceenPos;
                if(!CSprite::CalcScreenCoors(vecPos, &sceenPos, nullptr, nullptr, false, true))
                    return;

				static char szNickBuf[255];
				memset(szNickBuf, 0, sizeof(szNickBuf));
				sprintf(szNickBuf, "%s", pActor->m_szName);

                const auto textSize = ImGui::CalcTextSize(szNickBuf);

                ImVec2 pos(sceenPos.x - textSize.x / 2, sceenPos.y);

                pGUI->RenderText(pos, 0xFFFFFFFF, true, szNickBuf);

                DrawHealthAndArmour(
                        &sceenPos,
                        pActor->m_pPed->m_fHealth,
                        pActor->m_pPed->m_fArmour
                        );
				//Draw(&vecPos, szNickBuf, &distFromCam, pPlayer);
			}
		}
	}
}


void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors = nullptr);

void FilterColors(char* szStr);

void CPlayerTags::AddChatBubble(PLAYERID playerId, char* szText, uint32_t dwColor, float fDistance, uint32_t dwTime)
{
	if (m_bChatBubbleStatus[playerId])
	{
		ResetChatBubble(playerId);
		m_dwColors[playerId] = dwColor;
		m_fDistance[playerId] = fDistance;
		m_dwTime[playerId] = dwTime;
		m_dwStartTime[playerId] = GetTickCount();
		m_bChatBubbleStatus[playerId] = 1;
		m_fTrueX[playerId] = -1.0f;
		cp1251_to_utf8(m_pSzText[playerId], szText);
		cp1251_to_utf8(m_pSzTextWithoutColors[playerId], szText);
		FilterColors(m_pSzTextWithoutColors[playerId]);
		const char* pText = m_pSzTextWithoutColors[playerId];
		m_iOffset[playerId] = 0;
		while (*pText)
		{
			if (*pText == '\n')
			{
				m_iOffset[playerId]++;
			}
			pText++;
		}
		return;
	}
	m_dwColors[playerId] = dwColor;
	m_fDistance[playerId] = fDistance;
	m_dwTime[playerId] = dwTime;
	m_dwStartTime[playerId] = GetTickCount();
	m_bChatBubbleStatus[playerId] = 1;
	m_fTrueX[playerId] = -1.0f;
	m_pSzText[playerId] = new char[1024];
	m_pSzTextWithoutColors[playerId] = new char[1024];
	cp1251_to_utf8(m_pSzText[playerId], szText);
	cp1251_to_utf8(m_pSzTextWithoutColors[playerId], szText);
	FilterColors(m_pSzTextWithoutColors[playerId]);
	const char* pText = m_pSzTextWithoutColors[playerId];
	m_iOffset[playerId] = 0;
	while (*pText)
	{
		if (*pText == '\n')
		{
			m_iOffset[playerId]++;
		}
		pText++;
	}
}

void CPlayerTags::ResetChatBubble(PLAYERID playerId)
{
	if (m_bChatBubbleStatus[playerId])
	{
		m_dwTime[playerId] = 0;
	}
	m_bChatBubbleStatus[playerId] = 0;
}

void CPlayerTags::DrawChatBubble(PLAYERID playerId, CVector* vec, float fDistance)
{
	CVector TagPos;

	TagPos = vec;

	//TagPos.z = vec->Z;
	TagPos.z += 0.45f + (fDistance * 0.0675f) + ((float)m_iOffset[playerId] * pGUI->ScaleY(0.35f));

	CVector Out;

	CSprite::CalcScreenCoors(TagPos, &Out, nullptr, nullptr, false, false);

	if (Out.z < 1.0f)
		return;

	// name (id)
	ImVec2 pos = ImVec2(Out.x, Out.y);

	if (m_fTrueX[playerId] < 0)
	{
		char* curBegin = m_pSzTextWithoutColors[playerId];
		char* curPos = m_pSzTextWithoutColors[playerId];
		while (*curPos != '\0')
		{
			if (*curPos == '\n')
			{
				float width = ImGui::CalcTextSize(curBegin, (char*)(curPos - 1)).x;
				if (width > m_fTrueX[playerId])
				{
					m_fTrueX[playerId] = width;
				}

				curBegin = curPos + 1;
			}

			curPos++;
		}

		if (m_fTrueX[playerId] < 0)
		{
			m_fTrueX[playerId] = ImGui::CalcTextSize(m_pSzTextWithoutColors[playerId]).x;
		}
		//Log("m_fTrueX = %f", m_pTextLabels[x]->m_fTrueX);
	}

	pos.x -= (m_fTrueX[playerId] / 2);

	TextWithColors(pos, __builtin_bswap32(m_dwColors[playerId]), m_pSzText[playerId]);
}

void CPlayerTags::DrawHealthAndArmour(CVector* screenPos, float health, float armour) {

	if (health < 0.0f)
		return;


	HealthBarColor = ImColor(0xB9, 0x22, 0x28, 0xFF);
	HealthBarBGColor = ImColor(0x4B, 0x0B, 0x14, 0xFF);

	const float fWidth = pGUI->ScaleX(60.0f);
	const float fHeight = pGUI->ScaleY(10.0f);
	const float fOutline = static_cast<float>(CSettings::m_Settings.iFontOutline);

	// top left
	HealthBarBDR1 = ImVec2(screenPos->x - ((fWidth / 2) + fOutline), screenPos->y + (pGUI->GetFontSize() * 1.2f));
	// bottom right
	HealthBarBDR2 = ImVec2(screenPos->x + ((fWidth / 2) + fOutline), screenPos->y + (pGUI->GetFontSize() * 1.2f) + fHeight);

	// top left
	HealthBarBG1 = ImVec2(HealthBarBDR1.x + fOutline, HealthBarBDR1.y + fOutline);
	// bottom right
	HealthBarBG2 = ImVec2(HealthBarBDR2.x - fOutline, HealthBarBDR2.y - fOutline);

	// top left
	HealthBar1 = HealthBarBG1;
	// bottom right
	HealthBar2.y = HealthBarBG2.y;

	if (health > 100.0f)
		health = 100.0f;

	health *= fWidth / 100.0f;
	health -= (fWidth / 2);
	HealthBar2.x = screenPos->x + health;

	if (armour > 0.0f)
	{
		HealthBarBDR1.y += 13.0f;
		HealthBarBDR2.y += 13.0f;
		HealthBarBG1.y += 13.0f;
		HealthBarBG2.y += 13.0f;
		HealthBar1.y += 13.0f;
		HealthBar2.y += 13.0f;
	}

	ImGui::GetForegroundDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
	ImGui::GetForegroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
	ImGui::GetForegroundDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);

	// Armour Bar
	if (armour > 0.0f)
	{
		HealthBarBDR1.y -= 13.0f;
		HealthBarBDR2.y -= 13.0f;
		HealthBarBG1.y -= 13.0f;
		HealthBarBG2.y -= 13.0f;
		HealthBar1.y -= 13.0f;
		HealthBar2.y -= 13.0f;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if (armour > 100.0f)
			armour = 100.0f;

		armour *= fWidth / 100.0f;
		armour -= (fWidth / 2);
		HealthBar2.x = screenPos->x + armour;

		ImGui::GetForegroundDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
		ImGui::GetForegroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
		ImGui::GetForegroundDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);
	}
}

void CPlayerTags::Draw(CVector* tagPpos, const char* szName, const float *distFromCam, CRemotePlayer* pPlayer)
{
    tagPpos->z += 0.25f + (*distFromCam * 0.0475f);

    CVector Out;
    if(!CSprite::CalcScreenCoors(*tagPpos, &Out, nullptr, nullptr, false, true))
		return;

    const auto textSize = ImGui::CalcTextSize(szName);

    ImVec2 pos(Out.x - textSize.x / 2, Out.y);

    pGUI->RenderText(pos, __builtin_bswap32(pPlayer->GetPlayerColor() | (0x000000FF)), true, szName);

    // TAG
	if(pPlayer->m_nTag > CRemotePlayer::eTags::NONE && pPlayer->m_nTag <= CRemotePlayer::eTags::Developer) {
		auto tagText = CRemotePlayer::tagsName[pPlayer->m_nTag];
		ImVec2 textSize = ImGui::CalcTextSize(tagText.c_str());

		const ImVec2 backsize = ImVec2(textSize.x + 12, textSize.y + 5);
		const ImVec2 backPos = ImVec2(pos.x - (backsize.x + 12), pos.y - 2);

		ImGui::GetForegroundDrawList()->AddRectFilled(
				backPos,
				backPos + backsize,
				CRemotePlayer::tagsColors[pPlayer->m_nTag],
				8.0f,
				ImDrawFlags_RoundCornersAll
		);


		ImVec2 textPos = CGUI::GetCenterOf(backPos, backsize, textSize);
		ImGui::GetForegroundDrawList()->AddText(textPos, IM_COL32(0, 0, 0, 255), tagText.c_str());
	}

	DrawHealthAndArmour(&Out,
						pPlayer->m_fCurrentHealth,
						pPlayer->m_fCurrentArmor
						);

    // AFK Icon
    if (pPlayer->IsAFK() && m_pAfk_icon && m_pAfk_icon->raster) {
        ImVec2 a(HealthBarBDR1.x - (pGUI->GetFontSize() * 1.4f), HealthBarBDR1.y);
        ImVec2 b(a.x + (pGUI->GetFontSize() * 1.3f), a.y + (pGUI->GetFontSize() * 1.3f));
        ImGui::GetForegroundDrawList()->AddImage((ImTextureID)m_pAfk_icon->raster, a, b);
    }

    const ImVec2 kbPos(Out.x + (textSize.x / 2) + float(pPlayer->m_nKeyBoardStat / 5) + 5 , Out.y);

    if (pPlayer->m_bKeyboardOpened)
    {
        pPlayer->m_nKeyBoardStat ++;
		if(pPlayer->m_nKeyBoardStat == 50)
            pPlayer->m_nKeyBoardStat = 0;

        ImGui::GetForegroundDrawList()->AddImage((ImTextureID)m_pKeyboardIconTex->raster, kbPos, ImVec2(kbPos.x + pGUI->GetFontSize() * 1.3f, kbPos.y + pGUI->GetFontSize() * 1.3f));
    }
	if(BlackList::IsPlayerBlocked(pPlayer->GetID())) {
		ImVec2 a(Out.x + (textSize.x / 2) + 10, Out.y - (Voice::PluginConfig::kDefValSpeakerIconSize / 5));
        if(pPlayer->m_bKeyboardOpened)
            a.x = kbPos.x + pGUI->GetFontSize() * 1.3f + 10.f;

		const ImVec2 b = ImVec2(a.x + Voice::PluginConfig::kDefValSpeakerIconSize, a.y + Voice::PluginConfig::kDefValSpeakerIconSize);

		ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)pMuteIconTex->raster, a, b);
	}
    for(const auto& playerStream : Voice::SpeakerList::playerStreams[pPlayer->GetID()])
    {
        if(playerStream.second.GetType() == Voice::StreamType::LocalStreamAtPlayer)
        {
            const ImVec2 a(Out.x + (textSize.x / 2) + 10, Out.y - (Voice::PluginConfig::kDefValSpeakerIconSize / 5) );
            const ImVec2 b = ImVec2(a.x + Voice::PluginConfig::kDefValSpeakerIconSize, a.y + Voice::PluginConfig::kDefValSpeakerIconSize);

            ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)pSpeakIconTex->raster, a, b);
        }
    }
}

