#pragma once
#include "types/GI_PSWeapon06Phaser.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon06Phaser {
    extern pas::DynArray<GI_PSWeapon06Phaser::TPhaserPalette> PhaserPalettes;

    void LoadPhaserPalettes();

    void TPSWeapon06Phaser_Create(TPSWeapon06Phaser* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon06Phaser_Destroy(TPSWeapon06Phaser* Self);

} // namespace GI_PSWeapon06Phaser
