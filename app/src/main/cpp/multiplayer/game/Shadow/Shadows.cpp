//
// Created by Traw-GG on 05.10.2025.
//

#include "Shadows.h"
#include "Camera.h"
#include "util/patch.h"
#include "net/netgame.h"
#include "TxdStore.h"
#include "Core/Rect.h"
#include "RenderBuffer.h"
#include "Weather.h"
#include "World.h"
#include "WaterLevel.h"
#include "game/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "TimeCycle.h"
#include "Models/ModelInfo.h"
#include "Mobile/MobileSettings/MobileSettings.h"

void CShadows::RenderStaticShadows(bool renderAdditive) {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));

    RenderBuffer::ClearRenderBuffer();

    // Mark all as not-yet-rendered
    for (auto& shdw : aStaticShadows) {
        shdw.m_bRendered = false;
    }

    constexpr auto MAX_CAM_TO_LIGHT_DIST = 100.f;

    // Render all in batches
    for (auto& oshdw : aStaticShadows) {
        if (!oshdw.m_pPolyBunch || oshdw.m_bRendered) {
            continue;
        }

        // Setup additional render states for this shadow
        SetRenderModeForShadowType(oshdw.m_nType);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));

        // Batch all other shadows with the same texture and type into the buffer
        for (auto& ishdw : aStaticShadows) {
            if (!ishdw.m_pPolyBunch) {
                continue;
            }
            if (ishdw.m_nType != oshdw.m_nType) {
                continue;
            }
            if (ishdw.m_pTexture != oshdw.m_pTexture) {
                continue;
            }

            const auto shdwToCam2D       = CVector2D{ CCamera::Get().GetPosition() - ishdw.m_vecPosn };
            const auto shdwToCamDist2DSq = shdwToCam2D.SquaredMagnitude();
            if (shdwToCamDist2DSq < sq(MAX_CAM_TO_LIGHT_DIST)) {
                // No need to check if this one was rendered (because of the batching)

                // Render polies of this shadow
                for (auto poly = ishdw.m_pPolyBunch; poly; poly = poly->m_pNext) {
                    // 0x70841F: Calculate color
                    uint8 r, g, b;
                    AffectColourWithLighting(
                            ishdw.m_nType,
                            ishdw.m_nDayNightIntensity,
                            ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                            r, g, b
                    );

                    const auto totalNoIdx =
                            3 * (poly->m_wNumVerts - 2); // Total no. of indices we'll use

                    // 0x708432: Begin render buffer store
                    RwIm3DVertex *vtxIt{};
                    RwImVertexIndex *vtxIdxIt{};
                    RenderBuffer::StartStoring(
                            totalNoIdx,
                            poly->m_wNumVerts,
                            vtxIdxIt, vtxIt
                    );

                    // 0x70851D: Write vertices (`if` not necessary, it's part of the loop condition)
                    const auto a = (uint8) ((float) ishdw.m_nIntensity * (1.f - CWeather::Foggyness * 0.5f));
                    for (auto i{0}; i < poly->m_wNumVerts; i++, vtxIt++) {
                        const auto &pos = poly->m_avecPosn[i];
                        RwIm3DVertexSetPos(vtxIt, pos.x, pos.y, pos.z + 0.06f);
                        RwIm3DVertexSetRGBA(vtxIt, r, g, b, a);
                                RwIm3DVertexSetU(vtxIt, (float) poly->m_aU[i] / 200.f);
                                RwIm3DVertexSetV(vtxIt, (float) poly->m_aV[i] / 200.f);
                    }

                    // 0x7085BC: Write indices  (`if` not necessary, it's part of the loop condition)
                    for (auto i = 0; i < totalNoIdx; i++) {
                        *vtxIdxIt++ = g_ShadowVertices[i];
                    }

                    // Finish storing
                    RenderBuffer::StopStoring();
                }

                // Mark this as rendered
                ishdw.m_bRendered = true;
            }
        }

        // Render out this batch
        RenderBuffer::RenderStuffInBuffer();
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}

