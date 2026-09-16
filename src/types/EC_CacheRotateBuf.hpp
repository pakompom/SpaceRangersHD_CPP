#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"

namespace EC_CacheRotateBuf {
    struct TCRotateBufControlEC;

    struct TCRotateBufEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCRotateBufControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCRotateBufControlEC, EC_Cache::TCacheControlEC, "TCRotateBufControlEC", 24)
        void QueueLoadIfMissing(pas::List* PendingLoads) override;
        EC_Cache::TCacheDataEC* CreateData() override;
        EC_Cache::TCacheDataEC* AcquireData() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCRotateBufEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCRotateBufEC, EC_Cache::TCacheDataEC, "TCRotateBufEC", 36)
        void p_destroy() override;
        void LoadFromKey(const pas::WideString& Key) override;
        void* Buffer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheRotateBuf
