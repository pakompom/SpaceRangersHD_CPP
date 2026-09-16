#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GR_DX {
    struct TTextureGR;

} // namespace GR_DX

namespace GI_Label {
    struct TLabelGI;

    using TCreateLabelControlEventGI = pas::Method<GI_MessageLoop::TObjectGI*(TLabelGI*, EC_CacheFont::PFontObjectEC)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TLabelGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TLabelGI, GI_MessageLoop::TObjectGI, "TLabelGI", 364)
        void p_destroy() override;
        void Clear() override;
        void SetFontName(const pas::WideString& FontName);
        void SetTextBorderWidth(std::int32_t Value);
        void SetTextBorderColor(std::uint32_t Value);
        void SetShadowOffset(std::int32_t Value);
        void SetShadowColor(std::uint32_t Value);
        void SetText(const pas::WideString& Text);
        void LoadTextLinesFromBlockParam(EC_BlockPar::TBlockParEC* Block, const pas::WideString& ParamName);
        pas::WideString GetText();
        void SetTextAlignX(GI_Main::TTextAlignXGI Value);
        void SetTextAlignY(GI_Main::TTextAlignYGI Value);
        void SetWordWrapEnabled(std::uint8_t Value);
        void SetAutoHeightPadding(std::int32_t Value);
        // An empty path frees the embedded child.
        void SetEmbeddedImagePath(const pas::WideString& ImagePath);
        void SetEmbeddedImageKindX(GI_Main::TImageKindXGI Value);
        void SetEmbeddedImageKindY(GI_Main::TImageKindYGI Value);
        void SetEmbeddedImageHalfAlpha(std::uint8_t Value);
        void SetTextColor(std::uint32_t Value);
        void SetBorderLightColor(std::uint32_t Value);
        void SetBorderDarkColor(std::uint32_t Value);
        // TopAdjustment is optional; includes text outline/shadow padding.
        WindowsSdk::TPoint MeasureContentSize(WindowsSdk::PInteger TopAdjustment);
        std::int32_t GetLineHeight();
        // Includes word wrapping when enabled.
        std::int32_t GetRenderedLineCount();
        // May resize the control to fit its text.
        void UpdateHitTestBounds() override;
        void SetSize(WindowsSdk::TPoint Size) override;
        // Missing embedded controls are requested through the creation callback.
        void UpdateEmbeddedControls(EC_CacheFont::TCFontEC* Font);
        void RemoveUnusedEmbeddedControls(EC_CacheFont::TCFontEC* Font);
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void Draw(WindowsSdk::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheFont::TCFontControlEC* FontCache;
        GI_Image::TImageGI* EmbeddedImage;
        EC_Str::TStringsEC* TextLines;
        std::uint32_t TextColor;
        std::int32_t TextBorderWidth;
        std::uint32_t TextBorderColor;
        std::int32_t TextShadowOffset;
        std::uint32_t TextShadowColor;
        std::uint8_t BorderEnabled;
        std::uint8_t cpp_padding[3];
        std::uint32_t BorderLightColor;
        std::uint32_t BorderDarkColor;
        GI_Main::TTextAlignXGI TextAlignX;
        GI_Main::TTextAlignYGI TextAlignY;
        std::uint8_t cpp_padding_2[2];
        std::int32_t TextLeft;
        std::int32_t TextTop;
        std::uint8_t WordWrapEnabled;
        std::uint8_t cpp_padding_3[3];
        std::int32_t AutoHeightPadding;
        TCreateLabelControlEventGI CreateEmbeddedControl;
        GR_DX::TTextureGR* TextTexture;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Label
