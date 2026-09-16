#include "layout/PathClass.hpp"
#include "types/ParameterClass.hpp"
#include "types/SequenceClass.hpp"
#include "types/TextFieldClass.hpp"
#include "units/CalcParseClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EventClass.hpp"
#include "units/ParameterDeltaClass.hpp"
#include "units/PathClass.hpp"

namespace PathClass {
    void TPath_Create(TPath* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Caption = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Event = pas::construct_call<EventClass::TEvent>(EventClass::TEvent_Create);
        Self->ConditionExpression = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->ParameterChanges = pas::make_object<pas::List>();
        Self->Sequence = nullptr;
        Self->Reset();
        Self->Id = 0;
        Self->ToLocationId = 0;
        Self->FromLocationId = 0;
    }

    // Frees the sequence and containers without calling Reset.
    void TPath_Destroy(TPath* Self) {
        if (Self->Sequence != nullptr) {
            pas::free(Self->Sequence);
        }
        pas::free(Self->Caption);
        Self->Caption = nullptr;
        pas::free(Self->Event);
        Self->Event = nullptr;
        pas::free(Self->ConditionExpression);
        Self->ConditionExpression = nullptr;
        // Native destruction frees the list without resetting its owned entries.
        pas::free(Self->ParameterChanges);
        Self->ParameterChanges = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TPath::Reset() {
        std::int32_t i{};
        Days = 0;
        DisplayOrder = 5;
        Priority = 1.0;
        TraversalLimit = 0;
        TraversalCount = 0;
        Id = 0;
        FromLocationId = 0;
        ToLocationId = 0;
        Caption->Text = pas::WideString();
        Event->Text->Text = pas::WideString();
        Event->Picture->Text = pas::WideString();
        Event->Sound->Text = pas::WideString();
        Event->Music->Text = pas::WideString();
        ConditionExpression->Text = pas::WideString();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            pas::free(GetParameterChange(i));
        }
        pas::list_clear(ParameterChanges);
        IsAutomatic = true;
        UnknownFlag = 0;
        AlwaysShow = false;
    }

    std::int32_t TPath::GetParameterChangeCount() {
        return pas::list_count(ParameterChanges);
    }

    // Index is one-based.
    ParameterDeltaClass::TParameterDelta* TPath::GetParameterChange(std::int32_t Index) {
        return pas::list_at<ParameterDeltaClass::TParameterDelta>(ParameterChanges, Index - 1);
    }

    void TPath::AddParameterChange(ParameterDeltaClass::TParameterDelta* Change) {
        pas::list_add(ParameterChanges, reinterpret_cast<void*>(Change));
    }

