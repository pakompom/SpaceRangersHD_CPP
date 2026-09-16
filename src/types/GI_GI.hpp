#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheGI {
    struct TCGiControlEC;

} // namespace EC_CacheGI

namespace GI_GI {
    struct TgiGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TgiGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TgiGI, GI_MessageLoop::TObjectGI, "TgiGI", 296)
        void p_destroy() override;
        // Preserves alpha and the cache key.
        void Clear() override;
        void SetImagePath(const pas::WideString& ImagePath);
        pas::WideString GetImagePath();
        Types::TPoint GetContentSize();
        Types::TPoint GetContentOrigin();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetAlpha(std::uint8_t Value);
        // Black pixels do not count as hits.
        std::uint8_t HitTestPixel(Types::TPoint Point);
        // Returns the mean coordinates of nonzero rendered pixels, or (0,0) when none exist.
        Types::TPoint GetVisualCenter();
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        void SetHardwareMirrorHorizontal(std::uint8_t Value);
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheGI::TCGiControlEC* ImageCache;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t Alpha;
        // Passed to the hardware texture draw only.
        std::uint8_t HardwareMirrorHorizontal;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_GI
