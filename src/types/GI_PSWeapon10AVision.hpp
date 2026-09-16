#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon10AVision {
    struct TAVisionParticle;

    struct TPSWeapon10AVision;

    using TAVisionPalette = pas::Array<std::uint16_t, 0, 3>;

    using TGAISet = pas::Array<pas::WideString, 0, 0>;

    using PAVisionParticle = TAVisionParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon10AVision : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon10AVision, GI_PSWeapon::TPSWeaponGI, "TPSWeapon10AVision", 332)
        void p_destroy() override;
        void Invalidate() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateHitTestBounds() override;
        void ClearParticles();
        void GrowParticles();
        PAVisionParticle AddParticle();
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t Unknown130;
        PAVisionParticle Particles;
        std::int32_t ParticleCount;
        std::int32_t ParticleCapacity;
        float OriginalLength;
        TAVisionPalette Colors;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TAVisionParticle {
        std::int32_t Kind;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t FadeInTicks;
        std::uint8_t InitialFadeInTicks;
        std::uint8_t FadeOutThreshold;
        std::uint8_t cpp_padding_2[1];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon10AVision
