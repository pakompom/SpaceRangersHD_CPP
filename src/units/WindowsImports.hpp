#pragma once
#include "types/System.hpp"
#include "types/WindowsImports.hpp"
#include "types/Windows_group.hpp"

namespace WindowsImports {
    std::uint32_t PAS_STDCALL CreateFileA(std::uint8_t* FileName, std::uint32_t DesiredAccess, std::uint32_t ShareMode, void* SecurityAttributes, std::uint32_t CreationDisposition, std::uint32_t FlagsAndAttributes, std::uint32_t TemplateFile);

    std::uint32_t PAS_STDCALL CreateFileW(char16_t* FileName, std::uint32_t DesiredAccess, std::uint32_t ShareMode, void* SecurityAttributes, std::uint32_t CreationDisposition, std::uint32_t FlagsAndAttributes, std::uint32_t TemplateFile);

    std::int32_t PAS_STDCALL ReadFile(std::uint32_t Handle, void* Buffer, std::uint32_t Count, std::uint32_t& BytesRead, void* Overlapped);

    std::int32_t PAS_STDCALL WriteFile(std::uint32_t Handle, const void* Buffer, std::uint32_t Count, std::uint32_t& BytesWritten, void* Overlapped);

    std::uint32_t PAS_STDCALL SetFilePointer(std::uint32_t Handle, std::int32_t Distance, WindowsSdk::PInteger DistanceHigh, std::uint32_t Origin);

    std::int32_t PAS_STDCALL CloseHandle(std::uint32_t Handle);

    std::uint32_t PAS_STDCALL GetLastError();

    std::uint32_t PAS_STDCALL GetFileAttributesA(std::uint8_t* FileName);

    std::uint32_t PAS_STDCALL FindFirstFileA(std::uint8_t* FileName, Windows::TWin32FindDataA& Data);

    std::int32_t PAS_STDCALL FindClose(std::uint32_t Handle);

    std::int32_t PAS_STDCALL DeleteFileA(std::uint8_t* FileName);

    std::int32_t PAS_STDCALL QueryPerformanceCounter(std::int64_t& Counter);

    std::uint32_t PAS_STDCALL GetTickCount();

    std::uint32_t PAS_STDCALL GetCurrentThreadId();

    std::uint32_t PAS_STDCALL GetModuleFileNameA(std::uint32_t Module, std::uint8_t* FileName, std::uint32_t Capacity);

    std::uint32_t PAS_STDCALL GetActiveWindow();

    std::uint32_t PAS_STDCALL OpenEvent(std::uint32_t DesiredAccess, std::int32_t InheritHandle, std::uint8_t* Name);

    std::uint32_t PAS_STDCALL CreateEvent(void* Attributes, std::int32_t ManualReset, std::int32_t InitialState, std::uint8_t* Name);

    std::int32_t PAS_STDCALL MessageBox(std::uint32_t Window, std::uint8_t* Text, std::uint8_t* Caption, std::uint32_t Flags);

    std::uint8_t* PAS_STDCALL GetCommandLineA();

    std::uint8_t* PAS_STDCALL CharNext(std::uint8_t* P);

    std::uint32_t PAS_STDCALL LoadLibrary(std::uint8_t* FileName);

    void* PAS_STDCALL GetProcAddress(std::uint32_t Module, std::uint8_t* Name);

    std::int32_t PAS_STDCALL FreeLibrary(std::uint32_t Module);

    void PAS_STDCALL GetSystemTime(Windows::TSystemTime& Time);

    std::int32_t PAS_STDCALL TerminateThread(std::uint32_t Thread, std::uint32_t ExitCode);

    std::int32_t PAS_STDCALL FindNextFileA(std::uint32_t Handle, Windows::TWin32FindDataA& Data);

    std::int32_t PAS_STDCALL FileTimeToLocalFileTime(const Windows::TFileTime& FileTime, Windows::TFileTime& LocalTime);

    std::int32_t PAS_STDCALL FileTimeToDosDateTime(const Windows::TFileTime& FileTime, std::uint16_t& DatePart, std::uint16_t& TimePart);

    std::uint32_t PAS_STDCALL GetCurrentDirectoryA(std::uint32_t Capacity, std::uint8_t* Buffer);

    std::int32_t PAS_STDCALL SetCurrentDirectoryA(std::uint8_t* Directory);

    std::uint32_t PAS_STDCALL CharLowerBuffA(std::uint8_t* Buffer, std::uint32_t Length);

    void ZeroMemory(void* Destination, std::uint32_t Length);

    std::uint32_t CreateWindowExW(std::uint32_t ExStyle, char16_t* ClassName, char16_t* WindowName, std::uint32_t Style, std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::uint32_t Parent, std::uint32_t Menu, std::uint32_t Instance, void* Parameter);

    std::uint32_t PAS_STDCALL RawCreateWindowExW(std::uint32_t ExStyle, char16_t* ClassName, char16_t* WindowName, std::uint32_t Style, std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::uint32_t Parent, std::uint32_t Menu, std::uint32_t Instance, void* Parameter);

} // namespace WindowsImports
