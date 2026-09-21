#include "layout/EC_Expression.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Expression.hpp"
#include "units/Math.hpp"
#include "units/ScriptPorts.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_Expression {
    // Nested in ResizeScriptArray; collects dimensions by following each first child.
    void CollectScriptArrayDimensions(TVarArrayEC* Values, pas::DynArray<std::int32_t>& Dimensions);

    void AddScriptLocal(const std::u16string_view& TypeName, pas::WideString Name, TCodeEC* Self);

    pas::Array<EC_Expression::TVarEC*, 0, 19> ScriptCallTrace{};

    std::int32_t ScriptCallTracePosition = 0;

    std::int32_t ScriptCallTraceCount = 0;

    const pas::Array<char16_t, 0, 15> ScriptHexDigits = pas::Array<char16_t, 0, 15>{{
        u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7',
        u'8', u'9', u'a', u'b', u'c', u'd', u'e', u'f',
    }};

    std::int32_t ScriptStepInterval = 0;

    EC_Expression::TScriptStepCallback ScriptStepCallback = nullptr;

    // Callback is a Delphi register procedure taking the cumulative statement count.
    void SetScriptStepCallback(TScriptStepCallback Callback, std::int32_t Interval) {
        ScriptStepCallback = Callback;
        ScriptStepInterval = Interval;
    }

    // Requires an acyclic ownership tree.
    void FreeScriptArrayTree(TVarArrayEC* Values) {
        std::int32_t i{};
        TVarEC* Item{};
        std::int32_t Count = Values->Count;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Item = EC_Expression::TVarArrayEC_GetItem(Values, i);
            if (Item->Kind == vkArray && Item->GetArray() != nullptr) {
                EC_Expression::FreeScriptArrayTree(Item->GetArray());
                Item->SetArray(nullptr);
            }
        }
        pas::free(Values);
    }

    // Does not shrink or resize existing children.
    void GrowScriptArray(TVarArrayEC* Values, pas::OpenArray<std::int32_t> Dimensions, std::int32_t DimensionIndex) {
        auto cpp_array_copy = pas::copy_open_array(Dimensions);
        Dimensions = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        TVarEC* Item{};
        std::int32_t Count = Dimensions[DimensionIndex];
        if (Dimensions.length() - 1 == DimensionIndex) {
            for (auto cpp_range = pas::for_to<std::int32_t>(Values->Count, Count - 1); cpp_range.next(i); ) {
                Values->Add(u""_wref.get(), vkEmpty);
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(Values->Count, Count - 1); cpp_range_2.next(i); ) {
                Item = Values->Add(u""_wref.get(), vkArray);
                Item->SetArray(pas::construct_call<TVarArrayEC>(TVarArrayEC_Create));
                EC_Expression::GrowScriptArray(Item->GetArray(), Dimensions, DimensionIndex + 1);
            }
        }
    }

    // Only the outer dimension changes; new children inherit the first child's dimensions.
    void ResizeScriptArray(TVarArrayEC* Values, std::int32_t Count) {
        std::int32_t i{};
        TVarEC* Item{};
        pas::DynArray<std::int32_t> Dimensions{};
        std::int32_t OldCount = Values->Count;
        if (Count == OldCount) {
            return;
        }
        if (Count < OldCount) {
            for (auto cpp_range = pas::for_to<std::int32_t>(Count, OldCount - 1); cpp_range.next(i); ) {
                Item = EC_Expression::TVarArrayEC_GetItem(Values, i);
                if (Item->Kind == vkArray && Item->GetArray() != nullptr) {
                    EC_Expression::FreeScriptArrayTree(Item->GetArray());
                }
            }
            for (auto cpp_range_2 = pas::for_downto<std::int32_t>(OldCount - 1, Count); cpp_range_2.next(i); ) {
                Values->Delete(i);
            }
        } else {
            Dimensions = nullptr;
            EC_Expression::CollectScriptArrayDimensions(Values, Dimensions);
            Dimensions[0] = Count;
            EC_Expression::GrowScriptArray(Values, pas::open_array(Dimensions), 0);
        }
    }

    void RegisterExpressionBuiltins(TVarArrayEC* Scope) {
        Scope->Add(u"pi"_wref.get(), vkFloat)->SetFloat(pas::constant(static_cast<double>(SystemImports::Pi)));
        Scope->Add(u"min"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Min))));
        Scope->Add(u"max"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Max))));
        Scope->Add(u"newarray"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_NewArray))));
        Scope->Add(u"arraychange"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_ArrayChange))));
        Scope->Add(u"free"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Free))));
        Scope->Add(u"count"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Count))));
        Scope->Add(u"copy"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Copy))));
        Scope->Add(u"abs"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Abs))));
        Scope->Add(u"arctan"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_ArcTan))));
        Scope->Add(u"exp"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Exp))));
        Scope->Add(u"ln"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Ln))));
        Scope->Add(u"round"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Round))));
        Scope->Add(u"sin"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Sin))));
        Scope->Add(u"cos"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Cos))));
        Scope->Add(u"sqr"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Sqr))));
        Scope->Add(u"sqrt"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Sqrt))));
        Scope->Add(u"frac"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Frac))));
        Scope->Add(u"int"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Int))));
        Scope->Add(u"ord"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Ord))));
        Scope->Add(u"rnd"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Rnd))));
        Scope->Add(u"randomize"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Randomize))));
        Scope->Add(u"randseed"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_RandSeed))));
        Scope->Add(u"substr"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_SubStr))));
        Scope->Add(u"findsubstr"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_FindSubStr))));
        Scope->Add(u"trim"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Trim))));
        Scope->Add(u"toansi"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_ToAnsi))));
        Scope->Add(u"tounicode"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_ToUnicode))));
        Scope->Add(u"lowercase"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_LowerCase))));
        Scope->Add(u"uppercase"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_UpperCase))));
        Scope->Add(u"loadlibrary"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_LoadLibrary))));
        Scope->Add(u"freelibrary"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_FreeLibrary))));
        Scope->Add(u"libraryfunction"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_LibraryFunction))));
        Scope->Add(u"new"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_New))));
        Scope->Add(u"delete"_wref.get(), vkExternFun)->SetExternFun(reinterpret_cast<void*>(pas::callback_address(pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>(EF_Delete))));
    }

    pas::WideString TrimScriptString(pas::WideString Text) {
        pas::WideString Result{};
        std::int32_t C{};
        std::int32_t Count = Text.length();
        std::int32_t First = 0;
        while (First < Count) {
            C = Text.read(First + 1);
            if (C == ' ' || C == 9 || C == 13 || C == 10 || C == 0) {
                ++First;
            } else {
                break;
            }
        }
        if (First >= Count) {
            return pas::WideString();
        }
        std::int32_t Last = Count - 1;
        while (Last >= 0) {
            C = Text.read(Last + 1);
            if (C == ' ' || C == 9 || C == 13 || C == 10 || C == 0) {
                --Last;
            } else {
                break;
            }
        }
        if (Last < First) {
            return pas::WideString();
        }
        Result.set_length(Last - First + 1);
        return pas::copy(Text, First + 1, Last - First + 1);
    }

    // Collects decimal digits while ignoring other characters; negative only for a leading minus.
    std::int32_t ScriptStringToInt(const std::u16string_view& Text) {
        std::int32_t i{};
        std::int32_t Result = 0;
        std::int32_t Count = static_cast<std::int32_t>(Text.length());
        std::int32_t Sign = 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            if (Text[i - 1] >= '0' && Text[i - 1] <= '9') {
                Result = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text[i - 1])) + Result * 10;
            } else if (Text[i - 1] == u'-' && i == 1) {
                Sign *= -1;
            }
        }
        return Sign * Result;
    }

    // Uses a dot decimal separator.
    pas::WideString ScriptFloatToString(double Value) {
        pas::WideString Result{};
        std::uint8_t SavedSeparator = SysUtils::DecimalSeparator;
        SysUtils::DecimalSeparator = '.';
        Result = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Value), SysUtils::DecimalSeparator));
        SysUtils::DecimalSeparator = SavedSeparator;
        return Result;
    }

    pas::WideString ScriptDwordToHex(std::uint32_t Value) {
        pas::WideString Result{};
        while (Value != 0) {
            Result = pas::concat_wide({ScriptHexDigits[Value - (Value >> 4 << 4)], Result});
            Value = Value / 16;
        }
        if (Result == u"") {
            return u"0"_w;
        }
        return Result;
    }

    // Ignores nonnumeric characters; not a strict literal validator.
    double ScriptStringToFloat(const std::u16string_view& Text) {
        std::int32_t i{};
        std::int32_t C{};
        std::int32_t Count = static_cast<std::int32_t>(Text.length());
        if (Count < 1) {
            return 0.0;
        }
        double Value = 0.0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            C = Text[i + 1 - 1];
            if (C >= '0' && C <= '9') {
                Value = Value * 1.0E+1L + (C - '0');
            } else if (C == '.') {
                break;
            }
        }
        ++i;
        double Divisor = 1.0E+1;
        while (i < Count) {
            C = Text[i + 1 - 1];
            if (C >= '0' && C <= '9') {
                Value = pas::real_divide(C - '0', Divisor) + Value;
                Divisor = Divisor * 1.0E+1L;
            }
            ++i;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
            if (Text[i + 1 - 1] == '-') {
                Value = -Value;
                break;
            }
        }
        return Value;
    }

    // Also accepts empty text and a lone minus.
    std::uint8_t IsScriptIntegerText(const std::u16string_view& Text) {
        std::int32_t i{};
        std::int32_t Count = static_cast<std::int32_t>(Text.length());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            if ((Text[i + 1 - 1] < u'0' || Text[i + 1 - 1] > u'9') && (Text[i + 1 - 1] != u'-' || i > 0)) {
                return false;
            }
        }
        return true;
    }

    std::uint8_t IsNonIntegerScriptText(const std::u16string_view& Text) {
        return static_cast<std::uint8_t>(EC_Expression::IsScriptIntegerText(Text) ^ 1);
    }

    // Requires a decimal point and fractional digits; supports an exponent suffix. Token advances only on success; Value may change on failure.
    std::uint8_t TryReadFloatLiteral(TCodeAnalyzerUnitEC*& Token, double& Value) {
        char16_t C{};
        pas::WideString ExponentText{};
        TCodeAnalyzerUnitEC* Current = Token;
        std::uint8_t Result = false;
        double Sign = 1.0;
        if (Current == nullptr) {
            return Result;
        }
        if (Current->TokenKind == ctSubtract) {
            Sign = -1.0;
            Current = Current->Next;
            if (Current == nullptr) {
                return Result;
            }
        }
        if (Current->TokenKind != ctText) {
            return Result;
        }
        if (!EC_Expression::IsScriptIntegerText(pas::view(Current->Text))) {
            return Result;
        }
        Value = EC_Expression::ScriptStringToInt(pas::view(Current->Text));
        Current = Current->Next;
        if (Current == nullptr) {
            return Result;
        }
        if (Current->TokenKind != ctDot) {
            return Result;
        }
        Current = Current->Next;
        if (Current == nullptr) {
            return Result;
        }
        if (Current->TokenKind != ctText) {
            return Result;
        }
        std::int32_t Count = Current->Text.length();
        double Fraction = 0.0;
        std::int32_t i = 0;
        while (i < Count) {
            C = Current->Text.read(i + 1);
            if (C >= u'0' && C <= u'9') {
                Fraction = Fraction * 1.0E+1L + (C - '0');
            } else if (C == u'e' || C == u'E') {
                break;
            } else {
                return Result;
            }
            ++i;
        }
        if (i < 1) {
            return Result;
        }
        Value = pas::real_divide(Fraction, Math::Power(1.0E+1L, i)) + Value;
        double Exponent = 0.0;
        if (Count - 1 > i) {
            ExponentText = pas::copy(Current->Text, i + 2, Count - i - 1);
            if (!EC_Expression::IsScriptIntegerText(pas::view(ExponentText))) {
                return Result;
            }
            Exponent = EC_Expression::ScriptStringToInt(pas::view(ExponentText));
            Current = Current->Next;
        } else if (Count - 1 == i) {
            Current = Current->Next;
            if (Current == nullptr) {
                return Result;
            }
            if (Current->Next == nullptr) {
                return Result;
            }
            if (Current->Next->TokenKind != ctText) {
                return Result;
            }
            if (!EC_Expression::IsScriptIntegerText(pas::view(Current->Next->Text))) {
                return Result;
            }
            if (Current->TokenKind == ctSubtract) {
                Exponent = -EC_Expression::ScriptStringToInt(pas::view(Current->Next->Text));
            } else if (Current->TokenKind == ctAdd) {
                Exponent = EC_Expression::ScriptStringToInt(pas::view(Current->Next->Text));
            } else {
                return Result;
            }
            Current = Current->Next->Next;
        } else {
            Current = Current->Next;
        }
        if (Exponent > 0.0L) {
            Value = Math::Power(1.0E+1L, Exponent) * Value;
        } else if (Exponent < 0.0L) {
            Value = pas::real_divide(Value, Math::Power(1.0E+1L, -Exponent));
        }
        Value = static_cast<long double>(Value) * Sign;
        Token = Current;
        return true;
    }

    std::uint8_t TryReadIntegerLiteral(TCodeAnalyzerUnitEC*& Token, std::int32_t& Value) {
        TCodeAnalyzerUnitEC* Current = Token;
        std::uint8_t Result = false;
        std::int32_t Sign = 1;
        if (Current == nullptr) {
            return Result;
        }
        if (Current->TokenKind == ctSubtract) {
            Sign = -1;
            Current = Current->Next;
            if (Current == nullptr) {
                return Result;
            }
        }
        if (Current->TokenKind != ctText) {
            return Result;
        }
        if (!EC_Expression::IsScriptIntegerText(pas::view(Current->Text))) {
            return Result;
        }
        Value = EC_Expression::ScriptStringToInt(pas::view(Current->Text));
        Current = Current->Next;
        Value = Sign * Value;
        Token = Current;
        return true;
    }

    std::uint8_t TryReadStringLiteral(TCodeAnalyzerUnitEC*& Token, pas::WideString& Value) {
        TCodeAnalyzerUnitEC* Current = Token;
        std::uint8_t Result = false;
        if (Current == nullptr) {
            return Result;
        }
        if (Current->TokenKind != ctStringLiteral) {
            return Result;
        }
        Value = Current->Text;
        Current = Current->Next;
        Token = Current;
        return true;
    }

    // Reads h/H hexadecimal and b/B binary suffixes. Token advances only on success; Value may change on failure.
    std::uint8_t TryReadDwordLiteral(TCodeAnalyzerUnitEC*& Token, std::uint32_t& Value) {
        std::int32_t i{};
        Value = 0u;
        TCodeAnalyzerUnitEC* Current = Token;
        std::uint8_t Result = false;
        if (Current == nullptr) {
            return Result;
        }
        if (Current->TokenKind != ctText) {
            return Result;
        }
        std::int32_t Count = Current->Text.length();
        if (Count < 2) {
            return Result;
        }
        char16_t C = Current->Text.read(Count);
        if (C == u'h' || C == u'H') {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 2); cpp_range.next(i); ) {
                C = Current->Text.read(i + 1);
                if (C >= u'0' && C <= u'9') {
                    Value = Value * 16 + static_cast<std::uint32_t>(C - '0');
                } else if (C >= u'a' && C <= u'f') {
                    Value = Value * 16 + static_cast<std::uint32_t>(C + 10 - 'a');
                } else if (C >= u'A' && C <= u'F') {
                    Value = Value * 16 + static_cast<std::uint32_t>(C + 10 - 'A');
                } else {
                    return Result;
                }
            }
        } else if (C == u'b' || C == u'B') {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 2); cpp_range_2.next(i); ) {
                C = Current->Text.read(i + 1);
                if (C >= u'0' && C <= u'1') {
                    Value = Value * 2 + static_cast<std::uint32_t>(C - '0');
                } else {
                    return Result;
                }
            }
        } else {
            return Result;
        }
        Current = Current->Next;
        Token = Current;
        return true;
    }

    // Requires a nonnil initial Token.
    std::uint8_t TryReadMemberName(TCodeAnalyzerUnitEC*& Token, pas::WideString& Name) {
        Name = pas::WideString();
        while (Token->TokenKind == ctText && EC_Expression::IsNonIntegerScriptText(pas::view(Token->Text))) {
            Name = pas::concat_wide({Name, Token->Text});
            Token = Token->Next;
            if (Token == nullptr || Token->TokenKind != ctDot) {
                break;
            }
            Name = pas::concat_wide({Name, u"."});
            Token = Token->Next;
            if (Token == nullptr || Token->TokenKind != ctText || static_cast<std::uint8_t>(EC_Expression::IsNonIntegerScriptText(pas::view(Token->Text)) ^ 1)) {
                Name = pas::WideString();
                break;
            }
        }
        return Name != u"";
    }

    // Native expression callbacks: av[0] is the script result; code is the caller.
    void EF_Min(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->Assume(av[1], false);
        for (auto cpp_range = pas::for_to<std::int32_t>(2, Count - 1); cpp_range.next(i); ) {
            if (EC_Expression::TVarEC_RealVType(av[0]) == vkString && pas::is_one_of<vkInt, vkDword, vkFloat>(EC_Expression::TVarEC_RealVType(av[i]))) {
                av[0]->ConvertToKind(EC_Expression::TVarEC_RealVType(av[i]));
            } else if (EC_Expression::TVarEC_RealVType(av[i]) == vkFloat) {
                if (pas::is_one_of<vkInt, vkDword>(EC_Expression::TVarEC_RealVType(av[0]))) {
                    av[0]->ConvertToKind(EC_Expression::TVarEC_RealVType(av[i]));
                }
            }
            if (EC_Expression::TVarEC_GreaterThan(av[0], av[i])) {
                av[0]->Assume(av[i], false);
            }
        }
    }

    void EF_Max(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->Assume(av[1], false);
        for (auto cpp_range = pas::for_to<std::int32_t>(2, Count - 1); cpp_range.next(i); ) {
            if (EC_Expression::TVarEC_RealVType(av[0]) == vkString && pas::is_one_of<vkInt, vkDword, vkFloat>(EC_Expression::TVarEC_RealVType(av[i]))) {
                av[0]->ConvertToKind(EC_Expression::TVarEC_RealVType(av[i]));
            } else if (EC_Expression::TVarEC_RealVType(av[i]) == vkFloat) {
                if (pas::is_one_of<vkInt, vkDword>(EC_Expression::TVarEC_RealVType(av[0]))) {
                    av[0]->ConvertToKind(EC_Expression::TVarEC_RealVType(av[i]));
                }
            }
            if (EC_Expression::TVarEC_LessThan(av[0], av[i])) {
                av[0]->Assume(av[i], false);
            }
        }
    }

    void EF_NewArray(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        pas::DynArray<std::int32_t> Dimensions{};
        std::int32_t Count = av.length() - 1 + 1;
        av[0]->ResetKind(vkArray);
        if (Count < 2) {
            return;
        }
        --Count;
        Dimensions.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            if (EC_Expression::TVarEC_RealVType(av[i + 1]) != vkInt || av[i + 1]->GetInt() < 1) {
                Dimensions = nullptr;
                return;
            }
            Dimensions[i] = av[i + 1]->GetInt();
        }
        av[0]->CreateArray(pas::open_array(Dimensions));
        Dimensions = nullptr;
    }

    void EF_ArrayChange(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 4> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 4), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        if (av.length() - 1 < 2) {
            return;
        }
        std::int32_t Dimension = 0;
        if (av.length() - 1 >= 3) {
            Dimension = av[3]->GetInt();
        }
        EC_Expression::TVarEC_ResizeArray(av[1], av[2]->GetInt(), Dimension);
    }

    void EF_Free(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        std::int32_t Count = av.length() - 1 + 1 - 1;
        if (Count < 1) {
            return;
        }
        av[0]->Assume(av[1], false);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            EC_Expression::TVarEC_FreeArray(av[i + 1]);
        }
    }

    void EF_Count(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1 - 1;
        if (Count < 1) {
            return;
        }
        if (EC_Expression::TVarEC_RealVType(av[1]) == vkArray) {
            av[0]->SetInt(av[1]->GetArray()->Count);
        }
        if (EC_Expression::TVarEC_RealVType(av[1]) == vkString) {
            std::int32_t cpp_arg = av[1]->GetString().length();
            TVarEC* cpp_arg_2 = av[0];
            cpp_arg_2->SetInt(cpp_arg);
        }
    }

    void EF_Copy(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 3> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 3), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1 - 1;
        if (Count < 2) {
            return;
        }
        av[1]->ResetKind(EC_Expression::TVarEC_RealVType(av[2]));
        av[1]->Assume(av[2], true);
    }

    void EF_Abs(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        if (EC_Expression::TVarEC_RealVType(av[1]) == vkInt) {
            av[0]->SetInt(pas::abs(av[1]->GetInt()));
        } else {
            av[0]->SetFloat(std::fabs(static_cast<pas::Extended>(av[1]->GetFloat())));
        }
    }

    void EF_ArcTan(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::ArcTan(av[1]->GetFloat()));
    }

    void EF_Exp(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::Exp(av[1]->GetFloat()));
    }

    void EF_Ln(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::Ln(av[1]->GetFloat()));
    }

    void EF_Round(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 3> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 3), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Step{};
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        if (Count >= 3) {
            Step = av[2]->GetInt();
        } else {
            Step = 1;
        }
        if (EC_Expression::TVarEC_RealVType(av[1]) == vkFloat) {
            av[0]->SetInt(static_cast<std::int32_t>(System::Round(pas::real_divide(av[1]->GetFloat(), Step))) * Step);
        } else {
            av[0]->SetInt(static_cast<std::int32_t>(System::Round(pas::real_divide(av[1]->GetInt(), Step))) * Step);
        }
    }

    void EF_Sin(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::Sin(av[1]->GetFloat()));
    }

    void EF_Cos(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::Cos(av[1]->GetFloat()));
    }

    void EF_Sqr(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        if (EC_Expression::TVarEC_RealVType(av[1]) == vkInt) {
            std::int32_t cpp_left = av[1]->GetInt();
            av[0]->SetInt(cpp_left * av[1]->GetInt());
        } else {
            av[0]->SetFloat(pas::sqr(static_cast<pas::Extended>(av[1]->GetFloat())));
        }
    }

    void EF_Sqrt(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::Sqrt(av[1]->GetFloat()));
    }

    void EF_Frac(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetFloat(System::Frac(av[1]->GetFloat()));
    }

    void EF_Int(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        av[0]->SetInt(System::Trunc(av[1]->GetFloat()));
    }

    void EF_Ord(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        pas::WideString Text{};
        if (av.length() - 1 < 1) {
            return;
        }
        Text = av[1]->GetString();
        if (Text.length() > 0) {
            av[0]->SetInt(Text.read(1));
        }
    }

    void EF_Rnd(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        {
            std::int32_t cpp_arg = pas::random(av[1]->GetInt(), &System::RandSeed);
            TVarEC* cpp_arg_2 = av[0];
            cpp_arg_2->SetInt(cpp_arg);
        }
    }

    void EF_Randomize(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        SystemImports::Randomize();
    }

    void EF_RandSeed(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 1) {
            return;
        }
        av[0]->SetInt(System::RandSeed);
        if (Count >= 2) {
            System::RandSeed = av[1]->GetInt();
        }
    }

    void EF_SubStr(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t Start{};
        std::int32_t Size{};
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 3) {
            return;
        }
        std::int32_t TextLength = av[1]->GetString().length();
        Start = av[2]->GetInt();
        if (Count >= 4) {
            Size = av[3]->GetInt();
        } else {
            Size = 1999999999;
        }
        if (Start < 0 || Start >= TextLength) {
            av[0]->SetString(u""_wref.get());
            return;
        }
        if (Start + Size > TextLength) {
            Size = TextLength - Start;
        }
        {
            const pas::WideString& cpp_arg_2 = ([&] {
                const pas::WideString& string = av[1]->GetString();
                std::int32_t size = Size;
                std::int32_t cpp_arg = Start + 1;
                return pas::copy(string, cpp_arg, size);
            }());
            TVarEC* cpp_arg_3 = av[0];
            cpp_arg_3->SetString(cpp_arg_2);
        }
    }

    void EF_FindSubStr(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 4> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 4), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        pas::WideString Text{};
        pas::WideString Search{};
        if (av.length() - 1 < 2) {
            return;
        }
        Text = av[1]->GetString();
        Search = av[2]->GetString();
        std::int32_t Start = 0;
        if (av.length() - 1 >= 3) {
            Start = av[3]->GetInt();
        }
        std::int32_t TextLength = Text.length();
        std::int32_t SearchLength = Search.length();
        if (TextLength - Start < SearchLength) {
            av[0]->SetInt(-1);
            return;
        }
        if (TextLength < 1 && SearchLength < 1) {
            av[0]->SetInt(-1);
            return;
        }
        while (Start <= TextLength - SearchLength) {
            if (SysUtils::CompareMem(static_cast<std::uint8_t*>(static_cast<void*>(Text.data())) + Start * static_cast<std::int32_t>(sizeof(char16_t)), Search.pchar(), SearchLength * 2)) {
                av[0]->SetInt(Start);
                return;
            }
            ++Start;
        }
        av[0]->SetInt(-1);
    }

    void EF_Trim(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        {
            const pas::WideString& trimScriptString = EC_Expression::TrimScriptString(av[1]->GetString());
            TVarEC* cpp_arg = av[0];
            cpp_arg->SetString(trimScriptString);
        }
    }

    void EF_ToAnsi(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        {
            const pas::WideString& string = av[1]->GetString();
            TVarEC* cpp_arg = av[0];
            cpp_arg->SetString(string);
        }
        av[0]->PackAnsiString();
    }

    void EF_ToUnicode(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        std::int32_t Count = av.length() - 1 + 1;
        if (Count < 2) {
            return;
        }
        {
            const pas::WideString& string = av[1]->GetString();
            TVarEC* cpp_arg = av[0];
            cpp_arg->SetString(string);
        }
        av[0]->UnpackAnsiString();
    }

    void EF_LowerCase(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 4> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 4), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        pas::WideString Text{};
        pas::AnsiString AnsiText{};
        std::int32_t Count{};
        if (av.length() - 1 < 1) {
            return;
        }
        Text = av[1]->GetString();
        std::int32_t Start = 0;
        if (av.length() - 1 >= 2) {
            Start = av[2]->GetInt();
        }
        if (av.length() - 1 >= 3) {
            Count = av[3]->GetInt();
        } else {
            Count = Text.length() - Start;
        }
        if (Start < 0 || Start + Count > Text.length() || Count < 1) {
            av[0]->SetString(Text);
            return;
        }
        if (WindowsSdk::GetVersion() < 0x80000000u) {
            WindowsSdk::CharLowerBuffW(Text.pchar() + Start, Count);
            av[0]->SetString(Text);
        } else {
            AnsiText = static_cast<pas::AnsiString>(Text);
            WindowsImports::CharLowerBuffA(AnsiText.pchar() + Start, Count);
            av[0]->SetString(static_cast<pas::WideString>(AnsiText));
        }
    }

    void EF_UpperCase(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 4> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 4), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        pas::WideString Text{};
        pas::AnsiString AnsiText{};
        std::int32_t Count{};
        if (av.length() - 1 < 1) {
            return;
        }
        Text = av[1]->GetString();
        std::int32_t Start = 0;
        if (av.length() - 1 >= 2) {
            Start = av[2]->GetInt();
        }
        if (av.length() - 1 >= 3) {
            Count = av[3]->GetInt();
        } else {
            Count = Text.length() - Start;
        }
        if (Start < 0 || Start + Count > Text.length() || Count < 1) {
            av[0]->SetString(Text);
            return;
        }
        if (WindowsSdk::GetVersion() < 0x80000000u) {
            WindowsSdk::CharUpperBuffW(Text.pchar() + Start, Count);
            av[0]->SetString(Text);
        } else {
            AnsiText = static_cast<pas::AnsiString>(Text);
            // Native ANSI fallback lowercases even for UpperCase.
            WindowsImports::CharLowerBuffA(AnsiText.pchar() + Start, Count);
            av[0]->SetString(static_cast<pas::WideString>(AnsiText));
        }
    }

    void EF_LoadLibrary(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        pas::WideString cpp_text{};
        if (av.length() - 1 != 1) {
            return;
        }
        {
            std::uint32_t loadLibraryW = WindowsSdk::LoadLibraryW((cpp_text = av[1]->GetString(), cpp_text.pchar()));
            TVarEC* cpp_arg = av[0];
            cpp_arg->SetDword(loadLibraryW);
        }
    }

    void EF_FreeLibrary(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        if (av.length() - 1 != 1) {
            return;
        }
        {
            std::int32_t freeLibrary = WindowsImports::FreeLibrary(av[1]->GetDword());
            TVarEC* cpp_arg = av[0];
            cpp_arg->SetInt(freeLibrary);
        }
    }

    // av[1..3] are the module handle, return-kind name and export name; later arguments name parameter kinds.
    void EF_LibraryFunction(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        pas::AnsiString cpp_text{};
        std::int32_t i{};
        pas::WideString KindName{};
        if (av.length() - 1 < 3) {
            return;
        }
        void* Proc = ([&] {
            std::uint8_t* cpp_arg = (cpp_text = static_cast<pas::AnsiString>(av[3]->GetString()), cpp_text.pchar());
            std::uint32_t dword = av[1]->GetDword();
            return WindowsImports::GetProcAddress(dword, cpp_arg);
        }());
        if (Proc == nullptr) {
            av[0]->SetInt(0);
            return;
        }
        av[0]->ConvertToKind(vkLibraryFun);
        av[0]->LibraryFunData.set_length(2 + (av.length() - 1) - 3);
        if (av[2]->GetString() == u"int") {
            av[0]->LibraryFunData[0] = 1u;
        } else if (av[2]->GetString() == u"dword") {
            av[0]->LibraryFunData[0] = 2u;
        } else if (av[2]->GetString() == u"float") {
            av[0]->LibraryFunData[0] = 3u;
        } else if (av[2]->GetString() == u"str") {
            av[0]->LibraryFunData[0] = 4u;
        } else {
            av[0]->LibraryFunData[0] = 0u;
        }
        av[0]->LibraryFunData[1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Proc));
        {
            const std::int32_t cpp_last = av.length() - 1 - 3 - 1;
            if (0 <= cpp_last) {
                for (i = 0; i <= cpp_last; ++i) {
                    KindName = av[4 + i]->GetString();
                    if (KindName == u"int") {
                        av[0]->LibraryFunData[2 + i] = 1u;
                    } else if (KindName == u"dword") {
                        av[0]->LibraryFunData[2 + i] = 2u;
                    } else if (KindName == u"float") {
                        av[0]->LibraryFunData[2 + i] = 3u;
                    } else if (KindName == u"str") {
                        av[0]->LibraryFunData[2 + i] = 4u;
                    } else if (KindName == u"ref") {
                        av[0]->LibraryFunData[2 + i] = 5u;
                    } else if (KindName == u"code") {
                        av[0]->LibraryFunData[2 + i] = 6u;
                    } else {
                        pas::raise(pas::make_exception<ExceptionExpressionEC>("LibraryFunction. Unknown type"_a));
                    }
                }
            }
        }
    }

    // Looks up class definitions in the root scope; the instance shares their expression instructions.
    void EF_New(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        auto cpp_array_copy = pas::copy_open_array(av);
        av = pas::open_array(cpp_array_copy);
        TCodeEC* Definition{};
        TCodeEC* Instance{};
        if (av.length() - 1 != 1) {
            return;
        }
        if (code == nullptr) {
            return;
        }
        while (code != nullptr && code->Parent != nullptr) {
            code = code->Parent;
        }
        TVarEC* Found = ([&] {
            const pas::WideString& string = av[1]->GetString();
            TVarArrayEC* localVar = code->LocalVar;
            return localVar->GetVar(string);
        }());
        if (EC_Expression::TVarEC_RealVType(Found) == vkFunction) {
            Definition = Found->GetFunction();
            if (Definition->IsClassDefinition) {
                Instance = pas::construct_call<TCodeEC>(TCodeEC_Create);
                Instance->CopyFromFast(Definition);
                Instance->LinkLocalScopes();
                av[0]->SetClass(Instance);
            }
        }
    }

    // Also resets av[1].
    void EF_Delete(pas::OpenArray<TVarEC*> av, TCodeEC* code) {
        std::array<TVarEC*, 2> cpp_array_copy;
        std::copy_n(av.data(), std::min(av.length(), 2), cpp_array_copy.data());
        av.elements = cpp_array_copy.data();
        if (av.length() - 1 != 1) {
            return;
        }
        if (EC_Expression::TVarEC_RealVType(av[1]) == vkClass) {
            pas::free(av[1]->GetClass());
            av[1]->ResetKind(vkEmpty);
        }
    }

    // Sets Opcode only. Accepts arithmetic, logical and comparison tokens; other tokens raise.
    void InitInstr(TExpressionInstrEC* Instruction, TCodeTokenKind Token) {
        if (Token == ctAdd) {
            Instruction->Opcode = eoAdd;
        } else if (Token == ctSubtract) {
            Instruction->Opcode = eoSubtract;
        } else if (Token == ctMultiply) {
            Instruction->Opcode = eoMultiply;
        } else if (Token == ctDivide) {
            Instruction->Opcode = eoDivide;
        } else if (Token == ctModulo) {
            Instruction->Opcode = eoModulo;
        } else if (Token == ctBitAnd) {
            Instruction->Opcode = eoBitAnd;
        } else if (Token == ctBitOr) {
            Instruction->Opcode = eoBitOr;
        } else if (Token == ctBitXor) {
            Instruction->Opcode = eoBitXor;
        } else if (Token == ctBitNot) {
            Instruction->Opcode = eoBitNot;
        } else if (Token == ctAnd) {
            Instruction->Opcode = eoAnd;
        } else if (Token == ctOr) {
            Instruction->Opcode = eoOr;
        } else if (Token == ctNot) {
            Instruction->Opcode = eoNot;
        } else if (Token == ctShiftLeft) {
            Instruction->Opcode = eoShiftLeft;
        } else if (Token == ctShiftRight) {
            Instruction->Opcode = eoShiftRight;
        } else if (Token == ctEqual) {
            Instruction->Opcode = eoEqual;
        } else if (Token == ctNotEqual) {
            Instruction->Opcode = eoNotEqual;
        } else if (Token == ctLess) {
            Instruction->Opcode = eoLess;
        } else if (Token == ctGreater) {
            Instruction->Opcode = eoGreater;
        } else if (Token == ctLessEqual) {
            Instruction->Opcode = eoLessEqual;
        } else if (Token == ctGreaterEqual) {
            Instruction->Opcode = eoGreaterEqual;
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("InitInstr"_a));
        }
    }

    // Encodes error code and source position as a comma-separated decimal pair.
    void FormatScriptError(std::int32_t Code, std::int32_t Position, pas::WideString& Text) {
        Text = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Code), ",", SysUtils::IntToStr(Position)}));
    }

    // Reference parameters avoid copies of Self and RunStart in composed inline calls.
    void FlushTokenRun(TCodeAnalyzerEC*& Analyzer, const pas::WideString& Text, std::int32_t& RunStart, std::int32_t RunLength) {
        if (RunStart >= 0 && RunLength > 0) {
            Analyzer->Last->Text = pas::concat_wide({Analyzer->Last->Text, pas::copy(Text, RunStart + 1, RunLength)});
            Analyzer->Last->SourceLength += RunLength;
        }
    }

    void FlushQuotedRun(TCodeAnalyzerEC*& Analyzer, const pas::WideString& Text, std::int32_t RunStart, std::int32_t RunLength) {
        if (RunStart >= 0 && RunLength > 0) {
            Analyzer->Last->Text = pas::concat_wide({Analyzer->Last->Text, pas::copy(Text, RunStart + 1, RunLength - 1)});
            Analyzer->Last->SourceLength += RunLength - 1;
        }
    }

    // Reference parameters preserve caller storage when DCC32 expands these helpers.
    void EmitSourceToken(TCodeAnalyzerEC*& Analyzer, TCodeAnalyzerUnitEC*& Token, TCodeTokenKind Kind, std::int32_t& Index, std::int32_t& SourceOffset, std::int32_t SourceLength) {
        Token = Analyzer->AddToken();
        Token->TokenKind = Kind;
        Token->SourceStart = Index + SourceOffset;
        Token->SourceLength = SourceLength;
    }

    // Finish the pending text before beginning a punctuation or newline token.
    void EmitToken(TCodeAnalyzerEC*& Analyzer, const pas::WideString& Text, std::int32_t& RunStart, std::int32_t RunLength, TCodeAnalyzerUnitEC*& Token, TCodeTokenKind Kind, std::int32_t& Index, std::int32_t& SourceOffset, std::int32_t SourceLength) {
        EC_Expression::FlushTokenRun(Analyzer, Text, RunStart, RunLength);
        RunStart = -1;
        EC_Expression::EmitSourceToken(Analyzer, Token, Kind, Index, SourceOffset, SourceLength);
    }

    void TVarEC_Create(TVarEC* Self, TVarKind InitialKind) {
        pas::object_create(Self);
        Self->Kind = InitialKind;
        if (InitialKind == vkFunction) {
            Self->FunctionValue = pas::construct_call<TCodeEC>(TCodeEC_Create);
        }
    }

    void TVarEC_Destroy(TVarEC* Self) {
        if (Self->FunctionValue != nullptr) {
            pas::free(Self->FunctionValue);
            Self->FunctionValue = nullptr;
        }
        Self->LibraryFunData = nullptr;
        pas::object_destroy(Self);
    }

    // Preserves the value where conversion is supported; ResetKind discards it.
    void TVarEC::ConvertToKind(TVarKind NewKind) {
        if (FunctionValue != nullptr) {
            pas::free(FunctionValue);
            FunctionValue = nullptr;
        }
        if (NewKind == vkInt) {
            if (Kind != vkInt) {
                if (Kind == vkDword) {
                    IntValue = DwordValue;
                } else if (Kind == vkFloat) {
                    IntValue = System::Trunc(FloatValue);
                } else if (Kind == vkString) {
                    IntValue = EC_Expression::ScriptStringToInt(pas::view(StringValue));
                } else {
                    IntValue = 0;
                }
            }
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkDword) {
            if (Kind == vkInt) {
                DwordValue = IntValue;
            } else if (Kind != vkDword) {
                if (Kind == vkFloat) {
                    DwordValue = System::Trunc(FloatValue);
                } else if (Kind == vkString) {
                    DwordValue = EC_Expression::ScriptStringToInt(pas::view(StringValue));
                } else {
                    DwordValue = 0u;
                }
            }
            IntValue = 0;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkFloat) {
            if (Kind == vkInt) {
                FloatValue = IntValue;
            } else if (Kind == vkDword) {
                FloatValue = DwordValue;
            } else if (Kind != vkFloat) {
                if (Kind == vkString) {
                    FloatValue = EC_Expression::ScriptStringToFloat(pas::view(StringValue));
                } else {
                    FloatValue = 0.0;
                }
            }
            IntValue = 0;
            DwordValue = 0u;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkString) {
            if (Kind == vkInt) {
                StringValue = pas::wide_int_to_str(IntValue);
            } else if (Kind == vkDword) {
                StringValue = pas::wide_int64_to_str(static_cast<std::int64_t>(DwordValue));
            } else if (Kind == vkFloat) {
                try {
                    StringValue = EC_Expression::ScriptFloatToString(FloatValue);
                } catch (...) {
                    StringValue = pas::WideString();
                }
            } else if (Kind != vkString) {
                StringValue = pas::WideString();
            }
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkExternFun) {
            if (Kind != vkExternFun) {
                ExternFunValue = nullptr;
            }
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkLibraryFun) {
            if (Kind != vkLibraryFun) {
                LibraryFunData = nullptr;
            }
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkFunction) {
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            if (FunctionValue != nullptr) {
                pas::free(FunctionValue);
            }
            FunctionValue = pas::construct_call<TCodeEC>(TCodeEC_Create);
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkClass) {
            if (Kind != vkClass) {
                if (ClassValue != nullptr) {
                    pas::free(ClassValue);
                    ClassValue = nullptr;
                }
            }
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkArray) {
            if (Kind != vkArray) {
                ArrayValue = nullptr;
            }
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            RefValue = nullptr;
        } else if (NewKind == vkRef) {
            if (Kind != vkRef) {
                RefValue = nullptr;
            }
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
        }
        Kind = NewKind;
    }

    void TVarEC::ResetKind(TVarKind NewKind) {
        if (FunctionValue != nullptr) {
            pas::free(FunctionValue);
            FunctionValue = nullptr;
        }
        if (Kind == vkRef) {
            if (RefValue != nullptr) {
                RefValue->ResetKind(NewKind);
            }
        } else {
            Kind = NewKind;
            IntValue = 0;
            DwordValue = 0u;
            FloatValue = 0.0;
            StringValue = pas::WideString();
            ExternFunValue = nullptr;
            LibraryFunData = nullptr;
            FunctionValue = nullptr;
            ClassValue = nullptr;
            ArrayValue = nullptr;
            RefValue = nullptr;
            if (NewKind == vkFunction) {
                FunctionValue = pas::construct_call<TCodeEC>(TCodeEC_Create);
            }
        }
    }

    // Returns vkRef for an unresolved reference.
    TVarKind TVarEC_RealVType(TVarEC* Self) {
        TVarEC* Value = EC_Expression::TVarEC_Resolve(Self);
        if (Value == nullptr) {
            return vkRef;
        }
        return Value->Kind;
    }

    void TVarEC::AssignFrom(TVarEC* Source, std::uint8_t CopyArrays) {
        std::int32_t i{};
        if (FunctionValue != nullptr) {
            pas::free(FunctionValue);
            FunctionValue = nullptr;
        }
        Name = Source->Name;
        Kind = Source->Kind;
        IntValue = Source->IntValue;
        DwordValue = Source->DwordValue;
        FloatValue = Source->FloatValue;
        StringValue = Source->StringValue;
        ExternFunValue = Source->ExternFunValue;
        ClassValue = Source->ClassValue;
        RefValue = Source->RefValue;
        if (Source->ArrayValue != nullptr && CopyArrays) {
            if (GetArray() == nullptr) {
                SetArray(pas::construct_call<TVarArrayEC>(TVarArrayEC_Create));
            }
            if (GetArray()->Count > 0) {
                GetArray()->Clear();
            }
            {
                TVarArrayEC* array = Source->GetArray();
                TVarArrayEC* array_2 = GetArray();
                array_2->CopyFrom(array, true);
            }
        } else {
            ArrayValue = Source->ArrayValue;
        }
        LibraryFunData = nullptr;
        if (Source->LibraryFunData != nullptr) {
            LibraryFunData.set_length(Source->LibraryFunData.length() - 1 + 1);
            {
                const std::int32_t cpp_last = LibraryFunData.length() - 1;
                if (0 <= cpp_last) {
                    for (i = 0; i <= cpp_last; ++i) {
                        LibraryFunData[i] = Source->LibraryFunData[i];
                    }
                }
            }
        }
        FunctionValue = nullptr;
        if (Source->FunctionValue != nullptr) {
            FunctionValue = pas::construct_call<TCodeEC>(TCodeEC_Create);
            FunctionValue->CopyFrom(Source->FunctionValue);
        }
    }

    // Tests this cell's tag without dereferencing.
    std::uint8_t TVarEC::IsEmpty() {
        return Kind == vkEmpty;
    }

    std::int32_t TVarEC::GetInt() {
        if (Kind == vkEmpty) {
            return 0;
        } else if (Kind == vkInt) {
            return IntValue;
        } else if (Kind == vkDword) {
            return DwordValue;
        } else if (Kind == vkFloat) {
            return System::Trunc(FloatValue);
        } else if (Kind == vkString) {
            return EC_Expression::ScriptStringToInt(pas::view(StringValue));
        } else if (Kind == vkExternFun) {
            return 0;
        } else if (Kind == vkLibraryFun) {
            return 0;
        } else if (Kind == vkFunction) {
            return 0;
        } else if (Kind == vkClass) {
            return 0;
        } else if (Kind == vkArray) {
            return 0;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return 0;
            }
            return RefValue->GetInt();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    // Reference cells delegate to GetInt, then reinterpret its bits.
    std::uint32_t TVarEC::GetDword() {
        if (Kind == vkEmpty) {
            return 0u;
        } else if (Kind == vkInt) {
            return IntValue;
        } else if (Kind == vkDword) {
            return DwordValue;
        } else if (Kind == vkFloat) {
            return System::Trunc(FloatValue);
        } else if (Kind == vkString) {
            return EC_Expression::ScriptStringToInt(pas::view(StringValue));
        } else if (Kind == vkExternFun) {
            return 0u;
        } else if (Kind == vkLibraryFun) {
            return 0u;
        } else if (Kind == vkFunction) {
            return 0u;
        } else if (Kind == vkClass) {
            return 0u;
        } else if (Kind == vkArray) {
            return 0u;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return 0u;
            }
            return RefValue->GetInt();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    double TVarEC::GetFloat() {
        if (Kind == vkEmpty) {
            return 0.0;
        } else if (Kind == vkInt) {
            return IntValue;
        } else if (Kind == vkDword) {
            return DwordValue;
        } else if (Kind == vkFloat) {
            return FloatValue;
        } else if (Kind == vkString) {
            return EC_Expression::ScriptStringToFloat(pas::view(StringValue));
        } else if (Kind == vkExternFun) {
            return 0.0;
        } else if (Kind == vkLibraryFun) {
            return 0.0;
        } else if (Kind == vkFunction) {
            return 0.0;
        } else if (Kind == vkClass) {
            return 0.0;
        } else if (Kind == vkArray) {
            return 0.0;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return 0.0;
            }
            return RefValue->GetFloat();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    // Library cells return their import specification string.
    pas::WideString TVarEC::GetString() {
        if (Kind == vkEmpty) {
            return pas::WideString();
        } else if (Kind == vkInt) {
            return pas::wide_int_to_str(IntValue);
        } else if (Kind == vkDword) {
            return pas::wide_int64_to_str(static_cast<std::int64_t>(DwordValue));
        } else if (Kind == vkFloat) {
            return EC_Expression::ScriptFloatToString(FloatValue);
        } else if (Kind == vkString) {
            return StringValue;
        } else if (Kind == vkExternFun) {
            return pas::WideString();
        } else if (Kind == vkLibraryFun) {
            return StringValue;
        } else if (Kind == vkFunction) {
            return pas::WideString();
        } else if (Kind == vkClass) {
            return pas::WideString();
        } else if (Kind == vkArray) {
            return pas::WideString();
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return pas::WideString();
            }
            return RefValue->GetString();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    void* TVarEC::GetExternFun() {
        if (Kind == vkEmpty) {
            return nullptr;
        } else if (Kind == vkInt) {
            return nullptr;
        } else if (Kind == vkDword) {
            return nullptr;
        } else if (Kind == vkFloat) {
            return nullptr;
        } else if (Kind == vkString) {
            return nullptr;
        } else if (Kind == vkExternFun) {
            return ExternFunValue;
        } else if (Kind == vkLibraryFun) {
            return nullptr;
        } else if (Kind == vkFunction) {
            return nullptr;
        } else if (Kind == vkClass) {
            return nullptr;
        } else if (Kind == vkArray) {
            return nullptr;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return nullptr;
            }
            return RefValue->GetExternFun();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    TCodeEC* TVarEC::GetFunction() {
        if (Kind == vkEmpty) {
            return nullptr;
        } else if (Kind == vkInt) {
            return nullptr;
        } else if (Kind == vkDword) {
            return nullptr;
        } else if (Kind == vkFloat) {
            return nullptr;
        } else if (Kind == vkString) {
            return nullptr;
        } else if (Kind == vkExternFun) {
            return nullptr;
        } else if (Kind == vkLibraryFun) {
            return nullptr;
        } else if (Kind == vkFunction) {
            return FunctionValue;
        } else if (Kind == vkClass) {
            return nullptr;
        } else if (Kind == vkArray) {
            return nullptr;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return nullptr;
            }
            return RefValue->GetFunction();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    // Reference cells delegate to GetFunction in the native code.
    TCodeEC* TVarEC::GetClass() {
        if (Kind == vkEmpty) {
            return nullptr;
        } else if (Kind == vkInt) {
            return nullptr;
        } else if (Kind == vkDword) {
            return nullptr;
        } else if (Kind == vkFloat) {
            return nullptr;
        } else if (Kind == vkString) {
            return nullptr;
        } else if (Kind == vkExternFun) {
            return nullptr;
        } else if (Kind == vkLibraryFun) {
            return nullptr;
        } else if (Kind == vkFunction) {
            return nullptr;
        } else if (Kind == vkClass) {
            return ClassValue;
        } else if (Kind == vkArray) {
            return nullptr;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return nullptr;
            }
            return RefValue->GetFunction();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    TVarArrayEC* TVarEC::GetArray() {
        if (Kind == vkEmpty) {
            return nullptr;
        } else if (Kind == vkInt) {
            return nullptr;
        } else if (Kind == vkDword) {
            return nullptr;
        } else if (Kind == vkFloat) {
            return nullptr;
        } else if (Kind == vkString) {
            return nullptr;
        } else if (Kind == vkExternFun) {
            return nullptr;
        } else if (Kind == vkLibraryFun) {
            return nullptr;
        } else if (Kind == vkFunction) {
            return nullptr;
        } else if (Kind == vkClass) {
            return nullptr;
        } else if (Kind == vkArray) {
            return ArrayValue;
        } else if (Kind == vkRef) {
            if (RefValue == nullptr) {
                return nullptr;
            }
            return RefValue->GetArray();
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
        }
    }

    void TVarEC::SetInt(std::int32_t Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkInt);
            IntValue = Value;
        } else if (Kind == vkInt) {
            IntValue = Value;
        } else if (Kind == vkDword) {
            DwordValue = Value;
        } else if (Kind == vkFloat) {
            FloatValue = Value;
        } else if (Kind == vkString) {
            StringValue = pas::wide_int_to_str(Value);
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetInt(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    // Reference cells delegate to SetInt with the same bits.
    void TVarEC::SetDword(std::uint32_t Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkDword);
            DwordValue = Value;
        } else if (Kind == vkInt) {
            IntValue = Value;
        } else if (Kind == vkDword) {
            DwordValue = Value;
        } else if (Kind == vkFloat) {
            FloatValue = Value;
        } else if (Kind == vkString) {
            StringValue = pas::wide_int64_to_str(static_cast<std::int64_t>(Value));
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetInt(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    void TVarEC::SetFloat(double Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkFloat);
            FloatValue = Value;
        } else if (Kind == vkInt) {
            IntValue = System::Trunc(Value);
        } else if (Kind == vkDword) {
            DwordValue = System::Trunc(Value);
        } else if (Kind == vkFloat) {
            FloatValue = Value;
        } else if (Kind == vkString) {
            StringValue = EC_Expression::ScriptFloatToString(Value);
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetFloat(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    // Assigns through references and converts to an existing destination kind; an empty cell becomes a string.
    void TVarEC::SetString(const pas::WideString& Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkString);
            StringValue = Value;
        } else if (Kind == vkInt) {
            IntValue = EC_Expression::ScriptStringToInt(pas::view(Value));
        } else if (Kind == vkDword) {
            DwordValue = EC_Expression::ScriptStringToInt(pas::view(Value));
        } else if (Kind == vkFloat) {
            FloatValue = EC_Expression::ScriptStringToFloat(pas::view(Value));
        } else if (Kind == vkString) {
            StringValue = Value;
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            StringValue = Value;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetString(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    void TVarEC::SetExternFun(void* Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkExternFun);
            ExternFunValue = Value;
        } else if (Kind == vkInt) {
            IntValue = 0;
        } else if (Kind == vkDword) {
            DwordValue = 0u;
        } else if (Kind == vkFloat) {
            FloatValue = 0.0;
        } else if (Kind == vkString) {
            StringValue = pas::WideString();
        } else if (Kind == vkExternFun) {
            ExternFunValue = Value;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetExternFun(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    // Native leaves empty and function cells unchanged; other kinds clear their payload or delegate through a reference.
    void TVarEC::SetFunction(TCodeEC* Value) {
        if (!(Kind == vkEmpty)) {
            if (Kind == vkInt) {
                IntValue = 0;
            } else if (Kind == vkDword) {
                DwordValue = 0u;
            } else if (Kind == vkFloat) {
                FloatValue = 0.0;
            } else if (Kind == vkString) {
                StringValue = pas::WideString();
            } else if (Kind == vkExternFun) {
                ExternFunValue = nullptr;
            } else if (Kind == vkLibraryFun) {
                LibraryFunData = nullptr;
            } else if (!(Kind == vkFunction)) {
                if (Kind == vkClass) {
                    ClassValue = nullptr;
                } else if (Kind == vkArray) {
                    ArrayValue = nullptr;
                } else if (Kind == vkRef) {
                    if (RefValue != nullptr) {
                        RefValue->SetFunction(Value);
                    } else {
                        pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                    }
                }
            }
        }
    }

    // Value is borrowed; vkRef assignment uses the function-value setter.
    void TVarEC::SetClass(TCodeEC* Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkClass);
            ClassValue = Value;
        } else if (Kind == vkInt) {
            IntValue = 0;
        } else if (Kind == vkDword) {
            DwordValue = 0u;
        } else if (Kind == vkFloat) {
            FloatValue = 0.0;
        } else if (Kind == vkString) {
            StringValue = pas::WideString();
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = Value;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetFunction(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    // Value is borrowed; follows references.
    void TVarEC::SetArray(TVarArrayEC* Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkArray);
            ArrayValue = Value;
        } else if (Kind == vkInt) {
            IntValue = 0;
        } else if (Kind == vkDword) {
            DwordValue = 0u;
        } else if (Kind == vkFloat) {
            FloatValue = 0.0;
        } else if (Kind == vkString) {
            StringValue = pas::WideString();
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = Value;
            } else if (Kind == vkRef) {
                if (RefValue != nullptr) {
                    RefValue->SetArray(Value);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
                }
            }
        }
    }

    void TVarEC::SetRef(TVarEC* Value) {
        if (Kind == vkEmpty) {
            ResetKind(vkRef);
            RefValue = Value;
        } else if (Kind == vkInt) {
            IntValue = 0;
        } else if (Kind == vkDword) {
            DwordValue = 0u;
        } else if (Kind == vkFloat) {
            FloatValue = 0.0;
        } else if (Kind == vkString) {
            StringValue = pas::WideString();
        } else if (Kind == vkExternFun) {
            ExternFunValue = nullptr;
        } else if (Kind == vkLibraryFun) {
            LibraryFunData = nullptr;
        } else if (!(Kind == vkFunction)) {
            if (Kind == vkClass) {
                ClassValue = nullptr;
            } else if (Kind == vkArray) {
                ArrayValue = nullptr;
            } else if (Kind == vkRef) {
                RefValue = Value;
            } else {
                pas::raise(pas::make_exception<ExceptionExpressionEC>("Type error"_a));
            }
        }
    }

    // May return nil.
    TVarEC* TVarEC_Resolve(TVarEC* Self) {
        TVarEC* Result = Self;
        while (Result != nullptr && Result->Kind == vkRef) {
            Result = Result->RefValue;
        }
        return Result;
    }

    // Stores ANSI bytes inside StringValue's UTF-16 allocation.
    void TVarEC::PackAnsiString() {
        pas::AnsiString Text{};
        std::int32_t i{};
        std::uint8_t* Dest{};
        ConvertToKind(vkString);
        std::int32_t Count = StringValue.length();
        if (Count > 0) {
            Text = static_cast<pas::AnsiString>(StringValue);
            Dest = reinterpret_cast<std::uint8_t*>(StringValue.pchar());
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1 + 1); cpp_range.next(i); ) {
                *Dest = Text.read(i + 1);
                ++Dest;
            }
            if ((Count & 1) != 0) {
                StringValue.set_length(pas::shr(Count, 1) + 1);
            } else {
                StringValue.set_length(pas::shr(Count, 1));
            }
        }
    }

    // Non-string cells are converted to string without unpacking.
    void TVarEC::UnpackAnsiString() {
        pas::AnsiString Text{};
        std::int32_t Count{};
        if (Kind != vkString) {
            ConvertToKind(vkString);
        } else {
            Count = StringValue.length();
            if (Count > 0) {
                Text = static_cast<pas::AnsiString>(reinterpret_cast<std::uint8_t*>(StringValue.pchar()));
                StringValue = static_cast<pas::WideString>(Text);
            }
        }
    }

    void CollectScriptArrayDimensions(TVarArrayEC* Values, pas::DynArray<std::int32_t>& Dimensions) {
        Dimensions.set_length(Dimensions.length() - 1 + 1 + 1);
        Dimensions[Dimensions.length() - 1] = Values->Count;
        if (Values->Count > 0 && EC_Expression::TVarEC_RealVType(EC_Expression::TVarArrayEC_GetItem(Values, 0)) == vkArray) {
            EC_Expression::CollectScriptArrayDimensions(EC_Expression::TVarArrayEC_GetItem(Values, 0)->GetArray(), Dimensions);
        }
    }

    // Requires at least one dimension.
    void TVarEC::CreateArray(pas::OpenArray<std::int32_t> Dimensions) {
        auto cpp_array_copy = pas::copy_open_array(Dimensions);
        Dimensions = pas::open_array(cpp_array_copy);
        ResetKind(vkArray);
        ArrayValue = pas::construct_call<TVarArrayEC>(TVarArrayEC_Create);
        EC_Expression::GrowScriptArray(ArrayValue, Dimensions, 0);
    }

    // Nonpositive Count frees the array; positive Count resizes only when Dimension <= 0.
    void TVarEC_ResizeArray(TVarEC* Self, std::int32_t Count, std::int32_t Dimension) {
        pas::Array<std::int32_t, 0, 0> Dimensions{};
        if (EC_Expression::TVarEC_RealVType(Self) == vkArray) {
            if (Count <= 0) {
                EC_Expression::TVarEC_FreeArray(Self);
            } else if (Dimension <= 0) {
                if (Self->GetArray() == nullptr) {
                    Dimensions[0] = Count;
                    Self->CreateArray(pas::open_array(Dimensions));
                } else {
                    EC_Expression::ResizeScriptArray(Self->GetArray(), Count);
                }
            }
        }
    }

    // Frees nested arrays; retains vkArray with a nil pointer.
    void TVarEC_FreeArray(TVarEC* Self) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkArray && Self->GetArray() != nullptr) {
            EC_Expression::FreeScriptArrayTree(Self->GetArray());
            Self->SetArray(nullptr);
        }
    }

    void TVarEC_OAdd(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left + Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 + Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(cpp_left_3 + Right->GetFloat());
                    break;
                }
                case vkString: {
                    Self->SetString(pas::concat_wide_reverse({Right->GetString(), Left->GetString()}));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OAdd"_a));
            }
        }
    }

    void TVarEC_OSub(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left - Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 - Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(cpp_left_3 - Right->GetFloat());
                    break;
                }
                case vkString: {
                    Self->SetString(pas::concat_wide_reverse({Right->GetString(), Left->GetString()}));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OSub"_a));
            }
        }
    }

    void TVarEC_OMul(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left * Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 * Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(cpp_left_3 * Right->GetFloat());
                    break;
                }
                case vkString: {
                    Self->SetString(pas::concat_wide_reverse({Right->GetString(), Left->GetString()}));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OMul"_a));
            }
        }
    }

    void TVarEC_ODiv(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(pas::idiv(cpp_left, Right->GetInt()));
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(pas::idiv(cpp_left_2, Right->GetDword()));
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(pas::real_divide(cpp_left_3, Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::concat_wide_reverse({Right->GetString(), Left->GetString()}));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("ODiv"_a));
            }
        }
    }

    void TVarEC_OMod(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(pas::imod(cpp_left, Right->GetInt()));
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(pas::imod(cpp_left_2, Right->GetDword()));
                    break;
                }
                case vkFloat: {
                    std::int64_t cpp_right = System::Trunc(Right->GetFloat());
                    Self->SetFloat(pas::imod(System::Trunc(Left->GetFloat()), cpp_right));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OMod"_a));
            }
        }
    }

    void TVarEC_OBitAnd(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left & Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 & Right->GetDword());
                    break;
                }
                case vkFloat: {
                    std::int64_t cpp_left_3 = System::Trunc(Left->GetFloat());
                    Self->SetFloat(cpp_left_3 & System::Trunc(Right->GetFloat()));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OBitAnd"_a));
            }
        }
    }

    void TVarEC_OBitOr(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left | Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 | Right->GetDword());
                    break;
                }
                case vkFloat: {
                    std::int64_t cpp_left_3 = System::Trunc(Left->GetFloat());
                    Self->SetFloat(cpp_left_3 | System::Trunc(Right->GetFloat()));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OBitOr"_a));
            }
        }
    }

    void TVarEC_OBitXor(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left ^ Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 ^ Right->GetDword());
                    break;
                }
                case vkFloat: {
                    std::int64_t cpp_left_3 = System::Trunc(Left->GetFloat());
                    Self->SetFloat(cpp_left_3 ^ System::Trunc(Right->GetFloat()));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OBitXor"_a));
            }
        }
    }

    void TVarEC_OAnd(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: Self->SetInt(Left->GetInt() != 0 && Right->GetInt() != 0); break;
                case vkDword: Self->SetDword(Left->GetDword() != 0 && Right->GetDword() != 0); break;
                case vkFloat: {
                    Self->SetFloat(static_cast<std::int32_t>(Left->GetFloat() != 0.0L && Right->GetFloat() != 0.0L));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OAnd"_a));
            }
        }
    }

    void TVarEC_OOr(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: Self->SetInt(Left->GetInt() != 0 || Right->GetInt() != 0); break;
                case vkDword: Self->SetDword(Left->GetDword() != 0 || Right->GetDword() != 0); break;
                case vkFloat: {
                    Self->SetFloat(static_cast<std::int32_t>(Left->GetFloat() != 0.0L || Right->GetFloat() != 0.0L));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OOr"_a));
            }
        }
    }

    void TVarEC_OShl(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_right = Right->GetInt();
                    Self->SetInt(pas::shl(Left->GetInt(), cpp_right));
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_right_2 = Right->GetDword();
                    Self->SetDword(pas::shl(Left->GetDword(), cpp_right_2));
                    break;
                }
                case vkFloat: {
                    std::int64_t cpp_right_3 = System::Trunc(Right->GetFloat());
                    Self->SetFloat(pas::shl(System::Trunc(Left->GetFloat()), cpp_right_3));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OShl"_a));
            }
        }
    }

    void TVarEC_OShr(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_right = Right->GetInt();
                    Self->SetInt(pas::shr(Left->GetInt(), cpp_right));
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_right_2 = Right->GetDword();
                    Self->SetDword(pas::shr(Left->GetDword(), cpp_right_2));
                    break;
                }
                case vkFloat: {
                    std::int64_t cpp_right_3 = System::Trunc(Right->GetFloat());
                    Self->SetFloat(pas::shr(System::Trunc(Left->GetFloat()), cpp_right_3));
                    break;
                }
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OShr"_a));
            }
        }
    }

    void TVarEC_OEqual(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left == Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 == Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(static_cast<std::int32_t>(cpp_left_3 == Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::wide_int_to_str(static_cast<std::int32_t>(([&] {
                        pas::WideString cpp_string = Left->GetString();
                        pas::WideString cpp_string_2 = Right->GetString();
                        return cpp_string == cpp_string_2;
                    }()))));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OEqual"_a));
            }
        }
    }

    void TVarEC_ONotEqual(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left != Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 != Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(static_cast<std::int32_t>(cpp_left_3 != Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::wide_int_to_str(static_cast<std::int32_t>(([&] {
                        pas::WideString cpp_string = Left->GetString();
                        pas::WideString cpp_string_2 = Right->GetString();
                        return cpp_string != cpp_string_2;
                    }()))));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("ONotEqual"_a));
            }
        }
    }

    void TVarEC_OLess(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left < Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 < Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(static_cast<std::int32_t>(cpp_left_3 < Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::wide_int_to_str(static_cast<std::int32_t>(([&] {
                        pas::WideString cpp_string = Left->GetString();
                        pas::WideString cpp_string_2 = Right->GetString();
                        return cpp_string < cpp_string_2;
                    }()))));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OLess"_a));
            }
        }
    }

    void TVarEC_OMore(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left > Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 > Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(static_cast<std::int32_t>(cpp_left_3 > Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::wide_int_to_str(static_cast<std::int32_t>(([&] {
                        pas::WideString cpp_string = Left->GetString();
                        pas::WideString cpp_string_2 = Right->GetString();
                        return cpp_string > cpp_string_2;
                    }()))));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OMore"_a));
            }
        }
    }

    void TVarEC_OLessEqual(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left <= Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 <= Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(static_cast<std::int32_t>(cpp_left_3 <= Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::wide_int_to_str(static_cast<std::int32_t>(([&] {
                        pas::WideString cpp_string = Left->GetString();
                        pas::WideString cpp_string_2 = Right->GetString();
                        return cpp_string <= cpp_string_2;
                    }()))));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OLessEqual"_a));
            }
        }
    }

    void TVarEC_OMoreEqual(TVarEC* Self, TVarEC* Left, TVarEC* Right) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Left));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Left)) {
                case vkInt: {
                    std::int32_t cpp_left = Left->GetInt();
                    Self->SetInt(cpp_left >= Right->GetInt());
                    break;
                }
                case vkDword: {
                    std::uint32_t cpp_left_2 = Left->GetDword();
                    Self->SetDword(cpp_left_2 >= Right->GetDword());
                    break;
                }
                case vkFloat: {
                    pas::Extended cpp_left_3 = Left->GetFloat();
                    Self->SetFloat(static_cast<std::int32_t>(cpp_left_3 >= Right->GetFloat()));
                    break;
                }
                case vkString: {
                    Self->SetString(pas::wide_int_to_str(static_cast<std::int32_t>(([&] {
                        pas::WideString cpp_string = Left->GetString();
                        pas::WideString cpp_string_2 = Right->GetString();
                        return cpp_string >= cpp_string_2;
                    }()))));
                    break;
                }
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OMoreEqual"_a));
            }
        }
    }

    void TVarEC_OMinus(TVarEC* Self, TVarEC* Value) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Value));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Value)) {
                case vkInt: Self->SetInt(-Value->GetInt()); break;
                case vkDword: Self->SetDword(-static_cast<std::int64_t>(Value->GetDword())); break;
                case vkFloat: Self->SetFloat(-Value->GetFloat()); break;
                case vkString: Self->SetString(Value->GetString()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OMinus"_a));
            }
        }
    }

    void TVarEC_OBitNot(TVarEC* Self, TVarEC* Value) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Value));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Value)) {
                case vkInt: Self->SetInt(~Value->GetInt()); break;
                case vkDword: Self->SetDword(~Value->GetDword()); break;
                case vkFloat: Self->SetFloat(0.0); break;
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("OBitNot"_a));
            }
        }
    }

    void TVarEC_ONot(TVarEC* Self, TVarEC* Value) {
        if (EC_Expression::TVarEC_RealVType(Self) == vkEmpty) {
            Self->ResetKind(EC_Expression::TVarEC_RealVType(Value));
        }
        if (EC_Expression::TVarEC_RealVType(Self) != vkEmpty) {
            switch (EC_Expression::TVarEC_RealVType(Value)) {
                case vkInt: Self->SetInt(Value->GetInt() == 0); break;
                case vkDword: Self->SetDword(Value->GetInt() == 0); break;
                case vkFloat: Self->SetFloat(static_cast<std::int32_t>(Value->GetFloat() == 0.0L)); break;
                case vkString: Self->SetString(u""_wref.get()); break;
                case vkExternFun: Self->SetExternFun(nullptr); break;
                case vkFunction: Self->SetFunction(nullptr); break;
                case vkClass: Self->SetClass(nullptr); break;
                case vkArray: Self->SetArray(nullptr); break;
                default: pas::raise(pas::make_exception<ExceptionExpressionEC>("ONot"_a));
            }
        }
    }

    // Assigns through references, converting to the destination kind.
    void TVarEC::Assume(TVarEC* Source, std::uint8_t CopyArrays) {
        std::int32_t i{};
        TVarEC* Dest = this;
        if (Kind == vkRef) {
            Dest = EC_Expression::TVarEC_Resolve(this);
            if (Dest == nullptr) {
                return;
            }
        }
        if (Dest->Kind == vkExternFun && Source->Kind != vkExternFun) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error assigning to function ", Dest->Name}))));
        }
        if (Dest->Kind == vkEmpty) {
            Dest->ResetKind(EC_Expression::TVarEC_RealVType(Source));
        }
        if (!(Dest->Kind == vkEmpty)) {
            if (Dest->Kind == vkInt) {
                Dest->SetInt(Source->GetInt());
            } else if (Dest->Kind == vkDword) {
                Dest->SetDword(Source->GetDword());
            } else if (Dest->Kind == vkFloat) {
                Dest->SetFloat(Source->GetFloat());
            } else if (Dest->Kind == vkString) {
                Dest->SetString(Source->GetString());
            } else if (Dest->Kind == vkExternFun) {
                Dest->SetExternFun(Source->GetExternFun());
            } else if (Dest->Kind == vkLibraryFun) {
                Dest->LibraryFunData = nullptr;
                if (Source->LibraryFunData != nullptr) {
                    Dest->LibraryFunData.set_length(Source->LibraryFunData.length() - 1 + 1);
                    {
                        const std::int32_t cpp_last = Dest->LibraryFunData.length() - 1;
                        if (0 <= cpp_last) {
                            for (i = 0; i <= cpp_last; ++i) {
                                Dest->LibraryFunData[i] = Source->LibraryFunData[i];
                            }
                        }
                    }
                }
                Dest->SetString(Source->GetString());
            } else if (Dest->Kind == vkFunction) {
                Dest->SetFunction(Source->GetFunction());
            } else if (Dest->Kind == vkClass) {
                Dest->SetClass(Source->GetClass());
            } else if (Dest->Kind == vkArray) {
                if (!CopyArrays) {
                    Dest->SetArray(Source->GetArray());
                } else {
                    if (Dest->GetArray() == nullptr) {
                        Dest->SetArray(pas::construct_call<TVarArrayEC>(TVarArrayEC_Create));
                    }
                    if (Dest->GetArray()->Count > 0) {
                        Dest->GetArray()->Clear();
                    }
                    {
                        TVarArrayEC* array = Source->GetArray();
                        TVarArrayEC* array_2 = Dest->GetArray();
                        array_2->CopyFrom(array, true);
                    }
                }
            } else {
                pas::raise(pas::make_exception<ExceptionExpressionEC>("OAssume"_a));
            }
        }
    }

    std::uint8_t TVarEC_EqualsValue(TVarEC* Self, TVarEC* Other) {
        switch (EC_Expression::TVarEC_RealVType(Self)) {
            case vkEmpty: return Self->IsEmpty() == Other->IsEmpty();
            case vkInt: {
                std::int32_t cpp_left = Self->GetInt();
                return cpp_left == Other->GetInt();
            }
            case vkDword: {
                std::uint32_t cpp_left_2 = Self->GetDword();
                return cpp_left_2 == Other->GetDword();
            }
            case vkFloat: {
                pas::Extended cpp_left_3 = Self->GetFloat();
                return cpp_left_3 == Other->GetFloat();
            }
            case vkString: {
                pas::WideString cpp_string = Self->GetString();
                pas::WideString cpp_string_2 = Other->GetString();
                return cpp_string == cpp_string_2;
            }
            case vkExternFun: return false;
            case vkFunction: return false;
            case vkClass: {
                TCodeEC* cpp_left_4 = Self->GetClass();
                return cpp_left_4 == Other->GetClass();
            }
            case vkArray: return false;
            default: pas::raise(pas::make_exception<ExceptionExpressionEC>("Equal"_a));
        }
    }

    std::uint8_t TVarEC_LessThan(TVarEC* Self, TVarEC* Other) {
        switch (EC_Expression::TVarEC_RealVType(Self)) {
            case vkEmpty: return Self->IsEmpty() < Other->IsEmpty();
            case vkInt: {
                std::int32_t cpp_left = Self->GetInt();
                return cpp_left < Other->GetInt();
            }
            case vkDword: {
                std::uint32_t cpp_left_2 = Self->GetDword();
                return cpp_left_2 < Other->GetDword();
            }
            case vkFloat: {
                pas::Extended cpp_left_3 = Self->GetFloat();
                return cpp_left_3 < Other->GetFloat();
            }
            case vkString: {
                pas::WideString cpp_string = Self->GetString();
                pas::WideString cpp_string_2 = Other->GetString();
                return cpp_string < cpp_string_2;
            }
            case vkExternFun: return false;
            case vkFunction: return false;
            case vkClass: return false;
            case vkArray: return false;
            default: pas::raise(pas::make_exception<ExceptionExpressionEC>("Less"_a));
        }
    }

    std::uint8_t TVarEC_GreaterThan(TVarEC* Self, TVarEC* Other) {
        switch (EC_Expression::TVarEC_RealVType(Self)) {
            case vkEmpty: return Self->IsEmpty() > Other->IsEmpty();
            case vkInt: {
                std::int32_t cpp_left = Self->GetInt();
                return cpp_left > Other->GetInt();
            }
            case vkDword: {
                std::uint32_t cpp_left_2 = Self->GetDword();
                return cpp_left_2 > Other->GetDword();
            }
            case vkFloat: {
                pas::Extended cpp_left_3 = Self->GetFloat();
                return cpp_left_3 > Other->GetFloat();
            }
            case vkString: {
                pas::WideString cpp_string = Self->GetString();
                pas::WideString cpp_string_2 = Other->GetString();
                return cpp_string > cpp_string_2;
            }
            case vkExternFun: return false;
            case vkFunction: return false;
            case vkClass: return false;
            case vkArray: return false;
            default: pas::raise(pas::make_exception<ExceptionExpressionEC>("More"_a));
        }
    }

    std::uint8_t TVarEC_IsTrue(TVarEC* Self) {
        switch (EC_Expression::TVarEC_RealVType(Self)) {
            case vkEmpty: return false;
            case vkInt: return Self->GetInt() != 0;
            case vkDword: return Self->GetDword() != 0;
            case vkFloat: return Self->GetFloat() != 0.0L;
            case vkString: return Self->GetString() != u"";
            case vkExternFun: return false;
            case vkLibraryFun: return EC_Expression::TVarEC_Resolve(Self)->LibraryFunData != nullptr;
            case vkFunction: return false;
            case vkClass: return Self->GetClass() != nullptr;
            case vkArray: return false;
            default: pas::raise(pas::make_exception<ExceptionExpressionEC>("IsTrue"_a));
        }
    }

    // Only scalar, string and array kinds have serialized payloads.
    void TVarEC::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(Name);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
        if (!(Kind == vkEmpty)) {
            if (Kind == vkInt) {
                Buffer->AddIntegerValue(IntValue);
            } else if (Kind == vkDword) {
                Buffer->AddDWord(DwordValue);
            } else if (Kind == vkFloat) {
                Buffer->AddDouble(FloatValue);
            } else if (Kind == vkString) {
                Buffer->AddWideStringZ(StringValue);
            } else if (!(Kind == vkExternFun)) {
                if (!(Kind == vkLibraryFun)) {
                    if (!(Kind == vkFunction)) {
                        if (!(Kind == vkClass)) {
                            if (Kind == vkArray) {
                                ArrayValue->SaveToBuffer(Buffer);
                            } else {
                                static_cast<void>(Kind == vkRef);
                            }
                        }
                    }
                }
            }
        }
    }

    void TVarEC::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        Name = Buffer->ReadWideString();
        ResetKind(static_cast<TVarKind>(EC_Buf::TBufEC_GetByte(Buffer)));
        if (!(Kind == vkEmpty)) {
            if (Kind == vkInt) {
                IntValue = EC_Buf::TBufEC_GetInt32(Buffer);
            } else if (Kind == vkDword) {
                DwordValue = EC_Buf::TBufEC_GetUInt32(Buffer);
            } else if (Kind == vkFloat) {
                FloatValue = Buffer->GetDouble();
            } else if (Kind == vkString) {
                StringValue = Buffer->ReadWideString();
            } else if (!(Kind == vkExternFun)) {
                if (!(Kind == vkLibraryFun)) {
                    if (!(Kind == vkFunction)) {
                        if (!(Kind == vkClass)) {
                            if (Kind == vkArray) {
                                ArrayValue = pas::construct_call<TVarArrayEC>(TVarArrayEC_Create);
                                ArrayValue->LoadFromBuffer(Buffer);
                            } else {
                                static_cast<void>(Kind == vkRef);
                            }
                        }
                    }
                }
            }
        }
    }

    void TVarArrayEC_Create(TVarArrayEC* Self) {
        pas::object_create(Self);
    }

    void TVarArrayEC_Destroy(TVarArrayEC* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    // Does not free cells; use Clear for owned entries.
    void TVarArrayEC::ClearStorage() {
        if (Data != nullptr) {
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Data);
            Data = nullptr;
        }
        if (NameOrder != nullptr) {
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, NameOrder);
            NameOrder = nullptr;
        }
        Count = 0;
    }

    void TVarArrayEC::Clear() {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            if (EC_Expression::TVarArrayEC_GetItem(this, i)->Kind == vkArray && EC_Expression::TVarArrayEC_GetItem(this, i)->GetArray() != nullptr) {
                EC_Expression::TVarArrayEC_GetItem(this, i)->GetArray()->Clear();
            }
            pas::free(EC_Expression::TVarArrayEC_GetItem(this, i));
        }
        ClearStorage();
    }

    void TVarArrayEC::CopyFrom(TVarArrayEC* Source, std::uint8_t CopyArrays) {
        TVarEC* Item{};
        TVarEC* SourceItem{};
        std::int32_t i{};
        Clear();
        Count = Source->Count;
        if (Count < 1) {
            return;
        }
        Data = static_cast<PVarEC>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, Count * static_cast<std::int32_t>(sizeof(TVarEC*))));
        NameOrder = static_cast<std::int32_t*>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, Count * static_cast<std::int32_t>(sizeof(std::int32_t))));
        Windows::CopyMemory(NameOrder, Source->NameOrder, Count * static_cast<std::int32_t>(sizeof(std::int32_t)));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            SourceItem = EC_Expression::TVarArrayEC_GetItem(Source, i);
            Item = pas::construct_call<TVarEC>(TVarEC_Create, vkEmpty);
            EC_Expression::TVarArrayEC_SetItem(this, i, Item);
            Item->Name = SourceItem->Name;
            Item->AssignFrom(SourceItem, CopyArrays);
        }
    }

    // Returns -1 when absent.
    std::int32_t TVarArrayEC::FindNameOrderIndex(const pas::WideString& Name) {
        std::int32_t Middle{};
        std::int32_t Comparison{};
        TVarEC* Item{};
        if (Count < 1) {
            return -1;
        }
        std::int32_t Low = 0;
        std::int32_t High = Count - 1;
        do {
            Middle = (High - Low) / 2 + Low;
            Item = EC_Expression::TVarArrayEC_GetItemByNameOrder(this, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Item->Name.pchar());
            if (Comparison == 0) {
                return Middle;
            }
            if (Comparison < 0) {
                High = Middle - 1;
            } else {
                Low = Middle + 1;
            }
        } while (!(High < Low));
        return -1;
    }

    std::int32_t TVarArrayEC::FindNameInsertionIndex(const pas::WideString& Name) {
        std::int32_t Middle{};
        std::int32_t Comparison{};
        TVarEC* Item{};
        if (Count <= 0) {
            return 0;
        }
        std::int32_t Low = 0;
        std::int32_t High = Count - 1;
        do {
            Middle = (High - Low) / 2 + Low;
            Item = EC_Expression::TVarArrayEC_GetItemByNameOrder(this, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Item->Name.pchar());
            if (Comparison == 0) {
                return Middle;
            }
            if (Comparison < 0) {
                High = Middle - 1;
            } else {
                Low = Middle + 1;
            }
        } while (!(High < Low));
        if (Comparison < 0) {
            return Middle;
        }
        return Middle + 1;
    }

    // Returns nil for an out-of-range index.
    TVarEC* TVarArrayEC::GetItemNE(std::int32_t Index) {
        if (Index < 0 || Index >= Count) {
            return nullptr;
        }
        return EC_Expression::TVarArrayEC_GetItem(this, Index);
    }

    TVarEC* TVarArrayEC::GetVar(const pas::WideString& Name) {
        TVarEC* Result = GetVarNE(Name);
        if (Result == nullptr) {
            pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Var not found:", Name}))));
        }
        return Result;
    }

    // Returns nil when absent.
    TVarEC* TVarArrayEC::GetVarNE(const pas::WideString& Name) {
        std::int32_t Middle{};
        std::int32_t Comparison{};
        TVarEC* Item{};
        if (Count < 1) {
            return nullptr;
        }
        std::int32_t Low = 0;
        std::int32_t High = Count - 1;
        do {
            Middle = (High - Low) / 2 + Low;
            Item = EC_Expression::TVarArrayEC_GetItemByNameOrder(this, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Item->Name.pchar());
            if (Comparison == 0) {
                return EC_Expression::TVarArrayEC_GetItem(this, EC_Expression::TVarArrayEC_GetNameOrderIndex(this, Middle));
            }
            if (Comparison < 0) {
                High = Middle - 1;
            } else {
                Low = Middle + 1;
            }
        } while (!(High < Low));
        return nullptr;
    }

    // Frees the cell; ignores invalid indexes.
    void TVarArrayEC::Delete(std::int32_t Index) {
        std::int32_t i{};
        std::int32_t DataIndex{};
        if (Index < 0) {
            return;
        }
        if (Index >= Count) {
            return;
        }
        TVarEC* Item = EC_Expression::TVarArrayEC_GetItem(this, Index);
        if (Item != nullptr) {
            pas::free(Item);
        }
        std::int32_t NameIndex = EC_Expression::TVarArrayEC_FindNameOrderForDataIndex(this, Index);
        for (auto cpp_range = pas::for_to<std::int32_t>(NameIndex, Count - 2); cpp_range.next(i); ) {
            EC_Expression::TVarArrayEC_SetNameOrderIndex(this, i, EC_Expression::TVarArrayEC_GetNameOrderIndex(this, i + 1));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(Index, Count - 2); cpp_range_2.next(i); ) {
            EC_Expression::TVarArrayEC_SetItem(this, i, EC_Expression::TVarArrayEC_GetItem(this, i + 1));
        }
        --Count;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(i); ) {
            DataIndex = EC_Expression::TVarArrayEC_GetNameOrderIndex(this, i);
            if (DataIndex > Index) {
                EC_Expression::TVarArrayEC_SetNameOrderIndex(this, i, DataIndex - 1);
            }
        }
        if (Count < 1) {
            Clear();
        }
    }

    void TVarArrayEC::Remove(TVarEC* Value) {
        Delete(EC_Expression::TVarArrayEC_IndexOf(this, Value));
    }

    void TVarArrayEC::DeleteByName(const pas::WideString& Name) {
        std::int32_t i{};
        std::int32_t DataIndex{};
        std::int32_t NameIndex = FindNameOrderIndex(Name);
        if (NameIndex < 0) {
            return;
        }
        std::int32_t Index = EC_Expression::TVarArrayEC_GetNameOrderIndex(this, NameIndex);
        TVarEC* Item = EC_Expression::TVarArrayEC_GetItem(this, Index);
        if (Item != nullptr) {
            pas::free(Item);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(NameIndex, Count - 2); cpp_range.next(i); ) {
            EC_Expression::TVarArrayEC_SetNameOrderIndex(this, i, EC_Expression::TVarArrayEC_GetNameOrderIndex(this, i + 1));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(Index, Count - 2); cpp_range_2.next(i); ) {
            EC_Expression::TVarArrayEC_SetItem(this, i, EC_Expression::TVarArrayEC_GetItem(this, i + 1));
        }
        --Count;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(i); ) {
            DataIndex = EC_Expression::TVarArrayEC_GetNameOrderIndex(this, i);
            if (DataIndex > Index) {
                EC_Expression::TVarArrayEC_SetNameOrderIndex(this, i, DataIndex - 1);
            }
        }
        if (Count < 1) {
            Clear();
        }
    }

    // Takes ownership of Value.
    void TVarArrayEC::AddItem(TVarEC* Value) {
        std::int32_t i{};
        if (Data == nullptr) {
            Data = static_cast<PVarEC>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, (Count + 1) * static_cast<std::int32_t>(sizeof(TVarEC*))));
        } else {
            Data = static_cast<PVarEC>(WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, Data, (Count + 1) * static_cast<std::int32_t>(sizeof(TVarEC*))));
        }
        EC_Expression::TVarArrayEC_SetItem(this, Count, Value);
        std::int32_t InsertionIndex = FindNameInsertionIndex(Value->Name);
        if (InsertionIndex >= Count) {
            ++Count;
            if (NameOrder == nullptr) {
                NameOrder = static_cast<std::int32_t*>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, Count * static_cast<std::int32_t>(sizeof(std::int32_t))));
            } else {
                NameOrder = static_cast<std::int32_t*>(WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, NameOrder, Count * static_cast<std::int32_t>(sizeof(std::int32_t))));
            }
            EC_Expression::TVarArrayEC_SetNameOrderIndex(this, Count - 1, Count - 1);
        } else {
            ++Count;
            NameOrder = static_cast<std::int32_t*>(WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, NameOrder, Count * static_cast<std::int32_t>(sizeof(std::int32_t))));
            for (auto cpp_range = pas::for_downto<std::int32_t>(Count - 1, InsertionIndex + 1); cpp_range.next(i); ) {
                EC_Expression::TVarArrayEC_SetNameOrderIndex(this, i, EC_Expression::TVarArrayEC_GetNameOrderIndex(this, i - 1));
            }
            EC_Expression::TVarArrayEC_SetNameOrderIndex(this, InsertionIndex, Count - 1);
        }
    }

    TVarEC* TVarArrayEC::Add(const pas::WideString& Name, TVarKind Kind) {
        TVarEC* Item = pas::construct_call<TVarEC>(TVarEC_Create, Kind);
        Item->Name = Name;
        try {
            AddItem(Item);
        } catch (...) {
            pas::free(Item);
            throw;
        }
        return Item;
    }

    void TVarArrayEC::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t i{};
        Buffer->AddIntegerValue(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            EC_Expression::TVarArrayEC_GetItem(this, i)->SaveToBuffer(Buffer);
        }
    }

    // Clears existing cells before reading.
    void TVarArrayEC::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t i{};
        TVarEC* Item{};
        Clear();
        std::int32_t ItemCount = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range.next(i); ) {
            Item = pas::construct_call<TVarEC>(TVarEC_Create, vkEmpty);
            Item->LoadFromBuffer(Buffer);
            AddItem(Item);
        }
    }

    void TVarArrayEC::AppendFromBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t i{};
        TVarEC* Item{};
        std::int32_t ItemCount = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range.next(i); ) {
            Item = pas::construct_call<TVarEC>(TVarEC_Create, vkEmpty);
            Item->LoadFromBuffer(Buffer);
            AddItem(Item);
        }
    }

    void TCodeAnalyzerEC_Create(TCodeAnalyzerEC* Self) {
        pas::object_create(Self);
    }

    void TCodeAnalyzerEC_Destroy(TCodeAnalyzerEC* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    // Also frees pooled nodes.
    void TCodeAnalyzerEC::Clear() {
        TCodeAnalyzerUnitEC* Previous{};
        TCodeAnalyzerUnitEC* Token = First;
        while (Token != nullptr) {
            Previous = Token;
            Token = Token->Next;
            pas::free(Previous);
        }
        First = nullptr;
        Last = nullptr;
        Token = FirstFree;
        while (Token != nullptr) {
            Previous = Token;
            Token = Token->Next;
            pas::free(Previous);
        }
        FirstFree = nullptr;
        LastFree = nullptr;
    }

    void TCodeAnalyzerEC::ReserveTokens(std::int32_t Count) {
        TCodeAnalyzerUnitEC* Token{};
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Token = pas::make_object<TCodeAnalyzerUnitEC>();
            if (LastFree != nullptr) {
                LastFree->Next = Token;
            }
            Token->Prev = LastFree;
            Token->Next = nullptr;
            LastFree = Token;
            if (FirstFree == nullptr) {
                FirstFree = Token;
            }
        }
    }

    TCodeAnalyzerUnitEC* TCodeAnalyzerEC::AcquireToken() {
        if (FirstFree == nullptr) {
            ReserveTokens(64);
        }
        TCodeAnalyzerUnitEC* Token = LastFree;
        if (Token->Prev != nullptr) {
            Token->Prev->Next = Token->Next;
        }
        if (Token->Next != nullptr) {
            Token->Next->Prev = Token->Prev;
        }
        if (LastFree == Token) {
            LastFree = Token->Prev;
        }
        if (FirstFree == Token) {
            FirstFree = Token->Next;
        }
        return Token;
    }

    void TCodeAnalyzerEC::RecycleToken(TCodeAnalyzerUnitEC* Token) {
        if (LastFree != nullptr) {
            LastFree->Next = Token;
        }
        Token->Prev = LastFree;
        Token->Next = nullptr;
        LastFree = Token;
        if (FirstFree == nullptr) {
            FirstFree = Token;
        }
    }

    // Retains token storage for reuse.
    void TCodeAnalyzerEC::ClearTokens() {
        while (First != nullptr) {
            DeleteToken(Last);
        }
    }

    TCodeAnalyzerUnitEC* TCodeAnalyzerEC::AddToken() {
        TCodeAnalyzerUnitEC* Token = AcquireToken();
        if (Last != nullptr) {
            Last->Next = Token;
        }
        Token->Prev = Last;
        Token->Next = nullptr;
        Last = Token;
        if (First == nullptr) {
            First = Token;
        }
        return Token;
    }

    void TCodeAnalyzerEC::DeleteToken(TCodeAnalyzerUnitEC* Token) {
        if (Token->Prev != nullptr) {
            Token->Prev->Next = Token->Next;
        }
        if (Token->Next != nullptr) {
            Token->Next->Prev = Token->Prev;
        }
        if (Last == Token) {
            Last = Token->Prev;
        }
        if (First == Token) {
            First = Token->Next;
        }
        RecycleToken(Token);
    }

    // NewlineOffset is added to the source-position base at each newline.
    void TCodeAnalyzerEC_AppendText(TCodeAnalyzerEC* Self, pas::WideString Text, std::int32_t SourceOffset, std::int32_t NewlineOffset) {
        char16_t C{};
        std::int32_t Index{};
        TCodeAnalyzerUnitEC* Token{};
        std::int32_t RunStart{};
        std::int32_t HexValue{};
        std::int32_t HexDigits{};
        std::int32_t TextLength = Text.length();
        std::int32_t QuoteStart = -1;
        RunStart = -1;
        std::int32_t RunLength = 0;
        Index = 0;
        while (Index < TextLength) {
            C = Text.read(Index + 1);
            if (QuoteStart == -1) {
                if (C == u'(') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctOpenParen, Index, SourceOffset, 1);
                } else if (C == u')') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctCloseParen, Index, SourceOffset, 1);
                } else if (C == u'{') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctOpenBrace, Index, SourceOffset, 1);
                } else if (C == u'}') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctCloseBrace, Index, SourceOffset, 1);
                } else if (C == u'[') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctOpenBracket, Index, SourceOffset, 1);
                } else if (C == u']') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctCloseBracket, Index, SourceOffset, 1);
                } else if (C == u'/' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'*') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctBlockCommentStart, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'*' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'/') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctBlockCommentEnd, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'/' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'/') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctLineComment, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'.') {
                    // The native tokenizer counts a dot as two source characters.
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctDot, Index, SourceOffset, 2);
                } else if (C == u'-' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'>') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctArrow, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'&' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'&') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctAnd, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'|' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'|') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctOr, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'+') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctAdd, Index, SourceOffset, 1);
                } else if (C == u'-') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctSubtract, Index, SourceOffset, 1);
                } else if (C == u'*') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctMultiply, Index, SourceOffset, 1);
                } else if (C == u'/') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctDivide, Index, SourceOffset, 1);
                } else if (C == u'%') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctModulo, Index, SourceOffset, 1);
                } else if (C == u'&') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctBitAnd, Index, SourceOffset, 1);
                } else if (C == u'|') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctBitOr, Index, SourceOffset, 1);
                } else if (C == u'^') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctBitXor, Index, SourceOffset, 1);
                } else if (C == u'~') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctBitNot, Index, SourceOffset, 1);
                } else if (C == u'!' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'=') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctNotEqual, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'!') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctNot, Index, SourceOffset, 1);
                } else if (C == u'<' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'<') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctShiftLeft, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'>' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'>') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctShiftRight, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'=' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'=') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctEqual, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'=') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctAssign, Index, SourceOffset, 1);
                } else if (C == u'<' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'=') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctLessEqual, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'>' && Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'=') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctGreaterEqual, Index, SourceOffset, 2);
                    ++Index;
                } else if (C == u'<') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctLess, Index, SourceOffset, 1);
                } else if (C == u'>') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctGreater, Index, SourceOffset, 1);
                } else if (C == u';') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctSemicolon, Index, SourceOffset, 1);
                } else if (C == u':') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctColon, Index, SourceOffset, 1);
                } else if (C == u',') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctComma, Index, SourceOffset, 1);
                } else if (C == u' ' || C == u'\t') {
                    EC_Expression::FlushTokenRun(Self, Text, RunStart, RunLength);
                    RunStart = -1;
                    if (Self->Last == nullptr || Self->Last != nullptr && Self->Last->TokenKind != ctWhitespace) {
                        EC_Expression::EmitSourceToken(Self, Token, ctWhitespace, Index, SourceOffset, 1);
                    } else {
                        ++Self->Last->SourceLength;
                    }
                } else if (C == u'\r' || C == u'\n') {
                    EC_Expression::EmitToken(Self, Text, RunStart, RunLength, Token, ctNewline, Index, SourceOffset, 1);
                    SourceOffset += NewlineOffset;
                    if (Index + 1 < TextLength && (Text.read(Index + 1 + 1) == u'\r' || Text.read(Index + 1 + 1) == u'\n')) {
                        ++Index;
                        ++Token->SourceLength;
                    }
                } else if (C == u'\"' || C == u'\'') {
                    EC_Expression::FlushTokenRun(Self, Text, RunStart, RunLength);
                    Token = Self->AddToken();
                    Token->TokenKind = ctStringLiteral;
                    QuoteStart = Index;
                    Self->Last->SourceStart = Index + SourceOffset;
                    Self->Last->SourceLength = 2;
                    Self->Last->Text = pas::WideString();
                    RunStart = Index + 1;
                    RunLength = 0;
                } else if (C > u' ') {
                    if (Self->Last == nullptr || Self->Last->TokenKind != ctText) {
                        Token = Self->AddToken();
                        Token->TokenKind = ctText;
                        Self->Last->SourceStart = Index + SourceOffset;
                        Self->Last->SourceLength = 0;
                        Self->Last->Text = pas::WideString();
                        RunStart = Index;
                        RunLength = 1;
                    } else {
                        ++RunLength;
                    }
                }
            } else if (Self->Last->TokenKind == ctStringLiteral) {
                ++RunLength;
                if (C == u'\\') {
                    if (Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'\\') {
                        EC_Expression::FlushQuotedRun(Self, Text, RunStart, RunLength);
                        Self->Last->Text = pas::concat_wide({Self->Last->Text, C});
                        Self->Last->SourceLength += 2;
                        ++Index;
                        RunStart = Index + 1;
                        RunLength = 0;
                    } else if (Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'\"') {
                        EC_Expression::FlushQuotedRun(Self, Text, RunStart, RunLength);
                        Self->Last->Text = pas::concat_wide({Self->Last->Text, u"\""});
                        Self->Last->SourceLength += 2;
                        ++Index;
                        RunStart = Index + 1;
                        RunLength = 0;
                    } else if (Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'\'') {
                        EC_Expression::FlushQuotedRun(Self, Text, RunStart, RunLength);
                        Self->Last->Text = pas::concat_wide({Self->Last->Text, u"'"});
                        Self->Last->SourceLength += 2;
                        ++Index;
                        RunStart = Index + 1;
                        RunLength = 0;
                    } else if (Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'n') {
                        EC_Expression::FlushQuotedRun(Self, Text, RunStart, RunLength);
                        Self->Last->Text = pas::concat_wide({Self->Last->Text, u"\r\n"});
                        Self->Last->SourceLength += 2;
                        ++Index;
                        RunStart = Index + 1;
                        RunLength = 0;
                    } else if (Index + 1 < TextLength && Text.read(Index + 1 + 1) == u'x') {
                        EC_Expression::FlushQuotedRun(Self, Text, RunStart, RunLength);
                        Self->Last->SourceLength += 2;
                        Index += 2;
                        HexValue = 0;
                        HexDigits = 0;
                        while (Index < TextLength && HexDigits < 4) {
                            C = Text.read(Index + 1);
                            if (C >= u'0' && C <= u'9') {
                                HexValue = HexValue * 16 + C - '0';
                            } else if (C >= u'a' && C <= u'f') {
                                HexValue = HexValue * 16 + C - 'a' + 10;
                            } else if (C >= u'A' && C <= u'F') {
                                HexValue = HexValue * 16 + C - 'A' + 10;
                            } else {
                                break;
                            }
                            ++HexDigits;
                            ++Self->Last->SourceLength;
                            ++Index;
                        }
                        Self->Last->Text = pas::concat_wide({Self->Last->Text, static_cast<char16_t>(HexValue)});
                        --Index;
                        RunStart = Index + 1;
                        RunLength = 0;
                    }
                } else if (C == u'\"' || C == u'\'') {
                    EC_Expression::FlushQuotedRun(Self, Text, RunStart, RunLength);
                    RunStart = -1;
                    QuoteStart = -1;
                }
            }
            ++Index;
        }
        if (RunStart >= 0 && RunLength > 0) {
            Self->Last->Text = pas::concat_wide({Self->Last->Text, pas::copy(Text, RunStart + 1, RunLength)});
        }
    }

    // Replaces existing tokens; source offsets start at zero.
    void TCodeAnalyzerEC::Tokenize(pas::WideString Text, std::int32_t NewlineOffset) {
        ClearTokens();
        EC_Expression::TCodeAnalyzerEC_AppendText(this, Text, 0, NewlineOffset);
    }

    // Returns an empty string on success.
    pas::WideString TCodeAnalyzerEC::ValidateDelimiters() {
        pas::WideString Result{};
        std::int32_t Depth{};
        pas::DynArray<std::uint8_t> Stack{};
        Result = pas::WideString();
        std::int32_t OpenCount = 0;
        std::int32_t CloseCount = 0;
        TCodeAnalyzerUnitEC* Token = First;
        while (Token != nullptr) {
            if (Token->TokenKind == ctOpenParen || Token->TokenKind == ctOpenBrace || Token->TokenKind == ctOpenBracket || Token->TokenKind == ctBlockCommentStart) {
                ++OpenCount;
            }
            if (Token->TokenKind == ctCloseParen || Token->TokenKind == ctCloseBrace || Token->TokenKind == ctCloseBracket || Token->TokenKind == ctBlockCommentEnd) {
                ++CloseCount;
            }
            Token = Token->Next;
        }
        if (OpenCount != CloseCount) {
            EC_Expression::FormatScriptError(0, Last->SourceStart + Last->SourceLength, Result);
            return Result;
        }
        if (OpenCount < 1) {
            return Result;
        }
        Stack.set_length(OpenCount);
        try {
            Depth = 0;
            Token = First;
            while (Token != nullptr) {
                if (Token->TokenKind == ctOpenParen) {
                    Stack[Depth] = 1;
                    ++Depth;
                } else if (Token->TokenKind == ctOpenBrace) {
                    Stack[Depth] = 2;
                    ++Depth;
                } else if (Token->TokenKind == ctOpenBracket) {
                    Stack[Depth] = 3;
                    ++Depth;
                } else if (Token->TokenKind == ctBlockCommentStart) {
                    Stack[Depth] = 4;
                    ++Depth;
                } else if (Token->TokenKind == ctCloseParen) {
                    // Native checks total openings here, not the current stack depth.
                    if (OpenCount < 1 || Stack[Depth - 1] != 1) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, Result);
                        Stack = nullptr;
                        return Result;
                    }
                    --Depth;
                } else if (Token->TokenKind == ctCloseBrace) {
                    if (OpenCount < 1 || Stack[Depth - 1] != 2) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, Result);
                        Stack = nullptr;
                        return Result;
                    }
                    --Depth;
                } else if (Token->TokenKind == ctCloseBracket) {
                    if (OpenCount < 1 || Stack[Depth - 1] != 3) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, Result);
                        Stack = nullptr;
                        return Result;
                    }
                    --Depth;
                } else if (Token->TokenKind == ctBlockCommentEnd) {
                    if (OpenCount < 1 || Stack[Depth - 1] != 4) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, Result);
                        Stack = nullptr;
                        return Result;
                    }
                    --Depth;
                }
                Token = Token->Next;
            }
            if (Depth > 0) {
                EC_Expression::FormatScriptError(0, Last->SourceStart + Last->SourceLength, Result);
                Stack = nullptr;
                return Result;
            }
        } catch (...) {
            Stack = nullptr;
            throw;
        }
        Stack = nullptr;
        return Result;
    }

    void TCodeAnalyzerEC::RemoveWhitespace() {
        TCodeAnalyzerUnitEC* Previous{};
        TCodeAnalyzerUnitEC* Token = First;
        while (Token != nullptr) {
            Previous = Token;
            Token = Token->Next;
            if (Previous->TokenKind == ctWhitespace) {
                DeleteToken(Previous);
            }
        }
    }

    void TCodeAnalyzerEC::RemoveNewlines() {
        TCodeAnalyzerUnitEC* Previous{};
        TCodeAnalyzerUnitEC* Token = First;
        while (Token != nullptr) {
            Previous = Token;
            Token = Token->Next;
            if (Previous->TokenKind == ctNewline) {
                DeleteToken(Previous);
            }
        }
    }

    // Supports nested block comments.
    void TCodeAnalyzerEC::RemoveComments() {
        TCodeAnalyzerUnitEC* Previous{};
        std::uint8_t LineComment = false;
        std::int32_t CommentDepth = 0;
        TCodeAnalyzerUnitEC* Token = First;
        while (Token != nullptr) {
            Previous = Token;
            Token = Token->Next;
            if (static_cast<std::uint8_t>(LineComment ^ 1) && CommentDepth == 0) {
                if (Previous->TokenKind == ctBlockCommentStart) {
                    CommentDepth = 1;
                    DeleteToken(Previous);
                } else if (Previous->TokenKind == ctLineComment) {
                    LineComment = true;
                    DeleteToken(Previous);
                }
            } else {
                if (CommentDepth != 0 && Previous->TokenKind == ctBlockCommentStart) {
                    ++CommentDepth;
                } else if (LineComment && Previous->TokenKind == ctNewline) {
                    LineComment = false;
                } else if (CommentDepth != 0 && Previous->TokenKind == ctBlockCommentEnd) {
                    --CommentDepth;
                }
                DeleteToken(Previous);
            }
        }
    }

    void TExpressionInstrEC_Destroy(TExpressionInstrEC* Self) {
        Self->Operands = nullptr;
        pas::object_destroy(Self);
    }

    void TExpressionInstrEC::CopyFrom(TExpressionInstrEC* Source) {
        std::int32_t i{};
        Opcode = Source->Opcode;
        OperandCount = Source->OperandCount;
        Operands = nullptr;
        if (Source->Operands != nullptr) {
            Operands.set_length(OperandCount);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, OperandCount - 1); cpp_range.next(i); ) {
                Operands[i] = Source->Operands[i];
            }
        }
    }

    void TExpressionVarEC_Destroy(TExpressionVarEC* Self) {
        if (Self->Kind == evOwned) {
            pas::free(Self->Value);
        }
        Self->Value = nullptr;
        Self->MemberPath = nullptr;
        pas::object_destroy(Self);
    }

    void TExpressionVarEC::CopyFrom(TExpressionVarEC* Source) {
        std::int32_t i{};
        std::int32_t Count{};
        Name = Source->Name;
        Kind = Source->Kind;
        Value = nullptr;
        if (Kind == evNamed) {
            Value = Source->Value;
        } else if (Kind == evOwned) {
            if (Source->Value != nullptr) {
                Value = pas::construct_call<TVarEC>(TVarEC_Create, vkEmpty);
                Value->AssignFrom(Source->Value, false);
            }
        }
        if (Source->MemberPath != nullptr) {
            Count = Source->MemberPath.length() - 1 + 1;
            MemberPath.set_length(Count);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
                MemberPath[i] = Source->MemberPath[i];
            }
        }
    }

    // Replaces Name with its root component. Always returns true.
    std::uint8_t TExpressionVarEC::SplitMemberPath() {
        std::int32_t i{};
        pas::WideString Text{};
        std::uint8_t Result = true;
        Text = Name;
        std::int32_t Count = Text.length();
        std::int32_t Start = 0;
        std::int32_t Stop = Start;
        while (Stop < Count) {
            if (Text.read(Stop + 1) == u'.') {
                break;
            }
            ++Stop;
        }
        if (Stop >= Count) {
            return Result;
        }
        Name = pas::copy(Text, Start + 1, Stop - Start);
        std::int32_t Parts = 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(Stop + 1, Count - 1); cpp_range.next(i); ) {
            if (Text.read(i + 1) == u'.') {
                ++Parts;
            }
        }
        MemberPath.set_length(Parts);
        i = 0;
        while (Stop + 1 < Count) {
            Start = Stop + 1;
            Stop = Start;
            while (Stop < Count) {
                if (Text.read(Stop + 1) == u'.') {
                    break;
                }
                ++Stop;
            }
            MemberPath[i] = pas::copy(Text, Start + 1, Stop - Start);
            ++i;
        }
        return true;
    }

    pas::WideString TExpressionVarEC::GetFullName() {
        pas::WideString Result{};
        std::int32_t i{};
        Result = Name;
        if (MemberPath != nullptr) {
            const std::int32_t cpp_last = MemberPath.length() - 1;
            if (0 <= cpp_last) {
                for (i = 0; i <= cpp_last; ++i) {
                    Result = pas::concat_wide({Result, u".", MemberPath[i]});
                }
            }
        }
        return Result;
    }

    // Only evOwned slots allocate values.
    TVarEC* TExpressionVarEC::Resolve(TVarKind InitialKind) {
        TVarEC* Result{};
        std::int32_t i{};
        if (Value != nullptr) {
            if (MemberPath == nullptr) {
                return Value;
            }
            Result = Value;
            i = 0;
            while (i <= MemberPath.length() - 1) {
                if (EC_Expression::TVarEC_RealVType(Result) == vkClass) {
                    Result = Result->GetClass()->FindVar(MemberPath[i]);
                } else if (EC_Expression::TVarEC_RealVType(Result) == vkFunction) {
                    Result = Result->GetFunction()->FindVar(MemberPath[i]);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not link var :", GetFullName()}))));
                }
                if (Result == nullptr) {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not link var :", GetFullName()}))));
                }
                ++i;
            }
            return Result;
        }
        if (Kind == evOwned) {
            Value = pas::construct_call<TVarEC>(TVarEC_Create, InitialKind);
        } else {
            pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not link var :", GetFullName()}))));
        }
        return Value;
    }

    void TExpressionEC_Create(TExpressionEC* Self) {
        pas::object_create(Self);
    }

    void TExpressionEC_Destroy(TExpressionEC* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    void TExpressionEC::Clear() {
        while (VariableCount > 0) {
            DeleteVariable(VariableCount - 1);
        }
        if (!SharedInstructions) {
            while (InstructionCount > 0) {
                DeleteInstruction(InstructionCount - 1);
            }
        }
        ResultIndex = -1;
        SharedInstructions = false;
    }

    void TExpressionEC::CopyFrom(TExpressionEC* Source) {
        std::int32_t i{};
        Clear();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Source->VariableCount - 1); cpp_range.next(i); ) {
            TExpressionVarEC* variable = EC_Expression::TExpressionEC_GetVariable(Source, i);
            TExpressionVarEC* variable_2 = EC_Expression::TExpressionEC_GetVariable(this, AddVariable());
            variable_2->CopyFrom(variable);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Source->InstructionCount - 1); cpp_range_2.next(i); ) {
            TExpressionInstrEC* instruction = EC_Expression::TExpressionEC_GetInstruction(Source, i);
            TExpressionInstrEC* instruction_2 = EC_Expression::TExpressionEC_GetInstruction(this, AddInstruction());
            instruction_2->CopyFrom(instruction);
        }
        ResultIndex = Source->ResultIndex;
    }

    // Borrows Source's instruction array.
    void TExpressionEC::CopyFromFast(TExpressionEC* Source) {
        std::int32_t i{};
        Clear();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Source->VariableCount - 1); cpp_range.next(i); ) {
            TExpressionVarEC* variable = EC_Expression::TExpressionEC_GetVariable(Source, i);
            TExpressionVarEC* variable_2 = EC_Expression::TExpressionEC_GetVariable(this, AddVariable());
            variable_2->CopyFrom(variable);
        }
        InstructionCount = Source->InstructionCount;
        Instructions = Source->Instructions;
        SharedInstructions = true;
        ResultIndex = Source->ResultIndex;
    }

    // Returns a zero-based index; the new slot starts with zero-initialized evNamed kind.
    std::int32_t TExpressionEC::AddVariable() {
        ++VariableCount;
        if (Variables == nullptr) {
            Variables = static_cast<TExpressionVarEC**>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, VariableCount * static_cast<std::int32_t>(sizeof(TExpressionVarEC*))));
        } else {
            Variables = static_cast<TExpressionVarEC**>(WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, Variables, VariableCount * static_cast<std::int32_t>(sizeof(TExpressionVarEC*))));
        }
        EC_Expression::TExpressionEC_SetVariable(this, VariableCount - 1, pas::make_object<TExpressionVarEC>());
        return VariableCount - 1;
    }

    void TExpressionEC::DeleteVariable(std::int32_t Index) {
        std::int32_t i{};
        if (Index < 0 || Index >= VariableCount) {
            return;
        }
        pas::free(EC_Expression::TExpressionEC_GetVariable(this, Index));
        for (auto cpp_range = pas::for_to<std::int32_t>(Index, VariableCount - 2); cpp_range.next(i); ) {
            EC_Expression::TExpressionEC_SetVariable(this, i, EC_Expression::TExpressionEC_GetVariable(this, i + 1));
        }
        --VariableCount;
        if (VariableCount <= 0) {
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Variables);
            Variables = nullptr;
        }
    }

    std::int32_t TExpressionEC::AddInstruction() {
        ++InstructionCount;
        if (Instructions == nullptr) {
            Instructions = static_cast<TExpressionInstrEC**>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, InstructionCount * static_cast<std::int32_t>(sizeof(TExpressionInstrEC*))));
        } else {
            Instructions = static_cast<TExpressionInstrEC**>(WindowsSdk::HeapReAlloc(WindowsSdk::GetProcessHeap(), 0u, Instructions, InstructionCount * static_cast<std::int32_t>(sizeof(TExpressionInstrEC*))));
        }
        EC_Expression::TExpressionEC_SetInstruction(this, InstructionCount - 1, pas::make_object<TExpressionInstrEC>());
        return InstructionCount - 1;
    }

    void TExpressionEC::DeleteInstruction(std::int32_t Index) {
        std::int32_t i{};
        if (Index < 0 || Index >= InstructionCount) {
            return;
        }
        pas::free(EC_Expression::TExpressionEC_GetInstruction(this, Index));
        for (auto cpp_range = pas::for_to<std::int32_t>(Index, InstructionCount - 2); cpp_range.next(i); ) {
            EC_Expression::TExpressionEC_SetInstruction(this, i, EC_Expression::TExpressionEC_GetInstruction(this, i + 1));
        }
        --InstructionCount;
        if (InstructionCount <= 0) {
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Instructions);
            Instructions = nullptr;
        }
    }

    // Extract whole conditions: a helper inside an and/or chain adds DCC32 temporaries.
    std::uint8_t IsBinaryToken(TCodeAnalyzerUnitEC* Token) {
        // The native test includes ctSubtract twice.
        return Token->TokenKind == ctAdd || Token->TokenKind == ctSubtract || Token->TokenKind == ctMultiply || Token->TokenKind == ctDivide || Token->TokenKind == ctModulo || Token->TokenKind == ctSubtract || Token->TokenKind == ctBitAnd || Token->TokenKind == ctBitOr || Token->TokenKind == ctBitXor || Token->TokenKind == ctAnd || Token->TokenKind == ctOr || Token->TokenKind == ctShiftLeft || Token->TokenKind == ctShiftRight || Token->TokenKind == ctEqual || Token->TokenKind == ctNotEqual || Token->TokenKind == ctLess || Token->TokenKind == ctGreater || Token->TokenKind == ctLessEqual || Token->TokenKind == ctGreaterEqual;
    }

    std::uint8_t IsUnaryMinusPosition(TCompilerUnitEC* Item) {
        return Item->Prev == nullptr || Item->Prev->Kind != cuIntLiteral && Item->Prev->Kind != cuDwordLiteral && Item->Prev->Kind != cuFloatLiteral && Item->Prev->Kind != cuCloseParen && Item->Prev->Kind != cuCloseBracket && Item->Prev->Kind != cuName;
    }

    std::uint8_t InvalidBinaryOperands(TCompilerUnitEC* Item) {
        return Item->Prev == nullptr || Item->Next == nullptr || !(Item->Prev->Kind == cuName || Item->Prev->Kind == cuIntLiteral || Item->Prev->Kind == cuDwordLiteral || Item->Prev->Kind == cuFloatLiteral || Item->Prev->Kind == cuStringLiteral || Item->Prev->Kind == cuCloseParen || Item->Prev->Kind == cuCloseBracket) || !(Item->Next->Kind == cuName || Item->Next->Kind == cuIntLiteral || Item->Next->Kind == cuDwordLiteral || Item->Next->Kind == cuFloatLiteral || Item->Next->Kind == cuStringLiteral || Item->Next->Kind == cuOpenParen || Item->Next->Kind == cuCall || Item->Next->Kind == cuIndex || Item->Next->Kind == cuUnaryOperator);
    }

    std::uint8_t InvalidAssignmentOperands(TCompilerUnitEC* Item) {
        return Item->Prev == nullptr || Item->Next == nullptr || !(Item->Prev->Kind == cuName || Item->Prev->Kind == cuCloseBracket) || !(Item->Next->Kind == cuName || Item->Next->Kind == cuIntLiteral || Item->Next->Kind == cuDwordLiteral || Item->Next->Kind == cuFloatLiteral || Item->Next->Kind == cuStringLiteral || Item->Next->Kind == cuOpenParen || Item->Next->Kind == cuCall || Item->Next->Kind == cuIndex || Item->Next->Kind == cuUnaryOperator);
    }

    std::uint8_t InvalidUnaryOperand(TCompilerUnitEC* Item) {
        return Item->Kind == cuUnaryOperator && (Item->Next == nullptr || !(Item->Next->Kind == cuName || Item->Next->Kind == cuIntLiteral || Item->Next->Kind == cuDwordLiteral || Item->Next->Kind == cuFloatLiteral || Item->Next->Kind == cuStringLiteral || Item->Next->Kind == cuOpenParen || Item->Next->Kind == cuCall || Item->Next->Kind == cuIndex || Item->Next->Kind == cuUnaryOperator));
    }

    // Callers exit immediately after this; Compiler has been freed.
    void RejectExpression(TCompilerEC*& Compiler, std::int32_t SourceStart, pas::WideString& ErrorText) {
        EC_Expression::FormatScriptError(0, SourceStart, ErrorText);
        pas::free(Compiler);
    }

    // EndToken is exclusive; nil FirstToken starts at Analyzer.First. NextToken may be nil. Clears the previous expression before compiling.
    void TExpressionEC::Compile(TCodeAnalyzerEC* Analyzer, TCodeAnalyzerUnitEC* FirstToken, TCodeAnalyzerUnitEC* EndToken, PCodeAnalyzerUnitEC NextToken, pas::WideString& ErrorText) {
        TCodeAnalyzerUnitEC* Next{};
        TCompilerEC* Compiler{};
        TCompilerUnitEC* Item{};
        TCompilerUnitEC* Reduced{};
        TCompilerUnitEC* Closing{};
        std::int32_t ResultSlot{};
        std::int32_t ArgumentCount{};
        std::int32_t OperandIndex{};
        TExpressionVarEC* Slot{};
        TExpressionInstrEC* Instruction{};
        std::int32_t IntValue{};
        std::uint32_t DwordValue{};
        double FloatValue{};
        pas::WideString Text{};
        Clear();
        ErrorText = pas::WideString();
        if (FirstToken == nullptr) {
            FirstToken = Analyzer->First;
        }
        if (FirstToken == nullptr) {
            EC_Expression::FormatScriptError(0, 0, ErrorText);
            return;
        }
        Compiler = pas::construct_call<TCompilerEC>(TCompilerEC_Create);
        std::int32_t Depth = 0;
        TCodeAnalyzerUnitEC* Token = FirstToken;
        while (Token != EndToken) {
            if (Token->TokenKind != ctBlockCommentStart && Token->TokenKind != ctLineComment) {
                if (Token->TokenKind == ctStringLiteral) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuStringLiteral;
                    Item->Text = Token->Text;
                } else if (Token->TokenKind == ctComma) {
                    if (Depth <= 0) {
                        break;
                    }
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuComma;
                } else if (Token->TokenKind == ctAssign) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuAssignment;
                } else if (Token->TokenKind == ctOpenParen) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuOpenParen;
                    ++Depth;
                } else if (Token->TokenKind == ctCloseParen) {
                    --Depth;
                    if (Depth < 0) {
                        break;
                    }
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuCloseParen;
                } else if (Token->TokenKind == ctOpenBracket) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuOpenBracket;
                    ++Depth;
                } else if (Token->TokenKind == ctCloseBracket) {
                    --Depth;
                    if (Depth < 0) {
                        break;
                    }
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuCloseBracket;
                } else if (Token->TokenKind == ctAssign) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuAssignment;
                } else if (Token->TokenKind == ctBitNot || Token->TokenKind == ctNot) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuUnaryOperator;
                    Item->OperatorToken = Token->TokenKind;
                } else if (EC_Expression::IsBinaryToken(Token)) {
                    Item = Compiler->AddUnit();
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->SourceLength;
                    Item->Kind = cuBinaryOperator;
                    Item->OperatorToken = Token->TokenKind;
                } else if (Token->TokenKind == ctText) {
                    Next = Token;
                    if (EC_Expression::TryReadFloatLiteral(Next, FloatValue)) {
                        Item = Compiler->AddUnit();
                        Item->SourceStart = Token->SourceStart;
                        if (Next == nullptr) {
                            Item->SourceLength = Analyzer->Last->SourceStart + Analyzer->Last->SourceLength - Token->SourceStart;
                        } else {
                            Item->SourceLength = Next->Prev->SourceStart + Next->Prev->SourceLength - Token->SourceStart;
                        }
                        Item->Kind = cuFloatLiteral;
                        Item->FloatValue = FloatValue;
                        Token = Next;
                        continue;
                    } else if (EC_Expression::TryReadDwordLiteral(Next, DwordValue)) {
                        Item = Compiler->AddUnit();
                        Item->SourceStart = Token->SourceStart;
                        if (Next == nullptr) {
                            Item->SourceLength = Analyzer->Last->SourceStart + Analyzer->Last->SourceLength - Token->SourceStart;
                        } else {
                            Item->SourceLength = Next->Prev->SourceStart + Next->Prev->SourceLength - Token->SourceStart;
                        }
                        Item->Kind = cuDwordLiteral;
                        Item->DwordValue = DwordValue;
                        Token = Next;
                        continue;
                    } else if (EC_Expression::TryReadIntegerLiteral(Next, IntValue)) {
                        Item = Compiler->AddUnit();
                        Item->SourceStart = Token->SourceStart;
                        if (Next == nullptr) {
                            Item->SourceLength = Analyzer->Last->SourceStart + Analyzer->Last->SourceLength - Token->SourceStart;
                        } else {
                            Item->SourceLength = Next->Prev->SourceStart + Next->Prev->SourceLength - Token->SourceStart;
                        }
                        Item->Kind = cuIntLiteral;
                        Item->IntValue = IntValue;
                        Token = Next;
                        continue;
                    } else if (EC_Expression::TryReadStringLiteral(Next, Text)) {
                        Item = Compiler->AddUnit();
                        Item->SourceStart = Token->SourceStart;
                        if (Next == nullptr) {
                            Item->SourceLength = Analyzer->Last->SourceStart + Analyzer->Last->SourceLength - Token->SourceStart;
                        } else {
                            Item->SourceLength = Next->Prev->SourceStart + Next->Prev->SourceLength - Token->SourceStart;
                        }
                        Item->Kind = cuStringLiteral;
                        Item->Text = Text;
                        Token = Next;
                        continue;
                    } else if (EC_Expression::TryReadMemberName(Next, Text)) {
                        Item = Compiler->AddUnit();
                        Item->SourceStart = Token->SourceStart;
                        if (Next == nullptr) {
                            Item->SourceLength = Analyzer->Last->SourceStart + Analyzer->Last->SourceLength - Token->SourceStart;
                        } else {
                            Item->SourceLength = Next->Prev->SourceStart + Next->Prev->SourceLength - Token->SourceStart;
                        }
                        Item->Kind = cuName;
                        Item->Text = Text;
                        Token = Next;
                        continue;
                    } else {
                        EC_Expression::RejectExpression(Compiler, Token->SourceStart, ErrorText);
                        return;
                    }
                } else {
                    if (Token->TokenKind == ctSemicolon) {
                        break;
                    }
                    if (Token->TokenKind != ctNewline && Token->TokenKind != ctWhitespace) {
                        EC_Expression::RejectExpression(Compiler, Token->SourceStart, ErrorText);
                        return;
                    }
                }
            }
            Token = Token->Next;
        }
        if (NextToken != nullptr) {
            pas::store_unaligned<TCodeAnalyzerUnitEC*>(NextToken, Token);
        }
        Depth = 0;
        Item = Compiler->First;
        while (Item != nullptr) {
            if (Item->Kind == cuOpenParen && Item->Prev != nullptr && Item->Prev->Kind == cuName) {
                Item->Kind = cuCall;
                Item->Text = Item->Prev->Text;
                Compiler->DeleteUnit(Item->Prev);
                ++Depth;
            } else if (Item->Kind == cuOpenBracket) {
                if (Item->Prev != nullptr && Item->Prev->Kind == cuName) {
                    Item->Kind = cuIndex;
                    Item->Text = Item->Prev->Text;
                    Compiler->DeleteUnit(Item->Prev);
                    ++Depth;
                } else {
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
            } else if (Item->Kind == cuOpenParen) {
                ++Depth;
            } else if (Item->Kind == cuCloseParen) {
                --Depth;
            } else if (Item->Kind == cuCloseBracket) {
                --Depth;
            }
            Item = Item->Next;
        }
        if (Depth != 0) {
            if (Token == nullptr) {
                Token = Analyzer->Last;
            }
            EC_Expression::RejectExpression(Compiler, Token->SourceStart + Token->SourceLength, ErrorText);
            return;
        }
        Item = Compiler->First;
        while (Item != nullptr) {
            if (Item->Kind == cuBinaryOperator && Item->OperatorToken == ctSubtract && Item->Next != nullptr && (Item->Next->Kind == cuIntLiteral || Item->Next->Kind == cuFloatLiteral)) {
                if (EC_Expression::IsUnaryMinusPosition(Item)) {
                    Item = Item->Next;
                    Compiler->DeleteUnit(Item->Prev);
                    Item->IntValue = -Item->IntValue;
                    Item->FloatValue = -Item->FloatValue;
                }
            } else if (Item->Kind == cuBinaryOperator && Item->OperatorToken == ctSubtract && Item->Next != nullptr && (Item->Next->Kind == cuIntLiteral || Item->Next->Kind == cuDwordLiteral || Item->Next->Kind == cuFloatLiteral || Item->Next->Kind == cuOpenParen || Item->Next->Kind == cuCall || Item->Next->Kind == cuIndex || Item->Next->Kind == cuName)) {
                if (EC_Expression::IsUnaryMinusPosition(Item)) {
                    Item->Kind = cuUnaryOperator;
                }
            }
            Item = Item->Next;
        }
        Item = Compiler->First;
        while (Item != nullptr) {
            if (Item->Kind == cuBinaryOperator) {
                if (EC_Expression::InvalidBinaryOperands(Item)) {
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
            } else if (Item->Kind == cuAssignment) {
                if (EC_Expression::InvalidAssignmentOperands(Item)) {
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
            } else if (EC_Expression::InvalidUnaryOperand(Item)) {
                EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                return;
            }
            Item = Item->Next;
        }
        Item = Compiler->First;
        while (Item != nullptr) {
            if (Item->Kind == cuIntLiteral) {
                Item->Kind = cuVariable;
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evOwned;
                Slot->Value = pas::construct_call<TVarEC>(TVarEC_Create, vkInt);
                Slot->Value->SetInt(Item->IntValue);
            } else if (Item->Kind == cuDwordLiteral) {
                Item->Kind = cuVariable;
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evOwned;
                Slot->Value = pas::construct_call<TVarEC>(TVarEC_Create, vkDword);
                Slot->Value->SetDword(Item->DwordValue);
            } else if (Item->Kind == cuFloatLiteral) {
                Item->Kind = cuVariable;
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evOwned;
                Slot->Value = pas::construct_call<TVarEC>(TVarEC_Create, vkFloat);
                Slot->Value->SetFloat(Item->FloatValue);
            } else if (Item->Kind == cuStringLiteral) {
                Item->Kind = cuVariable;
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evOwned;
                Slot->Value = pas::construct_call<TVarEC>(TVarEC_Create, vkString);
                Slot->Value->SetString(Item->Text);
            } else if (Item->Kind == cuName) {
                Item->Kind = cuVariable;
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evNamed;
                Slot->Name = Item->Text;
                if (!Slot->SplitMemberPath()) {
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
            } else if (Item->Kind == cuCall) {
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evNamed;
                Slot->Name = Item->Text;
                if (!Slot->SplitMemberPath()) {
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
            } else if (Item->Kind == cuIndex) {
                Item->VariableIndex = AddVariable();
                Slot = EC_Expression::TExpressionEC_GetVariable(this, Item->VariableIndex);
                Slot->Kind = evNamed;
                Slot->Name = Item->Text;
                if (!Slot->SplitMemberPath()) {
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
            }
            Item = Item->Next;
        }
        if (Compiler->First == nullptr) {
            EC_Expression::RejectExpression(Compiler, 0, ErrorText);
            return;
        }
        while (Compiler->First->Next != nullptr) {
            Item = Compiler->FindReducibleIndex();
            if (Item == nullptr) {
                Item = Compiler->FindReducibleCall();
            }
            if (Item == nullptr) {
                Item = Compiler->FindReducibleOperator();
            }
            if (Item == nullptr) {
                Clear();
                if (Compiler->First == nullptr) {
                    ErrorText = u"Unknown error"_w;
                } else {
                    EC_Expression::FormatScriptError(0, Compiler->First->SourceStart, ErrorText);
                }
                pas::free(Compiler);
                return;
            }
            if (Item->Kind == cuCall || Item->Kind == cuIndex) {
                ArgumentCount = 0;
                Closing = Item->Next;
                while (Closing != nullptr) {
                    if (Closing->Kind == cuVariable) {
                        ++ArgumentCount;
                    } else if (Closing->Kind == cuCloseParen || Closing->Kind == cuCloseBracket) {
                        break;
                    }
                    Closing = Closing->Next;
                }
                if (Item->Kind == cuIndex && ArgumentCount < 1) {
                    Clear();
                    EC_Expression::RejectExpression(Compiler, Item->SourceStart, ErrorText);
                    return;
                }
                ResultSlot = AddVariable();
                if (Item->Kind == cuCall) {
                    EC_Expression::TExpressionEC_GetVariable(this, ResultSlot)->Kind = evOwned;
                } else {
                    EC_Expression::TExpressionEC_GetVariable(this, ResultSlot)->Kind = evIndexed;
                }
                Instruction = EC_Expression::TExpressionEC_GetInstruction(this, AddInstruction());
                if (Item->Kind == cuCall) {
                    Instruction->Opcode = eoCall;
                } else {
                    Instruction->Opcode = eoIndex;
                }
                Instruction->OperandCount = ArgumentCount + 2;
                Instruction->Operands.set_length(ArgumentCount + 2);
                Instruction->Operands[0] = ResultSlot;
                Instruction->Operands[1] = Item->VariableIndex;
                OperandIndex = 2;
                Reduced = Item->Next;
                while (Reduced != nullptr) {
                    if (Reduced->Kind == cuVariable) {
                        Instruction->Operands[OperandIndex] = Reduced->VariableIndex;
                        ++OperandIndex;
                    } else if (Reduced->Kind == cuCloseParen || Reduced->Kind == cuCloseBracket) {
                        break;
                    }
                    Reduced = Reduced->Next;
                }
                Item->Kind = cuVariable;
                Item->Text = pas::WideString();
                Item->VariableIndex = ResultSlot;
                Reduced = Item;
                while (Closing != Reduced) {
                    Item = Closing;
                    Closing = Closing->Prev;
                    Compiler->DeleteUnit(Item);
                }
            } else if (Item->Kind == cuUnaryOperator) {
                ResultSlot = AddVariable();
                EC_Expression::TExpressionEC_GetVariable(this, ResultSlot)->Kind = evOwned;
                Instruction = EC_Expression::TExpressionEC_GetInstruction(this, AddInstruction());
                if (Item->OperatorToken == ctSubtract) {
                    Instruction->Opcode = eoNegate;
                } else {
                    EC_Expression::InitInstr(Instruction, Item->OperatorToken);
                }
                Instruction->OperandCount = 2;
                Instruction->Operands.set_length(2);
                Instruction->Operands[0] = ResultSlot;
                Instruction->Operands[1] = Item->Next->VariableIndex;
                Item->Next->VariableIndex = ResultSlot;
                Reduced = Item->Next;
                Compiler->DeleteUnit(Item);
            } else if (Item->Kind == cuAssignment) {
                Instruction = EC_Expression::TExpressionEC_GetInstruction(this, AddInstruction());
                Instruction->Opcode = eoAssign;
                Instruction->OperandCount = 2;
                Instruction->Operands.set_length(2);
                Instruction->Operands[0] = Item->Prev->VariableIndex;
                Instruction->Operands[1] = Item->Next->VariableIndex;
                Reduced = Item->Prev;
                Compiler->DeleteUnit(Item->Next);
                Compiler->DeleteUnit(Item);
            } else {
                ResultSlot = AddVariable();
                EC_Expression::TExpressionEC_GetVariable(this, ResultSlot)->Kind = evOwned;
                Instruction = EC_Expression::TExpressionEC_GetInstruction(this, AddInstruction());
                EC_Expression::InitInstr(Instruction, Item->OperatorToken);
                Instruction->OperandCount = 3;
                Instruction->Operands.set_length(3);
                Instruction->Operands[0] = ResultSlot;
                Instruction->Operands[1] = Item->Prev->VariableIndex;
                Instruction->Operands[2] = Item->Next->VariableIndex;
                Item->Prev->VariableIndex = ResultSlot;
                Reduced = Item->Prev;
                Compiler->DeleteUnit(Item->Next);
                Compiler->DeleteUnit(Item);
            }
            while (Reduced != nullptr && Reduced->Prev != nullptr && Reduced->Prev->Kind == cuOpenParen && Reduced->Next != nullptr && Reduced->Next->Kind == cuCloseParen) {
                Compiler->DeleteUnit(Reduced->Prev);
                Compiler->DeleteUnit(Reduced->Next);
            }
        }
        ResultIndex = Compiler->First->VariableIndex;
        pas::free(Compiler);
    }

    void TExpressionEC::Link(TVarArrayEC* Scope, std::uint8_t OnlyUnlinked) {
        std::int32_t i{};
        TExpressionVarEC* Slot{};
        TVarEC* Found{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, VariableCount - 1); cpp_range.next(i); ) {
            Slot = EC_Expression::TExpressionEC_GetVariable(this, i);
            if (static_cast<std::uint8_t>(OnlyUnlinked ^ 1) || Slot->Value == nullptr) {
                if (Slot->Kind == evNamed) {
                    Found = Scope->GetVarNE(Slot->Name);
                    if (Found != nullptr) {
                        Slot->Value = Found;
                    }
                }
            }
        }
    }

    void TExpressionEC_Evaluate(TExpressionEC* Self, TCodeProcessEC* Process, TCodeEC* Code, TScriptDebugState* DebugContext) {
        pas::DynArray<std::uint32_t> LibraryArguments{};
        std::uint32_t LibraryWord{};
        std::int32_t j{};
        TExpressionInstrEC* Instruction{};
        TExpressionVarEC* Dest{};
        TExpressionVarEC* Left{};
        TExpressionVarEC* Right{};
        pas::DynArray<TVarEC*> Arguments{};
        TVarEC* Value{};
        TVarEC* IndexValue{};
        TVarEC* Callee{};
        TVarEC* Argument{};
        TCodeEC* Invocation{};
        TVarKind ResultKind{};
        float SingleValue{};
        std::int32_t i = 0;
        while (i < Self->VariableCount) {
            Dest = EC_Expression::TExpressionEC_GetVariable(Self, i);
            if (Dest->Kind == evIndexed) {
                Dest->Value = nullptr;
            }
            ++i;
        }
        i = 0;
        while (i < Self->InstructionCount) {
            Instruction = EC_Expression::TExpressionEC_GetInstruction(Self, i);
            if (Instruction->Opcode == eoNegate || Instruction->Opcode == eoBitNot || Instruction->Opcode == eoNot) {
                Dest = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[0]);
                Left = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[1]);
                switch (Instruction->Opcode) {
                    case eoNegate: {
                        TVarEC* resolve = Left->Resolve(vkEmpty);
                        TVarEC* resolve_2 = Dest->Resolve(EC_Expression::TVarEC_RealVType(Left->Value));
                        EC_Expression::TVarEC_OMinus(resolve_2, resolve);
                        break;
                    }
                    case eoBitNot: {
                        TVarEC* resolve_3 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_4 = Dest->Resolve(EC_Expression::TVarEC_RealVType(Left->Value));
                        EC_Expression::TVarEC_OBitNot(resolve_4, resolve_3);
                        break;
                    }
                    case eoNot: {
                        TVarEC* resolve_5 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_6 = Dest->Resolve(EC_Expression::TVarEC_RealVType(Left->Value));
                        EC_Expression::TVarEC_ONot(resolve_6, resolve_5);
                        break;
                    }
                }
            } else if (Instruction->Opcode != eoCall && Instruction->Opcode != eoAssign && Instruction->Opcode != eoIndex) {
                Dest = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[0]);
                Left = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[1]);
                Right = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[2]);
                ResultKind = vkEmpty;
                if (Dest->Value == nullptr && Dest->Kind == evOwned) {
                    if (Instruction->Opcode == eoAdd || Instruction->Opcode == eoSubtract || Instruction->Opcode == eoMultiply || Instruction->Opcode == eoDivide) {
                        ResultKind = EC_Expression::TVarEC_RealVType(Left->Resolve(vkEmpty));
                    } else {
                        ResultKind = vkInt;
                    }
                }
                switch (Instruction->Opcode) {
                    case eoAdd: {
                        TVarEC* resolve_7 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_8 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_9 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OAdd(resolve_9, resolve_8, resolve_7);
                        break;
                    }
                    case eoSubtract: {
                        TVarEC* resolve_10 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_11 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_12 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OSub(resolve_12, resolve_11, resolve_10);
                        break;
                    }
                    case eoMultiply: {
                        TVarEC* resolve_13 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_14 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_15 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OMul(resolve_15, resolve_14, resolve_13);
                        break;
                    }
                    case eoDivide: {
                        TVarEC* resolve_16 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_17 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_18 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_ODiv(resolve_18, resolve_17, resolve_16);
                        break;
                    }
                    case eoModulo: {
                        TVarEC* resolve_19 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_20 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_21 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OMod(resolve_21, resolve_20, resolve_19);
                        break;
                    }
                    case eoBitAnd: {
                        TVarEC* resolve_22 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_23 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_24 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OBitAnd(resolve_24, resolve_23, resolve_22);
                        break;
                    }
                    case eoBitOr: {
                        TVarEC* resolve_25 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_26 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_27 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OBitOr(resolve_27, resolve_26, resolve_25);
                        break;
                    }
                    case eoBitXor: {
                        TVarEC* resolve_28 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_29 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_30 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OBitXor(resolve_30, resolve_29, resolve_28);
                        break;
                    }
                    case eoAnd: {
                        TVarEC* resolve_31 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_32 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_33 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OAnd(resolve_33, resolve_32, resolve_31);
                        break;
                    }
                    case eoOr: {
                        TVarEC* resolve_34 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_35 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_36 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OOr(resolve_36, resolve_35, resolve_34);
                        break;
                    }
                    case eoShiftLeft: {
                        TVarEC* resolve_37 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_38 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_39 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OShl(resolve_39, resolve_38, resolve_37);
                        break;
                    }
                    case eoShiftRight: {
                        TVarEC* resolve_40 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_41 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_42 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OShr(resolve_42, resolve_41, resolve_40);
                        break;
                    }
                    case eoEqual: {
                        TVarEC* resolve_43 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_44 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_45 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OEqual(resolve_45, resolve_44, resolve_43);
                        break;
                    }
                    case eoNotEqual: {
                        TVarEC* resolve_46 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_47 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_48 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_ONotEqual(resolve_48, resolve_47, resolve_46);
                        break;
                    }
                    case eoLess: {
                        TVarEC* resolve_49 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_50 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_51 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OLess(resolve_51, resolve_50, resolve_49);
                        break;
                    }
                    case eoGreater: {
                        TVarEC* resolve_52 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_53 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_54 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OMore(resolve_54, resolve_53, resolve_52);
                        break;
                    }
                    case eoLessEqual: {
                        TVarEC* resolve_55 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_56 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_57 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OLessEqual(resolve_57, resolve_56, resolve_55);
                        break;
                    }
                    case eoGreaterEqual: {
                        TVarEC* resolve_58 = Right->Resolve(vkEmpty);
                        TVarEC* resolve_59 = Left->Resolve(vkEmpty);
                        TVarEC* resolve_60 = Dest->Resolve(ResultKind);
                        EC_Expression::TVarEC_OMoreEqual(resolve_60, resolve_59, resolve_58);
                        break;
                    }
                }
            } else if (Instruction->Opcode == eoAssign) {
                Dest = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[0]);
                Left = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[1]);
                {
                    TVarEC* resolve_61 = Left->Resolve(vkEmpty);
                    TVarEC* resolve_62 = Dest->Resolve(vkEmpty);
                    resolve_62->Assume(resolve_61, false);
                }
            } else if (Instruction->Opcode == eoIndex) {
                Dest = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[0]);
                Left = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[1]);
                Value = Left->Resolve(vkEmpty);
                if (EC_Expression::TVarEC_RealVType(Value) != vkArray) {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not array:", Left->Name}))));
                }
                for (auto cpp_range = pas::for_to<std::int32_t>(2, Instruction->OperandCount - 1); cpp_range.next(j); ) {
                    Right = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[j]);
                    IndexValue = Right->Resolve(vkEmpty);
                    if (EC_Expression::TVarEC_RealVType(IndexValue) == vkString) {
                        Value = ([&] {
                            const pas::WideString& string = IndexValue->GetString();
                            TVarArrayEC* array = Value->GetArray();
                            return array->GetVarNE(string);
                        }());
                        if (Value == nullptr) {
                            pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error array. name=", Left->Resolve(vkEmpty)->Name, u" index=", Right->Value->GetString(), u" level=", pas::wide_int_to_str(j - 1)}))));
                        }
                    } else {
                        {
                            std::int32_t pasint = IndexValue->GetInt();
                            TVarArrayEC* array_2 = Value->GetArray();
                            Value = array_2->GetItemNE(pasint);
                        }
                        if (Value == nullptr) {
                            pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error array. name=", Left->Resolve(vkEmpty)->Name, u" index=", pas::wide_int_to_str(Right->Value->GetInt()), u" level=", pas::wide_int_to_str(j - 1)}))));
                        }
                    }
                    if (j != Instruction->OperandCount - 1 && EC_Expression::TVarEC_RealVType(Value) != vkArray) {
                        pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error array:", Left->Name}))));
                    }
                }
                Dest->Value = Value;
            } else {
                Dest = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[0]);
                Left = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[1]);
                Dest->Resolve(vkEmpty);
                Callee = Left->Resolve(vkEmpty);
                if (EC_Expression::TVarEC_RealVType(Callee) == vkLibraryFun) {
                    Value = EC_Expression::TVarEC_Resolve(Callee);
                    if (Value->LibraryFunData.length() - 1 + 1 - 2 != Instruction->OperandCount - 2) {
                        pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Count variable : ", Left->Name}))));
                    }
                    LibraryArguments.set_length(Instruction->OperandCount - 2);
                    {
                        const std::int32_t cpp_first = Instruction->OperandCount - 2 - 1;
                        if (cpp_first >= 0) {
                            for (j = cpp_first; j >= 0; --j) {
                                Argument = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[j + 2])->Resolve(vkEmpty);
                                switch (static_cast<TLibraryValueKind>(Value->LibraryFunData[2 + j])) {
                                    case lvInt: LibraryWord = Argument->GetInt(); break;
                                    case lvDword: LibraryWord = Argument->GetDword(); break;
                                    case lvFloat: {
                                        SingleValue = Argument->GetFloat();
                                        LibraryWord = pas::load_unaligned<std::uint32_t>(reinterpret_cast<WindowsSdk::PDWORD>(&SingleValue));
                                        break;
                                    }
                                    case lvString: {
                                        IndexValue = EC_Expression::TVarEC_Resolve(Argument);
                                        if (IndexValue->Kind != vkString) {
                                            pas::raise(pas::make_exception<ExceptionExpressionEC>("Variable not string"_a));
                                        }
                                        if (IndexValue->StringValue.length() <= 0) {
                                            LibraryWord = 0u;
                                        } else {
                                            LibraryWord = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(IndexValue->StringValue.pchar()));
                                        }
                                        break;
                                    }
                                    case lvRef: {
                                        LibraryWord = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Argument));
                                        break;
                                    }
                                    case lvCode: {
                                        LibraryWord = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Code));
                                        break;
                                    }
                                    default: LibraryWord = 0u; break;
                                }
                                // The imported function consumes its dynamically constructed argument stack.
                                LibraryArguments[j] = LibraryWord;
                            }
                        }
                    }
                    ScriptCallTrace[ScriptCallTracePosition] = Callee;
                    ScriptCallTraceCount = std::min<std::int32_t>(20, ScriptCallTraceCount + 1);
                    ScriptCallTracePosition = (ScriptCallTracePosition + 1) % 20;
                    LibraryWord = Value->LibraryFunData[1];
                    LibraryWord = ScriptPorts::CallWords(LibraryWord, LibraryArguments);
                    if (Value->LibraryFunData[0] == 1) {
                        Dest->Value->SetInt(LibraryWord);
                    } else if (Value->LibraryFunData[0] == 2) {
                        Dest->Value->SetDword(LibraryWord);
                    } else if (Value->LibraryFunData[0] == 3) {
                        Dest->Value->SetFloat(pas::load_unaligned<float>(reinterpret_cast<WindowsSdk::PSingle>(&LibraryWord)));
                    } else if (Value->LibraryFunData[0] == 4) {
                        Dest->Value->SetString(pas::concat_wide({u"", static_cast<pas::WideString>(reinterpret_cast<char16_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(LibraryWord))))}));
                    }
                } else if (EC_Expression::TVarEC_RealVType(Callee) == vkExternFun) {
                    Arguments.set_length(Instruction->OperandCount - 1);
                    Arguments[0] = Dest->Value;
                    Dest->Value->ResetKind(vkEmpty);
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(2, Instruction->OperandCount - 1); cpp_range_2.next(j); ) {
                        Argument = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[j])->Resolve(vkEmpty);
                        Arguments[j - 1] = Argument;
                    }
                    if (Code != nullptr) {
                        Code->Process = Process;
                        Code->DebugContext = DebugContext;
                    }
                    ScriptCallTrace[ScriptCallTracePosition] = Callee;
                    ScriptCallTraceCount = std::min<std::int32_t>(20, ScriptCallTraceCount + 1);
                    ScriptCallTracePosition = (ScriptCallTracePosition + 1) % 20;
                    pas::callback_from_address<TExpressionCallback>(Callee->GetExternFun())(pas::open_array(Arguments), Code);
                } else if (EC_Expression::TVarEC_RealVType(Callee) == vkFunction) {
                    if (([&] {
                        std::int32_t cpp_left = Callee->GetFunction()->LocalVar->GetVar(u"funBaseVarCount"_wref.get())->GetInt();
                        return cpp_left < Instruction->OperandCount - 2;
                    }())) {
                        pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Count var error. fun:", Left->Name}))));
                    }
                    Invocation = pas::construct_call<TCodeEC>(TCodeEC_Create);
                    Invocation->Parent = Callee->GetFunction();
                    Invocation->CopyFromFast(Callee->GetFunction());
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(2, Instruction->OperandCount - 1); cpp_range_3.next(j); ) {
                        Argument = EC_Expression::TExpressionEC_GetVariable(Self, Instruction->Operands[j])->Resolve(vkEmpty);
                        if (EC_Expression::TVarArrayEC_GetItem(Invocation->LocalVar, j - 2)->Kind == vkRef) {
                            EC_Expression::TVarArrayEC_GetItem(Invocation->LocalVar, j - 2)->SetRef(Argument);
                        } else {
                            EC_Expression::TVarArrayEC_GetItem(Invocation->LocalVar, j - 2)->Assume(Argument, false);
                        }
                    }
                    try {
                        {
                            TVarEC* var = Invocation->LocalVar->GetVar(u"result"_wref.get());
                            TVarEC* value = Dest->Value;
                            var->SetRef(value);
                        }
                        if (DebugContext == nullptr) {
                            EC_Expression::TCodeEC_Run(Invocation, Process);
                        } else {
                            EC_Expression::TCodeEC_RunDebug(Invocation, Process, DebugContext);
                        }
                        ScriptCallTrace[ScriptCallTracePosition] = Callee;
                        ScriptCallTraceCount = std::min<std::int32_t>(20, ScriptCallTraceCount + 1);
                        ScriptCallTracePosition = (ScriptCallTracePosition + 1) % 20;
                    } catch (...) {
                        auto cpp_exception = pas::caught_object();
                        if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                            pas::free(Invocation);
                            if (static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))) == "EBreakMessageGI") {
                                throw;
                            }
                            ScriptCallTrace[ScriptCallTracePosition] = Callee;
                            ScriptCallTraceCount = std::min<std::int32_t>(20, ScriptCallTraceCount + 1);
                            ScriptCallTracePosition = (ScriptCallTracePosition + 1) % 20;
                            pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in function ", Callee->Name, u" (", static_cast<pas::WideString>(pas::class_name(pas::class_type(E))), u" ", static_cast<pas::WideString>(E->message), u")"}))));
                        } else {
                            throw;
                        }
                    }
                    pas::free(Invocation);
                } else {
                    pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not fun:", Left->Name}))));
                }
            }
            ++i;
        }
    }

    TVarEC* TExpressionEC::GetResult() {
        if (ResultIndex < 0 || EC_Expression::TExpressionEC_GetVariable(this, ResultIndex)->Value == nullptr) {
            pas::raise(pas::make_exception<ExceptionExpressionEC>("Not link var return"_a));
        }
        return EC_Expression::TExpressionEC_GetVariable(this, ResultIndex)->Value;
    }

    void TCompilerEC_Create(TCompilerEC* Self) {
        pas::object_create(Self);
    }

    void TCompilerEC_Destroy(TCompilerEC* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    void TCompilerEC::Clear() {
        while (First != nullptr) {
            DeleteUnit(Last);
        }
    }

    TCompilerUnitEC* TCompilerEC::AddUnit() {
        TCompilerUnitEC* Item = pas::make_object<TCompilerUnitEC>();
        if (Last != nullptr) {
            Last->Next = Item;
        }
        Item->Prev = Last;
        Item->Next = nullptr;
        Last = Item;
        if (First == nullptr) {
            First = Item;
        }
        return Item;
    }

    void TCompilerEC::DeleteUnit(TCompilerUnitEC* UnitNode) {
        if (UnitNode->Prev != nullptr) {
            UnitNode->Prev->Next = UnitNode->Next;
        }
        if (UnitNode->Next != nullptr) {
            UnitNode->Next->Prev = UnitNode->Prev;
        }
        if (Last == UnitNode) {
            Last = UnitNode->Prev;
        }
        if (First == UnitNode) {
            First = UnitNode->Next;
        }
        pas::free(UnitNode);
    }

    // Returns nil when no operator qualifies.
    TCompilerUnitEC* TCompilerEC::FindReducibleOperator() {
        std::int32_t i{};
        TCompilerUnitEC* Following{};
        pas::Array<TCompilerUnitEC*, 0, 10> Candidates{};
        for (i = 0; i <= 10; ++i) {
            Candidates[i] = nullptr;
        }
        TCompilerUnitEC* Item = First;
        while (Item != nullptr) {
            if (Item->Kind == cuUnaryOperator) {
                if (Item->Next != nullptr && Item->Next->Kind == cuVariable && Candidates[0] == nullptr) {
                    Candidates[0] = Item;
                }
            } else if (Item->Kind == cuBinaryOperator) {
                if (Item->Prev->Kind == cuVariable && Item->Next->Kind == cuVariable) {
                    Following = Item->Next->Next;
                    while (Following != nullptr) {
                        if (Following->Kind == cuOpenParen || Following->Kind == cuOpenBracket || Following->Kind == cuCall || Following->Kind == cuCloseParen || Following->Kind == cuCloseBracket) {
                            break;
                        }
                        Following = Following->Next;
                    }
                    if (Following == nullptr || Following->Kind != cuOpenParen && Following->Kind != cuOpenBracket && Following->Kind != cuCall) {
                        if (Item->OperatorToken == ctMultiply || Item->OperatorToken == ctDivide || Item->OperatorToken == ctModulo) {
                            if (Candidates[1] == nullptr) {
                                Candidates[1] = Item;
                            }
                        } else if (Item->OperatorToken == ctAdd || Item->OperatorToken == ctSubtract) {
                            if (Candidates[2] == nullptr) {
                                Candidates[2] = Item;
                            }
                        } else if (Item->OperatorToken == ctShiftLeft || Item->OperatorToken == ctShiftRight) {
                            if (Candidates[3] == nullptr) {
                                Candidates[3] = Item;
                            }
                        } else if (Item->OperatorToken == ctEqual || Item->OperatorToken == ctNotEqual || Item->OperatorToken == ctLess || Item->OperatorToken == ctGreater || Item->OperatorToken == ctLessEqual || Item->OperatorToken == ctGreaterEqual) {
                            if (Candidates[4] == nullptr) {
                                Candidates[4] = Item;
                            }
                        } else if (Item->OperatorToken == ctBitAnd) {
                            if (Candidates[5] == nullptr) {
                                Candidates[5] = Item;
                            }
                        } else if (Item->OperatorToken == ctBitXor) {
                            if (Candidates[6] == nullptr) {
                                Candidates[6] = Item;
                            }
                        } else if (Item->OperatorToken == ctBitOr) {
                            if (Candidates[7] == nullptr) {
                                Candidates[7] = Item;
                            }
                        } else if (Item->OperatorToken == ctAnd) {
                            if (Candidates[8] == nullptr) {
                                Candidates[8] = Item;
                            }
                        } else if (Item->OperatorToken == ctOr) {
                            if (Candidates[9] == nullptr) {
                                Candidates[9] = Item;
                            }
                        }
                    }
                }
            } else if (Item->Kind == cuAssignment) {
                if (Item->Prev->Kind == cuVariable && Item->Next->Kind == cuVariable && Candidates[10] == nullptr) {
                    Candidates[10] = Item;
                }
            }
            Item = Item->Next;
        }
        for (i = 0; i <= 10; ++i) {
            if (Candidates[i] != nullptr) {
                return Candidates[i];
            }
        }
        return nullptr;
    }

    TCompilerUnitEC* TCompilerEC::FindReducibleIndex() {
        TCompilerUnitEC* Following{};
        TCompilerUnitEC* Item = First;
        while (Item != nullptr) {
            if (Item->Kind == cuIndex) {
                Following = Item->Next;
                while (Following != nullptr) {
                    if (Following->Kind == cuCloseBracket) {
                        return Item;
                    }
                    if (Following->Kind != cuComma && Following->Kind != cuVariable) {
                        break;
                    }
                    Following = Following->Next;
                }
            }
            Item = Item->Next;
        }
        return nullptr;
    }

    TCompilerUnitEC* TCompilerEC::FindReducibleCall() {
        TCompilerUnitEC* Following{};
        TCompilerUnitEC* Item = First;
        while (Item != nullptr) {
            if (Item->Kind == cuCall) {
                Following = Item->Next;
                while (Following != nullptr) {
                    if (Following->Kind == cuCloseParen) {
                        return Item;
                    }
                    if (Following->Kind != cuComma && Following->Kind != cuVariable) {
                        break;
                    }
                    Following = Following->Next;
                }
            }
            Item = Item->Next;
        }
        return nullptr;
    }

    void TCodeUnitEC_Destroy(TCodeUnitEC* Self) {
        if (Self->Expression != nullptr) {
            pas::free(Self->Expression);
            Self->Expression = nullptr;
        }
        pas::object_destroy(Self);
    }

    void TCodeProcessEC_Create(TCodeProcessEC* Self) {
        pas::object_create(Self);
        Self->Handlers = pas::make_object<pas::List>();
        Self->Exceptions = pas::make_object<pas::List>();
    }

    void TCodeProcessEC_Destroy(TCodeProcessEC* Self) {
        Self->Clear();
        pas::free(Self->Handlers);
        Self->Handlers = nullptr;
        pas::free(Self->Exceptions);
        Self->Exceptions = nullptr;
        pas::object_destroy(Self);
    }

    void TCodeProcessEC::Clear() {
        PCodeExceptionHandler Handler{};
        PVarEC Value{};
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Handlers) - 1); cpp_range.next(i); ) {
            Handler = pas::list_at<TCodeExceptionHandler>(Handlers, i);
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Handler);
        }
        pas::list_clear(Handlers);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Exceptions) - 1); cpp_range_2.next(i); ) {
            Value = pas::list_at<TVarEC*>(Exceptions, i);
            if (pas::load_unaligned<TVarEC*>(Value) != nullptr) {
                pas::free(pas::load_unaligned<TVarEC*>(Value));
                pas::store_unaligned<TVarEC*>(Value, nullptr);
            }
            WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Value);
        }
        pas::list_clear(Exceptions);
    }

    void TCodeProcessEC::PushHandler(TCodeEC* Code, TCodeUnitEC* Handler) {
        PCodeExceptionHandler Entry = static_cast<PCodeExceptionHandler>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, static_cast<std::int32_t>(sizeof(TCodeExceptionHandler))));
        Entry->Code = Code;
        Entry->Handler = Handler;
        pas::list_add(Handlers, static_cast<void*>(Entry));
    }

    void TCodeProcessEC::PopHandler() {
        std::int32_t Count = pas::list_count(Handlers);
        if (Count < 1) {
            return;
        }
        PCodeExceptionHandler Entry = pas::list_at<TCodeExceptionHandler>(Handlers, Count - 1);
        WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Entry);
        pas::list_delete(Handlers, Count - 1);
    }

    PCodeExceptionHandler TCodeProcessEC::GetHandler() {
        std::int32_t Count = pas::list_count(Handlers);
        if (Count < 1) {
            return nullptr;
        }
        return pas::list_at<TCodeExceptionHandler>(Handlers, Count - 1);
    }

    void TCodeProcessEC::PushException(TVarEC* Value) {
        PVarEC Entry = static_cast<PVarEC>(WindowsSdk::HeapAlloc(WindowsSdk::GetProcessHeap(), 0u, static_cast<std::int32_t>(sizeof(TVarEC*))));
        pas::store_unaligned<TVarEC*>(Entry, pas::construct_call<TVarEC>(TVarEC_Create, EC_Expression::TVarEC_RealVType(Value)));
        pas::load_unaligned<TVarEC*>(Entry)->Assume(Value, false);
        pas::list_add(Exceptions, static_cast<void*>(Entry));
    }

    // Does not free the exception value; the caller assumes ownership.
    void TCodeProcessEC::PopException() {
        std::int32_t Count = pas::list_count(Exceptions);
        if (Count < 1) {
            return;
        }
        PVarEC Entry = pas::list_at<TVarEC*>(Exceptions, Count - 1);
        WindowsSdk::HeapFree(WindowsSdk::GetProcessHeap(), 0u, Entry);
        pas::list_delete(Exceptions, Count - 1);
    }

    PVarEC TCodeProcessEC::GetException() {
        std::int32_t Count = pas::list_count(Exceptions);
        if (Count < 1) {
            return nullptr;
        }
        return pas::list_at<TVarEC*>(Exceptions, Count - 1);
    }

    void TCodeProcessEC::RaiseUnhandledExceptions() {
        PVarEC Entry{};
        pas::WideString Text{};
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Exceptions) - 1); cpp_range.next(i); ) {
            Entry = pas::list_at<TVarEC*>(Exceptions, i);
            if (i > 0) {
                Text = pas::concat_wide({Text, u"\r\n"});
            }
            Text = pas::concat_wide({Text, u"Exception: ", pas::load_unaligned<TVarEC*>(Entry)->GetString()});
        }
        if (Text != u"") {
            pas::raise(pas::make_exception<ExceptionExpressionEC>(static_cast<pas::AnsiString>(Text)));
        }
    }

    void TCodeEC_Create(TCodeEC* Self) {
        pas::object_create(Self);
        Self->LocalVar = pas::construct_call<TVarArrayEC>(TVarArrayEC_Create);
        Self->ScriptFunLinked = false;
    }

    void TCodeEC_Destroy(TCodeEC* Self) {
        Self->Clear();
        pas::free(Self->LocalVar);
        pas::object_destroy(Self);
    }

    void TCodeEC::Clear() {
        while (First != nullptr) {
            DeleteCodeUnit(Last);
        }
        LocalVar->Clear();
        ScriptFunLinked = false;
    }

    void TCodeEC::CopyFrom(TCodeEC* Source) {
        TCodeUnitEC* Dest{};
        TCodeUnitEC* DestTarget{};
        TCodeUnitEC* SrcTarget{};
        Clear();
        IsClassDefinition = Source->IsClassDefinition;
        Name = Source->Name;
        Parent = Source->Parent;
        TCodeUnitEC* Src = Source->First;
        while (Src != nullptr) {
            Dest = AddCodeUnit();
            Dest->Opcode = Src->Opcode;
            Dest->SourceStart = Src->SourceStart;
            Dest->SourceLength = Src->SourceLength;
            Dest->SourceContext = Src->SourceContext;
            Dest->Target = Src->Target;
            Dest->Breakpoint = Src->Breakpoint;
            Dest->Expression = nullptr;
            if (Src->Expression != nullptr) {
                Dest->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                Dest->Expression->CopyFrom(Src->Expression);
            }
            Src = Src->Next;
        }
        Src = Source->First;
        Dest = First;
        while (Src != nullptr) {
            if (Src->Target != nullptr) {
                SrcTarget = Source->First;
                DestTarget = First;
                while (SrcTarget != nullptr) {
                    if (Src->Target == SrcTarget) {
                        Dest->Target = DestTarget;
                    }
                    SrcTarget = SrcTarget->Next;
                    DestTarget = DestTarget->Next;
                }
            }
            Src = Src->Next;
            Dest = Dest->Next;
        }
        LocalVar->CopyFrom(Source->LocalVar, false);
        ScriptFunLinked = Source->ScriptFunLinked;
    }

    // Expression instructions remain shared with Source.
    void TCodeEC::CopyFromFast(TCodeEC* Source) {
        TCodeUnitEC* Dest{};
        TCodeUnitEC* Src{};
        TCodeUnitEC* DestTarget{};
        TCodeUnitEC* SrcTarget{};
        Clear();
        Src = Source->First;
        while (Src != nullptr) {
            Dest = AddCodeUnit();
            Dest->Opcode = Src->Opcode;
            Dest->SourceStart = Src->SourceStart;
            Dest->SourceLength = Src->SourceLength;
            Dest->SourceContext = Src->SourceContext;
            Dest->Target = Src->Target;
            Dest->Breakpoint = Src->Breakpoint;
            Dest->Expression = nullptr;
            if (Src->Expression != nullptr) {
                Dest->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                Dest->Expression->CopyFromFast(Src->Expression);
            }
            Src = Src->Next;
        }
        Src = Source->First;
        Dest = First;
        while (Src != nullptr) {
            if (Src->Target != nullptr) {
                SrcTarget = Source->First;
                DestTarget = First;
                while (SrcTarget != nullptr) {
                    if (Src->Target == SrcTarget) {
                        Dest->Target = DestTarget;
                    }
                    SrcTarget = SrcTarget->Next;
                    DestTarget = DestTarget->Next;
                }
            }
            Src = Src->Next;
            Dest = Dest->Next;
        }
        LocalVar->CopyFrom(Source->LocalVar, false);
        Src = Source->First;
        Dest = First;
        while (Src != nullptr) {
            if (Src->ExceptionVar != nullptr) {
                Dest->ExceptionVar = LocalVar->GetVarNE(Src->ExceptionVar->Name);
            }
            Src = Src->Next;
            Dest = Dest->Next;
        }
        ScriptFunLinked = Source->ScriptFunLinked;
    }

    TVarEC* TCodeEC::FindVar(pas::WideString Name) {
        TVarEC* Item{};
        std::int32_t i{};
        TVarEC* Result = LocalVar->GetVarNE(Name);
        if (Result != nullptr) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LocalVar->Count - 1); cpp_range.next(i); ) {
            Item = EC_Expression::TVarArrayEC_GetItem(LocalVar, i);
            if (Item->Kind == vkFunction && Item->FunctionValue->IsClassDefinition) {
                Result = Item->FunctionValue->FindVar(Name);
                if (Result != nullptr) {
                    return Result;
                }
            }
        }
        return Result;
    }

    void TCodeEC::DeleteCodeUnit(TCodeUnitEC* CodeUnit) {
        if (CodeUnit->Prev != nullptr) {
            CodeUnit->Prev->Next = CodeUnit->Next;
        }
        if (CodeUnit->Next != nullptr) {
            CodeUnit->Next->Prev = CodeUnit->Prev;
        }
        if (Last == CodeUnit) {
            Last = CodeUnit->Prev;
        }
        if (First == CodeUnit) {
            First = CodeUnit->Next;
        }
        pas::free(CodeUnit);
    }

    TCodeUnitEC* TCodeEC::AddCodeUnit() {
        TCodeUnitEC* Item = pas::make_object<TCodeUnitEC>();
        if (Last != nullptr) {
            Last->Next = Item;
        }
        Item->Prev = Last;
        Item->Next = nullptr;
        Last = Item;
        if (First == nullptr) {
            First = Item;
        }
        return Item;
    }

    // Inserts before BeforeUnit; nil appends.
    TCodeUnitEC* TCodeEC::InsertCodeUnitBefore(TCodeUnitEC* BeforeUnit) {
        if (BeforeUnit == nullptr) {
            return AddCodeUnit();
        }
        TCodeUnitEC* Item = pas::make_object<TCodeUnitEC>();
        Item->Prev = BeforeUnit->Prev;
        Item->Next = BeforeUnit;
        if (BeforeUnit->Prev != nullptr) {
            BeforeUnit->Prev->Next = Item;
        }
        BeforeUnit->Prev = Item;
        if (First == BeforeUnit) {
            First = Item;
        }
        return Item;
    }

    // NextToken may be nil.
    void TCodeEC::Compile(TCodeAnalyzerEC* Analyzer, void* SourceContext, TScriptIncludeResolver IncludeResolver, TCodeAnalyzerUnitEC* FirstToken, PCodeAnalyzerUnitEC NextToken, pas::WideString& ErrorText) {
        ErrorText = pas::WideString();
        if (FirstToken == nullptr) {
            FirstToken = Analyzer->First;
        }
        CompileBlock(Analyzer, SourceContext, IncludeResolver, FirstToken, nullptr, NextToken, nullptr, nullptr, nullptr, ErrorText);
    }

    void TCodeEC::CompileBlock(TCodeAnalyzerEC* Analyzer, void* SourceContext, TScriptIncludeResolver IncludeResolver, TCodeAnalyzerUnitEC* Token, TCodeUnitEC* BeforeUnit, PCodeAnalyzerUnitEC NextToken, PCodeAnalyzerUnitEC StatementEnd, TCodeUnitEC* BreakTarget, TCodeUnitEC* ContinueTarget, pas::WideString& ErrorText) {
        TCodeAnalyzerEC* Included{};
        void* IncludedContext{};
        TCodeAnalyzerUnitEC* Next{};
        pas::WideString Keyword{};
        TCodeUnitEC* Item{};
        TCodeUnitEC* LoopStart{};
        TCodeUnitEC* StepStart{};
        TCodeUnitEC* EndLabel{};
        TCodeUnitEC* Branch{};
        TVarEC* Value{};
        TVarEC* BaseValue{};
        TCodeEC* Definition{};
        std::int32_t ParameterCount{};
        double FloatValue{};
        std::uint32_t DwordValue{};
        std::int32_t IntValue{};
        pas::WideString Text{};
        std::uint8_t InsertSource{};
        auto IsScriptLocalDeclaration = [&](TCodeAnalyzerUnitEC* Token) -> std::uint8_t {
            return Token != nullptr && Token->Next != nullptr && Token->Next->TokenKind == ctText && Token->TokenKind == ctText && (Token->Text == u"unknown" || Token->Text == u"int" || Token->Text == u"dword" || Token->Text == u"float" || Token->Text == u"str" || Token->Text == u"ref" || Token->Text == u"array") && EC_Expression::IsNonIntegerScriptText(pas::view(Token->Next->Text));
        };
        auto CompileScriptLocals = [&](TCodeAnalyzerUnitEC*& Token) -> pas::WideString {
            pas::WideString Result{};
            TCodeUnitEC* Item{};
            TCodeAnalyzerUnitEC* Next{};
            pas::WideString TypeName{};
            Result = pas::WideString();
            TypeName = Token->Text;
            Token = Token->Next;
            while (Token->TokenKind == ctText && EC_Expression::IsNonIntegerScriptText(pas::view(Token->Text))) {
                if (this->LocalVar->GetVarNE(Token->Text) != nullptr) {
                    EC_Expression::FormatScriptError(0, Token->SourceStart, Result);
                    return Result;
                }
                EC_Expression::AddScriptLocal(pas::view(TypeName), Token->Text, this);
                if (Token->Next == nullptr) {
                    EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, Result);
                    return Result;
                }
                Token = Token->Next;
                if (Token->TokenKind == ctComma) {
                    Token = Token->Next;
                    continue;
                }
                if (Token->TokenKind == ctAssign) {
                    Item = InsertCodeUnitBefore(BeforeUnit);
                    Item->Opcode = coExpression;
                    Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                    Item->SourceStart = Token->Prev->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    Item->Expression->Compile(Analyzer, Token->Prev, nullptr, &Next, Result);
                    if (Result != u"") {
                        return Result;
                    }
                    Item->SourceLength = Next->Prev->SourceStart + Next->Prev->SourceLength - Item->SourceStart;
                    Token = Next;
                    if (Token->TokenKind == ctComma) {
                        Token = Token->Next;
                    }
                }
            }
            return Result;
        };
        ErrorText = pas::WideString();
        std::int32_t Depth = 0;
        while (Token != nullptr) {
            if (Token->TokenKind == ctText) {
                Keyword = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Token->Text)));
                if (IsScriptLocalDeclaration(Token)) {
                    ErrorText = CompileScriptLocals(Token);
                    if (ErrorText != u"") {
                        return;
                    }
                    if (Token->TokenKind != ctSemicolon) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    Token = Token->Next;
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"if") {
                    EndLabel = InsertCodeUnitBefore(BeforeUnit);
                    EndLabel->Opcode = coLabel;
                    EndLabel->SourceStart = 0;
                    EndLabel->SourceLength = 0;
                    EndLabel->SourceContext = SourceContext;
                    while (true) {
                        if (Token->Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                            return;
                        }
                        if (Token->Next->TokenKind != ctOpenParen || Token->Next->Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                            return;
                        }
                        Item = InsertCodeUnitBefore(EndLabel);
                        Item->Opcode = coBranchFalse;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        Item->Target = EndLabel;
                        Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                        Item->Expression->Compile(Analyzer, Token->Next->Next, nullptr, &Next, ErrorText);
                        if (ErrorText != u"") {
                            return;
                        }
                        if (Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                            return;
                        }
                        if (Next->TokenKind != ctCloseParen) {
                            EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                            return;
                        }
                        Token = Next->Next;
                        Item->SourceLength = Next->SourceStart - Item->SourceStart + Next->SourceLength;
                        Branch = Item;
                        if (Token == nullptr) {
                            pas::raise(pas::make_exception<ExceptionExpressionEC>("Compiler error, code ends abruptly"_a));
                        }
                        Item = InsertCodeUnitBefore(EndLabel);
                        Item->Opcode = coJump;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        Item->Target = EndLabel;
                        LoopStart = Item;
                        if (Token->TokenKind == ctSemicolon) {
                            Token = Token->Next;
                        } else {
                            CompileBlock(Analyzer, SourceContext, IncludeResolver, Token, LoopStart, NextToken, &Token, BreakTarget, ContinueTarget, ErrorText);
                            if (ErrorText != u"") {
                                return;
                            }
                        }
                        if (Token == nullptr) {
                            return;
                        }
                        if (Token->TokenKind != ctText) {
                            break;
                        }
                        Keyword = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Token->Text)));
                        if (Keyword != u"else") {
                            break;
                        }
                        Item = InsertCodeUnitBefore(EndLabel);
                        Item->Opcode = coLabel;
                        Item->SourceStart = 0;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        Branch->Target = Item;
                        if (Token->Next != nullptr && Token->Next->TokenKind == ctText && SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Token->Next->Text)) == "if") {
                            Token = Token->Next;
                            continue;
                        }
                        if (Token->Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                            return;
                        }
                        if (Token->Next->TokenKind == ctSemicolon) {
                            continue;
                        }
                        CompileBlock(Analyzer, SourceContext, IncludeResolver, Token->Next, EndLabel, NextToken, &Token, BreakTarget, ContinueTarget, ErrorText);
                        if (ErrorText != u"") {
                            return;
                        }
                        if (StatementEnd != nullptr && Depth == 0) {
                            pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                            return;
                        }
                        break;
                    }
                    continue;
                } else if (Keyword == u"while") {
                    EndLabel = InsertCodeUnitBefore(BeforeUnit);
                    EndLabel->Opcode = coLabel;
                    EndLabel->SourceStart = 0;
                    EndLabel->SourceLength = 0;
                    EndLabel->SourceContext = SourceContext;
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctOpenParen || Token->Next->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    Item = InsertCodeUnitBefore(EndLabel);
                    Item->Opcode = coBranchFalse;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    Item->Target = EndLabel;
                    Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                    Item->Expression->Compile(Analyzer, Token->Next->Next, nullptr, &Next, ErrorText);
                    if (ErrorText != u"") {
                        return;
                    }
                    if (Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Next->TokenKind != ctCloseParen) {
                        EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                        return;
                    }
                    Token = Next->Next;
                    Item->SourceLength = Next->SourceStart - Item->SourceStart + Next->SourceLength;
                    LoopStart = Item;
                    Item = InsertCodeUnitBefore(EndLabel);
                    Item->Opcode = coJump;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    Item->Target = LoopStart;
                    if (Token->TokenKind == ctSemicolon) {
                        Token = Token->Next;
                    } else {
                        CompileBlock(Analyzer, SourceContext, IncludeResolver, Token, Item, NextToken, &Token, EndLabel, LoopStart, ErrorText);
                        if (ErrorText != u"") {
                            return;
                        }
                    }
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"for") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctOpenParen) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart + Token->Next->SourceLength, ErrorText);
                        return;
                    }
                    Token = Token->Next->Next;
                    if (Token->TokenKind != ctSemicolon) {
                        if (IsScriptLocalDeclaration(Token)) {
                            ErrorText = CompileScriptLocals(Token);
                            if (ErrorText != u"") {
                                return;
                            }
                        } else {
                            while (true) {
                                Item = InsertCodeUnitBefore(BeforeUnit);
                                Item->Opcode = coExpression;
                                Item->SourceStart = Token->SourceStart;
                                Item->SourceLength = 0;
                                Item->SourceContext = SourceContext;
                                Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                                Item->Expression->Compile(Analyzer, Token, nullptr, &Next, ErrorText);
                                if (ErrorText != u"") {
                                    return;
                                }
                                if (Next == nullptr) {
                                    EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                                    return;
                                }
                                Item->SourceLength = Next->SourceStart - Item->SourceStart + Next->SourceLength;
                                if (Next->TokenKind == ctSemicolon) {
                                    Token = Next;
                                    break;
                                } else if (Next->TokenKind == ctComma) {
                                    Token = Next->Next;
                                } else {
                                    EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                                    return;
                                }
                            }
                        }
                    }
                    if (Token->TokenKind != ctSemicolon) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    Token = Token->Next;
                    EndLabel = InsertCodeUnitBefore(BeforeUnit);
                    EndLabel->Opcode = coLabel;
                    EndLabel->SourceStart = 0;
                    EndLabel->SourceLength = 0;
                    EndLabel->SourceContext = SourceContext;
                    if (Token->TokenKind == ctSemicolon) {
                        Item = InsertCodeUnitBefore(EndLabel);
                        Item->Opcode = coLabel;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        Token = Token->Next;
                    } else {
                        Item = InsertCodeUnitBefore(EndLabel);
                        Item->Opcode = coBranchFalse;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        Item->Target = EndLabel;
                        Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                        Item->Expression->Compile(Analyzer, Token, nullptr, &Next, ErrorText);
                        if (ErrorText != u"") {
                            return;
                        }
                        if (Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                            return;
                        }
                        if (Next->TokenKind != ctSemicolon) {
                            EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                            return;
                        }
                        if (Next->Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Next->SourceStart + Next->SourceLength, ErrorText);
                            return;
                        }
                        Token = Next->Next;
                        Item->SourceLength = Next->Prev->SourceStart - Item->SourceStart + Next->Prev->SourceLength;
                    }
                    LoopStart = Item;
                    Item = InsertCodeUnitBefore(LoopStart);
                    Item->Opcode = coJump;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    Item->Target = LoopStart;
                    StepStart = nullptr;
                    if (Token->TokenKind == ctCloseParen) {
                        Item = InsertCodeUnitBefore(LoopStart);
                        Item->Opcode = coLabel;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        StepStart = Item;
                        Token = Token->Next;
                    } else {
                        while (true) {
                            Item = InsertCodeUnitBefore(LoopStart);
                            Item->Opcode = coExpression;
                            Item->SourceStart = Token->SourceStart;
                            Item->SourceLength = 0;
                            Item->SourceContext = SourceContext;
                            Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                            Item->Expression->Compile(Analyzer, Token, nullptr, &Next, ErrorText);
                            if (ErrorText != u"") {
                                return;
                            }
                            if (Next == nullptr) {
                                EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                                return;
                            }
                            Item->SourceLength = Next->Prev->SourceStart - Item->SourceStart + Next->Prev->SourceLength;
                            if (StepStart == nullptr) {
                                StepStart = Item;
                            }
                            if (Next->TokenKind == ctCloseParen) {
                                Token = Next->Next;
                                break;
                            } else if (Next->TokenKind == ctComma) {
                                Token = Next->Next;
                            } else {
                                EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                                return;
                            }
                        }
                    }
                    Item = InsertCodeUnitBefore(EndLabel);
                    Item->Opcode = coJump;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    Item->Target = StepStart;
                    if (Token->TokenKind == ctSemicolon) {
                        Token = Token->Next;
                    } else {
                        CompileBlock(Analyzer, SourceContext, IncludeResolver, Token, Item, NextToken, &Token, EndLabel, StepStart, ErrorText);
                        if (ErrorText != u"") {
                            return;
                        }
                    }
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"break") {
                    if (BreakTarget == nullptr || Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctSemicolon) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    Item = InsertCodeUnitBefore(BeforeUnit);
                    Item->Opcode = coJump;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->Next->SourceStart - Token->SourceStart + Token->Next->SourceLength;
                    Item->SourceContext = SourceContext;
                    Item->Target = BreakTarget;
                    Token = Token->Next->Next;
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"continue") {
                    if (ContinueTarget == nullptr || Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctSemicolon) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    Item = InsertCodeUnitBefore(BeforeUnit);
                    Item->Opcode = coJump;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->Next->SourceStart - Token->SourceStart + Token->Next->SourceLength;
                    Item->SourceContext = SourceContext;
                    Item->Target = ContinueTarget;
                    Token = Token->Next->Next;
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"exit") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctSemicolon) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    Item = InsertCodeUnitBefore(BeforeUnit);
                    Item->Opcode = coExit;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = Token->Next->SourceStart - Token->SourceStart + Token->Next->SourceLength;
                    Item->SourceContext = SourceContext;
                    Token = Token->Next->Next;
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"#include" || Keyword == u"#insert") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctStringLiteral) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    if (!(IncludeResolver != nullptr)) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    InsertSource = Keyword == u"#insert";
                    Included = pas::construct_call<TCodeAnalyzerEC>(TCodeAnalyzerEC_Create);
                    IntValue = IncludeResolver(SourceContext, Token->Next->Text, InsertSource, IncludedContext, Included);
                    if (IntValue == 2 || IntValue == 3) {
                        pas::free(Included);
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    if (IntValue == 0) {
                        Text = Included->ValidateDelimiters();
                        if (Text != u"") {
                            pas::free(Included);
                            EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                            return;
                        }
                        CompileBlock(Included, IncludedContext, IncludeResolver, Included->First, BeforeUnit, nullptr, nullptr, nullptr, nullptr, ErrorText);
                        if (ErrorText != u"") {
                            pas::free(Included);
                            return;
                        }
                    }
                    pas::free(Included);
                    Token = Token->Next->Next;
                    continue;
                } else if (Keyword == u"function") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctText || static_cast<std::uint8_t>(EC_Expression::IsNonIntegerScriptText(pas::view(Token->Next->Text)) ^ 1) || Token->Next->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->Next->TokenKind != ctOpenParen || Token->Next->Next->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->Next->Next->SourceStart, ErrorText);
                        return;
                    }
                    Value = LocalVar->Add(Token->Next->Text, vkFunction);
                    Definition = Value->GetFunction();
                    Definition->Parent = this;
                    Next = Token->Next->Next->Next;
                    while (Next != nullptr && Next->TokenKind == ctText && EC_Expression::IsNonIntegerScriptText(pas::view(Next->Text))) {
                        Keyword = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Next->Text)));
                        if (Keyword == u"unknown" || Keyword == u"int" || Keyword == u"dword" || Keyword == u"float" || Keyword == u"str" || Keyword == u"ref" || Keyword == u"array") {
                            Next = Next->Next;
                        } else {
                            Keyword = u"unknown"_w;
                        }
                        if (Next == nullptr || Next->TokenKind != ctText || static_cast<std::uint8_t>(EC_Expression::IsNonIntegerScriptText(pas::view(Next->Text)) ^ 1)) {
                            break;
                        }
                        Value = nullptr;
                        if (Keyword == u"unknown") {
                            Value = Definition->LocalVar->Add(Next->Text, vkEmpty);
                        } else if (Keyword == u"int") {
                            Value = Definition->LocalVar->Add(Next->Text, vkInt);
                        } else if (Keyword == u"dword") {
                            Value = Definition->LocalVar->Add(Next->Text, vkDword);
                        } else if (Keyword == u"float") {
                            Value = Definition->LocalVar->Add(Next->Text, vkFloat);
                        } else if (Keyword == u"str") {
                            Value = Definition->LocalVar->Add(Next->Text, vkString);
                        } else if (Keyword == u"ref") {
                            Value = Definition->LocalVar->Add(Next->Text, vkRef);
                        } else if (Keyword == u"array") {
                            Value = Definition->LocalVar->Add(Next->Text, vkArray);
                        }
                        Next = Next->Next;
                        if (Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Analyzer->Last->SourceStart + Analyzer->Last->SourceLength, ErrorText);
                            return;
                        }
                        if (Keyword != u"ref" && Next->TokenKind == ctAssign) {
                            if (Next->Next == nullptr) {
                                EC_Expression::FormatScriptError(0, Analyzer->Last->SourceStart + Analyzer->Last->SourceLength, ErrorText);
                                return;
                            }
                            Next = Next->Next;
                            if (EC_Expression::TryReadFloatLiteral(Next, FloatValue)) {
                                Value->SetFloat(FloatValue);
                            } else if (EC_Expression::TryReadDwordLiteral(Next, DwordValue)) {
                                Value->SetDword(DwordValue);
                            } else if (EC_Expression::TryReadIntegerLiteral(Next, IntValue)) {
                                Value->SetInt(IntValue);
                            } else if (EC_Expression::TryReadStringLiteral(Next, Text)) {
                                Value->SetString(Text);
                            } else {
                                EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                                return;
                            }
                        }
                        if (Next == nullptr || Next->TokenKind != ctComma) {
                            break;
                        }
                        Next = Next->Next;
                    }
                    if (Next == nullptr || Next->TokenKind != ctCloseParen) {
                        EC_Expression::FormatScriptError(0, Token->Next->Next->Next->SourceStart, ErrorText);
                        return;
                    }
                    Token = Next->Next;
                    if (Token == nullptr) {
                        EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->TokenKind != ctOpenBrace || Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    ParameterCount = Definition->LocalVar->Count;
                    Definition->LocalVar->Add(u"funBaseVarCount"_wref.get(), vkInt)->SetInt(ParameterCount);
                    Definition->LocalVar->Add(u"result"_wref.get(), vkRef);
                    Next = nullptr;
                    Definition->Compile(Analyzer, SourceContext, IncludeResolver, Token->Next, &Next, ErrorText);
                    if (ErrorText != u"") {
                        return;
                    }
                    if (Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Analyzer->Last->SourceStart + Analyzer->Last->SourceLength, ErrorText);
                        return;
                    }
                    if (Next->TokenKind != ctCloseBrace) {
                        EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                        return;
                    }
                    Token = Next->Next;
                    continue;
                } else if (Keyword == u"class") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctText || static_cast<std::uint8_t>(EC_Expression::IsNonIntegerScriptText(pas::view(Token->Next->Text)) ^ 1)) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart + Token->Next->SourceLength, ErrorText);
                        return;
                    }
                    Value = LocalVar->Add(Token->Next->Text, vkFunction);
                    Definition = Value->GetFunction();
                    Definition->Parent = this;
                    Definition->IsClassDefinition = true;
                    Definition->Name = Token->Next->Text;
                    Token = Token->Next->Next;
                    if (Token->TokenKind == ctColon) {
                        Token = Token->Next;
                        while (true) {
                            if (Token->TokenKind != ctText) {
                                break;
                            }
                            BaseValue = LocalVar->GetVarNE(Token->Text);
                            if (BaseValue == nullptr || EC_Expression::TVarEC_RealVType(BaseValue) != vkFunction) {
                                EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                                return;
                            }
                            {
                                TCodeEC* function = Definition->LocalVar->Add(Token->Text, vkFunction)->GetFunction();
                                TCodeEC* function_2 = BaseValue->GetFunction();
                                function->CopyFrom(function_2);
                            }
                            Token = Token->Next;
                            if (Token->TokenKind != ctComma) {
                                break;
                            }
                            Token = Token->Next;
                        }
                    }
                    if (Token->TokenKind != ctOpenBrace) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                        return;
                    }
                    Next = nullptr;
                    Definition->Compile(Analyzer, SourceContext, IncludeResolver, Token->Next, &Next, ErrorText);
                    if (ErrorText != u"") {
                        return;
                    }
                    if (Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Analyzer->Last->SourceStart + Analyzer->Last->SourceLength, ErrorText);
                        return;
                    }
                    if (Next->TokenKind != ctCloseBrace) {
                        EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                        return;
                    }
                    Token = Next->Next;
                    continue;
                } else if (Keyword == u"try") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind != ctOpenBrace) {
                        EC_Expression::FormatScriptError(0, Token->Next->SourceStart, ErrorText);
                        return;
                    }
                    LoopStart = InsertCodeUnitBefore(BeforeUnit);
                    LoopStart->Opcode = coPushHandler;
                    LoopStart->SourceStart = Token->SourceStart;
                    LoopStart->SourceLength = 0;
                    LoopStart->SourceContext = SourceContext;
                    CompileBlock(Analyzer, SourceContext, IncludeResolver, Token->Next, BeforeUnit, NextToken, &Token, nullptr, nullptr, ErrorText);
                    if (ErrorText != u"") {
                        return;
                    }
                    Item = InsertCodeUnitBefore(BeforeUnit);
                    Item->Opcode = coPopHandler;
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    if (Token == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->TokenKind != ctText) {
                        EC_Expression::FormatScriptError(1001, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Text != u"catch" && Token->Text != u"finally") {
                        EC_Expression::FormatScriptError(1001, Token->SourceStart, ErrorText);
                        return;
                    }
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(1001, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Text == u"catch") {
                        IntValue = 0;
                    } else {
                        IntValue = 1;
                    }
                    Token = Token->Next;
                    Value = nullptr;
                    if (Token->TokenKind == ctOpenParen) {
                        if (Token->Next == nullptr) {
                            EC_Expression::FormatScriptError(1001, Token->SourceStart + Token->SourceLength, ErrorText);
                            return;
                        }
                        Token = Token->Next;
                        if (Token->TokenKind != ctText) {
                            EC_Expression::FormatScriptError(1001, Token->SourceStart, ErrorText);
                            return;
                        }
                        Value = LocalVar->Add(Token->Text, vkEmpty);
                        if (Token->Next == nullptr) {
                            EC_Expression::FormatScriptError(1001, Token->SourceStart + Token->SourceLength, ErrorText);
                            return;
                        }
                        Token = Token->Next;
                        if (Token->TokenKind != ctCloseParen) {
                            EC_Expression::FormatScriptError(1001, Token->SourceStart, ErrorText);
                            return;
                        }
                        if (Token->Next == nullptr) {
                            EC_Expression::FormatScriptError(1001, Token->SourceStart + Token->SourceLength, ErrorText);
                            return;
                        }
                        Token = Token->Next;
                    }
                    if (Token->TokenKind != ctOpenBrace) {
                        EC_Expression::FormatScriptError(1001, Token->SourceStart, ErrorText);
                        return;
                    }
                    EndLabel = InsertCodeUnitBefore(BeforeUnit);
                    EndLabel->Opcode = coLabel;
                    EndLabel->SourceStart = 0;
                    EndLabel->SourceLength = 0;
                    EndLabel->SourceContext = SourceContext;
                    if (IntValue == 0) {
                        Item = InsertCodeUnitBefore(EndLabel);
                        Item->Opcode = coJump;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                        Item->Target = EndLabel;
                    }
                    Item = InsertCodeUnitBefore(EndLabel);
                    Item->Opcode = coLabel;
                    Item->SourceStart = 0;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    LoopStart->Target = Item;
                    Item->ExceptionVar = Value;
                    Item = EndLabel;
                    if (IntValue == 1) {
                        Item = InsertCodeUnitBefore(Item);
                        Item->Opcode = coThrow;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = 0;
                        Item->SourceContext = SourceContext;
                    }
                    CompileBlock(Analyzer, SourceContext, IncludeResolver, Token, Item, NextToken, &Token, nullptr, nullptr, ErrorText);
                    if (ErrorText != u"") {
                        return;
                    }
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else if (Keyword == u"throw") {
                    if (Token->Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Token->SourceStart + Token->SourceLength, ErrorText);
                        return;
                    }
                    if (Token->Next->TokenKind == ctSemicolon) {
                        Item = InsertCodeUnitBefore(BeforeUnit);
                        Item->Opcode = coThrow;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = Token->Next->SourceStart - Token->SourceStart + Token->Next->SourceLength;
                        Item->SourceContext = SourceContext;
                        Token = Token->Next->Next;
                    } else {
                        Item = InsertCodeUnitBefore(BeforeUnit);
                        Item->Opcode = coThrow;
                        Item->SourceStart = Token->SourceStart;
                        Item->SourceLength = Token->Next->SourceStart - Token->SourceStart + Token->Next->SourceLength;
                        Item->SourceContext = SourceContext;
                        Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                        Item->Expression->Compile(Analyzer, Token->Next, nullptr, &Next, ErrorText);
                        if (ErrorText != u"") {
                            return;
                        }
                        if (Next == nullptr) {
                            EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                            return;
                        }
                        if (Next->TokenKind != ctSemicolon) {
                            EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                            return;
                        }
                        Token = Next->Next;
                        Item->SourceLength = Next->SourceStart - Item->SourceStart + Next->SourceLength;
                    }
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token);
                        return;
                    }
                    continue;
                } else {
                    Item = InsertCodeUnitBefore(BeforeUnit);
                    Item->Opcode = coExpression;
                    Item->Expression = pas::construct_call<TExpressionEC>(TExpressionEC_Create);
                    Item->SourceStart = Token->SourceStart;
                    Item->SourceLength = 0;
                    Item->SourceContext = SourceContext;
                    Item->Expression->Compile(Analyzer, Token, nullptr, &Next, ErrorText);
                    if (ErrorText != u"") {
                        return;
                    }
                    if (Next == nullptr) {
                        EC_Expression::FormatScriptError(0, Analyzer->Last->SourceStart + Analyzer->Last->SourceLength, ErrorText);
                        return;
                    }
                    if (Next->TokenKind != ctSemicolon) {
                        EC_Expression::FormatScriptError(0, Next->SourceStart, ErrorText);
                        return;
                    }
                    Item->SourceLength = Next->SourceStart + Next->SourceLength - Item->SourceStart;
                    if (StatementEnd != nullptr && Depth == 0) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Next->Next);
                        return;
                    }
                    Token = Next->Next;
                    continue;
                }
            } else if (Token->TokenKind == ctOpenBrace) {
                ++Depth;
            } else if (Token->TokenKind == ctCloseBrace) {
                --Depth;
                if (StatementEnd != nullptr && Depth == 0) {
                    pas::store_unaligned<TCodeAnalyzerUnitEC*>(StatementEnd, Token->Next);
                    return;
                }
                if (Depth == -1) {
                    if (NextToken != nullptr) {
                        pas::store_unaligned<TCodeAnalyzerUnitEC*>(NextToken, Token);
                    }
                    return;
                }
            } else {
                EC_Expression::FormatScriptError(0, Token->SourceStart, ErrorText);
                return;
            }
            if (Token == nullptr) {
                return;
            }
            Token = Token->Next;
        }
    }

    void AddScriptLocal(const std::u16string_view& TypeName, pas::WideString Name, TCodeEC* Self) {
        if (TypeName == u"unknown"sv) {
            Self->LocalVar->Add(Name, vkEmpty);
        } else if (TypeName == u"int"sv) {
            Self->LocalVar->Add(Name, vkInt);
        } else if (TypeName == u"dword"sv) {
            Self->LocalVar->Add(Name, vkDword);
        } else if (TypeName == u"float"sv) {
            Self->LocalVar->Add(Name, vkFloat);
        } else if (TypeName == u"str"sv) {
            Self->LocalVar->Add(Name, vkString);
        } else if (TypeName == u"ref"sv) {
            Self->LocalVar->Add(Name, vkRef);
        } else if (TypeName == u"array"sv) {
            Self->LocalVar->Add(Name, vkArray);
        }
    }

    void TCodeEC::LinkAll(TVarArrayEC* Scope, std::uint8_t OnlyUnlinked) {
        std::int32_t i{};
        TCodeUnitEC* Item = First;
        while (Item != nullptr) {
            if (Item->Expression != nullptr) {
                Item->Expression->Link(Scope, OnlyUnlinked);
            }
            Item = Item->Next;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LocalVar->Count - 1); cpp_range.next(i); ) {
            TVarEC* cpp_with = EC_Expression::TVarArrayEC_GetItem(LocalVar, i);
            if (cpp_with->Kind == vkFunction && cpp_with->GetFunction() != nullptr) {
                cpp_with->GetFunction()->LinkAll(Scope, OnlyUnlinked);
            } else if (cpp_with->Kind == vkClass) {
                if (cpp_with->GetClass() != nullptr) {
                    cpp_with->GetClass()->LinkAll(Scope, OnlyUnlinked);
                }
            }
        }
    }

    void TCodeEC::LinkLocalScopes() {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LocalVar->Count - 1); cpp_range.next(i); ) {
            TVarEC* cpp_with = EC_Expression::TVarArrayEC_GetItem(LocalVar, i);
            if (cpp_with->Kind == vkFunction) {
                if (cpp_with->FunctionValue != nullptr) {
                    if (cpp_with->FunctionValue->IsClassDefinition) {
                        cpp_with->FunctionValue->LinkLocalScopes();
                    }
                }
            }
        }
        LinkAll(LocalVar, false);
    }

    void TCodeEC_Run(TCodeEC* Self, TCodeProcessEC* Process) {
        PCodeExceptionHandler Handler{};
        PVarEC Pending{};
        ScriptCallTracePosition = 0;
        ScriptCallTraceCount = 0;
        Self->LinkAll(Self->LocalVar, false);
        TVarEC* Caught = nullptr;
        TCodeUnitEC* Item = Self->First;
        std::int32_t Steps = 0;
        std::int32_t TotalSteps = 0;
        while (Item != nullptr) {
            ++Steps;
            if (Steps > ScriptStepInterval && ScriptStepInterval > 0) {
                TotalSteps += ScriptStepInterval;
                Steps -= ScriptStepInterval;
                if (ScriptStepCallback != nullptr) {
                    EC_Expression::ScriptStepCallback(TotalSteps);
                }
            }
            if (Item->Opcode == coExpression) {
                try {
                    EC_Expression::TExpressionEC_Evaluate(Item->Expression, Process, Self, nullptr);
                } catch (...) {
                    throw;
                }
            } else if (Item->Opcode == coJump) {
                Item = Item->Target;
                continue;
            } else if (Item->Opcode == coBranchFalse) {
                try {
                    EC_Expression::TExpressionEC_Evaluate(Item->Expression, Process, Self, nullptr);
                } catch (...) {
                    throw;
                }
                if (!EC_Expression::TVarEC_IsTrue(Item->Expression->GetResult())) {
                    Item = Item->Target;
                    continue;
                }
            } else if (Item->Opcode == coExit) {
                while (true) {
                    Handler = Process->GetHandler();
                    if (Handler == nullptr || Handler->Code != Self) {
                        break;
                    }
                    Process->PopHandler();
                }
                break;
            } else if (Item->Opcode == coPushHandler) {
                Process->PushHandler(Self, Item->Target);
            } else if (Item->Opcode == coPopHandler) {
                Process->PopHandler();
            } else if (Item->Opcode == coThrow) {
                if (Item->Expression != nullptr) {
                    EC_Expression::TExpressionEC_Evaluate(Item->Expression, Process, Self, nullptr);
                    Process->PushException(Item->Expression->GetResult());
                } else if (Caught != nullptr) {
                    Process->PushException(Caught);
                    Caught = nullptr;
                }
            }
            Pending = Process->GetException();
            if (Pending != nullptr) {
                Handler = Process->GetHandler();
                if (Handler != nullptr) {
                    if (Handler->Code != Self) {
                        break;
                    }
                    Item = Handler->Handler;
                    Caught = pas::load_unaligned<TVarEC*>(Pending);
                    pas::store_unaligned<TVarEC*>(Pending, nullptr);
                    if (Item->ExceptionVar != nullptr) {
                        Item->ExceptionVar->Assume(Caught, false);
                    }
                    Process->PopHandler();
                    Process->PopException();
                    continue;
                } else {
                    Process->RaiseUnhandledExceptions();
                }
            }
            Item = Item->Next;
        }
        if (Caught != nullptr) {
            pas::free(Caught);
        }
        ScriptCallTracePosition = 0;
        ScriptCallTraceCount = 0;
    }

    void TCodeEC_RunDebug(TCodeEC* Self, TCodeProcessEC* Process, TScriptDebugState* DebugContext) {
        pas::Array<std::uint32_t, 0, 1> Events{};
        std::uint32_t WaitResult{};
        PCodeExceptionHandler Handler{};
        PVarEC Pending{};
        ScriptCallTracePosition = 0;
        ScriptCallTraceCount = 0;
        Self->LinkAll(Self->LocalVar, false);
        TVarEC* Caught = nullptr;
        Events[0] = DebugContext->StopEvent;
        Events[1] = DebugContext->ResumeEvent;
        TCodeUnitEC* Item = Self->First;
        while (Item != nullptr) {
            WaitResult = WindowsSdk::WaitForSingleObject(DebugContext->StopEvent, 0u);
            if (WaitResult == WindowsSdk::WAIT_FAILED || WaitResult == WindowsSdk::WAIT_OBJECT_0 || WaitResult == WindowsSdk::WAIT_ABANDONED_0) {
                break;
            }
            if (DebugContext->Paused && Item->SourceLength > 0 || Item->Breakpoint) {
                DebugContext->CurrentUnit = Item;
                WindowsSdk::ResetEvent(DebugContext->ResumeEvent);
                WaitResult = WindowsSdk::WaitForMultipleObjects(2u, reinterpret_cast<WindowsSdk::PWOHandleArray>(&Events), 0, WindowsSdk::INFINITE);
                if (WaitResult == WindowsSdk::WAIT_FAILED || WaitResult == WindowsSdk::WAIT_OBJECT_0 || WaitResult >= WindowsSdk::WAIT_ABANDONED_0 && WaitResult < WindowsSdk::WAIT_ABANDONED_0 + 2) {
                    break;
                }
                DebugContext->CurrentCode = Self;
                if (DebugContext->StepMode == 1) {
                    DebugContext->Paused = true;
                }
            }
            if (Item->Opcode == coExpression) {
                try {
                    EC_Expression::TExpressionEC_Evaluate(Item->Expression, Process, Self, DebugContext);
                } catch (...) {
                    throw;
                }
            } else if (Item->Opcode == coJump) {
                Item = Item->Target;
                continue;
            } else if (Item->Opcode == coBranchFalse) {
                try {
                    EC_Expression::TExpressionEC_Evaluate(Item->Expression, Process, Self, DebugContext);
                } catch (...) {
                    throw;
                }
                if (!EC_Expression::TVarEC_IsTrue(Item->Expression->GetResult())) {
                    Item = Item->Target;
                    continue;
                }
            } else if (Item->Opcode == coExit) {
                while (true) {
                    Handler = Process->GetHandler();
                    if (Handler == nullptr || Handler->Code != Self) {
                        break;
                    }
                    Process->PopHandler();
                }
                break;
            } else if (Item->Opcode == coPushHandler) {
                Process->PushHandler(Self, Item->Target);
            } else if (Item->Opcode == coPopHandler) {
                Process->PopHandler();
            } else if (Item->Opcode == coThrow) {
                if (Item->Expression != nullptr) {
                    EC_Expression::TExpressionEC_Evaluate(Item->Expression, Process, Self, nullptr);
                    Process->PushException(Item->Expression->GetResult());
                } else if (Caught != nullptr) {
                    Process->PushException(Caught);
                    Caught = nullptr;
                }
            }
            Pending = Process->GetException();
            if (Pending != nullptr) {
                Handler = Process->GetHandler();
                if (Handler != nullptr) {
                    if (Handler->Code != Self) {
                        break;
                    }
                    Item = Handler->Handler;
                    Caught = pas::load_unaligned<TVarEC*>(Pending);
                    pas::store_unaligned<TVarEC*>(Pending, nullptr);
                    if (Item->ExceptionVar != nullptr) {
                        Item->ExceptionVar->Assume(Caught, false);
                    }
                    Process->PopHandler();
                    Process->PopException();
                    continue;
                } else {
                    break;
                }
            }
            if (DebugContext->StepMode == 2 && DebugContext->CurrentCode == Self) {
                DebugContext->Paused = true;
            }
            Item = Item->Next;
        }
        if ((DebugContext->StepMode == 2 || DebugContext->StepMode == 3) && DebugContext->CurrentCode == Self) {
            DebugContext->Paused = true;
        }
        if (Caught != nullptr) {
            pas::free(Caught);
        }
        ScriptCallTracePosition = 0;
        ScriptCallTraceCount = 0;
    }

    // Does not change Kind.
    void TVarEC::SetLibrarySignature(pas::OpenArray<std::uint32_t> Signature) {
        auto cpp_array_copy = pas::copy_open_array(Signature);
        Signature = pas::open_array(cpp_array_copy);
        std::int32_t i{};
        std::int32_t Last = Signature.length() - 1;
        LibraryFunData.set_length(Last + 1);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Last); cpp_range.next(i); ) {
            LibraryFunData[i] = Signature[i];
        }
    }

    void TVarEC::p_destroy() {
        EC_Expression::TVarEC_Destroy(this);
    }

    void TVarArrayEC::p_destroy() {
        EC_Expression::TVarArrayEC_Destroy(this);
    }

    void TCodeAnalyzerEC::p_destroy() {
        EC_Expression::TCodeAnalyzerEC_Destroy(this);
    }

    void TExpressionInstrEC::p_destroy() {
        EC_Expression::TExpressionInstrEC_Destroy(this);
    }

    void TExpressionVarEC::p_destroy() {
        EC_Expression::TExpressionVarEC_Destroy(this);
    }

    void TExpressionEC::p_destroy() {
        EC_Expression::TExpressionEC_Destroy(this);
    }

    void TCodeUnitEC::p_destroy() {
        EC_Expression::TCodeUnitEC_Destroy(this);
    }

    void TCodeProcessEC::p_destroy() {
        EC_Expression::TCodeProcessEC_Destroy(this);
    }

    void TCodeEC::p_destroy() {
        EC_Expression::TCodeEC_Destroy(this);
    }

    void TCompilerEC::p_destroy() {
        EC_Expression::TCompilerEC_Destroy(this);
    }

} // namespace EC_Expression
