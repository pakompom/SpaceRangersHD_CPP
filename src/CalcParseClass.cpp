#include "layout/CalcParseClass.hpp"
#include "types/ParameterClass.hpp"
#include "units/CPDiapClass.hpp"
#include "units/CPVarClass.hpp"
#include "units/CalcParseClass.hpp"
#include "units/EC_Str.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"

namespace CalcParseClass {
    // Operators borrow operands; OutValue must be an existing, distinct object.
    // Power/add/subtract/multiply promote floats; integer results saturate at +/-2000000000.
    // Negative bases stay negative even for even exponents; integer results round.
    void TCalcParse::ApplyPower(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        std::int32_t A{};
        std::int32_t B{};
        std::int32_t AbsValue{};
        pas::Extended X{};
        pas::Extended Y{};
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkFloat;
        if (Left->ValueKind == CPVarClass::cpvkFloat && Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left = MathImports::Sign(Left->AsExtended());
                return cpp_left * ([&] {
                    pas::Extended cpp_arg = std::fabs(Left->AsExtended());
                    pas::Extended asExtended = Right->AsExtended();
                    return Math::Power(cpp_arg, asExtended);
                }());
            }());
        } else if (Left->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_2 = MathImports::Sign(Left->AsExtended());
                return cpp_left_2 * ([&] {
                    pas::Extended cpp_arg_2 = std::fabs(Left->AsExtended());
                    std::int32_t asInteger = Right->AsInteger();
                    return Math::IntPower(cpp_arg_2, asInteger);
                }());
            }());
        } else if (Right->ValueKind == CPVarClass::cpvkFloat) {
            A = Left->AsInteger();
            AbsValue = pas::abs(A);
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_3 = Math::Power(AbsValue, Right->AsExtended());
                return cpp_left_3 * MathImports::Sign(A);
            }());
        } else {
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            A = Left->AsInteger();
            AbsValue = pas::abs(A);
            B = Right->AsInteger();
            X = A;
            Y = B;
            if (Math::Power(std::fabs(X), Y) > 2.0E+9L) {
                OutValue->IntValue = MathImports::Sign(A) * 2000000000;
            } else {
                std::int32_t cpp_left_4 = System::Round(Math::IntPower(AbsValue, B));
                OutValue->IntValue = cpp_left_4 * MathImports::Sign(A);
            }
        }
    }

    void TCalcParse::ApplyAdd(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        std::int32_t A{};
        std::int32_t B{};
        pas::Extended X{};
        pas::Extended Y{};
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkFloat;
        if (Left->ValueKind == CPVarClass::cpvkFloat && Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left = Left->AsExtended();
                return cpp_left + Right->AsExtended();
            }());
        } else if (Left->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_2 = Left->AsExtended();
                return cpp_left_2 + Right->AsInteger();
            }());
        } else if (Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_3 = Left->AsInteger();
                return cpp_left_3 + Right->AsExtended();
            }());
        } else {
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            A = Left->AsInteger();
            B = Right->AsInteger();
            X = A;
            Y = B;
            if (X + Y > 2.0E+9L) {
                OutValue->IntValue = 2000000000;
            } else if (X + Y < -2.0E+9L) {
                OutValue->IntValue = -2000000000;
            } else {
                OutValue->IntValue = A + B;
            }
        }
    }

    void TCalcParse::ApplySubtract(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        std::int32_t A{};
        std::int32_t B{};
        pas::Extended X{};
        pas::Extended Y{};
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkFloat;
        if (Left->ValueKind == CPVarClass::cpvkFloat && Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left = Left->AsExtended();
                return cpp_left - Right->AsExtended();
            }());
        } else if (Left->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_2 = Left->AsExtended();
                return cpp_left_2 - Right->AsInteger();
            }());
        } else if (Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_3 = Left->AsInteger();
                return cpp_left_3 - Right->AsExtended();
            }());
        } else {
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            A = Left->AsInteger();
            B = Right->AsInteger();
            X = A;
            Y = B;
            if (X - Y > 2.0E+9L) {
                OutValue->IntValue = 2000000000;
            } else if (X - Y < -2.0E+9L) {
                OutValue->IntValue = -2000000000;
            } else {
                OutValue->IntValue = A - B;
            }
        }
    }

    void TCalcParse::ApplyMultiply(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        std::int32_t A{};
        std::int32_t B{};
        pas::Extended X{};
        pas::Extended Y{};
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkFloat;
        if (Left->ValueKind == CPVarClass::cpvkFloat && Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left = Left->AsExtended();
                return cpp_left * Right->AsExtended();
            }());
        } else if (Left->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_2 = Left->AsExtended();
                return cpp_left_2 * Right->AsInteger();
            }());
        } else if (Right->ValueKind == CPVarClass::cpvkFloat) {
            OutValue->FloatValue = ([&] {
                pas::Extended cpp_left_3 = Left->AsInteger();
                return cpp_left_3 * Right->AsExtended();
            }());
        } else {
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            A = Left->AsInteger();
            B = Right->AsInteger();
            X = A;
            Y = B;
            if (X * Y > 2.0E+9L) {
                OutValue->IntValue = 2000000000;
            } else if (X * Y < -2.0E+9L) {
                OutValue->IntValue = -2000000000;
            } else {
                OutValue->IntValue = A * B;
            }
        }
    }

    // Returns float Left * (1 + Right * 0.01).
    void TCalcParse::ApplyPercentChange(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkFloat;
        OutValue->FloatValue = ([&] {
            pas::Extended cpp_left = Left->AsExtended();
            return cpp_left * (1.0L + Right->AsExtended() * 0.01L);
        }());
    }

    // Exact integer quotients stay integer. Zero divisor: +/-2000000000 for integers; float operands incorrectly leave integer zero.
    void TCalcParse::ApplyDivide(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        std::int32_t A{};
        std::int32_t B{};
        pas::Extended X{};
        pas::Extended Y{};
        OutValue->Reset();
        if (Left->ValueKind != CPVarClass::cpvkFloat && Right->ValueKind != CPVarClass::cpvkFloat) {
            A = Left->AsInteger();
            B = Right->AsInteger();
            if (B == 0) {
                OutValue->ValueKind = CPVarClass::cpvkInteger;
                if (A < 0) {
                    OutValue->IntValue = -2000000000;
                } else {
                    OutValue->IntValue = 2000000000;
                }
            } else if (pas::imod(A, B) == 0) {
                OutValue->ValueKind = CPVarClass::cpvkInteger;
                OutValue->IntValue = pas::idiv(A, B);
            } else {
                try {
                    OutValue->ValueKind = CPVarClass::cpvkFloat;
                    OutValue->FloatValue = pas::real_divide(A, B);
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::class_cast_if<pas::DivByZero*>(cpp_exception)) {
                    } else {
                        throw;
                    }
                }
            }
        } else {
            X = Left->AsExtended();
            Y = Right->AsExtended();
            if (Y == 0.0L) {
                if (X < 0.0L) {
                    OutValue->FloatValue = -2.0E+9L;
                } else {
                    OutValue->FloatValue = 2.0E+9L;
                }
            } else {
                try {
                    OutValue->ValueKind = CPVarClass::cpvkFloat;
                    OutValue->FloatValue = pas::real_divide(X, Y);
                } catch (...) {
                    auto cpp_exception_2 = pas::caught_object();
                    if (pas::class_cast_if<pas::DivByZero*>(cpp_exception_2)) {
                    } else {
                        throw;
                    }
                }
            }
        }
    }

    // Truncates toward zero; zero-divisor behavior matches ApplyDivide.
    void TCalcParse::ApplyIntDivide(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        std::int32_t A{};
        std::int32_t B{};
        pas::Extended X{};
        pas::Extended Y{};
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        if (Left->ValueKind != CPVarClass::cpvkFloat && Right->ValueKind != CPVarClass::cpvkFloat) {
            A = Left->AsInteger();
            B = Right->AsInteger();
            if (B == 0) {
                if (A < 0) {
                    OutValue->IntValue = -2000000000;
                } else {
                    OutValue->IntValue = 2000000000;
                }
            } else {
                OutValue->IntValue = pas::idiv(A, B);
            }
        } else {
            X = Left->AsExtended();
            Y = Right->AsExtended();
            if (Y == 0.0L) {
                if (X < 0.0L) {
                    OutValue->FloatValue = -2.0E+9L;
                } else {
                    OutValue->FloatValue = 2.0E+9L;
                }
            } else {
                try {
                    OutValue->IntValue = System::Trunc(pas::real_divide(X, Y));
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::class_cast_if<pas::DivByZero*>(cpp_exception)) {
                    } else {
                        throw;
                    }
                }
            }
        }
    }

    // Float operands produce a float remainder after truncation. Zero-divisor behavior matches ApplyDivide.
    void TCalcParse::ApplyModulo(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        pas::Extended X{};
        pas::Extended Y{};
        std::int32_t A{};
        std::int32_t B{};
        OutValue->Reset();
        std::uint8_t Negative = false;
        if (Left->ValueKind != CPVarClass::cpvkFloat && Right->ValueKind != CPVarClass::cpvkFloat) {
            A = Left->AsInteger();
            B = Right->AsInteger();
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            Negative = A < 0;
            if (B == 0) {
                if (Negative) {
                    OutValue->IntValue = -2000000000;
                } else {
                    OutValue->IntValue = 2000000000;
                }
            } else {
                OutValue->IntValue = pas::imod(pas::abs(A), pas::abs(B));
                if (Negative) {
                    OutValue->IntValue *= -1;
                }
            }
        } else {
            X = Left->AsExtended();
            Y = System::Trunc(Right->AsExtended());
            if (Y == 0.0L) {
                if (X < 0.0L) {
                    OutValue->FloatValue = -2.0E+9L;
                } else {
                    OutValue->FloatValue = 2.0E+9L;
                }
            } else {
                try {
                    if (Y < 0.0L) {
                        Y = Y * -1.0L;
                    }
                    if (X < 0.0L) {
                        X = X * -1.0L;
                        Negative = true;
                    }
                    OutValue->ValueKind = CPVarClass::cpvkFloat;
                    OutValue->FloatValue = System::Trunc(X - System::Trunc(pas::real_divide(X, Y)) * Y);
                    if (Negative) {
                        OutValue->FloatValue = OutValue->FloatValue * -1.0L;
                    }
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::class_cast_if<pas::DivByZero*>(cpp_exception)) {
                    } else {
                        throw;
                    }
                }
            }
        }
    }

    // Uses operand extrema, rounds floats and swaps reversed bounds. Range operands must be nonempty.
    void TCalcParse::ApplyRange(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        std::int64_t Maximum = 0;
        std::int64_t Minimum = 0;
        if (Left->ValueKind == CPVarClass::cpvkFloat) {
            Minimum = System::Round(Left->FloatValue);
        } else if (Left->ValueKind == CPVarClass::cpvkInteger) {
            Minimum = Left->IntValue;
        } else if (Left->ValueKind == CPVarClass::cpvkRange) {
            Minimum = Left->Range->GetMinimum();
        }
        if (Right->ValueKind == CPVarClass::cpvkFloat) {
            Maximum = System::Round(Right->FloatValue);
        } else if (Right->ValueKind == CPVarClass::cpvkInteger) {
            Maximum = Right->IntValue;
        } else if (Right->ValueKind == CPVarClass::cpvkRange) {
            Maximum = Right->Range->GetMaximum();
        }
        OutValue->ValueKind = CPVarClass::cpvkRange;
        OutValue->Range->AddRange(Minimum, Maximum);
    }

    // Two ranges sample Left once; scalar/range membership rounds the scalar.
    void TCalcParse::ApplyMembership(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        if (Left->ValueKind != CPVarClass::cpvkRange && Right->ValueKind != CPVarClass::cpvkRange) {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left == Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        } else if (Left->ValueKind == CPVarClass::cpvkRange && Right->ValueKind != CPVarClass::cpvkRange) {
            pas::Extended asExtended = Right->AsExtended();
            CPDiapClass::TCPDiapazone* range = Left->Range;
            if (range->Contains(asExtended)) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        } else if (Left->ValueKind != CPVarClass::cpvkRange && Right->ValueKind == CPVarClass::cpvkRange) {
            pas::Extended asExtended_2 = Left->AsExtended();
            CPDiapClass::TCPDiapazone* range_2 = Right->Range;
            if (range_2->Contains(asExtended_2)) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        } else if (Left->ValueKind == CPVarClass::cpvkRange && Right->ValueKind == CPVarClass::cpvkRange) {
            pas::Extended asInteger = Left->AsInteger();
            CPDiapClass::TCPDiapazone* range_3 = Right->Range;
            if (range_3->Contains(asInteger)) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    // Comparisons return integer 0 or 1 and sample each range operand once.
    void TCalcParse::ApplyLessThan(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left < Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    void TCalcParse::ApplyGreaterThan(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left > Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    void TCalcParse::ApplyLessOrEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left <= Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    void TCalcParse::ApplyGreaterOrEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left >= Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    void TCalcParse::ApplyEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left == Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    void TCalcParse::ApplyNotEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        OutValue->ValueKind = CPVarClass::cpvkInteger;
        {
            pas::Extended cpp_left = Left->AsExtended();
            if (cpp_left != Right->AsExtended()) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        }
    }

    // Range operands concatenate rather than intersect; duplicates remain.
    void TCalcParse::ApplyAnd(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        OutValue->Reset();
        if (Left->ValueKind != CPVarClass::cpvkRange && Right->ValueKind > CPVarClass::cpvkRange) {
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            if (Left->AsExtended() != 0.0L && Right->AsExtended() != 0.0L) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        } else if (Left->ValueKind == CPVarClass::cpvkRange && Right->ValueKind == CPVarClass::cpvkRange) {
            OutValue->Assign(Left, false);
            OutValue->Range->Append(Right->Range);
        } else if (Left->ValueKind != CPVarClass::cpvkRange && Right->ValueKind == CPVarClass::cpvkRange) {
            OutValue->Assign(Right, false);
            if (Left->ValueKind == CPVarClass::cpvkInteger) {
                OutValue->Range->AddRange(Left->IntValue, Left->IntValue);
            } else {
                OutValue->Range->AddValue(Left->FloatValue);
            }
        } else if (Left->ValueKind == CPVarClass::cpvkRange && Right->ValueKind != CPVarClass::cpvkRange) {
            OutValue->Assign(Left, false);
            if (Right->ValueKind == CPVarClass::cpvkInteger) {
                OutValue->Range->AddRange(Right->IntValue, Right->IntValue);
            } else {
                OutValue->Range->AddValue(Right->FloatValue);
            }
        }
    }

    // Range operands concatenate; duplicates remain.
    void TCalcParse::ApplyOr(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue) {
        if (Left->ValueKind != CPVarClass::cpvkRange && Right->ValueKind > CPVarClass::cpvkRange) {
            OutValue->ValueKind = CPVarClass::cpvkInteger;
            if (Left->AsExtended() != 0.0L || Right->AsExtended() != 0.0L) {
                OutValue->IntValue = 1;
            } else {
                OutValue->IntValue = 0;
            }
        } else if (Left->ValueKind == CPVarClass::cpvkRange && Right->ValueKind == CPVarClass::cpvkRange) {
            OutValue->Assign(Left, false);
            OutValue->Range->Append(Right->Range);
        } else if (Left->ValueKind != CPVarClass::cpvkRange && Right->ValueKind == CPVarClass::cpvkRange) {
            OutValue->Assign(Right, false);
            if (Left->ValueKind == CPVarClass::cpvkInteger) {
                OutValue->Range->AddRange(Left->IntValue, Left->IntValue);
            } else {
                OutValue->Range->AddValue(Left->FloatValue);
            }
        } else if (Left->ValueKind == CPVarClass::cpvkRange && Right->ValueKind != CPVarClass::cpvkRange) {
            OutValue->Assign(Left, false);
            if (Right->ValueKind == CPVarClass::cpvkInteger) {
                OutValue->Range->AddRange(Right->IntValue, Right->IntValue);
            } else {
                OutValue->Range->AddValue(Right->FloatValue);
            }
        }
    }

    // External spellings -> internal tokens: pct %, div f, mod g, in #,
    // to $, or |, and &, <> e, >= c, <= b, .. h, and decimal dot -> comma.
    // Text is read-only despite var. Uses ANSI lowercase and boundary-free substitutions; always wraps the result in parentheses.
    pas::WideString TCalcParse::NormalizeTokens(pas::WideString& Text) {
        pas::WideString Previous{};
        pas::WideString Current{};
        std::int32_t Index{};
        Current = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(EC_Str::TrimWideString(Text))));
        do {
            Previous = Current;
            Current = EC_Str::ReplaceAllWideString(Current, u"pct"_wref.get(), u"%"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"div"_wref.get(), u"f"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"mod"_wref.get(), u"g"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"in"_wref.get(), u"#"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"to"_wref.get(), u"$"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"or"_wref.get(), u"|"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"and"_wref.get(), u"&"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"<>"_wref.get(), u"e"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u">="_wref.get(), u"c"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"<="_wref.get(), u"b"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u".."_wref.get(), u"h"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"."_wref.get(), u","sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"  "_wref.get(), u" "sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"d"_wref.get(), u""sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"m"_wref.get(), u""sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"o"_wref.get(), u""sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"t"_wref.get(), u""sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"i"_wref.get(), u""sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"a"_wref.get(), u""sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"n"_wref.get(), u""sv);
        } while (!(Current == Previous));
        do {
            Previous = Current;
            Index = EC_Str::FindTextOffsetW(Current, u" "_wref.get(), 0);
            while (Index > 0) {
                if (EC_Str::FindTextOffsetW(u"%fg#$|&ecbh*+/-()><=[]{}"_wref.get(), static_cast<pas::WideString>(Current.read(Index)), 0) < 0 && EC_Str::FindTextOffsetW(u"%fg#$|&ecbh*+/-()><=[]{}"_wref.get(), static_cast<pas::WideString>(Current.read(Index + 2)), 0) < 0) {
                    Index = EC_Str::FindTextOffsetW(Current, u" "_wref.get(), Index + 1);
                } else {
                    Current = pas::concat_wide_reverse({EC_Str::CopyWideStringUnchecked(Current, Index + 2, Current.length() - Index - 1), EC_Str::CopyWideStringUnchecked(Current, 1, Index)});
                    Index = EC_Str::FindTextOffsetW(Current, u" "_wref.get(), Index);
                }
            }
        } while (!(Current == Previous));
        return pas::concat_wide({u"(", Previous, u")"});
    }

    // Text is read-only despite var. Removes at most one enclosing parenthesis pair; leaves outer whitespace.
    pas::WideString TCalcParse::FormatTokens(pas::WideString& Text) {
        pas::WideString Previous{};
        pas::WideString Current{};
        pas::WideString Inner{};
        std::int32_t i{};
        Current = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Text)));
        Current = EC_Str::ReplaceAllWideString(Current, u"$"_wref.get(), u" to "sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"#"_wref.get(), u" in "sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"|"_wref.get(), u" or "sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"&"_wref.get(), u" and "sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"e"_wref.get(), u"<>"sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"c"_wref.get(), u">="sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"b"_wref.get(), u"<="sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"f"_wref.get(), u" div "sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"g"_wref.get(), u" mod "sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"h"_wref.get(), u".."sv);
        Current = EC_Str::ReplaceAllWideString(Current, u"%"_wref.get(), u" pct "sv);
        do {
            Previous = Current;
            Current = EC_Str::ReplaceAllWideString(Current, u"  "_wref.get(), u" "sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"(0-"_wref.get(), u"(-"sv);
        } while (!(Current == Previous));
        std::int32_t Count = Current.length();
        if (Count >= 2 && Current.read(1) == u'(' && Current.read(Count) == u')') {
            for (auto cpp_range = pas::for_to<std::int32_t>(2, Count - 1); cpp_range.next(i); ) {
                Inner = pas::concat_wide({Inner, Current.read(i)});
            }
            if (TCalcParse::HasBalancedParenthesesInSlice(pas::view(Current), 2, Count - 1)) {
                Previous = std::move(Inner);
            }
        }
        return Previous;
    }

    // Lower ranks bind tighter; -1 means not an operator.
    // 1: ^ / f g; 2: * %; 3: -; 4: +; 5: $; 6: #;
    // 7: < > = b c e; 8: &; 9: |.
    std::int32_t TCalcParse::GetOperatorRank(char16_t Token) {
        std::int32_t Rank = -1;
        switch (Token) {
            case u'^': Rank = 1; break;
            case u'/': Rank = 1; break;
            case u'f': Rank = 1; break;
            case u'g': Rank = 1; break;
            case u'*': Rank = 2; break;
            case u'%': Rank = 2; break;
            case u'-': Rank = 3; break;
            case u'+': Rank = 4; break;
            case u'$': Rank = 5; break;
            case u'#': Rank = 6; break;
            case u'c': Rank = 7; break;
            case u'b': Rank = 7; break;
            case u'e': Rank = 7; break;
            case u'>': Rank = 7; break;
            case u'<': Rank = 7; break;
            case u'=': Rank = 7; break;
            case u'&': Rank = 8; break;
            case u'|': Rank = 9; break;
        }
        return Rank;
    }

    // Requires a nonempty operator run. Minus parity controls the sign; ties choose the leftmost weakest operator.
    pas::WideString TCalcParse::CollapseOperatorRun(const pas::WideString& Text) {
        std::int32_t i{};
        pas::WideString Operators{};
        std::int32_t Count = Text.length();
        std::int32_t MinusCount = 0;
        std::int32_t PlusCount = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            if (Text.read(i) == u'-') {
                ++MinusCount;
            }
            if (Text.read(i) == u'+') {
                ++PlusCount;
            }
        }
        Operators = EC_Str::ReplaceAllWideString(Text, u"-"_wref.get(), u""sv);
        Operators = EC_Str::ReplaceAllWideString(Operators, u"+"_wref.get(), u""sv);
        if (MinusCount % 2 == 1) {
            Operators = pas::concat_wide({Operators, u"-"});
        } else if (PlusCount > 0 || MinusCount > 0) {
            Operators = pas::concat_wide({Operators, u"+"});
        }
        Count = Operators.length();
        MinusCount = 0;
        PlusCount = 0;
        {
            const std::int32_t cpp_first = Count;
            if (cpp_first >= 1) {
                for (i = cpp_first; i >= 1; --i) {
                    if (TCalcParse::GetOperatorRank(Operators.read(i)) >= MinusCount) {
                        PlusCount = i;
                        MinusCount = TCalcParse::GetOperatorRank(Operators.read(i));
                    }
                }
            }
        }
        return static_cast<pas::WideString>(Operators.read(PlusCount));
    }

    // Square brackets do not nest. An unmatched opening bracket silently discards the remaining suffix.
    pas::WideString TCalcParse::NormalizeFragments(const pas::WideString& Text) {
        pas::WideString Fragment{};
        pas::WideString Output{};
        std::int32_t Index = 1;
        std::int32_t Count = Text.length();
        std::uint8_t Outside = true;
        while (Index <= Count) {
            if (Outside) {
                if (Text.read(Index) == u'[') {
                    Output = pas::concat_wide({Output, NormalizeScalarFragment(Fragment)});
                    Fragment = u"["_w;
                    Outside = false;
                    ++Index;
                    continue;
                } else if (Text.read(Index) != u'[') {
                    Fragment = pas::concat_wide({Fragment, Text.read(Index)});
                    ++Index;
                    if (Index > Count) {
                        Output = pas::concat_wide({Output, NormalizeScalarFragment(Fragment)});
                    }
                    continue;
                }
            }
            if (!Outside) {
                if (Index > Count || Text.read(Index) == u']') {
                    Output = pas::concat_wide({Output, NormalizeBracketFragment(pas::concat_wide({Fragment, u"]"}))});
                    Fragment = pas::WideString();
                    Outside = true;
                } else {
                    Fragment = pas::concat_wide({Fragment, Text.read(Index)});
                }
                ++Index;
            }
        }
        return Output;
    }

    // Silently discards unsupported characters, including decimal dots; call NormalizeTokens first.
    pas::WideString TCalcParse::NormalizeScalarFragment(pas::WideString Text) {
        pas::WideString Previous{};
        pas::WideString Working{};
        pas::WideString Output{};
        std::int32_t i{};
        std::int32_t Count = Text.length();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            {
                char16_t cpp_case = Text.read(i);
                if (cpp_case == u'^') {
                } else if (cpp_case == u'+') {
                } else if (cpp_case == u'-') {
                } else if (cpp_case == u'*') {
                } else if (cpp_case == u'/') {
                } else if (cpp_case == u'#') {
                } else if (cpp_case == u'%') {
                } else if (cpp_case == u'$') {
                } else if (cpp_case == u'c') {
                } else if (cpp_case == u'b') {
                } else if (cpp_case == u'e') {
                } else if (cpp_case == u'f') {
                } else if (cpp_case == u'g') {
                } else if (cpp_case == u'=') {
                } else if (cpp_case == u'>') {
                } else if (cpp_case == u'<') {
                } else if (cpp_case == u'&') {
                } else if (cpp_case == u'|') {
                } else if (cpp_case >= u'0' && cpp_case <= u'9') {
                } else if (cpp_case == u',') {
                } else if (cpp_case == u'(') {
                } else if (cpp_case == u')') {
                } else if (cpp_case == u' ') {
                } else {
                    continue;
                }
            }
            Previous = pas::concat_wide({Previous, Text.read(i)});
        }
        Text = Previous;
        do {
            Previous = Text;
            Working = Text;
            do {
                Text = Working;
                Working = EC_Str::ReplaceAllWideString(Working, u")("_wref.get(), u")*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"."_wref.get(), u","sv);
                Working = EC_Str::ReplaceAllWideString(Working, u",,"_wref.get(), u","sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(,"_wref.get(), u"(0,"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"),"_wref.get(), u")*0,"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")0"_wref.get(), u")*0"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")1"_wref.get(), u")*1"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")2"_wref.get(), u")*2"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")3"_wref.get(), u")*3"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")4"_wref.get(), u")*4"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")5"_wref.get(), u")*5"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")6"_wref.get(), u")*6"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")7"_wref.get(), u")*7"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")8"_wref.get(), u")*8"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")9"_wref.get(), u")*9"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u",("_wref.get(), u",*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"0("_wref.get(), u"0*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"1("_wref.get(), u"1*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"2("_wref.get(), u"2*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"3("_wref.get(), u"3*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"4("_wref.get(), u"4*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"5("_wref.get(), u"5*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"6("_wref.get(), u"6*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"7("_wref.get(), u"7*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"8("_wref.get(), u"8*("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"9("_wref.get(), u"9*("sv);
            } while (!(Text == Working));
            Count = Text.length();
            Working = pas::WideString();
            Output = pas::WideString();
            i = 1;
            while (i <= Count) {
                if (TCalcParse::GetOperatorRank(Text.read(i)) > 0 && i <= Count) {
                    Working = pas::WideString();
                    while (TCalcParse::GetOperatorRank(Text.read(i)) > 0 && i <= Count) {
                        Working = pas::concat_wide({Working, Text.read(i)});
                        ++i;
                    }
                    Output = pas::concat_wide({Output, CollapseOperatorRun(Working)});
                }
                if (TCalcParse::GetOperatorRank(Text.read(i)) < 0) {
                    Working = pas::WideString();
                    while (TCalcParse::GetOperatorRank(Text.read(i)) < 0 && i <= Count) {
                        Working = pas::concat_wide({Working, Text.read(i)});
                        ++i;
                    }
                    Output = pas::concat_wide({Output, Working});
                }
            }
            Text = Output;
            Working = Text;
            do {
                Text = Working;
                Working = EC_Str::ReplaceAllWideString(Working, u"(+"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(*"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(/"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(&"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(|"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(#"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"($"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(%"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(c"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(b"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(e"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(f"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(g"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(<"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(>"_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"(="_wref.get(), u"("sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"-)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"+)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"*)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"/)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"&)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"%)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"|)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"$)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"#)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"c)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"b)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"e)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"f)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"g)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u">)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"<)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u"=)"_wref.get(), u")"sv);
                Working = EC_Str::ReplaceAllWideString(Working, u")("_wref.get(), u")*("sv);
            } while (!(Text == Working));
        } while (!(Previous == Text));
        return Text;
    }

    // Any lowercase p selects parameter parsing, even outside the [pN] form.
    pas::WideString TCalcParse::NormalizeBracketFragment(pas::WideString Text) {
        if (EC_Str::ReplaceAllWideString(Text, u"p"_wref.get(), u""sv) != Text) {
            return NormalizeParameterReference(Text);
        }
        return NormalizeRangeLiteral(Text);
    }

    // Uses only the first three digits; zero/missing digits produce [err]. Does not check parameter-list bounds.
    pas::WideString TCalcParse::NormalizeParameterReference(pas::WideString Text) {
        pas::WideString Result{};
        std::int32_t i{};
        pas::WideString Digits{};
        std::int32_t Count = Text.length();
        Digits = pas::WideString();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            if (Digits.length() > 2) {
                break;
            }
            if (Text.read(i) >= u'0' && Text.read(i) <= u'9') {
                Digits = pas::concat_wide({Digits, Text.read(i)});
            }
        }
        i = EC_Str::ExtractDigitsToIntW(pas::view(pas::concat_wide({u"0", Digits})));
        if (i > 0) {
            return pas::concat_wide({u"[p", EC_Str::IntToWideString(i), u"]"});
        }
        Result = u"[err]"_w;
        InvalidParameterReference = true;
        HasError = true;
        return Result;
    }

    // Requires internal h notation, not '..'. Empty or rejected input yields [err]; existing errors remain set.
    pas::WideString TCalcParse::NormalizeRangeLiteral(pas::WideString Text) {
        pas::WideString Result{};
        std::int32_t i{};
        pas::WideString Clean{};
        CPDiapClass::TCPDiapazone* Range{};
        Clean = pas::WideString();
        std::int32_t Count = Text.length();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            {
                char16_t cpp_case = Text.read(i);
                if (cpp_case == u'[' || cpp_case == u']') {
                    continue;
                } else if (cpp_case >= u'0' && cpp_case <= u'9' || cpp_case == u'-' || cpp_case == u'h' || cpp_case == u';') {
                } else {
                    Result = u"[err]"_w;
                    InvalidRangeLiteral = true;
                    HasError = true;
                    return Result;
                }
            }
            Clean = pas::concat_wide({Clean, Text.read(i)});
        }
        Text = pas::concat_wide({u";", Clean, u";"});
        Clean = Text;
        do {
            Text = Clean;
            Clean = EC_Str::ReplaceAllWideString(Clean, u"--"_wref.get(), u""sv);
            Clean = EC_Str::ReplaceAllWideString(Clean, u";;"_wref.get(), u";"sv);
            Clean = EC_Str::ReplaceAllWideString(Clean, u"h;"_wref.get(), u";"sv);
            Clean = EC_Str::ReplaceAllWideString(Clean, u";h"_wref.get(), u";"sv);
            Clean = EC_Str::ReplaceAllWideString(Clean, u"-;"_wref.get(), u";"sv);
            Clean = EC_Str::ReplaceAllWideString(Clean, u"-h"_wref.get(), u"h"sv);
            Clean = EC_Str::ReplaceAllWideString(Clean, u"hh"_wref.get(), u"h"sv);
        } while (!(Text == Clean));
        if (Clean != u";" && Text.length() > 0) {
            Text.write(1) = u'[';
            Text.write(Text.length()) = u']';
            Range = pas::construct_call<CPDiapClass::TCPDiapazone>(CPDiapClass::TCPDiapazone_Create);
            Range->LoadFromText(Text);
            Text = Range->ToText();
            pas::destroy(Range);
            return Text;
        }
        Result = u"[err]"_w;
        InvalidRangeLiteral = true;
        HasError = true;
        return Result;
    }

    pas::WideString TCalcParse::InsertImplicitMultiplication(pas::WideString Text) {
        pas::WideString Current{};
        Current = Text;
        do {
            Text = Current;
            Current = EC_Str::ReplaceAllWideString(Current, u"-,"_wref.get(), u"-0,"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u")["_wref.get(), u")*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]("_wref.get(), u"]*("sv);
            Current = EC_Str::ReplaceAllWideString(Current, u")("_wref.get(), u")*("sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]["_wref.get(), u"]*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"],"_wref.get(), u"]*0,"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]0"_wref.get(), u"]*0"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]1"_wref.get(), u"]*1"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]2"_wref.get(), u"]*2"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]3"_wref.get(), u"]*3"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]4"_wref.get(), u"]*4"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]5"_wref.get(), u"]*5"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]6"_wref.get(), u"]*6"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]7"_wref.get(), u"]*7"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]8"_wref.get(), u"]*8"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"]9"_wref.get(), u"]*9"sv);
            Current = EC_Str::ReplaceAllWideString(Current, u",["_wref.get(), u",*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"0["_wref.get(), u"0*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"1["_wref.get(), u"1*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"2["_wref.get(), u"2*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"3["_wref.get(), u"3*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"4["_wref.get(), u"4*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"5["_wref.get(), u"5*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"6["_wref.get(), u"6*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"7["_wref.get(), u"7*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"8["_wref.get(), u"8*["sv);
            Current = EC_Str::ReplaceAllWideString(Current, u"9["_wref.get(), u"9*["sv);
        } while (!(Text == Current));
        return Text;
    }

    // One-based; zero when absent. Rightmost ties give left associativity. Delimiter balance is unchecked.
    std::int32_t TCalcParse::FindTopLevelOperator(const std::u16string_view& Text, std::int32_t TextLength) {
        std::int32_t Rank{};
        std::int32_t i{};
        std::int32_t BestRank = 0;
        std::int32_t BestIndex = 0;
        std::int32_t BracketDepth = 0;
        std::int32_t ParenthesisDepth = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            if (Text[i - 1] == u'(') {
                ++ParenthesisDepth;
            }
            if (Text[i - 1] == u'[') {
                ++BracketDepth;
            }
            if (Text[i - 1] == u')') {
                --ParenthesisDepth;
            }
            if (Text[i - 1] == u']') {
                --BracketDepth;
            }
            if (ParenthesisDepth == 0 && BracketDepth == 0) {
                Rank = TCalcParse::GetOperatorRank(Text[i - 1]);
                if (BestRank <= Rank) {
                    BestRank = Rank;
                    BestIndex = i;
                }
            }
        }
        return BestIndex;
    }

    // Caller owns the result. Evaluates right before left without short-circuiting. EvaluationError blocks evaluation; HasError alone does not. Native recursive intermediates leak.
    CPVarClass::TCPVariant* TCalcParse::EvaluateExpression(pas::WideString Text) {
        std::int32_t Count{};
        pas::WideString Inner{};
        pas::WideString LeftText{};
        pas::WideString RightText{};
        std::int32_t i{};
        std::int32_t Index{};
        CPVarClass::TCPVariant* Left{};
        CPVarClass::TCPVariant* Right{};
        CPVarClass::TCPVariant* Value{};
        Value = pas::construct_call<CPVarClass::TCPVariant>(CPVarClass::TCPVariant_Create);
        Left = pas::construct_call<CPVarClass::TCPVariant>(CPVarClass::TCPVariant_Create);
        Right = pas::construct_call<CPVarClass::TCPVariant>(CPVarClass::TCPVariant_Create);
        if (!EvaluationError) {
            Count = Text.length();
            if (!Value->TryLoadFromText(Text)) {
                if (Text.read(1) == u'(' && Text.read(Count) == u')' && TCalcParse::HasBalancedParenthesesInSlice(pas::view(Text), 2, Count - 1)) {
                    Inner = pas::WideString();
                    for (auto cpp_range = pas::for_to<std::int32_t>(2, Count - 1); cpp_range.next(i); ) {
                        Inner = pas::concat_wide({Inner, Text.read(i)});
                    }
                    if (Inner.length() == 0) {
                        HasError = true;
                    } else {
                        CPVarClass::TCPVariant* evaluateExpression = EvaluateExpression(Inner);
                        CPVarClass::TCPVariant* value = Value;
                        value->Assign(evaluateExpression, false);
                    }
                } else {
                    Index = FindTopLevelOperator(pas::view(Text), Count);
                    if (Index < 1) {
                        EvaluationError = true;
                    } else {
                        LeftText = pas::WideString();
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Index - 1); cpp_range_2.next(i); ) {
                            LeftText = pas::concat_wide({LeftText, Text.read(i)});
                        }
                        RightText = pas::WideString();
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(Index + 1, Count); cpp_range_3.next(i); ) {
                            RightText = pas::concat_wide({RightText, Text.read(i)});
                        }
                        {
                            CPVarClass::TCPVariant* evaluateExpression_2 = EvaluateExpression(RightText);
                            CPVarClass::TCPVariant* right = Right;
                            right->Assign(evaluateExpression_2, false);
                        }
                        if (!EvaluationError) {
                            {
                                CPVarClass::TCPVariant* evaluateExpression_3 = EvaluateExpression(LeftText);
                                CPVarClass::TCPVariant* left = Left;
                                left->Assign(evaluateExpression_3, false);
                            }
                            if (!EvaluationError) {
                                try {
                                    if (Text.read(Index) == u'^') {
                                        TCalcParse::ApplyPower(Left, Right, Value);
                                    } else if (Text.read(Index) == u'+') {
                                        TCalcParse::ApplyAdd(Left, Right, Value);
                                    } else if (Text.read(Index) == u'-') {
                                        TCalcParse::ApplySubtract(Left, Right, Value);
                                    } else if (Text.read(Index) == u'*') {
                                        TCalcParse::ApplyMultiply(Left, Right, Value);
                                    } else if (Text.read(Index) == u'/') {
                                        TCalcParse::ApplyDivide(Left, Right, Value);
                                    } else if (Text.read(Index) == u'f') {
                                        TCalcParse::ApplyIntDivide(Left, Right, Value);
                                    } else if (Text.read(Index) == u'g') {
                                        TCalcParse::ApplyModulo(Left, Right, Value);
                                    } else if (Text.read(Index) == u'%') {
                                        TCalcParse::ApplyPercentChange(Left, Right, Value);
                                    } else if (Text.read(Index) == u'$') {
                                        TCalcParse::ApplyRange(Left, Right, Value);
                                    } else if (Text.read(Index) == u'#') {
                                        TCalcParse::ApplyMembership(Left, Right, Value);
                                    } else if (Text.read(Index) == u'>') {
                                        TCalcParse::ApplyGreaterThan(Left, Right, Value);
                                    } else if (Text.read(Index) == u'<') {
                                        TCalcParse::ApplyLessThan(Left, Right, Value);
                                    } else if (Text.read(Index) == u'c') {
                                        TCalcParse::ApplyGreaterOrEqual(Left, Right, Value);
                                    } else if (Text.read(Index) == u'b') {
                                        TCalcParse::ApplyLessOrEqual(Left, Right, Value);
                                    } else if (Text.read(Index) == u'e') {
                                        TCalcParse::ApplyNotEqual(Left, Right, Value);
                                    } else if (Text.read(Index) == u'=') {
                                        TCalcParse::ApplyEqual(Left, Right, Value);
                                    } else if (Text.read(Index) == u'&') {
                                        TCalcParse::ApplyAnd(Left, Right, Value);
                                    } else if (Text.read(Index) == u'|') {
                                        TCalcParse::ApplyOr(Left, Right, Value);
                                    }
                                } catch (...) {
                                    auto cpp_exception = pas::caught_object();
                                    if (pas::class_cast_if<pas::MathError*>(cpp_exception)) {
                                        EvaluationError = true;
                                        HasError = true;
                                    } else if (pas::class_cast_if<pas::InvalidOp*>(cpp_exception)) {
                                        EvaluationError = true;
                                        HasError = true;
                                    } else if (pas::class_cast_if<pas::Overflow*>(cpp_exception)) {
                                        EvaluationError = true;
                                        HasError = true;
                                    } else if (pas::class_cast_if<pas::ZeroDivide*>(cpp_exception)) {
                                        EvaluationError = true;
                                        HasError = true;
                                    } else {
                                        throw;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        CPVarClass::TCPVariant* Result = pas::construct_call<CPVarClass::TCPVariant>(CPVarClass::TCPVariant_Create);
        Result->Assign(Value, false);
        pas::destroy(Value);
        pas::destroy(Right);
        pas::destroy(Left);
        return Result;
    }

    // Existing HasError preserves ResultValue; flags are not reset. Native scratch and returned variants leak.
    void TCalcParse::Evaluate(pas::List* Parameters) {
        CPVarClass::TCPVariant* Value = pas::construct_call<CPVarClass::TCPVariant>(CPVarClass::TCPVariant_Create);
        if (!HasError) {
            Value->Assign(EvaluateExpression(pas::concat_wide({u"(", SubstituteParameters(Parameters), u")"})), false);
            try {
                ResultValue = Value->AsInteger();
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::class_cast_if<pas::InvalidOp*>(cpp_exception)) {
                    EvaluationError = true;
                    HasError = true;
                    ResultValue = 0;
                } else {
                    throw;
                }
            }
            if (EvaluationError) {
                HasError = true;
            }
        }
    }

    // Resets state; stores Expression even on error. Empty input becomes (), not the default parameter.
    void TCalcParse::Prepare(pas::WideString Text, std::int32_t DefaultParameterIndex) {
        std::int32_t Count{};
        std::int32_t i{};
        pas::WideString Readable{};
        Reset();
        SourceText = Text;
        Text = TCalcParse::NormalizeTokens(Text);
        Text = NormalizeFragments(Text);
        Text = TCalcParse::InsertImplicitMultiplication(Text);
        Text = ClampNumericLiterals(Text);
        UnbalancedParentheses = static_cast<std::uint8_t>(HasBalancedParentheses(pas::view(Text)) ^ 1);
        if (UnbalancedParentheses) {
            HasError = true;
        }
        Readable = Text;
        if (!HasError) {
            Count = Text.length();
            if (Count >= 2 && Text.read(1) == u'(' && Text.read(Count) == u')' && TCalcParse::HasBalancedParenthesesInSlice(pas::view(Text), 2, Count - 1)) {
                Readable = pas::WideString();
                for (auto cpp_range = pas::for_to<std::int32_t>(2, Count - 1); cpp_range.next(i); ) {
                    Readable = pas::concat_wide({Readable, Text.read(i)});
                }
            }
        }
        Readable = FormatTokens(Readable);
        if (SourceText != Readable) {
            SourceWasChanged = true;
        }
        if (Text == u"" || Text == pas::concat_wide({u"[p", EC_Str::IntToWideString(DefaultParameterIndex), u"]"})) {
            UsesDefaultParameter = true;
            Text = pas::concat_wide({u"[p", EC_Str::IntToWideString(DefaultParameterIndex), u"]"});
        }
        Expression = Text;
    }

    void TCalcParse_Create(TCalcParse* Self) {
        Self->Reset();
    }

    void TCalcParse::Reset() {
        SourceText = pas::WideString();
        Expression = pas::WideString();
        ResultValue = 0;
        ResetValue10 = 0;
        SourceWasChanged = false;
        UnbalancedParentheses = false;
        InvalidNumericLiteral = false;
        InvalidParameterReference = false;
        InvalidRangeLiteral = false;
        EvaluationError = false;
        UsesDefaultParameter = false;
        HasError = false;
    }

    // One-based inclusive bounds, unchecked. Empty slices pass; square brackets are ignored.
    std::uint8_t TCalcParse::HasBalancedParenthesesInSlice(const std::u16string_view& Text, std::int32_t FirstIndex, std::int32_t LastIndex) {
        std::int32_t i{};
        std::int32_t Depth = 0;
        std::uint8_t Balanced = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(FirstIndex, LastIndex); cpp_range.next(i); ) {
            if (Text[i - 1] == u'(') {
                ++Depth;
            }
            if (Text[i - 1] == u')') {
                --Depth;
            }
            if (Depth < 0) {
                Balanced = false;
                break;
            }
        }
        if (Depth != 0) {
            Balanced = false;
        }
        return Balanced;
    }

    // Parameters: borrowed, non-nil TList of TParameter; [pN] is one-based.
    // Unmatched references remain unchanged; negative values are parenthesized.
    pas::WideString TCalcParse::SubstituteParameters(pas::List* Parameters) {
        std::int32_t i{};
        pas::WideString Text{};
        ParameterClass::TParameter* Parameter{};
        Text = Expression;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Parameters)); cpp_range.next(i); ) {
            Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, i - 1);
            if (Parameter->Value < 0) {
                Text = ([&] {
                    const pas::WideString& cpp_arg = pas::concat_wide({u"(0", EC_Str::IntToWideString(Parameter->Value), u")"});
                    const pas::WideString& cpp_arg_2 = pas::concat_wide({u"[p", EC_Str::IntToWideString(i), u"]"});
                    return EC_Str::ReplaceAllWideString(Text, cpp_arg_2, pas::view(cpp_arg));
                }());
            } else {
                Text = ([&] {
                    const pas::WideString& intToWideString = EC_Str::IntToWideString(Parameter->Value);
                    const pas::WideString& cpp_arg_3 = pas::concat_wide({u"[p", EC_Str::IntToWideString(i), u"]"});
                    return EC_Str::ReplaceAllWideString(Text, cpp_arg_3, pas::view(intToWideString));
                }());
            }
        }
        return Text;
    }

    // Empty text passes.
    std::uint8_t TCalcParse::HasBalancedParentheses(const std::u16string_view& Text) {
        std::uint8_t Balanced = TCalcParse::HasBalancedParenthesesInSlice(Text, 1, static_cast<std::int32_t>(Text.length()));
        return Balanced;
    }

    // Nonzero limits: 0.0001..999999999. The lower clamp emits a dot-decimal literal that evaluation rejects. Drops trailing numbers; conversion errors set flags and leave the caller's result storage unchanged.
    pas::WideString TCalcParse::ClampNumericLiterals(pas::WideString Text) {
        pas::WideString Result{};
        pas::Extended Value{};
        pas::WideString Output{};
        pas::AnsiString Digits{};
        pas::AnsiString Replacement{};
        std::uint8_t SavedSeparator{};
        std::int32_t Index = 1;
        std::int32_t Count = Text.length();
        Value = 0.0L;
        while (Index <= Count) {
            if (Text.read(Index) >= u'0' && Text.read(Index) <= u'9' || Text.read(Index) == u',') {
                Digits = static_cast<pas::AnsiString>(pas::concat_wide_reverse({Text.read(Index), static_cast<pas::WideString>(Digits)}));
            } else if (Digits != "") {
                SavedSeparator = SysUtils::DecimalSeparator;
                try {
                    SysUtils::DecimalSeparator = ',';
                    Value = pas::str_to_float(Digits, SysUtils::DecimalSeparator);
                    SysUtils::DecimalSeparator = SavedSeparator;
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::class_cast_if<pas::ConvertError*>(cpp_exception)) {
                        HasError = true;
                        InvalidNumericLiteral = true;
                        SysUtils::DecimalSeparator = SavedSeparator;
                        return Result;
                    } else {
                        throw;
                    }
                }
                if (Value > 999999999.0L) {
                    Replacement = "999999999"_a;
                } else if (Value < 1.0E-4L && Value != 0.0L) {
                    Replacement = "0.0001"_a;
                } else {
                    Replacement = Digits;
                }
                Output = pas::concat_wide({Output, static_cast<pas::WideString>(Replacement), Text.read(Index)});
                Digits = pas::AnsiString();
            } else {
                Output = pas::concat_wide({Output, Text.read(Index)});
            }
            ++Index;
        }
        return Output;
    }

} // namespace CalcParseClass
