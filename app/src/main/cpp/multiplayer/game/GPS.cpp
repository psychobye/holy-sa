#include "GPS.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "Entity/Ped/PlayerPed.h"
#include "PathFind.h"
#include "Radar.h"
#include "Widgets/TouchInterface.h"

bool GPS::enabled = false;

extern CGUI *pGUI;
void GPS::DoPathDraw() {
    if (!GPS::enabled) return;

    auto playerPed = CLocalPlayer::GetPlayerPed();
    if (!playerPed || !playerPed->m_pPed) return;

    CPathFind& paths = CPathFind::Get();
    CVector playerPos = playerPed->m_pPed->GetPosition();

    short nodesCount = 0;
    float dummyDist = 0.0f;

    paths.DoPathSearch(
            PATH_TYPE_VEH,
            playerPos,
            CNodeAddress(),
            GPS::to,
            resultNodes,
            &nodesCount,
            MAX_NODE_POINTS,
            &dummyDist,
            999999.0f,
            nullptr,
            999999.0f,
            false,
            CNodeAddress(),
            false,
            false
    );

    if (nodesCount <= 1) return;

    bool isPaused = CTimer::m_CodePause || CTimer::m_UserPause;
    float flMenuMapScaling = (float)RsGlobal->maximumHeight / 448.0f;

    int validCount = 0;
    for (int i = 0; i < nodesCount; ++i) {
        CPathNode* node = paths.GetPathNode(resultNodes[i]);
        if (!node) continue;

        CVector2D worldPos = node->GetPosition2D();
        CVector2D radarPos;
        CVector2D screenPos;

        CRadar::TransformRealWorldPointToRadarSpace(&radarPos, &worldPos);

        if (isPaused) {
            CRadar::TransformRadarPointToScreenSpace(&screenPos, &radarPos);
            screenPos.x *= flMenuMapScaling;
            screenPos.y *= flMenuMapScaling;
        } else {
            CRadar::LimitRadarPoint(&radarPos);
            CRadar::TransformRadarPointToScreenSpace(&screenPos, &radarPos);
        }

        nodePoints[validCount++] = screenPos;
    }

    if (validCount <= 1) return;

    bool bScissors = !isPaused;
    if (bScissors) {
        const auto* widget = CTouchInterface::m_pWidgets[WidgetIDs::WIDGET_RADAR];
        if (widget) {
            CRect rectCopy = widget->m_RectScreen;
            SetScissorRect(&rectCopy);
        }
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);

    for (int i = 0; i < validCount - 1; ++i) {
        CVector2D start = nodePoints[i];
        CVector2D end = nodePoints[i + 1];

        CVector2D dir = end - start;
        float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (len < 0.1f) continue;

        CVector2D normal = { -dir.y / len, dir.x / len };
        float thickness = isPaused ? (GPS_LINE_WIDTH * flMenuMapScaling) : GPS_LINE_WIDTH;
        normal *= (thickness * 0.5f);

        Setup2DVertex(lineVerts[0], start.x - normal.x, start.y - normal.y);
        Setup2DVertex(lineVerts[1], start.x + normal.x, start.y + normal.y);
        Setup2DVertex(lineVerts[2], end.x - normal.x, end.y - normal.y);
        Setup2DVertex(lineVerts[3], end.x + normal.x, end.y + normal.y);

        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, lineVerts, 4);
    }

    CRect emptyRect = { 0, 0, 0, 0 };
    if (bScissors) {
        SetScissorRect(&emptyRect);
    }
}

void GPS::Set(CVector pos, bool toggle) {
    GPS::to = pos;
    GPS::enabled = toggle;
}

void GPS::Setup2DVertex(RwIm2DVertex &vertex, float x, float y) {
    vertex.x = x;
    vertex.y = y;
    vertex.u = vertex.v = 0.0f;
    vertex.z = CSprite2d::NearScreenZ + 0.0001f;
    vertex.rhw = CSprite2d::RecipNearClip;
    vertex.emissiveColor = RWRGBALONG(GPS_LINE_B, GPS_LINE_G, GPS_LINE_R, GPS_LINE_A); // RGBA -> BGRA
}