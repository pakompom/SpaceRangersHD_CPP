#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon06Phaser {
    struct TPhaserParticle;

    struct TPSWeapon06Phaser;

    using TPhaserPalette = pas::Array<float, 0, 8>;

    using PPhaserParticle = TPhaserParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeapon06Phaser : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSWeapon06Phaser, GI_PSWeapon::TPSWeaponGI, "TPSWeapon06Phaser", 324)
        void p_destroy() override;
        void Invalidate() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateHitTestBounds() override;
        void ClearParticles();
        void GrowParticles();
        PPhaserParticle AddParticle();
        void AdvanceWave();
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        PPhaserParticle Particles;
        std::int32_t ParticleCount;
        std::int32_t ParticleCapacity;
        float OriginalLength;
        std::int32_t PaletteIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPhaserParticle {
        PPhaserParticle Next;
        PPhaserParticle Prev;
        std::uint8_t Kind;
        std::uint8_t cpp_padding[3];
        EC_Struct::TPointF Position;
        float Incoming;
        float Displacement;
        float Reflected;
        std::uint16_t Color;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding_2[1];
        float Phase;
        float PhaseStep;
        std::uint8_t PhaseCountdown;
        std::uint8_t cpp_padding_3[3];
    };
    #pragma pack(pop)

} // namespace GI_PSWeapon06Phaser
