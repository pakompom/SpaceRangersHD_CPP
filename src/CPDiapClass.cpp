#include "layout/CPDiapClass.hpp"
#include "types/EC_Buf.hpp"
#include "types/TextFieldClass.hpp"
#include "types/ValueListClass.hpp"
#include "units/CPDiapClass.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"

namespace CPDiapClass {
    void TCPDiapazone_Create(TCPDiapazone* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Clear();
    }

    void TCPDiapazone_Destroy(TCPDiapazone* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TCPDiapazone::Clear() {
        RangeCount = 0;
        RangeStarts.set_length(RangeCount);
        RangeEnds.set_length(RangeCount);
    }

    void TCPDiapazone::LoadFromReader(EC_Buf::TBufEC* Reader) {
        TextFieldClass::TTextField* Text = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Text->LoadTextLinesFromReader(Reader);
        LoadFromText(Text->Text);
        pas::destroy(Text);
    }

    // Requires at least one range.
    std::int64_t TCPDiapazone::GetMinimum() {
        std::int32_t i{};
        std::int64_t Result = RangeStarts[0];
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
            if (RangeStarts[i] <= Result) {
                Result = RangeStarts[i];
            }
        }
        return Result;
    }

    // Requires at least one range.
    std::int64_t TCPDiapazone::GetMaximum() {
        std::int32_t i{};
        std::int64_t Result = RangeEnds[0];
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
            if (RangeEnds[i] >= Result) {
                Result = RangeEnds[i];
            }
        }
        return Result;
    }

    // Zero when empty. Sampling weights overlaps repeatedly; lengths and results are 32-bit.
    std::int32_t TCPDiapazone::GetRandomValue() {
        std::int32_t i{};
        std::int32_t RandomValue{};
        pas::DynArray<std::int64_t> Ends{};
        pas::DynArray<std::int64_t> Starts{};
        std::int32_t Result = 0;
        if (RangeCount > 0) {
            Ends.set_length(RangeCount);
            Starts.set_length(RangeCount);
            RandomValue = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
                Starts[i] = RandomValue;
                Ends[i] = RangeEnds[i] - RangeStarts[i] + Starts[i];
                RandomValue = RandomValue + RangeEnds[i] - RangeStarts[i] + 1;
            }
            RandomValue = pas::random(RandomValue, &System::RandSeed);
            // Native scan includes RangeCount and draws again within the selected range.
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, RangeCount); cpp_range_2.next(i); ) {
                if (RandomValue >= Starts[i] && RandomValue <= Ends[i]) {
                    RandomValue = pas::random(static_cast<std::int32_t>(RangeEnds[i] - RangeStarts[i] + 1), &System::RandSeed);
                    Result = RandomValue + RangeStarts[i];
                    break;
                }
            }
        }
        return Result;
    }

    // Rounds with System.Round first.
    std::uint8_t TCPDiapazone::Contains(pas::Extended Value) {
        std::int32_t i{};
        std::int64_t Rounded = System::Round(Value);
        std::uint8_t Result = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
            if (RangeStarts[i] <= Rounded && RangeEnds[i] >= Rounded) {
                return Result;
            }
        }
        return false;
    }

    // Uses [ahb;c] and signed low 32-bit endpoints; empty output is '['.
    pas::WideString TCPDiapazone::ToText() {
        pas::WideString Result{};
        std::int32_t i{};
        Result = u"["_w;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
            if (RangeStarts[i] == RangeEnds[i]) {
                Result = pas::concat_wide({Result, EC_Str::IntToWideString(RangeStarts[i])});
            } else {
                Result = pas::concat_wide({Result, EC_Str::IntToWideString(RangeStarts[i]), u"h", EC_Str::IntToWideString(RangeEnds[i])});
            }
            if (i < RangeCount - 1) {
                Result = pas::concat_wide({Result, u";"});
            } else {
                Result = pas::concat_wide({Result, u"]"});
            }
        }
        return Result;
    }

    // Ignores Source.AcceptListed.
    void TCPDiapazone::LoadFromValues(ValueListClass::TValuesList*& Source) {
        std::int32_t i{};
        RangeCount = Source->Count;
        RangeStarts.set_length(RangeCount);
        RangeEnds.set_length(RangeCount);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
            RangeStarts[i] = Source->Values[i + 1];
            RangeEnds[i] = Source->Values[i + 1];
        }
    }

    void TCPDiapazone::Assign(TCPDiapazone*& Source) {
        std::int32_t i{};
        RangeCount = Source->RangeCount;
        RangeStarts.set_length(RangeCount);
        RangeEnds.set_length(RangeCount);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(i); ) {
            RangeStarts[i] = Source->RangeStarts[i];
            RangeEnds[i] = Source->RangeEnds[i];
        }
    }

    // Preserves overlapping and duplicate ranges.
    void TCPDiapazone::Append(TCPDiapazone*& Source) {
        std::int32_t i{};
        if (Source->RangeCount > 0) {
            RangeStarts.set_length(RangeCount + Source->RangeCount);
            RangeEnds.set_length(RangeCount + Source->RangeCount);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Source->RangeCount - 1); cpp_range.next(i); ) {
                RangeStarts[RangeCount + i] = Source->RangeStarts[i];
                RangeEnds[RangeCount + i] = Source->RangeEnds[i];
            }
            RangeCount += Source->RangeCount;
        }
    }

    // Swaps reversed bounds; does not merge ranges.
    void TCPDiapazone::AddRange(std::int64_t MinValue, std::int64_t MaxValue) {
        std::int64_t Temporary{};
        ++RangeCount;
        RangeStarts.set_length(RangeCount);
        RangeEnds.set_length(RangeCount);
        if (MinValue > MaxValue) {
            Temporary = MinValue;
            MinValue = MaxValue;
            MaxValue = Temporary;
        }
        RangeStarts[RangeCount - 1] = MinValue;
        RangeEnds[RangeCount - 1] = MaxValue;
    }

    // Truncates to Int64; caught conversion errors preserve existing ranges.
    void TCPDiapazone::AddValue(pas::Extended Value) {
        std::int64_t IntegerValue = 0;
        std::uint8_t Failed = false;
        try {
            IntegerValue = System::Trunc(Value);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::class_cast_if<pas::MathError*>(cpp_exception)) {
                Failed = true;
            } else {
                throw;
            }
        }
        if (!Failed) {
            ++RangeCount;
            RangeStarts.set_length(RangeCount);
            RangeEnds.set_length(RangeCount);
            RangeStarts[RangeCount - 1] = IntegerValue;
            RangeEnds[RangeCount - 1] = IntegerValue;
        }
    }

    // Accepts [a..b;c] or [ahb;c]. Endpoints beyond +/-200000000 can expand intervals unexpectedly; '..' normalization can overread.
    void TCPDiapazone::LoadFromText(pas::WideString Text) {
        std::int32_t i{};
        std::int64_t Value{};
        std::int64_t Minimum{};
        std::int64_t Maximum{};
        pas::WideString NumberText{};
        pas::WideString Normalized{};
        std::uint8_t Failed{};
        Clear();
        std::int32_t Count = Text.length();
        if (Text != u";") {
            // Native parsing retains the original Count after this shortening replacement.
            Normalized = EC_Str::ReplaceAllWideString(Text, u".."_wref.get(), u"h"sv);
            i = 1;
            NumberText = pas::WideString();
            Minimum = 200000000;
            Maximum = -200000000;
            Failed = false;
            while (i <= Count) {
                if (Normalized.read(i) >= u'0' && Normalized.read(i) <= u'9' || Normalized.read(i) == u'-') {
                    NumberText = pas::concat_wide({NumberText, Normalized.read(i)});
                    ++i;
                } else if (Normalized.read(i) == u'h' || Normalized.read(i) == u';' || Normalized.read(i) == u']') {
                    Value = 0;
                    try {
                        Value = EC_Str::ExtractSignedDigitsToIntW(pas::view(NumberText));
                    } catch (...) {
                        auto cpp_exception = pas::caught_object();
                        if (pas::class_cast_if<pas::MathError*>(cpp_exception)) {
                            Failed = true;
                        } else if (pas::class_cast_if<pas::ConvertError*>(cpp_exception)) {
                            Failed = true;
                        } else {
                            throw;
                        }
                    }
                    if (!Failed) {
                        if (Minimum > Value) {
                            Minimum = Value;
                        }
                        if (Maximum < Value) {
                            Maximum = Value;
                        }
                    }
                    Failed = false;
                    NumberText = pas::WideString();
                    if (Normalized.read(i) == u';' || Normalized.read(i) == u']') {
                        AddRange(Minimum, Maximum);
                        Minimum = 200000000;
                        Maximum = -200000000;
                        NumberText = pas::WideString();
                    }
                    ++i;
                } else {
                    ++i;
                }
            }
        }
    }

    void TCPDiapazone::p_destroy() {
        CPDiapClass::TCPDiapazone_Destroy(this);
    }

} // namespace CPDiapClass
