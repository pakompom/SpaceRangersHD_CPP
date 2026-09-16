#include "layout/fGameMenu.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fSaveManager.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/ShellAPI.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fGameMenu.hpp"

namespace fGameMenu {
    void TfGameMenu::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fGameMenu... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* Resume_Parent = MainPanel->FindByNameRecursive(u"Resume"_wref.get())->Parent;
                Resume_Parent->SetPosition(ClassesImports::Point(Resume_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, Resume_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        GetByName(u"MainPanel"_wref.get())->LeftButtonUpCallback = pas::bind_method<&TfGameMenu::BackgroundMouseUp>(this);
        {
            GI_GraphButton::TGraphButtonGI* Resume = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Resume"_wref.get()));
            Resume->UpCallback = pas::bind_method<&TfGameMenu::ResumeClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Save = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Save"_wref.get()));
            Save->UpCallback = pas::bind_method<&TfGameMenu::SaveClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Load = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Load"_wref.get()));
            Load->UpCallback = pas::bind_method<&TfGameMenu::LoadClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Settings = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Settings"_wref.get()));
            Settings->UpCallback = pas::bind_method<&TfGameMenu::SettingsClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Help = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Help"_wref.get()));
            Help->UpCallback = pas::bind_static_method<&TfGameMenu::HelpClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Exit = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Exit"_wref.get()));
            Exit->UpCallback = pas::bind_method<&TfGameMenu::ExitClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* Close = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()));
            Close->UpCallback = pas::bind_method<&TfGameMenu::ResumeClicked>(this);
        }
        {
            GI_GraphButton::TGraphButtonGI* with_Achievements = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Achievements"_wref.get()));
            with_Achievements->UpCallback = pas::bind_method<&TfGameMenu::AchievementsClicked>(this);
        }
    }

    void TfGameMenu::OnOpen() {
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        ContentPanel->KeyDownCallback = pas::bind_method<&TfGameMenu::MainPanelKeyDown>(this);
    }

    void TfGameMenu::OnClose() {
        if (GR_Main::AuxRenderBuffer != nullptr) {
            GR_Main::AuxRenderBuffer->Clear();
        }
    }

    void TfGameMenu::ResumeClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::RequestedScreenId = GlobalsV::GameMenuReturnScreenId;
        RequestClose(1);
    }

    void TfGameMenu::SaveClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aGalaxy::Galaxy->IronWill) {
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormGameSet2.IronWillText"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
        } else if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
            GlobalsV::SaveManagerReturnScreenId = GlobalsV::GameMenuReturnScreenId;
            Globals::SaveManagerMode = fSaveManager::smmSave;
            GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
            RequestClose(1);
        }
    }

    void TfGameMenu::LoadClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::GameMenuReturnScreenId;
        Globals::SaveManagerMode = fSaveManager::smmLoad;
        GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
        RequestClose(1);
    }

    void TfGameMenu::SettingsClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::SettingsReturnScreenId = GlobalsV::GameMenuReturnScreenId;
        GlobalsV::RequestedScreenId = GlobalsV::screenSettings;
        RequestClose(1);
    }

    void TfGameMenu::HelpClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::AnsiString cpp_text{};
        WindowsSdk::ShowWindow(GR_Main::MainWindowHandle, WindowsSdk::SW_MINIMIZE);
        ShellAPI::ShellExecuteA(0u, pas::literal_pointer("open"), (cpp_text = static_cast<pas::AnsiString>(aConst::LocalizedText(u"FormGameMenu.HelpFile"_wref.get())), cpp_text.pchar()), pas::literal_pointer(""), pas::literal_pointer(""), WindowsSdk::SW_SHOWNORMAL);
    }

    void TfGameMenu::ExitClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGameMenu.QExit"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
            if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
                pas::free(aSaveLoad::MemorySnapshotBuffer);
            }
            aSaveLoad::MemorySnapshotBuffer = nullptr;
            GlobalsV::MemorySnapshotActive = false;
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
                pas::free(aGalaxy::Galaxy);
            }
            aGalaxy::Galaxy = nullptr;
            EC_Cache::EvictRuinsAndGovernmentCaches();
            EC_Cache::EvictStarAndBackgroundCaches();
            GR_DX::ReleaseAllTextureSurfaces();
            Globals::ScreenLoadMode = 4;
            GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
            GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
            RequestClose(1);
        }
    }

    void TfGameMenu::AchievementsClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::AchievementsReturnScreenId = GlobalsV::GameMenuReturnScreenId;
        GlobalsV::RequestedScreenId = GlobalsV::screenAchievements;
        RequestClose(1);
    }

    void TfGameMenu::BackgroundMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (!pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"_wref.get()))->HitTestPixel(Point)) {
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Resume"_wref.get()))->ContainsPoint(Point)) {
                if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Save"_wref.get()))->ContainsPoint(Point)) {
                    if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Load"_wref.get()))->ContainsPoint(Point)) {
                        if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Settings"_wref.get()))->ContainsPoint(Point)) {
                            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Help"_wref.get()))->ContainsPoint(Point)) {
                                if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Exit"_wref.get()))->ContainsPoint(Point)) {
                                    if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Close"_wref.get()))->ContainsPoint(Point)) {
                                        if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Achievements"_wref.get()))->ContainsPoint(Point)) {
                                            ResumeClicked(nullptr);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void TfGameMenu::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == WindowsSdk::VK_ESCAPE) {
                ResumeClicked(nullptr);
            } else if (Key == 'S' || Key == WindowsSdk::VK_F2) {
                SaveClicked(nullptr);
            } else if (Key == 'L' || Key == WindowsSdk::VK_F3) {
                LoadClicked(nullptr);
            } else if (Key == 'C') {
                SettingsClicked(nullptr);
            } else if (Key == 'E') {
                ExitClicked(nullptr);
            } else if (Key == 'H') {
                TfGameMenu::HelpClicked(nullptr);
            }
        }
    }

    void TfGameMenu::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (!GR_Main::ExitScreenLoop) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10013);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20013);
        }
    }

    void TfGameMenu::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        } else if (aPlayer::GetPlayer()->RuinsMode != 0) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
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
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (pas::in_set<7, 7, 12, 12>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
            if (GlobalsV::MusicInSpaceEnabled) {
                if (aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
                    Globals::StarMapScreen->BattleMusicSelected = true;
                    GR_Main::MusicManager->PlayCategory(u"Destroyer"_wref.get());
                } else {
                    Globals::StarMapScreen->BattleMusicSelected = false;
                    GR_Main::MusicManager->PlayCategory(u"StarMap"_wref.get());
                }
            } else {
                GR_Main::MusicManager->RequestFadeOut();
            }
        }
    }

} // namespace fGameMenu
