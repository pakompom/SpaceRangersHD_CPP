#include "layout/SysUtilsImports.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

// Required Delphi 2007 RTL algorithms, expressed as ordinary Pascal input.
namespace SysUtilsImports {
    std::uint8_t DateSeparator = pas::initial_date_separator();

    std::uint8_t TimeSeparator = pas::initial_time_separator();

    pas::AnsiString ShortDateFormat = pas::initial_short_date_format();

    pas::AnsiString LongTimeFormat = pas::initial_long_time_format();

    pas::AnsiString TimeAMString = pas::initial_time_am_string();

    pas::AnsiString TimePMString = pas::initial_time_pm_string();

    void Abort() {
        pas::raise(pas::make_exception<pas::Abort>("Operation aborted"_a));
    }

    std::int32_t FindFirst(const pas::AnsiString& Path, std::int32_t Attr, SysUtils::TSearchRec& F) {
        std::int32_t Result{};
        F.ExcludeAttr = ~Attr & (faHidden | faSysFile | faDirectory);
        F.FindHandle = WindowsImports::FindFirstFileA(Path.pchar(), F.FindData);
        if (F.FindHandle != WindowsImports::INVALID_HANDLE_VALUE) {
            Result = SysUtilsImports::FindMatchingFile(F);
            if (Result != 0) {
                SysUtilsImports::FindClose(F);
            }
            return Result;
        }
        return WindowsImports::GetLastError();
    }

    std::int32_t FindNext(SysUtils::TSearchRec& F) {
        if (WindowsImports::FindNextFileA(F.FindHandle, F.FindData)) {
            return SysUtilsImports::FindMatchingFile(F);
        }
        return WindowsImports::GetLastError();
    }

    void FindClose(SysUtils::TSearchRec& F) {
        if (F.FindHandle != WindowsImports::INVALID_HANDLE_VALUE) {
            WindowsImports::FindClose(F.FindHandle);
            F.FindHandle = WindowsImports::INVALID_HANDLE_VALUE;
        }
    }

    std::uint8_t DeleteFile(const pas::AnsiString& FileName) {
        return WindowsImports::DeleteFileA(FileName.pchar()) != 0;
    }

    pas::AnsiString GetCurrentDir() {
        pas::Array<std::uint8_t, 0, 260> Buffer{};
        WindowsImports::GetCurrentDirectoryA(261u, &Buffer[0]);
        return static_cast<pas::AnsiString>(&Buffer[0]);
    }

    std::uint8_t SetCurrentDir(const pas::AnsiString& Dir) {
        return WindowsImports::SetCurrentDirectoryA(Dir.pchar()) != 0;
    }

    std::uint8_t CreateDir(const pas::AnsiString& Dir) {
        return WindowsSdk::CreateDirectoryA(Dir.pchar(), nullptr) != 0;
    }

    pas::AnsiString AnsiLowerCase(const pas::AnsiString& Text) {
        pas::AnsiString Result{};
        Result.set_length(Text.length());
        if (Text.length() > 0) {
            pas::move_memory(Text.pchar(), Result.pchar(), Text.length());
            WindowsImports::CharLowerBuffA(Result.pchar(), Result.length());
        }
        return Result;
    }

    pas::WideString WideLowerCase(const pas::WideString& S) {
        pas::WideString Result{};
        // Selected Delphi 2007 Windows NT path; Win9x is outside the game target.
        std::int32_t Len = S.length();
        Result = S;
        if (Len > 0) {
            WindowsSdk::CharLowerBuffW(Result.pchar(), Len);
        }
        return Result;
    }

