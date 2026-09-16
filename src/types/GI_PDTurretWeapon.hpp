#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/Types.hpp"

namespace GI_PDTurretWeapon {
    struct TPDWeaponParticle;

    struct TPSPDWeaponGI;

    using PPDWeaponParticle = TPDWeaponParticle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSPDWeaponGI : GI_PSWeapon::TPSWeaponGI {
        PAS_CLASS_META(TPSPDWeaponGI, GI_PSWeapon::TPSWeaponGI, "TPSPDWeaponGI", 324)
        void p_destroy() override;
        // Native empty override.
        void Invalidate() override;
        void SetPosition(Types::TPoint Position) override;
        void SetTargetPoint(Types::TPoint Point) override;
        void UpdateHitTestBounds() override;
        void SetActive(std::uint8_t Enabled) override;
        void ClearParticles();
        void GrowParticles();
        PPDWeaponParticle AddParticle();
        void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) override;
        void Draw(Types::TRect ClipRect) override;
        PPDWeaponParticle Particles;
        std::int32_t ParticleCount;
        std::int32_t ParticleCapacity;
        float OriginalLength;
        std::uint16_t ParticleColor;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPDWeaponParticle {
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

} // namespace GI_PDTurretWeapon
