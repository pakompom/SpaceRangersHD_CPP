#pragma once
#include "types/GI_PSWeapon.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon {
    void TPSWeaponGI_Create(TPSWeaponGI* Self, GI_MessageLoop::TObjectGI* Owner);

} // namespace GI_PSWeapon
