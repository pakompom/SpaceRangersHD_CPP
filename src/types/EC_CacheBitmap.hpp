#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace EC_CacheBitmap {
    struct TCBitmapControlEC;

    struct TCBitmapEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCBitmapControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCBitmapControlEC, EC_Cache::TCacheControlEC, "TCBitmapControlEC", 24)
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
    struct TCBitmapEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCBitmapEC, EC_Cache::TCacheDataEC, "TCBitmapEC", 36)
        void p_destroy() override;
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        GR_GraphBuf::TGraphBufGR* Bitmap;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheBitmap
