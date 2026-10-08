#pragma once
#include "units/EC_Str.hpp"

namespace EC_Str {
    inline void TStringsEC_Create(TStringsEC* Self) {
        pas::object_create(Self);
    }

} // namespace EC_Str
