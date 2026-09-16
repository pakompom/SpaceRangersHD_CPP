#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Cache.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_DX {
    struct TTextureGR;

} // namespace GR_DX

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace EC_CacheAlphaBitmap {
    struct TCAlphaBitmapControlEC;

    struct TCAlphaBitmapEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCAlphaBitmapControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCAlphaBitmapControlEC, EC_Cache::TCacheControlEC, "TCAlphaBitmapControlEC", 24)
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
    struct TCAlphaBitmapEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCAlphaBitmapEC, EC_Cache::TCacheDataEC, "TCAlphaBitmapEC", 56)
        void p_destroy() override;
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        void Draw16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, WindowsSdk::TRect Clip);
        void DecodeToGraphBuf(GR_GraphBuf::TGraphBufGR* Buffer);
        void GetTexture(Direct3D9::IDirect3DTexture9& Result);
        void* TransBuf16;
        void* TransAlphaBuf16;
        void* AlphaBuf;
        // Native controls copy these dimensions as one point.
        WindowsSdk::TPoint PixelSize;
        GR_DX::TTextureGR* SurfaceCache;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheAlphaBitmap
