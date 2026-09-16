#pragma once
#include "types/GI_PSWeapon13IMHO.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon13IMHO {
    extern pas::DynArray<GI_PSWeapon13IMHO::TIMHOPalette> IMHOPalettes;

    void LoadIMHOPalettes();

    void TPSWeapon13IMHO_Create(TPSWeapon13IMHO* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon13IMHO_Destroy(TPSWeapon13IMHO* Self);

} // namespace GI_PSWeapon13IMHO
