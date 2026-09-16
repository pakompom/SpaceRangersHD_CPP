#pragma once
#include "runtime_support.hpp"

namespace ExceptionInfo {
    using TRaiseExceptionCallback = pas::StdcallProc<void(std::uint32_t, std::uint32_t, std::uint32_t, void*)>;

} // namespace ExceptionInfo
