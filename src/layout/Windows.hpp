#pragma once
#include "types/Windows_group.hpp"

namespace Windows {
    static_assert(sizeof(void*) != 4 || sizeof(Windows::TSystemTime) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wYear) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wMonth) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wDayOfWeek) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wDay) == 6);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wHour) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wMinute) == 10);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wSecond) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TSystemTime, wMilliseconds) == 14);
    static_assert(sizeof(void*) != 4 || sizeof(Windows::TFileTime) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TFileTime, dwLowDateTime) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TFileTime, dwHighDateTime) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(Windows::TWin32FindDataA) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, dwFileAttributes) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, ftCreationTime) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, ftLastAccessTime) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, ftLastWriteTime) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, nFileSizeHigh) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, nFileSizeLow) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, dwReserved0) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, dwReserved1) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, cFileName) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TWin32FindDataA, cAlternateFileName) == 304);
    static_assert(sizeof(void*) != 4 || sizeof(Windows::TMsg) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TMsg, hwnd) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TMsg, message) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TMsg, wParam) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TMsg, lParam) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TMsg, time) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TMsg, pt) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(Windows::TTrackMouseEvent) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TTrackMouseEvent, cbSize) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TTrackMouseEvent, dwFlags) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TTrackMouseEvent, hwndTrack) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Windows::TTrackMouseEvent, dwHoverTime) == 12);

} // namespace Windows
