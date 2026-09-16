#pragma once
#include "runtime_support.hpp"

namespace ShellAPI {
    std::uint32_t PAS_STDCALL ShellExecuteA(std::uint32_t Window, std::uint8_t* Operation, std::uint8_t* FileName, std::uint8_t* Parameters, std::uint8_t* Directory, std::int32_t ShowCommand);

} // namespace ShellAPI
