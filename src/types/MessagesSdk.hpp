#pragma once
#include "runtime_support.hpp"

namespace MessagesSdk {
    inline constexpr std::int32_t WM_DESTROY = 0x00000002;

    inline constexpr std::int32_t WM_PAINT = 0x0000000f;

    inline constexpr std::int32_t WM_CLOSE = 0x00000010;

    inline constexpr std::int32_t WM_QUIT = 0x00000012;

    inline constexpr std::int32_t WM_ERASEBKGND = 0x00000014;

    inline constexpr std::int32_t WM_ACTIVATEAPP = 0x0000001c;

    inline constexpr std::int32_t WM_CANCELMODE = 0x0000001f;

    inline constexpr std::int32_t WM_SETCURSOR = 0x00000020;

    inline constexpr std::int32_t WM_KEYDOWN = 0x00000100;

    inline constexpr std::int32_t WM_KEYUP = 0x00000101;

    inline constexpr std::int32_t WM_CHAR = 0x00000102;

    inline constexpr std::int32_t WM_SYSKEYDOWN = 0x00000104;

    inline constexpr std::int32_t WM_SYSKEYUP = 0x00000105;

    inline constexpr std::int32_t WM_TIMER = 0x00000113;

    inline constexpr std::int32_t WM_MOUSEMOVE = 0x00000200;

    inline constexpr std::int32_t WM_LBUTTONDOWN = 0x00000201;

    inline constexpr std::int32_t WM_LBUTTONUP = 0x00000202;

    inline constexpr std::int32_t WM_LBUTTONDBLCLK = 0x00000203;

    inline constexpr std::int32_t WM_RBUTTONDOWN = 0x00000204;

    inline constexpr std::int32_t WM_RBUTTONUP = 0x00000205;

    inline constexpr std::int32_t WM_RBUTTONDBLCLK = 0x00000206;

    inline constexpr std::int32_t WM_MBUTTONDOWN = 0x00000207;

    inline constexpr std::int32_t WM_MOUSEWHEEL = 0x0000020a;

    inline constexpr std::int32_t WM_MOUSELEAVE = 0x000002a3;

} // namespace MessagesSdk
