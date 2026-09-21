#include "layout/LocationClass.hpp"
#include "types/SequenceClass.hpp"
#include "types/TextFieldClass.hpp"
#include "units/CalcParseClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EventClass.hpp"
#include "units/LocationClass.hpp"
#include "units/ParameterDeltaClass.hpp"
#include "units/System.hpp"

namespace LocationClass {
    void TLocation_Create(TLocation* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->EventCount = 1;
        Self->Events.set_length(2);
        Self->Events[1] = pas::construct_call<EventClass::TEvent>(EventClass::TEvent_Create);
        Self->Sequence = nullptr;
        Self->EventExpression = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->ParameterChanges = pas::make_object<pas::List>();
        Self->Reset();
    }

    void TLocation_Destroy(TLocation* Self) {
        std::int32_t i{};
        Self->Reset();
        // Reset retains only the first event; the native loop indexes that slot.
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->EventCount); cpp_range.next(i); ) {
            pas::free(Self->Events[1]);
            Self->Events[1] = nullptr;
        }
        Self->Events = nullptr;
        pas::free(Self->EventExpression);
        Self->EventExpression = nullptr;
        pas::free(Self->ParameterChanges);
        Self->ParameterChanges = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Retains the first event; frees parameter changes and Sequence.
    void TLocation::Reset() {
        std::int32_t i{};
        EditorX = 100;
        EditorY = 100;
        Days = 0;
        VisitLimit = 0;
        VisitCount = 0;
        if (Sequence != nullptr) {
            pas::free(Sequence);
        }
        Sequence = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            pas::free(GetParameterChange(i));
        }
        pas::list_clear(ParameterChanges);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(2, EventCount); cpp_range_2.next(i); ) {
            pas::free(Events[i]);
        }
        Events.set_length(2);
        EventCount = 1;
        Events[1]->ClearTextFields();
        UseEventExpression = false;
        NextEventIndex = 1;
        EventExpression->ClearText();
        Id = 0;
        IsStart = false;
        IsSuccess = false;
        IsFailure = false;
        IsDeath = false;
        IsEmpty = false;
    }

    std::int32_t TLocation::GetParameterChangeCount() {
        return pas::list_count(ParameterChanges);
    }

    // Index is one-based.
    ParameterDeltaClass::TParameterDelta* TLocation::GetParameterChange(std::int32_t Index) {
        return pas::list_at<ParameterDeltaClass::TParameterDelta>(ParameterChanges, Index - 1);
    }

    void TLocation::AddParameterChange(ParameterDeltaClass::TParameterDelta* Change) {
        pas::list_add(ParameterChanges, reinterpret_cast<void*>(Change));
    }

    // Evaluates every expression before applying any change.
    void TLocation::ApplyParameterChanges(pas::List*& Parameters) {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            GetParameterChange(i)->EvaluateChangeExpression(Parameters);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range_2.next(i); ) {
            GetParameterChange(i)->ApplyChange(Parameters);
        }
    }

    // Removed entries are not freed.
    void TLocation::PruneParameterChanges(pas::List* Parameters) {
        std::int32_t i{};
        {
            const std::int32_t cpp_first = GetParameterChangeCount();
            if (cpp_first >= 1) {
                for (i = cpp_first; i >= 1; --i) {
                    if (GetParameterChange(i)->ParameterIndex < 1 || GetParameterChange(i)->ParameterIndex > pas::list_count(Parameters)) {
                        pas::list_delete(ParameterChanges, i - 1);
                    } else if (GetParameterChange(i)->HasNoChange(Parameters)) {
                        pas::list_delete(ParameterChanges, i - 1);
                    }
                }
            }
        }
    }

    ParameterDeltaClass::TParameterDelta* TLocation::FindParameterChange(std::int32_t ParameterIndex) {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterChangeCount()); cpp_range.next(i); ) {
            if (GetParameterChange(i)->ParameterIndex == ParameterIndex) {
                return GetParameterChange(i);
            }
        }
        return nullptr;
    }

    void TLocation::AddEvent() {
        pas::WideString Text{};
        std::uint8_t Different{};
        std::int32_t i{};
        ++EventCount;
        Events.set_length(EventCount + 1);
        Events[EventCount] = pas::construct_call<EventClass::TEvent>(EventClass::TEvent_Create);
        if (EventCount != 1) {
            Text = EC_Str::TrimWideString(Events[1]->Picture->Text);
            Different = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(2, EventCount - 1); cpp_range.next(i); ) {
                if (EC_Str::TrimWideString(Events[i]->Picture->Text) != Text) {
                    Different = true;
                    break;
                }
            }
            if (!Different) {
                Events[EventCount]->Picture->Text = Text;
            }
            Text = EC_Str::TrimWideString(Events[1]->Sound->Text);
            Different = false;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(2, EventCount - 1); cpp_range_2.next(i); ) {
                if (EC_Str::TrimWideString(Events[i]->Sound->Text) != Text) {
                    Different = true;
                    break;
                }
            }
            if (!Different) {
                Events[EventCount]->Sound->Text = Text;
            }
            Text = EC_Str::TrimWideString(Events[1]->Music->Text);
            Different = false;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(2, EventCount - 1); cpp_range_3.next(i); ) {
                if (EC_Str::TrimWideString(Events[i]->Music->Text) != Text) {
                    Different = true;
                    break;
                }
            }
            if (!Different) {
                Events[EventCount]->Music->Text = std::move(Text);
            }
        }
    }

    // Retains at least one event.
    void TLocation::RemoveLastEvent() {
        if (EventCount >= 2) {
            pas::free(Events[EventCount]);
            --EventCount;
            Events.set_length(EventCount + 1);
        }
    }

    // Location format used by quest versions 1111111126 and later.
    void TLocation::LoadFromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        std::int32_t ParameterIndex{};
        ParameterDeltaClass::TParameterDelta* Change{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = EC_Buf::TBufEC_GetInt32(Reader);
        std::uint8_t LocationType = EC_Buf::TBufEC_GetByte(Reader);
        IsStart = LocationType == 1;
        IsEmpty = LocationType == 2;
        IsSuccess = LocationType == 3;
        IsFailure = LocationType == 4 || LocationType == 5;
        IsDeath = LocationType == 5;
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Reader);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            ParameterIndex = EC_Buf::TBufEC_GetInt32(Reader);
            Change = FindParameterChange(ParameterIndex);
            if (Change == nullptr) {
                Change = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                Change->ParameterIndex = ParameterIndex;
                AddParameterChange(Change);
            }
            Change->LoadChangeFromReader(Reader);
        }
        Count = EC_Buf::TBufEC_GetInt32(Reader);
        while (Count > EventCount) {
            AddEvent();
        }
        while (Count < EventCount && EventCount > 1) {
            RemoveLastEvent();
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Count); cpp_range_2.next(i); ) {
            Events[i]->Text->LoadTextLinesFromReader(Reader);
            Events[i]->Picture->LoadTextLinesFromReader(Reader);
            Events[i]->Sound->LoadTextLinesFromReader(Reader);
            Events[i]->Music->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        EventExpression->LoadTextLinesFromReader(Reader);
    }

    // Quest version 1111111125; repeated parameter indices overwrite earlier changes.
    void TLocation::LoadLegacyV8FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        std::int32_t ParameterIndex{};
        ParameterDeltaClass::TParameterDelta* Change{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        std::uint8_t LocationType = EC_Buf::TBufEC_GetByte(Reader);
        IsStart = LocationType == 1;
        IsEmpty = LocationType == 2;
        IsSuccess = LocationType == 3;
        IsFailure = LocationType == 4 || LocationType == 5;
        IsDeath = LocationType == 5;
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Reader);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(i); ) {
            ParameterIndex = EC_Buf::TBufEC_GetInt32(Reader);
            Change = FindParameterChange(ParameterIndex);
            if (Change == nullptr) {
                Change = pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create);
                Change->ParameterIndex = ParameterIndex;
                AddParameterChange(Change);
            }
            Change->LoadChangeFromReader(Reader);
        }
        Count = EC_Buf::TBufEC_GetInt32(Reader);
        while (Count > EventCount) {
            AddEvent();
        }
        while (Count < EventCount && EventCount > 1) {
            RemoveLastEvent();
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Count); cpp_range_2.next(i); ) {
            Events[i]->Text->LoadTextLinesFromReader(Reader);
            Events[i]->Picture->LoadTextLinesFromReader(Reader);
            Events[i]->Sound->LoadTextLinesFromReader(Reader);
            Events[i]->Music->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        EventExpression->LoadTextLinesFromReader(Reader);
    }

    // Quest version 1111111124.
    void TLocation::LoadLegacyV7FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        IsEmpty = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 96; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount < 10) {
            AddEvent();
        }
        while (EventCount > 10) {
            RemoveLastEvent();
        }
        for (i = 1; i <= 10; ++i) {
            Events[i]->ClearTextFields();
            Events[i]->Text->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        NextEventIndex = EC_Buf::TBufEC_GetInt32(Reader);
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        DiscardedText->ClearText();
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
        EventExpression->LoadTextLinesFromReader(Reader);
    }

    // Quest version 1111111123.
    void TLocation::LoadLegacyV6FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        IsEmpty = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 48; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount < 10) {
            AddEvent();
        }
        while (EventCount > 10) {
            RemoveLastEvent();
        }
        for (i = 1; i <= 10; ++i) {
            Events[i]->ClearTextFields();
            Events[i]->Text->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        NextEventIndex = EC_Buf::TBufEC_GetInt32(Reader);
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        DiscardedText->ClearText();
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
        EventExpression->LoadTextLinesFromReader(Reader);
    }

    // Quest versions 1111111121..1111111122.
    void TLocation::LoadLegacyV5FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        IsEmpty = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 24; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount < 10) {
            AddEvent();
        }
        while (EventCount > 10) {
            RemoveLastEvent();
        }
        for (i = 1; i <= 10; ++i) {
            Events[i]->ClearTextFields();
            Events[i]->Text->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        NextEventIndex = EC_Buf::TBufEC_GetInt32(Reader);
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        DiscardedText->ClearText();
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
        EventExpression->LoadTextLinesFromReader(Reader);
    }

    // Quest versions 1111111119..1111111120.
    void TLocation::LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        IsEmpty = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 24; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV3FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount < 10) {
            AddEvent();
        }
        while (EventCount > 10) {
            RemoveLastEvent();
        }
        for (i = 1; i <= 10; ++i) {
            Events[i]->ClearTextFields();
            Events[i]->Text->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        NextEventIndex = EC_Buf::TBufEC_GetInt32(Reader);
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        DiscardedText->ClearText();
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
    }

    // Quest versions 1111111117..1111111118.
    void TLocation::LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 12; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV2FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount < 10) {
            AddEvent();
        }
        while (EventCount > 10) {
            RemoveLastEvent();
        }
        for (i = 1; i <= 10; ++i) {
            Events[i]->ClearTextFields();
            Events[i]->Text->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        NextEventIndex = EC_Buf::TBufEC_GetInt32(Reader);
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        DiscardedText->ClearText();
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
    }

    // Quest version 1111111116.
    void TLocation::LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        Days = EC_Buf::TBufEC_GetInt32(Reader);
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 12; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV1FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount < 10) {
            AddEvent();
        }
        while (EventCount > 10) {
            RemoveLastEvent();
        }
        for (i = 1; i <= 10; ++i) {
            Events[i]->ClearTextFields();
            Events[i]->Text->LoadTextLinesFromReader(Reader);
        }
        UseEventExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        NextEventIndex = EC_Buf::TBufEC_GetInt32(Reader);
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        DiscardedText->ClearText();
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
    }

    // Quest version 1111111115.
    void TLocation::LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 12; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV0FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount > 1) {
            RemoveLastEvent();
        }
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
        Events[1]->ClearTextFields();
        Events[1]->Text->LoadTextLinesFromReader(Reader);
    }

    // Quest versions 1111111111..1111111114.
    void TLocation::LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader) {
        std::int32_t i{};
        Reset();
        EditorX = EC_Buf::TBufEC_GetInt32(Reader);
        EditorY = EC_Buf::TBufEC_GetInt32(Reader);
        Id = EC_Buf::TBufEC_GetInt32(Reader);
        VisitLimit = 0;
        IsStart = EC_Buf::TBufEC_GetBoolean(Reader);
        IsSuccess = EC_Buf::TBufEC_GetBoolean(Reader);
        IsFailure = EC_Buf::TBufEC_GetBoolean(Reader);
        IsDeath = EC_Buf::TBufEC_GetBoolean(Reader);
        for (i = 1; i <= 9; ++i) {
            AddParameterChange(pas::construct_call<ParameterDeltaClass::TParameterDelta>(ParameterDeltaClass::TParameterDelta_Create));
            GetParameterChange(GetParameterChangeCount())->LoadLegacyV0FromReader(Reader);
            GetParameterChange(GetParameterChangeCount())->ParameterIndex = i;
        }
        while (EventCount > 1) {
            RemoveLastEvent();
        }
        TextFieldClass::TTextField* DiscardedText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        DiscardedText->LoadTextLinesFromReader(Reader);
        pas::free(DiscardedText);
        Events[1]->ClearTextFields();
        Events[1]->Text->LoadTextLinesFromReader(Reader);
    }

    // Expression selection falls back to random choice.
    EventClass::TEvent* TLocation::SelectEvent(pas::List*& Parameters) {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t Attempts{};
        pas::WideString Text{};
        CalcParseClass::TCalcParse* Calc{};
        std::uint8_t Valid{};
        EventClass::TEvent* Result = nullptr;
        std::uint8_t Found = false;
        if (UseEventExpression) {
            Valid = true;
            Calc = pas::construct_call<CalcParseClass::TCalcParse>(CalcParseClass::TCalcParse_Create);
            if (EC_Str::TrimWideString(EventExpression->Text) != u"") {
                Calc->Prepare(EventExpression->Text, 1);
                if (Calc->HasError || Calc->UsesDefaultParameter) {
                    Valid = false;
                }
            } else {
                Valid = false;
            }
            if (Valid) {
                Calc->Evaluate(Parameters);
                if (Calc->EvaluationError) {
                    Valid = false;
                }
            }
            if (Valid) {
                if (Calc->ResultValue <= EventCount && Calc->ResultValue >= 1) {
                    Result = Events[Calc->ResultValue];
                }
            } else {
                Attempts = 0;
                while (!Found) {
                    i = pas::random(EventCount, &System::RandSeed) + 1;
                    Text = EC_Str::TrimWideString(Events[i]->Text->Text);
                    if (Text != u"") {
                        Found = true;
                        Result = Events[i];
                    } else if (Attempts > std::max<std::int32_t>(20, EventCount * 2)) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(i + 1, i + EventCount); cpp_range.next(j); ) {
                            Text = EC_Str::TrimWideString(Events[1 + pas::imod(j, EventCount)]->Text->Text);
                            if (Text != u"") {
                                break;
                            }
                        }
                        Found = true;
                        Result = Events[1 + pas::imod(j, EventCount)];
                    } else {
                        ++Attempts;
                    }
                }
            }
            pas::destroy(Calc);
        } else {
            i = NextEventIndex;
            Attempts = 0;
            while (!Found) {
                Text = EC_Str::TrimWideString(Events[i]->Text->Text);
                if (Text != u"" || Attempts > EventCount) {
                    Found = true;
                    Result = Events[i];
                    NextEventIndex = i + 1;
                    if (NextEventIndex > EventCount) {
                        NextEventIndex = 1;
                    }
                } else {
                    ++Attempts;
                }
                ++i;
                if (i > EventCount) {
                    i = 1;
                }
            }
        }
        return Result;
    }

    void TLocation::p_destroy() {
        LocationClass::TLocation_Destroy(this);
    }

} // namespace LocationClass
