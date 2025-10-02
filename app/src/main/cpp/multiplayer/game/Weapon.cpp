//
// Created on 07.04.2023.
//

#include "Weapon.h"
#include "util/patch.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "World.h"
#include "Camera.h"
#include "Stats.h"
#include "game.h"
#include "net/netgame.h"
#include "HUD.h"

void CWeapon__Update(CWeapon* thiz, CPed* owner) {
    thiz->Update(owner);
}

bool CWeapon__Fire(CWeapon *thiz, CEntity *pEntity, CVector *pStartPosn, CVector *pBarrelPosn, CEntity *pTargetEnt, CVector *pTargetPosn, CVector *pAltPosn) {
    CHUD::updateAmmo();
    return thiz->Fire(pEntity, pStartPosn, pBarrelPosn, pTargetEnt, pTargetPosn, pAltPosn);
}

void CWeapon::InjectHooks() {
    CHook::Redirect("_ZN7CWeapon6UpdateEP4CPed", &CWeapon__Update);
    CHook::Redirect("_ZN7CWeapon4FireEP7CEntityP7CVectorS3_S1_S3_S3_", &CWeapon__Fire);
}

CWeapon::CWeapon(eWeaponType weaponType, uint32_t ammo) :
        m_nType{ weaponType },
        m_nTotalAmmo{ std::min<uint32>(ammo, 99'999) }
{
    Reload();
}

void CWeapon::Reload(CPed* owner) {
    if (!m_nTotalAmmo)
        return;

    uint32 ammo = GetWeaponInfo(owner).m_nAmmoClip;
    m_nAmmoInClip = std::min(ammo, m_nTotalAmmo);
}

CWeaponInfo& CWeapon::GetWeaponInfo(CPed* owner) const {
    return GetWeaponInfo(owner ? owner->GetWeaponSkill(GetType()) : eWeaponSkill::STD);
}

CWeaponInfo& CWeapon::GetWeaponInfo(eWeaponSkill skill) const {
    return *CWeaponInfo::GetWeaponInfo(GetType(), skill);
}

void CWeapon::StopWeaponEffect() {
    CHook::CallFunction<void>("_ZN7CWeapon16StopWeaponEffectEv", this);
}

void CWeapon::Update(CPed* owner) {
    const auto wi = &GetWeaponInfo(owner);
    const auto ao = &wi->GetAimingOffset();

    const auto ProcessReloadAudioIf = [&](auto Pred) {
        const auto ProcessOne = [&](uint32 delay, eAudioEvents ae) {
            if (Pred(delay, ae)) {
                CHook::CallFunction<void>("_ZN23CAEPedWeaponAudioEntity13AddAudioEventEi", &owner->m_PedWeaponAudioEntity, ae);
            }
        };
        ProcessOne(owner->bIsDucking ? ao->CrouchRLoadA : ao->RLoadA, AE_WEAPON_RELOAD_A);
        ProcessOne(owner->bIsDucking ? ao->CrouchRLoadB : ao->RLoadB, AE_WEAPON_RELOAD_B);
    };

    switch (m_nState) {
        case WEAPONSTATE_FIRING: {
            if (owner && notsa::contains({ WEAPON_SPAS12_SHOTGUN, WEAPON_SHOTGUN }, m_nType)) { // 0x73DBA5
                ProcessReloadAudioIf([this](uint32 rload, eAudioEvents ae) {
                    if (!rload) {
                        return false;
                    }
                    const auto nextShotEnd = m_nTimer + rload;
                    return CTimer::GetPreviousTimeInMS() < nextShotEnd && CTimer::GetTimeInMS() >= nextShotEnd;
                });
            }
            if (CTimer::GetTimeInMS() > m_nTimer) {
                m_nState = wi->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE || m_nTotalAmmo != 0
                           ? eWeaponState::WEAPONSTATE_READY
                           : eWeaponState::WEAPONSTATE_OUT_OF_AMMO;
            }
            break;
        }
        case WEAPONSTATE_RELOADING: {
            if (owner && m_nType < WEAPON_LAST_WEAPON) {
                const auto DoPlayAnimlessReloadAudio = [&] {
                    ProcessReloadAudioIf([
                                                 &,
                                                 shootDelta = m_nTimer - wi->GetWeaponReloadTime()
                                         ](uint32 rload, eAudioEvents ae) {
                        const auto audioTimeMs = rload + shootDelta;
                        return CTimer::GetPreviousTimeInMS() < audioTimeMs && CTimer::GetTimeInMS() >= audioTimeMs;
                    });
                };
                if (wi->flags.bReload && (!owner->IsPlayer() || !FindPlayerInfo().FastReload)) { // 0x73DCCE
                    auto animRLoad = RpAnimBlendClumpGetAssociation(
                            owner->m_pRwClump,
                            ANIM_ID_RELOAD //(wi->m_Flags & 0x1000) != 0 ? ANIM_ID_RELOAD : ANIM_ID_WALK // Always going to be `ANIM_ID_RELOAD`
                    );
                    if (!animRLoad) {
                        animRLoad = RpAnimBlendClumpGetAssociation(owner->m_pRwClump, wi->GetCrouchReloadAnimationID());
                    }
                    if (animRLoad) { // 0x73DD30
                        ProcessReloadAudioIf([&](uint32 rloadMs, eAudioEvents ae) {
                            const auto rloadS = (float)rloadMs / 1000.f;
                            return rloadS <= animRLoad->m_fCurrentTime && animRLoad->m_fCurrentTime - animRLoad->m_fTimeStep < rloadS;
                        });
                        if (CTimer::GetTimeInMS() > m_nTimer) {
                            if (animRLoad->GetTimeProgress() < 0.9f) {
                                m_nTimer = CTimer::GetTimeInMS();
                            }
                        }
                    } else if (owner->GetIntelligence()->GetTaskUseGun()) { // 0x73DDF9
                        if (CTimer::GetTimeInMS() > m_nTimer) {
                            m_nTimer = CTimer::GetTimeInMS();
                        }
                    } else { // 0x73DE16
                        DoPlayAnimlessReloadAudio();
                    }
                } else {
                    DoPlayAnimlessReloadAudio();
                }
            }
            if (CTimer::GetTimeInMS() > m_nTimer) {
                Reload(owner);
                m_nState = WEAPONSTATE_READY;
            }
            StopWeaponEffect();
            break;
        }
        case WEAPONSTATE_MELEE_MADECONTACT: {
            m_nState = WEAPONSTATE_READY;
            StopWeaponEffect();
            break;
        }
        default: {
            StopWeaponEffect();
            break;
        }
    }
}

float CWeapon::TargetWeaponRangeMultiplier(CEntity* victim, CEntity* weaponOwner) {
    if (!victim || !weaponOwner) {
        return 1.0f;
    }

    switch (victim->m_nType) {
        case ENTITY_TYPE_VEHICLE: {
            if (!victim->AsVehicle()) {
                return 3.0f;
            }
            break;
        }
        case ENTITY_TYPE_PED: {
            CPed* pedVictim = victim->AsPed();

            if (pedVictim->pVehicle && !pedVictim->pVehicle->IsBike()) {
                return 3.0f;
            }

            if (CEntity* attachedTo = pedVictim->m_pAttachedTo) {
                if (attachedTo->IsVehicle()) {
                    return 3.0f;
                }
            }

            break;
        }
    }

    if (!weaponOwner->IsPed() || !weaponOwner->AsPed()->IsPlayer()) {
        return 1.0f;
    }

    switch (CCamera::GetActiveCamera().m_nMode) {
        case MODE_TWOPLAYER_IN_CAR_AND_SHOOTING:
            return 2.0f;
        case MODE_HELICANNON_1STPERSON:
            return 3.0f;
    }

    return 1.0f;
}

void CWeapon::CheckForShootingVehicleOccupant(CEntity** pCarEntity, CColPoint* colPoint, eWeaponType weaponType, const CVector& origin, const CVector& target) {
    CHook::CallFunction<bool>("_ZN7CWeapon31CheckForShootingVehicleOccupantEPP7CEntityP9CColPoint11eWeaponTypeRK7CVectorS8_",
                              pCarEntity,
                              colPoint,
                              weaponType,
                              &origin,
                              &target);
}

bool CWeapon::FireProjectile(CEntity* firedBy, const CVector& origin, CEntity* targetEntity, const CVector* targetPos, float force) {
    return CHook::CallFunction<bool>("_ZN7CWeapon14FireProjectileEP7CEntityP7CVectorS1_S3_f",
                                     this,
                                     firedBy,
                                     &origin,
                                     targetEntity,
                                     targetPos,
                                     force);
}

bool CWeapon::FireInstantHit(CEntity* firingEntity, CVector* origin, CVector* muzzlePosn, CEntity* targetEntity, CVector* target, CVector* originForDriveBy, bool arg6, bool muzzle) {
    return CHook::CallFunction<bool>("_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb", this, firingEntity, origin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}

void CWeapon::AddGunshell(CEntity *pEntity, const CVector *posGunshell, const CVector2D *dirGunshell, float fGunshellSize) {
    CHook::CallFunction<void>("_ZN7CWeapon11AddGunshellEP7CEntityRK7CVectorRK9CVector2Df", this, pEntity, posGunshell, dirGunshell, fGunshellSize);
}

void CWeapon::DoBulletImpact(CEntity* firedBy, CEntity* victim, const CVector& startPoint, const CVector& endPoint, const CColPoint& hitCP, int32 incrementalHit) {
    CHook::CallFunction<void>("_ZN7CWeapon14DoBulletImpactEP7CEntityS1_P7CVectorS3_P9CColPointi", this, firedBy, victim, &startPoint, &endPoint, &hitCP, incrementalHit);
}

bool CWeapon::FireSniper(CPed* shooter, CEntity* victim, CVector* target) {
    return CHook::CallFunction<bool>("_ZN7CWeapon10FireSniperEP4CPedP7CEntityP7CVector", this, shooter, victim, target);
}

bool CWeapon::FireAreaEffect(CEntity* firingEntity, const CVector& origin, CEntity* targetEntity, CVector* target) {
    return CHook::CallFunction<bool>("_ZN7CWeapon14FireAreaEffectEP7CEntityP7CVectorS1_S3_", this, firingEntity, &origin, targetEntity, target);
}

bool CWeapon::FireM16_1stPerson(CPed* owner) {
    const auto cam = &CCamera::GetActiveCamera();

    switch (cam->m_nMode) {
        case MODE_M16_1STPERSON:
        case MODE_SNIPER:
        case MODE_CAMERA:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON_RUNABOUT:
        case MODE_SNIPER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
        case MODE_HELICANNON_1STPERSON:
            break;
        default:
            return false;
    }

    const auto wi = &GetWeaponInfo(); // NOTE: Why not `GetWeaponInfo(owner)`

    CWorld::bIncludeDeadPeds = true;
    CWorld::bIncludeCarTyres = true;
    CWorld::bIncludeBikers   = true;

    const auto camOriginPos = cam->Source;
    const auto camTargetPos = camOriginPos + cam->Front * 3.f;

    //CBirds::HandleGunShot(&camOriginPos, &camTargetPos);
    //CShadows::GunShotSetsOilOnFire(camOriginPos, camTargetPos);

    CColPoint shotCP{};
    CEntity*  shotHitEntity{};
    if (CWorld::ProcessLineOfSight(&camOriginPos, &camTargetPos, &shotCP, &shotHitEntity, true, true, true, true, true, false, false, true)) {
        CheckForShootingVehicleOccupant(&shotHitEntity, &shotCP, m_nType, camOriginPos, camTargetPos);
    }

    CWorld::bIncludeDeadPeds = false;
    CWorld::bIncludeCarTyres = false;
    CWorld::bIncludeBikers   = false;
    CWorld::SetToIgnoreEntity(nullptr);

    if (shotHitEntity) {
        if (TargetWeaponRangeMultiplier(shotHitEntity, owner) * wi->m_fWeaponRange >= (camOriginPos - shotCP.m_vecPoint).SquaredMagnitude2D()) {
            shotHitEntity = nullptr;
        }
    }

    DoBulletImpact(owner, shotHitEntity, camOriginPos, camTargetPos, shotCP, false);

    //> 0x741E48 - Visual/physical feedback for the player(s)
    if (owner->IsPlayer()) {
        auto intensity = [&]{
            switch (m_nType) {
                case WEAPON_AK47:
                    return 0.00015f;
                case WEAPON_M4:
                    return 0.0003f;
                default:
                    return 0.0002f;
            }
        }();
        if (FindPlayerPed()->bIsDucking || FindPlayerPed()->m_pAttachedTo) {
            intensity *= 0.3f;
        }

        // Move the camera around a little
        cam->m_fHorizontalAngle += (float)CGeneral::GetRandomNumberInRange(-64, 64) * intensity;
        cam->Alpha += (float)CGeneral::GetRandomNumberInRange(-64, 64) * intensity;

        // Do pad shaking
        const auto shakeFreq = (uint8)lerp(130.f, 210.f, std::clamp((20.f - (wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.f) / 80.f, 0.f, 1.f));
        auto *pad = (uintptr *) (g_libGTASA + (VER_x32 ? 0x00959B1C : 0xBC29E0));
        CHook::CallFunction<void>("_ZN4CPad10StartShakeEshj", pad, (int16)(CTimer::GetTimeStep() * 20'000.f / (float)shakeFreq), shakeFreq, 0);
    }

    return true;
}

bool CWeapon::Fire(CEntity* firedBy, CVector* startPosn, CVector* barrelPosn, CEntity* targetEnt, CVector* targetPosn, CVector* altPosn) {
    const auto firedByPed = firedBy && firedBy->IsPed()
                            ? firedBy->AsPed()
                            : nullptr;

    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
    const auto wi = &GetWeaponInfo(firedByPed);

    CVector point{ 0.f, 0.f, 0.6f };

    const auto fxPos = startPosn
                       ? startPosn
                       : &point;
    const auto shotOrigin = startPosn
                            ? barrelPosn
                            : &point;
    if (!startPosn) {
        point     = firedBy->GetMatrix().TransformPoint(point);
        startPosn = &point;
    }

    if (m_bFirstPersonWeaponModeSelected) {
        const auto r = 0.15f;

        const auto h = firedBy->GetHeading();
        fxPos->x -= std::sin(h) * r;
        fxPos->y += std::cos(h) * r;
    }

    switch (m_nState) {
        case WEAPONSTATE_READY:
        case WEAPONSTATE_FIRING:
            break;
        default:
            return false;
    }

    if (!m_nAmmoInClip) {
        if (!m_nTotalAmmo) {
            return false;
        }
        m_nAmmoInClip = std::min<uint32>(m_nTotalAmmo, wi->m_nAmmoClip);
    }

    const auto [hasFired, delayNextShot] = [&]() -> std::pair<bool, bool> {
        switch (m_nType) {
            case WEAPON_GRENADE:
            case WEAPON_TEARGAS:
            case WEAPON_MOLOTOV:
            case WEAPON_REMOTE_SATCHEL_CHARGE: {
                if (targetPosn) {
                    return {
                            FireProjectile(
                                    firedBy,
                                    *shotOrigin,
                                    targetEnt,
                                    targetPosn,
                                    std::clamp(((firedBy->GetPosition() - *targetPosn).Magnitude() - 10.f) / 10.f, 0.2f, 1.f)
                            ),
                            true
                    };
                } else if (firedBy == FindPlayerPed()) {
                    return {
                            FireProjectile(
                                    firedBy,
                                    *shotOrigin,
                                    targetEnt,
                                    nullptr,
                                    firedBy->AsPed()->m_pPlayerData->m_fAttackButtonCounter * 0.0375f
                            ),
                            true
                    };
                }
                return {
                        FireProjectile(
                                firedBy,
                                *shotOrigin,
                                targetEnt,
                                nullptr,
                                0.3f
                        ),
                        true
                };
            }
            case WEAPON_PISTOL:
            case WEAPON_PISTOL_SILENCED:
            case WEAPON_DESERT_EAGLE:
            case WEAPON_MICRO_UZI:
            case WEAPON_MP5:
            case WEAPON_AK47:
            case WEAPON_M4:
            case WEAPON_TEC9:
            case WEAPON_COUNTRYRIFLE:
            case WEAPON_MINIGUN: {
                if (   firedByPed
                       && firedByPed == GamePool_FindPlayerPed()
                       && notsa::contains({ MODE_M16_1STPERSON, MODE_HELICANNON_1STPERSON }, (eCamMode)TheCamera.PlayerWeaponMode.m_nMode)
                        ) {
                    return { FireM16_1stPerson(firedByPed), true };
                }
                const auto fired = FireInstantHit(firedBy, startPosn, shotOrigin, targetEnt, targetPosn, altPosn, false, true);
                if (firedByPed) {
                    if (!firedByPed->bInVehicle) {
                        return { fired, false };
                    }
                    if (const auto t = firedByPed->GetTaskManager().GetActiveTask()) {
                        return { fired, t->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY };
                    }
                }
                return { fired, true };
            }
            case WEAPON_SHOTGUN:
            case WEAPON_SAWNOFF_SHOTGUN:
            case WEAPON_SPAS12_SHOTGUN:
                return {
                        FireInstantHit(
                                firedBy,
                                startPosn,
                                shotOrigin,
                                targetEnt,
                                targetPosn,
                                altPosn,
                                false,
                                true
                        ),
                        true
                };
            case WEAPON_SNIPERRIFLE: {
                if (firedByPed && firedByPed == GamePool_FindPlayerPed() && TheCamera.PlayerWeaponMode.m_nMode == MODE_SNIPER) {
                    return {
                            FireSniper(firedByPed, targetEnt, targetPosn),
                            true
                    };
                }
                return {
                        FireInstantHit(
                                firedBy,
                                startPosn,
                                shotOrigin,
                                targetEnt,
                                targetPosn,
                                nullptr,
                                false,
                                true
                        ),
                        true
                };
            }
            case WEAPON_RLAUNCHER:
            case WEAPON_RLAUNCHER_HS: {
                if (firedByPed) {
                    const auto CanFire = [&](CVector origin, CVector end) {
                        return (origin - end).SquaredMagnitude() <= sq(8.f) && !firedBy->IsPed();
                    };
                    if (   targetEnt  && !CanFire(firedBy->GetPosition(), targetEnt->GetPosition())
                           || targetPosn && !CanFire(firedBy->GetPosition(), *targetPosn)
                            ) {
                        return { false, true };
                    }
                }
                return {
                        FireProjectile(
                                firedBy,
                                *shotOrigin,
                                targetEnt,
                                targetPosn
                        ),
                        true
                };
            }
            case WEAPON_FLAMETHROWER:
            case WEAPON_SPRAYCAN:
            case WEAPON_EXTINGUISHER:
                return {
                        FireAreaEffect(
                                firedBy,
                                *shotOrigin,
                                targetEnt,
                                targetPosn
                        ),
                        true
                };
            case WEAPON_DETONATOR: {
                assert(firedByPed);
                CHook::CallFunction<void>("_ZN6CWorld12UseDetonatorEP7CEntity", firedByPed);
                m_nAmmoInClip = m_nTotalAmmo  = 1;
                return { true, true };
            }
            case WEAPON_CAMERA:
                return {
                        CHook::CallFunction<bool>("_ZN7CWeapon14TakePhotographEP7CEntityP7CVector", this, firedBy, shotOrigin),
                        true
                };
        }
    }();

    if (hasFired) {
        const bool isPlayerFiring = firedByPed && m_nType != WEAPON_CAMERA && firedByPed->IsPlayer();
        if (firedByPed) {
            if (m_nType != WEAPON_CAMERA) {
                firedByPed->bFiringWeapon = true;
            }
            CHook::CallFunction<void>("_ZN23CAEPedWeaponAudioEntity13AddAudioEventEi", &firedByPed->m_PedWeaponAudioEntity, AE_WEAPON_FIRE);
        }

        if (m_nType == WEAPON_REMOTE_SATCHEL_CHARGE) {
            firedByPed->GiveWeapon(WEAPON_DETONATOR, true, true);
            if (firedByPed->GetWeapon(WEAPON_REMOTE_SATCHEL_CHARGE).m_nTotalAmmo <= 1) {
                firedByPed->GetWeapon(WEAPON_DETONATOR).m_nState = eWeaponState::WEAPONSTATE_READY;
                firedByPed->SetCurrentWeapon(WEAPON_DETONATOR);
            }
        }

        if (isPlayerFiring) {
            switch (m_nType)
            {
                case WEAPON_GRENADE:
                case WEAPON_MOLOTOV:
                case WEAPON_ROCKET:
                case WEAPON_RLAUNCHER:
                case WEAPON_RLAUNCHER_HS:
                case WEAPON_REMOTE_SATCHEL_CHARGE:
                case WEAPON_DETONATOR:
                    CStats::IncrementStat(STAT_KGS_OF_EXPLOSIVES_USED);
                    break;
                case WEAPON_PISTOL:
                case WEAPON_PISTOL_SILENCED:
                case WEAPON_DESERT_EAGLE:
                case WEAPON_SHOTGUN:
                case WEAPON_SAWNOFF_SHOTGUN:
                case WEAPON_SPAS12_SHOTGUN:
                case WEAPON_MICRO_UZI:
                case WEAPON_MP5:
                case WEAPON_AK47:
                case WEAPON_M4:
                case WEAPON_TEC9:
                case WEAPON_COUNTRYRIFLE:
                case WEAPON_SNIPERRIFLE:
                case WEAPON_MINIGUN:
                    CStats::IncrementStat(STAT_BULLETS_FIRED);
                    break;
                default:
                    break;
            }
        }

        if (m_nAmmoInClip) {
            m_nAmmoInClip--;
        }
        if (m_nTotalAmmo > 0) {
            if (isPlayerFiring
                ? m_nType == WEAPON_DETONATOR || CStats::GetPercentageProgress() < 100.f
                : m_nTotalAmmo < 25'000
                    ) {
                m_nTotalAmmo--;
            }
        }
        m_nState = WEAPONSTATE_FIRING;

        if (!m_nAmmoInClip) {
            if (m_nTotalAmmo) {
                m_nState = WEAPONSTATE_RELOADING;
                m_nTimer = firedBy == FindPlayerPed() && FindPlayerInfo().FastReload
                           ? wi->GetWeaponReloadTime() / 4
                           : wi->GetWeaponReloadTime();

                m_nTimer += CTimer::GetTimeInMS();
            } else if (CCamera::GetActiveCamera().m_nMode == MODE_CAMERA) {
                auto *pad = (uintptr *) (g_libGTASA + (VER_x32 ? 0x00959B1C : 0xBC29E0));
                CHook::CallFunction<void>("_ZN4CPad5ClearEbb", pad, false, true);
            }
            return true;
        }

        m_nTimer = delayNextShot
                   ? m_nType == WEAPON_CAMERA
                     ? 1100
                     : (uint32)((wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.f)
                   : 0;
        m_nTimer += CTimer::GetTimeInMS();
    }
    if (m_nType == WEAPON_UNARMED || m_nType == WEAPON_BASEBALLBAT) {
        return true;
    }

    CLocalPlayer::ammoUpdated = true;
    return hasFired;
}

bool CWeapon::IsType2Handed() {
    switch (m_nType) {
        case eWeaponType::WEAPON_M4:
        case eWeaponType::WEAPON_AK47:
        case eWeaponType::WEAPON_SPAS12_SHOTGUN:
        case eWeaponType::WEAPON_SHOTGUN:
        case eWeaponType::WEAPON_SNIPERRIFLE:
        case eWeaponType::WEAPON_FLAMETHROWER:
        case eWeaponType::WEAPON_COUNTRYRIFLE:
            return true;
    }

    return false;
}

bool CWeapon::IsTypeProjectile() {
    switch (m_nType) {
        case eWeaponType::WEAPON_GRENADE:
        case eWeaponType::WEAPON_REMOTE_SATCHEL_CHARGE:
        case eWeaponType::WEAPON_TEARGAS:
        case eWeaponType::WEAPON_MOLOTOV:
        case eWeaponType::WEAPON_FREEFALL_BOMB:
            return true;
    }

    return false;
}

bool CWeapon::HasWeaponAmmoToBeUsed() {
    switch (m_nType) {
        case eWeaponType::WEAPON_UNARMED:
        case eWeaponType::WEAPON_BRASSKNUCKLE:
        case eWeaponType::WEAPON_GOLFCLUB:
        case eWeaponType::WEAPON_NIGHTSTICK:
        case eWeaponType::WEAPON_KNIFE:
        case eWeaponType::WEAPON_BASEBALLBAT:
        case eWeaponType::WEAPON_KATANA:
        case eWeaponType::WEAPON_CHAINSAW:
        case eWeaponType::WEAPON_DILDO1:
        case eWeaponType::WEAPON_DILDO2:
        case eWeaponType::WEAPON_VIBE1:
        case eWeaponType::WEAPON_VIBE2:
        case eWeaponType::WEAPON_FLOWERS:
        case eWeaponType::WEAPON_PARACHUTE:
            return true;
    }

    return m_nTotalAmmo != 0;
}

bool CWeapon::IsFiredWeapon() const {
    switch (m_nType) {
        case eWeaponType::WEAPON_UNARMED:
        case eWeaponType::WEAPON_BRASSKNUCKLE:
        case eWeaponType::WEAPON_GOLFCLUB:
        case eWeaponType::WEAPON_NIGHTSTICK:
        case eWeaponType::WEAPON_KNIFE:
        case eWeaponType::WEAPON_BASEBALLBAT:
        case eWeaponType::WEAPON_KATANA:
        case eWeaponType::WEAPON_CHAINSAW:
        case eWeaponType::WEAPON_DILDO1:
        case eWeaponType::WEAPON_DILDO2:
        case eWeaponType::WEAPON_VIBE1:
        case eWeaponType::WEAPON_VIBE2:
        case eWeaponType::WEAPON_FLOWERS:
        case eWeaponType::WEAPON_PARACHUTE:
            return false;
    }

    return true;
}

bool CWeapon::IsTypeMelee() {
    return GetWeaponInfo().m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE;
}

void CWeapon::DoTankDoomAiming(CEntity* vehicle, CEntity* owner, CVector* startPoint, CVector* endPoint) {
    CHook::CallFunction<void>("_ZN7CWeapon16DoTankDoomAimingEP7CEntityS1_P7CVectorS3_", vehicle, owner, startPoint, endPoint);
}