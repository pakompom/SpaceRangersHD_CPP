#include "layout/fListBox.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Frame.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_PanelScrollBar.hpp"
#include "units/GI_Window.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/fListBox.hpp"

namespace fListBox {
    std::uint32_t ShowListDialog(GI_MessageLoop::TMessageLoopGI* Parent, std::int32_t& SelectedIndex, pas::WideString Caption, pas::List* Items, std::int32_t OffsetX, std::int32_t OffsetY) {
        std::uint32_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        Parent->RootUiObject->NativeHook50();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        GR_Main::CaptureScreenBackground(false, 0);
        TfListBox* Dialog = pas::construct_call<TfListBox>(TfListBox_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeDefaults();
        Dialog->Items = Items;
        Dialog->Caption = std::move(Caption);
        Dialog->OffsetX = OffsetX;
        Dialog->OffsetY = OffsetY;
        {
            try {
                Result = Dialog->Run();
                SelectedIndex = Dialog->SelectedIndex;
                Parent->InvalidateViewport();
            } catch (...) {
                Dialog->Caption = pas::WideString();
                Dialog->Items = nullptr;
                Parent->ChildLoop = nullptr;
                pas::free(Dialog);
                throw;
            }
            Dialog->Caption = pas::WideString();
            Dialog->Items = nullptr;
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

    void TfListBox_Create(TfListBox* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
    }

    void TfListBox_Destroy(TfListBox* Self) {
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfListBox::OnOpen() {
        WindowsSdk::TPoint Size{};
        WindowsSdk::TRect Insets{};
        SelectedIndex = -1;
        SelectedControl = nullptr;
        GI_Window::TWindowGI* Window = pas::construct_call<GI_Window::TWindowGI>(GI_Window::TWindowGI_Create, ContentPanel);
        Window->SetDepth(1.0);
        Window->SetConfigPath(pas::concat_wide({u"Style.Window.", GR_Main::GiResourceSuffix(), u"MessageBox"}));
        Window->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(250), GR_Main::GiScalePixels(400)));
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
            cpp_with->UpCallback = pas::bind_method<&TfListBox::AcceptClicked>(this);
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
        CancelButton->UpCallback = pas::bind_method<&TfListBox::CancelClicked>(this);
        GI_Label::TLabelGI* CaptionLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ContentPanel);
        Window->SetDepth(2.0);
        CaptionLabel->SetText(Caption);
        CaptionLabel->SetSize(ClassesImports::Point(Size.X, GR_Main::GiScalePixels(21)));
        CaptionLabel->SetPosition(ClassesImports::Point(0, GR_Main::GiScalePixels(30)));
        CaptionLabel->SetTextAlignX(GI_Main::taxCenter);
        CaptionLabel->SetTextAlignY(GI_Main::tayCenter);
        CaptionLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        CaptionLabel->SetFontName(GlobalsV::NormalBoldFontName);
        ScrollPanel = pas::construct_call<GI_PanelScrollBar::TPanelScrollBarGI>(GI_PanelScrollBar::TPanelScrollBarGI_Create, Window);
        {
            GI_PanelScrollBar::TPanelScrollBarGI* cpp_with_2 = ScrollPanel;
            cpp_with_2->SetDepth(9.0);
            cpp_with_2->SetUnlimitedWorldEnabled(false);
            cpp_with_2->SetScrollbarsOutside(true);
            cpp_with_2->SetVerticalScrollBarConfigPath(pas::concat_wide({u"Style.ScrollBar.", GR_Main::GiResourceSuffix(), u"PQS2"}));
            cpp_with_2->SetHorizontalScrollbarEnabled(false);
            cpp_with_2->SetVerticalScrollbarEnabled(true);
            cpp_with_2->SetPosition(ClassesImports::Point(Insets.Left + GR_Main::GiScalePixels(10), CaptionLabel->LocalPosition.Y + CaptionLabel->ClientSize.Y + GR_Main::GiScalePixels(10)));
            cpp_with_2->SetSize(ClassesImports::Point(Size.X - Insets.Right - cpp_with_2->LocalPosition.X - cpp_with_2->VerticalScrollBar->ClientSize.X - GR_Main::GiScalePixels(10), AcceptButton->LocalPosition.Y - cpp_with_2->LocalPosition.Y - GR_Main::GiScalePixels(15)));
            cpp_with_2->ScrollAxis = GI_Panel::psaVertical;
        }
        {
            GI_Frame::TFrameGI* cpp_with_3 = pas::construct_call<GI_Frame::TFrameGI>(GI_Frame::TFrameGI_Create, Window);
            cpp_with_3->SetPosition(ClassesImports::Point(ScrollPanel->LocalPosition.X - 1, ScrollPanel->LocalPosition.Y - 1));
            cpp_with_3->SetSize(ClassesImports::Point(ScrollPanel->ClientSize.X - 1, ScrollPanel->ClientSize.Y + 2));
            cpp_with_3->SetDepth(8.0);
            cpp_with_3->SetKind(GI_Frame::fkRect);
            cpp_with_3->SetColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        }
        PopulateChoices();
        ViewportRect.Left = pas::shr(GR_Main::GameScreenWidth, 1) + OffsetX - Size.X / 2;
        ViewportRect.Top = pas::shr(GR_Main::GameScreenHeight, 1) + OffsetY - Size.Y / 2;
        ViewportRect.Right = pas::shr(GR_Main::GameScreenWidth, 1) + OffsetX + Size.X / 2;
        ViewportRect.Bottom = pas::shr(GR_Main::GameScreenHeight, 1) + OffsetY + Size.Y / 2;
        ContentPanel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&ViewportRect, 0)));
        ContentPanel->SetSize(Size);
        ContentPanel->UpdateAbsolutePosition();
        ContentPanel->UpdateSubtreeHitBounds();
    }

    void TfListBox::OnClose() {
    }

    void TfListBox::PopulateChoices() {
        std::int32_t I{};
        GI_Panel::TPanelGI* Row{};
        GI_Label::TLabelGI* LabelControl{};
        std::int32_t Y = 0;
        std::int32_t Count = pas::list_count(Items);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, ScrollPanel);
            Row->SetPositionModeW(true);
            Row->SetPosition(ClassesImports::Point(0, Y));
            Row->SetSize(ClassesImports::Point(ScrollPanel->ClientSize.X, GR_Main::GiScalePixels(15)));
            LabelControl = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
            LabelControl->SetPosition(ClassesImports::Point(0, 0));
            LabelControl->SetTextAlignX(GI_Main::taxLeft);
            LabelControl->SetTextAlignY(GI_Main::tayCenterEx);
            LabelControl->SetWordWrapEnabled(false);
            LabelControl->SetSize(Row->ClientSize);
            LabelControl->SetFontName(GlobalsV::NormalFontName);
            LabelControl->UserValue = I;
            LabelControl->SetText(*pas::list_at<pas::WideString>(Items, I));
            LabelControl->HelpText = LabelControl->GetText();
            LabelControl->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 200));
            LabelControl->LeftButtonDownCallback = pas::bind_method<&TfListBox::ChoiceMouseDown>(this);
            LabelControl->LeftButtonDoubleClickCallback = pas::bind_method<&TfListBox::ChoiceDoubleClick>(this);
            Y += Row->ClientSize.Y;
        }
        {
            GI_PanelScrollBar::TPanelScrollBarGI* cpp_with = ScrollPanel;
            cpp_with->UpdateScrollbarPlacement();
            cpp_with->UpdateScrollRanges();
            cpp_with->VerticalScrollBar->SetSmallChange(GR_Main::GiScalePixels(15));
            cpp_with->VerticalScrollBar->SetLargeChange(cpp_with->ClientSize.Y);
            cpp_with->VerticalScrollBar->SetPageSize(cpp_with->ClientSize.Y);
        }
    }

    void TfListBox::AcceptClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (SelectedControl != nullptr) {
            if (!AcceptButton->Disabled) {
                SelectedIndex = SelectedControl->UserValue;
                RequestClose(1);
            }
        }
    }

    void TfListBox::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        RequestClose(2);
    }

    void TfListBox::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_ScrollBar::TScrollBarGI* cpp_with = ScrollPanel->VerticalScrollBar;
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            cpp_with->SetPosition_2(cpp_with->Position - cpp_with->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            cpp_with->SetPosition_2(cpp_with->Position + cpp_with->SmallChange);
        }
    }

    void TfListBox::ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (SelectedControl != nullptr) {
            {
                std::uint32_t packRgbBytes = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 200);
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(SelectedControl);
                cpp_arg->SetTextColor(packRgbBytes);
            }
            SelectedControl = nullptr;
        }
        SelectedControl = Sender;
        {
            std::uint32_t packRgbBytes_2 = GR_Main::CurrentPixelFormat->PackRgbBytes(150, 150, 0);
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(SelectedControl);
            cpp_arg_2->SetTextColor(packRgbBytes_2);
        }
    }

    void TfListBox::ChoiceDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        AcceptClicked(nullptr);
    }

    void TfListBox::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0) {
            if (ExitCode == 0) {
                RequestClose(255);
            }
        }
    }

    void TfListBox::p_destroy() {
        fListBox::TfListBox_Destroy(this);
    }

} // namespace fListBox
