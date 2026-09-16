#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"
#include "types/Types.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CacheTBitmap {
    struct TCTBitmapControlEC;

    struct TCTBitmapEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCTBitmapControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCTBitmapControlEC, EC_Cache::TCacheControlEC, "TCTBitmapControlEC", 24)
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
    struct TCTBitmapEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCTBitmapEC, EC_Cache::TCacheDataEC, "TCTBitmapEC", 44)
        void p_destroy() override;
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        void* TransBuffer;
        Types::TPoint PixelSize;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheTBitmap
