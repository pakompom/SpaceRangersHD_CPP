#pragma once
#include "runtime_support.hpp"

namespace System {
    struct IInterface_Tag;

    struct TGUID;

    using IInterface = pas::ComPtr<IInterface_Tag>;

    using PInteger = std::int32_t*;

    using PCardinal = std::uint32_t*;

    using PWideString = pas::WideString*;

    using PPointer = void**;

    #pragma pack(push, 1)
    struct TGUID {
        std::uint32_t D1;
        std::uint16_t D2;
        std::uint16_t D3;
        pas::Array<std::uint8_t, 0, 7> D4;
    };
    #pragma pack(pop)

    using TDateTime = double;

    using TThreadFunc = pas::Proc<std::int32_t(void*)>;

    using TSystemThreadFuncProc = pas::Proc<void*(TThreadFunc, void*)>;

    using TClass = pas::ClassRef<pas::Object>;

    using PWord = std::uint16_t*;

    using PSingle = float*;

} // namespace System
