#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Cache.hpp"
#include "types/GR_GraphBuf.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CacheHSAI {
    struct THSAIHeaderEC;

} // namespace EC_CacheHSAI

namespace GR_DX {
    struct TTextureGR;

} // namespace GR_DX

namespace EC_CacheHSAI {
    struct TCHSAIControlEC;

    struct TCHSAIEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCHSAIControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCHSAIControlEC, EC_Cache::TCacheControlEC, "TCHSAIControlEC", 24)
        void QueueLoadIfMissing(pas::List* PendingLoads) override;
        EC_Cache::TCacheDataEC* CreateData() override;
        EC_Cache::TCacheDataEC* AcquireData() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PHSAIHeaderEC = THSAIHeaderEC*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCHSAIEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCHSAIEC, EC_Cache::TCacheDataEC, "TCHSAIEC", 52)
        void p_destroy() override;
        std::uint32_t GetFrameCount();
        void* GetFrameIndexPlane(std::uint32_t FrameIndex);
        GR_GraphBuf::PColorRGBA GetFramePalette(std::uint32_t FrameIndex);
        void GetOrCreateFrameSurface(std::uint32_t FrameIndex, Direct3D9::IDirect3DTexture9& Result);
        std::int32_t GetSourcePitchBytes();
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        void* BlobData;
        PHSAIHeaderEC Header;
        std::int32_t Width;
        std::int32_t Height;
        GR_DX::TTextureGR* FrameSurfaceCache;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct THSAIHeaderEC {
        std::uint8_t cpp_padding[4];
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t PitchBytes;
        std::uint32_t FrameCount;
        std::uint32_t FrameStride;
        std::uint8_t cpp_padding_2[24];
        std::uint32_t PalettePresent;
    };
    #pragma pack(pop)

} // namespace EC_CacheHSAI
