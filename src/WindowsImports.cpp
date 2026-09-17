#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"

// Narrow SDK declarations omitted from the recovered Windows unit.
// Parsed by the same frontend; resolved by the typed Win32 DLL bridge.
namespace WindowsImports {
    std::uint32_t PAS_STDCALL CreateFileA(std::uint8_t* FileName, std::uint32_t DesiredAccess, std::uint32_t ShareMode, void* SecurityAttributes, std::uint32_t CreationDisposition, std::uint32_t FlagsAndAttributes, std::uint32_t TemplateFile) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint8_t*, std::uint32_t, std::uint32_t, void*, std::uint32_t, std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CreateFileA");
        return cpp_import(FileName, DesiredAccess, ShareMode, SecurityAttributes, CreationDisposition, FlagsAndAttributes, TemplateFile);
    }

    std::uint32_t PAS_STDCALL CreateFileW(char16_t* FileName, std::uint32_t DesiredAccess, std::uint32_t ShareMode, void* SecurityAttributes, std::uint32_t CreationDisposition, std::uint32_t FlagsAndAttributes, std::uint32_t TemplateFile) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(char16_t*, std::uint32_t, std::uint32_t, void*, std::uint32_t, std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CreateFileW");
        return cpp_import(FileName, DesiredAccess, ShareMode, SecurityAttributes, CreationDisposition, FlagsAndAttributes, TemplateFile);
    }

    std::int32_t PAS_STDCALL ReadFile(std::uint32_t Handle, void* Buffer, std::uint32_t Count, std::uint32_t& BytesRead, void* Overlapped) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t, void*, std::uint32_t, std::uint32_t*, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "ReadFile");
        return cpp_import(Handle, Buffer, Count, &BytesRead, Overlapped);
    }

    std::int32_t PAS_STDCALL WriteFile(std::uint32_t Handle, const void* Buffer, std::uint32_t Count, std::uint32_t& BytesWritten, void* Overlapped) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t, const void*, std::uint32_t, std::uint32_t*, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "WriteFile");
        return cpp_import(Handle, Buffer, Count, &BytesWritten, Overlapped);
    }

    std::uint32_t PAS_STDCALL SetFilePointer(std::uint32_t Handle, std::int32_t Distance, WindowsSdk::PInteger DistanceHigh, std::uint32_t Origin) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, std::int32_t, WindowsSdk::PInteger, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetFilePointer");
        return cpp_import(Handle, Distance, DistanceHigh, Origin);
    }

    std::int32_t PAS_STDCALL CloseHandle(std::uint32_t Handle) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CloseHandle");
        return cpp_import(Handle);
    }

    std::uint32_t PAS_STDCALL GetLastError() {
        using CppImport = std::uint32_t (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetLastError");
        return cpp_import();
    }

    std::uint32_t PAS_STDCALL GetFileAttributesA(std::uint8_t* FileName) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetFileAttributesA");
        return cpp_import(FileName);
    }

    std::uint32_t PAS_STDCALL FindFirstFileA(std::uint8_t* FileName, Windows::TWin32FindDataA& Data) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint8_t*, Windows::TWin32FindDataA*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindFirstFileA");
        return cpp_import(FileName, &Data);
    }

    std::int32_t PAS_STDCALL FindClose(std::uint32_t Handle) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindClose");
        return cpp_import(Handle);
    }

    std::int32_t PAS_STDCALL DeleteFileA(std::uint8_t* FileName) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "DeleteFileA");
        return cpp_import(FileName);
    }

    std::int32_t PAS_STDCALL QueryPerformanceCounter(std::int64_t& Counter) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::int64_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "QueryPerformanceCounter");
        return cpp_import(&Counter);
    }

    std::uint32_t PAS_STDCALL GetTickCount() {
        using CppImport = std::uint32_t (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetTickCount");
        return cpp_import();
    }

    std::uint32_t PAS_STDCALL GetCurrentThreadId() {
        using CppImport = std::uint32_t (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetCurrentThreadId");
        return cpp_import();
    }

    std::uint32_t PAS_STDCALL GetModuleFileNameA(std::uint32_t Module, std::uint8_t* FileName, std::uint32_t Capacity) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, std::uint8_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetModuleFileNameA");
        return cpp_import(Module, FileName, Capacity);
    }

    std::uint32_t PAS_STDCALL GetActiveWindow() {
        using CppImport = std::uint32_t (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "GetActiveWindow");
        return cpp_import();
    }

    std::uint32_t PAS_STDCALL OpenEvent(std::uint32_t DesiredAccess, std::int32_t InheritHandle, std::uint8_t* Name) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, std::int32_t, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "OpenEventA");
        return cpp_import(DesiredAccess, InheritHandle, Name);
    }

    std::uint32_t PAS_STDCALL CreateEvent(void* Attributes, std::int32_t ManualReset, std::int32_t InitialState, std::uint8_t* Name) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(void*, std::int32_t, std::int32_t, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "CreateEventA");
        return cpp_import(Attributes, ManualReset, InitialState, Name);
    }

    std::int32_t PAS_STDCALL MessageBox(std::uint32_t Window, std::uint8_t* Text, std::uint8_t* Caption, std::uint32_t Flags) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t, std::uint8_t*, std::uint8_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "MessageBoxA");
        return cpp_import(Window, Text, Caption, Flags);
    }

    std::uint8_t* PAS_STDCALL GetCommandLineA() {
        using CppImport = std::uint8_t* (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetCommandLineA");
        return cpp_import();
    }

    std::uint8_t* PAS_STDCALL CharNext(std::uint8_t* P) {
        using CppImport = std::uint8_t* (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CharNextA");
        return cpp_import(P);
    }

    std::uint32_t PAS_STDCALL LoadLibrary(std::uint8_t* FileName) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "LoadLibraryA");
        return cpp_import(FileName);
    }

    void* PAS_STDCALL GetProcAddress(std::uint32_t Module, std::uint8_t* Name) {
        using CppImport = void* (PAS_STDCALL *)(std::uint32_t, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetProcAddress");
        return cpp_import(Module, Name);
    }

    std::int32_t PAS_STDCALL FreeLibrary(std::uint32_t Module) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FreeLibrary");
        return cpp_import(Module);
    }

    void PAS_STDCALL GetSystemTime(Windows::TSystemTime& Time) {
        using CppImport = void (PAS_STDCALL *)(Windows::TSystemTime*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetSystemTime");
        return cpp_import(&Time);
    }

    std::int32_t PAS_STDCALL TerminateThread(std::uint32_t Thread, std::uint32_t ExitCode) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "TerminateThread");
        return cpp_import(Thread, ExitCode);
    }

    std::int32_t PAS_STDCALL FindNextFileA(std::uint32_t Handle, Windows::TWin32FindDataA& Data) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t, Windows::TWin32FindDataA*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FindNextFileA");
        return cpp_import(Handle, &Data);
    }

    std::int32_t PAS_STDCALL FileTimeToLocalFileTime(pas::ConstRef<Windows::TFileTime> FileTime, Windows::TFileTime& LocalTime) {
        using CppImport = std::int32_t (PAS_STDCALL *)(const Windows::TFileTime*, Windows::TFileTime*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FileTimeToLocalFileTime");
        return cpp_import(static_cast<const Windows::TFileTime*>(FileTime.address), &LocalTime);
    }

    std::int32_t PAS_STDCALL FileTimeToDosDateTime(const Windows::TFileTime& FileTime, std::uint16_t& DatePart, std::uint16_t& TimePart) {
        using CppImport = std::int32_t (PAS_STDCALL *)(const Windows::TFileTime*, std::uint16_t*, std::uint16_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "FileTimeToDosDateTime");
        return cpp_import(&FileTime, &DatePart, &TimePart);
    }

    std::uint32_t PAS_STDCALL GetCurrentDirectoryA(std::uint32_t Capacity, std::uint8_t* Buffer) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "GetCurrentDirectoryA");
        return cpp_import(Capacity, Buffer);
    }

    std::int32_t PAS_STDCALL SetCurrentDirectoryA(std::uint8_t* Directory) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "SetCurrentDirectoryA");
        return cpp_import(Directory);
    }

    std::uint32_t PAS_STDCALL CharLowerBuffA(std::uint8_t* Buffer, std::uint32_t Length) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint8_t*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CharLowerBuffA");
        return cpp_import(Buffer, Length);
    }

    void ZeroMemory(void* Destination, std::uint32_t Length) {
        Windows::FillMemory(Destination, Length, 0);
    }

    std::uint32_t CreateWindowExW(std::uint32_t ExStyle, char16_t* ClassName, char16_t* WindowName, std::uint32_t Style, std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::uint32_t Parent, std::uint32_t Menu, std::uint32_t Instance, void* Parameter) {
        std::uint16_t ControlWord = System::Get8087CW();
        std::uint32_t Result = WindowsImports::RawCreateWindowExW(ExStyle, ClassName, WindowName, Style, X, Y, Width, Height, Parent, Menu, Instance, Parameter);
        System::Set8087CW(ControlWord);
        return Result;
    }

    std::uint32_t PAS_STDCALL RawCreateWindowExW(std::uint32_t ExStyle, char16_t* ClassName, char16_t* WindowName, std::uint32_t Style, std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::uint32_t Parent, std::uint32_t Menu, std::uint32_t Instance, void* Parameter) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, char16_t*, char16_t*, std::uint32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint32_t, std::uint32_t, std::uint32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("user32.dll", "CreateWindowExW");
        return cpp_import(ExStyle, ClassName, WindowName, Style, X, Y, Width, Height, Parent, Menu, Instance, Parameter);
    }

} // namespace WindowsImports
