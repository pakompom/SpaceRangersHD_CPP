#pragma once
#include "types/fGov.hpp"

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fGov {
    extern std::int32_t GovernmentBattleDifficulty;

    void PayBailMoney(aShip::TShip* Ship);

    void TfGov_Create(TfGov* Self);

    void TfGov_Destroy(TfGov* Self);

} // namespace fGov
