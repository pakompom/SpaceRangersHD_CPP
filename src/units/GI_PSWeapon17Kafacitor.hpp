#pragma once
#include "types/GI_PSWeapon17Kafacitor.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon17Kafacitor {
    extern pas::DynArray<GI_PSWeapon17Kafacitor::TKafacitorPalette> KafacitorPalettes;

    extern pas::DynArray<GI_PSWeapon17Kafacitor::TGAISet> KafacitorAnimationPaths;

    void LoadKafacitorPalettes();

    void TPSWeapon17Kafacitor_Create(TPSWeapon17Kafacitor* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon17Kafacitor_Destroy(TPSWeapon17Kafacitor* Self);

} // namespace GI_PSWeapon17Kafacitor
