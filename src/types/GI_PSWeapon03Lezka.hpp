#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon03Lezka {
    struct TLezkaParticle;

    struct TPSWeapon03Lezka;

    using TLezkaPalette = pas::Array<float, 0, 8>;

    using PLezkaParticle = TLezkaParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon03Lezka : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon03Lezka, GI_PSWeapon::TPSWeaponGI, "TPSWeapon03Lezka", 356)
        void p_destroy() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void SetActive(std::uint8_t Enabled) override;
        void UpdateProjectionBounds();
        void UpdateHitTestBounds() override;
        Types::TRect GetLocalBounds() override;
        PLezkaParticle AddParticle();
        void ClearParticles();
        void InvalidateRect(Types::TRect Rect) override;
        void Invalidate() override;
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        std::int32_t HalfWidth;
        PLezkaParticle FirstParticle;
        PLezkaParticle LastParticle;
        Types::TRect ProjectionBounds;
        std::uint8_t cpp_padding[4];
        double LengthScale;
        double OriginalLength;
        std::int32_t PaletteIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TLezkaParticle {
        PLezkaParticle Prev;
        PLezkaParticle Next;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t MaximumAlpha;
        std::int32_t AlphaStep;
        EC_Struct::TPointF Velocity;
        std::uint8_t State;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon03Lezka
