#include "types/BlockParException.hpp"
#include "types/BreakMessageGIException.hpp"
#include "units/ExceptionInfo.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"

// Hex helpers in the inferred ExceptionInfo region. Wider unit ownership remains inferred.
namespace ExceptionInfo {
    const pas::Array<char16_t, 0, 15> ExportHexDigits = pas::Array<char16_t, 0, 15>{{
        u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7',
        u'8', u'9', u'A', u'B', u'C', u'D', u'E', u'F',
    }};

    std::uint8_t ReportingException = false;

    ExceptionInfo::TRaiseExceptionCallback PreviousRaiseException = nullptr;

    pas::AnsiString ExceptionLogTimestamp() {
        pas::AnsiString Text{};
        System::TDateTime Time = SysUtilsImports::Now();
        SysUtilsImports::DateTimeToString(Text, "yyyy.mm.dd hh.nn.ss.zzz"_a, Time);
        return Text;
    }

    void ReportUnhandledException(pas::Exception* E, std::uint8_t& Handled) {
        pas::WideString TargetName{};
        pas::WideString SourceName{};
        if (pas::class_cast_if<BreakMessageGIException::EBreakMessageGI*>(E) != nullptr) {
            Handled = false;
        } else {
            if (BlockParException::EBlockPar* eBlockPar = pas::class_cast_if<BlockParException::EBlockPar*>(E)) {
                if (!eBlockPar->IsReportable()) {
                    Handled = false;
                }
            }
            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Exception ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " with message ", E->message}));
            Handled = false;
            if (GR_Main::SuppressExceptionLogCopy) {
                GR_Main::SuppressExceptionLogCopy = false;
            } else {
                SysUtilsImports::CreateDir(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Errors"})));
                TargetName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Errors\\", static_cast<pas::WideString>(ExceptionInfo::ExceptionLogTimestamp()), u".log"});
                SourceName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"########.log"});
                WindowsSdk::CopyFileW(SourceName.pchar(), TargetName.pchar(), 0);
            }
        }
    }

    void PAS_STDCALL RaiseExceptionWithLogging(std::uint32_t Code, std::uint32_t Flags, std::uint32_t ArgumentCount, void* Arguments) {
        std::uint8_t Handled{};
        if (PreviousRaiseException != nullptr) {
            if (ReportingException) {
                ExceptionInfo::PreviousRaiseException(Code, Flags, ArgumentCount, Arguments);
            } else {
                ReportingException = true;
                {
                    pas::ScopeExit cpp_cleanup = [&]() noexcept {
                        ReportingException = false;
                    };
                    try {
                        ExceptionInfo::PreviousRaiseException(Code, Flags, ArgumentCount, Arguments);
                    } catch (...) {
                        auto cpp_exception = pas::caught_object();
                        if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                            Handled = false;
                            ExceptionInfo::ReportUnhandledException(E, Handled);
                            if (!Handled) {
                                throw;
                            }
                        } else {
                            throw;
                        }
                    }
                }
            }
        }
    }

    char16_t HexDigit(std::uint8_t Value) {
        return ExportHexDigits[Value & 0x0000000f];
    }

    pas::WideString ByteToHexText(std::uint8_t Value) {
        pas::WideString Result{};
        Result.set_length(2);
        Result.write(1) = ExceptionInfo::HexDigit(pas::shr(static_cast<std::int32_t>(Value), 4));
        Result.write(2) = ExceptionInfo::HexDigit(pas::shr(static_cast<std::int32_t>(Value), 0));
        return Result;
    }

    // Native initializer saves and replaces the RTL raise hook.
    void UnitInitialize() {
        PreviousRaiseException = pas::callback_from_address<TRaiseExceptionCallback>(System::RaiseExceptionProc);
        System::RaiseExceptionProc = reinterpret_cast<void*>(pas::callback_address(pas::StdcallProc<void(std::uint32_t, std::uint32_t, std::uint32_t, void*)>(RaiseExceptionWithLogging)));
    }

} // namespace ExceptionInfo
