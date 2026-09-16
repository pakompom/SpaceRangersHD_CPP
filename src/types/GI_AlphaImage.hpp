#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheAlphaBitmap {
    struct TCAlphaBitmapControlEC;

} // namespace EC_CacheAlphaBitmap

namespace GI_AlphaImage {
    struct TAlphaImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAlphaImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TAlphaImageGI, GI_MessageLoop::TObjectGI, "TAlphaImageGI", 296)
        void p_destroy() override;
        void Clear() override;
        void SetImagePath(const pas::WideString& ImagePath);
        Types::TPoint GetContentSize();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        std::uint8_t HitTestPixel(Types::TPoint Point);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheAlphaBitmap::TCAlphaBitmapControlEC* ImageCache;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_AlphaImage
