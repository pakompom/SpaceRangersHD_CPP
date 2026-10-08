#pragma once
#include "units/EC_Struct.hpp"
#include "units/fTalk.hpp"

namespace fTalk {
    inline void TfTalkA_Create(TfTalkA* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace fTalk
