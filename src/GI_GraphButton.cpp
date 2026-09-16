#include "layout/GI_GraphButton.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/WindowsSdk.hpp"

namespace GI_GraphButton {
    void TGraphButtonGI_Create(TGraphButtonGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Kind = gbkNormal;
        Self->HitKind = gbhRect;
        Self->UpOnlyDown = false;
        Self->CaptionColors[0] = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->CaptionColors[1] = Self->CaptionColors[0];
        Self->CaptionColors[2] = Self->CaptionColors[0];
        Self->CaptionColors[3] = Self->CaptionColors[0];
        Self->CaptionColors[4] = Self->CaptionColors[0];
        Self->CaptionColors[5] = Self->CaptionColors[0];
        Self->CaptionAlignX = GI_Main::taxCenter;
        Self->CaptionAlignY = GI_Main::tayCenterEx;
        Self->OnPressCode = nullptr;
    }

    void TGraphButtonGI_Destroy(TGraphButtonGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TGraphButtonGI::Clear() {
        Kind = gbkNormal;
        if (ImageNormal != nullptr) {
            pas::free(ImageNormal);
            ImageNormal = nullptr;
        }
        if (ImageNormalActive != nullptr) {
            pas::free(ImageNormalActive);
            ImageNormalActive = nullptr;
        }
        if (ImageDown != nullptr) {
            pas::free(ImageDown);
            ImageDown = nullptr;
        }
        if (ImageDownActive != nullptr) {
            pas::free(ImageDownActive);
            ImageDownActive = nullptr;
        }
        if (ImageDisabled != nullptr) {
            pas::free(ImageDisabled);
            ImageDisabled = nullptr;
        }
        if (ImageDisabledActive != nullptr) {
            pas::free(ImageDisabledActive);
            ImageDisabledActive = nullptr;
        }
        if (ImageHit != nullptr) {
            pas::free(ImageHit);
            ImageHit = nullptr;
        }
        if (CaptionLabel != nullptr) {
            pas::free(CaptionLabel);
            CaptionLabel = nullptr;
        }
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TGraphButtonGI::SetCaptionFontName(const pas::WideString& FontName) {
        if (CaptionLabel == nullptr) {
            CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this);
            CaptionLabel->SetPosition(ClassesImports::Point(0, 0));
            CaptionLabel->SetSize(ClientSize);
            CaptionLabel->SetDepth(-9999.0);
            CaptionLabel->SetTextAlignX(CaptionAlignX);
            CaptionLabel->SetTextAlignY(CaptionAlignY);
        }
        CaptionLabel->SetFontName(FontName);
    }

    void TGraphButtonGI::SetCaption(const pas::WideString& Text) {
        if (CaptionLabel == nullptr) {
            CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this);
            CaptionLabel->SetPosition(ClassesImports::Point(0, 0));
            CaptionLabel->SetSize(ClientSize);
            CaptionLabel->SetDepth(-9999.0);
            CaptionLabel->SetTextAlignX(CaptionAlignX);
            CaptionLabel->SetTextAlignY(CaptionAlignY);
        }
        CaptionLabel->SetText(Text);
    }

