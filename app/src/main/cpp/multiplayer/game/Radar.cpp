//
// Created on 15.10.2023.
//

#include "Radar.h"
#include "../util/patch.h"
#include "World.h"
#include "net/netgame.h"
#include "Widgets/TouchInterface.h"
#include "game.h"
#include "Mobile/MobileMenu/MobileMenu.h"
#include "EntryExit.h"
#include "GPS.h"

tBlipHandle CRadar::SetCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName) {
    if (pNetGame && !strncmp(scriptName, "CODEWAY", 7)) {
        float findZ = CWorld::FindGroundZForCoord(posn.x, posn.y);
        findZ += 1.5f;

        CVector pos(posn.x, posn.y, findZ);
        GPS::Set(pos, true);

        RakNet::BitStream bsSend;
        bsSend.Write(posn.x);
        bsSend.Write(posn.y);
        bsSend.Write(findZ);
        pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }

    auto index = FindTraceNotTrackingBlipIndex();
    if (index == -1)
        return -1;

    auto& t = ms_RadarTrace[index];

    t.m_vPosition        = posn;
    t.m_nBlipDisplayFlag = blipDisplay;
    t.m_nBlipType        = type;
    t.m_nColour          = BLIP_COLOUR_DESTINATION;
    t.m_fSphereRadius    = 1.f;
    t.m_nEntityHandle    = 0;
    t.m_nBlipSize        = 1;
    t.m_nBlipSprite      = eRadarSprite::RADAR_SPRITE_NONE;
    t.m_bBright          = true;
    t.m_bTrackingBlip    = true;
    t.m_pEntryExit       = nullptr;

    return GetNewUniqueBlipIndex(index);
}

tBlipHandle CRadar::GetNewUniqueBlipIndex(int32 index) {
    auto& trace = ms_RadarTrace[index];
    if (trace.m_nCounter >= std::numeric_limits<uint16>::max() - 1)
        trace.m_nCounter = 1; // Wrap back to 1
    else
        trace.m_nCounter++; // Increment
    return index | (trace.m_nCounter << 16);
}

int32 CRadar::FindTraceNotTrackingBlipIndex() {
    for (size_t i = 0; i < ms_RadarTrace.size(); ++i) {
        if (!ms_RadarTrace[i].m_bTrackingBlip) {
            return static_cast<int32>(i);
        }
    }
    return -1;
}

int32 CRadar::GetActualBlipArrayIndex(tBlipHandle blip) {
    if (blip == -1)
        return -1;

    const auto  traceIndex = static_cast<uint16>(blip);
    const auto& trace      = ms_RadarTrace[traceIndex];
    const auto  counter    = static_cast<uint16>(blip >> 16);
    if (counter != trace.m_nCounter || !trace.m_bTrackingBlip)
        return -1;

    return traceIndex;
}

void CRadar::ClearBlip(tBlipHandle blip) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ClearActualBlip(index);
    }
}

void CRadar::ClearActualBlip(int32 blipIndex) {
    if (blipIndex < 0 || blipIndex >= MAX_RADAR_TRACES)
        return;

    ClearActualBlip(ms_RadarTrace[blipIndex]);
}

void CRadar::ClearActualBlip(tRadarTrace& trace) {
    trace.m_nBlipSize = 1;
    trace.m_fSphereRadius = 1.0f;
    trace.m_pEntryExit = nullptr;
    trace.m_nBlipSprite = RADAR_SPRITE_NONE;

    trace.m_bBright       = true;
    trace.m_bTrackingBlip = false;
    trace.m_bShortRange   = false;
    trace.m_bFriendly     = false;
    trace.m_bBlipRemain   = false;
    trace.m_bBlipFade     = false;

    trace.m_nCoordBlipAppearance = BLIP_FLAG_FRIEND;
    trace.m_nBlipDisplayFlag = BLIP_DISPLAY_NEITHER;
    trace.m_nBlipType = BLIP_NONE;
}

void CRadar::TransformRealWorldPointToRadarSpace(CVector2D* out, const CVector2D* in)
{
    float x = (in->x - CRadar::vec2DRadarOrigin.x) / CRadar::m_radarRange;
    float y = (in->y - CRadar::vec2DRadarOrigin.y) / CRadar::m_radarRange;

    float sinVal = getCachedSin();
    float cosVal = getCachedCos();

    out->x =  cosVal * x + sinVal * y;
    out->y = -sinVal * x + cosVal * y;
}

