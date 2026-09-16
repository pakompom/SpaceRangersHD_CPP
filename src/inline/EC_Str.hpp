#pragma once
#include "units/EC_Str.hpp"

namespace EC_Str {
    // Case-sensitive NUL-terminated comparison returning -1, 0 or 1. Nil sorts before every nonnil pointer, including an empty string.
    inline std::int32_t CompareWideChars(char16_t* Left, char16_t* Right) {
        return pas::compare_wide_chars(Left, Right);
    }

} // namespace EC_Str
