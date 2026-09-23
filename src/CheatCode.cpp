#include "layout/CheatCode.hpp"
#include "types/EC_Expression.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Process.hpp"
#include "types/SE_Space.hpp"
#include "types/ThreadCalc.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aNormalShip.hpp"
#include "types/aShip.hpp"
#include "types/aTranclucator.hpp"
#include "types/ab_Hit.hpp"
#include "types/ab_Object.hpp"
#include "types/fAbout.hpp"
#include "types/fGameSettings2.hpp"
#include "types/fGoodsShop2.hpp"
#include "types/fHangar.hpp"
#include "types/fInfo.hpp"
#include "types/fPanelMain.hpp"
#include "types/fPlanet.hpp"
#include "types/fPlanetNO.hpp"
#include "types/fRating2.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fSaveManager.hpp"
#include "types/fScaner.hpp"
#include "types/fScore.hpp"
#include "types/fStarMap.hpp"
#include "units/CheatCode.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aRuins.hpp"
#include "units/aScript.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Ship.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fListBox.hpp"
#include "units/fShip2.hpp"
#include "units/fTextBox.hpp"

// TCheatList VMT and its registration initializer identify the native cheat-code unit.
// Command identities and encoded spellings come from the native registration order.
namespace CheatCode {
    std::int32_t CheatCandidateIndex = 0;

    std::int32_t CheatPrefixLength = 0;

    std::int32_t CheatPrefixSum = 0;

    CheatCode::TCheatList* CheatEntries{};

    std::uint8_t IsCheatMessageBoxOpen() {
        pas::Object* Instance{};
        std::int32_t Index{};
        std::uint8_t Result = false;
        {
            const std::int32_t cpp_first = pas::list_count(GI_MessageLoop::MessageLoopStack) - 1;
            if (cpp_first >= 0) {
                for (Index = cpp_first; Index >= 0; --Index) {
                    Instance = pas::list_at<pas::Object>(GI_MessageLoop::MessageLoopStack, Index);
                    if (pas::class_cast_if<GI_MessageBox::TMessageBoxGI*>(Instance) != nullptr) {
                        Result = true;
                        break;
                    }
                }
            }
        }
        return Result;
    }

