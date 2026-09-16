#pragma once
#include "types/GI_PSWeapon02FragCannon.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon02FragCannon {
    extern pas::DynArray<GI_PSWeapon02FragCannon::TFragCannonPalette> FragCannonPalettes;

    extern std::int32_t FragCannonRandomIndex;

    extern pas::Array<std::int32_t, 0, 127> FragCannonRandomValues;

    std::int32_t NextFragCannonRandom();

    void LoadFragCannonPalettes();

    void TPSWeapon02FragCannon_Create(TPSWeapon02FragCannon* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon02FragCannon_Destroy(TPSWeapon02FragCannon* Self);

} // namespace GI_PSWeapon02FragCannon
