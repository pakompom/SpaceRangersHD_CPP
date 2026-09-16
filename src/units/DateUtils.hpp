#pragma once
#include "runtime_support.hpp"
#include "types/System.hpp"

namespace DateUtils {
    std::int64_t DateTimeToUnix(System::TDateTime AValue);

    System::TDateTime UnixToDateTime(std::int64_t AValue);

} // namespace DateUtils