void CShadows::RenderStoredShadows() {
    // Originally renderstates are still set even though there are no shadows to be rendered
    // I don't think this is necessary, so we early-out here
    if (!ShadowsStoredToBeRendered) {
        return;
    }

    RenderBuffer::ClearRenderBuffer();

    for (auto i = 0; i < ShadowsStoredToBeRendered; i++) {
        asShadowsStored[i].m_bAlreadyRenderedInBatch = false;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,       RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));

    const auto GetShadowRect = [](CRegisteredShadow& shdw) {
        return CRect{
                shdw.m_vecPosn,
                shdw.m_vecPosn + shdw.m_Side + shdw.m_Front
        };
    };

    for (auto o = 0; o < ShadowsStoredToBeRendered; o++) {
        auto& oshdw = asShadowsStored[o];

        // Setup additional render states for this shadow (and others in the batch below)
        SetRenderModeForShadowType(oshdw.m_nType);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));

        //
        // Render onto ground (If not already)
        //
        if (!oshdw.m_bAlreadyRenderedInBatch) {
            // We do a batched rendering here:
            // All shadows of the same type and texture are rendered together to save on drawcalls

            for (auto i = o; i < ShadowsStoredToBeRendered; i++) {
                auto& ishdw = asShadowsStored[i];

                if (&ishdw != &oshdw) {
                    if (ishdw.m_nType != oshdw.m_nType) {
                        continue;
                    }
                    if (ishdw.m_pTexture != oshdw.m_pTexture) {
                        continue;
                    }
                }

                CWorld::IncrementCurrentScanCode();

                const auto ishdwRect = GetShadowRect(ishdw);

                // Do shadow casting
                CWorld::IterateSectorsOverlappedByRect(
                        ishdwRect,
                        [&] (int32 x, int32 y) -> bool {
                            const auto sector = GetSector(x, y);

                            if (const auto rtshdw = ishdw.m_pRTShadow) {
                                CastRealTimeShadowSectorList(
                                        &sector->m_buildings,
                                        ishdwRect.left, ishdwRect.bottom,
                                        ishdwRect.right, ishdwRect.top,
                                        &ishdw.m_vecPosn,
                                        ishdw.m_Front.x, ishdw.m_Front.y,
                                        ishdw.m_Side.x, ishdw.m_Side.y,
                                        ishdw.m_nIntensity,
                                        ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                        ishdw.m_fZDistance,
                                        ishdw.m_fScale,
                                        nullptr,
                                        rtshdw,
                                        nullptr // Unused
                                );
                            } else {
                                uint8 unused{};
                                if (ishdw.m_bDrawOnBuildings) {
                                    CastShadowSectorList(
                                            &sector->m_buildings,
                                            ishdwRect.left, ishdwRect.bottom,
                                            ishdwRect.right, ishdwRect.top,
                                            &ishdw.m_vecPosn,
                                            ishdw.m_Front.x, ishdw.m_Front.y,
                                            ishdw.m_Side.x, ishdw.m_Side.y,
                                            ishdw.m_nIntensity,
                                            ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                            ishdw.m_fZDistance,
                                            ishdw.m_fScale,
                                            nullptr,
                                            &unused,
                                            ishdw.m_nType
                                    );
                                } else {
                                    CastPlayerShadowSectorList(
                                            &sector->m_buildings,
                                            ishdwRect.left, ishdwRect.bottom,
                                            ishdwRect.right, ishdwRect.top,
                                            &ishdw.m_vecPosn,
                                            ishdw.m_Front.x, ishdw.m_Front.y,
                                            ishdw.m_Side.x, ishdw.m_Side.y,
                                            ishdw.m_nIntensity,
                                            ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                            ishdw.m_fZDistance,
                                            ishdw.m_fScale,
                                            nullptr,
                                            &unused,
                                            ishdw.m_nType
                                    );
                                }
                            }
                            return true; // Inisde lambda -> Continue sector loop
                        }
                );

                // Mark this as rendered
                ishdw.m_bAlreadyRenderedInBatch = true;
            }

            // Render out shadows (Can't batch together with next iteration, as renderstates change)
            RenderBuffer::RenderStuffInBuffer();
        }

        // Render onto water (If needed)
        if (oshdw.m_bDrawOnWater) {
            float waterLevelZ{}, bigWavesZ{}, smallWavesZ{};
            if (!CWaterLevel::GetWaterLevelNoWaves(oshdw.m_vecPosn, &waterLevelZ, &bigWavesZ, &smallWavesZ)) {
                continue;
            }

            // Shadow under water?
            if (waterLevelZ >= oshdw.m_vecPosn.z) {
                continue;
            }

            // Clear the buffer as this is a new render pass (separate from the other)
            RenderBuffer::ClearRenderBuffer();

            // The smaller this value the detailed the shadows, but also (exponentially?) slower
            // This value basically represents the side of a square that is projected onto the
            const auto STEP_SIZE = 2;

            const auto shdwSideMagSq = oshdw.m_Side.SquaredMagnitude();
            const auto shdwFrontMagSq = oshdw.m_Front.SquaredMagnitude();

            const auto oshdwRect = GetShadowRect(oshdw);

            // Iterate shadow sectors (Using `CWorld::IterateSectors` for convenience)
            CWorld::IterateSectors(
                    (int32)(std::floor(oshdwRect.left / (float)STEP_SIZE)),
                    (int32)(std::floor(oshdwRect.top / (float)STEP_SIZE)),
                    (int32)(std::ceil(oshdwRect.right / (float)STEP_SIZE)),
                    (int32)(std::ceil(oshdwRect.bottom / (float)STEP_SIZE)),
                    [&](int32 ox, int32 oy) {
                        // 0x70B000 - 0x70B0DA: Start storing
                        RwIm3DVertex* vtxIt{};
                        RwImVertexIndex* vtxIdxIt{};
                        RenderBuffer::StartStoring(6, 4, vtxIdxIt, vtxIt);

                        // Function to process 1 vertex (out of the 4)
                        const auto ProcessOneVertex = [&] (int32 x, int32 y) {
                            // The x, y passed in are sector coords, so make them into world coords
                            x *= STEP_SIZE;
                            y *= STEP_SIZE;

                            const auto currPos = CVector2D{ (float)x, (float)y };

                            // Set color
                            RwIm3DVertexSetRGBA(vtxIt, oshdw.m_nRed, oshdw.m_nGreen, oshdw.m_nBlue, (uint8)((float)oshdw.m_nIntensity * 0.6f));

                            // Set texture coords (UV)
                            const auto CalcTexCoord  = [
                                    currPosToShdw = CVector2D{ currPos - oshdw.m_vecPosn }
                            ](CVector2D v, float vmagsq) {
                                return (currPosToShdw.Dot(v) / vmagsq + 1.f) * 0.5f;
                            };
                                    RwIm3DVertexSetU(vtxIt, CalcTexCoord(oshdw.m_Side, shdwSideMagSq));
                                    RwIm3DVertexSetV(vtxIt, CalcTexCoord(oshdw.m_Front, shdwFrontMagSq));

                            // Set position
                            RwIm3DVertexSetPos(
                                    vtxIt,
                                    currPos.x,
                                    currPos.y,
                                    CWaterLevel::CalculateWavesOnlyForCoordinate2_Direct(x, y, waterLevelZ, bigWavesZ, smallWavesZ) + 0.06f // Z pos of the wave at the given coord
                            );
                        };

                        // Process 4 vertices of this square
                        ProcessOneVertex(ox,     oy    ); // top left
                        ProcessOneVertex(ox + 1, oy    ); // top right
                        ProcessOneVertex(ox,     oy + 1); // bottom left
                        ProcessOneVertex(ox + 1, oy + 1); // bottom right

                        // Copy vertices into index buffer
                        //rng::copy(std::to_array({ 0, 1, 2, 1, 3, 2 }), vtxIdxIt);

                        // And we're done with this one... onto the next!
                        RenderBuffer::StopStoring();

                        // Continue iteration...
                        return true;
                    }
            );

            // Render it all
            RenderBuffer::RenderStuffInBuffer();
        }
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSWRAP));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));

    ShadowsStoredToBeRendered = 0;
}

