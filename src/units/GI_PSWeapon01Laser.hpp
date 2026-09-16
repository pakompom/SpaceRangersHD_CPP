#pragma once
#include "types/GI_PSWeapon01Laser.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon01Laser {
    extern pas::DynArray<GI_PSWeapon01Laser::TBeamLaserPalette> BeamLaserPalettes;

    extern pas::DynArray<float> BeamLaserWidths;

    extern pas::DynArray<std::int32_t> BeamLaserDurations;

    void LoadBeamLaserPalettes();

    void TPSWeapon01Laser_Create(TPSWeapon01Laser* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon01Laser_Destroy(TPSWeapon01Laser* Self);

} // namespace GI_PSWeapon01Laser
