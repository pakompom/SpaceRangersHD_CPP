#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace ValueListClass {
    struct TValuesList;

} // namespace ValueListClass

namespace CPDiapClass {
    struct TCPDiapazone;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCPDiapazone : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCPDiapazone, EC_Struct::TObjectEx, "TCPDiapazone", 16)
        void p_destroy() override;
        void Clear();
        void LoadFromReader(EC_Buf::TBufEC* Reader);
        std::int64_t GetMinimum();
        std::int64_t GetMaximum();
        std::int32_t GetRandomValue();
        std::uint8_t Contains(pas::Extended Value);
        pas::WideString ToText();
        void LoadFromValues(ValueListClass::TValuesList*& Source);
        void Assign(TCPDiapazone*& Source);
        void Append(TCPDiapazone*& Source);
        void AddRange(std::int64_t MinValue, std::int64_t MaxValue);
        void AddValue(pas::Extended Value);
        void LoadFromText(pas::WideString Text);
        pas::DynArray<std::int64_t> RangeStarts;
        pas::DynArray<std::int64_t> RangeEnds;
        std::int32_t RangeCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace CPDiapClass
