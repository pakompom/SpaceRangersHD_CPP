#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon16Esodafer {
    struct TEsodaferParticle;

    struct TPSWeapon16Esodafer;

    using TEsodaferPalette = pas::Array<std::uint16_t, 0, 2>;

    using PEsodaferParticle = TEsodaferParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon16Esodafer : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon16Esodafer, GI_PSWeapon::TPSWeaponGI, "TPSWeapon16Esodafer", 360)
        void p_destroy() override;
        void SetColors(std::uint16_t Primary, std::uint16_t Secondary, std::uint16_t Tertiary);
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PEsodaferParticle AddParticle();
        void ClearParticles();
        void InvalidateRect(Types::TRect Rect) override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        PEsodaferParticle FirstParticle;
        PEsodaferParticle LastParticle;
        TEsodaferPalette Colors;
        Types::TRect ProjectionBounds;
        std::uint8_t cpp_padding[6];
        double OriginalLength;
        double LengthScale;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TEsodaferParticle {
        PEsodaferParticle Prev;
        PEsodaferParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding_2[3];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon16Esodafer
