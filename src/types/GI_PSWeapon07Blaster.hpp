#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon07Blaster {
    struct TBlasterParticle;

    struct TPSWeapon07Blaster;

    using TBlasterPalette = pas::Array<std::uint16_t, 0, 1>;

    using PBlasterParticle = TBlasterParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon07Blaster : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon07Blaster, GI_PSWeapon::TPSWeaponGI, "TPSWeapon07Blaster", 336)
        void p_destroy() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PBlasterParticle AddParticle();
        void ClearParticles();
        void InvalidateRect(Types::TRect Rect) override;
        // Native empty override.
        void Invalidate() override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        PBlasterParticle FirstParticle;
        PBlasterParticle LastParticle;
        std::uint16_t PrimaryColor;
        std::uint16_t SecondaryColor;
        Types::TRect ProjectionBounds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TBlasterParticle {
        PBlasterParticle Prev;
        PBlasterParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[1];
        // Set to 30000; unused. Possibly a lifetime counter.
        std::uint16_t Unknown1E;
        std::uint8_t cpp_padding_3[4];
        float BaseX;
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon07Blaster
