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

    inline double Frac(double Value);

    inline double Exp(double Value);

    inline double Cos(double Value);

    inline double Sin(double Value);

    inline double Ln(double Value);

    inline double ArcTan(double Value);

    inline double Sqrt(double Value);

    // Uses the current x87 rounding mode, normally nearest with ties to even.
    inline std::int64_t Round(double Value);

    // Independent of the current x87 rounding mode; preserves it.
    inline std::int64_t Trunc(double Value);

    // Authored C++ runtime seam. Register this global slot before replacing the
    // forwarding callback; recursive reporting is guarded per thread by the runtime.
    inline void InstallRaiseExceptionHook(void*& Hook);

    inline pas::AnsiString WideCharToString(char16_t* Source);

    // DCC32 MAP System.AllocMem. Source rtl/sys/System.pas:2536.
    inline void* AllocMem(std::uint32_t Size);

    inline std::int32_t PAS_STDCALL IInterface_QueryInterface(pas::ComView<IInterface_Tag> Self, const TGUID& IID,
        void* Obj);

    inline std::int32_t PAS_STDCALL IInterface_u_Release(pas::ComView<IInterface_Tag> Self);

} // namespace System

#include "inline/System.hpp"
