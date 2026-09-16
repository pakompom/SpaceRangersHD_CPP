#pragma once
#include "types/fSelectFace.hpp"

namespace fSelectFace {
    static_assert(sizeof(void*) != 4 || sizeof(fSelectFace::TfSelectFace) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, PlayerRace) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, CaptainPortraitIndex) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, LastPortraitByRace) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, PlayerName) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, PlayerNameEdited) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, NationalityCosts) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, AvailableMoney) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fSelectFace::TfSelectFace, AcceptedCost) == 268);

} // namespace fSelectFace
