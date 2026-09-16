#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CacheBuf {
    struct TCBufControlEC;

    struct TCBufEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCBufControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCBufControlEC, EC_Cache::TCacheControlEC, "TCBufControlEC", 24)
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
    struct TCBufEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCBufEC, EC_Cache::TCacheDataEC, "TCBufEC", 36)
        void p_destroy() override;
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        EC_Buf::TBufEC* Buffer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheBuf
