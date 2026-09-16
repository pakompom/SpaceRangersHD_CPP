#pragma once
#include "types/GI_PDTurretWeapon.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PDTurretWeapon {
    void TPSPDWeaponGI_Create(TPSPDWeaponGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TPSPDWeaponGI_Destroy(TPSPDWeaponGI* Self);

} // namespace GI_PDTurretWeapon
