#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheBitmap {
    struct TCBitmapControlEC;

} // namespace EC_CacheBitmap

namespace EC_CacheRotateBuf {
    struct TCRotateBufControlEC;

} // namespace EC_CacheRotateBuf

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_RotateImage2 {
    struct TRotateImage2GI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRotateImage2GI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TRotateImage2GI, GI_MessageLoop::TObjectGI, "TRotateImage2GI", 304)
        void p_destroy() override;
        void Clear() override;
        void SetAngle(std::uint8_t Value);
        void SetAlpha(std::uint8_t Value);
        void SetImage(pas::WideString Path, Types::TPoint ImageSize, Types::TPoint Pivot);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        void Draw(Types::TRect ClipRect) override;
        EC_CacheBitmap::TCBitmapControlEC* ImageCache;
        EC_CacheRotateBuf::TCRotateBufControlEC* RotationCache;
        GR_GraphBuf::TGraphBufGR* RotatedImage;
        std::uint8_t RenderedAngle;
        std::uint8_t Angle;
        std::uint8_t Alpha;
        std::uint8_t ImageDirty;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_RotateImage2
