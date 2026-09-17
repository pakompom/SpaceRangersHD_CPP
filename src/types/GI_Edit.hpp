#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheBitmap {
    struct TCBitmapControlEC;

} // namespace EC_CacheBitmap

namespace EC_CacheFont {
    struct TCFontControlEC;

} // namespace EC_CacheFont

namespace GI_Edit {
    struct TEditGI;

    using TEditAcceptCharEventGI = pas::Method<std::uint8_t(GI_MessageLoop::TObjectGI*, char16_t)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEditGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TEditGI, GI_MessageLoop::TObjectGI, "TEditGI", 364)
        void p_destroy() override;
        void Clear() override;
        void SetFontName(pas::WideString FontName);
        void SetBorderEnabled(std::uint8_t Value);
        // Resets CaretPosition on change; does not clamp to MaxLength or invoke ChangedCallback.
        void SetText(pas::WideString Value);
        std::uint8_t HasGlyph(char16_t Character);
        void SetTextColor(std::uint32_t Value);
        void SetBorderLightColor(std::uint32_t Value);
        void SetBorderDarkColor(std::uint32_t Value);
        // Only Left and Center are accepted; other values raise.
        void SetTextAlignX(GI_Main::TTextAlignXGI Value);
        // Clamps to 0..Length(Text).
        void SetCaretPosition(std::int32_t Value);
        void ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void OnFocusGained() override;
        void OnFocusLost() override;
        void ProcessKeyDown(std::int32_t Key) override;
        // Requires a font glyph, acceptance by the optional callback, and length below MaxLength.
        void ProcessCharacter(char16_t Character) override;
        void OnCaretBlink() override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(WindowsSdk::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheFont::TCFontControlEC* FontCache;
        EC_CacheBitmap::TCBitmapControlEC* BackgroundCache;
        pas::WideString Text;
        std::uint32_t TextColor;
        std::uint32_t CaretColor;
        std::uint8_t BorderEnabled;
        std::uint8_t cpp_padding[3];
        std::uint32_t BorderLightColor;
        std::uint32_t BorderDarkColor;
        std::int32_t MaxLength;
        std::uint8_t HasFocus;
        std::uint8_t cpp_padding_2[3];
        std::int32_t CaretPosition;
        std::uint8_t AutoScrollText;
        GI_Main::TTextAlignXGI TextAlignX;
        std::uint8_t cpp_padding_3[2];
        GI_MessageLoop::TObjectNotifyEventGI ChangedCallback;
        GI_MessageLoop::TObjectNotifyEventGI FocusLostCallback;
        TEditAcceptCharEventGI AcceptCharCallback;
        std::uint8_t ClearFocusOnEnter;
        std::uint8_t cpp_padding_4[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Edit
