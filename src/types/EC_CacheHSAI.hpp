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
        // Returns nil when FrameIndex is outside the header count.
        void* GetFrameIndexPlane(std::uint32_t FrameIndex);
        // Returns nil for an invalid frame or absent palette.
        GR_GraphBuf::PColorRGBA GetFramePalette(std::uint32_t FrameIndex);
        // Requires a valid frame and palette; uses Width rather than PitchBytes as the source pitch.
        void GetOrCreateFrameSurface(std::uint32_t FrameIndex, Direct3D9::IDirect3DTexture9& Result);
        std::int32_t GetSourcePitchBytes();
        // Only the minimum 0x34-byte header size is validated. Ignores LoadOption.
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
        // HSAI/HAI disk header shared with OKGF; indexed ship frames use 1024 palette bytes.
        // $04210420.
        std::uint32_t Magic;
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t PitchBytes;
        std::uint32_t FrameCount;
        std::uint32_t FrameStride;
        std::uint32_t HasPalette;
        std::uint32_t BitsPerPixel;
        std::uint32_t RedMask;
        std::uint32_t GreenMask;
        std::uint32_t BlueMask;
        std::uint32_t AlphaMask;
        // Byte count, tested for zero by GetFramePalette.
        std::uint32_t PaletteBytes;
    };
    #pragma pack(pop)

} // namespace EC_CacheHSAI