void CShadows::CastRealTimeShadowSectorList(CPtrList* ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, CRealTimeShadow* realTimeShadow, uint8* pDayNightIntensity) {
    CHook::CallFunction<void>("_ZN8CShadows28CastRealTimeShadowSectorListER8CPtrListffffP7CVectorffffshhhffPP10CPolyBunchP15CRealTimeShadowPh", ptrList, conrerAX, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, ppPolyBunch, realTimeShadow, zDistance, scale, pDayNightIntensity);
}

void CShadows::CastShadowSectorList(CPtrList* ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    CHook::CallFunction<void>("_ZN8CShadows20CastShadowSectorListER8CPtrListffffP7CVectorffffshhhffPP10CPolyBunchPhi", ptrList, conrerAX, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, pDayNightIntensity, shadowType);
}

void CShadows::CastPlayerShadowSectorList(CPtrList* ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    const CRect shadowRect{
            conrerAX, cornerAY,
            cornerBX, cornerBY
    };

    for (CPtrNode* it = ptrList->m_node, *next{}; it; it = next) {
        next = it->GetNext();

        auto* entity = reinterpret_cast<CEntity*>(it->m_item);

        if (entity->IsScanCodeCurrent()) {
            continue;
        }
        entity->SetCurrentScanCode();

        if (!entity->m_bUsesCollision || entity->m_bDontCastShadowsOn) {
            continue;
        }

        //if (!entity->IsInCurrentAreaOrBarberShopInterior()) {
        //    continue;
        //}

        // If slightly tilted, ignore
        if (entity->GetMatrix().GetUp().z <= 0.97f) {
            continue;
        }

        if (!entity->GetBoundRect().OverlapsWith(shadowRect)) {
            continue;
        }

        // Quick Z height check of the bounding box
        const auto& cm = entity->GetColModel();
        const auto  entityPosZ = entity->GetPosition().z;
        if (cm->m_boundBox.m_vecMax.z + entityPosZ <= posn->z - zDistance) {
            continue;
        }
        if (cm->m_boundBox.m_vecMin.z + entityPosZ >= posn->z) {
            continue;
        }

        CastShadowEntityXY(
                entity,
                conrerAX,
                cornerAY,
                cornerBX,
                cornerBY,
                posn,
                frontX,
                frontY,
                sideX,
                sideY,
                intensity,
                red,
                green,
                blue,
                zDistance,
                scale,
                ppPolyBunch,
                pDayNightIntensity,
                shadowType
        );
    }
}

