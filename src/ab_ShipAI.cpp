#include "layout/ab_ShipAI.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aNormalShip.hpp"
#include "types/aShip.hpp"
#include "types/aTranclucator.hpp"
#include "types/ab_MainForm.hpp"
#include "types/ab_W.hpp"
#include "units/Achievements.hpp"
#include "units/Globals.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Item.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_Ship.hpp"
#include "units/ab_ShipAI.hpp"
#include "units/ab_Space.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_Zone.hpp"
#include "units/fShip2.hpp"

namespace ab_ShipAI {
    void TabShipAI_Create(TabShipAI* Self) {
        ab_Ship::TabShip_Create(Self);
        Self->CombatManeuver = amUnselected;
        Self->AIEnabled = true;
    }

    void TabShipAI_Destroy(TabShipAI* Self) {
        if (Self->RewardObject != nullptr) {
            pas::free(Self->RewardObject);
            Self->RewardObject = nullptr;
        }
        ab_Ship::TabShip_Destroy(Self);
    }

    aItem::TItem* TabShipAI::GetRewardItem(std::uint8_t Preview) {
        aItem::TItem* Reward{};
        std::int32_t LivingEnemies{};
        std::int32_t Index{};
        std::int32_t RepeatPenalty{};
        std::uint32_t SavedRandomState{};
        std::uint32_t SavedNextItemId{};
        std::uint8_t KellerPresent{};
        std::uint8_t UsedSubportal{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aGalaxy::THole* Hole{};
        std::uint8_t SavedChaoticRandom{};
        aItem::TItem* Result = nullptr;
        if (aPlayer::GetPlayer() != nullptr && ab_Ship::PlayerArcadeShip != nullptr) {
            if (RewardObject != nullptr) {
                if (aItem::TArtefact* artefact = pas::class_cast_if<aItem::TArtefact*>(RewardObject); artefact != nullptr && static_cast<std::uint8_t>(Preview ^ 1)) {
                    Result = static_cast<aItem::TItem*>(artefact);
                    RewardObject = nullptr;
                }
                return Result;
            } else if (!RandomRewardsDisabled) {
                UsedSubportal = false;
                RepeatPenalty = 0;
                if (aPlayer::GetPlayer()->Order == aShip::soJumpHole && aPlayer::GetPlayer()->OrderTarget != nullptr && pas::class_cast_if<aGalaxy::THole*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                    Hole = pas::checked_cast<aGalaxy::THole*>(aPlayer::GetPlayer()->OrderTarget);
                    {
                        const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
                        if (cpp_first >= 0) {
                            for (Index = cpp_first; Index >= 0; --Index) {
                                Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, Index);
                                if (Event->EventType == u"PlayerUsesSubportal" && Event->GetData(0) == static_cast<std::int32_t>(Hole->Id) && Event->GetData(1) == Hole->CreatedTurn) {
                                    UsedSubportal = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (UsedSubportal) {
                    const std::int32_t cpp_first_2 = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
                    if (cpp_first_2 >= 0) {
                        for (Index = cpp_first_2; Index >= 0; --Index) {
                            Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, Index);
                            if (Event->Turn + aConst::SubportalRewardPenaltyTurns < aGalaxy::Galaxy->CurrentTurn) {
                                break;
                            }
                            if (Event->EventType == u"PlayerJumpsThroughSubportal") {
                                RepeatPenalty += aConst::SubportalRewardPenalty;
                            }
                        }
                    }
                }
                Reward = nullptr;
                LivingEnemies = 0;
                KellerPresent = false;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ab_Ship::PlayerArcadeShip->Enemies) - 1); cpp_range.next(Index); ) {
                    if (ab_Ship::KellerArcadeShip != nullptr && pas::list_get(ab_Ship::PlayerArcadeShip->Enemies, Index) == ab_Ship::KellerArcadeShip && ab_Ship::KellerArcadeShip != this) {
                        KellerPresent = true;
                    }
                    if (pas::class_cast_if<ab_Ship::TabShip*>(pas::list_at<pas::Object>(ab_Ship::PlayerArcadeShip->Enemies, Index)) != nullptr && pas::list_at<ab_Ship::TabShip>(ab_Ship::PlayerArcadeShip->Enemies, Index)->Health > 0) {
                        ++LivingEnemies;
                    }
                }
                Index = 0;
                SavedRandomState = RandomState;
                RandomState = InitialRandomSeed;
                SavedChaoticRandom = false;
                SavedNextItemId = 0u;
                if (aGalaxy::Galaxy != nullptr) {
                    SavedChaoticRandom = aGalaxy::Galaxy->CustomRules.ChaoticRandom;
                    aGalaxy::Galaxy->CustomRules.ChaoticRandom = false;
                    SavedNextItemId = aGalaxy::Galaxy->NextItemId;
                }
                if (aPlayer::ArcadeKellerDefeats == 0 && static_cast<std::uint8_t>(KellerPresent ^ 1) && aPlayer::GetPlayer()->Order == aShip::soJumpHole && (LivingEnemies == 0 || Preview) && (LivingEnemies + aPlayer::GetPlayer()->BlackHoleKillCount < 20 || RandomRange(0, 100) > RepeatPenalty + 30)) {
                    while (Reward == nullptr) {
                        ++Index;
                        Reward = aItem::CreateRandomLootItem(aItem::ilpArcadeBattle, aGalaxyStruct::oiUninhabited, aMyFunction::AdvanceRandomSeed(RandomState));
                        if (aPlayer::GetPlayer()->HasMatchingArtefactOrCustomItem(Reward) && Index < 5) {
                            pas::free(Reward);
                            Reward = nullptr;
                            aGalaxy::Galaxy->NextItemId = SavedNextItemId;
                        } else if (aPlayer::GetPlayer()->GetBaseCargoHookPower() > 0 && aPlayer::GetPlayer()->GetBaseCargoHookPower() < Reward->Weight && LivingEnemies + aPlayer::GetPlayer()->BlackHoleKillCount < 5 && Index < 5) {
                            pas::free(Reward);
                            Reward = nullptr;
                            aGalaxy::Galaxy->NextItemId = SavedNextItemId;
                        } else {
                            break;
                        }
                    }
                    if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Reward)) {
                        pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->OwnerShip = aPlayer::GetPlayer();
                    }
                }
                RandomState = SavedRandomState;
                if (aGalaxy::Galaxy != nullptr) {
                    aGalaxy::Galaxy->CustomRules.ChaoticRandom = SavedChaoticRandom;
                    if (Preview) {
                        aGalaxy::Galaxy->NextItemId = SavedNextItemId;
                    }
                }
                if (UsedSubportal && static_cast<std::uint8_t>(Preview ^ 1) && LivingEnemies == 0) {
                    aGalaxyEvent::AddGalaxyEvent(u"PlayerJumpsThroughSubportal"_w, nullptr);
                }
                return Reward;
            } else {
                return Result;
            }
        }
        return Result;
    }

    void TabShipAI::ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) {
        std::int32_t Index{};
        std::int32_t Attempts{};
        aItem::TEquipment* Item{};
        aItem::TItem* Reward{};
        std::int32_t LivingEnemies{};
        float RewardScale{};
        std::uint32_t SavedNextItemId{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aConst::TItemType ItemType{};
        std::int32_t Weight{};
        std::int32_t Level{};
        aConst::PWeaponInfo Info{};
        float MinSize{};
        float MaxSize{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t WeaponTech{};
        auto AddArcadeRewardToList = [&](pas::Object* Item) -> void {
            pas::list_add(Globals::ArcadeBattleScreen->ListedObjects, reinterpret_cast<void*>(Item));
        };
        if (Health > 0) {
            if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->KellerLeaveTurn > 0 && ab_Ship::KellerArcadeShip == this) {
                return;
            }
            ab_Ship::TabShip::ApplyDamage(Amount, Source, Disrupt);
            LastDamageTick = ab_Global::ArcadeTickCount;
            ++RecentHitCount;
            if (aPlayer::GetPlayer() != nullptr && Health <= 0 && ab_Ship::PlayerArcadeShip != nullptr && pas::list_indexof(Enemies, reinterpret_cast<void*>(ab_Ship::PlayerArcadeShip)) < 0) {
                if (aGalaxy::Galaxy != nullptr) {
                    if (ScriptLabel != u"") {
                        Event = aGalaxyEvent::AddGalaxyEvent(u"LabeledShipKilledInAB"_w, nullptr);
                        Event->AddTextData(ScriptLabel);
                    }
                }
            } else if (aPlayer::GetPlayer() != nullptr && Health <= 0 && ab_Ship::PlayerArcadeShip != nullptr && pas::list_indexof(Enemies, reinterpret_cast<void*>(ab_Ship::PlayerArcadeShip)) >= 0) {
                aGalaxy::Galaxy->CheckIntegrityChecksum1(619);
                LivingEnemies = 0;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ab_Ship::PlayerArcadeShip->Enemies) - 1); cpp_range.next(Index); ) {
                    if (pas::class_cast_if<ab_Ship::TabShip*>(pas::list_at<pas::Object>(ab_Ship::PlayerArcadeShip->Enemies, Index)) != nullptr && pas::list_at<ab_Ship::TabShip>(ab_Ship::PlayerArcadeShip->Enemies, Index)->Health > 0) {
                        ++LivingEnemies;
                    }
                }
                RandomState = InitialRandomSeed;
                if (aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
                    ++aPlayer::GetPlayer()->BlackHoleKillCount;
                    Achievements::TryAddAchievementProgress(u"HOLEMAN"_w, 1);
                } else {
                    ++aPlayer::GetPlayer()->HyperspaceKillCount;
                    Achievements::TryAddAchievementProgress(u"HOLEMAN"_w, 1);
                    if (aPlayer::GetPlayer()->InHyperspace && aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                        aPlayer::GetPlayer()->AddRankPoints(2);
                    }
                }
                if (aGalaxy::Galaxy != nullptr && ScriptLabel != u"") {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"LabeledShipKilledInAB"_w, nullptr);
                    Event->AddTextData(ScriptLabel);
                }
                Reward = GetRewardItem(false);
                if (Reward != nullptr) {
                    if (ab_Ship::KellerArcadeShip == this) {
                        aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnABItemDrop, Reward, nullptr, 0);
                        aPlayer::ArcadeKellerReward = Reward;
                    } else {
                        fShip2::ClearPlayerHoldEntries();
                        aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnABItemDrop, Reward, nullptr, 0);
                        if (pas::class_cast_if<aItem::TArtefact*>(Reward) != nullptr) {
                            pas::list_insert(aPlayer::GetPlayer()->Artefacts, 0, reinterpret_cast<void*>(Reward));
                        } else {
                            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Reward));
                        }
                    }
                    AddArcadeRewardToList(Reward);
                } else if (RewardObject != nullptr) {
                    fShip2::ClearPlayerHoldEntries();
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnABItemDrop, RewardObject, nullptr, 0);
                    if (pas::class_cast_if<aItem::TArtefact*>(RewardObject) != nullptr) {
                        pas::list_insert(aPlayer::GetPlayer()->Artefacts, 0, reinterpret_cast<void*>(RewardObject));
                    } else {
                        pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(RewardObject));
                    }
                    AddArcadeRewardToList(RewardObject);
                    RewardObject = nullptr;
                    aPlayer::GetPlayer()->RefreshDerivedStats(true);
                } else if ((LivingEnemies == 0 || LivingEnemies == 1 && ([&] {
                    pas::Extended cpp_left = RandomRange(0, 100);
                    return cpp_left * LuckScale;
                }()) > 5.0E+1L || LivingEnemies >= 2 && ([&] {
                    pas::Extended cpp_left_2 = RandomRange(0, 100);
                    return cpp_left_2 * LuckScale;
                }()) > 6.0E+1L) && static_cast<std::uint8_t>(RandomRewardsDisabled ^ 1)) {
                    Attempts = 0;
                    RewardScale = static_cast<long double>(aGalaxy::Galaxy->GetArcadeDropValueModifier()) * aMyFunction::RemapClamped(LivingEnemies, 0.0, 5.0, 1.56, 0.26) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].ArcadeRewardScale;
                    if (aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
                        if (LivingEnemies == 0) {
                            RewardScale = RewardScale * 3.0L;
                        } else {
                            RewardScale = 1.2L * RewardScale;
                        }
                    } else {
                        RewardScale = static_cast<long double>(aMyFunction::RemapClamped(static_cast<long double>(ab_Space::CurrentArcadeSpace->Danger) + ab_Space::CurrentArcadeSpace->ApproachDanger, 5.0E+1, 2.5E+2, 0.3, 2.0)) * RewardScale;
                    }
                    MinSize = aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, aConst::EquipmentSizeFactors[4], aConst::EquipmentSizeFactors[5]);
                    MaxSize = aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, aConst::EquipmentSizeFactors[2], aConst::EquipmentSizeFactors[4]);
                    MinLevel = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 0.75) * 7.0L + 1.0L);
                    MaxLevel = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 1.0, 7.0, 0.5, 1.0) * 7.0L + 1.0L);
                    SavedNextItemId = aGalaxy::Galaxy->NextItemId;
                    while (true) {
                        if (RandomRange(1, 110) > 70) {
                            WeaponTech = RandomRange(std::max<std::int32_t>(1, aGalaxy::Galaxy->TechLevel - 1), std::min<std::int32_t>(8, aGalaxy::Galaxy->TechLevel + 1));
                            {
                                std::uint32_t randomRange = RandomRange(1, 100000);
                                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                                Info = galaxy->SelectWeaponInfo(randomRange, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}}), std::min<std::int32_t>(WeaponTech + 1, 8), std::max<std::int32_t>(1, WeaponTech - 1));
                            }
                            Weight = ([&] {
                                std::int32_t round = System::Round(static_cast<long double>(Info->AverageSize) * MaxSize);
                                std::int32_t round_2 = System::Round(static_cast<long double>(Info->AverageSize) * MinSize);
                                return RandomRange(round_2, round);
                            }());
                            Level = RandomRange(MinLevel, MaxLevel);
                            Item = aItem::CreateGeneratedWeapon(Info, Weight, Level, aGalaxyStruct::oiUninhabited);
                        } else {
                            ItemType = static_cast<aConst::TItemType>(aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}})));
                            {
                                std::int32_t round_3 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MaxSize);
                                std::int32_t round_4 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MinSize);
                                Weight = RandomRange(round_4, round_3);
                            }
                            Level = RandomRange(MinLevel, MaxLevel);
                            Item = aItem::CreateGeneratedEquipment(ItemType, Weight, Level, aGalaxyStruct::oiUninhabited);
                        }
                        Item->ConditionPercent = aMyFunction::SeededRandomFloatRange(Item->Id * (Attempts + 11) * 123, 1.0E+1, 1.0E+2);
                        ++Attempts;
                        if (Attempts > 10000) {
                            fShip2::ClearPlayerHoldEntries();
                            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnABItemDrop, Item, nullptr, 0);
                            pas::list_insert(aPlayer::GetPlayer()->Inventory, 1, reinterpret_cast<void*>(Item));
                            aPlayer::GetPlayer()->RefreshDerivedStats(true);
                            AddArcadeRewardToList(Item);
                            break;
                        }
                        if (aPlayer::GetPlayer()->GetBaseCargoHookPower() > 0 && aPlayer::GetPlayer()->GetBaseCargoHookPower() < Item->Weight && aPlayer::GetPlayer()->BlackHoleKillCount + aPlayer::GetPlayer()->HyperspaceKillCount < 17) {
                            pas::free(Item);
                            aGalaxy::Galaxy->NextItemId = SavedNextItemId;
                            continue;
                        }
                        if (([&] {
                            pas::Extended real_max = pas::real_max<pas::Extended>(8.0E+2L, static_cast<long double>(aPlayer::GetPlayer()->Wealth) * aMyFunction::RemapClamped(Attempts, 0.0, 5.0E+2, 0.01L * RewardScale, 0.03L * RewardScale));
                            return Item->GetConditionAdjustedCost() < real_max;
                        }()) && (Item->GetConditionAdjustedCost() > aPlayer::GetPlayer()->Wealth * 0.008L * RewardScale || Attempts > 500)) {
                            fShip2::ClearPlayerHoldEntries();
                            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnABItemDrop, Item, nullptr, 0);
                            pas::list_insert(aPlayer::GetPlayer()->Inventory, 1, reinterpret_cast<void*>(Item));
                            aPlayer::GetPlayer()->RefreshDerivedStats(true);
                            AddArcadeRewardToList(Item);
                            break;
                        }
                        pas::free(Item);
                        aGalaxy::Galaxy->NextItemId = SavedNextItemId;
                    }
                }
                if (ab_Ship::KellerArcadeShip == this) {
                    ++aPlayer::ArcadeKellerDefeats;
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(620);
            } else if (Source != nullptr) {
                if (TargetShip != Source && pas::class_cast_if<ab_Ship::TabShip*>(Source) != nullptr && pas::list_indexof(Enemies, reinterpret_cast<void*>(Source)) >= 0) {
                    if (TargetShip == nullptr || ([&] {
                        pas::Extended cpp_left_3 = DistanceTo(TargetShip);
                        return cpp_left_3 > MaximumWeaponRange();
                    }())) {
                        TargetShip = static_cast<ab_Ship::TabShip*>(Source);
                    }
                }
            }
        }
    }

    void TabShipAI::UpdateState() {
        ab_Ship::TabShip::UpdateState();
        InsideCurrentZone = ab_Zone::ab_Zone_FindContainingOrNearest(State.LongitudeDegrees, State.PolarAngleDegrees, CurrentZone);
        HeadingInsideCurrentZone = false;
        CurrentZoneBearing = 0.0;
        if (CurrentZone != nullptr) {
            if (!InsideCurrentZone) {
                HeadingInsideCurrentZone = ab_Zone::ab_Zone_IsHeadingInside(State, CurrentZone, CurrentZoneBearing, CurrentZoneAngularRadius);
            }
        }
    }

    void TabShipAI::Advance() {
        double ForwardDistance{};
        double BackwardDistance{};
        std::int32_t Attempt{};
        std::int32_t Index{};
        ab_Ship::TabShip::Advance();
        if (AIEnabled && Health > 0) {
            if ((ab_Global::ArcadeTickCount & 31) == 0) {
                RecentHitCount = 0;
            }
            if (ab_Ship::PlayerArcadeShip != this || ab_Global::ArcadeAutopilotEnabled) {
                if (WeaponCount > 1 && (ab_Global::ArcadeTickCount & 31) == 0 && Weapons[PrimaryWeapon].Ammo < pas::real_divide(Weapons[PrimaryWeapon].MaxAmmo, 4.0L)) {
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 2); cpp_range.next(Attempt); ) {
                        Index = RandomRange(0, WeaponCount - 1);
                        if (Weapons[Index].Ammo > Weapons[Index].MaxAmmo * 0.9L || (Weapons[Index].Kind == 13 || Weapons[Index].Kind == 14) && Weapons[Index].Ammo > Weapons[Index].MaxAmmo * 0.7L) {
                            SelectWeapon(Index);
                            break;
                        }
                    }
                }
                if (TargetShip != nullptr && pas::list_indexof(Enemies, reinterpret_cast<void*>(TargetShip)) < 0) {
                    TargetShip = nullptr;
                }
                if (TargetShip != nullptr && TargetShip->Health <= 0) {
                    TargetShip = nullptr;
                }
                if (TargetShip != nullptr && TargetShip->BonusTicks[ab_Global::abkInvisibility] > 0 && TargetShip->RevealTicks <= 0) {
                    TargetShip = nullptr;
                }
                if (TargetShip == nullptr) {
                    TargetShip = FindNearestEnemy(this);
                }
                if (TargetShip != nullptr) {
                    TargetBearing = BearingAndDistanceTo(TargetShip);
                    ReverseTargetBearing = TargetShip->BearingAndDistanceTo(this);
                }
                TargetPathClear = false;
                if (TargetShip != nullptr) {
                    ab_StopLine::ab_StopLine_GetDistances(ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees, aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + TargetBearing.BearingDeltaDegrees)), pas::Var<double>(&ForwardDistance), pas::Var<double>(&BackwardDistance));
                    TargetPathClear = TargetBearing.Distance < ForwardDistance;
                }
                DecideActions();
            }
        }
    }

    void TabShipAI::NoticeCollision() {
        if (ab_Ship::PlayerArcadeShip == this || CurrentZone == nullptr) {
            AvoidanceZone = nullptr;
            return;
        }
        AvoidanceZone = ab_Zone::ab_Zone_RandomRoute(CurrentZone, 2);
    }

    void TabShipAI::NoticeDamagingZone(ab_Zone::PabZone Zone) {
        if (aMyFunction::RandomIntRange(0, 10) == 0) {
            DamagingZone = Zone;
        }
    }

    void TabShipAI::ResetIntent() {
        Intent = 0;
        TargetShip = nullptr;
        TargetBonus = nullptr;
        BonusRouteZone = nullptr;
        AvoidanceZone = nullptr;
        DamagingZone = nullptr;
        LastDamageTick = 0;
        RetreatRequested = false;
    }

    void TabShipAI::DecideActions() {
        std::int32_t Index{};
        std::int32_t Score{};
        std::int32_t BestScore{};
        ab_Object::TabObject* Enemy{};
        ab_Item::TabItem* Bonus{};
        ab_Zone::PabZone Zone{};
        ab_Object::TabObject* Obj{};
        double NearestDistance{};
        ab_Global::TSphericalBearingDistance Info{};
        ab_Global::TSphericalBearingDistance ZoneInfo{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        DirectPathClear = false;
        IncomingThreat = false;
        if (ab_Ship::PlayerArcadeShip == this) {
            NearestDistance = 1.0E+30;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Enemies) - 1); cpp_range.next(Index); ) {
                Enemy = pas::list_at<ab_Object::TabObject>(Enemies, Index);
                Info = Enemy->BearingAndDistanceTo(this);
                NearestDistance = pas::real_min<double>(NearestDistance, Info.Distance);
                if (Enemy != TargetShip && std::fabs(static_cast<pas::Extended>(Info.BearingDeltaDegrees)) < 5.0L && Info.Distance < 4.0E+2L) {
                    IncomingThreat = true;
                }
            }
        }
        ClearRoute();
        if (TargetShip != nullptr) {
            SetRoute(pas::checked_cast<TabShipAI*>(TargetShip)->CurrentZone);
        }
        FollowDirectDestination();
        if (TargetPathClear) {
            if (ab_Global::ArcadeTickCount > ManeuverUntilTick) {
                CombatManeuver = amUnselected;
            }
            if (CombatManeuver == amApproach && ScoreApproach() < 0) {
                CombatManeuver = amUnselected;
            }
            if (CombatManeuver == amCloseEvasion && ScoreCloseEvasion() < 0) {
                CombatManeuver = amUnselected;
            }
            if (CombatManeuver == amFlank && ScoreFlanking() < 0) {
                CombatManeuver = amUnselected;
            }
            if (CombatManeuver == amReverseTurn && ScoreReverseTurn() < 0) {
                CombatManeuver = amUnselected;
            }
            if (CombatManeuver == amFollowReverse && ScoreReverseFollowing() < 0) {
                CombatManeuver = amUnselected;
            }
            if (CombatManeuver < 0) {
                BestScore = 0;
                Score = ScoreApproach();
                if (Score > BestScore) {
                    BestScore = Score;
                    CombatManeuver = amApproach;
                    ManeuverUntilTick = ab_Global::ArcadeTickCount + 100;
                }
                Score = ScoreCloseEvasion();
                if (Score > BestScore) {
                    BestScore = Score;
                    CombatManeuver = amCloseEvasion;
                    ManeuverUntilTick = ab_Global::ArcadeTickCount + 100;
                }
                Score = ScoreFlanking();
                if (Score > BestScore) {
                    BestScore = Score;
                    CombatManeuver = amFlank;
                    ManeuverUntilTick = ab_Global::ArcadeTickCount + 100;
                }
                Score = ScoreReverseTurn();
                if (Score > BestScore) {
                    BestScore = Score;
                    CombatManeuver = amReverseTurn;
                    ManeuverUntilTick = ab_Global::ArcadeTickCount + 100;
                }
                Score = ScoreReverseFollowing();
                if (Score > BestScore) {
                    CombatManeuver = amFollowReverse;
                    ManeuverUntilTick = ab_Global::ArcadeTickCount + 500;
                }
            }
            if (CombatManeuver == amApproach) {
                ApproachTarget();
            } else if (CombatManeuver == amCloseEvasion) {
                EvadeCloseTarget();
            } else if (CombatManeuver == amFlank) {
                FlankTarget();
            } else if (CombatManeuver == amReverseTurn) {
                ReverseTowardTarget();
            } else if (CombatManeuver == amFollowReverse) {
                MatchReversingTarget();
            }
        } else {
            CombatManeuver = amUnselected;
            ManeuverUntilTick = 0;
            FollowRoute();
        }
        if (AvoidanceZone == nullptr && ab_Global::ArcadeTickCount - LastDamageTick < 100 && (aMyFunction::RandomIntRange(0, 120) == 0 || BonusTicks[ab_Global::abkWeaponLock] > 0 && aMyFunction::RandomIntRange(0, 20) == 0 || TargetShip == nullptr && aMyFunction::RandomIntRange(0, 20) == 0 || ab_Ship::PlayerArcadeShip == this && Health < MaxHealth * 0.3L && aMyFunction::RandomIntRange(0, 20) == 0)) {
            AvoidanceZone = ab_Zone::ab_Zone_RandomRoute(CurrentZone, 2);
        }
        if (AvoidanceZone != nullptr && (AvoidanceZone == CurrentZone || IncomingThreat || aMyFunction::RandomIntRange(0, 100) == 0 || RecentHitCount >= 3 || static_cast<std::uint8_t>(TryMoveToDestination(AvoidanceZone, AvoidanceZone->Longitude, AvoidanceZone->PolarAngle) ^ 1))) {
            AvoidanceZone = nullptr;
        }
        if (RetreatRequested && aMyFunction::RandomIntRange(0, 50) == 0) {
            do {
                std::int32_t randomIntRange = aMyFunction::RandomIntRange(3, 4);
                ab_Zone::PabZone currentZone = CurrentZone;
                Zone = ab_Zone::ab_Zone_RandomRoute(currentZone, randomIntRange);
            } while (!(Zone != AvoidanceZone));
            AvoidanceZone = Zone;
        }
        if (TargetBonus == nullptr && static_cast<std::uint8_t>(RetreatRequested ^ 1) && (TargetShip == nullptr && aMyFunction::RandomIntRange(0, 20) == 0 || BonusTicks[ab_Global::abkWeaponLock] > 0 && aMyFunction::RandomIntRange(0, 20) == 0 || ab_Ship::PlayerArcadeShip != this && aMyFunction::RandomIntRange(0, 500) == 0 || Health < MaxHealth * 0.4L && aMyFunction::RandomIntRange(0, 80) == 0)) {
            if (ab_Ship::PlayerArcadeShip == this) {
                TargetBonus = ab_Item::ab_Item_FindRepairRoute(CurrentZone, BonusRouteZone);
                if (TargetBonus == nullptr) {
                    TargetBonus = ab_Item::ab_Item_FindBonusRoute(CurrentZone, BonusRouteZone);
                }
            } else {
                TargetBonus = ab_Item::ab_Item_FindBonusRoute(CurrentZone, BonusRouteZone);
                if (TargetBonus != nullptr) {
                    Obj = ab_Object::FirstArcadeObject;
                    while (Obj != nullptr) {
                        if (Obj != this && pas::class_cast_if<TabShipAI*>(Obj) != nullptr && static_cast<TabShipAI*>(Obj)->TargetBonus == TargetBonus) {
                            TargetBonus = nullptr;
                            break;
                        }
                        Obj = Obj->Next;
                    }
                }
            }
        }
        if (TargetBonus != nullptr && (RecentHitCount >= 3 || IncomingThreat && aMyFunction::RandomIntRange(0, 50) == 0 || static_cast<std::uint8_t>(TryMoveToDestination(BonusRouteZone, TargetBonus->State.LongitudeDegrees, TargetBonus->State.PolarAngleDegrees) ^ 1))) {
            TargetBonus = nullptr;
        }
        if (RouteZone != nullptr) {
            Bonus = ab_Item::ab_Item_FindNearestBonus(RouteZone);
            if (Bonus != nullptr) {
                cpp_with = BearingAndDistanceTo(Bonus);
                if (std::fabs(static_cast<pas::Extended>(cpp_with.BearingDeltaDegrees)) < 75.0L) {
                    SetDirectDestination(Bonus->State.LongitudeDegrees, Bonus->State.PolarAngleDegrees);
                    FollowDirectDestination();
                }
            }
        }
        if (pas::list_count(Enemies) <= 1 && ab_Ship::KellerArcadeShip != nullptr && ab_Ship::KellerArcadeShip->Health == 0) {
            SetDirectDestination(ab_Ship::KellerArcadeShip->State.LongitudeDegrees, ab_Ship::KellerArcadeShip->State.PolarAngleDegrees);
            FollowDirectDestination();
        }
        if (DamagingZone != nullptr) {
            ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&ZoneInfo.BearingDeltaDegrees), pas::Var<double>(&ZoneInfo.Distance), State.LongitudeDegrees, State.PolarAngleDegrees, State.BearingDegrees, DamagingZone->Longitude, DamagingZone->PolarAngle, ab_Global::SphereRadius);
            if (std::fabs(static_cast<pas::Extended>(ZoneInfo.BearingDeltaDegrees)) < 9.0E+1L) {
                StopThrust();
            } else {
                StartThrust();
            }
            if (ZoneInfo.Distance > (static_cast<long double>(DamagingZone->Radius) + ZoneRadius) * 1.4L) {
                DamagingZone = nullptr;
            } else {
                SetTurnInput(-ZoneInfo.BearingDeltaDegrees);
            }
        }
        AvoidImmediateObstacle();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Enemies) - 1); cpp_range_2.next(Index); ) {
            Enemy = pas::list_at<ab_Object::TabObject>(Enemies, Index);
            Info = BearingAndDistanceTo(Enemy);
            if (PrimaryWeapon >= 0) {
                if (Weapons[PrimaryWeapon].Kind == 13) {
                    FirePrimary();
                } else if (Weapons[PrimaryWeapon].Kind == 14) {
                    FirePrimaryAt(Enemy);
                } else if (Weapons[PrimaryWeapon].Kind == 12) {
                    FirePrimaryAt(Enemy);
                } else if (Weapons[PrimaryWeapon].Kind == 17) {
                    FirePrimary();
                } else if (std::fabs(static_cast<pas::Extended>(Info.BearingDeltaDegrees)) < 5.0L) {
                    if (static_cast<std::uint8_t>(pas::in_set<1, 2, 6, 6, 8, 8>(Weapons[PrimaryWeapon].Kind) ^ 1) || Thrust <= 1.25L || std::fabs(static_cast<pas::Extended>(TurnInput)) >= 0.01L) {
                        FirePrimaryAt(Enemy);
                    }
                }
            }
            if (SecondaryWeapon >= 0) {
                if (Weapons[SecondaryWeapon].Kind == 13) {
                    FireSecondary();
                } else if (Weapons[SecondaryWeapon].Kind == 14) {
                    FireSecondaryAt(Enemy);
                } else if (Weapons[SecondaryWeapon].Kind == 12) {
                    FireSecondaryAt(Enemy);
                } else if (std::fabs(static_cast<pas::Extended>(Info.BearingDeltaDegrees)) < 5.0L) {
                    if (static_cast<std::uint8_t>(pas::in_set<1, 2, 6, 6, 8, 8>(Weapons[SecondaryWeapon].Kind) ^ 1) || Thrust <= 1.25L || std::fabs(static_cast<pas::Extended>(TurnInput)) >= 0.01L) {
                        FireSecondaryAt(Enemy);
                    }
                }
            }
        }
    }

    void TabShipAI::SetDirectDestination(float Longitude, float PolarAngle) {
        double ForwardDistance{};
        double BackwardDistance{};
        DirectTargetLongitude = Longitude;
        DirectTargetPolarAngle = PolarAngle;
        DirectPathClear = false;
        ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&DirectBearing.BearingDeltaDegrees), pas::Var<double>(&DirectBearing.Distance), State.LongitudeDegrees, State.PolarAngleDegrees, State.BearingDegrees, DirectTargetLongitude, DirectTargetPolarAngle, ab_Global::SphereRadius);
        ab_StopLine::ab_StopLine_GetDistances(ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees, aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + DirectBearing.BearingDeltaDegrees)), pas::Var<double>(&ForwardDistance), pas::Var<double>(&BackwardDistance));
        DirectPathClear = DirectBearing.Distance < ForwardDistance;
    }

    void TabShipAI::FollowDirectDestination() {
        if (DirectPathClear) {
            SetTurnInput(DirectBearing.BearingDeltaDegrees);
            if (std::fabs(static_cast<pas::Extended>(DirectBearing.BearingDeltaDegrees)) < 45.0L) {
                Thrust = aMyFunction::RemapClamped(std::fabs(static_cast<pas::Extended>(DirectBearing.BearingDeltaDegrees)), 0.0, 45.0, 2.5, 0.0);
            } else {
                StopThrust();
            }
        }
    }

    void TabShipAI::AvoidImmediateObstacle() {
        if (Thrust > 0.0L && (ObstacleLevels[0] >= 3 || ObstacleLevels[1] >= 3 || ObstacleLevels[7] >= 3)) {
            if (ObstacleLevels[1] < ObstacleLevels[7]) {
                SetTurnInput(1.0E+2);
            } else if (ObstacleLevels[7] < ObstacleLevels[1]) {
                SetTurnInput(-1.0E+2);
            }
        }
    }

    void TabShipAI::ClearRoute() {
        RouteZone = nullptr;
        HeadingInsideRoute = false;
    }

    void TabShipAI::SetRoute(ab_Zone::PabZone Target) {
        RouteZone = ab_Zone::ab_Zone_GetRoute(CurrentZone, Target);
        HeadingInsideRoute = false;
        if (RouteZone != nullptr) {
            HeadingInsideRoute = ab_Zone::ab_Zone_IsHeadingInside(State, RouteZone, RouteBearing, RouteAngularRadius);
        }
    }

    void TabShipAI::FollowRoute() {
        if (RouteZone != nullptr) {
            if (HeadingInsideRoute && RouteZone != nullptr) {
                StartThrust();
                if (std::fabs(static_cast<pas::Extended>(RouteBearing)) < pas::real_divide(RouteAngularRadius, 2.0L)) {
                    SetTurnInput(0.0);
                } else if (RouteBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (RouteBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
                return;
            }
            if (RouteZone != nullptr && static_cast<std::uint8_t>(ab_StopLine::ab_StopLine_IsBlocked(State.LongitudeDegrees, State.PolarAngleDegrees, RouteZone->Longitude, RouteZone->PolarAngle) ^ 1)) {
                if (RouteBearing < 45.0L) {
                    StartThrust();
                } else {
                    StopThrust();
                }
                if (std::fabs(static_cast<pas::Extended>(RouteBearing)) < pas::real_divide(RouteAngularRadius, 2.0L)) {
                    SetTurnInput(0.0);
                } else if (RouteBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (RouteBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
                return;
            }
            if (!InsideCurrentZone) {
                if (HeadingInsideCurrentZone) {
                    StartThrust();
                    if (std::fabs(static_cast<pas::Extended>(CurrentZoneBearing)) < pas::real_divide(CurrentZoneAngularRadius, 2.0L)) {
                        SetTurnInput(0.0);
                    } else if (CurrentZoneBearing < 0.0L) {
                        SetTurnInput(-1.0E+2);
                    } else if (CurrentZoneBearing > 0.0L) {
                        SetTurnInput(1.0E+2);
                    }
                    return;
                }
                if (CurrentZoneBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (CurrentZoneBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
                return;
            }
            if (RouteZone != nullptr) {
                if (RouteBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (RouteBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
            }
        }
    }

    void TabShipAI::ApproachTarget() {
        CombatManeuver = amApproach;
        SetTurnInput(TargetBearing.BearingDeltaDegrees);
        if (MinimumWeaponRange() * 0.8L < TargetBearing.Distance) {
            Thrust = aMyFunction::RemapClamped(std::fabs(static_cast<pas::Extended>(TargetBearing.BearingDeltaDegrees)), 0.0, 1.8E+2, 2.5, 0.0);
            Thrust = static_cast<long double>(aMyFunction::RemapClamped(TargetBearing.Distance, 1.0E+2, 1.0E+3, 0.5, 1.0)) * Thrust;
            return;
        }
        StartReverseThrust();
    }

    std::int32_t TabShipAI::ScoreApproach() {
        std::int32_t Result = 1;
        Result += RandomRange(-1, 1);
        return Result;
    }

    void TabShipAI::EvadeCloseTarget() {
        CombatManeuver = amCloseEvasion;
        if (std::fabs(static_cast<pas::Extended>(ReverseTargetBearing.BearingDeltaDegrees)) < 2.0E+1L) {
            SetTurnInput(-TargetBearing.BearingDeltaDegrees);
        } else {
            SetTurnInput(TargetBearing.BearingDeltaDegrees);
        }
        StartReverseThrust();
    }

    std::int32_t TabShipAI::ScoreCloseEvasion() {
        std::int32_t Result = 0;
        if (TargetBearing.Distance < 2.0E+2L && std::fabs(static_cast<pas::Extended>(TargetBearing.BearingDeltaDegrees)) < 4.0E+1L) {
            ++Result;
            ++Result;
            return Result;
        } else if (std::fabs(static_cast<pas::Extended>(TargetBearing.BearingDeltaDegrees)) > 9.0E+1L) {
            return -1;
        } else {
            return Result;
        }
    }

    void TabShipAI::FlankTarget() {
        CombatManeuver = amFlank;
        if (TargetBearing.BearingDeltaDegrees > 0.0L) {
            SetTurnInput(TargetBearing.BearingDeltaDegrees - 65.0L);
        } else {
            SetTurnInput(TargetBearing.BearingDeltaDegrees + 65.0L);
        }
        StartThrust();
        Thrust = static_cast<long double>(aMyFunction::RemapClamped(TargetBearing.Distance, 1.0E+2, 1.0E+3, 0.5, 1.0)) * Thrust;
    }

    std::int32_t TabShipAI::ScoreFlanking() {
        std::int32_t Result = RandomRange(0, 2);
        if (std::fabs(static_cast<pas::Extended>(ReverseTargetBearing.BearingDeltaDegrees)) < 3.0E+1L && TargetBearing.Distance < 5.0E+2L) {
            ++Result;
            if (std::fabs(static_cast<pas::Extended>(TargetBearing.BearingDeltaDegrees)) > std::fabs(static_cast<pas::Extended>(ReverseTargetBearing.BearingDeltaDegrees))) {
                ++Result;
            }
            if (ab_Ship::PlayerArcadeShip == this && Health < System::Round(MaxHealth * 0.4L)) {
                ++Result;
            }
            if (IncomingThreat) {
                ++Result;
            }
            Result += RandomRange(-1, 1);
        }
        if (TargetBearing.Distance > 5.0E+2L) {
            --Result;
        }
        if (Health > MaxHealth * 0.7L) {
            Result -= 2;
        }
        return Result;
    }

    void TabShipAI::ReverseTowardTarget() {
        CombatManeuver = amReverseTurn;
        SetTurnInput(TargetBearing.BearingDeltaDegrees);
        StartReverseThrust();
    }

    std::int32_t TabShipAI::ScoreReverseTurn() {
        std::int32_t Result = 0;
        if (std::fabs(static_cast<pas::Extended>(TargetBearing.BearingDeltaDegrees)) > 1.2E+2L && (ObstacleLevels[3] == 0 || ObstacleLevels[4] == 0 || ObstacleLevels[5] == 0)) {
            Result += 3;
            if (ab_Ship::PlayerArcadeShip == this && Health < System::Round(MaxHealth * 0.4L)) {
                ++Result;
            }
            Result += RandomRange(-2, 2);
            return Result;
        }
        return -1;
    }

    void TabShipAI::MatchReversingTarget() {
        StartReverseThrust();
    }

    std::int32_t TabShipAI::ScoreReverseFollowing() {
        std::int32_t Result = 0;
        if (std::fabs(static_cast<pas::Extended>(TargetBearing.BearingDeltaDegrees)) < 2.0E+1L && std::fabs(static_cast<pas::Extended>(ReverseTargetBearing.BearingDeltaDegrees)) < 3.0E+1L && ObstacleLevels[4] <= 1 && TargetShip->Thrust < 0.0L) {
            Result += 2;
            if (MaximumWeaponRange() * 0.7L < TargetBearing.Distance) {
                Result += 2;
            }
            Result += RandomRange(-1, 1);
            return Result;
        }
        return -1;
    }

    std::uint8_t TabShipAI::TryMoveToDestination(ab_Zone::PabZone Zone, double Longitude, double PolarAngle) {
        double ForwardDistance{};
        double BackwardDistance{};
        double Distance{};
        double Bearing{};
        SetAndFollowRoute(Zone);
        ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing), pas::Var<double>(&Distance), State.LongitudeDegrees, State.PolarAngleDegrees, State.BearingDegrees, Longitude, PolarAngle, ab_Global::SphereRadius);
        ab_StopLine::ab_StopLine_GetDistances(ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees, aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Bearing)), pas::Var<double>(&ForwardDistance), pas::Var<double>(&BackwardDistance));
        if (Distance >= ForwardDistance) {
            return RouteZone != nullptr;
        }
        SetTurnInput(Bearing);
        if (std::fabs(static_cast<pas::Extended>(Bearing)) < 45.0L) {
            Thrust = aMyFunction::RemapClamped(std::fabs(static_cast<pas::Extended>(Bearing)), 0.0, 45.0, 2.5, 0.0);
        } else {
            StopThrust();
        }
        return true;
    }

    void TabShipAI::SetAndFollowRoute(ab_Zone::PabZone Target) {
        if (Target == nullptr) {
            RouteZone = nullptr;
        } else {
            RouteZone = ab_Zone::ab_Zone_GetRoute(CurrentZone, Target);
        }
        HeadingInsideRoute = false;
        if (RouteZone != nullptr) {
            HeadingInsideRoute = ab_Zone::ab_Zone_IsHeadingInside(State, RouteZone, RouteBearing, RouteAngularRadius);
        }
        FollowDestinationRoute();
    }

    void TabShipAI::FollowDestinationRoute() {
        if (RouteZone != nullptr) {
            if (HeadingInsideRoute && RouteZone != nullptr) {
                StartThrust();
                if (std::fabs(static_cast<pas::Extended>(RouteBearing)) < pas::real_divide(RouteAngularRadius, 2.0L)) {
                    SetTurnInput(0.0);
                } else if (RouteBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (RouteBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
                return;
            }
            if (RouteZone != nullptr && static_cast<std::uint8_t>(ab_StopLine::ab_StopLine_IsBlocked(State.LongitudeDegrees, State.PolarAngleDegrees, RouteZone->Longitude, RouteZone->PolarAngle) ^ 1)) {
                if (RouteBearing < 45.0L) {
                    StartThrust();
                } else {
                    StopThrust();
                }
                if (std::fabs(static_cast<pas::Extended>(RouteBearing)) < pas::real_divide(RouteAngularRadius, 2.0L)) {
                    SetTurnInput(0.0);
                } else if (RouteBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (RouteBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
                return;
            }
            if (!InsideCurrentZone) {
                if (HeadingInsideCurrentZone) {
                    StartThrust();
                    if (std::fabs(static_cast<pas::Extended>(CurrentZoneBearing)) < pas::real_divide(CurrentZoneAngularRadius, 2.0L)) {
                        SetTurnInput(0.0);
                    } else if (CurrentZoneBearing < 0.0L) {
                        SetTurnInput(-1.0E+2);
                    } else if (CurrentZoneBearing > 0.0L) {
                        SetTurnInput(1.0E+2);
                    }
                    return;
                }
                if (CurrentZoneBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (CurrentZoneBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
                return;
            }
            if (RouteZone != nullptr) {
                if (RouteBearing < 0.0L) {
                    SetTurnInput(-1.0E+2);
                } else if (RouteBearing > 0.0L) {
                    SetTurnInput(1.0E+2);
                }
            }
        }
    }

    void TabShipAI::p_destroy() {
        ab_ShipAI::TabShipAI_Destroy(this);
    }

} // namespace ab_ShipAI
