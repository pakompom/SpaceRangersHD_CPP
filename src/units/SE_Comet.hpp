#pragma once
#include "types/SE_Comet.hpp"
#include "types/Types.hpp"

namespace SE_Comet {
    void TCometSE_Create(TCometSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition);

    void TCometSE_Destroy(TCometSE* Self);

} // namespace SE_Comet
