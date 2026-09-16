#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_GraphButton {
    struct TGraphButtonGI;

    enum TGraphButtonKindGI : std::uint8_t {
        gbkNormal = 0,
        gbkFix = 1,
        gbkDisable = 2,
        gbkFixDisable = 3,
    };

    enum TGraphButtonHitKindGI : std::uint8_t {
        gbhRect = 0,
        gbhGraph = 1,
        gbhImageHit = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGraphButtonGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TGraphButtonGI, GI_MessageLoop::TObjectGI, "TGraphButtonGI", 500)
        void p_destroy() override;
        void Clear() override;
        void SetCaptionFontName(const pas::WideString& FontName);
        void SetCaption(const pas::WideString& Text);
        void SetCaptionColor(std::uint32_t Value);
        void SetCaptionShadowOffset(std::int32_t Value);
        void SetImageNormalPath(const pas::WideString& Path);
        void SetImageNormalActivePath(const pas::WideString& Path);
        void SetImageDownPath(const pas::WideString& Path);
        void SetImageDownActivePath(const pas::WideString& Path);
        void SetImageDisabledPath(const pas::WideString& Path);
        void SetImageDisabledActivePath(const pas::WideString& Path);
        void SetImageHitPath(const pas::WideString& Path);
        void SetKind(TGraphButtonKindGI Value);
        std::uint8_t HitTest(WindowsSdk::TPoint Point);
        void SetDown(std::uint8_t Value);
        void SetDisabled(std::uint8_t Value);
        std::uint8_t IsHovered();
        void SetHovered(std::uint8_t Value);
        WindowsSdk::TPoint GetMaxStateImageSize();
        void UpdateStateVisuals();
        void UpdateStateImagePlacement();
        void SetSize(WindowsSdk::TPoint Size) override;
        void SetOrigin(WindowsSdk::TPoint Origin) override;
        void OnActivate() override;
        void OnDeactivate() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessMouseMove(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void OnHoverGained() override;
        void OnHoverLost() override;
        void ExecuteOnPressCode();
        void ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void ProcessLeftButtonDoubleClick(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadButtonProperties(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        TGraphButtonKindGI Kind;
        TGraphButtonHitKindGI HitKind;
        std::uint8_t Down;
        std::uint8_t Disabled;
        std::uint8_t cpp_padding[4];
        GI_MessageLoop::TObjectNotifyEventGI DownCallback;
        GI_MessageLoop::TObjectNotifyEventGI UpCallback;
        GI_MessageLoop::TObjectNotifyEventGI StateChangedCallback;
        GI_Image::TImageGI* ImageNormal;
        GI_Image::TImageGI* ImageNormalActive;
        GI_Image::TImageGI* ImageDown;
        GI_Image::TImageGI* ImageDownActive;
        GI_Image::TImageGI* ImageDisabled;
        GI_Image::TImageGI* ImageDisabledActive;
        GI_Image::TImageGI* ImageHit;
        GI_Label::TLabelGI* CaptionLabel;
        WindowsSdk::TPoint NormalOffset;
        WindowsSdk::TPoint NormalActiveOffset;
        WindowsSdk::TPoint DownOffset;
        WindowsSdk::TPoint DownActiveOffset;
        WindowsSdk::TPoint DisabledOffset;
        WindowsSdk::TPoint DisabledActiveOffset;
        WindowsSdk::TPoint HitOffset;
        pas::WideString EnterSound;
        pas::WideString LeaveSound;
        pas::WideString ClickSound;
        WindowsSdk::TRect CaptionOffsets;
        pas::Array<std::uint32_t, 0, 5> CaptionColors;
        pas::Array<std::uint32_t, 0, 5> CaptionShadowColors;
        GI_Main::TTextAlignXGI CaptionAlignX;
        GI_Main::TTextAlignYGI CaptionAlignY;
        std::uint8_t cpp_padding_2[2];
        std::uint32_t ImageAutoUpdateFlags;
        std::uint8_t UpOnlyDown;
        std::uint8_t cpp_padding_3[3];
        EC_BlockPar::TBlockParEC* OnPressCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_GraphButton
