#include "layout/fSelectFace.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Edit.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/fSelectFace.hpp"

namespace fSelectFace {
    // Borrows the initialized SelectFaceScreen; returns True only for Run result 1. Caller supplies and reads its selection fields.
    std::uint8_t RunSelectFaceDialog(GI_MessageLoop::TMessageLoopGI* Parent) {
        GI_MessageLoop::TCursorStateGI State{};
        std::uint8_t Result = false;
        Parent->RootUiObject->OnModalSuspend();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        Globals::SelectFaceScreen->ParentLoop = Parent;
        Parent->ChildLoop = Globals::SelectFaceScreen;
        if (Globals::SelectFaceScreen->Run() == 1) {
            Result = true;
        }
        Globals::SelectFaceScreen->ParentLoop = nullptr;
        Parent->ChildLoop = nullptr;
        Parent->InvalidateViewport();
        Parent->RestoreCursorState(&State);
        Parent->UpdateCursorPosition();
        Parent->RootUiObject->OnModalResume();
        Parent->Present();
        GR_Main::PostMouseMoveMessage();
        return Result;
    }

    void TfSelectFace::InitializeLayout() {
        std::int32_t I{};
        pas::WideString Face{};
        aGalaxyStruct::TOwnerId Race{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range.next(Race); ) {
            I = -1;
            do {
                ++I;
                Face = pas::wide_int_to_str(I);
                if (I < 10) {
                    Face = pas::concat_wide({u"0", Face});
                }
            } while (!(([&] {
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"StyleFace", aConst::OwnerInfo[aConst::RaceToOwner(Race)].InternalName}));
                const pas::WideString& face = Face;
                return blockByPath->CountParams(face);
            }()) <= 0));
            LastPortraitByRace[Race] = I - 1;
        }
        GR_Main::AppendLogTextThreadSafe("fSelectFace... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* PlayerName_Parent = MainPanel->FindByNameRecursive(u"PlayerName"sv)->Parent;
                PlayerName_Parent->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GI_MessageLoop::TObjectGI* Control = FindControlByPath(u"SubPanel"_wref.get());
        if (Control != nullptr && GR_Main::ExtraScreenHeight < 0) {
            Control->SetPosition(ClassesImports::Point(0, GR_Main::ExtraScreenHeight));
            Control->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight - GR_Main::ExtraScreenHeight));
        }
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfSelectFace::MainPanelKeyDown>(this);
        GetByName(u"MainPanel"sv)->LeftButtonDownCallback = pas::bind_method<&TfSelectFace::PlayerNameMouseDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"FaceLeft"sv))->DownCallback = pas::bind_method<&TfSelectFace::PreviousPortraitClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"FaceRight"sv))->DownCallback = pas::bind_method<&TfSelectFace::NextPortraitClicked>(this);
        for (I = 0; I <= 4; ++I) {
            Face = aConst::OwnerToSys(aConst::RaceToOwner(aConst::NumberToRace(I)));
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"Race", Face}))));
                cpp_with_3->DownCallback = pas::bind_method<&TfSelectFace::RaceClicked>(this);
                cpp_with_3->UpCallback = pas::bind_method<&TfSelectFace::RaceClicked>(this);
                cpp_with_3->UserValue = I;
            }
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->UpCallback = pas::bind_method<&TfSelectFace::ApplyClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Cancel"sv))->UpCallback = pas::bind_method<&TfSelectFace::CancelClicked>(this);
        {
            GI_Edit::TEditGI* PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            PlayerName->ChangedCallback = pas::bind_method<&TfSelectFace::PlayerNameChanged>(this);
            PlayerName->MaxLength = 13;
        }
    }

    void TfSelectFace::OnOpen() {
        GR_Main::CaptureScreenBackground(true, 0);
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"sv))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        SelectRace(GetByName(pas::view(pas::concat_wide({u"Race", aConst::OwnerToSys(aConst::RaceToOwner(PlayerRace))}))));
        pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->SetText(PlayerName);
        RefreshPact();
        PlayerNameEdited = false;
        RefreshPortrait();
    }

    void TfSelectFace::OnClose() {
    }

    void TfSelectFace::PlayerNameMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        SetFocusedControl(GetByName(u"PlayerName"sv));
    }

    void TfSelectFace::RefreshPortrait() {
        if (CaptainPortraitIndex < 0) {
            CaptainPortraitIndex = LastPortraitByRace[PlayerRace];
        } else if (CaptainPortraitIndex > LastPortraitByRace[PlayerRace]) {
            CaptainPortraitIndex = 0;
        }
        {
            GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"sv));
            if (LastPortraitByRace[PlayerRace] >= 0) {
                CaptainI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(PlayerRace)].InternalName, pas::wide_int_to_str(CaptainPortraitIndex), u"i"}));
                CaptainI->SetImageKindX(GI_Main::ikxCenter);
                CaptainI->SetImageKindY(GI_Main::ikyCenter);
                CaptainI->SetActive(true);
            } else {
                CaptainI->SetActive(false);
            }
        }
        {
            GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"sv));
            CaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            if (LastPortraitByRace[PlayerRace] >= 0) {
                CaptainA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(PlayerRace)].InternalName, pas::wide_int_to_str(CaptainPortraitIndex), u"a"}));
                CaptainA->SequenceIndex = 0;
                CaptainA->UpdateAutoGeometry();
                CaptainA->SetImageKindX(GI_Main::ikxCenter);
                CaptainA->SetImageKindY(GI_Main::ikyCenter);
                CaptainA->SetActive(true);
                CaptainA->RestartPlayback();
            } else {
                CaptainA->SetActive(false);
            }
        }
    }

    void TfSelectFace::PreviousPortraitClicked(GI_MessageLoop::TObjectGI* Sender) {
        --CaptainPortraitIndex;
        RefreshPortrait();
    }

    void TfSelectFace::NextPortraitClicked(GI_MessageLoop::TObjectGI* Sender) {
        ++CaptainPortraitIndex;
        RefreshPortrait();
    }

    void TfSelectFace::SelectRace(GI_MessageLoop::TObjectGI* Sender) {
        PlayerRace = static_cast<aGalaxyStruct::TOwnerId>(Sender->UserValue);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceMaloc"sv))->SetDown(PlayerRace == aGalaxyStruct::oiMaloc);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RacePeleng"sv))->SetDown(PlayerRace == aGalaxyStruct::oiPeleng);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RacePeople"sv))->SetDown(PlayerRace == aGalaxyStruct::oiHuman);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceFei"sv))->SetDown(PlayerRace == aGalaxyStruct::oiFeyan);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"RaceGaal"sv))->SetDown(PlayerRace == aGalaxyStruct::oiGaal);
    }

    void TfSelectFace::RaceClicked(GI_MessageLoop::TObjectGI* Sender) {
        SelectRace(Sender);
        CaptainPortraitIndex = 0;
        RefreshPortrait();
        SetFocusedControl(GetByName(u"PlayerName"sv));
        {
            GI_Edit::TEditGI* PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            PlayerName->SetCaretPosition(PlayerName->Text.length());
        }
        PlayerNameChanged(nullptr);
    }

    void TfSelectFace::ApplyClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        std::int32_t Cost = 0;
        switch (PlayerRace) {
            case aGalaxyStruct::oiMaloc: Cost = NationalityCosts[aGalaxyStruct::oiMaloc]; break;
            case aGalaxyStruct::oiPeleng: Cost = NationalityCosts[aGalaxyStruct::oiPeleng]; break;
            case aGalaxyStruct::oiHuman: Cost = NationalityCosts[aGalaxyStruct::oiHuman]; break;
            case aGalaxyStruct::oiFeyan: Cost = NationalityCosts[aGalaxyStruct::oiFeyan]; break;
            case aGalaxyStruct::oiGaal: Cost = NationalityCosts[aGalaxyStruct::oiGaal]; break;
        }
        if (Cost <= AvailableMoney) {
            AcceptedCost = Cost;
            Text = ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Cost);
                pas::WideString paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormRuins.PB.ChangeNationality.Confirm"_wref.get());
                return aMyFunction::FormatText1(std::move(paramByPathOrMarker), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }());
            if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                RequestClose(1);
            }
        } else {
            AcceptedCost = 0;
            Text = ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                pas::WideString paramByPathOrMarker_2 = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormRuins.PB.ChangeNationality.NoMoney"_wref.get());
                return aMyFunction::FormatText1(std::move(paramByPathOrMarker_2), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
            }());
            GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgWarning, 0, 0, 0);
        }
    }

    void TfSelectFace::CancelClicked(GI_MessageLoop::TObjectGI* Sender) {
        RequestClose(2);
    }

    void TfSelectFace::RefreshPact() {
        pas::WideString Text{};
        Text = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PactText"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<CurName>"_w, PlayerName, u"<color=247,148,29>"_w);
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Pact"sv))->SetText(Text);
    }

    void TfSelectFace::PlayerNameChanged(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender != nullptr) {
            PlayerNameEdited = true;
        }
        {
            std::uint8_t cpp_arg = static_cast<std::uint8_t>(ValidatePlayerName(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text) ^ 1);
            GI_GraphButton::TGraphButtonGI* cpp_arg_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv));
            cpp_arg_2->SetDisabled(cpp_arg);
        }
        PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv))->Text;
        RefreshPact();
    }

    std::uint8_t TfSelectFace::ValidatePlayerName(pas::WideString Name) {
        std::int32_t I{};
        std::uint8_t Result = false;
        Name = EC_Str::RemoveWideStringChars(pas::view(Name), u"<>{}"_w);
        {
            GI_Edit::TEditGI* PlayerName = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            if (PlayerName->Text != Name) {
                I = PlayerName->CaretPosition;
                PlayerName->SetText(Name);
                if (I > 0) {
                    PlayerName->SetCaretPosition(I - 1);
                } else {
                    PlayerName->SetCaretPosition(0);
                }
            }
        }
        Name = EC_Str::TrimWideString(Name);
        if (Name.length() < 1) {
            return Result;
        }
        {
            GI_Edit::TEditGI* PlayerName_2 = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"PlayerName"sv));
            {
                const std::int32_t cpp_last = Name.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        if (!PlayerName_2->HasGlyph(Name.read(I + 1))) {
                            return Result;
                        }
                    }
                }
            }
        }
        return true;
    }

    void TfSelectFace::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == WindowsSdk::VK_RETURN) {
                if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Ok"sv))->Disabled) {
                    ApplyClicked(nullptr);
                }
            } else if (Key == WindowsSdk::VK_ESCAPE) {
                CancelClicked(nullptr);
            }
        }
    }

    void TfSelectFace::SelectMusic() {
        GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
    }

    void TfSelectFace::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

} // namespace fSelectFace
