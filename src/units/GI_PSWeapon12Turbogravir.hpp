#pragma once
#include "types/GI_PSWeapon12Turbogravir.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon12Turbogravir {
    extern pas::DynArray<GI_PSWeapon12Turbogravir::TTurbogravirPalette> TurbogravirPrimaryPalettes;

    extern pas::DynArray<GI_PSWeapon12Turbogravir::TTurbogravirPalette> TurbogravirSecondaryPalettes;

    void LoadTurbogravirPalettes();

    void TPSWeapon12Turbogravir_Create(TPSWeapon12Turbogravir* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon12Turbogravir_Destroy(TPSWeapon12Turbogravir* Self);

    void TPSBlueWhirlGI_Create(TPSBlueWhirlGI* Self, GI_MessageLoop::TObjectGI* Owner);

} // namespace GI_PSWeapon12Turbogravir
