//
// Created on 18.10.2023.
//

#include "Shadows.h"
#include "Camera.h"
#include "util/patch.h"
#include "net/netgame.h"
#include "TxdStore.h"
#include "Core/Rect.h"
#include "RenderBuffer.h"
#include "Weather.h"
#include "game/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "TimeCycle.h"

void CShadows::RenderStaticShadows() {
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
            // No need to check if this one was rendered (because of the batching)

            // Render polies of this shadow
            for (auto poly = ishdw.m_pPolyBunch; poly; poly = poly->m_pNext) {
                // 0x70841F: Calculate color
                uint8 r, g, b;
                CShadows::AffectColourWithLighting(
                        ishdw.m_nType,
                        ishdw.m_nDayNightIntensity,
                        ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                        r, g, b
                );

                const auto totalNoIdx = 3 * (poly->m_wNumVerts - 2); // Total no. of indices we'll use

                // 0x708432: Begin render buffer store
                RwIm3DVertex*    vtxIt{};
                RwImVertexIndex* vtxIdxIt{};
                RenderBuffer::StartStoring(
                        totalNoIdx,
                        poly->m_wNumVerts,
                        vtxIdxIt, vtxIt
                );

                // 0x70851D: Write vertices (`if` not necessary, it's part of the loop condition)
                const auto a = (uint8)((float)ishdw.m_nIntensity * (1.f - CWeather::Foggyness * 0.5f));
                for (auto i{ 0 }; i < poly->m_wNumVerts; i++, vtxIt++) {
                    const auto& pos = poly->m_avecPosn[i];
                    RwIm3DVertexSetPos(vtxIt, pos.x, pos.y, pos.z + 0.06f);
                    RwIm3DVertexSetRGBA(vtxIt, r, g, b, a);
                            RwIm3DVertexSetU(vtxIt, (float)poly->m_aU[i] / 200.f);
                            RwIm3DVertexSetV(vtxIt, (float)poly->m_aV[i] / 200.f);
                }

                // 0x7085BC: Write indices  (`if` not necessary, it's part of the loop condition)
                for (auto i = 0; i < totalNoIdx; i++) {
                    *vtxIdxIt++ = g_ShadowVertices[i];
                }

                // Finish storing
                RenderBuffer::StopStoring();

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

    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

    // Maximum distance (from camera to `posn`) after which shadows aren't stored (and rendered)
    constexpr auto MAX_CAM_TO_LIGHT_DIST = 35.f;

    // FIXME: move code to DoHeadLightReflectionTwin ( need reverse )
    uint16_t vehid = CVehiclePool::FindIDFromGtaPtr(vehicle);
    CVehicleSamp* pVeh = CVehiclePool::GetAt(vehid);
    if (pVeh)
    {
        pVeh->ProcessHeadlightsColor(red, green, blue);

        // !TODO: tex
//        if(pVeh->m_bIsLightOn == eLightsState::HIGH)
//            needTex = gpShadowHeadLightsTexLong;

    }

    if ([] { // Maybe ignore camera distance?
        switch (CCamera::GetActiveCamera().m_nMode) {
            case MODE_TOPDOWN:
            case MODE_TOP_DOWN_PED:
                return false;
        }

        return true;
    }()) {
        const auto shdwToCam2D       = CVector2D{ TheCamera.GetPosition() - *posn };
        const auto shdwToCamDist2DSq = shdwToCam2D.SquaredMagnitude();

        if (shdwToCamDist2DSq >= sq(MAX_CAM_TO_LIGHT_DIST)) {
            return;
        }

        // Check if the camera is facing the lights closely (in which case the camera can't see the shadow)
        if (shdwToCam2D.Dot(TheCamera.GetFrontNormal2D()) > maxViewAngleCosine) {
            return;
        }

        // If far enough from the camera, start fading out
//        if (const auto dist = std::sqrt(shdwToCamDist2DSq); dist >= MAX_CAM_TO_LIGHT_DIST * 0.75f) {
//            const auto t = 1.f - invLerp(MAX_CAM_TO_LIGHT_DIST * (2.f / 3.f), MAX_CAM_TO_LIGHT_DIST, dist);
//            red   = (uint8)((float)red * t);
//            green = (uint8)((float)green * t);
//            blue  = (uint8)((float)blue * t);
//        }
        auto t = 0.49f;
        red   = (uint8)((float)red * t);
        green = (uint8)((float)green * t);
        blue  = (uint8)((float)blue * t);
    }

//    const auto isPlyrVeh = CLocalPlayer::m_pPlayerPed->m_pPed->pVehicle == vehicle;
//    if (isPlyrVeh || vehicle->GetMoveSpeed().Magnitude() * CTimer::GetTimeStep() >= 0.4f) {
//        StoreShadowToBeRendered(
//                SHADOW_ADDITIVE,
//                needTex,
//                posn,
//                frontX, frontY,
//                sideX, sideY,
//                128,
//                red, green, blue,
//                6.f,
//                false,
//                1.f,
//                nullptr,
//                true
//              //  isPlyrVeh /*|| g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH*/ // NOTSA: At higher FX quality draw all vehicles's shadows on buildings too
//        );
//    } else {
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
   // }
}

void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, const CVector& posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    if (ShadowsStoredToBeRendered >= asShadowsStored.size())
        return;

    auto& shadow = asShadowsStored[ShadowsStoredToBeRendered];

    shadow.m_nType      = (eShadowType)type;
    shadow.m_pTexture   = texture;
    shadow.m_vecPosn    = posn;
    shadow.m_Front.x    = topX;
    shadow.m_Front.y    = topY;
    shadow.m_Side.x     = rightX;
    shadow.m_Side.y     = rightY;
    shadow.m_nIntensity = intensity;
    shadow.m_nRed       = red;
    shadow.m_nGreen     = green;
    shadow.m_nBlue      = blue;
    shadow.m_fZDistance = zDistance;
    shadow.m_bDrawOnWater     = drawOnWater;
    shadow.m_bDrawOnBuildings = drawOnBuildings;
    shadow.m_fScale     = scale;
    shadow.m_pRTShadow  = realTimeShadow;

    ShadowsStoredToBeRendered++;
}

