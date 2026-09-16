#pragma once
#include "runtime_support.hpp"

namespace SE_Garbage {
    extern std::int64_t PreviousVirtualUsageMB;

    extern std::int64_t PreviousPhysicalUsageMB;

    // Recursive checks return immediately.
    extern std::uint8_t CheckingMemoryUsage;

    // At most one warning per process.
    extern std::uint8_t LowMemoryWarningShown;

    void CheckMemoryUsage();

} // namespace SE_Garbage
