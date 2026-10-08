#include "layout/fGameSettings.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aRuins.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fGameSettings.hpp"
#include "units/fIntroduction.hpp"

// The native thread contribution ends before TfGameSettings2 metadata.
// Ownership by the linked fGameSettings unit is inferred from that boundary and
// PACKAGEINFO's adjacent fGameSettings2/fGameSettings dependency entries.
namespace fGameSettings {
    void TThreadCreateNewGame::Execute() {
        static const pas::Set<0, 255> InitialDominatorShipMask = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::stKling}});
        std::uint16_t ControlWord{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t N{};
        aGalaxy::TStar* Star{};
        aGalaxy::TStar* OtherStar{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TConstellation* Constellation{};
        aShip::TShip* Ship{};
        aRanger::TRanger* Ranger{};
        aGalaxy::TStar* SpecialStar{};
        aPlanet::TPlanet* HomePlanet{};
        std::int32_t Distance{};
        std::int32_t MaximumDistance{};
        float EdgeDistance{};
        float OtherEdgeDistance{};
        std::uint8_t Skill{};
        aPlayer::PStorageEntry Entry{};
        aItem::TEquipment* Item{};
        aPlayer::TPlayer* Player{};
        std::int32_t Stage{};
        std::int32_t Value{};
        EC_Struct::TPointF Center{};
        float Score{};
        aGalaxy::TStar* StartStar{};
        aGalaxyStruct::TOwnerId OwnerId{};
        pas::Array<pas::List*, 0, 7> NameLists{};
        Stage = 0;
        try {
            ControlWord = 0x0000103f;
            System::Set8087CW(ControlWord);
            fIntroduction::NewGameGenerationStage = 0;
            Globals::PlayerStarDayPrepared = true;
            aGalaxy::Galaxy = pas::construct_call<aGalaxy::TGalaxy>(aGalaxy::TGalaxy_Create);
            for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(7); ++Skill) {
                aGalaxy::Galaxy->DifficultyLevels[Skill] = DifficultyLevels[Skill];
            }
            aGalaxy::Galaxy->CustomRules.Enabled =
                GR_Main::NewGameSettingsConfig->CountParamsByPath(u"UseCustomRules"_wref.get()) > 0 &&
                    GI_Main::ParseEnabledNameGI(
                        pas::view(
                        GR_Main::NewGameSettingsConfig->GetParamByPathOrMarker(u"UseCustomRules"_wref.get())));
            if (aGalaxy::Galaxy->CustomRules.Enabled && GR_Main::NewGameSeedText != u"") {
                aGalaxy::Galaxy->GenerationSeed = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::NewGameSeedText));
                aGalaxy::Galaxy->RandomState = aGalaxy::Galaxy->GenerationSeed;
            }
            GR_Main::NewGameSeedText = pas::WideString();
            aGalaxy::Galaxy->SetCheatPoints(0);
            if (!aGalaxy::Galaxy->CustomRules.Enabled) {
                aGalaxy::Galaxy->CustomRules.DominatorStrength = 0;
                aGalaxy::Galaxy->CustomRules.DominatorAggression = 0;
                aGalaxy::Galaxy->CustomRules.DominatorSpawn = 0;
                aGalaxy::Galaxy->CustomRules.PirateAggression = 0;
                aGalaxy::Galaxy->CustomRules.CoalitionAggression = 0;
                aGalaxy::Galaxy->CustomRules.AsteroidModifier = 8;
                aGalaxy::Galaxy->CustomRules.SunDamageModifier = 8;
                aGalaxy::Galaxy->CustomRules.ExtraInventions = 0;
                aGalaxy::Galaxy->CustomRules.AcrynModifier = 16;
                aGalaxy::Galaxy->CustomRules.NodeDropModifier = 8;
                aGalaxy::Galaxy->CustomRules.ArcadeDropValueModifier = 8;
                aGalaxy::Galaxy->CustomRules.DropValueModifier = 8;
                aGalaxy::Galaxy->CustomRules.AgriculturalPlanetWeight = 1;
                aGalaxy::Galaxy->CustomRules.MixedPlanetWeight = 1;
                aGalaxy::Galaxy->CustomRules.IndustrialPlanetWeight = 1;
                aGalaxy::Galaxy->CustomRules.ExtraRangers = 0;
                aGalaxy::Galaxy->CustomRules.ArcadeHitpointsModifier = 8;
                aGalaxy::Galaxy->CustomRules.ArcadeDamageModifier = 8;
                aGalaxy::Galaxy->CustomRules.AIJunkTolerance = 7;
                aGalaxy::Galaxy->CustomRules.ChaoticRandom = false;
                aGalaxy::Galaxy->CustomRules.UnrestrictedEquipmentKnowledge = false;
                aGalaxy::Galaxy->CustomRules.StationsNearStars = false;
                aGalaxy::Galaxy->CustomRules.FullStationTargeting = false;
                aGalaxy::Galaxy->CustomRules.SpecialShips = false;
                aGalaxy::Galaxy->CustomRules.ZeroStartingExperience = false;
                aGalaxy::Galaxy->CustomRules.ArcadeBattleRoyale = false;
                aGalaxy::Galaxy->CustomRules.DominatorRacialWeapons = false;
                aGalaxy::Galaxy->CustomRules.MaxRangeMissiles = false;
                aGalaxy::Galaxy->CustomRules.OldHyperspace = false;
                aGalaxy::Galaxy->CustomRules.PirateNodes = false;
                aGalaxy::Galaxy->CustomRules.AIUseShops = false;
                aGalaxy::Galaxy->CustomRules.StationsUseShop = false;
                aGalaxy::Galaxy->CustomRules.DuplicateArtefacts = false;
                aGalaxy::Galaxy->CustomRules.HullGrowth = 0;
                aGalaxy::Galaxy->CustomRules.ArcadeEquipmentChange = false;
                aGalaxy::Galaxy->CustomRules.OldSpeedCalculation = false;
                aGalaxy::Galaxy->CustomRules.OldMissileBonuses = false;
            } else {
                EC_BlockPar::TBlockParEC* CustomRules = GR_Main::NewGameSettingsConfig->GetBlockByPath(
                    u"CustomRules"_wref.get());
                Value =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"KlingStrength"_wref.get())));
                if (Value < 0) {
                    Value = 0;
                    for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(7); ++Skill) {
                        Value += aGalaxy::Galaxy->DifficultyLevels[Skill];
                    }
                }
                aGalaxy::Galaxy->CustomRules.DominatorStrength = Value;
                Value =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"KlingAggro"_wref.get())));
                if (Value < 0) {
                    Value = 0;
                    for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(7); ++Skill) {
                        Value += aGalaxy::Galaxy->DifficultyLevels[Skill];
                    }
                }
                aGalaxy::Galaxy->CustomRules.DominatorAggression = Value;
                Value =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"KlingSpawn"_wref.get())));
                if (Value < 0) {
                    Value = 0;
                    for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(7); ++Skill) {
                        Value += aGalaxy::Galaxy->DifficultyLevels[Skill];
                    }
                }
                aGalaxy::Galaxy->CustomRules.DominatorSpawn = Value;
                Value =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"PirateAggro"_wref.get())));
                if (Value < 0) {
                    aGalaxy::Galaxy->CustomRules.PirateAggression = aGalaxy::Galaxy->DifficultyLevels[0] * 8;
                } else {
                    aGalaxy::Galaxy->CustomRules.PirateAggression = Value;
                }
                aGalaxy::Galaxy->CustomRules.CoalitionAggression =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"CoalAggro"_wref.get())));
                aGalaxy::Galaxy->CustomRules.AsteroidModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"AsteroidMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.SunDamageModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"SunDamageMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ExtraInventions =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"ExtraInventions"_wref.get())));
                aGalaxy::Galaxy->CustomRules.AcrynModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"AkrinMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.NodeDropModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"NodeDropMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ArcadeDropValueModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"ABDropValueMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.DropValueModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"DropValueMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.AgriculturalPlanetWeight =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"AgPlanets"_wref.get())));
                aGalaxy::Galaxy->CustomRules.MixedPlanetWeight =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"MiPlanets"_wref.get())));
                aGalaxy::Galaxy->CustomRules.IndustrialPlanetWeight =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"InPlanets"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ExtraRangers =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"ExtraRangers"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ArcadeHitpointsModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"ABHitpointsMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ArcadeDamageModifier =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"ABDamageMod"_wref.get())));
                aGalaxy::Galaxy->CustomRules.AIJunkTolerance =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPath(u"AITolerateJunk"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ChaoticRandom =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"RndChaotic"_wref.get())));
                aGalaxy::Galaxy->CustomRules.UnrestrictedEquipmentKnowledge =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"EqKnowledgeUnRestricted"_wref.get())));
                aGalaxy::Galaxy->CustomRules.StationsNearStars =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"RuinsNearStars"_wref.get())));
                aGalaxy::Galaxy->CustomRules.FullStationTargeting =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"RuinsTargettingFull"_wref.get())));
                aGalaxy::Galaxy->CustomRules.SpecialShips =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"SpecialShipsInGame"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ZeroStartingExperience =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"ZeroStartExp"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ArcadeBattleRoyale =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"ABattleRoyale"_wref.get())));
                aGalaxy::Galaxy->CustomRules.DominatorRacialWeapons =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"KlingRacialWeapons"_wref.get())));
                aGalaxy::Galaxy->CustomRules.StartInCenter =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"StartCenter"_wref.get())));
                aGalaxy::Galaxy->CustomRules.MaxRangeMissiles =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"MaxRangeMissiles"_wref.get())));
                aGalaxy::Galaxy->CustomRules.OldHyperspace =
                    GI_Main::ParseEnabledNameGI(pas::view(CustomRules->GetParamByPathOrMarker(u"OldHyper"_wref.get())));
                aGalaxy::Galaxy->CustomRules.PirateNodes =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"PirateNodes"_wref.get())));
                aGalaxy::Galaxy->CustomRules.AIUseShops =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"AIUseShops"_wref.get())));
                aGalaxy::Galaxy->CustomRules.StationsUseShop =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"RuinsUseShop"_wref.get())));
                aGalaxy::Galaxy->CustomRules.DuplicateArtefacts =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"DuplicateArts"_wref.get())));
                aGalaxy::Galaxy->CustomRules.HullGrowth =
                    SysUtils::StrToInt(
                        static_cast<pas::AnsiString>(CustomRules->GetParamByPathOrMarker(u"HullGrowth"_wref.get())));
                aGalaxy::Galaxy->CustomRules.ArcadeEquipmentChange =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"ABChangeEq"_wref.get())));
                aGalaxy::Galaxy->CustomRules.OldSpeedCalculation =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"OldSpeedCalc"_wref.get())));
                aGalaxy::Galaxy->CustomRules.OldMissileBonuses =
                    GI_Main::ParseEnabledNameGI(
                        pas::view(CustomRules->GetParamByPathOrMarker(u"OldMissileBonuses"_wref.get())));
            }
            aGalaxy::Galaxy->GenerationMachineHash = GR_Main::ComputeMachineFingerprintCRC();
            aGalaxy::Galaxy->InitializeCampaignState();
            fIntroduction::NewGameGenerationStage = 1;
            aGalaxy::PlayerStar = nullptr;
            Stage = 1;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::GalaxyStarCount); cpp_range.next(I); ) {
                Star = pas::construct_call<aGalaxy::TStar>(aGalaxy::TStar_Create);
                pas::list_add(aGalaxy::Galaxy->Stars, reinterpret_cast<void*>(Star));
            }
            aGalaxy::Galaxy->GenerateGalaxyLayout(PlayerRace);
            Stage = 2;
            {
                const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        Star->BackgroundImage = -1;
                    }
                }
            }
            Stage = 3;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 0)->BackgroundImage = 14;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 1)->BackgroundImage = 5;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 2)->BackgroundImage = 70;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 3)->BackgroundImage = 11;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 4)->BackgroundImage = 3;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 70)->BackgroundImage = 72;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 71)->BackgroundImage = 72;
            for (I = 0; I <= 5; ++I) {
                do {
                    J = aMyFunction::NextRandomIntRange(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1,
                            aGalaxy::Galaxy->RandomState);
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, J);
                } while (!(Star->BackgroundImage < 0));
                Star->BackgroundImage = 50 + I;
            }
            Stage = 4;
            {
                const std::int32_t cpp_last_2 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        if (Star->BackgroundImage < 0) {
                            Star->BackgroundImage = aMyFunction::NextRandomIntRange(0, 15,
                                    aGalaxy::Galaxy->RandomState);
                        }
                    }
                }
            }
            Stage = 5;
            fIntroduction::NewGameGenerationStage = 2;
            SpecialStar = nullptr;
            HomePlanet = nullptr;
            MaximumDistance = 0;
            N = 0;
            {
                const std::int32_t cpp_last_3 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        if (Star != SpecialStar) {
                            Star->GenerateSystemContents(false);
                        } else {
                            Star->BackgroundImage = 71;
                            Star->GenerateSystemContents(true);
                        }
                        if (HomePlanet == nullptr) {
                            const std::int32_t cpp_last_4 = pas::list_count(Star->Planets) - 1;
                            if (0 <= cpp_last_4) {
                                for (J = 0; J <= cpp_last_4; ++J) {
                                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                                    if (aConst::RaceToOwner(PlayerRace) == Planet->OwnerId) {
                                        HomePlanet = Planet;
                                        ++N;
                                        if (N == 1) {
                                            continue;
                                        }
                                        {
                                            const std::int32_t cpp_first = I + 1;
                                            const std::int32_t cpp_last_5 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                                            if (cpp_first <= cpp_last_5) {
                                                for (K = cpp_first; K <= cpp_last_5; ++K) {
                                                    OtherStar = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, K);
                                                    Distance = pas::real_round(aMyFunction::PointDistance(
                                                            OtherStar->Position, HomePlanet->CurrentStar->Position));
                                                    if (Distance > MaximumDistance) {
                                                        MaximumDistance = Distance;
                                                        SpecialStar = OtherStar;
                                                    } else if (Distance == MaximumDistance) {
                                                        {
                                                            double real_min = pas::real_min<double>(
                                                                static_cast<float>(static_cast<double>(aConst::GalaxySizeX) -
                                                                    SpecialStar->Position.X),
                                                                static_cast<double>(SpecialStar->Position.X));
                                                            EdgeDistance = static_cast<float>(pas::sqr(real_min) +
                                                                    pas::sqr(pas::real_min<double>(
                                                                        static_cast<float>(static_cast<double>(aConst::GalaxySizeY) -
                                                                        SpecialStar->Position.Y),
                                                                        static_cast<double>(SpecialStar->Position.Y))));
                                                        }
                                                        {
                                                            double real_min_3 = pas::real_min<double>(
                                                                static_cast<float>(static_cast<double>(aConst::GalaxySizeX) -
                                                                    OtherStar->Position.X),
                                                                static_cast<double>(OtherStar->Position.X));
                                                            OtherEdgeDistance = static_cast<float>(pas::sqr(real_min_3) +
                                                                    pas::sqr(pas::real_min<double>(
                                                                        static_cast<float>(static_cast<double>(aConst::GalaxySizeY) -
                                                                        OtherStar->Position.Y),
                                                                        static_cast<double>(OtherStar->Position.Y))));
                                                        }
                                                        if (OtherEdgeDistance < EdgeDistance) {
                                                            SpecialStar = OtherStar;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, pas::list_count(aGalaxy::Galaxy->Stars) - 1)->Name =
                SpecialStar->Name;
            for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc,
                aGalaxyStruct::oiPirate); cpp_range_2.next(OwnerId); ) {
                NameLists[OwnerId] = pas::make_object<pas::List>();
                if (GR_Main::LanguageDataConfig->GetBlock(
                    u"PlanetName"sv)->CountBlocks(aConst::OwnerInfo[OwnerId].InternalName) > 0) {
                    J =
                        GR_Main::LanguageDataConfig->GetBlock(
                            u"PlanetName"sv)->GetBlock(
                            pas::view(aConst::OwnerInfo[OwnerId].InternalName))->GetParamCount();
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, J - 1); cpp_range_3.next(K); ) {
                        pas::list_add(NameLists[OwnerId],
                            reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(K))));
                    }
                }
            }
            {
                const std::int32_t cpp_last_6 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                if (0 <= cpp_last_6) {
                    for (I = 0; I <= cpp_last_6; ++I) {
                        Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                        {
                            const std::int32_t cpp_last_7 = pas::list_count(Star->Planets) - 1;
                            if (0 <= cpp_last_7) {
                                for (J = 0; J <= cpp_last_7; ++J) {
                                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                                    if (Planet->Name == u"") {
                                        OwnerId = Planet->OwnerId;
                                        if (I < 5) {
                                            K = 0;
                                        } else if (pas::list_count(NameLists[OwnerId]) > 0) {
                                            K = aMyFunction::NextRandomIntRange(0,
                                                    pas::list_count(NameLists[OwnerId]) - 1,
                                                    aGalaxy::Galaxy->RandomState);
                                        } else {
                                            K = -1;
                                        }
                                        if (K >= 0) {
                                            Planet->Name = ([&] {
                                                    std::int32_t cpp_arg = static_cast<std::int32_t>(
                                                        reinterpret_cast<std::uintptr_t>(pas::list_get(
                                                            NameLists[OwnerId], K)));
                                                    EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(
                                                        u"PlanetName"sv)->GetBlock(
                                                        pas::view(aConst::OwnerInfo[OwnerId].InternalName));
                                                    return block->GetParamValue(cpp_arg);
                                                }());
                                            pas::list_delete(NameLists[OwnerId], K);
                                        } else {
                                            Planet->Name = pas::concat_wide({Star->Name, u"-",
                                                    pas::wide_int_to_str(J + 1)});
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc,
                aGalaxyStruct::oiPirate); cpp_range_4.next(OwnerId); ) {
                pas::free(NameLists[OwnerId]);
            }
            if (aGalaxy::Galaxy->CustomRules.StartInCenter) {
                Center.X = static_cast<float>(aConst::GalaxySizeX * 0.5);
                Center.Y = static_cast<float>(aConst::GalaxySizeY * 0.5);
                StartStar = HomePlanet->CurrentStar;
                Score = static_cast<float>(static_cast<float>(static_cast<double>(aMyFunction::PointDistanceSquared(Center, StartStar->Position)) -
                        (pas::list_count(StartStar->Planets) -
                            StartStar->CountPlanetsByOwner(aGalaxyStruct::oiUninhabited)) * 10) -
                        static_cast<float>(aMyFunction::PointDistanceSquared(SpecialStar->Position, StartStar->Position) * 0.25));
                {
                    const std::int32_t cpp_last_8 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                    if (0 <= cpp_last_8) {
                        for (I = 0; I <= cpp_last_8; ++I) {
                            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                            if (Star != SpecialStar && Star != StartStar &&
                                Star->CountPlanetsByOwner(aConst::RaceToOwner(PlayerRace)) >= 2) {
                                if (static_cast<float>(static_cast<float>(static_cast<double>(aMyFunction::PointDistanceSquared(Center, Star->Position)) -
                                    (pas::list_count(Star->Planets) -
                                        Star->CountPlanetsByOwner(aGalaxyStruct::oiUninhabited)) * 10) -
                                    static_cast<float>(aMyFunction::PointDistanceSquared(SpecialStar->Position, Star->Position) * 0.25)) <
                                    Score) {
                                    StartStar = Star;
                                    Score = static_cast<float>(static_cast<float>(static_cast<double>(aMyFunction::PointDistanceSquared(Center,
                                            Star->Position)) - (pas::list_count(Star->Planets) -
                                                Star->CountPlanetsByOwner(aGalaxyStruct::oiUninhabited)) * 10) -
                                            static_cast<float>(aMyFunction::PointDistanceSquared(SpecialStar->Position, Star->Position) *
                                                0.25));
                                }
                            }
                        }
                    }
                }
                if (StartStar != HomePlanet->CurrentStar) {
                    const std::int32_t cpp_last_9 = pas::list_count(StartStar->Planets) - 1;
                    if (0 <= cpp_last_9) {
                        for (I = 0; I <= cpp_last_9; ++I) {
                            aGalaxyStruct::TOwnerId cpp_left = pas::list_at<aPlanet::TPlanet>(StartStar->Planets,
                                I)->OwnerId;
                            if (cpp_left == aConst::RaceToOwner(PlayerRace)) {
                                HomePlanet = pas::list_at<aPlanet::TPlanet>(StartStar->Planets, I);
                                break;
                            }
                        }
                    }
                }
            }
            aGalaxy::Galaxy->HideSpecialConstellation();
            Stage = 6;
            aGalaxy::Galaxy->RefreshTechLevel();
            aGalaxy::Galaxy->RebuildStarDistances();
            Stage = 7;
            Player = pas::construct_call<aPlayer::TPlayer>(aPlayer::TPlayer_Create);
            Player->PortraitFaceId = CaptainPortraitIndex;
            Planet = HomePlanet;
            aGalaxy::PlayerStar = Planet->CurrentStar;
            switch (CharacterPreset) {
                case 1: {
                    Player->PreferredCareer = aGalaxyStruct::rcWarrior;
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = aMyFunction::NextRandomIntRange(70, 90,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcPirate] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcWarrior], cpp_right);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcTrader] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcWarrior] -
                            Player->CareerStatus[aGalaxyStruct::rcPirate];
                    break;
                }
                case 2: {
                    Player->PreferredCareer = aGalaxyStruct::rcWarrior;
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = aMyFunction::NextRandomIntRange(60, 70,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_2 = aMyFunction::NextRandomIntRange(3, 4, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcPirate] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcWarrior], cpp_right_2);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcTrader] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcWarrior] -
                            Player->CareerStatus[aGalaxyStruct::rcPirate];
                    break;
                }
                case 3: {
                    Player->PreferredCareer = aGalaxyStruct::rcTrader;
                    Player->CareerStatus[aGalaxyStruct::rcTrader] = aMyFunction::NextRandomIntRange(70, 90,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_3 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcPirate] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcTrader], cpp_right_3);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcTrader] -
                            Player->CareerStatus[aGalaxyStruct::rcPirate];
                    break;
                }
                case 4: {
                    Player->PreferredCareer = aGalaxyStruct::rcPirate;
                    Player->CareerStatus[aGalaxyStruct::rcPirate] = aMyFunction::NextRandomIntRange(60, 70,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_4 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcTrader] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcPirate], cpp_right_4);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcPirate] -
                            Player->CareerStatus[aGalaxyStruct::rcTrader];
                    break;
                }
                case 5: {
                    Player->PreferredCareer = aGalaxyStruct::rcPirate;
                    Player->CareerStatus[aGalaxyStruct::rcPirate] = aMyFunction::NextRandomIntRange(70, 90,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_5 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcTrader] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcPirate], cpp_right_5);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcPirate] -
                            Player->CareerStatus[aGalaxyStruct::rcTrader];
                    break;
                }
            }
            Player->InitializePlayerAtPlanet(Planet,
                aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[1]].StartingPlayerMoney,
                CharacterPreset);
            switch (CharacterPreset) {
                case 1: {
                    Player->PreferredCareer = aGalaxyStruct::rcWarrior;
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = aMyFunction::NextRandomIntRange(70, 90,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_6 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcPirate] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcWarrior], cpp_right_6);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcTrader] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcWarrior] -
                            Player->CareerStatus[aGalaxyStruct::rcPirate];
                    break;
                }
                case 2: {
                    Player->PreferredCareer = aGalaxyStruct::rcWarrior;
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = aMyFunction::NextRandomIntRange(60, 70,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_7 = aMyFunction::NextRandomIntRange(3, 4, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcPirate] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcWarrior], cpp_right_7);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcTrader] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcWarrior] -
                            Player->CareerStatus[aGalaxyStruct::rcPirate];
                    break;
                }
                case 3: {
                    Player->PreferredCareer = aGalaxyStruct::rcTrader;
                    Player->CareerStatus[aGalaxyStruct::rcTrader] = aMyFunction::NextRandomIntRange(70, 90,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_8 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcPirate] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcTrader], cpp_right_8);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcTrader] -
                            Player->CareerStatus[aGalaxyStruct::rcPirate];
                    break;
                }
                case 4: {
                    Player->PreferredCareer = aGalaxyStruct::rcPirate;
                    Player->CareerStatus[aGalaxyStruct::rcPirate] = aMyFunction::NextRandomIntRange(60, 70,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_9 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcTrader] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcPirate], cpp_right_9);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcPirate] -
                            Player->CareerStatus[aGalaxyStruct::rcTrader];
                    break;
                }
                case 5: {
                    Player->PreferredCareer = aGalaxyStruct::rcPirate;
                    Player->CareerStatus[aGalaxyStruct::rcPirate] = aMyFunction::NextRandomIntRange(70, 90,
                            aGalaxy::Galaxy->RandomState);
                    {
                        std::int32_t cpp_right_10 = aMyFunction::NextRandomIntRange(2, 3, aGalaxy::Galaxy->RandomState);
                        Player->CareerStatus[aGalaxyStruct::rcTrader] = pas::idiv(
                                100 - Player->CareerStatus[aGalaxyStruct::rcPirate], cpp_right_10);
                    }
                    Player->CareerStatus[aGalaxyStruct::rcWarrior] = 100 -
                            Player->CareerStatus[aGalaxyStruct::rcPirate] -
                            Player->CareerStatus[aGalaxyStruct::rcTrader];
                    break;
                }
            }
            aPlayer::SetPlayer(Player, aGalaxy::Galaxy);
            aPlayer::GetPlayer()->HomePlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
            aPlayer::GetPlayer()->Name = PlayerName;
            aRanger::PlayerOldQuests = pas::make_object<aMyFunction::TObjectList>();
            Globals::LastLoadedPlayerName = PlayerName;
            Stage = 8;
            aGalaxy::Galaxy->RefreshRangerWealthStats();
            aGalaxy::Galaxy->RefreshRangerStrengthStats();
            Stage = 9;
            Star =
                pas::checked_cast<aGalaxy::TStar*>(
                    static_cast<pas::Object*>(
                        aPlayer::GetPlayer()->CurrentStar->StarDistances[pas::list_count(aGalaxy::Galaxy->Stars) -
                        1].Star));
            aGalaxy::TGalaxy::CreateDominatorSpawnProxy(Star);
            I = 0;
            while (I < pas::list_count(aGalaxy::Galaxy->Stars)) {
                Star = SpecialStar->StarDistances[I].Star;
                if (aPlayer::GetPlayer()->CurrentStar == Star) {
                    ++I;
                } else {
                    aKling::TerronShip = pas::construct_call<aKling::TKling>(aShip::TShip_Create);
                    aKling::TerronShip->InitTerron(Star);
                    aKling::PieceCreatorTargetStarId = Star->Id;
                    ++I;
                    break;
                }
            }
            while (I < pas::list_count(aGalaxy::Galaxy->Stars)) {
                Star = SpecialStar->StarDistances[I].Star;
                if (aPlayer::GetPlayer()->CurrentStar == Star) {
                    ++I;
                } else {
                    aKling::BlazerShip = pas::construct_call<aKling::TKling>(aShip::TShip_Create);
                    aKling::BlazerShip->InitBlazer(Star);
                    ++I;
                    break;
                }
            }
            while (I < pas::list_count(aGalaxy::Galaxy->Stars)) {
                Star = SpecialStar->StarDistances[I].Star;
                if (aPlayer::GetPlayer()->CurrentStar == Star) {
                    ++I;
                } else {
                    aKling::KellerShip = pas::construct_call<aKling::TKling>(aShip::TShip_Create);
                    aKling::KellerShip->InitKeller(Star);
                    break;
                }
            }
            Stage = 10;
            N = pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetInitialDominatorControlPercent() *
                    (static_cast<float>(pas::list_count(aGalaxy::Galaxy->Stars) / 1.0E+2))));
            if (N > pas::list_count(aGalaxy::Galaxy->Stars) - 1) {
                N = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            }
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, N); cpp_range_5.next(I); ) {
                Star =
                    pas::checked_cast<aGalaxy::TStar*>(
                        static_cast<pas::Object*>(aKling::BlazerShip->CurrentStar->StarDistances[I].Star));
                if (aPlayer::GetPlayer()->CurrentStar->Constellation != Star->Constellation &&
                    aPlayer::GetPlayer()->CurrentStar->StarDistances[1].Star != Star && Star->Constellation->Id != 20) {
                    if (aKling::BlazerShip->CurrentStar == Star || aKling::KellerShip->CurrentStar == Star ||
                        aKling::TerronShip->CurrentStar == Star) {
                        Star->Status.ControlFaction = aGalaxyStruct::sfDominators;
                    }
                    if ((pas::list_indexof(aGalaxy::Galaxy->Constellations,
                        reinterpret_cast<void*>(Star->Constellation)) >= 5 ||
                        aMyFunction::NextRandomUnitFloat(aGalaxy::Galaxy->RandomState) >= 0.4) && (N / 2 > I ||
                            aMyFunction::NextRandomUnitFloat(aGalaxy::Galaxy->RandomState) < 0.8)) {
                        Star->Status.ControlFaction = aGalaxyStruct::sfDominators;
                    }
                }
            }
            Stage = 11;
            N = pas::real_round(static_cast<float>(static_cast<float>(pas::list_count(aGalaxy::Galaxy->Stars) / 1.0E+2) *
                    aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[0]].InitialPirateControlPercent));
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, N); cpp_range_6.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 70);
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Star->StarDistances[I].Star));
                if (aPlayer::GetPlayer()->CurrentStar->Constellation != Star->Constellation &&
                    aPlayer::GetPlayer()->CurrentStar->StarDistances[1].Star != Star && Star->Constellation->Id != 20 &&
                    (pas::list_indexof(aGalaxy::Galaxy->Constellations, reinterpret_cast<void*>(Star->Constellation)) >=
                        5 || aMyFunction::NextRandomUnitFloat(aGalaxy::Galaxy->RandomState) >= 0.4) && (N / 2 > I ||
                        aMyFunction::NextRandomUnitFloat(aGalaxy::Galaxy->RandomState) < 0.8)) {
                    Star->Status.ControlFaction = aGalaxyStruct::sfPirates;
                }
            }
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 70)->Status.ControlFaction = aGalaxyStruct::sfPirates;
            pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 71)->Status.ControlFaction = aGalaxyStruct::sfPirates;
            Stage = 12;
            fIntroduction::NewGameGenerationStage = 3;
            aGalaxy::Galaxy->AssignTextQuestsToPlanets();
            aGalaxy::Galaxy->InitializeConstellationDistanceTiers();
            {
                const std::int32_t cpp_last_10 = pas::list_count(aGalaxy::Galaxy->Constellations) - 1;
                if (0 <= cpp_last_10) {
                    for (I = 0; I <= cpp_last_10; ++I) {
                        Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
                        if (Constellation->SharesOutlineSegment(
                            aPlayer::GetPlayer()->HomePlanet->CurrentStar->Constellation)) {
                            Constellation->Visible = true;
                        } else {
                            Constellation->Visible = false;
                        }
                    }
                }
            }
            Stage = 13;
            fIntroduction::NewGameGenerationStage = 4;
            {
                const std::int32_t cpp_last_11 = pas::list_count(aGalaxy::Galaxy->Planets) - 1;
                if (0 <= cpp_last_11) {
                    for (I = 0; I <= cpp_last_11; ++I) {
                        Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
                        if ((Planet->IsCoalitionOwned || Planet->OwnerId == aGalaxyStruct::oiPirate) &&
                            Planet->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                            Planet->OwnerId = aGalaxyStruct::oiDominator;
                            Planet->UpdateOwnerFlags();
                        }
                        if (Planet->OwnerId != aGalaxyStruct::oiUninhabited &&
                            Planet->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                            Planet->OwnerId = aGalaxyStruct::oiPirate;
                            Planet->UpdateOwnerFlags();
                        }
                        {
                            aGalaxyStruct::TOwnerId cpp_case = Planet->OwnerId;
                            if (cpp_case >= aGalaxyStruct::oiMaloc && cpp_case <= aGalaxyStruct::oiGaal) {
                                Planet->SpawnTransport(0, 100);
                                Planet->SpawnTransport(0, 100);
                                Planet->BuyWarrior(100);
                                if (pas::list_count(aGalaxy::Galaxy->Rangers) <
                                    static_cast<float>(aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0x1.3333333333334p+0)) {
                                    Planet->BuyRanger(100);
                                    aGalaxy::Galaxy->RefreshRangerStrengthStats();
                                }
                            } else if (cpp_case == aGalaxyStruct::oiDominator) {
                                while (Planet->CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] < 10 &&
                                    (Planet->CurrentStar->SumBestRangerRelativeStrength(
                                        static_cast<aGalaxyStruct::TShipTypeMask>(InitialDominatorShipMask)) <
                                        aConst::DominatorRetreatStrengthByTier[
                                            Planet->CurrentStar->Constellation->HomeDistanceTier] ||
                                        Planet->CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] < 8)) {
                                    Planet->SpawnWeightedDominatorShip();
                                }
                            } else if (cpp_case == aGalaxyStruct::oiPirate) {
                                Planet->BuyPirate(100);
                                Planet->BuyPirate(100);
                                Planet->BuyWarrior(100);
                                Planet->BuyWarrior(100);
                                Planet->BuyWarrior(100);
                            }
                        }
                    }
                }
            }
            Stage = 14;
            fIntroduction::NewGameGenerationStage = 5;
            {
                aGalaxy::TStar* currentStar = aPlayer::GetPlayer()->CurrentStar;
                aRuins::TRuins* cpp_arg_2 = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                cpp_arg_2->Init(static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstRangerCenter), currentStar,
                    pas::WideString());
            }
            {
                aGalaxy::TStar* currentStar_2 = aPlayer::GetPlayer()->CurrentStar;
                aRuins::TRuins* cpp_arg_3 = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                cpp_arg_3->Init(static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstScienceBase), currentStar_2,
                    pas::WideString());
            }
            {
                aGalaxy::TStar* currentStar_3 = aPlayer::GetPlayer()->CurrentStar;
                aRuins::TRuins* cpp_arg_4 = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                cpp_arg_4->Init(static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstMedicalBase), currentStar_3,
                    pas::WideString());
            }
            {
                aGalaxy::TStar* cpp_arg_5 = pas::checked_cast<aGalaxy::TStar*>(
                    static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[3].Star));
                aRuins::TRuins* cpp_arg_6 = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                cpp_arg_6->Init(static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstPirateBase), cpp_arg_5,
                    pas::WideString());
            }
            {
                aGalaxy::TStar* cpp_arg_7 = pas::checked_cast<aGalaxy::TStar*>(
                    static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[1].Star));
                aRuins::TRuins* cpp_arg_8 = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                cpp_arg_8->Init(static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstBusinessCenter), cpp_arg_7,
                    pas::WideString());
            }
            {
                aGalaxy::TStar* cpp_arg_9 = pas::checked_cast<aGalaxy::TStar*>(
                    static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[2].Star));
                aRuins::TRuins* cpp_arg_10 = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                cpp_arg_10->Init(static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstMilitaryBase), cpp_arg_9,
                    pas::WideString());
            }
            aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
            Stage = 15;
            {
                const std::int32_t cpp_last_12 = pas::list_count(aGalaxy::Galaxy->Rangers) - 1;
                if (1 <= cpp_last_12) {
                    for (I = 1; I <= cpp_last_12; ++I) {
                        Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(1,
                            static_cast<std::int32_t>(pas::real_round(aMyFunction::RemapClamped(
                                aMyFunction::NextRandomIntRange(0, 1000, Ranger->RandomState), 0.0, 1.0E+3, 5.0,
                                5.0E+1)))); cpp_range_7.next(N); ) {
                            Ranger->SimulateUnseenProgression();
                        }
                    }
                }
            }
            Stage = 16;
            aGalaxy::TGalaxy::RunConfigOnStartHandlers();
            aGalaxy::TGalaxy::AppendIntegritySnapshot();
            fIntroduction::NewGameGenerationStage = 6;
            Stage = 17;
            aCalc::CalculateGalaxyTurnAndWait();
            if (GR_Main::ExitScreenLoop) {
                return;
            }
            for (I = 1; I <= aGalaxyStruct::GalaxyWarmupTurns; ++I) {
                if (I % 20 == 0) {
                    SysUtilsImports::Sleep(1u);
                }
                aCalc::CalculatePlayerStarTurnAndWait();
                if (GR_Main::ExitScreenLoop) {
                    return;
                }
                aCalc::CalculateGalaxyTurnAndWait();
                if (GR_Main::ExitScreenLoop) {
                    return;
                }
            }
            Stage = 18;
            aPlayer::GetPlayer()->CurrentPlanet = nullptr;
            aPlayer::GetPlayer()->DockedTo = nullptr;
            {
                const std::int32_t cpp_last_13 = pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1;
                if (0 <= cpp_last_13) {
                    for (I = 0; I <= cpp_last_13; ++I) {
                        Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
                        if (Ship->TypeId == aGalaxyStruct::rstRangerCenter) {
                            aPlayer::GetPlayer()->DockedTo = Ship;
                            break;
                        }
                    }
                }
            }
            if (aPlayer::GetPlayer()->DockedTo == nullptr) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Galaxy create exception, not found rc, seed = ",
                    SysUtils::IntToStr(aGalaxy::Galaxy->GenerationSeed)}));
                if (aPlayer::GetPlayer()->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                    const std::int32_t cpp_last_14 = pas::list_count(aPlayer::GetPlayer()->CurrentStar->Planets) - 1;
                    if (0 <= cpp_last_14) {
                        for (I = 0; I <= cpp_last_14; ++I) {
                            if (pas::list_at<aPlanet::TPlanet>(aPlayer::GetPlayer()->CurrentStar->Planets,
                                I)->OwnerId != aGalaxyStruct::oiUninhabited) {
                                aPlayer::GetPlayer()->CurrentPlanet = pas::list_at<aPlanet::TPlanet>(
                                        aPlayer::GetPlayer()->CurrentStar->Planets, I);
                                break;
                            }
                        }
                    }
                }
                if (aPlayer::GetPlayer()->CurrentPlanet == nullptr) {
                    const std::int32_t cpp_last_15 = pas::list_count(aPlayer::GetPlayer()->CurrentStar->Planets) - 1;
                    if (0 <= cpp_last_15) {
                        for (I = 0; I <= cpp_last_15; ++I) {
                            if (pas::list_at<aPlanet::TPlanet>(aPlayer::GetPlayer()->CurrentStar->Planets,
                                I)->OwnerId == aGalaxyStruct::oiUninhabited) {
                                aPlayer::GetPlayer()->CurrentPlanet = pas::list_at<aPlanet::TPlanet>(
                                        aPlayer::GetPlayer()->CurrentStar->Planets, I);
                                break;
                            }
                        }
                    }
                }
                if (aPlayer::GetPlayer()->CurrentPlanet == nullptr) {
                    aPlayer::GetPlayer()->CurrentPlanet = pas::list_at<aPlanet::TPlanet>(
                            aPlayer::GetPlayer()->CurrentStar->Planets, 0);
                }
            }
            aGalaxy::Galaxy->IronWill = Globals::NewGameGenerationThread->IronWill;
            for (I = 0; I <= 1; ++I) {
                ++aPlayer::GetPlayer()->BaseSkills[StartingSkills[I]];
            }
            for (I = 0; I <= 1; ++I) {
                switch (StartingItemTypes[I]) {
                    case 43: {
                        Item = pas::construct_call<aItem::TFuelTanks>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TFuelTanks*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::FuelTanksBaseSize) *
                                aConst::EquipmentSizeFactors[4])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 44: {
                        Item = pas::construct_call<aItem::TEngine>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TEngine*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::EngineBaseSize) *
                                aConst::EquipmentSizeFactors[3])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 45: {
                        Item = pas::construct_call<aItem::TRadar>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TRadar*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::RadarBaseSize) *
                                aConst::EquipmentSizeFactors[3])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 46: {
                        Item = pas::construct_call<aItem::TScaner>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TScaner*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::ScannerBaseSize) *
                                aConst::EquipmentSizeFactors[4])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 47: {
                        Item = pas::construct_call<aItem::TRepairRobot>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TRepairRobot*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::RepairRobotBaseSize) *
                                aConst::EquipmentSizeFactors[3])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 48: {
                        Item = pas::construct_call<aItem::TCargoHook>(aItem::TCargoHook_Create);
                        pas::checked_cast<aItem::TCargoHook*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::CargoHookBaseSize) *
                                aConst::EquipmentSizeFactors[3])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 49: {
                        Item = pas::construct_call<aItem::TDefGenerator>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TDefGenerator*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::DefGeneratorBaseSize) *
                                aConst::EquipmentSizeFactors[3])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 50: {
                        Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TWeapon*>(Item)->Init(aConst::t_IndustrialLaser,
                            pas::real_round(
                                static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_IndustrialLaser].AverageSize) *
                                    aConst::EquipmentSizeFactors[4])), 3, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 51: {
                        Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TWeapon*>(Item)->Init(aConst::t_FragmentationCannon,
                            pas::real_round(
                                static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_FragmentationCannon].AverageSize) *
                                    aConst::EquipmentSizeFactors[3])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 52: {
                        Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TWeapon*>(Item)->Init(aConst::t_Flux,
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Flux].AverageSize) *
                                aConst::EquipmentSizeFactors[2])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 53: {
                        Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TWeapon*>(Item)->Init(aConst::t_MissileLauncher,
                            pas::real_round(
                                static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_MissileLauncher].AverageSize) *
                                    aConst::EquipmentSizeFactors[3])), 1, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    case 54: {
                        Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TWeapon*>(Item)->Init(aConst::t_Treton,
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Treton].AverageSize) *
                                aConst::EquipmentSizeFactors[4])), 1, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                    default: {
                        Item = pas::construct_call<aItem::TRadar>(aItem::TEquipment_Create);
                        pas::checked_cast<aItem::TRadar*>(Item)->Init(
                            pas::real_round(static_cast<float>(static_cast<double>(aConst::RadarBaseSize) *
                                aConst::EquipmentSizeFactors[4])), 2, aPlayer::GetPlayer()->OwnerId);
                        break;
                    }
                }
                switch (aGalaxy::Galaxy->DifficultyLevels[7]) {
                    case 0: Item->Improve(aItem::ikMajor); break;
                    case 1: Item->Improve(aItem::ikMedium); break;
                    case 2: Item->Improve(aItem::ikMinor); break;
                }
                pas::get_mem_at(&Entry, 12);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                if (aPlayer::GetPlayer()->DockedTo != nullptr) {
                    Entry->LocationOwner = aPlayer::GetPlayer()->DockedTo;
                } else {
                    Entry->LocationOwner = aPlayer::GetPlayer()->CurrentPlanet;
                }
                Entry->Item = Item;
                Entry->SlotIndex = 0;
            }
            Stage = 19;
            // Native code passes the last planet visited by the population loop above.
            aPlayer::GetPlayer()->ApplyCharacterPreset(Planet,
                aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[1]].StartingPlayerMoney,
                CharacterPreset);
            aPlayer::GetPlayer()->RefreshStorageBubbles();
            aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 0);
            fIntroduction::NewGameGenerationStage = 8;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Galaxy create exception, label = ",
                    SysUtils::IntToStr(Stage), " seed = ", SysUtils::IntToStr(aGalaxy::Galaxy->GenerationSeed)}));
            } else {
                throw;
            }
        }
    }

} // namespace fGameSettings
