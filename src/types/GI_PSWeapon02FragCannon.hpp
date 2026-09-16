#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon02FragCannon {
    struct TFragCannonParticle;

    struct TPSWeapon02FragCannon;

    using TFragCannonPalette = pas::Array<std::uint16_t, 0, 1>;

    using PFragCannonParticle = TFragCannonParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon02FragCannon : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon02FragCannon, GI_PSWeapon::TPSWeaponGI, "TPSWeapon02FragCannon", 336)
        void p_destroy() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PFragCannonParticle AddParticle();
        void ClearParticles();
        void InvalidateRect(Types::TRect Rect) override;
        void Invalidate() override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        PFragCannonParticle FirstParticle;
        PFragCannonParticle LastParticle;
        std::uint16_t PrimaryColor;
        std::uint16_t SecondaryColor;
        Types::TRect ProjectionBounds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TFragCannonParticle {
        PFragCannonParticle Prev;
        PFragCannonParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[1];
        std::uint16_t Unknown1E;
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon02FragCannon
