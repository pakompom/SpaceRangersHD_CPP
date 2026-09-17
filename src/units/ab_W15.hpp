#pragma once
#include "types/ab_W15.hpp"

namespace ab_W15 {
    extern std::int32_t W15ProjectileCount;

    void TabW15_Create(TabW15* Self);

    void TabW15_Destroy(TabW15* Self);

    void TabW15_Advance(TabW15* Self);

} // namespace ab_W15
