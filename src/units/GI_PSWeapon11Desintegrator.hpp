#pragma once
#include "types/GI_PSWeapon11Desintegrator.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon11Desintegrator {
    extern pas::DynArray<GI_PSWeapon11Desintegrator::TDesintegratorPalette> DesintegratorPalettes;

    void LoadDesintegratorPalettes();

    void TPSWeapon11Desintegrator_Create(TPSWeapon11Desintegrator* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon11Desintegrator_Destroy(TPSWeapon11Desintegrator* Self);

} // namespace GI_PSWeapon11Desintegrator
