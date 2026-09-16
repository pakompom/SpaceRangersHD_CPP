#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_RadialEffect {
    struct TRadiationParticle;

    struct TPSRadEffectGI;

    using TRadiationPalette = pas::Array<float, 0, 5>;

    using PRadiationParticle = TRadiationParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSRadEffectGI : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSRadEffectGI, GI_PSWeapon::TPSWeaponGI, "TPSRadEffectGI", 356)
        void p_destroy() override;
        void SetColor(std::uint16_t Value);
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PRadiationParticle AddParticle();
        void ClearParticles();
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        PRadiationParticle FirstParticle;
        PRadiationParticle LastParticle;
        std::uint16_t Color;
        std::uint8_t cpp_padding[2];
        std::int32_t Radius;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding_2[3];
        std::int32_t PaletteIndex;
        Types::TRect ProjectionBounds;
        EC_Struct::TPointF LatticeOffset;
        std::uint8_t PositionInitialized;
        std::uint8_t cpp_padding_3[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TRadiationParticle {
        PRadiationParticle Prev;
        PRadiationParticle Next;
        Types::TPoint Position;
        std::uint16_t Color;
        pas::Array<std::uint8_t, 0, 11> Alpha;
        std::uint8_t cpp_padding[2];
        pas::Array<std::int32_t, 0, 11> EdgeDistance;
    };
    #pragma pack(pop)

} // namespace GI_RadialEffect
