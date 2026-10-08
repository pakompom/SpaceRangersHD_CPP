#pragma once
#include "units/CheatCode.hpp"

namespace CheatCode {
    // Compiler unit entry calls the virtual destructor directly.
    inline void UnitFinalize() {
        pas::destroy(CheatEntries);
    }

} // namespace CheatCode
