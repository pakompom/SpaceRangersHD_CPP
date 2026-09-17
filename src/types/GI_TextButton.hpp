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

namespace EC_CacheFont {
    struct TCFontControlEC;

} // namespace EC_CacheFont

namespace GI_TextButton {
    struct TTextButtonGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextButtonGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TTextButtonGI, GI_MessageLoop::TObjectGI, "TTextButtonGI", 344)
        void p_destroy() override;
        void Clear() override;
        void OnActivate() override;
        void OnDeactivate() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheFont::TCFontControlEC* FontCache;
        EC_CacheBitmap::TCBitmapControlEC* ImageCache;
        std::int32_t Kind;
        pas::WideString Caption;
        std::uint32_t CaptionColor;
        std::uint32_t CaptionActiveColor;
        std::uint32_t BorderLightColor;
        std::uint32_t BorderDarkColor;
        std::uint8_t Hover;
        std::uint8_t Down;
        std::uint8_t cpp_padding[6];
        GI_MessageLoop::TObjectNotifyEventGI DownCallback;
        GI_MessageLoop::TObjectNotifyEventGI UpCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_TextButton
