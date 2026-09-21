#pragma once
#include "runtime_support.hpp"

namespace FGInt {
    struct TFGInt;

    enum TSign : std::uint8_t {
        negative = 0,
        positive = 1,
    };

    // Native record RTTI.
    struct TFGInt {
        TSign Sign;
        std::uint8_t cpp_padding[3];
        // Count followed by 31-bit limbs.
        pas::DynArray<std::uint32_t> Number;
    };

} // namespace FGInt
