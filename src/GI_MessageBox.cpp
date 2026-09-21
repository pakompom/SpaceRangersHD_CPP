#include "layout/GI_MessageBox.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Window.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"

// Native routines; dialog VMT and managed-field table precede OnOpen.
namespace GI_MessageBox {
    std::uint32_t ShowMessageBoxGI(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& Text, std::uint32_t Options, std::int32_t UnusedOption, std::int32_t OffsetX, std::int32_t OffsetY) {
        std::uint32_t Result{};
        GI_MessageLoop::TCursorStateGI CursorState{};
        Parent->RootUiObject->OnModalSuspend();
        Parent->CaptureCursorState(&CursorState);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        TMessageBoxGI* Dialog = pas::construct_call<TMessageBoxGI>(GI_MessageLoop::TMessageLoopGI_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeDefaults();
        {
            try {
                Dialog->MessageText = Text;
                Dialog->Options = Options;
                Dialog->UnusedOption = UnusedOption;
                Dialog->OffsetX = OffsetX;
                Dialog->OffsetY = OffsetY;
                Result = Dialog->Run();
                Parent->InvalidateViewport();
            } catch (...) {
                Parent->ChildLoop = nullptr;
                pas::free(Dialog);
                throw;
            }
            Parent->ChildLoop = nullptr;
            pas::free(Dialog);
        }
        Parent->RestoreCursorState(&CursorState);
        Parent->UpdateCursorPosition();
        Parent->RootUiObject->OnModalResume();
        if (Result == 254) {
            GI_Main::BreakUiMessage();
        }
        return Result;
    }

    void TMessageBoxGI::OnOpen() {
        GI_GraphButton::TGraphButtonGI* AcceptButton{};
        GI_GraphButton::TGraphButtonGI* CancelButton{};
        WindowsSdk::TPoint TextSize{};
        WindowsSdk::TPoint WindowSize{};
        WindowsSdk::TPoint CandidateSize{};
        float ActualRatio{};
        std::int32_t IconTop{};
        WindowsSdk::TRect Borders{};
        float VerticalFactor = 0.4f;
        float TargetRatio = 1.6178011f;
        std::int32_t BottomMargin = GR_Main::GiScalePixels(35);
        ContentPanel->KeyDownCallback = pas::bind_method<&TMessageBoxGI::DialogKeyDown>(this);
        GI_Window::TWindowGI* Window = pas::construct_call<GI_Window::TWindowGI>(GI_Window::TWindowGI_Create, ContentPanel);
        Window->SetDepth(1.0);
        Window->SetConfigPath(pas::concat_wide({u"Style.Window.", GR_Main::GiResourceSuffix(), u"MessageBox"}));
        Borders = Window->WorkSubRect;
        CandidateSize = Window->AlignSizeToBorderTiles(ClassesImports::Point(0, 0));
        GI_Image::TImageGI* Light = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, ContentPanel);
        if ((Options & mbgWarning) == mbgWarning) {
            Light->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"LightRed"}));
        } else if ((Options & mbgQuestion) == mbgQuestion) {
            Light->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"LightBlue"}));
        } else if ((Options & mbgError) == mbgError) {
            Light->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"LightRed"}));
        } else {
            Light->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"LightBlue"}));
        }
        Light->SetSize(Light->GetContentSize());
        GI_Image::TImageGI* Icon = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, ContentPanel);
        if ((Options & mbgWarning) == mbgWarning) {
            Icon->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"Warning"}));
        } else if ((Options & mbgQuestion) == mbgQuestion) {
            Icon->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"Question"}));
        } else if ((Options & mbgError) == mbgError) {
            Icon->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"Error"}));
        } else {
            Icon->SetImagePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"Info"}));
        }
        Icon->SetSize(Icon->GetContentSize());
        Icon->SetPosition(ClassesImports::Point(Borders.Left, Borders.Top));
        GI_Label::TLabelGI* TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ContentPanel);
        TextLabel->SetDepth(0.0);
        TextLabel->SetFontName(GlobalsV::NormalFontName);
        TextLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        TextLabel->SetText(EC_Str::ReplaceAllWideString(EC_Str::ReplaceAllWideString(MessageText, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv), u"<color=0,255,0>"_wref.get(), u"<color=255,255,0>"sv));
        std::int32_t Attempts = 100;
        while (Attempts > 0) {
            TextLabel->SetTextAlignX(GI_Main::taxCenter);
            TextLabel->SetTextAlignY(GI_Main::tayCenter);
            TextLabel->SetSize(ClassesImports::Point(CandidateSize.X - Borders.Left - Borders.Right - Icon->ClientSize.X - GR_Main::GiScalePixels(10), 1));
            TextLabel->SetWordWrapEnabled(true);
            TextLabel->SetTextAlignX(GI_Main::taxLeft);
            TextLabel->SetTextAlignY(GI_Main::tayAuto);
            TextSize = TextLabel->ClientSize;
            Window->SetPosition(ClassesImports::Point(0, 0));
            Window->SetSize(ClassesImports::Point(TextSize.X + Borders.Left + Borders.Right + Icon->ClientSize.X + GR_Main::GiScalePixels(10), TextSize.Y + Borders.Top + Borders.Bottom + BottomMargin));
            Window->UpdateAutoGeometry();
            WindowSize = Window->ClientSize;
            ActualRatio = pas::real_divide(WindowSize.X, WindowSize.Y);
            if (static_cast<long double>(ActualRatio) - TargetRatio < -0.1L) {
                CandidateSize.X += 10;
            } else if (static_cast<long double>(ActualRatio) - TargetRatio > 0.1L) {
                CandidateSize.Y += 10;
            } else {
                break;
            }
            --Attempts;
        }
        if ((Options & mbgOK) == mbgOK) {
            AcceptButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, ContentPanel);
            AcceptButton->EnterSound = u"Sound.ButtonEnter"_w;
            AcceptButton->LeaveSound = u"Sound.ButtonLeave"_w;
            AcceptButton->ClickSound = u"Sound.ButtonClick"_w;
            AcceptButton->UpOnlyDown = true;
            AcceptButton->SetDepth(0.0);
            AcceptButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkN"}));
            AcceptButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkA"}));
            AcceptButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkD"}));
            AcceptButton->SetSize(AcceptButton->GetMaxStateImageSize());
            if ((Options & mbgCancel) != mbgCancel) {
                AcceptButton->SetPosition(ClassesImports::Point(WindowSize.X / 2 - AcceptButton->ClientSize.X / 2, WindowSize.Y - Borders.Bottom - AcceptButton->ClientSize.Y));
            } else {
                AcceptButton->SetPosition(ClassesImports::Point(WindowSize.X / 2 - AcceptButton->ClientSize.X - GR_Main::GiScalePixels(5), WindowSize.Y - Borders.Bottom - AcceptButton->ClientSize.Y));
            }
            AcceptButton->HitKind = GI_GraphButton::gbhRect;
            AcceptButton->UpdateStateImagePlacement();
            AcceptButton->UpdateStateVisuals();
            AcceptButton->UpCallback = pas::bind_method<&TMessageBoxGI::AcceptClick>(this);
        }
        if ((Options & mbgCancel) == mbgCancel) {
            CancelButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, ContentPanel);
            CancelButton->EnterSound = u"Sound.ButtonEnter"_w;
            CancelButton->LeaveSound = u"Sound.ButtonLeave"_w;
            CancelButton->ClickSound = u"Sound.ButtonClick"_w;
            CancelButton->UpOnlyDown = true;
            CancelButton->SetDepth(0.0);
            CancelButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"CancelN"}));
            CancelButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"CancelA"}));
            CancelButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"CancelD"}));
            CancelButton->SetSize(CancelButton->GetMaxStateImageSize());
            if ((Options & mbgOK) != mbgOK) {
                CancelButton->SetPosition(ClassesImports::Point(WindowSize.X / 2 - CancelButton->ClientSize.X / 2, WindowSize.Y - Borders.Bottom - CancelButton->ClientSize.Y));
            } else {
                CancelButton->SetPosition(ClassesImports::Point(WindowSize.X / 2 + GR_Main::GiScalePixels(5), WindowSize.Y - Borders.Bottom - CancelButton->ClientSize.Y));
            }
            CancelButton->HitKind = GI_GraphButton::gbhRect;
            CancelButton->UpdateStateImagePlacement();
            CancelButton->UpdateStateVisuals();
            CancelButton->UpCallback = pas::bind_method<&TMessageBoxGI::CancelClick>(this);
        }
        ViewportRect.Left = pas::shr(GR_Main::GameScreenWidth, 1) + OffsetX - WindowSize.X / 2;
        ViewportRect.Top = pas::shr(GR_Main::GameScreenHeight, 1) + OffsetY - WindowSize.Y / 2;
        ViewportRect.Right = pas::shr(GR_Main::GameScreenWidth, 1) + OffsetX + WindowSize.X / 2;
        ViewportRect.Bottom = pas::shr(GR_Main::GameScreenHeight, 1) + OffsetY + WindowSize.Y / 2;
        ContentPanel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&ViewportRect, 0)));
        ContentPanel->SetSize(WindowSize);
        ContentPanel->UpdateAbsolutePosition();
        ContentPanel->UpdateSubtreeHitBounds();
        if ((Options & mbgLeftAlign) == mbgLeftAlign) {
            TextLabel->SetTextAlignX(GI_Main::taxLeft);
        } else {
            TextLabel->SetTextAlignX(GI_Main::taxCenter);
        }
        TextLabel->SetTextAlignY(GI_Main::tayAuto);
        TextLabel->SetPosition(ClassesImports::Point(Icon->ClientSize.X + Borders.Left + GR_Main::GiScalePixels(10), static_cast<std::int32_t>(System::Round(static_cast<long double>(WindowSize.Y - Borders.Top - Borders.Bottom - BottomMargin - TextLabel->ClientSize.Y) * VerticalFactor)) + Borders.Top));
        Icon->SetPosition(ClassesImports::Point(Icon->LocalPosition.X, TextLabel->LocalPosition.Y));
        if (Icon->LocalPosition.Y + Icon->ClientSize.Y > TextLabel->LocalPosition.Y + TextLabel->ClientSize.Y) {
            if (TextLabel->LocalPosition.Y - (Icon->ClientSize.Y / 2 - TextLabel->ClientSize.Y / 2) < Borders.Top) {
                IconTop = Borders.Top;
            } else {
                IconTop = TextLabel->LocalPosition.Y - (Icon->ClientSize.Y / 2 - TextLabel->ClientSize.Y / 2);
            }
            Icon->SetPosition(ClassesImports::Point(Icon->LocalPosition.X, IconTop));
        }
        Light->SetPosition(ClassesImports::Point(WindowSize.X / 2 - Light->ClientSize.X / 2, 0));
        TextSize = TextLabel->MeasureContentSize(nullptr);
    }

    void TMessageBoxGI::AcceptClick(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(mbgResultOK);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TMessageBoxGI::CancelClick(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(mbgResultCancel);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TMessageBoxGI::DialogKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        if ((Options & mbgCancel) == mbgCancel && (VirtualKey == WindowsSdk::VK_ESCAPE || VirtualKey == 'N' || VirtualKey == WindowsSdk::VK_RETURN && (Options & mbgOK) != mbgOK)) {
            CancelClick(Sender);
        } else if ((Options & mbgOK) == mbgOK && (VirtualKey == WindowsSdk::VK_RETURN || VirtualKey == 'Y')) {
            AcceptClick(Sender);
        }
    }

    void TMessageBoxGI::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(255);
        }
    }

} // namespace GI_MessageBox
