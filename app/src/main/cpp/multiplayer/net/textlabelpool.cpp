#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "netgame.h"
#include "CSettings.h"
#include "game/Render/Sprite.h"
#include "../game/Entity/Ped/Ped.h"
#include "World.h"
#include "Camera.h"

extern CGUI *pGUI;

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors);

void CText3DLabelsPool::DrawAttachedToPlayer(CText3DLabel* pLabel)
{
	CPedSamp* pPed = CLocalPlayer::GetPlayerPed();

	if(!CSettings::m_Settings.iIsEnable3dTextInVehicle && pPed->m_pPed->IsInVehicle())
	    return;

	PLAYERID playerId = pLabel->attachedToPlayerID;

	CRemotePlayer* pPlayer = CPlayerPool::GetSpawnedPlayer(playerId);
	if (!pPlayer)
	{
		return;
	}

	CPedSamp* pPlayerPed = pPlayer->GetPlayerPed();

	CVector pos;
	pPlayerPed->m_pPed->GetBonePosition(&pos, BONE_HEAD, false);

	pos += pLabel->offsetCoords;

    float fDist = pPlayerPed->m_pPed->GetDistanceFromCamera();
    pos.z += (fDist * 0.055);

	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawAttachedToVehicle(CText3DLabel* pLabel)
{
	VEHICLEID vehId = pLabel->attachedToVehicleID;

	CVehicleSamp* pVehicle = CVehiclePool::GetAt(vehId);
	if (!pVehicle)
	{
		return;
	}
	if (!pVehicle->m_pVehicle->IsAdded())
	{
		return;
	}
	auto pos = pVehicle->m_pVehicle->GetPosition();

	pos += pLabel->offsetCoords;

	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawVehiclesInfo()
{
	static char strBuff[255];

	for(auto &pair : CVehiclePool::list) {
		auto pVehicle = pair.second;

		CVector screenPos;

		if (!CSprite::CalcScreenCoors(pVehicle->m_pVehicle->GetPosition(), &screenPos, nullptr, nullptr, false, true))
			return;

		memset(strBuff, 0, sizeof(strBuff));
		sprintf(strBuff, "id: %d, type: %d\nHealth: %.2f\nDistance: %.2fm\n Immun: %d",
				pair.first,
				pVehicle->m_pVehicle->m_nModelIndex,
				pVehicle->GetHealth(),
				pVehicle->m_pVehicle->GetDistanceFromCamera(),
				pVehicle->m_bIsInvulnerable
		);

		TextWithColors(ImVec2(screenPos.x, screenPos.y), __builtin_bswap32(0xf48fb1FF), strBuff, strBuff);
	}
}

void CText3DLabelsPool::DrawTextLabel(CText3DLabel* pLabel, const CVector* pos)
{
	int hitEntity = 0;

	CPedSamp* pPed = CLocalPlayer::GetPlayerPed();
	if (!pPed)
	{
		return;
	}
	if (!pPed->m_pPed->IsAdded())
	{
		return;
	}

    if (pPed->m_pPed->GetDistanceFromPoint(pos->x, pos->y, pos->z) <= pLabel->drawDistance) {
        if (pLabel->useLineOfSight) {
            CAMERA_AIM *pCam = GameGetInternalAim();
            if (!pCam) return;

            hitEntity = CWorld::GetIsLineOfSightClear(pos, CCamera::Get().GetPosition(), true, false, false, false, false, false, false);
        }
        if (!pLabel->useLineOfSight || hitEntity) {
            CVector Out;

            CSprite::CalcScreenCoors(*pos, &Out, nullptr, nullptr, false, false);
            if (Out.z < 1.0f) return;

            ImVec2 pos = ImVec2(Out.x, Out.y);

            if (pLabel->m_fTrueX < 0.0f) {
                char *curBegin = pLabel->textWithoutColors;
                char *curPos = pLabel->textWithoutColors;
                while (*curPos != '\0') {
                    if (*curPos == '\n') {
                        float width = ImGui::CalcTextSize(curBegin, (char *) (curPos - 1)).x;
                        if (width > pLabel->m_fTrueX) {
                            pLabel->m_fTrueX = width;
                        }

                        curBegin = curPos + 1;
                    }

                    curPos++;
                }

                if (pLabel->m_fTrueX < 0.0f) {
                    pLabel->m_fTrueX = ImGui::CalcTextSize(pLabel->textWithoutColors).x;
                }
            }

            //pos.x -= (pLabel->m_fTrueX / 2.0f);

            TextWithColors(pos, __builtin_bswap32(pLabel->color), pLabel->text,
                           pLabel->textWithoutColors);
        }
    }
}

void CText3DLabelsPool::Free()
{
    auto ids = GetAllIds();
    for (auto& id : ids) {
        Delete(id);
    }
}

void FilterColors(char* szStr) {
    if (!szStr) return;

    char* pSource = szStr;
    char* pDestination = szStr;

    while (*pSource != '\0') {
        if (*pSource == '{' && *(pSource + 7) == '}') {
            pSource += 8;
        } else {
            *pDestination++ = *pSource++;
        }
    }

    *pDestination = '\0';
}

void CText3DLabelsPool::CreateTextLabel(int labelId, char* text, uint32_t color, CVector pos, float drawDistance, bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID)
{
	Delete(labelId);

	auto pTextLabel = new CText3DLabel;

	cp1251_to_utf8(pTextLabel->text, text);
	cp1251_to_utf8(pTextLabel->textWithoutColors, text);
	FilterColors(pTextLabel->textWithoutColors);
	pTextLabel->color = color;
	pTextLabel->pos = pos;
	pTextLabel->drawDistance = drawDistance;
	pTextLabel->useLineOfSight = useLOS;
	pTextLabel->attachedToPlayerID = attachedToPlayerID;
	pTextLabel->attachedToVehicleID = attachedToVehicleID;
	pTextLabel->m_fTrueX = -1.0f;
	if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID) {
		pTextLabel->offsetCoords = pos;
	}
	list[labelId] = pTextLabel;
}

void CText3DLabelsPool::Delete(int labelId)
{
	if(GetAt(labelId)) {
		delete list[labelId];
		list.erase(labelId);
	}
}

void CText3DLabelsPool::Update3DLabel(int labelId, uint32_t color, char* text)
{
	auto pLabel = GetAt(labelId);
	if (pLabel)
	{
		pLabel->color = color;
		//m_pTextLabels[labelID]->text = text;
		cp1251_to_utf8(pLabel->text, text);
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors = nullptr) {
    float fLineOffsets[100];
    int iCounter = 0;

    if (szStrWithoutColors) {
        const char* curBegin = szStrWithoutColors;
        const char* curPos = szStrWithoutColors;
        while (*curPos != '\0') {
            if (*curPos == '\n') {
                float width = ImGui::CalcTextSize(curBegin, curPos).x / 2.0f;
                fLineOffsets[iCounter++] = width;
                curBegin = curPos + 1;
            }
            curPos++;
            if (*curPos == '\0') {
                float width = ImGui::CalcTextSize(curBegin, curPos).x / 2.0f;
                fLineOffsets[iCounter++] = width;
            }
        }
    }

    iCounter = 0;
    const char* textCur = szStr;
    ImVec2 vecPos = pos - ImVec2(szStrWithoutColors ? fLineOffsets[0] : 0, 0);
    const char* textStart = textCur;

    while (*textCur != '\0') {
        if (*textCur == '{') {
            if (textCur != textStart) {
                ImVec2 textSize = ImGui::CalcTextSize(textStart, textCur);
                pGUI->RenderText(vecPos, col, true, textStart, textCur);
                vecPos.x += textSize.x;
            }
            const char* colorStart = textCur + 1;
            do {
                ++textCur;
            } while (*textCur != '\0' && *textCur != '}');

            ImVec4 textColor;
            if (ProcessInlineHexColor(colorStart, textCur, textColor)) {
                col = textColor;
            }

            textStart = textCur + 1;
        } else if (*textCur == '\n') {
            ImVec2 textSize = ImGui::CalcTextSize(textStart, textCur);
            pGUI->RenderText(vecPos, col, true, textStart, textCur);
            vecPos = ImVec2(pos.x - (szStrWithoutColors ? fLineOffsets[++iCounter] : 0), vecPos.y + pGUI->GetFontSize());
            textStart = textCur + 1;
        }
        ++textCur;
    }

    if (textCur != textStart) {
        ImVec2 textSize = ImGui::CalcTextSize(textStart, textCur);
        pGUI->RenderText(vecPos, col, true, textStart, textCur);
        vecPos.x += textSize.x;
    } else {
        vecPos.y += pGUI->GetFontSize();
    }
}



#include "..//game/game.h"
void CText3DLabelsPool::Draw() {
    if (!CGame::IsToggledHUDElement(HUD_ELEMENT_TEXTLABELS)) {
        return;
    }

    if (CGame::m_bDl_enabled) {
        DrawVehiclesInfo();
    }

    for (auto& pair : list) {
        auto pLabel = pair.second;

        if (pLabel->attachedToPlayerID != INVALID_PLAYER_ID && pLabel->attachedToPlayerID != CPlayerPool::GetLocalPlayerID()) {
            DrawAttachedToPlayer(pLabel);
        } else if (pLabel->attachedToVehicleID != INVALID_VEHICLE_ID) {
            DrawAttachedToVehicle(pLabel);
        } else {
            DrawTextLabel(pLabel, &pLabel->pos);
        }
    }
}


bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = static_cast<int>(end - start);
	if (hexCount == 6 || hexCount == 8)
	{
		unsigned int hexColor = 0;
		for (int i = 0; i < hexCount; ++i)
		{
			char c = *(start + i);
			if (c >= '0' && c <= '9')
				hexColor = (hexColor << 4) | (c - '0');
			else if (c >= 'A' && c <= 'F')
				hexColor = (hexColor << 4) | (c - 'A' + 10);
			else if (c >= 'a' && c <= 'f')
				hexColor = (hexColor << 4) | (c - 'a' + 10);
			else
				return false;
		}

		color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
		color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
		color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
		color.w = 1.0f;

		if (hexCount == 8)
			color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;

		return true;
	}

	return false;
}
