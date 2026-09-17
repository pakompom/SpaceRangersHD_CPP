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

namespace GI_InfiniteImage {
    struct TInfiniteImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TInfiniteImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TInfiniteImageGI, GI_MessageLoop::TObjectGI, "TInfiniteImageGI", 292)
        void p_destroy() override;
        // Resets size to two billion pixels on each axis and centers the origin.
        void SetImagePath(pas::WideString Path);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        // The hardware drawing path is unimplemented.
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheBitmap::TCBitmapControlEC* ImageCache;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_InfiniteImage
