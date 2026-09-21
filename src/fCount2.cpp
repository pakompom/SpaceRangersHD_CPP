#include "layout/fCount2.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Main.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/fCount2.hpp"

namespace fCount2 {
    // Native TfCount2 VMT confirms the inferred unit ownership.
    std::uint32_t ShowCountDialogWithFont(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ImagePath, const pas::WideString& Description, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Limit, float UnitValue, std::int32_t Available, std::int32_t TotalLimit, std::int32_t& Value, pas::WideString PreviewImagePath, pas::WideString FontName) {
        std::uint32_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        Parent->RootUiObject->OnModalSuspend();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        GR_Main::CaptureScreenBackground(false, 0);
        TfCount2* Dialog = pas::construct_call<TfCount2>(TfCount2_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeFromConfig(GR_Main::UiStyleConfig, u"Count"_wref.get(), true);
        Dialog->InitializeLayout();
        {
            try {
                Dialog->ImagePath = ImagePath;
                Dialog->PreviewImagePath = std::move(PreviewImagePath);
                Dialog->Description = Description;
                Dialog->Minimum = Minimum;
                Dialog->Maximum = Maximum;
                Dialog->Limit = Limit;
                Dialog->Value = Value;
                Dialog->UnitValue = UnitValue;
                Dialog->TotalLimit = TotalLimit;
                Dialog->Available = Available;
                Dialog->FontName = std::move(FontName);
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
        Parent->RootUiObject->OnModalResume();
        return Result;
    }

    std::uint32_t ShowCountDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ImagePath, const pas::WideString& Description, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Limit, float UnitValue, std::int32_t Available, std::int32_t TotalLimit, std::int32_t& Value) {
        return fCount2::ShowCountDialogWithFont(Parent, ImagePath, Description, Minimum, Maximum, Limit, UnitValue, Available, TotalLimit, Value, pas::WideString(), GlobalsV::NormalFontName);
    }

    void TfCount2_Create(TfCount2* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
    }

    void TfCount2_Destroy(TfCount2* Self) {
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfCount2::InitializeLayout() {
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* Ok_Parent = MainPanel->FindByNameRecursive(u"Ok"sv)->Parent;
                Ok_Parent->SetPosition(ClassesImports::Point(Ok_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ok_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
    }

    void TfCount2::OnOpen() {
        Dragging = false;
        if (ImagePath != u"") {
            GI_Image::TImageGI* ItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ItemImage"sv));
            ItemImage->SetImagePath(ImagePath);
            ItemImage->SetImageKindX(GI_Main::ikxCenter);
            ItemImage->SetImageKindY(GI_Main::ikyCenter);
            ItemImage->SetActive(true);
        } else {
            GI_Image::TImageGI* ItemImage_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ItemImage"sv));
            ItemImage_2->SetActive(false);
        }
        if (PreviewImagePath != u"") {
            GI_GraphBuf::TGraphBufGI* ItemBuf = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"ItemBuf"sv));
            ItemBuf->SourceHasPerPixelAlpha = true;
            GI_GI::LoadGiByPathIntoGraphBuf(PreviewImagePath, ItemBuf->GraphBuf);
            if (static_cast<std::uint32_t>(ItemBuf->GraphBuf->Width) >= static_cast<std::uint32_t>(ItemBuf->GraphBuf->Height)) {
                ItemBuf->GraphBuf->RescaleRgba(ItemBuf->ClientSize.X, System::Round(pas::real_divide(ItemBuf->ClientSize.X, static_cast<std::uint32_t>(ItemBuf->GraphBuf->Width)) * static_cast<std::uint32_t>(ItemBuf->GraphBuf->Height)), 5);
            } else {
                ItemBuf->GraphBuf->RescaleRgba(System::Round(pas::real_divide(ItemBuf->ClientSize.Y, static_cast<std::uint32_t>(ItemBuf->GraphBuf->Height)) * static_cast<std::uint32_t>(ItemBuf->GraphBuf->Width)), ItemBuf->ClientSize.Y, 5);
            }
            ItemBuf->SetImageKindX(GI_Main::ikxCenter);
            ItemBuf->SetImageKindY(GI_Main::ikyCenter);
            ItemBuf->SetActive(true);
        } else {
            GI_GraphBuf::TGraphBufGI* ItemBuf_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"ItemBuf"sv));
            ItemBuf_2->SetActive(false);
        }
        {
            GI_Label::TLabelGI* Caption = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Caption"sv));
            Caption->SetText(Description);
            Caption->SetFontName(FontName);
        }
        {
            GI_GraphBuf::TGraphBufGI* BGBuf = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"sv));
            BGBuf->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        }
        {
            GI_GraphButton::TGraphButtonGI* Add = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Add"sv));
            Add->DownCallback = pas::bind_method<&TfCount2::IncreaseMouseDown>(this);
            Add->UpCallback = pas::bind_method<&TfCount2::IncreaseMouseUp>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Sub = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Sub"sv));
            Sub->DownCallback = pas::bind_method<&TfCount2::DecreaseMouseDown>(this);
            Sub->UpCallback = pas::bind_method<&TfCount2::DecreaseMouseUp>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Max"sv))->UpCallback = pas::bind_method<&TfCount2::MaximumClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->UpCallback = pas::bind_method<&TfCount2::AcceptClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"sv))->UpCallback = pas::bind_method<&TfCount2::CancelClicked>(this);
        GetByName(u"PanelBar"sv)->LeftButtonDownCallback = pas::bind_method<&TfCount2::SliderMouseDown>(this);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->MouseMoveCallback = pas::bind_method<&TfCount2::SliderMouseMove>(this);
            MainPanel->LeftButtonUpCallback = pas::bind_method<&TfCount2::SliderMouseUp>(this);
            MainPanel->KeyDownCallback = pas::bind_method<&TfCount2::MainPanelKeyDown>(this);
        }
        GetByName(u"Kind0"sv)->SetActive(UnitValue > 0.0L);
        GetByName(u"Count"sv)->SetActive(UnitValue > 0.0L);
        GetByName(u"Sum"sv)->SetActive(UnitValue > 0.0L);
        GetByName(u"Kind1"sv)->SetActive(UnitValue <= 0.0L);
        GetByName(u"Count2"sv)->SetActive(UnitValue <= 0.0L);
        RefreshValue();
    }

    void TfCount2::OnClose() {
        if (RepeatTimer != nullptr) {
            CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TfCount2::RefreshValue() {
        std::int32_t Position{};
        std::int32_t Width = GetByName(u"BarRange"sv)->ClientSize.X + 2;
        if (Maximum - Minimum <= 0) {
            Position = Width - 1;
        } else {
            Position = System::Round(pas::real_divide(Value, Maximum) * (Width - 1));
        }
        {
            GI_MessageLoop::TObjectGI* Bar = GetByName(u"Bar"sv);
            Bar->SetPosition(ClassesImports::Point(Position - 1 - Bar->ClientSize.X / 2, 0));
        }
        {
            GI_MessageLoop::TObjectGI* BarArrow = GetByName(u"BarArrow"sv);
            BarArrow->SetPosition(ClassesImports::Point(Position + 3, 0));
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->SetDisabled(Value > Limit);
        if (Value <= Available) {
            {
                std::uint32_t packRgbBytes = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"sv));
                cpp_arg->SetTextColor(packRgbBytes);
            }
            {
                std::uint32_t packRgbBytes_2 = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"sv));
                cpp_arg_2->SetTextColor(packRgbBytes_2);
            }
        } else {
            {
                std::uint32_t packRgbBytes_3 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"sv));
                cpp_arg_3->SetTextColor(packRgbBytes_3);
            }
            {
                std::uint32_t packRgbBytes_4 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"sv));
                cpp_arg_4->SetTextColor(packRgbBytes_4);
            }
        }
        if (static_cast<long double>(Value) * UnitValue <= TotalLimit) {
            std::uint32_t packRgbBytes_5 = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
            GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Sum"sv));
            cpp_arg_5->SetTextColor(packRgbBytes_5);
        } else {
            std::uint32_t packRgbBytes_6 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
            GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Sum"sv));
            cpp_arg_6->SetTextColor(packRgbBytes_6);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Value);
            GI_Label::TLabelGI* cpp_arg_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"sv));
            cpp_arg_7->SetText(intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Value);
            GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"sv));
            cpp_arg_8->SetText(intToStr_2);
        }
        {
            const pas::WideString& int64ToStr = pas::wide_int64_to_str(System::Round(static_cast<long double>(Value) * UnitValue));
            GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Sum"sv));
            cpp_arg_9->SetText(int64ToStr);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Add"sv))->SetDisabled(Value == Maximum);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Sub"sv))->SetDisabled(Value == Minimum);
        {
            GI_GraphButton::TGraphButtonGI* checked_cast = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Max"sv));
            checked_cast->SetDisabled(Value == std::min<std::int32_t>(Available, std::min<std::int32_t>(Limit, Maximum)));
        }
    }

    void TfCount2::IncreaseMouseDown(GI_MessageLoop::TObjectGI* Sender) {
        if (!Dragging) {
            ++Value;
            if (Value > Maximum) {
                Value = Maximum;
            }
            RefreshValue();
            if (RepeatTimer != nullptr) {
                CancelCallbackTimer(RepeatTimer);
                RepeatTimer = nullptr;
            }
            RepeatTimer = ScheduleCallbackTimer(300, 50, pas::bind_method<&TfCount2::RepeatChange>(this), 2);
        }
    }

    void TfCount2::DecreaseMouseDown(GI_MessageLoop::TObjectGI* Sender) {
        if (!Dragging) {
            --Value;
            if (Value < Minimum) {
                Value = Minimum;
            }
            RefreshValue();
            if (RepeatTimer != nullptr) {
                CancelCallbackTimer(RepeatTimer);
                RepeatTimer = nullptr;
            }
            RepeatTimer = ScheduleCallbackTimer(300, 50, pas::bind_method<&TfCount2::RepeatChange>(this), 0);
        }
    }

    void TfCount2::IncreaseMouseUp(GI_MessageLoop::TObjectGI* Sender) {
        if (RepeatTimer != nullptr) {
            CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TfCount2::DecreaseMouseUp(GI_MessageLoop::TObjectGI* Sender) {
        if (RepeatTimer != nullptr) {
            CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TfCount2::RepeatChange(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        Value = Value + UserData - 1;
        if (Value < Minimum) {
            Value = Minimum;
        } else if (Value > Maximum) {
            Value = Maximum;
        }
        RefreshValue();
    }

    void TfCount2::MaximumClicked(GI_MessageLoop::TObjectGI* Sender) {
        Value = std::min<std::int32_t>(Available, std::min<std::int32_t>(Limit, Maximum));
        RefreshValue();
    }

    void TfCount2::AcceptClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(1);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfCount2::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(2);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfCount2::SliderMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        Dragging = true;
        SliderMouseMove(Sender, KeyState, Point);
    }

    void TfCount2::SliderMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        Dragging = false;
    }

    void TfCount2::SliderMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t X{};
        std::int32_t Width{};
        if (Dragging) {
            X = GetByName(u"BarRange"sv)->ToLocalPoint(GetCursorPoint()).X;
            Width = GetByName(u"BarRange"sv)->ClientSize.X + 2;
            Value = System::Round(Maximum * pas::real_min<pas::Extended>(1.0L, pas::real_max<pas::Extended>(0.0L, pas::real_divide(X - 1, Width - 1))));
            if (Value < Minimum) {
                Value = Minimum;
            } else if (Value > Maximum) {
                Value = Maximum;
            }
            RefreshValue();
        }
    }

    void TfCount2::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == -WindowsSdk::WHEEL_DELTA) {
            ++Value;
            if (Value > Maximum) {
                Value = Maximum;
            }
            RefreshValue();
        } else if (Delta == WindowsSdk::WHEEL_DELTA) {
            --Value;
            if (Value < Minimum) {
                Value = Minimum;
            }
            RefreshValue();
        }
    }

    void TfCount2::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(255);
        }
    }

    void TfCount2::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_LEFT) {
            --Value;
            if (Value < Minimum) {
                Value = Minimum;
            }
            RefreshValue();
        } else if (Key == WindowsSdk::VK_RIGHT) {
            ++Value;
            if (Value > Maximum) {
                Value = Maximum;
            }
            RefreshValue();
        } else if (Key == WindowsSdk::VK_HOME) {
            Value = Minimum;
            RefreshValue();
        } else if (Key == WindowsSdk::VK_END) {
            Value = Maximum;
            RefreshValue();
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            CancelClicked(nullptr);
        } else if (Key == WindowsSdk::VK_RETURN && Value <= Limit) {
            AcceptClicked(nullptr);
        }
    }

    void TfCount2::p_destroy() {
        fCount2::TfCount2_Destroy(this);
    }

} // namespace fCount2
