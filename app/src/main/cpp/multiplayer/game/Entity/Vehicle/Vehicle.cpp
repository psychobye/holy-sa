//
// Created on 18.04.2023.
//
#include "game/common.h"
#include "Vehicle.h"
#include "util/patch.h"
#include "game/World.h"
#include "game/Models/ModelInfo.h"
#include "game/Coronas.h"
#include "Camera.h"
#include "net/netgame.h"
#include "widget.h"
#include "Widgets/TouchInterface.h"
#include "CPad.h"
#include "Shadow/Shadows.h"
#include "tools/ModelsDebugModule.h"

void CVehicle::RenderDriverAndPassengers() {
    if(IsRCVehicleModelID())
        return;

    if (pDriver && pDriver->m_nPedState == PEDSTATE_DRIVING) {
        CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4A6964 + 1 : 0x59D3B8), pDriver);
       // pDriver->Render();
    }

    for (auto& passenger : m_apPassengers) {
        if (passenger && passenger->m_nPedState == PEDSTATE_DRIVING) {
            CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4A6964 + 1 : 0x59D3B8), passenger);
           // passenger->Render();
        }
    }
}

void CVehicle::SetDriver(CPed* driver) {
    CEntity::ChangeEntityReference(pDriver, driver);

    ApplyTurnForceToOccupantOnEntry(driver);
}

bool CVehicle::AddPassenger(CPed* passenger) {
    ApplyTurnForceToOccupantOnEntry(passenger);

    // Now, find a seat and place them into it
    const auto seats = GetMaxPassengerSeats();

    for(auto & emptySeat : m_apPassengers) {
        emptySeat = passenger;
        CEntity::RegisterReference(emptySeat);
        m_nNumPassengers++;
        return false;
    }

    // No empty seats
    return false;
}

bool CVehicle::AddPassenger(CPed* passenger, uint8 seatIdx) {
    if (m_nVehicleFlags.bIsBus) {
        return AddPassenger(passenger);
    }

    // Check if seat is valid
    if (seatIdx >= m_nMaxPassengers) {
        return false;
    }

    // Check if anyone is already in that seat
    if (m_apPassengers[seatIdx]) {
        return false;
    }

    // Place passenger into seat, and add ref
    m_apPassengers[seatIdx] = passenger;
    CEntity::RegisterReference(m_apPassengers[seatIdx]);
    m_nNumPassengers++;

    return true;
}

void CVehicle::ApplyTurnForceToOccupantOnEntry(CPed* passenger) {
    // Apply some turn force
    switch (m_nVehicleType) {
        case VEHICLE_TYPE_BIKE: {
            ApplyTurnForce(
                    GetUp() * passenger->m_fMass / -50.f,
                    GetForward() / -10.f // Behind the bike
            );
            break;
        }
        default: {
            ApplyTurnForce(
                    CVector{ .0f, .0f, passenger->m_fMass / -5.f },
                    CVector{ CVector2D{passenger->GetPosition() - GetPosition()}, 0.f }
            );
            break;
        }
    }
}

int CVehicle::GetPassengerIndex(const CPed* passenger) {
    for(int i = 0; i <  std::size(m_apPassengers); i++) {
        if(passenger == m_apPassengers[i])
            return i;
    }
    return -1;
}

void CVehicle::AddVehicleUpgrade(int32 modelId) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0058C66C + 1 : 0x6AFF4C), this, modelId);
}

void CVehicle::RemoveVehicleUpgrade(int32 upgradeModelIndex) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x58CC2C + 1 : 0x6B0718), this, upgradeModelIndex);
}

// 0x6D3000
void CVehicle::SetGettingInFlags(uint8 doorId) {
    m_nGettingInFlags |= doorId;
}

// 0x6D3020
void CVehicle::SetGettingOutFlags(uint8 doorId) {
    m_nGettingOutFlags |= doorId;
}

// 0x6D3040
void CVehicle::ClearGettingInFlags(uint8 doorId) {
    m_nGettingInFlags &= ~doorId;
}

// 0x6D3060
void CVehicle::ClearGettingOutFlags(uint8 doorId) {
    m_nGettingOutFlags &= ~doorId;
}

// ----------------------------------- hooks

void RenderDriverAndPassengers_hook(CVehicle *thiz)
{
    thiz->RenderDriverAndPassengers();
}

void SetDriver_hook(CVehicle *thiz, CPed *pPed)
{
    thiz->SetDriver(pPed);
}

