#pragma once
#include "types/SE_Process.hpp"

namespace SE_Process {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Process::TProcessSE) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, Space) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, FirstObject) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, LastObject) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, RetainedObjects) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, BackgroundTimer) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, PreviousViewRect) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, ViewRect) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, RadarCenter) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, RadarRange) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, ActionRange) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, ActionColor) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Process::TProcessSE, SystemRadius) == 76);

} // namespace SE_Process
