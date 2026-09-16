#pragma once
#include "types/TlHelp32.hpp"

namespace TlHelp32 {
    static_assert(sizeof(void*) != 4 || sizeof(TlHelp32::TModuleEntry32) == 548);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, dwSize) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, th32ModuleID) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, th32ProcessID) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, GlblcntUsage) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, ProccntUsage) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, modBaseAddr) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, modBaseSize) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, hModule) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, szModule) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(TlHelp32::TModuleEntry32, szExePath) == 288);

} // namespace TlHelp32
