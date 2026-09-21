#include "layout/ValueListClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/ValueListClass.hpp"

namespace ValueListClass {
    // The initial character filter is overwritten; unrelated characters survive. Does not modify Self.
    pas::WideString TValuesList::NormalizeSemicolonText(pas::WideString Text) {
        pas::WideString Result{};
        std::int32_t i{};
        pas::WideString Normalized{};
        Normalized = pas::WideString();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Text.length()); cpp_range.next(i); ) {
            if (Text.read(i) >= u'0' && Text.read(i) <= u'9' || Text.read(i) == u';' || Text.read(i) == u',' || Text.read(i) == u'-') {
                Normalized = pas::concat_wide({Normalized, Text.read(i)});
            }
        }
        Result = Normalized;
        // The native routine discards the character-filtered string here.
        Normalized = pas::concat_wide({u"(", Text, u")"});
        do {
            Result = Normalized;
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u","_wref.get(), u";"sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u";;"_wref.get(), u";"sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u"-;"_wref.get(), u";"sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u"--"_wref.get(), u""sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u"(-;"_wref.get(), u"("sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u"(-)"_wref.get(), u"("sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u"(;"_wref.get(), u"("sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u";-)"_wref.get(), u")"sv);
            Normalized = EC_Str::ReplaceAllWideString(Normalized, u";)"_wref.get(), u")"sv);
        } while (!(Result == Normalized));
        Result = EC_Str::ReplaceAllWideString(Result, u"("_wref.get(), u""sv);
        return EC_Str::ReplaceAllWideString(Result, u")"_wref.get(), u""sv);
    }

    void TValuesList_Create(TValuesList* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Clear();
    }

    void TValuesList_Destroy(TValuesList* Self) {
        Self->Values = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TValuesList::LoadFromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Count = EC_Buf::TBufEC_GetInt32(Reader);
        AcceptListed = EC_Buf::TBufEC_GetBoolean(Reader);
        Values.set_length(Count + 2);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            Values[i] = EC_Buf::TBufEC_GetInt32(Reader);
        }
    }

    void TValuesList::Clear() {
        Values.set_length(1);
        Count = 0;
        AcceptListed = true;
    }

    void TValuesList::LoadFromSemicolonText(pas::WideString Text) {
        pas::WideString NumberText{};
        Clear();
        std::int32_t i = 1;
        Count = 0;
        NumberText = pas::WideString();
        Text = EC_Str::TrimWideString(TValuesList::NormalizeSemicolonText(Text));
        if (Text.length() != 0) {
            while (i <= Text.length()) {
                if (i == Text.length() || Text.read(i + 1) == u';') {
                    ++Count;
                }
                ++i;
            }
            Values.set_length(Count + 2);
            Count = 0;
            // Native parsing starts at zero, including Text[0].
            i = 0;
            while (i <= Text.length()) {
                if (Text.read(i) != u';') {
                    NumberText = pas::concat_wide({NumberText, Text.read(i)});
                }
                if (i == Text.length() || Text.read(i + 1) == u';') {
                    ++Count;
                    Values[Count] = EC_Str::ExtractSignedDigitsToIntW(pas::view(NumberText));
                    NumberText = pas::WideString();
                }
                ++i;
            }
        }
    }

    // An empty list accepts every value, regardless of AcceptListed.
    std::uint8_t TValuesList::AcceptsValue(std::int32_t Value) {
        std::int32_t i{};
        std::uint8_t Result = true;
        if (Count != 0) {
            Result = AcceptListed;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
                if (Values[i] == Value) {
                    return Result;
                }
            }
            return static_cast<std::uint8_t>(Result ^ 1);
        }
        return Result;
    }

    // Zero divisors are unchecked.
    std::uint8_t TValuesList::AcceptsMultiple(std::int32_t Value) {
        std::int32_t i{};
        std::uint8_t Result = true;
        if (Count != 0) {
            Result = AcceptListed;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
                if (pas::imod(Value, Values[i]) == 0) {
                    return Result;
                }
            }
            return static_cast<std::uint8_t>(Result ^ 1);
        }
        return Result;
    }

    void TValuesList::p_destroy() {
        ValueListClass::TValuesList_Destroy(this);
    }

} // namespace ValueListClass