    // Evaluates every expression before applying any change.
    void TPath::ApplyParameterChanges(pas::List*& Parameters) {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            GetParameterChange(i)->EvaluateChangeExpression(Parameters);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range_2.next(i); ) {
            GetParameterChange(i)->ApplyChange(Parameters);
        }
    }

    // Removed entries are not freed.
    void TPath::PruneParameterChanges(pas::List* Parameters) {
        std::int32_t i{};
        {
            const std::int32_t cpp_first = GetParameterChangeCount();
            if (cpp_first >= 1) {
                for (i = cpp_first; i >= 1; --i) {
                    if (GetParameterChange(i)->ParameterIndex < 1 || GetParameterChange(i)->ParameterIndex > pas::list_count(Parameters)) {
                        pas::list_delete(ParameterChanges, i - 1);
                    } else if (GetParameterChange(i)->HasNoChange(Parameters)) {
                        if (GetParameterChange(i)->HasNoValueConstraint(Parameters)) {
                            pas::list_delete(ParameterChanges, i - 1);
                        }
                    }
                }
            }
        }
    }

    // Updates Available. Invalid condition expressions are ignored; parameter constraints still apply.
    std::uint8_t TPath::CheckAvailable(pas::List* Parameters) {
        CalcParseClass::TCalcParse* Calc{};
        std::int32_t i{};
        Available = false;
        std::uint8_t Result = false;
        if (EC_Str::TrimWideString(ConditionExpression->Text) != u"") {
            Calc = pas::construct_call<CalcParseClass::TCalcParse>(CalcParseClass::TCalcParse_Create);
            Calc->Reset();
            Calc->Prepare(EC_Str::TrimWideString(ConditionExpression->Text), 0);
            // Native cleanup occurs only after a successfully prepared expression.
            if (static_cast<std::uint8_t>(Calc->HasError ^ 1) && static_cast<std::uint8_t>(Calc->UsesDefaultParameter ^ 1)) {
                Calc->Evaluate(Parameters);
                if (static_cast<std::uint8_t>(Calc->HasError ^ 1) && Calc->ResultValue == 0) {
                    pas::destroy(Calc);
                    return Result;
                }
                pas::destroy(Calc);
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            if (!GetParameterChange(i)->AcceptsParameter(Parameters)) {
                return Result;
            }
        }
        Available = true;
        return true;
    }

    ParameterDeltaClass::TParameterDelta* TPath::FindParameterChange(std::int32_t ParameterIndex) {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            if (GetParameterChange(i)->ParameterIndex == ParameterIndex) {
                return GetParameterChange(i);
            }
        }
        return nullptr;
    }

    // Path format used by quest versions 1111111125 and later.
    void TPath::LoadFromReader(EC_Buf::TBufEC* Reader, pas::List* Parameters) {
        std::int32_t i{};
        std::int32_t ParameterIndex{};
        ParameterDeltaClass::TParameterDelta* Change{};
        ParameterClass::TParameter* Parameter{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        DisplayOrder = EC_Buf::TBufEC_GetInt32(Reader);
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Reader);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            ParameterIndex = EC_Buf::TBufEC_GetInt32(Reader);
            Change = FindParameterChange(ParameterIndex);
            if (Change == nullptr) {
                Change = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                Change->ParameterIndex = ParameterIndex;
                Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, ParameterIndex - 1);
                Change->MinValue = Parameter->MinValue;
                Change->MaxValue = Parameter->MaxValue;
                AddParameterChange(Change);
            }
            Change->LoadValueConstraintsFromReader(Reader);
        }
        Count = EC_Buf::TBufEC_GetInt32(Reader);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Count); cpp_range_2.next(i); ) {
            ParameterIndex = EC_Buf::TBufEC_GetInt32(Reader);
            Change = FindParameterChange(ParameterIndex);
            if (Change == nullptr) {
                Change = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                Change->ParameterIndex = ParameterIndex;
                Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, ParameterIndex - 1);
                Change->MinValue = Parameter->MinValue;
                Change->MaxValue = Parameter->MaxValue;
                AddParameterChange(Change);
            }
            Change->LoadChangeFromReader(Reader);
        }
        ConditionExpression->LoadTextLinesFromReader(Reader);
        Caption->LoadTextLinesFromReader(Reader);
        Event->Text->LoadTextLinesFromReader(Reader);
        Event->Picture->LoadTextLinesFromReader(Reader);
        Event->Sound->LoadTextLinesFromReader(Reader);
        Event->Music->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest version 1111111124.
    void TPath::LoadLegacyV9FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        DisplayOrder = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 96; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        ConditionExpression->LoadTextLinesFromReader(Reader);
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest version 1111111123.
    void TPath::LoadLegacyV8FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        DisplayOrder = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 48; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        ConditionExpression->LoadTextLinesFromReader(Reader);
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest version 1111111122.
    void TPath::LoadLegacyV7FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        DisplayOrder = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 24; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        ConditionExpression->LoadTextLinesFromReader(Reader);
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest versions 1111111119..1111111121.
    void TPath::LoadLegacyV6FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 24; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        ConditionExpression->LoadTextLinesFromReader(Reader);
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest versions 1111111117..1111111118.
    void TPath::LoadLegacyV5FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 12; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV2FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest version 1111111116.
    void TPath::LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Priority = Reader->GetDouble();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 12; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV1FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest version 1111111115.
    void TPath::LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 12; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV0FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest version 1111111114.
    void TPath::LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        TraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        for (i = 1; i <= 9; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV0FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Quest versions 1111111112..1111111113.
    void TPath::LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        AlwaysShow = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 9; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV0FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    // Legacy readers derive IsAutomatic from the trimmed caption.
    // Quest version 1111111111.
    void TPath::LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        FromLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        ToLocationId = EC_Buf::TBufEC_GetInt32(Reader);
        IsAutomatic = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 9; ++i) {
            {
                ParameterDeltaClass::TParameterDelta* cpp_arg = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                TPath* self = this;
                self->AddParameterChange(cpp_arg);
            }
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV0FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        Caption->LoadTextLinesFromReader(Reader);
        Event->ClearTextFields();
        Event->Text->LoadTextLinesFromReader(Reader);
        IsAutomatic = EC_Str::TrimWideString(Caption->Text) == u"";
    }

    void TPath::p_destroy() {
        PathClass::TPath_Destroy(this);
    }

} // namespace PathClass
