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
        // Preserves the cache control and its key.
        void Clear() override;
        // The RGBA key suffix enables SourceRGBA; keys shorter than four characters preserve the previous flag.
        void SetImagePath(const pas::WideString& ImagePath);
        Types::TPoint GetContentSize();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetHalfAlpha(std::uint8_t Value);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        // Requires Image; does not update SourceRGBA from the key suffix.
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // CenterFill is unimplemented on both axes.
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
