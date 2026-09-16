#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon05Treton {
    struct TTretonParticle;

    struct TPSWeapon05Treton;

    using TTretonPalette = pas::Array<std::uint16_t, 0, 1>;

    using PTretonParticle = TTretonParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon05Treton : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon05Treton, GI_PSWeapon::TPSWeaponGI, "TPSWeapon05Treton", 352)
        void p_destroy() override;
        void SetColors(std::uint16_t FirstColor, std::uint16_t SecondColor);
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PTretonParticle AddParticle();
        void ClearParticles();
        void Invalidate() override;
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        PTretonParticle FirstParticle;
        PTretonParticle LastParticle;
        std::uint16_t PrimaryColor;
        std::uint16_t SecondaryColor;
        Types::TRect ProjectionBounds;
        double LengthScale;
        double OriginalLength;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TTretonParticle {
        PTretonParticle Prev;
        PTretonParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t MaximumAlpha;
        EC_Struct::TPointF Velocity;
        std::uint8_t Countdown;
        std::uint8_t State;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon05Treton
