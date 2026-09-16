#include "layout/fTextBox.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Edit.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Window.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/fTextBox.hpp"

namespace fTextBox {
    // Native TfTextBox VMT confirms the inferred unit ownership.
    // Runs the modal TfTextBox and copies its text back even on cancellation.
    std::uint32_t ShowTextInputDialog(GI_MessageLoop::TMessageLoopGI* Parent, pas::WideString Caption, pas::WideString& Value, std::int32_t MaximumLength, std::int32_t OffsetX, std::int32_t OffsetY) {
        std::uint32_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        Parent->RootUiObject->NativeHook50();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        GR_Main::CaptureScreenBackground(false, 0);
        TfTextBox* Dialog = pas::construct_call<TfTextBox>(TfTextBox_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeDefaults();
        {
            try {
                Dialog->MaximumLength = MaximumLength;
                Dialog->OffsetX = OffsetX;
                Dialog->OffsetY = OffsetY;
                Dialog->Caption = Caption;
                Dialog->Value = Value;
                Result = Dialog->Run();
                Value = Dialog->Value;
                Parent->InvalidateViewport();
            } catch (...) {
                Parent->ChildLoop = nullptr;
                pas::free(Dialog);
                throw;
            }
            Parent->ChildLoop = nullptr;
            pas::free(Dialog);
        }
        Parent->RestoreCursorState(&State);
        Parent->UpdateCursorPosition();
        Parent->RootUiObject->NativeHook48();
        if (Result == 254) {
            GI_Main::BreakUiMessage();
        }
        return Result;
    }

    void TfTextBox_Create(TfTextBox* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
    }

    void TfTextBox_Destroy(TfTextBox* Self) {
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfTextBox::OnOpen() {
        WindowsSdk::TPoint Size{};
        WindowsSdk::TRect Insets{};
        GI_Window::TWindowGI* Window = pas::construct_call<GI_Window::TWindowGI>(GI_Window::TWindowGI_Create, ContentPanel);
        Window->SetDepth(1.0);
        Window->SetConfigPath(pas::concat_wide({u"Style.Window.", GR_Main::GiResourceSuffix(), u"MessageBox"}));
        if (GR_Main::GiResourceVariant() == 1) {
            Window->SetSize(ClassesImports::Point(280, 80));
        } else {
            Window->SetSize(ClassesImports::Point(300, 100));
        }
        Window->UpdateAutoGeometry();
        Insets = Window->WorkSubRect;
        Size = Window->ClientSize;
        AcceptButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, ContentPanel);
        {
            GI_GraphButton::TGraphButtonGI* cpp_with = AcceptButton;
            cpp_with->EnterSound = u"Sound.ButtonEnter"_w;
            cpp_with->LeaveSound = u"Sound.ButtonLeave"_w;
            cpp_with->ClickSound = u"Sound.ButtonClick"_w;
            cpp_with->UpOnlyDown = true;
            cpp_with->SetDepth(0.0);
            cpp_with->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkN"}));
            cpp_with->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkA"}));
            cpp_with->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkD"}));
            cpp_with->SetSize(AcceptButton->GetMaxStateImageSize());
            cpp_with->SetPosition(ClassesImports::Point(Size.X / 2 - AcceptButton->ClientSize.X - GR_Main::GiScalePixels(5), Size.Y - Insets.Bottom - AcceptButton->ClientSize.Y));
            cpp_with->HitKind = GI_GraphButton::gbhRect;
            cpp_with->UpdateStateImagePlacement();
            cpp_with->UpdateStateVisuals();
            cpp_with->UpCallback = pas::bind_method<&TfTextBox::AcceptClicked>(this);
        }
        GI_GraphButton::TGraphButtonGI* CancelButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, ContentPanel);
        CancelButton->EnterSound = u"Sound.ButtonEnter"_w;
        CancelButton->LeaveSound = u"Sound.ButtonLeave"_w;
        CancelButton->ClickSound = u"Sound.ButtonClick"_w;
        CancelButton->UpOnlyDown = true;
        CancelButton->SetDepth(0.0);
        CancelButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"CancelN"}));
        CancelButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"CancelA"}));
        CancelButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"CancelD"}));
        CancelButton->SetSize(CancelButton->GetMaxStateImageSize());
        CancelButton->SetPosition(ClassesImports::Point(Size.X / 2 + GR_Main::GiScalePixels(5), Size.Y - Insets.Bottom - CancelButton->ClientSize.Y));
        CancelButton->HitKind = GI_GraphButton::gbhRect;
        CancelButton->UpdateStateImagePlacement();
        CancelButton->UpdateStateVisuals();
        CancelButton->UpCallback = pas::bind_method<&TfTextBox::CancelClicked>(this);
        GI_Label::TLabelGI* CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ContentPanel);
        if (GR_Main::GiResourceVariant() == 1) {
            CaptionLabel->SetPosition(ClassesImports::Point(25, 40));
            CaptionLabel->SetSize(ClassesImports::Point(500, 16));
        } else {
            CaptionLabel->SetPosition(ClassesImports::Point(25, 50));
            CaptionLabel->SetSize(ClassesImports::Point(500, 21));
        }
        CaptionLabel->SetTextAlignX(GI_Main::taxLeft);
        CaptionLabel->SetTextAlignY(GI_Main::tayCenter);
        CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        CaptionLabel->SetFontName(GlobalsV::NormalBoldFontName);
        CaptionLabel->SetText(EC_Str::ReplaceAllWideString(EC_Str::ReplaceAllWideString(Caption, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"_wref.get()), u"<color=0,255,0>"_wref.get(), u"<color=255,255,0>"_wref.get()));
        Edit = pas::construct_call<GI_Edit::TEditGI>(GI_Edit::TEditGI_Create, ContentPanel);
        {
            GI_Edit::TEditGI* cpp_with_2 = Edit;
            if (GR_Main::GiResourceVariant() == 1) {
                cpp_with_2->SetPosition(ClassesImports::Point(25, 60));
                cpp_with_2->SetSize(ClassesImports::Point(230, 19));
            } else {
                cpp_with_2->SetPosition(ClassesImports::Point(25, 75));
                cpp_with_2->SetSize(ClassesImports::Point(250, 21));
            }
            cpp_with_2->ClearFocusOnEnter = false;
            cpp_with_2->SetBorderEnabled(true);
            cpp_with_2->SetBorderLightColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 255));
            cpp_with_2->SetBorderDarkColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 255));
            cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            cpp_with_2->SetFontName(GlobalsV::NormalFontName);
            cpp_with_2->MaxLength = 30;
            cpp_with_2->SetText(Value);
            cpp_with_2->KeyDownCallback = pas::bind_method<&TfTextBox::EditKeyDown>(this);
            cpp_with_2->ChangedCallback = pas::bind_method<&TfTextBox::TextChanged>(this);
            cpp_with_2->MaxLength = MaximumLength;
            cpp_with_2->AutoScrollText = true;
        }
        ViewportRect.Left = pas::shr(GR_Main::GameScreenWidth, 1) + OffsetX - Size.X / 2;
        ViewportRect.Top = pas::shr(GR_Main::GameScreenHeight, 1) + OffsetY - Size.Y / 2;
        ViewportRect.Right = pas::shr(GR_Main::GameScreenWidth, 1) + OffsetX + Size.X / 2;
        ViewportRect.Bottom = pas::shr(GR_Main::GameScreenHeight, 1) + OffsetY + Size.Y / 2;
        ContentPanel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&ViewportRect, 0)));
        ContentPanel->SetSize(Size);
        ContentPanel->UpdateAbsolutePosition();
        ContentPanel->UpdateSubtreeHitBounds();
        FocusEdit();
    }

    void TfTextBox::OnClose() {
    }

    void TfTextBox::AcceptClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (!AcceptButton->Disabled) {
            Value = Edit->Text;
            RequestClose(1);
        }
    }

    void TfTextBox::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        RequestClose(2);
    }

    void TfTextBox::FocusEdit() {
        Edit->ProcessLeftButtonDown(0u, Edit->LocalPosition);
    }

    void TfTextBox::TextChanged(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t cpp_arg = static_cast<std::uint8_t>(IsValidText(Edit->Text) ^ 1);
        GI_GraphButton::TGraphButtonGI* acceptButton = AcceptButton;
        acceptButton->SetDisabled(cpp_arg);
    }

    std::uint8_t TfTextBox::IsValidText(pas::WideString Candidate) {
        std::int32_t I{};
        std::uint8_t Result = false;
        Candidate = EC_Str::TrimWideString(Candidate);
        if (Candidate.length() < 1) {
            return Result;
        }
        {
            GI_Edit::TEditGI* cpp_with = Edit;
            {
                const std::int32_t cpp_last = Candidate.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        if (!cpp_with->HasGlyph(Candidate.read(I + 1))) {
                            return Result;
                        }
                    }
                }
            }
        }
        return true;
    }

    void TfTextBox::EditKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
            if (Key == 'C') {
                GR_Main::SetClipboardWideText(Edit->Text);
            } else if (Key == 'V') {
                {
                    pas::WideString clipboardWideText = GR_Main::GetClipboardWideText();
                    GI_Edit::TEditGI* edit = Edit;
                    edit->SetText(std::move(clipboardWideText));
                }
                Edit->SetCaretPosition(Edit->Text.length());
            } else if (Key == WindowsSdk::VK_BACK) {
                Edit->SetText(pas::WideString());
            }
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            CancelClicked(nullptr);
        } else if (Key == WindowsSdk::VK_RETURN) {
            AcceptClicked(nullptr);
        }
    }

    void TfTextBox::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0) {
            if (ExitCode == 0) {
                RequestClose(255);
            }
        }
    }

    void TfTextBox::p_destroy() {
        fTextBox::TfTextBox_Destroy(this);
    }

} // namespace fTextBox