void CShadows::CastShadowEntityXY(CEntity* entity, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    CHook::CallFunction<void>("_ZN8CShadows18CastShadowEntityXYEP7CEntityffffP7CVectorffffshhhffPP10CPolyBunchPhi", entity, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, pDayNightIntensity, shadowType);
}

void CShadows::AffectColourWithLighting(
        eShadowType shadowType,
        uint8 dayNightIntensity, // packed 2x4 bits for day/night
        uint8 r, uint8 g, uint8 b,
        uint8& outR, uint8& outG, uint8& outB
) {
    if (shadowType != SHADOW_ADDITIVE) {
        const auto mult = std::min(
                0.4f + 0.6f * (1.f - CCustomBuildingDNPipeline::m_fDNBalanceParam),
                0.3f + 0.7f * lerp(
                        (float)(dayNightIntensity >> 0 & 0b1111) / 30.f,
                        (float)(dayNightIntensity >> 4 & 0b1111) / 30.f,
                        CCustomBuildingDNPipeline::m_fDNBalanceParam
                )
        );
        outR = (uint8)((float)r * mult);
        outG = (uint8)((float)g * mult);
        outB = (uint8)((float)b * mult);
    } else {
        outR = r;
        outG = g;
        outB = b;
    }
}

void CShadows::CalcPedShadowValues(CVector sunPosn, float& displacementX, float& displacementY, float& frontX, float& frontY, float& sideX, float& sideY) {
    const auto sunDist = sunPosn.Magnitude2D();
    const auto recip = 1.0f / sunDist;
    const auto mult = (sunDist + 1.0f) * recip;

    displacementX = -sunPosn.x * mult / 2.0f;
    displacementY = -sunPosn.y * mult / 2.0f;

    frontX = -sunPosn.y * recip / 2.0f;
    frontY = +sunPosn.x * recip / 2.0f;

    sideX = -sunPosn.x / 2.0f;
    sideY = -sunPosn.y / 2.0f;
}

