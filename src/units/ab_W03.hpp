#pragma once
#include "types/ab_W03.hpp"

namespace ab_W03 {
    extern std::int32_t ProjectileCount;

    void TabW03_Create(TabW03* Self);

    void TabW03_Destroy(TabW03* Self);

    void TabW03_Advance(TabW03* Self);

} // namespace ab_W03
