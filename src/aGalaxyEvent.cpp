#include "layout/aGalaxyEvent.hpp"
#include "types/System.hpp"
#include "types/aMyFunction.hpp"
#include "units/EC_Buf.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"

namespace aGalaxyEvent {
    // Nil Galaxy selects the current galaxy; returns nil if none exists. The galaxy owns the result, dated with its CurrentTurn. Trims the oldest events to retain at most 9999 entries.
    TGalaxyEvent* AddGalaxyEvent(pas::WideString EventType, aGalaxy::TGalaxy* Galaxy) {
        aGalaxy::TGalaxy* Target{};
        if (Galaxy != nullptr) {
            Target = Galaxy;
        } else {
            Target = aGalaxy::Galaxy;
        }
        if (Target == nullptr) {
            return nullptr;
        }
        TGalaxyEvent* Result = pas::construct_call<TGalaxyEvent>(TGalaxyEvent_Create, EventType);
        Result->Turn = Target->CurrentTurn;
        pas::list_add(Target->GalaxyEvents, reinterpret_cast<void*>(Result));
        while (pas::list_count(Target->GalaxyEvents) >= 10000) {
            pas::free(pas::list_at<pas::Object>(Target->GalaxyEvents, 0));
            pas::list_delete(Target->GalaxyEvents, 0);
        }
        return Result;
    }

    void TGalaxyEvent_Create(TGalaxyEvent* Self, pas::WideString EventType) {
        Self->EventType = std::move(EventType);
        Self->Turn = 0;
        Self->Data = nullptr;
        Self->TextData = nullptr;
    }

    void TGalaxyEvent_Destroy(TGalaxyEvent* Self) {
        Self->ClearData();
        Self->ClearTextData();
    }

    void TGalaxyEvent::AddData(std::int32_t Value) {
        if (Data == nullptr) {
            Data = pas::make_object<pas::List>();
        }
        pas::list_add(Data, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Value))));
    }

    // Copies Value into a separately allocated string cell.
    void TGalaxyEvent::AddTextData(pas::WideString Value) {
        System::PWideString Cell{};
        if (TextData == nullptr) {
            TextData = pas::make_object<pas::List>();
        }
        pas::new_value(Cell);
        *Cell = std::move(Value);
        pas::list_add(TextData, static_cast<void*>(Cell));
    }

    // Zero-based; returns zero for a missing list or an out-of-range index.
    std::int32_t TGalaxyEvent::GetData(std::int32_t Index) {
        std::int32_t Result = 0;
        if (Data == nullptr) {
            return Result;
        }
        if (Index < 0) {
            return Result;
        }
        if (pas::list_count(Data) <= Index) {
            return Result;
        }
        return static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Data, Index)));
    }

    // Zero-based; returns a copy, or empty for a missing list or an out-of-range index.
    pas::WideString TGalaxyEvent::GetTextData(std::int32_t Index) {
        pas::WideString Result{};
        if (TextData == nullptr) {
            return Result;
        }
        if (Index < 0) {
            return Result;
        }
        if (pas::list_count(TextData) <= Index) {
            return Result;
        }
        return *pas::list_at<pas::WideString>(TextData, Index);
    }

    void TGalaxyEvent::ClearData() {
        if (Data != nullptr) {
            pas::list_clear(Data);
            pas::free(Data);
            Data = nullptr;
        }
    }

    // Frees the string cells and list without finalizing the cells' WideStrings, leaking their BSTR storage.
    void TGalaxyEvent::ClearTextData() {
        std::int32_t i{};
        std::int32_t Count{};
        if (TextData != nullptr) {
            Count = pas::list_count(TextData);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
                pas::dispose(pas::list_get(TextData, i));
            }
            pas::list_clear(TextData);
            pas::free(TextData);
            TextData = nullptr;
        }
    }

    // Overwrites existing payload lists without freeing them; nonpositive stored counts produce nil lists.
    void TGalaxyEvent::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t i{};
        System::PWideString Cell{};
        EventType = Buffer->ReadWideString();
        Turn = EC_Buf::TBufEC_GetInt32(Buffer);
        Data = nullptr;
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Count > 0) {
            Data = pas::make_object<pas::List>();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
                void* int32 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetInt32(Buffer))));
                pas::List* data = Data;
                pas::list_add(data, int32);
            }
        }
        TextData = nullptr;
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Count > 0) {
            TextData = pas::make_object<pas::List>();
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
                pas::new_value(Cell);
                *Cell = Buffer->ReadWideString();
                pas::list_add(TextData, static_cast<void*>(Cell));
            }
        }
    }

    void TGalaxyEvent::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t i{};
        std::int32_t Count{};
        Buffer->AddWideStringZ(EventType);
        Buffer->AddIntegerValue(Turn);
        if (Data == nullptr) {
            Buffer->AddIntegerValue(0);
        } else {
            Count = pas::list_count(Data);
            Buffer->AddIntegerValue(Count);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
                Buffer->AddIntegerValue(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Data, i))));
            }
        }
        if (TextData == nullptr) {
            Buffer->AddIntegerValue(0);
        } else {
            Count = pas::list_count(TextData);
            Buffer->AddIntegerValue(Count);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
                Buffer->AddWideStringZ(*pas::list_at<pas::WideString>(TextData, i));
            }
        }
    }

    void TGalaxyEvent::p_destroy() {
        aGalaxyEvent::TGalaxyEvent_Destroy(this);
    }

} // namespace aGalaxyEvent
