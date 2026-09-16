#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_PSWeapon09MResonator {
    struct TMResonatorParticle;

    struct TPSWeapon09MResonator;

    struct TPSWeapon09BranchGI;

    using TMResonatorPalette = pas::Array<std::uint16_t, 0, 0>;

    using TGAISet = pas::Array<pas::WideString, 0, 0>;

    using PMResonatorParticle = TMResonatorParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon09BranchGI : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon09BranchGI, GI_PSWeapon::TPSWeaponGI, "TPSWeapon09BranchGI", 328)
        void p_destroy() override;
        // Native empty override.
        void Invalidate() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateHitTestBounds() override;
        void ClearParticles();
        void GrowParticles();
        PMResonatorParticle AddParticle();
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        // Explicitly zeroed by the constructor.
        std::int32_t Unknown130;
        PMResonatorParticle Particles;
        std::int32_t ParticleCount;
        std::int32_t ParticleCapacity;
        float OriginalLength;
        std::uint8_t Unknown144;
        std::uint8_t cpp_padding[1];
        std::uint16_t ParticleColor;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TMResonatorParticle {
        std::int32_t Kind;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t DelayTicks;
        std::uint8_t MovementDelay;
        std::uint8_t Unknown1A;
        std::uint8_t Unknown1B;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon09MResonator : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon09MResonator, GI_PSWeapon::TPSWeaponGI, "TPSWeapon09MResonator", 352)
        void p_destroy() override;
        // Native empty override.
        void Invalidate() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateHitTestBounds() override;
        void ClearParticles();
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        // Explicitly zeroed by the constructor.
        std::int32_t Unknown130;
        PMResonatorParticle Particles;
        std::int32_t ParticleCount;
        std::int32_t ParticleCapacity;
        float OriginalLength;
        GI_GAI::TgaiGI* Animation;
        EC_Struct::TPointF AnimationPosition;
        EC_Struct::TPointF AnimationVelocity;
        std::uint8_t Unknown158;
        std::uint8_t cpp_padding[3];
        pas::WideString AnimationPath;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_PSWeapon09MResonator
