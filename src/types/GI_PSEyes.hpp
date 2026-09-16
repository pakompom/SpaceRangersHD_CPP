#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSEyes {
    struct TEyesLine;

    struct TEyesParticle;

    struct TPSEyesGI;

    using TEyesPalette = pas::Array<std::uint16_t, 0, 1>;

    using PEyesParticle = TEyesParticle*;

    using PEyesLine = TEyesLine*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSEyesGI : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSEyesGI, GI_PSWeapon::TPSWeaponGI, "TPSEyesGI", 360)
        void p_destroy() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PEyesParticle AddParticle();
        void ClearParticles();
        void ClearLines();
        void Invalidate() override;
        void InvalidateRect(Types::TRect Rect) override;
        void EmitBurst(Types::TPoint Point, std::int32_t Radius);
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        std::int32_t SegmentLength;
        std::int32_t Dispersion;
        std::int32_t StartingAlpha;
        PEyesParticle FirstParticle;
        PEyesParticle LastParticle;
        PEyesLine FirstLine;
        PEyesLine LastLine;
        Types::TRect ProjectionBounds;
        std::uint16_t PrimaryColor;
        std::uint16_t SecondaryColor;
        std::int32_t BeamTicks;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TEyesParticle {
        PEyesParticle Prev;
        PEyesParticle Next;
        EC_Struct::TPointF Origin;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[3];
        std::int32_t Countdown;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TEyesLine {
        PEyesLine Next;
        std::uint8_t cpp_padding[4];
        Types::TPoint First;
        Types::TPoint Last;
        std::uint16_t Color;
        std::uint8_t cpp_padding_2[1];
        std::uint8_t Alpha;
    };
    #pragma pack(pop)

} // namespace GI_PSEyes
