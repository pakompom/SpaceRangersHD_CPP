#pragma once
#include "types/abWall.hpp"
#include "types/ab_Zone.hpp"

namespace abWall {
    extern std::uint32_t BarrierColor;

    // Native index local starts at zero and increments to the bound 2.
    extern pas::Array<std::uint32_t, 0, 1> BarrierHaloColors;

    TabWall* ab_Wall_FindZone(ab_Zone::PabZone Zone);

    void ab_Wall_BuildBarrierImages();

    void TabWall_Create(TabWall* Self);

    void TabWall_Destroy(TabWall* Self);

    void TabWall_Advance(TabWall* Self);

} // namespace abWall
