#pragma once
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace WindowsSdk {
    extern const WindowsSdk::MakeIntResource IDC_ARROW;

    extern const pas::WideString advapi32;

    extern const pas::WideString kernel32;

    extern const pas::WideString gdi32;

    extern const pas::WideString user32;

    std::uint32_t PAS_STDCALL GetVersion();

    HGLOBAL PAS_STDCALL GlobalAlloc(UINT uFlags, std::uint32_t dwBytes);

    void* PAS_STDCALL GlobalLock(HGLOBAL hMem);

    BOOL PAS_STDCALL GlobalUnlock(HGLOBAL hMem);

    void PAS_STDCALL GlobalMemoryStatus(TMemoryStatus& lpBuffer);

    THandle PAS_STDCALL HeapCreate(std::uint32_t flOptions, std::uint32_t dwInitialSize, std::uint32_t dwMaximumSize);

    BOOL PAS_STDCALL HeapDestroy(THandle hHeap);

    inline void* PAS_STDCALL HeapAlloc(THandle hHeap, std::uint32_t dwFlags, std::uint32_t dwBytes);

    inline void* PAS_STDCALL HeapReAlloc(THandle hHeap, std::uint32_t dwFlags, void* lpMem, std::uint32_t dwBytes);

    inline BOOL PAS_STDCALL HeapFree(THandle hHeap, std::uint32_t dwFlags, void* lpMem);

    inline THandle PAS_STDCALL GetProcessHeap();

    BOOL PAS_STDCALL SetProcessAffinityMask(THandle hProcess, std::uint32_t dwProcessAffinityMask);

    THandle PAS_STDCALL GetCurrentProcess();

    THandle PAS_STDCALL CreateThread(void* lpThreadAttributes, std::uint32_t dwStackSize, TFNThreadStartRoutine lpStartAddress, void* lpParameter, std::uint32_t dwCreationFlags, std::uint32_t& lpThreadId);

    THandle PAS_STDCALL GetCurrentThread();

    BOOL PAS_STDCALL SetThreadPriority(THandle hThread, std::int32_t nPriority);

    std::int32_t PAS_STDCALL GetThreadPriority(THandle hThread);

    std::uint32_t PAS_STDCALL ResumeThread(THandle hThread);

    BOOL PAS_STDCALL SetEvent(THandle hEvent);

    BOOL PAS_STDCALL ResetEvent(THandle hEvent);

    std::uint32_t PAS_STDCALL WaitForSingleObject(THandle hHandle, std::uint32_t dwMilliseconds);

    std::uint32_t PAS_STDCALL WaitForMultipleObjects(std::uint32_t nCount, PWOHandleArray lpHandles, BOOL bWaitAll, std::uint32_t dwMilliseconds);

    std::uint32_t PAS_STDCALL GetFileSize(THandle hFile, void* lpFileSizeHigh);

    void PAS_STDCALL GetLocalTime(Windows::TSystemTime& lpSystemTime);

    void PAS_STDCALL GetSystemInfo(TSystemInfo& lpSystemInfo);

    BOOL PAS_STDCALL FileTimeToSystemTime(pas::ConstRef<Windows::TFileTime> lpFileTime, Windows::TSystemTime& lpSystemTime);

    std::int32_t PAS_STDCALL CompareFileTime(pas::ConstRef<Windows::TFileTime> lpFileTime1, const Windows::TFileTime& lpFileTime2);

    std::uint32_t PAS_STDCALL FormatMessageA(std::uint32_t dwFlags, void* lpSource, std::uint32_t dwMessageId, std::uint32_t dwLanguageId, std::uint8_t* lpBuffer, std::uint32_t nSize, void* Arguments);

    HMODULE PAS_STDCALL LoadLibraryW(char16_t* lpLibFileName);

    HMODULE PAS_STDCALL GetModuleHandle(std::uint8_t* lpModuleName);

    HMODULE PAS_STDCALL GetModuleHandleW(char16_t* lpModuleName);

    UINT PAS_STDCALL GetSystemDirectoryW(char16_t* lpBuffer, UINT uSize);

    BOOL PAS_STDCALL CreateDirectoryA(std::uint8_t* lpPathName, PSecurityAttributes lpSecurityAttributes);

    BOOL PAS_STDCALL SetFileAttributesA(std::uint8_t* lpFileName, std::uint32_t dwFileAttributes);

    BOOL PAS_STDCALL DeleteFile(std::uint8_t* lpFileName);

    THandle PAS_STDCALL FindFirstFile(std::uint8_t* lpFileName, TWin32FindData& lpFindFileData);

    THandle PAS_STDCALL FindFirstFileW(char16_t* lpFileName, TWin32FindDataW& lpFindFileData);

    BOOL PAS_STDCALL FindNextFile(THandle hFindFile, TWin32FindData& lpFindFileData);

    BOOL PAS_STDCALL FindNextFileW(THandle hFindFile, TWin32FindDataW& lpFindFileData);

    BOOL PAS_STDCALL CopyFile(std::uint8_t* lpExistingFileName, std::uint8_t* lpNewFileName, BOOL bFailIfExists);

    BOOL PAS_STDCALL CopyFileW(char16_t* lpExistingFileName, char16_t* lpNewFileName, BOOL bFailIfExists);

    BOOL PAS_STDCALL MoveFileW(char16_t* lpExistingFileName, char16_t* lpNewFileName);

    BOOL PAS_STDCALL GetVolumeInformationA(std::uint8_t* lpRootPathName, std::uint8_t* lpVolumeNameBuffer, std::uint32_t nVolumeNameSize, PDWORD lpVolumeSerialNumber, std::uint32_t& lpMaximumComponentLength, std::uint32_t& lpFileSystemFlags, std::uint8_t* lpFileSystemNameBuffer, std::uint32_t nFileSystemNameSize);

    BOOL PAS_STDCALL SetPriorityClass(THandle hProcess, std::uint32_t dwPriorityClass);

    std::uint32_t PAS_STDCALL GetPriorityClass(THandle hProcess);

    BOOL PAS_STDCALL QueryPerformanceFrequency(Windows::TLargeInteger& lpFrequency);

    BOOL PAS_STDCALL GetVersionEx(TOSVersionInfo& lpVersionInformation);

    BOOL PAS_STDCALL DeleteObject(HGDIOBJ p1);

    HGDIOBJ PAS_STDCALL GetStockObject(std::int32_t Index);

    UINT PAS_STDCALL RegisterWindowMessage(std::uint8_t* lpString);

    BOOL PAS_STDCALL TrackMouseEvent(Windows::TTrackMouseEvent& EventTrack);

    std::int32_t PAS_STDCALL DispatchMessageW(const Windows::TMsg& lpMsg);

    BOOL PAS_STDCALL TranslateMessage(const Windows::TMsg& lpMsg);

    BOOL PAS_STDCALL PeekMessage(Windows::TMsg& lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

    BOOL PAS_STDCALL PeekMessageW(Windows::TMsg& lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

    BOOL PAS_STDCALL PostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    LRESULT PAS_STDCALL DefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    void PAS_STDCALL PostQuitMessage(std::int32_t nExitCode);

    UINT PAS_STDCALL GetDoubleClickTime();

    ATOM PAS_STDCALL RegisterClassW(const TWndClassW& lpWndClass);

    BOOL PAS_STDCALL DestroyWindow(HWND hWnd);

    BOOL PAS_STDCALL ShowWindow(HWND hWnd, std::int32_t nCmdShow);

    BOOL PAS_STDCALL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, std::int32_t X, std::int32_t Y, std::int32_t cx, std::int32_t cy, UINT uFlags);

    BOOL PAS_STDCALL OpenClipboard(HWND hWndNewOwner);

    BOOL PAS_STDCALL CloseClipboard();

    THandle PAS_STDCALL SetClipboardData(UINT uFormat, THandle hMem);

    THandle PAS_STDCALL GetClipboardData(UINT uFormat);

    BOOL PAS_STDCALL EmptyClipboard();

    std::uint32_t PAS_STDCALL CharUpperBuffW(char16_t* lpsz, std::uint32_t cchLength);

    std::uint32_t PAS_STDCALL CharLowerBuffW(char16_t* lpsz, std::uint32_t cchLength);

    HWND PAS_STDCALL SetFocus(HWND hWnd);

    SHORT PAS_STDCALL GetAsyncKeyState(std::int32_t vKey);

    std::uint32_t PAS_STDCALL MsgWaitForMultipleObjects(std::uint32_t nCount, void* pHandles, BOOL fWaitAll, std::uint32_t dwMilliseconds, std::uint32_t dwWakeMask);

    UINT PAS_STDCALL SetTimer(HWND hWnd, UINT nIDEvent, UINT uElapse, TFNTimerProc lpTimerFunc);

    BOOL PAS_STDCALL UpdateWindow(HWND hWnd);

    HWND PAS_STDCALL GetForegroundWindow();

    BOOL PAS_STDCALL SetForegroundWindow(HWND hWnd);

    HDC PAS_STDCALL GetDC(HWND hWnd);

    std::int32_t PAS_STDCALL ReleaseDC(HWND hWnd, HDC hDC);

    BOOL PAS_STDCALL RedrawWindow(HWND hWnd, PRect lprcUpdate, HRGN hrgnUpdate, UINT flags);

    BOOL PAS_STDCALL SetWindowTextA(HWND hWnd, std::uint8_t* lpString);

    BOOL PAS_STDCALL AdjustWindowRect(TRect& lpRect, std::uint32_t dwStyle, BOOL bMenu);

    std::int32_t PAS_STDCALL ShowCursor(BOOL bShow);

    BOOL PAS_STDCALL SetCursorPos(std::int32_t X, std::int32_t Y);

    HCURSOR PAS_STDCALL SetCursor(HICON hCursor);

    BOOL PAS_STDCALL GetCursorPos(TPoint& lpPoint);

    BOOL PAS_STDCALL ClipCursor(PRect lpRect);

    BOOL PAS_STDCALL ClientToScreen(HWND hWnd, TPoint& lpPoint);

    BOOL PAS_STDCALL ScreenToClient(HWND hWnd, TPoint& lpPoint);

    BOOL PAS_STDCALL IntersectRect(TRect& lprcDst, const TRect& lprcSrc1, const TRect& lprcSrc2);

    BOOL PAS_STDCALL UnionRect(TRect& lprcDst, const TRect& lprcSrc1, const TRect& lprcSrc2);

    std::int32_t PAS_STDCALL SetWindowLong(HWND hWnd, std::int32_t nIndex, std::int32_t dwNewLong);

    HCURSOR PAS_STDCALL LoadCursor(HINST hInstance, std::uint8_t* lpCursorName);

    HICON PAS_STDCALL LoadIcon(HINST hInstance, std::uint8_t* lpIconName);

    BOOL PAS_STDCALL DestroyIcon(HICON hIcon);

    HICON PAS_STDCALL CreateIconIndirect(TIconInfo& piconinfo);

    std::int32_t PAS_STDCALL CompareString(LCID Locale, std::uint32_t dwCmpFlags, std::uint8_t* lpString1, std::int32_t cchCount1, std::uint8_t* lpString2, std::int32_t cchCount2);

    std::int32_t PAS_STDCALL CompareStringA(LCID Locale, std::uint32_t dwCmpFlags, std::uint8_t* lpString1, std::int32_t cchCount1, std::uint8_t* lpString2, std::int32_t cchCount2);

    std::int32_t PAS_STDCALL RegCloseKey(HKEY hKey);

    std::int32_t PAS_STDCALL RegCreateKeyExW(HKEY hKey, char16_t* lpSubKey, std::uint32_t Reserved, char16_t* lpClass, std::uint32_t dwOptions, REGSAM samDesired, PSecurityAttributes lpSecurityAttributes, HKEY& phkResult, PDWORD lpdwDisposition);

    std::int32_t PAS_STDCALL RegFlushKey(HKEY hKey);

    std::int32_t PAS_STDCALL RegOpenKeyExA(HKEY hKey, std::uint8_t* lpSubKey, std::uint32_t ulOptions, REGSAM samDesired, HKEY& phkResult);

    std::int32_t PAS_STDCALL RegQueryValueExA(HKEY hKey, std::uint8_t* lpValueName, void* lpReserved, PDWORD lpType, std::uint8_t* lpData, PDWORD lpcbData);

    std::int32_t PAS_STDCALL RegSetValueExW(HKEY hKey, char16_t* lpValueName, std::uint32_t Reserved, std::uint32_t dwType, void* lpData, std::uint32_t cbData);

} // namespace WindowsSdk

#include "inline/WindowsSdk.hpp"
