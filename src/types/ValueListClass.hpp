#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace ValueListClass {
    struct TValuesList;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TValuesList : EC_Struct::TObjectEx {
        PAS_CLASS_META(TValuesList, EC_Struct::TObjectEx, "TValuesList", 16)
        void p_destroy() override;
        // The initial character filter is overwritten; unrelated characters survive. Does not modify Self.
        static pas::WideString NormalizeSemicolonText(pas::WideString Text);
        void LoadFromReader(EC_Buf::TBufEC* Reader);
        void Clear();
        void LoadFromSemicolonText(pas::WideString Text);
        // An empty list accepts every value, regardless of AcceptListed.
        std::uint8_t AcceptsValue(std::int32_t Value);
        // Zero divisors are unchecked.
        std::uint8_t AcceptsMultiple(std::int32_t Value);
        std::uint8_t AcceptListed;
        std::uint8_t cpp_padding[3];
        // Delphi dynamic array; entries 1..Count are used.
        pas::DynArray<std::int32_t> Values;
        std::int32_t Count;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ValueListClass
