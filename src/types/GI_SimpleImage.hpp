#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheBitmap {
    struct TCBitmapControlEC;

} // namespace EC_CacheBitmap

namespace GI_SimpleImage {
    struct TSimpleImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSimpleImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TSimpleImageGI, GI_MessageLoop::TObjectGI, "TSimpleImageGI", 296)
        void p_destroy() override;
        void Clear() override;
        void SetImagePath(const pas::WideString& ImagePath);
        Types::TPoint GetContentSize();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetHalfAlpha(std::uint8_t Value);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheBitmap::TCBitmapControlEC* ImageCache;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t HalfAlpha;
        std::uint8_t SourceRGBA;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_SimpleImage
