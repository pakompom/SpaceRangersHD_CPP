#pragma once
#include "types/fGov.hpp"

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fGov {
    // Native battle launch selector, 1..3.
    extern std::int32_t GovernmentBattleDifficulty;

    // Preserve the native receiver evaluation before the bounded payment,
    // with the clamp cells allocated before the receiver cell.
    void PayBailMoney(aShip::TShip* Ship);

    void TfGov_Create(TfGov* Self);

    void TfGov_Destroy(TfGov* Self);

} // namespace fGov