void CRadar::TransformRadarPointToScreenSpace(CVector2D* out, const CVector2D* in)
{
    if (gMobileMenu->DisplayingMap) {
        out->x = gMobileMenu->MAP_OFFSET_X + in->x * gMobileMenu->NEW_MAP_SCALE;
        out->y = gMobileMenu->MAP_OFFSET_Y - in->y * gMobileMenu->NEW_MAP_SCALE;
        return;
    }

    const auto* widget = CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR];
    if (!widget)
        return;

    float width = fabsf(widget->m_RectScreen.right - widget->m_RectScreen.left);
    float centerX = (widget->m_RectScreen.left + widget->m_RectScreen.right) * 0.5f;
    out->x = centerX + in->x * width * 0.5f;

    float height = fabsf(widget->m_RectScreen.bottom - widget->m_RectScreen.top);
    float centerY = (widget->m_RectScreen.top + widget->m_RectScreen.bottom) * 0.5f;
    out->y = centerY - in->y * height * 0.5f;
}

float CRadar::LimitRadarPoint(CVector2D* point) {
    const auto mag = point->Magnitude();

    if (gMobileMenu->DisplayingMap)
        return mag;

    if (mag > 1.0f) {
        point->Normalise();
    }

    return mag;
}

bool CRadar::DisplayThisBlip(eRadarSprite spriteId, int8 priority) {
    return CGame::currArea == AREA_CODE_NORMAL_WORLD && !FindPlayerPed(-1)->m_nAreaCode
           || spriteId <= 0x34 && ((1LL << spriteId) & 0x1012100200001FLL) != 0;
}

void CRadar::LimitToMap(float& x, float& y) {
    const auto zoom = (gMobileMenu->screenStack.numEntries || gMobileMenu->pendingScreen)
                      ? gMobileMenu->NEW_MAP_SCALE
                      : 140.0f;

    {
        const auto min = SCREEN_STRETCH_X(gMobileMenu->MAP_OFFSET_X - zoom);
        const auto max = SCREEN_STRETCH_X(gMobileMenu->MAP_OFFSET_X + zoom);
        x = std::clamp(x, min, max);
    }

    {
        const auto min = SCREEN_STRETCH_Y(gMobileMenu->MAP_OFFSET_Y - zoom);
        const auto max = SCREEN_STRETCH_Y(gMobileMenu->MAP_OFFSET_Y + zoom);
        y = std::clamp(y, min, max);
    }
}

void Limit(float& x, float& y) {
    if (gMobileMenu->DisplayingMap) {
        x = SCREEN_STRETCH_Y(x);
        y = SCREEN_STRETCH_Y(y);

        CRadar::LimitToMap(x, y);
    }
}

void CRadar::DrawRadarSprite(eRadarSprite spriteId, float x, float y, uint8 alpha) {
    Limit(x, y);

    const auto widget = CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR];
    float size = widget ? fabsf(widget->m_RectScreen.right - widget->m_RectScreen.left) * 0.1f : 10.0f;

    if (DisplayThisBlip(spriteId, -99)) {
        RadarBlipSprites[(size_t)spriteId].Draw(
                CRect{ x - size, y - size, x + size, y + size },
                { 255, 255, 255, alpha }
        );
    }
}

CVector tRadarTrace::GetWorldPos() const {
    CVector pos = { m_vPosition };
    if (m_pEntryExit) {
        m_pEntryExit->GetPositionRelativeToOutsideWorld(pos);
    }
    return pos;
}

