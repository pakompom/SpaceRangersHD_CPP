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
        void Reset();
        void Assign(TCPVariant* Source, std::uint8_t FreeSource);
        std::uint8_t TryLoadFromText(pas::WideString Text);
        static std::uint8_t HasNumericChars(pas::WideString& Text, std::int32_t TextLength);
        static std::uint8_t HasIntegerChars(pas::WideString& Text, std::int32_t TextLength);
        pas::Extended AsExtended();
        std::int32_t AsInteger();
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
