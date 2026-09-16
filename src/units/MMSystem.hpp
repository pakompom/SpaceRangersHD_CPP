#pragma once
#include "types/MMSystem.hpp"

namespace MMSystem {
    std::uint32_t PAS_STDCALL timeBeginPeriod(std::uint32_t Period);

    std::uint32_t PAS_STDCALL timeEndPeriod(std::uint32_t Period);

    std::uint32_t PAS_STDCALL timeGetTime();

    std::uint32_t PAS_STDCALL timeKillEvent(std::uint32_t TimerId);

    std::uint32_t PAS_STDCALL timeSetEvent(std::uint32_t Delay, std::uint32_t Resolution, TFNTimeCallBack Callback, std::uint32_t User, std::uint32_t Flags);

} // namespace MMSystem
