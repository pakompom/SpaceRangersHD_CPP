#include "layout/fGov.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/EC_Expression.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aNormalShip.hpp"
#include "types/aPirate.hpp"
#include "types/fHangar.hpp"
#include "types/fLoadRobot.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fSaveManager.hpp"
#include "types/fShip2.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Robot.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/ThreadCalc.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fGov.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelPlanet.hpp"
#include "units/fTalk.hpp"

namespace fGov {
    // Native battle launch selector, 1..3.
    std::int32_t GovernmentBattleDifficulty{};

    // Preserve the native receiver evaluation before the bounded payment,
    // with the clamp cells allocated before the receiver cell.
    void PayBailMoney(aShip::TShip* Ship) {
        std::int32_t Payment{};
        aPlayer::TPlayer* Player = aPlayer::GetPlayer();
        std::int32_t Remaining = aPlayer::GetPlayer()->Money - Ship->GetPrisonReleaseCost();
        if (Remaining < 0) {
            Payment = 0;
        } else {
            Payment = Remaining;
        }
        Player->SetMoney(Payment);
    }

    void TfGov_Create(TfGov* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->PlanetPanel = pas::construct_call<fPanelPlanet::TfPanelPlanet>(fPanelPlanet::TfPanelPlanet_Create);
        Self->ScriptDialogNames = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfGov_Destroy(TfGov* Self) {
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        if (Self->PlanetPanel != nullptr) {
            pas::free(Self->PlanetPanel);
            Self->PlanetPanel = nullptr;
        }
        if (Self->ScriptDialogNames != nullptr) {
            pas::free(Self->ScriptDialogNames);
            Self->ScriptDialogNames = nullptr;
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfGov::InitializeLayout() {
        std::int32_t HalfWidth{};
        std::int32_t ChoiceGrowth{};
        std::uint8_t Owner{};
        // Nested in TfGov.InitializeLayout; captures half-width and Self.
        auto LayoutPortrait = [&](pas::WideString Name, GI_MessageLoop::TMessageLoopGI* Screen) -> void {
            std::int32_t I{};
            std::int32_t PortraitX{};
            std::int32_t PortraitY{};
            std::int32_t TableY{};
            std::int32_t Bottom{};
            std::int32_t DeltaX{};
            std::int32_t DeltaY{};
            GI_MessageLoop::TObjectGI* Panel = Screen->FindControlByPath(Name);
            if (Panel != nullptr) {
                Panel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                PortraitY = static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 10;
                TableY = PortraitY + Panel->FindByNameRecursive(u"Gov_Anim0"_wref.get())->ClientSize.Y / 10 * 6;
                Bottom = TableY + Panel->FindByNameRecursive(u"Table"_wref.get())->ClientSize.Y / 10 * 9;
                DeltaX = Panel->FindByNameRecursive(u"Gov_Anim0"_wref.get())->LocalPosition.X - Panel->FindByNameRecursive(u"Gov_Anim1"_wref.get())->LocalPosition.X;
                DeltaY = Panel->FindByNameRecursive(u"Gov_Anim0"_wref.get())->ClientSize.Y - Panel->FindByNameRecursive(u"Gov_Anim1"_wref.get())->ClientSize.Y;
                if (GR_Main::GameScreenHeight > Bottom) {
                    PortraitY = PortraitY + GR_Main::GameScreenHeight - Bottom;
                    TableY = TableY + GR_Main::GameScreenHeight - Bottom;
                }
                {
                    GI_MessageLoop::TObjectGI* Table = Panel->FindByNameRecursive(u"Table"_wref.get());
                    Table->SetPosition(ClassesImports::Point(HalfWidth + (HalfWidth - Table->ClientSize.X) / 2, TableY));
                    Table->SetActive(this->UseClassicPortrait);
                }
                if (Panel->FindByNameRecursive(u"Table2"_wref.get()) != nullptr) {
                    GI_MessageLoop::TObjectGI* Table2 = Panel->FindByNameRecursive(u"Table2"_wref.get());
                    Table2->SetPosition(ClassesImports::Point(HalfWidth + (HalfWidth - Table2->ClientSize.X) / 2, TableY));
                    Table2->SetActive(false);
                }
                PortraitX = HalfWidth / 2 * 3 - Panel->FindByNameRecursive(u"Gov_Anim0"_wref.get())->ClientSize.X / 2;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, 1); cpp_range.next(I); ) {
                    {
                        GI_MessageLoop::TObjectGI* cpp_with_3 = Panel->FindByNameRecursive(static_cast<pas::WideString>(pas::concat_ansi({"Gov_Anim", SysUtils::IntToStr(I)})));
                        if (!this->UseClassicPortrait) {
                            cpp_with_3->SetPosition(ClassesImports::Point(cpp_with_3->LocalPosition.X + GR_Main::ExtraScreenWidth, cpp_with_3->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                        } else {
                            cpp_with_3->SetPosition(ClassesImports::Point(PortraitX - I * DeltaX, PortraitY + I * DeltaY));
                        }
                    }
                    {
                        GI_MessageLoop::TObjectGI* cpp_with_4 = Panel->FindByNameRecursive(static_cast<pas::WideString>(pas::concat_ansi({"GovHD_Anim", SysUtils::IntToStr(I)})));
                        cpp_with_4->SetPosition(ClassesImports::Point(HalfWidth + (HalfWidth - cpp_with_4->ClientSize.X) / 2, cpp_with_4->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                    }
                }
                Panel->FindByNameRecursive(u"BG"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
        };
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        PlanetPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fGov... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            HalfWidth = static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2;
            UseHdPortrait = false;
            UseClassicPortrait = false;
            if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1280 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 960) {
                UseHdPortrait = true;
                UseClassicPortrait = GlobalsV::UseTablesForGov;
            }
            for (Owner = static_cast<std::uint8_t>(0); Owner <= static_cast<std::uint8_t>(7); ++Owner) {
                LayoutPortrait(pas::concat_wide({u"Gov", aConst::OwnerInfo[Owner].InternalName}), this);
            }
            {
                GI_MessageLoop::TObjectGI* PanelTalk = MainPanel->FindByNameRecursive(u"PanelTalk"_wref.get());
                HalfWidth = std::min<std::int32_t>(std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0), 250) / 3;
                ChoiceGrowth = HalfWidth / 4 * 3;
                HalfWidth = HalfWidth * 3 - ChoiceGrowth;
                if (GR_Main::ExtraScreenHeight < 0) {
                    PanelTalk->SetPosition(ClassesImports::Point(PanelTalk->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelTalk->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
                } else {
                    PanelTalk->SetPosition(ClassesImports::Point(PanelTalk->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelTalk->LocalPosition.Y));
                }
                PanelTalk->SetSize(ClassesImports::Point(PanelTalk->ClientSize.X, PanelTalk->ClientSize.Y + HalfWidth + ChoiceGrowth));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_3 = PanelTalk->FirstChild;
                    cpp_with_3->SetPosition(ClassesImports::Point(cpp_with_3->LocalPosition.X, cpp_with_3->LocalPosition.Y + HalfWidth));
                    cpp_with_3->SetSize(ClassesImports::Point(cpp_with_3->ClientSize.X, cpp_with_3->ClientSize.Y + ChoiceGrowth));
                }
                {
                    GI_MessageLoop::TObjectGI* UserMsgAdd = PanelTalk->FindByNameRecursive(u"UserMsgAdd"_wref.get());
                    UserMsgAdd->SetPosition(ClassesImports::Point(UserMsgAdd->LocalPosition.X, UserMsgAdd->LocalPosition.Y + HalfWidth));
                }
                {
                    GI_MessageLoop::TObjectGI* ButFormClose = PanelTalk->FindByNameRecursive(u"ButFormClose"_wref.get());
                    ButFormClose->SetPosition(ClassesImports::Point(ButFormClose->LocalPosition.X, ButFormClose->LocalPosition.Y + HalfWidth + ChoiceGrowth));
                }
                {
                    GI_PanelScrollBar::TPanelScrollBarGI* TextScroll = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(PanelTalk->FindByNameRecursive(u"TextScroll"_wref.get()));
                    TextScroll->SetSize(ClassesImports::Point(TextScroll->ClientSize.X, TextScroll->ClientSize.Y + HalfWidth));
                    TextScroll->VerticalScrollBar->SetSize(ClassesImports::Point(TextScroll->VerticalScrollBar->ClientSize.X, TextScroll->VerticalScrollBar->ClientSize.Y + HalfWidth));
                    {
                        GI_MessageLoop::TObjectGI* TalkText = TextScroll->FindByNameRecursive(u"TalkText"_wref.get());
                        TalkText->SetSize(ClassesImports::Point(TalkText->ClientSize.X, TalkText->ClientSize.Y + HalfWidth));
                    }
                }
                {
                    GI_PanelScrollBar::TPanelScrollBarGI* TalkPA = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(PanelTalk->FindByNameRecursive(u"TalkPA"_wref.get()));
                    TalkPA->SetPosition(ClassesImports::Point(TalkPA->LocalPosition.X, TalkPA->LocalPosition.Y + HalfWidth));
                    TalkPA->SetSize(ClassesImports::Point(TalkPA->ClientSize.X, TalkPA->ClientSize.Y + ChoiceGrowth));
                    TalkPA->VerticalScrollBar->SetPosition(ClassesImports::Point(TalkPA->VerticalScrollBar->LocalPosition.X, TalkPA->VerticalScrollBar->LocalPosition.Y + HalfWidth));
                    TalkPA->VerticalScrollBar->SetSize(ClassesImports::Point(TalkPA->VerticalScrollBar->ClientSize.X, TalkPA->VerticalScrollBar->ClientSize.Y + ChoiceGrowth));
                    {
                        GI_MessageLoop::TObjectGI* cpp_with_9 = TalkPA->NextSibling;
                        cpp_with_9->SetSize(ClassesImports::Point(cpp_with_9->ClientSize.X, cpp_with_9->ClientSize.Y + HalfWidth + ChoiceGrowth));
                        {
                            GI_MessageLoop::TObjectGI* cpp_with_10 = cpp_with_9->NextSibling;
                            cpp_with_10->SetPosition(ClassesImports::Point(cpp_with_10->LocalPosition.X, cpp_with_10->LocalPosition.Y + HalfWidth + ChoiceGrowth));
                            {
                                GI_MessageLoop::TObjectGI* cpp_with_11 = cpp_with_10->NextSibling;
                                cpp_with_11->SetPosition(ClassesImports::Point(cpp_with_11->LocalPosition.X, cpp_with_11->LocalPosition.Y + HalfWidth));
                                {
                                    GI_MessageLoop::TObjectGI* cpp_with_12 = cpp_with_11->NextSibling;
                                    cpp_with_12->SetPosition(ClassesImports::Point(cpp_with_12->LocalPosition.X, cpp_with_12->LocalPosition.Y + HalfWidth));
                                }
                            }
                        }
                    }
                }
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfGov::MainPanelKeyDown>(this);
        {
            GI_GraphButton::TGraphButtonGI* UserMsgAdd_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()));
            UserMsgAdd_2->UpCallback = pas::bind_method<&TfGov::AddMessageClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButFormClose_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"_wref.get()));
            ButFormClose_2->UpCallback = pas::bind_method<&fPanelPlanet::TfPanelPlanet::PlanetClicked>(PlanetPanel);
        }
    }

    // Reviewed compiler-layout difference: native reserves one extra, unreferenced
    // dword at EBP-$F4, before its managed-string temporaries, and emits an extra
    // push ECX in the prologue. Rebuilt temporaries from $F8 onward are four bytes
    // nearer EBP. Calls, branches, constants and field accesses agree throughout.
    // Native diagnostic name: TfGov.BeforeRun.
    void TfGov::OnOpen() {
        std::uint8_t Owner{};
        std::int32_t MapIndex{};
        std::int32_t Money{};
        std::int32_t ExperienceAwarded{};
        pas::WideString Text{};
        pas::WideString WinText{};
        pas::WideString LossText{};
        pas::WideString TerronName{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::uint8_t Failed{};
        GI_MessageLoop::TObjectGI* Portrait{};
        std::int32_t Stage = 0;
        try {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            }
            MainPanel->OnOpen();
            fPanelPlanet::TfPanelPlanet::OnOpen();
            fPanelLoad::TfPanelLoad_OnOpen(LoadPanel);
            SavedChoiceScroll = -1;
            Stage = 1;
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->UpCallback = pas::bind_method<&TfGov::EndTurnClicked>(this);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->UpCallback = pas::bind_method<&TfGov::ShipClicked>(this);
            if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                SoundSection = 0;
            } else {
                SoundSection = aPlayer::GetPlayer()->CurrentPlanet->RaceId + 1;
            }
            Stage = 2;
            if (aPlayer::GetPlayer()->CurrentPlanet != fEquipmentShop::TemporaryShopPlanet) {
                SelectMusic();
                if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                    fEquipmentShop::RestoreTemporaryShopStock();
                }
                aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 0);
                Globals::PruneExpiredPersistentPlayerMessages();
                fEquipmentShop::BuildTemporaryShopSlotGrid();
            }
            Stage = 3;
            if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
                Event->AddTextData(u"PlanetCaptured"_w);
                GlobalsV::GameEndReason = 2;
                GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                RequestClose(1);
                return;
            }
            if (aPlayer::GetPlayer()->PendingDockDialogue == 1) {
                aPlayer::GetPlayer()->PendingDockDialogue = 0;
            }
            Stage = 4;
            for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range.next(Owner); ) {
                Portrait = FindControlByPath(pas::concat_wide({u"Gov", aConst::OwnerInfo[Owner].InternalName}));
                if (Portrait != nullptr) {
                    Portrait->SetActive(false);
                }
            }
            if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PortraitPanel = GetByName(u"GovPirateClan"_wref.get());
            } else {
                PortraitPanel = GetByName(pas::concat_wide({u"Gov", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName}));
            }
            PortraitPanel->SetActive(true);
            {
                GI_MessageLoop::TObjectGI* cpp_with = PortraitPanel;
                if (cpp_with->FindByNameRecursive(u"Table2"_wref.get()) != nullptr) {
                    cpp_with->FindByNameRecursive(u"Table"_wref.get())->SetActive(aPlayer::GetPlayer()->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && UseHdPortrait && UseClassicPortrait);
                    cpp_with->FindByNameRecursive(u"Table2"_wref.get())->SetActive(aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && UseHdPortrait && UseClassicPortrait);
                } else {
                    cpp_with->FindByNameRecursive(u"Table"_wref.get())->SetActive(UseHdPortrait && UseClassicPortrait);
                }
                {
                    GI_Image::TImageGI* BG = pas::checked_cast<GI_Image::TImageGI*>(cpp_with->FindByNameRecursive(u"BG"_wref.get()));
                    if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                        BG->SetImagePath(u"GI,Bm.Gov.PirateBG"_w);
                    } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                        BG->SetImagePath(pas::concat_wide({u"GI,Bm.Gov.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName, u"PirateBG"}));
                    } else {
                        BG->SetImagePath(pas::concat_wide({u"GI,Bm.Gov.2", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName, u"BGi"}));
                    }
                }
                if (UseHdPortrait && static_cast<std::uint8_t>(UseClassicPortrait ^ 1)) {
                    {
                        GI_GAI::TgaiGI* GovHD_Anim0 = pas::checked_cast<GI_GAI::TgaiGI*>(cpp_with->FindByNameRecursive(u"GovHD_Anim0"_wref.get()));
                        GovHD_Anim0->FirstFrameOnly = GlobalsV::AnimGov == 0;
                        GovHD_Anim0->PrimeImageCaches();
                    }
                    if (GlobalsV::AnimGov == 2) {
                        GI_GAI::TgaiGI* GovHD_Anim1 = pas::checked_cast<GI_GAI::TgaiGI*>(cpp_with->FindByNameRecursive(u"GovHD_Anim1"_wref.get()));
                        GovHD_Anim1->FirstFrameOnly = GlobalsV::AnimGov == 0;
                        GovHD_Anim1->PrimeImageCaches();
                    }
                } else {
                    {
                        GI_GAI::TgaiGI* Gov_Anim0 = pas::checked_cast<GI_GAI::TgaiGI*>(cpp_with->FindByNameRecursive(u"Gov_Anim0"_wref.get()));
                        Gov_Anim0->FirstFrameOnly = GlobalsV::AnimGov == 0;
                        Gov_Anim0->PrimeImageCaches();
                    }
                    if (GlobalsV::AnimGov == 2) {
                        GI_GAI::TgaiGI* Gov_Anim1 = pas::checked_cast<GI_GAI::TgaiGI*>(cpp_with->FindByNameRecursive(u"Gov_Anim1"_wref.get()));
                        Gov_Anim1->FirstFrameOnly = GlobalsV::AnimGov == 0;
                        Gov_Anim1->PrimeImageCaches();
                    }
                }
            }
            Stage = 5;
            Stage = 6;
            {
                GI_Label::TLabelGI* TalkText = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()));
                TalkText->SetText(u""_wref.get());
                if (GlobalsV::FontDialog == 0) {
                    TalkText->SetFontName(GlobalsV::NormalFontName);
                } else if (GlobalsV::FontDialog == 1) {
                    TalkText->SetFontName(GlobalsV::SmoothBigFontName);
                } else if (GlobalsV::FontDialog == 2) {
                    TalkText->SetFontName(GlobalsV::SmoothHugeFontName);
                } else if (GlobalsV::FontDialog >= 3) {
                    TalkText->SetFontName(GlobalsV::SmoothIntroFontName);
                }
            }
            UpdatePortraitAnimation(true);
            Stage = 7;
            if (PendingTransition == 1) {
                Stage = 8;
                MapIndex = Globals::FindRobotMapById(PlanetBattleMapId);
                Text = Globals::RobotMapDefinitions[MapIndex].RobotsStart;
                aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                aConst::ExpandLocalizedTextMarkupAndPrefixLines(Text);
                Text = pas::concat_wide({pas::wide_int_to_str(GovernmentBattleDifficulty), Text});
                Text = pas::concat_wide({pas::wide_int_to_str(std::min<std::int32_t>(aGalaxy::Galaxy->GetDifficultyTierIndex() & 0x0000007f, 3) + 1), Text});
                Text = pas::concat_wide({pas::wide_int_to_str(aPlayer::GetPlayer()->CurrentPlanet->RaceId + 1), Text});
                WinText = Globals::RobotMapDefinitions[MapIndex].RobotsWin;
                aMyFunction::ReplaceTextToken(WinText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(WinText, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(WinText, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                aConst::ExpandLocalizedTextMarkupAndPrefixLines(WinText);
                LossText = Globals::RobotMapDefinitions[MapIndex].RobotsLoss;
                aMyFunction::ReplaceTextToken(LossText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(LossText, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(LossText, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                aConst::ExpandLocalizedTextMarkupAndPrefixLines(LossText);
                TerronName = pas::concat_wide({aPlayer::GetPlayer()->CurrentPlanet->GetFullName(u" "_w), u", ", GR_Main::LookupLocalizedTextOrEmpty(u"FormShip.StorageInfo.StarInfo"_wref.get()), u" ", aPlayer::GetPlayer()->CurrentStar->Name});
                Stage = 9;
                if (ThreadCalc::IsTurnCalculationRunning() && WindowsSdk::WaitForSingleObject(Globals::ScriptUiRequestEvent, 0u) != WindowsSdk::WAIT_OBJECT_0) {
                    ThreadCalc::WaitForTurnCalculation();
                }
                if (!GlobalsV::MemorySnapshotActive) {
                    aSaveLoad::SaveGameToMemorySnapshot();
                }
                fPanelLoad::TfPanelLoad_OnOpen(LoadPanel);
                LoadPanel->SelectBackgroundStyle(3);
                LoadPanel->RefreshBackgroundImages();
                Stage = 10;
                try {
                    Failed = false;
                    PendingTransition = Robot::FRun(Globals::RobotMapDefinitions[MapIndex].Map, Text, WinText, LossText, TerronName);
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                        Failed = true;
                    } else {
                        throw;
                    }
                }
                Stage = 11;
                if (GlobalsV::MemorySnapshotActive) {
                    aSaveLoad::RestoreGameFromMemorySnapshot();
                }
                Stage = 12;
                if (Failed) {
                    if (([&] {
                        const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormGov.BattlePlanetQuestCrashed"_wref.get());
                        GI_MessageLoop::TMessageLoopGI* self = this;
                        return GI_MessageBox::ShowMessageBoxGI(self, localizedColorText, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0);
                    }()) == GI_MessageBox::mbgResultOK) {
                        PendingTransition = 3;
                    } else {
                        pas::raise(pas::make_exception<pas::Exception>("Error in Matrix.dll"_a));
                    }
                }
                if (PendingTransition == 0) {
                    return;
                }
                if (PendingTransition == 1) {
                    PendingTransition = 0;
                    if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
                        pas::free(aSaveLoad::MemorySnapshotBuffer);
                    }
                    aSaveLoad::MemorySnapshotBuffer = nullptr;
                    GlobalsV::MemorySnapshotActive = false;
                    if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
                        pas::free(aGalaxy::Galaxy);
                    }
                    aGalaxy::Galaxy = nullptr;
                    Globals::ScreenLoadMode = 4;
                    GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
                    GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
                    RequestClose(1);
                    return;
                }
                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                RequestClose(1);
                return;
            }
            if (PendingTransition == 2) {
                Stage = 13;
                MapIndex = Globals::FindRobotMapById(PlanetBattleMapId);
                Money = aMyFunction::RoundAndTruncateToTens(pas::real_min<pas::Extended>(aPlayer::GetPlayer()->Wealth * 0.03L, ([&] {
                    pas::Extended cpp_arg = static_cast<pas::Extended>(aGalaxy::Galaxy->ComputeScaledAverageMoney(2) * 7);
                    pas::Extended cpp_arg_2 = aGalaxy::Galaxy->ComputeScaledHugeMoney(2) * 1.5L;
                    return pas::real_min<pas::Extended>(cpp_arg, cpp_arg_2);
                }())));
                Money = System::Round(static_cast<long double>(Money) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].ArcadeRewardScale);
                switch (GovernmentBattleDifficulty) {
                    case 1: Money = aMyFunction::RoundAndTruncateToTens(Money * 0.5L); break;
                    case 2: Money = aMyFunction::RoundAndTruncateToTens(Money * 0.2L); break;
                    case 3: Money = aMyFunction::RoundAndTruncateToTens(Money * 0.1L); break;
                }
                Stage = 14;
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Money);
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                aPlayer::GetPlayer()->CurrentPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), -40);
                DialogText = Globals::RobotMapDefinitions[MapIndex].GovTextLoss;
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Money), u"<color=255,240,100>"_w);
                Stage = 15;
                BuildGovernmentChoices(true);
                Stage = 16;
                aPlayer::GetPlayer()->PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1 + 1);
                {
                    aPlayer::TPlanetBattleHistoryEntry& cpp_with_8 = aPlayer::GetPlayer()->PlanetBattleHistory[aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1];
                    cpp_with_8.MapId = PlanetBattleMapId;
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_8.Statistics, 0 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[0]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_8.Statistics, 1 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[1]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_8.Statistics, 2 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[2]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_8.Statistics, 3 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[3]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_8.Statistics, 4 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[4]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_8.Statistics, 5 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[5]);
                    cpp_with_8.ResultCode = GovernmentBattleDifficulty;
                    cpp_with_8.CompletionMode = PendingTransition;
                    cpp_with_8.DateTurn = aGalaxy::Galaxy->CurrentTurn;
                }
                aPlayer::GetPlayer()->LastPlanetBattleTurn = aGalaxy::Galaxy->CurrentTurn;
            } else if (PendingTransition == 3) {
                Stage = 17;
                MapIndex = Globals::FindRobotMapById(PlanetBattleMapId);
                Money = aMyFunction::RoundAndTruncateToTens(pas::real_max<pas::Extended>(aPlayer::GetPlayer()->Wealth * 0.03L, static_cast<pas::Extended>(aGalaxy::Galaxy->ComputeScaledBigMoney(2))));
                Money = System::Round(static_cast<long double>(Money) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestMoneyFactor);
                if (aPlayer::GetPlayer()->IsHealthEffectActive(23)) {
                    Money = System::Round(static_cast<long double>(aMyFunction::SeededRandomFloatRange((static_cast<std::int32_t>(aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 33, 1.3, 2.3)) * Money);
                }
                Money += System::Round(Money * (aPlayer::GetPlayer()->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.1L);
                switch (GovernmentBattleDifficulty) {
                    case 1: Money = aMyFunction::RoundAndTruncateToTens(Money * 4.0L); break;
                    case 2: Money = aMyFunction::RoundAndTruncateToTens(Money * 1.6L); break;
                    case 3: Money = aMyFunction::RoundAndTruncateToTens(Money * 0.8L); break;
                }
                Stage = 18;
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Money);
                GR_Main::SoundManager->PlaySound(u"Sound.LiberationSystem"_wref.get());
                Stage = 19;
                DialogText = Globals::RobotMapDefinitions[MapIndex].GovTextWin;
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Money), u"<color=255,240,100>"_w);
                DialogText = pas::concat_wide_reverse({aRanger::TRanger_GrantPlanetQuestReward(aPlayer::GetPlayer(), GovernmentBattleDifficulty, ExperienceAwarded), DialogText});
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerFinishesPlanetaryBattle"_w, nullptr);
                Event->AddData(PlanetBattleMapId);
                Event->AddData(Money);
                Event->AddData(ExperienceAwarded);
                Stage = 20;
                BuildGovernmentChoices(true);
                Stage = 21;
                aPlayer::GetPlayer()->PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1 + 1);
                {
                    aPlayer::TPlanetBattleHistoryEntry& cpp_with_9 = aPlayer::GetPlayer()->PlanetBattleHistory[aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1];
                    cpp_with_9.MapId = PlanetBattleMapId;
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_9.Statistics, 0 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[0]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_9.Statistics, 1 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[1]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_9.Statistics, 2 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[2]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_9.Statistics, 3 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[3]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_9.Statistics, 4 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[4]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_9.Statistics, 5 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[5]);
                    cpp_with_9.ResultCode = GovernmentBattleDifficulty;
                    cpp_with_9.CompletionMode = PendingTransition;
                    cpp_with_9.DateTurn = aGalaxy::Galaxy->CurrentTurn;
                }
                Stage = 22;
                aPlayer::GetPlayer()->LastPlanetBattleTurn = aGalaxy::Galaxy->CurrentTurn;
                ++aPlayer::GetPlayer()->PlanetBattles;
                Achievements::TryAddAchievementProgress(u"IRONMAN"_w, 1);
                Stage = 23;
                Globals::LoadRobotScreen->LoadCompletionData();
                if (GovernmentBattleDifficulty == 1) {
                    Globals::LoadRobotScreen->RecordCompletion(PlanetBattleMapId, -Robot::RobotBattleStatistics[0] / 1000, 2);
                } else {
                    Globals::LoadRobotScreen->RecordCompletion(PlanetBattleMapId, -Robot::RobotBattleStatistics[0] / 1000, 1);
                }
                Globals::LoadRobotScreen->SaveCompletionData();
            } else if (PendingTransition == 4) {
                Stage = 24;
                aPlayer::GetPlayer()->CurrentPlanet->SetRelationLevelToRanger(aPlayer::GetPlayer(), aGalaxyStruct::rlBad);
                DialogText = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.GovAfterCancel"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 124);
                ClearDialogChoices();
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfGov::DeclineBattleAndLeave>(this);
                    pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.PlayerAfterCancelNormal"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 123);
                    TfGov* self_2 = this;
                    self_2->AddChoice(std::move(pickLocalizedTextVariant), 0, cpp_arg_3);
                }
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfGov::ChoosePrisonInsteadOfBattle>(this);
                    pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.PlayerAfterCancelPrison"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 123);
                    TfGov* self_3 = this;
                    self_3->AddChoice(std::move(pickLocalizedTextVariant_2), 0, cpp_arg_4);
                }
                Stage = 25;
                aPlayer::GetPlayer()->PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1 + 1);
                {
                    aPlayer::TPlanetBattleHistoryEntry& cpp_with_10 = aPlayer::GetPlayer()->PlanetBattleHistory[aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1];
                    cpp_with_10.MapId = PlanetBattleMapId;
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_10.Statistics, 0 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[0]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_10.Statistics, 1 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[1]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_10.Statistics, 2 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[2]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_10.Statistics, 3 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[3]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_10.Statistics, 4 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[4]);
                    pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with_10.Statistics, 5 * sizeof(std::int32_t)), Robot::RobotBattleStatistics[5]);
                    cpp_with_10.ResultCode = GovernmentBattleDifficulty;
                    cpp_with_10.CompletionMode = PendingTransition;
                    cpp_with_10.DateTurn = aGalaxy::Galaxy->CurrentTurn;
                }
                aPlayer::GetPlayer()->LastPlanetBattleTurn = aGalaxy::Galaxy->CurrentTurn;
            }
            Stage = 26;
            if (PendingTransition == 0) {
                RefreshGovernmentDialog();
            }
            PendingTransition = 0;
            Stage = 27;
            RestartTextPresentation(false);
            Stage = 28;
            {
                GI_PanelScrollBar::TPanelScrollBarGI* TalkPA = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
                TalkPA->SetVerticalScrollbarEnabled(false);
            }
            AnimationRestartRequested = false;
            MainPanel->RebuildMessageButtons(false);
            Stage = 29;
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(0x00000018, nullptr, nullptr, 0);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(170);
        } catch (...) {
            auto cpp_exception_2 = pas::caught_object();
            if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception_2)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfGov.BeforeRun, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfGov::OnClose() {
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(171);
        }
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(0x00000019, nullptr, nullptr, 0);
        }
        LoadPanel->OnClose();
        Globals::ScriptDialogIndex = -1;
        ClearDialogChoices();
        MainPanel->OnClose();
        fPanelPlanet::TfPanelPlanet::OnClose();
        ScriptDialogNames->Clear();
    }

    void TfGov::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) > aGalaxyStruct::rlHostile && aPlayer::GetPlayer()->PendingDockDialogue <= 1) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(172);
            fEquipmentShop::RestoreTemporaryShopStock();
            MainPanel->EndTurnClicked(Sender);
            MainPanel->RebuildMessageButtons(false);
            if (ExitCode == 0) {
                fEquipmentShop::BuildTemporaryShopSlotGrid();
                UpdatePortraitAnimation(true);
                RefreshGovernmentDialog();
                aGalaxy::Galaxy->PrimeIntegrityChecksum(173);
                RestartTextPresentation(true);
                AnimationRestartRequested = false;
            }
        }
    }

    void TfGov::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        MainPanel->ShipClicked(Sender);
        if (Globals::ShipScreen->Flag3BC) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(302);
            RefreshGovernmentDialog();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(303);
            RestartTextPresentation(true);
            MainPanel->RebuildMessageButtons(false);
        }
    }

    void TfGov::RequestAnimationRestart() {
        AnimationRestartRequested = true;
    }

    void TfGov::PortraitAnimationComplete(GI_MessageLoop::TObjectGI* Sender) {
        if (AnimationRestartRequested) {
            UpdatePortraitAnimation(true);
            AnimationRestartRequested = false;
        } else {
            UpdatePortraitAnimation(false);
        }
    }

    void TfGov::UpdatePortraitAnimation(std::uint8_t Talking) {
        if (GlobalsV::AnimGov != 2) {
            Talking = false;
        }
        if (UseHdPortrait && static_cast<std::uint8_t>(UseClassicPortrait ^ 1)) {
            {
                GI_GAI::TgaiGI* GovHD_Anim0 = pas::checked_cast<GI_GAI::TgaiGI*>(PortraitPanel->FindByNameRecursive(u"GovHD_Anim0"_wref.get()));
                GovHD_Anim0->CycleCompleteCallback = pas::bind_method<&TfGov::PortraitAnimationComplete>(this);
                GovHD_Anim0->SetSequenceFrame(0);
                GovHD_Anim0->StopAutoPlayback();
                if (!Talking) {
                    GovHD_Anim0->RestartPlayback();
                } else {
                    GovHD_Anim0->StopAutoPlayback();
                }
                GovHD_Anim0->SetActive(static_cast<std::uint8_t>(Talking ^ 1));
            }
            {
                GI_GAI::TgaiGI* GovHD_Anim1 = pas::checked_cast<GI_GAI::TgaiGI*>(PortraitPanel->FindByNameRecursive(u"GovHD_Anim1"_wref.get()));
                GovHD_Anim1->CycleCompleteCallback = pas::bind_method<&TfGov::PortraitAnimationComplete>(this);
                GovHD_Anim1->SetSequenceFrame(0);
                GovHD_Anim1->StopAutoPlayback();
                if (Talking) {
                    GovHD_Anim1->RestartPlayback();
                } else {
                    GovHD_Anim1->StopAutoPlayback();
                }
                GovHD_Anim1->SetActive(Talking);
            }
        } else {
            {
                GI_GAI::TgaiGI* Gov_Anim0 = pas::checked_cast<GI_GAI::TgaiGI*>(PortraitPanel->FindByNameRecursive(u"Gov_Anim0"_wref.get()));
                Gov_Anim0->CycleCompleteCallback = pas::bind_method<&TfGov::PortraitAnimationComplete>(this);
                Gov_Anim0->SetSequenceFrame(0);
                Gov_Anim0->StopAutoPlayback();
                if (!Talking) {
                    Gov_Anim0->RestartPlayback();
                } else {
                    Gov_Anim0->StopAutoPlayback();
                }
                Gov_Anim0->SetActive(static_cast<std::uint8_t>(Talking ^ 1));
            }
            {
                GI_GAI::TgaiGI* Gov_Anim1 = pas::checked_cast<GI_GAI::TgaiGI*>(PortraitPanel->FindByNameRecursive(u"Gov_Anim1"_wref.get()));
                Gov_Anim1->CycleCompleteCallback = pas::bind_method<&TfGov::PortraitAnimationComplete>(this);
                Gov_Anim1->SetSequenceFrame(0);
                Gov_Anim1->StopAutoPlayback();
                if (Talking) {
                    Gov_Anim1->RestartPlayback();
                } else {
                    Gov_Anim1->StopAutoPlayback();
                }
                Gov_Anim1->SetActive(Talking);
            }
        }
    }

    void TfGov::RememberChoiceScroll() {
        SavedChoiceScroll = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()))->VerticalScrollBar->Position;
    }

    void TfGov::ClearDialogChoices() {
        NextChoiceTop = 0;
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"TalkPA"_wref.get());
        GI_MessageLoop::TObjectGI* Child = Panel->FirstChild;
        while (Child != nullptr) {
            pas::free(reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Child->UserValue))));
            Child = Child->NextSibling;
        }
        Panel->FreeOwnedChildren();
        Panel->Invalidate();
    }

    void TfGov::AddChoice(pas::WideString Text, std::int32_t Value, GI_MessageLoop::TDialogChoiceEventGI Callback) {
        std::int32_t I{};
        std::uint8_t BlockMode = 0;
        if (aScript::ScriptDialogBlocks != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogBlocks) - 1); cpp_range.next(I); ) {
                if (EC_Str::FindTextOffsetW(Text, pas::list_at<aScript::TDialogBlock>(aScript::ScriptDialogBlocks, I)->Text, 0) >= 0) {
                    BlockMode = std::max<std::int32_t>(static_cast<std::int32_t>(BlockMode), static_cast<std::int32_t>(pas::list_at<aScript::TDialogBlock>(aScript::ScriptDialogBlocks, I)->Mode));
                }
            }
        }
        if (BlockMode >= 2) {
            return;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
        I = 0;
        while (I < Text.length()) {
            if (Text.read(I + 1) != u'-' && Text.read(I + 1) != u' ') {
                break;
            }
            ++I;
        }
        if (I > 0) {
            Text = pas::copy(Text, I + 1, Text.length() - I);
        }
        fTalk::TfTalkA* Choice = pas::construct_call<fTalk::TfTalkA>(fTalk::TfTalkA_Create);
        Choice->Callback = Callback;
        Choice->Value = Value;
        if (BlockMode > 0) {
            Choice->Callback = nullptr;
        }
        GI_Panel::TPanelGI* Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
        Row->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Choice));
        Row->SetPosition(ClassesImports::Point(0, NextChoiceTop));
        Row->SetSize(ClassesImports::Point(Panel->ClientSize.X, 20));
        Row->SetPositionModeW(true);
        Row->MouseEnterCallback = pas::bind_static_method<&TfGov::ChoiceMouseEnter>(this);
        Row->MouseLeaveCallback = pas::bind_static_method<&TfGov::ChoiceMouseLeave>(this);
        Row->LeftButtonDownCallback = pas::bind_static_method<&TfGov::ChoiceMouseDown>(this);
        Row->LeftButtonUpCallback = pas::bind_method<&TfGov::ChoiceMouseUp>(this);
        GI_Image::TImageGI* Highlight = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
        Highlight->SetDepth(3.0);
        Highlight->SetPosition(ClassesImports::Point(0, 0));
        Highlight->SetSize(ClassesImports::Point(Panel->ClientSize.X, 20));
        Highlight->SetImagePath(pas::concat_wide({u"GI,Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"Line"}));
        Highlight->SetImageKindX(GI_Main::ikxLeftFill);
        Highlight->SetImageKindY(GI_Main::ikyTopFill);
        Highlight->SetActive(false);
        {
            GI_Label::TLabelGI* cpp_with = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
            if (GlobalsV::FontDialog == 0) {
                cpp_with->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                cpp_with->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                cpp_with->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                cpp_with->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            cpp_with->SetSize(ClassesImports::Point(Panel->ClientSize.X - GR_Main::GiScalePixels(20), 20));
            cpp_with->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(10), 0));
            cpp_with->SetWordWrapEnabled(true);
            cpp_with->SetTextAlignX(GI_Main::taxLeft);
            cpp_with->SetTextAlignY(GI_Main::tayAuto);
            if (!pas::assigned(Callback)) {
                Text = EC_Str::RemoveTextTagsW(Text);
            }
            cpp_with->SetText(pas::concat_wide({u"<Object=0,20,14,0>", EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"_wref.get())}));
            cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            if (!pas::assigned(Choice->Callback)) {
                cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(127, 127, 127));
            }
            cpp_with->CreateEmbeddedControl = pas::bind_static_method<&TfGov::CreateDialogObject>(this);
            cpp_with->SetTextAlignY(GI_Main::tayCenterEx);
            Row->SetSize(ClassesImports::Point(Row->ClientSize.X, cpp_with->ClientSize.Y + 2 * GR_Main::GiScalePixelsEx(2, 2)));
            cpp_with->SetSize(ClassesImports::Point(cpp_with->ClientSize.X, Row->ClientSize.Y));
            Highlight->SetSize(Row->ClientSize);
            NextChoiceTop += cpp_with->ClientSize.Y;
        }
    }

    void TfGov::ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(true);
    }

    void TfGov::ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(false);
    }

    void TfGov::ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(2, 0));
        }
    }

    void TfGov::ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(0, 0));
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(174);
        fTalk::TfTalkA* Choice = reinterpret_cast<fTalk::TfTalkA*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        if (pas::assigned(Choice->Callback)) {
            Choice->Callback(Choice->Value);
        } else if (pas::assigned(Choice->FallbackCallback)) {
            Choice->FallbackCallback(Choice->FallbackText);
        } else {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(176);
            return;
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(175);
        RestartTextPresentation(true);
        MainPanel->RefreshMoneyAndCargo();
        MainPanel->RebuildMessageButtons(false);
        GI_Main::BreakUiMessage();
    }

    void TfGov::RestartTextPresentation(std::uint8_t RestartAnimation) {
        if (RestartAnimation) {
            RequestAnimationRestart();
        }
        pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()))->SetActive(false);
        FormattedTextLength = 0;
        if (DialogRefreshTimer != nullptr) {
            CancelCallbackTimer(DialogRefreshTimer);
            DialogRefreshTimer = nullptr;
        }
        DialogRefreshTimer = ScheduleCallbackTimer(10, 10, pas::bind_method<&TfGov::AdvanceTextPresentation>(this), 0);
    }

    void TfGov::AdvanceTextPresentation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_PanelScrollBar::TPanelScrollBarGI* Choices{};
        GI_PanelScrollBar::TPanelScrollBarGI* TextPanel{};
        if (FormattedTextLength >= DialogText.length()) {
            Choices = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
            Choices->SetActive(true);
            {
                std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = Choices->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
            Choices->VerticalScrollBar->SetLargeChange(Choices->ClientSize.Y);
            Choices->VerticalScrollBar->SetPageSize(Choices->ClientSize.Y);
            Choices->SetScrollOffset(ClassesImports::Point(0, 0));
            Choices->VerticalScrollBar->SetActive(NextChoiceTop > Choices->ClientSize.Y);
            Choices->VerticalScrollBar->SetDepth(4.0);
            Choices->SetDragScrollingEnabled(Choices->VerticalScrollBar->Active);
            Choices->UpdateScrollRanges();
            if (DialogRefreshTimer != nullptr) {
                CancelCallbackTimer(DialogRefreshTimer);
                DialogRefreshTimer = nullptr;
            }
            if (SavedChoiceScroll >= 0) {
                Choices->VerticalScrollBar->SetPosition_2(SavedChoiceScroll);
            }
            SavedChoiceScroll = -1;
            GR_Main::PostMouseMoveMessage();
        } else {
            DialogText = pas::concat_wide_reverse({EC_Str::TrimWideString(DialogText), aConst::LocalizedTextLinePrefix});
            DialogText = EC_Str::ReplaceAllWideString(DialogText, pas::concat_wide({u"\r\n", aConst::LocalizedTextLinePrefix}), u"\r\n"_wref.get());
            DialogText = EC_Str::ReplaceAllWideString(DialogText, u"\r\n"_wref.get(), pas::concat_wide({u"\r\n", aConst::LocalizedTextLinePrefix}));
            FormattedTextLength = DialogText.length();
            DialogText = EC_Str::ReplaceAllWideString(DialogText, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"_wref.get());
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->SetText(DialogText);
            TextPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"_wref.get()));
            TextPanel->SetScrollOffset(ClassesImports::Point(0, 0));
            TextPanel->UpdateScrollRanges();
            TextPanel->VerticalScrollBar->SetActive(pas::checked_cast<GI_Label::TLabelGI*>(TextPanel->FindByNameRecursive(u"TalkText"_wref.get()))->ClientSize.Y > TextPanel->ClientSize.Y);
            {
                std::int32_t lineHeight_2 = pas::checked_cast<GI_Label::TLabelGI*>(TextPanel->FindByNameRecursive(u"TalkText"_wref.get()))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar_2 = TextPanel->VerticalScrollBar;
                verticalScrollBar_2->SetSmallChange(lineHeight_2);
            }
            TextPanel->VerticalScrollBar->SetLargeChange(TextPanel->ClientSize.Y);
            TextPanel->VerticalScrollBar->SetPageSize(TextPanel->ClientSize.Y);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->SetDisabled(false);
        }
    }

    void TfGov::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"_wref.get()));
        if (!Panel->ContainsPoint(Point)) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"_wref.get()));
        }
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
        }
    }

    GI_MessageLoop::TObjectGI* TfGov::CreateDialogObject(GI_Label::TLabelGI* LabelControl, EC_CacheFont::PFontObjectEC Item) {
        GI_MessageLoop::TObjectGI* Result = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, LabelControl);
        GI_Image::TImageGI* Image = pas::checked_cast<GI_Image::TImageGI*>(Result);
        Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"Answer"}));
        Image->SetImageKindX(GI_Main::ikxLeft);
        return Result;
    }

    void TfGov::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        GI_GraphButton::TGraphButtonGI* Button{};
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) || ExitCode != 0) {
            return;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"_wref.get()));
        if (Key == WindowsSdk::VK_SPACE) {
            if (GetByName(u"PM_EndTurn"_wref.get())->Active) {
                EndTurnClicked(nullptr);
            }
        } else if (Key == 'S') {
            ShipClicked(nullptr);
        } else if (Key == WindowsSdk::VK_UP) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
        } else if (Key == WindowsSdk::VK_DOWN) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
        } else if (Key == WindowsSdk::VK_PRIOR) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->LargeChange);
        } else if (Key == WindowsSdk::VK_NEXT) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->LargeChange);
        } else if (Key == WindowsSdk::VK_INSERT) {
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()));
            AddMessageClicked(Button);
        } else {
            MainPanel->ProcessKeyDown(Key);
            PlanetPanel->ProcessKeyDown(Key);
        }
    }

    void TfGov::AddMessageClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"_wref.get()))->GetText();
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,50,200>"_wref.get(), u"<color=255,240,100>"_wref.get());
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender)->SetDisabled(true);
        GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
        Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
        MainPanel->RebuildMessageButtons(false);
        GI_Main::BreakUiMessage();
    }

    void TfGov::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
            }
        } else {
            GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
        }
    }

    void TfGov::RefreshGovernmentDialog() {
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        if (aCalc::IsTurnCalculationRunningUI()) {
            aCalc::WaitForTurnCalculationUI();
        }
        if (ExitCode == 0) {
            if (aPlayer::GetPlayer()->InPrison) {
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    DialogText = aConst::LocalizedColorText(u"FormGov.Prison.GovAfterPrison"_wref.get());
                } else {
                    DialogText = aConst::LocalizedColorText(u"FormGov.PirateClanPrison.GovAfterPrison"_wref.get());
                }
                aPlayer::GetPlayer()->InPrison = false;
                ++aPlayer::GetPlayer()->PrisonStaysCompleted;
                Achievements::TryAddAchievementProgress(u"PRISON"_w, 1);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_2.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Planet->Warriors, J);
                        if (aPlayer::GetPlayer() == Ship->EnemyShip) {
                            Ship->EnemyShip = nullptr;
                        }
                        if (aPlayer::GetPlayer()->EnemyShip == Ship) {
                            aPlayer::GetPlayer()->EnemyShip = nullptr;
                        }
                    }
                }
                ClearDialogChoices();
                AddChoice(aConst::LocalizedColorText(u"FormGov.I_Continue"_wref.get()), 0, pas::bind_method<&TfGov::ContinueAfterPrison>(this));
            } else if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && static_cast<std::uint8_t>(aScript::HasPendingScriptRequests() ^ 1) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
                if (aPlayer::GetPlayer()->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    DialogText = aConst::LocalizedColorText(u"FormGov.Prison.GovBeforePrison"_wref.get());
                } else {
                    DialogText = aConst::LocalizedColorText(u"FormGov.PirateClanPrison.GovBeforePrison"_wref.get());
                }
                ClearDialogChoices();
                if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    AddChoice(aConst::LocalizedColorText(u"FormGov.Prison.PlayerGoToPrison"_wref.get()), 0, pas::bind_method<&TfGov::EnterPrison>(this));
                } else {
                    AddChoice(aConst::LocalizedColorText(u"FormGov.PirateClanPrison.PlayerGoToPrison"_wref.get()), 0, pas::bind_method<&TfGov::EnterPrison>(this));
                }
            } else if (aPlayer::GetPlayer()->PendingLiberationCeremonyPlanet == aPlayer::GetPlayer()->CurrentPlanet) {
                DialogText = aPlayer::GetPlayer()->CollectLiberationRewards();
                ClearDialogChoices();
                AddChoice(aConst::LocalizedColorText(u"FormGov.PlayerAfterCongratulationsLiberator"_wref.get()), 0, pas::bind_method<&TfGov::ExitGovernment>(this));
            } else {
                DialogText = aPlayer::GetPlayer()->CurrentPlanet->BuildGovernmentGreeting();
                BuildGovernmentChoices(false);
            }
        }
    }

    // DL flag: true suppresses selecting/appending response text from the script-choice list; script execution and choice construction still run. Callers pass 0 or 1.
    void TfGov::BuildGovernmentChoices(std::uint8_t SkipScriptResponseText) {
        aScript::TScript* Script{};
        pas::WideString Text{};
        pas::WideString Mode{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Selected{};
        std::int32_t Priority{};
        std::int32_t PartCount{};
        void* Swapped{};
        aScript::ClearScriptDialogRules();
        ClearDialogChoices();
        aPlayer::GetPlayer()->CurrentPlanet->CollectScriptDialogChoices(ScriptDialogNames);
        ScriptDialogCursor = 0;
        Script = nullptr;
        Globals::ScriptDialogIndex = -1;
        while (ScriptDialogCursor < ScriptDialogNames->GetCount()) {
            Script = static_cast<aScript::TScript*>(ScriptDialogNames->GetDataAt(ScriptDialogCursor));
            {
                pas::WideString textAt = ScriptDialogNames->GetTextAt(ScriptDialogCursor);
                aScript::TScript* script = Script;
                aScript::TScript_CallDialogByVariable(script, std::move(textAt));
            }
            if (Globals::ScriptDialogIndex >= 0) {
                break;
            }
            ++ScriptDialogCursor;
        }
        if (aPlayer::GetPlayer()->TryTurnInAnyQuest(Text)) {
            DialogText = Text;
        }
        if (Globals::ScriptDialogIndex < 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(I); ) {
                Script = pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, I);
                aScript::TScript_RunAuxiliaryCode(Script);
            }
            if (pas::list_count(aScript::ScriptDialogOverrides) > 0) {
                Selected = 0;
                Priority = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, 0)->Priority;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogOverrides) - 1); cpp_range_2.next(I); ) {
                    if (pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, I)->Priority > Priority) {
                        Selected = I;
                        Priority = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, I)->Priority;
                    }
                }
                Script = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, Selected)->Script;
                {
                    std::uint32_t answerData = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, Selected)->AnswerData;
                    EC_Expression::TVarEC* var = Script->InitCode->LocalVar->GetVar(u"GAnswerData"_wref.get());
                    var->SetDword(answerData);
                }
                Text = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, Selected)->DialogName;
                if (Text != u"") {
                    aScript::TScript_CallDialogByVariable(Script, Text);
                    if (Globals::ScriptDialogIndex < 0) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({Script->ScriptFileName, u" has overriden dialog with ", Text, u" but it failed to start"})));
                    }
                }
                if (Globals::ScriptDialogIndex < 0) {
                    AddBuiltinGovernmentChoices();
                } else {
                    StartScriptMessage(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script)));
                }
            } else {
                Selected = -1;
                Priority = 0;
                if (!SkipScriptResponseText) {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_3.next(I); ) {
                        if (pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->ReplaceGreeting) {
                            if (Selected < 0 || pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Priority > Priority) {
                                Priority = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Priority;
                                Selected = I;
                            }
                        }
                    }
                }
                if (Selected >= 0) {
                    DialogText = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, Selected)->Text;
                }
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_4.next(I); ) {
                    for (auto cpp_range_5 = pas::for_downto<std::int32_t>(pas::list_count(aScript::ScriptDialogInjections) - 1, I); cpp_range_5.next(J); ) {
                        if (([&] {
                            std::int32_t cpp_left = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, J)->Priority;
                            return cpp_left > pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, J - 1)->Priority;
                        }())) {
                            Swapped = pas::list_get(aScript::ScriptDialogInjections, J);
                            pas::list_put(aScript::ScriptDialogInjections, J, pas::list_get(aScript::ScriptDialogInjections, J - 1));
                            pas::list_put(aScript::ScriptDialogInjections, J - 1, Swapped);
                        }
                    }
                }
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_6.next(I); ) {
                    if (!pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->ReplaceGreeting) {
                        Text = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Text;
                        if (Text != u"" && static_cast<std::uint8_t>(SkipScriptResponseText ^ 1)) {
                            DialogText = pas::concat_wide({DialogText, u"\r\n", Text});
                        }
                    }
                    Text = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Answer;
                    if (Text != u"") {
                        Mode = pas::WideString();
                        PartCount = EC_Str::CountDelimitedPartsW(Text, u"~"_wref.get());
                        if (PartCount > 1) {
                            Mode = EC_Str::ExtractDelimitedPartW(Text, 0, u"~"_wref.get());
                            Text = EC_Str::ExtractDelimitedRangeW(Text, 1, PartCount - 1, u"~"_wref.get());
                        }
                        if (Mode == u"block") {
                            AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
                        } else if (Mode == u"snap") {
                            AddChoice(Text, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aScript::ScriptDialogInjections, I))), pas::bind_method<&TfGov::RunInjectedAnswerKeepingScroll>(this));
                        } else {
                            std::int32_t cpp_arg = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aScript::ScriptDialogInjections, I)));
                            pas::WideString answer = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Answer;
                            AddChoice(std::move(answer), cpp_arg, pas::bind_method<&TfGov::RunInjectedAnswer>(this));
                        }
                    }
                }
                AddBuiltinGovernmentChoices();
            }
        } else if (!Script->SkipGreeting) {
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormGov.I_Continue"_wref.get());
            std::int32_t script_2 = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script));
            AddChoice(std::move(localizedColorText), script_2, pas::bind_method<&TfGov::StartScriptMessage>(this));
        } else {
            Script->SkipGreeting = false;
            StartScriptMessage(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script)));
        }
    }

    void TfGov::AddBuiltinGovernmentChoices() {
        if (aPlayer::GetPlayer()->CurrentPlanet->CurrentStar->Constellation->Id != 20 || static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1) && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), aPlayer::GetPlayer()->CurrentPlanet->OwnerId)) {
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QueryQuest"_wref.get()), 0, pas::bind_method<&TfGov::RequestQuest>(this));
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlNormal && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            AddChoice(aConst::PickLocalizedTextVariant(u"FormGov.Bribe.I_Bribe"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 239516), 0, pas::bind_method<&TfGov::ShowBribeOffer>(this));
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->FindUnchartedNeighborConstellation() != nullptr) {
            AddChoice(aConst::LocalizedColorText(u"FormGov.BuyMap.I_BuyMap"_wref.get()), 0, pas::bind_method<&TfGov::ShowMapOffer>(this));
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->CountBailablePrisoners() > 0) {
            AddChoice(aConst::LocalizedColorText(u"FormGov.GuarantPrison.PlayerAsk"_wref.get()), 0, pas::bind_method<&TfGov::ShowPrisonBail>(this));
        }
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
    }

    void TfGov::ContinueScriptDialog() {
        Globals::ScriptDialogIndex = -1;
        ++ScriptDialogCursor;
        aScript::TScript* Script = nullptr;
        while (ScriptDialogCursor < ScriptDialogNames->GetCount()) {
            Script = static_cast<aScript::TScript*>(ScriptDialogNames->GetDataAt(ScriptDialogCursor));
            aScript::TScript_CallDialogByVariable(Script, ScriptDialogNames->GetTextAt(ScriptDialogCursor));
            if (Globals::ScriptDialogIndex >= 0) {
                break;
            }
            ++ScriptDialogCursor;
        }
        if (Globals::ScriptDialogIndex < 0) {
            BuildGovernmentChoices(true);
        } else {
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_Continue"_wref.get()), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script)), pas::bind_method<&TfGov::StartScriptMessage>(this));
        }
    }

    void TfGov::AddScriptTakeoffChoice(pas::WideString Caption) {
        AddChoice(Caption, aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptTakeoffAnswer>(this));
    }

    void TfGov::AddScriptPlanetChoice(pas::WideString Caption) {
        AddChoice(Caption, aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptPlanetAnswer>(this));
    }

    void TfGov::AddScriptGoodsChoice(pas::WideString Caption) {
        AddChoice(Caption, aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptGoodsAnswer>(this));
    }

    void TfGov::AddScriptShopChoice(pas::WideString Caption) {
        AddChoice(Caption, aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptShopAnswer>(this));
    }

    void TfGov::AddScriptHangarChoice(pas::WideString Caption) {
        AddChoice(Caption, aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptHangarAnswer>(this));
    }

    void TfGov::AddScriptNewsExitChoice(pas::WideString Caption) {
        AddChoice(Caption, aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptNewsExitAnswer>(this));
    }

    void TfGov::BuildQuestOfferChoices() {
        ClearDialogChoices();
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestAccept"_wref.get()), 0, pas::bind_method<&TfGov::AcceptQuest>(this));
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestReject"_wref.get()), 0, pas::bind_method<&TfGov::RejectQuest>(this));
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestEasy"_wref.get()), 0, pas::bind_method<&TfGov::MakeQuestEasier>(this));
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestDifficult"_wref.get()), 0, pas::bind_method<&TfGov::MakeQuestHarder>(this));
        if (QuestOffer.QuestType == aGalaxyStruct::qtPlanetQuest) {
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_PlanetQuestClose"_wref.get()), 0, pas::bind_method<&TfGov::PermanentlyDeclineQuest>(this));
        }
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
    }

    void TfGov::StartScriptMessage(std::int32_t Action) {
        ClearDialogChoices();
        aScript::CurrentScript = reinterpret_cast<aScript::TScript*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
    }

    void TfGov::RunScriptAnswer(std::int32_t Answer) {
        ClearDialogChoices();
        Globals::ScriptDialogIndex = -1;
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        if (Globals::ScriptDialogIndex < 0) {
            GR_Main::RaiseWideMessage(u"I_Script"_wref.get());
        }
        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
    }

    void TfGov::RunScriptAnswerKeepingScroll(std::int32_t Answer) {
        RememberChoiceScroll();
        RunScriptAnswer(Answer);
    }

    void TfGov::RunScriptTakeoffAnswer(std::int32_t Answer) {
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        if (!fHangar::TfHangar::TryTakeOff()) {
            GlobalsV::RequestedScreenId = GlobalsV::screenHangar;
        }
        RequestClose(1);
    }

    void TfGov::RunScriptPlanetAnswer(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
        RequestClose(1);
    }

    void TfGov::RunScriptGoodsAnswer(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        GlobalsV::RequestedScreenId = GlobalsV::screenGoodsShop;
        RequestClose(1);
    }

    void TfGov::RunScriptShopAnswer(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        GlobalsV::RequestedScreenId = GlobalsV::screenEquipmentShop;
        RequestClose(1);
    }

    void TfGov::RunScriptHangarAnswer(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        GlobalsV::RequestedScreenId = GlobalsV::screenHangar;
        RequestClose(1);
    }

    void TfGov::RunScriptNewsExitAnswer(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        GlobalsV::RequestedScreenId = GlobalsV::screenInfo;
        RequestClose(1);
    }

    void TfGov::EnterPrison(std::int32_t Action) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        aPlayer::GetPlayer()->InPrison = true;
        aPlayer::GetPlayer()->CurrentSystemKills.Normal = 0;
        aPlayer::GetPlayer()->CurrentSystemKills.Pirate = 0;
        if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            if (aPlanet::MainPiratePlanet != nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 80);
            } else {
                aPlayer::GetPlayer()->CurrentPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 80);
            }
        } else {
            aPlayer::GetPlayer()->CurrentPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 80);
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmRaiseTo, 20, pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition));
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), aPlayer::GetPlayer()->CurrentStar->Constellation, aRanger::rcmIncrease, 30, pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if ((Ship->TypeId == aGalaxyStruct::stWarrior || Ship->TypeId == aGalaxyStruct::stPirate && reinterpret_cast<aPirate::TPirate*>(Ship)->PirateType != 0) && aPlayer::GetPlayer() == Ship->EnemyShip) {
                Ship->EnemyShip = nullptr;
                if (aPlayer::GetPlayer() == Ship->OrderTarget) {
                    Ship->OrderNone(false);
                }
            }
        }
        Globals::StandaloneQuestMode = false;
        GlobalsV::QuestReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanetQuest;
        RequestClose(1);
    }

    void TfGov::ContinueAfterPrison(std::int32_t Action) {
        if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            DialogText = aConst::LocalizedColorText(u"FormGov.Prison.GovAfterPrisonNext"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormGov.PirateClanPrison.GovAfterPrisonNext"_wref.get());
        }
        BuildGovernmentChoices(true);
    }

    void TfGov::ShowBribeOffer(std::int32_t Action) {
        std::int32_t Cost{};
        std::int32_t RelationDeficit{};
        pas::WideString Text{};
        if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            RelationDeficit = 100 - (aPlayer::GetPlayer()->CurrentPlanet->RelationToShip(aPlayer::GetPlayer()) & 0x0000007f);
            Cost = System::Round(static_cast<long double>(aMyFunction::RemapClamped(RelationDeficit, 0.0, 1.0E+2, 1.0, 5.0)) * (aGalaxy::Galaxy->AverageRangerCapital / 100) * aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].FuelPriceFactor);
            Text = aConst::PickLocalizedTextVariant(u"FormGov.Bribe.Question"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 223429);
            aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
            DialogText = Text;
            ClearDialogChoices();
            if (aPlayer::GetPlayer()->Money >= Cost) {
                AddChoice(([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(Cost);
                    auto name = pas::borrow(aPlayer::GetPlayer()->CurrentPlanet->Name);
                    pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"FormGov.Bribe.Ok"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 5 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 8168236);
                    return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr), u"<Planet>"_w, name.get());
                }()), 0, pas::bind_method<&TfGov::PayBribe>(this));
            }
            AddChoice(aConst::PickLocalizedTextVariant(u"FormGov.Bribe.No"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 5 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 23985), 0, pas::bind_method<&TfGov::DeclineBribe>(this));
        } else {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.Bribe.GotoPB"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 112);
            BuildGovernmentChoices(true);
        }
    }

    void TfGov::PayBribe(std::int32_t Action) {
        std::int32_t RelationDeficit = 100 - (aPlayer::GetPlayer()->CurrentPlanet->RelationToShip(aPlayer::GetPlayer()) & 0x0000007f);
        std::int32_t Cost = System::Round(static_cast<long double>(aMyFunction::RemapClamped(RelationDeficit, 0.0, 1.0E+2, 1.0, 5.0)) * (aGalaxy::Galaxy->AverageRangerCapital / 100) * aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].FuelPriceFactor);
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            if (aPlanet::MainPiratePlanet != nullptr) {
                aPlanet::MainPiratePlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
            } else {
                aPlayer::GetPlayer()->CurrentPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
            }
        } else {
            aPlayer::GetPlayer()->CurrentPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), aPlayer::GetPlayer()->CurrentStar, aRanger::rcmIncrease, 20, pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition));
        }
        DialogText = aConst::PickLocalizedTextVariant(u"FormGov.Bribe.QuestionOk"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 5 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 7156317);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
        BuildGovernmentChoices(true);
    }

    void TfGov::DeclineBribe(std::int32_t Action) {
        DialogText = aConst::PickLocalizedTextVariant(u"FormGov.Bribe.QuestionNo"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 5 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 92874253);
        BuildGovernmentChoices(true);
    }

    void TfGov::RequestQuest(std::int32_t Action) {
        pas::WideString ResponseText{};
        std::int32_t MapIndex{};
        std::int32_t MapId{};
        if (aPlayer::GetPlayer()->CurrentPlanet->CurrentStar->Status.Battle != 0 && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.WarInSystemPirate"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 5 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 118123);
            BuildGovernmentChoices(true);
        } else if (aPlayer::GetPlayer()->CurrentPlanet->CurrentStar->Status.Battle != 0) {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.WarInSystem"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 5 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 118123);
            BuildGovernmentChoices(true);
        } else {
            if (aPlayer::GetPlayer()->DeclinePlanetBattleOffers) {
                MapId = -1;
            } else {
                MapId = aPlayer::GetPlayer()->SelectPlanetBattleMap();
            }
            if (MapId >= 0 && GlobalsV::ForcedPlanetQuestId < 0) {
                MapIndex = Globals::FindRobotMapById(MapId);
                DialogText = Globals::RobotMapDefinitions[MapIndex].GovTextStart;
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Planet>"_w, aPlayer::GetPlayer()->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                ClearDialogChoices();
                if (Robot::RobotInterface != nullptr && Robot::RobotInterface->Support() == 0) {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfGov::ShowPlanetBattleSupport>(this);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormGov.I_QuestAccept"_wref.get());
                    TfGov* self = this;
                    self->AddChoice(std::move(localizedColorText), MapId, cpp_arg);
                } else {
                    GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormGov.I_QuestAccept"_wref.get());
                    TfGov* self_2 = this;
                    self_2->AddChoice(std::move(localizedColorText_2), 0, scriptDialogBlockCallback);
                }
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_2 = pas::bind_method<&TfGov::DeclinePlanetBattle>(this);
                    pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormGov.I_PlanetBattleQuestClose"_wref.get());
                    TfGov* self_3 = this;
                    self_3->AddChoice(std::move(localizedColorText_3), MapId, cpp_arg_2);
                }
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfGov::ConfirmDeclineAllPlanetBattles>(this);
                    pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormGov.I_PlanetBattleRejectAll"_wref.get());
                    TfGov* self_4 = this;
                    self_4->AddChoice(std::move(localizedColorText_4), 0, cpp_arg_3);
                }
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfGov::ReturnToPlanet>(this);
                    pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get());
                    TfGov* self_5 = this;
                    self_5->AddChoice(std::move(localizedColorText_5), 0, cpp_arg_4);
                }
            } else if (!aRanger::TRanger_GenerateQuestOffer(aPlayer::GetPlayer(), QuestOffer, ResponseText)) {
                DialogText = ResponseText;
                BuildGovernmentChoices(true);
            } else {
                QuestNegotiationLevel = 0;
                DialogText = aPlayer::GetPlayer()->BuildQuestText(QuestOffer, aRanger::qtkOffer);
                if (QuestOffer.QuestType == aGalaxyStruct::qtPlanetQuest && QuestOffer.QuestNumber >= 10000) {
                    if (GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"_wref.get())->CountBlocks(u"PlanetQuestLic"_wref.get()) <= 0 || ([&] {
                        pas::WideString cpp_string = ([&] {
                            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(QuestOffer.QuestNumber));
                            EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"PlanetQuest"_wref.get())->GetBlock(u"PlanetQuestLic"_wref.get());
                            return block->GetParamOrMarker(intToStr);
                        }());
                        pas::WideString cpp_string_2 = fPlanetQuest::TfPlanetQuest::GetQuestContentHash(QuestOffer.QuestNumber);
                        return cpp_string != cpp_string_2;
                    }())) {
                        DialogText = pas::concat_wide({DialogText, u"\r\n", u" ", u"\r\n", aConst::LocalizedText(u"FormGov.QuestCertificate.NotCertificate"_wref.get())});
                    }
                }
                QuestRewardStep = System::Round(QuestOffer.RewardMoney * 0.3L);
                QuestDurationStep = System::Round((QuestOffer.DeadlineTurn - aGalaxy::Galaxy->CurrentTurn) * 0.5L);
                BuildQuestOfferChoices();
            }
        }
    }

    void TfGov::MakeQuestEasier(std::int32_t Action) {
        --QuestNegotiationLevel;
        {
            aGalaxyStruct::TQuestType cpp_case = QuestOffer.QuestType;
            if (cpp_case >= aGalaxyStruct::qtSendLetter && cpp_case <= aGalaxyStruct::qtPlanetQuest) {
                QuestOffer.DeadlineTurn += QuestDurationStep;
            } else if (cpp_case >= aGalaxyStruct::qtDefendSystem && cpp_case <= aGalaxyStruct::qtDefendShip) {
                QuestOffer.DeadlineTurn -= QuestDurationStep;
            }
        }
        QuestOffer.RewardMoney -= QuestRewardStep;
        DialogText = pas::concat_wide({aConst::LocalizedColorText(u"FormGov.CheckQuest.Easy"_wref.get()), u"\r\n", aPlayer::GetPlayer()->BuildQuestText(QuestOffer, aRanger::qtkOffer)});
        if (QuestNegotiationLevel <= -1) {
            ClearDialogChoices();
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestAccept"_wref.get()), 0, pas::bind_method<&TfGov::AcceptQuest>(this));
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestReject"_wref.get()), 0, pas::bind_method<&TfGov::RejectQuest>(this));
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestDifficult"_wref.get()), 0, pas::bind_method<&TfGov::MakeQuestHarder>(this));
            if (QuestOffer.QuestType == aGalaxyStruct::qtPlanetQuest) {
                AddChoice(aConst::LocalizedColorText(u"FormGov.I_PlanetQuestClose"_wref.get()), 0, pas::bind_method<&TfGov::PermanentlyDeclineQuest>(this));
            }
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
        } else {
            BuildQuestOfferChoices();
        }
    }

    void TfGov::MakeQuestHarder(std::int32_t Action) {
        ++QuestNegotiationLevel;
        {
            aGalaxyStruct::TQuestType cpp_case = QuestOffer.QuestType;
            if (cpp_case >= aGalaxyStruct::qtSendLetter && cpp_case <= aGalaxyStruct::qtPlanetQuest) {
                QuestOffer.DeadlineTurn -= QuestDurationStep;
            } else if (cpp_case >= aGalaxyStruct::qtDefendSystem && cpp_case <= aGalaxyStruct::qtDefendShip) {
                QuestOffer.DeadlineTurn += QuestDurationStep;
            }
        }
        QuestOffer.RewardMoney += QuestRewardStep;
        DialogText = pas::concat_wide({aConst::LocalizedColorText(u"FormGov.CheckQuest.Difficult"_wref.get()), u"\r\n", aPlayer::GetPlayer()->BuildQuestText(QuestOffer, aRanger::qtkOffer)});
        if (QuestNegotiationLevel >= 1) {
            ClearDialogChoices();
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestAccept"_wref.get()), 0, pas::bind_method<&TfGov::AcceptQuest>(this));
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestReject"_wref.get()), 0, pas::bind_method<&TfGov::RejectQuest>(this));
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_QuestEasy"_wref.get()), 0, pas::bind_method<&TfGov::MakeQuestEasier>(this));
            if (QuestOffer.QuestType == aGalaxyStruct::qtPlanetQuest) {
                AddChoice(aConst::LocalizedColorText(u"FormGov.I_PlanetQuestClose"_wref.get()), 0, pas::bind_method<&TfGov::PermanentlyDeclineQuest>(this));
            }
            AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
        } else {
            BuildQuestOfferChoices();
        }
    }

    void TfGov::AcceptQuest(std::int32_t Action) {
        aRanger::PQuest Quest{};
        aItem::TUselessItem* Item{};
        pas::new_value(Quest);
        *Quest = QuestOffer;
        Quest->Description = aPlayer::GetPlayer()->BuildQuestText(*Quest, aRanger::qtkOffer);
        Quest->CompletionText = aPlayer::GetPlayer()->BuildQuestText(*Quest, aRanger::qtkCompletion);
        pas::list_add(aPlayer::GetPlayer()->Quests, static_cast<void*>(Quest));
        static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::PublishQuestStatus(Quest, 0);
        if (Quest->QuestType == aGalaxyStruct::qtSendLetter) {
            Item = pas::construct_call<aItem::TUselessItem>(aItem::TUselessItem_Create);
            Item->Init(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest->QuestNumber), ".SysName"}))), aGalaxyStruct::dsBlazer, 0u, false);
            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
        }
        DialogText = aConst::PickLocalizedTextVariant(u"FormGov.AfterPlayerTakeQuest"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 902983);
        ClearDialogChoices();
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
    }

    void TfGov::RejectQuest(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormGov.PlayerDontTakeQuest"_wref.get());
        ClearDialogChoices();
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
    }

    void TfGov::PermanentlyDeclineQuest(std::int32_t Action) {
        aRanger::PPlayerOldQuest Quest{};
        DialogText = aConst::LocalizedColorText(u"FormGov.GovAfterPlanetQuestClose"_wref.get());
        pas::new_value(Quest);
        Quest->QuestType = QuestOffer.QuestType;
        Quest->QuestNumber = QuestOffer.QuestNumber;
        Quest->Planet = QuestOffer.Planet;
        Quest->Description = u"PlanetQuestClose"_w;
        Quest->Successful = false;
        Quest->Declined = true;
        pas::list_add(aRanger::PlayerOldQuests, static_cast<void*>(Quest));
        BuildGovernmentChoices(true);
    }

    void TfGov::DeclinePlanetBattle(std::int32_t Action) {
        PlanetBattleMapId = Action;
        DialogText = aConst::LocalizedColorText(u"FormGov.GovAfterBattlePlanetQuestClose"_wref.get());
        aPlayer::GetPlayer()->PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1 + 1);
        {
            aPlayer::TPlanetBattleHistoryEntry& cpp_with = aPlayer::GetPlayer()->PlanetBattleHistory[aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1];
            cpp_with.MapId = PlanetBattleMapId;
            pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.Statistics, 0 * sizeof(std::int32_t)), 0);
            pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.Statistics, 1 * sizeof(std::int32_t)), 0);
            pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.Statistics, 2 * sizeof(std::int32_t)), 0);
            pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.Statistics, 3 * sizeof(std::int32_t)), 0);
            pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.Statistics, 4 * sizeof(std::int32_t)), 0);
            pas::store_unaligned<std::int32_t>(pas::byte_offset(&cpp_with.Statistics, 5 * sizeof(std::int32_t)), 0);
            cpp_with.ResultCode = 1;
            cpp_with.CompletionMode = 0;
            cpp_with.DateTurn = aGalaxy::Galaxy->CurrentTurn;
        }
        aPlayer::GetPlayer()->LastPlanetBattleTurn = aGalaxy::Galaxy->CurrentTurn;
        BuildGovernmentChoices(true);
    }

    void TfGov::ConfirmDeclineAllPlanetBattles(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormGov.I_PlanetBattleRejectAllConfirm"_wref.get());
        ClearDialogChoices();
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_PlanetBattleRejectAllConfirmYes"_wref.get()), Action, pas::bind_method<&TfGov::DeclineAllPlanetBattles>(this));
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_PlanetBattleRejectAllConfirmNo"_wref.get()), Action, pas::bind_method<&TfGov::CancelDeclineAllPlanetBattles>(this));
    }

    void TfGov::DeclineAllPlanetBattles(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormGov.I_PlanetBattleRejectAllConfirmAfterYes"_wref.get());
        aPlayer::GetPlayer()->DeclinePlanetBattleOffers = true;
        BuildGovernmentChoices(true);
    }

    void TfGov::CancelDeclineAllPlanetBattles(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormGov.I_PlanetBattleRejectAllConfirmAfterNo"_wref.get());
        BuildGovernmentChoices(true);
    }

    void TfGov::ShowPlanetBattleSupport(std::int32_t Action) {
        std::int32_t MapIndex = Globals::FindRobotMapById(Action);
        if (Globals::RobotMapDefinitions[MapIndex].ReinforcementsDisabled) {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.GovBeforeBattleNoReinforcements"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 81263);
        } else {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.GovBeforeBattle"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 81263);
        }
        ClearDialogChoices();
        AddChoice(aConst::LocalizedColorText(u"FormGov.PlanetBattle.PlayerBeforeBattleNone"_wref.get()), Action, pas::bind_method<&TfGov::StartPlanetBattleWithoutSupport>(this));
        if (!Globals::RobotMapDefinitions[MapIndex].ReinforcementsDisabled) {
            AddChoice(aConst::LocalizedColorText(u"FormGov.PlanetBattle.PlayerBeforeBattleHelp"_wref.get()), Action, pas::bind_method<&TfGov::StartPlanetBattleWithReinforcements>(this));
        }
        AddChoice(aConst::LocalizedColorText(u"FormGov.PlanetBattle.PlayerBeforeBattleDamage"_wref.get()), Action, pas::bind_method<&TfGov::StartPlanetBattleWithBombardment>(this));
    }

    void TfGov::StartPlanetBattleWithoutSupport(std::int32_t Action) {
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        GovernmentBattleDifficulty = 1;
        PlanetBattleMapId = Action;
        PendingTransition = 1;
        GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
        LoadPanel->SelectBackgroundStyle(3);
        LoadPanel->RefreshBackgroundImages();
        LoadPanel->StartClosingShutters();
    }

    void TfGov::StartPlanetBattleWithReinforcements(std::int32_t Action) {
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        GovernmentBattleDifficulty = 2;
        PlanetBattleMapId = Action;
        PendingTransition = 1;
        GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
        LoadPanel->SelectBackgroundStyle(3);
        LoadPanel->RefreshBackgroundImages();
        LoadPanel->StartClosingShutters();
    }

    void TfGov::StartPlanetBattleWithBombardment(std::int32_t Action) {
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        GovernmentBattleDifficulty = 3;
        PlanetBattleMapId = Action;
        PendingTransition = 1;
        GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
        LoadPanel->SelectBackgroundStyle(3);
        LoadPanel->RefreshBackgroundImages();
        LoadPanel->StartClosingShutters();
    }

    void TfGov::DeclineBattleAndLeave(std::int32_t Action) {
        DialogText = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.GovBeforeNormal"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 81263);
        ClearDialogChoices();
        AddChoice(aConst::LocalizedColorText(u"FormGov.I_Exit"_wref.get()), 0, pas::bind_method<&TfGov::ReturnToPlanet>(this));
    }

    void TfGov::ChoosePrisonInsteadOfBattle(std::int32_t Action) {
        aPlayer::GetPlayer()->CurrentPlanet->SetRelationLevelToRanger(aPlayer::GetPlayer(), aGalaxyStruct::rlHostile);
        if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.GovBeforePrisonPirateClan"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 81263);
            ClearDialogChoices();
            AddChoice(aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.PlayerGoToPrisonPirateClan"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 61253), 0, pas::bind_method<&TfGov::EnterPrison>(this));
        } else {
            DialogText = aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.GovBeforePrison"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 81263);
            ClearDialogChoices();
            AddChoice(aConst::PickLocalizedTextVariant(u"FormGov.PlanetBattle.PlayerGoToPrison"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 61253), 0, pas::bind_method<&TfGov::EnterPrison>(this));
        }
    }

    void TfGov::ShowMapOffer(std::int32_t Action) {
        std::int32_t Cost = aMyFunction::RoundAndTruncateToTens(std::min<std::int32_t>(aPlayer::GetPlayer()->Wealth / 40, aGalaxy::Galaxy->ComputeScaledBigMoney(aPlayer::GetPlayer()->CurrentPlanet->OwnerId)) + 50);
        DialogText = ([&] {
            pas::WideString name = pas::checked_cast<aGalaxy::TConstellation*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentPlanet->FindUnchartedNeighborConstellation()))->GetName();
            pas::WideString intToStr = pas::wide_int_to_str(Cost);
            pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"FormGov.BuyMap.GovAsk"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed);
            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name), u"<Money>"_w, std::move(intToStr));
        }());
        ClearDialogChoices();
        if (aPlayer::GetPlayer()->Money >= Cost) {
            AddChoice(aConst::LocalizedColorText(u"FormGov.BuyMap.PlayerOk"_wref.get()), 0, pas::bind_method<&TfGov::BuyMap>(this));
        }
        AddChoice(aConst::LocalizedColorText(u"FormGov.BuyMap.PlayerNO"_wref.get()), 0, pas::bind_method<&TfGov::DeclineMapOffer>(this));
    }

    void TfGov::BuyMap(std::int32_t Action) {
        std::int32_t Cost = aMyFunction::RoundAndTruncateToTens(std::min<std::int32_t>(aPlayer::GetPlayer()->Wealth / 40, aGalaxy::Galaxy->ComputeScaledBigMoney(aPlayer::GetPlayer()->CurrentPlanet->OwnerId)) + 50);
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
        pas::checked_cast<aGalaxy::TConstellation*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentPlanet->FindUnchartedNeighborConstellation()))->Visible = true;
        static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckMapBuilderAchievement();
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        DialogText = aConst::PickLocalizedTextVariant(u"FormGov.BuyMap.GovAfterOk"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 711);
        BuildGovernmentChoices(true);
    }

    void TfGov::DeclineMapOffer(std::int32_t Action) {
        DialogText = aConst::PickLocalizedTextVariant(u"FormGov.BuyMap.GovAfterNo"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->CurrentPlanet->GenerationSeed + 112);
        BuildGovernmentChoices(true);
    }

    void TfGov::ShowPrisonBail(std::int32_t Action) {
        pas::WideString Text{};
        pas::WideString RowText{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t Cost{};
        DialogText = aConst::LocalizedColorText(u"FormGov.GuarantPrison.PlanetAsk"_wref.get());
        pas::List* Ships = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if (aPlayer::GetPlayer()->CurrentPlanet == Ship->CurrentPlanet && (Ship->ScriptShip == nullptr || pas::checked_cast<aScript::TScriptShip*>(Ship->ScriptShip)->State->StateKind == aScript::sskNormalAI) && aShip::TShip_IsInPrison(Ship) && Ship->GetPrisonTermRemaining() > 0) {
                RowText = ([&] {
                    pas::WideString fullName = Ship->GetFullName(u" "_wref.get());
                    pas::WideString intToStr = pas::wide_int_to_str(Ship->GetPrisonReleaseCost());
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormGov.GuarantPrison.ShipRow"_wref.get());
                    return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Ship>"_w, std::move(fullName), u"<Cost>"_w, std::move(intToStr));
                }());
                Text = pas::concat_wide({Text, u"\r\n", u" - ", RowText});
                pas::list_add(Ships, reinterpret_cast<void*>(Ship));
            }
        }
        DialogText = pas::concat_wide({DialogText, Text});
        ClearDialogChoices();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (aShip::TShip_IsInPrison(Ship)) {
                Cost = Ship->GetPrisonReleaseCost();
                Text = aConst::LocalizedColorText(u"FormGov.GuarantPrison.PlayerOk"_wref.get());
                Text = ([&] {
                    pas::WideString fullName_2 = Ship->GetFullName(u" "_wref.get());
                    pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                    return aMyFunction::FormatText2(Text, u"<color=255,240,100>"_w, u"<Ship>"_w, std::move(fullName_2), u"<Cost>"_w, std::move(intToStr_2));
                }());
                if (aPlayer::GetPlayer()->Money >= Cost) {
                    AddChoice(Text, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfGov::PayPrisonBail>(this));
                } else {
                    AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
                }
            }
        }
        AddChoice(aConst::LocalizedColorText(u"FormGov.GuarantPrison.PlayerNo"_wref.get()), 0, pas::bind_method<&TfGov::CancelPrisonBail>(this));
        pas::free(Ships);
    }

    void TfGov::PayPrisonBail(std::int32_t Action) {
        aShip::TShip* Ship = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        if (aShip::TShip_IsInPrison(Ship)) {
            fGov::PayBailMoney(Ship);
            Ship->ClearPrisonTerm();
            Ship->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
            Ship->OrderTakeoff();
            ++aPlayer::GetPlayer()->AchievementStats->PrisonersBailedOut;
            Achievements::TrySetAchievementProgress(u"PRISONBAIL"_w, aPlayer::GetPlayer()->AchievementStats->PrisonersBailedOut);
        }
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        DialogText = aConst::LocalizedColorText(u"FormGov.GuarantPrison.PlanetAfterOk"_wref.get());
        BuildGovernmentChoices(true);
    }

    void TfGov::CancelPrisonBail(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormGov.GuarantPrison.PlanetAfterNo"_wref.get());
        BuildGovernmentChoices(true);
    }

    void TfGov::ReturnToPlanet(std::int32_t Action) {
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
        RequestClose(1);
    }

    void TfGov::ExitGovernment(std::int32_t Action) {
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
        RequestClose(1);
    }

    void TfGov::RunInjectedAnswer(std::int32_t Answer) {
        pas::WideString Text{};
        aScript::PScriptDialogInjection Injection = reinterpret_cast<aScript::PScriptDialogInjection>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Answer)));
        if (Injection->ActionCode != u"") {
            aScript::CurrentScript = Injection->ActionScript;
            aScript::ExecuteScriptText(Injection->ActionCode, aScript::CurrentScript->InitCode->LocalVar);
        }
        Text = Injection->Answer;
        std::int32_t PartCount = EC_Str::CountDelimitedPartsW(Text, u"~"_wref.get());
        if (PartCount > 1) {
            Text = EC_Str::ExtractDelimitedPartW(static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Text))), 0, u"~"_wref.get());
            if (Text == u"snap") {
                RememberChoiceScroll();
            }
        }
        ClearDialogChoices();
        aScript::CurrentScript = Injection->Script;
        {
            EC_Expression::TVarEC* var = aScript::CurrentScript->InitCode->LocalVar->GetVar(u"GAnswerData"_wref.get());
            std::uint32_t answerData = Injection->AnswerData;
            var->SetDword(answerData);
        }
        Globals::ScriptDialogIndex = -1;
        aScript::TScript_CallDialogByVariable(aScript::CurrentScript, Injection->DialogName);
        if (Globals::ScriptDialogIndex < 0) {
            BuildGovernmentChoices(true);
        } else {
            aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
        }
    }

    void TfGov::RunInjectedAnswerKeepingScroll(std::int32_t Answer) {
        RememberChoiceScroll();
        RunInjectedAnswer(Answer);
    }

    void TfGov::RunScriptRestartAnswer(std::int32_t Answer) {
        DialogText = pas::WideString();
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        RefreshGovernmentDialog();
    }

    void TfGov::AddScriptRestartChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfGov::RunScriptRestartAnswer>(this));
    }

    void TfGov::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (MainPanel->NavigationLocked) {
            return;
        }
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        if (pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10008);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20008);
        }
    }

    void TfGov::p_destroy() {
        fGov::TfGov_Destroy(this);
    }

} // namespace fGov
