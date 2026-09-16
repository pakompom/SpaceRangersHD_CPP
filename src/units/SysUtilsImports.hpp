#pragma once
#include "types/SysUtilsImports.hpp"
#include "types/System.hpp"

namespace SysUtils {
    struct TSearchRec;

} // namespace SysUtils

namespace Windows {
    struct TSystemTime;

} // namespace Windows

namespace SysUtilsImports {
    extern std::uint8_t DateSeparator;

    extern std::uint8_t TimeSeparator;

    extern pas::AnsiString ShortDateFormat;

    extern pas::AnsiString LongTimeFormat;

    extern pas::AnsiString TimeAMString;

    extern pas::AnsiString TimePMString;

    void Abort();

    std::int32_t FindFirst(const pas::AnsiString& Path, std::int32_t Attr, SysUtils::TSearchRec& F);

    std::int32_t FindNext(SysUtils::TSearchRec& F);

    void FindClose(SysUtils::TSearchRec& F);

    std::uint8_t DeleteFile(const pas::AnsiString& FileName);

    pas::AnsiString GetCurrentDir();

    std::uint8_t SetCurrentDir(const pas::AnsiString& Dir);

    std::uint8_t CreateDir(const pas::AnsiString& Dir);

    pas::AnsiString AnsiLowerCase(const pas::AnsiString& Text);

    pas::WideString WideLowerCase(const pas::WideString& S);

    std::int32_t FileOpen(const pas::AnsiString& FileName, std::uint32_t Mode);

    std::uint8_t FileExists(const pas::AnsiString& FileName);

    std::uint8_t DirectoryExists(const pas::AnsiString& Directory);

    pas::AnsiString LowerCase(const pas::AnsiString& Text);

    pas::AnsiString UpperCase(const pas::AnsiString& Text);

    pas::AnsiString Trim(const pas::AnsiString& S);

    std::uint32_t StrLen(std::uint8_t* Str);

    std::int32_t StrComp(std::uint8_t* Left, std::uint8_t* Right);

    std::int32_t AnsiStrComp(std::uint8_t* Left, std::uint8_t* Right);

    pas::AnsiString StrPas(std::uint8_t* Str);

    std::uint8_t* StrAlloc(std::uint32_t Size);

    void StrDispose(std::uint8_t* Str);

    std::uint8_t IsLeapYear(std::uint16_t Year);

    System::TDateTime EncodeDate(std::uint16_t Year, std::uint16_t Month, std::uint16_t Day);

    System::TDateTime EncodeTime(std::uint16_t Hour, std::uint16_t Min, std::uint16_t Sec, std::uint16_t MSec);

    pas::AnsiString FloatToStrF(pas::Extended Value, TFloatFormat Format, std::int32_t Precision, std::int32_t Digits);

    inline pas::AnsiString FixedFloatText(pas::Extended Value, std::int32_t Precision, std::int32_t Digits, std::uint8_t Separator);

    System::TDateTime Now();

    void DecodeDate(System::TDateTime DateTime, std::uint16_t& Year, std::uint16_t& Month, std::uint16_t& Day);

    void DecodeTime(System::TDateTime DateTime, std::uint16_t& Hour, std::uint16_t& Min, std::uint16_t& Sec, std::uint16_t& MSec);

    System::TDateTime SystemTimeToDateTime(const Windows::TSystemTime& SystemTime);

    pas::AnsiString FormatDateTime(const pas::AnsiString& Format, System::TDateTime DateTime);

    pas::AnsiString DateTimeToStr(System::TDateTime DateTime);

    void DateTimeToString(pas::AnsiString& Result, const pas::AnsiString& Format, System::TDateTime DateTime);

    pas::AnsiString SysErrorMessage(std::int32_t ErrorCode);

    void PAS_STDCALL Sleep(std::uint32_t Milliseconds);

    std::int32_t FindMatchingFile(SysUtils::TSearchRec& F);

    void DateDayAndTime(System::TDateTime DateTime, std::int32_t& Date, std::int32_t& Time);

} // namespace SysUtilsImports

#include "inline/SysUtilsImports.hpp"