    std::int32_t FileOpen(const pas::AnsiString& FileName, std::uint32_t Mode) {
        static const pas::Array<std::uint32_t, 0, 2> AccessMode = pas::Array<std::uint32_t, 0, 2>{{WindowsImports::GENERIC_READ, static_cast<std::uint32_t>(WindowsImports::GENERIC_WRITE), WindowsImports::GENERIC_READ | WindowsImports::GENERIC_WRITE}};
        static const pas::Array<std::uint32_t, 0, 4> ShareMode = pas::Array<std::uint32_t, 0, 4>{{0u, 0u, static_cast<std::uint32_t>(WindowsImports::FILE_SHARE_READ), static_cast<std::uint32_t>(WindowsImports::FILE_SHARE_WRITE), static_cast<std::uint32_t>(WindowsImports::FILE_SHARE_READ | WindowsImports::FILE_SHARE_WRITE)}};
        std::int32_t Result = -1;
        if ((Mode & 3) <= 2 && (Mode & 0x000000f0) <= 0x00000040) {
            return WindowsImports::CreateFileA(FileName.pchar(), AccessMode[Mode & 3], ShareMode[(Mode & 0x000000f0) >> 4], nullptr, WindowsImports::OPEN_EXISTING, WindowsImports::FILE_ATTRIBUTE_NORMAL, 0u);
        }
        return Result;
    }

    std::uint8_t FileExists(const pas::AnsiString& FileName) {
        Windows::TWin32FindDataA Data{};
        std::uint32_t Code = WindowsImports::GetFileAttributesA(FileName.pchar());
        if (Code != 0xffffffffu) {
            return (Code & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) == 0;
        }
        std::uint32_t Error = WindowsImports::GetLastError();
        std::uint8_t Result = false;
        if (Error == WindowsImports::ERROR_FILE_NOT_FOUND || Error == WindowsImports::ERROR_PATH_NOT_FOUND || Error == WindowsImports::ERROR_INVALID_NAME) {
            return Result;
        }
        std::uint32_t Handle = WindowsImports::FindFirstFileA(FileName.pchar(), Data);
        if (Handle != WindowsImports::INVALID_HANDLE_VALUE) {
            WindowsImports::FindClose(Handle);
            return (Data.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) == 0;
        }
        return Result;
    }

    std::uint8_t DirectoryExists(const pas::AnsiString& Directory) {
        std::int32_t Code = WindowsImports::GetFileAttributesA(Directory.pchar());
        return Code != -1 && (Code & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    pas::AnsiString LowerCase(const pas::AnsiString& Text) {
        pas::AnsiString Result{};
        std::int32_t I{};
        std::uint8_t Ch{};
        Result.set_length(Text.length());
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Text.length()); cpp_range.next(I); ) {
            Ch = Text.read(I);
            if (Ch >= 'A' && Ch <= 'Z') {
                Ch = Ch + 32;
            }
            Result.write(I) = Ch;
        }
        return Result;
    }

