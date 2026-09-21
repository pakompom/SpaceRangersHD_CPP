#pragma once
#include "types/fGov.hpp"

namespace fGov {
    // Native battle launch selector, 1..3.
    extern std::int32_t GovernmentBattleDifficulty;

    void TfGov_Create(TfGov* Self);

    void TfGov_Destroy(TfGov* Self);

} // namespace fGov