bool CVehicle__GetVehicleLightsStatus_hook(CVehicle *pVehicle)
{
    return pVehicle->GetLightsStatus();
}

void (*CVehicle__DoVehicleLights)(CVehicle* thiz, CMatrix *matVehicle, uint32 nLightFlags);
void CVehicle__DoVehicleLights_hook(CVehicle* thiz, CMatrix *matVehicle, uint32 nLightFlags)
{
    uint8_t old = thiz->m_nVehicleFlags.bEngineOn;
    thiz->m_nVehicleFlags.bEngineOn = 1;
    CVehicle__DoVehicleLights(thiz, matVehicle, nLightFlags);
    thiz->m_nVehicleFlags.bEngineOn = old;
}

bool CVehicle::DoTailLightEffect(int32_t lightId, CMatrix* matVehicle, int isRight, int forcedOff, uint32_t nLightFlags, int lightsOn) {
    auto pModelInfoStart = CModelInfo::GetVehicleModelInfo(m_nModelIndex);

    CVector* m_avDummyPos = pModelInfoStart->m_pVehicleStruct->m_avDummyPos;

    auto v = CVector(m_avDummyPos[1]);

    if (!isRight)
        v.x = -v.x;

    uint8_t alpha = (m_fBreakPedal > 0) ? 200 : 96;
    if (GetLightsStatus() || (m_fBreakPedal > 0 && pDriver)) {
        CCoronas::RegisterCorona(
                (uintptr) &m_placement.m_vPosn.y + 2 * lightId + isRight,
                this,
                100, 0, 0, alpha,
                &v,
                0.65f,
                /*CCamera::Get().LODDistMultiplier*/ 70.f,
                eCoronaType::CORONATYPE_HEADLIGHT,
                eCoronaFlareType::FLARETYPE_HEADLIGHTS,
                false,
                false,
                0,
                0.0f,
                false,
                0,
                0,
                15.0f,
                false,
                false
        );
    }
    return true;
}

void CVehicle::DoHeadLightBeam(eVehicleDummy dummyId, CMatrix* matrix, bool isRight) {
    uint8_t r = 0xFF, g = 0xFF, b = 0xFF;

    auto* pVehicle = CVehiclePool::FindVehicle(this);
    if (pVehicle)
        pVehicle->ProcessHeadlightsColor(r, g, b);

    auto mi = CModelInfo::GetVehicleModelInfo(m_nModelIndex);
    CVector pointModelSpace = mi->GetModelDummyPosition(static_cast<eVehicleDummy>(2 * dummyId));
    if (dummyId == DUMMY_LIGHT_REAR_MAIN && pointModelSpace.IsZero())
        return;

    CVector point = matrix->GetPosition() + matrix->TransformVector(pointModelSpace);
    if (!isRight) {
        point -= 2 * pointModelSpace.x * matrix->GetRight();
    }
    const CVector pointToCamDir = Normalized(CCamera::Get().GetPosition() - point);
    const auto    alpha = (uint8)((1.0f - std::fabs(DotProduct(pointToCamDir, matrix->GetForward()))) * 45.0f);

    bool isHighBeam = pVehicle != nullptr && pVehicle->m_bIsLightOn == eLightsState::HIGH;
    const uint8 finalAlpha = isHighBeam ? std::min(255, alpha + 80) : alpha;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(FALSE));

    const float angleMult = isHighBeam ? 0.05f : 0.15f; // angle
    const float lengthMult = isHighBeam ? 5.0f : 3.0f; // length
    const float sideOffsetStart = isHighBeam ? 0.1f : 0.05; // width
    const float sideOffsetEnd   = isHighBeam ? 1.0f : 0.5; // width

    const CVector lightNormal = Normalized(matrix->GetForward() - matrix->GetUp() * angleMult);
    const CVector lightRight  = Normalized(CrossProduct(lightNormal, pointToCamDir));
    const CVector lightPos    = point - matrix->GetForward() * 0.1f;

    const CVector posn[] = {
            lightPos - lightRight * sideOffsetStart,
            lightPos + lightRight * sideOffsetStart,
            lightPos + lightNormal * lengthMult - lightRight * sideOffsetEnd,
            lightPos + lightNormal * lengthMult + lightRight * sideOffsetEnd,
            lightPos + lightNormal * 0.2f
    };
    const uint8 alphas[] = { finalAlpha, finalAlpha, 0, 0, finalAlpha };

    RxObjSpace3DVertex vertices[5];
    for (auto i = 0u; i < std::size(vertices); i++) {
        const RwRGBA color = { b, g, r, alphas[i] }; //!TODO FIX RGB

        RxObjSpace3DVertexSetPreLitColor(&vertices[i], &color);
        RxObjSpace3DVertexSetPos(&vertices[i], &posn[i]);
    }

    if (RwIm3DTransform(vertices, std::size(vertices), nullptr, rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ))
    {
        RxVertexIndex indices[] = { 0, 1, 4, 1, 3, 4, 2, 3, 4, 0, 2, 4 };
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, indices, std::size(indices));
        RwIm3DEnd();
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,           RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,              RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,             RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,     RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,              RWRSTATE(rwCULLMODECULLNONE));
}

