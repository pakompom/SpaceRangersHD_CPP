#pragma once
#include "types/PathClass.hpp"

namespace PathClass {
    void TPath_Create(TPath* Self);

    // Frees the sequence and containers without calling Reset.
    void TPath_Destroy(TPath* Self);

} // namespace PathClass
