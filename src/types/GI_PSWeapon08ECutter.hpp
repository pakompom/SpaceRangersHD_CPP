#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon08ECutter {
    struct TECutterParticle;

    struct TPSWeapon08ECutter;

    using TECutterPalette = pas::Array<std::uint16_t, 0, 7>;

    using PECutterParticle = TECutterParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon08ECutter : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon08ECutter, GI_PSWeapon::TPSWeaponGI, "TPSWeapon08ECutter", 336)
        void p_destroy() override;
        // Native empty override.
        void Invalidate() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateHitTestBounds() override;
        void ClearParticles();
        void GrowParticles();
        PECutterParticle AddParticle();
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        PECutterParticle Particles;
        std::int32_t ParticleCount;
        std::int32_t ParticleCapacity;
        float OriginalLength;
        TECutterPalette Colors;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TECutterParticle {
        std::int32_t Kind;
        EC_Struct::TPointF Position;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        EC_Struct::TPointF Velocity;
        std::uint8_t cpp_padding_2[2];
        // Initialized to zero; unused by this renderer.
        std::uint8_t Unknown1A;
        std::uint8_t cpp_padding_3[1];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon08ECutter
