#pragma once
#include "units/EC_Struct.hpp"
#include "units/GR_Sound.hpp"

namespace GR_Sound {
    inline void TSoundBufferControl_Create(TSoundBufferControl* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace GR_Sound
