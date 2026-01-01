//
// Created on 21.05.2024.
//

#include "TaskSimpleUseGun.h"
#include "Entity/Ped/Ped.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "TaskSimpleDuck.h"
#include "Animation/AnimManager.h"
#include "util/patch.h"

void CTaskSimpleUseGun::SetMoveAnim(CPed* ped) {
    const auto
            animGunStand   = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUN_STAND),
            animGunMoveFwd = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_FWD),
            animGunMoveL   = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_L),
            animGunMoveBwd = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_BWD),
            animGunMoveR   = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_R);

    // 0x61E444
    if (ped->bIsDucking) {
        if (const auto tDuck = ped->GetIntelligence()->GetTaskDuck()) {
            switch (m_LastCmd) {
                case eGunCommand::FIRE:
                case eGunCommand::FIREBURST:
                case eGunCommand::RELOAD:
                    tDuck->ForceStopMove();
            }
        }
        m_MoveCmd = {0.f, 0.f};
        m_HasMoveControl = false;
        return;
    }

    // 0x61E4A5
    const auto isInMoveControl = m_HasMoveControl && (m_LastCmd != eGunCommand::FIRE || m_WeaponInfo->flags.bMoveFire);
    const auto moveCmdDist = isInMoveControl
                             ? m_MoveCmd.Magnitude()
                             : 0.f;
    const auto moveCmdAbsSum = isInMoveControl
                               ? std::abs(m_MoveCmd.x) + std::abs(m_MoveCmd.y)
                               : 0.f;

    const auto DoBlendAnim = [ped](AnimationId animId, float blendDelta) {
        return CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, animId, blendDelta);
    };

    // 0x61E505
    if (moveCmdAbsSum < 0.1f) {
        const auto moveAnim = m_WeaponInfo->flags.bAimWithArm
                              ? DoBlendAnim(ANIM_ID_IDLE, 8.f) : DoBlendAnim(ANIM_ID_GUN_STAND, 8.f);

        const auto resetPlayTime = moveAnim && moveAnim->GetBlendAmount() > 0.95f;
        for (const auto anim : { animGunMoveBwd, animGunMoveL, animGunMoveR, animGunMoveBwd }) {
            if (anim) {
                anim->SetFlag(ANIMATION_STARTED, false);
                if (resetPlayTime) {
                    anim->SetCurrentTime(0.f);
                }
            }
        }

        m_MoveCmd        = { 0.f, 0.f };
        m_HasMoveControl = false;

        return; // 0x61E5F3
    }

    // 0x61E5F6 - Blend in new move anim according to the move command
    if (m_WeaponInfo->flags.bAimWithArm) {
        const auto moveAnim = DoBlendAnim(
                m_MoveCmd.x > 0.75f
                ? ANIM_ID_GUNMOVE_R
                : m_MoveCmd.x <= -0.75f
                  ? ANIM_ID_GUNMOVE_L
                  : m_MoveCmd.y <= 0.f
                    ? ANIM_ID_GUNMOVE_FWD
                    : ANIM_ID_GUNMOVE_BWD,
                8.f
        );
        moveAnim->SetFlag(ANIMATION_STARTED, true);
        ped->SetMoveState(PEDMOVE_NONE);
        return; // 0x61E675
    }

    //
    // Handle anim transition defined by the current move command
    //

    // 0x61E67C - Calculate lerp progress
    float t = 1.f;
    if (const auto aGS = animGunStand) {
        // 0x61E682 - Adjust blend amount
        if (aGS->GetBlendDelta() >= 0.f) {
            aGS->SetBlendDelta(0.f);
            aGS->SetBlendAmount(std::max(0.016f, aGS->GetBlendAmount() - CTimer::GetTimeStep() * 0.16f));
        }

        // Calculate lerp t
        t = 1.f - std::clamp(CTimer::GetTimeStep() * aGS->GetBlendDelta() * 0.02f + aGS->GetBlendAmount(), 0.f, 1.f);
    }

    // Process transition from one gun move to another
    const auto DoTransitionGunMoveAnim = [&, ped, this](CAnimBlendAssociation* fromAnim, CAnimBlendAssociation* toAnim, AnimationId toAnimId, float toAnimBlendAmnt) {
        // If previous anim exists, blend it out
        if (fromAnim) {
            fromAnim->SetBlendAmount(0.f);
            fromAnim->SetBlendDelta(0.f);
        }

        // If new anim doesn't exists, create it
        if (!toAnim) {
            toAnim = CAnimManager::AddAnimation(ped->m_pRwClump,ANIM_GROUP_DEFAULT, toAnimId);
        }

        // Set params of the anim we're transitioning to
        toAnim->SetBlendDelta(0.f);
        toAnim->SetBlendAmount(toAnimBlendAmnt);
        toAnim->SetFlag(ANIMATION_STARTED, true);
        toAnim->SetSpeed(m_WeaponInfo->m_fMoveSpeed * moveCmdDist);
    };

    const auto moveCmdLerp = m_MoveCmd * (1.f / moveCmdAbsSum) * t;

    // 0x61E73F - Process X axis (left, right)
    if (moveCmdLerp.x < 0.f) { // right => left
        DoTransitionGunMoveAnim(animGunMoveR, animGunMoveL, ANIM_ID_GUNMOVE_L, std::abs(moveCmdLerp.x));
    } else if (moveCmdLerp.x > 0.f) { // left => right
        DoTransitionGunMoveAnim(animGunMoveL, animGunMoveR, ANIM_ID_GUNMOVE_R, std::abs(moveCmdLerp.x));
    }

    // 0x61E7E5 - Process Y axis (fwd, bwd)
    if (moveCmdLerp.y < 0.f) { // bwd => fwd
        DoTransitionGunMoveAnim(animGunMoveBwd, animGunMoveFwd, ANIM_ID_GUNMOVE_FWD, std::abs(moveCmdLerp.y));
    } else if (moveCmdLerp.y > 0.f) { // fwd => bwd
        DoTransitionGunMoveAnim(animGunMoveFwd, animGunMoveBwd, ANIM_ID_GUNMOVE_BWD, std::abs(moveCmdLerp.y));
    }

    // 0x61E88B
    if (m_LastCmd == eGunCommand::AIM) {
        if (m_Anim && m_Anim->GetBlendAmount() > 0.f && m_Anim->GetBlendDelta() >= 0.f && !m_WeaponInfo->flags.bMoveAim) {
            m_Anim->SetBlendDelta(-4.f);
        }
    }

    m_HasMoveControl = false;
}

