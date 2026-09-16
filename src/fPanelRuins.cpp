#include "layout/fPanelRuins.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fSaveManager.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelRuins.hpp"

namespace fPanelRuins {
    void TfPanelRuins_Create(TfPanelRuins* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TfPanelRuins_Destroy(TfPanelRuins* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TfPanelRuins::InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen) {
        this->Screen = Screen;
        GR_Main::AppendLogTextThreadSafe("fPanelRuins... "_a);
        {
            GI_MessageLoop::TObjectGI* PanelRuins = this->Screen->GetByName(u"PanelRuins"_wref.get());
            PanelRuins->SetPosition(ClassesImports::Point(PanelRuins->LocalPosition.X + GR_Main::ExtraScreenWidth, PanelRuins->LocalPosition.Y + GR_Main::ExtraScreenHeight));
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PR_Gov"_wref.get()))->UpCallback = pas::bind_method<&TfPanelRuins::ServicesClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PR_Shop"_wref.get()))->UpCallback = pas::bind_method<&TfPanelRuins::EquipmentShopClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PR_Goods"_wref.get()))->UpCallback = pas::bind_method<&TfPanelRuins::GoodsShopClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PR_Info"_wref.get()))->UpCallback = pas::bind_method<&TfPanelRuins::InformationClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PR_Hangar"_wref.get()))->UpCallback = pas::bind_method<&TfPanelRuins::HangarClicked>(this);
    }

    void TfPanelRuins::OnOpen() {
        GI_MessageLoop::TObjectGI* PanelRuins = Screen->GetByName(u"PanelRuins"_wref.get());
        PanelRuins->SetActive(aPlayer::GetPlayer()->RuinsMode == 0);
    }

    // Native no-op lifecycle hook.
    void TfPanelRuins::OnClose() {
    }

    void TfPanelRuins::Show() {
        Screen->GetByName(u"PanelRuins"_wref.get())->SetActive(true);
    }

    void TfPanelRuins::Hide() {
        Screen->GetByName(u"PanelRuins"_wref.get())->SetActive(false);
    }

    void TfPanelRuins::ServicesClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
        Screen->RequestClose(1);
    }

    void TfPanelRuins::EquipmentShopClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->RuinsMode > 0 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter) && aPlayer::GetPlayer()->DebtDefaultCount > 1) {
            const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormRuins.BK.DebtNoAccess"_wref.get());
            GI_MessageLoop::TMessageLoopGI* screen = Screen;
            GI_MessageBox::ShowMessageBoxGI(screen, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenEquipmentShop;
            Screen->RequestClose(1);
        }
    }

    void TfPanelRuins::GoodsShopClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->RuinsMode > 0 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter) && aPlayer::GetPlayer()->DebtDefaultCount > 1) {
            const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormRuins.BK.DebtNoAccess"_wref.get());
            GI_MessageLoop::TMessageLoopGI* screen = Screen;
            GI_MessageBox::ShowMessageBoxGI(screen, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenGoodsShop;
            Screen->RequestClose(1);
        }
    }

    void TfPanelRuins::InformationClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->RuinsMode > 0 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter) && aPlayer::GetPlayer()->DebtDefaultCount > 1) {
            const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"FormRuins.BK.DebtNoAccess"_wref.get());
            GI_MessageLoop::TMessageLoopGI* screen = Screen;
            GI_MessageBox::ShowMessageBoxGI(screen, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenInfo;
            Screen->RequestClose(1);
        }
    }

    void TfPanelRuins::HangarClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->RuinsMode > 0 && Globals::RuinsTalkScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        GlobalsV::RequestedScreenId = GlobalsV::screenHangar;
        Screen->RequestClose(1);
    }

    void TfPanelRuins::TakeOffForStationTravel() {
        std::int32_t Index{};
        aGalaxy::Galaxy->CheckIntegrityChecksum(189);
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(Screen);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(Screen);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        if (GR_Main::MusicManager->CategoryOverride == u"") {
            GR_Main::MusicManager->RequestFadeOut();
        }
        aRanger::PlayerAutomaticControl = false;
        Globals::PruneExpiredPersistentPlayerMessages();
        aPlayer::GetPlayer()->OrderTakeoff();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(Index); ) {
            pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, Index)->RunTurnCode();
        }
        Globals::StarMapWeaponPanelOpen = false;
        Globals::FilmCameraFollow = true;
        aGalaxy::PlayerStar->RefreshSpaceObjectPositions();
        fEquipmentShop::RestoreTemporaryShopStock();
        aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 1);
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(3)) {
            aGalaxy::Galaxy->EnableDominatorSurfaces();
        } else {
            aGalaxy::Galaxy->DisableDominatorSurfaces();
        }
        aCalc::CalculatePlayerStarTurnAndWait();
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        aCalc::QueueGalaxyTurnCalculation();
        Globals::StarMapWeaponPanelOpen = false;
        Globals::StarMapScreen->ResumeMode = fStarMap::smrTurnFilm;
        Globals::ScreenLoadMode = 2;
        GlobalsV::PostLoadScreenId = GlobalsV::screenStarMap;
        GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
        if (fPanelLoad::ActiveLoadPanel != nullptr) {
            fPanelLoad::ActiveLoadPanel->SelectBackgroundStyle(0);
            fPanelLoad::ActiveLoadPanel->RefreshBackgroundImages();
            fPanelLoad::ActiveLoadPanel->StartClosingShutters();
        } else {
            Screen->RequestClose(1);
        }
    }

    void TfPanelRuins::ProcessKeyDown(std::int32_t Key) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (!aPlayer::GetPlayer()->IsDockedToShip()) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (Key == 'G' && Screen->GetByName(u"PR_Gov"_wref.get())->Active) {
            ServicesClicked(nullptr);
        } else if (Key == 'E' && Screen->GetByName(u"PR_Shop"_wref.get())->Active) {
            EquipmentShopClicked(nullptr);
        } else if (Key == 'T' && Screen->GetByName(u"PR_Goods"_wref.get())->Active) {
            GoodsShopClicked(nullptr);
        } else if (Key == 'I' && Screen->GetByName(u"PR_Info"_wref.get())->Active) {
            InformationClicked(nullptr);
        } else if (Key == 'H' && Screen->GetByName(u"PR_Hangar"_wref.get())->Active) {
            HangarClicked(nullptr);
        }
    }

    void TfPanelRuins::p_destroy() {
        fPanelRuins::TfPanelRuins_Destroy(this);
    }

} // namespace fPanelRuins
