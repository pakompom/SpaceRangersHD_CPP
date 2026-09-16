#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace TextFieldClass {
    struct TTextField;

} // namespace TextFieldClass

namespace ParViewStringClass {
    struct TParViewString;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TParViewString : EC_Struct::TObjectEx {
        PAS_CLASS_META(TParViewString, EC_Struct::TObjectEx, "TParViewString", 16)
        void p_destroy() override;
        void LoadFromReader(EC_Buf::TBufEC* Reader);
        std::int32_t MinValue;
        std::int32_t MaxValue;
        TextFieldClass::TTextField* Text;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ParViewStringClass
