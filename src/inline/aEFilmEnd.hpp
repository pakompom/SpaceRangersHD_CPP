#pragma once
#include "units/EC_Struct.hpp"
#include "units/aEFilmEnd.hpp"

namespace aEFilmEnd {
    inline void TEFilmEnd_Create(TEFilmEnd* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace aEFilmEnd
