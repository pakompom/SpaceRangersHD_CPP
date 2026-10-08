#pragma once
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"

namespace WindowsImports {
    inline void ZeroMemory(void* Destination, std::uint32_t Length) {
        Windows::FillMemory(Destination, Length, 0);
    }

} // namespace WindowsImports
