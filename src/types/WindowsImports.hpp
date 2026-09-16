#pragma once
#include "runtime_support.hpp"

namespace WindowsImports {
    using THandle = std::uint32_t;

    inline constexpr std::uint32_t GENERIC_READ = 0x80000000u;

    inline constexpr std::int32_t GENERIC_WRITE = 0x40000000;

    inline constexpr std::int32_t FILE_SHARE_READ = 1;

    inline constexpr std::int32_t FILE_SHARE_WRITE = 2;

    inline constexpr std::int32_t CREATE_ALWAYS = 2;

    inline constexpr std::int32_t OPEN_EXISTING = 3;

    inline constexpr std::int32_t FILE_ATTRIBUTE_DIRECTORY = 0x00000010;

    inline constexpr std::int32_t FILE_ATTRIBUTE_NORMAL = 0x00000080;

    inline constexpr std::int32_t FILE_BEGIN = 0;

    inline constexpr std::int32_t FILE_CURRENT = 1;

    inline constexpr std::int32_t FILE_END = 2;

    inline constexpr std::uint32_t INVALID_HANDLE_VALUE = 0xffffffffu;

    inline constexpr std::int32_t ERROR_FILE_NOT_FOUND = 2;

    inline constexpr std::int32_t ERROR_PATH_NOT_FOUND = 3;

    inline constexpr std::int32_t ERROR_INVALID_NAME = 123;

    inline constexpr std::int32_t EVENT_MODIFY_STATE = 2;

    inline constexpr std::int32_t MB_ICONERROR = 0x00000010;

    inline constexpr std::int32_t MB_OKCANCEL = 1;

    inline constexpr std::int32_t IDOK = 1;

    inline constexpr std::int32_t MAX_PATH = 260;

    inline constexpr std::uint32_t HKEY_LOCAL_MACHINE = 0x80000002u;

} // namespace WindowsImports
