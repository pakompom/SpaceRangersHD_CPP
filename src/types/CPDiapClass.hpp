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
        // Requires at least one range.
        std::int64_t GetMinimum();
        // Requires at least one range.
        std::int64_t GetMaximum();
        // Zero when empty. Sampling weights overlaps repeatedly; lengths and results are 32-bit.
        std::int32_t GetRandomValue();
        // Rounds with System.Round first.
        std::uint8_t Contains(pas::Extended Value);
        // Uses [ahb;c] and signed low 32-bit endpoints; empty output is '['.
        pas::WideString ToText();
        // Ignores Source.AcceptListed.
        void LoadFromValues(ValueListClass::TValuesList*& Source);
        void Assign(TCPDiapazone*& Source);
        // Preserves overlapping and duplicate ranges.
        void Append(TCPDiapazone*& Source);
        // Swaps reversed bounds; does not merge ranges.
        void AddRange(std::int64_t MinValue, std::int64_t MaxValue);
        // Truncates to Int64; caught conversion errors preserve existing ranges.
        void AddValue(pas::Extended Value);
        // Accepts [a..b;c] or [ahb;c]. Endpoints beyond +/-200000000 can expand intervals unexpectedly; '..' normalization can overread.
        void LoadFromText(pas::WideString Text);
        // Owned Delphi dynamic arrays, indexed 0..RangeCount-1; inclusive bounds.
        pas::DynArray<std::int64_t> RangeStarts;
        pas::DynArray<std::int64_t> RangeEnds;
        std::int32_t RangeCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace CPDiapClass
