#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon12Turbogravir {
    struct TTurbogravirParticle;

    struct TPSWeapon12Turbogravir;

    struct TPSBlueWhirlGI;

    using TTurbogravirPalette = pas::Array<float, 0, 8>;

    using PTurbogravirParticle = TTurbogravirParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon12Turbogravir : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon12Turbogravir, GI_PSWeapon::TPSWeaponGI, "TPSWeapon12Turbogravir", 692)
        void p_destroy() override;
        void BuildWaveTables();
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PTurbogravirParticle AddParticle();
        void ClearParticles();
        // Native empty override.
        void Invalidate() override;
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        pas::Array<std::int32_t, 0, 63> OffsetTable;
        pas::Array<std::uint8_t, 0, 63> AlphaTable;
        std::int32_t HalfWidth;
        std::int32_t Wavelength;
        std::int32_t HalfWavelength;
        std::int32_t EnabledStrands;
        std::int32_t PhaseMask;
        PTurbogravirParticle FirstParticle;
        PTurbogravirParticle LastParticle;
        Types::TRect ProjectionBounds;
        std::uint8_t cpp_padding[4];
        double LengthScale;
        double OriginalLength;
        std::int32_t PaletteIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSBlueWhirlGI : GI_PSWeapon12Turbogravir::TPSWeapon12Turbogravir {
        PAS_CLASS_META(TPSBlueWhirlGI, GI_PSWeapon12Turbogravir::TPSWeapon12Turbogravir, "TPSBlueWhirlGI", 696)
        std::uint8_t cpp_padding[4];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TTurbogravirParticle {
        PTurbogravirParticle Prev;
        PTurbogravirParticle Next;
        Types::TPoint Position;
        EC_Struct::TPointF FloatPosition;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        Types::TPoint Velocity;
        EC_Struct::TPointF FloatVelocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[1];
        // Set to 30000; unused. Possibly a lifetime counter.
        std::uint16_t Unknown2E;
        std::int32_t Radius;
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon12Turbogravir
