#pragma once
#include "types/SE_Meteorite.hpp"
#include "types/Types.hpp"

namespace SE_Meteorite {
    void TMeteoriteSE_Create(TMeteoriteSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition);

    void TMeteoriteSE_Destroy(TMeteoriteSE* Self);

} // namespace SE_Meteorite