std::pair<CVector2D, CVector2D> tRadarTrace::GetRadarAndScreenPos(float* radarPointDist) const {
    CVector worldPos = GetWorldPos();

    CVector2D radarPos;
    radarPos.x = (worldPos.x - CRadar::vec2DRadarOrigin.x) * (1.0f / CRadar::m_radarRange);
    radarPos.y = (worldPos.y - CRadar::vec2DRadarOrigin.y) * (1.0f / CRadar::m_radarRange);

    float rotatedX = CRadar::getCachedCos() * radarPos.x + CRadar::getCachedSin() * radarPos.y;
    float rotatedY = CRadar::getCachedCos() * radarPos.y - CRadar::getCachedSin() * radarPos.x;
    radarPos = CVector2D(rotatedX, rotatedY);

    float dist = sqrtf(radarPos.x * radarPos.x + radarPos.y * radarPos.y);
    if (dist > 1.0f && !gMobileMenu->DisplayingMap) {
        radarPos.x *= (1.0f / dist);
        radarPos.y *= (1.0f / dist);
    }

    if (radarPointDist) {
        *radarPointDist = dist;
    }

    CVector2D screenPos;
    if (gMobileMenu->DisplayingMap) {
        screenPos.x = gMobileMenu->MAP_OFFSET_X + radarPos.x * gMobileMenu->NEW_MAP_SCALE;
        screenPos.y = gMobileMenu->MAP_OFFSET_Y - radarPos.y * gMobileMenu->NEW_MAP_SCALE;
    } else if (CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR]) {
        const auto& widgetRect = CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR]->m_RectScreen;
        float width = fabsf(widgetRect.right - widgetRect.left);
        float height = fabsf(widgetRect.top - widgetRect.bottom);

        screenPos.x = (widgetRect.left + widgetRect.right) * 0.5f + radarPos.x * width * 0.5f;
        screenPos.y = (widgetRect.top + widgetRect.bottom) * 0.5f - radarPos.y * height * 0.5f;
    }

    return std::make_pair(radarPos, screenPos);
}

CVector2D CRadar::TransformRealWorldPointToRadarSpace(const CVector2D& in) {
    CVector2D diff = in - vec2DRadarOrigin;
    CVector2D scaled(diff.x / m_radarRange, diff.y / m_radarRange);
    return CachedRotateClockwise(scaled);
}

CVector2D CRadar::TransformRadarPointToScreenSpace(const CVector2D& in) {
    if (gMobileMenu->DisplayingMap) {
        return {
                gMobileMenu->MAP_OFFSET_X + gMobileMenu->NEW_MAP_SCALE * in.x,
                gMobileMenu->MAP_OFFSET_Y - gMobileMenu->NEW_MAP_SCALE * in.y
        };
    } else {
        return {
                SCREEN_STRETCH_X(94.0f) / 2.0f + SCREEN_STRETCH_X(40.0f) + SCREEN_STRETCH_X(94.0f * in.x) / 2.0f,
                SCREEN_STRETCH_FROM_BOTTOM(104.0f) + SCREEN_STRETCH_Y(76.0f) / 2.0f - SCREEN_STRETCH_Y(76.0f * in.y) / 2.0f
        };
    }
}

bool bFullMap = false;
void CRadar::DrawRadarGangOverlay(bool inMenu) {
    bFullMap = inMenu;
    CGangZonePool::Draw();
    GPS::DoPathDraw();
}

uint32 CRadar::GetRadarTraceColour(uint32 color, bool bright, bool friendly) {
    return TranslateColorCodeToRGBA(color);
}

void CRadar::ShowRadarTraceWithHeight(float x, float y, uint32 size, uint32 R, uint32 G, uint32 B, uint32 A, eRadarTraceHeight height) {
    Limit(x, y);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

    const auto r = static_cast<uint8>(R), g = static_cast<uint8>(G), b = static_cast<uint8>(B), a = static_cast<uint8>(A);

    const auto widget = CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR];
    if (!widget) return;

    const float scale = fabsf(widget->m_RectScreen.right - widget->m_RectScreen.left) * 0.01f;

    const auto size0 = scale * float(size + 0), size1 = scale * float(size + 1);
    const auto size2 = scale * float(size + 2), size3 = scale * float(size + 3);

    /*

     If anyone has a cool idea to use the texture, then I will immediately attach how to do it correctly:
     CSprite2d* texture;
     texture->Draw(
                        {
                                x - size1, y - (size1 * 2),
                                x + size1, y
                        },
                        { r, g, b, a }
                );

     */

    switch (height) {
        case RADAR_TRACE_HIGH:
            CSprite2d::Draw2DPolygon( // draw black border
                    x, y + size3,
                    x, y + size3,
                    x + size3, y - size2,
                    x - size3, y - size2,
                    {0, 0, 0, a}
            );
            CSprite2d::Draw2DPolygon( // draw triangle
                    x, y + size1,
                    x, y + size1,
                    x + size1, y - size1,
                    x - size1, y - size1,
                    {r, g, b, a}
            );
            break;
        case RADAR_TRACE_NORMAL:
            CSprite2d::DrawRect( // draw black border
                    {
                            x - size1, y - size1,
                            x + size1, y + size1
                    },
                    {0, 0, 0, a}
            );

            CSprite2d::DrawRect( // draw box
                    {
                            x - size0, y - size0,
                            x + size0, y + size0
                    },
                    {r, g, b, a}
            );
            break;
        case RADAR_TRACE_LOW:
            CSprite2d::Draw2DPolygon( // draw black border
                    x + size3, y + size2,
                    x - size3, y + size2,
                    x, y - size3,
                    x, y - size3,
                    {0, 0, 0, a}
            );

            CSprite2d::Draw2DPolygon( // draw triangle
                    x + size1, y + size1,
                    x - size1, y + size1,
                    x, y - size1,
                    x, y - size1,
                    {r, g, b, a}
            );
            break;
    }
}