void CVehicle::DoHeadLightReflectionTwin(CMatrix* matVehicle)
{
    auto* mi = CModelInfo::GetVehicleModelInfo(m_nModelIndex);
    if (!mi)
        return;

    CVector dummy = mi->GetModelDummyPosition(DUMMY_LIGHT_FRONT_MAIN);
    if (dummy.IsZero())
        return;

    CVector pos = matVehicle->GetPosition() + matVehicle->TransformVector(dummy);
    pos.z += 2.0f;

    CVector forward = matVehicle->GetForward();
    float xy = forward.x;
    float yy = forward.y;

    float len = xy * xy + yy * yy;
    if (len != 0.0f) len = 1.0f / sqrtf(len);

    float nx = xy * len;
    float ny = yy * len;

    float w = mi->GetColModel()->m_boundBox.m_vecMax.x * 2.0f;
    float offset = mi->GetColModel()->m_boundBox.m_vecMax.y + (w + w) - 2.0f;

    pos.x += offset * nx;
    pos.y += offset * ny;

    float leftOffset = 0.65f;
    pos.x += leftOffset * (-ny);
    pos.y += leftOffset * nx;

    auto* pVeh = CVehiclePool::FindVehicle(this);

    uint8 red = 0x2D, green = 0x2D, blue = 0x2D;

    if (pVeh) {
        pVeh->ProcessHeadlightsColor(red, green, blue);
        red     /= 4;
        green   /= 4;
        blue    /= 4;
    }

    auto* needTex = CShadows::gpShadowHeadLightsTex;
    if (pVeh && pVeh->m_bIsLightOn == eLightsState::HIGH)
        needTex = CShadows::gpShadowHeadLightsTexLong;

    CShadows::StoreCarLightShadow(
            this,
            reinterpret_cast<uintptr>(this) + 22,
            needTex,
            &pos,
            (w + w) * nx,
            (w + w) * ny,
            w * ny,
            -w * nx,
            red,
            green,
            blue,
            7.0f
    );
}

void CVehicle::DoSirenEffect(int32_t lightId, bool isRight) {
    if(!m_nVehicleFlags.bSirenOrAlarm) return;

    float v50, v213, zy, v1;

    switch (m_nModelIndex) {
        case 596:
        case 597:
        case 598:
            v50 = -0.7f; v213 = 0.7f; zy = -0.4f; v1 = 1.0f;
            break;
        case 407:
            v50 = -0.9f; v213 = 0.9f; zy = 3.2f; v1 = 1.3f;
            break;
        case 416:
            v50 = -0.6f; v213 = 0.6f; zy = 0.9f; v1 = 1.2f;
            break;
        case 427:
            v50 = -0.55f; v213 = 0.55f; zy = 1.1f; v1 = 1.4f;
            break;
        default:
            v50 = -0.7f; v213 = 0.7f; zy = -0.4f; v1 = 1.0f;
            break;
    }

    for (int i = 0; i < 4; ++i) {
        float v111 = 3.0f - float(i);
        int v112 = static_cast<int>(((i * 64u + CTimer::m_snTimeInMilliseconds) >> 8) & 3u);

        CVector SirenCoors;
        SirenCoors.x = ((v50 * v111) + (v213 * float(i))) * 0.33333f;
        SirenCoors.y = ((zy * v111) + (zy * float(i))) * 0.33333f;
        SirenCoors.z = ((v1 * v111) + (v1 * float(i))) * 0.33333f;

        if (v112 == 2) {
            CCoronas::RegisterCorona(
                    (uintptr)&m_placement.m_vPosn.y + 5 * lightId + isRight,
                    this,
                    255, 0, 0, 255,
                    &SirenCoors,
                    0.6f,
                    70.f,
                    eCoronaType::CORONATYPE_HEADLIGHT,
                    eCoronaFlareType::FLARETYPE_HEADLIGHTS,
                    false, false, 0, 0.0f, false, 0, 0, 15.0f, false, false
            );
        } else if (v112 == 0) {
            CCoronas::RegisterCorona(
                    (uintptr)&m_placement.m_vPosn.y + 5 * lightId + isRight,
                    this,
                    0, 0, 255, 255,
                    &SirenCoors,
                    0.6f,
                    70.f,
                    eCoronaType::CORONATYPE_HEADLIGHT,
                    eCoronaFlareType::FLARETYPE_HEADLIGHTS,
                    false, false, 0, 0.0f, false, 0, 0, 15.0f, false, false
            );
        }
    }
}

