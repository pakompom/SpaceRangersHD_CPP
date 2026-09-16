#pragma once
#include "types/GI_PSWeapon07Blaster.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSWeapon07Blaster {
    extern pas::DynArray<GI_PSWeapon07Blaster::TBlasterPalette> BlasterPalettes;

    extern std::int32_t BlasterRandomIndex;

    extern pas::Array<std::int32_t, 0, 127> BlasterRandomValues;

    std::int32_t NextBlasterRandom();

    void LoadBlasterPalettes();

    void TPSWeapon07Blaster_Create(TPSWeapon07Blaster* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSWeapon07Blaster_Destroy(TPSWeapon07Blaster* Self);

} // namespace GI_PSWeapon07Blaster
