#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon01Laser {
    struct TBeamLaserParticle;

    struct TPSWeapon01Laser;

    using TBeamLaserPalette = pas::Array<float, 0, 8>;

    using PBeamLaserParticle = TBeamLaserParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon01Laser : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon01Laser, GI_PSWeapon::TPSWeaponGI, "TPSWeapon01Laser", 356)
        void p_destroy() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PBeamLaserParticle AddParticle();
        void RemoveParticle(PBeamLaserParticle Particle);
        void ClearParticles();
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        float HalfWidth;
        PBeamLaserParticle FirstParticle;
        PBeamLaserParticle LastParticle;
        Types::TRect ProjectionBounds;
        std::uint8_t cpp_padding[4];
        double OriginalLength;
        double LengthScale;
        std::int32_t PaletteIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TBeamLaserParticle {
        PBeamLaserParticle Prev;
        PBeamLaserParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[3];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon01Laser