void CShadows::Init() {
    CTxdStore::PushCurrentTxd();
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

    g_ShadowVertices = { 0, 2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 6, 5, 0, 7, 6, 0, 8, 7, 0, 9, 8 };

    for (auto& shadow : aStaticShadows) {
        shadow.Init();
    }

    pEmptyBunchList = aPolyBunches.data();

    for (auto i = 0u; i < aPolyBunches.size() - 1; i++) {
        aPolyBunches[i].m_pNext = &aPolyBunches[i + 1];
    }
    aPolyBunches.back().m_pNext = nullptr;

    for (auto& shadow : aPermanentShadows) {
        shadow.Init();
    }
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

void CShadows::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677BE4 : 0x84D7F8), &asShadowsStored);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x679914 : 0x851250), &ShadowsStoredToBeRendered);
//    CHook::Redirect("_ZN8CShadows4InitEv", &CShadows::Init);
    CHook::Redirect("_ZN8CShadows19StoreCarLightShadowEP8CVehicleiP9RwTextureP7CVectorffffhhhf", &StoreCarLightShadow);
}

void CShadows::StoreShadowForPole(CEntity* entity, float offsetX, float offsetY, float offsetZ, float poleHeight, float poleWidth, uint32 localId) {
//    if (GraphicsHighQuality() || !CTimeCycle::m_CurrentColours.m_nPoleShadowStrength) {
//        return;
//    }

    CHook::CallFunction<void>("_ZN8CShadows18StoreShadowForPoleEP7CEntityfffffj", entity, offsetX, offsetY, offsetZ, poleHeight, poleWidth, localId);
//    const auto& mat = entity->GetMatrix();
//
//    if (mat.GetUp().z < .5f) { // More than 45 deg tilted
//        return;
//    }
//
//    const auto intensity = 2.f * (mat.GetUp().z - 0.5f) * (float)(CTimeCycle::m_CurrentColours.m_nPoleShadowStrength);
//
//    const auto front     = CVector2D{ CTimeCycle::GetVectorToSun() } * (-poleHeight / 2.f);
//    const auto right     = CVector2D{ CTimeCycle::GetShadowSide() } * poleWidth;
//
//    StoreStaticShadow(
//            reinterpret_cast<uint32>(&entity->m_pLod) + localId + 3,
//            SHADOW_DEFAULT,
//            gpPostShadowTex,
//            mat.GetPosition() + CVector{ front, 0.f } + CVector{
//                    offsetX * mat.GetRight().x + offsetY * mat.GetForward().x, // Simplified matrix transform (Ignoring the Z axis)
//                    offsetX * mat.GetRight().y + offsetY * mat.GetForward().y, // >^^^
//                    offsetZ
//            },
//            front.x, front.y,
//            right.x, right.y,
//            2 * (int16)intensity / 3,
//            0, 0, 0,
//            9.f,
//            1.f,
//            40.f,
//            false,
//            0.f
//    );
}
