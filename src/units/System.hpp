#pragma once
#include "types/System.hpp"

namespace System {
    extern std::uint32_t RandSeed;

    extern std::uint8_t IsMultiThread;

    extern System::TSystemThreadFuncProc SystemThreadFuncProc;

    // RTL hook, called with the Windows RaiseException stdcall ABI.
    extern void* RaiseExceptionProc;

    extern std::uint32_t HInstance;

    inline std::uint16_t Get8087CW();

    // Also clears pending x87 exceptions.
    inline void Set8087CW(std::uint16_t ControlWord);

    inline pas::Extended Frac(pas::Extended Value);

    inline pas::Extended Exp(pas::Extended Value);

    inline pas::Extended Cos(pas::Extended Value);

    inline pas::Extended Sin(pas::Extended Value);

    inline pas::Extended Ln(pas::Extended Value);

    inline pas::Extended ArcTan(pas::Extended Value);

    inline pas::Extended Sqrt(pas::Extended Value);

    // Uses the current x87 rounding mode, normally nearest with ties to even.
    inline std::int64_t Round(pas::Extended Value);

    // Independent of the current x87 rounding mode; preserves it.
    inline std::int64_t Trunc(pas::Extended Value);

    inline pas::AnsiString WideCharToString(char16_t* Source);

    // DCC32 MAP System.AllocMem. Source rtl/sys/System.pas:2536.
    inline void* AllocMem(std::uint32_t Size);

    inline std::int32_t PAS_STDCALL IInterface_QueryInterface(pas::ComView<IInterface_Tag> Self, const TGUID& IID, void* Obj);

    inline std::int32_t PAS_STDCALL IInterface_u_Release(pas::ComView<IInterface_Tag> Self);

} // namespace System

#include "inline/System.hpp"
