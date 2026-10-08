#pragma once
#include "units/EC_Cache.hpp"
#include "units/EC_Struct.hpp"

namespace EC_Cache {
    inline void TCacheControlEC_Create(TCacheControlEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    inline void TCacheDataEC_Create(TCacheDataEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace EC_Cache
