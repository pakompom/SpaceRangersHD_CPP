#pragma once
#include "types/GI_PSWeapon05Treton.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon05Treton {
    extern pas::DynArray<GI_PSWeapon05Treton::TTretonPalette> TretonPalettes;

    void LoadTretonPalettes();

    void TPSWeapon05Treton_Create(TPSWeapon05Treton* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon05Treton_Destroy(TPSWeapon05Treton* Self);

} // namespace GI_PSWeapon05Treton
