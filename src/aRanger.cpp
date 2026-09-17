#include "layout/aRanger.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/TextFieldClass.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMissile.hpp"
#include "types/aPirate.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/aTransport.hpp"
#include "types/aWarrior.hpp"
#include "units/Achievements.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/TextQuest.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aShip.hpp"

namespace aRanger {
    aShip::TShip* PendingPlayerFollowTarget = nullptr;

    // Auto-equips in the follow helper and uses NPC dialogue branches for the player.
    std::uint8_t PlayerAutomaticControl = false;

    // Set by equipped-item breakage; ShouldContinuePlayerTravel checks it.
    std::uint8_t PlayerEquipmentBrokenThisTurn = false;

    // Career, then bonSkill1..bonSkill6.
    pas::Array<pas::Array<std::int32_t, 22, 27>, 0, 2> RangerSkillBonusEvaluationWeights = pas::Array<pas::Array<std::int32_t, 22, 27>, 0, 2>{{pas::Array<std::int32_t, 22, 27>{{70, 70, 100, 100, 50, 50}}, pas::Array<std::int32_t, 22, 27>{{100, 80, 80, 80, 70, 50}}, pas::Array<std::int32_t, 22, 27>{{80, 100, 100, 80, 50, 80}}}};

    // Career, then bonSlotRadar..bonSlotForsage.
    pas::Array<pas::Array<std::int32_t, 13, 20>, 0, 2> RangerSlotBonusEvaluationWeights = pas::Array<pas::Array<std::int32_t, 13, 20>, 0, 2>{{pas::Array<std::int32_t, 13, 20>{{150, 80, 100, 500, 100, 75, 30, 30}}, pas::Array<std::int32_t, 13, 20>{{100, 150, 150, 500, 150, 150, 30, 50}}, pas::Array<std::int32_t, 13, 20>{{120, 120, 200, 500, 200, 100, 30, 30}}}};

    // Owned PPlayerOldQuest records.
    pas::List* PlayerOldQuests{};

