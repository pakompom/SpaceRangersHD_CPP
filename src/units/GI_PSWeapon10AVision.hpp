#pragma once
#include "types/GI_PSWeapon10AVision.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon10AVision {
    extern pas::DynArray<GI_PSWeapon10AVision::TAVisionPalette> AVisionPalettes;

    extern pas::DynArray<GI_PSWeapon10AVision::TGAISet> AVisionAnimationPaths;

    void LoadAVisionPalettes();

    void TPSWeapon10AVision_Create(TPSWeapon10AVision* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon10AVision_Destroy(TPSWeapon10AVision* Self);

} // namespace GI_PSWeapon10AVision
