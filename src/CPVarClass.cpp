#include "layout/CPVarClass.hpp"
#include "units/CPDiapClass.hpp"
#include "units/CPVarClass.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"

namespace CPVarClass {
    void TCPVariant_Create(TCPVariant* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Range = pas::construct_call<CPDiapClass::TCPDiapazone>(CPDiapClass::TCPDiapazone_Create);
        Self->Reset();
    }

    void TCPVariant_Destroy(TCPVariant* Self) {
        Self->Reset();
        pas::free(Self->Range);
        Self->Range = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Resets to integer zero; retains the range object.
    void TCPVariant::Reset() {
        FloatValue = 0.0L;
        IntValue = 0;
        Range->Clear();
        ValueKind = cpvkInteger;
    }

    // Deep-copies the range.
    void TCPVariant::Assign(TCPVariant* Source, std::uint8_t FreeSource) {
        Range->Assign(Source->Range);
        FloatValue = Source->FloatValue;
        IntValue = Source->IntValue;
        ValueKind = Source->ValueKind;
        if (FreeSource) {
            pas::free(Source);
        }
    }

    // Comma decimals use Single precision; uppercase E is ignored. Ranges require h, not '..'. Failure preserves the value; empty text becomes zero.
    std::uint8_t TCPVariant::TryLoadFromText(pas::WideString Text) {
        std::int32_t i{};
        std::uint8_t Result = false;
        std::int32_t Count = Text.length();
        if (Count == 0) {
            Text = u"0"_w;
        }
        if (TCPVariant::HasNumericChars(Text, Count)) {
            if (TCPVariant::HasIntegerChars(Text, Count)) {
                ValueKind = cpvkInteger;
                Range->Clear();
                IntValue = EC_Str::ExtractDigitsToIntW(pas::view(Text));
                FloatValue = 0.0L;
                return true;
            }
            ValueKind = cpvkFloat;
            Range->Clear();
            FloatValue = EC_Str::ExtractDecimalToSingleW(Text);
            IntValue = 0;
            return true;
        } else if (Count > 1 && Text.read(1) == u'[' && Text.read(Count) == u']') {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
                char16_t cpp_case = Text.read(i);
                if (cpp_case >= u'0' && cpp_case <= u'9' || cpp_case == u'[' || cpp_case == u']' || cpp_case == u'h' || cpp_case == u';' || cpp_case == u'-') {
                } else {
                    return Result;
                }
            }
            ValueKind = cpvkRange;
            Range->LoadFromText(Text);
            FloatValue = 0.0L;
            IntValue = 0;
            return true;
        } else {
            return Result;
        }
    }

    // Permits digits, comma and uppercase E; not a syntax check.
    std::uint8_t TCPVariant::HasNumericChars(pas::WideString& Text, std::int32_t TextLength) {
        std::int32_t i{};
        std::uint8_t Result = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            if ((Text.read(i) < u'0' || Text.read(i) > u'9') && Text.read(i) != u',' && Text.read(i) != u'E') {
                return Result;
            }
        }
        return true;
    }

    // Permits digits and uppercase E; not a syntax check.
    std::uint8_t TCPVariant::HasIntegerChars(pas::WideString& Text, std::int32_t TextLength) {
        std::int32_t i{};
        std::uint8_t Result = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            if ((Text.read(i) < u'0' || Text.read(i) > u'9') && Text.read(i) != u'E') {
                return Result;
            }
        }
        return true;
    }

    // Numeric conversions resample ranges; unknown tags return zero.
    pas::Extended TCPVariant::AsExtended() {
        pas::Extended Result{};
        Result = 0.0L;
        if (ValueKind == cpvkRange) {
            return Range->GetRandomValue();
        } else if (ValueKind == cpvkFloat) {
            return FloatValue;
        } else if (ValueKind == cpvkInteger) {
            return IntValue;
        } else {
            return Result;
        }
    }

    // Float conversion clamps at +/-2000000000; within bounds, uses System.Round(value + 1E-11).
    std::int32_t TCPVariant::AsInteger() {
        std::int32_t Result = 0;
        if (ValueKind == cpvkRange) {
            return Range->GetRandomValue();
        } else if (ValueKind == cpvkFloat) {
            if (FloatValue < pas::constant(static_cast<long double>(-QuestNumericLimit))) {
                return -QuestNumericLimit;
            } else if (FloatValue > pas::constant(static_cast<long double>(QuestNumericLimit))) {
                return QuestNumericLimit;
            } else {
                return System::Round(FloatValue + 1.0E-11L);
            }
        } else if (ValueKind == cpvkInteger) {
            return IntValue;
        } else {
            return Result;
        }
    }

    void TCPVariant::p_destroy() {
        CPVarClass::TCPVariant_Destroy(this);
    }

} // namespace CPVarClass
