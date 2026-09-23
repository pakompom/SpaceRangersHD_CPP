#include "layout/fPanelPlanet.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/fGov.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelPlanet.hpp"

namespace fPanelPlanet {
    void TfPanelPlanet_Create(TfPanelPlanet* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TfPanelPlanet_Destroy(TfPanelPlanet* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TfPanelPlanet::InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen) {
        this->Screen = Screen;
        GR_Main::AppendLogTextThreadSafe("fPanelPlanet... "_a);
        GI_MessageLoop::TObjectGI* Panel = this->Screen->GetByName(u"PanelPlanet"sv);
        Panel->SetPosition(ClassesImports::Point(Panel->LocalPosition.X + GR_Main::ExtraScreenWidth, Panel->LocalPosition.Y + GR_Main::ExtraScreenHeight));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PP_Hangar"sv))->UpCallback = pas::bind_method<&TfPanelPlanet::HangarClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PP_Shop"sv))->UpCallback = pas::bind_method<&TfPanelPlanet::EquipmentShopClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PP_Goods"sv))->UpCallback = pas::bind_method<&TfPanelPlanet::GoodsShopClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PP_Gov"sv))->UpCallback = pas::bind_method<&TfPanelPlanet::GovernmentClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(this->Screen->GetByName(u"PP_Info"sv))->UpCallback = pas::bind_method<&TfPanelPlanet::InformationClicked>(this);
    }

    // Native no-op lifecycle hook.
    void TfPanelPlanet::OnOpen() {
    }

    // Native no-op lifecycle hook.
    void TfPanelPlanet::OnClose() {
    }

    void TfPanelPlanet::Show() {
        Screen->GetByName(u"PanelPlanet"sv)->SetActive(true);
    }

    void TfPanelPlanet::Hide() {
        Screen->GetByName(u"PanelPlanet"sv)->SetActive(false);
    }

    void TfPanelPlanet::HangarClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        GlobalsV::RequestedScreenId = GlobalsV::screenHangar;
        Screen->RequestClose(1);
    }

    void TfPanelPlanet::EquipmentShopClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                const pas::WideString& replaceColoredToken = ([&] {
                    auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), textHighlightColorTag.get());
                }());
                GI_MessageLoop::TMessageLoopGI* screen = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen, replaceColoredToken, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                const pas::WideString& replaceColoredToken_2 = ([&] {
                    auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), textHighlightColorTag_2.get());
                }());
                GI_MessageLoop::TMessageLoopGI* screen_2 = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen_2, replaceColoredToken_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenEquipmentShop;
            Screen->RequestClose(1);
        }
    }

    void TfPanelPlanet::GoodsShopClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                const pas::WideString& replaceColoredToken = ([&] {
                    auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), textHighlightColorTag.get());
                }());
                GI_MessageLoop::TMessageLoopGI* screen = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen, replaceColoredToken, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                const pas::WideString& replaceColoredToken_2 = ([&] {
                    auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), textHighlightColorTag_2.get());
                }());
                GI_MessageLoop::TMessageLoopGI* screen_2 = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen_2, replaceColoredToken_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenGoodsShop;
            Screen->RequestClose(1);
        }
    }

    void TfPanelPlanet::GovernmentClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
        Screen->RequestClose(1);
    }

    void TfPanelPlanet::InformationClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                const pas::WideString& replaceColoredToken = ([&] {
                    auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPiratePlanetAndBadRelations"_wref.get());
                    pas::WideString name = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Planet>"_w, std::move(name), textHighlightColorTag.get());
                }());
                GI_MessageLoop::TMessageLoopGI* screen = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen, replaceColoredToken, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            } else {
                const pas::WideString& replaceColoredToken_2 = ([&] {
                    auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormShip.SellOrBuyInPlanetAndBadRelations"_wref.get());
                    pas::WideString name_2 = aPlayer::GetPlayer()->CurrentPlanet->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Planet>"_w, std::move(name_2), textHighlightColorTag_2.get());
                }());
                GI_MessageLoop::TMessageLoopGI* screen_2 = Screen;
                GI_MessageBox::ShowMessageBoxGI(screen_2, replaceColoredToken_2, GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }
        } else {
            GlobalsV::RequestedScreenId = GlobalsV::screenInfo;
            Screen->RequestClose(1);
        }
    }

    void TfPanelPlanet::PlanetClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aPlayer::GetPlayer()->PendingDockDialogue > 1 && Globals::GovernmentScreen == Screen) {
            return;
        }
        if (aScript::HasPendingScriptRequests()) {
            return;
        }
        GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
        Screen->RequestClose(1);
    }

    void TfPanelPlanet::ProcessKeyDown(std::int32_t Key) {
        if (Screen->ExitCode != 0) {
            return;
        }
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (!aPlayer::GetPlayer()->IsOnPlanet()) {
            return;
        }
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->IsAnimatingShutters()) {
            return;
        }
        if (Key == 'H') {
            HangarClicked(nullptr);
        } else if (Key == 'E') {
            EquipmentShopClicked(nullptr);
        } else if (Key == 'T') {
            GoodsShopClicked(nullptr);
        } else if (Key == 'G') {
            GovernmentClicked(nullptr);
        } else if (Key == 'I') {
            InformationClicked(nullptr);
        } else if (Key == 'P') {
            PlanetClicked(nullptr);
        }
    }

    void TfPanelPlanet::p_destroy() {
        fPanelPlanet::TfPanelPlanet_Destroy(this);
    }

} // namespace fPanelPlanet
