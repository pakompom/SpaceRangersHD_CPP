#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon11Desintegrator {
    struct TDesintegratorParticle;

    struct TPSWeapon11Desintegrator;

    using TDesintegratorPalette = pas::Array<std::uint16_t, 0, 0>;

    using PDesintegratorParticle = TDesintegratorParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon11Desintegrator : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon11Desintegrator, GI_PSWeapon::TPSWeaponGI, "TPSWeapon11Desintegrator", 368)
        void p_destroy() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PDesintegratorParticle AddParticle();
        void RemoveParticle(PDesintegratorParticle Particle);
        void ClearParticles();
        void Invalidate() override;
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void AdvanceImpactSparks(Types::TRect ClipRect);
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        std::int32_t Wavelength;
        std::int32_t PhaseMask;
        PDesintegratorParticle FirstParticle;
        PDesintegratorParticle LastParticle;
        std::int32_t PendingSparkSteps;
        std::uint16_t Color;
        Types::TRect ProjectionBounds;
        std::uint8_t cpp_padding[6];
        double LengthScale;
        double OriginalLength;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TDesintegratorParticle {
        PDesintegratorParticle Prev;
        PDesintegratorParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[1];
        std::uint16_t RemainingTicks;
        std::int32_t BaseAlpha;
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon11Desintegrator
