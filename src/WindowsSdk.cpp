#include "layout/WindowsSdk.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "units/WindowsSdk.hpp"

namespace WindowsSdk {
    const WindowsSdk::MakeIntResource IDC_ARROW = reinterpret_cast<WindowsSdk::MakeIntResource>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(32512)));

    const pas::WideString advapi32 = u"advapi32.dll"_w;

    const pas::WideString kernel32 = u"kernel32.dll"_w;

    const pas::WideString gdi32 = u"gdi32.dll"_w;

    const pas::WideString user32 = u"user32.dll"_w;

    std::uint32_t PAS_STDCALL GetVersion() {
        using CppImport = std::uint32_t (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetVersion");
        return cpp_import();
    }

    HGLOBAL PAS_STDCALL GlobalAlloc(UINT uFlags, std::uint32_t dwBytes) {
        using CppImport = HGLOBAL (PAS_STDCALL *)(UINT, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GlobalAlloc");
        return cpp_import(uFlags, dwBytes);
    }

    void* PAS_STDCALL GlobalLock(HGLOBAL hMem) {
        using CppImport = void* (PAS_STDCALL *)(HGLOBAL);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GlobalLock");
        return cpp_import(hMem);
    }

    BOOL PAS_STDCALL GlobalUnlock(HGLOBAL hMem) {
        using CppImport = BOOL (PAS_STDCALL *)(HGLOBAL);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GlobalUnlock");
        return cpp_import(hMem);
    }

    void PAS_STDCALL GlobalMemoryStatus(TMemoryStatus& lpBuffer) {
        using CppImport = void (PAS_STDCALL *)(TMemoryStatus*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GlobalMemoryStatus");
        return cpp_import(&lpBuffer);
    }

    THandle PAS_STDCALL HeapCreate(std::uint32_t flOptions, std::uint32_t dwInitialSize, std::uint32_t dwMaximumSize) {
        using CppImport = THandle (PAS_STDCALL *)(std::uint32_t, std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "HeapCreate");
        return cpp_import(flOptions, dwInitialSize, dwMaximumSize);
    }

    BOOL PAS_STDCALL HeapDestroy(THandle hHeap) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "HeapDestroy");
        return cpp_import(hHeap);
    }

    BOOL PAS_STDCALL SetProcessAffinityMask(THandle hProcess, std::uint32_t dwProcessAffinityMask) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetProcessAffinityMask");
        return cpp_import(hProcess, dwProcessAffinityMask);
    }

    THandle PAS_STDCALL GetCurrentProcess() {
        using CppImport = THandle (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetCurrentProcess");
        return cpp_import();
    }

    THandle PAS_STDCALL CreateThread(void* lpThreadAttributes, std::uint32_t dwStackSize, TFNThreadStartRoutine lpStartAddress, void* lpParameter, std::uint32_t dwCreationFlags, std::uint32_t& lpThreadId) {
        using CppImport = THandle (PAS_STDCALL *)(void*, std::uint32_t, TFNThreadStartRoutine, void*, std::uint32_t, std::uint32_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CreateThread");
        return cpp_import(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, &lpThreadId);
    }

    THandle PAS_STDCALL GetCurrentThread() {
        using CppImport = THandle (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetCurrentThread");
        return cpp_import();
    }

    BOOL PAS_STDCALL SetThreadPriority(THandle hThread, std::int32_t nPriority) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetThreadPriority");
        return cpp_import(hThread, nPriority);
    }

    std::int32_t PAS_STDCALL GetThreadPriority(THandle hThread) {
        using CppImport = std::int32_t (PAS_STDCALL *)(THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetThreadPriority");
        return cpp_import(hThread);
    }

    std::uint32_t PAS_STDCALL ResumeThread(THandle hThread) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "ResumeThread");
        return cpp_import(hThread);
    }

    BOOL PAS_STDCALL SetEvent(THandle hEvent) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetEvent");
        return cpp_import(hEvent);
    }

    BOOL PAS_STDCALL ResetEvent(THandle hEvent) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "ResetEvent");
        return cpp_import(hEvent);
    }

    std::uint32_t PAS_STDCALL WaitForSingleObject(THandle hHandle, std::uint32_t dwMilliseconds) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(THandle, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "WaitForSingleObject");
        return cpp_import(hHandle, dwMilliseconds);
    }

    std::uint32_t PAS_STDCALL WaitForMultipleObjects(std::uint32_t nCount, PWOHandleArray lpHandles, BOOL bWaitAll, std::uint32_t dwMilliseconds) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, PWOHandleArray, BOOL, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "WaitForMultipleObjects");
        return cpp_import(nCount, lpHandles, bWaitAll, dwMilliseconds);
    }

    std::uint32_t PAS_STDCALL GetFileSize(THandle hFile, void* lpFileSizeHigh) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(THandle, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetFileSize");
        return cpp_import(hFile, lpFileSizeHigh);
    }

    void PAS_STDCALL GetLocalTime(Windows::TSystemTime& lpSystemTime) {
        using CppImport = void (PAS_STDCALL *)(Windows::TSystemTime*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetLocalTime");
        return cpp_import(&lpSystemTime);
    }

    void PAS_STDCALL GetSystemInfo(TSystemInfo& lpSystemInfo) {
        using CppImport = void (PAS_STDCALL *)(TSystemInfo*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetSystemInfo");
        return cpp_import(&lpSystemInfo);
    }

    BOOL PAS_STDCALL FileTimeToSystemTime(pas::ConstRef<Windows::TFileTime> lpFileTime, Windows::TSystemTime& lpSystemTime) {
        using CppImport = BOOL (PAS_STDCALL *)(const Windows::TFileTime*, Windows::TSystemTime*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FileTimeToSystemTime");
        return cpp_import(static_cast<const Windows::TFileTime*>(lpFileTime.address), &lpSystemTime);
    }

    std::int32_t PAS_STDCALL CompareFileTime(pas::ConstRef<Windows::TFileTime> lpFileTime1, const Windows::TFileTime& lpFileTime2) {
        using CppImport = std::int32_t (PAS_STDCALL *)(const Windows::TFileTime*, const Windows::TFileTime*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CompareFileTime");
        return cpp_import(static_cast<const Windows::TFileTime*>(lpFileTime1.address), &lpFileTime2);
    }

    std::uint32_t PAS_STDCALL FormatMessageA(std::uint32_t dwFlags, void* lpSource, std::uint32_t dwMessageId, std::uint32_t dwLanguageId, std::uint8_t* lpBuffer, std::uint32_t nSize, void* Arguments) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, void*, std::uint32_t, std::uint32_t, std::uint8_t*, std::uint32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FormatMessageA");
        return cpp_import(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);
    }

    HMODULE PAS_STDCALL LoadLibraryW(char16_t* lpLibFileName) {
        using CppImport = HMODULE (PAS_STDCALL *)(char16_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "LoadLibraryW");
        return cpp_import(lpLibFileName);
    }

    HMODULE PAS_STDCALL GetModuleHandle(std::uint8_t* lpModuleName) {
        using CppImport = HMODULE (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetModuleHandleA");
        return cpp_import(lpModuleName);
    }

    HMODULE PAS_STDCALL GetModuleHandleW(char16_t* lpModuleName) {
        using CppImport = HMODULE (PAS_STDCALL *)(char16_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetModuleHandleW");
        return cpp_import(lpModuleName);
    }

    UINT PAS_STDCALL GetSystemDirectoryW(char16_t* lpBuffer, UINT uSize) {
        using CppImport = UINT (PAS_STDCALL *)(char16_t*, UINT);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetSystemDirectoryW");
        return cpp_import(lpBuffer, uSize);
    }

    BOOL PAS_STDCALL CreateDirectoryA(std::uint8_t* lpPathName, PSecurityAttributes lpSecurityAttributes) {
        using CppImport = BOOL (PAS_STDCALL *)(std::uint8_t*, PSecurityAttributes);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CreateDirectoryA");
        return cpp_import(lpPathName, lpSecurityAttributes);
    }

    BOOL PAS_STDCALL SetFileAttributesA(std::uint8_t* lpFileName, std::uint32_t dwFileAttributes) {
        using CppImport = BOOL (PAS_STDCALL *)(std::uint8_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetFileAttributesA");
        return cpp_import(lpFileName, dwFileAttributes);
    }

    BOOL PAS_STDCALL DeleteFile(std::uint8_t* lpFileName) {
        using CppImport = BOOL (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "DeleteFileA");
        return cpp_import(lpFileName);
    }

    THandle PAS_STDCALL FindFirstFile(std::uint8_t* lpFileName, TWin32FindData& lpFindFileData) {
        using CppImport = THandle (PAS_STDCALL *)(std::uint8_t*, TWin32FindData*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindFirstFileA");
        return cpp_import(lpFileName, &lpFindFileData);
    }

    THandle PAS_STDCALL FindFirstFileW(char16_t* lpFileName, TWin32FindDataW& lpFindFileData) {
        using CppImport = THandle (PAS_STDCALL *)(char16_t*, TWin32FindDataW*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindFirstFileW");
        return cpp_import(lpFileName, &lpFindFileData);
    }

    BOOL PAS_STDCALL FindNextFile(THandle hFindFile, TWin32FindData& lpFindFileData) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle, TWin32FindData*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindNextFileA");
        return cpp_import(hFindFile, &lpFindFileData);
    }

    BOOL PAS_STDCALL FindNextFileW(THandle hFindFile, TWin32FindDataW& lpFindFileData) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle, TWin32FindDataW*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindNextFileW");
        return cpp_import(hFindFile, &lpFindFileData);
    }

    BOOL PAS_STDCALL CopyFile(std::uint8_t* lpExistingFileName, std::uint8_t* lpNewFileName, BOOL bFailIfExists) {
        using CppImport = BOOL (PAS_STDCALL *)(std::uint8_t*, std::uint8_t*, BOOL);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CopyFileA");
        return cpp_import(lpExistingFileName, lpNewFileName, bFailIfExists);
    }

    BOOL PAS_STDCALL CopyFileW(char16_t* lpExistingFileName, char16_t* lpNewFileName, BOOL bFailIfExists) {
        using CppImport = BOOL (PAS_STDCALL *)(char16_t*, char16_t*, BOOL);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CopyFileW");
        return cpp_import(lpExistingFileName, lpNewFileName, bFailIfExists);
    }

    BOOL PAS_STDCALL MoveFileW(char16_t* lpExistingFileName, char16_t* lpNewFileName) {
        using CppImport = BOOL (PAS_STDCALL *)(char16_t*, char16_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "MoveFileW");
        return cpp_import(lpExistingFileName, lpNewFileName);
    }

    BOOL PAS_STDCALL GetVolumeInformationA(std::uint8_t* lpRootPathName, std::uint8_t* lpVolumeNameBuffer, std::uint32_t nVolumeNameSize, PDWORD lpVolumeSerialNumber, std::uint32_t& lpMaximumComponentLength, std::uint32_t& lpFileSystemFlags, std::uint8_t* lpFileSystemNameBuffer, std::uint32_t nFileSystemNameSize) {
        using CppImport = BOOL (PAS_STDCALL *)(std::uint8_t*, std::uint8_t*, std::uint32_t, PDWORD, std::uint32_t*, std::uint32_t*, std::uint8_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetVolumeInformationA");
        return cpp_import(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, &lpMaximumComponentLength, &lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
    }

    BOOL PAS_STDCALL SetPriorityClass(THandle hProcess, std::uint32_t dwPriorityClass) {
        using CppImport = BOOL (PAS_STDCALL *)(THandle, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetPriorityClass");
        return cpp_import(hProcess, dwPriorityClass);
    }

    std::uint32_t PAS_STDCALL GetPriorityClass(THandle hProcess) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetPriorityClass");
        return cpp_import(hProcess);
    }

    BOOL PAS_STDCALL QueryPerformanceFrequency(Windows::TLargeInteger& lpFrequency) {
        using CppImport = BOOL (PAS_STDCALL *)(Windows::TLargeInteger*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "QueryPerformanceFrequency");
        return cpp_import(&lpFrequency);
    }

    BOOL PAS_STDCALL GetVersionEx(TOSVersionInfo& lpVersionInformation) {
        using CppImport = BOOL (PAS_STDCALL *)(TOSVersionInfo*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetVersionExA");
        return cpp_import(&lpVersionInformation);
    }

    BOOL PAS_STDCALL DeleteObject(HGDIOBJ p1) {
        using CppImport = BOOL (PAS_STDCALL *)(HGDIOBJ);
        static const auto cpp_import = pas::win::load_import<CppImport>("gdi32.dll", "DeleteObject");
        return cpp_import(p1);
    }

    HGDIOBJ PAS_STDCALL GetStockObject(std::int32_t Index) {
        using CppImport = HGDIOBJ (PAS_STDCALL *)(std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("gdi32.dll", "GetStockObject");
        return cpp_import(Index);
    }

    UINT PAS_STDCALL RegisterWindowMessage(std::uint8_t* lpString) {
        using CppImport = UINT (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "RegisterWindowMessageA");
        return cpp_import(lpString);
    }

    BOOL PAS_STDCALL TrackMouseEvent(Windows::TTrackMouseEvent& EventTrack) {
        using CppImport = BOOL (PAS_STDCALL *)(Windows::TTrackMouseEvent*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "TrackMouseEvent");
        return cpp_import(&EventTrack);
    }

    std::int32_t PAS_STDCALL DispatchMessageW(const Windows::TMsg& lpMsg) {
        using CppImport = std::int32_t (PAS_STDCALL *)(const Windows::TMsg*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "DispatchMessageW");
        return cpp_import(&lpMsg);
    }

    BOOL PAS_STDCALL TranslateMessage(const Windows::TMsg& lpMsg) {
        using CppImport = BOOL (PAS_STDCALL *)(const Windows::TMsg*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "TranslateMessage");
        return cpp_import(&lpMsg);
    }

    BOOL PAS_STDCALL PeekMessage(Windows::TMsg& lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
        using CppImport = BOOL (PAS_STDCALL *)(Windows::TMsg*, HWND, UINT, UINT, UINT);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "PeekMessageA");
        return cpp_import(&lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    }

    BOOL PAS_STDCALL PeekMessageW(Windows::TMsg& lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
        using CppImport = BOOL (PAS_STDCALL *)(Windows::TMsg*, HWND, UINT, UINT, UINT);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "PeekMessageW");
        return cpp_import(&lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    }

    BOOL PAS_STDCALL PostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, UINT, WPARAM, LPARAM);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "PostMessageA");
        return cpp_import(hWnd, Msg, wParam, lParam);
    }

    LRESULT PAS_STDCALL DefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        using CppImport = LRESULT (PAS_STDCALL *)(HWND, UINT, WPARAM, LPARAM);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "DefWindowProcW");
        return cpp_import(hWnd, Msg, wParam, lParam);
    }

    void PAS_STDCALL PostQuitMessage(std::int32_t nExitCode) {
        using CppImport = void (PAS_STDCALL *)(std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "PostQuitMessage");
        return cpp_import(nExitCode);
    }

    UINT PAS_STDCALL GetDoubleClickTime() {
        using CppImport = UINT (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetDoubleClickTime");
        return cpp_import();
    }

    ATOM PAS_STDCALL RegisterClassW(const TWndClassW& lpWndClass) {
        using CppImport = ATOM (PAS_STDCALL *)(const TWndClassW*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "RegisterClassW");
        return cpp_import(&lpWndClass);
    }

    BOOL PAS_STDCALL DestroyWindow(HWND hWnd) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "DestroyWindow");
        return cpp_import(hWnd);
    }

    BOOL PAS_STDCALL ShowWindow(HWND hWnd, std::int32_t nCmdShow) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "ShowWindow");
        return cpp_import(hWnd, nCmdShow);
    }

    BOOL PAS_STDCALL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, std::int32_t X, std::int32_t Y, std::int32_t cx, std::int32_t cy, UINT uFlags) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, HWND, std::int32_t, std::int32_t, std::int32_t, std::int32_t, UINT);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetWindowPos");
        return cpp_import(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    BOOL PAS_STDCALL OpenClipboard(HWND hWndNewOwner) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "OpenClipboard");
        return cpp_import(hWndNewOwner);
    }

    BOOL PAS_STDCALL CloseClipboard() {
        using CppImport = BOOL (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CloseClipboard");
        return cpp_import();
    }

    THandle PAS_STDCALL SetClipboardData(UINT uFormat, THandle hMem) {
        using CppImport = THandle (PAS_STDCALL *)(UINT, THandle);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetClipboardData");
        return cpp_import(uFormat, hMem);
    }

    THandle PAS_STDCALL GetClipboardData(UINT uFormat) {
        using CppImport = THandle (PAS_STDCALL *)(UINT);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetClipboardData");
        return cpp_import(uFormat);
    }

    BOOL PAS_STDCALL EmptyClipboard() {
        using CppImport = BOOL (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "EmptyClipboard");
        return cpp_import();
    }

    std::uint32_t PAS_STDCALL CharUpperBuffW(char16_t* lpsz, std::uint32_t cchLength) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(char16_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CharUpperBuffW");
        return cpp_import(lpsz, cchLength);
    }

    std::uint32_t PAS_STDCALL CharLowerBuffW(char16_t* lpsz, std::uint32_t cchLength) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(char16_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CharLowerBuffW");
        return cpp_import(lpsz, cchLength);
    }

    HWND PAS_STDCALL SetFocus(HWND hWnd) {
        using CppImport = HWND (PAS_STDCALL *)(HWND);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetFocus");
        return cpp_import(hWnd);
    }

    SHORT PAS_STDCALL GetAsyncKeyState(std::int32_t vKey) {
        using CppImport = SHORT (PAS_STDCALL *)(std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetAsyncKeyState");
        return cpp_import(vKey);
    }

    std::uint32_t PAS_STDCALL MsgWaitForMultipleObjects(std::uint32_t nCount, void* pHandles, BOOL fWaitAll, std::uint32_t dwMilliseconds, std::uint32_t dwWakeMask) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, void*, BOOL, std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "MsgWaitForMultipleObjects");
        return cpp_import(nCount, pHandles, fWaitAll, dwMilliseconds, dwWakeMask);
    }

    UINT PAS_STDCALL SetTimer(HWND hWnd, UINT nIDEvent, UINT uElapse, TFNTimerProc lpTimerFunc) {
        using CppImport = UINT (PAS_STDCALL *)(HWND, UINT, UINT, TFNTimerProc);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetTimer");
        return cpp_import(hWnd, nIDEvent, uElapse, lpTimerFunc);
    }

    BOOL PAS_STDCALL UpdateWindow(HWND hWnd) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "UpdateWindow");
        return cpp_import(hWnd);
    }

    HWND PAS_STDCALL GetForegroundWindow() {
        using CppImport = HWND (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetForegroundWindow");
        return cpp_import();
    }

    BOOL PAS_STDCALL SetForegroundWindow(HWND hWnd) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetForegroundWindow");
        return cpp_import(hWnd);
    }

    HDC PAS_STDCALL GetDC(HWND hWnd) {
        using CppImport = HDC (PAS_STDCALL *)(HWND);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetDC");
        return cpp_import(hWnd);
    }

    std::int32_t PAS_STDCALL ReleaseDC(HWND hWnd, HDC hDC) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HWND, HDC);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "ReleaseDC");
        return cpp_import(hWnd, hDC);
    }

    BOOL PAS_STDCALL RedrawWindow(HWND hWnd, PRect lprcUpdate, HRGN hrgnUpdate, UINT flags) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, PRect, HRGN, UINT);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "RedrawWindow");
        return cpp_import(hWnd, lprcUpdate, hrgnUpdate, flags);
    }

    BOOL PAS_STDCALL SetWindowTextA(HWND hWnd, std::uint8_t* lpString) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetWindowTextA");
        return cpp_import(hWnd, lpString);
    }

    BOOL PAS_STDCALL AdjustWindowRect(TRect& lpRect, std::uint32_t dwStyle, BOOL bMenu) {
        using CppImport = BOOL (PAS_STDCALL *)(TRect*, std::uint32_t, BOOL);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "AdjustWindowRect");
        return cpp_import(&lpRect, dwStyle, bMenu);
    }

    std::int32_t PAS_STDCALL ShowCursor(BOOL bShow) {
        using CppImport = std::int32_t (PAS_STDCALL *)(BOOL);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "ShowCursor");
        return cpp_import(bShow);
    }

    BOOL PAS_STDCALL SetCursorPos(std::int32_t X, std::int32_t Y) {
        using CppImport = BOOL (PAS_STDCALL *)(std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetCursorPos");
        return cpp_import(X, Y);
    }

    HCURSOR PAS_STDCALL SetCursor(HICON hCursor) {
        using CppImport = HCURSOR (PAS_STDCALL *)(HICON);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetCursor");
        return cpp_import(hCursor);
    }

    BOOL PAS_STDCALL GetCursorPos(TPoint& lpPoint) {
        using CppImport = BOOL (PAS_STDCALL *)(TPoint*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetCursorPos");
        return cpp_import(&lpPoint);
    }

    BOOL PAS_STDCALL ClipCursor(PRect lpRect) {
        using CppImport = BOOL (PAS_STDCALL *)(PRect);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "ClipCursor");
        return cpp_import(lpRect);
    }

    BOOL PAS_STDCALL ClientToScreen(HWND hWnd, TPoint& lpPoint) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, TPoint*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "ClientToScreen");
        return cpp_import(hWnd, &lpPoint);
    }

    BOOL PAS_STDCALL ScreenToClient(HWND hWnd, TPoint& lpPoint) {
        using CppImport = BOOL (PAS_STDCALL *)(HWND, TPoint*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "ScreenToClient");
        return cpp_import(hWnd, &lpPoint);
    }

    BOOL PAS_STDCALL IntersectRect(TRect& lprcDst, const TRect& lprcSrc1, const TRect& lprcSrc2) {
        using CppImport = BOOL (PAS_STDCALL *)(TRect*, const TRect*, const TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "IntersectRect");
        return cpp_import(&lprcDst, &lprcSrc1, &lprcSrc2);
    }

    BOOL PAS_STDCALL UnionRect(TRect& lprcDst, const TRect& lprcSrc1, const TRect& lprcSrc2) {
        using CppImport = BOOL (PAS_STDCALL *)(TRect*, const TRect*, const TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "UnionRect");
        return cpp_import(&lprcDst, &lprcSrc1, &lprcSrc2);
    }

    std::int32_t PAS_STDCALL SetWindowLong(HWND hWnd, std::int32_t nIndex, std::int32_t dwNewLong) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HWND, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "SetWindowLongA");
        return cpp_import(hWnd, nIndex, dwNewLong);
    }

    HCURSOR PAS_STDCALL LoadCursor(HINST hInstance, std::uint8_t* lpCursorName) {
        using CppImport = HCURSOR (PAS_STDCALL *)(HINST, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "LoadCursorA");
        return cpp_import(hInstance, lpCursorName);
    }

    HICON PAS_STDCALL LoadIcon(HINST hInstance, std::uint8_t* lpIconName) {
        using CppImport = HICON (PAS_STDCALL *)(HINST, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "LoadIconA");
        return cpp_import(hInstance, lpIconName);
    }

    BOOL PAS_STDCALL DestroyIcon(HICON hIcon) {
        using CppImport = BOOL (PAS_STDCALL *)(HICON);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "DestroyIcon");
        return cpp_import(hIcon);
    }

    HICON PAS_STDCALL CreateIconIndirect(TIconInfo& piconinfo) {
        using CppImport = HICON (PAS_STDCALL *)(TIconInfo*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CreateIconIndirect");
        return cpp_import(&piconinfo);
    }

    std::int32_t PAS_STDCALL CompareString(LCID Locale, std::uint32_t dwCmpFlags, std::uint8_t* lpString1, std::int32_t cchCount1, std::uint8_t* lpString2, std::int32_t cchCount2) {
        using CppImport = std::int32_t (PAS_STDCALL *)(LCID, std::uint32_t, std::uint8_t*, std::int32_t, std::uint8_t*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CompareStringA");
        return cpp_import(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
    }

    std::int32_t PAS_STDCALL CompareStringA(LCID Locale, std::uint32_t dwCmpFlags, std::uint8_t* lpString1, std::int32_t cchCount1, std::uint8_t* lpString2, std::int32_t cchCount2) {
        using CppImport = std::int32_t (PAS_STDCALL *)(LCID, std::uint32_t, std::uint8_t*, std::int32_t, std::uint8_t*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CompareStringA");
        return cpp_import(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
    }

    std::int32_t PAS_STDCALL RegCloseKey(HKEY hKey) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HKEY);
        static const auto cpp_import = pas::win::load_import<CppImport>("advapi32.dll", "RegCloseKey");
        return cpp_import(hKey);
    }

    std::int32_t PAS_STDCALL RegCreateKeyExW(HKEY hKey, char16_t* lpSubKey, std::uint32_t Reserved, char16_t* lpClass, std::uint32_t dwOptions, REGSAM samDesired, PSecurityAttributes lpSecurityAttributes, HKEY& phkResult, PDWORD lpdwDisposition) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HKEY, char16_t*, std::uint32_t, char16_t*, std::uint32_t, REGSAM, PSecurityAttributes, HKEY*, PDWORD);
        static const auto cpp_import = pas::win::load_import<CppImport>("advapi32.dll", "RegCreateKeyExW");
        return cpp_import(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, &phkResult, lpdwDisposition);
    }

    std::int32_t PAS_STDCALL RegFlushKey(HKEY hKey) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HKEY);
        static const auto cpp_import = pas::win::load_import<CppImport>("advapi32.dll", "RegFlushKey");
        return cpp_import(hKey);
    }

    std::int32_t PAS_STDCALL RegOpenKeyExA(HKEY hKey, std::uint8_t* lpSubKey, std::uint32_t ulOptions, REGSAM samDesired, HKEY& phkResult) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HKEY, std::uint8_t*, std::uint32_t, REGSAM, HKEY*);
        static const auto cpp_import = pas::win::load_import<CppImport>("advapi32.dll", "RegOpenKeyExA");
        return cpp_import(hKey, lpSubKey, ulOptions, samDesired, &phkResult);
    }

    std::int32_t PAS_STDCALL RegQueryValueExA(HKEY hKey, std::uint8_t* lpValueName, void* lpReserved, PDWORD lpType, std::uint8_t* lpData, PDWORD lpcbData) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HKEY, std::uint8_t*, void*, PDWORD, std::uint8_t*, PDWORD);
        static const auto cpp_import = pas::win::load_import<CppImport>("advapi32.dll", "RegQueryValueExA");
        return cpp_import(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    }

    std::int32_t PAS_STDCALL RegSetValueExW(HKEY hKey, char16_t* lpValueName, std::uint32_t Reserved, std::uint32_t dwType, void* lpData, std::uint32_t cbData) {
        using CppImport = std::int32_t (PAS_STDCALL *)(HKEY, char16_t*, std::uint32_t, std::uint32_t, void*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("advapi32.dll", "RegSetValueExW");
        return cpp_import(hKey, lpValueName, Reserved, dwType, lpData, cbData);
    }

} // namespace WindowsSdk