    void TGraphButtonGI::SetCaptionColor(std::uint32_t Value) {
        CaptionColors[0] = Value;
        CaptionColors[1] = Value;
        CaptionColors[2] = Value;
        CaptionColors[3] = Value;
        CaptionColors[4] = Value;
        CaptionColors[5] = Value;
        if (CaptionLabel == nullptr) {
            CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this);
            CaptionLabel->SetPosition(ClassesImports::Point(0, 0));
            CaptionLabel->SetSize(ClientSize);
            CaptionLabel->SetDepth(-9999.0);
            CaptionLabel->SetTextAlignX(CaptionAlignX);
            CaptionLabel->SetTextAlignY(CaptionAlignY);
        }
        CaptionLabel->SetTextColor(Value);
    }

    void TGraphButtonGI::SetCaptionShadowOffset(std::int32_t Value) {
        if (CaptionLabel == nullptr) {
            CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this);
            CaptionLabel->SetPosition(ClassesImports::Point(0, 0));
            CaptionLabel->SetSize(ClientSize);
            CaptionLabel->SetDepth(-9999.0);
            CaptionLabel->SetTextAlignX(CaptionAlignX);
            CaptionLabel->SetTextAlignY(CaptionAlignY);
        }
        CaptionLabel->SetShadowOffset(Value);
    }

    void TGraphButtonGI::SetImageNormalPath(const pas::WideString& Path) {
        if (ImageNormal == nullptr) {
            ImageNormal = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageNormal->SetDepth(1.0);
        ImageNormal->AutoUpdateFlags = ImageAutoUpdateFlags;
        ImageNormal->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageNormal->GetContentSize();
            GI_Image::TImageGI* imageNormal = ImageNormal;
            imageNormal->SetSize(contentSize);
        }
        ImageNormal->SetPosition(NormalOffset);
    }

    void TGraphButtonGI::SetImageNormalActivePath(const pas::WideString& Path) {
        if (ImageNormalActive == nullptr) {
            ImageNormalActive = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageNormalActive->SetDepth(1.0);
        ImageNormalActive->AutoUpdateFlags = ImageAutoUpdateFlags;
        ImageNormalActive->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageNormalActive->GetContentSize();
            GI_Image::TImageGI* imageNormalActive = ImageNormalActive;
            imageNormalActive->SetSize(contentSize);
        }
        ImageNormalActive->SetPosition(NormalActiveOffset);
    }

    void TGraphButtonGI::SetImageDownPath(const pas::WideString& Path) {
        if (ImageDown == nullptr) {
            ImageDown = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageDown->SetDepth(1.0);
        ImageDown->AutoUpdateFlags = ImageAutoUpdateFlags;
        ImageDown->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageDown->GetContentSize();
            GI_Image::TImageGI* imageDown = ImageDown;
            imageDown->SetSize(contentSize);
        }
        ImageDown->SetPosition(DownOffset);
    }

    void TGraphButtonGI::SetImageDownActivePath(const pas::WideString& Path) {
        if (ImageDownActive == nullptr) {
            ImageDownActive = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageDownActive->SetDepth(1.0);
        ImageDownActive->AutoUpdateFlags = ImageAutoUpdateFlags;
        ImageDownActive->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageDownActive->GetContentSize();
            GI_Image::TImageGI* imageDownActive = ImageDownActive;
            imageDownActive->SetSize(contentSize);
        }
        ImageDownActive->SetPosition(DownActiveOffset);
    }

    void TGraphButtonGI::SetImageDisabledPath(const pas::WideString& Path) {
        if (ImageDisabled == nullptr) {
            ImageDisabled = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageDisabled->SetDepth(1.0);
        ImageDisabled->AutoUpdateFlags = ImageAutoUpdateFlags;
        ImageDisabled->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageDisabled->GetContentSize();
            GI_Image::TImageGI* imageDisabled = ImageDisabled;
            imageDisabled->SetSize(contentSize);
        }
        ImageDisabled->SetPosition(DisabledOffset);
    }

    void TGraphButtonGI::SetImageDisabledActivePath(const pas::WideString& Path) {
        if (ImageDisabledActive == nullptr) {
            ImageDisabledActive = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageDisabledActive->SetDepth(1.0);
        ImageDisabledActive->AutoUpdateFlags = ImageAutoUpdateFlags;
        ImageDisabledActive->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageDisabledActive->GetContentSize();
            GI_Image::TImageGI* imageDisabledActive = ImageDisabledActive;
            imageDisabledActive->SetSize(contentSize);
        }
        ImageDisabledActive->SetPosition(DisabledActiveOffset);
    }

    void TGraphButtonGI::SetImageHitPath(const pas::WideString& Path) {
        if (ImageHit == nullptr) {
            ImageHit = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        }
        ImageHit->SetImagePath(Path);
        {
            Types::TPoint contentSize = ImageHit->GetContentSize();
            GI_Image::TImageGI* imageHit = ImageHit;
            imageHit->SetSize(contentSize);
        }
        ImageHit->SetPosition(HitOffset);
    }

    void TGraphButtonGI::SetKind(TGraphButtonKindGI Value) {
        if (Kind != Value) {
            Kind = Value;
            UpdateStateVisuals();
        }
    }

    std::uint8_t TGraphButtonGI::HitTest(WindowsSdk::TPoint Point) {
        std::uint8_t Result = false;
        if (HitKind == gbhRect) {
            return ContainsPoint(Point);
        }
        if (HitKind == gbhGraph) {
            if (ImageNormal != nullptr) {
                Result = ImageNormal->HitTestPixel(Point);
            }
            if (Result) {
                return Result;
            }
            if (ImageNormalActive != nullptr) {
                Result = ImageNormalActive->HitTestPixel(Point);
            }
            if (Result) {
                return Result;
            }
            if (ImageDown != nullptr) {
                Result = ImageDown->HitTestPixel(Point);
            }
            if (Result) {
                return Result;
            }
            if (ImageDownActive != nullptr) {
                Result = ImageDownActive->HitTestPixel(Point);
            }
            if (Result) {
                return Result;
            }
            if (ImageDisabled != nullptr) {
                Result = ImageDisabled->HitTestPixel(Point);
            }
            if (Result) {
                return Result;
            }
            if (ImageDisabledActive != nullptr) {
                Result = ImageDisabledActive->HitTestPixel(Point);
            }
            static_cast<void>(Result);
        } else if (HitKind == gbhImageHit) {
            if (ImageHit != nullptr) {
                return ImageHit->HitTestPixel(Point);
            }
        }
        return Result;
    }

    void TGraphButtonGI::SetDown(std::uint8_t Value) {
        if (Down != Value) {
            Down = Value;
            UpdateStateVisuals();
        }
    }

    void TGraphButtonGI::SetDisabled(std::uint8_t Value) {
        if (Disabled != Value) {
            Disabled = Value;
            UpdateStateVisuals();
        }
    }

    std::uint8_t TGraphButtonGI::IsHovered() {
        return MessageLoop->HoveredControl == this;
    }

    void TGraphButtonGI::SetHovered(std::uint8_t Value) {
        if (Value) {
            MessageLoop->SetHoveredControl(this);
        } else if (MessageLoop->HoveredControl == this) {
            MessageLoop->SetHoveredControl(nullptr);
        }
    }

    WindowsSdk::TPoint TGraphButtonGI::GetMaxStateImageSize() {
        WindowsSdk::TPoint Result{};
        WindowsSdk::TPoint Size{};
        Result.X = 0;
        Result.Y = 0;
        if (ImageNormal != nullptr) {
            Size = ImageNormal->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        if (ImageNormalActive != nullptr) {
            Size = ImageNormalActive->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        if (ImageDown != nullptr) {
            Size = ImageDown->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        if (ImageDownActive != nullptr) {
            Size = ImageDownActive->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        if (ImageDisabled != nullptr) {
            Size = ImageDisabled->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        if (ImageDisabledActive != nullptr) {
            Size = ImageDisabledActive->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        if (ImageHit != nullptr) {
            Size = ImageHit->GetContentSize();
        }
        Result.X = std::max<std::int32_t>(Result.X, Size.X);
        Result.Y = std::max<std::int32_t>(Result.Y, Size.Y);
        return Result;
    }

    void TGraphButtonGI::UpdateStateVisuals() {
        if (ImageNormal != nullptr) {
            ImageNormal->SetActive(false);
        }
        if (ImageNormalActive != nullptr) {
            ImageNormalActive->SetActive(false);
        }
        if (ImageDown != nullptr) {
            ImageDown->SetActive(false);
        }
        if (ImageDownActive != nullptr) {
            ImageDownActive->SetActive(false);
        }
        if (ImageDisabled != nullptr) {
            ImageDisabled->SetActive(false);
        }
        if (ImageDisabledActive != nullptr) {
            ImageDisabledActive->SetActive(false);
        }
        if (ImageHit != nullptr) {
            ImageHit->SetActive(false);
        }
        if (Disabled && (Kind == gbkDisable || Kind == gbkFixDisable)) {
            if (MessageLoop->HoveredControl == this) {
                if (ImageDisabledActive != nullptr) {
                    ImageDisabledActive->SetActive(true);
                } else if (ImageDisabled != nullptr) {
                    ImageDisabled->SetActive(true);
                }
            } else if (ImageDisabled != nullptr) {
                ImageDisabled->SetActive(true);
            }
        } else if (Down) {
            if (MessageLoop->HoveredControl == this) {
                if (ImageDownActive != nullptr) {
                    ImageDownActive->SetActive(true);
                } else if (ImageDown != nullptr) {
                    ImageDown->SetActive(true);
                }
            } else if (ImageDown != nullptr) {
                ImageDown->SetActive(true);
            }
        } else if (MessageLoop->HoveredControl == this) {
            if (ImageNormalActive != nullptr) {
                ImageNormalActive->SetActive(true);
            } else if (ImageNormal != nullptr) {
                ImageNormal->SetActive(true);
            }
        } else if (ImageNormal != nullptr) {
            ImageNormal->SetActive(true);
        }
        if (CaptionLabel != nullptr) {
            if (!Down) {
                CaptionLabel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&CaptionOffsets, 0)));
            } else {
                CaptionLabel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&CaptionOffsets, 8)));
            }
            if (Disabled && (Kind == gbkDisable || Kind == gbkFixDisable)) {
                if (MessageLoop->HoveredControl == this) {
                    CaptionLabel->SetTextColor(CaptionColors[5]);
                    CaptionLabel->SetShadowColor(CaptionShadowColors[5]);
                } else {
                    CaptionLabel->SetTextColor(CaptionColors[4]);
                    CaptionLabel->SetShadowColor(CaptionShadowColors[4]);
                }
            } else if (Down) {
                if (MessageLoop->HoveredControl == this) {
                    CaptionLabel->SetTextColor(CaptionColors[3]);
                    CaptionLabel->SetShadowColor(CaptionShadowColors[3]);
                } else {
                    CaptionLabel->SetTextColor(CaptionColors[2]);
                    CaptionLabel->SetShadowColor(CaptionShadowColors[2]);
                }
            } else if (MessageLoop->HoveredControl == this) {
                CaptionLabel->SetTextColor(CaptionColors[1]);
                CaptionLabel->SetShadowColor(CaptionShadowColors[1]);
            } else {
                CaptionLabel->SetTextColor(CaptionColors[0]);
                CaptionLabel->SetShadowColor(CaptionShadowColors[0]);
            }
        }
        if (ImageNormal != nullptr) {
            if (ImageNormal->Active) {
                ImageNormal->RestartPlayback();
            }
        }
        if (ImageNormalActive != nullptr) {
            if (ImageNormalActive->Active) {
                ImageNormalActive->RestartPlayback();
            }
        }
        if (ImageDown != nullptr) {
            if (ImageDown->Active) {
                ImageDown->RestartPlayback();
            }
        }
        if (ImageDownActive != nullptr) {
            if (ImageDownActive->Active) {
                ImageDownActive->RestartPlayback();
            }
        }
        if (ImageDisabled != nullptr) {
            if (ImageDisabled->Active) {
                ImageDisabled->RestartPlayback();
            }
        }
        if (ImageDisabledActive != nullptr) {
            if (ImageDisabledActive->Active) {
                ImageDisabledActive->RestartPlayback();
            }
        }
        Invalidate();
        if (pas::assigned(StateChangedCallback)) {
            StateChangedCallback(this);
        }
    }

    void TGraphButtonGI::UpdateStateImagePlacement() {
        if (ImageNormal != nullptr) {
            ImageNormal->SetPosition(NormalOffset);
        }
        if (ImageNormalActive != nullptr) {
            ImageNormalActive->SetPosition(NormalActiveOffset);
        }
        if (ImageDown != nullptr) {
            ImageDown->SetPosition(DownOffset);
        }
        if (ImageDownActive != nullptr) {
            ImageDownActive->SetPosition(DownActiveOffset);
        }
        if (ImageDisabled != nullptr) {
            ImageDisabled->SetPosition(DisabledOffset);
        }
        if (ImageDisabledActive != nullptr) {
            ImageDisabledActive->SetPosition(DisabledActiveOffset);
        }
        if (ImageHit != nullptr) {
            ImageHit->SetPosition(HitOffset);
        }
        if (CaptionLabel != nullptr) {
            CaptionLabel->SetPosition(ClassesImports::Point(0, 0));
            CaptionLabel->SetSize(ClientSize);
        }
    }

    void TGraphButtonGI::SetSize(WindowsSdk::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        UpdateStateImagePlacement();
    }

    void TGraphButtonGI::SetOrigin(WindowsSdk::TPoint Origin) {
        GI_MessageLoop::TObjectGI::SetOrigin(Origin);
        UpdateStateImagePlacement();
    }

    void TGraphButtonGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        if (HitTestCursor()) {
            MessageLoop->HoveredControl = this;
        }
        if (Kind == gbkNormal || Kind == gbkDisable) {
            Down = false;
        }
        UpdateStateVisuals();
    }

    void TGraphButtonGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        if (MessageLoop->HoveredControl == this) {
            MessageLoop->HoveredControl = nullptr;
        }
        if (Kind == gbkNormal || Kind == gbkDisable) {
            Down = false;
        }
        UpdateStateVisuals();
    }

    void TGraphButtonGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
    }

    void TGraphButtonGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
        if (MessageLoop->HoveredControl == this) {
            MessageLoop->SetHoveredControl(nullptr);
        }
    }

    void TGraphButtonGI::ProcessMouseMove(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (MouseBlockingTest && IsOccludedAtPoint(AbsolutePosition)) {
            return;
        }
        if (HitTest(Point)) {
            if (!Disabled) {
                MessageLoop->SetHoveredControl(this);
            }
        } else if (MessageLoop->HoveredControl == this) {
            MessageLoop->SetHoveredControl(nullptr);
        }
    }

    void TGraphButtonGI::OnHoverGained() {
        if (EnterSound != u"" && static_cast<std::uint8_t>(Disabled ^ 1)) {
            GR_Main::SoundManager->PlaySound(EnterSound);
        }
        if (pas::assigned(HelpCallback)) {
            HelpCallback(this, true);
        }
        UpdateStateVisuals();
    }

    void TGraphButtonGI::OnHoverLost() {
        if (Kind == gbkNormal || Kind == gbkDisable) {
            if (Down) {
                Down = false;
                if (pas::assigned(UpCallback)) {
                    UpCallback(this);
                }
            }
        }
        if (LeaveSound != u"" && static_cast<std::uint8_t>(Disabled ^ 1)) {
            GR_Main::SoundManager->PlaySound(LeaveSound);
        }
        if (pas::assigned(HelpCallback)) {
            HelpCallback(this, false);
        }
        UpdateStateVisuals();
    }

    void TGraphButtonGI::ExecuteOnPressCode() {
        if (OnPressCode != nullptr) {
            MessageLoop->QueueUiCode(OnPressCode, true);
            if (pas::assigned(HelpCallback)) {
                HelpCallback(this, false);
            }
        } else {
            MessageLoop->RefreshMouseDispatch();
        }
    }

    void TGraphButtonGI::ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (IsOccludedAtPoint(Point)) {
            return;
        }
        if (!HitTest(Point)) {
            return;
        }
        if (MessageLoop->HoveredControl != this) {
            return;
        }
        if ((Kind == gbkDisable || Kind == gbkFixDisable) && Disabled == true) {
            return;
        }
        if (Kind == gbkNormal || Kind == gbkDisable) {
            Down = true;
            if (ClickSound != u"") {
                GR_Main::SoundManager->PlaySound(ClickSound);
            }
            if (pas::assigned(DownCallback)) {
                DownCallback(this);
                ExecuteOnPressCode();
            }
        } else if (Down) {
            Down = false;
            if (pas::assigned(UpCallback)) {
                UpCallback(this);
                ExecuteOnPressCode();
            } else if (!pas::assigned(DownCallback)) {
                ExecuteOnPressCode();
            }
        } else {
            Down = true;
            if (ClickSound != u"") {
                GR_Main::SoundManager->PlaySound(ClickSound);
            }
            if (pas::assigned(DownCallback)) {
                DownCallback(this);
                ExecuteOnPressCode();
            } else if (!pas::assigned(UpCallback)) {
                ExecuteOnPressCode();
            }
        }
        UpdateStateVisuals();
    }

    void TGraphButtonGI::ProcessLeftButtonUp(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::uint8_t WasDown{};
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        if (IsOccludedAtPoint(Point)) {
            return;
        }
        if (!HitTest(Point)) {
            return;
        }
        if (MessageLoop->HoveredControl != this) {
            return;
        }
        if ((Kind == gbkDisable || Kind == gbkFixDisable) && Disabled == true) {
            return;
        }
        if (Kind == gbkNormal || Kind == gbkDisable) {
            WasDown = Down;
            Down = false;
            if (pas::assigned(UpCallback) && MessageLoop->ConsumeTimerTickChange()) {
                if (UpOnlyDown == false || WasDown != false) {
                    UpCallback(this);
                }
                ExecuteOnPressCode();
            }
            if (static_cast<std::uint8_t>(pas::assigned(UpCallback) ^ 1) && static_cast<std::uint8_t>(pas::assigned(DownCallback) ^ 1)) {
                ExecuteOnPressCode();
            }
            UpdateStateVisuals();
        }
    }

    void TGraphButtonGI::ProcessLeftButtonDoubleClick(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDoubleClick(KeyState, Point);
    }

    void TGraphButtonGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        LoadButtonProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TGraphButtonGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadButtonProperties(Block);
    }

    void TGraphButtonGI::LoadButtonProperties(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        if (Block->CountParams(u"CaptionAlignY"_wref.get()) > 0) {
            Text = EC_Str::TrimWideString(Block->GetParam(u"CaptionAlignY"_wref.get()));
            CaptionAlignY = GI_Main::ParseTextAlignYName(Text);
        }
        if (Block->CountParams(u"CaptionAlignX"_wref.get()) > 0) {
            Text = EC_Str::TrimWideString(Block->GetParam(u"CaptionAlignX"_wref.get()));
            CaptionAlignX = GI_Main::ParseTextAlignXName(Text);
        }
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            SetCaptionFontName(Block->GetParam(u"Font"_wref.get()));
        }
        if (Block->CountParams(u"Caption"_wref.get()) > 0) {
            Text = Block->GetParam(u"Caption"_wref.get());
            SetCaption(Text);
            if (GR_Main::LanguageDataConfig->CountParamsByPath(Text) > 0) {
                SetCaption(GR_Main::LanguageDataConfig->GetParamByPathOrMarker(Text));
            }
        }
        if (Block->CountParams(u"CaptionColor"_wref.get()) > 0) {
            SetCaptionColor(GI_Main::GetColorGI(Block->GetParam(u"CaptionColor"_wref.get())));
        }
        if (Block->CountParams(u"CaptionShadow"_wref.get()) > 0) {
            SetCaptionShadowOffset(EC_Str::ExtractDigitsToIntW(Block->GetParam(u"CaptionShadow"_wref.get())));
        }
        if (Block->CountParams(u"CaptionColorNormal"_wref.get()) > 0) {
            CaptionColors[0] = GI_Main::GetColorGI(Block->GetParam(u"CaptionColorNormal"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColorNormalA"_wref.get()) > 0) {
            CaptionColors[1] = GI_Main::GetColorGI(Block->GetParam(u"CaptionColorNormalA"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColorDown"_wref.get()) > 0) {
            CaptionColors[2] = GI_Main::GetColorGI(Block->GetParam(u"CaptionColorDown"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColorDownA"_wref.get()) > 0) {
            CaptionColors[3] = GI_Main::GetColorGI(Block->GetParam(u"CaptionColorDownA"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColorDisable"_wref.get()) > 0) {
            CaptionColors[4] = GI_Main::GetColorGI(Block->GetParam(u"CaptionColorDisable"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColorDisableA"_wref.get()) > 0) {
            CaptionColors[5] = GI_Main::GetColorGI(Block->GetParam(u"CaptionColorDisableA"_wref.get()));
        }
        if (Block->CountParams(u"CaptionShadowColorNormal"_wref.get()) > 0) {
            CaptionShadowColors[0] = GI_Main::GetColorGI(Block->GetParam(u"CaptionShadowColorNormal"_wref.get()));
        }
        if (Block->CountParams(u"CaptionShadowColorNormalA"_wref.get()) > 0) {
            CaptionShadowColors[1] = GI_Main::GetColorGI(Block->GetParam(u"CaptionShadowColorNormalA"_wref.get()));
        }
        if (Block->CountParams(u"CaptionShadowColorDown"_wref.get()) > 0) {
            CaptionShadowColors[2] = GI_Main::GetColorGI(Block->GetParam(u"CaptionShadowColorDown"_wref.get()));
        }
        if (Block->CountParams(u"CaptionShadowColorDownA"_wref.get()) > 0) {
            CaptionShadowColors[3] = GI_Main::GetColorGI(Block->GetParam(u"CaptionShadowColorDownA"_wref.get()));
        }
        if (Block->CountParams(u"CaptionShadowColorDisable"_wref.get()) > 0) {
            CaptionShadowColors[4] = GI_Main::GetColorGI(Block->GetParam(u"CaptionShadowColorDisable"_wref.get()));
        }
        if (Block->CountParams(u"CaptionShadowColorDisableA"_wref.get()) > 0) {
            CaptionShadowColors[5] = GI_Main::GetColorGI(Block->GetParam(u"CaptionShadowColorDisableA"_wref.get()));
        }
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            Text = Block->GetParam(u"Kind"_wref.get());
            if (Text == u"Normal") {
                SetKind(gbkNormal);
            } else if (Text == u"Fix") {
                SetKind(gbkFix);
            } else if (Text == u"Disable") {
                SetKind(gbkDisable);
            } else if (Text == u"FixDisable") {
                SetKind(gbkFixDisable);
            }
        }
        if (Block->CountParams(u"Auto"_wref.get()) > 0) {
            ImageAutoUpdateFlags = GI_Main::ParseAutoGeometryFlagsGI(Block->GetParam(u"Auto"_wref.get()));
        }
        if (Block->CountParams(u"KindHit"_wref.get()) > 0) {
            Text = Block->GetParam(u"KindHit"_wref.get());
            if (Text == u"Rect") {
                HitKind = gbhRect;
            } else if (Text == u"Graph") {
                HitKind = gbhGraph;
            } else if (Text == u"ImageHit") {
                HitKind = gbhImageHit;
            }
        }
        if (Block->CountParams(u"ImageNormal"_wref.get()) > 0) {
            SetImageNormalPath(Block->GetParam(u"ImageNormal"_wref.get()));
        }
        if (Block->CountParams(u"ImageNormalA"_wref.get()) > 0) {
            SetImageNormalActivePath(Block->GetParam(u"ImageNormalA"_wref.get()));
        }
        if (Block->CountParams(u"ImageDown"_wref.get()) > 0) {
            SetImageDownPath(Block->GetParam(u"ImageDown"_wref.get()));
        }
        if (Block->CountParams(u"ImageDownA"_wref.get()) > 0) {
            SetImageDownActivePath(Block->GetParam(u"ImageDownA"_wref.get()));
        }
        if (Block->CountParams(u"ImageDisable"_wref.get()) > 0) {
            SetImageDisabledPath(Block->GetParam(u"ImageDisable"_wref.get()));
        }
        if (Block->CountParams(u"ImageDisableA"_wref.get()) > 0) {
            SetImageDisabledActivePath(Block->GetParam(u"ImageDisableA"_wref.get()));
        }
        if (Block->CountParams(u"ImageHit"_wref.get()) > 0) {
            SetImageHitPath(Block->GetParam(u"ImageHit"_wref.get()));
        }
        if (Block->CountParams(u"ImageNormal_Pos"_wref.get()) > 0) {
            NormalOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageNormal_Pos"_wref.get()));
        } else {
            NormalOffset = LocalPosition;
        }
        if (Block->CountParams(u"ImageNormalA_Pos"_wref.get()) > 0) {
            NormalActiveOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageNormalA_Pos"_wref.get()));
        } else {
            NormalActiveOffset = LocalPosition;
        }
        if (Block->CountParams(u"ImageDown_Pos"_wref.get()) > 0) {
            DownOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageDown_Pos"_wref.get()));
        } else {
            DownOffset = LocalPosition;
        }
        if (Block->CountParams(u"ImageDownA_Pos"_wref.get()) > 0) {
            DownActiveOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageDownA_Pos"_wref.get()));
        } else {
            DownActiveOffset = LocalPosition;
        }
        if (Block->CountParams(u"ImageDisable_Pos"_wref.get()) > 0) {
            DisabledOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageDisable_Pos"_wref.get()));
        } else {
            DisabledOffset = LocalPosition;
        }
        if (Block->CountParams(u"ImageDisableA_Pos"_wref.get()) > 0) {
            DisabledActiveOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageDisableA_Pos"_wref.get()));
        } else {
            DisabledActiveOffset = LocalPosition;
        }
        if (Block->CountParams(u"ImageHit_Pos"_wref.get()) > 0) {
            HitOffset = GI_Main::GetPointGI(Block->GetParam(u"ImageHit_Pos"_wref.get()));
        } else {
            HitOffset = LocalPosition;
        }
        NormalOffset = EC_Struct::SubtractPoints(NormalOffset, LocalPosition);
        NormalActiveOffset = EC_Struct::SubtractPoints(NormalActiveOffset, LocalPosition);
        DownOffset = EC_Struct::SubtractPoints(DownOffset, LocalPosition);
        DownActiveOffset = EC_Struct::SubtractPoints(DownActiveOffset, LocalPosition);
        DisabledOffset = EC_Struct::SubtractPoints(DisabledOffset, LocalPosition);
        DisabledActiveOffset = EC_Struct::SubtractPoints(DisabledActiveOffset, LocalPosition);
        HitOffset = EC_Struct::SubtractPoints(HitOffset, LocalPosition);
        if (Block->CountParams(u"Disable"_wref.get()) > 0) {
            SetDisabled(GI_Main::ParseEnabledNameGI(Block->GetParam(u"Disable"_wref.get())));
        }
        if (Block->CountParams(u"Down"_wref.get()) > 0) {
            SetDown(GI_Main::ParseEnabledNameGI(Block->GetParam(u"Down"_wref.get())));
        }
        if (Block->CountParams(u"UpOnlyDown"_wref.get()) > 0) {
            UpOnlyDown = GI_Main::ParseEnabledNameGI(Block->GetParam(u"UpOnlyDown"_wref.get()));
        }
        if (Block->CountParams(u"SoundEnter"_wref.get()) > 0) {
            EnterSound = Block->GetParam(u"SoundEnter"_wref.get());
        }
        if (Block->CountParams(u"SoundLeave"_wref.get()) > 0) {
            LeaveSound = Block->GetParam(u"SoundLeave"_wref.get());
        }
        if (Block->CountParams(u"SoundClick"_wref.get()) > 0) {
            ClickSound = Block->GetParam(u"SoundClick"_wref.get());
        }
        if (Block->CountParams(u"CaptionSme"_wref.get()) > 0) {
            CaptionOffsets = GI_Main::GetRectGI(Block->GetParam(u"CaptionSme"_wref.get()));
        }
        if (Block->CountBlocks(u"OnPressCode"_wref.get()) > 0) {
            OnPressCode = Block->GetBlock(u"OnPressCode"_wref.get());
        }
        UpdateStateImagePlacement();
    }

    void TGraphButtonGI::UpdateAutoGeometry() {
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect ImageBounds{};
        GI_MessageLoop::TObjectGI::UpdateAutoGeometry();
        if (ImageAutoUpdateFlags != 0) {
            Bounds.Left = 0;
            Bounds.Top = 0;
            Bounds.Right = 0;
            Bounds.Bottom = 0;
            if (ImageNormal != nullptr) {
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0), ImageNormal->GetContentOrigin());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), ImageNormal->GetContentSize());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8))));
                Bounds = ImageBounds;
            }
            if (ImageNormalActive != nullptr) {
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0), ImageNormalActive->GetContentOrigin());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), ImageNormalActive->GetContentSize());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8))));
                if (Bounds.Right - Bounds.Left < 1) {
                    Bounds = ImageBounds;
                } else {
                    WindowsSdk::UnionRect(Bounds, Bounds, ImageBounds);
                }
            }
            if (ImageDown != nullptr) {
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0), ImageDown->GetContentOrigin());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), ImageDown->GetContentSize());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8))));
                if (Bounds.Right - Bounds.Left < 1) {
                    Bounds = ImageBounds;
                } else {
                    WindowsSdk::UnionRect(Bounds, Bounds, ImageBounds);
                }
            }
            if (ImageDownActive != nullptr) {
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0), ImageDownActive->GetContentOrigin());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), ImageDownActive->GetContentSize());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8))));
                if (Bounds.Right - Bounds.Left < 1) {
                    Bounds = ImageBounds;
                } else {
                    WindowsSdk::UnionRect(Bounds, Bounds, ImageBounds);
                }
            }
            if (ImageDisabled != nullptr) {
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0), ImageDisabled->GetContentOrigin());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), ImageDisabled->GetContentSize());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8))));
                if (Bounds.Right - Bounds.Left < 1) {
                    Bounds = ImageBounds;
                } else {
                    WindowsSdk::UnionRect(Bounds, Bounds, ImageBounds);
                }
            }
            if (ImageDisabledActive != nullptr) {
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0), ImageDisabledActive->GetContentOrigin());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), ImageDisabledActive->GetContentSize());
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 0)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&ImageBounds, 8))));
                if (Bounds.Right - Bounds.Left < 1) {
                    Bounds = ImageBounds;
                } else {
                    WindowsSdk::UnionRect(Bounds, Bounds, ImageBounds);
                }
            }
            if ((ImageAutoUpdateFlags & GI_Main::agfPosition) == GI_Main::agfPosition) {
                SetPosition(Parent->ToLocalPoint(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Bounds, 0))));
            }
            if ((ImageAutoUpdateFlags & GI_Main::agfSize) == GI_Main::agfSize) {
                SetSize(EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Bounds, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Bounds, 0))));
            }
        }
        GI_MessageLoop::TObjectGI::UpdateAutoGeometry();
    }

    void TGraphButtonGI::p_destroy() {
        GI_GraphButton::TGraphButtonGI_Destroy(this);
    }

} // namespace GI_GraphButton
