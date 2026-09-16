#pragma once
#include "types/GI_PSWeapon03Lezka.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon03Lezka {
    extern pas::DynArray<GI_PSWeapon03Lezka::TLezkaPalette> LezkaPrimaryPalettes;

    extern pas::DynArray<GI_PSWeapon03Lezka::TLezkaPalette> LezkaSecondaryPalettes;

    void LoadLezkaPalettes();

    void TPSWeapon03Lezka_Create(TPSWeapon03Lezka* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon03Lezka_Destroy(TPSWeapon03Lezka* Self);

} // namespace GI_PSWeapon03Lezka