void CShadows::StoreCarLightShadow(CVehicle* vehicle, int32 id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, uint8 red, uint8 green, uint8 blue, float maxViewAngleCosine) {
    auto needTex = texture;

    // Maximum distance (from camera to `posn`) after which shadows aren't stored (and rendered)
    constexpr auto MAX_CAM_TO_LIGHT_DIST = 35.f;

    // FIXME: move code to DoHeadLightReflectionTwin ( need reverse )

    auto pVeh = CVehiclePool::FindVehicle(vehicle);
    if (pVeh) {
        pVeh->ProcessHeadlightsColor(red, green, blue);
        red     /= 4;
        green   /= 4;
        blue    /= 4;

        if (pVeh->m_bIsLightOn == eLightsState::HIGH)
            needTex = gpShadowHeadLightsTexLong;
    }

    if ([] { // Maybe ignore camera distance?
        switch (CCamera::GetActiveCamera().m_nMode) {
            case MODE_TOPDOWN:
            case MODE_TOP_DOWN_PED:
                return false;
        }

        return true;
    }()) {
        const auto shdwToCam2D       = CVector2D{ CCamera::Get().GetPosition() - *posn };
        const auto shdwToCamDist2DSq = shdwToCam2D.SquaredMagnitude();

        if (shdwToCamDist2DSq >= sq(MAX_CAM_TO_LIGHT_DIST)) {
            return;
        }

        // Check if the camera is facing the lights closely (in which case the camera can't see the shadow)
        if (shdwToCam2D.Dot(CCamera::Get().GetFrontNormal2D()) > maxViewAngleCosine) {
            return;
        }

        // If far enough from the camera, start fading out
        if (const auto dist = std::sqrt(shdwToCamDist2DSq); dist >= MAX_CAM_TO_LIGHT_DIST * 0.75f) {
            const auto t = 1.f - invLerp(MAX_CAM_TO_LIGHT_DIST * (2.f / 3.f), MAX_CAM_TO_LIGHT_DIST, dist);
            red   = (uint8)((float)red * t);
            green = (uint8)((float)green * t);
            blue  = (uint8)((float)blue * t);
        }
    }

    StoreStaticShadow(
            reinterpret_cast<uintptr>(vehicle) + id,
            SHADOW_ADDITIVE,
            needTex,
            posn,
            frontX, frontY,
            sideX, sideY,
            128,
            red, green, blue,
            6.f,
            1.f,
            0.f,
            false,
            0.05f
    );
}

void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, const CVector& posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    if (ShadowsStoredToBeRendered >= asShadowsStored.size()) {
        DLOG("asShadowsStored to be full");
        return;
    }

    auto& shadow = asShadowsStored[ShadowsStoredToBeRendered];

    shadow.m_nType              = (eShadowType)type;
    shadow.m_pTexture           = texture;
    shadow.m_vecPosn            = posn;
    shadow.m_Front.x            = topX;
    shadow.m_Front.y            = topY;
    shadow.m_Side.x             = rightX;
    shadow.m_Side.y             = rightY;
    shadow.m_nIntensity         = intensity;
    shadow.m_nRed               = red;
    shadow.m_nGreen             = green;
    shadow.m_nBlue              = blue;
    shadow.m_fZDistance         = zDistance;
    shadow.m_bDrawOnWater       = drawOnWater;
    shadow.m_bDrawOnBuildings   = drawOnBuildings;
    shadow.m_fScale             = scale;
    shadow.m_pRTShadow          = realTimeShadow;

    ShadowsStoredToBeRendered++;
}

void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, CVector* posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    CHook::CallFunction<void>("_ZN8CShadows23StoreShadowToBeRenderedEhP9RwTextureP7CVectorffffshhhfbfP15CRealTimeShadowb", type, texture, posn, topX, topY, rightX, rightY, intensity, red, green, blue, zDistance, drawOnWater, scale, realTimeShadow, drawOnBuildings);
}

void CShadows::Init() {
    CHook::CallFunction<void>("_ZN8CShadows4InitEv");
    /*CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    gpShadowCarTex              = RwTextureRead("shad_car",     nullptr);
    gpShadowPedTex              = RwTextureRead("shad_ped",     nullptr);
    gpShadowHeliTex             = RwTextureRead("shad_heli",    nullptr);
    gpShadowBikeTex             = RwTextureRead("shad_bike",    nullptr);
    gpShadowBaronTex            = RwTextureRead("shad_rcbaron", nullptr);
    gpShadowExplosionTex        = RwTextureRead("shad_exp",     nullptr);
    gpShadowHeadLightsTex       = RwTextureRead("headlight",    nullptr);
    gpShadowHeadLightsTexLong   = RwTextureRead("headlight_l",  nullptr);
    gpShadowHeadLightsTex2      = RwTextureRead("headlight1",   nullptr);
    gpBloodPoolTex              = RwTextureRead("bloodpool_64", nullptr);
    gpHandManTex                = RwTextureRead("handman",      nullptr);
    gpCrackedGlassTex           = RwTextureRead("wincrack_32",  nullptr);
    gpPostShadowTex             = RwTextureRead("lamp_shad_64", nullptr);

    CTxdStore::PopCurrentTxd();

    std::ranges::for_each(aStaticShadows, [&](auto& shadow) { shadow.Init(); });

    pEmptyBunchList = aPolyBunches.data();

    for (auto i = 0u; i < aPolyBunches.size() - 1; i++) {
        aPolyBunches[i].m_pNext = &aPolyBunches[i + 1];
    }
    aPolyBunches.back().m_pNext = nullptr;

    std::ranges::for_each(aPermanentShadows, [&](auto& shadow) { shadow.Init(); });*/
}

