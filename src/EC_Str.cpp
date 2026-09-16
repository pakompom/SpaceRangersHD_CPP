#include "layout/EC_Str.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_Str {
    const EC_Str::THexDigits HexDigits = EC_Str::THexDigits{{
        u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7',
        u'8', u'9', u'a', u'b', u'c', u'd', u'e', u'f',
    }};

    std::int32_t CountDelimitedPartsW(const pas::WideString& Text, const pas::WideString& Delimiters) {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t Count = 1;
        std::int32_t TextLength = Text.length();
        std::int32_t DelimiterCount = Delimiters.length();
        if (static_cast<std::uint32_t>(TextLength) < 1) {
            return 0;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, DelimiterCount); cpp_range_2.next(j); ) {
                if (Text.read(i) == Delimiters.read(j)) {
                    ++Count;
                    break;
                }
            }
        }
        return Count;
    }

    std::int32_t GetDelimitedPartStartIndexW(const pas::WideString& Text, std::int32_t PartIndex, const pas::WideString& Delimiters) {
        std::int32_t TextLength{};
        std::int32_t DelimiterCount{};
        std::int32_t i{};
        std::int32_t j{};
        if (PartIndex > 0) {
            TextLength = Text.length();
            DelimiterCount = Delimiters.length();
            for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, DelimiterCount); cpp_range_2.next(j); ) {
                    if (Text.read(i) == Delimiters.read(j)) {
                        --PartIndex;
                        if (PartIndex == 0) {
                            return i + 1;
                        }
                        break;
                    }
                }
            }
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetSmeParEC. Str=", Text, u" np=", pas::wide_int_to_str(PartIndex), u" raz=", Delimiters}))));
        }
        return 1;
    }

    std::int32_t GetCharDelimitedPartStartIndexW(const pas::WideString& Text, std::int32_t PartIndex, char16_t Delimiter) {
        std::int32_t TextLength{};
        std::int32_t i{};
        if (PartIndex > 0) {
            TextLength = Text.length();
            for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
                if (Text.read(i) == Delimiter) {
                    --PartIndex;
                    if (PartIndex == 0) {
                        return i + 1;
                    }
                    break;
                }
            }
            return -1;
        }
        return 1;
    }

    std::int32_t GetDelimitedPartLengthW(const pas::WideString& Text, std::int32_t StartIndex, const pas::WideString& Delimiters) {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t TextLength = Text.length();
        std::int32_t DelimiterCount = Delimiters.length();
        for (auto cpp_range = pas::for_to<std::int32_t>(StartIndex, TextLength); cpp_range.next(i); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, DelimiterCount); cpp_range_2.next(j); ) {
                if (Text.read(i) == Delimiters.read(j)) {
                    return i - StartIndex;
                }
            }
        }
        return TextLength - StartIndex + 1;
    }

    pas::WideString ExtractDelimitedPartW(const pas::WideString& Text, std::int32_t PartIndex, const pas::WideString& Delimiters) {
        std::int32_t StartIndex = EC_Str::GetDelimitedPartStartIndexW(Text, PartIndex, Delimiters);
        return pas::copy(Text, StartIndex, EC_Str::GetDelimitedPartLengthW(Text, StartIndex, Delimiters));
    }

    pas::WideString ExtractDelimitedRangeW(const pas::WideString& Text, std::int32_t FirstPart, std::int32_t LastPart, const pas::WideString& Delimiters) {
        std::int32_t StartIndex = EC_Str::GetDelimitedPartStartIndexW(Text, FirstPart, Delimiters);
        std::int32_t EndIndex = EC_Str::GetDelimitedPartStartIndexW(Text, LastPart, Delimiters);
        EndIndex += EC_Str::GetDelimitedPartLengthW(Text, EndIndex, Delimiters);
        return pas::copy(Text, StartIndex, EndIndex - StartIndex);
    }

    pas::WideString ExtractNextDelimitedPartW(pas::WideString& Text, char16_t Delimiter) {
        pas::WideString Result{};
        std::int32_t i{};
        std::int32_t StartIndex = EC_Str::GetCharDelimitedPartStartIndexW(Text, 1, Delimiter);
        if (StartIndex < 0) {
            Result = Text;
            Text = pas::WideString();
            return Result;
        }
        if (StartIndex >= 3) {
            Result = pas::copy(Text, 1, StartIndex - 2);
        } else {
            Result = pas::WideString();
        }
        std::int32_t TextLength = Text.length();
        for (auto cpp_range = pas::for_to<std::int32_t>(StartIndex, TextLength); cpp_range.next(i); ) {
            Text.write(i - (StartIndex - 1)) = Text.read(i);
        }
        Text.set_length(TextLength - (StartIndex - 1));
        return Result;
    }

    pas::WideString ExtractLineCommentW(const pas::WideString& Text) {
        std::int32_t Position = pas::pos(u"//", Text);
        if (Position < 1) {
            return pas::WideString();
        }
        std::int32_t i = Position - 1;
        while (i >= 1) {
            if (Text.read(i) != u' ' && Text.read(i) != u'\t' && Text.read(i) != u'\r' && Text.read(i) != u'\n') {
                break;
            }
            --i;
        }
        return pas::copy(Text, i + 1, Text.length() - i);
    }

    pas::WideString RemoveLineCommentW(const pas::WideString& Text) {
        std::int32_t Position = pas::pos(u"//", Text);
        if (Position < 1) {
            return Text;
        }
        if (Position == 1) {
            return pas::WideString();
        }
        return pas::trim_right(pas::copy(Text, 1, Position - 1));
    }

    pas::WideString ReplaceAllWideString(const pas::WideString& Text, const pas::WideString& Search, const pas::WideString& Replacement) {
        pas::WideString Result{};
        std::int32_t j{};
        std::int32_t TextLength = Text.length();
        std::int32_t SearchLength = Search.length();
        if (TextLength < SearchLength || TextLength < 1 || SearchLength < 1) {
            return Text;
        }
        std::int32_t i = 0;
        while (i <= TextLength - SearchLength) {
            j = 0;
            while (j < SearchLength) {
                if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + (i + j)) != pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Search.data())) + j)) {
                    break;
                }
                ++j;
            }
            if (j >= SearchLength) {
                Result = pas::concat_wide({Result, Replacement});
                i += SearchLength;
            } else {
                Result = pas::concat_wide({Result, pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i)});
                ++i;
            }
        }
        if (i < TextLength) {
            return pas::concat_wide({Result, pas::copy(Text, i + 1, TextLength - i)});
        }
        return Result;
    }

    std::int32_t FindTextOffsetW(const pas::WideString& Text, const pas::WideString& Search, std::int32_t StartIndex) {
        std::int32_t TextLength = Text.length();
        std::int32_t SearchLength = Search.length();
        if (TextLength - StartIndex < SearchLength) {
            return -1;
        }
        if (TextLength < 1 && SearchLength < 1) {
            return -1;
        }
        char16_t* TextPtr = Text.pchar();
        char16_t* SearchPtr = Search.pchar();
        if (SearchLength == 1) {
            while (StartIndex <= TextLength - SearchLength) {
                if (pas::load_unaligned<char16_t>(reinterpret_cast<char16_t*>(StartIndex * static_cast<std::int32_t>(sizeof(char16_t)) + reinterpret_cast<std::uint8_t*>(TextPtr))) == pas::load_unaligned<char16_t>(SearchPtr)) {
                    return StartIndex;
                }
                ++StartIndex;
            }
        } else {
            while (StartIndex <= TextLength - SearchLength) {
                if (SysUtils::CompareMem(StartIndex * static_cast<std::int32_t>(sizeof(char16_t)) + reinterpret_cast<std::uint8_t*>(TextPtr), SearchPtr, SearchLength * 2)) {
                    return StartIndex;
                }
                ++StartIndex;
            }
        }
        return -1;
    }

    std::int32_t FindTextPosW(const pas::WideString& Search, const pas::WideString& Text) {
        return EC_Str::FindTextOffsetW(Text, Search, 0) + 1;
    }

    std::int32_t ExtractDigitsToIntW(const pas::WideString& Text) {
        std::int32_t i{};
        std::int32_t Result = 0;
        std::int32_t TextLength = Text.length();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            if (Text.read(i) >= '0' && Text.read(i) <= '9') {
                Result = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text.read(i))) + Result * 10;
            }
        }
        return Result;
    }

    std::uint8_t IsIntegerTextW(const pas::WideString& Text) {
        std::int32_t i{};
        std::int32_t TextLength = Text.length();
        if (TextLength < 1) {
            return false;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            if ((Text.read(i) < u'0' || Text.read(i) > u'9') && Text.read(i) != u'-') {
                return false;
            }
        }
        return true;
    }

    std::int32_t ExtractSignedDigitsToIntW(const pas::WideString& Text) {
        std::int32_t i{};
        std::int32_t Result = 0;
        std::int32_t TextLength = Text.length();
        std::uint8_t Negative = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            if (Text.read(i) >= '0' && Text.read(i) <= '9') {
                Result = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text.read(i))) + Result * 10;
            } else if (Text.read(i) == '-' && Result == 0) {
                Negative = true;
            }
        }
        if (Negative) {
            return -Result;
        }
        return Result;
    }

    float ExtractDecimalToSingleW(const pas::WideString& Text) {
        std::int32_t i{};
        std::int32_t Code{};
        std::int32_t TextLength = Text.length();
        if (TextLength < 1) {
            return 0.0f;
        }
        float Value = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TextLength - 1); cpp_range.next(i); ) {
            Code = pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i);
            if (Code >= '0' && Code <= '9') {
                Value = Value * 1.0E+1L + (Code - '0');
            } else if (Code == '.' || Code == ',') {
                break;
            }
        }
        ++i;
        float Divisor = 1.0E+1f;
        while (i < TextLength) {
            Code = pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i);
            if (Code >= '0' && Code <= '9') {
                Value = pas::real_divide(Code - '0', Divisor) + Value;
                Divisor = Divisor * 1.0E+1L;
            }
            ++i;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, TextLength - 1); cpp_range_2.next(i); ) {
            if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i) == '-') {
                Value = -Value;
                break;
            }
        }
        return Value;
    }

    float ParseDecimalToSingleW(const pas::WideString& Text) {
        std::int32_t i{};
        std::int32_t Code{};
        std::int32_t TextLength = Text.length();
        if (TextLength < 1) {
            return 0.0f;
        }
        float Value = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TextLength - 1); cpp_range.next(i); ) {
            Code = pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i);
            if (Code >= '0' && Code <= '9') {
                Value = Value * 1.0E+1L + (Code - '0');
            } else if (Code == '.' || Code == ',') {
                break;
            }
        }
        ++i;
        float Divisor = 1.0E+1f;
        while (i < TextLength) {
            Code = pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i);
            if (Code >= '0' && Code <= '9') {
                Value = pas::real_divide(Code - '0', Divisor) + Value;
                Divisor = Divisor * 1.0E+1L;
            }
            ++i;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, TextLength - 1); cpp_range_2.next(i); ) {
            if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i) == '-') {
                Value = -Value;
                break;
            }
        }
        return Value;
    }

    pas::WideString FloatToWideString(double Value) {
        pas::WideString Result{};
        std::uint8_t SavedSeparator = SysUtils::DecimalSeparator;
        SysUtils::DecimalSeparator = '.';
        Result = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Value), SysUtils::DecimalSeparator));
        SysUtils::DecimalSeparator = SavedSeparator;
        return Result;
    }

    pas::WideString CardinalToHexWideString(std::uint32_t Value) {
        pas::WideString Result{};
        while (Value != 0) {
            Result = pas::concat_wide({HexDigits[Value - Value / 16 * 16], Result});
            Value = Value / 16;
        }
        if (Result == u"") {
            return u"0"_w;
        }
        return Result;
    }

    pas::WideString IntToFixedWidthWideString(std::int32_t Value, std::int32_t Width) {
        pas::WideString Result{};
        std::int32_t Digit{};
        std::int32_t i{};
        while (Value > 0) {
            Digit = Value;
            Value = Value / 10;
            Digit -= Value * 10;
            Result = pas::concat_wide({pas::wide_character(static_cast<std::uint8_t>(Digit + '0')), Result});
        }
        std::int32_t TextLength = Result.length();
        if (TextLength < Width) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Width - TextLength - 1); cpp_range.next(i); ) {
                Result = pas::concat_wide({u"0", Result});
            }
            return Result;
        }
        return pas::copy(Result, 0, Width);
    }

    pas::WideString IntToWideString(std::int32_t Value) {
        pas::WideString Result{};
        std::int32_t Magnitude = pas::abs(Value);
        while (Magnitude > 0) {
            Result = pas::concat_wide({pas::wide_character(static_cast<std::uint8_t>(Magnitude % 10 + '0')), Result});
            Magnitude = Magnitude / 10;
        }
        if (Result == u"") {
            Result = u"0"_w;
        }
        if (Value < 0) {
            return pas::concat_wide({u"-", Result});
        }
        return Result;
    }

    pas::WideString BoolToWideString(std::uint8_t Value) {
        if (!Value) {
            return u"False"_w;
        }
        return u"True"_w;
    }

    pas::WideString TrimWideString(const pas::WideString& Text) {
        pas::WideString Result{};
        std::int32_t Code{};
        std::int32_t TextLength = Text.length();
        std::int32_t FirstIndex = 0;
        while (FirstIndex < TextLength) {
            Code = pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + FirstIndex);
            if (Code != ' ' && Code != 9 && Code != 13 && Code != 10 && Code != 0) {
                break;
            }
            ++FirstIndex;
        }
        if (FirstIndex >= TextLength) {
            return pas::WideString();
        }
        std::int32_t LastIndex = TextLength - 1;
        while (LastIndex >= 0) {
            Code = pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + LastIndex);
            if (Code != ' ' && Code != 9 && Code != 13 && Code != 10 && Code != 0) {
                break;
            }
            --LastIndex;
        }
        if (LastIndex < FirstIndex) {
            return pas::WideString();
        }
        Result.set_length(LastIndex - FirstIndex + 1);
        Windows::CopyMemory(Result.pchar(), EC_Mem::AddPointerOffset(Text.pchar(), FirstIndex * 2), (LastIndex - FirstIndex + 1) * 2);
        return Result;
    }

    pas::WideString UpperCaseWideString(const pas::WideString& Text) {
        pas::WideString Result{};
        std::int32_t i{};
        std::int32_t j{};
        Result = Text;
        std::int32_t TextLength = Result.length();
        std::int32_t PairCount = GR_Main::WideCaseTable.length() - 1 + 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TextLength - 1); cpp_range.next(i); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PairCount - 1); cpp_range_2.next(j); ) {
                if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Result.data())) + i) == GR_Main::WideCaseTable[j].LowerChar) {
                    pas::store_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Result.data())) + i, GR_Main::WideCaseTable[j].UpperChar);
                    break;
                }
            }
        }
        return Result;
    }

    pas::WideString LowerCaseWideString(const pas::WideString& Text) {
        pas::WideString Result{};
        std::int32_t i{};
        std::int32_t j{};
        Result = Text;
        std::int32_t TextLength = Result.length();
        std::int32_t PairCount = GR_Main::WideCaseTable.length() - 1 + 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TextLength - 1); cpp_range.next(i); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PairCount - 1); cpp_range_2.next(j); ) {
                if (pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Result.data())) + i) == GR_Main::WideCaseTable[j].UpperChar) {
                    pas::store_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Result.data())) + i, GR_Main::WideCaseTable[j].LowerChar);
                    break;
                }
            }
        }
        return Result;
    }

    pas::WideString RemoveWideStringChars(const pas::WideString& Text, pas::WideString Chars) {
        pas::WideString Result{};
        std::int32_t i{};
        std::int32_t j{};
        std::uint8_t Found{};
        char16_t Current{};
        std::uint8_t Changed = false;
        Result = Text;
        std::int32_t TextLength = Result.length();
        std::int32_t CharsLength = Chars.length();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, TextLength); cpp_range.next(i); ) {
            Current = Result.read(i);
            Found = false;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, CharsLength); cpp_range_2.next(j); ) {
                if (Chars.read(j) == Current) {
                    Found = true;
                    break;
                }
            }
            if (Found) {
                Changed = true;
            } else {
                ++Count;
                if (Changed) {
                    Result.write(Count) = Current;
                }
            }
        }
        if (Changed) {
            if (Count > 0) {
                return EC_Str::CopyWideStringUnchecked(Result, 1, Count);
            }
            return pas::WideString();
        }
        return Result;
    }

    std::int32_t GetTextTagLengthW(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 2) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) == u'<') {
            return 1;
        }
        std::int32_t i = 1;
        while (i < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + i) == u'>') {
                break;
            }
            ++i;
        }
        if (i < CharCount) {
            return i + 1;
        }
        return Result;
    }

    std::uint8_t MatchTextTagPrefixW(char16_t* Text, std::int32_t CharCount, const pas::WideString& Pattern, const pas::WideString& AlternatePattern) {
        std::int32_t i{};
        std::uint8_t Result = false;
        std::int32_t PatternLength = Pattern.length();
        if (AlternatePattern.length() != PatternLength) {
            return Result;
        }
        if (PatternLength + 1 > CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PatternLength - 1); cpp_range.next(i); ) {
            if (pas::load_unaligned<char16_t>(Text + (1 + i)) != Pattern.read(1 + i) && pas::load_unaligned<char16_t>(Text + (1 + i)) != AlternatePattern.read(1 + i)) {
                return Result;
            }
        }
        return true;
    }

    pas::WideString RemoveTextTagsW(const pas::WideString& Text) {
        pas::WideString Result{};
        std::int32_t TagLength{};
        std::int32_t i = 0;
        while (i < Text.length()) {
            TagLength = EC_Str::GetTextTagLengthW(Text.pchar() + i, Text.length() - i);
            if (TagLength > 0) {
                i += TagLength;
            } else {
                Result = pas::concat_wide({Result, Text.read(i + 1)});
                ++i;
            }
        }
        return Result;
    }

    pas::WideString RemoveMatchingTextTagsW(pas::WideString Text, const pas::WideString& Pattern, const pas::WideString& AlternatePattern) {
        pas::WideString Result{};
        std::int32_t TagLength{};
        std::int32_t i = 0;
        while (i < Text.length()) {
            TagLength = EC_Str::GetTextTagLengthW(Text.pchar() + i, Text.length() - i);
            if (TagLength > 0) {
                if (EC_Str::MatchTextTagPrefixW(Text.pchar() + i, Text.length() - i, Pattern, AlternatePattern)) {
                    i += TagLength;
                } else {
                    Result = pas::concat_wide({Result, pas::copy(Text, i + 1, TagLength)});
                    i += TagLength;
                }
            } else {
                Result = pas::concat_wide({Result, pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i)});
                ++i;
            }
        }
        return Result;
    }

    pas::WideString ExtractFileNameNoExtW(const pas::WideString& Path) {
        pas::WideString Result{};
        std::int32_t Count = EC_Str::CountDelimitedPartsW(Path, u"\\/"_wref.get());
        Result = EC_Str::ExtractDelimitedPartW(Path, Count - 1, u"\\/"_wref.get());
        Count = EC_Str::CountDelimitedPartsW(Result, u"."_wref.get());
        if (Count > 1) {
            return EC_Str::ExtractDelimitedRangeW(Result, 0, Count - 2, u"."_wref.get());
        }
        return Result;
    }

    pas::WideString ExtractFileExtNoDotW(const pas::WideString& Path) {
        pas::WideString Result{};
        std::int32_t Count = EC_Str::CountDelimitedPartsW(Path, u"\\/"_wref.get());
        Result = EC_Str::ExtractDelimitedPartW(Path, Count - 1, u"\\/"_wref.get());
        Count = EC_Str::CountDelimitedPartsW(Result, u"."_wref.get());
        if (Count > 1) {
            return EC_Str::ExtractDelimitedPartW(Result, Count - 1, u"."_wref.get());
        }
        return pas::WideString();
    }

    pas::WideString ExtractFileDirW(const pas::WideString& Path) {
        std::int32_t Count = EC_Str::CountDelimitedPartsW(Path, u"\\/"_wref.get());
        if (Count <= 1) {
            return pas::WideString();
        }
        return EC_Str::ExtractDelimitedRangeW(Path, 0, Count - 2, u"\\/"_wref.get());
    }

    pas::WideString DecodeTextW(pas::WideString Text) {
        pas::WideString Result{};
        std::int32_t TextLength = Text.length();
        std::int32_t i = 0;
        while (i < TextLength) {
            Result = pas::concat_wide({Result, pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i)});
            i += 2;
        }
        return Result;
    }

    pas::WideString CopyWideStringUnchecked(pas::WideString Text, std::int32_t Index, std::int32_t Count) {
        pas::WideString Result{};
        Result.set_length(Count);
        Windows::CopyMemory(Result.pchar(), EC_Mem::AddPointerOffset(Text.pchar(), Index * 2 - 2), Count * 2);
        return Result;
    }

    void WriteRegistryStringLegacy(std::uint32_t RootKey, pas::WideString KeyPath, pas::WideString ValueName, pas::WideString Value) {
        WindowsSdk::HKEY Key{};
        std::uint32_t Disposition{};
        pas::AnsiString AnsiValue{};
        if (WindowsSdk::RegCreateKeyExW(RootKey, KeyPath.pchar(), 0u, nullptr, 0u, WindowsSdk::KEY_WRITE, nullptr, Key, &Disposition) != WindowsSdk::ERROR_SUCCESS) {
            return;
        }
        AnsiValue = static_cast<pas::AnsiString>(Value);
        if (WindowsSdk::RegSetValueExW(Key, ValueName.pchar(), 0u, WindowsSdk::REG_SZ, AnsiValue.pchar(), AnsiValue.length() + 1) != WindowsSdk::ERROR_SUCCESS) {
            WindowsSdk::RegCloseKey(Key);
            return;
        }
        WindowsSdk::RegCloseKey(Key);
    }

    pas::WideString EncodeTextW(pas::WideString Text) {
        pas::WideString Result{};
        std::int32_t TextLength = Text.length();
        std::int32_t LastPair = GR_Main::WideCaseTable.length() - 1;
        std::int32_t i = 0;
        while (i < TextLength) {
            Result = pas::concat_wide({Result, pas::load_unaligned<char16_t>(static_cast<char16_t*>(static_cast<void*>(Text.data())) + i)});
            if (pas::random(10, &System::RandSeed) >= 8) {
                Result = pas::concat_wide({Result, GR_Main::WideCaseTable[pas::random(LastPair + 1, &System::RandSeed)].UpperChar});
            } else {
                Result = pas::concat_wide({Result, GR_Main::WideCaseTable[pas::random(LastPair + 1, &System::RandSeed)].LowerChar});
            }
            ++i;
        }
        return Result;
    }

    pas::WideString TransliterateCyrillicToLatin(pas::WideString Text) {
        pas::WideString Result{};
        Result = std::move(Text);
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0410"_wref.get(), u"A"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0430"_wref.get(), u"a"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0411"_wref.get(), u"B"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0431"_wref.get(), u"b"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0412"_wref.get(), u"V"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0432"_wref.get(), u"v"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0413"_wref.get(), u"G"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0433"_wref.get(), u"g"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0414"_wref.get(), u"D"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0434"_wref.get(), u"d"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0415"_wref.get(), u"E"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0435"_wref.get(), u"e"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0401"_wref.get(), u"Yo"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0451"_wref.get(), u"yo"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0416"_wref.get(), u"Zh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0436"_wref.get(), u"zh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0417"_wref.get(), u"Z"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0437"_wref.get(), u"z"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0418"_wref.get(), u"I"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0438"_wref.get(), u"i"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0419"_wref.get(), u"J"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0439"_wref.get(), u"j"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u041a"_wref.get(), u"K"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u043a"_wref.get(), u"k"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u041b"_wref.get(), u"L"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u043b"_wref.get(), u"l"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u041c"_wref.get(), u"M"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u043c"_wref.get(), u"m"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u041d"_wref.get(), u"N"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u043d"_wref.get(), u"n"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u041e"_wref.get(), u"O"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u043e"_wref.get(), u"o"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u041f"_wref.get(), u"P"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u043f"_wref.get(), u"p"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0420"_wref.get(), u"R"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0440"_wref.get(), u"r"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0421"_wref.get(), u"S"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0441"_wref.get(), u"s"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0422"_wref.get(), u"T"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0442"_wref.get(), u"t"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0423"_wref.get(), u"U"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0443"_wref.get(), u"u"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0424"_wref.get(), u"F"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0444"_wref.get(), u"f"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0425"_wref.get(), u"Kh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0445"_wref.get(), u"kh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0426"_wref.get(), u"Ts"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0446"_wref.get(), u"ts"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0427"_wref.get(), u"Ch"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0447"_wref.get(), u"ch"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0428"_wref.get(), u"Sh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0448"_wref.get(), u"sh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0429"_wref.get(), u"Shh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u0449"_wref.get(), u"shh"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u042a"_wref.get(), u"\""_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u044a"_wref.get(), u"\""_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u042b"_wref.get(), u"Y"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u044b"_wref.get(), u"y"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u042c"_wref.get(), u"`"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u044c"_wref.get(), u"`"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u042d"_wref.get(), u"E"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u044d"_wref.get(), u"e"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u042e"_wref.get(), u"Yu"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u044e"_wref.get(), u"yu"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"\u042f"_wref.get(), u"Ya"_wref.get());
        return EC_Str::ReplaceAllWideString(Result, u"\u044f"_wref.get(), u"ya"_wref.get());
    }

    void TStringsEC_Create(TStringsEC* Self) {
        pas::object_create(Self);
    }

    void TStringsEC_Destroy(TStringsEC* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    void TStringsEC::Clear() {
        while (FirstElement != nullptr) {
            RemoveAndFreeElement(LastElement);
        }
        CurrentElement = nullptr;
    }

    TStringsElEC* TStringsEC::AddEmptyElement() {
        TStringsElEC* Item = pas::make_object<TStringsElEC>();
        AppendElement(Item);
        return Item;
    }

    void TStringsEC::AppendElement(TStringsElEC* Item) {
        if (LastElement != nullptr) {
            LastElement->Next = Item;
        }
        Item->Prev = LastElement;
        Item->Next = nullptr;
        LastElement = Item;
        if (FirstElement == nullptr) {
            FirstElement = Item;
        }
    }

    void TStringsEC::RemoveAndFreeElement(TStringsElEC* Item) {
        if (Item->Prev != nullptr) {
            Item->Prev->Next = Item->Next;
        }
        if (Item->Next != nullptr) {
            Item->Next->Prev = Item->Prev;
        }
        if (LastElement == Item) {
            LastElement = Item->Prev;
        }
        if (FirstElement == Item) {
            FirstElement = Item->Next;
        }
        pas::free(Item);
    }

    TStringsElEC* TStringsEC::GetElement(std::int32_t Index) {
        TStringsElEC* Item = FirstElement;
        while (Item != nullptr) {
            if (Index == 0) {
                return Item;
            }
            --Index;
            Item = Item->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TStringsEC.El_Get. i=", SysUtils::IntToStr(Index)})));
    }

    TStringsElEC* TStringsEC::EnsureElement(std::int32_t Index) {
        if (Index < 0) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TStringsEC.El_GetEx. i=", SysUtils::IntToStr(Index)})));
        }
        TStringsElEC* Item = FirstElement;
        while (Item != nullptr) {
            if (Index == 0) {
                return Item;
            }
            --Index;
            Item = Item->Next;
        }
        while (Index >= 0) {
            AddEmptyElement();
            --Index;
        }
        return LastElement;
    }

    std::int32_t TStringsEC::GetCount() {
        TStringsElEC* Item = FirstElement;
        std::int32_t Result = 0;
        while (Item != nullptr) {
            ++Result;
            Item = Item->Next;
        }
        return Result;
    }

    pas::WideString TStringsEC::GetTextAt(std::int32_t Index) {
        return EnsureElement(Index)->Text;
    }

    void* TStringsEC::GetDataAt(std::int32_t Index) {
        return EnsureElement(Index)->Data;
    }

    void TStringsEC::SetDataAt(std::int32_t Index, void* Data) {
        EnsureElement(Index)->Data = Data;
    }

    std::int32_t TStringsEC::IndexOf(const pas::WideString& Text) {
        TStringsElEC* Item = FirstElement;
        std::int32_t i = 0;
        while (Item != nullptr) {
            if (Item->Text == Text) {
                return i;
            }
            ++i;
            Item = Item->Next;
        }
        return -1;
    }

    void TStringsEC::Add(const pas::WideString& Text) {
        AddEmptyElement()->Text = Text;
    }

    void TStringsEC::AddSlice(char16_t* Text, std::int32_t CharCount) {
        TStringsElEC* Item{};
        Item = AddEmptyElement();
        if (CharCount > 0) {
            Item->Text.set_length(CharCount);
            Windows::CopyMemory(Item->Text.pchar(), Text, CharCount * 2);
        }
    }

    void TStringsEC::Delete(std::int32_t Index) {
        TStringsElEC* Item = GetElement(Index);
        if (Item == CurrentElement) {
            CurrentElement = Item->Next;
            if (CurrentElement == nullptr) {
                CurrentElement = Item->Prev;
            }
        }
        RemoveAndFreeElement(Item);
    }

    pas::WideString TStringsEC::GetCurrentText() {
        if (CurrentElement == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TStringsEC.Get."_a));
        }
        return CurrentElement->Text;
    }

    void* TStringsEC::GetCurrentData() {
        if (CurrentElement == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TStringsEC.GetData."_a));
        }
        return CurrentElement->Data;
    }

    std::uint8_t TStringsEC::IsAtEnd() {
        return !(CurrentElement != nullptr);
    }

    std::uint8_t TStringsEC::IsAtLast() {
        return !(CurrentElement->Next != nullptr);
    }

    void TStringsEC::First() {
        CurrentElement = FirstElement;
    }

    void TStringsEC::Next() {
        CurrentElement = CurrentElement->Next;
    }

    std::uint8_t TStringsEC::IsEmpty() {
        return FirstElement == nullptr;
    }

    void TStringsEC::SetText(const pas::WideString& Text) {
        char16_t* Start{};
        Clear();
        char16_t* Cursor = Text.pchar();
        if (Cursor != nullptr) {
            while (pas::load_unaligned<char16_t>(Cursor) != u'\000') {
                Start = Cursor;
                while (pas::load_unaligned<char16_t>(Cursor) != u'\000' && pas::load_unaligned<char16_t>(Cursor) != u'\n' && pas::load_unaligned<char16_t>(Cursor) != u'\r') {
                    ++Cursor;
                }
                AddSlice(Start, pas::idiv(static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(Cursor) - reinterpret_cast<std::uint8_t*>(Start)), static_cast<std::int32_t>(sizeof(char16_t))));
                if (pas::load_unaligned<char16_t>(Cursor) == u'\r') {
                    ++Cursor;
                }
                if (pas::load_unaligned<char16_t>(Cursor) == u'\n') {
                    ++Cursor;
                }
            }
        }
    }

    pas::WideString TStringsEC::GetText() {
        pas::WideString Result{};
        TStringsElEC* Item = FirstElement;
        while (Item != nullptr) {
            if (Item->Next == nullptr) {
                Result = pas::concat_wide({Result, Item->Text});
            } else {
                Result = pas::concat_wide({Result, Item->Text, u"\r", u"\n"});
            }
            Item = Item->Next;
        }
        return Result;
    }

    void TStringsEC::p_destroy() {
        EC_Str::TStringsEC_Destroy(this);
    }

} // namespace EC_Str
