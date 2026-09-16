#pragma once
#include "types/EC_CacheLightPal.hpp"
#include "types/System.hpp"
#include "types/Windows_group.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace EC_CacheLightPal {
    TCLightPalEC* AcquireOrCreateLightPalette(EC_Cache::TCacheControlEC* Control);

    void GetLightPaletteMaskInfo(std::uint32_t Mask, System::PCardinal Shift, System::PCardinal BitCount, System::PCardinal LevelCount);

    WindowsSdk::PWORD BuildLightPalette(System::PCardinal Palette, std::int32_t ColorCount, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask);

    void FreeLightPalette(WindowsSdk::PWORD Palette);

    void TCLightPalEC_Create(TCLightPalEC* Self);

    void TCLightPalEC_Destroy(TCLightPalEC* Self);

} // namespace EC_CacheLightPal
