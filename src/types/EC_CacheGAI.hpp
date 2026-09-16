#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Cache.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_DX {
    struct TTextureGR;

} // namespace GR_DX

namespace EC_CacheGAI {
    struct TCGaiControlEC;

    struct TCGaiEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCGaiControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCGaiControlEC, EC_Cache::TCacheControlEC, "TCGaiControlEC", 24)
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
    struct TCGaiEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCGaiEC, EC_Cache::TCacheDataEC, "TCGaiEC", 60)
        void p_destroy() override;
        std::int32_t GetFrameCount();
        std::uint8_t HasPlaybackFlags();
        WindowsSdk::TRect GetBoundsRect();
        WindowsSdk::TPoint GetCanvasSize();
        void GetOrCreateFrameSurface(std::int32_t FrameIndex, Direct3D9::IDirect3DTexture9& Result);
        WindowsSdk::TPoint GetFrameOrigin(std::int32_t FrameIndex);
        GR_gi::TgiGR* LoadFrameGi(std::int32_t FrameIndex);
        std::uint8_t IsFrameCompressed(std::int32_t FrameIndex);
        std::int32_t GetSequenceCount();
        std::int32_t GetSequenceFrameCount(std::int32_t SequenceIndex);
        void FillSequenceFrameIndexTable(std::int32_t SequenceIndex, void* DestTable, std::int32_t EntryStride);
        void FillSequenceFrameDelayTable(std::int32_t SequenceIndex, void* DestTable, std::int32_t EntryStride);
        std::int32_t GetSequenceFrameIndex(std::int32_t SequenceIndex, std::int32_t FrameInSequence);
        std::int32_t GetSequenceFrameDelay(std::int32_t SequenceIndex, std::int32_t FrameInSequence);
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        static void ApplyAB2BackgroundFixup(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& ResourceKey);
        void* RawGaiData;
        GR_gi::PGaiHeader Header;
        GR_gi::TgiGR* DecodedFrameGi;
        GR_gi::PGaiSequenceTableHeader SequenceTableData;
        std::uint8_t SkipPalettedColorCacheBuild;
        std::uint8_t cpp_padding[3];
        GR_DX::TTextureGR* FrameSurfaceCache;
        pas::DynArray<WindowsSdk::TPoint> CachedFrameOrigins;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheGAI
