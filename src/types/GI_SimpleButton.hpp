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

namespace GI_SimpleButton {
    struct TSimpleButtonGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSimpleButtonGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TSimpleButtonGI, GI_MessageLoop::TObjectGI, "TSimpleButtonGI", 300)
        void p_destroy() override;
        void Clear() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheBitmap::TCBitmapControlEC* CurrentImage;
        EC_CacheBitmap::TCBitmapControlEC* NormalImage;
        EC_CacheBitmap::TCBitmapControlEC* ActiveImage;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_SimpleButton
