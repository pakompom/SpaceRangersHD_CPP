#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon17Kafacitor {
    struct TKafacitorParticle;

    struct TPSWeapon17Kafacitor;

    using TKafacitorPalette = pas::Array<std::uint16_t, 0, 1>;

    using TGAISet = pas::Array<pas::WideString, 0, 0>;

    using PKafacitorParticle = TKafacitorParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon17Kafacitor : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon17Kafacitor, GI_PSWeapon::TPSWeaponGI, "TPSWeapon17Kafacitor", 352)
        void p_destroy() override;
        void SetColors(std::uint16_t Primary, std::uint16_t Secondary);
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PKafacitorParticle AddParticle();
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        PKafacitorParticle FirstParticle;
        PKafacitorParticle LastParticle;
        std::uint16_t PrimaryColor;
        std::uint16_t SecondaryColor;
        Types::TRect ProjectionBounds;
        double OriginalLength;
        double LengthScale;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TKafacitorParticle {
        PKafacitorParticle Prev;
        PKafacitorParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[3];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon17Kafacitor
