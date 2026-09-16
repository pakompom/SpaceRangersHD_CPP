#pragma once
#include "types/GI_PSWeapon08ECutter.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon08ECutter {
    extern pas::DynArray<GI_PSWeapon08ECutter::TECutterPalette> ECutterPalettes;

    void LoadECutterPalettes();

    void TPSWeapon08ECutter_Create(TPSWeapon08ECutter* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon08ECutter_Destroy(TPSWeapon08ECutter* Self);

} // namespace GI_PSWeapon08ECutter
