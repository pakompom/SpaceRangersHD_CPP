#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace CPDiapClass {
    struct TCPDiapazone;

} // namespace CPDiapClass

namespace CPVarClass {
    struct TCPVariant;

    enum TCPValueKind : std::uint8_t {
        cpvkRange = 0,
        cpvkFloat = 1,
        cpvkInteger = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCPVariant : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCPVariant, EC_Struct::TObjectEx, "TCPVariant", 28)
        void p_destroy() override;
        // Resets to integer zero; retains the range object.
        void Reset();
        // Deep-copies the range.
        void Assign(TCPVariant* Source, std::uint8_t FreeSource);
        // Comma decimals use Single precision; uppercase E is ignored. Ranges require h, not '..'. Failure preserves the value; empty text becomes zero.
        std::uint8_t TryLoadFromText(pas::WideString Text);
        // Permits digits, comma and uppercase E; not a syntax check.
        static std::uint8_t HasNumericChars(pas::WideString& Text, std::int32_t TextLength);
        // Permits digits and uppercase E; not a syntax check.
        static std::uint8_t HasIntegerChars(pas::WideString& Text, std::int32_t TextLength);
        // Numeric conversions resample ranges; unknown tags return zero.
        pas::Extended AsExtended();
        // Float conversion clamps at +/-2000000000; within bounds, uses System.Round(value + 1E-11).
        std::int32_t AsInteger();
        // Owned for every ValueKind.
        CPDiapClass::TCPDiapazone* Range;
        pas::Extended FloatValue;
        std::uint8_t cpp_padding[2];
        std::int32_t IntValue;
        TCPValueKind ValueKind;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace CPVarClass
