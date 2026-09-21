#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Data {
    struct TDataEC;

} // namespace EC_Data

namespace EC_Cache {
    struct TCacheDataEC;

    struct TCacheControlEC;

    struct TCacheEC;

    using TCacheDataClass = pas::ClassRef<TCacheDataEC>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCacheControlEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCacheControlEC, EC_Struct::TObjectEx, "TCacheControlEC", 24)
        void p_destroy() override;
        // Drops all retains and the data binding.
        virtual void Reset();
        // Drops existing retains and the data binding; may apply configured key substitutions.
        virtual void SetCacheKey(const pas::WideString& NewKey);
        std::uint8_t HasEmptyCacheKey();
        virtual void QueueLoadIfMissing(pas::List* PendingLoads);
        // Base implementation returns nil.
        virtual TCacheDataEC* CreateData();
        // Acquisitions may block on pending loads and evict other cache entries.
        // Nested acquisitions reuse BoundData; the class argument selects existing entries.
        TCacheDataEC* AcquireDataFromConfig(TCacheDataClass CacheDataClass);
        TCacheDataEC* AcquireDataFromDirectKey(TCacheDataClass CacheDataClass);
        // Base implementation returns nil.
        virtual TCacheDataEC* AcquireData();
        // Saturates at zero; the data remains bound.
        virtual void Release();
        // Only checks this control's RetainCount; frees the shared entry and detaches all its controls.
        void EvictData(TCacheDataClass CacheDataClass);
        TCacheControlEC* PrevBoundControl;
        TCacheControlEC* NextBoundControl;
        TCacheDataEC* BoundData;
        pas::WideString CacheKey;
        std::int32_t RetainCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCacheDataEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCacheDataEC, EC_Struct::TObjectEx, "TCacheDataEC", 32)
        void p_destroy() override;
        // Caller must set Control.BoundData.
        void AppendControl(TCacheControlEC* Control);
        // Clears BoundData but preserves RetainCount.
        void UnlinkControl(TCacheControlEC* Control);
        // Base load hooks are empty in the native implementation.
        virtual void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption);
        virtual void LoadFromKey(const pas::WideString& Key);
        TCacheDataEC* PrevData;
        TCacheDataEC* NextData;
        TCacheControlEC* FirstBoundControl;
        TCacheControlEC* LastBoundControl;
        pas::WideString CacheKey;
        std::int32_t ResidentBytes;
        // Win32 event handle; zero after loading has completed.
        std::uint32_t LoadCompleteEvent;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCacheEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCacheEC, EC_Struct::TObjectEx, "TCacheEC", 28)
        void p_destroy() override;
        // Invalidates all entries, including retained ones.
        void Clear();
        // Root is borrowed; invalidates existing cached entries.
        void SetDataRoot(EC_Data::TDataEC* Root);
        static void ResetControl(TCacheControlEC* Control);
        // List and lookup helpers below do not acquire CacheLock.
        void AddDataToLruHead(TCacheDataEC* Data);
        // Does not adjust ResidentBytes. Accepts nil.
        void RemoveAndFreeData(TCacheDataEC* Data);
        // Case-sensitive key and exact class match; returns nil when absent.
        TCacheDataEC* FindDataByKeyAndClass(const std::u16string_view& Key, TCacheDataClass CacheDataClass);
        void TouchData(TCacheDataEC* Data);
        // Caller owns the returned buffer.
        EC_Buf::TBufEC* OpenDataBuffer(const pas::WideString& Path);
        // Retained entries can prevent reaching the budget.
        void TrimToBudget(std::int32_t BudgetBytes);
        // PendingLoads owns added controls; duplicate pending entries are possible.
        void QueueNamedLoadIfMissing(pas::List* PendingLoads, const std::u16string_view& CacheKind, const pas::WideString& Key);
        pas::CriticalSection* CacheLock;
        TCacheDataEC* MostRecentData;
        TCacheDataEC* LeastRecentData;
        EC_Data::TDataEC* DataRoot;
        std::int32_t ResidentBytes;
        std::int32_t ResidentByteLimit;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_Cache
