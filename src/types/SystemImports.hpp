#pragma once
#include "runtime_support.hpp"
#include "types/System.hpp"

namespace SystemImports {
    struct TThreadStart;

    struct TMethod;

    using THandle = std::uint32_t;

    using HINST = THandle;

    using HMODULE = HINST;

    #pragma pack(push, 1)
    struct TThreadStart {
        System::TThreadFunc Func;
        void* Parameter;
    };
    #pragma pack(pop)

    using PThreadStart = TThreadStart*;

    using PDouble = double*;

    #pragma pack(push, 1)
    struct TMethod {
        void* Code;
        void* Data;
    };
    #pragma pack(pop)

    // DCC32 _ValExt spelling of System.Pi: $4000:C90FDAA22168C235.
    // The longer mathematical decimal rounds one ulp higher in this compiler.
    inline constexpr long double Pi = 3.1415926535897932385L;

    inline constexpr std::int32_t MaxInt = 0x7fffffff;

} // namespace SystemImports