void CShadows::StoreShadowToBeRendered(eShadowType type, RwTexture* tex, const CVector& posn, CVector2D top, CVector2D right, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    StoreShadowToBeRendered(
            type,
            tex,
            posn,
            top.x, top.y,
            right.x, right.y,
            intensity,
            red, green, blue,
            zDistance,
            drawOnWater,
            scale,
            realTimeShadow,
            drawOnBuildings
    );
}

void CShadows::StoreShadowToBeRendered(uint8 type, const CVector& posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue) {
    const auto Store = [=](auto mtype, auto texture) {
        StoreShadowToBeRendered(mtype, texture, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, 15.0f, 0, 1.0f, nullptr, 0);
    };

    switch (type) {
        case SHADOW_DEFAULT:
            Store(SHADOW_TEX_CAR, gpShadowCarTex);
            break;
        case SHADOW_ADDITIVE:
            Store(SHADOW_TEX_CAR, gpShadowPedTex);
            break;
        case SHADOW_INVCOLOR:
            Store(SHADOW_TEX_PED, gpShadowExplosionTex);
            break;
        case SHADOW_OIL_1:
            Store(SHADOW_TEX_CAR, gpShadowHeliTex);
            break;
        case SHADOW_OIL_2:
            Store(SHADOW_TEX_PED, gpShadowHeadLightsTex);
            break;
        case SHADOW_OIL_3:
            Store(SHADOW_TEX_CAR, gpBloodPoolTex);
            break;
        default:
            return;
    }
}

void CStaticShadow::Free() {
    if (m_pPolyBunch) {
        const auto prevHead = CShadows::pEmptyBunchList;
        CShadows::pEmptyBunchList = m_pPolyBunch;

        // Find last in the list and make it point to the previous head
        auto it{ m_pPolyBunch };
        while (it->m_pNext) {
            it = static_cast<CPolyBunch*>(it->m_pNext);
        }
        it->m_pNext = prevHead;

        m_pPolyBunch = nullptr;
        m_nId = 0;
    }
}

void CShadows::Shutdown() {
    RwTextureDestroy(gpShadowCarTex);
    RwTextureDestroy(gpShadowPedTex);
    RwTextureDestroy(gpShadowHeliTex);
    RwTextureDestroy(gpShadowBikeTex);
    RwTextureDestroy(gpShadowBaronTex);
    RwTextureDestroy(gpShadowExplosionTex);
    RwTextureDestroy(gpShadowHeadLightsTex);
    RwTextureDestroy(gpShadowHeadLightsTex2);
    RwTextureDestroy(gpBloodPoolTex);
    RwTextureDestroy(gpHandManTex);
    RwTextureDestroy(gpCrackedGlassTex);
    RwTextureDestroy(gpPostShadowTex);
}

void CShadows::TidyUpShadows() {
    for (auto& shadow : aPermanentShadows) {
        shadow.m_nType = SHADOW_NONE;
    }
}

void CShadows::SetRenderModeForShadowType(eShadowType type) {
    switch (type) {
        case SHADOW_DEFAULT:
        case SHADOW_OIL_1:
        case SHADOW_OIL_2:
        case SHADOW_OIL_3:
            /* case SHADOW_OIL_4: */ // missing
        case SHADOW_OIL_5:
            RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            break;
        case SHADOW_ADDITIVE:
            RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
            break;
        case SHADOW_INVCOLOR:
            RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDZERO));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCCOLOR));
            break;
        default:
            return;
    }
}

