#include "layout/fChameleon.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Window.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/fChameleon.hpp"

// Native TfChameleon VMT and callers establish this unit's ownership.
namespace fChameleon {
    std::uint32_t ShowChameleonDialog(GI_MessageLoop::TMessageLoopGI* Parent, std::int32_t BlazerCharges, std::int32_t KellerCharges, std::int32_t TerronCharges, aGalaxyStruct::TKlingType VisualType, std::uint8_t Active, std::int32_t& Choice) {
        std::uint32_t Result{};
        GI_MessageLoop::TCursorStateGI CursorState{};
        Parent->RootUiObject->OnModalSuspend();
        Parent->CaptureCursorState(&CursorState);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        TfChameleon* Dialog = pas::construct_call<TfChameleon>(GI_MessageLoop::TMessageLoopGI_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeDefaults();
        {
            try {
                Dialog->ChameleonActive = Active;
                Dialog->VisualType = VisualType;
                Dialog->Charges[aGalaxyStruct::dsBlazer] = BlazerCharges;
                Dialog->Charges[aGalaxyStruct::dsKeller] = KellerCharges;
                Dialog->Charges[aGalaxyStruct::dsTerron] = TerronCharges;
                Result = Dialog->Run();
                Choice = Dialog->Choice;
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

    void TfChameleon::OnOpen() {
        WindowsSdk::TPoint Size{};
        pas::WideString SeriesText{};
        pas::WideString NameText{};
        pas::WideString ShipName{};
        aGalaxyStruct::TDominatorSeries Series{};
        std::uint8_t Disabled{};
        WindowsSdk::TRect WorkRect{};
        // Nested OnOpen helper; does not access its parent frame.
        auto ChameleonChargeUnavailable = [&](std::int32_t Count) -> std::uint8_t {
            return !(Count > 0);
        };
        auto FormatChameleonChargeCount = [&](std::int32_t Count) -> pas::WideString {
            return pas::concat_wide({u" (", ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Count);
                pas::WideString localizedText = aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Count"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), pas::WideString(), u"<Count>"_w, std::move(intToStr));
            }()), u")"});
        };
        auto ChameleonSeriesColor = [&](aGalaxyStruct::TDominatorSeries Series) -> pas::WideString {
            pas::WideString Result{};
            switch (Series) {
                case aGalaxyStruct::dsBlazer: return aMyFunction::RedColorTag;
                case aGalaxyStruct::dsKeller: return aMyFunction::AzureColorTag;
                case aGalaxyStruct::dsTerron: return aMyFunction::DarkGreenColorTag;
                default: return Result;
            }
        };
        ContentPanel->KeyDownCallback = pas::bind_method<&TfChameleon::MainKeyDown>(this);
        GI_Window::TWindowGI* Window = pas::construct_call<GI_Window::TWindowGI>(GI_Window::TWindowGI_Create, ContentPanel);
        Window->SetDepth(1.0);
        Window->SetConfigPath(pas::concat_wide({u"Style.Window.", GR_Main::GiResourceSuffix(), u"MessageBox"}));
        Window->SetSize(ClassesImports::Point(280, 200));
        Window->UpdateAutoGeometry();
        WorkRect = Window->WorkSubRect;
        Size = Window->ClientSize;
        GI_GraphButton::TGraphButtonGI* AcceptButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, ContentPanel);
        AcceptButton->EnterSound = u"Sound.ButtonEnter"_w;
        AcceptButton->LeaveSound = u"Sound.ButtonLeave"_w;
        AcceptButton->ClickSound = u"Sound.ButtonClick"_w;
        AcceptButton->UpOnlyDown = true;
        AcceptButton->SetDepth(0.0);
        AcceptButton->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkN"}));
        AcceptButton->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkA"}));
        AcceptButton->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormMessageBox.", GR_Main::GiResourceSuffix(), u"OkD"}));
        AcceptButton->SetSize(AcceptButton->GetMaxStateImageSize());
        AcceptButton->SetPosition(ClassesImports::Point(Size.X / 2 - AcceptButton->ClientSize.X - GR_Main::GiScalePixels(5), Size.Y - WorkRect.Bottom - AcceptButton->ClientSize.Y));
        AcceptButton->HitKind = GI_GraphButton::gbhRect;
        AcceptButton->UpdateStateImagePlacement();
        AcceptButton->UpdateStateVisuals();
        AcceptButton->UpCallback = pas::bind_method<&TfChameleon::AcceptClicked>(this);
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
        CancelButton->SetPosition(ClassesImports::Point(Size.X / 2 + GR_Main::GiScalePixels(5), Size.Y - WorkRect.Bottom - CancelButton->ClientSize.Y));
        CancelButton->HitKind = GI_GraphButton::gbhRect;
        CancelButton->UpdateStateImagePlacement();
        CancelButton->UpdateStateVisuals();
        CancelButton->UpCallback = pas::bind_method<&TfChameleon::CancelClicked>(this);
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ContentPanel);
        Caption->SetDepth(0.0);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        ShipName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[aGalaxyStruct::dsBlazer], u".", pas::wide_int_to_str(static_cast<std::int32_t>(VisualType))}));
        Caption->SetText(pas::concat_wide({aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Name"_wref.get()), u" - ", aMyFunction::WrapTextInColor(pas::view(ShipName), pas::view(aMyFunction::DialogHighlightColorTag))}));
        Caption->SetTextAlignX(GI_Main::taxCenter);
        Caption->SetTextAlignY(GI_Main::tayAuto);
        Caption->SetPosition(ClassesImports::Point(0, WorkRect.Bottom));
        Caption->SetSize(ClassesImports::Point(Window->ClientSize.X, 1));
        std::int32_t Y = Caption->ClientSize.Y + 10;
        std::int32_t Index = 1;
        SeriesText = aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Series"_wref.get());
        Y += 30;
        {
            pas::WideString localizedText = aConst::LocalizedText(u"ShipInfo.AddInfo.Chameleon.Disable"_wref.get());
            std::uint8_t chameleonActive = ChameleonActive;
            std::uint8_t cpp_arg = static_cast<std::uint8_t>(ChameleonActive ^ 1);
            TfChameleon* self = this;
            self->AddChoice(Index, 20, Y, std::move(localizedText), chameleonActive, cpp_arg);
        }
        std::uint8_t NeedSelection = static_cast<std::uint8_t>(ChameleonActive ^ 1);
        std::uint8_t HasSelection = ChameleonActive;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(Series); ) {
            Y += 20;
            ++Index;
            NameText = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[Series], u".0"}));
            Disabled = ChameleonChargeUnavailable(Charges[Series]);
            {
                pas::WideString cpp_arg_2 = pas::concat_wide({SeriesText, u" ", aMyFunction::WrapTextInColor(pas::view(NameText), pas::view(ChameleonSeriesColor(Series))), FormatChameleonChargeCount(Charges[Series])});
                TfChameleon* self_2 = this;
                self_2->AddChoice(Index, 20, Y, std::move(cpp_arg_2), NeedSelection && static_cast<std::uint8_t>(Disabled ^ 1), Disabled);
            }
            if (NeedSelection && static_cast<std::uint8_t>(Disabled ^ 1)) {
                NeedSelection = false;
                HasSelection = true;
            }
        }
        AcceptButton->SetDisabled(static_cast<std::uint8_t>(HasSelection ^ 1));
        ViewportRect.Left = pas::shr(GR_Main::GameScreenWidth, 1) - Size.X / 2;
        ViewportRect.Top = pas::shr(GR_Main::GameScreenHeight, 1) - Size.Y / 2;
        ViewportRect.Right = pas::shr(GR_Main::GameScreenWidth, 1) + Size.X / 2;
        ViewportRect.Bottom = pas::shr(GR_Main::GameScreenHeight, 1) + Size.Y / 2;
        ContentPanel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&ViewportRect, 0)));
        ContentPanel->SetSize(Size);
        ContentPanel->UpdateAbsolutePosition();
        ContentPanel->UpdateSubtreeHitBounds();
    }

    void TfChameleon::AddChoice(std::int32_t Index, std::int32_t X, std::int32_t Y, pas::WideString Text, std::uint8_t Selected, std::uint8_t Disabled) {
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, ContentPanel);
        Image->SetName(u"ImgRadio"_wref.get());
        if (Disabled) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchH"}));
        } else if (!Selected) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"}));
            Choice = Index;
        }
        Image->SetPosition(ClassesImports::Point(X, Y));
        Image->SetSize(Image->GetContentSize());
        Image->SetImageKindY(GI_Main::ikyCenter);
        ChoiceImages[Index] = nullptr;
        if (!Disabled) {
            Image->LeftButtonDownCallback = pas::bind_method<&TfChameleon::ChoiceMouseDown>(this);
            Image->MouseEnterCallback = pas::bind_static_method<&TfChameleon::ChoiceMouseEnter>(this);
            Image->MouseLeaveCallback = pas::bind_static_method<&TfChameleon::ChoiceMouseLeave>(this);
            Image->UserValue = Index;
            ChoiceImages[Index] = Image;
        }
        std::int32_t Width = GR_Main::GiScalePixelsEx(300, 300);
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ContentPanel);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetPositionModeW(false);
        Caption->SetPosition(ClassesImports::Point(X + Image->GetContentSize().X, Y));
        Caption->SetSize(ClassesImports::Point(Width, 1));
        Caption->SetTextAlignX(GI_Main::taxLeft);
        Caption->SetTextAlignY(GI_Main::tayAuto);
        Caption->SetText(Text);
        if (Disabled) {
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(127, 127, 127));
        } else {
            Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        }
        if (!Disabled) {
            Caption->LeftButtonDownCallback = pas::bind_method<&TfChameleon::ChoiceMouseDown>(this);
            Caption->MouseEnterCallback = pas::bind_static_method<&TfChameleon::ChoiceMouseEnter>(this);
            Caption->MouseLeaveCallback = pas::bind_static_method<&TfChameleon::ChoiceMouseLeave>(this);
        }
        Caption->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Image));
    }

    void TfChameleon::ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (pas::class_cast_if<GI_Label::TLabelGI*>(Sender) != nullptr) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        GI_MessageLoop::TObjectGI* Child = ContentPanel->FirstChild;
        while (Child != nullptr) {
            if (Child->ControlName == Sender->ControlName && pas::class_cast_if<GI_Image::TImageGI*>(Child) != nullptr) {
                if (Child == Sender) {
                    {
                        GI_Image::TImageGI* cpp_arg = static_cast<GI_Image::TImageGI*>(Child);
                        pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchD"});
                        cpp_arg->SetImagePath(std::move(cpp_arg_2));
                    }
                    Choice = pas::checked_cast<GI_Image::TImageGI*>(Child)->UserValue;
                } else if (static_cast<GI_Image::TImageGI*>(Child)->UserValue != 0) {
                    GI_Image::TImageGI* cpp_arg_3 = static_cast<GI_Image::TImageGI*>(Child);
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
                    cpp_arg_3->SetImagePath(std::move(cpp_arg_4));
                }
            }
            Child = Child->NextSibling;
        }
        if (Point.X != -1000 || Point.Y != -1000) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        }
    }

    void TfChameleon::ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (!(pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr)) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        if (([&] {
            pas::WideString cpp_string = pas::checked_cast<GI_Image::TImageGI*>(Sender)->GetImagePath();
            pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
            return cpp_string == cpp_string_2;
        }())) {
            GI_Image::TImageGI* cpp_arg = static_cast<GI_Image::TImageGI*>(Sender);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchA"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
    }

    void TfChameleon::ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (!(pas::class_cast_if<GI_Image::TImageGI*>(Sender) != nullptr)) {
            Sender = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        }
        if (([&] {
            pas::WideString cpp_string = pas::checked_cast<GI_Image::TImageGI*>(Sender)->GetImagePath();
            pas::WideString cpp_string_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchA"});
            return cpp_string == cpp_string_2;
        }())) {
            GI_Image::TImageGI* cpp_arg = static_cast<GI_Image::TImageGI*>(Sender);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
    }

    void TfChameleon::MoveChoice(std::int32_t Delta) {
        std::int32_t Index{};
        auto Advance = [&]() -> void {
            Index += Delta;
            if (Index < 1) {
                Index = 4;
            } else if (Index > 4) {
                Index = 1;
            }
        };
        Index = Choice;
        Advance();
        while (ChoiceImages[Index] == nullptr && Index != Choice) {
            Advance();
        }
        if (ChoiceImages[Index] != nullptr) {
            ChoiceMouseDown(ChoiceImages[Index], 0u, ChoiceImages[Index]->LocalPosition);
        }
    }

    void TfChameleon::AcceptClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(1);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfChameleon::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(2);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfChameleon::MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_ESCAPE || Key == 'N') {
            CancelClicked(Sender);
        } else if (Key == WindowsSdk::VK_RETURN || Key == 'Y') {
            AcceptClicked(Sender);
        } else if (Key == WindowsSdk::VK_UP) {
            MoveChoice(-1);
        } else if (Key == WindowsSdk::VK_DOWN) {
            MoveChoice(1);
        }
    }

    void TfChameleon::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(255);
        }
    }

} // namespace fChameleon
