#pragma once
#include "runtime_support.hpp"

namespace SE_Garbage {
    extern std::int64_t PreviousVirtualUsageMB;

    extern std::int64_t PreviousPhysicalUsageMB;

    extern std::uint8_t CheckingMemoryUsage;

    extern std::uint8_t LowMemoryWarningShown;

    void CheckMemoryUsage();

} // namespace SE_Garbage