void CShadows::RemoveOilInArea(float x1, float x2, float y1, float y2) {
    CRect rect{ {x1, y1}, {x2, y2} };
    for (auto& shadow : aPermanentShadows) {
        switch (shadow.m_nType) {
            case SHADOW_OIL_1:
            case SHADOW_OIL_5:
                break;
            default:
                continue;
        }
        if (rect.IsPointInside(shadow.m_vecPosn)) {
            shadow.m_nType = SHADOW_NONE;
        }
    }
}

void CShadows::UpdateStaticShadows() {
    // Remove shadows that have no polies/are temporary and have expired
    for (auto& sshdw : aStaticShadows) {
        if (!sshdw.m_pPolyBunch || sshdw.m_bJustCreated) {
            goto skip; // Not even created fully
        }

        if (sshdw.m_bTemporaryShadow && CTimer::GetTimeInMS() <= sshdw.m_nTimeCreated + 5000u) {
            goto skip; // Not expired yet
        }

        sshdw.Free();

        skip:
        sshdw.m_bJustCreated = false;
    }
}

bool CShadows::StoreStaticShadow(uint32 id, eShadowType type, RwTexture* texture, const CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistane, float scale, float drawDistance, bool temporaryShadow, float upDistance) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x005B8D38 + 1 : 0x6DD6A4), id, type, texture, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistane, scale, drawDistance, temporaryShadow, upDistance);
}

uint16 CalculateShadowStrength(float currDist, float maxDist, uint16 maxStrength) {
    const auto halfMaxDist = maxDist / 2.f;
    if (currDist >= halfMaxDist) { // Anything further than half the distance is faded out
        return (uint16)((1.f - (currDist - halfMaxDist) / halfMaxDist) * maxStrength);
    } else { // Anything closer than half the max distance is full strength
        return (uint16)maxStrength;
    }
}

void CShadows::StoreRealTimeShadow(CPhysical* physical, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    const auto rtshdw = physical->m_pShadowData;
    if (!rtshdw) {
        return;
    }

    const auto& camPos = CCamera::Get().GetPosition();
    const auto  shdwPos = physical->IsPed()
                          ? physical->AsPed()->GetBonePosition(BONE_NORMAL)
                          : physical->GetPosition();
    const auto shdwToCamDist2DSq = (shdwPos - camPos).SquaredMagnitude2D();

    // Check distance to camera
    if (shdwToCamDist2DSq > MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    // Check if the object is visible to the camera
    if (FindPlayerPed() != physical) {  // Optimization: Assume player ped is always visible
        if (!CCamera::Get().IsSphereVisible(&shdwPos, 2.f)) {
            return;
        }
    }

    const auto strength = (float)CalculateShadowStrength(std::sqrt(shdwToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    const auto cc       = (uint8)((float)rtshdw->m_nIntensity / 100.f * strength);

    const auto& vecToSun = CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue];
    const auto lightFrame = rtshdw->SetLightProperties(
            RWRAD2DEG(+std::atan2(-vecToSun.x, -vecToSun.y)),
            RWRAD2DEG(-std::atan2(+vecToSun.x, -vecToSun.z)),
            true
    );
    CalcPedShadowValues(
            *RwMatrixGetAt(RwFrameGetMatrix(lightFrame)),
            displacementX, displacementY,
            frontX, frontY,
            sideX, sideY
    );
    CVector vector = shdwPos - CVector{ displacementX, displacementY, 0.f } * 2.5f;
    StoreShadowToBeRendered(
            SHADOW_INVCOLOR,
            CHook::CallFunction<RwTexture*>("_ZN15CRealTimeShadow18GetShadowRwTextureEv", rtshdw),
            &vector,
            frontX * 1.5f, frontY * 1.5f,
            sideX * 1.5f, sideY * 1.5f,
            cc,
            cc, cc, cc,
            4.f,
            false,
            1.f,
            rtshdw,
            false
    );
}

void (*CShadows_StoreRealTimeShadow)(CPhysical* physical, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY);
void CShadows_StoreRealTimeShadow_hook(CPhysical* physical, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    const auto& camPos = CCamera::Get().GetPosition();
    const auto  shdwPos = physical->IsPed()
                          ? physical->AsPed()->GetBonePosition(BONE_NORMAL)
                          : physical->GetPosition();
    const auto shdwToCamDist2DSq = (shdwPos - camPos).SquaredMagnitude2D();

    // Check distance to camera
    if (shdwToCamDist2DSq > MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    CShadows_StoreRealTimeShadow(physical, displacementX, displacementY, frontX, frontY, sideX, sideY);
}

void CShadows::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677BE4 : 0x84D7F8), &asShadowsStored);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679914 : 0x851250), &ShadowsStoredToBeRendered);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676F38 : 0x84BEC8), &pEmptyBunchList);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678F88 : 0x84FF40), &aPolyBunches);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6798E8 : 0x8511F8), &aStaticShadows);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679FE0 : 0x851FD8), &aPermanentShadows);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x679D44 : 0x851AA0), &gpShadowPedTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677130 : 0x84C2B0), &gpShadowHeadLightsTex2);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678BE8 : 0x84F800), &gpShadowExplosionTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6784F0 : 0x84EA08), &gpShadowCarTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6798C8 : 0x8511B8), &gpShadowHeliTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679020 : 0x850070), &gpShadowBikeTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678A38 : 0x84F498), &gpShadowBaronTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x675FF8 : 0x84A078), &gpShadowHeadLightsTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6769D4 : 0x84B408), &gpBloodPoolTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676128 : 0x84A2D0), &gpHandManTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678000 : 0x84E030), &gpCrackedGlassTex);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x675F64 : 0x849F50), &gpPostShadowTex);

    CHook::Redirect("_ZN8CShadows19StoreCarLightShadowEP8CVehicleiP9RwTextureP7CVectorffffhhhf", &StoreCarLightShadow);
    CHook::Redirect("_ZN8CShadows19RenderStaticShadowsEb", &RenderStaticShadows);
}

