#include "layout/fCount1.hpp"
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
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/System.hpp"
#include "units/fCount1.hpp"

namespace fCount1 {
    // Native TfCount1 VMT confirms the inferred unit ownership.
    // Borrows optional PWideString choices. Maximum bounds the slider; Limit bounds acceptance and the Max button.
    std::uint32_t ShowNumberDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ImagePath, const pas::WideString& KindImagePath, const pas::WideString& Caption, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Limit, pas::List* Items, std::int32_t& Value) {
        std::uint32_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        Parent->RootUiObject->NativeHook50();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        GR_Main::CaptureScreenBackground(false, 0);
        TfCount1* Dialog = pas::construct_call<TfCount1>(TfCount1_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeFromConfig(GR_Main::UiStyleConfig, u"Number"_wref.get(), true);
        Dialog->InitializeLayout();
        {
            try {
                Dialog->ImagePath = ImagePath;
                Dialog->KindImagePath = KindImagePath;
                Dialog->Caption = Caption;
                Dialog->Minimum = Minimum;
                Dialog->Maximum = Maximum;
                Dialog->Limit = Limit;
                Dialog->Value = Value;
                Dialog->Items = Items;
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
        return Result;
    }

    void TfCount1_Create(TfCount1* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
    }

    void TfCount1_Destroy(TfCount1* Self) {
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfCount1::InitializeLayout() {
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* Ok_Parent = MainPanel->FindByNameRecursive(u"Ok"_wref.get())->Parent;
                Ok_Parent->SetPosition(ClassesImports::Point(Ok_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Ok_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
    }

    void TfCount1::OnOpen() {
        Dragging = false;
        {
            GI_Image::TImageGI* ItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ItemImage"_wref.get()));
            ItemImage->SetImagePath(ImagePath);
            ItemImage->SetImageKindX(GI_Main::ikxCenter);
            ItemImage->SetImageKindY(GI_Main::ikyCenter);
            ItemImage->SetActive(true);
        }
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Caption"_wref.get()))->SetText(Caption);
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        {
            GI_GraphButton::TGraphButtonGI* Add = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Add"_wref.get()));
            Add->DownCallback = pas::bind_method<&TfCount1::AddPressed>(this);
            Add->UpCallback = pas::bind_method<&TfCount1::AddReleased>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Sub = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Sub"_wref.get()));
            Sub->DownCallback = pas::bind_method<&TfCount1::SubPressed>(this);
            Sub->UpCallback = pas::bind_method<&TfCount1::SubReleased>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Max"_wref.get()))->UpCallback = pas::bind_method<&TfCount1::MaxClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"_wref.get()))->UpCallback = pas::bind_method<&TfCount1::AcceptClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()))->UpCallback = pas::bind_method<&TfCount1::CancelClicked>(this);
        GetByName(u"PanelBar"_wref.get())->LeftButtonDownCallback = pas::bind_method<&TfCount1::BarMouseDown>(this);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->MouseMoveCallback = pas::bind_method<&TfCount1::MainMouseMove>(this);
            MainPanel->LeftButtonUpCallback = pas::bind_method<&TfCount1::MainMouseUp>(this);
            MainPanel->KeyDownCallback = pas::bind_method<&TfCount1::MainKeyDown>(this);
        }
        if (KindImagePath != u"") {
            {
                GI_Image::TImageGI* Kind0 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Kind0"_wref.get()));
                Kind0->SetImagePath(KindImagePath);
                Kind0->SetActive(true);
            }
            GetByName(u"Count"_wref.get())->SetActive(true);
            GetByName(u"Kind1"_wref.get())->SetActive(false);
            GetByName(u"Count2"_wref.get())->SetActive(false);
        } else {
            GetByName(u"Kind1"_wref.get())->SetActive(true);
            GetByName(u"Count2"_wref.get())->SetActive(true);
            GetByName(u"Count"_wref.get())->SetActive(false);
            GetByName(u"Kind0"_wref.get())->SetActive(false);
        }
        RefreshValue();
    }

    void TfCount1::OnClose() {
        if (RepeatTimer != nullptr) {
            CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TfCount1::RefreshValue() {
        std::int32_t Position{};
        std::int32_t Width = GetByName(u"BarRange"_wref.get())->ClientSize.X + 2;
        if (Maximum - Minimum <= 0) {
            Position = Width - 1;
        } else {
            Position = System::Round(pas::real_divide(Value - Minimum, Maximum - Minimum) * (Width - 1));
        }
        {
            GI_MessageLoop::TObjectGI* Bar = GetByName(u"Bar"_wref.get());
            Bar->SetPosition(ClassesImports::Point(Position - 1 - Bar->ClientSize.X / 2, 0));
        }
        {
            GI_MessageLoop::TObjectGI* BarArrow = GetByName(u"BarArrow"_wref.get());
            BarArrow->SetPosition(ClassesImports::Point(Position + 3, 0));
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"_wref.get()))->SetDisabled(Value > Limit);
        if (Value <= Limit) {
            {
                std::uint32_t packRgbBytes = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"_wref.get()));
                cpp_arg->SetTextColor(packRgbBytes);
            }
            {
                std::uint32_t packRgbBytes_2 = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"_wref.get()));
                cpp_arg_2->SetTextColor(packRgbBytes_2);
            }
        } else {
            {
                std::uint32_t packRgbBytes_3 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"_wref.get()));
                cpp_arg_3->SetTextColor(packRgbBytes_3);
            }
            {
                std::uint32_t packRgbBytes_4 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"_wref.get()));
                cpp_arg_4->SetTextColor(packRgbBytes_4);
            }
        }
        if (Items != nullptr) {
            {
                auto cpp_arg_5 = pas::borrow(*pas::list_at<pas::WideString>(Items, Value - Minimum));
                GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"_wref.get()));
                cpp_arg_6->SetText(cpp_arg_5.get());
            }
            {
                auto cpp_arg_7 = pas::borrow(*pas::list_at<pas::WideString>(Items, Value - Minimum));
                GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"_wref.get()));
                cpp_arg_8->SetText(cpp_arg_7.get());
            }
        } else {
            {
                const pas::WideString& intToStr = pas::wide_int_to_str(Value);
                GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count"_wref.get()));
                cpp_arg_9->SetText(intToStr);
            }
            {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(Value);
                GI_Label::TLabelGI* cpp_arg_10 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Count2"_wref.get()));
                cpp_arg_10->SetText(intToStr_2);
            }
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Add"_wref.get()))->SetDisabled(Value == Maximum);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Sub"_wref.get()))->SetDisabled(Value == Minimum);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Max"_wref.get()))->SetDisabled(Value == std::min<std::int32_t>(Limit, Maximum));
    }

    void TfCount1::AddPressed(GI_MessageLoop::TObjectGI* Sender) {
        if (!Dragging) {
            if (Value < Maximum) {
                ++Value;
            }
            RefreshValue();
            if (RepeatTimer != nullptr) {
                CancelCallbackTimer(RepeatTimer);
                RepeatTimer = nullptr;
            }
            RepeatTimer = ScheduleCallbackTimer(300, 50, pas::bind_method<&TfCount1::RepeatChange>(this), 1);
            RepeatCount = 0u;
        }
    }

    void TfCount1::SubPressed(GI_MessageLoop::TObjectGI* Sender) {
        if (!Dragging) {
            if (Value > Minimum) {
                --Value;
            }
            RefreshValue();
            if (RepeatTimer != nullptr) {
                CancelCallbackTimer(RepeatTimer);
                RepeatTimer = nullptr;
            }
            RepeatTimer = ScheduleCallbackTimer(300, 50, pas::bind_method<&TfCount1::RepeatChange>(this), 0);
            RepeatCount = 0u;
        }
    }

    void TfCount1::AddReleased(GI_MessageLoop::TObjectGI* Sender) {
        if (RepeatTimer != nullptr) {
            CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TfCount1::SubReleased(GI_MessageLoop::TObjectGI* Sender) {
        if (RepeatTimer != nullptr) {
            CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TfCount1::RepeatChange(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t Data) {
        std::uint32_t Step = System::Round(System::Exp(RepeatCount * 0.1L));
        if (static_cast<std::uint32_t>(Maximum - Minimum) / 10 < Step) {
            Step = (Maximum - Minimum) / 10;
        } else {
            ++RepeatCount;
        }
        Step = std::max<std::int64_t>(static_cast<std::int64_t>(1), static_cast<std::int64_t>(Step));
        if (static_cast<std::uint32_t>(Data) > 0) {
            Value += std::min<std::int64_t>(static_cast<std::int64_t>(Maximum - Value), static_cast<std::int64_t>(Step));
        } else {
            Value -= std::min<std::int64_t>(static_cast<std::int64_t>(Value - Minimum), static_cast<std::int64_t>(Step));
        }
        RefreshValue();
    }

    void TfCount1::MaxClicked(GI_MessageLoop::TObjectGI* Sender) {
        Value = std::min<std::int32_t>(Limit, Maximum);
        RefreshValue();
    }

    void TfCount1::AcceptClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(1);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfCount1::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(2);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfCount1::BarMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        Dragging = true;
        MainMouseMove(Sender, KeyState, Point);
    }

    void TfCount1::MainMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        Dragging = false;
    }

    void TfCount1::MainMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t X{};
        std::int32_t Width{};
        if (Dragging) {
            X = GetByName(u"BarRange"_wref.get())->ToLocalPoint(GetCursorPoint()).X;
            Width = GetByName(u"BarRange"_wref.get())->ClientSize.X + 2;
            Value = System::Round(pas::real_min<pas::Extended>(1.0L, pas::real_max<pas::Extended>(0.0L, pas::real_divide(X - 1, Width - 1))) * (Maximum - Minimum) + Minimum);
            if (Value < Minimum) {
                Value = Minimum;
            } else if (Value > Maximum) {
                Value = Maximum;
            }
            RefreshValue();
        }
    }

    void TfCount1::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == -WindowsSdk::WHEEL_DELTA) {
            if (Value < Maximum) {
                ++Value;
            }
            RefreshValue();
        } else if (Delta == WindowsSdk::WHEEL_DELTA) {
            if (Value > Minimum) {
                --Value;
            }
            RefreshValue();
        }
    }

    void TfCount1::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0) {
            if (ExitCode == 0) {
                RequestClose(255);
            }
        }
    }

    void TfCount1::MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_LEFT) {
            if (Value > Minimum) {
                --Value;
            }
            RefreshValue();
        } else if (Key == WindowsSdk::VK_RIGHT) {
            if (Value < Maximum) {
                ++Value;
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

    void TfCount1::p_destroy() {
        fCount1::TfCount1_Destroy(this);
    }

} // namespace fCount1
