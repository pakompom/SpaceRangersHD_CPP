#pragma once
#include "types/CheatCode.hpp"

namespace CheatCode {
    static_assert(sizeof(void*) != 4 || sizeof(CheatCode::TCheatList) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(CheatCode::TCheatEntry) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(CheatCode::TCheatEntry, Text) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(CheatCode::TCheatEntry, Callback) == 4);

} // namespace CheatCode
