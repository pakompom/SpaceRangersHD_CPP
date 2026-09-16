#pragma once
#include "runtime_support.hpp"

namespace ab_Global {
    struct TSphericalBearingState;

    struct TSphericalBearingDistance;

    #pragma pack(push, 1)
    struct TSphericalBearingState {
        double LongitudeDegrees;
        double PolarAngleDegrees;
        double BearingDegrees;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TSphericalBearingDistance {
        double BearingDeltaDegrees;
        double Distance;
    };
    #pragma pack(pop)

    using TMatrix4D = pas::Array<pas::Array<double, 0, 3>, 0, 3>;

    inline constexpr std::int32_t abkRegeneration = 0;

    inline constexpr std::int32_t abkSpeed = 1;

    inline constexpr std::int32_t abkSlow = 2;

    inline constexpr std::int32_t abkWeaponLock = 3;

    inline constexpr std::int32_t abkDamage = 4;

    inline constexpr std::int32_t abkRecharge = 5;

    inline constexpr std::int32_t abkShield = 6;

    inline constexpr std::int32_t abkInvisibility = 7;

    inline constexpr std::int32_t ArcadeBonusKindMask = 0x000000ff;

    inline constexpr std::uint32_t ArcadeHiddenBonusFlag = 0x80000000u;

} // namespace ab_Global
