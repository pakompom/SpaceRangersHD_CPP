#pragma once
#include "types/GI_PSWeapon16Esodafer.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon16Esodafer {
    extern pas::DynArray<GI_PSWeapon16Esodafer::TEsodaferPalette> EsodaferPalettes;

    void LoadEsodaferPalettes();

    void TPSWeapon16Esodafer_Create(TPSWeapon16Esodafer* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon16Esodafer_Destroy(TPSWeapon16Esodafer* Self);

} // namespace GI_PSWeapon16Esodafer
