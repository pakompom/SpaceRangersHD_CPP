#include "layout/ParameterClass.hpp"
#include "types/TextFieldClass.hpp"
#include "types/TextQuestInterface.hpp"
#include "units/CPDiapClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EventClass.hpp"
#include "units/MessageText.hpp"
#include "units/ParViewStringClass.hpp"
#include "units/ParameterClass.hpp"
#include "units/ValueListClass.hpp"

namespace ParameterClass {
    void TParameter_Create(TParameter* Self, std::int32_t Index) {
        EC_Struct::TObjectEx_Create(Self);
        Self->ViewStringCount = 0;
        Self->ViewStringCapacity = 0;
        Self->ViewStrings.set_length(0);
        Self->NameText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->ValueText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->CriticalEvent = pas::construct_call<EventClass::TEvent>(EventClass::TEvent_Create);
        Self->CriticalEventOverride = nullptr;
        Self->InitialRange = pas::construct_call<CPDiapClass::TCPDiapazone>(CPDiapClass::TCPDiapazone_Create);
        Self->Reset(Index);
    }

    void TParameter_Destroy(TParameter* Self) {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->ViewStringCapacity); cpp_range.next(i); ) {
            pas::free(Self->ViewStrings[i]);
            Self->ViewStrings[i] = nullptr;
        }
        Self->ViewStrings.set_length(0);
        pas::free(Self->NameText);
        Self->NameText = nullptr;
        pas::free(Self->ValueText);
        Self->ValueText = nullptr;
        pas::free(Self->CriticalEvent);
        Self->CriticalEvent = nullptr;
        pas::free(Self->InitialRange);
        Self->InitialRange = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TParameter::Reset(std::int32_t Index) {
        IsMoney = false;
        Enabled = false;
        Hidden = false;
        ShowWhenZero = true;
        CriticalAtMinimum = true;
        MinValue = 0;
        MaxValue = 1;
        InitialRange->Clear();
        ViewStringCount = 1;
        EnsureViewStringCapacity(1, Index);
        ViewStrings[1]->MinValue = MinValue;
        ViewStrings[1]->MaxValue = MaxValue;
        Value = 0;
        CriticalOutcome = TextQuestInterface::qoNone;
        NameText->Text = pas::concat_wide({MessageText::QuestMessages->GetTextOrKey(u"ParameterDefaultName"_w), u" ", EC_Str::IntToWideString(Index)});
        ValueText->Text = pas::concat_wide({MessageText::QuestMessages->GetTextOrKey(u"ParameterDefaultName"_w), u" ", EC_Str::IntToWideString(Index), u": <>"});
        ViewStrings[1]->Text->Text = ValueText->Text;
        CriticalEvent->ClearTextFields();
        CriticalEventOverride = nullptr;
        CriticalEvent->Text->Text = pas::concat_wide({MessageText::QuestMessages->GetTextOrKey(u"ParameterDefaultCriticalMessage"_w), u" ", EC_Str::IntToWideString(Index)});
    }

    void TParameter::EnsureViewStringCapacity(std::int32_t RequiredCapacity, std::int32_t ParameterIndex) {
        while (RequiredCapacity > ViewStringCapacity) {
            ++ViewStringCapacity;
            ViewStrings.set_length(ViewStringCapacity + 1);
            ViewStrings[ViewStringCapacity] = pas::construct_call<ParViewStringClass::TParViewString>(ParViewStringClass::TParViewString_Create, pas::concat_wide({MessageText::QuestMessages->GetTextOrKey(u"ParameterDefaultName"_w), u" ", EC_Str::IntToWideString(ParameterIndex), u": <>"}));
        }
    }

    pas::WideString TParameter::GetValueText(std::int32_t Value) {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ViewStringCount); cpp_range.next(i); ) {
            if (ViewStrings[i]->MinValue <= Value && ViewStrings[i]->MaxValue >= Value) {
                return EC_Str::TrimWideString(ViewStrings[i]->Text->Text);
            }
        }
        if (ViewStrings[ViewStringCount]->MaxValue < Value) {
            return ViewStrings[ViewStringCount]->Text->Text;
        }
        return ViewStrings[1]->Text->Text;
    }

    std::int32_t TParameter::GetNonCriticalMinimum() {
        std::int32_t Result = MinValue;
        if (CriticalOutcome != TextQuestInterface::qoNone && CriticalOutcome != TextQuestInterface::qoSuccess && CriticalAtMinimum) {
            ++Result;
        }
        return Result;
    }

    std::int32_t TParameter::GetNonCriticalMaximum() {
        std::int32_t Result = MaxValue;
        if (CriticalOutcome != TextQuestInterface::qoNone && CriticalOutcome != TextQuestInterface::qoSuccess && static_cast<std::uint8_t>(CriticalAtMinimum ^ 1)) {
            --Result;
        }
        return Result;
    }

    void TParameter::SetValue(std::int32_t NewValue) {
        if (IsMoney) {
            if (NewValue < 0) {
                Value = 0;
            } else {
                Value = NewValue;
            }
        } else if (NewValue > MaxValue) {
            Value = MaxValue;
        } else if (NewValue < MinValue) {
            Value = MinValue;
        } else {
            Value = NewValue;
        }
    }

    void TParameter::LoadFromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        CriticalOutcome = static_cast<TextQuestInterface::TQuestOutcome>(EC_Buf::TBufEC_GetInt32(Reader));
        Hidden = false;
        ShowWhenZero = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalAtMinimum = EC_Buf::TBufEC_GetBoolean(Reader);
        Enabled = EC_Buf::TBufEC_GetBoolean(Reader);
        ViewStringCount = EC_Buf::TBufEC_GetInt32(Reader);
        IsMoney = EC_Buf::TBufEC_GetBoolean(Reader);
        NameText->LoadTextLinesFromReader(Reader);
        EnsureViewStringCapacity(ViewStringCount, 0);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ViewStringCount); cpp_range.next(i); ) {
            ViewStrings[i]->LoadFromReader(Reader);
        }
        if (ViewStringCount <= 0) {
            ViewStringCount = 1;
            EnsureViewStringCapacity(1, 0);
            ViewStrings[1]->MinValue = MinValue;
            ViewStrings[1]->MaxValue = MaxValue;
        }
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        CriticalEvent->Picture->LoadTextLinesFromReader(Reader);
        CriticalEvent->Sound->LoadTextLinesFromReader(Reader);
        CriticalEvent->Music->LoadTextLinesFromReader(Reader);
        InitialRange->LoadFromReader(Reader);
    }

    void TParameter::LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        Value = EC_Buf::TBufEC_GetInt32(Reader);
        CriticalOutcome = static_cast<TextQuestInterface::TQuestOutcome>(EC_Buf::TBufEC_GetInt32(Reader));
        Hidden = EC_Buf::TBufEC_GetBoolean(Reader);
        ShowWhenZero = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalAtMinimum = EC_Buf::TBufEC_GetBoolean(Reader);
        Enabled = EC_Buf::TBufEC_GetBoolean(Reader);
        ViewStringCount = EC_Buf::TBufEC_GetInt32(Reader);
        IsMoney = EC_Buf::TBufEC_GetBoolean(Reader);
        NameText->LoadTextLinesFromReader(Reader);
        EnsureViewStringCapacity(ViewStringCount, 0);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ViewStringCount); cpp_range.next(i); ) {
            ViewStrings[i]->LoadFromReader(Reader);
        }
        if (ViewStringCount <= 0) {
            ViewStringCount = 1;
            EnsureViewStringCapacity(1, 0);
            ViewStrings[1]->MinValue = MinValue;
            ViewStrings[1]->MaxValue = MaxValue;
        }
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        InitialRange->LoadFromReader(Reader);
    }

    void TParameter::LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        ValueListClass::TValuesList* Values{};
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        Value = EC_Buf::TBufEC_GetInt32(Reader);
        CriticalOutcome = static_cast<TextQuestInterface::TQuestOutcome>(EC_Buf::TBufEC_GetInt32(Reader));
        Hidden = EC_Buf::TBufEC_GetBoolean(Reader);
        ShowWhenZero = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalAtMinimum = EC_Buf::TBufEC_GetBoolean(Reader);
        Enabled = EC_Buf::TBufEC_GetBoolean(Reader);
        ViewStringCount = EC_Buf::TBufEC_GetInt32(Reader);
        IsMoney = EC_Buf::TBufEC_GetBoolean(Reader);
        NameText->LoadTextLinesFromReader(Reader);
        EnsureViewStringCapacity(ViewStringCount, 0);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ViewStringCount); cpp_range.next(i); ) {
            ViewStrings[i]->LoadFromReader(Reader);
        }
        if (ViewStringCount <= 0) {
            ViewStringCount = 1;
            EnsureViewStringCapacity(1, 0);
            ViewStrings[1]->MinValue = MinValue;
            ViewStrings[1]->MaxValue = MaxValue;
        }
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        Values = pas::construct_call<ValueListClass::TValuesList>(ValueListClass::TValuesList_Create);
        Values->LoadFromReader(Reader);
        InitialRange->LoadFromValues(Values);
        Values->Clear();
        pas::free(Values);
    }

    void TParameter::LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        Value = EC_Buf::TBufEC_GetInt32(Reader);
        CriticalOutcome = static_cast<TextQuestInterface::TQuestOutcome>(EC_Buf::TBufEC_GetInt32(Reader));
        Hidden = EC_Buf::TBufEC_GetBoolean(Reader);
        ShowWhenZero = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalAtMinimum = EC_Buf::TBufEC_GetBoolean(Reader);
        Enabled = EC_Buf::TBufEC_GetBoolean(Reader);
        ViewStringCount = EC_Buf::TBufEC_GetInt32(Reader);
        IsMoney = EC_Buf::TBufEC_GetBoolean(Reader);
        NameText->LoadTextLinesFromReader(Reader);
        EnsureViewStringCapacity(ViewStringCount, 0);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ViewStringCount); cpp_range.next(i); ) {
            ViewStrings[i]->LoadFromReader(Reader);
        }
        if (ViewStringCount <= 0) {
            ViewStringCount = 1;
            EnsureViewStringCapacity(1, 0);
            ViewStrings[1]->MinValue = MinValue;
            ViewStrings[1]->MaxValue = MaxValue;
        }
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        InitialRange->Clear();
        InitialRange->AddRange(Value, Value);
    }

    void TParameter::LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        Value = EC_Buf::TBufEC_GetInt32(Reader);
        CriticalOutcome = static_cast<TextQuestInterface::TQuestOutcome>(EC_Buf::TBufEC_GetInt32(Reader));
        Hidden = EC_Buf::TBufEC_GetBoolean(Reader);
        ShowWhenZero = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalAtMinimum = EC_Buf::TBufEC_GetBoolean(Reader);
        Enabled = EC_Buf::TBufEC_GetBoolean(Reader);
        ViewStringCount = EC_Buf::TBufEC_GetInt32(Reader);
        IsMoney = false;
        NameText->LoadTextLinesFromReader(Reader);
        EnsureViewStringCapacity(ViewStringCount, 0);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ViewStringCount); cpp_range.next(i); ) {
            ViewStrings[i]->LoadFromReader(Reader);
        }
        if (ViewStringCount <= 0) {
            ViewStringCount = 1;
            EnsureViewStringCapacity(1, 0);
            ViewStrings[1]->MinValue = MinValue;
            ViewStrings[1]->MaxValue = MaxValue;
        }
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        InitialRange->Clear();
        InitialRange->AddRange(Value, Value);
    }

    void TParameter::LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader) {
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        Value = EC_Buf::TBufEC_GetInt32(Reader);
        CriticalOutcome = static_cast<TextQuestInterface::TQuestOutcome>(EC_Buf::TBufEC_GetInt32(Reader));
        Hidden = EC_Buf::TBufEC_GetBoolean(Reader);
        ShowWhenZero = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalAtMinimum = EC_Buf::TBufEC_GetBoolean(Reader);
        Enabled = EC_Buf::TBufEC_GetBoolean(Reader);
        ViewStringCount = 1;
        IsMoney = false;
        NameText->LoadTextLinesFromReader(Reader);
        ValueText->LoadTextLinesFromReader(Reader);
        EnsureViewStringCapacity(ViewStringCount, 0);
        ViewStrings[1]->MaxValue = MaxValue;
        ViewStrings[1]->MinValue = MinValue;
        ViewStrings[1]->Text->Text = EC_Str::TrimWideString(ValueText->Text);
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        InitialRange->Clear();
        InitialRange->AddRange(Value, Value);
    }

    void TParameter::p_destroy() {
        ParameterClass::TParameter_Destroy(this);
    }

} // namespace ParameterClass
