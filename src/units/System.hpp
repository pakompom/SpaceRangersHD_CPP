#pragma once
#include "types/System.hpp"

namespace System {
    extern std::uint32_t RandSeed;

    extern std::uint8_t IsMultiThread;

    extern System::TSystemThreadFuncProc SystemThreadFuncProc;

    extern void* RaiseExceptionProc;

    extern std::uint32_t HInstance;

    inline std::uint16_t Get8087CW();

    inline void Set8087CW(std::uint16_t ControlWord);

    inline pas::Extended Frac(pas::Extended Value);

    inline pas::Extended Exp(pas::Extended Value);

    inline pas::Extended Cos(pas::Extended Value);

    inline pas::Extended Sin(pas::Extended Value);

    inline pas::Extended Ln(pas::Extended Value);

    inline pas::Extended ArcTan(pas::Extended Value);

    inline pas::Extended Sqrt(pas::Extended Value);

    inline std::int64_t Round(pas::Extended Value);

    inline std::int64_t Trunc(pas::Extended Value);

    inline pas::AnsiString WideCharToString(char16_t* Source);

    inline void* AllocMem(std::uint32_t Size);

    inline std::int32_t PAS_STDCALL IInterface_QueryInterface(pas::ComView<IInterface_Tag> Self, const TGUID& IID, void* Obj);

    inline std::int32_t PAS_STDCALL IInterface_u_Release(pas::ComView<IInterface_Tag> Self);

} // namespace System

#include "inline/System.hpp"