    pas::AnsiString UpperCase(const pas::AnsiString& Text) {
        pas::AnsiString Result{};
        std::int32_t I{};
        std::uint8_t Ch{};
        Result.set_length(Text.length());
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Text.length()); cpp_range.next(I); ) {
            Ch = Text.read(I);
            if (Ch >= 'a' && Ch <= 'z') {
                Ch = Ch - 32;
            }
            Result.write(I) = Ch;
        }
        return Result;
    }

    pas::AnsiString Trim(const pas::AnsiString& S) {
        std::int32_t L = S.length();
        std::int32_t I = 1;
        while (I <= L && S.read(I) <= ' ') {
            ++I;
        }
        if (I > L) {
            return pas::AnsiString();
        }
        while (S.read(L) <= ' ') {
            --L;
        }
        return pas::copy(S, I, L - I + 1);
    }

    std::uint32_t StrLen(std::uint8_t* Str) {
        std::uint32_t Result = 0u;
        while (Str[Result] != '\000') {
            ++Result;
        }
        return Result;
    }

    std::int32_t StrComp(std::uint8_t* Left, std::uint8_t* Right) {
        if (Left == Right) {
            return 0;
        }
        while (*Left == *Right && *Left != '\000') {
            ++Left;
            ++Right;
        }
        return *Left - *Right;
    }

    std::int32_t AnsiStrComp(std::uint8_t* Left, std::uint8_t* Right) {
        return WindowsSdk::CompareStringA(WindowsSdk::LOCALE_USER_DEFAULT, 0u, Left, -1, Right, -1) - 2;
    }

    pas::AnsiString StrPas(std::uint8_t* Str) {
        return static_cast<pas::AnsiString>(Str);
    }

    std::uint8_t* StrAlloc(std::uint32_t Size) {
        std::uint8_t* Result{};
        Size += static_cast<std::int32_t>(sizeof(std::uint32_t));
        pas::get_mem_at(&Result, static_cast<std::int32_t>(Size));
        pas::store_unaligned<std::uint32_t>(reinterpret_cast<System::PCardinal>(Result), Size);
        Result += static_cast<std::int32_t>(sizeof(std::uint32_t));
        return Result;
    }

    void StrDispose(std::uint8_t* Str) {
        if (Str != nullptr) {
            Str -= static_cast<std::int32_t>(sizeof(std::uint32_t));
            pas::free_mem(Str);
        }
    }

    std::uint8_t IsLeapYear(std::uint16_t Year) {
        return static_cast<std::int32_t>(Year) % 4 == 0 && (static_cast<std::int32_t>(Year) % 100 != 0 || static_cast<std::int32_t>(Year) % 400 == 0);
    }

    System::TDateTime EncodeDate(std::uint16_t Year, std::uint16_t Month, std::uint16_t Day) {
        static const pas::Array<std::uint16_t, 1, 12> Days = pas::Array<std::uint16_t, 1, 12>{{
            static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(28), static_cast<std::uint16_t>(31),
            static_cast<std::uint16_t>(30), static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(30),
            static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(30),
            static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(30), static_cast<std::uint16_t>(31),
        }};
        std::int32_t I{};
        if (Year < 1 || Year > 9999 || Month < 1 || Month > 12) {
            pas::raise(pas::make_exception<pas::ConvertError>("Invalid argument to date encode"_a));
        }
        std::int32_t MaximumDay = Days[Month];
        if (Month == 2 && SysUtilsImports::IsLeapYear(Year)) {
            ++MaximumDay;
        }
        if (Day < 1 || Day > MaximumDay) {
            pas::raise(pas::make_exception<pas::ConvertError>("Invalid argument to date encode"_a));
        }
        {
            const std::int32_t cpp_last = Month - 1;
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Day += Days[I];
                }
            }
        }
        if (Month > 2 && SysUtilsImports::IsLeapYear(Year)) {
            ++Day;
        }
        I = Year - 1;
        return I * 365 + I / 4 - I / 100 + I / 400 + Day - 693594;
    }

    System::TDateTime EncodeTime(std::uint16_t Hour, std::uint16_t Min, std::uint16_t Sec, std::uint16_t MSec) {
        if (Hour >= 24 || Min >= 60 || Sec >= 60 || MSec >= 1000) {
            pas::raise(pas::make_exception<pas::ConvertError>("Invalid argument to time encode"_a));
        }
        return pas::real_divide(Hour * 3600000 + Min * 60000 + Sec * 1000 + MSec, 8.64E+7L);
    }

    pas::AnsiString FloatToStrF(pas::Extended Value, TFloatFormat Format, std::int32_t Precision, std::int32_t Digits) {
        if (Format != ffFixed) {
            pas::raise(pas::make_exception<pas::ConvertError>("Unsupported FloatToStrF format"_a));
        }
        return SysUtilsImports::FixedFloatText(Value, Precision, Digits, SysUtils::DecimalSeparator);
    }

    System::TDateTime Now() {
        Windows::TSystemTime Local{};
        WindowsSdk::GetLocalTime(Local);
        pas::Extended cpp_left = SysUtilsImports::EncodeDate(Local.wYear, Local.wMonth, Local.wDay);
        return cpp_left + SysUtilsImports::EncodeTime(Local.wHour, Local.wMinute, Local.wSecond, Local.wMilliseconds);
    }

    void DecodeDate(System::TDateTime DateTime, std::uint16_t& Year, std::uint16_t& Month, std::uint16_t& Day) {
        static const pas::Array<std::uint16_t, 1, 12> Days = pas::Array<std::uint16_t, 1, 12>{{
            static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(28), static_cast<std::uint16_t>(31),
            static_cast<std::uint16_t>(30), static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(30),
            static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(30),
            static_cast<std::uint16_t>(31), static_cast<std::uint16_t>(30), static_cast<std::uint16_t>(31),
        }};
        std::int32_t T{};
        std::int32_t Time{};
        std::int32_t DaysInMonth{};
        SysUtilsImports::DateDayAndTime(DateTime, T, Time);
        Year = 0;
        Month = 0;
        Day = 0;
        if (T <= 0) {
            return;
        }
        --T;
        std::int32_t Y = 1;
        while (T >= 146097) {
            T -= 146097;
            Y += 400;
        }
        std::int32_t I = T / 36524;
        if (I == 4) {
            --I;
        }
        T -= I * 36524;
        Y += I * 100;
        I = T / 1461;
        T -= I * 1461;
        Y += I * 4;
        I = T / 365;
        if (I == 4) {
            --I;
        }
        T -= I * 365;
        Y += I;
        std::int32_t M = 1;
        while (true) {
            DaysInMonth = Days[M];
            if (M == 2 && SysUtilsImports::IsLeapYear(Y)) {
                ++DaysInMonth;
            }
            if (T < DaysInMonth) {
                break;
            }
            T -= DaysInMonth;
            ++M;
        }
        Year = Y;
        Month = M;
        Day = T + 1;
    }

    void DecodeTime(System::TDateTime DateTime, std::uint16_t& Hour, std::uint16_t& Min, std::uint16_t& Sec, std::uint16_t& MSec) {
        std::int32_t Date{};
        std::int32_t Time{};
        SysUtilsImports::DateDayAndTime(DateTime, Date, Time);
        Hour = Time / 3600000;
        Min = Time / 60000 % 60;
        Sec = Time / 1000 % 60;
        MSec = Time % 1000;
    }

    System::TDateTime SystemTimeToDateTime(const Windows::TSystemTime& SystemTime) {
        System::TDateTime Result = SysUtilsImports::EncodeDate(SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay);
        System::TDateTime Time = SysUtilsImports::EncodeTime(SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds);
        if (Result >= 0.0L) {
            return static_cast<long double>(Result) + Time;
        }
        return static_cast<long double>(Result) - Time;
    }

    // Game numeric formats and the default locale date/time expansion. Named
    // dates and non-Gregorian eras remain explicit unsupported format diagnostics.
    pas::AnsiString FormatDateTime(const pas::AnsiString& Format, System::TDateTime DateTime) {
        pas::AnsiString Result{};
        std::uint16_t Year{};
        std::uint16_t Month{};
        std::uint16_t Day{};
        std::uint16_t Hour{};
        std::uint16_t Min{};
        std::uint16_t Sec{};
        std::uint16_t MSec{};
        std::int32_t I{};
        std::int32_t Start{};
        std::int32_t Count{};
        std::int32_t Number{};
        std::int32_t Digits{};
        std::int32_t P{};
        std::uint8_t Starter{};
        std::uint8_t Token{};
        pas::AnsiString Text{};
        std::uint8_t BetweenQuotes{};
        SysUtilsImports::DecodeDate(DateTime, Year, Month, Day);
        SysUtilsImports::DecodeTime(DateTime, Hour, Min, Sec, MSec);
        if (Format == "") {
            return SysUtilsImports::FormatDateTime("c"_a, DateTime);
        }
        std::uint8_t Use12HourClock = false;
        std::uint8_t LastToken = ' ';
        I = 1;
        while (I <= Format.length() && Format.read(I) != '\000') {
            Starter = Format.read(I);
            Token = Starter;
            ++I;
            if (pas::in_range(Token, static_cast<std::int32_t>('a'), static_cast<std::int32_t>('z'))) {
                Token = Token - 32;
            }
            if (pas::in_range(Token, static_cast<std::int32_t>('A'), static_cast<std::int32_t>('Z'))) {
                if (Token == 'M' && LastToken == 'H') {
                    Token = 'N';
                }
                LastToken = Token;
            }
            switch (Token) {
                case 'Y':
                case 'M':
                case 'D':
                case 'H':
                case 'N':
                case 'S':
                case 'Z': {
                    Count = 1;
                    while (I <= Format.length() && Format.read(I) == Starter) {
                        ++I;
                        ++Count;
                    }
                    Digits = Count;
                    switch (Token) {
                        case 'Y': {
                            if (Count <= 2) {
                                Number = static_cast<std::int32_t>(Year) % 100;
                                Digits = 2;
                            } else {
                                Number = Year;
                                Digits = 4;
                            }
                            break;
                        }
                        case 'M': {
                            Number = Month;
                            if (Count > 2) {
                                pas::raise(pas::make_exception<pas::ConvertError>("Unsupported date format: month name"_a));
                            }
                            break;
                        }
                        case 'D': {
                            Number = Day;
                            if (Count > 2) {
                                pas::raise(pas::make_exception<pas::ConvertError>("Unsupported date format: day name"_a));
                            }
                            break;
                        }
                        case 'H': {
                            BetweenQuotes = false;
                            P = I;
                            while (P <= Format.length() && Format.read(P) != '\000') {
                                if (pas::in_set<'A', 'A', 'a', 'a'>(Format.read(P)) && static_cast<std::uint8_t>(BetweenQuotes ^ 1)) {
                                    Text = SysUtilsImports::UpperCase(pas::copy(Format, P, 5));
                                    if (Text == "AM/PM" || pas::copy(Text, 1, 3) == "A/P" || pas::copy(Text, 1, 4) == "AMPM") {
                                        Use12HourClock = true;
                                    }
                                    break;
                                }
                                if (pas::in_set<'H', 'H', 'h', 'h'>(Format.read(P))) {
                                    break;
                                }
                                if (pas::in_set<'\"', '\"', '\'', '\''>(Format.read(P))) {
                                    BetweenQuotes = static_cast<std::uint8_t>(BetweenQuotes ^ 1);
                                }
                                ++P;
                            }
                            Number = Hour;
                            if (Use12HourClock) {
                                if (Number == 0) {
                                    Number = 12;
                                } else if (Number > 12) {
                                    Number -= 12;
                                }
                            }
                            if (Digits > 2) {
                                Digits = 2;
                            }
                            break;
                        }
                        case 'N': {
                            Number = Min;
                            if (Digits > 2) {
                                Digits = 2;
                            }
                            break;
                        }
                        case 'S': {
                            Number = Sec;
                            if (Digits > 2) {
                                Digits = 2;
                            }
                            break;
                        }
                        case 'Z': {
                            Number = MSec;
                            if (Digits > 3) {
                                Digits = 3;
                            }
                            break;
                        }
                    }
                    Text = SysUtils::IntToStr(Number);
                    while (Text.length() < Digits) {
                        Text = pas::concat_ansi({"0", Text});
                    }
                    Result = pas::concat_ansi({Result, Text});
                    break;
                }
                case 'A': {
                    Text = SysUtilsImports::UpperCase(pas::copy(Format, I - 1, 5));
                    if (Text == "AM/PM") {
                        if (Hour < 12) {
                            Result = pas::concat_ansi({Result, pas::copy(Format, I - 1, 2)});
                        } else {
                            Result = pas::concat_ansi({Result, pas::copy(Format, I + 2, 2)});
                        }
                        I += 4;
                        Use12HourClock = true;
                    } else if (pas::copy(Text, 1, 3) == "A/P") {
                        if (Hour < 12) {
                            Result = pas::concat_ansi({Result, Format.read(I - 1)});
                        } else {
                            Result = pas::concat_ansi({Result, Format.read(I + 1)});
                        }
                        I += 2;
                        Use12HourClock = true;
                    } else if (pas::copy(Text, 1, 4) == "AMPM") {
                        if (Hour < 12) {
                            Result = pas::concat_ansi({Result, TimeAMString});
                        } else {
                            Result = pas::concat_ansi({Result, TimePMString});
                        }
                        I += 3;
                        Use12HourClock = true;
                    } else {
                        pas::raise(pas::make_exception<pas::ConvertError>(pas::concat_ansi({"Unsupported date format token: ", Starter})));
                    }
                    break;
                }
                case 'C': {
                    while (I <= Format.length() && Format.read(I) == Starter) {
                        ++I;
                    }
                    Result = pas::concat_ansi({Result, SysUtilsImports::FormatDateTime(ShortDateFormat, DateTime)});
                    if (Hour != 0 || Min != 0 || Sec != 0) {
                        Result = pas::concat_ansi({Result, " ", SysUtilsImports::FormatDateTime(LongTimeFormat, DateTime)});
                    }
                    break;
                }
                case 'E':
                case 'G':
                case 'T': {
                    pas::raise(pas::make_exception<pas::ConvertError>(pas::concat_ansi({"Unsupported date format token: ", Starter})));
                }
                case '/': {
                    if (DateSeparator != '\000') {
                        Result = pas::concat_ansi({Result, DateSeparator});
                    }
                    break;
                }
                case ':': {
                    if (TimeSeparator != '\000') {
                        Result = pas::concat_ansi({Result, TimeSeparator});
                    }
                    break;
                }
                case '\'':
                case '\"': {
                    Start = I;
                    while (I <= Format.length() && Format.read(I) != '\000' && Format.read(I) != Starter) {
                        ++I;
                    }
                    Result = pas::concat_ansi({Result, pas::copy(Format, Start, I - Start)});
                    if (I <= Format.length() && Format.read(I) != '\000') {
                        ++I;
                    }
                    break;
                }
                default: Result = pas::concat_ansi({Result, Starter}); break;
            }
        }
        // Selected DateTimeToString writes into a 256-byte buffer.
        if (Result.length() > 256) {
            Result.set_length(256);
        }
        return Result;
    }

    pas::AnsiString DateTimeToStr(System::TDateTime DateTime) {
        return SysUtilsImports::FormatDateTime(pas::AnsiString(), DateTime);
    }

    void DateTimeToString(pas::AnsiString& Result, const pas::AnsiString& Format, System::TDateTime DateTime) {
        Result = SysUtilsImports::FormatDateTime(Format, DateTime);
    }

    pas::AnsiString SysErrorMessage(std::int32_t ErrorCode) {
        pas::AnsiString Result{};
        pas::Array<std::uint8_t, 0, 255> Buffer{};
        std::int32_t Len = WindowsSdk::FormatMessageA(WindowsSdk::FORMAT_MESSAGE_FROM_SYSTEM | WindowsSdk::FORMAT_MESSAGE_IGNORE_INSERTS | WindowsSdk::FORMAT_MESSAGE_ARGUMENT_ARRAY, nullptr, ErrorCode, 0u, Buffer.elements, static_cast<std::int32_t>(sizeof(pas::Array<std::uint8_t, 0, 255>)), nullptr);
        while (Len > 0 && pas::in_set<'\000', ' ', '.', '.'>(Buffer[Len - 1])) {
            --Len;
        }
        Result.set_length(Len);
        if (Len > 0) {
            pas::move_memory(&Buffer, Result.pchar(), Len);
        }
        return Result;
    }

    void PAS_STDCALL Sleep(std::uint32_t Milliseconds) {
        using CppImport = void (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32.dll", "Sleep");
        return cpp_import(Milliseconds);
    }

    std::int32_t FindMatchingFile(SysUtils::TSearchRec& F) {
        Windows::TFileTime LocalFileTime{};
        std::uint16_t DatePart{};
        std::uint16_t TimePart{};
        while ((F.FindData.dwFileAttributes & static_cast<std::uint32_t>(F.ExcludeAttr)) != 0) {
            if (!WindowsImports::FindNextFileA(F.FindHandle, F.FindData)) {
                return WindowsImports::GetLastError();
            }
        }
        WindowsImports::FileTimeToLocalFileTime(pas::ConstRef<Windows::TFileTime>(&F.FindData.ftLastWriteTime), LocalFileTime);
        WindowsImports::FileTimeToDosDateTime(LocalFileTime, DatePart, TimePart);
        F.Time = pas::shl(static_cast<std::int32_t>(DatePart), 16) | TimePart;
        F.Size = F.FindData.nFileSizeLow | pas::shl(static_cast<std::int64_t>(F.FindData.nFileSizeHigh), 32);
        F.Attr = F.FindData.dwFileAttributes;
        F.Name = pas::array_text<pas::AnsiString>(F.FindData.cFileName.elements, 260);
        return 0;
    }

    // Semantic port of the selected RTL's millisecond timestamp conversion.
    // The day is truncated toward zero; the time is the positive remainder.
    void DateDayAndTime(System::TDateTime DateTime, std::int32_t& Date, std::int32_t& Time) {
        pas::Extended Value{};
        Value = DateTime;
        std::int64_t Ticks = System::Round(Value * 8.64E+7L);
        Date = Ticks / 86400000 + 693594;
        Time = pas::abs(Ticks % 86400000);
    }

} // namespace SysUtilsImports