uint8 CRadar::CalculateBlipAlpha(float distance) {
    if (gMobileMenu->DisplayingMap) {
        return 255;
    }

    const auto alpha = 255 - (uint32)(distance / 6.0f * 255.0f);
    return (uint8)std::max((float)alpha, 70.0f);
}

void CRadar::DrawCoordBlip(int32 blipIndex, bool isSprite, uint8 nWidgetAlpha, float circleSize) {
    const auto& trace = ms_RadarTrace[blipIndex];
    if (trace.m_nBlipType == BLIP_CONTACT_POINT)
        return;

    if (isSprite == !trace.HasSprite())
        return;

    if (trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BOTH && trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BLIPONLY)
        return;

    float realDist{};
    const auto [radarPos, screenPos] = trace.GetRadarAndScreenPos(&realDist);
    const auto radarZoomValue = *(uint8*)(g_libGTASA + (VER_x32 ? 0x819D85 : 0x9FF3A5));
    const auto zoomedDist = radarZoomValue != 0u ? 255.0f : realDist;

    if (isSprite) {
        // either the blip is close to the player or we're drawing the whole map.
        const auto canBeDrawn = !trace.m_bShortRange || zoomedDist <= 1.0f || gMobileMenu->DisplayingMap;

        if (trace.HasSprite() && canBeDrawn) {
            DrawRadarSprite(trace.m_nBlipSprite, screenPos.x, screenPos.y, 255);
        }
        return;
    }

    if (trace.HasSprite())
        return;

    const auto GetHeight = [&] {
        const auto zDiff = trace.GetWorldPos().z - FindPlayerCentreOfWorld_NoInteriorShift(ePedType::PEDTYPE_PLAYER1).z;

        if (zDiff > 2.0f) {
            // trace is higher
            return RADAR_TRACE_LOW;
        } else if (zDiff >= -4.0f) {
            // they are at the around the same elevation.
            return RADAR_TRACE_NORMAL;
        } else {
            // player is higher
            return RADAR_TRACE_HIGH;
        }
    };

    CRGBA color;
    color.Set(trace.m_nColour);
    CRadar::ShowRadarTraceWithHeight(
            screenPos.x,
            screenPos.y,
            trace.m_nBlipSize,
            color.b,
            color.g,
            color.r,
            trace.m_bBlipFade ? color.a : CalculateBlipAlpha(realDist),
            GetHeight()
    );
}

void CRadar::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6773C4 : 0x84C7D0), &ms_RadarTrace);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6776F0 : 0x84CE18), &RadarBlipSprites);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676E00 : 0x84BC60), &m_radarRange);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677E3C : 0x84DCA8), &vec2DRadarOrigin);

    CHook::Redirect("_ZN6CRadar12SetCoordBlipE9eBlipType7CVectorj12eBlipDisplayPc", &SetCoordBlip);
    CHook::Redirect("_ZN6CRadar13DrawCoordBlipEihif", &DrawCoordBlip);
    CHook::Redirect("_ZN6CRadar15DrawRadarSpriteEtffh", &DrawRadarSprite);

    CHook::Redirect("_ZN6CRadar20DrawRadarGangOverlayEb", &DrawRadarGangOverlay);
    CHook::Redirect("_ZN6CRadar19GetRadarTraceColourEjhh", &GetRadarTraceColour);
    CHook::Redirect("_ZN6CRadar9ClearBlipEi", &ClearBlip);
}