#pragma once
#include "units/fRuinsTalk.hpp"

namespace fRuinsTalk {
    inline void TfRuinsTalk_ProcessWindowMessage(fRuinsTalk::TfRuinsTalk* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        Self->fRuinsTalk::TfRuinsTalk::ProcessWindowMessage(Message, WParam, LParam);
    }

} // namespace fRuinsTalk
