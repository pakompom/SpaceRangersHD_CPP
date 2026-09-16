#pragma once
#include "types/GR_gi.hpp"

namespace GR_gi {
    void PrepareRawGiColorCache(void* Data);

    void TgiGR_Create(TgiGR* Self);

    void TgiGR_Destroy(TgiGR* Self);

} // namespace GR_gi
