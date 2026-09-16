#pragma once
#include "types/SE_Weapon.hpp"

namespace SE_Weapon {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Weapon::TWeaponSE) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, ShotSoundPath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, HitSoundPath) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, PlayShotSound) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, SourceObject) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, TargetObject) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, HitDamage) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, TargetDestroyed) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DestructionEffect) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DestructionFrameInterval) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DestructionDetachStep) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, HitColor) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, SourceAnimation) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, SourceAnimationInterval) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, TargetAnimation) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, TargetAnimationInterval) == 132);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, HitEffect) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, HitVariant) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, Projectile) == 144);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DestructionAnimation) == 148);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, ExtraDestructionAnimations) == 152);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DamageLabel) == 176);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, ImmediateDestruction) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DestructionAlpha) == 184);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DestructionAlphaStep) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, DamageLabelPoint) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, ProjectileFinished) == 201);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, StepIndex) == 204);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponSE, ShotVisual) == 208);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Weapon::TWeaponEffect) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, Owner) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, FirstItem) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, LastItem) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, EffectIndex) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, DepthExpression) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, SourcePoint) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, TargetPoint) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, Started) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, LeftTime) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, Direction) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, AnimationInterval) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, AnimationCountdown) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffect, BeforeEnd) == 60);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Weapon::TWeaponEffectItem) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Next) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Image) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Position) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Angle) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Lifetime) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Speed) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, Acceleration) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, AtTarget) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, AutoAnimation) == 37);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, LoopAnimation) == 38);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Weapon::TWeaponEffectItem, SkipTime) == 40);

} // namespace SE_Weapon
