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
        void AddTextData(pas::WideString Value);
        std::int32_t GetData(std::int32_t Index);
        pas::WideString GetTextData(std::int32_t Index);
        void ClearData();
        void ClearTextData();
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        pas::WideString EventType;
        std::int32_t Turn;
        pas::List* Data;
        pas::List* TextData;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aGalaxyEvent
