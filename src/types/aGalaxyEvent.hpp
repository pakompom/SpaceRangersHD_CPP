#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxyEvent {
    struct TGalaxyEvent;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGalaxyEvent : EC_Struct::TObjectEx {
        PAS_CLASS_META(TGalaxyEvent, EC_Struct::TObjectEx, "TGalaxyEvent", 20)
        void p_destroy() override;
        void AddData(std::int32_t Value);
        // Copies Value into a separately allocated string cell.
        void AddTextData(pas::WideString Value);
        // Zero-based; returns zero for a missing list or an out-of-range index.
        std::int32_t GetData(std::int32_t Index);
        // Zero-based; returns a copy, or empty for a missing list or an out-of-range index.
        pas::WideString GetTextData(std::int32_t Index);
        void ClearData();
        // Frees the string cells and list without finalizing the cells' WideStrings, leaking their BSTR storage.
        void ClearTextData();
        // Overwrites existing payload lists without freeing them; nonpositive stored counts produce nil lists.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        pas::WideString EventType;
        std::int32_t Turn;
        // Owned list of Integer payloads stored in pointer slots; may be nil.
        pas::List* Data;
        // Owned list of separately allocated PWideString cells; may be nil.
        pas::List* TextData;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aGalaxyEvent
