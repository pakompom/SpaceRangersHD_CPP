#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Ether {
    struct TEtherUnit;

    struct TEther;

    using TEtherIndex = pas::Array<TEtherUnit*, 0, 536870910>;

    using PEtherIndex = TEtherIndex*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEther : EC_Struct::TObjectEx {
        PAS_CLASS_META(TEther, EC_Struct::TObjectEx, "TEther", 24)
        void p_destroy() override;
        void Clear();
        TEtherUnit* AppendEntry();
        // Unlinks and frees the entry without updating SortedItems or Count.
        void RemoveEntry(TEtherUnit* Item);
        void SetIndexedEntry(std::int32_t Index, TEtherUnit* Item);
        std::int32_t FindInsertionIndex(const pas::WideString& Name);
        void Add(const pas::WideString& Name, std::int32_t Value);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void Enter();
        void Leave();
        TEtherUnit* First;
        TEtherUnit* Last;
        std::int32_t Count;
        PEtherIndex SortedItems;
        pas::CriticalSection* Lock;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEtherUnit : pas::Object {
        PAS_CLASS_META(TEtherUnit, pas::Object, "TEtherUnit", 20)
        TEtherUnit* Prev;
        TEtherUnit* Next;
        std::int32_t Value;
        pas::WideString Name;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_Ether