    // Ctrl+Shift key callback installed by Rangers. Ignores input during message boxes, disabled cheats and active calculation phases.
    void HandleDebugKey(std::uint16_t Key) {
        auto SumCheatPrefix = [&](std::int32_t Index, std::int32_t Count) -> std::int32_t {
            std::int32_t Position{};
            std::int32_t Result = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(Position); ) {
                Result += CheatEntries->GetEntry(Index)->Text.read(Position);
            }
            return Result;
        };
        if (CheatCode::IsCheatMessageBoxOpen()) {
            return;
        }
        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->CheatsDisabled) {
            return;
        }
        // The native subtract/test chain is DCC32 set membership, not a case statement.
        if (!pas::in_set<0, 0, 2, 2, 4, 4, 6, 6>(static_cast<std::int32_t>(aCalc::TurnCalculationPhase))) {
            return;
        }
        while (true) {
            if (pas::list_count(CheatEntries) - 1 < CheatCandidateIndex) {
                CheatCandidateIndex = 0;
                CheatPrefixLength = 0;
                CheatPrefixSum = 0;
                return;
            }
            if (CheatEntries->GetEntry(CheatCandidateIndex)->Text.length() >= CheatPrefixLength + 1 && CheatEntries->GetEntry(CheatCandidateIndex)->Text.read(CheatPrefixLength + 1) == Key && SumCheatPrefix(CheatCandidateIndex, CheatPrefixLength) == CheatPrefixSum) {
                break;
            }
            ++CheatCandidateIndex;
        }
        ++CheatPrefixLength;
        CheatPrefixSum += Key;
        if (CheatEntries->GetEntry(CheatCandidateIndex)->Text.length() == CheatPrefixLength) {
            if (pas::list_count(CheatEntries) > CheatCandidateIndex) {
                CheatEntries->GetEntry(CheatCandidateIndex)->Callback();
            }
            CheatCandidateIndex = 0;
            CheatPrefixLength = 0;
            CheatPrefixSum = 0;
        }
    }

    // Nested in HandleDebugKey; the native parent frame is passed but not read.
    // Actions registered by the native unit initializer.
    void AddCheatPoints(std::int32_t Points) {
        std::int32_t PreviousPoints = aGalaxy::Galaxy->GetCheatPoints();
        aGalaxy::Galaxy->SetCheatPoints(PreviousPoints + Points);
        if (PreviousPoints == 0 && Points > 0) {
            aGalaxy::TGalaxy::AppendIntegritySnapshot();
        }
    }

    void ReportCheat(std::int32_t Points, const pas::WideString& Name) {
        pas::WideString Text{};
        GI_MessageLoop::TMessageLoopGI* Parent{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        Text = aConst::LocalizedColorText(u"Cheat.Info"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, Name, aMyFunction::TextHighlightColorTag);
        aMyFunction::ReplaceTextToken(Text, u"<CheatPoints>"_w, pas::wide_int_to_str(Points), aMyFunction::TextHighlightColorTag);
        if (aGalaxy::Galaxy != nullptr) {
            CheatCode::AddCheatPoints(Points);
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerEntersCheatCode"_w, nullptr);
            Event->AddTextData(CheatEntries->GetEntry(CheatCandidateIndex)->Text);
            Event->AddData(Points);
        }
        Text = pas::concat_wide({Text, u"\r\n", aConst::LocalizedColorText(u"Cheat.Ok"_wref.get())});
        // Native code reads the total without a nil-galaxy guard.
        {
            auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
            pas::WideString intToStr = pas::wide_int_to_str(aGalaxy::Galaxy->GetCheatPoints());
            aMyFunction::ReplaceTextToken(Text, u"<AllPoints>"_w, std::move(intToStr), textHighlightColorTag.get());
        }
        if (Globals::ShipScreen->IsOpen) {
            Parent = Globals::ShipScreen;
        } else if (Globals::RangerRatingScreen->IsOpen) {
            Parent = Globals::RangerRatingScreen;
        } else {
            Parent = pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
        }
        GI_MessageBox::ShowMessageBoxGI(Parent, Text, GI_MessageBox::mbgCancel, 0, 0, 0);
        GR_Main::FullFrameRedrawRequested = true;
        Parent->InvalidateViewport();
        Parent->DrawQueuedUpdateRects();
        if (Globals::ShipScreen->IsOpen) {
            Globals::ShipScreen->ShipStateChanged = true;
            Globals::ShipScreen->ReopenRequested = true;
            Globals::ShipScreen->PlayTransitionSounds = false;
            Globals::ShipScreen->CloseClicked(nullptr);
        }
    }

    void CheatRepair() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        if (aGalaxy::Galaxy != nullptr && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->ItemType == aConst::t_Hull || Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1L) {
                    Item->Repair();
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Artefacts, I);
                if (Item->EquippedFlag != 0 && Item->ConditionPercent < 9.0E+1L) {
                    Item->Repair();
                }
            }
            CheatCode::ReportCheat(40, EC_Str::DecodeTextW(u"ROEMPOAYIURU"_w));
        }
    }

    void CheatKlissanmax() {
        std::int32_t PlanetIndex{};
        std::int32_t StarIndex{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(StarIndex); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, StarIndex);
                if (!(Star->Status.ControlFaction == aGalaxyStruct::sfDominators && Star->Status.CustomFaction == u"")) {
                    continue;
                }
                PlanetIndex = -1;
                while (true) {
                    if (Star->ShipTypeCounts[aGalaxyStruct::stKling] >= 12) {
                        break;
                    }
                    Planet = nullptr;
                    do {
                        ++PlanetIndex;
                        if (PlanetIndex >= pas::list_count(Star->Planets)) {
                            PlanetIndex = 0;
                        }
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, PlanetIndex);
                    } while (!(Planet->OwnerId != aGalaxyStruct::oiUninhabited));
                    Planet->SpawnWeightedDominatorShip();
                }
            }
            CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"KULTIZSOSOASNOMEANXI"_w));
        }
    }

    void CheatPiratemax() {
        std::int32_t PlanetIndex{};
        std::int32_t StarIndex{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        std::int32_t Created = 0;
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(StarIndex); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, StarIndex);
                if (!(Star->Status.ControlFaction == aGalaxyStruct::sfPirates && Star->Status.CustomFaction == u"")) {
                    continue;
                }
                PlanetIndex = -1;
                while (true) {
                    if (Star->CountPirateShips(true) >= 12) {
                        break;
                    }
                    Planet = nullptr;
                    do {
                        ++PlanetIndex;
                        if (PlanetIndex >= pas::list_count(Star->Planets)) {
                            PlanetIndex = 0;
                        }
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, PlanetIndex);
                    } while (!(Planet->OwnerId != aGalaxyStruct::oiUninhabited));
                    Planet->BuyWarrior(100);
                    ++Created;
                    if (Created >= 500) {
                        CheatCode::ShowCheatFeedback(u"Sudden break"_w);
                        break;
                    }
                }
            }
            CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"PVISREAXTMETMOARX9"_w));
        }
    }

    void CheatWarriormax() {
        std::int32_t PlanetIndex{};
        std::int32_t StarIndex{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        std::int32_t Created = 0;
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(StarIndex); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, StarIndex);
                if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"") {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(PlanetIndex); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, PlanetIndex);
                        if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                            while (static_cast<long double>(pas::list_count(Planet->Warriors)) < aMyFunction::RemapClamped(Planet->Radius, 6.0E+1, 1.0E+2, 2.0, 6.0) && Created < 500) {
                                Planet->BuyWarrior(100);
                                ++Created;
                            }
                        }
                    }
                }
            }
            CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"WIAGRARUILOIRAMOARX9"_w));
        }
    }

    void CheatKlissancall() {
        std::int32_t DistanceIndex{};
        std::int32_t ShipIndex{};
        std::int32_t Sent{};
        std::int32_t Eligible{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            Sent = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(DistanceIndex); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[DistanceIndex].Star));
                if (!(Star->Status.ControlFaction == aGalaxyStruct::sfDominators && Star->Status.Battle == 0 && aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) && Star->Status.CustomFaction == u"")) {
                    continue;
                }
                Eligible = 0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(ShipIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, ShipIndex);
                    if (Ship->OwnerId == aGalaxyStruct::oiDominator && Ship->Order == aShip::soNone && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1)) {
                        ++Eligible;
                    }
                }
                if (Eligible > 2) {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(ShipIndex); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, ShipIndex);
                        if (Ship->OwnerId == aGalaxyStruct::oiDominator && Ship->Order == aShip::soNone && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1)) {
                            Ship->OrderJump(aPlayer::GetPlayer()->CurrentStar, true);
                            ++Sent;
                            --Eligible;
                            if (Eligible <= 2) {
                                break;
                            }
                        }
                    }
                }
                if (Sent > 20) {
                    break;
                }
            }
            CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"KOLEINSOSUAINOCRABLELS"_w));
        }
    }

    void CheatPiratecall() {
        std::int32_t DistanceIndex{};
        std::int32_t ShipIndex{};
        std::int32_t Sent{};
        std::int32_t Eligible{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            Sent = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(DistanceIndex); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[DistanceIndex].Star));
                if (!(Star->Status.ControlFaction == aGalaxyStruct::sfPirates && Star->Status.Battle == 0 && Star->Status.CustomFaction == u"")) {
                    continue;
                }
                Eligible = 0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(ShipIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, ShipIndex);
                    if (Ship->OwnerId == aGalaxyStruct::oiPirate && Ship->Order == aShip::soNone && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1)) {
                        ++Eligible;
                    }
                }
                if (Eligible > 2) {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(ShipIndex); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, ShipIndex);
                        if (Ship->OwnerId == aGalaxyStruct::oiPirate && Ship->Order == aShip::soNone && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1)) {
                            Ship->OrderJump(aPlayer::GetPlayer()->CurrentStar, true);
                            ++Sent;
                            --Eligible;
                            if (Eligible <= 2) {
                                break;
                            }
                        }
                    }
                }
                if (Sent > 20) {
                    break;
                }
            }
            CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"PAIORNAMTZEXCOASLOL"_w));
        }
    }

    void CheatRangerpoints() {
        if (aGalaxy::Galaxy != nullptr && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstRangerCenter && aPlayer::GetPlayer()->FreeExperience < 1000) {
            aPlayer::GetPlayer()->GainExperience(1000, aGalaxyStruct::esUnscaled);
            CheatCode::ReportCheat(150, EC_Str::DecodeTextW(u"RIALNOGDEPROPRONIHNITIS"_w));
        }
    }

    void CheatNextrank() {
        CheatCode::ReportCheat(90, EC_Str::DecodeTextW(u"NIETXATARNARNAK"_w));
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && GlobalsV::CurrentScreenId != GlobalsV::screenShip) {
            if (aPlayer::GetPlayer()->Rank != 7) {
                aPlayer::GetPlayer()->AddRankPoints(aPlayer::GetPlayer()->GetRankPointsToNextRank());
            }
            if (aPlayer::GetPlayer()->PirateClanReal && aPlayer::GetPlayer()->PirateRank != 7) {
                aPlayer::GetPlayer()->AddPirateRankPoints(aPlayer::GetPlayer()->GetPirateRankPointsToNextRank());
            }
        }
        if (GlobalsV::CurrentScreenId == GlobalsV::screenRuinsTalk) {
            Globals::RuinsTalkScreen->I_Start();
            Globals::RuinsTalkScreen->RestartTextPresentation();
        }
    }

    void CheatCoolweapon() {
        aItem::TEquipment* Item{};
        if (aGalaxy::Galaxy != nullptr && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet()) {
            fEquipmentShop::RestoreTemporaryShopStock();
            Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
            pas::list_add(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop, reinterpret_cast<void*>(Item));
            {
                std::uint8_t randomIntRange = aMyFunction::RandomIntRange(4, 8);
                aGalaxyStruct::TOwnerId raceToOwner = aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId);
                std::int32_t randomIntRange_2 = aMyFunction::RandomIntRange(50, 100);
                aConst::TItemType randomIntRange_3 = static_cast<aConst::TItemType>(aMyFunction::RandomIntRange(58, 61));
                aItem::TWeapon* cpp_arg = pas::checked_cast<aItem::TWeapon*>(Item);
                cpp_arg->Init(randomIntRange_3, randomIntRange_2, randomIntRange, raceToOwner);
            }
            pas::checked_cast<aItem::TWeapon*>(Item)->Improve(aItem::ikAny);
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            if (GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                Globals::EquipmentShopScreen->BuildGoodsControls();
                Globals::EquipmentShopScreen->UpdateScrollButtons();
            }
            CheatCode::ReportCheat(100, EC_Str::DecodeTextW(u"CRONOBLAWSENAIPROSN"_w));
        }
    }

    void CheatLowcostweapon() {
        aItem::TWeapon* Item{};
        aConst::PWeaponInfo Info{};
        if (aGalaxy::Galaxy != nullptr && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet()) {
            fEquipmentShop::RestoreTemporaryShopStock();
            {
                std::uint32_t randomIntRange = aMyFunction::RandomIntRange(1, 100000);
                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                Info = galaxy->SelectWeaponInfo(randomIntRange, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{aGalaxyStruct::waFree}}), 8, 1);
            }
            {
                aGalaxyStruct::TOwnerId raceToOwner = aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId);
                std::int32_t randomIntRange_2 = aMyFunction::RandomIntRange(1, 8);
                std::int32_t randomIntRange_3 = aMyFunction::RandomIntRange(14, 200);
                Item = aItem::CreateGeneratedWeapon(Info, randomIntRange_3, randomIntRange_2, raceToOwner);
            }
            pas::list_add(aPlayer::GetPlayer()->CurrentPlanet->EquipmentShop, reinterpret_cast<void*>(Item));
            Item->Cost = aMyFunction::RandomIntRange(1, 10);
            if (Item->Weight > 100) {
                Item->Improve(aItem::ikMajor);
            }
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            if (GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                Globals::EquipmentShopScreen->BuildGoodsControls();
                Globals::EquipmentShopScreen->UpdateScrollButtons();
            }
            CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"LLOYWACSONSETIWIEFAIPROLNO"_w));
        }
    }

    void CheatBomb() {
        aItem::TArtefact* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            Item = pas::construct_call<aItem::TArtefact>(aItem::TArtefact_Create);
            Item->Init(aPlayer::GetPlayer()->HomePlanet->OwnerId, aConst::t_ArtefactBomb);
            pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
            Item = pas::construct_call<aItem::TArtefact>(aItem::TArtefact_Create);
            Item->Init(aPlayer::GetPlayer()->HomePlanet->OwnerId, aConst::t_ArtefactBomb);
            pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
            CheatCode::ReportCheat(60, EC_Str::DecodeTextW(u"BRODMEB"_w));
        }
    }

    void CheatRobotforce() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1)) {
            {
                void* createConfiguredArtefactByItemType = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiMaloc));
                aMyFunction::TObjectList* artefacts = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts, createConfiguredArtefactByItemType);
            }
            {
                void* createConfiguredArtefactByItemType_2 = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiPeleng));
                aMyFunction::TObjectList* artefacts_2 = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts_2, createConfiguredArtefactByItemType_2);
            }
            {
                void* createConfiguredArtefactByItemType_3 = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiHuman));
                aMyFunction::TObjectList* artefacts_3 = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts_3, createConfiguredArtefactByItemType_3);
            }
            {
                void* createConfiguredArtefactByItemType_4 = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiFeyan));
                aMyFunction::TObjectList* artefacts_4 = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts_4, createConfiguredArtefactByItemType_4);
            }
            {
                void* createConfiguredArtefactByItemType_5 = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiGaal));
                aMyFunction::TObjectList* artefacts_5 = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts_5, createConfiguredArtefactByItemType_5);
            }
            {
                void* createConfiguredArtefactByItemType_6 = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiUninhabited));
                aMyFunction::TObjectList* artefacts_6 = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts_6, createConfiguredArtefactByItemType_6);
            }
            {
                void* createConfiguredArtefactByItemType_7 = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTranclucator, aGalaxyStruct::oiPirate));
                aMyFunction::TObjectList* artefacts_7 = aPlayer::GetPlayer()->Artefacts;
                pas::list_add(artefacts_7, createConfiguredArtefactByItemType_7);
            }
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            CheatCode::ReportCheat(60, EC_Str::DecodeTextW(u"RFOCBIOLTQFNOCROCRE"_w));
        }
    }

    void CheatArtefact() {
        aItem::TItem* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
            {
                std::uint32_t randomIntRange = aMyFunction::RandomIntRange(1, 1000000000);
                aGalaxyStruct::TOwnerId raceToOwner = aConst::RaceToOwner(aPlayer::GetPlayer()->HomePlanet->RaceId);
                Item = aItem::CreateRandomLootItem(aItem::ilpAnyAvailable, raceToOwner, randomIntRange);
            }
            if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
            } else {
                pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
            }
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            CheatCode::ReportCheat(40, EC_Str::DecodeTextW(u"ASRATIENFOARCAT"_w));
        }
    }

    void CheatMoney() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders || GlobalsV::CurrentScreenId == GlobalsV::screenPlanet || GlobalsV::CurrentScreenId == GlobalsV::screenPlanetNO || GlobalsV::CurrentScreenId == GlobalsV::screenGoodsShop || GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + 10000);
                if (GlobalsV::CurrentScreenId == GlobalsV::screenStarMap) {
                    Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanet) {
                    Globals::PlanetScreen->MainPanel->RefreshMoneyAndCargo();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanetNO) {
                    Globals::UninhabitedPlanetScreen->MainPanel->RefreshMoneyAndCargo();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenGoodsShop) {
                    Globals::GoodsShopScreen->MainPanel->RefreshMoneyAndCargo();
                    Globals::GoodsShopScreen->RefreshGoodsDisplay();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                    Globals::EquipmentShopScreen->MainPanel->RefreshMoneyAndCargo();
                }
                CheatCode::ReportCheat(40, EC_Str::DecodeTextW(u"MEOLNIERYE"_w));
            }
        }
    }

    void CheatDrop() {
        aShip::TShip* Ship{};
        aShip::TShip* Nearest{};
        std::int32_t I{};
        float Distance{};
        float BestDistance{};
        aItem::TEquipment* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            BestDistance = 1.0E+30f;
            Nearest = nullptr;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
                if (aPlayer::GetPlayer() != Ship && aKling::BlazerShip != Ship && aKling::KellerShip != Ship && aKling::TerronShip != Ship && pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::stKling), static_cast<std::int32_t>(aGalaxyStruct::stWarrior)) && static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1)) {
                    Distance = aMyFunction::PointDistanceSquared(Ship->Position, aPlayer::GetPlayer()->Position);
                    if (Distance < BestDistance) {
                        BestDistance = Distance;
                        Nearest = Ship;
                    }
                }
            }
            if (Nearest != nullptr) {
                Ship = Nearest;
                {
                    const std::int32_t cpp_first = pas::list_count(Ship->Inventory) - 1;
                    if (cpp_first >= 0) {
                        for (I = cpp_first; I >= 0; --I) {
                            Item = pas::list_at<aItem::TEquipment>(Ship->Inventory, I);
                            if (Item->ItemType != aConst::t_Hull && (Item->ItemType != aConst::t_Engine || Item->EquippedFlag == 0) && (Item->ItemType != aConst::t_FuelTanks || Item->EquippedFlag == 0)) {
                                Ship->DropCarriedItemAsMovingLoot(Item);
                            }
                        }
                    }
                }
                Ship->RefreshDerivedStats(true);
                CheatCode::ReportCheat(60, EC_Str::DecodeTextW(u"DIRIOSPA"_w));
            }
        }
    }

    void CheatPacking() {
        aItem::TItem* Item{};
        std::int32_t I{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && (GlobalsV::CurrentScreenId != GlobalsV::screenStarMap || Globals::StarMapScreen->Mode == fStarMap::smmOrders)) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->ItemType != aConst::t_Hull) {
                    Item->Weight = std::max<std::int32_t>(1, Item->Weight / 2);
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Artefacts, I);
                if (Item->ItemType == aConst::t_ArtefactTranclucator) {
                    static_cast<aTranclucator::TTranclucator*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Item)->Ship)->ArtefactSize = std::max<std::int32_t>(1, Item->Weight / 2);
                }
                Item->Weight = std::max<std::int32_t>(1, Item->Weight / 2);
            }
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
            CheatCode::ReportCheat(300, EC_Str::DecodeTextW(u"PRANCIKCIINEG"_w));
        }
    }

    void CheatKlissanitem() {
        aItem::TWeapon* Item{};
        aConst::PWeaponInfo Info{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsDockedToShip() && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstScienceBase) {
            {
                std::uint32_t randomIntRange = aMyFunction::RandomIntRange(1, 100000);
                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                Info = galaxy->SelectWeaponInfo(randomIntRange, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{aGalaxyStruct::waNotSoldAndNodeRepair}}), 8, 1);
            }
            {
                std::int32_t randomIntRange_2 = aMyFunction::RandomIntRange(1, 8);
                std::int32_t randomIntRange_3 = aMyFunction::RandomIntRange(77, 200);
                Item = aItem::CreateGeneratedWeapon(Info, randomIntRange_3, randomIntRange_2, aGalaxyStruct::oiDominator);
            }
            switch (aMyFunction::RandomIntRange(1, 3)) {
                case 1: Item->DominatorSeries = aGalaxyStruct::dsBlazer; break;
                case 2: Item->DominatorSeries = aGalaxyStruct::dsKeller; break;
                case 3: Item->DominatorSeries = aGalaxyStruct::dsTerron; break;
            }
            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            if (GlobalsV::CurrentScreenId == GlobalsV::screenRuinsTalk) {
                Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenGoodsShop) {
                Globals::GoodsShopScreen->MainPanel->RefreshMoneyAndCargo();
                Globals::GoodsShopScreen->RefreshGoodsDisplay();
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                Globals::EquipmentShopScreen->MainPanel->RefreshMoneyAndCargo();
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenInfo) {
                Globals::InfoScreen->MainPanel->RefreshMoneyAndCargo();
            }
            CheatCode::ReportCheat(160, EC_Str::DecodeTextW(u"KALKINSOSUANNIINTHEMM"_w));
        }
    }

    void CheatWeaponstrength() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsDockedToShip() && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstPirateBase) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (!pas::in_set<aGalaxyStruct::stKling, aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator, aGalaxyStruct::rstCustomStation>(Ship->TypeId)) {
                        if (!(Ship->TypeId == aGalaxyStruct::stPirate || aPlayer::GetPlayer() == Ship || Ship->TypeId == aGalaxyStruct::stRanger && pas::checked_cast<aRanger::TRanger*>(Ship)->PreferredCareer == aGalaxyStruct::rcPirate)) {
                            if (aPlayer::GetPlayer() != Ship->PartnerShip) {
                                Ship->ChangeRelationToRanger(aPlayer::GetPlayer(), -60);
                            }
                        } else {
                            {
                                const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
                                if (1 <= cpp_last) {
                                    for (K = 1; K <= cpp_last; ++K) {
                                        {
                                            std::int32_t min = std::min<std::int32_t>(255, ([&] {
                                                std::int32_t cpp_right = aMyFunction::RandomIntRange(10, 20);
                                                return Ship->Weapons[K]->MaxDamage + cpp_right;
                                            }()));
                                            Ship->Weapons[K]->MaxDamage = min;
                                        }
                                        if (aPlayer::GetPlayer() != Ship) {
                                            Ship->ChangeRelationToRanger(aPlayer::GetPlayer(), 50);
                                        }
                                    }
                                }
                            }
                            Ship->RefreshDerivedStats(true);
                        }
                    }
                }
            }
            aPlayer::GetPlayer()->CareerStatus[aGalaxyStruct::rcPirate] = 100;
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmDecrease, 60, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}}));
            if (GlobalsV::CurrentScreenId == GlobalsV::screenRuinsTalk) {
                Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenGoodsShop) {
                Globals::GoodsShopScreen->MainPanel->RefreshMoneyAndCargo();
                Globals::GoodsShopScreen->RefreshGoodsDisplay();
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                Globals::EquipmentShopScreen->MainPanel->RefreshMoneyAndCargo();
            } else if (GlobalsV::CurrentScreenId == GlobalsV::screenInfo) {
                Globals::InfoScreen->MainPanel->RefreshMoneyAndCargo();
            }
            CheatCode::ReportCheat(200, EC_Str::DecodeTextW(u"WRENARPBOSNASOTERLEINAGATOHE"_w));
        }
    }

    void CheatTenbomb() {
        aItem::TArtefact* Item{};
        float Radius{};
        float Angle{};
        std::int32_t I{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            for (I = 0; I <= 9; ++I) {
                Item = pas::construct_call<aItem::TArtefact>(aItem::TArtefact_Create);
                Item->Init(aPlayer::GetPlayer()->HomePlanet->OwnerId, aConst::t_ArtefactBomb);
                Radius = aMyFunction::RandomIntRange(300, 700);
                Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 360));
                Item->Position.X = aPlayer::GetPlayer()->Position.X + System::Sin(Angle) * Radius;
                Item->Position.Y = aPlayer::GetPlayer()->Position.Y - System::Cos(Angle) * Radius;
                pas::list_add(aPlayer::GetPlayer()->CurrentStar->Items, reinterpret_cast<void*>(Item));
                {
                    SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                    SE_Space::TSpaceSE* space = Globals::SpaceProcess->Space;
                    graphObject->AttachToSpace(space);
                }
                Item->DestroyFlag = 2;
            }
            CheatCode::ReportCheat(140, EC_Str::DecodeTextW(u"TIECN0BEOAMOB"_w));
        }
    }

    void CheatRangersdream() {
        aItem::TProtoplasm* Item{};
        std::int32_t Count{};
        std::int32_t Total{};
        float X{};
        float Y{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            Total = 0;
            while (Total < 10000) {
                Item = pas::construct_call<aItem::TProtoplasm>(aItem::TEquipment_Create);
                if (10000 - Total < 100) {
                    Count = 10000 - Total;
                } else {
                    Count = aMyFunction::RandomIntRange(10, 100);
                }
                Item->Init_2(Count, 1);
                Total += Count;
                Item->DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(aMyFunction::RandomIntRange(0, 2));
                do {
                    X = aMyFunction::RandomIntRange(-250, 250);
                    Y = aMyFunction::RandomIntRange(-250, 250);
                } while (!(pas::sqr(static_cast<pas::Extended>(X)) + pas::sqr(static_cast<pas::Extended>(Y)) > 2.5E+3L && pas::sqr(static_cast<pas::Extended>(X)) + pas::sqr(static_cast<pas::Extended>(Y)) < 6.25E+4L));
                Item->Position.X = static_cast<long double>(aPlayer::GetPlayer()->Position.X) + X;
                Item->Position.Y = static_cast<long double>(aPlayer::GetPlayer()->Position.Y) + Y;
                pas::list_add(aPlayer::GetPlayer()->CurrentStar->Items, reinterpret_cast<void*>(Item));
                {
                    SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                    SE_Space::TSpaceSE* space = Globals::SpaceProcess->Space;
                    graphObject->AttachToSpace(space);
                }
            }
            CheatCode::ReportCheat(140, EC_Str::DecodeTextW(u"RIALNEGREFRESIDUREEKALMA"_w));
        }
    }

    void CheatRndbase() {
        aRuins::TRuins* Station{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t TotalKinds{};
        std::int32_t Remaining{};
        std::int32_t Choice{};
        aGalaxyStruct::TStationType Kind{};
        // Shared DCU set has the native word-aligned local layout.
        aGalaxyStruct::TShipTypeMask Kinds{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            TotalKinds = 7;
            Remaining = TotalKinds;
            Kinds = pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::rstRangerCenter, aGalaxyStruct::rstDominion}});
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
                if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) {
                    --Remaining;
                    pas::exclude_at(&Kinds, Ship->TypeId);
                }
            }
            if (TotalKinds - Remaining < 3) {
                Choice = aMyFunction::RandomIntRange(1, Remaining);
                I = 0;
                for (Kind = static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstRangerCenter); Kind <= static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstDominion); ++Kind) {
                    if (pas::contains(Kinds, Kind)) {
                        ++I;
                        if (I == Choice) {
                            Station = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                            Station->Init(Kind, aPlayer::GetPlayer()->CurrentStar, pas::WideString());
                            break;
                        }
                    }
                }
                CheatCode::ReportCheat(30, EC_Str::DecodeTextW(u"RONNDOBNASSAEY"_w));
            }
        }
    }

    void CheatMapsector() {
        std::int32_t I{};
        aGalaxy::TConstellation* Constellation{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsDockedToShip() && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstPirateBase) {
            I = 0;
            // Native search skips hidden sectors here, then randomly seeks a hidden one.
            while (I < pas::list_count(aGalaxy::Galaxy->Constellations)) {
                if (pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I)->Visible) {
                    break;
                }
                ++I;
            }
            if (I < pas::list_count(aGalaxy::Galaxy->Constellations)) {
                do {
                    std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1);
                    aMyFunction::TObjectList* constellations = aGalaxy::Galaxy->Constellations;
                    Constellation = pas::list_at<aGalaxy::TConstellation>(constellations, randomIntRange);
                } while (Constellation->Visible);
                Constellation->Visible = true;
                CheatCode::ReportCheat(20, EC_Str::DecodeTextW(u"MOARPESHESCOTROLR2"_w));
            }
        }
    }

    void CheatHugemoney() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders || GlobalsV::CurrentScreenId == GlobalsV::screenPlanet || GlobalsV::CurrentScreenId == GlobalsV::screenPlanetNO || GlobalsV::CurrentScreenId == GlobalsV::screenGoodsShop || GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                aPlayer::GetPlayer()->SetMoney((aPlayer::GetPlayer()->Money / 1000000 + 1) * 1000000);
                if (GlobalsV::CurrentScreenId == GlobalsV::screenStarMap) {
                    Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanet) {
                    Globals::PlanetScreen->MainPanel->RefreshMoneyAndCargo();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanetNO) {
                    Globals::UninhabitedPlanetScreen->MainPanel->RefreshMoneyAndCargo();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenGoodsShop) {
                    Globals::GoodsShopScreen->MainPanel->RefreshMoneyAndCargo();
                    Globals::GoodsShopScreen->RefreshGoodsDisplay();
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenEquipmentShop) {
                    Globals::EquipmentShopScreen->MainPanel->RefreshMoneyAndCargo();
                }
                CheatCode::ReportCheat(300, EC_Str::DecodeTextW(u"HAUNGLESMIOMNEELYS"_w));
            }
        }
    }

    void CheatPelengsurprise() {
        aItem::TItem* Item{};
        std::int32_t I{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPeleng && GlobalsV::CurrentScreenId != GlobalsV::screenShip) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I)->Item;
                if (Item != nullptr && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                    static_cast<aItem::TWeapon*>(Item)->Range = static_cast<aItem::TWeapon*>(Item)->Range * 2;
                }
            }
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmDecrease, 50, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}}));
            CheatCode::ReportCheat(100, EC_Str::DecodeTextW(u"PLEVLIESNOGASRUEROPTROINSAEN"_w));
        }
    }

    void CheatSuperhull() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && (GlobalsV::CurrentScreenId != GlobalsV::screenStarMap || Globals::StarMapScreen->Mode == fStarMap::smmOrders)) {
            aPlayer::GetPlayer()->GetHull()->Weight = std::min<std::int64_t>(static_cast<std::int64_t>(2000), System::Round(aPlayer::GetPlayer()->GetHull()->Weight * 1.3L));
            aPlayer::GetPlayer()->GetHull()->HullPoints = aPlayer::GetPlayer()->GetHull()->Weight;
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            aPlayer::GetPlayer()->RefreshGraphicSize();
            Globals::StarMapScreen->MainPanel->RefreshMoneyAndCargo();
            CheatCode::ReportCheat(250, EC_Str::DecodeTextW(u"SRUNPRESROHLUALELS"_w));
        }
    }

    void CheatBoom() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            if (GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle && ab_Ship::KellerArcadeShip == nullptr) {
                const std::int32_t cpp_first = pas::list_count(ab_Ship::PlayerArcadeShip->Enemies) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        std::int32_t round = System::Round(pas::real_divide(pas::list_at<ab_Hit::TabHit>(ab_Ship::PlayerArcadeShip->Enemies, I)->Health * 2, ab_Global::ShieldDamageScale));
                        ab_Hit::TabHit* cpp_arg = pas::list_at<ab_Hit::TabHit>(ab_Ship::PlayerArcadeShip->Enemies, I);
                        cpp_arg->ApplyDamage(round, nullptr, false);
                    }
                }
            }
            if (GlobalsV::CurrentScreenId == GlobalsV::screenStarMap) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
                    if (aPlayer::GetPlayer() != Ship && static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && (Ship->CurrentPlanet == nullptr || Ship->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited)) {
                        Ship->DestroyQueued = true;
                    }
                }
            }
            CheatCode::ReportCheat(30, EC_Str::DecodeTextW(u"BLOSOMM"_w));
        }
    }

    void CheatHaterangers() {
        static const pas::Set<0, 255> ShipTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htPirate, aGalaxyStruct::htDiplomat}});
        static const pas::Set<0, 255> Owners = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate}});
        std::int32_t I{};
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"HEAVTIERROASNAGZEOROST"_w));
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(aGalaxy::Galaxy->Rangers, I);
                pas::checked_cast<aRanger::TRanger*>(Ship)->ChangeShipRelations(nullptr, aRanger::rcmDecrease, 80, static_cast<aConst::THullShipTypeMask>(ShipTypes), static_cast<aGalaxyStruct::TOwnerMask>(Owners));
            }
        }
    }

    void CheatPirates() {
        std::int32_t I{};
        std::int32_t Attempts{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsDockedToShip() && GlobalsV::CurrentScreenId != GlobalsV::screenShip && aPlayer::GetPlayer()->DockedTo->TypeId == aGalaxyStruct::rstPirateBase) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                if (pas::is_one_of<aGalaxyStruct::sfCoalition, aGalaxyStruct::sfPirates>(Star->Status.ControlFaction) && Star->Status.CustomFaction == u"") {
                    Attempts = 0;
                    do {
                        {
                            std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, pas::list_count(Star->Planets) - 1);
                            aMyFunction::TObjectList* planets = Star->Planets;
                            Planet = pas::list_at<aPlanet::TPlanet>(planets, randomIntRange);
                        }
                        if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                            Planet->BuyPirate(100);
                            break;
                        }
                        ++Attempts;
                    } while (!(Attempts == 6));
                }
            }
            CheatCode::ReportCheat(100, EC_Str::DecodeTextW(u"PRIVRVATTIERS"_w));
        }
    }

    void CheatGun() {
        std::uint8_t Kind{};
        std::int32_t I{};
        aConst::PWeaponInfo Info{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}))); cpp_range.next(I); ) {
                Kind = aConst::GetItemTypeFromMask(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}), I);
                {
                    void* createGeneratedEquipment = reinterpret_cast<void*>(aItem::CreateGeneratedEquipment(static_cast<aConst::TItemType>(Kind), System::Round(static_cast<long double>(aConst::WeaponInfos[static_cast<aConst::TItemType>(Kind)].AverageSize) * aConst::EquipmentSizeFactors[5]), aGalaxy::Galaxy->TechLevel, aPlayer::GetPlayer()->OwnerId));
                    aMyFunction::TObjectList* inventory = aPlayer::GetPlayer()->Inventory;
                    pas::list_add(inventory, createGeneratedEquipment);
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->CustomWeaponTypes) - 1); cpp_range_2.next(I); ) {
                Info = pas::list_at<aConst::TWeaponInfo>(aGalaxy::Galaxy->CustomWeaponTypes, I);
                if (Info->Availability != aGalaxyStruct::waSystemOnly) {
                    void* createGeneratedWeapon = reinterpret_cast<void*>(aItem::CreateGeneratedWeapon(Info, System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[5]), aGalaxy::Galaxy->TechLevel, aPlayer::GetPlayer()->OwnerId));
                    aMyFunction::TObjectList* inventory_2 = aPlayer::GetPlayer()->Inventory;
                    pas::list_add(inventory_2, createGeneratedWeapon);
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"GOUMNO"_w));
        }
    }

    void CheatVertix() {
        aGalaxyStruct::TOwnerId Owner{};
        aItem::TEquipment* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range.next(Owner); ) {
                Item = aItem::CreateGeneratedEquipment(aConst::t_Vertix, 20, aGalaxy::Galaxy->TechLevel, Owner);
                pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"VREVRETOIYX"_w));
        }
    }

    void CheatDevice() {
        aConst::TItemType Kind{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_FuelTanks, aConst::t_DefGenerator); cpp_range.next(Kind); ) {
                void* createGeneratedEquipment = reinterpret_cast<void*>(aItem::CreateGeneratedEquipment(Kind, System::Round(static_cast<long double>(aConst::GetAverageItemSize(Kind)) * aConst::EquipmentSizeFactors[5]), 8, aPlayer::GetPlayer()->OwnerId));
                aMyFunction::TObjectList* inventory = aPlayer::GetPlayer()->Inventory;
                pas::list_add(inventory, createGeneratedEquipment);
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"DREAVNIYCHER"_w));
        }
    }

    void CheatArts() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            {
                const std::int32_t cpp_last = aConst::ArtefactLootPools[3].length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Item = pas::checked_cast<aItem::TEquipmentWithActCode*>(static_cast<pas::Object*>(aItem::CreateConfiguredArtefactByItemType(aConst::ArtefactLootPools[3][I], aPlayer::GetPlayer()->OwnerId)));
                        pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
                    }
                }
            }
            {
                const std::int32_t cpp_last_2 = aConst::CustomArtefactLootPools[3].length() - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Item = pas::construct_call<aItem::TArtefactCustom>(aItem::TArtefact_Create);
                        Item->ConfigBlockName = aConst::CustomArtefactLootPools[3][I];
                        reinterpret_cast<aItem::TArtefactCustom*>(Item)->LoadConfig(true);
                        reinterpret_cast<aItem::TArtefact*>(Item)->Init(aPlayer::GetPlayer()->OwnerId, Item->ItemType);
                        pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
                    }
                }
            }
            {
                const std::int32_t cpp_last_3 = aConst::UselessItemLootPools[3].length() - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        Item = pas::construct_call<aItem::TUselessItem>(aItem::TUselessItem_Create);
                        reinterpret_cast<aItem::TUselessItem*>(Item)->Init(aConst::UselessItemLootPools[3][I], aGalaxyStruct::dsBlazer, 0u, false);
                        Item->OwnerId = aPlayer::GetPlayer()->OwnerId;
                        pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
                    }
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"ANROTOS"_w));
        }
    }

    void CheatModule() {
        std::int32_t I{};
        aItem::TMicroModule* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(I); ) {
                if (!aConst::MicroModuleTemplates[I].SpecialOnly) {
                    Item = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                    Item->Init(I);
                    pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"MAOZDEUNLHE"_w));
        }
    }

    void CheatSkill() {
        aGalaxyStruct::TPilotSkill Skill{};
        aShip::TShip* Ship{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            Ship = nullptr;
            if (Globals::GetInnermostScreenLoop() == Globals::HangarScreen) {
                Ship = Globals::HangarScreen->SelectedShip;
            } else if (Globals::GetInnermostScreenLoop() == Globals::ScannerScreen) {
                Ship = Globals::ScannerScreen->ShipToInspect;
            } else if (Globals::GetInnermostScreenLoop() == Globals::ShipScreen) {
                Ship = fShip2::PlayerHoldShip;
            }
            if (Ship == nullptr) {
                Ship = aPlayer::GetPlayer();
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"SXKOINLAL0"_w));
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TPilotSkill>(aGalaxyStruct::psAccuracy, aGalaxyStruct::psLeadership); cpp_range.next(Skill); ) {
                Ship->BaseSkills[Skill] = 6;
            }
            if (Globals::GetInnermostScreenLoop() == Globals::ScannerScreen) {
                Globals::ScannerScreen->CloseClicked(nullptr);
            } else if (Globals::GetInnermostScreenLoop() == Globals::ShipScreen) {
                Globals::ShipScreen->CloseClicked(nullptr);
            }
        }
    }

    void CheatProgram() {
        aGalaxyStruct::TProgramIndex I{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TProgramIndex>(static_cast<aGalaxyStruct::TProgramIndex>(0), static_cast<aGalaxyStruct::TProgramIndex>(11)); cpp_range.next(I); ) {
                aPlayer::GetPlayer()->ProgramCounts[I] = 100;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"PARZONG3ROALMS"_w));
        }
    }

    void CheatIllness() {
        aGalaxyStruct::TCaptainHealthEffect I{};
        aPlayer::TPlayer* Player{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            Player = aPlayer::GetPlayer();
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TCaptainHealthEffect>(static_cast<aGalaxyStruct::TCaptainHealthEffect>(1), static_cast<aGalaxyStruct::TCaptainHealthEffect>(12)); cpp_range.next(I); ) {
                Player->CaptainHealth[I].Progress = 1.0E+2;
                Player->CaptainHealth[I].AppliedTurn = aGalaxy::Galaxy->CurrentTurn;
                Player->CaptainHealth[I].ExpireTurn = aGalaxy::Galaxy->CurrentTurn + aGalaxyStruct::TurnsPerYear;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"IALALENOERSASH"_w));
        }
    }

    void CheatStimulant() {
        aGalaxyStruct::TCaptainHealthEffect I{};
        aPlayer::TPlayer* Player{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            Player = aPlayer::GetPlayer();
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TCaptainHealthEffect>(static_cast<aGalaxyStruct::TCaptainHealthEffect>(13), static_cast<aGalaxyStruct::TCaptainHealthEffect>(24)); cpp_range.next(I); ) {
                Player->CaptainHealth[I].Progress = 1.0E+2;
                Player->CaptainHealth[I].AppliedTurn = aGalaxy::Galaxy->CurrentTurn;
                Player->CaptainHealth[I].ExpireTurn = aGalaxy::Galaxy->CurrentTurn + aGalaxyStruct::TurnsPerYear;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"SATAISMAUILOAONOTS"_w));
        }
    }

    void CheatIdeal() {
        std::int32_t I{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::HullSeriesCount - 1); cpp_range.next(I); ) {
                if (aConst::HullSeriesDefinitions[I].SystemName == u"99") {
                    aPlayer::GetPlayer()->GetHull()->HullSeries = I;
                    CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"INDFENAELE"_w));
                    break;
                }
            }
        }
    }

    void CheatShowmap() {
        std::int32_t I{};
        aGalaxy::TConstellation* Constellation{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range.next(I); ) {
                Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
                if (aPlanet::MainPiratePlanet == nullptr || aPlanet::MainPiratePlanet->CurrentStar->Constellation != Constellation) {
                    Constellation->Visible = true;
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"SIHSONWEMEANPA"_w));
        }
    }

    void CheatMedal() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t LastAward{};
        std::uint8_t Award{};
        std::uint8_t Found{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"MIELDOAELI"_w));
            LastAward = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"Reward.Count"_wref.get()))) - 1;
            if (aPlayer::GetPlayer()->AwardIds == nullptr) {
                aPlayer::GetPlayer()->AwardIds = pas::make_object<pas::List>();
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, LastAward); cpp_range.next(I); ) {
                Found = false;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->AwardIds) - 1); cpp_range_2.next(J); ) {
                    Award = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlayer::GetPlayer()->AwardIds, J)));
                    if (Award == I) {
                        Found = true;
                        break;
                    }
                }
                if (!Found) {
                    aPlayer::GetPlayer()->AddAward(I);
                }
            }
        }
    }

    void SetCheatDominatorLevel(std::int32_t Level) {
        pas::WideString Name{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            switch (Level) {
                case 1: Name = EC_Str::DecodeTextW(u"HRONRERGOLR"_w); break;
                case 2: Name = EC_Str::DecodeTextW(u"NGISGIHATRMOAERE"_w); break;
                case 3: Name = EC_Str::DecodeTextW(u"HAESLOL"_w); break;
            }
            if (aGalaxy::Galaxy->DominatorModLevel != Level) {
                aGalaxy::Galaxy->DominatorModLevel = Level;
            } else {
                aGalaxy::Galaxy->DominatorModLevel = 0;
            }
            CheatCode::ReportCheat(10, Name);
            Globals::StarMapScreen->RefreshScoreModsLabel();
        }
    }

    void CheatHorror() {
        CheatCode::SetCheatDominatorLevel(1);
    }

    void CheatNightmare() {
        CheatCode::SetCheatDominatorLevel(2);
    }

    void CheatHell() {
        CheatCode::SetCheatDominatorLevel(3);
    }

    void CheatTechnic() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            if (aGalaxy::Galaxy->TechnicModEnabled != 1) {
                aGalaxy::Galaxy->TechnicModEnabled = 1;
            } else {
                aGalaxy::Galaxy->TechnicModEnabled = 0;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"TOESCAHENOINC"_w));
            Globals::StarMapScreen->RefreshScoreModsLabel();
        }
    }

    void CheatAmmo() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            if (aGalaxy::Galaxy->AmmoModEnabled != 1) {
                aGalaxy::Galaxy->AmmoModEnabled = 1;
            } else {
                aGalaxy::Galaxy->AmmoModEnabled = 0;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"ACMEMEO"_w));
            Globals::StarMapScreen->RefreshScoreModsLabel();
        }
    }

    void CheatGod() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && pas::in_set<0, 1>(aGalaxy::Galaxy->GodModEnabled)) {
            aGalaxy::Galaxy->GodModEnabled = 1 - aGalaxy::Galaxy->GodModEnabled;
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"GHOID"_w));
            Globals::StarMapScreen->RefreshScoreModsLabel();
        }
    }

    void CheatHole() {
        pas::List* Items{};
        std::int32_t Index{};
        std::int32_t Count{};
        aGalaxy::THole* Hole{};
        float Angle{};
        float Radius{};
        pas::WideString MapName{};
        GI_MessageLoop::TMessageLoopGI* Parent{};
        EC_BlockPar::TBlockParEC* Block{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        // Nested in CheatHole; appends an owned PWideString to the list at ParentFrame-4. Caller removes the static link.
        auto CheatHoleAddName = [&](pas::WideString Text) -> void {
            System::PWideString Cell{};
            pas::new_value(Cell);
            *Cell = std::move(Text);
            pas::list_add(Items, static_cast<void*>(Cell));
        };
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace() && GlobalsV::CurrentScreenId == GlobalsV::screenStarMap && Globals::StarMapScreen->Mode == fStarMap::smmOrders) {
            Parent = pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
            Items = pas::make_object<pas::List>();
            Block = GR_Main::GameDataConfig->GetBlock(u"ABMap"sv);
            Count = Block->GetBlockCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count); cpp_range.next(Index); ) {
                if (Index < Count) {
                    CheatHoleAddName(Block->GetBlockByIndex(Index)->GetParam(u"Path"sv));
                } else {
                    CheatHoleAddName(u"ABMap.map_boss"_w);
                }
            }
            if (fListBox::ShowListDialog(Parent, Index, aConst::LocalizedColorText(u"Cheat.SelectABMap"_wref.get()), Items, 0, 0) == 1) {
                MapName = *pas::list_at<pas::WideString>(Items, Index);
                CheatCode::AddCheatPoints(10);
                if (aGalaxy::Galaxy != nullptr) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerEntersCheatCode"_w, nullptr);
                    Event->AddTextData(u"Hole"_w);
                    Event->AddData(10);
                }
                Hole = pas::construct_call<aGalaxy::THole>(aGalaxy::THole_Create);
                Hole->InitializeGraphic(pas::WideString());
                reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->SetState(1);
                Hole->Star1 = aPlayer::GetPlayer()->CurrentStar;
                Hole->Star2 = aPlayer::GetPlayer()->CurrentStar;
                Hole->ArcadeMapName = std::move(MapName);
                Angle = Math::ArcTan2(aPlayer::GetPlayer()->Position.X, -aPlayer::GetPlayer()->Position.Y);
                Radius = pas::real_max<pas::Extended>(aPlayer::GetPlayer()->CurrentStar->SafeRadius + 1.0E+2L, System::Sqrt(aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, EC_Struct::MakePointF(0.0f, 0.0f))) + 2.0E+2L);
                {
                    float cpp_arg = System::Sin(Angle) * Radius;
                    float cpp_arg_2 = -System::Cos(Angle) * Radius;
                    Hole->Position1 = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
                }
                Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 359));
                Radius = aMyFunction::RandomIntRange(1000, 2000);
                {
                    float cpp_arg_3 = System::Sin(Angle) * Radius;
                    float cpp_arg_4 = -System::Cos(Angle) * Radius;
                    Hole->Position2 = EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4);
                }
                Hole->CreatedTurn = aGalaxy::Galaxy->CurrentTurn;
                Hole->HoleType = 1;
                Globals::StarMapScreen->PendingHoleRefresh = Hole;
                pas::list_add(aGalaxy::Galaxy->Holes, reinterpret_cast<void*>(Hole));
            }
            while (pas::list_count(Items) > 0) {
                // Native cleanup frees only the cell, leaving its string allocation intact.
                pas::dispose(pas::list_get(Items, 0));
                pas::list_delete(Items, 0);
            }
            pas::free(Items);
            GlobalsV::RequestedScreenId = GlobalsV::CurrentScreenId;
            Parent->RequestClose(1);
        }
    }

    void CheatWin() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && GlobalsV::CurrentScreenId != GlobalsV::screenArcadeBattle) {
            CheatCode::AddCheatPoints(10);
            Globals::ScoreScreen->RecordPlayerResult(true);
            Globals::AboutScreen->ReturnToScores = true;
            GlobalsV::RequestedScreenId = GlobalsV::screenAbout;
            pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
        }
    }

    void CheatHweapon() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        if (aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->EquippedFlag != 0 && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                    aItem::TWeapon* cpp_with = static_cast<aItem::TWeapon*>(Item);
                    cpp_with->TechLevel = 8;
                    cpp_with->MinDamage = cpp_with->CalculateGeneratedMinDamage();
                    cpp_with->MaxDamage = cpp_with->CalculateStandardMaxDamage();
                    cpp_with->Range = cpp_with->CalculateStandardRange();
                    cpp_with->Cost = aItem::CalculateGeneratedWeaponCost(cpp_with->GetWeaponInfo(), cpp_with->Weight, cpp_with->TechLevel, cpp_with->OwnerId);
                    if (pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(cpp_with->GetWeaponInfo()->ShotType)) {
                        cpp_with->AmmoCapacity = cpp_with->CalculateGeneratedAmmoCapacity();
                        if (cpp_with->MicroModuleIndex != 0) {
                            cpp_with->AmmoCapacity += aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses[aConst::bonAmmo];
                        }
                        if (cpp_with->SpecialModuleIndex != 0) {
                            cpp_with->AmmoCapacity += aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].StatBonuses[aConst::bonAmmo];
                        }
                    }
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"HOWIETANPEOLN"_w));
        }
    }

    void CheatUltrascan() {
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            if (aGalaxy::Galaxy->UltraScanModEnabled != 1) {
                aGalaxy::Galaxy->UltraScanModEnabled = 1;
            } else {
                aGalaxy::Galaxy->UltraScanModEnabled = 0;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"USLATOREAMSACRAWN"_w));
            Globals::StarMapScreen->RefreshScoreModsLabel();
        }
    }

    void CheatTentm() {
        std::int32_t I{};
        aItem::TArtefactTransmitter* Item{};
        if (aPlayer::GetPlayer() != nullptr) {
            for (I = 1; I <= 10; ++I) {
                Item = reinterpret_cast<aItem::TArtefactTransmitter*>(aItem::CreateConfiguredArtefactByItemType(aConst::t_ArtefactTransmitter, aPlayer::GetPlayer()->OwnerId));
                Item->Power = 100;
                pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"TREANTTIME"_w));
        }
    }

    void CheatEncharge() {
        std::int32_t I{};
        aItem::TArtefact* Item{};
        if (aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TArtefact>(aPlayer::GetPlayer()->Artefacts, I);
                if (aItem::TArtefactTransmitter* artefactTransmitter = pas::class_cast_if<aItem::TArtefactTransmitter*>(Item); artefactTransmitter != nullptr && artefactTransmitter->Power <= 1000) {
                    artefactTransmitter->Power += 100;
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"ECNDCAHAALRIGEE"_w));
        }
    }

    void CheatExpa() {
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->FreeExperience += 1000000;
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"ELXIPOAN"_w));
        }
    }

    void CheatMadeinchina() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        if (aPlayer::GetPlayer() != nullptr) {
            // The native loop deliberately starts at one.
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->EquippedFlag != 0) {
                    Item->OwnerId = aGalaxyStruct::oiUninhabited;
                }
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"MRALDIETISNOCIHSIMNIA"_w));
        }
    }

    void CheatZawarudo() {
        if (aPlayer::GetPlayer() != nullptr) {
            if (aGalaxy::Galaxy->StasisModEnabled != 1) {
                aGalaxy::Galaxy->StasisModEnabled = 1;
            } else {
                aGalaxy::Galaxy->StasisModEnabled = 0;
            }
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"ZIANWRASRIUNDAOL"_w));
            Globals::StarMapScreen->RefreshScoreModsLabel();
        }
    }

    void ShowCheatFeedback(pas::WideString Text) {
        if (aGalaxy::Galaxy == nullptr) {
            GI_MessageBox::ShowMessageBoxGI(nullptr, Text, GI_MessageBox::mbgOK, 0, 0, 0);
        } else {
            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
        }
    }

    void CheatMakedump() {
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->IronWill ^ 1) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && aGalaxy::Galaxy->FinalizationNameEncoded == u"" && pas::is_one_of<GlobalsV::screenHangar, GlobalsV::screenPlanet, GlobalsV::screenPlanetNO, GlobalsV::screenEquipmentShop, GlobalsV::screenGovernment, GlobalsV::screenStarMap, GlobalsV::screenRuinsTalk, GlobalsV::screenInfo, GlobalsV::screenGoodsShop>(GlobalsV::CurrentScreenId) && reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->ChildLoop == nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(888);
            aGalaxy::Galaxy->CampaignFlag183 = 1;
            GR_Main::CaptureSavePreview();
            fGalaxy2::CaptureGalaxyPreview(reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]));
            aGalaxy::Galaxy->PrimeIntegrityChecksum(889);
            GlobalsV::SaveManagerReturnScreenId = GlobalsV::CurrentScreenId;
            Globals::SaveManagerMode = fSaveManager::smmSave;
            GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
            reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
        }
    }

    void CheatFitness() {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString FileName{};
        std::int32_t Index{};
        std::int32_t Number{};
        pas::WideString Path{};
        aItem::TEquipment* Item{};
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            FileName = pas::concat_wide_reverse({EC_Str::DecodeTextW(u"PaliatyseoraFainta.Atoxita"_w), GR_Main::GetGameUserDirectory()});
            Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            Number = 1;
            Block->AddParam(EC_Str::DecodeTextW(u"GraemlenVoenrusSimoun"_w), GR_Main::GameVersionText);
            {
                const pas::WideString& name = aPlayer::GetPlayer()->GetName();
                const pas::WideString& localizedTypeName = aPlayer::GetPlayer()->GetLocalizedTypeName();
                Block->AddParam(localizedTypeName, name);
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(Index); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, Index);
                if (pas::class_cast_if<aItem::THull*>(Item) != nullptr || Item->EquippedFlag != 0) {
                    if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                        Path = static_cast<pas::WideString>(pas::concat_ansi({"Weapon", SysUtils::IntToStr(Number)}));
                        ++Number;
                    } else {
                        Path = aConst::ItemTypeNames[Item->ItemType];
                    }
                    Item->SaveToBlock(Block->AddBlockByPath(Path));
                }
            }
            Number = 1;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range_2.next(Index); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Artefacts, Index);
                if (Item->EquippedFlag != 0) {
                    Path = static_cast<pas::WideString>(pas::concat_ansi({"Artefact", SysUtils::IntToStr(Number)}));
                    ++Number;
                    Item->SaveToBlock(Block->AddBlockByPath(Path));
                }
            }
            Block->SaveTextFile(FileName.pchar(), true, false);
            pas::free(Block);
            CheatCode::ReportCheat(0, EC_Str::DecodeTextW(u"FLITTONLEISES"_w));
        }
    }

    void CheatExtraone() {
        std::int32_t I{};
        std::int32_t Count{};
        std::uint8_t Good{};
        aItem::TEquipment* Item{};
        if (aPlayer::GetPlayer() != nullptr) {
            Count = pas::list_count(aPlayer::GetPlayer()->Inventory);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->EquippedFlag == 0 && Item->ScriptItem == nullptr && !(pas::class_cast_if<aItem::TTreasureMap*>(Item) != nullptr)) {
                    if (aItem::TCountableItem* countableItem = pas::class_cast_if<aItem::TCountableItem*>(Item)) {
                        countableItem->StackCount = countableItem->StackCount * 2;
                        Item->Weight *= 2;
                    } else {
                        Item = reinterpret_cast<aItem::TEquipment*>(Item->Clone());
                        if (Item != nullptr) {
                            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
                        }
                    }
                }
            }
            Count = pas::list_count(aPlayer::GetPlayer()->Artefacts);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Artefacts, I);
                if (Item->EquippedFlag == 0 && Item->ScriptItem == nullptr) {
                    Item = reinterpret_cast<aItem::TEquipment*>(Item->Clone());
                    if (Item != nullptr) {
                        pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Item));
                    }
                }
            }
            for (Good = 0; Good <= 7; ++Good) {
                aPlayer::GetPlayer()->CargoGoods[Good].Count = aPlayer::GetPlayer()->CargoGoods[Good].Count * 2;
            }
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            CheatCode::ReportCheat(10, EC_Str::DecodeTextW(u"EIXATIRIANORNAEL"_w));
        }
    }

    void CheatSudo() {
        pas::WideString Value{};
        pas::WideString ScriptName{};
        char16_t Quote{};
        std::int32_t Index{};
        std::int32_t Count{};
        aScript::TScript* Script{};
        std::uint8_t Found{};
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(888);
        }
        Value = pas::WideString();
        if (fTextBox::ShowTextInputDialog(pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]), u"Enter script command"_w, Value, 255, 0, 0) == 1) {
            try {
                Count = Value.length();
                if (Count > 0) {
                    Quote = Value.read(1);
                    // The native double-quote test compares against two characters.
                    if (Quote == u'\'' || static_cast<pas::WideString>(Quote) == u"\"\"") {
                        for (auto cpp_range = pas::for_to<std::int32_t>(2, Count); cpp_range.next(Index); ) {
                            if (Value.read(Index) == Quote) {
                                break;
                            }
                        }
                        if (Index < Count) {
                            ScriptName = EC_Str::CopyWideStringUnchecked(Value, 2, Index - 2);
                            Value = EC_Str::CopyWideStringUnchecked(Value, Index + 1, Count - Index);
                            Found = false;
                            if (aGalaxy::Galaxy != nullptr) {
                                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range_2.next(Index); ) {
                                    Script = pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, Index);
                                    if (Script->ScriptFileName == ScriptName) {
                                        Found = true;
                                        aScript::ExecuteScriptText(Value, Script->InitCode->LocalVar);
                                        break;
                                    }
                                }
                            }
                            if (!Found) {
                                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error, script with name ", ScriptName, u"is not found"}))));
                            }
                        }
                    } else {
                        aScript::ExecuteScriptText(Value, nullptr);
                    }
                }
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GI_MessageLoop::TMessageLoopGI* cpp_arg = pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
                    const pas::WideString& message = static_cast<pas::WideString>(E->message);
                    GI_MessageBox::ShowMessageBoxGI(cpp_arg, message, GI_MessageBox::mbgCancel, 0, 0, 0);
                } else {
                    throw;
                }
            }
        }
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(889);
        }
    }

    void CheatEvents() {
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::int32_t Index{};
        std::int32_t I{};
        if (aGalaxy::Galaxy != nullptr) {
            Index = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            GR_Main::AppendLogLineThreadSafe("----------------------------------------"_a);
            GR_Main::AppendLogLineThreadSafe("Events:"_a);
            GR_Main::AppendLogLineThreadSafe("----------------------------------------"_a);
            while (Index >= 0) {
                Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, Index);
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Event->EventType));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({aGalaxy::FormatGameTurnDate(Event->Turn), u" (", pas::wide_int_to_str(Event->Turn), u")"})));
                if (Event->TextData != nullptr) {
                    GR_Main::AppendLogLineThreadSafe("  Text data:"_a);
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Event->TextData) - 1); cpp_range.next(I); ) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"  ", *pas::list_at<pas::WideString>(Event->TextData, I)})));
                    }
                }
                if (Event->Data != nullptr) {
                    GR_Main::AppendLogLineThreadSafe("  Data:"_a);
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Event->Data) - 1); cpp_range_2.next(I); ) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"  ", SysUtils::IntToStr(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Event->Data, I))))}));
                    }
                    GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
                }
                --Index;
            }
            GR_Main::AppendLogLineThreadSafe("----------------------------------------"_a);
        }
    }

    void CheatSeed() {
        pas::WideString Value{};
        if (reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]) == Globals::NewGameScreen) {
            if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Globals::NewGameScreen->GetByName(u"ButExtended"sv))->Down) {
                if (GR_Main::NewGameSeedText == u"") {
                    SystemImports::Randomize();
                    GR_Main::NewGameSeedText = EC_Str::IntToWideString(aMyFunction::RandomIntRange(100000, SystemImports::MaxInt));
                }
                Value = GR_Main::NewGameSeedText;
                if (fTextBox::ShowTextInputDialog(Globals::NewGameScreen, u"SEED"_w, Value, 30, 0, 0) == 1) {
                    GR_Main::NewGameSeedText = Value;
                }
            }
        }
    }

    void CheatInfos() {
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        aShip::PCustomShipInfo Info{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::uint8_t PrintedHeader{};
        if (aGalaxy::Galaxy != nullptr) {
            GR_Main::AppendLogLineThreadSafe("----------------------------------------"_a);
            GR_Main::AppendLogLineThreadSafe("ShipInfos:"_a);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    PrintedHeader = false;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->CustomShipInfos) - 1); cpp_range_3.next(K); ) {
                        Info = pas::list_at<aShip::TCustomShipInfo>(Ship->CustomShipInfos, K);
                        if (!Info->DeleteQueued) {
                            if (!PrintedHeader) {
                                GR_Main::AppendLogLineThreadSafe("----------------------------------------"_a);
                                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Ship->GetFullName(u" "_wref.get())));
                            }
                            PrintedHeader = true;
                            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({Info->TypeName, u" ", pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)))), u",", pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)))), u",", pas::wide_int_to_str(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)))), u",\"", Info->TextData1, u"\",\"", Info->TextData2, u"\",\"", Info->TextData3, u"\""})));
                        }
                    }
                }
            }
            GR_Main::AppendLogLineThreadSafe("----------------------------------------"_a);
        }
    }

    std::int32_t TCheatList::AddEntry(PCheatEntry Entry) {
        return pas::list_add(this, static_cast<void*>(Entry));
    }

    void TCheatList_Destroy(TCheatList* Self) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self) - 1); cpp_range.next(Index); ) {
            pas::dispose(Self->GetEntry(Index));
        }
        pas::list_destroy(Self);
    }

    PCheatEntry TCheatList::GetEntry(std::int32_t Index) {
        return pas::list_at<TCheatEntry>(this, Index);
    }

    std::int32_t TCheatList::AddCheat(pas::WideString Text, TCheatCallback Callback) {
        PCheatEntry Entry{};
        pas::new_value(Entry);
        Entry->Text = std::move(Text);
        Entry->Callback = Callback;
        return AddEntry(Entry);
    }

    // Compiler unit entry registers the native command order.
    // 'INFOS'
    void UnitInitialize() {
        CheatEntries = pas::make_object<TCheatList>();
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ROEMPOAYIURU"_w), TCheatCallback(CheatRepair));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"KULTIZSOSOASNOMEANXI"_w), TCheatCallback(CheatKlissanmax));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"PVISREAXTMETMOARX9"_w), TCheatCallback(CheatPiratemax));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"WIAGRARUILOIRAMOARX9"_w), TCheatCallback(CheatWarriormax));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"KOLEINSOSUAINOCRABLELS"_w), TCheatCallback(CheatKlissancall));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"PAIORNAMTZEXCOASLOL"_w), TCheatCallback(CheatPiratecall));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"RIALNOGDEPROPRONIHNITIS"_w), TCheatCallback(CheatRangerpoints));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"NIETXATARNARNAK"_w), TCheatCallback(CheatNextrank));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"CRONOBLAWSENAIPROSN"_w), TCheatCallback(CheatCoolweapon));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"LLOYWACSONSETIWIEFAIPROLNO"_w), TCheatCallback(CheatLowcostweapon));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"BRODMEB"_w), TCheatCallback(CheatBomb));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ASRATIENFOARCAT"_w), TCheatCallback(CheatArtefact));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"MEOLNIERYE"_w), TCheatCallback(CheatMoney));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"DIRIOSPA"_w), TCheatCallback(CheatDrop));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"PRANCIKCIINEG"_w), TCheatCallback(CheatPacking));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"KALKINSOSUANNIINTHEMM"_w), TCheatCallback(CheatKlissanitem));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"WRENARPBOSNASOTERLEINAGATOHE"_w), TCheatCallback(CheatWeaponstrength));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"TIECN0BEOAMOB"_w), TCheatCallback(CheatTenbomb));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"RONNDOBNASSAEY"_w), TCheatCallback(CheatRndbase));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"MOARPESHESCOTROLR2"_w), TCheatCallback(CheatMapsector));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"HAUNGLESMIOMNEELYS"_w), TCheatCallback(CheatHugemoney));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"PLEVLIESNOGASRUEROPTROINSAEN"_w), TCheatCallback(CheatPelengsurprise));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"SRUNPRESROHLUALELS"_w), TCheatCallback(CheatSuperhull));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"BLOSOMM"_w), TCheatCallback(CheatBoom));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"HEAVTIERROASNAGZEOROST"_w), TCheatCallback(CheatHaterangers));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"PRIVRVATTIERS"_w), TCheatCallback(CheatPirates));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"GOUMNO"_w), TCheatCallback(CheatGun));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"VREVRETOIYX"_w), TCheatCallback(CheatVertix));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"DREAVNIYCHER"_w), TCheatCallback(CheatDevice));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ANROTOS"_w), TCheatCallback(CheatArts));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"MAOZDEUNLHE"_w), TCheatCallback(CheatModule));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"SXKOINLAL0"_w), TCheatCallback(CheatSkill));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"PARZONG3ROALMS"_w), TCheatCallback(CheatProgram));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"IALALENOERSASH"_w), TCheatCallback(CheatIllness));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"SATAISMAUILOAONOTS"_w), TCheatCallback(CheatStimulant));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"INDFENAELE"_w), TCheatCallback(CheatIdeal));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"SIHSONWEMEANPA"_w), TCheatCallback(CheatShowmap));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"MIELDOAELI"_w), TCheatCallback(CheatMedal));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"HRONRERGOLR"_w), TCheatCallback(CheatHorror));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"NGISGIHATRMOAEREE"_w), TCheatCallback(CheatNightmare));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"HAESLOL"_w), TCheatCallback(CheatHell));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"TOESCAHENOINC"_w), TCheatCallback(CheatTechnic));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ACMEMEO"_w), TCheatCallback(CheatAmmo));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"GHOID"_w), TCheatCallback(CheatGod));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"HLOILAEN"_w), TCheatCallback(CheatHole));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"WHINNE"_w), TCheatCallback(CheatWin));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"HOWIETANPEOLN"_w), TCheatCallback(CheatHweapon));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"USLATOREAMSACRAWN"_w), TCheatCallback(CheatUltrascan));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"TREANTTIME"_w), TCheatCallback(CheatTentm));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ECNDCAHAALRIGEE"_w), TCheatCallback(CheatEncharge));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ELXIPOAN"_w), TCheatCallback(CheatExpa));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"MRALDIETISNOCIHSIMNIA"_w), TCheatCallback(CheatMadeinchina));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"ZIANWRASRIUNDAOL"_w), TCheatCallback(CheatZawarudo));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"FLITTONLEISES"_w), TCheatCallback(CheatFitness));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"EIXATIRIANORNAEL"_w), TCheatCallback(CheatExtraone));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"RFOCBIOLTQFNOCROCRE"_w), TCheatCallback(CheatRobotforce));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"RIALNEGREFRESIDUREEKALMA"_w), TCheatCallback(CheatRangersdream));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"MOABKREIDLUCMEPT"_w), TCheatCallback(CheatMakedump));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"SAENEEDO"_w), TCheatCallback(CheatSeed));
        CheatEntries->AddCheat(u"SUDO"_w, TCheatCallback(CheatSudo));
        CheatEntries->AddCheat(u"EVENTS"_w, TCheatCallback(CheatEvents));
        CheatEntries->AddCheat(EC_Str::DecodeTextW(u"IONOFROSS"_w), TCheatCallback(CheatInfos));
    }

    // Compiler unit entry calls the virtual destructor directly.
    void UnitFinalize() {
        pas::destroy(CheatEntries);
    }

    void TCheatList::p_destroy() {
        CheatCode::TCheatList_Destroy(this);
    }

} // namespace CheatCode
