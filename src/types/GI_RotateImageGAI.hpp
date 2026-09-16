#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_DX.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheGAI {
    struct TCGaiControlEC;

} // namespace EC_CacheGAI

namespace EC_CacheRotateBuf {
    struct TCRotateBufControlEC;

} // namespace EC_CacheRotateBuf

namespace GR_GraphBufPal {
    struct TGraphBufPalGR;

} // namespace GR_GraphBufPal

namespace GI_RotateImageGAI {
    struct TRotateImageGaiGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRotateImageGaiGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TRotateImageGaiGI, GI_MessageLoop::TObjectGI, "TRotateImageGaiGI", 452)
        void p_destroy() override;
        void Clear() override;
        void SetAngle(std::uint8_t Value);
        void SetAlpha(std::uint8_t Value);
        void SetImage(pas::WideString Path, Types::TPoint ImageSize, Types::TPoint Pivot);
        void SetFrame(std::int32_t Value);
        void ClearFrameSequence();
        std::int32_t GetFrameSourceIndex(std::int32_t Index);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        // Diagnostic retains TgaiGI.AfterLoad, but this is TRotateImageGaiGI's geometry-update override.
        void UpdateAutoGeometry() override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheGAI::TCGaiControlEC* ImageCache;
        EC_CacheRotateBuf::TCRotateBufControlEC* RotationCache;
        GR_GraphBufPal::TGraphBufPalGR* RotatedImage;
        std::uint8_t RenderedAngle;
        std::uint8_t Angle;
        std::uint8_t Alpha;
        std::uint8_t ImageDirty;
        std::int32_t RenderedFrameIndex;
        std::int32_t FrameIndex;
        std::int32_t FrameCount;
        System::PInteger FrameIndexTable;
        System::PInteger FrameDelayTable;
        std::int32_t AnimationIndex;
        Types::TPoint ImageSize;
        pas::Array<GR_DX::TScreenVertexGR, 0, 3> Vertices;
        Direct3D9::IDirect3DTexture9 FrameTexture;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_RotateImageGAI
