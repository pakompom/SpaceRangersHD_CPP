#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace TextFieldClass {
    struct TTextField;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextField : EC_Struct::TObjectEx {
        PAS_CLASS_META(TTextField, EC_Struct::TObjectEx, "TTextField", 8)
        void ClearText();
        void LoadTextLinesFromReader(EC_Buf::TBufEC* Reader);
        pas::WideString Text;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace TextFieldClass
