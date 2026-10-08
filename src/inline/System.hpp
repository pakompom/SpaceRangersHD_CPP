#pragma once
#include "units/System.hpp"

namespace System {
    inline std::uint16_t Get8087CW() {
        return pas::get_fpu_control();
    }

    // Also clears pending x87 exceptions.
    inline void Set8087CW(std::uint16_t ControlWord) {
        pas::set_fpu_control(ControlWord);
    }

    inline double Frac(double Value) {
        return pas::real_frac(Value);
    }

    inline double Exp(double Value) {
        return pas::real_exp(Value);
    }

    inline double Cos(double Value) {
        return pas::real_cos(Value);
    }

    inline double Sin(double Value) {
        return pas::real_sin(Value);
    }

    inline double Ln(double Value) {
        return pas::real_ln(Value);
    }

    inline double ArcTan(double Value) {
        return pas::real_arctan(Value);
    }

    inline double Sqrt(double Value) {
        return pas::real_sqrt(Value);
    }

    // Uses the current x87 rounding mode, normally nearest with ties to even.
    inline std::int64_t Round(double Value) {
        return pas::real_round(Value);
    }

    // Independent of the current x87 rounding mode; preserves it.
    inline std::int64_t Trunc(double Value) {
        return pas::real_trunc(Value);
    }

    // Authored C++ runtime seam. Register this global slot before replacing the
    // forwarding callback; recursive reporting is guarded per thread by the runtime.
    inline void InstallRaiseExceptionHook(void*& Hook) {
        pas::install_raise_exception_hook(Hook);
    }

    inline pas::AnsiString WideCharToString(char16_t* Source) {
        return pas::AnsiString(Source);
    }

    // DCC32 MAP System.AllocMem. Source rtl/sys/System.pas:2536.
    inline void* AllocMem(std::uint32_t Size) {
        return pas::alloc_mem(Size);
    }

    inline std::int32_t PAS_STDCALL IInterface_QueryInterface(pas::ComView<IInterface_Tag> Self, const TGUID& IID,
        void* Obj) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, const TGUID*, void*);
        return pas::com_method<CppMethod>(Self.get(), 0)(Self.get(), &IID, Obj);
    }

    inline std::int32_t PAS_STDCALL IInterface_u_Release(pas::ComView<IInterface_Tag> Self) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 2)(Self.get());
    }

} // namespace System
