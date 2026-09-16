#pragma once
#include "types/GI_PSWeapon09MResonator.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon09MResonator {
    extern pas::DynArray<GI_PSWeapon09MResonator::TMResonatorPalette> MResonatorPalettes;

    extern pas::DynArray<GI_PSWeapon09MResonator::TGAISet> MResonatorAnimationPaths;

    void LoadMResonatorPalettes();

    void TPSWeapon09BranchGI_Create(TPSWeapon09BranchGI* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon09BranchGI_Destroy(TPSWeapon09BranchGI* Self);

    void TPSWeapon09MResonator_Create(TPSWeapon09MResonator* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon09MResonator_Destroy(TPSWeapon09MResonator* Self);

} // namespace GI_PSWeapon09MResonator
