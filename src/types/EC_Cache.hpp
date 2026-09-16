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
        virtual void Reset();
        virtual void SetCacheKey(const pas::WideString& NewKey);
        std::uint8_t HasEmptyCacheKey();
        virtual void QueueLoadIfMissing(pas::List* PendingLoads);
        virtual TCacheDataEC* CreateData();
        TCacheDataEC* AcquireDataFromConfig(TCacheDataClass CacheDataClass);
        TCacheDataEC* AcquireDataFromDirectKey(TCacheDataClass CacheDataClass);
        virtual TCacheDataEC* AcquireData();
        virtual void Release();
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
        void AppendControl(TCacheControlEC* Control);
        void UnlinkControl(TCacheControlEC* Control);
        virtual void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption);
        virtual void LoadFromKey(const pas::WideString& Key);
        TCacheDataEC* PrevData;
        TCacheDataEC* NextData;
        TCacheControlEC* FirstBoundControl;
        TCacheControlEC* LastBoundControl;
        pas::WideString CacheKey;
        std::int32_t ResidentBytes;
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
        void Clear();
        void SetDataRoot(EC_Data::TDataEC* Root);
        static void ResetControl(TCacheControlEC* Control);
        void AddDataToLruHead(TCacheDataEC* Data);
        void RemoveAndFreeData(TCacheDataEC* Data);
        TCacheDataEC* FindDataByKeyAndClass(const pas::WideString& Key, TCacheDataClass CacheDataClass);
        void TouchData(TCacheDataEC* Data);
        EC_Buf::TBufEC* OpenDataBuffer(const pas::WideString& Path);
        void TrimToBudget(std::int32_t BudgetBytes);
        void QueueNamedLoadIfMissing(pas::List* PendingLoads, const pas::WideString& CacheKind, const pas::WideString& Key);
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
