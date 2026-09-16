#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon {
    struct TPSWeaponGI;

} // namespace GI_PSWeapon

namespace SE_Weapon {
    struct TWeaponEffectItem;

    struct TWeaponEffect;

    struct TWeaponSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TWeaponSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TWeaponSE, SE_Space::TObjectSE, "TWeaponSE", 212)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetHit(std::int32_t Color, std::int32_t Damage, std::uint8_t Destroyed, std::uint8_t PlaySound);
        // Retains both scene references.
        void SetEndpoints(SE_Space::TObjectSE* Source, SE_Space::TObjectSE* Target);
        EC_Struct::TPointF GetTargetPoint();
        EC_Struct::TPointF GetSourcePoint();
        void Advance() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString ShotSoundPath;
        pas::WideString HitSoundPath;
        std::uint8_t PlayShotSound;
        std::uint8_t cpp_padding[3];
        SE_Space::TObjectSE* SourceObject;
        SE_Space::TObjectSE* TargetObject;
        std::int32_t HitDamage;
        std::uint8_t TargetDestroyed;
        std::uint8_t cpp_padding_2[3];
        std::int32_t DestructionEffect;
        std::int32_t DestructionFrameInterval;
        std::int32_t DestructionDetachStep;
        std::int32_t HitColor;
        GI_GAI::TgaiGI* SourceAnimation;
        std::int32_t SourceAnimationInterval;
        GI_GAI::TgaiGI* TargetAnimation;
        std::int32_t TargetAnimationInterval;
        TWeaponEffect* HitEffect;
        std::int32_t HitVariant;
        // Nonzero selects removal from trailing effects at end turn.
        GI_PSWeapon::TPSWeaponGI* Projectile;
        GI_GAI::TgaiGI* DestructionAnimation;
        pas::Array<GI_GAI::TgaiGI*, 0, 5> ExtraDestructionAnimations;
        GI_Label::TLabelGI* DamageLabel;
        std::uint8_t ImmediateDestruction;
        std::uint8_t cpp_padding_3[3];
        float DestructionAlpha;
        float DestructionAlphaStep;
        EC_Struct::TPointF DamageLabelPoint;
        std::uint8_t cpp_padding_4[1];
        std::uint8_t ProjectileFinished;
        std::uint8_t cpp_padding_5[2];
        std::int32_t StepIndex;
        std::int32_t ShotVisual;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PWeaponEffectItem = TWeaponEffectItem*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TWeaponEffect : pas::Object {
        PAS_CLASS_META(TWeaponEffect, pas::Object, "TWeaponEffect", 64)
        void p_destroy() override;
        void Clear();
        PWeaponEffectItem AddItem();
        void RemoveItem(PWeaponEffectItem Item);
        void AddTargetEffect(std::int32_t Index);
        void AddSourceEffect(std::int32_t Index);
        void Start();
        void Advance();
        void AnimationComplete(GI_MessageLoop::TObjectGI* Sender);
        std::uint8_t IsFinished();
        void SetSourcePoint(EC_Struct::TPointF Point);
        void SetTargetPoint(EC_Struct::TPointF Point);
        GI_MessageLoop::TObjectGI* Owner;
        PWeaponEffectItem FirstItem;
        PWeaponEffectItem LastItem;
        std::int32_t EffectIndex;
        pas::WideString DepthExpression;
        EC_Struct::TPointF SourcePoint;
        EC_Struct::TPointF TargetPoint;
        std::uint8_t Started;
        std::uint8_t cpp_padding[3];
        std::int32_t LeftTime;
        float Direction;
        std::int32_t AnimationInterval;
        std::int32_t AnimationCountdown;
        std::uint8_t BeforeEnd;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TWeaponEffectItem {
        PWeaponEffectItem Next;
        PWeaponEffectItem Prev;
        GI_GAI::TgaiGI* Image;
        // Y is the evolving radial distance; X starts at zero.
        EC_Struct::TPointF Position;
        float Angle;
        std::int32_t Lifetime;
        float Speed;
        float Acceleration;
        std::uint8_t AtTarget;
        std::uint8_t AutoAnimation;
        std::uint8_t LoopAnimation;
        std::uint8_t cpp_padding[1];
        std::int32_t SkipTime;
    };
    #pragma pack(pop)

} // namespace SE_Weapon
