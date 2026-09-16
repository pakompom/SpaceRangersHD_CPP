#pragma once
#include "types/ThreadCalc.hpp"

namespace ThreadCalc {
    static_assert(sizeof(void*) != 4 || sizeof(ThreadCalc::TThreadCalc) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ThreadCalc::TThreadCalc, Job) == 44);

} // namespace ThreadCalc