void CVehicle::ProcessSirenAndHorn(bool bHornAvailable)
{
    auto Pads = CPad::GetPad(0);
    auto sirenWidget = CTouchInterface::m_pWidgets[WIDGET_HORN];

    if (IsLawEnforcementVehicle() && sirenWidget) {
        if (CPad::HornJustDown(Pads) && sirenWidget->m_fTapHoldTime <= 0.2f) {
            m_nVehicleFlags.bSirenOrAlarm = !m_nVehicleFlags.bSirenOrAlarm;
        }
    }
    else if (bHornAvailable) {
        m_cHorn = CPad::GetHorn(Pads, true);
    }
}

bool DoTailLightEffect_hooked(CVehicle* vehicle, int32_t lightId, CMatrix* matVehicle, int isRight, int forcedOff, uint32_t nLightFlags, int lightsOn) {
    return vehicle->DoTailLightEffect(lightId, matVehicle, isRight, forcedOff, nLightFlags, lightsOn);
}

void DoHeadLightBeam_hooked(CVehicle* vehicle, eVehicleDummy dummyId, CMatrix* matrix, bool isRight) {
    vehicle->DoHeadLightBeam(dummyId, matrix, isRight);
}

void DoHeadLightReflectionTwin_hooked(CVehicle* vehicle, CMatrix* matVehicle) {
    return vehicle->DoHeadLightReflectionTwin(matVehicle);
}

void ProcessSirenAndHorn_hooked(CVehicle* vehicle, bool bHornAvailable) {
    return vehicle->ProcessSirenAndHorn(bHornAvailable);
}

void CVehicle::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x675F10 : 0x849EA8), &m_aSpecialColModel);

    CHook::Redirect("_ZN8CVehicle25RenderDriverAndPassengersEv", &RenderDriverAndPassengers_hook);
    CHook::Redirect("_ZN8CVehicle9SetDriverEP4CPed", &SetDriver_hook);

    CHook::Redirect("_ZN8CVehicle17DoTailLightEffectEiR7CMatrixhhjh", &DoTailLightEffect_hooked);

    // !TODO: reverse DoHeadLightReflectionSingle and DoHeadLightReflection
    CHook::Redirect("_ZN8CVehicle25DoHeadLightReflectionTwinER7CMatrix", &DoHeadLightReflectionTwin_hooked);

    // CHook::Redirect("_ZN8CVehicle19ProcessSirenAndHornEb", &ProcessSirenAndHorn_hooked);

    CHook::InlineHook("_ZN8CVehicle15DoVehicleLightsER7CMatrixj", &CVehicle__DoVehicleLights_hook, &CVehicle__DoVehicleLights);
    CHook::Redirect("_ZN8CVehicle22GetVehicleLightsStatusEv", &CVehicle__GetVehicleLightsStatus_hook);
    CHook::Redirect("_ZN8CVehicle15DoHeadLightBeamEiR7CMatrixh", &DoHeadLightBeam_hooked);
}

bool CVehicle::IsRCVehicleModelID() {
    switch (m_nModelIndex) {
        case 441:
        case 464:
        case 465:
        case 594:
        case 501:
        case 564:
            return true;

        default:
            break;
    }
    return false;
}

bool CVehicle::UsesSiren() {
    switch (m_nModelIndex) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_MRWHOOP:
            return true;
        case MODEL_RHINO:
            return false;
        default:
            return IsLawEnforcementVehicle() != false;
    }
}

bool CVehicle::IsLawEnforcementVehicle() const {
    switch (m_nModelIndex) {
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_FBIRANCH:
        case MODEL_COPBIKE:
        case MODEL_FBITRUCK:
        case MODEL_COPCARLA:
        case MODEL_COPCARSF:
        case MODEL_COPCARVG:
        case MODEL_COPCARRU:
        case MODEL_SWATVAN:
            return true;
        default:
            return false;
    }
}
