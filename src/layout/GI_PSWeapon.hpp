#pragma once
#include "types/GI_PSWeapon.hpp"

namespace GI_PSWeapon {
    static_assert(sizeof(void*) != 4 || sizeof(GI_PSWeapon::TPSWeaponGI) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon::TPSWeaponGI, TargetPoint) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon::TPSWeaponGI, RemainingTicks) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_PSWeapon::TPSWeaponGI, LifetimeTicks) == 300);

} // namespace GI_PSWeapon
