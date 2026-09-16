#pragma once
#include "runtime_support.hpp"

namespace TlHelp32 {
    struct TModuleEntry32;

    #pragma pack(push, 1)
    struct TModuleEntry32 {
        std::uint32_t dwSize;
        std::uint32_t th32ModuleID;
        std::uint32_t th32ProcessID;
        std::uint32_t GlblcntUsage;
        std::uint32_t ProccntUsage;
        std::uint8_t* modBaseAddr;
        std::uint32_t modBaseSize;
        std::uint32_t hModule;
        pas::Array<std::uint8_t, 0, 255> szModule;
        pas::Array<std::uint8_t, 0, 259> szExePath;
    };
    #pragma pack(pop)

} // namespace TlHelp32
