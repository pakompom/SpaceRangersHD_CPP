#pragma once
#include "types/ExceptionInfo.hpp"

namespace ExceptionInfo {
    extern const pas::Array<char16_t, 0, 15> ExportHexDigits;

    extern std::uint8_t ReportingException;

    extern ExceptionInfo::TRaiseExceptionCallback PreviousRaiseException;

    pas::AnsiString ExceptionLogTimestamp();

    void ReportUnhandledException(pas::Exception* E, std::uint8_t& Handled);

    void PAS_STDCALL RaiseExceptionWithLogging(std::uint32_t Code, std::uint32_t Flags, std::uint32_t ArgumentCount, void* Arguments);

    char16_t HexDigit(std::uint8_t Value);

    pas::WideString ByteToHexText(std::uint8_t Value);

    // Native initializer saves and replaces the RTL raise hook.
    void UnitInitialize();

} // namespace ExceptionInfo
