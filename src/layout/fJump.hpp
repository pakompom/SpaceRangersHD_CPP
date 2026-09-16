#pragma once
#include "types/fJump.hpp"

namespace fJump {
    static_assert(sizeof(void*) != 4 || sizeof(fJump::TfJump) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, TransitionTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, LoadingStarted) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, NoPendingLoads) == 213);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, Progress) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, LoadPanel) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, MovieStartTick) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, MovieTimer) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fJump::TfJump, RestoreOrdersOnArrival) == 232);

} // namespace fJump