    // Requires registered ranger/home-planet state. Removes quests and relation-column entries, adjusts the player index and refreshes galaxy ratings.
    void TRanger_Destroy(TRanger* Self) {
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        PQuest Quest{};
        PPlayerOldQuest OldQuest{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        --Self->HomePlanet->HomeRangerCount;
        if (Self->Quests != nullptr) {
            const std::int32_t cpp_first = pas::list_count(Self->Quests) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Quest = pas::list_at<TQuest>(Self->Quests, I);
                    pas::list_delete(Self->Quests, I);
                    pas::dispose(Quest);
                }
            }
        }
        if (aPlayer::GetPlayer() == Self) {
            {
                const std::int32_t cpp_first_2 = pas::list_count(PlayerOldQuests) - 1;
                if (cpp_first_2 >= 0) {
                    for (I = cpp_first_2; I >= 0; --I) {
                        OldQuest = pas::list_at<TPlayerOldQuest>(PlayerOldQuests, I);
                        pas::list_delete(PlayerOldQuests, I);
                        pas::dispose(OldQuest);
                    }
                }
            }
            pas::list_clear(PlayerOldQuests);
        }
        std::int32_t RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(Self));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (pas::in_set<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate, 6, 13>(Ship->TypeId) && Ship != Self) {
                    pas::list_delete(Ship->RangerRelations, RangerIndex);
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range_3.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
            pas::list_delete(Planet->RangerRelations, RangerIndex);
        }
        if (aGalaxy::Galaxy->PlayerRangerIndex == RangerIndex) {
            aGalaxy::Galaxy->PlayerRangerIndex = -1;
            aPlayer::SetPlayer(nullptr, aGalaxy::Galaxy);
        } else if (aGalaxy::Galaxy->PlayerRangerIndex > RangerIndex) {
            --aGalaxy::Galaxy->PlayerRangerIndex;
        }
        pas::list_delete(aGalaxy::Galaxy->Rangers, RangerIndex);
        if (aGalaxy::Galaxy->WealthiestRanger == Self) {
            aGalaxy::Galaxy->RefreshRangerWealthStats();
        }
        if (aGalaxy::Galaxy->StrongestRanger == Self) {
            aGalaxy::Galaxy->RefreshRangerStrengthStats();
        }
        aGalaxy::Galaxy->RefreshRangerRatingPlaces();
        aNormalShip::TNormalShip_Destroy(Self);
    }

    // For a fresh inherited TNormalShip instance; creates loadout, career, quests and relation entries and registers it in the galaxy.
    void TRanger::InitializeAtPlanet(aPlanet::TPlanet* Planet, std::int32_t InitialMoney) {
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* OtherPlanet{};
        std::uint8_t ProgramIndex{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        TRanger* Ranger{};
        // Caller-popped static link; ranger at -4. Tries unused race/faction names and appends an ID suffix when exhausted.
        auto SelectName = [&](EC_BlockPar::TBlockParEC* Config) -> void {
            std::int32_t Index{};
            std::int32_t I{};
            std::int32_t J{};
            std::int32_t LastIndex{};
            std::int32_t FirstIndex{};
            std::uint8_t Used{};
            TRanger* Other{};
            EC_BlockPar::TBlockParEC* Block{};
            if (Config != nullptr && Config->CountBlocks(u"Ranger"_wref.get()) != 0) {
                Block = Config->GetBlock(u"Ranger"_wref.get());
                if (aConst::RaceToOwner(this->PilotRace) == this->OwnerId) {
                    Block = Block->GetBlock(aConst::OwnerToSys(this->OwnerId));
                } else {
                    if (Block->CountBlocks(aConst::OwnerToSys(this->OwnerId)) > 0) {
                        Block = Block->GetBlock(aConst::OwnerToSys(this->OwnerId));
                    }
                    if (Block->CountBlocks(aConst::OwnerToSys(aConst::RaceToOwner(this->PilotRace))) > 0) {
                        Block = Block->GetBlock(aConst::OwnerToSys(aConst::RaceToOwner(this->PilotRace)));
                    }
                }
                FirstIndex = 0;
                LastIndex = Block->GetParamCount() - 1;
                Index = aMyFunction::NextRandomIntRange(FirstIndex, LastIndex, this->RandomState);
                for (auto cpp_range = pas::for_to<std::int32_t>(FirstIndex, LastIndex); cpp_range.next(I); ) {
                    this->Name = Block->GetParamValue(Index);
                    Used = false;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_2.next(J); ) {
                        Other = pas::list_at<TRanger>(aGalaxy::Galaxy->Rangers, J);
                        if (this != Other && Other->Name == this->Name) {
                            Used = true;
                            break;
                        }
                    }
                    if (!Used) {
                        break;
                    }
                    aMyFunction::IncrementWrapped(Index, FirstIndex, LastIndex);
                    if (I == LastIndex) {
                        this->Name = pas::concat_wide({this->Name, u" ", u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(this->Id) % 100 + 1)), u"-"});
                    }
                }
            }
        };
        HomePlanet = Planet;
        CurrentPlanet = HomePlanet;
        CurrentStar = CurrentPlanet->CurrentStar;
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        TypeId = aGalaxyStruct::stRanger;
        PilotRace = HomePlanet->RaceId;
        OwnerId = aConst::RaceToOwner(PilotRace);
        SetMoney(InitialMoney);
        Aggression = aMyFunction::SeededRandomIntRange(0, 100, Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn));
        LastDockedPlanet = nullptr;
        LastDockedNonPlanetLocation = nullptr;
        BaseNodes = 200;
        for (ProgramIndex = static_cast<std::uint8_t>(0); ProgramIndex <= static_cast<std::uint8_t>(11); ++ProgramIndex) {
            ProgramCounts[ProgramIndex] = 0;
        }
        {
            std::int32_t cpp_case = aMyFunction::NextRandomIntRange(0, 100, RandomState);
            if (cpp_case >= 0 && cpp_case <= 39) {
                PreferredCareer = aGalaxyStruct::rcTrader;
            } else if (cpp_case >= 40 && cpp_case <= 49) {
                PreferredCareer = aGalaxyStruct::rcPirate;
            } else if (cpp_case >= 50 && cpp_case <= 100) {
                PreferredCareer = aGalaxyStruct::rcWarrior;
            }
        }
        switch (PreferredCareer) {
            case aGalaxyStruct::rcTrader: {
                CareerStatus[aGalaxyStruct::rcTrader] = 90;
                CareerStatus[aGalaxyStruct::rcPirate] = 5;
                CareerStatus[aGalaxyStruct::rcWarrior] = 5;
                EminentProgress[aGalaxyStruct::rcTrader] = 90;
                EminentProgress[aGalaxyStruct::rcPirate] = 0;
                EminentProgress[aGalaxyStruct::rcWarrior] = 0;
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[1];
                }
                ++BaseSkills[2];
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[2];
                }
                ++BaseSkills[3];
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[3];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[4];
                }
                break;
            }
            case aGalaxyStruct::rcPirate: {
                CareerStatus[aGalaxyStruct::rcTrader] = 30;
                CareerStatus[aGalaxyStruct::rcPirate] = 40;
                CareerStatus[aGalaxyStruct::rcWarrior] = 30;
                EminentProgress[aGalaxyStruct::rcTrader] = 0;
                EminentProgress[aGalaxyStruct::rcPirate] = 70;
                EminentProgress[aGalaxyStruct::rcWarrior] = 0;
                ++BaseSkills[0];
                ++BaseSkills[0];
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[1];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[1];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[3];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[3];
                }
                ++BaseSkills[4];
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[4];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[5];
                }
                break;
            }
            case aGalaxyStruct::rcWarrior: {
                CareerStatus[aGalaxyStruct::rcTrader] = 30;
                CareerStatus[aGalaxyStruct::rcPirate] = 30;
                CareerStatus[aGalaxyStruct::rcWarrior] = 40;
                EminentProgress[aGalaxyStruct::rcTrader] = 0;
                EminentProgress[aGalaxyStruct::rcPirate] = 0;
                EminentProgress[aGalaxyStruct::rcWarrior] = 90;
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[1];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[1];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[3];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[0];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[2];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[2];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    ++BaseSkills[5];
                }
                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) {
                    ++BaseSkills[5];
                }
                break;
            }
        }
        ClearPendingCareerActivity();
        Name = pas::WideString();
        SelectName(GR_Main::ModShipNameConfig);
        if (GetName().length() == 0) {
            SelectName(GR_Main::LanguageDataConfig->GetBlock(u"ShipName"_wref.get()));
        }
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled && !(pas::class_cast_if<aPlayer::TPlayer*>(this) != nullptr);
        {
            std::uint8_t raceToOwner = aConst::RaceToOwner(PilotRace);
            std::int32_t selectRandomHullSeries = SelectRandomHullSeries();
            std::uint8_t cpp_arg = HomePlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
            std::uint16_t round = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
            aShip::TShip* self = this;
            aShip::TShip_CreateAndEquipHull(self, round, 1, raceToOwner, selectRandomHullSeries, cpp_arg);
        }
        CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, OwnerId);
        {
            std::uint8_t ownerId = OwnerId;
            pas::Extended cpp_left = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(1, 2, RandomState)];
            std::int32_t round_2 = System::Round(cpp_left * aConst::EngineBaseSize);
            std::uint8_t nextRandomIntRange = aMyFunction::NextRandomIntRange(1, 2, RandomState);
            aShip::TShip* self_2 = this;
            self_2->CreateAndEquipEngine(round_2, nextRandomIntRange, ownerId);
        }
        if (GetSlotCountForItemType(aConst::t_CargoHook) > 0) {
            std::uint8_t ownerId_2 = OwnerId;
            std::uint8_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(1, 2, RandomState);
            std::int32_t cargoHookBaseSize = aConst::CargoHookBaseSize;
            aShip::TShip* self_3 = this;
            self_3->CreateAndEquipCargoHook(cargoHookBaseSize, nextRandomIntRange_2, ownerId_2);
        }
        if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
            CreateAndEquipWeapon(aConst::t_Weapon1, aConst::WeaponInfos[aConst::t_Weapon1].AverageSize, 1, OwnerId);
        }
        if (GetSlotCountForItemType(aConst::t_Radar) > 0) {
            std::uint8_t ownerId_3 = OwnerId;
            pas::Extended cpp_left_2 = aConst::EquipmentSizeFactors[aMyFunction::NextRandomIntRange(2, 4, RandomState)];
            std::int32_t round_3 = System::Round(cpp_left_2 * aConst::RadarBaseSize);
            aShip::TShip* self_4 = this;
            self_4->CreateAndEquipRadar(round_3, 1, ownerId_3);
        }
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
        SmoothedSpeed = Speed;
        SmoothedEnemySpeed = Speed;
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        aShip::TShip_BuyEquipmentAtLocation(this, true);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(I); ) {
            OtherPlanet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
            pas::list_add(OtherPlanet->RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(*([&] {
                auto cpp_index = aConst::RaceToOwner(PilotRace) & 0x0000007f;
                auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(OtherPlanet->RaceId) & 0x0000007f];
                return &(*cpp_array)[cpp_index];
            }())))));
        }
        pas::list_add(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(this));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (pas::in_set<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate, 6, 13>(Ship->TypeId) && Ship != this) {
                    pas::list_add(Ship->RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(*([&] {
                        auto cpp_index_2 = aConst::RaceToOwner(PilotRace) & 0x0000007f;
                        auto* cpp_array_2 = &aConst::OwnerRelations[aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f];
                        return &(*cpp_array_2)[cpp_index_2];
                    }())))));
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_4.next(I); ) {
            Ranger = pas::list_at<TRanger>(aGalaxy::Galaxy->Rangers, I);
            pas::list_add(RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(*([&] {
                auto cpp_index_3 = aConst::RaceToOwner(Ranger->PilotRace) & 0x0000007f;
                auto* cpp_array_3 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                return &(*cpp_array_3)[cpp_index_3];
            }())))));
        }
        aGalaxy::Galaxy->RefreshRangerRatingPlaces();
        Quests = pas::make_object<pas::List>();
        PrisonTermRemaining = 0;
    }

    // Appends Self and relation entries; requires an unregistered ranger with initialized lists.
    void TRanger::RegisterInGalaxyRelations() {
        aPlanet::TPlanet* Planet{};
        std::int32_t I{};
        std::int32_t J{};
        TRanger* Ranger{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, I);
            pas::list_add(Planet->RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(aConst::OwnerRelations[aConst::RaceToOwner(Planet->RaceId) & 0x0000007f][OwnerId]))));
        }
        pas::list_add(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(this));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (pas::in_set<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate, 6, 13>(Ship->TypeId) && Ship != this) {
                    pas::list_add(Ship->RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(aConst::OwnerRelations[Ship->OwnerId][OwnerId]))));
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_4.next(I); ) {
            Ranger = pas::list_at<TRanger>(aGalaxy::Galaxy->Rangers, I);
            pas::list_add(RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(aConst::OwnerRelations[OwnerId][Ranger->OwnerId]))));
        }
    }

    void TRanger::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        PQuest Quest{};
        std::uint8_t ProgramIndex{};
        aNormalShip::TNormalShip::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(CareerStatus[aGalaxyStruct::rcTrader]);
        Buffer->AddAnsiChar(CareerStatus[aGalaxyStruct::rcPirate]);
        Buffer->AddAnsiChar(CareerStatus[aGalaxyStruct::rcWarrior]);
        Buffer->AddAnsiChar(EminentProgress[aGalaxyStruct::rcTrader]);
        Buffer->AddAnsiChar(EminentProgress[aGalaxyStruct::rcPirate]);
        Buffer->AddAnsiChar(EminentProgress[aGalaxyStruct::rcWarrior]);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(PreferredCareer));
        Buffer->AddAnsiChar(Aggression);
        std::int32_t Count = pas::list_count(Quests);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Quest = pas::list_at<TQuest>(Quests, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Quest->QuestType));
            Buffer->AddWideChar(Quest->QuestNumber);
            Buffer->AddIntegerValue(Quest->DeadlineTurn);
            Buffer->AddIntegerValue(Quest->RewardMoney);
            if (Quest->Planet == nullptr) {
                Buffer->AddDWord(0u);
            } else {
                Buffer->AddDWord(Quest->Planet->Id);
            }
            if (pas::class_cast_if<aPlanet::TPlanet*>(Quest->ObjectiveTarget) != nullptr) {
                Buffer->AddDWord(pas::checked_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget)->Id);
            } else if (pas::class_cast_if<aShip::TShip*>(Quest->ObjectiveTarget) != nullptr) {
                Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(Quest->ObjectiveTarget)->Id);
            } else if (pas::class_cast_if<aItem::TArtefact*>(Quest->ObjectiveTarget) != nullptr) {
                Buffer->AddDWord(pas::checked_cast<aItem::TArtefact*>(Quest->ObjectiveTarget)->Id);
            } else if (pas::class_cast_if<aGalaxy::TStar*>(Quest->ObjectiveTarget) != nullptr) {
                Buffer->AddDWord(pas::checked_cast<aGalaxy::TStar*>(Quest->ObjectiveTarget)->Id);
            } else {
                Buffer->AddDWord(0u);
            }
            Buffer->AddBoolean(Quest->Successful);
            Buffer->AddWideStringZ(Quest->Description);
            Buffer->AddWideStringZ(Quest->CompletionText);
            Buffer->AddWideStringZ(Quest->SpecialCompletionText);
        }
        Buffer->AddAnsiChar(PendingCareerActivity[0]);
        Buffer->AddAnsiChar(PendingCareerActivity[1]);
        Buffer->AddAnsiChar(PendingCareerActivity[2]);
        Buffer->AddDWord(PrisonTermRemaining);
        if (LastDockedNonPlanetLocation == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(LastDockedNonPlanetLocation->Id);
        }
        Buffer->AddIntegerValue(BaseNodes);
        for (ProgramIndex = static_cast<std::uint8_t>(0); ProgramIndex <= static_cast<std::uint8_t>(11); ++ProgramIndex) {
            Buffer->AddIntegerValue(ProgramCounts[ProgramIndex]);
        }
        Buffer->AddBoolean(ExcludedFromRating);
    }

    // Creates the quest list and loads IDs for later resolution; rejects quest counts above 10000.
    void TRanger::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        PQuest Quest{};
        std::uint8_t ProgramIndex{};
        aNormalShip::TNormalShip::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion < 139 && CreationTurn < 666) {
            CreationTurn = 667;
        }
        CareerStatus[aGalaxyStruct::rcTrader] = EC_Buf::TBufEC_GetByte(Buffer);
        CareerStatus[aGalaxyStruct::rcPirate] = EC_Buf::TBufEC_GetByte(Buffer);
        CareerStatus[aGalaxyStruct::rcWarrior] = EC_Buf::TBufEC_GetByte(Buffer);
        EminentProgress[aGalaxyStruct::rcTrader] = EC_Buf::TBufEC_GetByte(Buffer);
        EminentProgress[aGalaxyStruct::rcPirate] = EC_Buf::TBufEC_GetByte(Buffer);
        EminentProgress[aGalaxyStruct::rcWarrior] = EC_Buf::TBufEC_GetByte(Buffer);
        PreferredCareer = static_cast<aGalaxyStruct::TRangerCareer>(EC_Buf::TBufEC_GetByte(Buffer));
        Aggression = EC_Buf::TBufEC_GetByte(Buffer);
        // Native loading uses the custom list; fresh initialization uses TList.
        Quests = pas::make_object<aMyFunction::TObjectList>();
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        // The signed lower bound is retained even though GetWord cannot return it.
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err in FQuests load"_a));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            pas::new_value(Quest);
            pas::list_add(Quests, static_cast<void*>(Quest));
            Quest->QuestType = static_cast<aGalaxyStruct::TQuestType>(EC_Buf::TBufEC_GetByte(Buffer));
            Quest->QuestNumber = EC_Buf::TBufEC_GetWord(Buffer);
            Quest->DeadlineTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Quest->RewardMoney = EC_Buf::TBufEC_GetInt32(Buffer);
            Quest->Planet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            Quest->ObjectiveTarget = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            Quest->Successful = EC_Buf::TBufEC_GetBoolean(Buffer);
            Quest->Description = Buffer->ReadWideString();
            Quest->CompletionText = Buffer->ReadWideString();
            Quest->SpecialCompletionText = Buffer->ReadWideString();
            if (GlobalsV::LoadedSaveVersion <= 141) {
                EC_Buf::TBufEC_GetUInt32(Buffer);
            }
        }
        PendingCareerActivity[0] = EC_Buf::TBufEC_GetByte(Buffer);
        PendingCareerActivity[1] = EC_Buf::TBufEC_GetByte(Buffer);
        PendingCareerActivity[2] = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 60) {
            PrisonTermRemaining = EC_Buf::TBufEC_GetUInt32(Buffer);
        } else {
            PrisonTermRemaining = EC_Buf::TBufEC_GetByte(Buffer);
        }
        LastDockedNonPlanetLocation = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        BaseNodes = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion < 49) {
            ProgramCounts[aGalaxyStruct::prgKellerCall] = 0;
            for (ProgramIndex = static_cast<std::uint8_t>(aGalaxyStruct::prgLogicalNegation); ProgramIndex <= static_cast<std::uint8_t>(11); ++ProgramIndex) {
                ProgramCounts[ProgramIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
            }
        } else {
            for (ProgramIndex = static_cast<std::uint8_t>(0); ProgramIndex <= static_cast<std::uint8_t>(11); ++ProgramIndex) {
                ProgramCounts[ProgramIndex] = EC_Buf::TBufEC_GetInt32(Buffer);
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 110) {
            ExcludedFromRating = EC_Buf::TBufEC_GetBoolean(Buffer);
        }
    }

    // Resolves quest targets by quest type and LastDockedNonPlanetLocation.
    void TRanger_ResolveLoadedReferences(TRanger* Self, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        PQuest Quest{};
        aNormalShip::TNormalShip_ResolveLoadedReferences(Self, Galaxy);
        std::int32_t Count = pas::list_count(Self->Quests);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Quest = pas::list_at<TQuest>(Self->Quests, I);
            Quest->Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Quest->Planet)), true)));
            switch (Quest->QuestType) {
                case aGalaxyStruct::qtSendLetter: {
                    Quest->ObjectiveTarget = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Quest->ObjectiveTarget)), true)));
                    break;
                }
                case aGalaxyStruct::qtKillShip: {
                    Quest->ObjectiveTarget = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Quest->ObjectiveTarget)), true)));
                    break;
                }
                case aGalaxyStruct::qtPlanetQuest: {
                    Quest->ObjectiveTarget = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Quest->ObjectiveTarget)), true)));
                    break;
                }
                case aGalaxyStruct::qtDefendSystem: {
                    Quest->ObjectiveTarget = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Quest->ObjectiveTarget)))));
                    break;
                }
                case aGalaxyStruct::qtDefendShip: {
                    Quest->ObjectiveTarget = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Quest->ObjectiveTarget)), true)));
                    break;
                }
            }
        }
        Self->LastDockedNonPlanetLocation = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->LastDockedNonPlanetLocation)), false)));
    }

    void TRanger_NextDay(TRanger* Self) {
        aNormalShip::TNormalShip_NextDay(Self);
        try {
            if (Self->IsFemaleHumanPilot()) {
                Self->PreferredCareer = aGalaxyStruct::rcTrader;
            }
            Self->ProcessCareerActivityAndEminentProgress();
            Self->ProcessQuestTimersAndOutcomes();
            if (aPlayer::GetPlayer() == Self) {
                PlayerEquipmentBrokenThisTurn = false;
                if (!aRanger::TRanger_ProcessPendingPlayerFollowTargeting(Self)) {
                    return;
                }
            }
            if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
                Self->ScriptNextDay();
                if (Self->ScriptShip != nullptr) {
                    if (static_cast<std::uint32_t>(Self->PrisonTermRemaining) > 0) {
                        Self->ProcessPrisonAndHostileCheck();
                    }
                    return;
                }
            }
            Self->virtual_TShip_NextDayLogic();
            if (Self->ScriptShip != nullptr && static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1)) {
                Self->ScriptNextDay();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TRanger.NextDay ", Self->GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
    }

    void TRanger_NextDayLogic(TRanger* Self) {
        aPlanet::TPlanet* Location{};
        std::int32_t Stage = 0;
        try {
            if (Self->CurrentPlanet != nullptr) {
                Stage = 1;
                if (pas::in_set<0, 4, 7, 7>(Self->CurrentPlanet->OwnerId)) {
                    Stage = 2;
                    Self->TryTurnInQuests();
                    if (Self->ProcessPrisonAndHostileCheck()) {
                        return;
                    }
                    Self->RefuelAtLocation();
                    Self->RepairBrokenEquipmentAtLocation();
                    Self->AutoEquipInventory();
                    Self->OptimizeInventory();
                    Self->BuildReachablePlanetQueue();
                    Self->SellCargoGoods();
                    Self->RefuelAtLocation();
                    Self->ReloadWeaponAmmo();
                    if (!Self->RepairHullAtLocation()) {
                        aShip::TShip_BuyEquipmentAtLocation(Self, false);
                        Self->RestoreEssentialEquipment();
                        if (static_cast<std::uint8_t>(Self->ScanForCollectableItems() ^ 1) && Self->NeedsWealthCatchup() && Self->CountWingmen() == 0 && Self->PartnerShip == nullptr) {
                            Self->BuyProfitableGoods();
                        }
                        if (aPlayer::GetPlayer() != Self) {
                            Self->SimulateUnseenProgression();
                        }
                        Self->TrainSkillsAutomatically();
                        Self->OrderTakeoff();
                    }
                } else {
                    Self->OrderTakeoff();
                }
                return;
            }
            Stage = 3;
            if (Self->DockedTo != nullptr) {
                Stage = 4;
                if (!pas::in_range(Self->DockedTo->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                    if (Self->DockedTo->InNormalSpace()) {
                        Self->OrderTakeoff();
                    } else {
                        Self->OrderNone(false);
                    }
                    return;
                }
                Self->SynchronizeDockedLocation();
                switch (Self->DockedTo->TypeId) {
                    case aGalaxyStruct::rstRangerCenter: {
                        Self->DepositCarriedNodes();
                        Self->TrainSkillsAutomatically();
                        break;
                    }
                    case aGalaxyStruct::rstMilitaryBase: {
                        if (Self->Rank < 6 || aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Rank >= 7) {
                            Self->TryPromoteRank();
                        }
                        break;
                    }
                }
                if (aPlayer::GetPlayer() != Self) {
                    Self->SimulateUnseenProgression();
                }
                Self->RepairBrokenEquipmentAtLocation();
                Self->AutoEquipInventory();
                Self->OptimizeInventory();
                Self->BuildReachablePlanetQueue();
                Self->SellCargoGoods();
                Self->RefuelAtLocation();
                Self->ReloadWeaponAmmo();
                if (aGalaxy::TGalaxy::FindMilitaryBaseInTransit() == Self->DockedTo) {
                    return;
                }
                if (!Self->RepairHullAtLocation()) {
                    aShip::TShip_BuyEquipmentAtLocation(Self, false);
                    Self->RestoreEssentialEquipment();
                    Self->RepairBrokenEquipmentAtLocation();
                    if (Self->DockedTo->InNormalSpace()) {
                        Self->OrderTakeoff();
                    } else {
                        Self->OrderNone(false);
                    }
                }
                return;
            }
            Stage = 5;
            if (!Self->InNormalSpace()) {
                return;
            }
            Stage = 7;
            if (aPlayer::GetPlayer() != Self && (static_cast<std::int32_t>(Self->Seed) + aGalaxy::Galaxy->CurrentTurn) % 53 == 0) {
                Self->SimulateUnseenProgression();
            }
            Self->AutoApplyMicroModules();
            Self->BuildReachablePlanetQueue();
            if (Self->virtual_TShip_RecomputeFearState()) {
                aRanger::TRanger_TryOfferRansomToPursuer(Self);
            }
            Self->AssignWeaponTargetsInStar();
            Self->CheckForPartnershipBreakup();
            Self->TryRecruitWingman();
            if (Self->PartnerShip != nullptr) {
                if (!Self->InFear) {
                    if ((Self->OrderTarget == Self->PartnerShip || Self->OrderTarget == Self->PartnerShip->OrderTarget) && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1) && (Self->OrderTarget == Self->PartnerShip || static_cast<std::uint8_t>(pas::is_one_of<aShip::soNone, aShip::soMove>(Self->Order) ^ 1))) {
                        Self->TryCollectBestFloatingItem(0);
                        if (Self->TryMirrorPartnerTravelOrders()) {
                            return;
                        }
                    }
                } else if (pas::is_one_of<aShip::soLand, aShip::soJump>(Self->Order) && (Self->OrderTarget == Self->PartnerShip->OrderTarget || Self->EstimateOrderTravelTurns() < 4)) {
                    return;
                }
            }
            if (Self->EnemyShip != nullptr && Self->OrderTarget == Self->EnemyShip && Self->EnemyShip->CurrentPlanet != nullptr && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L) {
                Self->OrderNone(false);
            }
            Self->ProcessCombatDialogue();
            Self->AfterburnerActive = false;
            if (Self->InFear) {
                Stage = 8;
                if (Self->PartnerShip != nullptr) {
                    if (Self->PartnerShip->CurrentStar == Self->CurrentStar) {
                        aRanger::TRanger_SelectAlternateReachableDestination(Self);
                    } else if (Self->PartnerShip->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition && Self->PartnerShip->CurrentStar->Status.CustomFaction == u"") {
                        if (Self->PartnerShip->Order == aShip::soJump && pas::class_cast_if<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget) != nullptr && Self->PartnerShip->OrderTarget != Self->CurrentStar && Self->PartnerShip->OrderTarget != Self->PartnerShip->CurrentStar && static_cast<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfCoalition && static_cast<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget)->Status.CustomFaction == u"") {
                            Self->OrderJump(pas::checked_cast<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget), true);
                            return;
                        }
                        Self->OrderJump(Self->PartnerShip->CurrentStar, true);
                        return;
                    } else {
                        aRanger::TRanger_SelectAlternateReachableDestination(Self);
                    }
                } else {
                    aRanger::TRanger_SelectAlternateReachableDestination(Self);
                    if (Self->GetCarriedNodeCount() > 0) {
                        Self->TryOrderTravelToShipTypeLocation(6);
                    }
                    if (aKling::BlazerShip != nullptr && aKling::BlazerShip->CurrentStar == Self->CurrentStar && aKling::BlazerShip->InNormalSpace() && Self->Aggression < 40) {
                        Self->NavigateToEscapePlanet(true);
                    }
                    if (Self->Order != aShip::soLand && Self->Order != aShip::soJump) {
                        Self->EngageEnemyShip();
                    } else if (Self->EstimateOrderTravelTurns() > 4 && Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self && pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Self->EnemyShip->TypeId) && Self->EnemyShip->EstimateOrderTravelTurns() < 3 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L && (static_cast<std::int32_t>(Self->Seed) + aGalaxy::Galaxy->CurrentTurn) % 2 == 0) {
                        if (Self->JettisonCargoGoodsTowardTargetValue(std::max<std::int32_t>(200, aGalaxy::Galaxy->ComputeScaledMiniMoney(Self->OwnerId) / 2))) {
                            Self->NotifyFearCargoDrop(Self->EnemyShip);
                        }
                    }
                }
                if (Self->Order == aShip::soLand || Self->Order == aShip::soJump) {
                    Self->UpdateAfterburnerState();
                }
            } else {
                Stage = 9;
                if (Self->PartnerShip != nullptr) {
                    if (Self->PartnerShip->CurrentStar == Self->CurrentStar && pas::is_one_of<aShip::soNone, aShip::soMove>(Self->PartnerShip->Order) && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
                        Self->TryCollectBestFloatingItem(2);
                    } else {
                        Self->TryCollectBestFloatingItem(0);
                    }
                    if (Self->PartnerShip->CurrentStar == Self->CurrentStar && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
                        if (Self->PartnerShip->EnemyShip != nullptr && (Self->PartnerShip->OrderTarget == Self->PartnerShip->EnemyShip || Self->PartnerShip->EnemyShip->OrderTarget == Self->PartnerShip) && (static_cast<std::uint8_t>(Self->IsFemaleHumanPilot() ^ 1) || static_cast<std::uint8_t>(Self->PartnerShip->EnemyShip->IsFemaleHumanPilot() ^ 1))) {
                            Self->EnemyShip = Self->PartnerShip->EnemyShip;
                            Self->EngageEnemyShip();
                        } else if (Self->EnemyShip != nullptr && Self->EnemyShip->InNormalSpace() && pas::class_cast_if<aShip::TShip*>(Self->PartnerShip->OrderTarget) != nullptr && aShip::TShip_GetRelationLevelToShip(Self->PartnerShip, pas::checked_cast<aShip::TShip*>(Self->PartnerShip->OrderTarget)) == aGalaxyStruct::rlHostile && aShip::TShip_GetRelationLevelToShip(Self->PartnerShip, Self->EnemyShip) == aGalaxyStruct::rlHostile) {
                            Self->EngageEnemyShip();
                        }
                    }
                    if (Self->Order == aShip::soFollowShip && Self->OrderTarget != Self->PartnerShip && aShip::TShip_GetRelationLevelToShip(Self, pas::checked_cast<aShip::TShip*>(Self->OrderTarget)) != aGalaxyStruct::rlHostile) {
                        Self->OrderNone(false);
                    }
                    if (Self->Order == aShip::soNone && Self->HasCargoGoods() && Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace) {
                        aRanger::TRanger_SelectNearestReachableDestination(Self);
                    }
                    if (Self->Order == aShip::soNone) {
                        if (([&] {
                            std::int32_t cpp_left = Self->GetHull()->Weight - Self->GetDesiredCargoFreeSpace();
                            return cpp_left < Self->GetCarriedItemWeight();
                        }()) || Self->GetHullIntegrityPercent() < 70 && Self->HasHullDamageOrBrokenEquippedItems()) {
                            aRanger::TRanger_SelectNearestReachableDestination(Self);
                        }
                    }
                    if (Self->Order == aShip::soNone && Self->HasInactiveDirectEquipment != 0 && Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace) {
                        aRanger::TRanger_SelectNearestReachableDestination(Self);
                    }
                    if (Self->Order == aShip::soNone || Self->OrderTarget == Self->PartnerShip || Self->Order == aShip::soJump && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
                        Self->TryMirrorPartnerTravelOrders();
                    }
                    if (Self->Order == aShip::soNone && Self->CanRefuel()) {
                        aRanger::TRanger_SelectNearestReachableDestination(Self);
                    }
                    if (Self->Order == aShip::soNone && Self->GetCarriedNodeCount() > 0) {
                        Self->TryOrderTravelToShipTypeLocation(6);
                    }
                    if (Self->Order == aShip::soNone && Self->CanPromoteRank()) {
                        Self->TryOrderTravelToShipTypeLocation(8);
                    }
                } else {
                    if (static_cast<std::uint8_t>(Self->TryCollectBestFloatingItem(50) ^ 1) && static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
                        Self->SelectEnemyShipInStar();
                        Self->EngageEnemyShip();
                    }
                    if (Self->Order == aShip::soNone && Self->GetCarriedNodeCount() > 0) {
                        Self->TryOrderTravelToShipTypeLocation(6);
                    }
                    if (Self->Order == aShip::soNone && Self->CanPromoteRank()) {
                        Self->TryOrderTravelToShipTypeLocation(8);
                    }
                    if (Self->Order == aShip::soNone && Self->HasCargoGoods() && Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace) {
                        if (Self->NeedsWealthCatchup()) {
                            Self->OrderBestQueuedTradePlanet();
                        } else {
                            aRanger::TRanger_SelectNearestReachableDestination(Self);
                        }
                    }
                    if (Self->Order == aShip::soNone) {
                        if (([&] {
                            std::int32_t cpp_left_2 = Self->GetHull()->Weight - Self->GetDesiredCargoFreeSpace();
                            return cpp_left_2 < Self->GetCarriedItemWeight();
                        }()) || Self->HasHullDamageOrBrokenEquippedItems() || Self->CanRefuel()) {
                            aRanger::TRanger_SelectNearestReachableDestination(Self);
                        }
                    }
                    if (Self->Order == aShip::soNone && Self->HasInactiveDirectEquipment != 0 && Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace) {
                        aRanger::TRanger_SelectNearestReachableDestination(Self);
                    }
                }
                if (Self->Order == aShip::soNone && static_cast<std::uint8_t>(Self->InFear ^ 1)) {
                    aRanger::TRanger_SelectIdleFreeFlightDestination(Self, 0);
                }
                if (Self->Order == aShip::soNone) {
                    Location = Self->SelectBestTradePlanetFromQueue();
                    if (Location == nullptr && Self->GetHull()->HullPoints < Self->GetHull()->Weight) {
                        Location = Self->SelectRandomPlanetFromQueue();
                    }
                    if (Location != nullptr) {
                        if (Self->CurrentStar == Location->CurrentStar) {
                            Self->OrderLanding(Location, false);
                        } else {
                            Self->OrderJump(Location->CurrentStar, false);
                        }
                    }
                }
            }
            if (Self->Order == aShip::soNone) {
                Self->NavigateToEscapePlanet(false);
            }
            if (Self->Order == aShip::soNone) {
                Self->OrderRandomFreeFlightMove();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TRanger.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    // True only for the pending auto-equip branch; normal follow/weapon assignment returns false.
    std::uint8_t TRanger_ProcessPendingPlayerFollowTargeting(TRanger* Self) {
        std::uint8_t Result{};
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        if (PlayerAutomaticControl) {
            Result = true;
            Self->AutoEquipInventory();
            return Result;
        }
        if (Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar != Self->CurrentStar) {
            Self->EnemyShip = nullptr;
        }
        if (PendingPlayerFollowTarget != nullptr) {
            if (PendingPlayerFollowTarget->InNormalSpace() && Self->InNormalSpace() && PendingPlayerFollowTarget->CurrentStar == Self->CurrentStar) {
                Self->OrderFollowShip(PendingPlayerFollowTarget, 1, false);
                {
                    const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
                    if (1 <= cpp_last) {
                        for (I = 1; I <= cpp_last; ++I) {
                            Weapon = Self->Weapons[I];
                            if (aShip::TShip_IsEquipmentUsable(Self, Weapon) && (static_cast<std::uint8_t>(pas::in_range(static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType), static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo > 0) && ([&] {
                                std::int32_t cpp_left_2 = aShip::TShip_GetWeaponRange(Self, Weapon);
                                pas::Extended cpp_left = cpp_left_2 * aShip::TShip_GetWeaponRange(Self, Weapon);
                                return cpp_left >= aMyFunction::PointDistanceSquared(Self->Position, PendingPlayerFollowTarget->Position);
                            }())) {
                                Weapon->Target = PendingPlayerFollowTarget;
                            } else {
                                Weapon->Target = nullptr;
                            }
                        }
                    }
                }
            } else {
                PendingPlayerFollowTarget = nullptr;
            }
        }
        return false;
    }

    // Requires HomePlanet.
    aGalaxy::TStar* TRanger::GetHomeStar() {
        return HomePlanet->CurrentStar;
    }

    pas::WideString TRanger::GetName() {
        return Name;
    }

    pas::WideString TRanger::GetFullName(const pas::WideString& Separator) {
        pas::WideString Path{};
        pas::WideString Text{};
        if (TypeNameOverrideKey == u"") {
            return pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"ShipType.", aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", GetTypeNameKey()})), Separator, Name});
        }
        Path = pas::concat_wide({u"ShipType.", aConst::OwnerToSys(aConst::RaceToOwner(PilotRace)), u".", TypeNameOverrideKey});
        if (GR_Main::LanguageDataConfig->CountParamsByPath(Path) > 0) {
            Text = aConst::LocalizedText(Path);
        } else {
            Text = aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey}));
        }
        if (Text != u"") {
            return pas::concat_wide({Text, Separator, Name});
        }
        return Name;
    }

    // Returns the ranger category used by ship-greeting filters.
    std::uint8_t TRanger::GetGreetingShipCategory() {
        return aGalaxyStruct::gscRanger;
    }

    // Ties favor trader, then pirate.
    aGalaxyStruct::TRangerCareer TRanger::GetDominantCareer() {
        std::int32_t Maximum = std::max<std::int32_t>(std::max<std::int32_t>(static_cast<std::int32_t>(CareerStatus[aGalaxyStruct::rcTrader]), static_cast<std::int32_t>(CareerStatus[aGalaxyStruct::rcPirate])), static_cast<std::int32_t>(CareerStatus[aGalaxyStruct::rcWarrior]));
        if (CareerStatus[aGalaxyStruct::rcTrader] == Maximum) {
            return aGalaxyStruct::rcTrader;
        } else if (CareerStatus[aGalaxyStruct::rcPirate] == Maximum) {
            return aGalaxyStruct::rcPirate;
        } else {
            return aGalaxyStruct::rcWarrior;
        }
    }

    // Values occupies the low three bytes of one stack slot; result is the average of 100 minus each career-distance.
    std::uint8_t TRanger::GetCareerSimilarity(TRangerCareerValues Values) {
        std::int32_t TraderSimilarity = 100 - pas::abs(CareerStatus[aGalaxyStruct::rcTrader] - Values[aGalaxyStruct::rcTrader]);
        std::int32_t PirateSimilarity = 100 - pas::abs(CareerStatus[aGalaxyStruct::rcPirate] - Values[aGalaxyStruct::rcPirate]);
        std::int32_t WarriorSimilarity = 100 - pas::abs(CareerStatus[aGalaxyStruct::rcWarrior] - Values[aGalaxyStruct::rcWarrior]);
        return (TraderSimilarity + PirateSimilarity + WarriorSimilarity) / 3;
    }

    // Selects the closest configured ShipCharacter profile; equal similarities retain the earlier profile.
    pas::WideString TRanger::GetCharacterName() {
        pas::WideString Result{};
        std::int32_t I{};
        TRangerCareerValues Values{};
        pas::WideString Text{};
        pas::WideString Path{};
        std::int32_t Best = 0;
        Result = u"Error in CharacterName"_w;
        Path = u"ShipCharacter"_w;
        if (TypeNameOverrideKey != u"" && GR_Main::LanguageDataConfig->CountBlocks(pas::concat_wide({Path, TypeNameOverrideKey})) > 0) {
            Path = pas::concat_wide({Path, TypeNameOverrideKey});
        } else if (IsFemaleHumanPilot()) {
            Path = pas::concat_wide({Path, u"Female"});
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, GR_Main::LanguageDataConfig->GetBlock(Path)->GetParamCount() - 1); cpp_range.next(I); ) {
            Text = GR_Main::LanguageDataConfig->GetBlock(Path)->GetParamValue(I);
            Values[0] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()))));
            Values[1] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()))));
            Values[2] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()))));
            if ((GetCareerSimilarity(Values) & 0x0000007f) > Best) {
                Best = GetCareerSimilarity(Values) & 0x0000007f;
                Result = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, 3, u","_wref.get()));
            }
        }
        return Result;
    }

    // Rounded pirate career status times StrengthInBestRanger, clamped to 0..100.
    std::uint8_t TRanger::GetStrengthScaledPirateStatus() {
        return System::Round(aMyFunction::RemapClamped(static_cast<long double>(CareerStatus[aGalaxyStruct::rcPirate]) * StrengthInBestRanger, 0.0, 1.0E+2, 0.0, 1.0E+2));
    }

    std::int32_t TRanger::GetDesiredCargoFreeSpace() {
        switch (PreferredCareer) {
            case aGalaxyStruct::rcTrader: return System::Trunc(GetHull()->Weight * 0.15L) + 30;
            case aGalaxyStruct::rcPirate: return System::Trunc(GetHull()->Weight * 0.1L) + 40;
            case aGalaxyStruct::rcWarrior: return System::Trunc(GetHull()->Weight * 0.1L) + 30;
            default: return 50;
        }
    }

    // Dispatches by object class and radar distance; unsupported objects yield unknown object.
    pas::WideString TRanger::GetObjectInfoText(pas::Object* Instance) {
        if (pas::class_cast_if<aGalaxy::TStar*>(Instance) != nullptr) {
            return pas::checked_cast<aGalaxy::TStar*>(Instance)->Name;
        } else if (pas::class_cast_if<aPlanet::TPlanet*>(Instance) != nullptr) {
            return pas::checked_cast<aPlanet::TPlanet*>(Instance)->GetInfoText(false);
        } else if (pas::class_cast_if<aItem::TItem*>(Instance) != nullptr) {
            {
                pas::Extended cpp_right = aMyFunction::PointDistance(Position, pas::checked_cast<aItem::TItem*>(Instance)->Position);
                if (GetRadarRange() < cpp_right) {
                    return static_cast<void>(pas::checked_cast<aItem::TItem*>(Instance)), aItem::TItem::GetSmallInfoText();
                }
            }
            return pas::checked_cast<aItem::TItem*>(Instance)->GetDisplayName();
        } else if (pas::class_cast_if<aShip::TShip*>(Instance) != nullptr) {
            {
                pas::Extended cpp_right_2 = aMyFunction::PointDistance(Position, pas::checked_cast<aShip::TShip*>(Instance)->Position);
                if (GetRadarRange() < cpp_right_2) {
                    return pas::checked_cast<aShip::TShip*>(Instance)->GetName();
                }
            }
            return aShip::TShip_GetSpaceInfoText(pas::checked_cast<aShip::TShip*>(Instance));
        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Instance) != nullptr) {
            {
                pas::Extended cpp_right_3 = aMyFunction::PointDistance(Position, pas::checked_cast<aAsteroid::TAsteroid*>(Instance)->Position);
                if (GetRadarRange() < cpp_right_3) {
                    return pas::checked_cast<aAsteroid::TAsteroid*>(Instance)->GetDisplayName();
                }
            }
            return pas::checked_cast<aAsteroid::TAsteroid*>(Instance)->GetInfoText();
        } else {
            return u"unknown object"_w;
        }
    }

    // Counts galaxy star-list ships whose PartnerShip is Self; includes docked ships.
    std::int32_t TRanger::CountWingmen() {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (Ship->PartnerShip == this) {
                    ++Result;
                }
            }
        }
        return Result;
    }

    std::uint8_t TRanger::NeedsStrengthCatchup() {
        return pas::real_divide(Strength, aGalaxy::Galaxy->AverageRangerStrength) < aConst::CareerTuning[PreferredCareer].MinimumStrengthToAverageRatio || StrengthInBestRanger < aConst::CareerTuning[PreferredCareer].MinimumStrengthToBestRatio;
    }

    // Either the absolute or relative career threshold can trigger catch-up.
    std::uint8_t TRanger::NeedsWealthCatchup() {
        return pas::real_divide(Wealth, aGalaxy::Galaxy->AverageRangerCapital) < aConst::CareerTuning[PreferredCareer].MinimumWealthToAverageRatio || WealthInBestRanger < aConst::CareerTuning[PreferredCareer].MinimumWealthToBestRatio;
    }

    // Only buys a full refill when its positive cost is affordable.
    void TRanger::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr && GetFullRefuelCost() > 0 && GetFullRefuelCost() <= Money) {
            SetMoney(Money - GetFullRefuelCost());
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    // Can grant money, experience, equipment, awards and simulated kills; requires the unseen-day threshold, a player and unresolved Dominators.
    void TRanger::SimulateUnseenProgression() {
        std::uint8_t Award{};
        if (DaysSincePlayerSeen < 5.0E+1L * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestTimeAndExperienceFactor) {
            return;
        }
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (!aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}}))) {
            return;
        }
        if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
            TryPromoteRank();
        }
        if (Money < 50000 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.6L && ([&] {
            pas::Extended cpp_left = aMyFunction::NextRandomFloatRange(0.0, 1.0, RandomState);
            return cpp_left > WealthInBestRanger;
        }())) {
            std::int32_t cpp_case = static_cast<std::int32_t>(System::Round(aPlayer::GetPlayer()->WealthInBestRanger * 1.0E+2L));
            if (cpp_case >= 0 && cpp_case <= 20) {
                SetMoney(Money + aGalaxy::Galaxy->ComputeScaledAverageMoney(2));
            } else if (cpp_case >= 21 && cpp_case <= 40) {
                SetMoney(Money + aGalaxy::Galaxy->ComputeScaledAverageMoney(2));
            } else if (cpp_case >= 41 && cpp_case <= 60) {
                SetMoney(Money + aGalaxy::Galaxy->ComputeScaledAverageMoney(2));
            } else if (cpp_case >= 61 && cpp_case <= 90) {
                SetMoney(Money + aGalaxy::Galaxy->ComputeScaledBigMoney(2));
            } else if (cpp_case >= 91 && cpp_case <= 100) {
                SetMoney(Money + aGalaxy::Galaxy->ComputeScaledHugeMoney(2));
            }
        }
        if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L && (NeedsStrengthCatchup() || aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L) || aMyFunction::NextRandomUnitFloat(RandomState) < 0.01L) {
            if (([&] {
                pas::Extended cpp_left_2 = aMyFunction::NextRandomFloatRange(0.0, 0.7, RandomState);
                return cpp_left_2 > WealthInBestRanger;
            }()) && Money < 25000) {
                SetMoney(Money + aGalaxy::Galaxy->ComputeScaledBigMoney(2));
            } else if (NeedsStrengthCatchup() && aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L || aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
                ImproveRandomEquipment(true);
            }
        }
        if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.02L && (NeedsStrengthCatchup() || aMyFunction::NextRandomUnitFloat(RandomState) < 0.001L)) {
            aShip::TShip_GenerateExtraWeapon(this);
        }
        if (CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] > 0 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L || CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] > 0 && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->PlaceInRating < pas::list_count(aGalaxy::Galaxy->Rangers) / 3 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L || aGalaxy::Galaxy->CurrentTurn < 300 && (aMyFunction::NextRandomUnitFloat(RandomState) < 0.4L || static_cast<std::int32_t>(Seed) % 5 == 0 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.8L)) {
            ++TotalShipKillCount;
            ++DominatorKillCount;
            ++CurrentSystemKills.Dominator;
            {
                std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(aConst::DominatorShipDefinitions[aGalaxyStruct::ktShtip].RankPoints, aConst::DominatorShipDefinitions[aGalaxyStruct::ktEquentor].RankPoints, RandomState);
                aNormalShip::TNormalShip* self = this;
                self->AddRankPoints(nextRandomIntRange);
            }
            {
                std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                aShip::TShip* self_2 = this;
                self_2->GainExperience(nextRandomIntRange_2, 0);
            }
            AddWarriorCareerActivity(4);
            if (aGalaxy::Galaxy->CurrentTurn < 300 && (DominatorKillCount % 13 == 0 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L || DominatorKillCount % 20 == 0)) {
                ++LiberatedSystemCount;
                AddRankPoints(30);
                {
                    std::int32_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(500, 1000, aGalaxy::Galaxy->RandomState);
                    aShip::TShip* self_3 = this;
                    self_3->GainExperience(nextRandomIntRange_3, 0);
                }
                {
                    std::uint8_t pickRandomEquipmentOwner = aConst::PickRandomEquipmentOwner(RandomState);
                    aNormalShip::TNormalShip* self_4 = this;
                    std::uint8_t selectAward = self_4->SelectAward(pickRandomEquipmentOwner, pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atLiberation}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                    aShip::TShip* self_5 = this;
                    self_5->AddAward(selectAward);
                }
            }
        } else if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L && (GetDominantCareer() != aGalaxyStruct::rcPirate || aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L)) {
            ++TotalShipKillCount;
            ++PirateKillCount;
            AddRankPoints(10);
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L) {
                ++TotalShipKillCount;
                ++PirateKillCount;
                AddRankPoints(10);
            }
            AddWarriorCareerActivity(2);
        } else if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L && (GetDominantCareer() == aGalaxyStruct::rcPirate || aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L)) {
            ++TotalShipKillCount;
            if (PreferredCareer == aGalaxyStruct::rcPirate) {
                AddPirateCareerActivity(2);
            } else {
                AddPirateCareerActivity(1);
            }
        }
        if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.09L || aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->PlaceInRating < pas::list_count(aGalaxy::Galaxy->Rangers) / 3 && (aPlayer::GetPlayer()->PlaceInRating < PlaceInRating || aMyFunction::NextRandomUnitFloat(RandomState) < 0.4L) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L || aGalaxy::Galaxy->CurrentTurn < 300 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L) {
            {
                std::int32_t cpp_case_2 = static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(aPlayer::GetPlayer()->PlaceInRating, 1.0, pas::list_count(aGalaxy::Galaxy->Rangers), 0.0, 1.0E+2)));
                if (cpp_case_2 >= 0 && cpp_case_2 <= 20) {
                    std::int32_t nextRandomIntRange_8 = aMyFunction::NextRandomIntRange(100, 1000, RandomState);
                    aShip::TShip* self_10 = this;
                    self_10->GainExperience(nextRandomIntRange_8, 0);
                } else if (cpp_case_2 >= 21 && cpp_case_2 <= 40) {
                    std::int32_t nextRandomIntRange_7 = aMyFunction::NextRandomIntRange(100, 500, RandomState);
                    aShip::TShip* self_9 = this;
                    self_9->GainExperience(nextRandomIntRange_7, 0);
                } else if (cpp_case_2 >= 41 && cpp_case_2 <= 60) {
                    std::int32_t nextRandomIntRange_6 = aMyFunction::NextRandomIntRange(100, 500, RandomState);
                    aShip::TShip* self_8 = this;
                    self_8->GainExperience(nextRandomIntRange_6, 0);
                } else if (cpp_case_2 >= 61 && cpp_case_2 <= 80) {
                    std::int32_t nextRandomIntRange_5 = aMyFunction::NextRandomIntRange(100, 500, RandomState);
                    aShip::TShip* self_7 = this;
                    self_7->GainExperience(nextRandomIntRange_5, 0);
                } else if (cpp_case_2 >= 81 && cpp_case_2 <= 100) {
                    std::int32_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(100, 500, RandomState);
                    aShip::TShip* self_6 = this;
                    self_6->GainExperience(nextRandomIntRange_4, 0);
                }
            }
            if (CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] > 0) {
                std::int32_t nextRandomIntRange_9 = aMyFunction::NextRandomIntRange(100, 500, RandomState);
                aShip::TShip* self_11 = this;
                self_11->GainExperience(nextRandomIntRange_9, 0);
            }
        }
        if (Rank < 5 && aPlayer::GetPlayer()->Rank > static_cast<std::uint8_t>(Rank + 1) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
            {
                std::uint16_t nextRandomIntRange_10 = aMyFunction::NextRandomIntRange(2, 20, RandomState);
                aNormalShip::TNormalShip* self_12 = this;
                self_12->AddRankPoints(nextRandomIntRange_10);
            }
            ++TotalShipKillCount;
            ++PirateKillCount;
            ++TotalShipKillCount;
            ++PirateKillCount;
        }
        if (CurrentPlanet != nullptr) {
            if (aPlayer::GetPlayer()->AwardIds != nullptr && (AwardIds == nullptr || pas::list_count(AwardIds) < std::min<std::int32_t>(5, pas::list_count(aPlayer::GetPlayer()->AwardIds))) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L || aPlayer::GetPlayer()->AwardIds != nullptr && (AwardIds == nullptr || pas::list_count(aPlayer::GetPlayer()->AwardIds) + 5 > pas::list_count(AwardIds)) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.004L || aPlayer::GetPlayer()->AwardIds == nullptr && (AwardIds == nullptr || pas::list_count(AwardIds) < 4) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.006L || (AwardIds == nullptr || DominatorKillCount / 10 > pas::list_count(AwardIds)) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.004L) {
                switch (GetDominantCareer()) {
                    case aGalaxyStruct::rcTrader: {
                        Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}, {aGalaxyStruct::atCowardice}, {aGalaxyStruct::atPlanetBattle}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                        break;
                    }
                    case aGalaxyStruct::rcPirate: {
                        Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}, {aGalaxyStruct::atCowardice}, {aGalaxyStruct::atPerfidy}, {aGalaxyStruct::atPlanetBattle}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                        break;
                    }
                    case aGalaxyStruct::rcWarrior: {
                        Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}, {aGalaxyStruct::atPlanetBattle}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                        break;
                    }
                    default: Award = 255; break;
                }
                if (Award != aGalaxyStruct::AwardNotFound) {
                    AddAward(Award);
                }
            }
            if (aGalaxy::Galaxy->TechLevel > 3 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L && (aGalaxy::Galaxy->TechLevel > 5 || aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) && (aPlayer::GetPlayer()->StrengthInBestRanger > 0.9L && StrengthInBestRanger < 0.7L || StrengthInBestRanger < 0.3L)) {
                aShip::TShip_GenerateExtraWeapon(this);
            }
        }
    }

    // Saturates at 100.
    void TRanger::AddTraderCareerActivity(std::uint8_t Amount) {
        if (static_cast<std::uint32_t>(PendingCareerActivity[0]) + Amount < 100) {
            PendingCareerActivity[0] += Amount;
        } else {
            PendingCareerActivity[0] = 100;
        }
    }

    // Saturates at 100.
    void TRanger::AddPirateCareerActivity(std::uint8_t Amount) {
        if (static_cast<std::uint32_t>(PendingCareerActivity[1]) + Amount < 100) {
            PendingCareerActivity[1] += Amount;
        } else {
            PendingCareerActivity[1] = 100;
        }
    }

    // Saturates at 100.
    void TRanger::AddWarriorCareerActivity(std::uint8_t Amount) {
        if (static_cast<std::uint32_t>(PendingCareerActivity[2]) + Amount < 100) {
            PendingCareerActivity[2] += Amount;
        } else {
            PendingCareerActivity[2] = 100;
        }
    }

    void TRanger::ClearPendingCareerActivity() {
        PendingCareerActivity[0] = 0;
        PendingCareerActivity[1] = 0;
        PendingCareerActivity[2] = 0;
    }

    // Clears pending activity even when an NPC already holds a featured title. New titles require a non-excluded ranger in the upper half of the ranking and an undefeated Coalition.
    void TRanger::ProcessCareerActivityAndEminentProgress() {
        std::int32_t Delta{};
        pas::WideString Text{};
        std::int32_t Amount{};
        // Raises Selected up to 100 and proportionally reduces the other axes when needed.
        auto IncreaseRangerCareerAxis = [&](std::uint8_t& Selected, std::uint8_t& OtherA, std::uint8_t& OtherB, std::int32_t Amount) -> void {
            std::uint8_t* Smaller{};
            std::uint8_t* Larger{};
            std::int32_t SmallReduction{};
            std::int32_t LargeReduction{};
            std::int32_t Reduction = Amount;
            if (static_cast<std::uint32_t>(Selected) + OtherA + OtherB < 100) {
                Reduction = std::max<std::int32_t>(0, Selected + OtherA + OtherB + (Reduction - 100));
            }
            if (OtherA + OtherB < Reduction) {
                Reduction = OtherA + OtherB;
            }
            Selected = std::max<std::int32_t>(static_cast<std::int32_t>(Selected), std::min<std::int32_t>(100, Selected + Amount));
            if (Reduction > 0) {
                if (OtherA >= OtherB) {
                    Smaller = &OtherB;
                    Larger = &OtherA;
                } else {
                    Smaller = &OtherA;
                    Larger = &OtherB;
                }
                SmallReduction = System::Round(pas::real_divide(Reduction, OtherA + OtherB) * *Smaller);
                LargeReduction = System::Round(pas::real_divide(Reduction, OtherA + OtherB) * *Larger);
                if (SmallReduction + LargeReduction > Reduction) {
                    --LargeReduction;
                }
                if (SmallReduction + LargeReduction < Reduction) {
                    ++SmallReduction;
                }
                if (*Smaller < SmallReduction) {
                    *Larger = OtherA + OtherB - Reduction;
                    *Smaller = 0;
                } else if (*Larger < LargeReduction) {
                    *Smaller = OtherA + OtherB - Reduction;
                    *Larger = 0;
                } else {
                    pas::dec_unaligned<std::uint8_t>(Smaller, SmallReduction);
                    pas::dec_unaligned<std::uint8_t>(Larger, LargeReduction);
                }
            }
        };
        if (aPlayer::GetPlayer() != this && (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader] == this || aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] == this || aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior] == this)) {
            ClearPendingCareerActivity();
            return;
        }
        if (PendingCareerActivity[0] > 0) {
            Delta = PendingCareerActivity[0];
            if (aPlayer::GetPlayer() == this) {
                Amount = TradeExperience;
                TradeExperience = 0;
            } else {
                Amount = System::Round(aMyFunction::RemapClamped(Delta, 1.0, 1.0E+2, 1.0E+1, 1.0E+2));
            }
            if (IsHealthEffectActive(22)) {
                Amount = System::Round(Amount * 1.5L);
            }
            GainExperience(Amount, 4);
            if (Delta % 2 != 0) {
                ++Delta;
            }
            Delta = std::min<std::int32_t>(8, Delta);
            Delta = Delta / 2;
            IncreaseRangerCareerAxis(CareerStatus[aGalaxyStruct::rcTrader], CareerStatus[aGalaxyStruct::rcPirate], CareerStatus[aGalaxyStruct::rcWarrior], Delta);
            if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader] != this && PlaceInRating < pas::list_count(aGalaxy::Galaxy->Rangers) / 2 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0 && static_cast<std::uint8_t>(ExcludedFromRating ^ 1)) {
                if (EminentProgress[aGalaxyStruct::rcTrader] + Delta * 2 >= 100) {
                    if (CareerStatus[aGalaxyStruct::rcTrader] < 60) {
                        IncreaseRangerCareerAxis(CareerStatus[aGalaxyStruct::rcTrader], CareerStatus[aGalaxyStruct::rcPirate], CareerStatus[aGalaxyStruct::rcWarrior], (60 - CareerStatus[aGalaxyStruct::rcTrader]) / 2 + 1);
                    }
                    {
                        pas::Extended cpp_left = aMyFunction::NextRandomIntRange(100, 250, RandomState);
                        Amount = aMyFunction::RoundAndTruncateToTens(pas::real_divide(cpp_left, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor));
                    }
                    Text = ([&] {
                        auto name = pas::borrow(Name);
                        pas::WideString intToStr = pas::wide_int_to_str(Amount);
                        pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.EminentRangers.EminentTrader"_wref.get(), Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Name>"_w, name.get(), u"<Points>"_w, std::move(intToStr));
                    }());
                    if (aPlayer::GetPlayer() == this) {
                        aGalaxy::Galaxy->AddPlanetNewsWithPlayerBubble(38, Text);
                    } else {
                        aGalaxy::Galaxy->AddPlanetNews(38, Text);
                    }
                    EminentProgress[aGalaxyStruct::rcTrader] = 0;
                    TRanger::HalveAllRangerEminentProgress(aGalaxyStruct::rcTrader);
                    GainExperience(Amount, 0);
                    aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader] = this;
                } else {
                    EminentProgress[aGalaxyStruct::rcTrader] += Delta * 2;
                }
            }
        }
        if (PendingCareerActivity[1] > 0) {
            Delta = PendingCareerActivity[1];
            if (Delta % 2 != 0) {
                ++Delta;
            }
            Delta = std::min<std::int32_t>(8, Delta);
            Delta = Delta / 2;
            IncreaseRangerCareerAxis(CareerStatus[aGalaxyStruct::rcPirate], CareerStatus[aGalaxyStruct::rcTrader], CareerStatus[aGalaxyStruct::rcWarrior], Delta);
            if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] != this && PlaceInRating < pas::list_count(aGalaxy::Galaxy->Rangers) / 2 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0 && static_cast<std::uint8_t>(ExcludedFromRating ^ 1)) {
                if (EminentProgress[aGalaxyStruct::rcPirate] + Delta * 2 >= 100) {
                    if (CareerStatus[aGalaxyStruct::rcPirate] < 60) {
                        IncreaseRangerCareerAxis(CareerStatus[aGalaxyStruct::rcPirate], CareerStatus[aGalaxyStruct::rcTrader], CareerStatus[aGalaxyStruct::rcWarrior], (60 - CareerStatus[aGalaxyStruct::rcPirate]) / 2 + 1);
                    }
                    {
                        pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(250, 1000, RandomState);
                        Amount = aMyFunction::RoundAndTruncateToTens(cpp_left_2 * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor);
                    }
                    Text = ([&] {
                        auto name_2 = pas::borrow(Name);
                        pas::WideString intToStr_2 = pas::wide_int_to_str(Amount);
                        pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.EminentRangers.EminentPirate"_wref.get(), Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_2), u"<color=255,240,100>"_w, u"<Name>"_w, name_2.get(), u"<Points>"_w, std::move(intToStr_2));
                    }());
                    if (aPlayer::GetPlayer() == this) {
                        aGalaxy::Galaxy->AddPlanetNewsWithPlayerBubble(39, Text);
                    } else {
                        aGalaxy::Galaxy->AddPlanetNews(39, Text);
                    }
                    EminentProgress[aGalaxyStruct::rcPirate] = 0;
                    TRanger::HalveAllRangerEminentProgress(aGalaxyStruct::rcPirate);
                    RemoveExperience(Amount);
                    aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] = this;
                } else {
                    EminentProgress[aGalaxyStruct::rcPirate] += Delta * 2;
                }
            }
        }
        if (PendingCareerActivity[2] > 0) {
            Delta = PendingCareerActivity[2];
            if (Delta % 2 != 0) {
                ++Delta;
            }
            Delta = std::min<std::int32_t>(8, Delta);
            Delta = Delta / 2;
            IncreaseRangerCareerAxis(CareerStatus[aGalaxyStruct::rcWarrior], CareerStatus[aGalaxyStruct::rcTrader], CareerStatus[aGalaxyStruct::rcPirate], Delta);
            if (aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior] != this && PlaceInRating < pas::list_count(aGalaxy::Galaxy->Rangers) / 2 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0 && static_cast<std::uint8_t>(ExcludedFromRating ^ 1)) {
                if (EminentProgress[aGalaxyStruct::rcWarrior] + Delta * 2 >= 100) {
                    if (CareerStatus[aGalaxyStruct::rcWarrior] < 60) {
                        IncreaseRangerCareerAxis(CareerStatus[aGalaxyStruct::rcWarrior], CareerStatus[aGalaxyStruct::rcTrader], CareerStatus[aGalaxyStruct::rcPirate], (60 - CareerStatus[aGalaxyStruct::rcWarrior]) / 2 + 1);
                    }
                    {
                        pas::Extended cpp_left_3 = aMyFunction::NextRandomIntRange(250, 1000, RandomState);
                        Amount = aMyFunction::RoundAndTruncateToTens(pas::real_divide(cpp_left_3, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor));
                    }
                    Text = ([&] {
                        auto name_3 = pas::borrow(Name);
                        pas::WideString intToStr_3 = pas::wide_int_to_str(Amount);
                        pas::WideString pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.EminentRangers.EminentWarrior"_wref.get(), Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_3), u"<color=255,240,100>"_w, u"<Name>"_w, name_3.get(), u"<Points>"_w, std::move(intToStr_3));
                    }());
                    if (aPlayer::GetPlayer() == this) {
                        aGalaxy::Galaxy->AddPlanetNewsWithPlayerBubble(37, Text);
                    } else {
                        aGalaxy::Galaxy->AddPlanetNews(37, Text);
                    }
                    EminentProgress[aGalaxyStruct::rcWarrior] = 0;
                    TRanger::HalveAllRangerEminentProgress(aGalaxyStruct::rcWarrior);
                    GainExperience(Amount, 0);
                    aGalaxy::Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior] = this;
                } else {
                    EminentProgress[aGalaxyStruct::rcWarrior] += Delta * 2;
                }
            }
        }
        ClearPendingCareerActivity();
    }

    // Affects every galaxy ranger, including excluded entries.
    void TRanger::HalveAllRangerEminentProgress(aGalaxyStruct::TRangerCareer Career) {
        std::int32_t I{};
        TRanger* Ranger{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<TRanger>(aGalaxy::Galaxy->Rangers, I);
            Ranger->EminentProgress[Career] = System::Round(Ranger->EminentProgress[Career] * 0.5L);
        }
    }

    std::uint8_t TRanger::OrderBestQueuedTradePlanet() {
        aPlanet::TPlanet* Planet = SelectBestTradePlanetFromQueue();
        if (Planet != nullptr) {
            if (CurrentStar == Planet->CurrentStar) {
                OrderLanding(Planet, false);
            } else {
                OrderJump(Planet->CurrentStar, false);
            }
            return true;
        }
        return false;
    }

    aPlanet::TPlanet* TRanger::SelectBestTradePlanetFromQueue() {
        std::uint8_t Good{};
        float Profit{};
        float PurchaseProfit{};
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        float BestScore = 0.0f;
        aPlanet::TPlanet* BestPlanet = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlanetQueue) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(PlanetQueue, I);
            if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId)) {
                Profit = 0.0f;
                PurchaseProfit = 1.0f;
                for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                    if (CargoGoods[Good].Count > 0) {
                        pas::Extended cpp_left = ShopGoodsSellPrice(Good, Planet);
                        if (cpp_left > GetAverageCargoCost(Good)) {
                            pas::Extended cpp_left_3 = ShopGoodsSellPrice(Good, Planet);
                            pas::Extended cpp_left_2 = cpp_left_3 - GetAverageCargoCost(Good);
                            Profit = Profit + cpp_left_2 * CargoGoods[Good].Count;
                        }
                    } else if (([&] {
                        pas::Extended cpp_right = ShopGoodsPurchasePrice(Good, Planet);
                        return aConst::GoodsMarket[Good].AveragePrice * 0.7L > cpp_right;
                    }())) {
                        PurchaseProfit = static_cast<long double>(PurchaseProfit) + ([&] {
                            std::int32_t cpp_right_2 = ShopGoodsPurchasePrice(Good, Planet);
                            std::int32_t cpp_arg = pas::idiv(Money, cpp_right_2);
                            std::int32_t cpp_arg_2 = std::min<std::int32_t>(CargoFreeSpace, Planet->Goods[Good].Count);
                            std::int32_t cpp_left_4 = std::min<std::int32_t>(cpp_arg, cpp_arg_2);
                            return cpp_left_4 * ([&] {
                                std::int32_t cpp_right_3 = ShopGoodsPurchasePrice(Good, Planet);
                                return aConst::GoodsMarket[Good].AveragePrice - cpp_right_3;
                            }());
                        }());
                    }
                }
                Profit = static_cast<long double>(Profit) + pas::real_min<float>(Profit, PurchaseProfit);
                Profit = Profit - static_cast<long double>(aMyFunction::RemapClamped(CurrentStar->Status.ThreatLevel, 0.0, 1.0E+2, 0.0, 0.5)) * Profit;
                Profit = Profit - static_cast<long double>(aMyFunction::RemapClamped(CurrentStar->Status.TrafficLevel, 5.0E+1, 1.0E+2, 0.0, 0.8)) * Profit;
                if (Planet->CurrentStar == CurrentStar) {
                    Profit = 1.6L * Profit;
                }
                if (Profit > BestScore) {
                    BestScore = Profit;
                    BestPlanet = Planet;
                }
            }
        }
        return BestPlanet;
    }

    // Borrowed result, nil for an empty queue.
    aPlanet::TPlanet* TRanger::SelectRandomPlanetFromQueue() {
        if (pas::list_count(PlanetQueue) > 0) {
            std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(PlanetQueue) - 1, RandomState);
            pas::List* planetQueue = PlanetQueue;
            return pas::list_at<aPlanet::TPlanet>(planetQueue, nextRandomIntRange);
        }
        return nullptr;
    }

    // Replaces PlanetQueue; excludes LastDockedPlanet. A scripted system encountered in distance order ends the scan.
    void TRanger::BuildReachablePlanetQueue() {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        ClearPlanetQueue();
        PlanetQueue = pas::make_object<pas::List>();
        if (Speed == 0) {
            return;
        }
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
            if (I > 0 && CurrentStar->StarDistances[I].Distance > JumpRange) {
                break;
            }
            if (Star->Constellation->Id == 20) {
                continue;
            }
            if (Star->Status.CustomFaction != u"") {
                return;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId) && this->virtual_TShip_CanQueueReachablePlanet(Planet) && Planet != LastDockedPlanet) {
                    pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                }
            }
        }
    }

    std::uint8_t TRanger_CanQueueReachablePlanet(TRanger* Self, aPlanet::TPlanet* Planet) {
        return Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && (Self->InFear || Planet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlHostile);
    }

    // The native UnusedMode comparison has no branch effect. Chooses travel toward combat opportunities only with no cargo, a gripper and a full hull.
    void TRanger_SelectIdleFreeFlightDestination(TRanger* Self, std::uint8_t UnusedMode) {
        static const pas::Set<0, 255> CoalitionShipTypes = pas::constant_set<pas::Set<0, 255>>({{1, 5}});
        static const pas::Set<0, 255> DominatorShipType = pas::constant_set<pas::Set<0, 255>>({{0}});
        std::int32_t I{};
        std::int32_t CareerThreshold{};
        std::int32_t ShipCount{};
        aGalaxy::TStar* Star{};
        std::uint8_t Good{};
        float EnemyStrength{};
        float FriendlyStrength{};
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (Self->CargoGoods[Good].Count > 0) {
                return;
            }
        }
        if (Self->GetCargoHook() == nullptr) {
            return;
        }
        if (Self->GetHull()->Weight > Self->GetHull()->HullPoints) {
            return;
        }
        switch (Self->PreferredCareer) {
            case aGalaxyStruct::rcTrader: CareerThreshold = 3; break;
            case aGalaxyStruct::rcPirate: CareerThreshold = 1; break;
            case aGalaxyStruct::rcWarrior: CareerThreshold = 0; break;
            default: CareerThreshold = 0; break;
        }
        // Native retains this comparison although neither branch does anything.
        static_cast<void>(UnusedMode == 0);
        aGalaxy::TStar* NextStar = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
            if (Star->Constellation->Id == 20) {
                continue;
            }
            if (Self->CurrentStar->StarDistances[I].Distance > Self->JumpRange) {
                break;
            }
            if (aKling::BlazerShip == nullptr || aKling::BlazerShip->CurrentStar != Star || Self->Aggression >= 20 && (Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShipTypes)) <= 6 || aMyFunction::NextRandomUnitFloat(Self->RandomState) >= 0.95L) && static_cast<std::uint8_t>(Self->virtual_TShip_AcceptsRansomDemandFrom(aKling::BlazerShip) ^ 1) && (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition || Self->GetFuelTanks()->Fuel / 2 >= Self->CurrentStar->StarDistances[I].Distance)) {
                if (pas::list_count(Star->Ships) <= 20) {
                    ShipCount = Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShipTypes));
                    FriendlyStrength = Star->SumBestRangerRelativeStrength(static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShipTypes));
                    EnemyStrength = Star->SumBestRangerRelativeStrength(static_cast<aGalaxyStruct::TShipTypeMask>(DominatorShipType));
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators && (Self->GetFuelTanks()->Fuel / 2 > Self->CurrentStar->StarDistances[I].Distance && ShipCount > 0 || ShipCount > 4 && ShipCount > Star->ShipTypeCounts[aGalaxyStruct::stKling] && FriendlyStrength > EnemyStrength)) {
                        if (Star->Status.Battle != 0) {
                            Self->OrderJump(Star, false);
                            return;
                        }
                        if (NextStar == nullptr) {
                            NextStar = Star;
                        }
                    }
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators && static_cast<long double>(FriendlyStrength) + Self->StrengthInBestRanger + 2.0L > EnemyStrength && Self->GetFuelTanks()->Fuel / 2 > Self->CurrentStar->StarDistances[I].Distance) {
                        if (static_cast<long double>(FriendlyStrength) + Self->StrengthInBestRanger > EnemyStrength) {
                            Self->OrderJump(Star, false);
                            return;
                        }
                        if (NextStar == nullptr) {
                            NextStar = Star;
                        }
                    }
                    if (Star->Status.Battle != 0 && Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && (pas::list_count(Star->Ships) / 2 > Star->ShipTypeCounts[aGalaxyStruct::stKling] || CareerThreshold + 4 < ShipCount || Self->GetFuelTanks()->Fuel / 2 > Self->CurrentStar->StarDistances[I].Distance || static_cast<long double>(FriendlyStrength) + Self->StrengthInBestRanger > EnemyStrength)) {
                        Self->OrderJump(Star, false);
                        return;
                    }
                }
            }
        }
        if (NextStar == nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
                if (Star->Constellation->Id != 20 && Star->Status.ControlFaction != aGalaxyStruct::sfCoalition && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShipTypes)) <= 12 && (aKling::BlazerShip == nullptr || aKling::BlazerShip->CurrentStar != Star || Self->Aggression >= 30 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShipTypes)) <= 6)) {
                    NextStar = Self->FindNextStarTowardDestination(Star, true);
                    if (NextStar != nullptr) {
                        break;
                    }
                }
            }
        }
        if (NextStar != nullptr) {
            Self->OrderJump(NextStar, false);
        }
    }

    // Native distant-system branch tests ships in the current system (), rather than the candidate system.
    std::uint8_t TRanger::TryOrderTravelToShipTypeLocation(std::uint8_t ShipType) {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        if (CurrentStar->ShipTypeCounts[ShipType] > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship->TypeId == ShipType && Ship->InNormalSpace() && Ship->virtual_TShip_CanDock(this)) {
                    OrderLanding(Ship, true);
                    return true;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
            if (Star->ShipTypeCounts[ShipType] != 0 && Star->ShipTypeCounts[aGalaxyStruct::stRanger] <= 13) {
                if (GetEngine()->JumpRange < CurrentStar->StarDistances[I].Distance) {
                    break;
                }
                if (Star->Constellation->Id == 20) {
                    continue;
                }
                // Native scans the current system here, even though Star is a remote candidate.
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_3.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, J);
                    if (Ship->TypeId == ShipType && Ship->InNormalSpace()) {
                        OrderJump(Star, true);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // Can follow a partner's travel order; otherwise favors short travel to a suitable planet, station or peaceful system.
    void TRanger_SelectNearestReachableDestination(TRanger* Self) {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        std::int32_t CurrentTurns{};
        std::int32_t Turns{};
        pas::Object* BestTarget{};
        std::int32_t BestTurns{};
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        if (Self->PartnerShip != nullptr) {
            if (Self->PartnerShip->CurrentStar == Self->CurrentStar) {
                if (Self->Order == aShip::soLand && Self->PartnerShip->OrderTarget == Self->OrderTarget) {
                    return;
                }
                if (Self->PartnerShip->Order == aShip::soLand) {
                    if (Self->CanRefuel() || Self->HasCargoGoods() && Self->GetDesiredCargoFreeSpace() > Self->CargoFreeSpace || pas::class_cast_if<aRuins::TRuins*>(Self->PartnerShip->OrderTarget) != nullptr || ([&] {
                        std::int32_t cpp_left = Self->GetHull()->Weight - Self->GetDesiredCargoFreeSpace();
                        return cpp_left < Self->GetCarriedItemWeight();
                    }()) || Self->GetHullIntegrityPercent() < 70 || Self->HasHullDamageOrBrokenEquippedItems()) {
                        if (!(pas::class_cast_if<aRuins::TRuins*>(Self->PartnerShip->OrderTarget) != nullptr) || pas::checked_cast<aRuins::TRuins*>(Self->PartnerShip->OrderTarget)->virtual_TShip_CanDock(Self)) {
                            Self->OrderLanding(Self->PartnerShip->OrderTarget, true);
                        }
                        return;
                    }
                } else if (Self->PartnerShip->Order == aShip::soJump) {
                    Star = pas::checked_cast<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget);
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"") {
                        Self->OrderJump(Star, false);
                        return;
                    }
                }
            } else {
                if (Self->PartnerShip->Order == aShip::soJump && pas::class_cast_if<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget) != nullptr && Self->PartnerShip->OrderTarget != Self->CurrentStar) {
                    Self->OrderJump(pas::checked_cast<aGalaxy::TStar*>(Self->PartnerShip->OrderTarget), true);
                } else {
                    Self->OrderJump(Self->PartnerShip->CurrentStar, true);
                }
                return;
            }
        }
        if (pas::is_one_of<aShip::soLand, aShip::soJump, aShip::soJumpHole>(Self->Order)) {
            CurrentTurns = Self->EstimateOrderTravelTurns();
            BestTarget = Self->OrderTarget;
            BestTurns = CurrentTurns;
        } else {
            CurrentTurns = 1000;
            BestTarget = nullptr;
            BestTurns = CurrentTurns;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Self->CurrentStar->Planets, I);
            if (Self->virtual_TShip_CanQueueReachablePlanet(Planet) && pas::in_set<0, 4, 7, 7>(Planet->OwnerId)) {
                Turns = Self->EstimateTravelTurnsToObject(Planet);
                if (BestTurns > Turns) {
                    BestTurns = Turns;
                    BestTarget = Planet;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
            if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Ship->virtual_TShip_CanDock(Self)) {
                Turns = Self->EstimateTravelTurnsToObject(Ship);
                if (BestTurns > Turns) {
                    BestTurns = Turns;
                    BestTarget = Ship;
                }
            }
        }
        if (!(pas::class_cast_if<aPlanet::TPlanet*>(BestTarget) != nullptr) && !(pas::class_cast_if<aShip::TShip*>(BestTarget) != nullptr)) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
                if (Self->CurrentStar->StarDistances[I].Distance > Self->JumpRange) {
                    break;
                }
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
                if (Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Constellation->Id != 20 && Star->Status.CustomFaction == u"") {
                    Turns = Self->EstimateTravelTurnsToObject(Star);
                    if (BestTurns > Turns) {
                        BestTurns = Turns;
                        BestTarget = Star;
                    }
                }
            }
        }
        if (BestTarget != nullptr && BestTarget != Self->OrderTarget) {
            if (pas::class_cast_if<aPlanet::TPlanet*>(BestTarget) != nullptr) {
                Self->OrderLanding(BestTarget, false);
            } else if (pas::class_cast_if<aShip::TShip*>(BestTarget) != nullptr) {
                Self->OrderLanding(BestTarget, false);
            } else if (pas::class_cast_if<aGalaxy::TStar*>(BestTarget) != nullptr) {
                Self->OrderJump(pas::checked_cast<aGalaxy::TStar*>(BestTarget), false);
            }
        }
    }

    // Excludes the last docked planet/station and can favor leaving their system.
    void TRanger_SelectAlternateReachableDestination(TRanger* Self) {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        std::int32_t CurrentTurns{};
        std::int32_t Turns{};
        pas::Object* BestTarget{};
        std::int32_t BestTurns{};
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        if (pas::is_one_of<aShip::soLand, aShip::soJump, aShip::soJumpHole>(Self->Order)) {
            CurrentTurns = Self->EstimateOrderTravelTurns();
            BestTarget = Self->OrderTarget;
            BestTurns = CurrentTurns;
        } else {
            CurrentTurns = 1000;
            BestTarget = nullptr;
            BestTurns = CurrentTurns;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Self->CurrentStar->Planets, I);
            if (Planet != Self->LastDockedPlanet && Self->virtual_TShip_CanQueueReachablePlanet(Planet) && pas::in_set<0, 4, 7, 7>(Planet->OwnerId)) {
                Turns = Self->EstimateTravelTurnsToObject(Planet);
                if (BestTurns > Turns) {
                    BestTurns = Turns;
                    BestTarget = Planet;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
            if (Ship != Self->LastDockedNonPlanetLocation && pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Ship->virtual_TShip_CanDock(Self)) {
                Turns = Self->EstimateTravelTurnsToObject(Ship);
                if (BestTurns > Turns) {
                    BestTurns = Turns;
                    BestTarget = Ship;
                }
            }
        }
        std::int32_t LeavingBonus = 0;
        if (Self->GetHullIntegrityPercent() > 70 && (Self->LastDockedNonPlanetLocation != nullptr && Self->LastDockedNonPlanetLocation->CurrentStar == Self->CurrentStar || Self->LastDockedPlanet != nullptr && Self->LastDockedPlanet->CurrentStar == Self->CurrentStar)) {
            LeavingBonus += 10;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
            if (Self->CurrentStar->StarDistances[I].Distance > Self->JumpRange) {
                break;
            }
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
            if (Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Constellation->Id != 20 && Star->Status.CustomFaction == u"") {
                Turns = Self->EstimateTravelTurnsToObject(Star) - LeavingBonus;
                if (BestTurns > Turns) {
                    BestTurns = Turns;
                    BestTarget = Star;
                }
            }
        }
        if (BestTarget != nullptr && BestTarget != Self->OrderTarget) {
            if (pas::class_cast_if<aPlanet::TPlanet*>(BestTarget) != nullptr) {
                Self->OrderLanding(BestTarget, false);
            } else if (pas::class_cast_if<aShip::TShip*>(BestTarget) != nullptr) {
                Self->OrderLanding(BestTarget, false);
            } else if (pas::class_cast_if<aGalaxy::TStar*>(BestTarget) != nullptr) {
                Self->OrderJump(pas::checked_cast<aGalaxy::TStar*>(BestTarget), false);
            }
        }
    }

    void TRanger::SellCargoGoods() {
        std::uint8_t Good{};
        float Cost{};
        aPlanet::TPlanet* BestPlanet{};
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (CargoGoods[Good].Count != 0) {
                Cost = GetAverageCargoCost(Good);
                if (static_cast<long double>(ShopGoodsSellPrice(Good, nullptr)) > Cost || FindBestQueuedSellPlanetProfitScore(Good, BestPlanet, Cost) < 80 || static_cast<std::uint8_t>(NeedsWealthCatchup() ^ 1)) {
                    SellGoodsToLocation(Good, CargoGoods[Good].Count);
                }
            }
        }
        RefreshDerivedStats(true);
    }

    void TRanger::BuyProfitableGoods() {
        std::uint8_t Good{};
        std::uint8_t BestGood{};
        std::int32_t Count{};
        double BestRatio{};
        aPlanet::TPlanet* BestPlanet{};
        std::uint8_t KeepBuying = true;
        while (KeepBuying & (Money > 0)) {
            BestRatio = 0.0;
            BestGood = 0;
            for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                if (CurrentPlanet->Goods[Good].Count > 0) {
                    if (([&] {
                        pas::Extended cpp_right = ShopGoodsPurchasePrice(Good, nullptr);
                        return aConst::GoodsMarket[Good].AveragePrice * 1.1L > cpp_right;
                    }()) && ([&] {
                        pas::Extended cpp_right_2 = ShopGoodsPurchasePrice(Good, nullptr);
                        return pas::real_divide(aConst::GoodsMarket[Good].AveragePrice, cpp_right_2);
                    }()) > BestRatio && FindBestQueuedSellPlanetProfitScore(Good, BestPlanet, ShopGoodsPurchasePrice(Good, nullptr)) > 50) {
                        std::int32_t cpp_right_3 = ShopGoodsPurchasePrice(Good, nullptr);
                        pas::Extended cpp_left = std::min<std::int32_t>(CargoFreeSpace, CurrentPlanet->Goods[Good].Count) * cpp_right_3;
                        if (cpp_left > pas::real_min<pas::Extended>(Money * 0.2L, Wealth * 0.05L)) {
                            BestRatio = ([&] {
                                pas::Extended cpp_right_4 = ShopGoodsPurchasePrice(Good, nullptr);
                                return pas::real_divide(aConst::GoodsMarket[Good].AveragePrice, cpp_right_4);
                            }());
                            BestGood = Good;
                        }
                    }
                }
            }
            if (BestRatio > 0.0L) {
                {
                    pas::Extended cpp_right_5 = ShopGoodsPurchasePrice(BestGood, nullptr);
                    std::int64_t trunc = System::Trunc(pas::real_divide(Money, cpp_right_5));
                    std::int64_t cargoFreeSpace = static_cast<std::int64_t>(CargoFreeSpace);
                    Count = std::min<std::int64_t>(trunc, cargoFreeSpace);
                }
                if (Count > 0) {
                    Count = std::min<std::int32_t>(CurrentPlanet->Goods[BestGood].Count, Count);
                    BuyGoodsFromLocation(BestGood, Count);
                } else {
                    KeepBuying = false;
                }
            } else {
                KeepBuying = false;
            }
            RefreshDerivedStats(true);
        }
    }

    // Skips queue index 0. Leaves BestPlanet unchanged unless a candidate improves the score; UnitCost must be nonzero.
    std::uint8_t TRanger::FindBestQueuedSellPlanetProfitScore(std::uint8_t Good, aPlanet::TPlanet*& BestPlanet, double UnitCost) {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        double Score{};
        std::uint8_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(PlanetQueue) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(PlanetQueue, I);
            if (Planet->CurrentStar == CurrentStar) {
                Score = aMyFunction::RemapClamped(pas::real_divide(ShopGoodsSellPrice(Good, Planet), UnitCost), 1.0, 1.4, 0.0, 1.0E+2);
            } else {
                Score = aMyFunction::RemapClamped(pas::real_divide(ShopGoodsSellPrice(Good, Planet), UnitCost), 1.0, 2.0, 0.0, 1.0E+2);
            }
            if (static_cast<long double>(Result) < Score) {
                BestPlanet = Planet;
                Result = System::Round(Score);
            }
        }
        return Result;
    }

    void TRanger::ApplyIllegalGoodsTradeRelationsPenalty(std::int32_t TotalTradeValue) {
        if (CurrentPlanet != nullptr) {
            CurrentPlanet->ChangeRelationToRanger(this, -System::Round(aMyFunction::RemapClamped(TotalTradeValue, aGalaxy::Galaxy->AverageRangerCapital / 30, aGalaxy::Galaxy->AverageRangerCapital / 7, 2.0E+1, 7.0E+1)));
            if (aPlayer::GetPlayer() == this) {
                AddPirateCareerActivity(4);
            } else if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L) {
                AddPirateCareerActivity(4);
            }
        }
    }

    // Repairs eligible installed items even without sufficient money; subtracts cost only when Money is strictly greater.
    void TRanger::RepairBrokenEquipmentAtLocation() {
        std::int32_t I{};
        std::int32_t Cost{};
        aItem::TEquipment* Equipment{};
        aItem::TEquipment* Artefact{};
        {
            const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if ((!(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && aShip::TShip_CanRepairEquipmentTech(this, Equipment) && (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 5.0E+1L) && Equipment->EquippedFlag != 0) {
                        Cost = aItem::TEquipment_CalculateRepairCost(Equipment);
                        // Native repair proceeds even when there is not enough money.
                        if (Money > Cost) {
                            SetMoney(Money - Cost);
                        }
                        Equipment->Repair();
                    }
                }
            }
        }
        if (CanRepairArtefactsAtLocation()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
                Artefact = pas::list_at<aItem::TEquipment>(Artefacts, I);
                if ((Artefact->BrokenFlag != 0 || Artefact->ConditionPercent < 5.0E+1L) && Artefact->EquippedFlag != 0) {
                    Cost = aItem::TEquipment_CalculateRepairCost(Artefact);
                    if (Money > Cost) {
                        SetMoney(Money - Cost);
                    }
                    Artefact->Repair();
                }
            }
        }
    }

    std::uint8_t TRanger::RelationToNonRanger(aShip::TShip* Ship) {
        std::uint8_t Result{};
        if (CurrentStanding == aGalaxyStruct::ssCoalitionActive && Ship->CurrentStanding == aGalaxyStruct::ssPirateMilitary) {
            return 0;
        }
        if (CurrentStanding == aGalaxyStruct::ssPirateActive && Ship->CurrentStanding == aGalaxyStruct::ssCoalitionMilitary) {
            return 0;
        }
        switch (Ship->TypeId) {
            case aGalaxyStruct::stTransport: {
                if (PreferredCareer == aGalaxyStruct::rcTrader) {
                    return 90;
                } else if (PreferredCareer == aGalaxyStruct::rcWarrior) {
                    return *([&] {
                        auto cpp_index = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                        auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                        return &(*cpp_array)[cpp_index];
                    }());
                } else {
                    return pas::shr(static_cast<std::int32_t>(*([&] {
                        auto cpp_index_2 = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                        auto* cpp_array_2 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                        return &(*cpp_array_2)[cpp_index_2];
                    }())), 1);
                }
            }
            case aGalaxyStruct::stPirate: {
                if (PreferredCareer == aGalaxyStruct::rcPirate) {
                    Result = *([&] {
                        auto cpp_index_3 = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                        auto* cpp_array_3 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                        return &(*cpp_array_3)[cpp_index_3];
                    }());
                } else {
                    Result = pas::shr(static_cast<std::int32_t>(*([&] {
                        auto cpp_index_4 = aConst::RaceToOwner(Ship->PilotRace) & 0x0000007f;
                        auto* cpp_array_4 = &aConst::OwnerRelations[aConst::RaceToOwner(PilotRace) & 0x0000007f];
                        return &(*cpp_array_4)[cpp_index_4];
                    }())), 1);
                }
                if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    if (([&] {
                        std::uint32_t cpp_left = (static_cast<std::uint32_t>(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates)) & 0x0000007f) * 2;
                        return cpp_left > (static_cast<std::uint32_t>(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition)) & 0x0000007f) * 3;
                    }()) && aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates) > 7) {
                        return 10;
                    }
                }
                return Result;
            }
            case aGalaxyStruct::stWarrior: {
                std::int32_t cpp_arg = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(this));
                pas::List* rangerRelations = pas::checked_cast<aWarrior::TWarrior*>(Ship)->HomePlanet->RangerRelations;
                return static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(rangerRelations, cpp_arg)));
            }
            case aGalaxyStruct::rstDominion: {
                if (PreferredCareer == aGalaxyStruct::rcTrader) {
                    return 40;
                } else if (PreferredCareer == aGalaxyStruct::rcWarrior) {
                    return 45;
                } else {
                    return 50;
                }
            }
            default: {
                if (pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator>(Ship->TypeId)) {
                    return 50;
                }
                return 100;
            }
        }
    }

    // Two female human pilots receive 100; otherwise reads the stored galaxy-indexed relation.
    std::uint8_t TRanger::RelationToRanger(void* Ranger) {
        if (IsFemaleHumanPilot() && static_cast<aShip::TShip*>(Ranger)->IsFemaleHumanPilot()) {
            return 100;
        }
        return static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<TRanger*>(static_cast<pas::Object*>(Ranger)))))));
    }

    // Applies the target's Charisma to positive changes, clamps to 0..100, and may break partnership or select an enemy.
    void TRanger::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index + 0)));
        if (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) > 0 && Amount > 0) {
            Amount += System::Round(Amount * (static_cast<aShip::TShip*>(Ranger)->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.2L);
        }
        std::int32_t NewRelation = Relation + Amount;
        if (NewRelation < 0) {
            Relation = 0;
        } else if (NewRelation > 100) {
            Relation = 100;
        } else {
            Relation = NewRelation;
        }
        pas::list_put(RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (PartnerShip == Ranger && Relation <= 30) {
            CheckForPartnershipBreakup();
            Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index + 0)));
        }
        if (Relation < 10 && (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar)) {
            EnemyShip = static_cast<aShip::TShip*>(Ranger);
        }
        if (aPlayer::GetPlayer() == Ranger) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index + 0))) != Relation && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    // Sets EnemyShip and penalizes relations with the attacking ranger or its controlling ranger.
    void TRanger::ReactToAttack(aShip::TShip* Attacker) {
        EnemyShip = Attacker;
        if (Attacker->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker, System::Round(aMyFunction::RemapClamped(GetHull()->HullPoints, GetHull()->Weight * 0.5L, GetHull()->Weight, -8.0E+1, -2.0E+1)));
        }
        if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(Attacker->PartnerShip, System::Round(aMyFunction::RemapClamped(GetHull()->HullPoints, GetHull()->Weight * 0.5L, GetHull()->Weight, -8.0E+1, -2.0E+1)));
        }
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
            ChangeRelationToRanger(reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip, System::Round(aMyFunction::RemapClamped(GetHull()->HullPoints, GetHull()->Weight * 0.5L, GetHull()->Weight, -8.0E+1, -2.0E+1)));
        }
    }

    // Updates InFear and can replace EnemyShip; special simulation mode clears fear.
    std::uint8_t TRanger_RecomputeFearState(TRanger* Self) {
        std::uint8_t Result{};
        std::int32_t I{};
        std::int32_t AttackerCount{};
        aShip::TShip* Ship{};
        double HullThreshold{};
        double Threat{};
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            Result = false;
            Self->InFear = Result;
            return Result;
        }
        if (Self->CurrentPlanet != nullptr || Self->DockedTo != nullptr) {
            Result = Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar && Self->EnemyShip->Strength > Self->Strength;
            Self->InFear = Result;
            return Result;
        }
        if (Self->HasNoUsableWeapons() && Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar) {
            Result = true;
            Self->InFear = true;
            return Result;
        }
        Result = Self->GetHull()->Weight * (1.0L - aMyFunction::RemapClamped(Self->Aggression, 0.0, 1.0E+2, 0.7, 0.9)) > Self->GetHull()->HullPoints || Self->EnemyShip != nullptr && (Self->EnemyShip->OrderTarget == Self || aPlayer::GetPlayer() == Self->EnemyShip) && Self->virtual_TShip_AcceptsRansomDemandFrom(Self->EnemyShip);
        if (!Result) {
            Threat = 0.0;
            AttackerCount = 0;
            HullThreshold = aMyFunction::RemapClamped(Self->GetHull()->HullPoints, 5.0E+1, Self->GetHull()->Weight, 0.0, 3.0);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if (Ship->InNormalSpace() && Ship != Self) {
                    if (Ship == Self->EnemyShip) {
                        Threat = static_cast<long double>(Threat) + aShip::TShip_ChanceToWin(Ship, Self);
                        ++AttackerCount;
                    } else if ((Ship->EnemyShip == Self && Ship->OrderTarget == Self || Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) && aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) < 1.44E+6L || aShip::TShip_RelationToShip(Ship, Self) < 10 && aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) < 3.6E+5L) {
                        ++AttackerCount;
                        Threat = static_cast<long double>(Threat) + aShip::TShip_ChanceToWin(Ship, Self);
                        if (Self->EnemyShip == nullptr || Self->EnemyShip->CurrentStar != Self->CurrentStar || Self->EnemyShip->IsOutsideStarSpace()) {
                            Self->EnemyShip = Ship;
                        } else if (Self->EnemyShip != Ship && Self->OrderTarget != Self->EnemyShip) {
                            if (aMyFunction::PointDistanceSquared(Self->EnemyShip->Position, Self->Position) > aMyFunction::PointDistanceSquared(Ship->Position, Self->Position)) {
                                Self->EnemyShip = Ship;
                            }
                        }
                    }
                }
            }
            if (Threat + static_cast<long double>(AttackerCount - 1) * Threat * 0.3L > static_cast<long double>(HullThreshold) + aMyFunction::RemapClamped(Self->Aggression, 0.0, 1.0E+2, 0.0, 1.0)) {
                Result = true;
            }
        }
        if (aPlayer::GetPlayer() == Self->PartnerShip && Self->GetHullIntegrityPercent() > 35 && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar && aPlayer::GetPlayer()->InNormalSpace() && Self->OrderAbsolute) {
            Result = false;
        }
        Self->InFear = Result;
        return Result;
    }

    void TRanger_TryOfferRansomToPursuer(TRanger* Self) {
        pas::WideString Text{};
        std::int32_t Amount{};
        float MaximumOffer{};
        float MinimumOffer{};
        std::uint8_t Accepted{};
        aShip::TShip* OtherShip{};
        if (Self->InNormalSpace() && Self->EnemyShip != nullptr && Self->EnemyShip->OrderTarget == Self && Self->EnemyShip->TruceShip != Self && Self->Money > 100 && static_cast<std::uint8_t>(Self->CanEscapePursuer(Self->EnemyShip) ^ 1) && static_cast<std::uint8_t>(pas::contains(aConst::NonNegotiatingShipTypes, Self->EnemyShip->TypeId) ^ 1)) {
            if (([&] {
                std::int32_t cpp_left = Self->EnemyShip->GetMaxWeaponRange();
                pas::Extended cpp_right = cpp_left * Self->EnemyShip->GetMaxWeaponRange();
                return aMyFunction::PointDistanceSquared(Self->Position, Self->EnemyShip->Position) <= cpp_right;
            }()) && (aGalaxy::Galaxy->CurrentTurn * Self->EnemyShip->Id % 3 == 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.2L || Self->GetHullIntegrityPercent() < 20) && static_cast<std::uint8_t>(Self->NoTalk ^ 1) && static_cast<std::uint8_t>(Self->EnemyShip->NoTalk ^ 1)) {
                MaximumOffer = std::min<std::int32_t>(Self->Money, Self->GetWealthScaledAmount(1));
                MinimumOffer = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Self->Money), ([&] {
                    std::int32_t cpp_left_2 = Self->GetWealthScaledAmount(4);
                    return cpp_left_2 + Self->EnemyShip->GetWealthScaledAmount(4);
                }()) * 0.5L);
                Amount = System::Round(pas::real_max<double>(1.0E+2, aMyFunction::RemapClamped(Self->GetHull()->HullPoints, 0.0, Self->GetHull()->Weight, MinimumOffer, MaximumOffer)));
                OtherShip = Self->EnemyShip;
                Accepted = OtherShip->BuildTrucePaymentResponse(Self, Text, Amount);
                if (aPlayer::GetPlayer() != OtherShip && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar) {
                    Self->NotifyTruceOffer(OtherShip, Text, Amount);
                }
                if (Accepted && Self->virtual_TShip_RecomputeFearState()) {
                    aRanger::TRanger_TryOfferRansomToPursuer(Self);
                }
            }
        }
    }

    std::uint8_t TRanger_AcceptsRansomDemandFrom(TRanger* Self, aShip::TShip* Ship) {
        float LicenseFactor{};
        if (aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
            LicenseFactor = 1.15f;
        } else {
            LicenseFactor = 1.0f;
        }
        return 8.0E+1L * LicenseFactor > (Self->GetHullIntegrityPercent() & 0x0000007f) && 1.0L * LicenseFactor > ([&] {
            pas::Extended cpp_left = aShip::TShip_ChanceToWin(Self, Ship);
            return cpp_left + Self->Aggression * 0.005L;
        }()) || 0.2L * LicenseFactor > aShip::TShip_ChanceToWin(Self, Ship);
    }

    // Propagates reactions among nearby ships/planets; can update player achievements. Script-bound victims suppress the relation pass.
    void TRanger_ApplyAttackReputationChanges(TRanger* Self, aShip::TShip* Victim, double Severity) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t LastStar{};
        float Effect{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        if (aPlayer::GetPlayer() == Self && Victim->CurrentStanding != aGalaxyStruct::ssCustom) {
            if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                if (pas::checked_cast<aKling::TKling*>(Victim)->KlingType == aGalaxyStruct::ktBertor) {
                    Achievements::TrySetAchievementProgress(u"BERTORSLAYER"_w, aPlayer::GetPlayer()->DominatorKillsByType[6]);
                }
                switch (pas::checked_cast<aKling::TKling*>(Victim)->DominatorSeries) {
                    case aGalaxyStruct::dsBlazer: Achievements::TryAddAchievementProgress(u"REDKILLS"_w, 1); break;
                    case aGalaxyStruct::dsTerron: Achievements::TryAddAchievementProgress(u"GREENKILLS"_w, 1); break;
                    case aGalaxyStruct::dsKeller: Achievements::TryAddAchievementProgress(u"BLUEKILLS"_w, 1); break;
                }
            }
            if (Victim->TypeId == aGalaxyStruct::stWarrior) {
                Achievements::TryAddAchievementProgress(u"WARRIORKILLS"_w, 1);
            }
            if (Victim->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
                Achievements::TryAddAchievementProgress(u"COUNTERTERRORIST"_w, 1);
            } else if (Victim->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMedicalBase) && Self->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                Achievements::TryAddAchievementProgress(u"TERRORIST"_w, 1);
            } else if (pas::in_set<6, 6, 8, 10>(Victim->TypeId)) {
                Achievements::TryAddAchievementProgress(u"TERRORIST"_w, 1);
            }
        }
        if (Self == Victim) {
            return;
        }
        if (Victim->HasScriptBindings()) {
            return;
        }
        if (aPlayer::GetPlayer() == Self) {
            LastStar = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
        } else {
            LastStar = pas::list_count(aGalaxy::Galaxy->Stars) / 3;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LastStar); cpp_range.next(I); ) {
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
            if (Star->Status.ControlFaction != aGalaxyStruct::sfDominators || Star->Status.Battle != 0) {
                Effect = static_cast<long double>(aMyFunction::RemapClamped(I, 0.0, LastStar, 0.5, 0.05)) * Severity;
                if (Victim->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || Victim->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    Effect = 0.05L * Effect;
                } else if (aPlayer::GetPlayer() != Self) {
                    Effect = 0.1L * Effect;
                }
                if (pas::class_cast_if<aRuins::TRuins*>(Victim) != nullptr) {
                    Effect = Effect * 3.0L;
                }
                if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr && static_cast<aWarrior::TWarrior*>(Victim)->WarriorType == aWarrior::wtFlagship) {
                    Effect = Effect * 2.0L;
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship != Victim && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && pas::in_range(Ship->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                        Ship->ChangeRelationToRanger(Self, System::Round(static_cast<long double>(50 - (aShip::TShip_RelationToShip(Ship, Victim) & 0x0000007f)) * Effect));
                    }
                }
                if (Star->Status.CustomFaction == u"") {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(J); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                        if (Planet->IsCoalitionOwned) {
                            Planet->ChangeRelationToRanger(Self, System::Round(static_cast<long double>(50 - (Planet->RelationToShip(Victim) & 0x0000007f)) * Effect));
                        }
                    }
                }
            }
        }
        Effect = Severity * 0.5L;
        if (Victim->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
            Effect = Effect * 0.5L;
        }
        if (pas::class_cast_if<aRuins::TRuins*>(Victim) != nullptr) {
            Effect = Effect * 3.0L;
        } else if (Victim->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            Effect = 0.05L * Effect;
        }
        if (aPlayer::GetPlayer() != Self) {
            Effect = 0.1L * Effect;
        }
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn != 0 && Self->CurrentStar->Status.Battle == 0) {
            Effect = 0.2L * Effect;
        }
        if (aPlanet::MainPiratePlanet != nullptr) {
            std::int32_t round = System::Round(static_cast<long double>(50 - (aPlanet::MainPiratePlanet->RelationToShip(Victim) & 0x0000007f)) * Effect);
            aPlanet::TPlanet* mainPiratePlanet = aPlanet::MainPiratePlanet;
            mainPiratePlanet->ChangeRelationToRanger(Self, round);
        }
        if (Victim->HomePlanet != nullptr && Victim->HomePlanet->IsCoalitionOwned && Victim->HomePlanet->CurrentStar->Status.CustomFaction == u"") {
            std::int32_t round_2 = System::Round(pas::real_divide(50 - (Victim->HomePlanet->RelationToShip(Victim) & 0x0000007f), 2.0L));
            aPlanet::TPlanet* homePlanet = Victim->HomePlanet;
            homePlanet->ChangeRelationToRanger(Self, round_2);
        }
    }

    // Also adds pirate career activity and improves the main pirate planet's relation.
    void TRanger_ApplyExtortionReputationPenalty(TRanger* Self, aShip::TShip* Victim) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Activity{};
        float Effect{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        if (aPlayer::GetPlayer() == Self) {
            Activity = 8;
        } else {
            Activity = 1;
        }
        Self->AddPirateCareerActivity(Activity);
        std::int32_t LastStar = pas::list_count(aGalaxy::Galaxy->Stars) / 3;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LastStar); cpp_range.next(I); ) {
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
            if ((Star->Status.ControlFaction == aGalaxyStruct::sfCoalition || Star->Status.Battle != 0) && (Star->Status.CustomFaction == u"" || Star->Status.Battle != 0)) {
                Effect = aMyFunction::RemapClamped(I, 0.0, LastStar, 0.3, 0.05);
                if (aPlayer::GetPlayer() != Self) {
                    Effect = 0.1L * Effect;
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (pas::in_range(Ship->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate) && (I != 0 || Ship != Self && Ship != Victim)) {
                        Ship->ChangeRelationToRanger(Self, System::Round(static_cast<long double>(50 - (aShip::TShip_RelationToShip(Ship, Victim) & 0x0000007f)) * Effect));
                    }
                }
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(J); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                    if (Planet->IsCoalitionOwned) {
                        Planet->ChangeRelationToRanger(Self, System::Round(static_cast<long double>(50 - (Planet->RelationToShip(Victim) & 0x0000007f)) * Effect));
                    }
                }
            }
        }
        if (aPlanet::MainPiratePlanet != nullptr) {
            aPlanet::MainPiratePlanet->ChangeRelationToRanger(Self, 1);
        }
    }

    void TRanger::TryRecruitWingman() {
        std::int32_t I{};
        std::int32_t Offers{};
        std::int32_t MaxDistance{};
        std::int32_t Amount{};
        aShip::TShip* Ship{};
        TRanger* Ranger{};
        pas::WideString Text{};
        if (([&] {
            std::int32_t cpp_left = GetEffectiveSkillLevel(aShip::psLeadership, false);
            return cpp_left > CountWingmen();
        }()) && PartnerShip == nullptr && Wealth / 20 <= Money && Wealth >= aGalaxy::Galaxy->AverageRangerCapital * 1.1L && Strength >= 1.1L * aGalaxy::Galaxy->AverageRangerStrength && static_cast<std::uint8_t>(HasScriptControl() ^ 1)) {
            MaxDistance = std::max<std::int32_t>(([&] {
                std::int32_t cpp_left_2 = GetRadarRange();
                return cpp_left_2 * GetRadarRange();
            }()), 250000);
            Offers = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship->TypeId == aGalaxyStruct::stRanger && Ship->InNormalSpace() && (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) == (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) && static_cast<std::uint8_t>(Ship->HasScriptControl() ^ 1)) {
                    Ranger = pas::checked_cast<TRanger*>(Ship);
                    if (Ranger != this && Ranger->PartnerShip != this && Ranger != EnemyShip && Ranger->EnemyShip != this && aPlayer::GetPlayer() != Ranger && static_cast<long double>(MaxDistance) >= aMyFunction::PointDistanceSquared(Position, Ranger->Position) && aShip::TShip_RelationToShip(Ranger, this) >= 30 && (aMyFunction::NextRandomUnitFloat(RandomState) >= 0.7L || Rank >= Ranger->Rank) && PlaceInRating <= Ranger->PlaceInRating) {
                        Amount = std::min<std::int64_t>(System::Round(Money * 0.7L), static_cast<std::int64_t>(Ranger->Wealth / 8));
                        if (static_cast<std::uint8_t>(Ranger->virtual_TShip_BuildPartnershipOfferResponse(this, Text, Amount) ^ 1) && aMyFunction::NextRandomUnitFloat(RandomState) > 0.1L) {
                            continue;
                        }
                        if ((Ranger->virtual_TShip_AcceptPartnershipOffer(this, Text, Amount) || aMyFunction::NextRandomUnitFloat(RandomState) > 0.8L) && aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                            NotifyPartnershipOffer(Ship, Text, Amount);
                            break;
                        }
                        ++Offers;
                        if (Offers == 3) {
                            break;
                        }
                    }
                }
            }
        }
    }

    void TRanger::CheckForPartnershipBreakup() {
        aShip::TShip* Leader{};
        // Nested in the partnership check; caller-popped static link.
        auto CanNotifyPartner = [&]() -> std::uint8_t {
            std::uint8_t Result{};
            if (this->CurrentStar != this->PartnerShip->CurrentStar || static_cast<std::uint8_t>(InNormalSpace() ^ 1) || static_cast<std::uint8_t>(this->PartnerShip->InNormalSpace() ^ 1)) {
                return false;
            } else if (CanContactShip(this->PartnerShip)) {
                return true;
            } else if (static_cast<std::uint8_t>(this->PartnerShip->NoTalk ^ 1) || aPlayer::GetPlayer() != this->PartnerShip) {
                return false;
            } else {
                this->PartnerShip->NoTalk = false;
                Result = CanContactShip(this->PartnerShip);
                this->PartnerShip->NoTalk = true;
                return Result;
            }
        };
        if (PartnerShip == nullptr) {
            return;
        }
        if (PartnershipDaysRemaining < 1) {
            if (CanNotifyPartner()) {
                Leader = PartnerShip;
                if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                    OrderNone(false);
                }
                PartnerShip = nullptr;
                NotifyPartnershipExpired(Leader);
            }
        } else if (aShip::TShip_RelationToShip(this, PartnerShip) < 30) {
            if (CanNotifyPartner()) {
                Leader = PartnerShip;
                if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                    OrderNone(false);
                }
                PartnerShip = nullptr;
                NotifyPartnerBreak(Leader);
            }
        } else if (EnemyShip != nullptr && EnemyShip->PartnerShip != nullptr && EnemyShip->PartnerShip == PartnerShip && CanNotifyPartner()) {
            Leader = PartnerShip;
            if (Order == aShip::soFollowShip && OrderTarget == PartnerShip) {
                OrderNone(false);
            }
            PartnerShip = nullptr;
            NotifyPartnerRebellion(Leader);
        }
    }

    // Relation of at least 30.
    std::uint8_t TRanger_TrustsAttackRequester(TRanger* Self, aShip::TShip* Ship) {
        return aShip::TShip_RelationToShip(Self, Ship) >= 30;
    }

    // Tests relation plus a relative-strength score against 120; precise dialogue role remains unresolved.
    std::uint8_t TRanger::EvaluateAllyRelationAndStrength(aShip::TShip* Ship) {
        pas::Extended cpp_right = aMyFunction::RemapClamped(Ship->Strength, 0.9L * Strength, Strength * 3.0L, 0.0, 1.0E+2);
        return (aShip::TShip_RelationToShip(this, Ship) & 0x0000007f) + cpp_right > 1.2E+2L;
    }

    // Returns whether imprisonment blocks this turn; may imprison, release or update standing.
    std::uint8_t TRanger::ProcessPrisonAndHostileCheck() {
        std::uint8_t Result{};
        std::int32_t I{};
        aShip::TShip* Warrior{};
        // Caller-popped static link; ranger -4, blocked-turn output -5. Female human pilots have their resulting prison term cleared.
        auto Imprison = [&]() -> void {
            std::int32_t I{};
            aShip::TShip* Ship{};
            pas::WideString Text{};
            this->PrisonTermRemaining = System::Round(aMyFunction::RemapClamped(this->CareerStatus[aGalaxyStruct::rcPirate], 0.0, 1.0E+2, 61.0, 1.4E+2));
            this->CurrentSystemKills.Normal = 0;
            this->CurrentSystemKills.Pirate = 0;
            if (this->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                if (aPlanet::MainPiratePlanet != nullptr) {
                    aPlanet::MainPiratePlanet->ChangeRelationToRanger(this, 80);
                } else {
                    this->CurrentPlanet->ChangeRelationToRanger(this, 80);
                }
            } else {
                this->CurrentPlanet->ChangeRelationToRanger(this, 80);
                aRanger::TRanger_ChangePlanetRelations(this, nullptr, rcmRaiseTo, 45, pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition));
            }
            Result = true;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(this->CurrentStar->Ships, I);
                if (Ship->TypeId == aGalaxyStruct::stWarrior && this == Ship->EnemyShip) {
                    Ship->EnemyShip = nullptr;
                    if (this == Ship->OrderTarget) {
                        Ship->OrderNone(false);
                    }
                }
            }
            if (IsFemaleHumanPilot()) {
                this->PrisonTermRemaining = 0;
            } else {
                Text = aConst::PickLocalizedTextVariant(pas::concat_wide({u"GalaxyNews.GoToPrison.", GetTypeNameKey()}), this->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, this->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, this->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Month>"_w, pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(this->PrisonTermRemaining) / 30)), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, GetName(), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FullName>"_w, GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                if (aPlayer::GetPlayer()->CurrentStar == this->CurrentStar && aPlayer::GetPlayer()->InNormalSpace() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    aGalaxy::Galaxy->AddPlanetNewsWithPlayerBubble(40, Text);
                } else {
                    aGalaxy::Galaxy->AddPlanetNews(40, Text);
                }
            }
        };
        Result = false;
        if (CurrentPlanet != nullptr) {
            if (static_cast<std::uint32_t>(PrisonTermRemaining) > 0) {
                if (CurrentStar->Status.Battle != 0 && aGalaxy::Galaxy->CurrentTurn - CurrentStar->LastDominatorPresenceTurn <= 1) {
                    PrisonTermRemaining = 0;
                    Result = false;
                    this->virtual_TShip_RefreshCurrentStanding();
                    return Result;
                }
                --PrisonTermRemaining;
                if (PrisonTermRemaining == 0) {
                    Result = false;
                    this->virtual_TShip_RefreshCurrentStanding();
                    return Result;
                }
                return true;
            }
            if (CurrentPlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlHostile) {
                Imprison();
                this->virtual_TShip_RefreshCurrentStanding();
            } else {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentPlanet->Warriors) - 1); cpp_range.next(I); ) {
                    Warrior = pas::list_at<aShip::TShip>(CurrentPlanet->Warriors, I);
                    if (Warrior->EnemyShip == this) {
                        Imprison();
                        this->virtual_TShip_RefreshCurrentStanding();
                        break;
                    }
                }
            }
        }
        return Result;
    }

    void TRanger_ChangeGlobalRelations(TRanger* Self, pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask) {
        Self->ChangeShipRelations(Scope, Mode, Amount, HullTypeMask, OwnerMask);
        aRanger::TRanger_ChangePlanetRelations(Self, Scope, Mode, Amount, OwnerMask);
    }

    // Scope filters by ship, star or sector; nil selects all. Bulk changes skip scripted ships for which HasScriptControl is true. Masks use ShipToHullType categories and owner IDs, not TShip.TypeId.
    void TRanger::ChangeShipRelations(pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aConst::THullShipTypeMask HullTypeMask, aGalaxyStruct::TOwnerMask OwnerMask) {
        std::uint8_t Previous{};
        aShip::TShip* Ship{};
        std::int32_t RangerIndex{};
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        // Caller-popped static link; mode -1, previous relation -2, target ship -8, ranger index -12, ranger -16, amount +16.
        auto ApplyToShip = [&]() -> void {
            switch (Mode) {
                case rcmCapAt: {
                    if (Previous > Amount) {
                        pas::list_put(Ship->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Amount))));
                    }
                    break;
                }
                case rcmRaiseTo: {
                    if (Previous < Amount) {
                        pas::list_put(Ship->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Amount))));
                    }
                    break;
                }
                case rcmIncrease: Ship->ChangeRelationToRanger(this, Amount); break;
                case rcmDecrease: Ship->ChangeRelationToRanger(this, -Amount); break;
                case rcmDecreaseWithFloor20: {
                    if (Previous > 20) {
                        if (Previous - Amount < 20) {
                            pas::list_put(Ship->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(20))));
                        } else {
                            Ship->ChangeRelationToRanger(this, -Amount);
                        }
                    }
                    break;
                }
            }
            return;
        };
        RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(this));
        if (pas::class_cast_if<aShip::TShip*>(Scope) != nullptr) {
            Ship = pas::checked_cast<aShip::TShip*>(Scope);
            if (pas::contains(HullTypeMask, aConst::ShipToHullType(Ship)) && pas::contains(OwnerMask, Ship->OwnerId)) {
                Previous = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->RangerRelations, RangerIndex)));
                ApplyToShip();
            }
        } else {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                if (pas::class_cast_if<aGalaxy::TConstellation*>(Scope) != nullptr) {
                    if (pas::checked_cast<aGalaxy::TConstellation*>(Scope) != Star->Constellation) {
                        continue;
                    }
                }
                if (pas::class_cast_if<aGalaxy::TStar*>(Scope) != nullptr) {
                    if (pas::checked_cast<aGalaxy::TStar*>(Scope) != Star) {
                        continue;
                    }
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship->ScriptShip != nullptr && Ship->HasScriptControl()) {
                        continue;
                    }
                    if (pas::contains(HullTypeMask, aConst::ShipToHullType(Ship)) && pas::contains(OwnerMask, Ship->OwnerId) && pas::list_count(Ship->RangerRelations) >= 1) {
                        Previous = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->RangerRelations, RangerIndex)));
                        ApplyToShip();
                    }
                }
            }
        }
    }

    // Scope filters by planet, star or sector; nil selects all. Only coalition planets are affected. OwnerMask uses owner IDs as bits.
    void TRanger_ChangePlanetRelations(TRanger* Self, pas::Object* Scope, TRelationChangeMode Mode, std::uint8_t Amount, aGalaxyStruct::TOwnerMask OwnerMask) {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t Previous{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::int32_t RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(Self));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (pas::class_cast_if<aGalaxy::TConstellation*>(Scope) != nullptr) {
                if (pas::checked_cast<aGalaxy::TConstellation*>(Scope) != Star->Constellation) {
                    continue;
                }
            }
            if (pas::class_cast_if<aGalaxy::TStar*>(Scope) != nullptr) {
                if (pas::checked_cast<aGalaxy::TStar*>(Scope) != Star) {
                    continue;
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                if (pas::contains(OwnerMask, Planet->OwnerId) && Planet->IsCoalitionOwned) {
                    if (pas::class_cast_if<aPlanet::TPlanet*>(Scope) != nullptr) {
                        if (pas::checked_cast<aPlanet::TPlanet*>(Scope) != Planet) {
                            continue;
                        }
                    }
                    Previous = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Planet->RangerRelations, RangerIndex)));
                    switch (Mode) {
                        case rcmCapAt: {
                            if (Previous > Amount) {
                                pas::list_put(Planet->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Amount))));
                            }
                            break;
                        }
                        case rcmRaiseTo: {
                            if (Previous < Amount) {
                                pas::list_put(Planet->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Amount))));
                            }
                            break;
                        }
                        case rcmIncrease: Planet->ChangeRelationToRanger(Self, Amount); break;
                        case rcmDecrease: Planet->ChangeRelationToRanger(Self, -Amount); break;
                        case rcmDecreaseWithFloor20: {
                            if (Previous > 20) {
                                if (Previous - Amount < 20) {
                                    pas::list_put(Planet->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(20))));
                                } else {
                                    Planet->ChangeRelationToRanger(Self, -Amount);
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
        if (aPlayer::GetPlayer() == Self) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckHaterAchievement();
            if (Scope == nullptr) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"GlobalChangeToPlayerReputation"_w, nullptr);
                Event->AddData(Mode);
                Event->AddData(Amount);
                Event->AddData(pas::load_unaligned<std::uint8_t>(&OwnerMask));
            }
        }
    }

    // Averages stored relations for matching ships; empty selection returns 50. A single ship uses its virtual RelationToRanger.
    std::uint8_t TRanger::GlobalRelationsShips(pas::Object* Scope, std::uint16_t HullTypeMask, std::uint8_t OwnerMask) {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        std::uint8_t Previous{};
        if (pas::class_cast_if<aShip::TShip*>(Scope) != nullptr) {
            return pas::checked_cast<aShip::TShip*>(Scope)->RelationToRanger(this);
        }
        std::int32_t RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(this));
        std::int32_t Total = 0;
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (pas::class_cast_if<aGalaxy::TConstellation*>(Scope) != nullptr) {
                if (pas::checked_cast<aGalaxy::TConstellation*>(Scope) != Star->Constellation) {
                    continue;
                }
            }
            if (pas::class_cast_if<aGalaxy::TStar*>(Scope) != nullptr) {
                if (pas::checked_cast<aGalaxy::TStar*>(Scope) != Star) {
                    continue;
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (pas::contains(pas::load_unaligned<aConst::THullShipTypeMask>(&HullTypeMask), aConst::ShipToHullType(Ship)) && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&OwnerMask), Ship->OwnerId)) {
                    ++Count;
                    Previous = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ship->RangerRelations, RangerIndex)));
                    Total += Previous;
                }
            }
        }
        if (Count != 0) {
            return pas::idiv(Total, Count);
        }
        return 50;
    }

    // Averages Coalition planets in matching stars/sectors; empty selection returns 50. A planet Scope does not narrow this native scan.
    std::uint8_t TRanger::GlobalRelationsPlanets(pas::Object* Scope, std::uint8_t OwnerMask) {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t Previous{};
        std::int32_t RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(this));
        std::int32_t Total = 0;
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (pas::class_cast_if<aGalaxy::TConstellation*>(Scope) != nullptr) {
                if (pas::checked_cast<aGalaxy::TConstellation*>(Scope) != Star->Constellation) {
                    continue;
                }
            }
            if (pas::class_cast_if<aGalaxy::TStar*>(Scope) != nullptr) {
                if (pas::checked_cast<aGalaxy::TStar*>(Scope) != Star) {
                    continue;
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&OwnerMask), Planet->OwnerId) && Planet->IsCoalitionOwned) {
                    ++Count;
                    Previous = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Planet->RangerRelations, RangerIndex)));
                    Total += Previous;
                }
            }
        }
        if (Count != 0) {
            return pas::idiv(Total, Count);
        }
        return 50;
    }

    void TRanger::AssignWeaponTargetsInStar() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        aAsteroid::TAsteroid* Asteroid{};
        float Distance{};
        aMissile::TMissile* Missile{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Weapons[I];
                    Weapon->Target = nullptr;
                }
            }
        }
        std::int32_t AssignedCount = 0;
        if (CurrentStar->Status.Battle != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Ship->InNormalSpace()) {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            Weapon = Weapons[J];
                            if ((static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapon->GetWeaponInfo()->ShotType) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                pas::Extended cpp_right = pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon));
                                if (aMyFunction::PointDistanceSquared(Position, Ship->Position) <= cpp_right) {
                                    Weapon->Target = Ship;
                                    ++AssignedCount;
                                    if (WeaponCount == AssignedCount) {
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
            const std::int32_t cpp_last_3 = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last_3) {
                for (J = 1; J <= cpp_last_3; ++J) {
                    Weapon = Weapons[J];
                    if ((static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapon->GetWeaponInfo()->ShotType) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                        pas::Extended cpp_right_2 = pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon));
                        if (aMyFunction::PointDistanceSquared(Position, EnemyShip->Position) <= cpp_right_2) {
                            Weapon->Target = EnemyShip;
                            ++AssignedCount;
                            if (WeaponCount == AssignedCount) {
                                return;
                            }
                        }
                    }
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && Ship != this && (aShip::TShip_RelationToShip(this, Ship) < 10 || Ship == EnemyShip || Ship->EnemyShip == this) && Ship->LiberationGroup == nullptr && TruceShip != Ship && (aPlayer::GetPlayer() != Ship || aPlayer::GetPlayer()->TruceShip != this)) {
                // Native can replace an existing weapon target in this pass.
                const std::int32_t cpp_last_4 = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last_4) {
                    for (J = 1; J <= cpp_last_4; ++J) {
                        Weapon = Weapons[J];
                        if ((static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapon->GetWeaponInfo()->ShotType) ^ 1) || Weapon->Ammo != 0) && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                            pas::Extended cpp_right_3 = pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon));
                            if (aMyFunction::PointDistanceSquared(Position, Ship->Position) <= cpp_right_3) {
                                Weapon->Target = Ship;
                                ++AssignedCount;
                                if (WeaponCount == AssignedCount) {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Missiles) - 1); cpp_range_3.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
            if (Missile->Target == this && Missile->OwnerShip != this) {
                const std::int32_t cpp_last_5 = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last_5) {
                    for (J = 1; J <= cpp_last_5; ++J) {
                        Weapon = Weapons[J];
                        if (static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapon->GetWeaponInfo()->ShotType) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                            pas::Extended cpp_right_4 = pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon));
                            if (aMyFunction::PointDistanceSquared(Position, Missile->Position) <= cpp_right_4) {
                                Weapon->Target = Missile;
                                ++AssignedCount;
                                if (WeaponCount == AssignedCount) {
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (aShip::TShip_IsEquipmentUsable(this, GetCargoHook()) && static_cast<std::uint8_t>(OrderAbsolute ^ 1)) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Asteroids) - 1); cpp_range_4.next(I); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                if (Asteroid->MineralCount <= CargoFreeSpace) {
                    Distance = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                    if (Distance <= 1.0E+6L) {
                        const std::int32_t cpp_last_6 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_6) {
                            for (J = 1; J <= cpp_last_6; ++J) {
                                Weapon = Weapons[J];
                                if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                    if (static_cast<long double>(Distance) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                        Weapon->Target = Asteroid;
                                        ++AssignedCount;
                                        if (WeaponCount == AssignedCount) {
                                            return;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace() && aPlayer::GetPlayer()->ChameleonActive && IsPlayerChameleonEffectiveAgainstSelf()) {
            const std::int32_t cpp_last_7 = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last_7) {
                for (J = 1; J <= cpp_last_7; ++J) {
                    Weapon = Weapons[J];
                    if ((static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapon->GetWeaponInfo()->ShotType) ^ 1) || Weapon->Ammo != 0) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                        pas::Extended cpp_right_5 = pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon));
                        if (aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) <= cpp_right_5) {
                            Weapon->Target = aPlayer::GetPlayer();
                            ++AssignedCount;
                            if (WeaponCount == AssignedCount) {
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    // May attempt extortion and assign weapon targets; preserves a prior enemy when no replacement qualifies.
    void TRanger::SelectEnemyShipInStar() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        double Chance{};
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            return;
        }
        if (UsableWeaponCount == 0) {
            return;
        }
        if (PartnerShip != nullptr && PartnerShip->EnemyShip != nullptr && (PartnerShip->OrderTarget == PartnerShip->EnemyShip || PartnerShip->EnemyShip->OrderTarget == PartnerShip) && (static_cast<std::uint8_t>(IsFemaleHumanPilot() ^ 1) || static_cast<std::uint8_t>(PartnerShip->EnemyShip->IsFemaleHumanPilot() ^ 1))) {
            EnemyShip = PartnerShip->EnemyShip;
            return;
        }
        if (PreferredCareer == aGalaxyStruct::rcPirate) {
            if (aPlayer::GetPlayer()->QuestTargetDefendShip != nullptr && aPlayer::GetPlayer()->QuestTargetDefendShip->CurrentStar == CurrentStar && aPlayer::GetPlayer()->QuestTargetDefendShip != this && aPlayer::GetPlayer()->QuestTargetDefendShip->InNormalSpace() && aPlayer::GetPlayer()->QuestTargetDefendShip != TruceShip && aPlayer::GetPlayer()->QuestTargetDefendShip->ScriptShip == nullptr) {
                EnemyShip = aPlayer::GetPlayer()->QuestTargetDefendShip;
                AssignWeaponTargetsInStar();
                return;
            }
        }
        aShip::TShip* PreviousEnemy = EnemyShip;
        EnemyShip = nullptr;
        double BestChance = 0.0;
        std::uint8_t HasPriorityTarget = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
            if (HasPriorityTarget && Ship->TargetingRestriction != 6) {
                continue;
            }
            if (Ship == this || Ship->IsOutsideStarSpace() || TruceShip == Ship || pas::in_range(Ship->TargetingRestriction, 1, 4) || IsFemaleHumanPilot() && Ship->IsFemaleHumanPilot() || Ship->PartnerShip != nullptr && Ship->PartnerShip->CurrentStar == CurrentStar || Ship->LiberationGroup != nullptr) {
                continue;
            }
            Chance = aShip::TShip_ChanceToWin(this, Ship);
            if (Ship->TargetingRestriction == 6 && PreferredCareer == aGalaxyStruct::rcPirate && static_cast<std::uint8_t>(HasPriorityTarget ^ 1)) {
                BestChance = Chance;
                HasPriorityTarget = true;
                EnemyShip = Ship;
                continue;
            }
            if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                if (Chance < BestChance && aMyFunction::NextRandomUnitFloat(RandomState) > 0.5L) {
                    continue;
                }
            } else {
                if (Chance < BestChance || Chance < 0.3L && StrengthInBestRanger < 0.8L) {
                    continue;
                }
                if (PreferredCareer == aGalaxyStruct::rcPirate && GetDesiredCargoFreeSpace() <= CargoFreeSpace && GetCargoHook() != nullptr) {
                    {
                        std::int32_t cpp_left = aMyFunction::NextRandomIntRange(0, 30, RandomState) + 60;
                        if (cpp_left < (aShip::TShip_RelationToShip(this, Ship) & 0x0000007f)) {
                            continue;
                        }
                    }
                    if (aShip::TShip_RelationToShip(this, Ship) >= 60 && (Aggression * 0.01L + Chance < 2.0L || aMyFunction::NextRandomUnitFloat(RandomState) > 0.2L)) {
                        continue;
                    }
                } else if (CurrentStar->Status.Battle != 0 || aShip::TShip_RelationToShip(this, Ship) >= 10) {
                    continue;
                }
                if (Chance < 1.0L && static_cast<std::uint8_t>(IsTargetStillPursuable(Ship) ^ 1) || Ship->TypeId == aGalaxyStruct::stRanger && Chance < 0.9L && aPlayer::GetPlayer() != Ship) {
                    continue;
                }
            }
            EnemyShip = Ship;
            BestChance = Chance;
        }
        if (EnemyShip != nullptr) {
            if (EnemyShip->TargetingRestriction == 6) {
                return;
            }
            if (!CanContactShip(EnemyShip)) {
                return;
            }
            if (!aShip::TShip_TryExtortShip(this, EnemyShip)) {
                AssignWeaponTargetsInStar();
                return;
            }
        }
        EnemyShip = PreviousEnemy;
    }

    void TRanger::EngageEnemyShip() {
        if (Order == aShip::soFollowShip) {
            OrderNone(false);
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            if (EnemyShip->InNormalSpace()) {
                OrderFollowShip(EnemyShip, 1, false);
                if (aShip::TShip_ChanceToWin(this, EnemyShip) < 0.8L) {
                    aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
                }
            } else if (aShip::TShip_ChanceToWin(this, EnemyShip) > 3.0L && GetHullIntegrityPercent() > 70 && EnemyShip->GetHullIntegrityPercent() > 70) {
                if (EnemyShip->CurrentPlanet != nullptr) {
                    if (pas::class_cast_if<TRanger*>(EnemyShip) != nullptr && static_cast<std::uint32_t>(static_cast<TRanger*>(EnemyShip)->PrisonTermRemaining) > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                        return;
                    }
                    if (pas::class_cast_if<aPirate::TPirate*>(EnemyShip) != nullptr && static_cast<aPirate::TPirate*>(EnemyShip)->PrisonTermRemaining > 0) {
                        EnemyShip = nullptr;
                        OrderNone(false);
                        return;
                    }
                    OrderMove(EnemyShip->CurrentPlanet->GetPosition(), false);
                } else if (EnemyShip->DockedTo != nullptr) {
                    OrderMove(EnemyShip->DockedTo->Position, false);
                }
            }
        }
    }

    void TRanger::ProcessCombatDialogue() {
        if (EnemyShip != nullptr && OrderTarget == EnemyShip && (static_cast<std::int32_t>(Seed) + aGalaxy::Galaxy->CurrentTurn) % 4 == 0 && static_cast<std::uint8_t>(this->virtual_TShip_AcceptsRansomDemandFrom(EnemyShip) ^ 1)) {
            aShip::TShip_TryExtortShip(this, EnemyShip);
        }
        if (EnemyShip != nullptr && OrderTarget == EnemyShip && (static_cast<std::int32_t>(Seed) + aGalaxy::Galaxy->CurrentTurn) % 6 == 0 && aShip::TShip_ChanceToWin(this, EnemyShip) < 1.1L && GetHullIntegrityPercent() > 30) {
            aShip::TShip_RequestAlliesAttackShip(this, EnemyShip);
        }
    }

    // Can reduce ship/home-planet relations and adds pirate career activity to the requester.
    void TRanger::ReactToExtortionDemand(void* Ranger) {
        if (aPlayer::GetPlayer() == Ranger || aMyFunction::NextRandomUnitFloat(RandomState) < 0.05L) {
            ChangeRelationToRanger(Ranger, -15);
            HomePlanet->ChangeRelationToRanger(Ranger, -5);
            pas::checked_cast<TRanger*>(static_cast<pas::Object*>(Ranger))->AddPirateCareerActivity(2);
        }
    }

    // May execute payment and truce, or open the player's response dialogue; not a text-only query.
    std::uint8_t TRanger_BuildMoneyExtortionResponse(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        float LicenseFactor{};
        // Caller-popped static link; paying ranger -4, requester -8, amount +8.
        auto AcceptMoneyDemand = [&]() -> void {
            aGalaxyEvent::TGalaxyEvent* Event{};
            Self->AddTraderCareerActivity(4);
            Self->AbductedByPirateClan = false;
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->LastPlayerExtortionTurn = aGalaxy::Galaxy->CurrentTurn;
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerExtortsMoney"_w, nullptr);
                Event->AddData(DemandedAmount);
                Event->AddData(Self->TypeId);
                Event->AddData(Self->CurrentStar->Id);
                Event->AddData(Self->Id);
                Event->AddData(Self->OwnerId);
                Event->AddTextData(Self->GetName());
                Event->AddTextData(Self->TypeNameOverrideKey);
                if (aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                    aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + System::Round(DemandedAmount * 0.9L));
                    aPlayer::GetPlayer()->PendingPirateLicenseCash += System::Round(DemandedAmount * 0.1L);
                    if (aPlayer::GetPlayer()->PendingPirateLicenseCash > 100000000) {
                        aPlayer::GetPlayer()->PendingPirateLicenseCash = 100000000;
                    }
                } else {
                    aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + DemandedAmount);
                }
            } else {
                OtherShip->SetMoney(OtherShip->Money + DemandedAmount);
            }
            Self->SetMoney(Self->Money - DemandedAmount);
            aShip::TShip_TruceWithShip(OtherShip, Self);
            if (pas::class_cast_if<TRanger*>(OtherShip) != nullptr) {
                aRanger::TRanger_ApplyExtortionReputationPenalty(pas::checked_cast<TRanger*>(OtherShip), Self);
            }
            if (OtherShip->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->AddPirateRankPoints(2u);
            }
        };
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() == OtherShip && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
            LicenseFactor = 1.15f;
        } else {
            LicenseFactor = 1.0f;
        }
        std::int32_t NextDemandTurn = Self->LastPlayerExtortionTurn + 30;
        if (pas::class_cast_if<TRanger*>(OtherShip) != nullptr) {
            Self->ReactToExtortionDemand(OtherShip);
        }
        if ((aPlayer::GetPlayer() != OtherShip || PlayerAutomaticControl != false) && (Self->EnemyShip == nullptr || Self->CurrentStar != Self->EnemyShip->CurrentStar)) {
            Self->EnemyShip = OtherShip;
        }
        if (aPlayer::GetPlayer() == Self && static_cast<std::uint8_t>(PlayerAutomaticControl ^ 1)) {
            if (([&] {
                std::int32_t demandedAmount = DemandedAmount;
                const pas::WideString& formatText1 = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(DemandedAmount);
                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(OtherShip, u"Talk.Money.Send"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                }());
                aShip::TShip* otherShip = OtherShip;
                return aShip::TShip_ShowPlayerDialogue(otherShip, aGalaxyStruct::tkMoneyDemand, formatText1, demandedAmount);
            }()) != 0) {
                AcceptMoneyDemand();
                Result = true;
                aGalaxy::PlayerStar->InterruptLongTravel = true;
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, OtherShip, nullptr, 0);
            }
            return Result;
        } else if (OtherShip->TruceShip == Self) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && Self->PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (!Self->virtual_TShip_AcceptsRansomDemandFrom(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (Self->CanEscapePursuer(OtherShip)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"LongDistance"}), OtherShip);
            return Result;
        } else {
            double cpp_arg = static_cast<long double>(aShip::TShip_GetWinChancePercent(Self, OtherShip) & 0x0000007f) * LicenseFactor;
            double wealthScaledAmount = Self->GetWealthScaledAmount(4);
            double wealthScaledAmount_2 = Self->GetWealthScaledAmount(2);
            pas::Extended cpp_right = aMyFunction::RemapClamped(cpp_arg, 0.0, 1.0E+2, wealthScaledAmount, wealthScaledAmount_2);
            if (DemandedAmount > cpp_right) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"SumIsVeryBig"}), OtherShip);
                return Result;
            } else if (Self->Money < DemandedAmount) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Money.AnswerNotMoney"_wref.get(), OtherShip);
                return Result;
            } else {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Money.", Self->GetTypeNameKey(), u"Ok"}), OtherShip);
                AcceptMoneyDemand();
                return true;
            }
        }
    }

    // Successful requests jettison cargo and establish a truce.
    std::uint8_t TRanger_BuildCargoExtortionResponse(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        // Caller-popped static link; paying ranger -4, requester -8. Drops goods and can set the player pact flag.
        auto AcceptCargoDemand = [&]() -> void {
            std::uint8_t Good{};
            std::int32_t Pass{};
            std::int32_t Count{};
            float Divisor{};
            aGalaxyEvent::TGalaxyEvent* Event{};
            Self->AddTraderCareerActivity(4);
            Self->AbductedByPirateClan = false;
            std::int32_t TotalValue = 0;
            std::uint8_t Enough = false;
            std::int32_t LowValue = Self->GetWealthScaledAmount(1);
            std::int32_t HighValue = Self->GetWealthScaledAmount(4);
            for (Pass = 1; Pass <= 3; ++Pass) {
                for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                    if (Self->CargoGoods[Good].Count > 0) {
                        Divisor = aMyFunction::RemapClamped(Self->CargoGoods[Good].Count * aConst::GoodsMarket[Good].AveragePrice, LowValue, HighValue, 2.0, 8.0);
                        Count = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(pas::real_divide(Self->CargoGoods[Good].Count, Divisor)));
                        TotalValue += Count * aConst::GoodsMarket[Good].AveragePrice;
                        aShip::TShip_DropGoodsIntoSpace(Self, Good, Count);
                        if (TotalValue > HighValue) {
                            Enough = true;
                            break;
                        }
                    }
                }
                if (Enough) {
                    break;
                }
            }
            aShip::TShip_TruceWithShip(OtherShip, Self);
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->LastPlayerExtortionTurn = aGalaxy::Galaxy->CurrentTurn;
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerExtortsGoods"_w, nullptr);
                Event->AddData(TotalValue);
                Event->AddData(Self->TypeId);
                Event->AddData(Self->CurrentStar->Id);
                Event->AddData(Self->Id);
                Event->AddData(Self->OwnerId);
                Event->AddTextData(Self->GetName());
                Event->AddTextData(Self->TypeNameOverrideKey);
            }
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->PlayerExtortionPactActive = true;
            }
            OtherShip->OrderMove(Self->Position, true);
            if (pas::class_cast_if<TRanger*>(OtherShip) != nullptr) {
                aRanger::TRanger_ApplyExtortionReputationPenalty(pas::checked_cast<TRanger*>(OtherShip), Self);
            }
            if (OtherShip->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                reinterpret_cast<aNormalShip::TNormalShip*>(OtherShip)->AddPirateRankPoints(2u);
            }
        };
        std::uint8_t Result = false;
        if (pas::class_cast_if<TRanger*>(OtherShip) != nullptr) {
            Self->ReactToExtortionDemand(OtherShip);
        }
        if ((aPlayer::GetPlayer() != OtherShip || PlayerAutomaticControl != false) && (Self->EnemyShip == nullptr || Self->CurrentStar != Self->EnemyShip->CurrentStar)) {
            Self->EnemyShip = OtherShip;
        }
        if (aPlayer::GetPlayer() == Self && static_cast<std::uint8_t>(PlayerAutomaticControl ^ 1)) {
            if (([&] {
                const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(OtherShip, u"Talk.Goods.Send"_wref.get());
                aShip::TShip* otherShip = OtherShip;
                return aShip::TShip_ShowPlayerDialogue(otherShip, aGalaxyStruct::tkGoodsDemand, lookupTalkText, 0);
            }()) != 0) {
                AcceptCargoDemand();
                Result = true;
                aGalaxy::PlayerStar->InterruptLongTravel = true;
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, OtherShip, nullptr, 0);
            }
            return Result;
        }
        std::uint8_t Forced = aPlayer::GetPlayer() == OtherShip && OtherShip->IsHealthEffectActive(14);
        std::int32_t NextDemandTurn = Self->LastPlayerExtortionTurn + 30;
        if (OtherShip->TruceShip == Self) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && Self->PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (static_cast<std::uint8_t>(Self->virtual_TShip_AcceptsRansomDemandFrom(OtherShip) ^ 1) && static_cast<std::uint8_t>(Forced ^ 1)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        } else if (Self->CanEscapePursuer(OtherShip) && static_cast<std::uint8_t>(Forced ^ 1)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"LongDistance"}), OtherShip);
            return Result;
        } else if (!Self->HasCargoGoods()) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Goods.AnswerNotGoods"_wref.get(), OtherShip);
            return Result;
        } else {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Goods.", Self->GetTypeNameKey(), u"Ok"}), OtherShip);
            AcceptCargoDemand();
            if (aPlayer::GetPlayer() == OtherShip) {
                Self->PlayerExtortionPactActive = true;
            }
            return true;
        }
    }

    // Acceptance transfers OfferedAmount from OtherShip to Self and establishes a truce.
    std::uint8_t TRanger::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) {
        pas::WideString Text{};
        // Caller-popped static link; payer -4, receiving ranger -8, amount +8.
        auto AcceptTrucePayment = [&]() -> void {
            aGalaxyEvent::TGalaxyEvent* Event{};
            OtherShip->SetMoney(OtherShip->Money - OfferedAmount);
            if (aPlayer::GetPlayer() == this) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerAcceptsMoneyForTruce"_w, nullptr);
                Event->AddData(OfferedAmount);
                Event->AddData(OtherShip->TypeId);
                Event->AddData(OtherShip->CurrentStar->Id);
                Event->AddData(OtherShip->Id);
                Event->AddData(OtherShip->OwnerId);
                Event->AddTextData(OtherShip->GetName());
                Event->AddTextData(OtherShip->TypeNameOverrideKey);
                if (aPlayer::GetPlayer()->PirateLicenseTicks > 0 && OtherShip->TypeId != aGalaxyStruct::stPirate && (OtherShip->TypeId != aGalaxyStruct::stRanger || OtherShip->GetDominantCareer() != aGalaxyStruct::rcPirate)) {
                    SetMoney(this->Money + System::Round(OfferedAmount * 0.9L));
                    aPlayer::GetPlayer()->PendingPirateLicenseCash += System::Round(OfferedAmount * 0.1L);
                    if (aPlayer::GetPlayer()->PendingPirateLicenseCash > 100000000) {
                        aPlayer::GetPlayer()->PendingPirateLicenseCash = 100000000;
                    }
                } else {
                    SetMoney(this->Money + OfferedAmount);
                }
            } else {
                SetMoney(this->Money + OfferedAmount);
            }
            if (this->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::class_cast_if<TRanger*>(OtherShip) != nullptr) {
                AddPirateRankPoints(2u);
            }
            if (this->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::class_cast_if<aTransport::TTransport*>(OtherShip) != nullptr) {
                AddPirateRankPoints(1u);
            }
            aShip::TShip_TruceWithShip(this, OtherShip);
        };
        std::uint8_t Result = false;
        std::int32_t NextDemandTurn = LastPlayerExtortionTurn + 30;
        if (pas::class_cast_if<TRanger*>(OtherShip) != nullptr) {
            pas::checked_cast<TRanger*>(OtherShip)->AddTraderCareerActivity(1);
        }
        if (aPlayer::GetPlayer() == this && static_cast<std::uint8_t>(PlayerAutomaticControl ^ 1)) {
            if (static_cast<std::uint32_t>(aGalaxy::ReservedMessageCounter) < 7) {
                return Result;
            }
            Text = aShip::TShip_LookupTalkText(OtherShip, pas::concat_wide({u"Talk.Truce.", OtherShip->GetTypeNameKey(), u"Send"}));
            if (([&] {
                const pas::WideString& formatText1 = aMyFunction::FormatText1(Text, u"<color=255,240,100>"_w, u"<Money>"_w, pas::wide_int_to_str(OfferedAmount));
                aShip::TShip* otherShip = OtherShip;
                return aShip::TShip_ShowPlayerDialogue(otherShip, aGalaxyStruct::tkTruceOffer, formatText1, 0);
            }()) != 0) {
                AcceptTrucePayment();
                Result = true;
                aGalaxy::PlayerStar->InterruptLongTravel = true;
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, OtherShip, nullptr, 0);
            }
            return Result;
        } else if (OtherShip->TruceShip == this) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && PlayerExtortionPactActive) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (aPlayer::GetPlayer() == OtherShip && aGalaxy::Galaxy->CurrentTurn < NextDemandTurn) {
            Response = aShip::TShip_LookupVisibleTalkText(this, u"Talk.Truce.WeAlreadyHavePact"_wref.get(), OtherShip);
            return Result;
        } else if (this->virtual_TShip_RecomputeFearState() || aShip::TShip_ChanceToWin(this, OtherShip) < 1.0L && GetHullIntegrityPercent() < 40 || aShip::TShip_ChanceToWin(this, OtherShip) < 0.25L || ([&] {
            double cpp_arg = aShip::TShip_GetWinChancePercent(this, OtherShip) & 0x0000007f;
            double wealthScaledAmount = GetWealthScaledAmount(1);
            double wealthScaledAmount_2 = GetWealthScaledAmount(3);
            pas::Extended cpp_right = aMyFunction::RemapClamped(cpp_arg, 0.0, 1.0E+2, wealthScaledAmount, wealthScaledAmount_2);
            return OfferedAmount > cpp_right;
        }())) {
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Ok"}), OtherShip);
            AcceptTrucePayment();
            return true;
        } else {
            Response = aShip::TShip_LookupVisibleTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"No"}), OtherShip);
            return Result;
        }
    }

    // May change relations/career activity even on refusal; acceptance issues a joint attack.
    std::uint8_t TRanger_BuildAttackRequestResponse(TRanger* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        std::uint8_t Result{};
        // Caller-popped static link; ranger -4, requester -8, text output -12, Boolean output -13, target +8.
        auto AcceptAttackRequest = [&]() -> void {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Ok"}), Requester);
            Self->SetJointAttackTarget(Requester, Target);
            Result = true;
        };
        Result = false;
        if (pas::class_cast_if<TRanger*>(Requester) != nullptr) {
            if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                Target->ChangeRelationToRanger(Requester, -20);
            }
            if (Target->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || Target->TypeId == aGalaxyStruct::stPirate) {
                pas::checked_cast<TRanger*>(Requester)->AddWarriorCareerActivity(1);
            } else {
                pas::checked_cast<TRanger*>(Requester)->AddPirateCareerActivity(8);
            }
        }
        if (aPlayer::GetPlayer() == Self && static_cast<std::uint8_t>(PlayerAutomaticControl ^ 1)) {
            if (([&] {
                const pas::WideString& formatText1 = ([&] {
                    pas::WideString cpp_arg = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Target, false), Target->GetName()});
                    pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(Requester, pas::concat_wide({u"Talk.Attack.", Requester->GetTypeNameKey(), u"Send"}));
                    return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(cpp_arg));
                }());
                aShip::TShip* requester = Requester;
                return aShip::TShip_ShowPlayerDialogue(requester, aGalaxyStruct::tkAttack, formatText1, 0);
            }()) != 0) {
                Self->SetJointAttackTarget(Requester, Target);
                Result = true;
                aGalaxy::PlayerStar->InterruptLongTravel = true;
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, Requester, nullptr, 0);
            }
        } else if (Self->PartnerShip == Requester || Self->OrderTarget == Target && aShip::TShip_GetRelationLevelToShip(Self, Target) == aGalaxyStruct::rlHostile) {
            AcceptAttackRequest();
        } else if (Self->TruceShip == Target) {
            Response = ([&] {
                pas::WideString name = Target->GetName();
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Attack.WeAlreadyHavePact"_wref.get(), Requester);
                return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Target>"_w, std::move(name));
            }());
        } else if ((aShip::TShip_RelationToShip(Self, Target) >= 80 || aShip::TShip_RelationToShip(Self, Target) >= 30 && Self->PreferredCareer != aGalaxyStruct::rcPirate) && Self->PartnerShip != Requester) {
            if (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Target) != nullptr)) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriends"}), Requester);
            } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Target)->OwnerShip == Self) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsMyTranc"}), Requester);
            } else if (reinterpret_cast<aTranclucator::TTranclucator*>(Target)->OwnerShip == Requester) {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"ItsYourTranc"}), Requester);
            } else {
                Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"WeFriendsTranc"}), Requester);
            }
        } else if (Self->virtual_TShip_AcceptsRansomDemandFrom(Target) || Self->InFear) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Fear"}), Requester);
        } else if (!Self->virtual_TShip_TrustsAttackRequester(Requester)) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"Suspect"}), Requester);
        } else if (Self->HasLockedOrFollowOrder() && Self->PartnerShip != Requester) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, pas::concat_wide({u"Talk.Attack.", Self->GetTypeNameKey(), u"HaveBusiness"}), Requester);
        } else {
            AcceptAttackRequest();
        }
        return Result;
    }

    // Checks eligibility and formats refusal text; OtherShip must be a ranger. Success does not clear preexisting Response.
    std::uint8_t TRanger_BuildPartnershipOfferResponse(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        if (aShip::TShip_RelationToShip(Self, OtherShip) < 45) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.Suspect"_wref.get(), OtherShip);
        } else if (Self->PartnerShip != nullptr) {
            Response = ([&] {
                auto name = pas::borrow(pas::checked_cast<TRanger*>(Self->PartnerShip)->Name);
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.AlreadyHavePartner"_wref.get(), OtherShip);
                return aMyFunction::FormatText1(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Partner>"_w, name.get());
            }());
        } else if (Self->CountWingmen() > 0) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.ILeader"_wref.get(), OtherShip);
        } else if (pas::class_cast_if<TRanger*>(OtherShip) != nullptr && ([&] {
            std::int32_t cpp_left = OtherShip->GetEffectiveSkillLevel(aShip::psLeadership, false);
            return cpp_left <= static_cast<TRanger*>(OtherShip)->CountWingmen();
        }())) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.NeedLeadership"_wref.get(), OtherShip);
        } else if (pas::class_cast_if<aNormalShip::TNormalShip*>(OtherShip) != nullptr && static_cast<aNormalShip::TNormalShip*>(OtherShip)->Rank < Self->Rank) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.YouNeedInMoreRank"_wref.get(), OtherShip);
        } else if (Self->CalculatePartnershipMonths(PaymentAmount, OtherShip) == 0) {
            Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.SmallMoney"_wref.get(), OtherShip);
        } else {
            Result = true;
        }
        Response = aMyFunction::FormatText1(Response, u"<color=255,240,100>"_w, u"<Ranger>"_w, pas::checked_cast<TRanger*>(OtherShip)->Name);
        return Result;
    }

    // Calls the eligibility method, then sets PartnerShip/duration and transfers payment. Requires a ranger requester.
    std::uint8_t TRanger_AcceptPartnershipOffer(TRanger* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result{};
        if (Self->virtual_TShip_BuildPartnershipOfferResponse(OtherShip, Response, PaymentAmount)) {
            Self->PartnershipDaysRemaining = 30 * Self->CalculatePartnershipMonths(PaymentAmount, OtherShip);
            Response = ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Self->CalculatePartnershipMonths(PaymentAmount, OtherShip));
                auto name = pas::borrow(pas::checked_cast<TRanger*>(OtherShip)->Name);
                pas::WideString lookupVisibleTalkText = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Partner.Ok"_wref.get(), OtherShip);
                return aMyFunction::FormatText2(std::move(lookupVisibleTalkText), u"<color=255,240,100>"_w, u"<Month>"_w, std::move(intToStr), u"<Ranger>"_w, name.get());
            }());
            Self->PartnerShip = OtherShip;
            Self->OrderAbsolute = false;
            Result = true;
            Self->SetMoney(Self->Money + PaymentAmount);
            OtherShip->SetMoney(OtherShip->Money - PaymentAmount);
            if (aPlayer::GetPlayer() == OtherShip) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckMasterAchievement();
            }
            return Result;
        }
        return false;
    }

    pas::WideString TRanger::GetProgramName(std::uint8_t ProgramIndex) {
        return aConst::LocalizedText(pas::concat_wide({u"Programms.", aConst::ProgramNames[ProgramIndex], u".Name"}));
    }

    pas::WideString TRanger::GetProgramInfoText(std::uint8_t ProgramIndex) {
        pas::WideString intToStr = pas::wide_int_to_str(ProgramCounts[ProgramIndex]);
        pas::WideString localizedText = aConst::LocalizedText(pas::concat_wide({u"Programms.", aConst::ProgramNames[ProgramIndex], u".Text"}));
        return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr));
    }

    // Does not mask or validate ProgramIndex.
    std::uint8_t TRanger::HasProgram(std::uint8_t ProgramIndex) {
        return ProgramCounts[ProgramIndex] > 0;
    }

    // Sums owned quantities for bits 0..11; higher bits are ignored. Native signed 32-bit additions wrap on overflow.
    std::int32_t TRanger::CountProgramsInFilter(TRangerProgramMask Filter) {
        std::uint8_t I{};
        std::int32_t Result = 0;
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(11); ++I) {
            if (pas::contains(Filter, I)) {
                Result += ProgramCounts[I];
            }
        }
        return Result;
    }

    // Selects an allowed ID regardless of inventory counts; deterministic system/turn seed. Empty filter returns zero after 10000 attempts.
    std::uint8_t TRanger::SelectRandomProgramIdFromFilter(TRangerProgramMask Filter) {
        std::uint8_t ProgramId{};
        std::int32_t Attempt = 0;
        std::uint8_t Result = 0;
        while (true) {
            ++Attempt;
            if (Attempt > 10000) {
                break;
            }
            ProgramId = aMyFunction::SeededRandomIntRange(0, 11, CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 65) + Attempt);
            if (pas::contains(Filter, ProgramId)) {
                Result = ProgramId;
                break;
            }
        }
        return Result;
    }

    // Favors program 5 until enough copies exist; otherwise selects among IDs 6..11.
    std::uint8_t TRanger::SelectProgramReward() {
        static const pas::Set<0, 255> BasicProgram = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::prgIntercom}});
        static const pas::Set<0, 255> OtherPrograms = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::prgShipwreck, aGalaxyStruct::prgDisconnection}});
        if (CountProgramsInFilter(static_cast<TRangerProgramMask>(BasicProgram)) == 0 || ([&] {
            std::int32_t cpp_left = CountProgramsInFilter(static_cast<TRangerProgramMask>(BasicProgram));
            return cpp_left < aMyFunction::RandomIntRange(3, 10);
        }())) {
            return aGalaxyStruct::prgIntercom;
        }
        return SelectRandomProgramIdFromFilter(static_cast<TRangerProgramMask>(OtherPrograms));
    }

    // At least one; uses galaxy seed, turn and difficulty.
    std::int32_t TRanger::GetProgramRewardCount(std::uint8_t ProgramIndex) {
        std::int32_t Result{};
        if (ProgramIndex == aGalaxyStruct::prgIntercom) {
            pas::Extended cpp_left = aMyFunction::SeededRandomIntRange(12, 20, aGalaxy::Galaxy->GenerationSeed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 35));
            Result = System::Round(pas::real_divide(cpp_left, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestTimeAndExperienceFactor));
        } else {
            pas::Extended cpp_left_2 = aMyFunction::SeededRandomIntRange(1, 3, aGalaxy::Galaxy->GenerationSeed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 35));
            Result = System::Round(pas::real_divide(cpp_left_2, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor));
        }
        return std::max<std::int32_t>(1, Result);
    }

    float TRanger::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float FragilityScale{};
        std::int32_t Price{};
        float DesiredMoneyFraction{};
        float HullValueScale{};
        switch (PreferredCareer) {
            case aGalaxyStruct::rcWarrior: {
                DesiredMoneyFraction = 0.05f;
                FragilityScale = 1.2f;
                HullValueScale = 1.5f;
                break;
            }
            case aGalaxyStruct::rcPirate: {
                DesiredMoneyFraction = 0.07f;
                FragilityScale = 1.0f;
                HullValueScale = 1.0f;
                break;
            }
            case aGalaxyStruct::rcTrader: {
                DesiredMoneyFraction = 0.1f;
                FragilityScale = 0.8f;
                HullValueScale = 0.5f;
                break;
            }
            default: {
                DesiredMoneyFraction = 0.1f;
                FragilityScale = 1.0f;
                HullValueScale = 1.0f;
                break;
            }
        }
        if (pas::is_one_of<aConst::t_FuelTanks, aConst::t_Radar, aConst::t_Scaner>(Item->ItemType)) {
            FragilityScale = FragilityScale * 0.5L;
        }
        float DesiredFreeFraction = pas::real_max<pas::Extended>(0.01L, pas::real_min<pas::Extended>(0.99L, pas::real_divide(GetDesiredCargoFreeSpace(), std::max<std::int32_t>(100, GetHull()->Weight))));
        pas::Extended cpp_left = pas::sqr(([&] {
            pas::Extended cpp_left_2 = pas::real_divide(1.0L, pas::real_max<float>(0.01f, SmoothedMoneyFraction)) - 1.0L;
            return pas::real_divide(cpp_left_2, pas::real_divide(1.0L, DesiredMoneyFraction) - 1.0L);
        }()));
        float MoneyPenalty = pas::real_divide(cpp_left, pas::real_max<pas::Extended>(SmoothedWealth * 0.05L, 1.0E+3L));
        float EffectivenessScale = pas::real_divide(2.0L, pas::real_max<float>(1.0E+1f, SmoothedEquipmentEffectiveness));
        pas::Extended cpp_left_3 = pas::sqr(([&] {
            pas::Extended cpp_left_4 = pas::real_divide(1.0L, pas::real_max<float>(0.01f, SmoothedFreeCapacityFraction)) - 1.0L;
            return pas::real_divide(cpp_left_4, pas::real_divide(1.0L, DesiredFreeFraction) - 1.0L);
        }()));
        float WeightPenalty = pas::real_divide(cpp_left_3, pas::real_max<pas::Extended>(1.0E+1L, GetHull()->Weight * 0.1L));
        MoneyPenalty = MoneyPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Id + Seed));
        EffectivenessScale = EffectivenessScale * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Id * 3 + Seed));
        WeightPenalty = WeightPenalty * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Id * 5 + Seed));
        switch (PriceMode) {
            case 4: {
                Price = Item->Cost;
                MoneyPenalty = static_cast<long double>(MoneyPenalty) * aMyFunction::RemapClamped(EquipmentPriceSensitivity, 0.0, 1.0, 0.2, 1.0);
                break;
            }
            case 3: {
                Price = Item->CalculateResaleValue(GetEffectiveSkillLevel(aShip::psTrading, false));
                MoneyPenalty = static_cast<long double>(MoneyPenalty) * aMyFunction::RemapClamped(EquipmentPriceSensitivity, 0.0, 1.0, 0.2, 1.0);
                break;
            }
            case 1: Price = -Item->Cost; break;
            case 2: Price = 0; break;
            case 0: {
                Price = 0;
                WeightPenalty = 0.0f;
                FragilityScale = 0.0f;
                break;
            }
            default: {
                Price = 0;
                WeightPenalty = 0.0f;
                break;
            }
        }
        if (Item->ItemType != aConst::t_Hull) {
            return static_cast<long double>(Effectiveness) * EffectivenessScale * (1.0L + (2.0L - reinterpret_cast<aItem::TEquipment*>(Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))) * FragilityScale) - static_cast<long double>(Item->Weight) * WeightPenalty - static_cast<long double>(Price) * MoneyPenalty;
        }
        return (pas::real_divide(static_cast<long double>(Item->Weight) * HullValueScale, pas::real_max<float>(0.01f, reinterpret_cast<aItem::TEquipment*>(Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags)))) + Effectiveness) * EffectivenessScale + static_cast<long double>(Item->Weight) * WeightPenalty - Price * 0.25L * MoneyPenalty;
    }

    float TRanger::EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float Result = 0.0f;
        if (Value == 0) {
            return Result;
        }
        {
            aConst::TEquipmentBonusKind cpp_case = BonusKind;
            if (cpp_case == aConst::bonHull) {
                Result = Value * 200;
            } else if (cpp_case == aConst::bonFuel) {
                Result = Value * 2.5L;
            } else if (cpp_case == aConst::bonSpeed) {
                Result = Value;
            } else if (cpp_case == aConst::bonJump) {
                Result = Value * 25;
            } else if (cpp_case == aConst::bonRadar) {
                Result = Value * 0.063L;
            } else if (cpp_case == aConst::bonScan) {
                Result = Value * 5 + Value * 20 * (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f);
            } else if (cpp_case == aConst::bonDroid) {
                Result = pas::real_divide(Value * 10, pas::real_max<float>(0.1f, GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))));
            } else if (cpp_case == aConst::bonHook) {
                Result = (pas::real_min<pas::Extended>(static_cast<pas::Extended>(Value), static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]) + Value * 0.1L) * 1.5L;
            } else if (cpp_case == aConst::bonDef) {
                Result = pas::real_divide(pas::real_divide(Value * 5 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                Result = Value * 10;
            } else if (cpp_case == aConst::bonWSplinter) {
                Result = Value * 10;
            } else if (cpp_case == aConst::bonWMissile) {
                Result = Value * 10 * (0.1L + static_cast<std::int8_t>(GetRadarRange() > 0) * 0.9L);
            } else if (cpp_case == aConst::bonWRadius) {
                Result = Value * pas::sqr(pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed), std::max<std::int32_t>(100, SmoothedSpeed)));
            } else if (cpp_case == aConst::bonHookRadius) {
                Result = Value * 0.15L;
            } else if (cpp_case == aConst::bonMass) {
                Result = aMyFunction::RemapClamped(Value + GetHull()->Weight * 0.1L, aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333) * 5.0E+3L;
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.3L;
                } else if (GetRadar() != nullptr && Value < 0) {
                    Result = -RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] - RangerSlotBonusEvaluationWeights[PreferredCareer][18] * (CountMissileWeapons() & 0x0000007f);
                } else if (GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotScaner) {
                if (GetSlotCount(aConst::sskScanner) == 0 && Value > 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.3L;
                } else if (GetScanner() != nullptr && Value < 0) {
                    Result = -RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] - (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f) * 0.1L * RangerSlotBonusEvaluationWeights[PreferredCareer][18];
                } else if (GetSlotCount(aConst::sskScanner) == 1 && Value < 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.3L;
                } else if (GetRepairRobot() != nullptr && Value < 0) {
                    Result = -RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                } else if (GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotHook) {
                if (GetSlotCount(aConst::sskCargoHook) == 0 && Value > 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.3L;
                } else if (GetCargoHook() != nullptr && Value < 0) {
                    Result = -RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                } else if (GetSlotCount(aConst::sskCargoHook) == 1 && Value < 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.3L;
                } else if (GetDefGenerator() != nullptr && Value < 0) {
                    Result = -RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                } else if (GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * -0.3L;
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - GetSlotCount(aConst::sskWeapon)) * RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskWeapon)) * RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
                {
                    std::int32_t cpp_right = std::max<std::int32_t>(Value + GetSlotCount(aConst::sskWeapon), 1);
                    if ((CountEquippedWeapons() & 0x0000007f) > cpp_right) {
                        std::int32_t cpp_right_2 = std::max<std::int32_t>(1, Value + GetSlotCount(aConst::sskWeapon));
                        Result = Result - RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.6L * ((CountEquippedWeapons() & 0x0000007f) - cpp_right_2);
                    }
                }
            } else if (cpp_case == aConst::bonSlotArt) {
                if (GetSlotCount(aConst::sskArtefact) < aConst::DefaultHullSlotCounts[8] && Value > 0) {
                    Result = std::min<std::int32_t>(Value, aConst::DefaultHullSlotCounts[8] - GetSlotCount(aConst::sskArtefact)) * RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskArtefact)) * RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
                if (Artefacts != nullptr) {
                    if (pas::list_count(Artefacts) > std::max<std::int32_t>(Value + GetSlotCount(aConst::sskArtefact), 0)) {
                        Result = -1.0E+3f;
                    }
                }
            } else if (cpp_case == aConst::bonSlotForsage) {
                if (GetSlotCount(aConst::sskAfterburner) == 0 && Value > 0) {
                    Result = RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                } else if (GetSlotCount(aConst::sskAfterburner) == 1 && Value < 0) {
                    Result = -RangerSlotBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(6 - (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f), Value) * RangerSkillBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
                if (Value > 0 && Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) > 6) {
                    Result = Result + RangerSkillBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.05L * (Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) - 6);
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f, -Value) * -RangerSkillBonusEvaluationWeights[PreferredCareer][BonusKind];
                }
                if (Value < 0 && Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) < 0) {
                    Result = Result + RangerSkillBonusEvaluationWeights[PreferredCareer][BonusKind] * 0.03L * (Value + (GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f));
                }
            } else {
                Result = 0.0f;
            }
        }
        if (PreferredCareer == aGalaxyStruct::rcWarrior && pas::is_one_of<aConst::bonHull, aConst::bonRadar, aConst::bonDroid, aConst::bonDef, aConst::bonWRadius>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (PreferredCareer == aGalaxyStruct::rcPirate && pas::is_one_of<aConst::bonSpeed, aConst::bonWEnergy, aConst::bonWSplinter, aConst::bonMass>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (PreferredCareer == aGalaxyStruct::rcTrader && pas::is_one_of<aConst::bonFuel, aConst::bonJump>(BonusKind)) {
            Result = Result * 1.3L;
        }
        if (pas::in_range(BonusKind, static_cast<std::int32_t>(aConst::bonSkill1), static_cast<std::int32_t>(aConst::bonSkill6))) {
            pas::Extended cpp_left = Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-60, 60, BonusKind * 131 + Seed));
            return cpp_left * aConst::RaceSkillEvaluationFactors[PilotRace][aConst::EquipmentBonusSkills[BonusKind - 22]];
        }
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-15, 15, BonusKind * 131 + Seed));
    }

    float TRanger::EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> ShockFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkShock}});
        static const pas::Set<0, 255> AcidFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkAcid}});
        float ScannerFactor{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        std::int32_t I{};
        std::int32_t ShotTotal{};
        Flags = Weapon->GetDamageFlags();
        if (Flags * static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags) != pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}) && GetScanner() != nullptr && GetRadar() != nullptr) {
            std::int32_t cpp_right = aItem::DefenseDamageFactorToPercent(aItem::GetGeneratedDefenseDamageFactor(aGalaxy::Galaxy->TechLevel)) & 0x0000007f;
            ScannerFactor = aMyFunction::RemapClamped(GetScannerPower() - cpp_right + 1, -5.0, 1.0E+1, 0.1, 2.0);
        } else {
            ScannerFactor = 0.0f;
        }
        float Result = static_cast<long double>(BaseDamage) * GetWeaponArtefactDamageFactor(Weapon);
        if (pas::contains(Flags, aGalaxyStruct::dkDrain)) {
            Result = Result * 1.5L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            Result = Result * (1.15L + (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) & 0x0000007f) * 0.07L - static_cast<std::int8_t>(PreferredCareer == aGalaxyStruct::rcWarrior) * 0.05L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
            Result = Result * (1.25L - static_cast<std::int8_t>(PreferredCareer == aGalaxyStruct::rcWarrior) * 0.05L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkMagnetic)) {
            Result = Result * (1.25L - static_cast<std::int8_t>(PreferredCareer == aGalaxyStruct::rcWarrior) * 0.05L);
        }
        if (PreferredCareer == aGalaxyStruct::rcTrader && pas::contains(Flags, aGalaxyStruct::dkDestruct)) {
            Result = Result * 1.05L;
        }
        float StatusFactor = 1.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkScanBonus)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkBonusToDamaged)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
            Result = Result * (1.0L + ScannerFactor * (0.1L + static_cast<std::int8_t>(PreferredCareer == aGalaxyStruct::rcTrader) * 0.1L));
        }
        StatusFactor = StatusFactor - 1.0L;
        if (IncludeAdditiveBonuses) {
            if (PreferredCareer == aGalaxyStruct::rcPirate && CountActiveArtefacts(aConst::t_ArtDecelerate) > 0 && pas::contains(Flags, aGalaxyStruct::dkSplinter)) {
                std::int32_t cpp_left = 2 + 2 * (CanBoostArtefact(aConst::t_ArtDecelerate, Weapon, false) & 127 & 0x0000007f);
                Result = static_cast<long double>(Result) + cpp_left * CountActiveArtefacts(aConst::t_ArtDecelerate);
            }
            if (PreferredCareer == aGalaxyStruct::rcPirate && pas::contains(Flags, aGalaxyStruct::dkDecelerate)) {
                Result = Result + 2.0L;
            }
            if (PreferredCareer == aGalaxyStruct::rcTrader && pas::contains(Flags, aGalaxyStruct::dkDestruct)) {
                Result = Result + 1.0L;
            }
            {
                std::int32_t cpp_left_2 = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<long double>(Result) + cpp_left_2 * Weapon->GetShotCount();
            }
            if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
                ShotTotal = 1;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
                    ShotTotal += Weapons[I]->GetShotCount();
                }
                Result = static_cast<long double>(Result) + ShotTotal * 3;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkMoreDrop)) {
                Result = Result + static_cast<long double>(ScannerFactor) * (5 + 5 * ((PreferredCareer == aGalaxyStruct::rcPirate) & 0x0000007f));
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDropCargo)) {
                Result = Result + static_cast<long double>(ScannerFactor) * (5 + 5 * ((PreferredCareer == aGalaxyStruct::rcPirate) & 0x0000007f));
            }
            if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
                Result = Result + static_cast<long double>(ScannerFactor) * (1 + ((PreferredCareer == aGalaxyStruct::rcTrader) & 0x0000007f));
            }
            if (pas::contains(Flags, aGalaxyStruct::dkBlockWeapon)) {
                Result = Result + static_cast<long double>(ScannerFactor) * (5 + 5 * ((PreferredCareer == aGalaxyStruct::rcTrader) & 0x0000007f));
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDroidBlock)) {
                Result = Result + ScannerFactor * 5.0L;
            }
        }
        float SpeedFactor = pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed) * GetHull()->Weight, aConst::HullBaseSize * pas::real_max<pas::Extended>(1.0E+2L, static_cast<long double>(SmoothedSpeed) * aConst::EquipmentSizeFactors[1]));
        switch (PreferredCareer) {
            case aGalaxyStruct::rcWarrior: {
                switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
                    case aGalaxyStruct::wstRocket: {
                        Result = Result * 0.8L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstMissile: {
                        Result = Result * (0.8L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                        break;
                    }
                    case aGalaxyStruct::wstTorpedo: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5L * 0.01L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstChain: {
                        Result = Result * (1.1L + (Weapon->GetShotCount() - 1) * 0.2L) * (1.0L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstSplash: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 1.0L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstExploder: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.1L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstAreaDamage: {
                        Result = Result * (1.0L + Weapon->Range * 1.3L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    default: Result = Result * (1.0L + StatusFactor); break;
                }
                break;
            }
            case aGalaxyStruct::rcPirate: {
                switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
                    case aGalaxyStruct::wstRocket: {
                        Result = Result * 0.6L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstMissile: {
                        Result = Result * (0.6L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.1L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                        break;
                    }
                    case aGalaxyStruct::wstTorpedo: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.1L * 0.01L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstChain: {
                        Result = Result * (1.1L + (Weapon->GetShotCount() - 1) * 0.2L) * (1.0L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstSplash: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstAreaDamage: {
                        Result = Result * (1.0L + Weapon->Range * 0.16L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    default: Result = Result * (1.0L + StatusFactor); break;
                }
                break;
            }
            case aGalaxyStruct::rcTrader: {
                switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
                    case aGalaxyStruct::wstRocket: {
                        Result = Result * 1.2L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstMissile: {
                        Result = Result * (1.2L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L + StatusFactor) * Weapon->GetShotCount();
                        break;
                    }
                    case aGalaxyStruct::wstTorpedo: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.2L * 0.01L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstChain: {
                        Result = Result * (1.1L + (Weapon->GetShotCount() - 1) * 0.2L) * (1.0L + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstSplash: {
                        Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.1L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    case aGalaxyStruct::wstAreaDamage: {
                        Result = Result * (1.0L + Weapon->Range * 0.16L * 0.01L * SpeedFactor + StatusFactor);
                        break;
                    }
                    default: Result = Result * (1.0L + StatusFactor); break;
                }
                break;
            }
        }
        Result = static_cast<long double>(Result) * Weapon->GetAttackCount();
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Seed + Weapon->GetWeaponInfo()->TypeHash));
    }

    // Always true.
    std::uint8_t TRanger::AcceptPickupItem(aItem::TItem* Item) {
        return true;
    }

    std::uint8_t TRanger::AcceptPickupDistance(aItem::TItem* Item, double Distance) {
        if (Speed < 1) {
            return false;
        }
        if (Item->ItemType == aConst::t_Protoplasm) {
            return Item->Weight >= 30 || pas::real_divide(Distance, Speed) <= 3.0L;
        }
        return Speed * 1.2L >= Distance || Item->Cost >= static_cast<long double>(aMyFunction::RemapClamped(pas::real_divide(Distance, Speed), 1.0, 1.0E+1, 0.01, 0.05)) * Wealth;
    }

    void TRanger_RefreshCurrentStanding(TRanger* Self) {
        std::uint8_t Owner{};
        std::int32_t StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
            return;
        }
        if (StandingMode == aGalaxyStruct::ssmFixed) {
            return;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer() == Self->PartnerShip) {
            Owner = aPlayer::GetPlayer()->OwnerId;
        } else {
            Owner = Self->OwnerId;
        }
        if (aShip::TShip_IsInPrison(Self)) {
            Self->CurrentStanding = aGalaxyStruct::ssNeutral;
        } else if (Owner != 7 || aGalaxy::Galaxy->PirateWinType == 3) {
            if (Self->CurrentSystemKills.Pirate > 0 || Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                Self->CurrentStanding = aGalaxyStruct::ssCoalitionActive;
            } else {
                Self->CurrentStanding = aGalaxyStruct::ssCoalitionPassive;
            }
        } else if (Self->CurrentSystemKills.Normal > 0 || Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            Self->CurrentStanding = aGalaxyStruct::ssPirateActive;
        } else {
            Self->CurrentStanding = aGalaxyStruct::ssPiratePassive;
        }
    }

    void TRanger::ProcessQuestTimersAndOutcomes() {
        std::int32_t I{};
        PQuest Quest{};
        pas::WideString Text{};
        {
            const std::int32_t cpp_first = pas::list_count(Quests) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    // The neutral index expression preserves DCC32's native argument evaluation order.
                    Quest = pas::list_at<TQuest>(Quests, I + 0);
                    if (aGalaxy::Galaxy->CurrentTurn >= Quest->DeadlineTurn) {
                        if (pas::is_one_of<aGalaxyStruct::qtSendLetter, aGalaxyStruct::qtKillShip, aGalaxyStruct::qtPlanetQuest>(Quest->QuestType) && static_cast<std::uint8_t>(Quest->Successful ^ 1)) {
                            if (Quest->QuestType != aGalaxyStruct::qtPlanetQuest || GlobalsV::CurrentScreenId != GlobalsV::screenPlanetQuest || !(pas::class_cast_if<aPlanet::TPlanet*>(Quest->ObjectiveTarget) != nullptr) || aPlayer::GetPlayer()->CurrentPlanet != pas::checked_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget)) {
                                TRanger::PublishQuestStatus(Quest, -1);
                                if (Quest->Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) > aGalaxyStruct::rlBad) {
                                    if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlNormal) {
                                        aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlBad);
                                    }
                                    if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlGood) {
                                        aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlNormal);
                                    }
                                    if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) >= aGalaxyStruct::rlExcellent) {
                                        aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlGood);
                                    }
                                }
                                if (Quest->Planet->GetRelationLevelToShip(this) > aGalaxyStruct::rlBad) {
                                    Quest->Planet->SetRelationLevelToRanger(this, aGalaxyStruct::rlBad);
                                }
                                Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Failure.Time"_wref.get(), Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                aMyFunction::ReplaceTextToken(Text, u"<Quest>"_w, Quest->Description, u"<color=255,240,100>"_w);
                                aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Quest->Planet->Name, u"<color=255,240,100>"_w);
                                aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, Quest->Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                                aMyFunction::ReplaceTextToken(Text, u"<Relation>"_w, Quest->Planet->GetRelationLevelTextToShip(this), u"<color=255,240,100>"_w);
                                if (Quest->QuestType == aGalaxyStruct::qtSendLetter) {
                                    Achievements::TryAddAchievementProgress(u"POSTMAN"_w, 1);
                                }
                                Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                                CheckQuestFailureAward(Quest, pas::constant_set<aGalaxyStruct::TQuestTypes>({{aGalaxyStruct::qtSendLetter}, {aGalaxyStruct::qtKillShip}, {aGalaxyStruct::qtPlanetQuest}}));
                                ArchiveQuest(I);
                            }
                        } else if (pas::is_one_of<aGalaxyStruct::qtDefendSystem, aGalaxyStruct::qtDefendShip>(Quest->QuestType) && static_cast<std::uint8_t>(Quest->Successful ^ 1)) {
                            switch (Quest->QuestType) {
                                case aGalaxyStruct::qtDefendSystem: {
                                    if (Quest->Planet != nullptr) {
                                        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Successful.DefSystem"_wref.get(), aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                        aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, Quest->Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                                    } else {
                                        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Successful.DefSystemRuins"_wref.get(), aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                        aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, pas::checked_cast<aGalaxy::TStar*>(Quest->ObjectiveTarget)->Name, u"<color=255,240,100>"_w);
                                    }
                                    break;
                                }
                                case aGalaxyStruct::qtDefendShip: {
                                    if (Quest->Planet != nullptr) {
                                        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Successful.DefShip"_wref.get(), aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                    } else {
                                        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Successful.DefShipRuins"_wref.get(), aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                    }
                                    aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, pas::checked_cast<aShip::TShip*>(Quest->ObjectiveTarget)->GetName(), u"<color=255,240,100>"_w);
                                    break;
                                }
                            }
                            aMyFunction::ReplaceTextToken(Text, u"<Player>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                            // Native code still dereferences Planet after the nil-planet text branches.
                            aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Quest->Planet->Name, u"<color=255,240,100>"_w);
                            Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                            Quest->Successful = true;
                            TRanger::PublishQuestStatus(Quest, 0);
                        } else {
                            TRanger::PublishQuestStatus(Quest, 0);
                        }
                    } else {
                        TRanger::PublishQuestStatus(Quest, 0);
                    }
                }
            }
        }
    }

    // Uses player history.
    std::int32_t TRanger::CountFailedQuests(std::uint8_t OwnerId, aGalaxyStruct::TQuestTypes QuestTypes) {
        std::int32_t I{};
        PPlayerOldQuest Quest{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlayerOldQuests) - 1); cpp_range.next(I); ) {
            Quest = pas::list_at<TPlayerOldQuest>(PlayerOldQuests, I);
            if (static_cast<std::uint8_t>(Quest->Successful ^ 1) && static_cast<std::uint8_t>(Quest->Declined ^ 1) && Quest->Planet->OwnerId == OwnerId && pas::contains(QuestTypes, Quest->QuestType)) {
                ++Result;
            }
        }
        return Result;
    }

    void TRanger::CheckQuestFailureAward(PQuest Quest, aGalaxyStruct::TQuestTypes QuestTypes) {
        std::int32_t FailureCount{};
        // Caller-popped static link; ranger -4. Awards only at an exact geometric milestone, checking at most ten thresholds.
        auto GrantQuestFailureMilestoneAward = [&](std::int32_t InitialThreshold, std::int32_t Multiplier, std::int32_t FailureCount, std::uint8_t OwnerId) -> void {
            aConst::TRewardInfo cpp_result{};
            std::int32_t I{};
            std::int32_t Award{};
            pas::WideString Text{};
            std::int32_t Threshold = InitialThreshold;
            for (I = 1; I <= 10; ++I) {
                if (FailureCount < Threshold) {
                    break;
                }
                if (FailureCount == Threshold) {
                    Award = SelectAward(OwnerId, pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atCowardice}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}})) & 0x000000ff;
                    if (Award != aGalaxyStruct::AwardNotFound) {
                        AddAward(Award);
                        if (aPlayer::GetPlayer() == this) {
                            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.BadReward.FailQuest"_wref.get(), this->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                            aMyFunction::ReplaceTextToken(Text, u"<Reward>"_w, (aNormalShip::TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name, u"<color=255,240,100>"_w);
                            Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                        }
                    }
                    break;
                }
                Threshold = std::min<std::int32_t>(Threshold * Multiplier, 10000000);
            }
        };
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0 && Quest->Planet != nullptr) {
            FailureCount = TRanger::CountFailedQuests(Quest->Planet->OwnerId, QuestTypes) + 1;
            if (FailureCount > 0) {
                GrantQuestFailureMilestoneAward(3, 2, FailureCount, aConst::RaceToOwner(Quest->Planet->RaceId));
            }
        }
    }

    void TRanger::TryTurnInQuests() {
        pas::WideString Text{};
        TryTurnInAnyQuest(Text);
    }

    void TRanger::ArchiveQuest(std::int32_t Index) {
        PPlayerOldQuest OldQuest{};
        PQuest Quest = pas::list_at<TQuest>(Quests, Index);
        if (aPlayer::GetPlayer() == this) {
            pas::new_value(OldQuest);
            OldQuest->QuestType = Quest->QuestType;
            OldQuest->QuestNumber = Quest->QuestNumber;
            OldQuest->Planet = Quest->Planet;
            OldQuest->Description = Quest->Description;
            OldQuest->Successful = Quest->Successful;
            OldQuest->Declined = false;
            pas::list_add(PlayerOldQuests, static_cast<void*>(OldQuest));
        }
        pas::list_delete(Quests, Index);
        pas::dispose(Quest);
        RefreshPlayerQuestTargets();
    }

    std::uint8_t TRanger::TryTurnInAnyQuest(pas::WideString& ResponseText) {
        std::int32_t I{};
        std::uint8_t Result = false;
        ResponseText = pas::WideString();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Quests) - 1); cpp_range.next(I); ) {
            if (TryTurnInQuest(I, ResponseText)) {
                return true;
            }
        }
        return Result;
    }

    std::uint8_t TRanger::TryTurnInQuest(std::int32_t Index, pas::WideString& ResponseText) {
        aConst::TRewardInfo cpp_result{};
        static const pas::Set<0, 255> RewardPrograms = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::prgShipwreck, aGalaxyStruct::prgDisconnection}});
        PQuest Quest{};
        std::int32_t GenerationSeed{};
        std::int32_t Experience{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::int32_t MinimumPriority{};
        std::int32_t RewardKind{};
        std::int32_t Quantity{};
        std::int32_t ModuleIndex{};
        std::uint8_t Award{};
        aItem::TItem* RewardItem{};
        pas::WideString RewardText{};
        std::uint8_t ProgramIndex{};
        aItem::TMicroModule* ModuleItem{};
        pas::WideString Factions{};
        // Caller-popped static link; ranger -4, quest -8. Removes/frees matching delivery cargo; true also when this planet quest requires no item.
        auto ConsumeQuestDeliveryItem = [&]() -> std::uint8_t {
            std::int32_t I{};
            aItem::TItem* Item{};
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(this->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(this->Inventory, I);
                if (Item->ItemType == aConst::t_UselessItem) {
                    if (Quest->QuestType == aGalaxyStruct::qtSendLetter && ([&] {
                        pas::WideString cpp_string = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest->QuestNumber), ".SysName"})));
                        const pas::WideString& cpp_string_ref = pas::checked_cast<aItem::TUselessItem*>(Item)->ConfigBlockName;
                        return cpp_string == cpp_string_ref;
                    }()) || Quest->QuestType == aGalaxyStruct::qtPlanetQuest && ([&] {
                        pas::WideString cpp_string_2 = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.ItemForPlanetQuest.", SysUtils::IntToStr(Quest->QuestNumber)})));
                        const pas::WideString& cpp_string_ref_2 = pas::checked_cast<aItem::TUselessItem*>(Item)->ConfigBlockName;
                        return cpp_string_2 == cpp_string_ref_2;
                    }())) {
                        pas::list_delete(this->Inventory, I);
                        pas::free(Item);
                        RefreshDerivedStats(true);
                        return true;
                    }
                }
            }
            return Quest->QuestType == aGalaxyStruct::qtPlanetQuest && (GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.ItemForPlanetQuest.", SysUtils::IntToStr(Quest->QuestNumber)}))) == u"None" || GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.ItemForPlanetQuest.", SysUtils::IntToStr(Quest->QuestNumber)}))) == u"");
        };
        // Caller-popped static link; ranger -4, quest -8, seed input -12, experience -16, response output -20, event -24, quest index -28. Grants experience, archives the quest and updates relations.
        auto FinalizeSuccessfulQuestTurnIn = [&]() -> void {
            Experience = aMyFunction::RoundAndTruncateToTens(([&] {
                pas::Extended cpp_right = pas::real_divide(aGalaxy::Galaxy->ScaleIntByTechLevel(aConst::QuestExperience[Quest->QuestType], 2 * aConst::QuestExperience[Quest->QuestType]), aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestTimeAndExperienceFactor);
                return aMyFunction::SeededRandomFloatRange((GenerationSeed + aGalaxy::Galaxy->CurrentTurn) / 100 + 123, 0.7, 1.5) * cpp_right;
            }()));
            GainExperience(Experience, 0);
            if (aPlayer::GetPlayer() == this) {
                ResponseText = pas::concat_wide({ResponseText, u"\r\n", u" ", u"\r\n", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddPoints"_wref.get()), u"<color=45,105,45>"_w)});
                aMyFunction::ReplaceTextToken(ResponseText, u"<Points>"_w, pas::wide_int_to_str(Experience), pas::WideString());
            } else {
                ResponseText = pas::WideString();
            }
            if (aPlayer::GetPlayer() == this) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerFinishesQuest"_w, nullptr);
                Event->AddData(Quest->QuestType);
                Event->AddData(Quest->QuestNumber);
                Event->AddData(Quest->RewardMoney);
                Event->AddData(Experience);
            }
            if (aPlayer::GetPlayer() == this && this->CurrentPlanet != nullptr) {
                aMyFunction::ReplaceTextToken(ResponseText, u"<Star>"_w, this->CurrentPlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(ResponseText, u"<Planet>"_w, this->CurrentPlanet->Name, u"<color=255,240,100>"_w);
            }
            TRanger::PublishQuestStatus(Quest, 1);
            if (aPlayer::GetPlayer() == this) {
                aMyFunction::ReplaceTextToken(ResponseText, u"<Ranger>"_w, this->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
            }
            if (Quest->QuestType == aGalaxyStruct::qtDefendSystem) {
                Achievements::TryAddAchievementProgress(u"GUARD"_w, 1);
            }
            if (Quest->QuestType == aGalaxyStruct::qtSendLetter) {
                Achievements::TryAddAchievementProgress(u"DELIVERY"_w, 1);
            }
            ArchiveQuest(Index);
            RefreshPlayerQuestTargets();
            if (this->CurrentPlanet != nullptr) {
                std::int32_t cpp_arg = std::max<std::int32_t>(0, 70 - (this->CurrentPlanet->RelationToShip(this) & 0x0000007f));
                aPlanet::TPlanet* currentPlanet = this->CurrentPlanet;
                currentPlanet->ChangeRelationToRanger(this, cpp_arg);
            }
            if (this->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlanet::MainPiratePlanet != nullptr) {
                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlHostile) {
                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlBad);
                }
                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlBad) {
                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlNormal);
                }
                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlNormal) {
                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlGood);
                }
                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) == aGalaxyStruct::rlGood) {
                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(this, aGalaxyStruct::rlExcellent);
                }
                // Native calls the ranger's own virtual method here, after upgrading the planet relation.
                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(this) >= aGalaxyStruct::rlExcellent) {
                    ChangeRelationToRanger(this, std::max<std::int32_t>(0, 100 - (aPlanet::MainPiratePlanet->RelationToShip(this) & 0x0000007f)));
                }
            }
            Achievements::TryAddAchievementProgress(u"AGENT"_w, 1);
        };
        GenerationSeed = 0;
        if (CurrentPlanet != nullptr) {
            GenerationSeed = CurrentPlanet->GenerationSeed;
        }
        if (DockedTo != nullptr) {
            GenerationSeed = DockedTo->Seed;
        }
        std::uint8_t Result = false;
        Quest = pas::list_at<TQuest>(Quests, Index);
        switch (Quest->QuestType) {
            case aGalaxyStruct::qtSendLetter: {
                if (pas::checked_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget) == CurrentPlanet && ConsumeQuestDeliveryItem()) {
                    Result = true;
                    Quest->Successful = true;
                    SetMoney(Money + Quest->RewardMoney);
                    ResponseText = Quest->CompletionText;
                    Factions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest->QuestNumber), ".ToRace"})));
                    if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyNonPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratPirate"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratCoal"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    RewardText = GR_Main::LookupLocalizedTextOrEmpty(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest->QuestNumber), ".GovernmentAward"})));
                }
                break;
            }
            case aGalaxyStruct::qtKillShip: {
                if (Quest->Planet != nullptr && Quest->Planet == CurrentPlanet && Quest->Successful) {
                    Result = true;
                    SetMoney(Money + Quest->RewardMoney);
                    ResponseText = Quest->CompletionText;
                    Factions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(Quest->QuestNumber), ".PlanetRace"})));
                    if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyNonPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratPirate"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratCoal"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    RewardText = GR_Main::LookupLocalizedTextOrEmpty(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(Quest->QuestNumber), ".GovernmentAward"})));
                }
                break;
            }
            case aGalaxyStruct::qtPlanetQuest: {
                if (Quest->Planet == CurrentPlanet && (Quest->Successful || ConsumeQuestDeliveryItem())) {
                    Result = true;
                    Quest->Successful = true;
                    SetMoney(Money + Quest->RewardMoney);
                    ResponseText = Quest->CompletionText;
                    RewardText = u"Reward,Programms,Artefact"_w;
                }
                break;
            }
            case aGalaxyStruct::qtDefendSystem: {
                if (Quest->Planet != nullptr && Quest->Planet == CurrentPlanet && Quest->Successful) {
                    Result = true;
                    SetMoney(Money + Quest->RewardMoney);
                    ResponseText = Quest->CompletionText;
                    Factions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(Quest->QuestNumber), ".PlanetRace"})));
                    if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyNonPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratPirate"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratCoal"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    RewardText = GR_Main::LookupLocalizedTextOrEmpty(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(Quest->QuestNumber), ".GovernmentAward"})));
                }
                break;
            }
            case aGalaxyStruct::qtDefendShip: {
                if (Quest->Planet != nullptr && Quest->Planet == CurrentPlanet && Quest->Successful) {
                    Result = true;
                    SetMoney(Money + Quest->RewardMoney);
                    if (Quest->ObjectiveTarget == nullptr) {
                        ResponseText = Quest->SpecialCompletionText;
                    } else {
                        ResponseText = Quest->CompletionText;
                    }
                    Factions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(Quest->QuestNumber), ".PlanetRace"})));
                    if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyNonPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratPirate"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::pos(u"OnlyPirate", Factions) > 0) {
                        ResponseText = GR_Main::LookupLocalizedTextByKey(u"Quest.GenericCongratCoal"_wref.get());
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Player>"_w, Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Money>"_w, pas::wide_int_to_str(Quest->RewardMoney), u"<color=255,240,100>"_w);
                    }
                    RewardText = GR_Main::LookupLocalizedTextOrEmpty(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(Quest->QuestNumber), ".GovernmentAward"})));
                }
                break;
            }
        }
        if (!Result) {
            return Result;
        }
        {
            pas::Extended cpp_right = aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.1, 0.5);
            if (aMyFunction::SeededRandomUnitFloat((GenerationSeed + aGalaxy::Galaxy->CurrentTurn) / 51 + 1767) < cpp_right) {
                FinalizeSuccessfulQuestTurnIn();
                return Result;
            }
        }
        float AwardWeight = EC_Str::FindTextPosW(u"Reward"_wref.get(), RewardText);
        float ProgramWeight = EC_Str::FindTextPosW(u"Programms"_wref.get(), RewardText);
        float ArtefactWeight = EC_Str::FindTextPosW(u"Artefact"_wref.get(), RewardText);
        float ModuleWeight = EC_Str::FindTextPosW(u"Nod"_wref.get(), RewardText);
        if (AwardWeight > 0.0L) {
            if (AwardIds == nullptr) {
                AwardWeight = 8.0E+1f;
            } else {
                AwardWeight = aMyFunction::RemapClamped(pas::list_count(AwardIds), 0.0, 15.0, 8.0E+1, 1.0E+1);
            }
        }
        if (HasProgram(aGalaxyStruct::prgIntercom) && ProgramWeight > 0.0L) {
            ProgramWeight = aMyFunction::RemapClamped(CountProgramsInFilter(static_cast<TRangerProgramMask>(RewardPrograms)), 0.0, 1.0E+1, 8.0E+1, 1.0E+1);
        } else {
            ProgramWeight = 0.0f;
        }
        if (ArtefactWeight > 0.0L) {
            ArtefactWeight = aMyFunction::RemapClamped(pas::list_count(Artefacts), 1.0, 5.0, 8.0E+1, 1.0E+1);
        }
        if (ModuleWeight > 0.0L) {
            ModuleWeight = aMyFunction::RandomIntRange(10, 90);
        }
        if (AwardWeight == 0.0L && ProgramWeight == 0.0L && ArtefactWeight == 0.0L && ModuleWeight == 0.0L) {
            FinalizeSuccessfulQuestTurnIn();
            return Result;
        }
        if (AwardWeight > 0.0L) {
            AwardWeight = static_cast<long double>(AwardWeight) * aMyFunction::SeededRandomIntRange(5, 25, GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 33 + 667);
        }
        if (ProgramWeight > 0.0L) {
            ProgramWeight = static_cast<long double>(ProgramWeight) * aMyFunction::SeededRandomIntRange(5, 25, GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 41 + 767);
        }
        if (ArtefactWeight > 0.0L) {
            ArtefactWeight = static_cast<long double>(ArtefactWeight) * aMyFunction::SeededRandomIntRange(5, 25, GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 57 + 967);
        }
        if (ModuleWeight > 0.0L) {
            ModuleWeight = static_cast<long double>(ModuleWeight) * aMyFunction::SeededRandomIntRange(5, 25, GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 77 + 1967);
        }
        if (AwardWeight > 0.0L && AwardWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(ProgramWeight, ArtefactWeight))) {
            RewardKind = 1;
        } else if (ProgramWeight > 0.0L && ProgramWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(AwardWeight, ArtefactWeight))) {
            RewardKind = 2;
        } else if (ArtefactWeight > 0.0L && ArtefactWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(AwardWeight, ProgramWeight))) {
            RewardKind = 3;
        } else if (ModuleWeight > 0.0L && ModuleWeight >= pas::real_max<float>(ArtefactWeight, pas::real_max<float>(AwardWeight, ProgramWeight))) {
            RewardKind = 4;
        } else {
            FinalizeSuccessfulQuestTurnIn();
            return Result;
        }
        switch (RewardKind) {
            case 1: {
                if (CurrentPlanet != nullptr) {
                    Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                } else {
                    Award = SelectAward(DockedTo->OwnerId, pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atAccomplishment}, {aGalaxyStruct::atSecretMission}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                }
                if (Award != aGalaxyStruct::AwardNotFound) {
                    AddAward(Award);
                    if (aPlayer::GetPlayer() == this) {
                        ResponseText = pas::concat_wide({ResponseText, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddReward"_wref.get())});
                        aMyFunction::ReplaceTextToken(ResponseText, u"<Reward>"_w, (aNormalShip::TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name, u"<color=255,240,100>"_w);
                    } else {
                        ResponseText = pas::WideString();
                    }
                }
                break;
            }
            case 2: {
                ProgramIndex = SelectRandomProgramIdFromFilter(static_cast<TRangerProgramMask>(RewardPrograms));
                Quantity = aMyFunction::SeededRandomIntRange(1, System::Round(aMyFunction::RemapClamped(CountProgramsInFilter(static_cast<TRangerProgramMask>(RewardPrograms)), 2.0, 1.0E+1, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].MaximumQuestProgramRewardCount, 1.0)), ProgramIndex + CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 25));
                ProgramCounts[ProgramIndex] += Quantity;
                if (aPlayer::GetPlayer() == this) {
                    ResponseText = pas::concat_wide({ResponseText, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddProgramms"_wref.get())});
                    aMyFunction::ReplaceTextToken(ResponseText, u"<Programm>"_w, TRanger::GetProgramName(ProgramIndex), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(ResponseText, u"<Count>"_w, pas::wide_int_to_str(Quantity), u"<color=255,240,100>"_w);
                } else {
                    ResponseText = pas::WideString();
                }
                break;
            }
            case 3: {
                if (CurrentPlanet != nullptr) {
                    RewardItem = aItem::CreateRandomLootItem(aItem::ilpReward, CurrentPlanet->OwnerId, (static_cast<std::int32_t>(CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 50 + 172334671);
                } else {
                    RewardItem = aItem::CreateRandomLootItem(aItem::ilpReward, DockedTo->OwnerId, (static_cast<std::int32_t>(DockedTo->Seed) + aGalaxy::Galaxy->CurrentTurn) / 50 + 172334671);
                }
                if (pas::class_cast_if<aItem::TArtefactTranclucator*>(RewardItem) != nullptr) {
                    static_cast<aTranclucator::TTranclucator*>(reinterpret_cast<aItem::TArtefactTranclucator*>(RewardItem)->Ship)->OwnerShip = this;
                }
                if (pas::class_cast_if<aItem::TArtefact*>(RewardItem) != nullptr) {
                    pas::list_add(Artefacts, reinterpret_cast<void*>(RewardItem));
                } else {
                    pas::list_add(Inventory, reinterpret_cast<void*>(RewardItem));
                }
                if (aPlayer::GetPlayer() == this) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnGovItemReward, RewardItem, nullptr, 0);
                    ResponseText = pas::concat_wide({ResponseText, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddArtefact"_wref.get()), u"\r\n", RewardItem->GetDescriptionText()});
                    aMyFunction::ReplaceTextToken(ResponseText, u"<Artefact>"_w, RewardItem->GetDisplayName(), u"<color=255,240,100>"_w);
                } else {
                    ResponseText = pas::WideString();
                }
                break;
            }
            case 4: {
                Experience = 0;
                do {
                    MinimumPriority = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 7.0E+1, 2.0E+1));
                    if (CurrentPlanet != nullptr) {
                        ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(MinimumPriority, std::min<std::int32_t>(MinimumPriority + 30, 100), aGalaxy::Galaxy->CurrentTurn / 77 + 17 * Experience + CurrentPlanet->Id, CurrentPlanet);
                    } else {
                        ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(MinimumPriority, std::min<std::int32_t>(MinimumPriority + 30, 100), aGalaxy::Galaxy->CurrentTurn / 77 + 17 * Experience + DockedTo->Id, DockedTo);
                    }
                    ++Experience;
                    if (Experience > 50) {
                        break;
                    }
                } while (!aPlayer::GetPlayer()->NeedsMicroModule(ModuleIndex + 1));
                ModuleItem = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                ModuleItem->Init(ModuleIndex);
                if (aPlayer::GetPlayer() == this) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnGovItemReward, ModuleItem, nullptr, 0);
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerReceivesMMAsReward"_w, nullptr);
                    Event->AddData(ModuleItem->Id);
                    Event->AddData(ModuleItem->MicroModuleIndex - 1);
                }
                pas::list_add(Inventory, reinterpret_cast<void*>(ModuleItem));
                if (aPlayer::GetPlayer() == this) {
                    if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                        ResponseText = pas::concat_wide({ResponseText, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddNodPirate"_wref.get()), u"\r\n", ModuleItem->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, nullptr)});
                    } else {
                        ResponseText = pas::concat_wide({ResponseText, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddNod"_wref.get()), u"\r\n", ModuleItem->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, nullptr)});
                    }
                    aMyFunction::ReplaceTextToken(ResponseText, u"<Nod>"_w, aConst::MicroModuleTemplates[ModuleIndex].Name, u"<color=255,240,100>"_w);
                } else {
                    ResponseText = pas::WideString();
                }
                break;
            }
        }
        FinalizeSuccessfulQuestTurnIn();
        return Result;
    }

    // Requires CurrentPlanet. Grants an award, program, item or module plus experience and relation effects. The hidden WideString result is cleared on entry; NPC result is empty.
    pas::WideString TRanger_GrantPlanetQuestReward(TRanger* Self, std::int32_t Difficulty, std::int32_t& ExperienceAwarded) {
        aConst::TRewardInfo cpp_result{};
        pas::WideString Result{};
        static const pas::Set<0, 255> RewardPrograms = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::prgShipwreck, aGalaxyStruct::prgDisconnection}});
        std::int32_t Amount{};
        std::int32_t MinimumPriority{};
        std::int32_t RewardKind{};
        std::int32_t Quantity{};
        std::int32_t ModuleIndex{};
        std::uint8_t Award{};
        aItem::TItem* RewardItem{};
        std::uint8_t ProgramIndex{};
        aItem::TMicroModule* ModuleItem{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        Result = pas::WideString();
        float AwardWeight = 1.0f;
        float ProgramWeight = 1.0f;
        float ArtefactWeight = 1.0f;
        float ModuleWeight = 1.0f;
        if (AwardWeight > 0.0L) {
            if (Self->AwardIds == nullptr) {
                AwardWeight = 8.0E+1f;
            } else {
                AwardWeight = aMyFunction::RemapClamped(pas::list_count(Self->AwardIds), 0.0, 25.0, 8.0E+1, 1.0E+1);
            }
        }
        if (Self->HasProgram(aGalaxyStruct::prgIntercom) && ProgramWeight > 0.0L) {
            ProgramWeight = aMyFunction::RemapClamped(Self->CountProgramsInFilter(static_cast<TRangerProgramMask>(RewardPrograms)), 0.0, 1.0E+1, 8.0E+1, 1.0E+1);
        }
        if (ArtefactWeight > 0.0L) {
            ArtefactWeight = aMyFunction::RemapClamped(pas::list_count(Self->Artefacts), 0.0, 5.0, 4.0E+1, 5.0);
        }
        if (ModuleWeight > 0.0L) {
            ModuleWeight = aMyFunction::RandomIntRange(40, 95);
        }
        if (AwardWeight == 0.0L && ProgramWeight == 0.0L && ArtefactWeight == 0.0L && ModuleWeight == 0.0L) {
            return Result;
        }
        if (AwardWeight > 0.0L) {
            AwardWeight = static_cast<long double>(AwardWeight) * aMyFunction::SeededRandomIntRange(5, 25, Self->CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 33 + 667);
        }
        if (ProgramWeight > 0.0L) {
            ProgramWeight = static_cast<long double>(ProgramWeight) * aMyFunction::SeededRandomIntRange(5, 25, Self->CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 41 + 767);
        }
        if (ArtefactWeight > 0.0L) {
            ArtefactWeight = static_cast<long double>(ArtefactWeight) * aMyFunction::SeededRandomIntRange(5, 25, Self->CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 57 + 149671);
        }
        if (ModuleWeight > 0.0L) {
            ModuleWeight = static_cast<long double>(ModuleWeight) * aMyFunction::SeededRandomIntRange(5, 25, Self->CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 77 + 1967);
        }
        if (AwardWeight > 0.0L && AwardWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(ProgramWeight, ArtefactWeight))) {
            RewardKind = 1;
        } else if (ProgramWeight > 0.0L && ProgramWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(AwardWeight, ArtefactWeight))) {
            RewardKind = 2;
        } else if (ArtefactWeight > 0.0L && ArtefactWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(AwardWeight, ProgramWeight))) {
            RewardKind = 3;
        } else if (ModuleWeight > 0.0L && ModuleWeight >= pas::real_max<float>(ArtefactWeight, pas::real_max<float>(AwardWeight, ProgramWeight))) {
            RewardKind = 4;
        } else {
            return Result;
        }
        switch (RewardKind) {
            case 1: {
                Award = Self->SelectAward(aConst::RaceToOwner(Self->CurrentPlanet->RaceId), pas::constant_set<aNormalShip::TAwardTypeMask>({{aGalaxyStruct::atPlanetBattle}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
                if (Award != aGalaxyStruct::AwardNotFound) {
                    Self->AddAward(Award);
                    if (aPlayer::GetPlayer() == Self) {
                        Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddReward"_wref.get())});
                        aMyFunction::ReplaceTextToken(Result, u"<Reward>"_w, (aNormalShip::TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name, u"<color=255,240,100>"_w);
                    } else {
                        Result = pas::WideString();
                    }
                }
                break;
            }
            case 2: {
                ProgramIndex = Self->SelectRandomProgramIdFromFilter(static_cast<TRangerProgramMask>(RewardPrograms));
                Quantity = aMyFunction::SeededRandomIntRange(1, System::Round(aMyFunction::RemapClamped(Self->CountProgramsInFilter(static_cast<TRangerProgramMask>(RewardPrograms)), 2.0, 1.0E+1, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].MaximumQuestProgramRewardCount, 1.0)), ProgramIndex + Self->CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 25));
                Self->ProgramCounts[ProgramIndex] += Quantity;
                if (aPlayer::GetPlayer() == Self) {
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddProgramms"_wref.get())});
                    aMyFunction::ReplaceTextToken(Result, u"<Programm>"_w, TRanger::GetProgramName(ProgramIndex), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Result, u"<Count>"_w, pas::wide_int_to_str(Quantity), u"<color=255,240,100>"_w);
                } else {
                    Result = pas::WideString();
                }
                break;
            }
            case 3: {
                RewardItem = aItem::CreateRandomLootItem(aItem::ilpReward, Self->CurrentPlanet->OwnerId, (static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 50 + 172334671);
                if (pas::class_cast_if<aItem::TArtefactTranclucator*>(RewardItem) != nullptr) {
                    static_cast<aTranclucator::TTranclucator*>(reinterpret_cast<aItem::TArtefactTranclucator*>(RewardItem)->Ship)->OwnerShip = Self;
                }
                if (pas::class_cast_if<aItem::TArtefact*>(RewardItem) != nullptr) {
                    pas::list_add(Self->Artefacts, reinterpret_cast<void*>(RewardItem));
                } else {
                    pas::list_add(Self->Inventory, reinterpret_cast<void*>(RewardItem));
                }
                if (aPlayer::GetPlayer() == Self) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnGovItemReward, RewardItem, nullptr, 0);
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddArtefact"_wref.get()), u"\r\n", RewardItem->GetDescriptionText()});
                    aMyFunction::ReplaceTextToken(Result, u"<Artefact>"_w, RewardItem->GetDisplayName(), u"<color=255,240,100>"_w);
                } else {
                    Result = pas::WideString();
                }
                break;
            }
            case 4: {
                Amount = 0;
                MinimumPriority = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 7.0E+1, 0.0));
                do {
                    ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(MinimumPriority, std::min<std::int32_t>(MinimumPriority + 30, 100), aGalaxy::Galaxy->CurrentTurn / 77 + 17 * Amount + Self->CurrentPlanet->Id, Self->CurrentPlanet);
                    ++Amount;
                    if (Amount > 50) {
                        break;
                    }
                } while (!aPlayer::GetPlayer()->NeedsMicroModule(ModuleIndex + 1));
                ModuleItem = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                ModuleItem->Init(ModuleIndex);
                if (aPlayer::GetPlayer() == Self) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnGovItemReward, ModuleItem, nullptr, 0);
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerReceivesMMAsReward"_w, nullptr);
                    Event->AddData(ModuleItem->Id);
                    Event->AddData(ModuleItem->MicroModuleIndex - 1);
                }
                pas::list_add(Self->Inventory, reinterpret_cast<void*>(ModuleItem));
                if (aPlayer::GetPlayer() == Self) {
                    if (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                        Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddNodPirate"_wref.get()), u"\r\n", ModuleItem->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, nullptr)});
                    } else {
                        Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddNod"_wref.get()), u"\r\n", ModuleItem->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, nullptr)});
                    }
                    aMyFunction::ReplaceTextToken(Result, u"<Nod>"_w, aConst::MicroModuleTemplates[ModuleIndex].Name, u"<color=255,240,100>"_w);
                } else {
                    Result = pas::WideString();
                }
                break;
            }
        }
        {
            pas::Extended cpp_right = pas::real_divide(aGalaxy::Galaxy->ScaleIntByTechLevel(aConst::QuestExperience[2], 2 * aConst::QuestExperience[2]), aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestTimeAndExperienceFactor);
            Amount = aMyFunction::RoundAndTruncateToTens(aMyFunction::SeededRandomFloatRange((static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 100 + 0x00707d5b, 0.7, 1.5) * cpp_right);
        }
        switch (Difficulty) {
            case 1: Amount *= 2; break;
            case 2: Amount = System::Round(Amount * 0.9L); break;
            case 3: Amount = System::Round(Amount * 0.6L); break;
        }
        Self->GainExperience(Amount, 0);
        ExperienceAwarded = Amount;
        if (aPlayer::GetPlayer() == Self) {
            Result = pas::concat_wide({Result, u"\r\n", u" ", u"\r\n", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"PlanetCongratulations.Quest.AddPoints"_wref.get()), u"<color=45,105,45>"_w)});
            aMyFunction::ReplaceTextToken(Result, u"<Points>"_w, pas::wide_int_to_str(Amount), pas::WideString());
        } else {
            Result = pas::WideString();
        }
        if (Self->CurrentPlanet != nullptr) {
            std::int32_t cpp_arg = std::max<std::int32_t>(0, 70 - (Self->CurrentPlanet->RelationToShip(Self) & 0x0000007f));
            aPlanet::TPlanet* currentPlanet = Self->CurrentPlanet;
            currentPlanet->ChangeRelationToRanger(Self, cpp_arg);
        }
        if (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlanet::MainPiratePlanet != nullptr) {
            if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlHostile) {
                aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlBad);
            }
            if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlBad) {
                aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlNormal);
            }
            if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlNormal) {
                aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlGood);
            }
            if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlGood) {
                aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlExcellent);
            }
            // Native calls the ranger's own virtual method here, after upgrading the planet relation.
            if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) >= aGalaxyStruct::rlExcellent) {
                Self->ChangeRelationToRanger(Self, std::max<std::int32_t>(0, 100 - (aPlanet::MainPiratePlanet->RelationToShip(Self) & 0x0000007f)));
            }
        }
        return Result;
    }

    // Requires CurrentPlanet.
    std::uint8_t TRanger_GenerateQuestOffer(TRanger* Self, TQuest& Quest, pas::WideString& ResponseText) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t Attempts{};
        std::int32_t QuestNumber{};
        std::int32_t MaximumQuest{};
        std::int32_t FirstStar{};
        aGalaxy::TStar* Star{};
        aPlanet::TPlanet* Planet{};
        PQuest ExistingQuest{};
        aShip::TShip* Target{};
        aShip::TShip* DefendedShip{};
        TextQuest::TTextQuest* TextQuest{};
        EC_Cache::TCacheControlEC* Control{};
        EC_CacheBuf::TCBufEC* Buffer{};
        std::uint8_t Found{};
        pas::WideString ShipTypes{};
        pas::WideString UnusedText{};
        pas::WideString FromFactions{};
        pas::WideString ToFactions{};
        std::uint8_t Result = false;
        ResponseText = pas::WideString();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->Quests) - 1); cpp_range.next(I); ) {
            ExistingQuest = pas::list_at<TQuest>(Self->Quests, I);
            if (ExistingQuest->Planet == Self->CurrentPlanet) {
                ResponseText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.YouHaveQuest"_wref.get(), Self->CurrentPlanet->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 5) + 165856);
                return Result;
            }
        }
        if (Self->CurrentPlanet->GetRelationLevelToShip(Self) < aGalaxyStruct::rlGood) {
            ResponseText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.Distrust"_wref.get(), Self->CurrentPlanet->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 5) + 23236);
            return Result;
        }
        if (Self->CurrentPlanet->CurrentStar->Status.Battle != 0 && Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            ResponseText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.WarInSystemPirate"_wref.get(), Self->CurrentPlanet->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 5) + 118123);
            return Result;
        }
        if (Self->CurrentPlanet->CurrentStar->Status.Battle != 0) {
            ResponseText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.WarInSystem"_wref.get(), Self->CurrentPlanet->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 5) + 118123);
            return Result;
        }
        std::int32_t Interval = 10;
        if (!(aMyFunction::FractionalQuotient(Self->CurrentPlanet->GenerationSeed, aGalaxy::Galaxy->CurrentTurn + 100) >= 1.5L && GlobalsV::ForcedPlanetQuestId < 0)) {
            if (static_cast<std::uint8_t>(Self->HasQuestOfType(aGalaxyStruct::qtSendLetter) ^ 1) && GlobalsV::ForcedPlanetQuestId < 0) {
                if (aMyFunction::FractionalQuotient(Self->CurrentPlanet->GenerationSeed, pas::idiv(aGalaxy::Galaxy->CurrentTurn + 111, Interval)) < pas::load_unaligned<float>(pas::byte_offset(&aConst::PlanetGovernmentMarket[Self->CurrentPlanet->Government].QuestOfferProbabilities, 0 * sizeof(float)))) {
                    Attempts = 0;
                    do {
                        ++Attempts;
                        I = aMyFunction::SeededRandomIntRange(pas::list_count(aGalaxy::Galaxy->Stars) / 6, pas::list_count(aGalaxy::Galaxy->Stars) / 3, pas::idiv(Attempts + aGalaxy::Galaxy->CurrentTurn, Interval));
                        Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
                        if (Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Constellation->Id != 20 && Star->IsConstellationVisible()) {
                            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                                if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId) && static_cast<std::uint8_t>(Planet->NoLanding ^ 1) && Planet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlHostile) {
                                    MaximumQuest = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"Quest.SendLetter.Count"_wref.get()))) - 1;
                                    QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval));
                                    Found = false;
                                    UnusedText = pas::WideString();
                                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, MaximumQuest); cpp_range_3.next(K); ) {
                                        FromFactions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(QuestNumber), ".FromRace"})));
                                        ToFactions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(QuestNumber), ".ToRace"})));
                                        if (pas::pos(aConst::OwnerToSys(aConst::RaceToOwner(Self->CurrentPlanet->RaceId)), FromFactions) > 0 && (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyPirate", FromFactions) <= 0) && (Self->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyNonPirate", FromFactions) <= 0) && pas::pos(aConst::OwnerToSys(aConst::RaceToOwner(Planet->RaceId)), ToFactions) > 0 && (Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyPirate", ToFactions) <= 0) && (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyNonPirate", ToFactions) <= 0) && (GR_Main::LanguageDataConfig->CountParamsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"}))) == 0 || ([&] {
                                            const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"})));
                                            const pas::WideString& ownerToSys = aConst::OwnerToSys(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                                            return pas::pos(ownerToSys, lookupLocalizedTextByKey);
                                        }()) > 0) && static_cast<std::uint8_t>(aGalaxy::TGalaxy::HasPlayerQuestHistory(aGalaxyStruct::qtSendLetter, QuestNumber) ^ 1) && ([&] {
                                            const pas::WideString& lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(QuestNumber), ".Status"})));
                                            std::uint8_t cpp_arg = Self->GetDominantCareer();
                                            return aConst::MatchesCareerName(cpp_arg, lookupLocalizedTextByKey_2);
                                        }())) {
                                            Found = true;
                                            break;
                                        }
                                        QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval) + 37 * K);
                                    }
                                    if (Found) {
                                        Quest.QuestType = aGalaxyStruct::qtSendLetter;
                                        Quest.Planet = Self->CurrentPlanet;
                                        Quest.Successful = false;
                                        Quest.DeadlineTurn = aConst::QuestTuning[Quest.QuestType].BaseDuration + 15 * (Self->CurrentStar->StarDistances[I].Distance / 20 + 1);
                                        if (Self->IsHealthEffectActive(24)) {
                                            Quest.DeadlineTurn = System::Round(Quest.DeadlineTurn * 1.5L);
                                        }
                                        Quest.DeadlineTurn = aGalaxy::Galaxy->CurrentTurn + System::Round(pas::real_divide(Quest.DeadlineTurn, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestTimeAndExperienceFactor));
                                        Quest.RewardMoney = aConst::QuestTuning[Quest.QuestType].BaseRewardMoney + System::Round(aConst::QuestTuning[Quest.QuestType].RewardCapitalPercent * pas::real_min<pas::Extended>(aGalaxy::Galaxy->AverageRangerCapital * 0.01L, aPlayer::GetPlayer()->Wealth * 0.01L));
                                        Quest.RewardMoney = System::Round(static_cast<long double>(Quest.RewardMoney) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestMoneyFactor);
                                        if (Self->IsHealthEffectActive(23)) {
                                            Quest.RewardMoney = System::Round(([&] {
                                                pas::Extended cpp_right = aMyFunction::SeededRandomFloatRange(pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval), 1.3, 2.3);
                                                return Quest.RewardMoney * cpp_right;
                                            }()));
                                        }
                                        Quest.RewardMoney += System::Round(Quest.RewardMoney * (Self->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.1L);
                                        Quest.RewardMoney = aMyFunction::RoundAndTruncateToHundreds(Quest.RewardMoney);
                                        Quest.ObjectiveTarget = Planet;
                                        Quest.QuestNumber = QuestNumber;
                                        Quest.CompletionText = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(QuestNumber), ".Status"})));
                                        Self->RefreshPlayerQuestTargets();
                                        return true;
                                    }
                                }
                            }
                        }
                    } while (!(Attempts > 20));
                }
            }
            if (static_cast<std::uint8_t>(Self->HasQuestOfType(aGalaxyStruct::qtKillShip) ^ 1) && aGalaxy::Galaxy->CurrentTurn > 665 && GlobalsV::ForcedPlanetQuestId < 0) {
                if (aMyFunction::FractionalQuotient(Self->CurrentPlanet->GenerationSeed, pas::idiv(aGalaxy::Galaxy->CurrentTurn + 222, Interval)) < pas::load_unaligned<float>(pas::byte_offset(&aConst::PlanetGovernmentMarket[Self->CurrentPlanet->Government].QuestOfferProbabilities, 1 * sizeof(float)))) {
                    const std::int32_t cpp_first = std::min<std::int32_t>(20, pas::list_count(aGalaxy::Galaxy->Stars) - 1);
                    if (cpp_first >= 1) {
                        for (I = cpp_first; I >= 1; --I) {
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
                            if (Star->Constellation->Id != 20 && Star->IsConstellationVisible() && Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Status.CustomFaction == u"") {
                                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_4.next(J); ) {
                                    Target = pas::list_at<aShip::TShip>(Star->Ships, J);
                                    if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate) && Target != Self && Target->ScriptShip == nullptr && static_cast<std::uint8_t>(Target->DestroyQueued ^ 1) && Target->GetHull()->HullPoints >= pas::real_divide(Target->GetHull()->Weight, 1.5L)) {
                                        switch (Target->TypeId) {
                                            case aGalaxyStruct::stRanger: {
                                                if (Self->CurrentPlanet->GetRelationLevelToShip(Target) > aGalaxyStruct::rlBad || aShip::TShip_IsInPrison(Target)) {
                                                    continue;
                                                }
                                                break;
                                            }
                                            case aGalaxyStruct::stTransport: {
                                                switch (pas::checked_cast<aTransport::TTransport*>(Target)->TransportType) {
                                                    case aTransport::ttTransport: {
                                                        if (Self->CurrentPlanet->GetRelationLevelToShip(Target) > aGalaxyStruct::rlNormal && Target->GetTurnSeedFraction(10) < 0.99L) {
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                    case aTransport::ttLiner: {
                                                        if (Self->CurrentPlanet->GetRelationLevelToShip(Target) > aGalaxyStruct::rlNormal && Target->GetTurnSeedFraction(10) < 0.99L) {
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                    case aTransport::ttDiplomat: {
                                                        if (Self->CurrentPlanet->GetRelationLevelToShip(Target) > aGalaxyStruct::rlNormal && Target->GetTurnSeedFraction(10) < 0.99L) {
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                }
                                                break;
                                            }
                                            case aGalaxyStruct::stPirate: {
                                                if (Self->CurrentPlanet->GetRelationLevelToShip(Target) > aGalaxyStruct::rlNormal && Target->GetTurnSeedFraction(10) < 0.99L || aShip::TShip_IsInPrison(Target)) {
                                                    continue;
                                                }
                                                break;
                                            }
                                        }
                                        MaximumQuest = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"Quest.KillShip.Count"_wref.get()))) - 1;
                                        QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval));
                                        Found = false;
                                        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, MaximumQuest); cpp_range_5.next(K); ) {
                                            FromFactions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".PlanetRace"})));
                                            if (pas::pos(aConst::OwnerToSys(aConst::RaceToOwner(Self->CurrentPlanet->RaceId)), FromFactions) > 0 && (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyPirate", FromFactions) <= 0) && (Self->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyNonPirate", FromFactions) <= 0) && (([&] {
                                                const pas::WideString& lookupLocalizedTextByKey_3 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".ShipRace"})));
                                                const pas::WideString& ownerToSys_2 = aConst::OwnerToSys(Target->OwnerId);
                                                return pas::pos(ownerToSys_2, lookupLocalizedTextByKey_3);
                                            }()) > 0 || pas::class_cast_if<aPirate::TPirate*>(Target) != nullptr && static_cast<aPirate::TPirate*>(Target)->PirateType == 0 && Target->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && ([&] {
                                                const pas::WideString& lookupLocalizedTextByKey_4 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".ShipRace"})));
                                                const pas::WideString& ownerToSys_3 = aConst::OwnerToSys(aConst::RaceToOwner(Target->PilotRace));
                                                return pas::pos(ownerToSys_3, lookupLocalizedTextByKey_4);
                                            }()) > 0) && (GR_Main::LanguageDataConfig->CountParamsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"}))) == 0 || ([&] {
                                                const pas::WideString& lookupLocalizedTextByKey_5 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"})));
                                                const pas::WideString& ownerToSys_4 = aConst::OwnerToSys(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                                                return pas::pos(ownerToSys_4, lookupLocalizedTextByKey_5);
                                            }()) > 0) && static_cast<std::uint8_t>(aGalaxy::TGalaxy::HasPlayerQuestHistory(aGalaxyStruct::qtKillShip, QuestNumber) ^ 1) && ([&] {
                                                const pas::WideString& lookupLocalizedTextByKey_6 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".Status"})));
                                                std::uint8_t cpp_arg_2 = Self->GetDominantCareer();
                                                return aConst::MatchesCareerName(cpp_arg_2, lookupLocalizedTextByKey_6);
                                            }())) {
                                                ShipTypes = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(QuestNumber), ".ShipType"})));
                                                switch (Target->TypeId) {
                                                    case aGalaxyStruct::stRanger: {
                                                        if (pas::pos(u"Ranger", ShipTypes) > 0) {
                                                            Found = true;
                                                        }
                                                        break;
                                                    }
                                                    case aGalaxyStruct::stTransport: {
                                                        switch (pas::checked_cast<aTransport::TTransport*>(Target)->TransportType) {
                                                            case aTransport::ttTransport: {
                                                                if (pas::pos(u"Transport", ShipTypes) > 0) {
                                                                    Found = true;
                                                                }
                                                                break;
                                                            }
                                                            case aTransport::ttLiner: {
                                                                if (pas::pos(u"Liner", ShipTypes) > 0) {
                                                                    Found = true;
                                                                }
                                                                break;
                                                            }
                                                            case aTransport::ttDiplomat: {
                                                                if (pas::pos(u"Diplomat", ShipTypes) > 0) {
                                                                    Found = true;
                                                                }
                                                                break;
                                                            }
                                                        }
                                                        break;
                                                    }
                                                    case aGalaxyStruct::stPirate: {
                                                        if (pas::pos(u"Pirate", ShipTypes) > 0) {
                                                            Found = true;
                                                        }
                                                        break;
                                                    }
                                                }
                                                if (ShipTypes == u"Any") {
                                                    Found = true;
                                                }
                                                if (Found) {
                                                    break;
                                                }
                                            }
                                            QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval) + 47 * K);
                                        }
                                        if (Found) {
                                            Quest.QuestType = aGalaxyStruct::qtKillShip;
                                            Quest.Planet = Self->CurrentPlanet;
                                            Quest.Successful = false;
                                            Quest.DeadlineTurn = aConst::QuestTuning[Quest.QuestType].BaseDuration + ([&] {
                                                pas::Extended cpp_left = aMyFunction::RemapClamped(Target->Wealth, aPlayer::GetPlayer()->Wealth / 2, 2 * aPlayer::GetPlayer()->Wealth, 0.0, 3.0E+1);
                                                return System::Round(cpp_left + aMyFunction::PointDistance(Self->CurrentStar->Position, Target->CurrentStar->Position));
                                            }());
                                            if (Self->IsHealthEffectActive(24)) {
                                                Quest.DeadlineTurn = System::Round(Quest.DeadlineTurn * 1.5L);
                                            }
                                            Quest.DeadlineTurn = aGalaxy::Galaxy->CurrentTurn + System::Round(pas::real_divide(Quest.DeadlineTurn, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestTimeAndExperienceFactor));
                                            Quest.RewardMoney = aConst::QuestTuning[Quest.QuestType].BaseRewardMoney + System::Round(([&] {
                                                pas::Extended cpp_left_2 = aConst::QuestTuning[Quest.QuestType].RewardCapitalPercent * pas::real_min<pas::Extended>(aGalaxy::Galaxy->AverageRangerCapital * 0.01L, aPlayer::GetPlayer()->Wealth * 0.01L);
                                                return cpp_left_2 * aMyFunction::RemapClamped(Target->Wealth, aPlayer::GetPlayer()->Wealth / 2, 2 * aPlayer::GetPlayer()->Wealth, 0.8, 1.2);
                                            }()));
                                            Quest.RewardMoney = System::Round(static_cast<long double>(Quest.RewardMoney) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestMoneyFactor);
                                            if (Self->IsHealthEffectActive(23)) {
                                                Quest.RewardMoney = System::Round(([&] {
                                                    pas::Extended cpp_right_2 = aMyFunction::SeededRandomFloatRange(pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval), 1.3, 2.3);
                                                    return Quest.RewardMoney * cpp_right_2;
                                                }()));
                                            }
                                            Quest.RewardMoney += System::Round(Quest.RewardMoney * (Self->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.1L);
                                            Quest.RewardMoney = aMyFunction::RoundAndTruncateToHundreds(Quest.RewardMoney);
                                            Quest.ObjectiveTarget = Target;
                                            Quest.QuestNumber = QuestNumber;
                                            Result = true;
                                            Self->RefreshPlayerQuestTargets();
                                            return Result;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            FirstStar = 10;
            if (GlobalsV::ForcedPlanetQuestId >= 0) {
                FirstStar = 0;
            }
            if (!Self->HasQuestOfType(aGalaxyStruct::qtPlanetQuest)) {
                if (GlobalsV::ForcedPlanetQuestId >= 0 || aMyFunction::FractionalQuotient(Self->CurrentPlanet->GenerationSeed, pas::idiv(aGalaxy::Galaxy->CurrentTurn + 333, Interval)) < pas::load_unaligned<float>(pas::byte_offset(&aConst::PlanetGovernmentMarket[Self->CurrentPlanet->Government].QuestOfferProbabilities, 2 * sizeof(float)))) {
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(FirstStar, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_6.next(I); ) {
                        Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
                        if (Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.Battle == 0 && Star->Status.ControlFaction != aGalaxyStruct::sfDominators && Star->Status.CustomFaction == u"" && Star->Constellation->Id != 20 && Star->IsConstellationVisible()) {
                            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_7.next(J); ) {
                                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                                if (static_cast<std::uint8_t>(Planet->NoLanding ^ 1) && Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Planet->TextQuestId != -1) {
                                    if (Planet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlHostile) {
                                        if (([&] {
                                            const pas::WideString& intToStr = pas::wide_int_to_str(Planet->TextQuestId);
                                            EC_BlockPar::TBlockParEC* blockByPath = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.PlanetQuest"_wref.get());
                                            return blockByPath->CountParams(intToStr);
                                        }()) > 0 && (GlobalsV::ForcedPlanetQuestId < 0 || Planet->TextQuestId == GlobalsV::ForcedPlanetQuestId)) {
                                            TextQuest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                                            Control = nullptr;
                                            {
                                                std::exception_ptr cpp_error{};
                                                try {
                                                    Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                                                    EC_Cache::TCacheEC::ResetControl(Control);
                                                    Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.", SysUtils::IntToStr(Planet->TextQuestId)})));
                                                    Buffer = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                                                    TextQuest->LoadFromReader(Buffer->Buffer, true);
                                                } catch (...) {
                                                    cpp_error = std::current_exception();
                                                }
                                                if (Control != nullptr) {
                                                    Control->Release();
                                                    pas::free(Control);
                                                }
                                                if (cpp_error) {
                                                    std::rethrow_exception(cpp_error);
                                                }
                                            }
                                            if (GlobalsV::ForcedPlanetQuestId >= 0 || (Self->CurrentPlanet->RaceId == static_cast<std::uint8_t>(aGalaxyStruct::oiMaloc) && (TextQuest->IssuerRaceMask & 1) != 0 || Self->CurrentPlanet->RaceId == static_cast<std::uint8_t>(aGalaxyStruct::oiPeleng) && (TextQuest->IssuerRaceMask & 2) != 0 || Self->CurrentPlanet->RaceId == static_cast<std::uint8_t>(aGalaxyStruct::oiHuman) && (TextQuest->IssuerRaceMask & 4) != 0 || Self->CurrentPlanet->RaceId == static_cast<std::uint8_t>(aGalaxyStruct::oiFeyan) && (TextQuest->IssuerRaceMask & 8) != 0 || Self->CurrentPlanet->RaceId == static_cast<std::uint8_t>(aGalaxyStruct::oiGaal) && (TextQuest->IssuerRaceMask & 16) != 0) && (Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) && (TextQuest->TargetOwnerMask & 0x00000040) != 0 || (TextQuest->TargetOwnerMask & 1) != 0 && Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiMaloc) || (TextQuest->TargetOwnerMask & 2) != 0 && Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPeleng) || (TextQuest->TargetOwnerMask & 4) != 0 && Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiHuman) || (TextQuest->TargetOwnerMask & 8) != 0 && Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiFeyan) || (TextQuest->TargetOwnerMask & 16) != 0 && Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiGaal) || pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&TextQuest->TargetOwnerMask) == pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && Self->CurrentPlanet->OwnerId == Planet->OwnerId) && static_cast<std::uint8_t>(aGalaxy::TGalaxy::HasPlayerQuestHistory(aGalaxyStruct::qtPlanetQuest, Planet->TextQuestId) ^ 1) && ((TextQuest->PlayerCareerMask & 1) != 0 && Self->GetDominantCareer() == aGalaxyStruct::rcTrader || (TextQuest->PlayerCareerMask & 2) != 0 && Self->GetDominantCareer() == aGalaxyStruct::rcPirate || (TextQuest->PlayerCareerMask & 4) != 0 && Self->GetDominantCareer() == aGalaxyStruct::rcWarrior) && ((TextQuest->PlayerRaceMask & 1) != 0 && Self->PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiMaloc) || (TextQuest->PlayerRaceMask & 2) != 0 && Self->PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiPeleng) || (TextQuest->PlayerRaceMask & 4) != 0 && Self->PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiHuman) || (TextQuest->PlayerRaceMask & 8) != 0 && Self->PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiFeyan) || (TextQuest->PlayerRaceMask & 16) != 0 && Self->PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiGaal)) && TextQuest->Difficulty < ([&] {
                                                pas::Extended cpp_right_3 = 3.0E+1L * pas::real_max<float>(1.0f, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].GoodsEventDurationFactor);
                                                return aGalaxy::Galaxy->InterpolateSingleByTechLevel(0.0f, 71.0f) + cpp_right_3;
                                            }())) {
                                                Quest.QuestType = aGalaxyStruct::qtPlanetQuest;
                                                Quest.Planet = Self->CurrentPlanet;
                                                Quest.Successful = false;
                                                Quest.DeadlineTurn = 15 * (Self->CurrentStar->StarDistances[I].Distance / 20 + 1);
                                                if (!TextQuest->CompleteOnFinish) {
                                                    Quest.DeadlineTurn *= 2;
                                                }
                                                Quest.DeadlineTurn += aConst::QuestTuning[Quest.QuestType].BaseDuration;
                                                if (Self->IsHealthEffectActive(24)) {
                                                    Quest.DeadlineTurn = System::Round(Quest.DeadlineTurn * 1.5L);
                                                }
                                                Quest.DeadlineTurn = aGalaxy::Galaxy->CurrentTurn + System::Round(pas::real_divide(Quest.DeadlineTurn, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestTimeAndExperienceFactor));
                                                Quest.RewardMoney = aConst::QuestTuning[Quest.QuestType].BaseRewardMoney + System::Round(([&] {
                                                    pas::Extended cpp_left_3 = aConst::QuestTuning[Quest.QuestType].RewardCapitalPercent * pas::real_min<pas::Extended>(aGalaxy::Galaxy->AverageRangerCapital * 0.01L, aPlayer::GetPlayer()->Wealth * 0.01L);
                                                    return cpp_left_3 * aMyFunction::RemapClamped(TextQuest->Difficulty, 5.0E+1, 1.0E+2, 1.0, 2.1);
                                                }()));
                                                Quest.RewardMoney = System::Round(static_cast<long double>(Quest.RewardMoney) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestMoneyFactor);
                                                if (Self->IsHealthEffectActive(23)) {
                                                    Quest.RewardMoney = System::Round(([&] {
                                                        pas::Extended cpp_right_4 = aMyFunction::SeededRandomFloatRange(pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval), 1.3, 2.3);
                                                        return Quest.RewardMoney * cpp_right_4;
                                                    }()));
                                                }
                                                Quest.RewardMoney += System::Round(Quest.RewardMoney * (Self->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.1L);
                                                Quest.RewardMoney = aMyFunction::RoundAndTruncateToHundreds(Quest.RewardMoney);
                                                Quest.ObjectiveTarget = Planet;
                                                Quest.QuestNumber = Planet->TextQuestId;
                                                Result = true;
                                                Self->RefreshPlayerQuestTargets();
                                                pas::free(TextQuest);
                                                return Result;
                                            }
                                            pas::free(TextQuest);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (static_cast<std::uint8_t>(Self->HasQuestOfType(aGalaxyStruct::qtDefendSystem) ^ 1) && GlobalsV::ForcedPlanetQuestId < 0 && Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] == 0) {
                if (aMyFunction::FractionalQuotient(Self->CurrentPlanet->GenerationSeed, pas::idiv(aGalaxy::Galaxy->CurrentTurn + 444, Interval)) < pas::load_unaligned<float>(pas::byte_offset(&aConst::PlanetGovernmentMarket[Self->CurrentPlanet->Government].QuestOfferProbabilities, 3 * sizeof(float)))) {
                    for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_8.next(I); ) {
                        if (Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stPirate] < 2) {
                            break;
                        }
                        Target = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                        if (Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stPirate] >= 3 || Target->TypeId == aGalaxyStruct::stPirate && pas::class_cast_if<aTransport::TTransport*>(Target->OrderTarget) != nullptr) {
                            MaximumQuest = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"Quest.DefSystem.Count"_wref.get()))) - 1;
                            QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval));
                            Found = false;
                            for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, MaximumQuest); cpp_range_9.next(K); ) {
                                FromFactions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(QuestNumber), ".PlanetRace"})));
                                if (pas::pos(aConst::OwnerToSys(aConst::RaceToOwner(Self->CurrentPlanet->RaceId)), FromFactions) > 0 && (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyPirate", FromFactions) <= 0) && (Self->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyNonPirate", FromFactions) <= 0) && (GR_Main::LanguageDataConfig->CountParamsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"}))) == 0 || ([&] {
                                    const pas::WideString& lookupLocalizedTextByKey_7 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"})));
                                    const pas::WideString& ownerToSys_5 = aConst::OwnerToSys(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                                    return pas::pos(ownerToSys_5, lookupLocalizedTextByKey_7);
                                }()) > 0) && static_cast<std::uint8_t>(aGalaxy::TGalaxy::HasPlayerQuestHistory(aGalaxyStruct::qtDefendSystem, QuestNumber) ^ 1) && ([&] {
                                    const pas::WideString& lookupLocalizedTextByKey_8 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(QuestNumber), ".Status"})));
                                    std::uint8_t cpp_arg_3 = Self->GetDominantCareer();
                                    return aConst::MatchesCareerName(cpp_arg_3, lookupLocalizedTextByKey_8);
                                }())) {
                                    Found = true;
                                    break;
                                }
                                QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval) + 57 * K);
                            }
                            if (Found) {
                                Quest.QuestType = aGalaxyStruct::qtDefendSystem;
                                Quest.Planet = Self->CurrentPlanet;
                                Quest.Successful = false;
                                Quest.DeadlineTurn = ([&] {
                                    std::int32_t cpp_right_5 = aMyFunction::SeededRandomIntRange(-10, 10, Self->CurrentPlanet->GenerationSeed);
                                    return aConst::QuestTuning[Quest.QuestType].BaseDuration + cpp_right_5;
                                }());
                                if (Self->IsHealthEffectActive(24)) {
                                    Quest.DeadlineTurn = System::Round(pas::real_divide(Quest.DeadlineTurn, 1.5L));
                                }
                                Quest.DeadlineTurn = aGalaxy::Galaxy->CurrentTurn + System::Round(static_cast<long double>(Quest.DeadlineTurn) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestTimeAndExperienceFactor);
                                Quest.RewardMoney = aConst::QuestTuning[Quest.QuestType].BaseRewardMoney + System::Round(aConst::QuestTuning[Quest.QuestType].RewardCapitalPercent * pas::real_min<pas::Extended>(aGalaxy::Galaxy->AverageRangerCapital * 0.01L, aPlayer::GetPlayer()->Wealth * 0.01L));
                                Quest.RewardMoney = System::Round(static_cast<long double>(Quest.RewardMoney) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestMoneyFactor);
                                if (Self->IsHealthEffectActive(23)) {
                                    Quest.RewardMoney = System::Round(([&] {
                                        pas::Extended cpp_right_6 = aMyFunction::SeededRandomFloatRange(pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval), 1.3, 2.3);
                                        return Quest.RewardMoney * cpp_right_6;
                                    }()));
                                }
                                Quest.RewardMoney += System::Round(Quest.RewardMoney * (Self->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.1L);
                                Quest.RewardMoney = aMyFunction::RoundAndTruncateToHundreds(Quest.RewardMoney);
                                Quest.ObjectiveTarget = Self->CurrentPlanet->CurrentStar;
                                Quest.QuestNumber = QuestNumber;
                                Result = true;
                                Self->RefreshPlayerQuestTargets();
                                return Result;
                            }
                        }
                    }
                }
            }
            if (static_cast<std::uint8_t>(Self->HasQuestOfType(aGalaxyStruct::qtDefendShip) ^ 1) && GlobalsV::ForcedPlanetQuestId < 0) {
                if (aMyFunction::FractionalQuotient(Self->CurrentPlanet->GenerationSeed, pas::idiv(aGalaxy::Galaxy->CurrentTurn + 555, Interval)) < pas::load_unaligned<float>(pas::byte_offset(&aConst::PlanetGovernmentMarket[Self->CurrentPlanet->Government].QuestOfferProbabilities, 4 * sizeof(float)))) {
                    for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, std::min<std::int32_t>(20, pas::list_count(aGalaxy::Galaxy->Stars) - 1)); cpp_range_10.next(I); ) {
                        Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Self->CurrentStar->StarDistances[I].Star));
                        if (Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->ShipTypeCounts[aGalaxyStruct::stPirate] >= 1 && Star->Constellation->Id != 20 && Star->IsConstellationVisible()) {
                            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_11.next(J); ) {
                                Target = pas::list_at<aShip::TShip>(Star->Ships, J);
                                if (aPlayer::GetPlayer() != Target) {
                                    DefendedShip = Target;
                                    if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate) && Target != Self && Target->ScriptShip == nullptr && (Target->TypeId != aGalaxyStruct::stPirate && Star->ShipTypeCounts[aGalaxyStruct::stPirate] >= 2 || Target->EnemyShip != nullptr && Target->EnemyShip->OrderTarget == Target && Target->EnemyShip->TypeId == aGalaxyStruct::stPirate) && (I <= 0 || Self->CurrentPlanet == DefendedShip->HomePlanet) && Self->CurrentPlanet->OwnerId == DefendedShip->OwnerId && Self->CurrentPlanet->GetRelationLevelToShip(DefendedShip) >= aGalaxyStruct::rlBad && (DefendedShip->Order != aShip::soJump || DefendedShip->EstimateOrderTravelTurns() >= 6)) {
                                        if (DefendedShip->GetHull()->HullPoints >= DefendedShip->GetHull()->Weight * 0.6L && static_cast<std::uint8_t>(DefendedShip->DestroyQueued ^ 1)) {
                                            MaximumQuest = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"Quest.DefShip.Count"_wref.get()))) - 1;
                                            QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval));
                                            Found = false;
                                            for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, MaximumQuest); cpp_range_12.next(K); ) {
                                                FromFactions = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".PlanetRace"})));
                                                if (pas::pos(aConst::OwnerToSys(aConst::RaceToOwner(Self->CurrentPlanet->RaceId)), FromFactions) > 0 && (Self->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyPirate", FromFactions) <= 0) && (Self->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || pas::pos(u"OnlyNonPirate", FromFactions) <= 0) && static_cast<std::uint8_t>(aGalaxy::TGalaxy::HasPlayerQuestHistory(aGalaxyStruct::qtDefendShip, QuestNumber) ^ 1) && (GR_Main::LanguageDataConfig->CountParamsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"}))) == 0 || ([&] {
                                                    const pas::WideString& lookupLocalizedTextByKey_9 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".PlayerRace"})));
                                                    const pas::WideString& ownerToSys_6 = aConst::OwnerToSys(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                                                    return pas::pos(ownerToSys_6, lookupLocalizedTextByKey_9);
                                                }()) > 0) && ([&] {
                                                    const pas::WideString& lookupLocalizedTextByKey_10 = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".Status"})));
                                                    std::uint8_t cpp_arg_4 = Self->GetDominantCareer();
                                                    return aConst::MatchesCareerName(cpp_arg_4, lookupLocalizedTextByKey_10);
                                                }())) {
                                                    if (I == 0 && GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".InThisSystem"}))) == u"Yes" || I > 0 && GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".InThisSystem"}))) == u"No") {
                                                        ShipTypes = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(QuestNumber), ".ShipType"})));
                                                        switch (DefendedShip->TypeId) {
                                                            case aGalaxyStruct::stRanger: {
                                                                if (pas::pos(u"Ranger", ShipTypes) > 0) {
                                                                    Found = true;
                                                                }
                                                                break;
                                                            }
                                                            case aGalaxyStruct::stTransport: {
                                                                switch (pas::checked_cast<aTransport::TTransport*>(DefendedShip)->TransportType) {
                                                                    case aTransport::ttTransport: {
                                                                        if (pas::pos(u"Transport", ShipTypes) > 0) {
                                                                            Found = true;
                                                                        }
                                                                        break;
                                                                    }
                                                                    case aTransport::ttLiner: {
                                                                        if (pas::pos(u"Liner", ShipTypes) > 0) {
                                                                            Found = true;
                                                                        }
                                                                        break;
                                                                    }
                                                                    case aTransport::ttDiplomat: {
                                                                        if (pas::pos(u"Diplomat", ShipTypes) > 0) {
                                                                            Found = true;
                                                                        }
                                                                        break;
                                                                    }
                                                                }
                                                                break;
                                                            }
                                                            case aGalaxyStruct::stPirate: {
                                                                if (pas::pos(u"Pirate", ShipTypes) > 0) {
                                                                    Found = true;
                                                                }
                                                                break;
                                                            }
                                                        }
                                                        if (ShipTypes == u"Any") {
                                                            Found = true;
                                                        }
                                                        if (Found) {
                                                            break;
                                                        }
                                                    }
                                                }
                                                QuestNumber = aMyFunction::SeededRandomIntRange(0, MaximumQuest, pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval) + 67 * K);
                                            }
                                            if (Found) {
                                                Quest.QuestType = aGalaxyStruct::qtDefendShip;
                                                Quest.Planet = Self->CurrentPlanet;
                                                Quest.Successful = false;
                                                Quest.DeadlineTurn = ([&] {
                                                    std::int32_t cpp_right_7 = aMyFunction::SeededRandomIntRange(-10, 10, Self->CurrentPlanet->GenerationSeed);
                                                    return aConst::QuestTuning[Quest.QuestType].BaseDuration + cpp_right_7;
                                                }());
                                                if (Self->IsHealthEffectActive(24)) {
                                                    Quest.DeadlineTurn = System::Round(pas::real_divide(Quest.DeadlineTurn, 1.5L));
                                                }
                                                Quest.DeadlineTurn = aGalaxy::Galaxy->CurrentTurn + System::Round(static_cast<long double>(Quest.DeadlineTurn) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestTimeAndExperienceFactor);
                                                Quest.RewardMoney = aConst::QuestTuning[Quest.QuestType].BaseRewardMoney + System::Round(aConst::QuestTuning[Quest.QuestType].RewardCapitalPercent * pas::real_min<pas::Extended>(aGalaxy::Galaxy->AverageRangerCapital * 0.01L, aPlayer::GetPlayer()->Wealth * 0.01L));
                                                Quest.RewardMoney = System::Round(static_cast<long double>(Quest.RewardMoney) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[5]].QuestMoneyFactor);
                                                if (Self->IsHealthEffectActive(23)) {
                                                    Quest.RewardMoney = System::Round(([&] {
                                                        pas::Extended cpp_right_8 = aMyFunction::SeededRandomFloatRange(pas::idiv(static_cast<std::int32_t>(Self->CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn, Interval), 1.3, 2.3);
                                                        return Quest.RewardMoney * cpp_right_8;
                                                    }()));
                                                }
                                                Quest.RewardMoney += System::Round(Quest.RewardMoney * (Self->GetEffectiveSkillLevel(aShip::psCharisma, false) & 0x0000007f) * 0.1L);
                                                Quest.RewardMoney = aMyFunction::RoundAndTruncateToHundreds(Quest.RewardMoney);
                                                Quest.ObjectiveTarget = DefendedShip;
                                                Quest.QuestNumber = QuestNumber;
                                                Result = true;
                                                Self->RefreshPlayerQuestTargets();
                                                return Result;
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
        ResponseText = aConst::PickLocalizedTextVariant(u"FormGov.DontQuest.WeDontHaveQuest"_wref.get(), Self->CurrentPlanet->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 5) + 1895643);
        return Result;
    }

    pas::WideString TRanger::BuildQuestText(const TQuest& cpp_value_arg, TQuestTextKind Kind) {
        TQuest Quest = cpp_value_arg;
        pas::WideString Result{};
        pas::WideString Text{};
        pas::WideString Suffix{};
        TextQuest::TTextQuest* TextQuest{};
        EC_Cache::TCacheControlEC* Control{};
        EC_CacheBuf::TCBufEC* Buffer{};
        switch (Quest.QuestType) {
            case aGalaxyStruct::qtSendLetter: {
                if (Kind == qtkCompletion) {
                    Suffix = u".End"_w;
                } else {
                    Suffix = u".Start"_w;
                }
                Text = aConst::LocalizedColorText(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest.QuestNumber)})), Suffix}));
                aMyFunction::ReplaceTextToken(Text, u"<ToPlanet>"_w, pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<ToStar>"_w, pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->CurrentStar->Name, u"<color=255,240,100>"_w);
                if (CurrentPlanet != nullptr) {
                    aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::wide_int64_to_str(System::Round(aMyFunction::PointDistance(pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->CurrentStar->Position, CurrentPlanet->CurrentStar->Position))), u"<color=255,240,100>"_w);
                } else {
                    aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::wide_int64_to_str(System::Round(aMyFunction::PointDistance(pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->CurrentStar->Position, CurrentStar->Position))), u"<color=255,240,100>"_w);
                }
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(Quest.DeadlineTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Day>"_w, pas::wide_int_to_str(Quest.DeadlineTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Quest.RewardMoney), u"<color=255,240,100>"_w);
                if (Quest.Planet != nullptr) {
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Quest.Planet->Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, Quest.Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                } else {
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, u"*** \u041a\u0430\u043a\u0430\u044f \u0435\u0449\u0435 \u043f\u043b\u0430\u043d\u0435\u0442\u0430? \u042d\u0442\u043e \u0431\u0430\u0437\u0430!!! ***"_w, u"<color=255,0,0>"_w);
                }
                return Text;
            }
            case aGalaxyStruct::qtKillShip: {
                if (Kind == qtkCompletion) {
                    Suffix = u".End"_w;
                } else {
                    Suffix = u".Start"_w;
                }
                Text = aConst::LocalizedColorText(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"Quest.KillShip.", SysUtils::IntToStr(Quest.QuestNumber)})), Suffix}));
                aMyFunction::ReplaceTextToken(Text, u"<InStar>"_w, pas::checked_cast<aShip::TShip*>(Quest.ObjectiveTarget)->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(Quest.DeadlineTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Day>"_w, pas::wide_int_to_str(Quest.DeadlineTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Quest.RewardMoney), u"<color=255,240,100>"_w);
                if (Quest.Planet != nullptr) {
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Quest.Planet->Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, Quest.Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                }
                aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, pas::checked_cast<aShip::TShip*>(Quest.ObjectiveTarget)->GetName(), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FullShip>"_w, pas::checked_cast<aShip::TShip*>(Quest.ObjectiveTarget)->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                return Text;
            }
            case aGalaxyStruct::qtPlanetQuest: {
                TextQuest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                Control = nullptr;
                {
                    std::exception_ptr cpp_error{};
                    try {
                        Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                        EC_Cache::TCacheEC::ResetControl(Control);
                        Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.", SysUtils::IntToStr(Quest.QuestNumber)})));
                        Buffer = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                        TextQuest->LoadFromReader(Buffer->Buffer, true);
                    } catch (...) {
                        cpp_error = std::current_exception();
                    }
                    if (Control != nullptr) {
                        Control->Release();
                        pas::free(Control);
                    }
                    if (cpp_error) {
                        std::rethrow_exception(cpp_error);
                    }
                }
                if (Kind == qtkCompletion) {
                    Text = TextQuest->QuestSuccessGovMessageText->Text;
                } else {
                    Text = TextQuest->QuestDescriptionText->Text;
                }
                aMyFunction::ReplaceTextToken(Text, u"<Ranger>"_w, aPlayer::GetPlayer()->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<ToPlanet>"_w, pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<ToStar>"_w, pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Parsec>"_w, pas::wide_int64_to_str(System::Round(aMyFunction::PointDistance(Quest.Planet->CurrentStar->Position, pas::checked_cast<aPlanet::TPlanet*>(Quest.ObjectiveTarget)->CurrentStar->Position))), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(Quest.DeadlineTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Day>"_w, pas::wide_int_to_str(Quest.DeadlineTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Quest.RewardMoney), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Quest.Planet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, Quest.Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                aConst::ExpandLocalizedTextMarkup(Text);
                pas::free(TextQuest);
                return Text;
            }
            case aGalaxyStruct::qtDefendSystem: {
                if (Kind == qtkCompletion) {
                    Suffix = u".End"_w;
                } else {
                    Suffix = u".Start"_w;
                }
                Text = aConst::LocalizedColorText(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefSystem.", SysUtils::IntToStr(Quest.QuestNumber)})), Suffix}));
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(Quest.DeadlineTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Day>"_w, pas::wide_int_to_str(Quest.DeadlineTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Quest.RewardMoney), u"<color=255,240,100>"_w);
                if (Quest.Planet != nullptr) {
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Quest.Planet->Name, u"<color=255,240,100>"_w);
                } else {
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, u"*** \u041a\u0430\u043a\u0430\u044f \u0435\u0449\u0435 \u043f\u043b\u0430\u043d\u0435\u0442\u0430? \u042d\u0442\u043e \u0431\u0430\u0437\u0430!!! ***"_w, u"<color=255,0,0>"_w);
                }
                aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, pas::checked_cast<aGalaxy::TStar*>(Quest.ObjectiveTarget)->Name, u"<color=255,240,100>"_w);
                return Text;
            }
            case aGalaxyStruct::qtDefendShip: {
                if (Kind == qtkCompletion) {
                    Suffix = u".End"_w;
                } else if (Kind == qtkProtectedShipLost) {
                    Suffix = u".Special"_w;
                } else {
                    Suffix = u".Start"_w;
                }
                Text = aConst::LocalizedColorText(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"Quest.DefShip.", SysUtils::IntToStr(Quest.QuestNumber)})), Suffix}));
                aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(Quest.DeadlineTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Day>"_w, pas::wide_int_to_str(Quest.DeadlineTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Quest.RewardMoney), u"<color=255,240,100>"_w);
                if (Quest.Planet != nullptr) {
                    aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Quest.Planet->Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, Quest.Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                }
                aMyFunction::ReplaceTextToken(Text, u"<InStar>"_w, pas::checked_cast<aShip::TShip*>(Quest.ObjectiveTarget)->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, pas::checked_cast<aShip::TShip*>(Quest.ObjectiveTarget)->GetName(), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FullShip>"_w, pas::checked_cast<aShip::TShip*>(Quest.ObjectiveTarget)->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                if (aGalaxy::Galaxy->TechLevel < 7 && Suffix == u".Start") {
                    Text = pas::concat_wide({Text, u"\r\n", aConst::LocalizedColorText(u"Quest.DefShip.AddText"_wref.get())});
                }
                return Text;
            }
            default: return Result;
        }
    }

    // Outcome: 0=active, positive=completed, negative=failed.
    void TRanger::PublishQuestStatus(PQuest Quest, std::int32_t Outcome) {
        pas::WideString Text{};
        Globals::TMessagePlayer* Message{};
        Text = pas::WideString();
        if (Outcome == 0) {
            if (Quest->Successful) {
                Text = pas::concat_wide({Text, aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Quest.Info.CurQuests.Accepted"_wref.get()), u"<color=255,240,100>"_w), u"\r\n"});
            } else {
                Text = pas::concat_wide({Text, aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Quest.Info.CurQuests.NotAccepted"_wref.get()), u"<color=255,240,100>"_w), u"\r\n"});
                Text = pas::concat_wide({Text, ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(Quest->DeadlineTurn - aGalaxy::Galaxy->CurrentTurn);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"Quest.Info.CountDay"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Day>"_w, std::move(intToStr));
                }()), u"\r\n"});
            }
        } else if (Outcome > 0) {
            Text = pas::concat_wide({Text, aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Quest.Info.OldQuests.Accepted"_wref.get()), u"<color=0,255,0>"_w), u"\r\n"});
        } else {
            Text = pas::concat_wide({Text, aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"Quest.Info.OldQuests.NotAccepted"_wref.get()), u"<color=255,0,0>"_w), u"\r\n"});
        }
        Text = pas::concat_wide({Text, ([&] {
            auto name = pas::borrow(Quest->Planet->Name);
            auto name_2 = pas::borrow(Quest->Planet->CurrentStar->Name);
            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"Quest.Info.FromPlanet"_wref.get());
            return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Planet>"_w, name.get(), u"<System>"_w, name_2.get());
        }()), u"\r\n"});
        Text = pas::concat_wide({Text, u"\r\n", u" ", u"\r\n", Quest->Description});
        if (Outcome == 0) {
            if (Quest->Planet != nullptr) {
                Message = Globals::AddOrUpdatePlayerBubble(3, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"ZP_", SysUtils::Int64ToStr(Quest->Planet->Id), "_", SysUtils::IntToStr(Quest->QuestNumber)})));
                if (Quest->ObjectiveTarget != nullptr) {
                    if (pas::class_cast_if<aShip::TShip*>(Quest->ObjectiveTarget) != nullptr) {
                        Message->Targets[0].ShipId = reinterpret_cast<aShip::TShip*>(Quest->ObjectiveTarget)->Id;
                    } else if (pas::class_cast_if<aPlanet::TPlanet*>(Quest->ObjectiveTarget) != nullptr) {
                        Message->Targets[0].PlanetId = reinterpret_cast<aPlanet::TPlanet*>(Quest->ObjectiveTarget)->Id;
                    } else if (pas::class_cast_if<aGalaxy::TStar*>(Quest->ObjectiveTarget) != nullptr) {
                        Message->Targets[0].PlanetId = Quest->Planet->Id;
                    }
                }
            }
        } else if (Outcome > 0) {
            if (Quest->Planet != nullptr) {
                Globals::AddOrUpdatePlayerBubble(4, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"ZP_", SysUtils::Int64ToStr(Quest->Planet->Id), "_", SysUtils::IntToStr(Quest->QuestNumber)})));
            }
        } else if (Quest->Planet != nullptr) {
            Globals::AddOrUpdatePlayerBubble(5, aGalaxy::Galaxy->CurrentTurn, Text, static_cast<pas::WideString>(pas::concat_ansi({"ZP_", SysUtils::Int64ToStr(Quest->Planet->Id), "_", SysUtils::IntToStr(Quest->QuestNumber)})));
        }
    }

    void TRanger_ProcessShipDestructionQuests(TRanger* Self, aShip::TShip* Ship) {
        std::int32_t I{};
        std::int32_t J{};
        TRanger* Ranger{};
        PQuest Quest{};
        pas::WideString Text{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<TRanger>(aGalaxy::Galaxy->Rangers, I);
            {
                const std::int32_t cpp_first = pas::list_count(Ranger->Quests) - 1;
                if (cpp_first >= 0) {
                    for (J = cpp_first; J >= 0; --J) {
                        Quest = pas::list_at<TQuest>(Ranger->Quests, J);
                        if (pas::class_cast_if<aTransport::TTransport*>(Ship) != nullptr && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->DestroyQueued ^ 1) && static_cast<std::uint8_t>(Quest->Successful ^ 1) && Quest->QuestType == aGalaxyStruct::qtDefendSystem && pas::checked_cast<aGalaxy::TStar*>(Quest->ObjectiveTarget) == Ship->CurrentStar) {
                            if (Quest->Planet != nullptr && Quest->Planet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlBad) {
                                Quest->Planet->SetRelationLevelToRanger(Ranger, aGalaxyStruct::rlBad);
                            }
                            // Native assumes Planet is present after the guarded update above.
                            if (Quest->Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlBad) {
                                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlNormal) {
                                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlBad);
                                }
                                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlGood) {
                                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlNormal);
                                }
                                if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) >= aGalaxyStruct::rlExcellent) {
                                    aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlGood);
                                }
                            }
                            if (aPlayer::GetPlayer() == Ranger) {
                                Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Failure.DeadShipInDefSystem"_wref.get(), Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Quest->Planet->Name, u"<color=255,240,100>"_w);
                                aMyFunction::ReplaceTextToken(Text, u"<Relation>"_w, Quest->Planet->GetRelationLevelTextToShip(Ranger), u"<color=255,240,100>"_w);
                                aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, Ship->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                                aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, pas::checked_cast<aGalaxy::TStar*>(Quest->ObjectiveTarget)->Name, u"<color=255,240,100>"_w);
                                Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                            }
                            // The original calls these on Self, even while iterating another ranger's quests.
                            TRanger::PublishQuestStatus(Quest, -1);
                            Self->ArchiveQuest(J);
                        }
                        // Native continues reading Quest after that archive; retain the original ordering.
                        if (pas::in_range(Ship->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate) && Quest->QuestType == aGalaxyStruct::qtDefendShip && Ship == Quest->ObjectiveTarget) {
                            if (!Quest->Successful) {
                                if (Quest->Planet != nullptr && Quest->Planet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlBad) {
                                    Quest->Planet->SetRelationLevelToRanger(Ranger, aGalaxyStruct::rlBad);
                                }
                                // Native assumes Planet is present after the guarded update above.
                                if (Quest->Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && aPlanet::MainPiratePlanet != nullptr && aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) > aGalaxyStruct::rlBad) {
                                    if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlNormal) {
                                        aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlBad);
                                    }
                                    if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) == aGalaxyStruct::rlGood) {
                                        aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlNormal);
                                    }
                                    if (aPlanet::MainPiratePlanet->GetRelationLevelToShip(Self) >= aGalaxyStruct::rlExcellent) {
                                        aPlanet::MainPiratePlanet->SetRelationLevelToRanger(Self, aGalaxyStruct::rlGood);
                                    }
                                }
                                if (aPlayer::GetPlayer() == Ranger) {
                                    Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Failure.DeadDefShip"_wref.get(), Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                                    aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Quest->Planet->Name, u"<color=255,240,100>"_w);
                                    aMyFunction::ReplaceTextToken(Text, u"<Relation>"_w, Quest->Planet->GetRelationLevelTextToShip(Ranger), u"<color=255,240,100>"_w);
                                    aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, Ship->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                                    aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, Ship->CurrentStar->Name, u"<color=255,240,100>"_w);
                                    Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                                }
                                TRanger::PublishQuestStatus(Quest, -1);
                                Self->ArchiveQuest(J);
                            } else {
                                Quest->SpecialCompletionText = aPlayer::GetPlayer()->BuildQuestText(*Quest, qtkProtectedShipLost);
                                Quest->ObjectiveTarget = nullptr;
                                Self->RefreshPlayerQuestTargets();
                            }
                        }
                    }
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ranger->Quests) - 1); cpp_range_2.next(J); ) {
                Quest = pas::list_at<TQuest>(Ranger->Quests, J);
                if (static_cast<std::uint8_t>(Quest->Successful ^ 1) && Quest->QuestType == aGalaxyStruct::qtKillShip && Ship == Quest->ObjectiveTarget) {
                    if (aPlayer::GetPlayer() == Ranger) {
                        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Quest.Successful.KillShip"_wref.get(), aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                        aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Quest->Planet->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, Ship->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                        Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                    }
                    Quest->Successful = true;
                    TRanger::PublishQuestStatus(Quest, 0);
                    Quest->ObjectiveTarget = nullptr;
                    Ship->DestroyQueued = true;
                    Self->RefreshPlayerQuestTargets();
                }
            }
        }
    }

    std::uint8_t TRanger::HasQuestOfType(aGalaxyStruct::TQuestType QuestType) {
        std::int32_t I{};
        PQuest Quest{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Quests) - 1); cpp_range.next(I); ) {
            Quest = pas::list_at<TQuest>(Quests, I);
            if (Quest->QuestType == QuestType) {
                return true;
            }
        }
        return false;
    }

    // Player-only; pending history can also preserve unrelated ships.
    std::uint8_t TRanger::ShouldKeepShipForQuests(aShip::TShip* Ship) {
        std::int32_t I{};
        PQuest Quest{};
        PPlayerOldQuest OldQuest{};
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() != this) {
            return Result;
        }
        aPlayer::TPlayer* Player = aPlayer::GetPlayer();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Player->Quests) - 1); cpp_range.next(I); ) {
            Quest = pas::list_at<TQuest>(Player->Quests, I);
            switch (Quest->QuestType) {
                case aGalaxyStruct::qtKillShip: {
                    Result = pas::checked_cast<aShip::TShip*>(Quest->ObjectiveTarget) == Ship;
                    break;
                }
                case aGalaxyStruct::qtDefendShip: {
                    Result = pas::checked_cast<aShip::TShip*>(Quest->ObjectiveTarget) == Ship;
                    break;
                }
            }
            if (Result) {
                return Result;
            }
        }
        if (pas::list_count(PlayerOldQuests) > 0) {
            OldQuest = pas::list_at<TPlayerOldQuest>(PlayerOldQuests, pas::list_count(PlayerOldQuests) - 1);
            if (pas::pos(Ship->Name, OldQuest->Description) > 0) {
                return true;
            }
        }
        return Result;
    }

    // Does nothing for NPC rangers.
    void TRanger::RefreshPlayerQuestTargets() {
        std::int32_t I{};
        PQuest Quest{};
        if (aPlayer::GetPlayer() != this) {
            return;
        }
        aPlayer::TPlayer* Player = aPlayer::GetPlayer();
        Player->QuestTargetKillShip = nullptr;
        Player->QuestTargetDefendShip = nullptr;
        Player->QuestTargetDefendStar = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Player->Quests) - 1); cpp_range.next(I); ) {
            Quest = pas::list_at<TQuest>(Player->Quests, I);
            if (!Quest->Successful) {
                switch (Quest->QuestType) {
                    case aGalaxyStruct::qtKillShip: {
                        Player->QuestTargetKillShip = pas::checked_cast<aShip::TShip*>(Quest->ObjectiveTarget);
                        break;
                    }
                    case aGalaxyStruct::qtDefendShip: {
                        Player->QuestTargetDefendShip = pas::checked_cast<aShip::TShip*>(Quest->ObjectiveTarget);
                        break;
                    }
                    case aGalaxyStruct::qtDefendSystem: {
                        Player->QuestTargetDefendStar = reinterpret_cast<aGalaxy::TStar*>(Quest->ObjectiveTarget);
                        break;
                    }
                }
            }
        }
    }

    void TRanger::p_destroy() {
        aRanger::TRanger_Destroy(this);
    }

    void TRanger::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aRanger::TRanger_ResolveLoadedReferences(this, Galaxy);
    }

    void TRanger::virtual_TShip_NextDay() {
        aRanger::TRanger_NextDay(this);
    }

    void TRanger::virtual_TShip_NextDayLogic() {
        aRanger::TRanger_NextDayLogic(this);
    }

    std::uint8_t TRanger::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aRanger::TRanger_CanQueueReachablePlanet(this, Planet);
    }

    std::uint8_t TRanger::virtual_TShip_RecomputeFearState() {
        return aRanger::TRanger_RecomputeFearState(this);
    }

    std::uint8_t TRanger::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aRanger::TRanger_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TRanger::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aRanger::TRanger_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TRanger::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        return aRanger::TRanger_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TRanger::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aRanger::TRanger_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TRanger::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        return aRanger::TRanger_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TRanger::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aRanger::TRanger_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TRanger::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aRanger::TRanger_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    void TRanger::virtual_TShip_RefreshCurrentStanding() {
        aRanger::TRanger_RefreshCurrentStanding(this);
    }

} // namespace aRanger