void CShadows::StoreShadowForPedObject(CPed* ped, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    assert(ped->IsPed() || ped->IsObject());

    const auto  bonePos = ped->GetBonePosition(ePedBones::BONE_NORMAL);
    const auto& camPos = CCamera::Get().GetPosition();
    const auto  boneToCamDist2DSq = (bonePos - camPos).SquaredMagnitude2D();

    // Check if ped is close enough
    if (boneToCamDist2DSq >= MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    const auto isPlayerPed = FindPlayerPed() == ped;

    // Check if ped is visible to the camera
    if (!isPlayerPed) {  // Optimization: Assume player ped is always visible
        if (!CCamera::Get().IsSphereVisible(&ped->GetPosition(), 2.f)) {
            return;
        }
    }

    // Now store a shadow to be rendered
    const auto strength = (uint8)CalculateShadowStrength(std::sqrt(boneToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    StoreShadowToBeRendered(
            SHADOW_DEFAULT,
            gpShadowPedTex,
            bonePos + CVector{displacementX, displacementY, 0.f},
            frontX, frontY,
            sideX, sideY,
            strength,
            strength, strength, strength,
            4.f,
            false,
            1.f,
            nullptr,
            false
    );
}

void CShadows::StoreShadowForPole(CEntity* entity, float offsetX, float offsetY, float offsetZ, float poleHeight, float poleWidth, uint32 localId) {
    const auto& mat = entity->GetMatrix();
    if (mat.GetUp().z < .5f) { // More than 45 deg tilted
        return;
    }

    const auto intensity = 2.f * (mat.GetUp().z - 0.5f) * (float)(CTimeCycle::m_CurrentColours.m_nPoleShadowStrength);

    const auto front     = CVector2D{ CTimeCycle::GetVectorToSun() } * (-poleHeight / 2.f);
    const auto right     = CVector2D{ CTimeCycle::GetShadowSide() } * poleWidth;
    CVector posn = mat.GetPosition() + CVector{front, 0.f } + CVector{
            offsetX * mat.GetRight().x + offsetY * mat.GetForward().x, // Simplified matrix transform (Ignoring the Z axis)
            offsetX * mat.GetRight().y + offsetY * mat.GetForward().y, // >^^^
            offsetZ};

    StoreStaticShadow(
            reinterpret_cast<uintptr>(&entity->m_pLod) + localId + 3,
            SHADOW_DEFAULT,
            gpPostShadowTex,
            &posn,
            front.x, front.y,
            right.x, right.y,
            2 * (int16)intensity / 3,
            0, 0, 0,
            9.f,
            1.f,
            40.f,
            false,
            0.f
    );
}
