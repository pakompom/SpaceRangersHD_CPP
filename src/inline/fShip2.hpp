#pragma once
#include "units/fShip2.hpp"

namespace fShip2 {
    inline void TfShip2_ProcessWindowMessage(fShip2::TfShip2* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        Self->fShip2::TfShip2::ProcessWindowMessage(Message, WParam, LParam);
    }

} // namespace fShip2