bool CTaskSimpleUseGun::ControlGunMove(const CVector2D& moveSpeed) {
    float factor = CTimer::GetTimeStep() * 0.35f;

    float deltaY = moveSpeed.y - m_MoveCmd.y;
    if(deltaY > factor) {
        m_MoveCmd.y += factor;
    } else if (deltaY < -factor) {
        m_MoveCmd.y -= factor;
    } else {
        m_MoveCmd.y = moveSpeed.y;
    }

    float deltaX = moveSpeed.x - m_MoveCmd.x;
    if(deltaX > factor) {
        m_MoveCmd.x += factor;
    } else if (deltaX < -factor) {
        m_MoveCmd.x -= factor;
    } else {
        m_MoveCmd.x = moveSpeed.x;
    }

    m_HasMoveControl = true;
    return true;
}

void CTaskSimpleUseGun::FinishGunAnimCB(CAnimBlendAssociation *anim, void *data) {

}

void CTaskSimpleUseGun__SetMoveAnim_hook(CTaskSimpleUseGun *thiz, CPed *pPed)
{
    thiz->SetMoveAnim(pPed);
}

void CTaskSimpleUseGun__ControlGunMove_hook(CTaskSimpleUseGun *thiz, const CVector2D& moveSpeed)
{
    thiz->ControlGunMove(moveSpeed);
}

void CTaskSimpleUseGun::InjectHooks() {
    CHook::Redirect("_ZN17CTaskSimpleUseGun11SetMoveAnimEP4CPed", &CTaskSimpleUseGun__SetMoveAnim_hook);
    CHook::Redirect("_ZN17CTaskSimpleUseGun14ControlGunMoveEP9CVector2D", &CTaskSimpleUseGun__ControlGunMove_hook);
}
