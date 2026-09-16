#pragma once
#include "runtime_support.hpp"

namespace MMSystem {
    using TFNTimeCallBack = pas::StdcallProc<void(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)>;

    inline constexpr std::int32_t TIME_PERIODIC = 0x00000001;

    inline constexpr std::int32_t TIME_CALLBACK_EVENT_SET = 0x00000010;

} // namespace MMSystem
