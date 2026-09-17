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

namespace GI_RotateImage {
    struct TRotateImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRotateImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TRotateImageGI, GI_MessageLoop::TObjectGI, "TRotateImageGI", 300)
        void p_destroy() override;
        // Preserves both cache keys.
        void Clear() override;
        // A full turn has 256 steps.
        void SetAngle(std::uint8_t Value);
        // Leaves bounds unchanged when the rotation cache key is empty.
        void UpdateHitTestBounds() override;
        // Leaves Result unwritten when the rotation cache key is empty.
        Types::TRect GetLocalBounds() override;
        // Uses the current Origin as the rotation pivot.
        void SetImage(pas::WideString Path, Types::TPoint ImageSize);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheBitmap::TCBitmapControlEC* ImageCache;
        EC_CacheRotateBuf::TCRotateBufControlEC* RotationCache;
        std::uint8_t Angle;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_RotateImage
