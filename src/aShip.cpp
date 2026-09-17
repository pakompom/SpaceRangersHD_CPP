#include "layout/aShip.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/WindowsImports.hpp"
#include "types/aAsteroid.hpp"
#include "types/aEFilm.hpp"
#include "types/aGroup.hpp"
#include "types/aMissile.hpp"
#include "types/aPirate.hpp"
#include "types/aTransport.hpp"
#include "types/aWarrior.hpp"
#include "types/fGoodsShop2.hpp"
#include "types/fScore.hpp"
#include "types/fTalk.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/Dialogs.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SE_GAIEffect.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Ship2.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/ThreadCalc.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aEFilmEnd.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aGalaxyStruct.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPath.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aRuins.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/aTranclucator.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fShip2.hpp"

namespace aShip {
    // Caller-popped static link; recursively damages nearby hostiles. Returns the accumulated signed damage results.
    std::int32_t ApplyChainExplosion(TShip* ExplodingShip, TShip*& Self, aItem::TWeapon*& Weapon, std::uint8_t& RecordFilm, std::uint32_t& Color, aGalaxyStruct::TDamageFlagSet& Flags, std::int32_t& Count, aGalaxy::TStar*& Star, float& DistanceSquared, aConst::PWeaponInfo& Info, std::int32_t& Damage, std::int32_t& DrainedDamage, SE_Space::TObjectSE*& Effect, aEFilm::TEFilmObj*& Film, std::int32_t& StepIndex, pas::List*& Ships, pas::List*& Damages, pas::List*& Colors, pas::List*& Films, pas::WideString& GraphKey);

    // Caller-popped static link; ship -4, selected-item output -8.
    void SelectOrdinal(std::int32_t Ordinal, std::uint8_t Equipped, TShip* Self, aItem::TEquipment*& Item);

    // Suppresses duplicate Pirate Clan abduction effects during turn simulation.
    std::int8_t SimulationContext = 0;

    // Energy, splinter and missile hit callbacks.
    pas::Array<std::uint8_t, 0, 2> DamageScriptActionTypes = pas::Array<std::uint8_t, 0, 2>{{static_cast<std::uint8_t>(7), static_cast<std::uint8_t>(8), static_cast<std::uint8_t>(9)}};

    // Reused script-event payload for the goods leaving the ship.
    aItem::TGoods* TradeGoodsSold = nullptr;

    // Reused payload for the purchased portion of the sale.
    aItem::TGoods* TradeGoodsCostBasis = nullptr;

    pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorShipSmallSizes = pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2>{{pas::Array<std::int32_t, 0, 7>{{127, 110, 70, 60, 45, 40, 130, 40}}, pas::Array<std::int32_t, 0, 7>{{127, 110, 70, 60, 45, 40, 130, 40}}, pas::Array<std::int32_t, 0, 7>{{127, 110, 70, 60, 45, 40, 130, 40}}}};

    pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorShipLargeSizes = pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2>{{pas::Array<std::int32_t, 0, 7>{{127, 127, 100, 90, 65, 60, 160, 60}}, pas::Array<std::int32_t, 0, 7>{{127, 127, 100, 90, 65, 60, 160, 60}}, pas::Array<std::int32_t, 0, 7>{{127, 127, 100, 90, 65, 60, 160, 60}}}};

    pas::Array<std::int32_t, 0, 7> RangerSmallSizes = pas::Array<std::int32_t, 0, 7>{{50, 50, 50, 50, 50, 50, 50, 50}};

    pas::Array<std::int32_t, 0, 7> RangerLargeSizes = pas::Array<std::int32_t, 0, 7>{{80, 80, 80, 80, 80, 80, 80, 80}};

    pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5> TransportSmallSizes = pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5>{{pas::Array<std::int32_t, 0, 7>{{50, 50, 50, 50, 50, 50, 50, 50}}, pas::Array<std::int32_t, 0, 7>{{50, 50, 50, 50, 50, 50, 50, 50}}, pas::Array<std::int32_t, 0, 7>{{50, 50, 50, 50, 50, 50, 50, 50}}}};

    pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5> TransportLargeSizes = pas::Array<pas::Array<std::int32_t, 0, 7>, 3, 5>{{pas::Array<std::int32_t, 0, 7>{{90, 90, 90, 90, 90, 90, 90, 90}}, pas::Array<std::int32_t, 0, 7>{{90, 90, 90, 90, 90, 90, 90, 90}}, pas::Array<std::int32_t, 0, 7>{{90, 90, 90, 90, 90, 90, 90, 90}}}};

    pas::Array<std::int32_t, 0, 7> PirateSmallSizes = pas::Array<std::int32_t, 0, 7>{{45, 45, 45, 55, 45, 45, 45, 45}};

    pas::Array<std::int32_t, 0, 7> PirateLargeSizes = pas::Array<std::int32_t, 0, 7>{{80, 80, 80, 90, 80, 80, 80, 80}};

    pas::Array<std::int32_t, 0, 7> PirateClanSmallSizes = pas::Array<std::int32_t, 0, 7>{{45, 45, 45, 55, 45, 45, 45, 45}};

    pas::Array<std::int32_t, 0, 7> PirateClanLargeSizes = pas::Array<std::int32_t, 0, 7>{{80, 80, 80, 90, 80, 80, 80, 80}};

    pas::Array<std::int32_t, 0, 7> WarriorSmallSizes = pas::Array<std::int32_t, 0, 7>{{45, 45, 45, 55, 45, 45, 45, 45}};

    pas::Array<std::int32_t, 0, 7> WarriorLargeSizes = pas::Array<std::int32_t, 0, 7>{{80, 80, 80, 80, 80, 80, 80, 80}};

    pas::Array<std::int32_t, 0, 7> BigWarriorSmallSizes = pas::Array<std::int32_t, 0, 7>{{80, 80, 80, 80, 80, 80, 80, 80}};

    pas::Array<std::int32_t, 0, 7> BigWarriorLargeSizes = pas::Array<std::int32_t, 0, 7>{{130, 130, 130, 130, 130, 130, 130, 130}};

    std::int32_t TranclucatorSmallSize = 40;

    std::int32_t TranclucatorLargeSize = 50;

    std::int32_t SpecialHullSmallSize = 50;

    std::int32_t SpecialHullLargeSize = 80;

    std::int32_t StationSize = 128;

    std::int32_t DefaultShipSmallSize = 50;

    std::int32_t DefaultShipLargeSize = 80;

    // bonSkill1..bonSkill6.
    pas::Array<std::int32_t, 22, 27> SkillBonusEvaluationWeights = pas::Array<std::int32_t, 22, 27>{{100, 100, 80, 80, 60, 60}};

    // bonSlotRadar..bonSlotForsage.
    pas::Array<std::int32_t, 13, 20> SlotBonusEvaluationWeights = pas::Array<std::int32_t, 13, 20>{{100, 100, 200, 100, 200, 75, 10, 30}};

    // Indexed by KlingType.
    pas::Array<double, 0, 7> KlingCheapDropValueFactors = pas::Array<double, 0, 7>{{0.1, 0.85, 0.9, 1.0, 1.2, 1.5, 0.7, 4.0}};

    // Indexed by KlingType.
    pas::Array<double, 0, 7> KlingValuableDropValueFactors = pas::Array<double, 0, 7>{{0.1, 0.8, 0.9, 1.0, 2.0, 4.0, 0.7, 8.0}};

    // TKlingType order.
    const pas::Array<double, 0, 7> DominatorProgramDropCostFactors = pas::Array<double, 0, 7>{{0.2, 1.6, 1.8, 2.0, 4.0, 8.0, 1.4, 16.0}};

    // Allocates an unregistered instance; caller must initialize or deserialize it.
    TShip* CreateShipByType(std::uint8_t ShipType) {
        TShip* Result = nullptr;
        {
            std::uint8_t cpp_case = ShipType;
            if (cpp_case == 1) {
                return pas::construct_call<aRanger::TRanger>(aNormalShip::TNormalShip_Create);
            } else if (cpp_case == 0) {
                return pas::construct_call<aKling::TKling>(TShip_Create);
            } else if (cpp_case == 2) {
                return pas::construct_call<aTransport::TTransport>(aNormalShip::TNormalShip_Create);
            } else if (cpp_case == 3) {
                return pas::construct_call<aPirate::TPirate>(aNormalShip::TNormalShip_Create);
            } else if (cpp_case == 4) {
                return pas::construct_call<aWarrior::TWarrior>(aNormalShip::TNormalShip_Create);
            } else if (cpp_case == 5) {
                return pas::construct_call<aTranclucator::TTranclucator>(aTranclucator::TTranclucator_Create);
            } else if (cpp_case >= 6 && cpp_case <= 13) {
                return pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
            } else {
                // The original constructs the exception without raising it.
                pas::make_exception<pas::Exception>("function CreateShipByType(shiptype: tShipType): TShip;"_a);
                return Result;
            }
        }
    }

    // Lists contain TShip. Sum of pairwise ChanceToWin divided by Opponents.Count squared; requires nonempty Opponents when Ships is nonempty.
    float CompareShipGroupsStrength(pas::List* Ships, pas::List* Opponents) {
        std::int32_t I{};
        std::int32_t J{};
        TShip* Ship{};
        TShip* Target{};
        float Sum{};
        float Chance{};
        std::int32_t ShipCount = pas::list_count(Ships);
        std::int32_t Count = pas::list_count(Opponents);
        float Result = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(Ships, I);
            Sum = 0.0f;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(J); ) {
                Target = pas::list_at<TShip>(Opponents, J);
                Chance = aShip::TShip_ChanceToWin(Ship, Target);
                Sum = static_cast<long double>(Sum) + Chance;
            }
            Result = pas::real_divide(pas::real_divide(Sum, Count), Count) + Result;
        }
        return Result;
    }

    // Owner six skips racial scaling. Uses active galaxy turn and difficulty.
    float CalculateFuelCost(std::int32_t Amount, std::uint8_t OwnerId) {
        float Value = Amount + 0;
        Value = static_cast<long double>(Value) * aMyFunction::RemapClamped(aGalaxy::Galaxy->CurrentTurn, 1.0E+3, 1.5E+4, 1.0, 1.0E+1);
        if (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
            Value = static_cast<long double>(Value) * aConst::OwnerInfo[OwnerId].FuelPriceFactor;
        }
        Value = static_cast<long double>(Value) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor;
        return Value;
    }

    std::int32_t CalculateRoundedFuelCost(std::int32_t Amount, std::uint8_t OwnerId) {
        return System::Round(aShip::CalculateFuelCost(Amount, OwnerId));
    }

    // Source helper: preserve the native radar-before-clamp evaluation and local order.
    void ClampMissileWeaponRange(TShip* Ship, std::int32_t TemplateRange, std::int32_t& Range) {
        std::int32_t MaximumRange{};
        std::int32_t MinimumRange{};
        std::int32_t RadarRange = Ship->GetRadarRange();
        if (Range > TemplateRange) {
            MaximumRange = Range;
        } else {
            MaximumRange = TemplateRange;
        }
        if (RadarRange < MaximumRange) {
            MinimumRange = RadarRange;
        } else {
            MinimumRange = MaximumRange;
        }
        Range = MinimumRange;
    }

    void TShip_Create(TShip* Self) {
        std::int32_t I{};
        std::uint8_t Kind{};
        std::uint8_t Skill{};
        std::uint8_t Series{};
        EC_Struct::TObjectEx_Create(Self);
        Self->PortraitFaceId = -1;
        Self->Money = 0;
        Self->EncodedMoney = Self->Money ^ 0xa4a576adu;
        if (aGalaxy::Galaxy != nullptr) {
            Self->Id = aGalaxy::Galaxy->NextShipId;
            ++aGalaxy::Galaxy->NextShipId;
            Self->Seed = aMyFunction::NextRandomIntRange(100000, SystemImports::MaxInt, aGalaxy::Galaxy->RandomState);
            Self->CreationTurn = aGalaxy::Galaxy->CurrentTurn;
        }
        if (static_cast<std::int32_t>(Self->Seed) < 0) {
            GR_Main::RaiseWideMessage(u"TShip.Create; - FRnd<0"_wref.get());
        }
        Self->RandomState = Self->Seed;
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
            Self->CargoGoods[Kind].Count = 0;
            Self->CargoGoods[Kind].TotalCost = 0;
        }
        // The contiguous Hull..DefGenerator fields are indexed by native item type.
        for (Kind = static_cast<std::uint8_t>(42); Kind <= static_cast<std::uint8_t>(49); ++Kind) {
            pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(Self)->Slots, (Kind - 42) * sizeof(aItem::TEquipment*)), nullptr);
        }
        for (I = 1; I <= 5; ++I) {
            Self->Weapons[I] = nullptr;
        }
        Self->WeaponCount = 0;
        for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(5); ++Skill) {
            Self->BaseSkills[Skill] = 0;
        }
        Self->MovementPath = pas::construct_call<aPath::TSPath>(aPath::TSPath_Create);
        Self->OrderNone(false);
        Self->Inventory = pas::make_object<aMyFunction::TObjectList>();
        Self->Artefacts = pas::make_object<aMyFunction::TObjectList>();
        Self->GuaranteedDeathDropItems = pas::make_object<aMyFunction::TObjectList>();
        Self->StatBonuses = nullptr;
        Self->CombatStatusEffects = nullptr;
        Self->MovementTurnRate = 1.2;
        Self->EnemyShip = nullptr;
        Self->TruceShip = nullptr;
        Self->PartnerShip = nullptr;
        Self->PlanetQueue = nullptr;
        Self->RangerRelations = pas::make_object<pas::List>();
        Self->AwardIds = nullptr;
        Self->NodeReserve = 0;
        Self->TotalExperience = 0;
        Self->FreeExperience = 0;
        Self->DaysSincePlayerSeen = 100;
        Self->LastProcessedTurn = -1;
        Self->LiberationGroup = nullptr;
        Self->LiberationGroupRouteIndex = 0;
        for (I = 1; I <= 24; ++I) {
            Self->CaptainHealth[I].Progress = 0.0;
            Self->CaptainHealth[I].AppliedTurn = 0;
            Self->CaptainHealth[I].ExpireTurn = 0;
            Self->CaptainHealth[I].ApplicationCount = 0;
        }
        for (I = 1; I <= 1; ++I) {
            Self->RadiationHealth[I].Progress = 0.0;
            Self->RadiationHealth[I].AppliedTurn = 0;
            Self->RadiationHealth[I].ExpireTurn = 0;
            Self->RadiationHealth[I].ApplicationCount = 0;
        }
        Self->TechKnowledge = 0;
        Self->CustomShipInfos = pas::make_object<pas::List>();
        Self->ChameleonActive = false;
        for (Series = static_cast<std::uint8_t>(0); Series <= static_cast<std::uint8_t>(2); ++Series) {
            Self->ChameleonDetected[Series] = false;
            Self->ChameleonCharges[Series] = 0;
        }
        Self->ChameleonDisplayCount = 0;
        Self->AwardVisibleCount = 0;
        Self->PlayerExtortionPactActive = false;
        Self->PlayerScratchHitsReceived = 0;
        Self->InterceptorPassesRemaining = 0;
        Self->InterceptorSourceShip = nullptr;
        Self->InterceptorGraphic = nullptr;
        Self->AbductedByPirateClan = false;
        Self->Graphic = nullptr;
        Self->EquipmentPriceSensitivity = 1.0f;
    }

    void TShip_Destroy(TShip* Self) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t PartnerIndex{};
        aGalaxy::TStar* Star{};
        TShip* Ship{};
        aScript::TScriptShip* Binding{};
        aMissile::TMissile* Missile{};
        aPlayer::PStorageEntry Storage{};
        std::uint8_t SavedAbsoluteOrder{};
        if (aRanger::PendingPlayerFollowTarget == Self) {
            aRanger::PendingPlayerFollowTarget = nullptr;
        }
        if (aKling::TerronShip == Self && static_cast<std::uint8_t>(Self->DestroyQueued ^ 1)) {
            aGalaxy::Galaxy->TerronLandingLockTurn = 0;
        }
        if (Self->ScriptShip != nullptr) {
            Binding = pas::checked_cast<aScript::TScriptShip*>(Self->ScriptShip);
            Binding->Script->UnbindShip(Self);
        }
        if (Self->LiberationGroup != nullptr) {
            Self->LeaveLiberationGroup();
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->StorageEntries != nullptr && Self->TypeId != aGalaxyStruct::stTranclucator) {
            const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Storage = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
                    if (Storage->LocationOwner == Self) {
                        Storage->LocationOwner = nullptr;
                        if (Storage->Item != nullptr) {
                            pas::free(Storage->Item);
                        }
                        Storage->Item = nullptr;
                        pas::list_delete(aPlayer::GetPlayer()->StorageEntries, I);
                        pas::dispose(Storage);
                    }
                }
            }
        }
        Self->ClearPlanetQueue();
        I = -1;
        if (aGalaxy::WingmenPendingLeadershipPenalty != nullptr) {
            I = pas::list_indexof(aGalaxy::WingmenPendingLeadershipPenalty, reinterpret_cast<void*>(Self));
        }
        if (I >= 0) {
            pas::list_delete(aGalaxy::WingmenPendingLeadershipPenalty, I);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<TShip>(Star->Ships, J);
                if (Ship->EnemyShip == Self) {
                    Ship->EnemyShip = nullptr;
                }
                if (Ship->TruceShip == Self) {
                    Ship->TruceShip = nullptr;
                }
                if (Ship->PartnerShip == Self) {
                    Ship->PartnerShip = nullptr;
                }
                if (Ship->DockedTo == Self) {
                    Ship->DockedTo = nullptr;
                }
                if (Ship->OrderTarget == Self) {
                    SavedAbsoluteOrder = Ship->AbsoluteScriptOrder;
                    Ship->AbsoluteScriptOrder = 0;
                    Ship->OrderNone(false);
                    Ship->AbsoluteScriptOrder = SavedAbsoluteOrder;
                    if (Star->RecordingTurnFilm && Ship->FilmObject != nullptr) {
                        Ship->FilmAlpha = 255.0f;
                        Ship->FilmAlphaStep = 0.0f;
                    }
                }
                if (Ship->GetHull()->InterceptorTarget == Self) {
                    Ship->GetHull()->InterceptorTarget = nullptr;
                }
                if (Ship->InterceptorSourceShip == Self) {
                    Ship->InterceptorSourceShip = nullptr;
                }
                if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == Self) {
                    pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip = nullptr;
                    pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->FollowOwner = false;
                }
                if (pas::class_cast_if<aPlayer::TPlayer*>(Ship) != nullptr && static_cast<aPlayer::TPlayer*>(Ship)->PiratePartners != nullptr) {
                    PartnerIndex = pas::list_indexof(pas::checked_cast<aPlayer::TPlayer*>(Ship)->PiratePartners, reinterpret_cast<void*>(Self));
                    if (PartnerIndex >= 0) {
                        pas::list_delete(pas::checked_cast<aPlayer::TPlayer*>(Ship)->PiratePartners, PartnerIndex);
                    }
                }
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Missiles) - 1); cpp_range_3.next(J); ) {
                Missile = pas::list_at<aMissile::TMissile>(Star->Missiles, J);
                Missile->ClearReferencesTo(Self);
            }
        }
        Self->ClearPickupTargets();
        Self->ClearRecentlyDroppedItems();
        if (Self->CurrentStar != nullptr) {
            I = pas::list_indexof(Self->CurrentStar->Ships, reinterpret_cast<void*>(Self));
            if (I >= 0) {
                pas::list_delete(Self->CurrentStar->Ships, I);
            }
        }
        I = pas::list_indexof(aGalaxy::Galaxy->ShipsInTransit, reinterpret_cast<void*>(Self));
        if (I >= 0) {
            pas::list_delete(aGalaxy::Galaxy->ShipsInTransit, I);
        }
        if (Self->Graphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
        }
        if (Self->InterceptorGraphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->InterceptorGraphic));
        }
        pas::free(Self->Inventory);
        pas::free(Self->Artefacts);
        pas::free(Self->GuaranteedDeathDropItems);
        if (Self->StatBonuses != nullptr) {
            {
                const std::int32_t cpp_first_2 = pas::list_count(Self->StatBonuses) - 1;
                if (cpp_first_2 >= 0) {
                    for (I = cpp_first_2; I >= 0; --I) {
                        pas::dispose(pas::list_get(Self->StatBonuses, I));
                    }
                }
            }
            pas::free(Self->StatBonuses);
            Self->StatBonuses = nullptr;
        }
        if (Self->CombatStatusEffects != nullptr) {
            {
                const std::int32_t cpp_first_3 = pas::list_count(Self->CombatStatusEffects) - 1;
                if (cpp_first_3 >= 0) {
                    for (I = cpp_first_3; I >= 0; --I) {
                        pas::dispose(pas::list_get(Self->CombatStatusEffects, I));
                    }
                }
            }
            pas::free(Self->CombatStatusEffects);
            Self->CombatStatusEffects = nullptr;
        }
        if (Self->CustomShipInfos != nullptr) {
            // Native releases raw records here without finalizing their string fields.
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CustomShipInfos) - 1); cpp_range_4.next(I); ) {
                pas::dispose(pas::list_get(Self->CustomShipInfos, I));
            }
            pas::free(Self->CustomShipInfos);
            Self->CustomShipInfos = nullptr;
        }
        pas::free(Self->MovementPath);
        if (aPlayer::GetPlayer() == Self) {
            aPlayer::SetPlayer(nullptr, aGalaxy::Galaxy);
        }
        if (aKling::BlazerShip == Self) {
            aKling::BlazerShip = nullptr;
        }
        if (aKling::KellerShip == Self) {
            aKling::KellerShip = nullptr;
        }
        if (aKling::TerronShip == Self) {
            aKling::TerronShip = nullptr;
        }
        pas::free(Self->RangerRelations);
        if (Self->AwardIds != nullptr) {
            pas::free(Self->AwardIds);
        }
        Self->Id = 0;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TShip::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::uint8_t Good{};
        aItem::TItem* Item{};
        std::int32_t I{};
        std::uint8_t Award{};
        std::uint8_t Skill{};
        std::uint8_t Series{};
        std::uint32_t SourceId{};
        PCustomShipInfo Info{};
        Buffer->AddDWord(Id);
        Buffer->AddWideStringZ(Name);
        Buffer->AddWideStringZ(TypeNameOverrideKey);
        Buffer->AddAnsiChar(TypeId);
        Buffer->AddAnsiChar(OwnerId);
        Buffer->AddSingle(Position.X);
        Buffer->AddSingle(Position.Y);
        if (TransitOriginStar == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(TransitOriginStar->Id);
        }
        if (CurrentPlanet == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(CurrentPlanet->Id);
        }
        if (DockedTo == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(DockedTo->Id);
        }
        if (HomePlanet == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(HomePlanet->Id);
        }
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            Buffer->AddDWord(CargoGoods[Good].Count);
            Buffer->AddDWord(CargoGoods[Good].TotalCost);
            Buffer->AddDWord(CargoGoods[Good].PurchasedCount);
            Buffer->AddDWord(CargoGoods[Good].PurchasedTotalCost);
        }
        Buffer->AddDWord(Money);
        Buffer->AddDWord(Seed);
        Buffer->AddDWord(RandomState);
        Buffer->AddDWord(CreationTurn);
        Buffer->AddIntegerValue(PortraitFaceId);
        Buffer->AddAnsiChar(PilotRace);
        std::int32_t Count = pas::list_count(Inventory);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Artefacts);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Artefacts, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(GuaranteedDeathDropItems);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Item = pas::list_at<aItem::TItem>(GuaranteedDeathDropItems, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        if (StatBonuses == nullptr) {
            Buffer->AddWideChar(u'\000');
        } else {
            Count = pas::list_count(StatBonuses);
            Buffer->AddWideChar(Count);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                Buffer->AddAnsiChar(pas::list_at<TShipStatBonusEntry>(StatBonuses, I)->BonusKind);
                Buffer->AddIntegerValue(pas::list_at<TShipStatBonusEntry>(StatBonuses, I)->BonusValue);
            }
        }
        if (CombatStatusEffects == nullptr) {
            Buffer->AddWideChar(u'\000');
        } else {
            Count = pas::list_count(CombatStatusEffects);
            Buffer->AddWideChar(Count);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->EffectType));
                Buffer->AddSingle(pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->Strength);
                SourceId = pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->SourceShipId;
                Buffer->AddDWord(SourceId);
            }
        }
        {
            const std::int32_t cpp_first = pas::list_count(CustomShipInfos) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Info = pas::list_at<TCustomShipInfo>(CustomShipInfos, I);
                    if (Info->DeleteQueued) {
                        pas::list_delete(CustomShipInfos, I);
                        pas::dispose(Info);
                    }
                }
            }
        }
        Buffer->AddIntegerValue(pas::list_count(CustomShipInfos));
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(CustomShipInfos) - 1); cpp_range_6.next(I); ) {
            Info = pas::list_at<TCustomShipInfo>(CustomShipInfos, I);
            Buffer->AddWideStringZ(Info->TypeName);
            Buffer->AddWideStringZ(Info->Description);
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t))));
            Buffer->AddIntegerValue(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t))));
            Buffer->AddWideStringZ(Info->TextData1);
            Buffer->AddWideStringZ(Info->TextData2);
            Buffer->AddWideStringZ(Info->TextData3);
        }
        if (PickupTargets == nullptr) {
            Buffer->AddWideChar(u'\000');
        } else {
            Count = pas::list_count(PickupTargets);
            Buffer->AddWideChar(Count);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
                Item = pas::list_at<aItem::TItem>(PickupTargets, I);
                Buffer->AddDWord(Item->Id);
            }
        }
        if (RecentlyDroppedItemIds == nullptr) {
            Buffer->AddWideChar(u'\000');
        } else {
            Count = pas::list_count(RecentlyDroppedItemIds);
            Buffer->AddWideChar(Count);
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(I); ) {
                Buffer->AddDWord(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RecentlyDroppedItemIds, I))));
            }
        }
        if (EnemyShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(EnemyShip->Id);
        }
        if (TruceShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(TruceShip->Id);
        }
        if (PartnerShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(PartnerShip->Id);
            Buffer->AddDWord(std::max<std::int32_t>(0, PartnershipDaysRemaining));
        }
        Buffer->AddBoolean(AfterburnerActive);
        Buffer->AddSingle(MovementDirection);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Order));
        Buffer->AddDWord(OrderStateData);
        if (Order == soJump) {
            Buffer->AddDWord(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Id);
        } else if (Order == soJumpHole) {
            if (OrderTarget != nullptr) {
                Buffer->AddDWord(pas::checked_cast<aGalaxy::THole*>(OrderTarget)->Id);
            } else {
                Buffer->AddDWord(0u);
                GR_Main::AppendDebugLogLine(static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"Error [t_JumpHole]: Hole not found, id = ", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Id)), ", Name = "})), Name})));
            }
        } else if (Order == soTeleport) {
            Buffer->AddDWord(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Id);
        } else if (Order == soLand) {
            if (pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
                Buffer->AddDWord(pas::checked_cast<TShip*>(OrderTarget)->Id | 0x80000000u);
            } else {
                Buffer->AddDWord(pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->Id);
            }
        } else if (Order == soFollowShip) {
            Buffer->AddDWord(pas::checked_cast<TShip*>(OrderTarget)->Id);
        } else {
            Buffer->AddDWord(0u);
        }
        Buffer->AddSingle(OrderDestination.X);
        Buffer->AddSingle(OrderDestination.Y);
        Buffer->AddBoolean(OrderAbsolute);
        Buffer->AddBoolean(AbductedByPirateClan);
        Buffer->AddIntegerValue(ConsecutiveDockedDays);
        Buffer->AddAnsiChar(AbsoluteScriptOrder);
        Buffer->AddBoolean(GraphDominator);
        Buffer->AddWideStringZ(GraphName);
        Buffer->AddAnsiChar(Graphic->GetAlpha());
        Buffer->AddBoolean(InHyperspace);
        Buffer->AddSingle(CollisionRadius);
        Count = pas::list_count(RangerRelations);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(I); ) {
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, I))));
        }
        if (AwardIds == nullptr) {
            Buffer->AddAnsiChar('\000');
        } else {
            Buffer->AddAnsiChar(pas::list_count(AwardIds));
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(AwardIds) - 1); cpp_range_10.next(I); ) {
                Award = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(AwardIds, I)));
                Buffer->AddAnsiChar(Award);
            }
        }
        Buffer->AddBoolean(DestroyQueued);
        for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(5); ++Skill) {
            Buffer->AddAnsiChar(BaseSkills[Skill]);
        }
        Buffer->AddWideChar(NodeReserve);
        Buffer->AddDWord(TotalExperience);
        Buffer->AddDWord(FreeExperience);
        Buffer->AddWideChar(DaysSincePlayerSeen);
        Buffer->AddDWord(EncodedMoney);
        Buffer->AddWideChar(LiberationGroupRouteIndex);
        for (I = 1; I <= 24; ++I) {
            Buffer->AddSingle(CaptainHealth[I].Progress);
            Buffer->AddIntegerValue(CaptainHealth[I].AppliedTurn);
            Buffer->AddIntegerValue(CaptainHealth[I].ExpireTurn);
            Buffer->AddIntegerValue(CaptainHealth[I].ApplicationCount);
        }
        Buffer->AddIntegerValue(LastProcessedTurn);
        Buffer->AddIntegerValue(UnknownF4);
        Buffer->AddBoolean(ChameleonActive);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(ChameleonSeries));
        Buffer->AddAnsiChar(ChameleonVisualType);
        Buffer->AddIntegerValue(ChameleonDisplayCount);
        for (Series = static_cast<std::uint8_t>(0); Series <= static_cast<std::uint8_t>(2); ++Series) {
            Buffer->AddBoolean(ChameleonDetected[Series]);
            Buffer->AddIntegerValue(ChameleonCharges[Series]);
        }
        for (I = 1; I <= 1; ++I) {
            Buffer->AddSingle(RadiationHealth[I].Progress);
            Buffer->AddIntegerValue(RadiationHealth[I].AppliedTurn);
            Buffer->AddIntegerValue(RadiationHealth[I].ExpireTurn);
            Buffer->AddIntegerValue(RadiationHealth[I].ApplicationCount);
        }
        Buffer->AddAnsiChar(TechKnowledge);
        Buffer->AddIntegerValue(TradeLossBalance);
        Buffer->AddIntegerValue(TradeExperience);
        Buffer->AddIntegerValue(ContrabandProfit);
        Buffer->AddIntegerValue(AwardVisibleCount);
        Buffer->AddBoolean(NoDrop);
        Buffer->AddAnsiChar(TargetingRestriction);
        Buffer->AddBoolean(NoTalk);
        Buffer->AddBoolean(NoScan);
        Buffer->AddBoolean(ScriptChameleon);
        Buffer->AddBoolean(PlayerExtortionPactActive);
        Buffer->AddWideChar(PlayerScratchHitsReceived);
        Buffer->AddIntegerValue(InterceptorPassesRemaining);
        if (InterceptorSourceShip != nullptr) {
            Buffer->AddDWord(InterceptorSourceShip->Id);
        } else {
            Buffer->AddDWord(0u);
        }
        if (InterceptorPassesRemaining > 0) {
            Buffer->AddWideStringZ(InterceptorGraphic->GraphKey);
        }
        Buffer->AddAnsiChar(CurrentStanding);
        Buffer->AddIntegerValue(SmoothedSpeed);
        Buffer->AddIntegerValue(SmoothedEnemySpeed);
        Buffer->AddSingle(SmoothedEquipmentEffectiveness);
        Buffer->AddIntegerValue(SmoothedWealth);
        Buffer->AddSingle(SmoothedMoneyFraction);
        Buffer->AddSingle(SmoothedFreeCapacityFraction);
        Buffer->AddSingle(EquipmentPriceSensitivity);
    }

    void TShip::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::uint8_t Good{};
        aItem::TItem* Item{};
        std::int32_t I{};
        std::int32_t InfoData{};
        std::uint8_t Award{};
        std::uint8_t Skill{};
        std::uint8_t Series{};
        PShipStatBonusEntry Bonus{};
        PCombatStatusEffect Effect{};
        PCustomShipInfo Info{};
        EC_BlockPar::TBlockParEC* Block{};
        Id = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (static_cast<std::uint32_t>(Id) >= Galaxy->NextShipId) {
            Galaxy->NextShipId = Id + 1;
        }
        Name = Buffer->ReadWideString();
        if (GlobalsV::LoadedSaveVersion >= 97) {
            TypeNameOverrideKey = Buffer->ReadWideString();
        } else {
            TypeNameOverrideKey = pas::WideString();
        }
        TypeId = EC_Buf::TBufEC_GetByte(Buffer);
        OwnerId = EC_Buf::TBufEC_GetByte(Buffer);
        Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
        Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        TransitOriginStar = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        CurrentPlanet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        DockedTo = reinterpret_cast<TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        HomePlanet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            CargoGoods[Good].Count = EC_Buf::TBufEC_GetUInt32(Buffer);
            CargoGoods[Good].TotalCost = EC_Buf::TBufEC_GetUInt32(Buffer);
            CargoGoods[Good].PurchasedCount = EC_Buf::TBufEC_GetUInt32(Buffer);
            CargoGoods[Good].PurchasedTotalCost = EC_Buf::TBufEC_GetUInt32(Buffer);
        }
        {
            std::int32_t uInt32 = EC_Buf::TBufEC_GetUInt32(Buffer);
            TShip* self = this;
            self->SetMoney(uInt32);
        }
        Seed = EC_Buf::TBufEC_GetUInt32(Buffer);
        RandomState = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (static_cast<std::int32_t>(Seed) < 0) {
            Dialogs::ShowMessage("TShip.Create; - FRnd<0"_a);
        }
        CreationTurn = EC_Buf::TBufEC_GetUInt32(Buffer);
        PortraitFaceId = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 125) {
            PilotRace = EC_Buf::TBufEC_GetByte(Buffer);
        } else if (OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
            PilotRace = EC_Buf::TBufEC_GetByte(Buffer);
        } else if (pas::in_range(OwnerId, static_cast<std::int32_t>(aGalaxyStruct::oiMaloc), static_cast<std::int32_t>(aGalaxyStruct::oiGaal))) {
            PilotRace = aConst::OwnerToRace(OwnerId);
        } else {
            PilotRace = static_cast<std::uint8_t>(aGalaxyStruct::oiMaloc);
        }
        if (GlobalsV::LoadedSaveVersion < 102) {
            if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMedicalBase)) {
                OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiGaal);
                PilotRace = static_cast<std::uint8_t>(aGalaxyStruct::oiGaal);
                if (PortraitFaceId > 14) {
                    PortraitFaceId = -1;
                }
            }
            if (TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstBusinessCenter)) {
                OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiHuman);
                PilotRace = static_cast<std::uint8_t>(aGalaxyStruct::oiHuman);
            }
        }
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Item count in equipment > 10000"_a));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
            pas::list_add(Inventory, reinterpret_cast<void*>(Item));
            Item->LoadFromBuffer(Buffer, Galaxy);
            if (pas::class_cast_if<aItem::THull*>(Item) != nullptr) {
                reinterpret_cast<aItem::THull*>(Item)->OwnerShip = this;
            }
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Artefacts count > 10000"_a));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
            pas::list_add(Artefacts, reinterpret_cast<void*>(Item));
            Item->LoadFromBuffer(Buffer, Galaxy);
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
            pas::list_add(GuaranteedDeathDropItems, reinterpret_cast<void*>(Item));
            Item->LoadFromBuffer(Buffer, Galaxy);
        }
        if (GlobalsV::LoadedSaveVersion >= 68) {
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > 10000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            if (Count > 0) {
                StatBonuses = pas::make_object<pas::List>();
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                    pas::new_value(Bonus);
                    Bonus->BonusKind = EC_Buf::TBufEC_GetByte(Buffer);
                    Bonus->BonusValue = EC_Buf::TBufEC_GetInt32(Buffer);
                    pas::list_add(StatBonuses, static_cast<void*>(Bonus));
                }
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 77) {
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > 10000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            if (Count > 0) {
                CombatStatusEffects = pas::make_object<pas::List>();
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
                    pas::new_value(Effect);
                    Effect->EffectType = static_cast<TCombatStatusEffectType>(EC_Buf::TBufEC_GetByte(Buffer));
                    Effect->Strength = EC_Buf::TBufEC_GetSingle(Buffer);
                    Effect->SourceShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
                    pas::list_add(CombatStatusEffects, static_cast<void*>(Effect));
                }
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 104) {
            Count = EC_Buf::TBufEC_GetInt32(Buffer);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(I); ) {
                pas::new_value(Info);
                Info->TypeName = Buffer->ReadWideString();
                Info->Description = Buffer->ReadWideString();
                InfoData = EC_Buf::TBufEC_GetInt32(Buffer);
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)), InfoData);
                InfoData = EC_Buf::TBufEC_GetInt32(Buffer);
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (2 - 1) * sizeof(std::int32_t)), InfoData);
                InfoData = EC_Buf::TBufEC_GetInt32(Buffer);
                pas::store_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (3 - 1) * sizeof(std::int32_t)), InfoData);
                Info->TextData1 = Buffer->ReadWideString();
                Info->TextData2 = Buffer->ReadWideString();
                Info->TextData3 = Buffer->ReadWideString();
                Info->ActionCode = nullptr;
                Info->ActionCodeInitialized = false;
                Info->DeleteQueued = false;
                Block = GR_Main::LanguageDataConfig->GetBlock(u"ShipInfo"_wref.get())->GetBlock(u"AddInfo"_wref.get())->GetBlock(u"CustomInfos"_wref.get())->FindBlock(Info->TypeName);
                if (Block == nullptr) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning! Ship info ", Info->TypeName, u" not found, deleting."})));
                    pas::dispose(Info);
                } else {
                    pas::list_add(CustomShipInfos, static_cast<void*>(Info));
                    Info->StatusEffect = Block->CountParams(u"StatusEffect"_wref.get()) > 0;
                }
            }
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err"_a));
        }
        if (Count > 0) {
            if (PickupTargets != nullptr) {
                pas::free(PickupTargets);
            }
            PickupTargets = pas::make_object<pas::List>();
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
                void* uInt32_2 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                pas::List* pickupTargets = PickupTargets;
                pas::list_add(pickupTargets, uInt32_2);
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 81) {
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > 10000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            if (Count > 0) {
                if (RecentlyDroppedItemIds != nullptr) {
                    pas::free(RecentlyDroppedItemIds);
                }
                RecentlyDroppedItemIds = pas::make_object<pas::List>();
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(I); ) {
                    void* uInt32_3 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                    pas::List* recentlyDroppedItemIds = RecentlyDroppedItemIds;
                    pas::list_add(recentlyDroppedItemIds, uInt32_3);
                }
            }
        }
        EnemyShip = reinterpret_cast<TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        TruceShip = reinterpret_cast<TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        TShip* SavedPartner = reinterpret_cast<TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        PartnerShip = SavedPartner;
        if (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(SavedPartner)) > 0) {
            PartnershipDaysRemaining = EC_Buf::TBufEC_GetUInt32(Buffer);
        }
        AfterburnerActive = EC_Buf::TBufEC_GetBoolean(Buffer);
        MovementDirection = EC_Buf::TBufEC_GetSingle(Buffer);
        Order = static_cast<TShipOrder>(EC_Buf::TBufEC_GetByte(Buffer));
        OrderStateData = EC_Buf::TBufEC_GetUInt32(Buffer);
        OrderTarget = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        OrderDestination.X = EC_Buf::TBufEC_GetSingle(Buffer);
        OrderDestination.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        OrderAbsolute = EC_Buf::TBufEC_GetBoolean(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 107) {
            AbductedByPirateClan = EC_Buf::TBufEC_GetBoolean(Buffer);
        }
        if (GlobalsV::LoadedSaveVersion >= 48) {
            ConsecutiveDockedDays = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            ConsecutiveDockedDays = 0;
        }
        AbsoluteScriptOrder = EC_Buf::TBufEC_GetByte(Buffer);
        GraphDominator = EC_Buf::TBufEC_GetBoolean(Buffer);
        GraphName = Buffer->ReadWideString();
        if (GraphName.read(1) == u'R') {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), GraphName, ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
        } else {
            if (GlobalsV::LoadedSaveVersion <= 66) {
                GraphName = EC_Str::ReplaceAllWideString(GraphName, u"Adon"_wref.get(), u"Akrin"_wref.get());
                GraphName = EC_Str::ReplaceAllWideString(GraphName, u"Custom.1"_wref.get(), u"AkrinFemale.J"_wref.get());
            }
            {
                SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), GraphName, ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic_2 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_2, createSpaceObjectByName_2);
            }
        }
        Graphic->SetPosition(Position);
        Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
        {
            std::uint8_t byte = EC_Buf::TBufEC_GetByte(Buffer);
            SE_Space::TObjectSE* graphic_3 = Graphic;
            graphic_3->SetAlpha(byte);
        }
        InHyperspace = EC_Buf::TBufEC_GetBoolean(Buffer);
        CollisionRadius = EC_Buf::TBufEC_GetSingle(Buffer);
        // Native replaces the constructor-created list without freeing it here.
        RangerRelations = pas::make_object<pas::List>();
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Error FRelationToRangers not in 0..10000"_a));
        }
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(I); ) {
            void* byte_2 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetByte(Buffer))));
            pas::List* rangerRelations = RangerRelations;
            pas::list_add(rangerRelations, byte_2);
        }
        AwardIds = nullptr;
        Count = EC_Buf::TBufEC_GetByte(Buffer);
        if (Count < 0 || Count > 255) {
            pas::raise(pas::make_exception<pas::Abort>("Error FRewards not in 0..255"_a));
        }
        if (Count > 0) {
            AwardIds = pas::make_object<pas::List>();
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(I); ) {
                Award = EC_Buf::TBufEC_GetByte(Buffer);
                pas::list_add(AwardIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Award))));
            }
            AwardVisibleCount = Count;
        }
        DestroyQueued = EC_Buf::TBufEC_GetBoolean(Buffer);
        for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(5); ++Skill) {
            BaseSkills[Skill] = EC_Buf::TBufEC_GetByte(Buffer);
        }
        NodeReserve = EC_Buf::TBufEC_GetWord(Buffer);
        TotalExperience = EC_Buf::TBufEC_GetUInt32(Buffer);
        FreeExperience = EC_Buf::TBufEC_GetUInt32(Buffer);
        DaysSincePlayerSeen = EC_Buf::TBufEC_GetWord(Buffer);
        EncodedMoney = EC_Buf::TBufEC_GetUInt32(Buffer);
        LiberationGroupRouteIndex = EC_Buf::TBufEC_GetWord(Buffer);
        for (I = 1; I <= 24; ++I) {
            CaptainHealth[I].Progress = EC_Buf::TBufEC_GetSingle(Buffer);
            CaptainHealth[I].AppliedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            CaptainHealth[I].ExpireTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 54) {
                CaptainHealth[I].ApplicationCount = EC_Buf::TBufEC_GetInt32(Buffer);
            } else {
                CaptainHealth[I].ApplicationCount = 0;
            }
        }
        LastProcessedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        UnknownF4 = EC_Buf::TBufEC_GetInt32(Buffer);
        ChameleonActive = EC_Buf::TBufEC_GetBoolean(Buffer);
        ChameleonSeries = static_cast<aGalaxyStruct::TDominatorSeries>(EC_Buf::TBufEC_GetByte(Buffer));
        ChameleonVisualType = EC_Buf::TBufEC_GetByte(Buffer);
        ChameleonDisplayCount = EC_Buf::TBufEC_GetInt32(Buffer);
        for (Series = static_cast<std::uint8_t>(0); Series <= static_cast<std::uint8_t>(2); ++Series) {
            ChameleonDetected[Series] = EC_Buf::TBufEC_GetBoolean(Buffer);
            ChameleonCharges[Series] = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        for (I = 1; I <= 1; ++I) {
            RadiationHealth[I].Progress = EC_Buf::TBufEC_GetSingle(Buffer);
            RadiationHealth[I].AppliedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            RadiationHealth[I].ExpireTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 54) {
                RadiationHealth[I].ApplicationCount = EC_Buf::TBufEC_GetInt32(Buffer);
            } else {
                RadiationHealth[I].ApplicationCount = 0;
            }
        }
        if (GlobalsV::LoadedSaveVersion >= 146) {
            TechKnowledge = EC_Buf::TBufEC_GetByte(Buffer);
        } else {
            TechKnowledge = 3;
        }
        TradeLossBalance = EC_Buf::TBufEC_GetInt32(Buffer);
        TradeExperience = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 58) {
            ContrabandProfit = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            ContrabandProfit = 0;
        }
        AwardVisibleCount = EC_Buf::TBufEC_GetInt32(Buffer);
        NoDrop = EC_Buf::TBufEC_GetBoolean(Buffer);
        TargetingRestriction = EC_Buf::TBufEC_GetByte(Buffer);
        NoTalk = EC_Buf::TBufEC_GetBoolean(Buffer);
        NoScan = EC_Buf::TBufEC_GetBoolean(Buffer);
        ScriptChameleon = EC_Buf::TBufEC_GetBoolean(Buffer);
        PlayerExtortionPactActive = EC_Buf::TBufEC_GetBoolean(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 99) {
            PlayerScratchHitsReceived = EC_Buf::TBufEC_GetWord(Buffer);
        } else {
            PlayerScratchHitsReceived = 0;
        }
        if (GlobalsV::LoadedSaveVersion >= 51) {
            InterceptorPassesRemaining = EC_Buf::TBufEC_GetInt32(Buffer);
            InterceptorSourceShip = reinterpret_cast<TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            if (InterceptorPassesRemaining > 0) {
                if (pas::load_unaligned<char16_t>(reinterpret_cast<char16_t*>(static_cast<std::uint8_t*>(Buffer->Data) + Buffer->Position)) == u'R') {
                    SE_Space::TObjectSE* createSpaceObjectByName_3 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), Buffer->ReadWideString(), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> interceptorGraphic = pas::Var<SE_Space::TObjectSE*>(&InterceptorGraphic);
                    SE_Space::RetainSpaceObject(interceptorGraphic, createSpaceObjectByName_3);
                } else {
                    SE_Space::TObjectSE* createSpaceObjectByName_4 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), Buffer->ReadWideString(), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> interceptorGraphic_2 = pas::Var<SE_Space::TObjectSE*>(&InterceptorGraphic);
                    SE_Space::RetainSpaceObject(interceptorGraphic_2, createSpaceObjectByName_4);
                }
                InterceptorGraphic->SetAlpha(255);
                InterceptorGraphic->SetPosition(Position);
                InterceptorGraphic->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
            } else {
                InterceptorGraphic = nullptr;
            }
        } else {
            InterceptorPassesRemaining = 0;
            InterceptorSourceShip = nullptr;
            InterceptorGraphic = nullptr;
        }
        if (GlobalsV::LoadedSaveVersion >= 85) {
            CurrentStanding = EC_Buf::TBufEC_GetByte(Buffer);
            SmoothedSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
            SmoothedEnemySpeed = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            CurrentStanding = aGalaxyStruct::ssUnaligned;
            SmoothedSpeed = Speed;
            SmoothedEnemySpeed = Speed;
        }
        if (GlobalsV::LoadedSaveVersion >= 90) {
            SmoothedEquipmentEffectiveness = EC_Buf::TBufEC_GetSingle(Buffer);
            SmoothedWealth = EC_Buf::TBufEC_GetInt32(Buffer);
            SmoothedMoneyFraction = EC_Buf::TBufEC_GetSingle(Buffer);
            SmoothedFreeCapacityFraction = EC_Buf::TBufEC_GetSingle(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 138) {
                EquipmentPriceSensitivity = EC_Buf::TBufEC_GetSingle(Buffer);
            } else {
                EquipmentPriceSensitivity = 1.0f;
            }
        } else {
            SmoothedEquipmentEffectiveness = 0.0f;
            SmoothedWealth = 0;
            SmoothedMoneyFraction = 0.0f;
            SmoothedFreeCapacityFraction = 0.0f;
            EquipmentPriceSensitivity = 1.0f;
        }
    }

    // Editable subset; not a full save.
    void TShip::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        std::int32_t RemainingTurns{};
        pas::WideString Text{};
        aItem::TItem* Item{};
        {
            const pas::WideString& fullName = GetFullName(u" "_wref.get());
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"ImFluelalaNrahmaet"_w);
            Block->AddParam(decodeTextW, fullName);
        }
        Block->AddParam(EC_Str::DecodeTextW(u"InToyAple"_w), aConst::ShipTypeNames[TypeId].Name);
        Block->AddParam(EC_Str::DecodeTextW(u"Noasmler"_w), Name);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(PortraitFaceId);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"Fiascoee"_w);
            Block->AddParam(decodeTextW_2, intToStr);
        }
        if (ScriptShip != nullptr) {
            // Decoded: 'IScript'
            const pas::WideString& cpp_arg = pas::concat_wide({reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->Script->ScriptFileName, u",", reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->GetGroup()->Name, u",", reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->State->Name, u"(", pas::wide_int_to_str(pas::list_indexof(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->Script->States, reinterpret_cast<void*>(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->State))), u")"});
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"IsSacaraiOpit"_w);
            Block->AddParam(decodeTextW_3, cpp_arg);
        }
        // Decoded: 'IPlanet'
        if (CurrentPlanet != nullptr) {
            Block->AddParam(EC_Str::DecodeTextW(u"ImPolearnBelt"_w), CurrentPlanet->Name);
        } else {
            Block->AddParam(EC_Str::DecodeTextW(u"ImPolearnBelt"_w), u""_wref.get());
        }
        // Decoded: 'IRuins'
        if (DockedTo != nullptr) {
            Block->AddParam(EC_Str::DecodeTextW(u"ImRyuWirnas"_w), DockedTo->Name);
        } else {
            Block->AddParam(EC_Str::DecodeTextW(u"ImRyuWirnas"_w), u""_wref.get());
        }
        Text = pas::wide_int_to_str(CargoGoods[0].Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 7); cpp_range.next(I); ) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(CargoGoods[static_cast<std::uint8_t>(I)].Count)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"Gronordos"_w), Text);
        Text = pas::wide_int_to_str(static_cast<std::int32_t>(BaseSkills[0]));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 5); cpp_range_2.next(I); ) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(static_cast<std::int32_t>(BaseSkills[static_cast<std::uint8_t>(I)]))});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SekaiAlalas"_w), Text);
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Money);
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"Mnognoenyj"_w);
            Block->AddParam(decodeTextW_4, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(TotalExperience);
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"Eoxepl"_w);
            Block->AddParam(decodeTextW_5, intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(FreeExperience);
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"FarweyeAETxopa"_w);
            Block->AddParam(decodeTextW_6, intToStr_4);
        }
        EC_BlockPar::TBlockParEC* HealthBlock = Block->AddBlockByPath(EC_Str::DecodeTextW(u"Hrenasletaha"_w));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, 24); cpp_range_3.next(I); ) {
            if (IsHealthEffectActive(I)) {
                RemainingTurns = CaptainHealth[I].ExpireTurn - aGalaxy::Galaxy->CurrentTurn;
            } else {
                RemainingTurns = 0;
            }
            Text = pas::concat_wide({aConst::CaptainHealthDefinitions[I].Name, u",", pas::wide_int_to_str(RemainingTurns)});
            HealthBlock->AddParam(pas::concat_wide_reverse({pas::wide_int_to_str(I), EC_Str::DecodeTextW(u"FralcatMoar"_w)}), Text);
        }
        if (HasRadiationSickness()) {
            RemainingTurns = RadiationHealth[1].ExpireTurn - aGalaxy::Galaxy->CurrentTurn;
        } else {
            RemainingTurns = 0;
        }
        Text = pas::concat_wide({aConst::RadiationHealthDefinitions[1].Name, u",", pas::wide_int_to_str(RemainingTurns)});
        HealthBlock->AddParam(pas::concat_wide_reverse({pas::wide_int_to_str(25), EC_Str::DecodeTextW(u"FralcatMoar"_w)}), Text);
        // Decoded: 'EqList'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->AddBlockByPath(EC_Str::DecodeTextW(u"ElqiLoinsato"_w));
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range_4.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Inventory, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->SaveToBlock(cpp_with->AddBlockByPath(Text));
            }
            cpp_with->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w), u""_wref.get());
        }
        // Decoded: 'ArtsList'
        {
            EC_BlockPar::TBlockParEC* cpp_with_2 = Block->AddBlockByPath(EC_Str::DecodeTextW(u"AsrotyseLeidsot"_w));
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_5.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Artefacts, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->SaveToBlock(cpp_with_2->AddBlockByPath(Text));
            }
            cpp_with_2->AddParam(EC_Str::DecodeTextW(u"AsdediAmrot"_w), u""_wref.get());
        }
        // Decoded: 'DropList'
        {
            EC_BlockPar::TBlockParEC* cpp_with_3 = Block->AddBlockByPath(EC_Str::DecodeTextW(u"DarlokpuLainsata"_w));
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(GuaranteedDeathDropItems) - 1); cpp_range_6.next(I); ) {
                Item = pas::list_at<aItem::TItem>(GuaranteedDeathDropItems, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->SaveToBlock(cpp_with_3->AddBlockByPath(Text));
            }
            cpp_with_3->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w), u""_wref.get());
        }
        if (pas::list_count(RangerRelations) > 0) {
            const pas::WideString& intToStr_5 = pas::wide_int_to_str(static_cast<std::int32_t>(static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, 0)))));
            const pas::WideString& decodeTextW_7 = EC_Str::DecodeTextW(u"Rpe7lyamtgi4oendThokP4lWasyfeKry"_w);
            Block->AddParam(decodeTextW_7, intToStr_5);
        }
        Text = EC_Str::DecodeTextW(u"CrolnatariaOcitaeAddTrogSaheiOppIld"_w);
        if (PartnerShip != nullptr) {
            Block->AddParam(Text, pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(PartnerShip->Id))));
        } else {
            Block->AddParam(Text, u"0"_wref.get());
        }
        {
            const pas::WideString& intToStr_6 = pas::wide_int_to_str(PartnershipDaysRemaining);
            const pas::WideString& decodeTextW_8 = EC_Str::DecodeTextW(u"CrolnatariaOcitaDiaOyeseLaeAfoto"_w);
            Block->AddParam(decodeTextW_8, intToStr_6);
        }
        Text = pas::WideString();
        if (AwardIds != nullptr) {
            if (pas::list_count(AwardIds) > 0) {
                Text = pas::wide_int_to_str(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(AwardIds, 0))));
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, pas::list_count(AwardIds) - 1); cpp_range_7.next(I); ) {
                    Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(AwardIds, I))))});
                }
            }
        }
        Block->AddParam(EC_Str::DecodeTextW(u"Mreodlaslis"_w), Text);
        {
            const pas::WideString& boolToWideString = EC_Str::BoolToWideString(DestroyQueued);
            const pas::WideString& decodeTextW_9 = EC_Str::DecodeTextW(u"Dreisatarlony"_w);
            Block->AddParam(decodeTextW_9, boolToWideString);
        }
        Block->AddParam(EC_Str::DecodeTextW(u"GhilvienOrradlehr"_w), u""_wref.get());
        {
            const pas::WideString& boolToWideString_2 = EC_Str::BoolToWideString(NoDrop);
            const pas::WideString& decodeTextW_10 = EC_Str::DecodeTextW(u"NaosDireosp"_w);
            Block->AddParam(decodeTextW_10, boolToWideString_2);
        }
        {
            const pas::WideString& boolToWideString_3 = EC_Str::BoolToWideString(NoTalk);
            const pas::WideString& decodeTextW_11 = EC_Str::DecodeTextW(u"NoooTraslak"_w);
            Block->AddParam(decodeTextW_11, boolToWideString_3);
        }
        {
            const pas::WideString& boolToWideString_4 = EC_Str::BoolToWideString(NoScan);
            const pas::WideString& decodeTextW_12 = EC_Str::DecodeTextW(u"NtorSickamn"_w);
            Block->AddParam(decodeTextW_12, boolToWideString_4);
        }
        Block->AddParam(EC_Str::DecodeTextW(u"Sokoilna"_w), GraphName);
    }

    // Applies edits to existing state and can append items or issue new orders. Cargo quantities change without updating cost basis.
    void TShip::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        std::int32_t OldTurns{};
        std::int32_t NewTurns{};
        pas::WideString Text{};
        pas::WideString Part{};
        aItem::TItem* Item{};
        aGalaxy::THole* Hole{};
        std::uint8_t ItemType{};
        EC_Struct::TPointF Destination{};
        Name = Block->GetParam(EC_Str::DecodeTextW(u"Noasmler"_w));
        PortraitFaceId = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Fiascoee"_w))));
        Text = Block->GetParam(EC_Str::DecodeTextW(u"Gronordos"_w));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            CargoGoods[static_cast<std::uint8_t>(I)].Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get())));
        }
        Text = Block->GetParam(EC_Str::DecodeTextW(u"SekaiAlalas"_w));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 5); cpp_range_2.next(I); ) {
            BaseSkills[static_cast<std::uint8_t>(I)] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get())));
        }
        SetMoney(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Mnognoenyj"_w)))));
        TotalExperience = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Eoxepl"_w))));
        FreeExperience = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"FarweyeAETxopa"_w))));
        // Decoded: 'Health'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->GetBlockByPath(EC_Str::DecodeTextW(u"Hrenasletaha"_w));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, 24); cpp_range_3.next(I); ) {
                if (IsHealthEffectActive(I)) {
                    OldTurns = CaptainHealth[I].ExpireTurn - aGalaxy::Galaxy->CurrentTurn;
                } else {
                    OldTurns = 0;
                }
                Text = cpp_with->GetParam(pas::concat_wide_reverse({pas::wide_int_to_str(I), EC_Str::DecodeTextW(u"FralcatMoar"_w)}));
                NewTurns = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                if (OldTurns > 0 && NewTurns == 0) {
                    CaptainHealth[I].Progress = 0.0;
                    CaptainHealth[I].ExpireTurn = 0;
                }
                if (OldTurns == 0 && NewTurns > 0) {
                    CaptainHealth[I].Progress = 1.0E+2;
                    CaptainHealth[I].ExpireTurn = NewTurns + aGalaxy::Galaxy->CurrentTurn;
                }
                if (OldTurns > 0 && NewTurns > 0) {
                    CaptainHealth[I].ExpireTurn = NewTurns + aGalaxy::Galaxy->CurrentTurn;
                }
            }
            if (HasRadiationSickness()) {
                OldTurns = RadiationHealth[1].ExpireTurn - aGalaxy::Galaxy->CurrentTurn;
            } else {
                OldTurns = 0;
            }
            Text = cpp_with->GetParam(pas::concat_wide_reverse({pas::wide_int_to_str(25), EC_Str::DecodeTextW(u"FralcatMoar"_w)}));
            NewTurns = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            if (OldTurns > 0 && NewTurns == 0) {
                RadiationHealth[1].Progress = 0.0;
                RadiationHealth[1].ExpireTurn = 0;
            }
            if (OldTurns == 0 && NewTurns > 0) {
                RadiationHealth[1].Progress = 0.5;
                RadiationHealth[1].ExpireTurn = NewTurns + aGalaxy::Galaxy->CurrentTurn;
            }
            if (OldTurns > 0 && NewTurns > 0) {
                RadiationHealth[1].ExpireTurn = NewTurns + aGalaxy::Galaxy->CurrentTurn;
            }
        }
        // Decoded: 'EqList'
        {
            EC_BlockPar::TBlockParEC* cpp_with_2 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"ElqiLoinsato"_w));
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range_4.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Inventory, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->virtual_TItem_LoadFromBlock(cpp_with_2->GetBlockByPath(Text));
            }
            Text = cpp_with_2->GetParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w));
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) - 1); cpp_range_5.next(I); ) {
                Part = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
                for (ItemType = static_cast<std::uint8_t>(aConst::t_Food); ItemType <= static_cast<std::uint8_t>(aConst::t_UselessCountableItem); ++ItemType) {
                    if (aConst::ItemTypeNames[ItemType] == Part) {
                        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_Satellite)) && ItemType != static_cast<std::uint8_t>(aConst::t_Hull)) {
                            Item = aItem::CreateDefaultItemByType(static_cast<aConst::TItemType>(ItemType));
                            if (Item != nullptr) {
                                pas::list_add(Inventory, reinterpret_cast<void*>(Item));
                            }
                        }
                        break;
                    }
                }
            }
        }
        // Decoded: 'ArtsList'
        {
            EC_BlockPar::TBlockParEC* cpp_with_3 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"AsrotyseLeidsot"_w));
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_6.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Artefacts, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->virtual_TItem_LoadFromBlock(cpp_with_3->GetBlockByPath(Text));
            }
            Text = cpp_with_3->GetParam(EC_Str::DecodeTextW(u"AsdediAmrot"_w));
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) - 1); cpp_range_7.next(I); ) {
                Part = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
                for (ItemType = static_cast<std::uint8_t>(aConst::t_Food); ItemType <= static_cast<std::uint8_t>(aConst::t_UselessCountableItem); ++ItemType) {
                    if (aConst::ItemTypeNames[ItemType] == Part) {
                        if (aConst::ItemTypeNames[ItemType] == Part && pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks))) {
                            void* createConfiguredArtefactByItemType = reinterpret_cast<void*>(aItem::CreateConfiguredArtefactByItemType(static_cast<aConst::TItemType>(ItemType), 6));
                            aMyFunction::TObjectList* artefacts = Artefacts;
                            pas::list_add(artefacts, createConfiguredArtefactByItemType);
                        }
                        break;
                    }
                }
            }
        }
        // Decoded: 'DropList'
        {
            EC_BlockPar::TBlockParEC* cpp_with_4 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"DarlokpuLainsata"_w));
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(GuaranteedDeathDropItems) - 1); cpp_range_8.next(I); ) {
                Item = pas::list_at<aItem::TItem>(GuaranteedDeathDropItems, I);
                Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                Item->virtual_TItem_LoadFromBlock(cpp_with_4->GetBlockByPath(Text));
            }
            Text = cpp_with_4->GetParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w));
            for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) - 1); cpp_range_9.next(I); ) {
                Part = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
                for (ItemType = static_cast<std::uint8_t>(aConst::t_Food); ItemType <= static_cast<std::uint8_t>(aConst::t_UselessCountableItem); ++ItemType) {
                    if (aConst::ItemTypeNames[ItemType] == Part) {
                        if ((pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics)) || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon)) || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks)) || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Protoplasm), static_cast<std::int32_t>(aConst::t_Satellite))) && ItemType != static_cast<std::uint8_t>(aConst::t_Hull)) {
                            Item = aItem::CreateDefaultItemByType(static_cast<aConst::TItemType>(ItemType));
                            if (Item != nullptr) {
                                pas::list_add(GuaranteedDeathDropItems, reinterpret_cast<void*>(Item));
                            }
                        }
                        break;
                    }
                }
            }
        }
        if (pas::list_count(RangerRelations) > 0) {
            pas::list_put(RangerRelations, 0, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Rpe7lyamtgi4oendThokP4lWasyfeKry"_w))))))));
        }
        Text = Block->GetParam(EC_Str::DecodeTextW(u"CrolnatariaOcitaeAddTrogSaheiOppIld"_w));
        if (SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)) == 0) {
            PartnerShip = nullptr;
        } else {
            PartnerShip = static_cast<TShip*>(aGalaxy::Galaxy->IdToShip(SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)), true));
        }
        PartnershipDaysRemaining = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"CrolnatariaOcitaDiaOyeseLaeAfoto"_w))));
        Text = Block->GetParam(EC_Str::DecodeTextW(u"Mreodlaslis"_w));
        if (AwardIds != nullptr) {
            pas::free(AwardIds);
        }
        AwardVisibleCount = 0;
        AwardIds = nullptr;
        if (Text != u"") {
            AwardIds = pas::make_object<pas::List>();
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) - 1); cpp_range_10.next(I); ) {
                Part = EC_Str::ExtractDelimitedPartW(Text, I, u","_wref.get());
                pas::list_add(AwardIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Part))))));
            }
            AwardVisibleCount = EC_Str::CountDelimitedPartsW(Text, u","_wref.get());
        }
        DestroyQueued = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Dreisatarlony"_w)))) == "true";
        Text = Block->GetParam(EC_Str::DecodeTextW(u"GhilvienOrradlehr"_w));
        if (EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) > 1) {
            // Decoded: 'JumpToStar'
            if (([&] {
                pas::WideString cpp_string = EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get());
                pas::WideString cpp_string_2 = EC_Str::DecodeTextW(u"JiunmApeThorSitraer"_w);
                return cpp_string == cpp_string_2;
            }())) {
                if (CurrentPlanet == nullptr && DockedTo == nullptr) {
                    OrderJump(aGalaxy::Galaxy->IdToStar(SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())))), true);
                    if (EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) > 2) {
                        OrderStateData = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
                    }
                }
            } else if (([&] {
                pas::WideString cpp_string_3 = EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get());
                pas::WideString cpp_string_4 = EC_Str::DecodeTextW(u"JiunmApeIonsHroelMel"_w);
                return cpp_string_3 == cpp_string_4;
            }())) {
                Hole = aGalaxy::Galaxy->IdToHole(SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()))));
                if (CurrentPlanet == nullptr && DockedTo == nullptr && (Hole->Star1 == CurrentStar || Hole->Star2 == CurrentStar)) {
                    OrderJumpHole(Hole, true);
                    if (EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) > 2) {
                        OrderStateData = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
                    }
                }
            } else if (([&] {
                pas::WideString cpp_string_5 = EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get());
                pas::WideString cpp_string_6 = EC_Str::DecodeTextW(u"MiokvaenThor"_w);
                return cpp_string_5 == cpp_string_6;
            }()) && InNormalSpace()) {
                Destination.X = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                Destination.Y = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                OrderMove(Destination, true);
            }
        }
        NoDrop = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"NaosDireosp"_w)))) == "true";
        NoTalk = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"NoooTraslak"_w)))) == "true";
        NoScan = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"NtorSickamn"_w)))) == "true";
        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
        GraphName = Block->GetParam(EC_Str::DecodeTextW(u"Sokoilna"_w));
        if (GraphName.read(1) == u'R') {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), GraphName, ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
        } else {
            SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), GraphName, ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic_2 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic_2, createSpaceObjectByName_2);
        }
        RefreshDerivedStats(true);
        RefreshGraphicSize();
        TShip::DerivedStateCompatibilityHook();
    }

    // Converts saved IDs to object references; requires all referenced objects to have been loaded.
    void TShip_ResolveLoadedReferences(TShip* Self, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t Count{};
        aItem::TItem* Item{};
        pas::list_add(Galaxy->LoadedShips, reinterpret_cast<void*>(Self));
        Self->TransitOriginStar = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->TransitOriginStar)))));
        Self->CurrentPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->CurrentPlanet)), true)));
        if (pas::class_cast_if<aPlayer::TPlayer*>(Self) != nullptr && static_cast<aPlayer::TPlayer*>(Self)->RuinsMode > 0) {
            Self->DockedTo = pas::checked_cast<aPlayer::TPlayer*>(Self)->RuinsProxy;
        } else {
            Self->DockedTo = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->DockedTo)), true)));
        }
        Self->HomePlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->HomePlanet)), true)));
        if (GlobalsV::LoadedSaveVersion < 146 && Self->HomePlanet != nullptr) {
            Self->TechKnowledge = std::max<std::int32_t>(static_cast<std::int32_t>(Self->TechKnowledge), static_cast<std::int32_t>(Self->HomePlanet->InventionLevels[7]));
        }
        if (Self->PickupTargets != nullptr) {
            Count = pas::list_count(Self->PickupTargets);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                pas::list_put(Self->PickupTargets, I, reinterpret_cast<void*>(pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Galaxy->IdToItem(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Self->PickupTargets, I))), true)))));
            }
        }
        Self->EnemyShip = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->EnemyShip)), false)));
        Self->TruceShip = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->TruceShip)), false)));
        Self->PartnerShip = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->PartnerShip)), false)));
        Self->InterceptorSourceShip = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->InterceptorSourceShip)), false)));
        if (Self->Order == soJump) {
            Self->OrderTarget = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)))));
        } else if (Self->Order == soJumpHole) {
            Self->OrderTarget = pas::checked_cast<aGalaxy::THole*>(static_cast<pas::Object*>(Galaxy->IdToHole(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)))));
        } else if (Self->Order == soTeleport) {
            Self->OrderTarget = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)))));
        } else if (Self->Order == soLand) {
            if ((static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)) & 0x80000000u) == 0x80000000u) {
                Self->OrderTarget = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)) & 0x7fffffff, true)));
            } else {
                Self->OrderTarget = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)), true)));
            }
        } else if (Self->Order == soFollowShip) {
            Self->OrderTarget = pas::checked_cast<TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->OrderTarget)), true)));
        }
        Count = pas::list_count(Self->Inventory);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Self->Inventory, I);
            Item->ResolveLoadedReferences(Galaxy);
        }
        Count = pas::list_count(Self->Artefacts);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Self->Artefacts, I);
            Item->ResolveLoadedReferences(Galaxy);
        }
        Count = pas::list_count(Self->GuaranteedDeathDropItems);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Self->GuaranteedDeathDropItems, I);
            Item->ResolveLoadedReferences(Galaxy);
        }
        if (GlobalsV::LoadedSaveVersion < 85 && Self->TypeId != aGalaxyStruct::stTranclucator) {
            Self->virtual_TShip_RefreshCurrentStanding();
        }
        if (pas::in_set<92, 93>(GlobalsV::LoadedSaveVersion) && pas::class_cast_if<aNormalShip::TNormalShip*>(Self) != nullptr && !(pas::class_cast_if<aPlayer::TPlayer*>(Self) != nullptr)) {
            // Preserve the native legacy portrait exception, including its repeated type test.
            if (Self->PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiHuman) && pas::in_range(Self->PortraitFaceId, 25, 32) && aPlayer::GetPlayer() != Self && aPlayer::GetPlayer() != nullptr && pas::class_cast_if<aNormalShip::TNormalShip*>(Self) != nullptr) {
                return;
            }
            if (pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr && Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                Self->GetHull()->OwnerId = aConst::RaceToOwner(Self->PilotRace);
            } else {
                Self->GetHull()->OwnerId = Self->OwnerId;
            }
            Self->GetHull()->HullType = aConst::ShipToHullType(Self);
            Self->GetHull()->SpecialModuleIndex = 0;
            if (Self->GetHull()->HullSeries != -1) {
                if (static_cast<std::uint8_t>(pas::contains(aConst::HullSeriesDefinitions[Self->GetHull()->HullSeries].AllowedOwners, Self->GetHull()->OwnerId) ^ 1) || static_cast<std::uint8_t>(pas::contains(aConst::HullSeriesDefinitions[Self->GetHull()->HullSeries].AllowedShipTypes, Self->GetHull()->HullType) ^ 1)) {
                    Self->GetHull()->HullSeries = -1;
                }
            }
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
            aShip::TShip_RefreshGraphic(Self);
        }
    }

    // Clamps to 0..100000000.
    void TShip::SetMoney(std::int32_t Value) {
        if (Value > 100000000) {
            Value = 100000000;
        } else if (Value < 0) {
            Value = 0;
        }
        if (static_cast<std::int32_t>(EncodedMoney ^ 0xa4a576adu) != Money && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        Money = Value;
        if (aPlayer::GetPlayer() == this) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckMoneyAchievement();
            SysUtilsImports::Sleep(1u);
            if (Money != Value && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
        EncodedMoney = Value ^ 0xa4a576adu;
    }

    // Can cancel the player's jump when remaining fuel is insufficient.
    void TShip::ProcessBrokenFuelTankLeak() {
        std::int32_t Lost{};
        if (GetFuelTanks() == nullptr || GetFuelTanks()->BrokenFlag == 0 || CountActiveArtefacts(aConst::t_ArtefactFuel) > 0) {
            return;
        }
        double Roll = aMyFunction::NextRandomUnitFloat(RandomState);
        if (Roll < 0.1L) {
            Lost = 1;
        } else if (Roll < 0.3L) {
            Lost = 2;
        } else if (Roll < 0.7L) {
            Lost = 3;
        } else if (Roll < 0.9L) {
            Lost = 4;
        } else {
            Lost = 5;
        }
        if (GetFuelTanks()->Fuel < Lost) {
            Lost = GetFuelTanks()->Fuel;
        }
        GetFuelTanks()->Fuel -= Lost;
        if (aPlayer::GetPlayer() == this) {
            auto& cpp_target = ([&] {
                const pas::WideString& localizedText = aConst::LocalizedText(u"Items.FuelTanks.LostFuel"_wref.get());
                std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                return Globals::AddOrUpdatePlayerBubble(8, currentTurn, localizedText, u""_wref.get());
            }())->Targets[0].ShipId;
            cpp_target = Id;
        }
        if (aPlayer::GetPlayer() == this && InNormalSpace() && Order == soJump) {
            if (GetFuelTanks()->Fuel < System::Round(aMyFunction::PointDistance(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Position, CurrentStar->Position))) {
                {
                    auto& cpp_target_2 = ([&] {
                        const pas::WideString& formatText1 = ([&] {
                            auto name = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Name);
                            pas::WideString localizedText_2 = aConst::LocalizedText(u"Items.FuelTanks.NoFuelJump"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
                        }());
                        std::int32_t currentTurn_2 = aGalaxy::Galaxy->CurrentTurn;
                        return Globals::AddOrUpdatePlayerBubble(8, currentTurn_2, formatText1, u""_wref.get());
                    }())->Targets[0].ShipId;
                    cpp_target_2 = Id;
                }
                OrderMove(OrderDestination, false);
            }
        }
    }

    // Zero-byte pointer additions below retain native argument evaluation order
    // without narrowing object addresses to Integer.
    // Can unlock carried equipment use/repair and notify the player.
    void TShip::RefreshTechKnowledgeAtLocation() {
        pas::List* UseList{};
        pas::List* RepairList{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        if (DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(DockedTo) != nullptr || CurrentPlanet != nullptr && pas::in_set<0, 4, 7, 7>(CurrentPlanet->OwnerId) && reinterpret_cast<aPlanet::TPlanet*>(reinterpret_cast<std::uint8_t*>(CurrentPlanet) + 0)->GetRelationLevelToShip(this) != 0 || pas::class_cast_if<aRuins::TRuins*>(this) != nullptr) {
            if (aPlayer::GetPlayer() != this) {
                TechKnowledge = std::max<std::int32_t>(static_cast<std::int32_t>(TechKnowledge), static_cast<std::int32_t>(aGalaxy::Galaxy->TechLevel));
            } else {
                UseList = pas::make_object<pas::List>();
                RepairList = pas::make_object<pas::List>();
                for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                    Item = pas::list_at<aItem::TEquipment>(reinterpret_cast<pas::List*>(reinterpret_cast<std::uint8_t*>(Inventory) + 0), I);
                    if (!aShip::TShip_CanUseEquipmentTech(this, Item)) {
                        pas::list_add(UseList, reinterpret_cast<void*>(Item));
                    }
                    if (!aShip::TShip_CanRepairEquipmentTech(this, Item)) {
                        pas::list_add(RepairList, reinterpret_cast<void*>(Item));
                    }
                }
                TechKnowledge = std::max<std::int32_t>(static_cast<std::int32_t>(TechKnowledge), static_cast<std::int32_t>(aGalaxy::Galaxy->TechLevel));
                {
                    const std::int32_t cpp_first = pas::list_count(UseList) - 1;
                    if (cpp_first >= 0) {
                        for (I = cpp_first; I >= 0; --I) {
                            Item = pas::list_at<aItem::TEquipment>(UseList, I);
                            if (!aShip::TShip_CanUseEquipmentTech(this, Item)) {
                                pas::list_delete(UseList, I);
                            }
                        }
                    }
                }
                {
                    const std::int32_t cpp_first_2 = pas::list_count(RepairList) - 1;
                    if (cpp_first_2 >= 0) {
                        for (I = cpp_first_2; I >= 0; --I) {
                            Item = pas::list_at<aItem::TEquipment>(RepairList, I);
                            if (!aShip::TShip_CanRepairEquipmentTech(this, Item)) {
                                pas::list_delete(RepairList, I);
                            }
                        }
                    }
                }
                if (pas::list_count(UseList) > 0) {
                    Text = aConst::LocalizedText(u"Items.Equpments.NowCanUse"_wref.get());
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(UseList) - 1); cpp_range_2.next(I); ) {
                        Item = pas::list_at<aItem::TEquipment>(UseList, I);
                        Text = pas::concat_wide({Text, u"\r\n", u"- ", Item->GetDisplayName()});
                    }
                    Globals::AddOrUpdatePlayerBubble(2, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                }
                if (pas::list_count(RepairList) > 0) {
                    Text = aConst::LocalizedText(u"Items.Equpments.NowCanRepair"_wref.get());
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(RepairList) - 1); cpp_range_3.next(I); ) {
                        Item = pas::list_at<aItem::TEquipment>(RepairList, I);
                        Text = pas::concat_wide({Text, u"\r\n", u"- ", Item->GetDisplayName()});
                    }
                    Globals::AddOrUpdatePlayerBubble(2, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                }
                pas::free(UseList);
                pas::free(RepairList);
                aPlayer::GetPlayer()->RefreshStorageBubbles();
            }
        }
    }

    void TShip_NextDay(TShip* Self) {
        std::int32_t I{};
        float WearFactor{};
        aKling::TKling* Kling{};
        std::int32_t Stage = 0;
        try {
            if (Self->EnemyShip == Self) {
                Self->EnemyShip = nullptr;
            }
            if (Self->Order == soFollowShip) {
                if (aShip::TShip_GetRelationLevelToShip(Self, reinterpret_cast<TShip*>(Self->OrderTarget)) > aGalaxyStruct::rlHostile) {
                    if (Self->PartnerShip != Self->OrderTarget) {
                        if (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) || pas::checked_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip != Self->OrderTarget) {
                            if (aPlayer::GetPlayer() != Self && static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1)) {
                                Self->OrderNone(false);
                            }
                        }
                    }
                }
            }
            Stage = 1;
            Self->CancelInvalidTravelOrder();
            if (Self->InHyperspace || Self->CurrentPlanet != nullptr) {
                Self->TruceShip = nullptr;
            }
            if (!Self->InNormalSpace()) {
                Self->ClearCombatStatusEffects();
                Self->ClearRecentlyDroppedItems();
            }
            Self->RefreshTechKnowledgeAtLocation();
            Stage = 2;
            if (aGalaxy::Galaxy->CurrentTurn > Self->LastProcessedTurn || aGalaxy::Galaxy->StasisModEnabled == 1 && aPlayer::GetPlayer() == Self) {
                if ((Self->CurrentPlanet != nullptr || Self->DockedTo != nullptr) && !(aPlayer::GetPlayer() == Self && GlobalsV::CurrentScreenId == GlobalsV::screenPlanetQuest)) {
                    ++Self->ConsecutiveDockedDays;
                } else {
                    Self->ConsecutiveDockedDays = 0;
                }
                ++Self->DaysSincePlayerSeen;
                Self->LastProcessedTurn = aGalaxy::Galaxy->CurrentTurn;
                Self->SimulateNpcHealthEffects();
                Stage = 3;
                if (aPlayer::GetPlayer() != Self && Self->TypeId != aGalaxyStruct::stTranclucator) {
                    Self->AutoEquipInventory();
                    Self->AutoEquipArtefacts();
                    if (Self->InNormalSpace()) {
                        aShip::TShip_DropCargoUntilNotOverloaded(Self);
                    }
                    if (!(pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr) && Self->DaysSincePlayerSeen > 100 && (aGalaxy::Galaxy->CurrentTurn + Self->Id) % 50 == 0) {
                        Self->ReloadWeaponAmmo();
                    }
                }
                if (Self->PartnerShip != nullptr && Self->PartnershipDaysRemaining > 0) {
                    --Self->PartnershipDaysRemaining;
                }
                Stage = 4;
                if (Self->GetHull()->Weight > Self->GetHull()->HullPoints) {
                    Self->ApplyRepairDroidHealing();
                }
                if (Self->GetHull()->EnergyMax > Self->GetHull()->Energy) {
                    Self->GetHull()->Energy = std::min<std::int32_t>(Self->GetHull()->EnergyMax, Self->GetHull()->Energy + Self->GetHullEnergyRegeneration());
                }
                Stage = 5;
                if (Self->GetEngine() != nullptr && Self->GetEngine()->OutputPercent < 100) {
                    Self->RefreshDerivedStats(true);
                    if (Self->GetEngine()->OutputPercent + 10 > 100) {
                        Self->GetEngine()->OutputPercent = 100;
                    } else {
                        Self->GetEngine()->OutputPercent += 10;
                    }
                }
                Stage = 6;
                Self->ProcessBrokenFuelTankLeak();
                Stage = 7;
                if (pas::list_count(Self->Artefacts) > 0) {
                    if (Self->GetFuelTanks() != nullptr) {
                        if (Self->GetFuelTanks()->Fuel < Self->GetFuelTanks()->Capacity) {
                            Self->GetFuelTanks()->Fuel += std::min<std::int32_t>(([&] {
                                std::int32_t cpp_left = aConst::FuelArtefactBase + aConst::FuelArtefactBoost * (Self->CanBoostArtefact(aConst::t_ArtefactFuel, nullptr, false) & 127);
                                return cpp_left * Self->CountActiveArtefacts(aConst::t_ArtefactFuel);
                            }()), Self->GetFuelTanks()->Capacity - Self->GetFuelTanks()->Fuel);
                        }
                    }
                    if (Self->GetEngine() != nullptr && Self->GetEngine()->OutputPercent < 100) {
                        Self->GetEngine()->OutputPercent += std::min<std::int32_t>(([&] {
                            std::int32_t cpp_left_2 = aConst::EngineArtefactBase + aConst::EngineArtefactBoost * (Self->CanBoostArtefact(aConst::t_ArtefactPower, Self->GetEngine(), false) & 127);
                            return cpp_left_2 * Self->CountActiveArtefacts(aConst::t_ArtefactPower);
                        }()), 100 - Self->GetEngine()->OutputPercent);
                    }
                    for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtefactNano)); cpp_range.next(I); ) {
                        Self->ApplyNanoArtefactRepair();
                    }
                    Self->RefreshDerivedStats(true);
                }
                Stage = 8;
                if (aPlayer::GetPlayer() == Self) {
                    if (Self->InNormalSpace()) {
                        Stage = 9;
                        if (Self->IsHealthEffectActive(10)) {
                            WearFactor = 3.0f;
                        } else {
                            WearFactor = 1.0f;
                        }
                        if (pas::is_one_of<soMove, soLand, soJump, soTakeoff, soFollowShip>(Self->Order)) {
                            {
                                pas::Extended cpp_right = aMyFunction::RemapClamped(Self->CalculateMass(), aConst::WearMassMin, aConst::WearMassMax, 1.0, 1.0E+1);
                                double cpp_arg = aMyFunction::NextRandomUnitFloat(Self->RandomState) * 0.5L * cpp_right * WearFactor;
                                aItem::TEquipment* engine = Self->GetEngine();
                                TShip* self = Self;
                                aShip::TShip_ApplyItemDegradation(self, engine, idkUse, cpp_arg);
                            }
                            {
                                double cpp_arg_2 = aMyFunction::NextRandomUnitFloat(Self->RandomState) * 0.5L * WearFactor;
                                aItem::TEquipment* fuelTanks = Self->GetFuelTanks();
                                TShip* self_2 = Self;
                                aShip::TShip_ApplyItemDegradation(self_2, fuelTanks, idkUse, cpp_arg_2);
                            }
                            if (Self->GetEngine() != nullptr) {
                                if (Self->GetEngine()->BrokenFlag != 0) {
                                    if (Self->Order == soJump) {
                                        std::int64_t cpp_right_2 = System::Round(aMyFunction::PointDistance(pas::checked_cast<aGalaxy::TStar*>(Self->OrderTarget)->Position, Self->CurrentStar->Position));
                                        if (Self->GetFuelLimitedJumpRange() < cpp_right_2) {
                                            {
                                                auto& cpp_target = ([&] {
                                                    const pas::WideString& formatText1 = ([&] {
                                                        auto name = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(Self->OrderTarget)->Name);
                                                        pas::WideString localizedText = aConst::LocalizedText(u"Items.Engine.NoPowerJump"_wref.get());
                                                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
                                                    }());
                                                    std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                                                    return Globals::AddOrUpdatePlayerBubble(8, currentTurn, formatText1, u""_wref.get());
                                                }())->Targets[0].ShipId;
                                                cpp_target = Self->Id;
                                            }
                                            Self->OrderMove(Self->OrderDestination, false);
                                        }
                                    }
                                }
                            }
                        }
                        if (Self->GetRadar() != nullptr) {
                            double cpp_arg_3 = aMyFunction::NextRandomUnitFloat(Self->RandomState) * 0.3L * WearFactor;
                            aItem::TEquipment* radar = Self->GetRadar();
                            TShip* self_3 = Self;
                            aShip::TShip_ApplyItemDegradation(self_3, radar, idkUse, cpp_arg_3);
                        }
                        if (Self->GetScanner() != nullptr) {
                            double cpp_arg_4 = aMyFunction::NextRandomUnitFloat(Self->RandomState) * 0.3L * WearFactor;
                            aItem::TEquipment* scanner = Self->GetScanner();
                            TShip* self_4 = Self;
                            aShip::TShip_ApplyItemDegradation(self_4, scanner, idkUse, cpp_arg_4);
                        }
                        if (Self->GetDefGenerator() != nullptr) {
                            double cpp_arg_5 = aMyFunction::NextRandomUnitFloat(Self->RandomState) * 0.3L * WearFactor;
                            aItem::TEquipment* defGenerator = Self->GetDefGenerator();
                            TShip* self_5 = Self;
                            aShip::TShip_ApplyItemDegradation(self_5, defGenerator, idkUse, cpp_arg_5);
                        }
                        {
                            double nextRandomFloatRange = aMyFunction::NextRandomFloatRange(0.1L * WearFactor, 0.3L * WearFactor, Self->RandomState);
                            TShip* self_6 = Self;
                            self_6->ApplyArtefactUseDegradation(nextRandomFloatRange);
                        }
                    }
                    if (Self->DockedTo != nullptr) {
                        Stage = 10;
                        Self->SynchronizeDockedLocation();
                    }
                } else if (Self->InNormalSpace()) {
                    Stage = 11;
                    {
                        double nextRandomFloatRange_2 = aMyFunction::NextRandomFloatRange(0.1, 0.3, Self->RandomState);
                        TShip* self_7 = Self;
                        self_7->ApplyArtefactUseDegradation(nextRandomFloatRange_2);
                    }
                    if (pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stTransport, aGalaxyStruct::stPirate>(Self->TypeId) && Self->CalculateMass() > aConst::WearMassMin && Self->Order != soNone) {
                        pas::Extended cpp_right_3 = aMyFunction::RemapClamped(Self->CalculateMass(), aConst::WearMassMin, aConst::WearMassMax, 0.0, 9.0);
                        double cpp_arg_6 = aMyFunction::NextRandomUnitFloat(Self->RandomState) * 0.5L * cpp_right_3 * 0.5L;
                        aItem::TEquipment* engine_2 = Self->GetEngine();
                        TShip* self_8 = Self;
                        aShip::TShip_ApplyItemDegradation(self_8, engine_2, idkUse, cpp_arg_6);
                    }
                    if (Self->AfterburnerActive) {
                        Self->AfterburnerActive = false;
                        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) {
                            Self->AfterburnerActive = Self->GetSlotCount(aConst::sskAfterburner) > 0 && Self->GetEngine() != nullptr && Self->GetEngine()->ConditionPercent > 1.0E+1L && Self->EstimateOrderTravelTurns() > 1;
                        }
                    }
                    if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr && static_cast<aKling::TKling*>(Self)->ActiveProgramAppliedTurn > 0) {
                        Stage = 12;
                        Kling = pas::checked_cast<aKling::TKling*>(Self);
                        {
                            std::uint8_t cpp_case = Kling->ActiveProgramId;
                            if (cpp_case == aGalaxyStruct::prgShipwreck) {
                                Kling->ActiveProgramAppliedTurn = 0;
                            } else if (cpp_case >= aGalaxyStruct::prgWeaponBlocking && cpp_case <= aGalaxyStruct::prgShock) {
                                if (aGalaxy::Galaxy->CurrentTurn - aConst::ProgramDuration[Kling->ActiveProgramId] > Kling->ActiveProgramAppliedTurn) {
                                    Kling->ActiveProgramAppliedTurn = 0;
                                }
                            }
                        }
                    }
                }
                Stage = 13;
                Self->DecayCombatStatusEffects();
                Self->UpdateSpeedTrackingMetrics();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TShip.NextDay ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    void TShip_NextDayLogic(TShip* Self) {
    }

    // The base implementation clears all weapon targets.
    void TShip::AssignWeaponTargetsInStar() {
        std::int32_t I{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapons[I]->Target = nullptr;
                }
            }
        }
    }

    // Native no-op, called after loading/editing and refreshing derived stats.
    void TShip::DerivedStateCompatibilityHook() {
    }

    pas::WideString TShip::GetTypeNameKey() {
        return aConst::ShipTypeNames[TypeId].Name;
    }

    pas::WideString TShip::GetLocalizedTypeName() {
        if (TypeNameOverrideKey != u"") {
            return aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey}));
        } else if (pas::class_cast_if<aWarrior::TWarrior*>(this) != nullptr && static_cast<aWarrior::TWarrior*>(this)->WarriorType == aWarrior::wtFlagship) {
            return aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", GetTypeNameKey(), u"Big"}));
        } else {
            return aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", GetTypeNameKey()}));
        }
    }

    pas::WideString TShip::GetFactionNameKey() {
        if (HasNamedScriptFaction()) {
            return reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText;
        } else if (pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
            return aConst::DominatorSeriesNames[pas::checked_cast<aKling::TKling*>(this)->DominatorSeries];
        } else {
            return aConst::OwnerInfo[OwnerId].InternalName;
        }
    }

    std::uint8_t TShip::GetDefaultHullType() {
        std::uint8_t Kind{};
        {
            std::uint8_t cpp_case = TypeId;
            if (cpp_case == aGalaxyStruct::stKling) {
                Kind = aGalaxyStruct::htKling;
            } else if (cpp_case == aGalaxyStruct::stRanger) {
                Kind = aGalaxyStruct::htRanger;
            } else if (cpp_case == aGalaxyStruct::stTransport) {
                switch (pas::checked_cast<aTransport::TTransport*>(this)->TransportType) {
                    case aTransport::ttTransport: Kind = aGalaxyStruct::htTransport; break;
                    case aTransport::ttLiner: Kind = aGalaxyStruct::htLiner; break;
                    case aTransport::ttDiplomat: Kind = aGalaxyStruct::htDiplomat; break;
                    default: Kind = aGalaxyStruct::htRanger; break;
                }
            } else if (cpp_case == aGalaxyStruct::stPirate) {
                Kind = aGalaxyStruct::htPirate;
            } else if (cpp_case == aGalaxyStruct::stWarrior) {
                switch (pas::checked_cast<aWarrior::TWarrior*>(this)->WarriorType) {
                    case aWarrior::wtRegular: Kind = aGalaxyStruct::htWarrior; break;
                    case aWarrior::wtFlagship: Kind = aGalaxyStruct::htFlagship; break;
                    default: Kind = aGalaxyStruct::htWarrior; break;
                }
            } else if (cpp_case == aGalaxyStruct::stTranclucator) {
                Kind = aGalaxyStruct::htTranclucator;
            } else if (cpp_case >= aGalaxyStruct::rstRangerCenter && cpp_case <= aGalaxyStruct::rstCustomStation) {
                Kind = aGalaxyStruct::htStation;
            } else {
                Kind = aGalaxyStruct::htRanger;
            }
        }
        return Kind;
    }

    std::int32_t TShip::NextRandomInteger(std::int32_t Minimum, std::int32_t Maximum) {
        return aMyFunction::NextRandomIntRange(Minimum, Maximum, RandomState);
    }

    // Requires a player; includes current order, hull, speed and relation information.
    pas::WideString TShip_GetSpaceInfoText(TShip* Self) {
        pas::WideString Result{};
        Result = Self->GetName();
        switch (Self->Order) {
            case soLand: {
                if (pas::class_cast_if<aPlanet::TPlanet*>(Self->OrderTarget) != nullptr) {
                    Result = pas::concat_wide({Result, u" ", ([&] {
                        pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.LandingToPlanet"_wref.get());
                        pas::WideString name = pas::checked_cast<aPlanet::TPlanet*>(Self->OrderTarget)->Name;
                        return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey), u"<Planet>"_w, std::move(name), u"<color=255,240,100>"_w);
                    }())});
                } else if (pas::class_cast_if<TShip*>(Self->OrderTarget) != nullptr) {
                    Result = pas::concat_wide({Result, u" ", ([&] {
                        pas::WideString lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.LandingToShip"_wref.get());
                        pas::WideString name_2 = pas::checked_cast<TShip*>(Self->OrderTarget)->Name;
                        return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey_2), u"<Ship>"_w, std::move(name_2), u"<color=255,240,100>"_w);
                    }())});
                }
                break;
            }
            case soJump: {
                Result = pas::concat_wide({Result, u" ", ([&] {
                    pas::WideString lookupLocalizedTextByKey_3 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.GoToStar"_wref.get());
                    pas::WideString name_3 = pas::checked_cast<aGalaxy::TStar*>(Self->OrderTarget)->Name;
                    return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey_3), u"<Star>"_w, std::move(name_3), u"<color=255,240,100>"_w);
                }())});
                break;
            }
            case soFollowShip: {
                if (pas::checked_cast<TShip*>(Self->OrderTarget)->CurrentPlanet == nullptr) {
                    if (Self->EnemyShip != Self->OrderTarget) {
                        Result = pas::concat_wide({Result, u" ", ([&] {
                            pas::WideString name_4 = pas::checked_cast<TShip*>(Self->OrderTarget)->GetName();
                            pas::WideString lookupLocalizedTextByKey_4 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.GoToShip"_wref.get());
                            return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey_4), u"<Ship>"_w, std::move(name_4), u"<color=255,240,100>"_w);
                        }())});
                    } else {
                        Result = pas::concat_wide({Result, u" ", ([&] {
                            pas::WideString name_5 = pas::checked_cast<TShip*>(Self->OrderTarget)->GetName();
                            pas::WideString lookupLocalizedTextByKey_5 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.GoToShipBad"_wref.get());
                            return aMyFunction::ReplaceColoredToken(std::move(lookupLocalizedTextByKey_5), u"<Ship>"_w, std::move(name_5), u"<color=255,240,100>"_w);
                        }())});
                    }
                } else {
                    Result = pas::concat_wide({Result, u" ", GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.None"_wref.get())});
                }
                break;
            }
            case soMove: {
                Result = pas::concat_wide({Result, u" ", GR_Main::LookupLocalizedTextByKey(u"ShipInfo.Order.Move"_wref.get())});
                break;
            }
        }
        Result = pas::concat_wide({Result, u"\r\n", ([&] {
            pas::WideString intToStr = pas::wide_int_to_str(Self->GetHull()->Weight);
            pas::WideString lookupLocalizedTextByKey_6 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.SpaceSize"_wref.get());
            return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_6), u"<color=255,240,100>"_w, u"<Size>"_w, std::move(intToStr));
        }())});
        if (Self->GetHull()->Weight > Self->GetHull()->HullPoints) {
            Result = pas::concat_wide({Result, u" ", ([&] {
                pas::WideString int64ToStr = pas::wide_int64_to_str(System::Trunc(1.0E+2L - pas::real_divide(Self->GetHull()->HullPoints, Self->GetHull()->Weight * 0.01L)));
                pas::WideString lookupLocalizedTextByKey_7 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.SpaceDamageProc"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_7), u"<color=255,240,100>"_w, u"<Proc>"_w, std::move(int64ToStr));
            }())});
        }
        Result = pas::concat_wide({Result, u"\r\n", ([&] {
            pas::WideString intToStr_2 = pas::wide_int_to_str(Self->CalculateSpeed());
            pas::WideString lookupLocalizedTextByKey_8 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.SpaceSpeed"_wref.get());
            return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_8), u"<color=255,240,100>"_w, u"<Speed>"_w, std::move(intToStr_2));
        }())});
        Result = pas::concat_wide({Result, u"\r\n", ([&] {
            pas::WideString intToStr_3 = pas::wide_int_to_str(Self->GetDefensePercent() & 0x0000007f);
            pas::WideString lookupLocalizedTextByKey_9 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.SpaceDefField"_wref.get());
            return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_9), u"<color=255,240,100>"_w, u"<Proc>"_w, std::move(intToStr_3));
        }())});
        if (aPlayer::GetPlayer() != Self) {
            Result = pas::concat_wide({Result, u"\r\n", ([&] {
                pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aShip::TShip_GetRelationLevelTextToShip(Self, aPlayer::GetPlayer()));
                pas::WideString lookupLocalizedTextByKey_10 = GR_Main::LookupLocalizedTextByKey(u"ShipInfo.SpaceRelation"_wref.get());
                return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_10), u"<color=255,240,100>"_w, u"<Type>"_w, std::move(lowerCaseWideString));
            }())});
            if (aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
                return pas::concat_wide({Result, u"\r\n", ([&] {
                    pas::WideString intToStr_4 = pas::wide_int_to_str(aShip::TShip_GetWinChancePercent(aPlayer::GetPlayer(), Self) & 0x0000007f);
                    pas::WideString lookupLocalizedTextByKey_11 = GR_Main::LookupLocalizedTextByKey(u"Artefacts.Analyzer.TextToRadar"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_11), u"<color=255,240,100>"_w, u"<ChanceToWin>"_w, std::move(intToStr_4));
                }())});
            }
        }
        return Result;
    }

    std::int32_t TShip::GetDesiredCargoFreeSpace() {
        return 0;
    }

    std::uint8_t TShip::IsHullDestroyed() {
        return GetHull()->HullPoints <= 0;
    }

    // Fractional part of signed Seed divided by CurrentTurn + TurnOffset; denominator must be nonzero. Does not advance RandomState.
    float TShip::GetTurnSeedFraction(std::int32_t TurnOffset) {
        return System::Frac(pas::real_divide(static_cast<std::int32_t>(Seed), aGalaxy::Galaxy->CurrentTurn + TurnOffset));
    }

    std::uint8_t TShip::HasCargoGoods() {
        std::uint8_t Good{};
        std::uint8_t Result = false;
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (CargoGoods[Good].Count > 0) {
                Result = true;
                break;
            }
        }
        return Result;
    }

    // Counts positive cargo quantities.
    std::uint8_t TShip::CountCargoGoodsTypes() {
        std::uint8_t Good{};
        std::uint8_t Result = 0;
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (CargoGoods[Good].Count > 0) {
                ++Result;
            }
        }
        return Result;
    }

    std::int32_t TShip::GetCarriedNodeCount() {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            if (Item->ItemType == aConst::t_Protoplasm) {
                Result += Item->Weight;
            }
        }
        return Result;
    }

    // True for OrderAbsolute, AbsoluteScriptOrder, or a follow-ship order.
    std::uint8_t TShip::HasLockedOrFollowOrder() {
        return OrderAbsolute || AbsoluteScriptOrder > 0 || Order == soFollowShip;
    }

    std::uint8_t TShip::HasHullDamageOrBrokenEquippedItems() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::uint8_t Result = GetHull()->HullPoints < GetHull()->Weight;
        if (Result) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag != 0 && Item->BrokenFlag != 0) {
                Result = true;
                break;
            }
        }
        return Result;
    }

    std::uint8_t TShip::CanRefuel() {
        if (GetFuelTanks() != nullptr) {
            return GetFuelTanks()->Fuel < GetFuelTanks()->Capacity;
        }
        return false;
    }

    // Returns empty unless the player shares CurrentStar. Substitutes OtherShip for <TalkShip>.
    pas::WideString TShip_LookupVisibleTalkText(TShip* Self, const pas::WideString& Path, TShip* OtherShip) {
        pas::WideString Result{};
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar) {
            Result = aShip::TShip_LookupTalkText(Self, Path);
            aMyFunction::ReplaceTextToken(Result, u"<TalkShip>"_w, OtherShip->GetName(), u"<color=255,240,100>"_w);
            return Result;
        }
        return pas::WideString();
    }

    void TShip::UpdateBestRangerRelativeRatings() {
        if (aGalaxy::Galaxy->MaxRangerWealth != 0) {
            WealthInBestRanger = pas::real_divide(Wealth, aGalaxy::Galaxy->MaxRangerWealth);
        } else {
            WealthInBestRanger = 0.0f;
        }
        if (aGalaxy::Galaxy->BestRangerStrength != 0.0L) {
            StrengthInBestRanger = pas::real_divide(Strength, aGalaxy::Galaxy->BestRangerStrength);
        } else {
            StrengthInBestRanger = 0.0f;
        }
    }

    // Does not guard against zero AverageRangerStrength.
    void TShip::UpdateAverageRangerRelativeStrength() {
        StrengthInAverageRanger = pas::real_divide(Strength, aGalaxy::Galaxy->AverageRangerStrength);
    }

    // Updates Wealth. Includes player storage and accrued deposit but does not subtract debt; capped at MaxInt.
    std::int32_t TShip::CalculateWealth() {
        std::int32_t Result{};
        std::int32_t I{};
        aItem::TItem* Item{};
        std::uint8_t Good{};
        aPlayer::PStorageEntry Entry{};
        std::int64_t Capital = Money;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            Capital += Item->Cost;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Artefacts, I);
            Capital += Item->Cost;
        }
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            Capital += CargoGoods[Good].TotalCost;
        }
        if (aPlayer::GetPlayer() == this) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_3.next(I); ) {
                Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
                Capital += Entry->Item->Cost;
            }
            Capital += aPlayer::GetPlayer()->ComputeDepositAccruedValue();
        }
        if (Capital > SystemImports::MaxInt) {
            Result = SystemImports::MaxInt;
        } else {
            Result = Capital;
        }
        Wealth = Result;
        return Result;
    }

    double TShip::CalculateAttackStrength() {
        std::int32_t I{};
        double Result = 1.0E-7;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (aShip::TShip_IsEquipmentUsable(this, Weapons[I])) {
                        double cpp_arg = GetEffectiveSkillLevel(psAccuracy, false) & 0x0000007f;
                        std::int32_t cpp_left_2 = GetWeaponMaxDamage(Weapons[I]);
                        double cpp_arg_2 = (cpp_left_2 + GetWeaponMinDamage(Weapons[I])) / 2;
                        double weaponMaxDamage = GetWeaponMaxDamage(Weapons[I]);
                        pas::Extended cpp_left = aMyFunction::RemapClamped(cpp_arg, 0.0, 6.0, cpp_arg_2, weaponMaxDamage);
                        Result = Result + cpp_left * Weapons[I]->GetAttackCount();
                    }
                }
            }
        }
        if (pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
            switch (pas::checked_cast<aKling::TKling*>(this)->KlingType) {
                case aGalaxyStruct::ktBoss: Result = Result * 1.5L; break;
                case aGalaxyStruct::ktBertor: Result = Result + 4.0E+2L; break;
                case aGalaxyStruct::ktKlig: Result = Result * 0.33L; break;
            }
        }
        return pas::real_max<pas::Extended>(9.9999999999999999998E-8L, static_cast<long double>(Result) * (UsableWeaponCount + 7) * GetAttackMultiplier());
    }

    double TShip::CalculateDefenseStrength() {
        pas::Extended cpp_left = pas::real_divide(GetHull()->HullPoints, pas::real_max<float>(0.01f, GetHull()->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}))));
        double Result = pas::real_divide(cpp_left * (GetArmor() + 5), DefenseDamageFactor);
        if (pas::class_cast_if<aKling::TKling*>(this) != nullptr && static_cast<aKling::TKling*>(this)->KlingType == aGalaxyStruct::ktKlig) {
            return Result * 3.0L;
        }
        return Result;
    }

    double TShip::GetRepairStrengthFactor() {
        if (GetHullIntegrityPercent() > 50 && aShip::TShip_IsEquipmentUsable(this, GetRepairRobot())) {
            return GetRepairRobot()->TechLevel + 5;
        }
        return 5.0;
    }

    // Updates Strength; calculating the player's strength also refreshes galaxy ranger strength statistics.
    double TShip::CalculateStrength() {
        pas::Extended cpp_left_2 = CalculateAttackStrength();
        pas::Extended cpp_left = cpp_left_2 * CalculateDefenseStrength();
        double Result = cpp_left * GetRepairStrengthFactor();
        Strength = Result;
        if (aPlayer::GetPlayer() == this && aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->RefreshRangerStrengthStats();
        }
        return Result;
    }

    // Temporarily restores hull points. Leaves cached Strength and player galaxy-strength statistics at the full-hull values; byte result is not clamped.
    std::uint8_t TShip::GetFullHullRelativeStrengthPercent() {
        std::int32_t HullPoints = GetHull()->HullPoints;
        GetHull()->HullPoints = GetHull()->Weight;
        pas::Extended cpp_left = CalculateStrength() * 1.0E+2L;
        std::uint8_t Result = System::Round(pas::real_divide(cpp_left, aGalaxy::Galaxy->AverageRangerStrength));
        GetHull()->HullPoints = HullPoints;
        return Result;
    }

    // Uses cached Wealth and the configured scale table; no index validation.
    std::int32_t TShip::GetWealthScaledAmount(std::uint8_t ScaleIndex) {
        float Value = System::Round(static_cast<long double>(Wealth) * aConst::WealthDemandScales[ScaleIndex]);
        if (Value < 5.0E+3L) {
            return System::Round(Value);
        }
        return System::Round((Value - 5.0E+3L) * 0.3L + 5.0E+3L);
    }

    // For the player, checks the script-binding list; for NPC ships, checks ScriptShip.
    std::uint8_t TShip::HasScriptBindings() {
        if (pas::class_cast_if<aPlayer::TPlayer*>(this) != nullptr) {
            return pas::list_count(reinterpret_cast<aPlayer::TPlayer*>(this)->ScriptShipBindings) > 0;
        }
        return ScriptShip != nullptr;
    }

    // Leaves the result storage unchanged when Graphic is not a supported graphic class.
    pas::WideString TShip::GetShipPortraitImagePath() {
        pas::WideString Result{};
        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Graphic) != nullptr) {
            return GR_Main::GameDataConfig->GetParamByPathOrMarker(pas::concat_wide({u"SE.", Graphic->GraphKey, u".2ImageP"}));
        } else if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Graphic) != nullptr) {
            return pas::checked_cast<SE_Ruins::TRuinsSE*>(Graphic)->StaticImagePath;
        } else {
            return Result;
        }
    }

    // May assign PortraitFaceId lazily. Returns a resource base without the GI prefix or animation suffix.
    pas::WideString TShip_GetCaptainPortraitResourceBase(TShip* Self) {
        pas::WideString Result{};
        std::int32_t PlanetIndex{};
        std::int32_t FaceIndex{};
        std::int32_t ShipIndex{};
        std::int32_t I{};
        std::int32_t Limit{};
        EC_BlockPar::TBlockParEC* Styles{};
        pas::WideString Role{};
        std::int32_t CandidateCount{};
        aGalaxy::TStar* Star{};
        TShip* Ship{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t Owner{};
        pas::Array<std::int32_t, 0, 100> Faces{};
        pas::Array<std::int32_t, 0, 100> Usage{};
        if (aPlayer::GetPlayer() == Self) {
            if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->SpecialSimulationMode != 0) {
                return u"Bm.Captain.2Tranclucator"_w;
            }
            return pas::concat_wide({u"Bm.Captain.2", aConst::OwnerInfo[aConst::RaceToOwner(Self->PilotRace) & 0x0000007f].InternalName, pas::wide_int_to_str(Self->PortraitFaceId)});
        }
        if (Self->HasScriptStateText() && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.Captain.2", reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText, pas::wide_int_to_str(Self->PortraitFaceId), u"i"})) && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({u"Bm.Captain.2", reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText, pas::wide_int_to_str(Self->PortraitFaceId), u"a"}))) {
            return pas::concat_wide({u"Bm.Captain.2", reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText, pas::wide_int_to_str(Self->PortraitFaceId)});
        }
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) {
            if (Self->PortraitFaceId >= 0 && GR_Main::CacheDataRoot->FileExistsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Bm.Captain.2Tranclucator", SysUtils::IntToStr(Self->PortraitFaceId), "i"}))) && GR_Main::CacheDataRoot->FileExistsByPath(static_cast<pas::WideString>(pas::concat_ansi({"Bm.Captain.2Tranclucator", SysUtils::IntToStr(Self->PortraitFaceId), "a"})))) {
                return static_cast<pas::WideString>(pas::concat_ansi({"Bm.Captain.2Tranclucator", SysUtils::IntToStr(Self->PortraitFaceId)}));
            }
            return u"Bm.Captain.2Tranclucator"_w;
        }
        if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr) {
            if (pas::checked_cast<aKling::TKling*>(Self)->KlingType == aGalaxyStruct::ktBoss) {
                if (pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsBlazer) {
                    return u"Bm.Captain.2BlazerB"_w;
                } else if (pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsKeller) {
                    return u"Bm.Captain.2KellerB"_w;
                } else if (pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsTerron) {
                    return u"Bm.Captain.2TerronB"_w;
                } else {
                    return Result;
                }
            } else if (pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsBlazer) {
                return u"Bm.Captain.2BlazerN"_w;
            } else if (pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsKeller) {
                return u"Bm.Captain.2KellerN"_w;
            } else if (pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsTerron) {
                return u"Bm.Captain.2TerronN"_w;
            } else {
                return Result;
            }
        }
        Owner = aConst::RaceToOwner(Self->PilotRace);
        if (Self->PortraitFaceId < 0) {
            if (Self->TypeId == aGalaxyStruct::stRanger) {
                Role = u"R"_w;
            } else if (Self->TypeId == aGalaxyStruct::stWarrior) {
                Role = u"W"_w;
            } else if (Self->TypeId == aGalaxyStruct::stPirate) {
                Role = u"P"_w;
            } else if (Self->TypeId == aGalaxyStruct::stTransport) {
                if (pas::checked_cast<aTransport::TTransport*>(Self)->TransportType == aTransport::ttTransport) {
                    Role = u"T"_w;
                } else if (pas::checked_cast<aTransport::TTransport*>(Self)->TransportType == aTransport::ttLiner) {
                    Role = u"L"_w;
                } else if (pas::checked_cast<aTransport::TTransport*>(Self)->TransportType == aTransport::ttDiplomat) {
                    Role = u"D"_w;
                } else {
                    Role = u"L"_w;
                }
            } else {
                Role = u"L"_w;
            }
            CandidateCount = 0;
            Styles = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"StyleFace", aConst::OwnerInfo[Owner].InternalName}));
            Limit = std::min<std::int32_t>(100, Styles->GetParamCount());
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Limit - 1); cpp_range.next(I); ) {
                if (EC_Str::FindTextOffsetW(Styles->GetParamValue(I), Role, 0) >= 0) {
                    Faces[CandidateCount] = EC_Str::ExtractDigitsToIntW(Styles->GetParamName(I));
                    Usage[CandidateCount] = 0;
                    if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->PortraitFaceId != Faces[CandidateCount] || aPlayer::GetPlayer()->PilotRace != Self->PilotRace) {
                        ++CandidateCount;
                    }
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(ShipIndex); ) {
                    Ship = pas::list_at<TShip>(Star->Ships, ShipIndex);
                    if (Ship->PilotRace != Self->PilotRace || Ship->TypeId == aGalaxyStruct::stWarrior) {
                        continue;
                    }
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_4.next(FaceIndex); ) {
                        if (Ship->PortraitFaceId == Faces[FaceIndex]) {
                            ++Usage[FaceIndex];
                            break;
                        }
                    }
                }
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_5.next(PlanetIndex); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, PlanetIndex);
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_6.next(ShipIndex); ) {
                        Ship = pas::list_at<TShip>(Planet->Warriors, ShipIndex);
                        if (Ship->PilotRace != Self->PilotRace) {
                            continue;
                        }
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_7.next(FaceIndex); ) {
                            if (Ship->PortraitFaceId == Faces[FaceIndex]) {
                                ++Usage[FaceIndex];
                                break;
                            }
                        }
                    }
                }
            }
            Self->PortraitFaceId = Faces[0];
            ShipIndex = Usage[0];
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, CandidateCount - 1); cpp_range_8.next(I); ) {
                if (Usage[I] < ShipIndex) {
                    Self->PortraitFaceId = Faces[I];
                    ShipIndex = Usage[I];
                }
            }
        }
        return pas::concat_wide({u"Bm.Captain.2", aConst::OwnerInfo[Owner].InternalName, pas::wide_int_to_str(Self->PortraitFaceId)});
    }

    // Reads the player rather than Self; requires a player.
    std::uint8_t TShip::HasPlayerChameleonCharges() {
        std::uint8_t I{};
        std::uint8_t Result = false;
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(2); ++I) {
            if (aPlayer::GetPlayer()->ChameleonCharges[I] > 0) {
                return true;
            }
        }
        return Result;
    }

    std::uint8_t TShip::SelectChameleonVisualType() {
        std::int32_t AverageSize{};
        double Distance{};
        std::uint8_t Kind{};
        std::int32_t HullSize = System::Round(pas::real_divide(GetHull()->Weight, aConst::HullCapacityScale));
        double BestDistance = -1.0;
        std::uint8_t BestKind = aGalaxyStruct::ktKlig;
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
            if (Kind != aGalaxyStruct::ktBoss) {
                AverageSize = (aConst::DominatorShipDefinitions[Kind].MinimumHullSize + aConst::DominatorShipDefinitions[Kind].MaximumHullSize) / 2;
                Distance = pas::real_divide(pas::abs(AverageSize - HullSize), std::min<std::int32_t>(1, AverageSize));
                if (Distance < BestDistance || BestDistance < 0.0L) {
                    BestDistance = Distance;
                    BestKind = Kind;
                }
            }
        }
        return BestKind;
    }

    // Requires a player; action-17 script handlers can override the default result.
    std::uint8_t TShip::IsPlayerChameleonEffectiveAgainstSelf() {
        if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->ChameleonActive ^ 1) || aPlayer::GetPlayer() == this) {
            return false;
        }
        std::uint8_t Result = TypeId != aGalaxyStruct::stPirate && aPlayer::GetPlayer() != PartnerShip && !(ScriptShip != nullptr && reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->Script->ScriptFileName == u"Script.PC_fem_rangers" && reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->GetGroup()->Name == u"GroupFem");
        Result = aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnChameleonConfusion, this, nullptr, Result) != 0;
        return ScriptItemsAct(aConst::satOnChameleonConfusion, nullptr, nullptr, Result) != 0;
    }

    // ScriptChameleon preserves the assigned graphic while updating GraphDominator.
    void TShip_RefreshGraphic(TShip* Self) {
        std::uint8_t Dominator = !(pas::class_cast_if<aPlayer::TPlayer*>(Self) != nullptr) && aGalaxy::Galaxy->GraphDominatorSurfacesEnabled || Self->ChameleonActive;
        if (Self->ScriptChameleon) {
            Self->GraphDominator = Dominator;
            return;
        }
        if (Self->Graphic == nullptr || Dominator != Self->GraphDominator) {
            if (Dominator) {
                Self->CreateDominatorGraphic();
            } else {
                Self->CreateNormalGraphic();
            }
        }
    }

    void TShip::CreateNormalGraphic() {
        if (Graphic != nullptr && static_cast<std::uint8_t>(GraphDominator ^ 1)) {
            return;
        }
        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
        CollisionRadius = 32.0f;
        if (aPlayer::GetPlayer() == this && aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), u"Ship.Tranclucator"_wref.get(), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
            }
            Graphic->SetAlpha(0);
        } else if (GetHull()->HullType == aGalaxyStruct::htSpecial) {
            if (IsFemaleHumanPilot() && TypeId == aGalaxyStruct::stRanger && GetHull()->HullType == aGalaxyStruct::htSpecial && GetHull()->GetSpecialKindGraph() == u"J") {
                SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.AkrinFemale.", GetHull()->GetSpecialKindGraph()}), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic_2 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_2, createSpaceObjectByName_2);
            } else {
                SE_Space::TObjectSE* createSpaceObjectByName_3 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.Akrin.", GetHull()->GetSpecialKindGraph()}), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic_3 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_3, createSpaceObjectByName_3);
            }
            Graphic->SetAlpha(0);
        } else if (!(pas::class_cast_if<aNormalShip::TNormalShip*>(this) != nullptr)) {
            if (pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
                if (pas::checked_cast<aKling::TKling*>(this)->KlingType == aGalaxyStruct::ktBoss) {
                    switch (pas::checked_cast<aKling::TKling*>(this)->DominatorSeries) {
                        case aGalaxyStruct::dsBlazer: {
                            SE_Space::TObjectSE* createSpaceObjectByName_4 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), u"Ruins.Blazer"_wref.get(), ClassesImports::Point(0, 0));
                            pas::Var<SE_Space::TObjectSE*> graphic_4 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                            SE_Space::RetainSpaceObject(graphic_4, createSpaceObjectByName_4);
                            break;
                        }
                        case aGalaxyStruct::dsKeller: {
                            SE_Space::TObjectSE* createSpaceObjectByName_5 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), u"Ruins.Keller"_wref.get(), ClassesImports::Point(0, 0));
                            pas::Var<SE_Space::TObjectSE*> graphic_5 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                            SE_Space::RetainSpaceObject(graphic_5, createSpaceObjectByName_5);
                            break;
                        }
                        case aGalaxyStruct::dsTerron: {
                            SE_Space::TObjectSE* createSpaceObjectByName_6 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), u"Ruins.Terron"_wref.get(), ClassesImports::Point(0, 0));
                            pas::Var<SE_Space::TObjectSE*> graphic_6 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                            SE_Space::RetainSpaceObject(graphic_6, createSpaceObjectByName_6);
                            break;
                        }
                    }
                    CollisionRadius = 74.0f;
                    Graphic->SetAlpha(255);
                } else {
                    {
                        SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Ship2::TShip2SE>(SE_Ship2::TShip2SE_CreateEmpty);
                        pas::Var<SE_Space::TObjectSE*> graphic_7 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                        SE_Space::RetainSpaceObject(graphic_7, cpp_arg);
                    }
                    switch (pas::checked_cast<aKling::TKling*>(this)->DominatorSeries) {
                        case aGalaxyStruct::dsBlazer: {
                            Globals::BlazerShipTemplates[pas::checked_cast<aKling::TKling*>(this)->KlingType]->CopyTo(Graphic);
                            break;
                        }
                        case aGalaxyStruct::dsKeller: {
                            Globals::KellerShipTemplates[pas::checked_cast<aKling::TKling*>(this)->KlingType]->CopyTo(Graphic);
                            break;
                        }
                        case aGalaxyStruct::dsTerron: {
                            Globals::TerronShipTemplates[pas::checked_cast<aKling::TKling*>(this)->KlingType]->CopyTo(Graphic);
                            break;
                        }
                    }
                    Graphic->SetAlpha(200);
                }
            } else if (pas::class_cast_if<aRuins::TRuins*>(this) != nullptr) {
                if (TypeNameOverrideKey != u"") {
                    SE_Space::TObjectSE* createSpaceObjectByName_7 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), pas::concat_wide({u"Ruins.", TypeNameOverrideKey}), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic_8 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                    SE_Space::RetainSpaceObject(graphic_8, createSpaceObjectByName_7);
                } else {
                    SE_Space::TObjectSE* createSpaceObjectByName_8 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), pas::concat_wide({u"Ruins.", aConst::ShipTypeNames[TypeId].Name}), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic_9 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                    SE_Space::RetainSpaceObject(graphic_9, createSpaceObjectByName_8);
                }
                CollisionRadius = 0.0f;
                Graphic->SetAlpha(255);
            } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(this) != nullptr) {
                {
                    SE_Space::TObjectSE* createSpaceObjectByName_9 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), u"Ship.Tranclucator"_wref.get(), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic_10 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                    SE_Space::RetainSpaceObject(graphic_10, createSpaceObjectByName_9);
                }
                Graphic->SetAlpha(255);
            }
        } else if (pas::class_cast_if<aPirate::TPirate*>(this) != nullptr && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && reinterpret_cast<aPirate::TPirate*>(this)->PirateType != 0) {
            {
                SE_Space::TObjectSE* cpp_arg_2 = pas::construct_call<SE_Ship2::TShip2SE>(SE_Ship2::TShip2SE_CreateEmpty);
                pas::Var<SE_Space::TObjectSE*> graphic_11 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_11, cpp_arg_2);
            }
            Globals::PirateClanShipTemplates[GetHull()->OwnerId]->CopyTo(Graphic);
            Graphic->SetAlpha(0);
        } else if (pas::class_cast_if<aWarrior::TWarrior*>(this) != nullptr && static_cast<aWarrior::TWarrior*>(this)->WarriorType == aWarrior::wtFlagship) {
            {
                SE_Space::TObjectSE* createSpaceObjectByName_10 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[OwnerId].InternalName, u".WarriorBig"}), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic_12 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_12, createSpaceObjectByName_10);
            }
            Graphic->SetAlpha(0);
        } else if (TypeId == aGalaxyStruct::stRanger && UsesVeteranHumanRangerAppearance()) {
            {
                SE_Space::TObjectSE* createSpaceObjectByName_11 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), u"Ship.People.RangerOldFag"_wref.get(), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic_13 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_13, createSpaceObjectByName_11);
            }
            Graphic->SetAlpha(0);
        } else {
            {
                SE_Space::TObjectSE* cpp_arg_3 = pas::construct_call<SE_Ship2::TShip2SE>(SE_Ship2::TShip2SE_CreateEmpty);
                pas::Var<SE_Space::TObjectSE*> graphic_14 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic_14, cpp_arg_3);
            }
            Globals::RaceShipTemplates[GetHull()->OwnerId][GetHull()->HullType]->CopyTo(Graphic);
            Graphic->SetAlpha(0);
        }
        GraphName = Graphic->GraphKey;
        Graphic->SetPosition(Position);
        Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
        GraphDominator = false;
        RefreshGraphicSize();
    }

    void TShip::CreateDominatorGraphic() {
        aGalaxyStruct::TDominatorSeries Series{};
        std::uint8_t Kind{};
        std::int32_t Divisor{};
        if (Graphic != nullptr && GraphDominator) {
            return;
        }
        if (!(pas::class_cast_if<aNormalShip::TNormalShip*>(this) != nullptr)) {
            if (Graphic == nullptr) {
                CreateNormalGraphic();
            }
            GraphDominator = true;
            return;
        }
        if (ChameleonActive) {
            ChameleonVisualType = SelectChameleonVisualType();
            Series = ChameleonSeries;
            Kind = ChameleonVisualType;
        } else {
            Divisor = 7;
            Divisor = pas::imod(pas::shr(Id, 1), Divisor);
            Kind = static_cast<std::uint8_t>(Divisor) + 1;
            Divisor = 3;
            Series = static_cast<aGalaxyStruct::TDominatorSeries>(pas::imod(Id, Divisor));
        }
        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
        {
            SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Ship2::TShip2SE>(SE_Ship2::TShip2SE_CreateEmpty);
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, cpp_arg);
        }
        switch (Series) {
            case aGalaxyStruct::dsBlazer: Globals::BlazerShipTemplates[Kind]->CopyTo(Graphic); break;
            case aGalaxyStruct::dsKeller: Globals::KellerShipTemplates[Kind]->CopyTo(Graphic); break;
            case aGalaxyStruct::dsTerron: Globals::TerronShipTemplates[Kind]->CopyTo(Graphic); break;
        }
        GraphName = Graphic->GraphKey;
        Graphic->SetPosition(Position);
        Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
        Graphic->SetAlpha(0);
        CollisionRadius = 32.0f;
        GraphDominator = true;
        RefreshGraphicSize();
    }

    // Moves Self and docked ships between star lists; does not clear InHyperspace.
    void TShip::TransferToStar(aGalaxy::TStar* Star) {
        TShip* Ship{};
        if (CurrentStar != Star) {
            TransitOriginStar = CurrentStar;
        }
        aGalaxy::TStar* PreviousStar = CurrentStar;
        std::int32_t I = pas::list_indexof(PreviousStar->Ships, reinterpret_cast<void*>(this));
        if (I >= 0) {
            pas::list_delete(PreviousStar->Ships, I);
        }
        pas::list_add(Star->Ships, reinterpret_cast<void*>(this));
        CurrentStar = Star;
        I = pas::list_count(PreviousStar->Ships) - 1;
        while (I >= 0) {
            Ship = pas::list_at<TShip>(PreviousStar->Ships, I);
            if (Ship->DockedTo == this) {
                Ship->TransferToStar(Star);
                if (aPlayer::GetPlayer() == Ship) {
                    aGalaxy::PlayerStar = Star;
                }
            }
            --I;
        }
    }

    // Includes artefacts, guaranteed drops, stored Tranclucator inventories and station shop stock; result is borrowed.
    aItem::TItem* TShip::FindCarriedItemById(std::uint32_t Id) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aRuins::TRuins* Station{};
        aItem::TItem* Result = nullptr;
        std::int32_t Count = pas::list_count(Inventory);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            if (static_cast<std::uint32_t>(Item->Id) == Id) {
                return Item;
            }
        }
        Count = pas::list_count(Artefacts);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Artefacts, I);
            if (static_cast<std::uint32_t>(Item->Id) == Id) {
                return Item;
            }
            if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr) {
                Result = static_cast<TShip*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Item)->Ship)->FindCarriedItemById(Id);
                if (Result != nullptr) {
                    return Result;
                }
            }
        }
        Count = pas::list_count(GuaranteedDeathDropItems);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Item = pas::list_at<aItem::TItem>(GuaranteedDeathDropItems, I);
            if (static_cast<std::uint32_t>(Item->Id) == Id) {
                return Item;
            }
            if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr) {
                Result = static_cast<TShip*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Item)->Ship)->FindCarriedItemById(Id);
                if (Result != nullptr) {
                    return Result;
                }
            }
        }
        if (pas::class_cast_if<aRuins::TRuins*>(this) != nullptr) {
            Station = pas::checked_cast<aRuins::TRuins*>(this);
            Count = pas::list_count(Station->EquipmentShop);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Station->EquipmentShop, I);
                if (static_cast<std::uint32_t>(Item->Id) == Id) {
                    return Item;
                }
            }
        }
        return Result;
    }

    // Can subsidize and queue an unseen NPC for relocation to a peaceful Coalition system; excludes protected quest/script/partner ships.
    void TShip_TryRelocateUnseenShip(TShip* Self) {
        aGalaxy::TStar* Candidate{};
        std::int32_t I{};
        float Distance{};
        aPlanet::TPlanet* Planet{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->ShouldKeepShipForQuests(Self)) {
            return;
        }
        if (aPlayer::GetPlayer() == Self->PartnerShip) {
            return;
        }
        if (Self->ScriptShip != nullptr) {
            return;
        }
        if (Self->AbsoluteScriptOrder != 0) {
            return;
        }
        if (Self->DestroyQueued) {
            return;
        }
        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.002L) {
            return;
        }
        if (Self->DaysSincePlayerSeen < 20) {
            return;
        }
        if (([&] {
            pas::Extended cpp_left = aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 2.5L;
            return cpp_left < aGalaxy::Galaxy->CountEligibleRangers();
        }()) && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr && static_cast<aRanger::TRanger*>(Self)->PlaceInRating > 10) {
            return;
        }
        if (aPlayer::GetPlayer() == Self) {
            return;
        }
        float BestDistance = 1.0E+20f;
        aGalaxy::TStar* Destination = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Candidate = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Candidate != Self->CurrentStar && Candidate->Status.ControlFaction == aGalaxyStruct::sfCoalition && Candidate->Status.CustomFaction == u"" && Candidate->Status.Battle == 0 && aPlayer::GetPlayer()->CurrentStar != Candidate && Candidate->ShipTypeCounts[aGalaxyStruct::stRanger] < 3) {
                Distance = aMyFunction::PointDistanceSquared(Self->CurrentStar->Position, Candidate->Position);
                if (Distance < BestDistance && (Destination == nullptr || aMyFunction::NextRandomIntRange(0, 1, Self->RandomState) == 0)) {
                    BestDistance = Distance;
                    Destination = Candidate;
                }
            }
        }
        if (Destination != nullptr) {
            Planet = nullptr;
            I = 0;
            while (I < pas::list_count(Destination->Planets)) {
                Planet = pas::list_at<aPlanet::TPlanet>(Destination->Planets, I);
                if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId)) {
                    break;
                }
                Planet = nullptr;
                ++I;
            }
            if (Planet != nullptr) {
                Self->OrderNone(false);
                if (Self->GetFuelTanks() == nullptr) {
                    Self->CreateAndEquipFuelTanks(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1, Self->OwnerId);
                }
                if (Self->GetEngine() == nullptr) {
                    Self->CreateAndEquipEngine(System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]), 1, Self->OwnerId);
                }
                Self->CurrentStar->HandleObjectLeavingStar(Self);
                Self->DockedTo = nullptr;
                Self->CurrentPlanet = Planet;
                Self->OrderTarget = Destination;
                Self->GetHull()->HullPoints = Self->GetHull()->Weight / 2;
                pas::list_add(aGalaxy::Galaxy->ShipsInTransit, reinterpret_cast<void*>(Self));
                if (aGalaxy::Galaxy->AverageRangerCapital / 2 > Self->Wealth) {
                    Self->SetMoney(Self->Money + (aGalaxy::Galaxy->AverageRangerCapital - Self->Wealth) / 2);
                    Self->CalculateWealth();
                }
            }
        }
    }

    // Copies the carrier's star and position, moving star-list membership when necessary. Does nothing without DockedTo.
    void TShip::SynchronizeDockedLocation() {
        if (DockedTo == nullptr) {
            return;
        }
        if (DockedTo->CurrentStar != CurrentStar) {
            pas::list_delete(CurrentStar->Ships, pas::list_indexof(CurrentStar->Ships, reinterpret_cast<void*>(this)));
            CurrentStar = DockedTo->CurrentStar;
            pas::list_add(DockedTo->CurrentStar->Ships, reinterpret_cast<void*>(this));
        }
        if (DockedTo->CurrentPlanet == nullptr) {
            Position = DockedTo->Position;
        } else {
            Position = DockedTo->CurrentPlanet->GetPosition();
        }
    }

    std::uint8_t TShip::InNormalSpace() {
        if (aPlayer::GetPlayer() != this || aPlayer::GetPlayer()->RuinsMode == 0) {
            return CurrentStar != nullptr && CurrentPlanet == nullptr && DockedTo == nullptr && static_cast<std::uint8_t>(InHyperspace ^ 1);
        }
        return CurrentStar != nullptr && aPlayer::GetPlayer()->RuinsSavedPlanet == nullptr && aPlayer::GetPlayer()->RuinsSavedDockedTo == nullptr;
    }

    // True without a current star, while docked/in hyperspace, or with a saved player ruins docking target.
    std::uint8_t TShip::IsOutsideStarSpace() {
        return CurrentPlanet != nullptr || DockedTo != nullptr || InHyperspace || CurrentStar == nullptr || aPlayer::GetPlayer() == this && (aPlayer::GetPlayer()->RuinsSavedPlanet != nullptr || aPlayer::GetPlayer()->RuinsSavedDockedTo != nullptr);
    }

    std::uint8_t TShip::IsOnPlanet() {
        return CurrentPlanet != nullptr;
    }

    std::uint8_t TShip::IsDockedToShip() {
        return DockedTo != nullptr;
    }

    std::uint8_t TShip::IsDocked() {
        return DockedTo != nullptr || CurrentPlanet != nullptr;
    }

    std::uint8_t TShip::HasPositiveSpeed() {
        return Speed > 0;
    }

    // Checks only ships in star space; clears the player auto-follow target when cancelling.
    void TShip::CancelInvalidTravelOrder() {
        // Caller-popped static link; ship at ParentFrame-4.
        auto Cancel = [&]() -> void {
            if (aPlayer::GetPlayer() == this) {
                aRanger::PendingPlayerFollowTarget = nullptr;
            }
            OrderNone(false);
        };
        if (IsOutsideStarSpace()) {
            return;
        }
        if (pas::class_cast_if<TShip*>(OrderTarget) != nullptr && static_cast<TShip*>(OrderTarget)->CurrentStar != CurrentStar) {
            Cancel();
        }
        if (GetEngine() == nullptr || GetFuelTanks() == nullptr) {
            Cancel();
        }
        if (CalculateSpeed() <= 0 && Order != soTeleport) {
            Cancel();
        }
    }

    void TShip::ClearPlanetQueue() {
        if (PlanetQueue != nullptr) {
            pas::free(PlanetQueue);
            PlanetQueue = nullptr;
        }
    }

    // Requires a non-nil queue ordered with current-star planets first; otherwise falls back to the first entry. Empty queue returns nil.
    aPlanet::TPlanet* TShip::SelectNearestQueuedPlanet() {
        aPlanet::TPlanet* Planet{};
        std::int32_t I{};
        double BestDistance{};
        double Distance{};
        aPlanet::TPlanet* Result = nullptr;
        if (pas::list_count(PlanetQueue) != 0) {
            Result = pas::list_at<aPlanet::TPlanet>(PlanetQueue, 0);
            if (Result->CurrentStar == CurrentStar) {
                BestDistance = aMyFunction::PointDistanceSquared(Position, Result->GetPosition());
                for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(PlanetQueue) - 1); cpp_range.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(PlanetQueue, I);
                    if (Planet->CurrentStar != CurrentStar) {
                        break;
                    }
                    Distance = aMyFunction::PointDistanceSquared(Position, Planet->GetPosition());
                    if (Distance < BestDistance) {
                        Result = Planet;
                        BestDistance = Distance;
                    }
                }
            }
        }
        return Result;
    }

    // Types 6..13 whose CanDock(Self) succeeds. Zero mask permits every standing; does not independently filter hyperspace/docking.
    TShip* TShip::FindNearestDockableStation(TStationStandingMask StandingMask) {
        std::int32_t I{};
        TShip* Ship{};
        double Distance{};
        TShip* Result = nullptr;
        double BestDistance = 0.0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
            if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && (StandingMask == pas::constant_set<TStationStandingMask>({}) || pas::contains(StandingMask, Ship->CurrentStanding)) && Ship->virtual_TShip_CanDock(this)) {
                Distance = aMyFunction::PointDistanceSquared(Position, Ship->Position);
                if (Distance < BestDistance || Result == nullptr) {
                    Result = Ship;
                    BestDistance = Distance;
                }
            }
        }
        return Result;
    }

    // Requires a nonempty planet list; returns the last planet if all are uninhabited.
    aPlanet::TPlanet* TShip::FindFirstInhabitedPlanetInStar() {
        std::int32_t I{};
        aPlanet::TPlanet* Result = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, 0);
        if (Result->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(CurrentStar->Planets) - 1); cpp_range.next(I); ) {
                Result = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                if (Result->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                    break;
                }
            }
        }
        return Result;
    }

    // Can build PlanetQueue and issue a landing or jump order. Result is borrowed and may be nil.
    aPlanet::TPlanet* TShip::NavigateToQueuedPlanet(std::uint8_t Absolute) {
        aPlanet::TPlanet* Planet{};
        if (PlanetQueue == nullptr) {
            BuildReachablePlanetQueue();
            if (PlanetQueue == nullptr) {
                return nullptr;
            }
        }
        if (pas::list_count(PlanetQueue) > 0) {
            Planet = SelectNearestQueuedPlanet();
            if (CurrentStar == Planet->CurrentStar) {
                OrderLanding(Planet, Absolute);
            } else {
                OrderJump(Planet->CurrentStar, Absolute);
            }
            return Planet;
        }
        return nullptr;
    }

    // Uses the existing PlanetQueue; false means it is nil or empty. True does not guarantee that the order was accepted.
    std::uint8_t TShip::NavigateToEscapePlanet(std::uint8_t Absolute) {
        aPlanet::TPlanet* Planet{};
        if (PlanetQueue == nullptr) {
            return false;
        }
        if (pas::list_count(PlanetQueue) > 0) {
            if (aMyFunction::NextRandomUnitFloat(RandomState) > 0.9L || GetFuelTanks()->Fuel == GetFuelTanks()->Capacity) {
                Planet = pas::list_at<aPlanet::TPlanet>(PlanetQueue, pas::list_count(PlanetQueue) - 1);
            } else {
                Planet = SelectNearestQueuedPlanet();
            }
            if (CurrentStar == Planet->CurrentStar) {
                OrderLanding(Planet, Absolute);
            } else {
                OrderJump(Planet->CurrentStar, Absolute);
            }
            return true;
        }
        return false;
    }

    // Requires a ranger PartnerShip. True reports handled travel, not necessarily a changed or accepted order.
    std::uint8_t TShip::TryMirrorPartnerTravelOrders() {
        aGalaxy::TStar* Star{};
        if (static_cast<std::uint32_t>(pas::checked_cast<aRanger::TRanger*>(PartnerShip)->PrisonTermRemaining) > 0) {
            return false;
        }
        if (PartnerShip->CurrentStar == CurrentStar) {
            if (PartnerShip->InNormalSpace()) {
                if (pas::class_cast_if<aGalaxy::TStar*>(PartnerShip->OrderTarget) != nullptr && PartnerShip->EstimateOrderTravelTurns() < 5 && (static_cast<std::uint8_t>(OrderAbsolute ^ 1) || OrderTarget == PartnerShip)) {
                    OrderJump(pas::checked_cast<aGalaxy::TStar*>(PartnerShip->OrderTarget), false);
                    return true;
                }
                if (pas::class_cast_if<aPlanet::TPlanet*>(PartnerShip->OrderTarget) != nullptr && this->virtual_TShip_CanQueueReachablePlanet(pas::checked_cast<aPlanet::TPlanet*>(PartnerShip->OrderTarget)) && PartnerShip->EstimateOrderTravelTurns() < 5 && (static_cast<std::uint8_t>(OrderAbsolute ^ 1) || OrderTarget == PartnerShip)) {
                    OrderLanding(PartnerShip->OrderTarget, CanRefuel());
                    return true;
                }
                if (pas::class_cast_if<TShip*>(PartnerShip->OrderTarget) != nullptr && PartnerShip->Order == soLand && PartnerShip->EstimateOrderTravelTurns() < 5 && (static_cast<std::uint8_t>(OrderAbsolute ^ 1) || OrderTarget == PartnerShip) && reinterpret_cast<TShip*>(PartnerShip->OrderTarget)->virtual_TShip_CanDock(this)) {
                    OrderLanding(PartnerShip->OrderTarget, CanRefuel() || OrderTarget == PartnerShip);
                    return true;
                }
                if (PartnerShip->Order == soFollowShip && PartnerShip->EstimateOrderTravelTurns() < 3 && static_cast<std::uint8_t>(OrderAbsolute ^ 1)) {
                    OrderFollowShip(pas::checked_cast<TShip*>(PartnerShip->OrderTarget), 0, false);
                    return true;
                }
                if (pas::class_cast_if<aGalaxy::TStar*>(PartnerShip->OrderTarget) != nullptr && CanRefuel()) {
                    if (pas::class_cast_if<aPirate::TPirate*>(this) != nullptr) {
                        pas::checked_cast<aPirate::TPirate*>(this)->SelectNearestReachableDestination();
                    } else if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr) {
                        aRanger::TRanger_SelectNearestReachableDestination(pas::checked_cast<aRanger::TRanger*>(this));
                    }
                    return false;
                }
                if (OrderTarget == PartnerShip) {
                    return true;
                }
                OrderFollowShip(PartnerShip, 0, false);
                return true;
            } else if (PartnerShip->CurrentPlanet != nullptr) {
                if (this->virtual_TShip_CanQueueReachablePlanet(PartnerShip->CurrentPlanet)) {
                    OrderLanding(PartnerShip->CurrentPlanet, true);
                    return true;
                }
            } else if (PartnerShip->DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(PartnerShip->DockedTo) != nullptr && pas::checked_cast<aRuins::TRuins*>(PartnerShip->DockedTo)->virtual_TShip_CanDock(this)) {
                OrderLanding(PartnerShip->DockedTo, false);
                return true;
            }
        } else {
            if (pas::class_cast_if<aGalaxy::TStar*>(PartnerShip->OrderTarget) != nullptr) {
                Star = pas::checked_cast<aGalaxy::TStar*>(PartnerShip->OrderTarget);
            } else {
                Star = nullptr;
            }
            if (Star != nullptr && (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"" || static_cast<long double>(GetFuelTanks()->Fuel / 2) >= aMyFunction::PointDistance(CurrentStar->Position, Star->Position))) {
                if (Star != CurrentStar && (static_cast<long double>(JumpRange * JumpRange) >= aMyFunction::PointDistanceSquared(CurrentStar->Position, Star->Position) || GetFuelTanks()->Fuel == GetFuelTanks()->Capacity) && (!(pas::class_cast_if<aPirate::TPirate*>(this) != nullptr) || Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"")) {
                    OrderJump(Star, true);
                    return true;
                }
            } else {
                Star = PartnerShip->CurrentStar;
                if ((static_cast<long double>(JumpRange * JumpRange) >= aMyFunction::PointDistanceSquared(CurrentStar->Position, Star->Position) || GetFuelTanks()->Fuel == GetFuelTanks()->Capacity) && (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"" || static_cast<long double>(GetFuelTanks()->Fuel / 2) >= aMyFunction::PointDistance(CurrentStar->Position, Star->Position)) && (!(pas::class_cast_if<aPirate::TPirate*>(this) != nullptr) || Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.CustomFaction == u"")) {
                    OrderJump(Star, true);
                    return true;
                }
            }
        }
        return false;
    }

    void TShip::OrderRandomFreeFlightMove() {
        aMyFunction::TPolarPoint Polar{};
        Polar.Radius = aMyFunction::NextRandomIntRange(CurrentStar->SystemRadius + 300, CurrentStar->ComputeMapDiameter(), RandomState);
        Polar.AngleDegrees = aMyFunction::NextRandomUnitFloat(RandomState) * 3.6E+2L;
        OrderMove(aMyFunction::PolarToPoint(Polar), false);
    }

    // False only when Target is jumping and its distance is at least twice Self.Speed; does not validate shared star or speed.
    std::uint8_t TShip::IsTargetStillPursuable(TShip* Target) {
        return Target->Order != soJump || static_cast<long double>(aMyFunction::PointDistance(Position, Target->Position)) < Speed * 2;
    }

    // AI estimate using range, hull and travel time. Pursuer weapon range is evaluated using Self's bonuses.
    std::uint8_t TShip::CanEscapePursuer(TShip* Pursuer) {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        std::uint8_t Result = true;
        if (EstimateOrderTravelTurns() == 1 && GetHull()->HullPoints > GetHull()->Weight * 0.5L || EstimateOrderTravelTurns() == 2 && GetHull()->HullPoints > GetHull()->Weight * 0.9L) {
            return Result;
        }
        double Distance = aMyFunction::PointDistance(Position, Pursuer->Position);
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Pursuer->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Pursuer->Weapons[I];
                    if (aShip::TShip_IsEquipmentUsable(Pursuer, Weapon) && static_cast<long double>(aShip::TShip_GetWeaponRange(this, Weapon)) > Distance) {
                        return false;
                    }
                }
            }
        }
        if (Pursuer->Speed < Speed && static_cast<long double>(2 * Pursuer->Speed) < Distance) {
            return Result;
        }
        if (static_cast<long double>(2 * Pursuer->Speed) > Distance) {
            return false;
        }
        if (static_cast<long double>(6 * Pursuer->Speed) < Distance) {
            return Result;
        }
        if (Order == soLand) {
            if (pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
                if (pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<TShip*>(OrderTarget)->Position, Position), Speed + 1) > 3.0L) {
                    return false;
                }
                return Result;
            } else if (pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->GetPosition(), Position), Speed + 1) > 3.0L) {
                return false;
            } else {
                return Result;
            }
        } else if (Pursuer->Order == soJump && pas::real_divide(aMyFunction::PointDistance(OrderDestination, Position), Speed + 1) > 6.0L) {
            return false;
        } else {
            return Result;
        }
    }

    std::uint8_t TShip::HasLandablePlanetInStar(aGalaxy::TStar* Star) {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, I);
            if (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) && this->virtual_TShip_CanQueueReachablePlanet(Planet)) {
                return true;
            }
        }
        return false;
    }

    // Minimum of raw tank Capacity and engine JumpRange; requires both items and ignores bonuses/condition.
    std::int32_t TShip::GetFullFuelBaseJumpRange() {
        return std::min<std::int32_t>(static_cast<std::int32_t>(GetFuelTanks()->Capacity), static_cast<std::int32_t>(GetEngine()->JumpRange));
    }

    // Returns a borrowed intermediate star or nil. Uses raw full-fuel range and current distance caches; excludes sector 20. A direct reachable destination returns the current star.
    aGalaxy::TStar* TShip::FindNextStarTowardDestination(aGalaxy::TStar* Destination, std::uint8_t RequireFuelMargin) {
        aGalaxy::TStar* Result{};
        static const pas::Set<0, 255> CoalitionShips = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::stRanger, aGalaxyStruct::stTranclucator}});
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        aGalaxy::TStar* Origin{};
        aGalaxy::TStar* Star{};
        std::int32_t Previous{};
        pas::List* Predecessors = pas::make_object<pas::List>();
        pas::list_add(Predecessors, nullptr);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            pas::list_add(Predecessors, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(-1))));
        }
        std::int32_t RangeSquared = GetFullFuelBaseJumpRange() * GetFullFuelBaseJumpRange();
        std::int32_t CapacitySquared = GetFuelTanks()->Capacity * GetFuelTanks()->Capacity;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 2); cpp_range_2.next(I); ) {
            Origin = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
            if (Origin->Constellation->Id == 20 || static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Predecessors, I))) == -1) {
                continue;
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(J); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Origin->StarDistances[J].Star));
                if (Star->Constellation->Id == 20) {
                    continue;
                }
                if (static_cast<long double>(RangeSquared) < aMyFunction::PointDistanceSquared(Origin->Position, Star->Position) || RequireFuelMargin && CapacitySquared < aMyFunction::PointDistanceSquared(Origin->Position, Star->Position) * 2.0L) {
                    break;
                }
                if (Star == Destination) {
                    Previous = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Predecessors, I)));
                    Result = Origin;
                    while (Previous > 1) {
                        Result = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[Previous - 1].Star));
                        Previous = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Predecessors, Previous - 1)));
                    }
                    pas::free(Predecessors);
                    return Result;
                }
                if (HasLandablePlanetInStar(Star) && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShips)) <= 20 && Star->ShipTypeCounts[aGalaxyStruct::stRanger] <= 9) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_4.next(K); ) {
                        if (pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[K].Star)) == Star) {
                            if (static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Predecessors, K))) == -1) {
                                pas::list_put(Predecessors, K, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(I + 1))));
                            }
                            break;
                        }
                    }
                }
            }
        }
        Result = nullptr;
        pas::free(Predecessors);
        return Result;
    }

    // Excludes Self but includes docked/hyperspace entries; no match returns sqrt(1000000000).
    double TShip::DistanceToNearestShipByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask) {
        std::int32_t I{};
        TShip* Ship{};
        float Distance{};
        float BestDistance = 1.0E+9f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
            if (pas::contains(ShipTypeMask, Ship->TypeId) && Ship != this) {
                Distance = aMyFunction::PointDistanceSquared(Position, Ship->Position);
                if (Distance < BestDistance) {
                    BestDistance = Distance;
                }
            }
        }
        return System::Sqrt(BestDistance);
    }

    // Uses rounded distance / (Speed + 1) + 1; unsupported orders return zero.
    std::int32_t TShip::EstimateOrderTravelTurns() {
        switch (Order) {
            case soMove: {
                return System::Round(pas::real_divide(aMyFunction::PointDistance(OrderDestination, Position), Speed + 1)) + 1;
            }
            case soLand: {
                if (pas::class_cast_if<aPlanet::TPlanet*>(OrderTarget) != nullptr) {
                    return System::Round(pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->GetPosition(), Position), Speed + 1)) + 1;
                }
                return System::Round(pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<TShip*>(OrderTarget)->Position, Position), Speed + 1)) + 1;
            }
            case soJump: {
                return System::Round(pas::real_divide(aMyFunction::PointDistance(OrderDestination, Position), Speed + 1)) + 1;
            }
            case soJumpHole: {
                return System::Round(pas::real_divide(aMyFunction::PointDistance(OrderDestination, Position), Speed + 1)) + 1;
            }
            case soFollowShip: {
                return System::Round(pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<TShip*>(OrderTarget)->Position, Position), Speed + 1)) + 1;
            }
            default: return 0;
        }
    }

    // Accepts planet, ship or star; a star estimates travel to the current system boundary, excluding hyperspace transit. Other classes return zero.
    std::int32_t TShip::EstimateTravelTurnsToObject(pas::Object* Target) {
        double Angle{};
        double Radius{};
        EC_Struct::TPointF Point{};
        if (pas::class_cast_if<aPlanet::TPlanet*>(Target) != nullptr) {
            return System::Round(pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<aPlanet::TPlanet*>(Target)->GetPosition(), Position), Speed + 1)) + 1;
        } else if (pas::class_cast_if<TShip*>(Target) != nullptr) {
            return System::Round(pas::real_divide(aMyFunction::PointDistance(pas::checked_cast<TShip*>(Target)->Position, Position), Speed + 1)) + 1;
        } else if (pas::class_cast_if<aGalaxy::TStar*>(Target) != nullptr) {
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(CurrentStar->Position, pas::checked_cast<aGalaxy::TStar*>(Target)->Position));
            Radius = pas::real_divide(CurrentStar->ComputeMapDiameter(), 2.0L);
            Point.X = System::Trunc(System::Sin(Angle) * Radius);
            Point.Y = System::Trunc(-System::Cos(Angle) * Radius);
            return System::Round(pas::real_divide(aMyFunction::PointDistance(Point, Position), Speed + 1)) + 1;
        } else {
            return 0;
        }
    }

    // Returns -1 for zero speed, nil planet/star, or a different star.
    std::int32_t TShip::EstimateTravelTurnsToPlanet(aPlanet::TPlanet* Planet) {
        EC_Struct::TPointF Point{};
        if (Speed == 0 || Planet == nullptr || Planet->CurrentStar == nullptr || Planet->CurrentStar != CurrentStar) {
            return -1;
        }
        Point = Planet->GetPosition();
        return System::Round(pas::real_divide(aMyFunction::PointDistance(pas::load_unaligned<EC_Struct::TPointF>(reinterpret_cast<EC_Struct::PPointF>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&Point)) + 0)))), Position), Speed + 1)) + 1;
    }

    // Ranger, pirate or Tranclucator death notification; does not itself check player partnership.
    void TShip_NotifyCompanionDeath(TShip* Self) {
        pas::WideString Text{};
        if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.DeadShip.Partner"_wref.get(), Self->Seed + aGalaxy::Galaxy->CurrentTurn / 11);
        } else if (pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) {
            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.DeadShip.Pirate"_wref.get(), Self->Seed + aGalaxy::Galaxy->CurrentTurn / 11);
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) {
            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.DeadShip.Tranclucator"_wref.get(), Self->Seed + aGalaxy::Galaxy->CurrentTurn / 11);
        } else {
            return;
        }
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) {
            aPlayer::GetPlayer()->RefreshStorageBubbles();
        }
        aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, Self->CurrentStar->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(-1), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, Self->GetName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<FullName>"_w, Self->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
        Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
    }

    // Source may be nil, ship or missile. HitRange=-1 selects direct-hit rules; other values select area-hit rules. Returns adjusted damage, zero for rejection, or negative damage for an impulse-shield block; not actual hull loss. May run death handling without freeing Self.
    std::int32_t TShip_ApplyDamage(TShip* Self, pas::Object* Source, std::int32_t Damage, float HitRange, std::uint32_t& DamageColor, aGalaxyStruct::TDamageFlagSet DamageFlags) {
        std::int32_t Result{};
        static const pas::Set<0, 255> ScannerDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> NoDamageFlags = pas::constant_set<pas::Set<0, 255>>({});
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t EngineLoss{};
        std::int32_t DropCount{};
        std::int32_t MinimumPriority{};
        std::int32_t MaximumPriority{};
        float Wear{};
        float DropRoll{};
        aItem::TProtoplasm* Nodes{};
        aItem::TUselessItem* Debris{};
        aItem::TItem* Item{};
        // Native initializes/finalizes this otherwise unused managed local.
        pas::WideString UnusedText{};
        aItem::TMicroModule* Module{};
        aItem::TCistern* Cistern{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aEFilm::TEFilmObj* Effect{};
        SE_Space::TObjectSE* SceneObject{};
        TShip* Attacker = nullptr;
        if (Source != nullptr) {
            if (pas::class_cast_if<TShip*>(Source) != nullptr) {
                Attacker = reinterpret_cast<TShip*>(Source);
            } else if (pas::class_cast_if<aMissile::TMissile*>(Source) != nullptr) {
                Attacker = reinterpret_cast<aMissile::TMissile*>(Source)->OwnerShip;
            }
        }
        if (HitRange == -1.0L && Attacker != nullptr) {
            Self->ReactToAttack(Attacker);
        }
        DamageColor = 0u;
        if (Self->TypeId == aGalaxyStruct::stKling && Attacker != nullptr && Attacker->TypeId == aGalaxyStruct::stKling && ([&] {
            aGalaxyStruct::TDominatorSeries cpp_left = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
            return cpp_left == pas::checked_cast<aKling::TKling*>(Attacker)->DominatorSeries;
        }()) && static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Attacker)->IsProgramActive(aGalaxyStruct::prgInsanity) ^ 1) && Self->HasIndependentScriptFaction() == Attacker->HasIndependentScriptFaction() && (static_cast<std::uint8_t>(Self->HasIndependentScriptFaction() ^ 1) || reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText == reinterpret_cast<aScript::TScriptShip*>(Attacker->ScriptShip)->StateText)) {
            return 0;
        }
        if (HitRange != -1.0L && (pas::in_set<0, 4, 7, 7>(Self->OwnerId) || Self->TypeId == aGalaxyStruct::stTranclucator) && Attacker != nullptr && aShip::TShip_GetRelationLevelToShip(Self, Attacker) > aGalaxyStruct::rlHostile) {
            return 0;
        }
        if (HitRange != -1.0L && Self->TypeId == aGalaxyStruct::stKling && aPlayer::GetPlayer() == Attacker && pas::checked_cast<aKling::TKling*>(Self)->IsPlayerCamouflageEffective(Attacker)) {
            return 0;
        }
        if (HitRange != -1.0L && Attacker != nullptr && Attacker->TypeId == aGalaxyStruct::stKling && aPlayer::GetPlayer() == Self && pas::checked_cast<aKling::TKling*>(Attacker)->IsPlayerCamouflageEffective(Self)) {
            return 0;
        }
        if (Attacker != nullptr && Self != Attacker && (!(pas::class_cast_if<aMissile::TMissile*>(Source) != nullptr) || HitRange == -1.0L || aShip::TShip_GetRelationLevelToShip(Self, Attacker) <= aGalaxyStruct::rlHostile)) {
            if (aPlayer::GetPlayer() == Self && Attacker->ScriptShip != nullptr) {
                reinterpret_cast<aScript::TScriptShip*>(Attacker->ScriptShip)->HitPlayer = true;
            }
            if (Self->ScriptShip != nullptr) {
                if (aPlayer::GetPlayer() == Attacker || aPlayer::GetPlayer() == Attacker->PartnerShip || pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker) != nullptr && static_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip == aPlayer::GetPlayer()) {
                    reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->Hit = true;
                }
            }
        }
        float DamageValue = Damage;
        if (HitRange == -1.0L && Attacker != nullptr && pas::class_cast_if<aKling::TKling*>(Attacker) != nullptr && ([&] {
            std::int32_t cpp_left_2 = Attacker->GetScannerPower();
            return cpp_left_2 > (Self->GetDefensePercent() & 0x0000007f);
        }()) && static_cast<long double>(([&] {
            std::int32_t cpp_left_3 = Attacker->GetRadarRange();
            return cpp_left_3 * Attacker->GetRadarRange();
        }())) >= aMyFunction::PointDistanceSquared(Self->Position, Attacker->Position)) {
            std::int32_t cpp_left_4 = Attacker->GetScannerPower();
            DamageValue = (1.0L + (cpp_left_4 - (Self->GetDefensePercent() & 0x0000007f)) * 0.01L) * DamageValue;
        }
        std::uint8_t ScannerEffects = std::bit_cast<aGalaxyStruct::TDamageFlagSet>(pas::load_unaligned<std::uint32_t>(&DamageFlags) + 0) * static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerDamageFlags) != static_cast<aGalaxyStruct::TDamageFlagSet>(NoDamageFlags) && HitRange == -1.0L && Attacker != nullptr && aShip::TShip_IsEquipmentUsable(Attacker, Attacker->GetScanner()) && ([&] {
            std::int32_t cpp_left_5 = Attacker->GetScannerPower();
            return cpp_left_5 >= (Self->GetDefensePercent() & 0x0000007f);
        }()) && static_cast<long double>(([&] {
            std::int32_t cpp_left_6 = Attacker->GetRadarRange();
            return cpp_left_6 * Attacker->GetRadarRange();
        }())) >= aMyFunction::PointDistanceSquared(Self->Position, Attacker->Position);
        if (ScannerEffects) {
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkScanBonus))) != 0) {
                DamageValue = DamageValue * 1.15L;
            }
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkBonusToDamaged))) != 0) {
                DamageValue = (1.0L + (1.0L - pas::real_divide(Self->GetHull()->HullPoints, Self->GetHull()->Weight)) * 0.33L) * DamageValue;
            }
        }
        if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkEnergy))) != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtEnergyDef)); cpp_range.next(J); ) {
                pas::Extended cpp_right = 0.3L * ((Self->CanBoostArtefact(aConst::t_ArtEnergyDef, nullptr, false) & 127 & 0x0000007f) + 1);
                if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < cpp_right) {
                    DamageValue = 0.0f;
                    break;
                }
            }
        }
        if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkMissile))) != 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtMissileDef)); cpp_range_2.next(J); ) {
                pas::Extended cpp_right_2 = (Self->CanBoostArtefact(aConst::t_ArtMissileDef, nullptr, false) & 127 & 0x0000007f) + 1;
                DamageValue = pas::real_divide(DamageValue, 1.0L + aMyFunction::NextRandomFloatRange(0.1, 0.4, Self->RandomState) * cpp_right_2);
            }
        }
        if (Attacker != nullptr && aPlayer::GetPlayer() == Attacker) {
            if (Attacker->IsHealthEffectActive(10) || Attacker->IsHealthEffectActive(7) || Attacker->IsHealthEffectActive(8)) {
                DamageValue = static_cast<long double>(DamageValue) * aMyFunction::NextRandomUnitFloat(Self->RandomState);
            }
            if (Self->TypeId == aGalaxyStruct::stKling) {
                pas::checked_cast<aKling::TKling*>(Self)->DetectAttackingPlayer(Attacker);
            }
        }
        if (DamageValue < 1.0L) {
            if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.5L) {
                DamageValue = 1.0f;
            } else {
                DamageValue = 2.0f;
            }
        }
        if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr && DamageValue > 0.0L && static_cast<aKling::TKling*>(Self)->DominatorSeries == aGalaxyStruct::dsKeller && static_cast<aKling::TKling*>(Self)->HasNearbyBertorAura()) {
            if (Self->CurrentStar->RecordingTurnFilm && static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Self)->AuraEffectShownThisTurn ^ 1)) {
                SceneObject = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.AuraEffect"_wref.get(), ClassesImports::Point(0, 0), 1, -1);
                Effect = Globals::PrimaryFilm->AddObject(0u, SceneObject, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(Self->CurrentStar->CurrentStepIndex, Effect, Self->FilmObject, Self->FilmObject);
                Globals::PrimaryFilm->SetWeaponHit(Self->CurrentStar->CurrentStepIndex, Effect, 0, 0, false, true);
                Globals::PrimaryFilm->AttachObject(Self->CurrentStar->CurrentStepIndex, Effect);
                pas::checked_cast<aKling::TKling*>(Self)->AuraEffectShownThisTurn = true;
            }
            DamageValue = DamageValue * 0.8L;
        }
        DamageValue = static_cast<long double>(DamageValue) * Self->GetHull()->GetFragilityFactor(DamageFlags);
        float StatusStrength = Self->GetCombatStatusStrength(cseBWBuff);
        if (StatusStrength > 0.01L) {
            DamageValue = pas::real_divide(DamageValue, 1.0L + StatusStrength * 0.01L);
        }
        if (Attacker != nullptr) {
            StatusStrength = Attacker->GetCombatStatusStrength(cseBWBuff);
        } else {
            StatusStrength = 0.0f;
        }
        if (StatusStrength > 0.01L) {
            DamageValue = DamageValue * (1.0L + StatusStrength * 0.01L);
        }
        if (Self->TypeId == aGalaxyStruct::stKling) {
            if (pas::checked_cast<aKling::TKling*>(Self)->KlingType == aGalaxyStruct::ktBoss) {
                if (aPlayer::GetPlayer()->CurrentStar != Self->CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
                    if (Self->DaysSincePlayerSeen > 20) {
                        DamageValue = DamageValue * 0.5L;
                    }
                    if (System::Round(DamageValue) >= Self->GetHull()->HullPoints) {
                        DamageValue = Self->GetHull()->HullPoints - 1;
                    }
                }
                if (System::Round(DamageValue) >= Self->GetHull()->HullPoints && aKling::KellerShip == Self) {
                    DamageValue = Self->GetHull()->HullPoints - 1;
                }
            }
        } else if (Attacker != nullptr && Attacker->TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(Attacker)->KlingType == aGalaxyStruct::ktBoss) {
            DamageValue = DamageValue * 2.0L * aGalaxy::Galaxy->InterpolateDifficulty(-1, 0.7f, 1.0f, 1.2f, 1.5f);
        }
        std::int32_t AdjustedDamage = System::Round(DamageValue);
        if (aPlayer::GetPlayer() == Self && (aGalaxy::Galaxy->GodModEnabled == 1 || aGalaxy::Galaxy->SpecialSimulationMode != 0)) {
            AdjustedDamage = 0;
        }
        if (Self->Order == soTeleport && Self->OrderTarget != nullptr && Self->OrderTarget != Self->CurrentStar) {
            AdjustedDamage = 0;
        }
        if (Self->GetHull()->ImpulseShieldsEnabled && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkUndefendable))) == 0 && ([&] {
            pas::Extended cpp_left_7 = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
            return cpp_left_7 > Self->GetDefenseDamageFactor() * 1.0E+2L;
        }())) {
            Result = -AdjustedDamage;
            DamageColor = 0u;
            return Result;
        }
        if (Attacker != nullptr) {
            AdjustedDamage = Attacker->ScriptItemsAct(aConst::satOnDealingDamage, Self, nullptr, AdjustedDamage);
        }
        AdjustedDamage = Self->ScriptItemsAct(DamageScriptActionTypes[static_cast<std::int32_t>(aConst::ClassifyWeaponDamageFlags(pas::load_unaligned<std::uint32_t>(&DamageFlags))) & 0x0000007f], Source, nullptr, AdjustedDamage);
        if (AdjustedDamage <= 0) {
            Result = 0;
            DamageColor = 0u;
            return Result;
        }
        std::int32_t ExpectedHullPoints = 0;
        if (Self->GetHull()->HullPoints - AdjustedDamage <= 0 && aKling::KellerShip != Self && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkNonLethal))) == 0) {
            if (Attacker != nullptr) {
                Attacker->ScriptItemsAct(aConst::satOnDealingFatalDamage, Self, nullptr, 0);
                if (pas::class_cast_if<aNormalShip::TNormalShip*>(Attacker) != nullptr) {
                    aNormalShip::TNormalShip_ProcessShipKill(pas::checked_cast<aNormalShip::TNormalShip*>(Attacker), Self);
                }
                if (Attacker->TypeId == aGalaxyStruct::stRanger) {
                    aRanger::TRanger_ApplyAttackReputationChanges(pas::checked_cast<aRanger::TRanger*>(Attacker), Self, 1.0);
                }
                if (Attacker->PartnerShip != nullptr && Attacker->PartnerShip->TypeId == aGalaxyStruct::stRanger) {
                    aRanger::TRanger_ApplyAttackReputationChanges(pas::checked_cast<aRanger::TRanger*>(Attacker->PartnerShip), Self, 0.5);
                }
                if (pas::class_cast_if<aTranclucator::TTranclucator*>(Attacker) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip->TypeId == aGalaxyStruct::stRanger) {
                    aRanger::TRanger_ApplyAttackReputationChanges(pas::checked_cast<aRanger::TRanger*>(pas::checked_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip), Self, 1.0);
                    if (pas::checked_cast<aTranclucator::TTranclucator*>(Attacker)->OwnerShip == aPlayer::GetPlayer()) {
                        Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerTranclucatorKillsShip"_w, nullptr);
                        Event->AddData(Self->TypeId);
                        Event->AddData(Self->CurrentStar->Id);
                        Event->AddData(Self->Id);
                        Event->AddData(Self->OwnerId);
                        Event->AddTextData(Self->GetName());
                        Event->AddData(Attacker->Id);
                        Event->AddData(Attacker->OwnerId);
                        Event->AddTextData(Attacker->GetName());
                        Event->AddData(Self->GetFullHullRelativeStrengthPercent());
                        Event->AddTextData(Self->GetFullName(u" "_wref.get()));
                        Event->AddTextData(Self->TypeNameOverrideKey);
                        if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr) {
                            Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Self)->KlingType));
                        } else if (pas::class_cast_if<aTransport::TTransport*>(Self) != nullptr) {
                            Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aTransport::TTransport*>(Self)->TransportType));
                        } else if (pas::class_cast_if<aWarrior::TWarrior*>(Self) != nullptr) {
                            Event->AddData(pas::checked_cast<aWarrior::TWarrior*>(Self)->WarriorType);
                        } else if (pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) {
                            Event->AddData(pas::checked_cast<aPirate::TPirate*>(Self)->PirateType);
                        } else {
                            Event->AddData(0);
                        }
                    }
                }
            }
            if (Source == nullptr) {
                Self->ScriptItemsAct(aConst::satOnDeath, nullptr, nullptr, 0);
            } else if (pas::class_cast_if<aMissile::TMissile*>(Source) != nullptr) {
                Self->ScriptItemsAct(aConst::satOnDeath, reinterpret_cast<aMissile::TMissile*>(Source)->OwnerShip, Source, 0);
            } else if (pas::class_cast_if<TShip*>(Source) != nullptr) {
                Self->ScriptItemsAct(aConst::satOnDeath, Source, nullptr, 0);
            } else {
                Self->ScriptItemsAct(aConst::satOnDeath, nullptr, nullptr, 0);
            }
            Self->DropGuaranteedDeathDropItems();
            Self->GetHull()->HullPoints = 0;
            if (aPlayer::GetPlayer() == Self->PartnerShip || pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && static_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip == aPlayer::GetPlayer()) {
                aShip::TShip_NotifyCompanionDeath(Self);
            }
            Self->CurrentStar->UpdateControlFaction();
            aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), Self);
            if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.5L && static_cast<std::uint8_t>(Self->HasScriptStateText() ^ 1)) {
                Cistern = pas::construct_call<aItem::TCistern>(aItem::TEquipment_Create);
                I = aMyFunction::NextRandomIntRange(10, System::Round(aMyFunction::RemapClamped(Self->GetHull()->Weight, 2.0E+2, 2.0E+3, 1.0E+1, 1.5E+2)), Self->RandomState);
                {
                    std::uint8_t ownerId = Self->OwnerId;
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(1, I, Self->RandomState);
                    std::uint8_t roundAndTruncateToFives = aMyFunction::RoundAndTruncateToFives(I);
                    Cistern->Init(nextRandomIntRange, roundAndTruncateToFives, ownerId);
                }
                if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                    Cistern->DominatorSeries = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
                }
                pas::list_add(Self->Inventory, reinterpret_cast<void*>(Cistern));
                Self->DropCarriedItemAsMovingLoot(Cistern);
            }
            if (pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr) {
                Self->JettisonCargoGoodsTowardTargetValue(aGalaxy::Galaxy->ComputeScaledHugeMoney(2));
            } else if (Self->CurrentStar->Items == nullptr || pas::list_count(Self->CurrentStar->Items) < 20 || Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) || Self->HasIndependentScriptFaction()) {
                DropRoll = aMyFunction::NextRandomUnitFloat(Self->RandomState);
                if (DropRoll < 0.1L && (!(pas::class_cast_if<aKling::TKling*>(Self) != nullptr) || Attacker == nullptr || !(pas::class_cast_if<aKling::TKling*>(Attacker) != nullptr) || DropRoll < pas::real_divide(0.1L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].QuestTimeAndExperienceFactor))) {
                    Self->DropRandomValuableItemsOnDestruction(1);
                } else {
                    if (DropRoll < 0.85L) {
                        DropCount = 1;
                    } else {
                        DropCount = 2;
                    }
                    if (Attacker != nullptr && Attacker->CountActiveArtefacts(aConst::t_ArtefactMiniExpl) > 0) {
                        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.6L) {
                            ++DropCount;
                        } else if (([&] {
                            pas::Extended cpp_right_4 = (Attacker->CanBoostArtefact(aConst::t_ArtefactMiniExpl, nullptr, false) & 127 & 0x0000007f) + 1;
                            pas::Extended cpp_right_3 = 0.8L - Attacker->CountActiveArtefacts(aConst::t_ArtefactMiniExpl) * 0.2L * cpp_right_4;
                            return aMyFunction::NextRandomUnitFloat(Self->RandomState) > cpp_right_3;
                        }()) || ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkMoreDrop))) != 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.9L) {
                            Self->DropRandomValuableItemsOnDestruction(1);
                        }
                    }
                    if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkMoreDrop))) != 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.6L) {
                        ++DropCount;
                    }
                    Self->DropRandomCheapItemsOnDestruction(DropCount);
                }
                if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && Self->TypeId == aGalaxyStruct::stPirate && (Self->RandomState + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 31 == 0) {
                    Module = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                    MinimumPriority = System::Round(aMyFunction::RemapClamped(Self->WealthInBestRanger, 0.5, 2.0, 1.0E+1, 0.0));
                    MinimumPriority += System::Round(aMyFunction::RemapClamped(aPlayer::GetPlayer()->PlaceInRating, 1.0, pas::list_count(aGalaxy::Galaxy->Rangers), 0.0, 1.0E+1));
                    MinimumPriority += System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(aPlayer::GetPlayer()->PirateRank + static_cast<std::uint8_t>(0)), 0.0, 7.0, 1.0E+1, 0.0));
                    MinimumPriority += aGalaxy::Galaxy->ScaleIntByTechLevel(20, 0);
                    MinimumPriority += aMyFunction::SeededRandomIntRange(-10, 10, Self->Id + System::Trunc(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed)));
                    MinimumPriority = std::max<std::int32_t>(1, std::min<std::int32_t>(MinimumPriority, 100));
                    MaximumPriority = 100;
                    Module->Init(aGalaxy::TGalaxy::SelectMicroModule(MinimumPriority, MaximumPriority, Self->Id + System::Trunc(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed)), Self));
                    Module->DominatorSeries = aGalaxyStruct::dsBlazer;
                    Module->OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
                    pas::list_add(Self->Inventory, reinterpret_cast<void*>(Module));
                    Self->DropCarriedItemAsMovingLoot(Module);
                }
                if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                    if ((Self->RandomState + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 2 == 0 || pas::checked_cast<aKling::TKling*>(Self)->KlingType == aGalaxyStruct::ktBoss) {
                        Nodes = pas::construct_call<aItem::TProtoplasm>(aItem::TEquipment_Create);
                        Nodes->Init_2(Self->NodeReserve + 1, 1);
                        Nodes->DominatorSeries = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
                        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Nodes));
                        Self->DropCarriedItemAsMovingLoot(Nodes);
                    } else {
                        Debris = pas::construct_call<aItem::TUselessItem>(aItem::TUselessItem_Create);
                        Debris->Init(u"Remains"_w, pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries, Self->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10), false);
                        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Debris));
                        Self->DropCarriedItemAsMovingLoot(Debris);
                    }
                    if ((Self->RandomState + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 47 == 0) {
                        Module = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                        MinimumPriority = System::Round(aMyFunction::RemapClamped(Self->WealthInBestRanger, 0.5, 2.0, 1.0E+1, 0.0));
                        MinimumPriority += System::Round(aMyFunction::RemapClamped(aPlayer::GetPlayer()->PlaceInRating, 1.0, pas::list_count(aGalaxy::Galaxy->Rangers), 0.0, 1.0E+1));
                        MinimumPriority += System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(aPlayer::GetPlayer()->Rank + static_cast<std::uint8_t>(0)), 0.0, 7.0, 1.0E+1, 0.0));
                        MinimumPriority += aGalaxy::Galaxy->ScaleIntByTechLevel(20, 0);
                        MinimumPriority += aMyFunction::SeededRandomIntRange(-10, 10, Self->Id + System::Trunc(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed)));
                        MinimumPriority = std::max<std::int32_t>(1, std::min<std::int32_t>(MinimumPriority, 100));
                        MaximumPriority = 100;
                        Module->Init(aGalaxy::TGalaxy::SelectMicroModule(MinimumPriority, MaximumPriority, Self->Id + System::Trunc(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed)), Self));
                        Module->DominatorSeries = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
                        Module->OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiDominator);
                        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Module));
                        Self->DropCarriedItemAsMovingLoot(Module);
                    }
                } else if (pas::in_range(Self->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate)) {
                    Self->JettisonCargoGoodsTowardTargetValue(aGalaxy::Galaxy->ComputeScaledAverageMoney(Self->OwnerId));
                }
            }
            if (aPlayer::GetPlayer() == Attacker) {
                aShip::TShip_TryDropTreasureMap(Self);
            }
            if (pas::list_count(Self->Artefacts) > 0) {
                Self->DropAllArtefactsOnDestruction();
            }
            {
                const std::int32_t cpp_first = pas::list_count(Self->Inventory) - 1;
                if (cpp_first >= 1) {
                    for (I = cpp_first; I >= 1; --I) {
                        Item = pas::list_at<aItem::TItem>(Self->Inventory, I);
                        if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && Item->DestroyFlag <= 0) {
                            Self->DropCarriedItemAsMovingLoot(Item);
                        }
                    }
                }
            }
        } else {
            ExpectedHullPoints = Self->GetHull()->HullPoints;
            Self->GetHull()->HullPoints -= AdjustedDamage;
            if (Self->GetHull()->HullPoints != ExpectedHullPoints - AdjustedDamage) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
            if ((aKling::KellerShip == Self || (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkNonLethal))) != 0) && Self->GetHull()->HullPoints < 1) {
                Self->GetHull()->HullPoints = 1;
            }
            ExpectedHullPoints = Self->GetHull()->HullPoints;
            if (AdjustedDamage == 1 && aPlayer::GetPlayer() == Attacker && Attacker != nullptr) {
                ++Self->PlayerScratchHitsReceived;
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckScratchDamageAchievement(Self->PlayerScratchHitsReceived);
            }
            EngineLoss = aMyFunction::NextRandomIntRange(0, 2, Self->RandomState);
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDecelerate))) != 0) {
                EngineLoss += aMyFunction::NextRandomIntRange(5, 15, Self->RandomState);
            }
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDecelerateA))) != 0) {
                EngineLoss += aMyFunction::NextRandomIntRange(5, 15, Self->RandomState);
            }
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDecelerateAEx))) != 0) {
                EngineLoss += aMyFunction::NextRandomIntRange(5, 15, Self->RandomState);
            }
            if (EngineLoss > 0 && Self->GetEngine() != nullptr) {
                if (Self->GetEngine()->OutputPercent > EngineLoss) {
                    Self->GetEngine()->OutputPercent -= EngineLoss;
                } else {
                    Self->GetEngine()->OutputPercent = 0;
                }
            }
            if (Self->TypeId != aGalaxyStruct::stTranclucator) {
                if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
                    Wear = aMyFunction::RemapClamped(AdjustedDamage, 1.0, Self->GetHull()->Weight * 0.1L, 0.05, 0.15);
                    if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                        if (aPlayer::GetPlayer() == Self) {
                            Wear = Wear;
                        } else {
                            Wear = Wear * 0.8L;
                        }
                    } else if (pas::class_cast_if<aWarrior::TWarrior*>(Self) != nullptr) {
                        Wear = Wear * 0.5L;
                        if (pas::checked_cast<aWarrior::TWarrior*>(Self)->WarriorType == aWarrior::wtFlagship) {
                            Wear = Wear * 0.6L;
                        }
                    }
                    if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkReduceEngine))) != 0 && Self->GetEngine() != nullptr && Self->GetEngine()->BrokenFlag == 0) {
                        Self->ApplyCombatItemDegradation(0.5L * Wear);
                        aShip::TShip_ApplyItemDegradation(Self, Self->GetEngine(), idkBattle, 15.0L * Wear);
                    } else {
                        Self->ApplyCombatItemDegradation(Wear);
                    }
                    if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDestruct))) != 0) {
                        Wear = aMyFunction::NextRandomFloatRange(5.0, 15.0, Self->RandomState);
                        if (pas::class_cast_if<aWarrior::TWarrior*>(Self) != nullptr) {
                            Wear = Wear * 0.3L;
                        } else if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                            if (aPlayer::GetPlayer() == Self) {
                                Wear = Wear;
                            } else {
                                Wear = Wear * 0.6L;
                            }
                        }
                        Wear = Wear * 0.1L;
                        if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkReduceEngine))) != 0 && Self->GetEngine() != nullptr && Self->GetEngine()->BrokenFlag == 0) {
                            Self->ApplyCombatItemDegradation(0.5L * Wear);
                            aShip::TShip_ApplyItemDegradation(Self, Self->GetEngine(), idkBattle, 15.0L * Wear);
                        } else {
                            Self->ApplyCombatItemDegradation(Wear);
                        }
                    }
                } else if (aKling::TerronShip != Self) {
                    if ((Self->RandomState + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) % 4 == 0 && pas::list_count(Self->CurrentStar->Items) < 25) {
                        Nodes = pas::construct_call<aItem::TProtoplasm>(aItem::TEquipment_Create);
                        Nodes->Init_2(aMyFunction::NextRandomIntRange(Self->NodeReserve / 8, Self->NodeReserve / 4, Self->RandomState) + 1, 1);
                        Nodes->DominatorSeries = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
                        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Nodes));
                        Self->DropCarriedItemAsMovingLoot(Nodes);
                    }
                    if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDropCargo))) != 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.95L) {
                        Nodes = pas::construct_call<aItem::TProtoplasm>(aItem::TEquipment_Create);
                        Nodes->Init_2(aMyFunction::NextRandomIntRange(Self->NodeReserve / 4, Self->NodeReserve / 2, Self->RandomState) + 1, 1);
                        Nodes->DominatorSeries = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
                        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Nodes));
                        Self->DropCarriedItemAsMovingLoot(Nodes);
                    }
                }
            }
            if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDropCargo))) != 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.95L) {
                Self->JettisonCargoGoodsTowardTargetValue(aGalaxy::Galaxy->ComputeScaledMiniMoney(2));
            }
            if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkBlockWeapon))) != 0) {
                Self->AddCombatStatusStrength(cseWeaponBlock, 0.1f, Attacker);
            }
            if (ScannerEffects && (pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDroidBlock))) != 0) {
                Self->AddCombatStatusStrength(cseDroidBlock, 0.1f, Attacker);
            }
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkShock))) != 0) {
                if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr) {
                    Self->AddCombatStatusStrength(cseShock, AdjustedDamage * 0.12L, Attacker);
                } else {
                    Self->AddCombatStatusStrength(cseShock, AdjustedDamage * 0.2L, Attacker);
                }
            }
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkAcid))) != 0 && static_cast<long double>(Self->GetHull()->Armor) > Self->GetCombatStatusStrength(cseAcid)) {
                if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr) {
                    Self->AddCombatStatusStrength(cseAcid, 0.66f, Attacker);
                } else {
                    Self->AddCombatStatusStrength(cseAcid, 1.0f, Attacker);
                }
            }
            if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkMagnetic))) != 0) {
                if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr) {
                    Self->AddCombatStatusStrength(cseMagnetic, 0.07L + std::max<std::int32_t>(0, Damage + Self->GetArmor()) * 0.075L, Attacker);
                } else {
                    Self->AddCombatStatusStrength(cseMagnetic, 0.1L + std::max<std::int32_t>(0, Damage + Self->GetArmor()) * 0.05L, Attacker);
                }
            }
        }
        if (aPlayer::GetPlayer() == Self) {
            DamageColor = aConst::OwnerToFilmColor(aConst::RaceToOwner(Self->PilotRace));
        } else if (Self->HasNamedScriptFaction()) {
            DamageColor = aConst::CustomFactionToFilmColor(reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText);
        } else {
            DamageColor = aConst::OwnerToFilmColor(Self->OwnerId);
        }
        if (AdjustedDamage <= 0) {
            DamageColor = 0u;
        }
        Self->RefreshDerivedStats(true);
        Result = AdjustedDamage;
        if ((pas::load_unaligned<std::uint32_t>(&DamageFlags) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkDrain))) != 0 && Attacker != nullptr) {
            if (aPlayer::GetPlayer() == Attacker && AdjustedDamage > 0 && Attacker->GetHull()->Weight > Attacker->GetHull()->HullPoints) {
                aPlayer::GetPlayer()->AchievementStats->DrainedHullPoints += std::min<std::int32_t>(Attacker->GetHull()->Weight - Attacker->GetHull()->HullPoints, AdjustedDamage);
                Achievements::TrySetAchievementProgress(u"DRAIN"_w, aPlayer::GetPlayer()->AchievementStats->DrainedHullPoints);
            }
            Attacker->GetHull()->HullPoints = std::min<std::int32_t>(Attacker->GetHull()->Weight, AdjustedDamage + Attacker->GetHull()->HullPoints);
        }
        if (aPlayer::GetPlayer() == Self && aMyFunction::RandomIntRange(0, 100) == 0) {
            SysUtilsImports::Sleep(1u);
        }
        if (Self->GetHull()->HullPoints != ExpectedHullPoints && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        if (Self->IsHullDestroyed()) {
            Self->CurrentStar->ClearShipReferences(Self);
        }
        return Result;
    }

    // Returns ApplyDamage's signed result. Positive FixedDamage bypasses the initial roll/armor stage unless weapon flag 0x800 is already set; later effects still apply.
    std::int32_t TShip_ApplyWeaponHit(TShip* Self, TShip* Source, aItem::TWeapon* Weapon, float HitRange, std::uint32_t& DamageColor, pas::Var<std::uint32_t> DamageFlags, float DamageScale, std::int32_t FixedDamage) {
        static const pas::Set<0, 255> EnergyDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy}});
        std::int32_t I{};
        std::int32_t RolledDamage{};
        std::int32_t MaxDamage{};
        std::int32_t MinDamage{};
        std::int32_t Spread{};
        std::int32_t SkillDifference{};
        float AdjustedDamage{};
        float DamageStep{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        aEFilm::TEFilmObj* Film{};
        SE_Space::TObjectSE* Effect{};
        Flags = Weapon->GetDamageFlags();
        if (FixedDamage > 0 && static_cast<std::uint8_t>(pas::contains(Flags, aGalaxyStruct::dkUndefendable) ^ 1)) {
            AdjustedDamage = FixedDamage;
            pas::include_at(&Flags, aGalaxyStruct::dkUndefendable);
        } else {
            {
                std::int32_t cpp_left = Source->GetEffectiveSkillLevel(psAccuracy, false);
                SkillDifference = cpp_left - (Self->GetEffectiveSkillLevel(psManeuverability, false) & 0x0000007f);
            }
            MaxDamage = Source->GetWeaponMaxDamage(Weapon);
            MinDamage = Source->GetWeaponMinDamage(Weapon);
            DamageStep = pas::real_divide(MaxDamage - MinDamage, 12.0L);
            Spread = System::Round(static_cast<long double>(6 - pas::abs(SkillDifference)) * DamageStep);
            RolledDamage = System::Round(aMyFunction::RemapClamped(SkillDifference, -6.0, 6.0, MinDamage, MaxDamage));
            RolledDamage = aMyFunction::NextRandomIntRange(std::max<std::int32_t>(MinDamage, RolledDamage - Spread), std::min<std::int32_t>(MaxDamage, RolledDamage + Spread), Self->RandomState);
            RolledDamage = System::Round(static_cast<long double>(RolledDamage) * DamageScale);
            if (pas::class_cast_if<aKling::TKling*>(Source) != nullptr && static_cast<aKling::TKling*>(Source)->DominatorSeries == aGalaxyStruct::dsBlazer) {
                if (pas::checked_cast<aKling::TKling*>(Source)->HasNearbyBertorAura()) {
                    if (Self->CurrentStar->RecordingTurnFilm && static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Source)->AuraEffectShownThisTurn ^ 1)) {
                        Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.AuraEffect"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                        Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                        Globals::PrimaryFilm->SetWeaponEndpoints(Self->CurrentStar->CurrentStepIndex, Film, Source->FilmObject, Source->FilmObject);
                        Globals::PrimaryFilm->SetWeaponHit(Self->CurrentStar->CurrentStepIndex, Film, 0, 0, false, true);
                        Globals::PrimaryFilm->AttachObject(Self->CurrentStar->CurrentStepIndex, Film);
                        pas::checked_cast<aKling::TKling*>(Source)->AuraEffectShownThisTurn = true;
                    }
                    RolledDamage = System::Round(RolledDamage * 1.25L);
                }
            }
            RolledDamage = Source->ScriptItemsAct(aConst::satOnWeaponShot, Self, Weapon, RolledDamage);
            if (pas::contains(Flags, aGalaxyStruct::dkUndefendable)) {
                AdjustedDamage = RolledDamage;
            } else if (!Self->GetHull()->ImpulseShieldsEnabled) {
                pas::Extended cpp_left_2 = static_cast<long double>(Self->GetDefenseDamageFactor()) * RolledDamage;
                AdjustedDamage = cpp_left_2 - Self->GetArmor();
            } else {
                pas::Extended cpp_left_3 = (Self->GetDefenseDamageFactor() * 0.5L + 0.5L) * RolledDamage;
                AdjustedDamage = cpp_left_3 - Self->GetArmor();
            }
        }
        if (Source->GetDefGenerator() != nullptr && Source->CountActiveArtefacts(aConst::t_ArtDefToEnergy) > 0 && (Weapon->GetWeaponInfo()->DamageFlags & 1) != 0) {
            double cpp_arg = Source->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(EnergyDamageFlags)) & 0x0000007f;
            double cpp_arg_2 = aConst::DefenseToEnergyUpperFactor + static_cast<long double>(static_cast<std::int8_t>(Source->CanBoostArtefact(aConst::t_ArtDefToEnergy, Weapon, false))) * aConst::DefenseToEnergyUpperBoost;
            double cpp_arg_3 = aConst::DefenseToEnergyMinimumFactor + static_cast<long double>(static_cast<std::int8_t>(Source->CanBoostArtefact(aConst::t_ArtDefToEnergy, Weapon, false))) * aConst::DefenseToEnergyMinimumBoost;
            pas::Extended cpp_left_4 = aMyFunction::RemapClamped(cpp_arg, 1.0, 5.0, cpp_arg_2, cpp_arg_3) - 1.0L;
            AdjustedDamage = (1.0L + cpp_left_4 * Source->CountActiveArtefacts(aConst::t_ArtDefToEnergy)) * AdjustedDamage;
        }
        if ((Weapon->GetWeaponInfo()->DamageFlags & 1) != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Source->CountActiveArtefacts(aConst::t_ArtEnergyPulse)); cpp_range.next(I); ) {
                pas::Extended cpp_left_5 = aMyFunction::NextRandomUnitFloat(Self->RandomState);
                if (cpp_left_5 < aConst::EnergyPulseArtefactChance) {
                    AdjustedDamage = (aConst::EnergyPulseArtefactFactor + static_cast<long double>(static_cast<std::int8_t>(Source->CanBoostArtefact(aConst::t_ArtEnergyPulse, Weapon, false))) * aConst::EnergyPulseArtefactBoostFactor) * AdjustedDamage;
                }
            }
        }
        if (Source->CountActiveArtefacts(aConst::t_ArtDecelerate) > 0 && (Weapon->GetWeaponInfo()->DamageFlags & 2) != 0) {
            pas::include_at(&Flags, aGalaxyStruct::dkDecelerateA);
            if (Source->CanBoostArtefact(aConst::t_ArtDecelerate, Weapon, false) || Source->CountActiveArtefacts(aConst::t_ArtDecelerate) > 1) {
                pas::include_at(&Flags, aGalaxyStruct::dkDecelerateAEx);
            }
        }
        if ((Weapon->GetWeaponInfo()->DamageFlags & 2) != 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Source->CountActiveArtefacts(aConst::t_ArtSplinter)); cpp_range_2.next(I); ) {
                AdjustedDamage = (aConst::SplinterArtefactFactor + static_cast<long double>(static_cast<std::int8_t>(Source->CanBoostArtefact(aConst::t_ArtSplinter, Weapon, false))) * aConst::SplinterArtefactBoostFactor) * AdjustedDamage;
            }
        }
        std::int32_t Damage = System::Round(AdjustedDamage);
        std::int32_t ScriptFlags = pas::load_unaligned<std::int32_t>(&Flags);
        ScriptFlags = Source->ScriptItemsAct(aConst::satOnWeaponShot2, Self, Weapon, ScriptFlags);
        ScriptFlags = Self->ScriptItemsAct(aConst::satOnGettingWeaponHit, Source, Weapon, ScriptFlags);
        Flags = pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&ScriptFlags);
        pas::store_unaligned<std::uint32_t>(DamageFlags.address, pas::load_unaligned<std::uint32_t>(&Flags));
        std::int32_t Result = aShip::TShip_ApplyDamage(Self, Source, Damage, HitRange, DamageColor, Flags);
        if (Self->GetHull()->HullPoints < 1 && aPlayer::GetPlayer() == Source) {
            if ((Weapon->GetWeaponInfo()->DamageFlags & 2) != 0) {
                Achievements::TryAddAchievementProgress(u"SPLINTER"_w, 1);
            }
            if ((Weapon->GetWeaponInfo()->DamageFlags & 1) != 0) {
                Achievements::TryAddAchievementProgress(u"ENERGY"_w, 1);
            }
        }
        if (aPlayer::GetPlayer() == Self && Self->GetHull()->HullPoints < 1) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
            Event->AddTextData(u"KilledByBeamWeapon"_w);
            Globals::ScoreScreen->RecordPlayerResult(false);
        }
        return Result;
    }

    // Requires a TMissile; returns ApplyDamage's signed result.
    std::int32_t TShip_ApplyMissileHit(TShip* Self, pas::Object* Missile, std::uint32_t& DamageColor, pas::Var<std::uint32_t> DamageFlags) {
        std::int32_t SkillDifference{};
        float AdjustedDamage{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        float HitRange{};
        aMissile::TMissile* Shot = pas::checked_cast<aMissile::TMissile*>(Missile);
        Flags = pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Shot->GetWeaponInfo()->DamageFlags);
        if (Shot->MicroModuleIndex != 0) {
            Flags = Flags + pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&aConst::MicroModuleTemplates[Shot->MicroModuleIndex - 1].WeaponDamageFlags);
        }
        if (Shot->SpecialModuleIndex != 0) {
            Flags = Flags + pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&aConst::MicroModuleTemplates[Shot->SpecialModuleIndex - 1].WeaponDamageFlags);
        }
        std::int32_t ScriptFlags = pas::load_unaligned<std::int32_t>(&Flags);
        if (Shot->OwnerShip != nullptr) {
            ScriptFlags = Shot->OwnerShip->ScriptItemsAct(aConst::satOnMissileShot2, Self, Missile, ScriptFlags);
        }
        ScriptFlags = Self->ScriptItemsAct(aConst::satOnGettingMissileHit, Shot->OwnerShip, Missile, ScriptFlags);
        Flags = pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&ScriptFlags);
        pas::store_unaligned<std::uint32_t>(DamageFlags.address, pas::load_unaligned<std::uint32_t>(&Flags));
        if (Shot->OwnerShip == nullptr) {
            SkillDifference = 0 - (Self->GetEffectiveSkillLevel(psManeuverability, false) & 0x0000007f);
        } else {
            std::int32_t cpp_left = Shot->OwnerShip->GetEffectiveSkillLevel(psAccuracy, false);
            SkillDifference = cpp_left - (Self->GetEffectiveSkillLevel(psManeuverability, false) & 0x0000007f);
        }
        std::int32_t MinDamage = Shot->MinDamage;
        std::int32_t MaxDamage = Shot->MaxDamage;
        float DamageStep = pas::real_divide(MaxDamage - MinDamage, 12.0L);
        std::int32_t Spread = System::Round(static_cast<long double>(6 - pas::abs(SkillDifference)) * DamageStep);
        std::int32_t RolledDamage = System::Round(aMyFunction::RemapClamped(SkillDifference, -6.0, 6.0, MinDamage, MaxDamage));
        RolledDamage = aMyFunction::NextRandomIntRange(std::max<std::int32_t>(MinDamage, RolledDamage - Spread), std::min<std::int32_t>(MaxDamage, RolledDamage + Spread), Self->RandomState);
        if (pas::contains(Flags, aGalaxyStruct::dkUndefendable)) {
            AdjustedDamage = RolledDamage;
        } else {
            if (!Self->GetHull()->ImpulseShieldsEnabled) {
                AdjustedDamage = static_cast<long double>(Self->GetDefenseDamageFactor()) * RolledDamage;
            } else {
                AdjustedDamage = (Self->GetDefenseDamageFactor() * 0.5L + 0.5L) * RolledDamage;
            }
            AdjustedDamage = static_cast<long double>(AdjustedDamage) - Self->GetArmor();
        }
        if (Shot->Target == Self) {
            HitRange = -1.0f;
        } else {
            HitRange = 0.0f;
        }
        std::int32_t Damage = System::Round(AdjustedDamage);
        std::int32_t Result = aShip::TShip_ApplyDamage(Self, Shot, Damage, HitRange, DamageColor, pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(DamageFlags.address));
        if (Self->GetHull()->HullPoints < 1 && Shot->OwnerShip != nullptr && aPlayer::GetPlayer() == Shot->OwnerShip) {
            Achievements::TryAddAchievementProgress(u"ROCKET"_w, 1);
        }
        if (aPlayer::GetPlayer() == Self && Self->GetHull()->HullPoints < 1) {
            Globals::ScoreScreen->RecordPlayerResult(false);
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
            Event->AddTextData(u"KilledByMissile"_w);
        }
        return Result;
    }

    // Ignores non-missile targets; can free the target and nearby missiles.
    void TShip_FireWeaponAtMissile(TShip* Self, aItem::TWeapon* Weapon, pas::Object* Target, std::uint8_t RecordFilm) {
        aGalaxy::TStar* Star{};
        aMissile::TMissile* Shot{};
        aMissile::TMissile* Other{};
        std::uint8_t Hit{};
        std::uint8_t OtherHit{};
        std::int32_t StepIndex{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* Film{};
        std::int32_t I{};
        aConst::PWeaponInfo Info{};
        if (pas::class_cast_if<aMissile::TMissile*>(Target) != nullptr) {
            Shot = reinterpret_cast<aMissile::TMissile*>(Target);
            Star = Self->CurrentStar;
            StepIndex = Star->CurrentStepIndex;
            Hit = Shot->CanBeHit(Self, Weapon);
            Hit = Self->ScriptItemsAct(aConst::satOnWeaponShot, Shot, Weapon, static_cast<std::uint8_t>(Hit)) != 0;
            Info = Weapon->GetWeaponInfo();
            if (RecordFilm) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Shot->FilmObject);
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, Film, Shot->Position);
                Globals::PrimaryFilm->SetDestructionEffect(StepIndex, Film, 2);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                if (Hit) {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                    Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, Film, Shot->Position);
                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                    Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                }
            }
            if ((Info->DamageFlags & 1) != 0 && Info->ShotType == aGalaxyStruct::wstSplash) {
                I = 0;
                while (I < pas::list_count(Star->Missiles)) {
                    Other = pas::list_at<aMissile::TMissile>(Star->Missiles, I);
                    ++I;
                    if (Shot == Other) {
                        continue;
                    }
                    if (aMyFunction::PointDistanceSquared(Shot->Position, Other->Position) > pas::sqr(static_cast<pas::Extended>(Info->SecondaryDamageRadius))) {
                        continue;
                    }
                    OtherHit = Other->CanBeHit(Self, Weapon);
                    OtherHit = Self->ScriptItemsAct(aConst::satOnWeaponShot, Other, Weapon, static_cast<std::uint8_t>(OtherHit)) != 0;
                    if (OtherHit) {
                        if (RecordFilm) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                            Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            Globals::PrimaryFilm->SetObjectPosition(StepIndex, Film, Other->Position);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                            Globals::PrimaryFilm->DetachObject(StepIndex, Other->FilmObject);
                            pas::list_add(Star->PendingFilmObjectRemovals, reinterpret_cast<void*>(Other->FilmObject));
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Other->Graphic));
                        }
                        Star->ClearTargetReferences(Other);
                        pas::free(Other);
                    }
                }
            } else if ((Info->DamageFlags & 1) != 0 && Info->ShotType == aGalaxyStruct::wstAreaDamage) {
                I = 0;
                while (I < pas::list_count(Star->Missiles)) {
                    Other = pas::list_at<aMissile::TMissile>(Star->Missiles, I);
                    ++I;
                    if (Shot == Other) {
                        continue;
                    }
                    {
                        pas::Extended cpp_right = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon)) * 1.3L;
                        if (aMyFunction::PointDistanceSquared(Self->Position, Other->Position) > cpp_right) {
                            continue;
                        }
                    }
                    if (Self->TypeId == aGalaxyStruct::stKling && Other->OwnerShip != nullptr && Other->OwnerShip->TypeId == aGalaxyStruct::stKling && ([&] {
                        aGalaxyStruct::TDominatorSeries cpp_left = pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries;
                        return cpp_left == pas::checked_cast<aKling::TKling*>(Other->OwnerShip)->DominatorSeries;
                    }())) {
                        continue;
                    }
                    OtherHit = Other->CanBeHit(Self, Weapon);
                    OtherHit = Self->ScriptItemsAct(aConst::satOnWeaponShot, Other, Weapon, static_cast<std::uint8_t>(OtherHit)) != 0;
                    if (OtherHit) {
                        if (RecordFilm) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                            Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            Globals::PrimaryFilm->SetObjectPosition(StepIndex, Film, Other->Position);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                            Globals::PrimaryFilm->DetachObject(StepIndex, Other->FilmObject);
                            pas::list_add(Star->PendingFilmObjectRemovals, reinterpret_cast<void*>(Other->FilmObject));
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Other->Graphic));
                        }
                        Star->ClearTargetReferences(Other);
                        pas::free(Other);
                    }
                }
            }
            if (Hit) {
                if (RecordFilm) {
                    Globals::PrimaryFilm->DetachObject(StepIndex, Shot->FilmObject);
                    pas::list_add(Star->PendingFilmObjectRemovals, reinterpret_cast<void*>(Shot->FilmObject));
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Shot->Graphic));
                }
                Star->ClearTargetReferences(Shot);
                pas::free(Shot);
            }
        }
    }

    // Can affect additional ships through chained, area or penetrating fire.
    void TShip_FireWeaponAtShip(TShip* Self, aItem::TWeapon* Weapon, TShip* Target, std::uint8_t RecordFilm) {
        std::uint32_t Color{};
        aGalaxyStruct::TDamageFlagSet Flags{};
        std::int32_t Count{};
        float DistanceSquared{};
        aConst::PWeaponInfo Info{};
        std::int32_t Damage{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* Film{};
        std::int32_t StepIndex{};
        pas::List* Ships{};
        pas::List* Damages{};
        pas::List* Colors{};
        pas::List* Films{};
        pas::WideString GraphKey{};
        TShip* Ship{};
        TShip* Nearest{};
        std::int32_t I{};
        float RadiusSquared{};
        float DamageScale{};
        float NearestDistance{};
        float CandidateDistance{};
        std::int32_t ShotIndex{};
        std::uint8_t PlaySound{};
        std::uint32_t DisplayColor{};
        std::int32_t J{};
        std::uint8_t Reflect{};
        // Caller-popped static link; merges repeated targets' damage in the parent's film lists.
        auto FinishChainExplosionFilm = [&]() -> void {
            std::int32_t Damage{};
            std::uint32_t Color{};
            TShip* Ship{};
            std::int32_t I{};
            std::int32_t Index = pas::list_count(Ships) - 1;
            while (Index >= 0) {
                Ship = pas::list_at<TShip>(Ships, Index);
                Damage = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Damages, Index)));
                Color = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Colors, Index)));
                {
                    const std::int32_t cpp_first = Index - 1;
                    if (cpp_first >= 0) {
                        for (I = cpp_first; I >= 0; --I) {
                            if (pas::list_get(Ships, I) == Ship) {
                                Damage += static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Damages, I)));
                            }
                        }
                    }
                }
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, pas::list_at<aEFilm::TEFilmObj>(Films, Index), Color, Damage, Ship->IsHullDestroyed(), true);
                Globals::PrimaryFilm->AttachObject(StepIndex, pas::list_at<aEFilm::TEFilmObj>(Films, Index));
                pas::list_delete(Ships, Index);
                pas::list_delete(Damages, Index);
                pas::list_delete(Colors, Index);
                pas::list_delete(Films, Index);
                {
                    const std::int32_t cpp_first_2 = Index - 1;
                    if (cpp_first_2 >= 0) {
                        for (I = cpp_first_2; I >= 0; --I) {
                            if (pas::list_get(Ships, I) == Ship) {
                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, pas::list_at<aEFilm::TEFilmObj>(Films, I), 0, 0, false, true);
                                Globals::PrimaryFilm->AttachObject(StepIndex, pas::list_at<aEFilm::TEFilmObj>(Films, I));
                                pas::list_delete(Ships, I);
                                pas::list_delete(Damages, I);
                                pas::list_delete(Colors, I);
                                pas::list_delete(Films, I);
                            }
                        }
                    }
                }
                Index = pas::list_count(Ships) - 1;
            }
        };
        if (Target->IsHullDestroyed()) {
            return;
        }
        aGalaxy::TStar* Star = Self->CurrentStar;
        StepIndex = Star->CurrentStepIndex;
        if (aPlayer::GetPlayer() == Target || aPlayer::GetPlayer() == Self) {
            Star->PlayerCombatOccurred = true;
        }
        if (RecordFilm & (aPlayer::GetPlayer() == Target)) {
            Globals::PrimaryFilm->AddCameraEvent(StepIndex, aPlayer::GetPlayer()->Position, Self->Position, 1);
        }
        std::int32_t PrimaryDamage = 0;
        std::int32_t DrainedDamage = 0;
        Info = Weapon->GetWeaponInfo();
        if (Info->ShotType == aGalaxyStruct::wstNormal) {
            Damage = aShip::TShip_ApplyWeaponHit(Target, Self, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
            if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                DrainedDamage += Damage;
            }
            PrimaryDamage = Damage;
            if (RecordFilm) {
                if (RecordFilm & (aPlayer::GetPlayer() == Self)) {
                    Globals::PrimaryFilm->AddCameraEvent(StepIndex, aPlayer::GetPlayer()->Position, Target->Position, 1);
                }
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Target->FilmObject);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Target->IsHullDestroyed(), true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
            }
        } else if (Info->ShotType == aGalaxyStruct::wstChain) {
            if (RecordFilm & (aPlayer::GetPlayer() == Self) && Star->PlayerFilmPath != nullptr) {
                float cpp_arg = pas::real_divide(static_cast<long double>(Target->Position.X) + Self->Position.X, 2.0L);
                float cpp_arg_2 = pas::real_divide(static_cast<long double>(Target->Position.Y) + Self->Position.Y, 2.0L);
                Star->PlayerFilmPath->AppendWaypoint(EC_Struct::MakePointF(cpp_arg, cpp_arg_2), StepIndex + 25);
            }
            RadiusSquared = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon)) * 1.3L;
            Damage = aShip::TShip_ApplyWeaponHit(Target, Self, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
            if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                DrainedDamage += Damage;
            }
            PrimaryDamage = Damage;
            Ships = pas::make_object<pas::List>();
            Damages = pas::make_object<pas::List>();
            Colors = pas::make_object<pas::List>();
            Films = pas::make_object<pas::List>();
            pas::list_add(Ships, reinterpret_cast<void*>(Self));
            pas::list_add(Ships, reinterpret_cast<void*>(Target));
            pas::list_add(Damages, nullptr);
            pas::list_add(Damages, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Damage))));
            pas::list_add(Colors, nullptr);
            pas::list_add(Colors, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Color))));
            if (!Target->IsHullDestroyed()) {
                Count = pas::list_count(Star->Ships);
                DamageScale = 1.0f;
                for (auto cpp_range = pas::for_to<std::int32_t>(2, Weapon->GetShotCount()); cpp_range.next(ShotIndex); ) {
                    {
                        pas::Extended cpp_left = static_cast<long double>(RadiusSquared) * Weapon->GetShotCount();
                        RadiusSquared = pas::real_divide(cpp_left, Weapon->GetShotCount() + 3);
                    }
                    DamageScale = DamageScale - pas::real_divide(1.0L, Weapon->GetShotCount() + 1);
                    if (RadiusSquared <= 0.0L || DamageScale <= 0.0L) {
                        break;
                    }
                    NearestDistance = 1.0E+30f;
                    Nearest = nullptr;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                        Ship = pas::list_at<TShip>(Star->Ships, I);
                        if (Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && pas::list_indexof(Ships, reinterpret_cast<void*>(Ship)) < 0 && aShip::TShip_GetRelationLevelToShip(Self, Ship) <= aGalaxyStruct::rlHostile) {
                            if (!(pas::class_cast_if<aKling::TKling*>(Self) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Self)->IsPlayerCamouflageEffective(Ship) ^ 1)) {
                                if (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Ship)->IsPlayerCamouflageEffective(Self) ^ 1)) {
                                    CandidateDistance = aMyFunction::PointDistanceSquared(Ship->Position, pas::list_at<TShip>(Ships, pas::list_count(Ships) - 1)->Position);
                                    if (CandidateDistance <= RadiusSquared && CandidateDistance < NearestDistance) {
                                        NearestDistance = CandidateDistance;
                                        Nearest = Ship;
                                    }
                                }
                            }
                        }
                    }
                    if (Nearest == nullptr) {
                        DamageScale = DamageScale - pas::real_divide(1.0L, Weapon->GetShotCount() + 1);
                        Nearest = pas::list_at<TShip>(Ships, pas::list_count(Ships) - 1);
                        Damage = aShip::TShip_ApplyWeaponHit(Nearest, Self, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), DamageScale, 0);
                        pas::list_put(Damages, pas::list_count(Damages) - 1, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Damages, pas::list_count(Damages) - 1))) + Damage))));
                        break;
                    }
                    Damage = aShip::TShip_ApplyWeaponHit(Nearest, Self, Weapon, 0.0f, Color, pas::Var<std::uint32_t>(&Flags), DamageScale, 0);
                    if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                        DrainedDamage += Damage;
                    }
                    pas::list_add(Ships, reinterpret_cast<void*>(Nearest));
                    pas::list_add(Damages, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Damage))));
                    pas::list_add(Colors, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Color))));
                    if (Nearest->IsHullDestroyed() || Damage <= 0) {
                        break;
                    }
                }
            }
            if (RecordFilm) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Target->FilmObject);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Damages, 1))), Target->IsHullDestroyed(), true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(2, pas::list_count(Damages) - 1); cpp_range_3.next(ShotIndex); ) {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                    Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    {
                        aEFilm::TEFilmObj* filmObject = pas::list_at<TShip>(Ships, ShotIndex - 1)->FilmObject;
                        aEFilm::TEFilmObj* filmObject_2 = pas::list_at<TShip>(Ships, ShotIndex)->FilmObject;
                        Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, filmObject, filmObject_2);
                    }
                    {
                        std::uint16_t cpp_arg_3 = static_cast<std::uint16_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Colors, ShotIndex)));
                        std::int32_t cpp_arg_4 = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Damages, ShotIndex)));
                        std::uint8_t isHullDestroyed = pas::list_at<TShip>(Ships, ShotIndex)->IsHullDestroyed();
                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, cpp_arg_3, cpp_arg_4, isHullDestroyed, false);
                    }
                    Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                }
            }
            pas::list_clear(Ships);
            pas::free(Ships);
            Ships = nullptr;
            pas::list_clear(Damages);
            pas::free(Damages);
            Damages = nullptr;
            pas::list_clear(Colors);
            pas::free(Colors);
            Colors = nullptr;
            pas::list_clear(Films);
            pas::free(Films);
            Films = nullptr;
        } else if (Info->ShotType == aGalaxyStruct::wstSplash) {
            if (RecordFilm & (aPlayer::GetPlayer() == Self) && Star->PlayerFilmPath != nullptr) {
                float cpp_arg_5 = pas::real_divide(static_cast<long double>(Target->Position.X) + Self->Position.X, 2.0L);
                float cpp_arg_6 = pas::real_divide(static_cast<long double>(Target->Position.Y) + Self->Position.Y, 2.0L);
                Star->PlayerFilmPath->AppendWaypoint(EC_Struct::MakePointF(cpp_arg_5, cpp_arg_6), StepIndex + 25);
            }
            Damage = aShip::TShip_ApplyWeaponHit(Target, Self, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
            if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                DrainedDamage += Damage;
            }
            PrimaryDamage = Damage;
            if (RecordFilm) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Target->FilmObject);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Target->IsHullDestroyed(), true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
            }
            Count = pas::list_count(Star->Ships);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                Ship = pas::list_at<TShip>(Star->Ships, I);
                if (Ship != Self && Ship != Target && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1)) {
                    DistanceSquared = aMyFunction::PointDistanceSquared(Ship->Position, Target->Position);
                    if (pas::sqr(static_cast<pas::Extended>(Info->SecondaryDamageRadius)) >= DistanceSquared) {
                        Damage = aShip::TShip_ApplyWeaponHit(Ship, Self, Weapon, System::Sqrt(DistanceSquared), Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
                        if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                            DrainedDamage += Damage;
                        }
                        if (RecordFilm) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->SecondarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                            Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Target->FilmObject, Ship->FilmObject);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Ship->IsHullDestroyed(), true);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                        }
                    }
                }
            }
        } else if (Info->ShotType == aGalaxyStruct::wstExploder) {
            if (RecordFilm & (aPlayer::GetPlayer() == Self)) {
                Globals::PrimaryFilm->AddCameraEvent(StepIndex, aPlayer::GetPlayer()->Position, Target->Position, 1);
            }
            Damage = aShip::TShip_ApplyWeaponHit(Target, Self, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
            if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                DrainedDamage += Damage;
            }
            PrimaryDamage = Damage;
            if (RecordFilm) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Target->FilmObject);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Target->IsHullDestroyed(), true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
            }
            if (Target->IsHullDestroyed()) {
                Count = pas::list_count(Star->Ships);
                if (RecordFilm) {
                    GraphKey = Info->AreaSE;
                    if (GraphKey != u"") {
                        Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, GraphKey, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                        Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                        Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Target->FilmObject, Target->FilmObject);
                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                        Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                    }
                }
                Ships = pas::make_object<pas::List>();
                Damages = pas::make_object<pas::List>();
                Colors = pas::make_object<pas::List>();
                Films = pas::make_object<pas::List>();
                Damage = aShip::ApplyChainExplosion(Target, Self, Weapon, RecordFilm, Color, Flags, Count, Star, DistanceSquared, Info, Damage, DrainedDamage, Effect, Film, StepIndex, Ships, Damages, Colors, Films, GraphKey);
                if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                    DrainedDamage += Damage;
                }
                if (RecordFilm) {
                    FinishChainExplosionFilm();
                }
                pas::list_clear(Ships);
                pas::free(Ships);
                Ships = nullptr;
                pas::list_clear(Damages);
                pas::free(Damages);
                Damages = nullptr;
                pas::list_clear(Colors);
                pas::free(Colors);
                Colors = nullptr;
                pas::list_clear(Films);
                pas::free(Films);
                Films = nullptr;
            }
        } else if (Info->ShotType == aGalaxyStruct::wstAreaDamage) {
            PlaySound = true;
            RadiusSquared = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon)) * 1.3L;
            if (RecordFilm) {
                GraphKey = Info->AreaSE;
                if (GraphKey != u"") {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, GraphKey, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                    Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Self->FilmObject);
                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                    Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                }
            }
            Count = pas::list_count(Star->Ships);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
                Ship = pas::list_at<TShip>(Star->Ships, I);
                if (Ship != Self && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1)) {
                    if (aMyFunction::PointDistanceSquared(Ship->Position, Self->Position) <= RadiusSquared) {
                        if (Target == Ship) {
                            Damage = aShip::TShip_ApplyWeaponHit(Ship, Self, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
                        } else {
                            Damage = aShip::TShip_ApplyWeaponHit(Ship, Self, Weapon, aMyFunction::PointDistance(Self->Position, Ship->Position), Color, pas::Var<std::uint32_t>(&Flags), 1.0f, 0);
                        }
                        if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                            DrainedDamage += Damage;
                        }
                        if (RecordFilm) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->SecondarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                            Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Ship->FilmObject);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Ship->IsHullDestroyed(), PlaySound);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                            PlaySound = false;
                        }
                    }
                }
            }
        }
        if (RecordFilm && DrainedDamage > 0) {
            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
            Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Self->FilmObject, Self->FilmObject);
            if (aPlayer::GetPlayer() == Self) {
                DisplayColor = aConst::OwnerToFilmColor(aConst::RaceToOwner(Self->PilotRace));
            } else if (Self->HasNamedScriptFaction()) {
                DisplayColor = aConst::CustomFactionToFilmColor(reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText);
            } else {
                DisplayColor = aConst::OwnerToFilmColor(Self->OwnerId);
            }
            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, DisplayColor, -DrainedDamage, false, true);
            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
        }
        if (PrimaryDamage != 0 && Target->CountActiveArtefacts(aConst::t_ArtefactDef) > 0) {
            Reflect = PrimaryDamage < 0;
            if (!Reflect) {
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, Target->CountActiveArtefacts(aConst::t_ArtefactDef)); cpp_range_6.next(J); ) {
                    pas::Extended cpp_right = Target->GetDefenseDamageFactor();
                    if (aMyFunction::RandomUnitFloat() > cpp_right) {
                        Reflect = true;
                        break;
                    }
                }
            }
            if (Reflect) {
                Damage = aShip::TShip_ApplyWeaponHit(Self, Target, Weapon, -1.0f, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, pas::abs(PrimaryDamage));
                if (RecordFilm) {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                    Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Target->FilmObject, Self->FilmObject);
                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Self->IsHullDestroyed(), true);
                    Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                }
            }
        }
    }

    std::int32_t ApplyChainExplosion(TShip* ExplodingShip, TShip*& Self, aItem::TWeapon*& Weapon, std::uint8_t& RecordFilm, std::uint32_t& Color, aGalaxyStruct::TDamageFlagSet& Flags, std::int32_t& Count, aGalaxy::TStar*& Star, float& DistanceSquared, aConst::PWeaponInfo& Info, std::int32_t& Damage, std::int32_t& DrainedDamage, SE_Space::TObjectSE*& Effect, aEFilm::TEFilmObj*& Film, std::int32_t& StepIndex, pas::List*& Ships, pas::List*& Damages, pas::List*& Colors, pas::List*& Films, pas::WideString& GraphKey) {
        std::int32_t I{};
        TShip* Ship{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(Star->Ships, I);
            if (Ship != ExplodingShip && Self != Ship && Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1)) {
                DistanceSquared = aMyFunction::PointDistanceSquared(Ship->Position, ExplodingShip->Position);
                if (pas::sqr(static_cast<pas::Extended>(Info->SecondaryDamageRadius)) >= DistanceSquared && aShip::TShip_GetRelationLevelToShip(Self, Ship) <= aGalaxyStruct::rlHostile) {
                    if (!(pas::class_cast_if<aKling::TKling*>(Self) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Self)->IsPlayerCamouflageEffective(Ship) ^ 1)) {
                        if (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Ship)->IsPlayerCamouflageEffective(Self) ^ 1)) {
                            {
                                float sqrt = System::Sqrt(DistanceSquared);
                                std::int32_t round = System::Round(ExplodingShip->CalculateMass() * 0.1L);
                                Damage = aShip::TShip_ApplyWeaponHit(Ship, Self, Weapon, sqrt, Color, pas::Var<std::uint32_t>(&Flags), 1.0f, round);
                            }
                            if (pas::contains(Flags, aGalaxyStruct::dkDrain) && Damage > 0) {
                                DrainedDamage += Damage;
                            }
                            Result += Damage;
                            if (RecordFilm) {
                                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Info->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, ExplodingShip->FilmObject, Ship->FilmObject);
                                pas::list_add(Ships, reinterpret_cast<void*>(Ship));
                                pas::list_add(Damages, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Damage))));
                                pas::list_add(Colors, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Color))));
                                pas::list_add(Films, reinterpret_cast<void*>(Film));
                            }
                            if (Ship->IsHullDestroyed()) {
                                GraphKey = Info->AreaSE;
                                if (GraphKey != u"") {
                                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, GraphKey, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                                    Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, ExplodingShip->FilmObject, ExplodingShip->FilmObject);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                                }
                                Result += aShip::ApplyChainExplosion(Ship, Self, Weapon, RecordFilm, Color, Flags, Count, Star, DistanceSquared, Info, Damage, DrainedDamage, Effect, Film, StepIndex, Ships, Damages, Colors, Films, GraphKey);
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Weapon may be nil. Script handlers can change the target; a destroyed item may explode and be freed.
    void TShip::FireWeaponAtItem(aItem::TWeapon* Weapon, aItem::TItem* Target, std::uint8_t RecordFilm) {
        aGalaxy::TStar* Star{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* Film{};
        std::int32_t I{};
        std::int32_t Quantity{};
        std::int32_t Count{};
        std::int32_t Damage{};
        TShip* Ship{};
        aItem::TItem* Item{};
        float DistanceSquared{};
        std::uint32_t Color{};
        Star = CurrentStar;
        std::int32_t StepIndex = Star->CurrentStepIndex;
        std::int32_t ActionResult = ScriptItemsAct(aConst::satOnWeaponShot, Target, Weapon, 0);
        if (Target->DestroyFlag == 0) {
            Target->DestroyFlag = 1;
        }
        std::uint8_t WasWeaponTarget = Weapon != nullptr && Weapon->Target == Target;
        if (Target->ScriptItem != nullptr) {
            ActionResult = reinterpret_cast<aScript::TScriptItem*>(Target->ScriptItem)->RunActionCode(aConst::satOnItemHit, this, Weapon, CurrentStar, ActionResult);
        }
        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Target) != nullptr) {
            ActionResult = aScript::RunItemConfigActionCode(Target, aConst::satOnItemHit, this, Weapon, CurrentStar, ActionResult);
        }
        if (WasWeaponTarget && Weapon->Target != Target) {
            return;
        }
        std::uint8_t Explodes = Target->ItemType == aConst::t_ArtefactBomb || pas::class_cast_if<aItem::TCistern*>(Target) != nullptr && static_cast<aItem::TCistern*>(Target)->Fuel > 0 || ActionResult > 0 || Target->DestroyFlag >= 2;
        if (RecordFilm) {
            if (Weapon != nullptr) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Weapon->GetWeaponInfo()->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, FilmObject, Target->FilmObject);
            } else {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Target->FilmObject, Target->FilmObject);
            }
            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, Target->DestroyFlag >= 0, true);
            if (Target->DestroyFlag >= 0) {
                if (Target->DestroyFlag == 0) {
                    Globals::PrimaryFilm->SetDestructionEffect(StepIndex, Film, 6);
                } else if (Explodes) {
                    Globals::PrimaryFilm->SetDestructionEffect(StepIndex, Film, 1);
                } else {
                    Globals::PrimaryFilm->SetDestructionEffect(StepIndex, Film, 3);
                }
            }
            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
        }
        if (Explodes) {
            Count = pas::list_count(Star->Ships);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<TShip>(Star->Ships, I);
                if (Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (aPlayer::GetPlayer() != Ship || aGalaxy::Galaxy->GodModEnabled != 1 && aGalaxy::Galaxy->SpecialSimulationMode == 0)) {
                    DistanceSquared = aMyFunction::PointDistanceSquared(Ship->Position, Target->Position);
                    if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) >= DistanceSquared) {
                        Damage = aShip::TShip_ApplyExplosionDamage(Ship, this, Target, ActionResult, nullptr);
                        if (Ship->IsHullDestroyed() && aPlayer::GetPlayer() != nullptr && Target->ItemType == aConst::t_ArtefactBomb && aPlayer::GetPlayer() == this) {
                            ++aPlayer::GetPlayer()->BombKillsThisTurn;
                        }
                        Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                        if (RecordFilm && Damage > 0) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                            Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, Ship->FilmObject, Ship->FilmObject);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, Color, Damage, Ship->IsHullDestroyed(), true);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                        }
                    }
                }
            }
            Count = pas::list_count(Star->Items);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Star->Items, I);
                if (Item != Target) {
                    DistanceSquared = aMyFunction::PointDistanceSquared(Item->Position, Target->Position);
                    if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) >= DistanceSquared) {
                        pas::Extended cpp_right = aMyFunction::NextRandomIntRange(1, 100, Star->RandomState);
                        if (2.0E+1L - pas::real_divide(2.0E+1L * DistanceSquared, aConst::ItemExplosionRadiusSquared) >= cpp_right) {
                            if (Item->DestroyFlag < 0) {
                                ++Item->DestroyFlag;
                            } else {
                                pas::list_add(Star->ReferencedItems, reinterpret_cast<void*>(Item));
                            }
                        }
                    }
                }
            }
        } else if (aPlayer::GetPlayer() == this && pas::class_cast_if<aItem::TGoods*>(Target) != nullptr && static_cast<aItem::TGoods*>(Target)->NaturalFlag) {
            Quantity = pas::checked_cast<aItem::TGoods*>(Target)->Quantity;
            if (Quantity >= 5) {
                Star->DropMinerals(System::Trunc(pas::real_divide(Quantity * 0.8L, Weapon->GetWeaponInfo()->MiningFactor)), Target->Position, Target->Id * Seed);
            }
        } else if (pas::class_cast_if<aItem::TUselessItem*>(Target) != nullptr && reinterpret_cast<aItem::TEquipment*>(Target)->ConfigBlockName == u"ExampleAsteroid") {
            std::uint32_t cpp_arg = Target->Id * Star->GenerationSeed;
            std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(20, 30, Target->Id * Star->GenerationSeed);
            EC_Struct::TPointF position = Target->Position;
            aGalaxy::TStar* star = Star;
            star->DropMinerals(seededRandomIntRange, position, cpp_arg);
        }
        if (Target->DestroyFlag < 0) {
            ++Target->DestroyFlag;
        } else {
            Star->ClearItemReferences(Target);
            if (RecordFilm) {
                pas::list_add(Star->PendingFilmObjectRemovals, reinterpret_cast<void*>(Target->FilmObject));
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Target->GraphObject));
            }
            pas::list_delete(Star->Items, pas::list_indexof(Star->Items, reinterpret_cast<void*>(Target)));
            pas::free(Target);
        }
    }

    // Ignores non-asteroid targets. Respawning preserves the target object but invalidates its previous motion and mineral reserve.
    void TShip::FireWeaponAtAsteroid(aItem::TWeapon* Weapon, pas::Object* Target, std::uint8_t RecordFilm) {
        aGalaxy::TStar* Star{};
        std::int32_t StepIndex{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* Film{};
        aAsteroid::TAsteroid* Asteroid{};
        std::int32_t Minerals{};
        if (pas::class_cast_if<aAsteroid::TAsteroid*>(Target) != nullptr) {
            Asteroid = reinterpret_cast<aAsteroid::TAsteroid*>(Target);
            Star = CurrentStar;
            StepIndex = Star->CurrentStepIndex;
            ScriptItemsAct(aConst::satOnWeaponShot, Asteroid, Weapon, 0);
            if (RecordFilm) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Weapon->GetWeaponInfo()->PrimarySE, ClassesImports::Point(0, 0), Weapon->GetShotPalette(), -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, Film, FilmObject, nullptr);
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, Film, Asteroid->Position);
                Globals::PrimaryFilm->SetDestructionEffect(StepIndex, Film, 2);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                Film = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, Film, Asteroid->Position);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, Film, 0, 0, false, true);
                Globals::PrimaryFilm->AttachObject(StepIndex, Film);
            }
            Star->ClearTargetReferences(Asteroid);
            Minerals = Star->DropMinerals(System::Trunc(pas::real_divide(Asteroid->MineralCount, Weapon->GetWeaponInfo()->MiningFactor)), Asteroid->Position, Asteroid->Id * Star->GenerationSeed);
            if (aPlayer::GetPlayer() == this) {
                Star->ProcessPlayerAsteroidKill(Minerals, Asteroid->Position, Asteroid->Id);
            }
            Asteroid->Respawn();
            if (aGalaxy::Galaxy->StasisModEnabled == 1) {
                Asteroid->AdvanceOrbitStep(StepIndex, RecordFilm);
            }
        }
    }

    // Uses InterceptorSourceShip; absent source gives base damage 25. Returns ApplyDamage's signed result.
    std::int32_t TShip::ApplyInterceptorDamage(std::uint32_t& DamageColor) {
        static const pas::Set<0, 255> InterceptorDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy}});
        std::int32_t Damage{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        TShip* SourceShip = InterceptorSourceShip;
        if (SourceShip == nullptr) {
            Damage = 25;
        } else {
            Damage = SourceShip->GetInterceptorDamage();
        }
        if (pas::class_cast_if<aKling::TKling*>(this) != nullptr && static_cast<aKling::TKling*>(this)->KlingType == 0) {
            Damage = std::max<std::int32_t>(1, Damage / 2);
        }
        std::int32_t Result = aShip::TShip_ApplyDamage(this, SourceShip, Damage, -1.0f, DamageColor, static_cast<aGalaxyStruct::TDamageFlagSet>(InterceptorDamageFlags));
        if (aPlayer::GetPlayer() == this && GetHull()->HullPoints < 1) {
            Globals::ScoreScreen->RecordPlayerResult(false);
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
            Event->AddTextData(u"KilledByInterceptor"_w);
        }
        return Result;
    }

    // Uses rounded shock strength and nonlethal flag 0x1000; returns ApplyDamage's signed result.
    std::int32_t TShip::ApplyShockStatusDamage(std::uint32_t& DamageColor) {
        static const pas::Set<0, 255> ShockDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy}, {aGalaxyStruct::dkNonLethal}});
        return aShip::TShip_ApplyDamage(this, nullptr, System::Round(GetCombatStatusStrength(cseShock)), -1.0f, DamageColor, static_cast<aGalaxyStruct::TDamageFlagSet>(ShockDamageFlags));
    }

    // Returns script-adjusted damage, which may exceed actual hull loss. DamageColor uses the current packed pixel format and is zero for nonpositive damage.
    std::int32_t TShip::ApplyAsteroidImpactDamage(aAsteroid::TAsteroid* Asteroid, std::uint32_t& DamageColor) {
        std::int32_t Damage{};
        float Factor{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (ScriptShip != nullptr && static_cast<std::uint8_t>(HasScriptStateText() ^ 1)) {
            Damage = 0;
        } else if (aPlayer::GetPlayer() == this && (aGalaxy::Galaxy->GodModEnabled == 1 || aGalaxy::Galaxy->SpecialSimulationMode != 0)) {
            Damage = 0;
        } else {
            Factor = pas::real_max<pas::Extended>(0.0L, 1.0L - GetTotalStatBonus(aConst::bonResistAsteroid) * 0.01L);
            if (aShip::TShip_IsEquipmentUsable(this, GetDefGenerator())) {
                double nextRandomUnitFloat = aMyFunction::NextRandomUnitFloat(RandomState);
                double cpp_arg = static_cast<long double>(GetHull()->Weight) * aConst::AsteroidMinDamageFactorWithDefGenerator;
                double cpp_arg_2 = static_cast<long double>(GetHull()->Weight) * aConst::AsteroidMaxDamageFactorWithDefGenerator;
                Damage = System::Round(static_cast<long double>(aMyFunction::RemapClamped(nextRandomUnitFloat, 0.0, 1.0, cpp_arg, cpp_arg_2)) * Factor);
            } else {
                double nextRandomUnitFloat_2 = aMyFunction::NextRandomUnitFloat(RandomState);
                double cpp_arg_3 = static_cast<long double>(GetHull()->Weight) * aConst::AsteroidMinDamageFactor;
                double cpp_arg_4 = static_cast<long double>(GetHull()->Weight) * aConst::AsteroidMaxDamageFactor;
                Damage = System::Round(static_cast<long double>(aMyFunction::RemapClamped(nextRandomUnitFloat_2, 0.0, 1.0, cpp_arg_3, cpp_arg_4)) * Factor);
            }
            Damage = ScriptItemsAct(aConst::satOnTakingDamage, Asteroid, nullptr, Damage);
            if (GetHull()->HullPoints - Damage <= 0 && aKling::BlazerShip != this && aKling::KellerShip != this && aKling::TerronShip != this) {
                ScriptItemsAct(aConst::satOnDeath, nullptr, Asteroid, 0);
                GetHull()->HullPoints = 0;
                if (aPlayer::GetPlayer() == PartnerShip || pas::class_cast_if<aTranclucator::TTranclucator*>(this) != nullptr && static_cast<aTranclucator::TTranclucator*>(this)->OwnerShip == aPlayer::GetPlayer()) {
                    aShip::TShip_NotifyCompanionDeath(this);
                }
                aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), this);
            } else {
                GetHull()->HullPoints -= Damage;
                if (GetHull()->HullPoints < 1) {
                    GetHull()->HullPoints = 1;
                }
            }
        }
        if (aPlayer::GetPlayer() == this) {
            DamageColor = aConst::OwnerToFilmColor(aConst::RaceToOwner(PilotRace));
        } else if (HasNamedScriptFaction()) {
            DamageColor = aConst::CustomFactionToFilmColor(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText);
        } else {
            DamageColor = aConst::OwnerToFilmColor(OwnerId);
        }
        if (Damage <= 0) {
            DamageColor = 0u;
        }
        std::int32_t Result = Damage;
        if (aPlayer::GetPlayer() == this && GetHull()->HullPoints < 1) {
            Globals::ScoreScreen->RecordPlayerResult(false);
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
            Event->AddTextData(u"KilledByAsteroid"_w);
        }
        if (IsHullDestroyed()) {
            CurrentStar->ClearShipReferences(this);
        }
        return Result;
    }

    // ExplodingObject may be an item or ship; SourceShip and Missile may be nil. Returns script-adjusted damage, not actual hull loss; can trigger death handling without freeing Self.
    std::int32_t TShip_ApplyExplosionDamage(TShip* Self, TShip* SourceShip, pas::Object* ExplodingObject, std::int32_t ExtraDamage, pas::Object* Missile) {
        TShip* Ship{};
        float DistanceSquared{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::int32_t Damage = ExtraDamage;
        aItem::TItem* Item = nullptr;
        std::int32_t Result = 0;
        if (pas::class_cast_if<aItem::TItem*>(ExplodingObject) != nullptr) {
            Item = reinterpret_cast<aItem::TItem*>(ExplodingObject);
            DistanceSquared = aMyFunction::PointDistanceSquared(Self->Position, Item->Position);
            if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) < DistanceSquared) {
                return Result;
            }
            if (Item->ItemType == aConst::t_ArtefactBomb) {
                Damage += System::Round(aMyFunction::RemapClamped(DistanceSquared, 0.0, aConst::ItemExplosionRadiusSquared, aConst::BombMaximumDamage, aConst::BombMinimumDamage));
            } else if (pas::class_cast_if<aItem::TCistern*>(Item) != nullptr) {
                Damage += pas::checked_cast<aItem::TCistern*>(Item)->Fuel;
            }
            if (Item->DestroyFlag == 2) {
                Damage += aConst::ItemExplosionBonusDamage;
            } else if (Item->DestroyFlag > 2) {
                Damage += Item->DestroyFlag;
            }
            if (Damage == 0) {
                return Result;
            }
            if (Missile != nullptr) {
                Damage = Self->ScriptItemsAct(aConst::satOnTakingDamage, Item, Missile, Damage);
            } else {
                Damage = Self->ScriptItemsAct(aConst::satOnTakingDamage, Item, SourceShip, Damage);
            }
        } else if (pas::class_cast_if<TShip*>(ExplodingObject) != nullptr) {
            Ship = reinterpret_cast<TShip*>(ExplodingObject);
            if (aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) > 2.25E+4L) {
                return Result;
            }
            Damage = Ship->GetHull()->Weight / 3;
            Damage = Ship->ScriptItemsAct(aConst::satOnDealingKamikazeDamage, Self, nullptr, Damage);
            Damage = Self->ScriptItemsAct(aConst::satOnTakingDamage, Ship, nullptr, Damage);
        }
        if (Damage > 0) {
            Result = Damage;
            if (SourceShip != nullptr && Self != SourceShip && Item != nullptr) {
                if (Missile == nullptr || reinterpret_cast<aMissile::TMissile*>(Missile)->Target == Item) {
                    if (aPlayer::GetPlayer() == Self && SourceShip->ScriptShip != nullptr) {
                        reinterpret_cast<aScript::TScriptShip*>(SourceShip->ScriptShip)->HitPlayer = true;
                    }
                    if (aPlayer::GetPlayer() == SourceShip && Self->ScriptShip != nullptr) {
                        reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->Hit = true;
                    }
                }
                if (Item->ItemType == aConst::t_ArtefactBomb) {
                    Self->ReactToAttack(SourceShip);
                } else if (pas::class_cast_if<aRanger::TRanger*>(SourceShip) != nullptr) {
                    Self->ChangeRelationToRanger(SourceShip, System::Round(aMyFunction::RemapClamped(Damage, 1.0, Self->GetHull()->HullPoints, -1.0, -8.0E+1)));
                }
            }
            if (Self->GetHull()->HullPoints <= Damage && pas::class_cast_if<aKling::TKling*>(Self) != nullptr && static_cast<aKling::TKling*>(Self)->KlingType == aGalaxyStruct::ktBoss && (aKling::KellerShip == Self || aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar != Self->CurrentStar)) {
                Self->GetHull()->HullPoints = Damage + 1;
            }
            if (Self->GetHull()->HullPoints <= Damage && aPlayer::GetPlayer() != nullptr) {
                Self->ScriptItemsAct(aConst::satOnDeath, SourceShip, ExplodingObject, 0);
                aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), Self);
                if (SourceShip != nullptr) {
                    if (pas::class_cast_if<aNormalShip::TNormalShip*>(SourceShip) != nullptr) {
                        aNormalShip::TNormalShip_ProcessShipKill(reinterpret_cast<aNormalShip::TNormalShip*>(SourceShip), Self);
                    }
                    if (pas::class_cast_if<aRanger::TRanger*>(SourceShip) != nullptr) {
                        aRanger::TRanger_ApplyAttackReputationChanges(reinterpret_cast<aRanger::TRanger*>(SourceShip), Self, 1.0);
                    }
                }
                if (aPlayer::GetPlayer() == Self->PartnerShip || pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && static_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip == aPlayer::GetPlayer()) {
                    aShip::TShip_NotifyCompanionDeath(Self);
                }
                if (aPlayer::GetPlayer() == Self) {
                    Globals::ScoreScreen->RecordPlayerResult(false);
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
                    Event->AddTextData(u"KilledByExplosion"_w);
                }
            }
            if (Self->TypeId == aGalaxyStruct::stKling && SourceShip != nullptr) {
                pas::checked_cast<aKling::TKling*>(Self)->DetectAttackingPlayer(SourceShip);
            }
            Self->GetHull()->HullPoints = std::max<std::int32_t>(Self->GetHull()->HullPoints - Damage, 0);
            Self->RefreshDerivedStats(true);
            if (Self->IsHullDestroyed()) {
                Self->CurrentStar->ClearShipReferences(Self);
            }
        }
        return Result;
    }

    // Also refuels up to five units inside the damage radius. Returns script-adjusted damage; Dominator bosses survive with at least one hull point.
    std::int32_t TShip::ApplyStarHeatDamage() {
        float Damage{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::int32_t FuelAdded{};
        std::int32_t I{};
        std::int32_t Result = 0;
        if (CurrentStar == nullptr) {
            return Result;
        }
        float DistanceSquared = pas::sqr(static_cast<pas::Extended>(Position.X)) + pas::sqr(static_cast<pas::Extended>(Position.Y));
        if (pas::sqr(static_cast<pas::Extended>(CurrentStar->DamageRadius)) > DistanceSquared) {
            if (TypeId == aGalaxyStruct::stKling) {
                Damage = pas::real_max<pas::Extended>((1.0L - pas::real_divide(System::Sqrt(DistanceSquared), CurrentStar->DamageRadius)) * 3.0E+1L * aGalaxy::Galaxy->GetStarDamageDifficultyScale(), 1.0L);
            } else {
                Damage = pas::real_max<pas::Extended>((1.0L - pas::real_divide(System::Sqrt(DistanceSquared), CurrentStar->DamageRadius)) * 1.0E+2L * aGalaxy::Galaxy->GetStarDamageDifficultyScale(), 1.0L);
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtefactPower)); cpp_range.next(I); ) {
                Damage = (1.0L - aConst::StarHeatArtefactReduction - static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtefactPower, GetHull(), false))) * aConst::StarHeatArtefactBoostReduction) * Damage;
            }
            Damage = ScriptItemsAct(aConst::satOnTakingDamage, CurrentStar, nullptr, System::Round(Damage));
            Result = System::Round(Damage);
            GetHull()->HullPoints = std::max<std::int32_t>(GetHull()->HullPoints - System::Round(Damage), 0);
            if (GetHull()->HullPoints <= 0 && pas::class_cast_if<aKling::TKling*>(this) != nullptr && static_cast<aKling::TKling*>(this)->KlingType == aGalaxyStruct::ktBoss) {
                GetHull()->HullPoints = 1;
            }
            if (GetHull()->HullPoints <= 0 && aPlayer::GetPlayer() != nullptr) {
                ScriptItemsAct(aConst::satOnDeath, nullptr, CurrentStar, 0);
                aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), this);
                if (aShip::TShip_GetRelationLevelToShip(this, aPlayer::GetPlayer()) <= aGalaxyStruct::rlHostile) {
                    if (aPlayer::GetPlayer() == OrderTarget || aPlayer::GetPlayer() == EnemyShip && pas::is_one_of<soNone, soLand, soJump>(Order) || aPlayer::GetPlayer()->IsAttackingShip(this)) {
                        ++aPlayer::GetPlayer()->AchievementStats->EnemiesDestroyedByStarHeat;
                        Achievements::TrySetAchievementProgress(u"FRY"_w, aPlayer::GetPlayer()->AchievementStats->EnemiesDestroyedByStarHeat);
                    }
                }
                if (aPlayer::GetPlayer() == PartnerShip || pas::class_cast_if<aTranclucator::TTranclucator*>(this) != nullptr && static_cast<aTranclucator::TTranclucator*>(this)->OwnerShip == aPlayer::GetPlayer()) {
                    aShip::TShip_NotifyCompanionDeath(this);
                }
                if (aPlayer::GetPlayer() == this) {
                    Globals::ScoreScreen->RecordPlayerResult(false);
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
                    Event->AddTextData(u"KilledBySunDamage"_w);
                }
            }
            if (GetFuelTanks() != nullptr) {
                FuelAdded = std::max<std::int32_t>(0, std::min<std::int32_t>(5, GetFuelTanks()->Capacity - GetFuelTanks()->Fuel));
                GetFuelTanks()->Fuel += FuelAdded;
                if (FuelAdded > 0 && aPlayer::GetPlayer() == this) {
                    if (GetFuelTanks()->Id != aPlayer::GetPlayer()->AchievementStats->StarFuelTankId) {
                        aPlayer::GetPlayer()->AchievementStats->StarFuelCollected = 0u;
                    }
                    aPlayer::GetPlayer()->AchievementStats->StarFuelTankId = GetFuelTanks()->Id;
                    aPlayer::GetPlayer()->AchievementStats->StarFuelCollected += FuelAdded;
                    aPlayer::GetPlayer()->AchievementStats->CheckStarFuelAchievement();
                }
            }
            RefreshDerivedStats(true);
            if (IsHullDestroyed()) {
                CurrentStar->ClearShipReferences(this);
            }
        }
        return Result;
    }

    std::uint8_t TShip::CountEquippedWeapons() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag != 0 && pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                ++Count;
            }
        }
        return Count;
    }

    // Counts torpedo/missile/rocket shot types; does not check usability or ammunition.
    std::uint8_t TShip::CountMissileWeapons() {
        std::int32_t I{};
        std::uint8_t Result = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (pas::in_range(Weapons[I]->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        ++Result;
                    }
                }
            }
        }
        return Result;
    }

    // Complement of CountMissileWeapons over cached equipped weapons.
    std::uint8_t TShip::CountDirectFireWeapons() {
        std::int32_t I{};
        std::uint8_t Result = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (!pas::in_range(Weapons[I]->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        ++Result;
                    }
                }
            }
        }
        return Result;
    }

    void TShip::ClearUnequippedWeaponTargets() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag == 0 && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && static_cast<aItem::TWeapon*>(Item)->Target != nullptr) {
                pas::checked_cast<aItem::TWeapon*>(Item)->Target = nullptr;
            }
        }
    }

    // Counts any intersection; does not check usability or ammunition.
    std::uint8_t TShip::CountWeaponsByDamageFlags(aGalaxyStruct::TDamageFlagSet Flags) {
        std::int32_t I{};
        std::uint8_t Result = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (Weapons[I]->GetDamageFlags() * Flags != pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})) {
                        ++Result;
                    }
                }
            }
        }
        return Result;
    }

    pas::WideString TShip_GetWeaponDamageSummary(TShip* Self) {
        pas::WideString Result{};
        pas::Extended DifficultyFactor{};
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        Result = u"0"_w;
        std::int32_t MinimumDamage = 0;
        std::int32_t MaximumDamage = 0;
        if (Self->WeaponCount != 0) {
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapon = Self->Weapons[I];
                        if (aShip::TShip_IsEquipmentUsable(Self, Weapon)) {
                            if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstMissile), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                                MinimumDamage += ([&] {
                                    std::int32_t cpp_left_2 = Self->GetWeaponMinDamage(Weapon);
                                    std::int32_t cpp_left = cpp_left_2 * Weapon->GetAttackCount();
                                    return cpp_left * Weapon->GetShotCount();
                                }());
                                MaximumDamage += ([&] {
                                    std::int32_t cpp_left_4 = Self->GetWeaponMaxDamage(Weapon);
                                    std::int32_t cpp_left_3 = cpp_left_4 * Weapon->GetAttackCount();
                                    return cpp_left_3 * Weapon->GetShotCount();
                                }());
                            } else {
                                MinimumDamage += ([&] {
                                    std::int32_t cpp_left_5 = Self->GetWeaponMinDamage(Weapon);
                                    return cpp_left_5 * Weapon->GetAttackCount();
                                }());
                                MaximumDamage += ([&] {
                                    std::int32_t cpp_left_6 = Self->GetWeaponMaxDamage(Weapon);
                                    return cpp_left_6 * Weapon->GetAttackCount();
                                }());
                            }
                        }
                    }
                }
            }
            MinimumDamage *= Self->GetAttackMultiplier();
            MaximumDamage *= Self->GetAttackMultiplier();
            if (Self->TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(Self)->KlingType == 0) {
                DifficultyFactor = aGalaxy::Galaxy->InterpolateDifficulty(-1, 0.7f, 1.0f, 1.2f, 1.5f) * 2.0L;
                MinimumDamage = System::Round(MinimumDamage * DifficultyFactor);
                MaximumDamage = System::Round(MaximumDamage * DifficultyFactor);
            }
            Result = pas::concat_wide({pas::wide_int_to_str(MinimumDamage), aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w), pas::wide_int_to_str(MaximumDamage)});
            return pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(Self->GetEffectiveSkillLevel(psAccuracy, false) & 0x0000007f), ") "})), Result});
        }
        return Result;
    }

    pas::WideString TShip::GetManeuverabilitySummary() {
        return static_cast<pas::WideString>(pas::concat_ansi({"(", SysUtils::IntToStr(GetEffectiveSkillLevel(psManeuverability, false) & 0x0000007f), ") "}));
    }

    pas::WideString TShip_GetRepairPointsSummary(TShip* Self) {
        pas::WideString Result{};
        Result = u"0"_w;
        if (Self->GetRepairRobot() != nullptr) {
            return pas::wide_int_to_str(aShip::TShip_CalculateRepairPoints(Self, Self->GetRepairRobot()));
        }
        return Result;
    }

    // Callers pass the target ship in EDX. This routine ignores it and only checks Self's active scanner artefact count.
    std::uint8_t TShip::HasScannerArtefact(TShip* UnusedTarget) {
        return CountActiveArtefacts(aConst::t_ArtefactScaner) > 0;
    }

    // Same result as GetWeaponRange in this binary.
    std::int32_t TShip_GetWeaponActionRange(TShip* Self, aItem::TWeapon* Weapon) {
        return aShip::TShip_GetWeaponRange(Self, Weapon);
    }

    // Weapon slots are numbered 1..5.
    std::int32_t TShip::GetWeaponSlotRange(std::int32_t SlotIndex) {
        return aShip::TShip_GetWeaponRange(this, Weapons[SlotIndex]);
    }

    std::int32_t TShip::GetWeaponMinDamage(aItem::TWeapon* Weapon) {
        if ((std::bit_cast<std::uint32_t>(Weapon->GetDamageFlags()) & aGalaxyStruct::DamageNoDeltaMask) != 0) {
            return GetWeaponMaxDamage(Weapon);
        }
        return Weapon->MinDamage;
    }

    std::int32_t TShip::GetWeaponMaxDamage(aItem::TWeapon* Weapon) {
        std::uint8_t BonusKind{};
        std::int32_t Bonus{};
        std::int32_t I{};
        aItem::PExtraSpecial Extra{};
        std::int32_t Result = std::max<std::int32_t>(Weapon->MaxDamage, Weapon->MinDamage);
        if (pas::in_range(static_cast<std::uint8_t>(Weapon->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            BonusKind = aConst::WeaponDamageClasses[aConst::ClassifyWeaponDamageFlags(Weapon->GetWeaponInfo()->DamageFlags)].BonusKind;
            Bonus = GetTotalStatBonus(BonusKind);
            if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstMissile), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && static_cast<std::uint8_t>(aGalaxy::Galaxy->AreOldMissileBonusesEnabled() ^ 1)) {
                if (Weapon->MicroModuleIndex > 0) {
                    Result -= pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Weapon->MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
                    Bonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Weapon->MicroModuleIndex - 1].StatBonuses, BonusKind * sizeof(std::int32_t)));
                }
                Bonus = MathImports::Ceil(pas::real_divide(Bonus, Weapon->GetShotCount()));
            }
            if (Weapon->ExtraSpecials != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Weapon->ExtraSpecials) - 1); cpp_range.next(I); ) {
                    Extra = pas::list_at<aItem::TExtraSpecial>(Weapon->ExtraSpecials, I);
                    Bonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Extra->ModuleIndexPlusOne - 1].StatBonuses, BonusKind * sizeof(std::int32_t))) * Extra->Count;
                }
            }
            return std::max<std::int32_t>(Weapon->MinDamage, Result + Bonus);
        }
        return Result;
    }

    std::int32_t TShip::GetMaxWeaponRange() {
        std::int32_t I{};
        std::int32_t Result = 0;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (aShip::TShip_IsEquipmentUsable(this, Weapons[I]) && GetWeaponSlotRange(I) > Result) {
                        Result = GetWeaponSlotRange(I);
                    }
                }
            }
        }
        return Result;
    }

    // Nil clears every cached weapon target; otherwise clears only matches.
    void TShip::ClearWeaponTargets(pas::Object* Target) {
        std::int32_t I{};
        if (Target == nullptr) {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapons[I]->Target = nullptr;
                }
            }
        } else {
            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last_2) {
                for (I = 1; I <= cpp_last_2; ++I) {
                    if (Weapons[I]->Target == Target) {
                        Weapons[I]->Target = nullptr;
                    }
                }
            }
        }
    }

    // Includes weapon targets, interceptor attribution and shock/acid source IDs; requires non-nil Target.
    std::uint8_t TShip::IsAttackingShip(TShip* Target) {
        std::int32_t I{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (Weapons[I]->Target == Target) {
                        return true;
                    }
                }
            }
        }
        if (Target->InterceptorSourceShip == this) {
            return true;
        }
        if (Target->GetCombatStatusSourceId(cseShock) == Id) {
            return true;
        }
        if (Target->GetCombatStatusSourceId(cseAcid) == Id) {
            return true;
        }
        return false;
    }

    // Returns a combat strength ratio, not a probability.
    double TShip_ChanceToWin(TShip* Self, TShip* Target) {
        double Result{};
        std::int32_t Index{};
        double Damage{};
        double OwnDamage{};
        double TargetDamage{};
        double DefenseFactor{};
        double Armor{};
        try {
            if (Self->GetHull()->HullPoints < 1) {
                GR_Main::RaiseWideMessage(u"ChanceToWin error, hitpoints=0!"_wref.get());
            }
            if (Target->GetHull()->HullPoints < 1) {
                return 1.0E+2;
            }
            OwnDamage = 0.0;
            DefenseFactor = Target->GetDefenseDamageFactor();
            Armor = Target->GetArmor();
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last) {
                    for (Index = 1; Index <= cpp_last; ++Index) {
                        if (aShip::TShip_IsEquipmentUsable(Self, Self->Weapons[Index])) {
                            Damage = static_cast<long double>(Self->GetWeaponMaxDamage(Self->Weapons[Index])) * DefenseFactor - Armor;
                            if (Damage > 0.0L) {
                                OwnDamage = static_cast<long double>(OwnDamage) + Damage;
                            }
                        }
                    }
                }
            }
            if (OwnDamage == 0.0L) {
                return 0.0;
            }
            TargetDamage = 0.0;
            DefenseFactor = Self->GetDefenseDamageFactor();
            Armor = Self->GetArmor();
            {
                const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Target->WeaponCount);
                if (1 <= cpp_last_2) {
                    for (Index = 1; Index <= cpp_last_2; ++Index) {
                        if (aShip::TShip_IsEquipmentUsable(Target, Target->Weapons[Index])) {
                            Damage = static_cast<long double>(Target->GetWeaponMaxDamage(Target->Weapons[Index])) * DefenseFactor - Armor;
                            if (Damage > 0.0L) {
                                TargetDamage = static_cast<long double>(TargetDamage) + Damage;
                            }
                        }
                    }
                }
            }
            if (TargetDamage == 0.0L) {
                return 1.0E+2;
            }
            {
                pas::Extended cpp_left = pas::real_divide(static_cast<long double>(Self->GetHull()->HullPoints) * OwnDamage, pas::real_max<float>(0.01f, Self->GetHull()->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}))));
                Result = pas::real_divide(cpp_left, pas::real_divide(static_cast<long double>(Target->GetHull()->HullPoints) * TargetDamage, pas::real_max<float>(0.01f, Target->GetHull()->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})))));
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TShip.ChanceToWin ", Self->GetFullName(u" "_wref.get())})));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"vs ", Target->GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
        return Result;
    }

    std::uint8_t TShip_GetWinChancePercent(TShip* Self, TShip* Target) {
        double Value = aShip::TShip_ChanceToWin(Self, Target);
        if (Value >= 1.0L) {
            return System::Round(aMyFunction::RemapClamped(Value, 1.0, 4.0, 5.0E+1, 1.0E+2));
        }
        return System::Round(aMyFunction::RemapClamped(Value, 0.0, 1.0, 0.0, 5.0E+1));
    }

    // Assigns both ships' enemies and eligible weapons, then issues pursuit orders where possible.
    void TShip::SetJointAttackTarget(TShip* Ally, TShip* Target) {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        EnemyShip = Target;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Weapons[I];
                    if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) {
                        if (([&] {
                            pas::Extended cpp_left = pas::sqr(GetWeaponSlotRange(I));
                            return cpp_left >= aMyFunction::PointDistanceSquared(Position, Target->Position);
                        }()) && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                            Weapon->Target = Target;
                        }
                    }
                }
            }
        }
        if (HasPositiveSpeed()) {
            if (aPlayer::GetPlayer() == this) {
                OrderNone(false);
                aRanger::PendingPlayerFollowTarget = Target;
            } else {
                OrderFollowShip(EnemyShip, 1, true);
            }
        }
        Ally->EnemyShip = Target;
        {
            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Ally->WeaponCount);
            if (1 <= cpp_last_2) {
                for (I = 1; I <= cpp_last_2; ++I) {
                    Weapon = Ally->Weapons[I];
                    if (static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapon->Ammo != 0) {
                        if (([&] {
                            pas::Extended cpp_left_2 = pas::sqr(Ally->GetWeaponSlotRange(I));
                            return cpp_left_2 >= aMyFunction::PointDistanceSquared(Ally->Position, Target->Position);
                        }()) && aShip::TShip_IsEquipmentUsable(Ally, Weapon)) {
                            Weapon->Target = Target;
                        }
                    }
                }
            }
        }
        if (Ally->HasPositiveSpeed()) {
            if (aPlayer::GetPlayer() == Ally) {
                aPlayer::GetPlayer()->OrderNone(false);
                aRanger::PendingPlayerFollowTarget = Target;
            } else {
                Ally->OrderFollowShip(Ally->EnemyShip, 1, true);
            }
        }
    }

    std::uint8_t TShip::IsEnemyPursuingSelf() {
        if (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar || static_cast<std::uint8_t>(EnemyShip->InNormalSpace() ^ 1)) {
            return false;
        }
        return EnemyShip->OrderTarget == this;
    }

    // Rejects bomb/explosive cargo near non-hostile normal-space ships, including Self. Does not test shot range.
    std::uint8_t TShip::CanSafelyDetonateItem(aItem::TItem* Item) {
        std::int32_t I{};
        TShip* Ship{};
        float Distance{};
        std::uint8_t Result = true;
        if (CurrentStar != nullptr && (Item->ItemType == aConst::t_ArtefactBomb || pas::class_cast_if<aItem::TCistern*>(Item) != nullptr)) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
                if (!Ship->IsOutsideStarSpace()) {
                    Distance = aMyFunction::PointDistanceSquared(Ship->Position, Item->Position);
                    if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) >= Distance && aShip::TShip_GetRelationLevelToShip(this, Ship) > aGalaxyStruct::rlHostile) {
                        return false;
                    }
                }
            }
        }
        return Result;
    }

    // Requires a non-nil relation list; nonempty lists require a registered ranger. Stores representative values 5, 20, 45, 70 or 90.
    void TShip::SetStoredRangerRelationLevel(TShip* Ranger, aGalaxyStruct::TRelationLevel Level) {
        if (pas::list_count(RangerRelations) < 1) {
            return;
        }
        switch (Level) {
            case aGalaxyStruct::rlHostile: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(Ranger))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(5))));
                break;
            }
            case aGalaxyStruct::rlBad: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(Ranger))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(20))));
                break;
            }
            case aGalaxyStruct::rlNormal: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(Ranger))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(45))));
                break;
            }
            case aGalaxyStruct::rlGood: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(Ranger))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(70))));
                break;
            }
            case aGalaxyStruct::rlExcellent: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(Ranger))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(90))));
                break;
            }
            default: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(Ranger))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(5))));
                break;
            }
        }
    }

    aGalaxyStruct::TRelationLevel TShip_GetRelationLevelToShip(TShip* Self, TShip* Ship) {
        std::int32_t cpp_case = aShip::TShip_RelationToShip(Self, Ship) & 0x0000007f;
        if (cpp_case >= 0 && cpp_case <= 9) {
            return aGalaxyStruct::rlHostile;
        } else if (cpp_case >= 10 && cpp_case <= 29) {
            return aGalaxyStruct::rlBad;
        } else if (cpp_case >= 30 && cpp_case <= 59) {
            return aGalaxyStruct::rlNormal;
        } else if (cpp_case >= 60 && cpp_case <= 79) {
            return aGalaxyStruct::rlGood;
        } else if (cpp_case >= 80 && cpp_case <= 100) {
            return aGalaxyStruct::rlExcellent;
        } else {
            return aGalaxyStruct::rlNormal;
        }
    }

    // Stations can display their stored ranger relation instead of the effective relation.
    pas::WideString TShip_GetRelationLevelTextToShip(TShip* Self, TShip* Ship) {
        pas::WideString Result{};
        aGalaxyStruct::TRelationLevel Level = aShip::TShip_GetRelationLevelToShip(Self, Ship);
        Result = aConst::RelationInfo[Level].DisplayName;
        if (Level != aGalaxyStruct::rlHostile && pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr && pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr && Self->RangerRelations != nullptr && pas::list_count(Self->RangerRelations) > 0) {
            Level = aConst::RelationValueToLevel(static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Self->RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(Ship))))));
            if (Level != aGalaxyStruct::rlHostile) {
                return aConst::RelationInfo[Level].DisplayName;
            }
        }
        return Result;
    }

    // Attempts a cargo or money demand through the target's dialogue handler; requires a player.
    std::uint8_t TShip_TryExtortShip(TShip* Self, TShip* Target) {
        std::int32_t Amount{};
        pas::WideString Response{};
        std::uint8_t Result = false;
        if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate) && Target != Self->TruceShip && Target->InNormalSpace() && (Self->CurrentStanding != aGalaxyStruct::ssPirateMilitary || Target->CurrentStanding != aGalaxyStruct::ssCoalitionMilitary) && (Target->CurrentStanding != aGalaxyStruct::ssPirateMilitary || Self->CurrentStanding != aGalaxyStruct::ssCoalitionMilitary) && Self->CanContactShip(Target)) {
            if (Self->TypeId == aGalaxyStruct::stRanger) {
                pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(2);
            }
            if (aPlayer::GetPlayer() == Target && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1)) {
                if (static_cast<std::uint32_t>(aGalaxy::ReservedMessageCounter) < 7) {
                    return Result;
                }
                aGalaxy::ReservedMessageCounter = 0;
                ++aGalaxy::PlayerDialogueRequestCount;
                if (aGalaxy::PlayerDialogueRequestCount > 1) {
                    return Result;
                }
            }
            if (Self->GetCargoHook() != nullptr && Self->CargoFreeSpace > 20 && aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.4L && Target->HasCargoGoods()) {
                if (Target->virtual_TShip_BuildCargoExtortionResponse(Self, Response)) {
                    Result = true;
                }
                if (aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar && (Result || aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.8L) && (aPlayer::GetPlayer() != Target || aRanger::PlayerAutomaticControl)) {
                    Self->NotifyCargoDemand(Target, Response);
                }
            } else {
                Amount = System::Round(Self->Wealth * 0.033333333333333333335L);
                if (Target->virtual_TShip_BuildMoneyExtortionResponse(Self, Response, Amount)) {
                    Result = true;
                }
                if (aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar && (Result || aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.8L) && (aPlayer::GetPlayer() != Target || aRanger::PlayerAutomaticControl)) {
                    Self->NotifyMoneyDemand(Target, Response, Amount);
                }
            }
        }
        return Result;
    }

    // Cancels reciprocal attacks and pursuers, including wingmen/Tranclucators and missiles. Only the player's TruceShip is set reciprocally.
    void TShip_TruceWithShip(TShip* Self, TShip* Ship) {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        aMissile::TMissile* Missile{};
        TShip* Other{};
        if (aPlayer::GetPlayer() == Ship && aRanger::PendingPlayerFollowTarget == Self || aPlayer::GetPlayer() == Self && aRanger::PendingPlayerFollowTarget == Ship) {
            aRanger::PendingPlayerFollowTarget = nullptr;
        }
        if (Self->EnemyShip == Ship) {
            Self->EnemyShip = nullptr;
        }
        if (Ship->EnemyShip == Self) {
            Ship->EnemyShip = nullptr;
        }
        Self->TruceShip = Ship;
        if (aPlayer::GetPlayer() == Ship) {
            aPlayer::GetPlayer()->TruceShip = Self;
        }
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Self->Weapons[I];
                    if (Weapon->Target == Ship) {
                        Weapon->Target = nullptr;
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Ship->WeaponCount);
            if (1 <= cpp_last_2) {
                for (I = 1; I <= cpp_last_2; ++I) {
                    Weapon = Ship->Weapons[I];
                    if (Weapon->Target == Self) {
                        Weapon->Target = nullptr;
                    }
                }
            }
        }
        if (Self->GetHull()->InterceptorTarget == Ship) {
            Self->GetHull()->InterceptorTarget = nullptr;
        }
        if (Ship->InterceptorSourceShip == Self) {
            Ship->ClearIncomingInterceptors();
        }
        if (Ship->GetHull()->InterceptorTarget == Self) {
            Ship->GetHull()->InterceptorTarget = nullptr;
        }
        if (Self->InterceptorSourceShip == Ship) {
            Self->ClearIncomingInterceptors();
        }
        if (Self->TypeId == aGalaxyStruct::stRanger && Ship->RelationToRanger(Self) < 10) {
            Ship->ChangeRelationToRanger(Self, 10);
        }
        if (Ship->TypeId == aGalaxyStruct::stRanger) {
            Self->ChangeRelationToRanger(Ship, 30);
        }
        if (Self->Order == soFollowShip && pas::checked_cast<TShip*>(Self->OrderTarget) == Ship) {
            Self->OrderNone(false);
            if (aPlayer::GetPlayer() != Self) {
                Self->virtual_TShip_NextDay();
            }
            Self->RefreshDerivedStats(true);
        }
        if (Ship->Order == soFollowShip && pas::checked_cast<TShip*>(Ship->OrderTarget) == Self) {
            Ship->OrderNone(false);
            if (aPlayer::GetPlayer() != Ship) {
                Ship->virtual_TShip_NextDay();
            }
            Ship->RefreshDerivedStats(true);
        }
        Self->ClearCombatStatusSourceReferences(Ship);
        Ship->ClearCombatStatusSourceReferences(Self);
        if (pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) {
            pas::checked_cast<aPirate::TPirate*>(Self)->RaidPressure = 0.0f;
        }
        if (pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr) {
            pas::checked_cast<aPirate::TPirate*>(Ship)->RaidPressure = 0.0f;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Other = pas::list_at<TShip>(Self->CurrentStar->Ships, I);
            if (Other->PartnerShip == Self) {
                aShip::TShip_TruceWithShip(Other, Ship);
            }
            if (Other->PartnerShip == Ship) {
                aShip::TShip_TruceWithShip(Other, Self);
            }
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Other) != nullptr) {
                if (pas::checked_cast<aTranclucator::TTranclucator*>(Other)->OwnerShip == Self) {
                    aShip::TShip_TruceWithShip(Other, Ship);
                }
                if (pas::checked_cast<aTranclucator::TTranclucator*>(Other)->OwnerShip == Ship) {
                    aShip::TShip_TruceWithShip(Other, Self);
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Missiles) - 1); cpp_range_2.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Self->CurrentStar->Missiles, I);
            if (Missile->OwnerShip == Self && Missile->Target == Ship) {
                Missile->Target = nullptr;
            }
            if (Missile->OwnerShip == Ship && Missile->Target == Self) {
                Missile->Target = nullptr;
            }
        }
    }

    // May ask the player or nearby allied NPCs; requires a player.
    void TShip_RequestAlliesAttackShip(TShip* Self, TShip* Target) {
        std::int32_t I{};
        std::int32_t Requests{};
        TShip* Other{};
        pas::WideString Response{};
        if (Target->InNormalSpace()) {
            if (static_cast<long double>(std::max<std::int32_t>(([&] {
                std::int32_t cpp_left = Self->GetRadarRange();
                return cpp_left * Self->GetRadarRange();
            }()), 250000)) >= aMyFunction::PointDistanceSquared(Self->Position, Target->Position)) {
                if (Self->TypeId == aGalaxyStruct::stRanger) {
                    pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(1);
                }
                Requests = 0;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                    Other = pas::list_at<TShip>(Self->CurrentStar->Ships, I);
                    if (Other != Self && Other != Target && Other->InNormalSpace() && static_cast<std::uint8_t>(pas::contains(aConst::NonNegotiatingShipTypes, Other->TypeId) ^ 1) && Other->OrderTarget != Target && (Other->EnemyShip == nullptr || Other->EnemyShip->CurrentStar != Self->CurrentStar) && aShip::TShip_GetRelationLevelToShip(Other, Self) >= aGalaxyStruct::rlGood && (aPlayer::GetPlayer() != Other || aShip::TShip_GetRelationLevelToShip(Self, Other) >= aGalaxyStruct::rlGood) && (aShip::TShip_GetRelationLevelToShip(Other, Target) <= aGalaxyStruct::rlNormal || pas::class_cast_if<aPirate::TPirate*>(Other) != nullptr && pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) && static_cast<std::uint8_t>(pas::in_set<1, 2>(Other->TargetingRestriction) ^ 1) && static_cast<std::uint8_t>(pas::in_set<1, 2>(Target->TargetingRestriction) ^ 1) && Self->CanContactShip(Other) && Other->PartnerShip != Target && Target->PartnerShip != Other && aMyFunction::NextRandomUnitFloat(Self->RandomState) <= 0.9L && (!(pas::class_cast_if<aWarrior::TWarrior*>(Other) != nullptr) || pas::checked_cast<aWarrior::TWarrior*>(Other)->WarriorType != aWarrior::wtFlagship) && (aPlayer::GetPlayer() != Other || Self->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfCoalition || Self->CurrentStar->Status.CustomFaction != u"" || Self->TypeId != aGalaxyStruct::stWarrior || aPlayer::GetPlayer()->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || !(pas::class_cast_if<aNormalShip::TNormalShip*>(Target) != nullptr) || static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::stTransport, aGalaxyStruct::stWarrior>(Target->TypeId) ^ 1) && (Target->TypeId != aGalaxyStruct::stRanger || Target->GetDominantCareer() == aGalaxyStruct::rcPirate)) && (aPlayer::GetPlayer() != Other || Self->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates || Self->CurrentStar->Status.CustomFaction != u"" || Self->TypeId != aGalaxyStruct::stPirate || Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || aPlayer::GetPlayer()->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || !(pas::class_cast_if<aNormalShip::TNormalShip*>(Target) != nullptr) || Target->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate))) {
                        if (aPlayer::GetPlayer() == Other && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1)) {
                            if (static_cast<std::uint32_t>(aGalaxy::ReservedMessageCounter) < 7) {
                                continue;
                            }
                            aGalaxy::ReservedMessageCounter = 0;
                            ++aGalaxy::PlayerDialogueRequestCount;
                            if (aGalaxy::PlayerDialogueRequestCount > 1) {
                                continue;
                            }
                        }
                        if ((Other->virtual_TShip_BuildAttackRequestResponse(Self, Response, Target) || aMyFunction::NextRandomUnitFloat(Self->RandomState) > 0.8L) && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar && (aPlayer::GetPlayer() != Other || aRanger::PlayerAutomaticControl)) {
                            Self->NotifyAttackRequest(Other, Response, Target);
                            break;
                        }
                        ++Requests;
                        if (Requests == 2) {
                            break;
                        }
                    }
                }
            }
        }
    }

    std::uint8_t TShip_IsInPrison(TShip* Self) {
        if (aPlayer::GetPlayer() == Self) {
            return aPlayer::GetPlayer()->InPrison;
        }
        switch (Self->TypeId) {
            case aGalaxyStruct::stRanger: {
                if (static_cast<std::uint32_t>(pas::checked_cast<aRanger::TRanger*>(Self)->PrisonTermRemaining) > 0) {
                    return true;
                }
                break;
            }
            case aGalaxyStruct::stPirate: {
                if (pas::checked_cast<aPirate::TPirate*>(Self)->PrisonTermRemaining > 0) {
                    return true;
                }
                break;
            }
        }
        return false;
    }

    // Rangers and pirates only; other classes return zero.
    std::int32_t TShip::GetPrisonTermRemaining() {
        std::int32_t Result = 0;
        switch (TypeId) {
            case aGalaxyStruct::stRanger: return pas::checked_cast<aRanger::TRanger*>(this)->PrisonTermRemaining;
            case aGalaxyStruct::stPirate: return pas::checked_cast<aPirate::TPirate*>(this)->PrisonTermRemaining;
            default: return Result;
        }
    }

    void TShip::ClearPrisonTerm() {
        switch (TypeId) {
            case aGalaxyStruct::stRanger: pas::checked_cast<aRanger::TRanger*>(this)->PrisonTermRemaining = 0; break;
            case aGalaxyStruct::stPirate: pas::checked_cast<aPirate::TPirate*>(this)->PrisonTermRemaining = 0u; break;
        }
    }

    // Zero without a positive term; otherwise at least 100, using cached Wealth.
    std::int32_t TShip::GetPrisonReleaseCost() {
        std::int32_t Divisor{};
        std::int32_t Result = 0;
        std::int32_t Term = GetPrisonTermRemaining();
        if (Term > 0) {
            Divisor = 141 - Term;
            if (Divisor > 0) {
                Result = System::Round(pas::real_divide(Wealth, Divisor));
            } else {
                Result = Wealth;
            }
            if (Result < 100) {
                return 100;
            }
        }
        return Result;
    }

    // Result is borrowed; raises when no market is available.
    aGalaxyStruct::PGoodsTradePriceEntry TShip::GetLocationGoodsEntry(std::uint8_t Good) {
        if (IsOnPlanet()) {
            return &CurrentPlanet->Goods[Good];
        } else if (DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(DockedTo) != nullptr) {
            return &reinterpret_cast<aRuins::TRuins*>(DockedTo)->ShopGoods[Good];
        } else if (pas::class_cast_if<aRuins::TRuins*>(this) != nullptr) {
            return &reinterpret_cast<aRuins::TRuins*>(this)->ShopGoods[Good];
        } else if (Globals::TalkShip != nullptr) {
            return reinterpret_cast<aGalaxyStruct::PGoodsTradePriceEntry>(&Globals::GoodsShopScreen->TradeRows[Good]);
        } else {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in ShopGoods, ship = ", GetFullName(u" "_wref.get())})));
            if (DockedTo == nullptr) {
                GR_Main::AppendLogLineThreadSafe("FCurShip is nil"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"FCurShip is ", DockedTo->GetFullName(u" "_wref.get()), u" (", static_cast<pas::WideString>(pas::class_name(pas::class_type(DockedTo))), u")"})));
            }
            pas::raise(pas::make_exception<pas::Exception>("Error in TShip.ShopGoods"_a));
        }
    }

    // Location is a planet or station; nil selects the current trade context.
    std::int32_t TShip::ShopGoodsPurchasePrice(std::uint8_t Good, pas::Object* Location) {
        aGalaxyStruct::PGoodsTradePriceEntry Entry = nullptr;
        if (Location == nullptr) {
            Entry = GetLocationGoodsEntry(Good);
        } else if (pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) {
            Entry = &reinterpret_cast<aPlanet::TPlanet*>(Location)->Goods[Good];
        } else if (pas::class_cast_if<aRuins::TRuins*>(Location) != nullptr) {
            Entry = &reinterpret_cast<aRuins::TRuins*>(Location)->ShopGoods[Good];
        }
        if (Entry == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in ShopGoodsPurchasePrice, ship = ", GetFullName(u" "_wref.get())})));
            if (Location == nullptr) {
                GR_Main::AppendLogLineThreadSafe("obj is nil"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"obj is ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Location)))}));
            }
            pas::raise(pas::make_exception<pas::Exception>("Error in TShip.ShopGoodsPurchasePrice"_a));
        }
        return std::max<std::int32_t>(1, Entry->PurchasePrice);
    }

    // Includes the Trading skill bonus. Location=nil selects the current trade context.
    std::int32_t TShip::ShopGoodsSellPrice(std::uint8_t Good, pas::Object* Location) {
        aGalaxyStruct::PGoodsTradePriceEntry Entry = nullptr;
        if (Location == nullptr) {
            Entry = GetLocationGoodsEntry(Good);
        } else if (pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) {
            Entry = &reinterpret_cast<aPlanet::TPlanet*>(Location)->Goods[Good];
        } else if (pas::class_cast_if<aRuins::TRuins*>(Location) != nullptr) {
            Entry = &reinterpret_cast<aRuins::TRuins*>(Location)->ShopGoods[Good];
        }
        if (Entry == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in ShopGoodsSellPrice, ship = ", GetFullName(u" "_wref.get())})));
            if (Location == nullptr) {
                GR_Main::AppendLogLineThreadSafe("obj is nil"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"obj is ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Location)))}));
            }
            pas::raise(pas::make_exception<pas::Exception>("Error in TShip.ShopGoodsSellPrice"_a));
        }
        return std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(aConst::TradingSkillSalePercent[GetEffectiveSkillLevel(psTrading, false) & 0x0000007f] * (Entry->PurchasePrice - Entry->BaseSalePrice) * 0.01L + Entry->BaseSalePrice));
    }

    // Returns 0 for an empty cargo entry.
    double TShip::GetAverageCargoCost(std::uint8_t Good) {
        if (CargoGoods[Good].Count > 0) {
            return pas::real_divide(CargoGoods[Good].TotalCost, CargoGoods[Good].Count);
        }
        return 0.0;
    }

    // Count is not clamped; remaining cost basis uses the previous average price.
    void TShip::ConsumeCargoGoods(std::uint8_t Good, std::int32_t Count) {
        if (CargoGoods[Good].Count == Count) {
            CargoGoods[Good].Count = 0;
            CargoGoods[Good].TotalCost = 0;
        } else {
            CargoGoods[Good].TotalCost -= System::Round(static_cast<long double>(GetAverageCargoCost(Good)) * Count);
            CargoGoods[Good].Count -= Count;
        }
    }

    std::uint8_t TShip::IsCargoGoodIllegalOnCurrentPlanet(std::uint8_t Good) {
        std::uint8_t Result = false;
        if (CurrentPlanet != nullptr && static_cast<std::uint8_t>(CurrentPlanet->IsMainPiratePlanet ^ 1)) {
            if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                return false;
            } else if (!aConst::GoodsLegalOnPlanet[Good][CurrentPlanet->RaceId][CurrentPlanet->Government]) {
                return true;
            } else if (pas::in_range(Good, 0, 1) && IsHealthEffectActive(12)) {
                return true;
            } else {
                return Result;
            }
        }
        return Result;
    }

    // Rejects Count above carried stock; does not reject a negative Count. Player trade losses offset later profit before trade experience is awarded.
    void TShip::SellGoodsToLocation(std::uint8_t Good, std::int32_t Count) {
        std::int32_t PurchasedCount{};
        std::int32_t PurchasedCost{};
        std::int32_t Profit{};
        std::int32_t ProfitableCount{};
        std::int32_t UnitCost{};
        float Experience{};
        float ExperienceFactor{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (CargoGoods[Good].Count < Count) {
            pas::raise(pas::make_exception<pas::Exception>("Error in SaleCurrProduct"_a));
        }
        if (TradeGoodsSold == nullptr) {
            TradeGoodsSold = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
        }
        if (TradeGoodsCostBasis == nullptr) {
            TradeGoodsCostBasis = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
        }
        TradeGoodsSold->Init(static_cast<aConst::TItemType>(Good), Count);
        TradeGoodsCostBasis->Init(static_cast<aConst::TItemType>(Good), std::min<std::int32_t>(Count, CargoGoods[Good].PurchasedCount));
        std::int32_t Value = ShopGoodsSellPrice(Good, nullptr) * Count;
        TradeGoodsSold->Cost = Value;
        if (TradeGoodsCostBasis->Quantity > 0) {
            TradeGoodsCostBasis->Cost = System::Round(pas::real_divide(CargoGoods[Good].PurchasedTotalCost, CargoGoods[Good].PurchasedCount)) * TradeGoodsCostBasis->Quantity;
        }
        std::uint8_t Illegal = IsCargoGoodIllegalOnCurrentPlanet(Good);
        std::int32_t LocationCount = GetLocationGoodsEntry(Good)->Count;
        std::int32_t ShipCount = CargoGoods[Good].Count;
        GetLocationGoodsEntry(Good)->Count += Count;
        SetMoney(Money + Value);
        if (aPlayer::GetPlayer() == this) {
            if (CurrentPlanet != nullptr) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerSellsGoodsToPlanet"_w, nullptr);
                Event->AddData(Good);
                Event->AddData(Count);
                Event->AddData(Value);
                Event->AddData(CurrentPlanet->Id);
            }
            Achievements::TryAddAchievementProgress(u"DEALER"_w, Value);
        }
        ConsumeCargoGoods(Good, Count);
        Illegal = ScriptItemsAct(aConst::satOnShipSellsGoods, TradeGoodsSold, TradeGoodsCostBasis, Illegal) != 0;
        if (GetLocationGoodsEntry(Good)->Count != LocationCount + Count || CargoGoods[Good].Count != ShipCount - Count) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        LocationCount = GetLocationGoodsEntry(Good)->Count;
        ShipCount = CargoGoods[Good].Count;
        if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr) {
            if (Illegal) {
                reinterpret_cast<aRanger::TRanger*>(this)->ApplyIllegalGoodsTradeRelationsPenalty(ShopGoodsSellPrice(Good, nullptr) * Count);
                if (aPlayer::GetPlayer() == this && Count >= 50) {
                    Achievements::TryAddAchievementProgress(u"CONTRABAND"_w, 1);
                }
            }
            if (aPlayer::GetPlayer() != this) {
                reinterpret_cast<aRanger::TRanger*>(this)->AddTraderCareerActivity(2);
            }
            if (aPlayer::GetPlayer() == this && Count > 0 && CargoGoods[Good].PurchasedCount > 0) {
                if (CargoGoods[Good].PurchasedCount < Count) {
                    PurchasedCount = CargoGoods[Good].PurchasedCount;
                } else {
                    PurchasedCount = Count;
                }
                UnitCost = System::Round(pas::real_divide(CargoGoods[Good].PurchasedTotalCost, CargoGoods[Good].PurchasedCount));
                PurchasedCost = System::Round(pas::real_divide(CargoGoods[Good].PurchasedTotalCost, CargoGoods[Good].PurchasedCount) * PurchasedCount);
                Profit = System::Round((ShopGoodsSellPrice(Good, nullptr) - UnitCost) * PurchasedCount);
                if (Profit < 0) {
                    TradeLossBalance -= Profit;
                } else {
                    if (pas::class_cast_if<aNormalShip::TNormalShip*>(this) != nullptr && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && Illegal) {
                        ContrabandProfit += Profit;
                        if (ContrabandProfit >= 3000) {
                            reinterpret_cast<aNormalShip::TNormalShip*>(this)->AddPirateRankPoints(ContrabandProfit / 3000);
                            ContrabandProfit = ContrabandProfit % 3000;
                        }
                    }
                    if (TradeLossBalance > 0) {
                        if (TradeLossBalance > Profit) {
                            TradeLossBalance -= Profit;
                            Profit = 0;
                        } else {
                            Profit -= TradeLossBalance;
                            TradeLossBalance = 0;
                        }
                    }
                    ProfitableCount = MathImports::Floor(pas::real_divide(Profit, ShopGoodsSellPrice(Good, nullptr) - UnitCost));
                    ExperienceFactor = 1.0E-4L * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[1]].GoodsEventDurationFactor * std::max<std::int32_t>(0, pas::checked_cast<aPlayer::TPlayer*>(this)->CareerStatus[aGalaxyStruct::rcTrader] - 50);
                    Experience = ([&] {
                        pas::Extended cpp_right = pas::real_min<pas::Extended>(1.0L, ([&] {
                            pas::Extended cpp_left = ([&] {
                                pas::Extended cpp_left_2 = ShopGoodsSellPrice(Good, nullptr) - UnitCost;
                                return cpp_left_2 * aConst::GoodsMarketBase[Good].TradeExperienceFactor;
                            }());
                            return pas::real_divide(cpp_left, ShopGoodsSellPrice(Good, nullptr));
                        }()));
                        return static_cast<long double>(ProfitableCount) * ExperienceFactor * aConst::GoodsMarketBase[Good].AveragePrice * cpp_right;
                    }());
                    TradeExperience += MathImports::Floor(Experience);
                    if (Profit > 0) {
                        pas::checked_cast<aRanger::TRanger*>(this)->AddTraderCareerActivity(2);
                    }
                }
                CargoGoods[Good].PurchasedTotalCost -= PurchasedCost;
                CargoGoods[Good].PurchasedCount -= PurchasedCount;
            }
        }
        RefreshDerivedStats(true);
        if (GetLocationGoodsEntry(Good)->Count != LocationCount || CargoGoods[Good].Count != ShipCount) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
    }

    // Checks stock and cash, but not free cargo space or negative Count.
    void TShip::BuyGoodsFromLocation(std::uint8_t Good, std::int32_t Count) {
        std::int32_t LocationCount{};
        std::int32_t ShipCount{};
        std::int32_t Value{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        std::uint8_t Illegal{};
        if (GetLocationGoodsEntry(Good)->Count < Count || ([&] {
            std::int32_t cpp_left = ShopGoodsPurchasePrice(Good, nullptr) * Count;
            return cpp_left > Money;
        }())) {
            Dialogs::ShowMessage("\315\345 \342\345\360\355\373\345 \357\340\360\340\354\345\362\360\373 \357\356\352\363\357\352\350"_a);
        } else {
            if (TradeGoodsSold == nullptr) {
                TradeGoodsSold = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
            }
            TradeGoodsSold->Init(static_cast<aConst::TItemType>(Good), Count);
            Illegal = IsCargoGoodIllegalOnCurrentPlanet(Good);
            LocationCount = GetLocationGoodsEntry(Good)->Count;
            ShipCount = CargoGoods[Good].Count;
            GetLocationGoodsEntry(Good)->Count -= Count;
            Value = ShopGoodsPurchasePrice(Good, nullptr) * Count;
            SetMoney(Money - Value);
            CargoGoods[Good].Count += Count;
            CargoGoods[Good].TotalCost += Value;
            Illegal = ScriptItemsAct(aConst::satOnShipBuysGoods, TradeGoodsSold, nullptr, Illegal) != 0;
            if (GetLocationGoodsEntry(Good)->Count != LocationCount - Count || CargoGoods[Good].Count != ShipCount + Count) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
            LocationCount = GetLocationGoodsEntry(Good)->Count;
            ShipCount = CargoGoods[Good].Count;
            if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr) {
                if (Illegal) {
                    reinterpret_cast<aRanger::TRanger*>(this)->ApplyIllegalGoodsTradeRelationsPenalty(ShopGoodsPurchasePrice(Good, nullptr) * Count);
                }
                if (aPlayer::GetPlayer() == this) {
                    if (CurrentPlanet != nullptr) {
                        Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerBuysGoodsFromPlanet"_w, nullptr);
                        Event->AddData(Good);
                        Event->AddData(Count);
                        Event->AddData(Value);
                        Event->AddData(CurrentPlanet->Id);
                    }
                    CargoGoods[Good].PurchasedTotalCost += Value;
                    CargoGoods[Good].PurchasedCount += Count;
                } else {
                    pas::checked_cast<aRanger::TRanger*>(this)->AddTraderCareerActivity(8);
                }
            }
            RefreshDerivedStats(true);
            if (GetLocationGoodsEntry(Good)->Count != LocationCount || CargoGoods[Good].Count != ShipCount) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    // Requires an in-range route index when a group is assigned; may leave the group or issue travel/combat orders.
    void TShip::ProcessLiberationGroupRoute() {
        EC_Struct::TPointF Destination{};
        std::int32_t I{};
        TShip* Ship{};
        aGroup::TGroupRouteOrder RouteOrder{};
        if (LiberationGroup == nullptr) {
            return;
        }
        RouteOrder = pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route[LiberationGroupRouteIndex];
        switch (RouteOrder.Kind) {
            case 3: {
                if (IsOutsideStarSpace()) {
                    return;
                }
                if (CurrentStar == RouteOrder.Target) {
                    ++LiberationGroupRouteIndex;
                    if (LiberationGroupRouteIndex >= pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route.length()) {
                        LeaveLiberationGroup();
                    }
                    ProcessLiberationGroupRoute();
                    return;
                }
                if (Order == soNone || Order == soFollowShip) {
                    OrderJump(pas::checked_cast<aGalaxy::TStar*>(RouteOrder.Target), false);
                }
                break;
            }
            case 2: {
                if (CurrentPlanet == RouteOrder.Target) {
                    ++LiberationGroupRouteIndex;
                    if (LiberationGroupRouteIndex >= pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route.length()) {
                        LeaveLiberationGroup();
                    }
                    return;
                }
                if (IsOutsideStarSpace()) {
                    return;
                }
                if (pas::checked_cast<aPlanet::TPlanet*>(RouteOrder.Target)->CurrentStar != CurrentStar) {
                    return;
                }
                if (static_cast<std::uint8_t>(pas::checked_cast<aPlanet::TPlanet*>(RouteOrder.Target)->IsCoalitionOwned ^ 1) || CurrentStar->Status.Battle != 0) {
                    LeaveLiberationGroup();
                    if (pas::class_cast_if<aWarrior::TWarrior*>(this) != nullptr) {
                        pas::checked_cast<aWarrior::TWarrior*>(this)->AssignWeaponTargetsInStar();
                        pas::checked_cast<aWarrior::TWarrior*>(this)->SelectEnemyShipInStar();
                        pas::checked_cast<aWarrior::TWarrior*>(this)->EngageEnemyShip();
                    }
                    return;
                }
                if (Order == soNone || Order == soFollowShip) {
                    OrderLanding(RouteOrder.Target, false);
                }
                break;
            }
            case 1: {
                if (IsOutsideStarSpace()) {
                    return;
                }
                if (CurrentStar->Status.Battle != 0) {
                    LeaveLiberationGroup();
                    if (pas::class_cast_if<aWarrior::TWarrior*>(this) != nullptr) {
                        pas::checked_cast<aWarrior::TWarrior*>(this)->AssignWeaponTargetsInStar();
                        pas::checked_cast<aWarrior::TWarrior*>(this)->SelectEnemyShipInStar();
                        pas::checked_cast<aWarrior::TWarrior*>(this)->EngageEnemyShip();
                    }
                    return;
                }
                if (aMyFunction::PointDistance(Position, RouteOrder.Destination) < 3.0E+2L && pas::in_range(RouteOrder.WaitMode, 0, 0)) {
                    ++LiberationGroupRouteIndex;
                    if (LiberationGroupRouteIndex >= pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route.length()) {
                        LeaveLiberationGroup();
                    }
                    return;
                }
                if (pas::in_range(RouteOrder.WaitMode, 2, 2) && pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->AreShipsAssembled()) {
                    pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->AdvanceRouteForShips();
                    return;
                }
                if (pas::in_range(RouteOrder.WaitMode, 3, 3) && pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route[LiberationGroupRouteIndex].WaitUntilTurn <= aGalaxy::Galaxy->CurrentTurn) {
                    ++LiberationGroupRouteIndex;
                    if (LiberationGroupRouteIndex >= pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route.length()) {
                        LeaveLiberationGroup();
                    }
                    ProcessLiberationGroupRoute();
                    return;
                }
                if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace() && pas::in_range(RouteOrder.WaitMode, 3, 3) && pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Route[LiberationGroupRouteIndex].WaitUntilTurn > aGalaxy::Galaxy->CurrentTurn + 7) {
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                        Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
                        if (Ship->InNormalSpace() && Ship->LiberationGroup == LiberationGroup && Ship->EnemyShip == nullptr && Ship->TruceShip != EnemyShip && Ship->LiberationGroupRouteIndex == LiberationGroupRouteIndex) {
                            Ship->EnemyShip = EnemyShip;
                        }
                    }
                    pas::checked_cast<aWarrior::TWarrior*>(this)->AssignWeaponTargetsInStar();
                    pas::checked_cast<aWarrior::TWarrior*>(this)->EngageEnemyShip();
                } else {
                    Destination.X = static_cast<long double>(RouteOrder.Destination.X) + aMyFunction::NextRandomFloatRange(-1.0E+2, 1.0E+2, RandomState);
                    Destination.Y = static_cast<long double>(RouteOrder.Destination.Y) + aMyFunction::NextRandomFloatRange(-1.0E+2, 1.0E+2, RandomState);
                    OrderMove(Destination, false);
                }
                break;
            }
        }
    }

    // Requires a current liberation group.
    void TShip::LeaveLiberationGroup() {
        {
            std::int32_t cpp_arg = pas::list_indexof(pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Ships, reinterpret_cast<void*>(this));
            pas::List* ships = pas::checked_cast<aGroup::TGroup*>(LiberationGroup)->Ships;
            pas::list_delete(ships, cpp_arg);
        }
        LiberationGroup = nullptr;
        LiberationGroupRouteIndex = 0;
    }

    aItem::THull* TShip::GetHull() {
        return Hull;
    }

    aItem::TFuelTanks* TShip::GetFuelTanks() {
        return FuelTanks;
    }

    aItem::TEngine* TShip::GetEngine() {
        return Engine;
    }

    aItem::TRadar* TShip::GetRadar() {
        return Radar;
    }

    aItem::TScaner* TShip::GetScanner() {
        return Scanner;
    }

    aItem::TRepairRobot* TShip::GetRepairRobot() {
        return RepairRobot;
    }

    aItem::TCargoHook* TShip::GetCargoHook() {
        return CargoHook;
    }

    aItem::TDefGenerator* TShip::GetDefGenerator() {
        return DefGenerator;
    }

    // Pirate/science bases, licensed station names and the main pirate planet; follows DockedTo recursively.
    std::uint8_t TShip::CanRepairArtefactsAtLocation() {
        if (pas::in_set<7, 7, 9, 9>(TypeId)) {
            return true;
        }
        if (TypeNameOverrideKey != u"" && EC_Str::FindTextOffsetW(TypeNameOverrideKey, u"_licensed"_wref.get(), 0) >= 0) {
            return true;
        }
        if (CurrentPlanet != nullptr && CurrentPlanet->IsMainPiratePlanet) {
            return true;
        }
        if (DockedTo == nullptr) {
            return false;
        }
        return DockedTo->CanRepairArtefactsAtLocation();
    }

    std::uint8_t TShip_CanUseEquipmentTech(TShip* Self, aItem::TEquipment* Item) {
        std::int32_t Level{};
        if (aGalaxy::Galaxy == nullptr || aGalaxy::Galaxy->IsEquipmentKnowledgeUnrestricted()) {
            return true;
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip != nullptr) {
            return aShip::TShip_CanUseEquipmentTech(reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip, Item);
        } else if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr || Item->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
            return true;
        } else {
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                Level = reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->TechLevel;
            } else {
                Level = Item->GetLevel();
            }
            Level -= 4;
            return Self->TechKnowledge >= Level;
        }
    }

    std::uint8_t TShip_CanRepairEquipmentTech(TShip* Self, aItem::TEquipment* Item) {
        std::int32_t Level{};
        if (aGalaxy::Galaxy == nullptr || aGalaxy::Galaxy->IsEquipmentKnowledgeUnrestricted()) {
            return true;
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip != nullptr) {
            return aShip::TShip_CanRepairEquipmentTech(reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip, Item);
        } else if (pas::class_cast_if<aItem::THull*>(Item) != nullptr || pas::class_cast_if<aKling::TKling*>(Self) != nullptr || Item->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiDominator)) {
            return true;
        } else {
            Level = Item->GetLevel();
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                Level = std::max<std::int32_t>(Level, static_cast<std::int32_t>(reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->TechLevel));
            }
            Level -= 2;
            return Self->TechKnowledge >= Level;
        }
    }

    // Does not require EquippedFlag.
    std::uint8_t TShip_IsEquipmentUsable(TShip* Self, aItem::TEquipment* Item) {
        return Item != nullptr && (static_cast<std::uint8_t>(pas::in_set<43, 68, 73, 73>(static_cast<std::uint8_t>(Item->ItemType)) ^ 1) || Item->BrokenFlag == 0) && aShip::TShip_CanUseEquipmentTech(Self, Item);
    }

    // Does not add Item to the inventory.
    void TShip::EquipItem(aItem::TEquipment* Item) {
        if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            if (pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (static_cast<std::uint8_t>(Item->ItemType) - 42) * sizeof(aItem::TEquipment*))) != nullptr) {
                pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (static_cast<std::uint8_t>(Item->ItemType) - 42) * sizeof(aItem::TEquipment*)))->Unequip();
            }
            pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (static_cast<std::uint8_t>(Item->ItemType) - 42) * sizeof(aItem::TEquipment*)), Item);
        } else if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            if (WeaponCount < 5) {
                ++WeaponCount;
            }
            if (Weapons[WeaponCount] != nullptr) {
                Weapons[WeaponCount]->Unequip();
            }
            Weapons[WeaponCount] = pas::checked_cast<aItem::TWeapon*>(Item);
        }
        Item->Equip();
    }

    // Direct types 42..49 require a populated slot; weapon types 50..68 require a valid one-based WeaponIndex. Compacts the weapon cache; does not remove/free inventory.
    void TShip::UnequipSlot(std::uint8_t ItemType, std::int32_t WeaponIndex) {
        std::int32_t I{};
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (ItemType - 42) * sizeof(aItem::TEquipment*)))->Unequip();
            pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (ItemType - 42) * sizeof(aItem::TEquipment*)), nullptr);
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Weapons[WeaponIndex]->Unequip();
            Weapons[WeaponIndex] = nullptr;
            if (WeaponCount > WeaponIndex) {
                {
                    const std::int32_t cpp_first = WeaponIndex;
                    const std::int32_t cpp_last = WeaponCount - 1;
                    if (cpp_first <= cpp_last) {
                        for (I = cpp_first; I <= cpp_last; ++I) {
                            Weapons[I] = Weapons[I + 1];
                        }
                    }
                }
                Weapons[WeaponCount] = nullptr;
            }
            --WeaponCount;
        }
    }

    // Only affects cached installed equipment. Requires non-nil Item.
    void TShip::UnequipItem(aItem::TEquipment* Item) {
        std::int32_t I{};
        if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
                if (Weapons[I] == Item) {
                    UnequipSlot(aConst::t_Weapon1, I);
                    break;
                }
            }
        } else if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator)) && pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (static_cast<std::uint8_t>(Item->ItemType) - 42) * sizeof(aItem::TEquipment*))) == Item) {
            UnequipSlot(static_cast<std::uint8_t>(Item->ItemType), 0);
        }
    }

    // Uses cached CargoFreeSpace; includes cargo, artefact and status modifiers.
    std::int32_t TShip::CalculateMass() {
        std::int32_t I{};
        double Mass = GetHull()->Weight - CargoFreeSpace + GetHull()->CalculateMass();
        if (pas::list_count(Artefacts) > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtefactAntigrav)); cpp_range.next(I); ) {
                Mass = Mass * (aConst::AntigravityArtefactMassFactor + static_cast<long double>(aConst::AntigravityArtefactBoostFactor) * static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtefactAntigrav, nullptr, false)));
            }
        }
        if (PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiMaloc) && IsHealthEffectActive(9)) {
            Mass = Mass * 1.2L;
        }
        std::int32_t Bonus = GetTotalStatBonus(aConst::bonMass);
        if (GetHull()->MicroModuleIndex != 0) {
            Bonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[GetHull()->MicroModuleIndex - 1].StatBonuses, aConst::bonMass * sizeof(std::int32_t)));
        }
        Mass = Mass * (1.0L + pas::real_divide(Bonus, 1.0E+2L));
        return System::Round(Mass);
    }

    // Excludes cargo and unequipped items. Uses the current hull, adding ItemForModule's micromodule mass bonus when supplied.
    std::int32_t TShip::CalculateEquippedMass(aItem::TEquipment* ItemForModule) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        double Mass = GetHull()->CalculateMass();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag != 0) {
                Mass = static_cast<long double>(Mass) + Item->Weight;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
            if (Item->EquippedFlag != 0) {
                Mass = static_cast<long double>(Mass) + Item->Weight;
            }
        }
        std::int32_t Bonus = GetTotalStatBonus(aConst::bonMass);
        if (ItemForModule != nullptr && ItemForModule->SpecialModuleIndex != 0) {
            Bonus += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[ItemForModule->SpecialModuleIndex - 1].StatBonuses, aConst::bonMass * sizeof(std::int32_t)));
        }
        Mass = Mass * (1.0L + pas::real_divide(Bonus, 1.0E+2L));
        if (pas::list_count(Artefacts) > 0) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtefactAntigrav)); cpp_range_3.next(I); ) {
                Mass = Mass * (aConst::AntigravityArtefactMassFactor + static_cast<long double>(aConst::AntigravityArtefactBoostFactor) * static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtefactAntigrav, nullptr, false)));
            }
        }
        return System::Round(Mass);
    }

    // Not clamped to 0..100.
    std::uint8_t TShip::GetHullIntegrityPercent() {
        return System::Round(pas::real_divide(GetHull()->HullPoints, GetHull()->Weight) * 1.0E+2L);
    }

    std::int32_t TShip::GetArmor() {
        std::int32_t cpp_right = System::Round(GetCombatStatusStrength(cseAcid));
        std::int32_t Result = aShip::TShip_CalculateHullArmor(this, GetHull()) - cpp_right;
        return std::max<std::int32_t>(0, Result);
    }

    // Truncated map distance for a jump order; zero otherwise.
    std::int32_t TShip::GetJumpDestinationDistance() {
        if (Order == soJump) {
            return System::Trunc(aMyFunction::PointDistance(CurrentStar->Position, pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Position));
        }
        return 0;
    }

    // Zero without tanks. Uses current planet owner, or owner six off-planet; overfilled tanks can produce a negative cost.
    std::int32_t TShip::GetFullRefuelCost() {
        if (GetFuelTanks() != nullptr) {
            if (CurrentPlanet != nullptr) {
                return System::Round(aShip::CalculateFuelCost(GetFuelTanks()->Capacity - GetFuelTanks()->Fuel, CurrentPlanet->OwnerId));
            }
            return System::Round(aShip::CalculateFuelCost(GetFuelTanks()->Capacity - GetFuelTanks()->Fuel, 6));
        }
        return 0;
    }

    // Uses engine owner and current turn; no engine gives one before artefact modifiers.
    std::int32_t TShip::GetAfterburnerWear() {
        std::int32_t Result{};
        std::int32_t I{};
        float Factor{};
        float Wear{};
        if (GetEngine() != nullptr) {
            Result = aMyFunction::SeededRandomIntRange(aConst::OwnerInfo[GetEngine()->OwnerId].MinimumAfterburnerWear, aConst::OwnerInfo[GetEngine()->OwnerId].MaximumAfterburnerWear, aGalaxy::Galaxy->CurrentTurn);
        } else {
            Result = 1;
        }
        std::int32_t Count = CountActiveArtefacts(aConst::t_ArtForsage);
        if (Count != 0) {
            Factor = aConst::AfterburnerArtefactWearFactor;
            if (CanBoostArtefact(aConst::t_ArtForsage, nullptr, false)) {
                Factor = static_cast<long double>(Factor) + aConst::AfterburnerArtefactBoostWearFactor;
            }
            Wear = Result;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(I); ) {
                Wear = static_cast<long double>(Wear) * Factor;
            }
            return System::Round(Wear);
        }
        return Result;
    }

    std::int32_t TShip::GetFuelLimitedJumpRange() {
        if (GetFuelTanks() == nullptr || static_cast<std::uint8_t>(aShip::TShip_CanUseEquipmentTech(this, GetFuelTanks()) ^ 1) || GetEngine() == nullptr || static_cast<std::uint8_t>(aShip::TShip_CanUseEquipmentTech(this, GetEngine()) ^ 1)) {
            return 0;
        }
        std::int32_t cpp_arg = GetFuelTanks()->Fuel + GetOwnStatBonus(aConst::bonFuel);
        std::int32_t jumpRange = GetJumpRange();
        return std::min<std::int32_t>(cpp_arg, jumpRange);
    }

    // Ignores fuel; broken engines retain 60% range.
    std::int32_t TShip::GetJumpRange() {
        if (GetEngine() == nullptr) {
            return 0;
        }
        if (GetEngine()->BrokenFlag != 0) {
            return System::Round(aShip::TShip_CalculateEngineJumpRange(this, GetEngine()) * 0.6L);
        }
        return aShip::TShip_CalculateEngineJumpRange(this, GetEngine());
    }

    // Zero-based template index; true means disallowed. Includes custom faction, Dominator series and pilot-race restrictions.
    std::uint8_t TShip_IsMicroModuleRaciallyRestricted(TShip* Self, std::int32_t ModuleIndex) {
        static const pas::Set<0, 255> AllSeries = pas::constant_set<pas::Set<0, 255>>({{0, 2}});
        static const pas::Set<0, 255> PlanetOwners = pas::constant_set<pas::Set<0, 255>>({{0, 4}});
        static const pas::Set<0, 255> NoOwners = pas::constant_set<pas::Set<0, 255>>({});
        std::int32_t Position{};
        std::int32_t NameLength{};
        std::uint8_t Result = false;
        if (aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction) {
            if (Self->ScriptShip != nullptr && reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText != u"") {
                Position = pas::pos(reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText, aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions);
                if (Position > 1) {
                    NameLength = reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText.length();
                    if (aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions.read(Position - 1) == u'<' && aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions.length() >= Position + NameLength && aConst::MicroModuleTemplates[ModuleIndex].AllowedCustomHullFactions.read(Position + NameLength) == u'>') {
                        return Result;
                    }
                }
                if (Self->HasIndependentScriptFaction()) {
                    return true;
                }
                if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr && pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask) != static_cast<aGalaxy::TDominatorSeriesMask>(AllSeries)) {
                    return true;
                }
                if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) {
                    return true;
                }
            }
            if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), aGalaxyStruct::oiDominator) && pas::contains(pas::load_unaligned<aGalaxy::TDominatorSeriesMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask), static_cast<std::uint8_t>(static_cast<aKling::TKling*>(Self)->DominatorSeries))) {
                return Result;
            }
            if ((pas::class_cast_if<aNormalShip::TNormalShip*>(Self) != nullptr || pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr) && (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), aConst::RaceToOwner(Self->PilotRace)) || Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), aGalaxyStruct::oiPirate))) {
                return Result;
            }
            if (!(pas::class_cast_if<aKling::TKling*>(Self) != nullptr) && pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask) * static_cast<aGalaxyStruct::TOwnerMask>(PlanetOwners) == static_cast<aGalaxyStruct::TOwnerMask>(NoOwners) && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), Self->OwnerId)) {
                return Result;
            }
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask), aGalaxyStruct::oiUninhabited)) {
                return Result;
            }
            return true;
        }
        return Result;
    }

    std::int32_t TShip_GetEquipmentStatBonus(TShip* Self, std::uint8_t BonusKind, aItem::TEquipment* Item) {
        if (Item->SpecialModuleIndex != 0 && aShip::TShip_IsMicroModuleRaciallyRestricted(Self, Item->SpecialModuleIndex - 1)) {
            return 0;
        }
        return Item->GetStatBonus(static_cast<aConst::TEquipmentBonusKind>(BonusKind));
    }

    std::int32_t TShip::GetTotalStatBonus(std::uint8_t BonusKind) {
        aItem::TEquipment* Item{};
        std::int32_t I{};
        std::int32_t Result = GetOwnStatBonus(BonusKind);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if ((I == 0 || Item->EquippedFlag != 0) && aShip::TShip_IsEquipmentUsable(this, Item)) {
                Result += aShip::TShip_GetEquipmentStatBonus(this, BonusKind, Item);
            }
        }
        if (Artefacts != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
                if (Item->EquippedFlag != 0 && Item->BrokenFlag == 0) {
                    Result += aShip::TShip_GetEquipmentStatBonus(this, BonusKind, Item);
                }
            }
        }
        std::int32_t Strength = System::Round(GetCombatStatusStrength(cseMagnetic));
        if (Strength >= 1) {
            switch (BonusKind) {
                case aConst::bonScan:
                case aConst::bonDef: Result -= Strength; return Result;
                case aConst::bonWRadius: Result -= Strength * 10; return Result;
                case aConst::bonRadar: Result -= Strength * 100; return Result;
                default: return Result;
            }
        }
        return Result;
    }

    std::int32_t TShip::GetRadarRange() {
        std::int32_t Result{};
        if (!aShip::TShip_IsEquipmentUsable(this, GetRadar())) {
            Result = 0;
        } else {
            Result = aShip::TShip_CalculateRadarRange(this, GetRadar());
        }
        if (aPlayer::GetPlayer() == this) {
            if (IsHealthEffectActive(21)) {
                Result *= 2;
            }
            if (aGalaxy::Galaxy->UltraScanModEnabled != 0) {
                return std::max<std::int32_t>(Result, 25000);
            }
        }
        return Result;
    }

    std::int32_t TShip::GetScannerPower() {
        std::int32_t Result = 0;
        if (aShip::TShip_IsEquipmentUsable(this, GetScanner())) {
            return aShip::TShip_CalculateScannerPower(this, GetScanner()) + 12 * (IsHealthEffectActive(21) & 127 & 0x0000007f);
        }
        return Result;
    }

    // Ignores radar range and Dominator scanner series; non-ship targets require only a usable scanner.
    std::uint8_t TShip::CanResolveObjectWithScanner(pas::Object* Target) {
        if (!aShip::TShip_IsEquipmentUsable(this, GetScanner())) {
            return false;
        }
        if (pas::class_cast_if<TShip*>(Target) != nullptr) {
            if (aPlayer::GetPlayer() == this) {
                if (pas::checked_cast<TShip*>(Target)->PartnerShip == aPlayer::GetPlayer() || pas::checked_cast<TShip*>(Target)->TypeId == aGalaxyStruct::stTranclucator || pas::checked_cast<TShip*>(Target) == this) {
                    return true;
                }
            }
            std::int32_t cpp_left = pas::checked_cast<TShip*>(Target)->GetDefensePercent();
            return cpp_left <= GetScannerPower();
        }
        return true;
    }

    void TShip::ApplyRepairDroidHealing() {
        std::int32_t Repair{};
        if (!aShip::TShip_IsEquipmentUsable(this, GetRepairRobot())) {
            return;
        }
        if (TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(this)->IsProgramActive(aGalaxyStruct::prgDisconnection)) {
            return;
        }
        {
            pas::Extended cpp_right = GetCombatStatusStrength(cseDroidBlock);
            if (aMyFunction::NextRandomUnitFloat(RandomState) + 0.01L >= cpp_right) {
                Repair = aShip::TShip_CalculateRepairPoints(this, GetRepairRobot());
                Repair = ScriptItemsAct(aConst::satOnDroidRepair, nullptr, nullptr, Repair);
                if (TypeId != aGalaxyStruct::stKling) {
                    if (CountActiveArtefacts(aConst::t_ArtefactDroid) > 0) {
                        if (CanBoostArtefact(aConst::t_ArtefactDroid, nullptr, false)) {
                            pas::Extended cpp_right_2 = 1.0L + CountActiveArtefacts(aConst::t_ArtefactDroid) * (static_cast<long double>(aConst::DroidArtefactWear) + aConst::DroidArtefactBoostWear);
                            double cpp_arg = aMyFunction::NextRandomUnitFloat(RandomState) * 2.0L * cpp_right_2;
                            aItem::TEquipment* repairRobot = GetRepairRobot();
                            TShip* self = this;
                            aShip::TShip_ApplyItemDegradation(self, repairRobot, idkUse, cpp_arg);
                        } else {
                            pas::Extended cpp_right_3 = 1.0L + static_cast<long double>(CountActiveArtefacts(aConst::t_ArtefactDroid)) * aConst::DroidArtefactWear;
                            double cpp_arg_2 = aMyFunction::NextRandomUnitFloat(RandomState) * 2.0L * cpp_right_3;
                            aItem::TEquipment* repairRobot_2 = GetRepairRobot();
                            TShip* self_2 = this;
                            aShip::TShip_ApplyItemDegradation(self_2, repairRobot_2, idkUse, cpp_arg_2);
                        }
                    } else {
                        double cpp_arg_3 = aMyFunction::NextRandomUnitFloat(RandomState) * 2.0L;
                        aItem::TEquipment* repairRobot_3 = GetRepairRobot();
                        TShip* self_3 = this;
                        aShip::TShip_ApplyItemDegradation(self_3, repairRobot_3, idkUse, cpp_arg_3);
                    }
                }
            } else if (CountActiveArtefacts(aConst::t_ArtefactDroid) > 0) {
                std::int32_t cpp_right_4 = aConst::DroidArtefactRepair + aConst::DroidArtefactBoostRepair * static_cast<std::uint8_t>(CanBoostArtefact(aConst::t_ArtefactDroid, nullptr, false));
                Repair = CountActiveArtefacts(aConst::t_ArtefactDroid) * cpp_right_4;
            } else {
                return;
            }
        }
        if (Repair > 0) {
            GetHull()->HullPoints += std::min<std::int32_t>(Repair, GetHull()->Weight - GetHull()->HullPoints);
        }
    }

    float TShip::GetCargoHookMinPullSpeed() {
        float Result = 0.0f;
        if (GetCargoHook() != nullptr) {
            return pas::real_max<pas::Extended>(0.1L, ([&] {
                std::int32_t cpp_right = aConst::CargoHookArtefactSpeed + aConst::CargoHookArtefactBoostSpeed * static_cast<std::uint8_t>(CanBoostArtefact(aConst::t_ArtefactHook, nullptr, false));
                pas::Extended cpp_left = static_cast<long double>(GetCargoHook()->MinPullSpeed) + CountActiveArtefacts(aConst::t_ArtefactHook) * cpp_right;
                return cpp_left + GetTotalStatBonus(aConst::bonHookMinSpeed);
            }()));
        }
        return Result;
    }

    float TShip::GetCargoHookMaxPullSpeed() {
        float Result = 0.0f;
        if (GetCargoHook() != nullptr) {
            return pas::real_max<pas::Extended>(0.1L, ([&] {
                std::int32_t cpp_right = aConst::CargoHookArtefactSpeed + aConst::CargoHookArtefactBoostSpeed * static_cast<std::uint8_t>(CanBoostArtefact(aConst::t_ArtefactHook, nullptr, false));
                pas::Extended cpp_left = static_cast<long double>(GetCargoHook()->MaxPullSpeed) + CountActiveArtefacts(aConst::t_ArtefactHook) * cpp_right;
                return cpp_left + GetTotalStatBonus(aConst::bonHookMaxSpeed);
            }()));
        }
        return Result;
    }

    std::int32_t TShip::GetCargoHookRange() {
        std::int32_t Result = 0;
        if (GetCargoHook() != nullptr) {
            std::int32_t cpp_right = aConst::CargoHookArtefactRange + aConst::CargoHookArtefactBoostRange * static_cast<std::uint8_t>(CanBoostArtefact(aConst::t_ArtefactHook, nullptr, false));
            std::int32_t cpp_left = GetCargoHook()->Range + CountActiveArtefacts(aConst::t_ArtefactHook) * cpp_right;
            return cpp_left + GetTotalStatBonus(aConst::bonHookRadius);
        }
        return Result;
    }

    std::int32_t TShip::GetCargoHookRangeSquared() {
        return pas::sqr(GetCargoHookRange());
    }

    // Raw PickupPower; zero without a hook. Does not check usability.
    std::int32_t TShip::GetBaseCargoHookPower() {
        if (GetCargoHook() != nullptr) {
            return GetCargoHook()->PickupPower;
        }
        return 0;
    }

    // 1 means no damage reduction.
    double TShip::GetDefenseDamageFactor() {
        if (!aShip::TShip_IsEquipmentUsable(this, GetDefGenerator())) {
            return 1.0;
        }
        return aShip::TShip_CalculateDefGeneratorFactor(this, GetDefGenerator());
    }

    std::uint8_t TShip::GetDefensePercent() {
        return aItem::DefenseDamageFactorToPercent(GetDefenseDamageFactor());
    }

    std::int32_t TShip::GetAttackMultiplier() {
        return std::max<std::int32_t>(0, 1 + GetOwnStatBonus(aConst::bonAttacks));
    }

    // Rejects ID 255 and appends without deduplication; stops when list count equals 255. Extends the visible prefix only when all previous awards were visible.
    void TShip::AddAward(std::uint8_t AwardId) {
        if (AwardId == 255) {
            GR_Main::RaiseWideMessage(u"Error RewardNumber=255"_wref.get());
        }
        if (AwardIds == nullptr) {
            AwardIds = pas::make_object<pas::List>();
        }
        if (pas::list_count(AwardIds) == 255) {
            return;
        }
        if (pas::list_count(AwardIds) == AwardVisibleCount) {
            ++AwardVisibleCount;
        }
        pas::list_add(AwardIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(AwardId))));
        if (aPlayer::GetPlayer() == this) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckAllAwardsAchievement();
        }
    }

    void TShip::RefreshDerivedStats(std::uint8_t UpdateRelativeRatings) {
        RebuildEquipmentCache();
        CargoFreeSpace = GetCargoFreeSpace();
        Speed = CalculateSpeed();
        if (aPlayer::GetPlayer() == this) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckSpeedAchievement();
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckBestEquipmentAchievement();
        }
        JumpRange = GetFuelLimitedJumpRange();
        DefenseDamageFactor = GetDefenseDamageFactor();
        CalculateStrength();
        CalculateWealth();
        if (UpdateRelativeRatings) {
            UpdateBestRangerRelativeRatings();
            UpdateAverageRangerRelativeStrength();
        }
        if (GetEngine() != nullptr) {
            MovementTurnRate = aMyFunction::RemapClamped(Speed, pas::real_divide(aConst::EngineLevelStats[1].Speed, 2.0L), aConst::EngineLevelStats[8].Speed, 1.0, 5.0) * 2.0E+2L * 0.005L;
            MovementSpeed = Speed * 0.005L;
        } else {
            MovementSpeed = 0.0;
        }
        TShip::DerivedStateCompatibilityHook();
    }

    // Requires Graphic; chooses dimensions from ship class, hull and special equipment.
    void TShip::RefreshGraphicSize() {
        std::int32_t Small{};
        std::int32_t Large{};
        std::uint8_t Kind{};
        aGalaxyStruct::TDominatorSeries Series{};
        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Graphic) != nullptr && reinterpret_cast<SE_Ship2::TShip2SE*>(Graphic)->SmallSize > 0 && reinterpret_cast<SE_Ship2::TShip2SE*>(Graphic)->LargeSize > 0) {
            Small = reinterpret_cast<SE_Ship2::TShip2SE*>(Graphic)->SmallSize;
            Large = reinterpret_cast<SE_Ship2::TShip2SE*>(Graphic)->LargeSize;
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(this) != nullptr) {
            Small = TranclucatorSmallSize;
            Large = TranclucatorLargeSize;
        } else if (ScriptChameleon) {
            Small = DefaultShipSmallSize;
            Large = DefaultShipLargeSize;
        } else if (GetHull()->HullType == aGalaxyStruct::htSpecial && static_cast<std::uint8_t>(ChameleonActive ^ 1)) {
            Small = SpecialHullSmallSize;
            Large = SpecialHullLargeSize;
        } else if (ChameleonActive || pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
            if (ChameleonActive) {
                Kind = ChameleonVisualType;
                Series = ChameleonSeries;
            } else {
                Kind = pas::checked_cast<aKling::TKling*>(this)->KlingType;
                Series = pas::checked_cast<aKling::TKling*>(this)->DominatorSeries;
            }
            Small = DominatorShipSmallSizes[Series][Kind];
            Large = DominatorShipLargeSizes[Series][Kind];
        } else if (pas::class_cast_if<aRuins::TRuins*>(this) != nullptr) {
            Small = StationSize;
            Large = StationSize;
        } else if (pas::class_cast_if<aPirate::TPirate*>(this) != nullptr && OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && static_cast<aPirate::TPirate*>(this)->PirateType != 0) {
            Small = PirateClanSmallSizes[GetHull()->OwnerId];
            Large = PirateClanLargeSizes[GetHull()->OwnerId];
        } else if (pas::class_cast_if<aWarrior::TWarrior*>(this) != nullptr && static_cast<aWarrior::TWarrior*>(this)->WarriorType == aWarrior::wtFlagship) {
            Small = BigWarriorSmallSizes[GetHull()->OwnerId];
            Large = BigWarriorLargeSizes[GetHull()->OwnerId];
        } else {
            std::uint8_t cpp_case = GetHull()->HullType;
            if (cpp_case == aGalaxyStruct::htRanger) {
                Small = RangerSmallSizes[GetHull()->OwnerId];
                Large = RangerLargeSizes[GetHull()->OwnerId];
            } else if (cpp_case >= aGalaxyStruct::htTransport && cpp_case <= aGalaxyStruct::htDiplomat) {
                Small = TransportSmallSizes[GetHull()->HullType][GetHull()->OwnerId];
                Large = TransportLargeSizes[GetHull()->HullType][GetHull()->OwnerId];
            } else if (cpp_case == aGalaxyStruct::htPirate) {
                Small = PirateSmallSizes[GetHull()->OwnerId];
                Large = PirateLargeSizes[GetHull()->OwnerId];
            } else if (cpp_case == aGalaxyStruct::htWarrior) {
                Small = WarriorSmallSizes[GetHull()->OwnerId];
                Large = WarriorLargeSizes[GetHull()->OwnerId];
            } else {
                Small = DefaultShipSmallSize;
                Large = DefaultShipLargeSize;
            }
        }
        if (GetHull()->HullType == aGalaxyStruct::htSpecial && GetHull()->SpecialModuleIndex != 0 && static_cast<std::uint8_t>(ChameleonActive ^ 1) && static_cast<std::uint8_t>(ScriptChameleon ^ 1)) {
            Small = System::Round(Small * aConst::MicroModuleTemplates[GetHull()->SpecialModuleIndex - 1].HullGraphSizePercent * 0.01L);
            Large = System::Round(Large * aConst::MicroModuleTemplates[GetHull()->SpecialModuleIndex - 1].HullGraphSizePercent * 0.01L);
        }
        pas::Extended cpp_left = System::Ln(pas::real_max<pas::Extended>(1.0L, pas::real_divide(GetHull()->Weight, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]))) * (Large - Small);
        std::int32_t Size = System::Round(pas::real_divide(cpp_left, System::Ln(pas::real_divide(2.0L * aConst::EquipmentSizeFactors[1], aConst::EquipmentSizeFactors[5]))) + Small);
        if (GR_Main::GiResourceVariant() == 1) {
            Size = System::Round(Size * 0.78125L);
        }
        Graphic->SetSize(ClassesImports::Point(Size, Size));
    }

    // Can unequip items whose slots are unavailable.
    void TShip::RebuildEquipmentCache() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::uint8_t Kind{};
        Hull = pas::list_at<aItem::THull>(Inventory, 0);
        for (Kind = static_cast<std::uint8_t>(43); Kind <= static_cast<std::uint8_t>(49); ++Kind) {
            pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (Kind - 42) * sizeof(aItem::TEquipment*)), nullptr);
        }
        for (I = 1; I <= 5; ++I) {
            Weapons[I] = nullptr;
        }
        WeaponCount = 0;
        UsableWeaponCount = 0;
        HasInactiveDirectEquipment = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag != 0 && static_cast<std::uint8_t>(pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_Engine)) ^ 1) && GetSlotCountForItemType(static_cast<std::uint8_t>(Item->ItemType)) <= 0 && aConst::ItemTypeToSlotKind(static_cast<std::uint8_t>(Item->ItemType)) != aConst::sskUnsupported) {
                Item->EquippedFlag = 0;
            }
            if (Item->EquippedFlag != 0) {
                if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
                    pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (static_cast<std::uint8_t>(Item->ItemType) - 42) * sizeof(aItem::TEquipment*)), Item);
                } else if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    ++WeaponCount;
                    Weapons[WeaponCount] = pas::checked_cast<aItem::TWeapon*>(Item);
                    if (aShip::TShip_IsEquipmentUsable(this, Weapons[WeaponCount])) {
                        ++UsableWeaponCount;
                    }
                }
            } else if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
                HasInactiveDirectEquipment = 1;
            }
        }
        RemoveInvalidPickupTargets();
    }

    // Can be negative.
    std::int32_t TShip::GetCargoFreeSpace() {
        return GetHull()->Weight - GetCarriedItemWeight() - GetCargoGoodsWeight();
    }

    // Excludes the hull.
    std::int32_t TShip::GetCarriedItemWeight() {
        aItem::TItem* Item{};
        std::int32_t I{};
        std::int32_t Weight = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            Weight += Item->Weight;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Artefacts, I);
            Weight += Item->Weight;
        }
        return Weight;
    }

    std::int32_t TShip::GetCargoGoodsWeight() {
        std::uint8_t Good{};
        std::int32_t Result = 0;
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            Result += CargoGoods[Good].Count;
        }
        return Result;
    }

    // Requires a follow order; uses weapon ranges or the ships' collision radii.
    std::int32_t TShip::CalculateFollowRadius() {
        std::int32_t Result{};
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        if (Order != soFollowShip) {
            pas::raise(pas::make_exception<pas::Exception>("TShip.CalcFollowRadius()"_a));
        }
        TShip* Target = pas::checked_cast<TShip*>(OrderTarget);
        std::uint8_t Mode = OrderStateData;
        switch (Mode) {
            case 1: {
                Result = 999999;
                {
                    const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last) {
                        for (I = 1; I <= cpp_last; ++I) {
                            Weapon = Weapons[I];
                            if (aShip::TShip_IsEquipmentUsable(this, Weapon) && aShip::TShip_GetWeaponRange(this, Weapon) < Result) {
                                Result = aShip::TShip_GetWeaponRange(this, Weapon);
                            }
                        }
                    }
                }
                break;
            }
            case 2: {
                Result = 0;
                {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (I = 1; I <= cpp_last_2; ++I) {
                            Weapon = Weapons[I];
                            if (aShip::TShip_IsEquipmentUsable(this, Weapon) && aShip::TShip_GetWeaponRange(this, Weapon) > Result) {
                                Result = aShip::TShip_GetWeaponRange(this, Weapon);
                            }
                        }
                    }
                }
                break;
            }
            default: Result = 999999; break;
        }
        if (Result > 0 && Result < 999999) {
            return System::Round(Result * 0.85L);
        }
        return System::Trunc(static_cast<long double>(CollisionRadius) + Target->CollisionRadius) + 15;
    }

    // Raises when the current order is not follow.
    std::uint8_t TShip::GetFollowMode() {
        if (Order != soFollowShip) {
            pas::raise(pas::make_exception<pas::Exception>("TShip.CalcFollowRadius()"_a));
        }
        return OrderStateData;
    }

    // Requires a follow order; applies tactical and map-edge adjustments without modifying OrderStateData.
    std::uint8_t TShip::GetEffectiveFollowMode() {
        if (Order != soFollowShip) {
            pas::raise(pas::make_exception<pas::Exception>("TShip.GetRealFollowType()"_a));
        }
        std::uint8_t Result = 0;
        std::uint8_t Mode = OrderStateData;
        if (pas::in_set<0, 0, 3, 3>(Mode) || WeaponCount <= 0) {
            return Result;
        }
        if (aPlayer::GetPlayer() != this && aMyFunction::SeededRandomIntRange(0, 6, Seed + aGalaxy::Galaxy->CurrentTurn) == 0) {
            return Result;
        }
        float BoundarySquared = pas::sqr(pas::real_divide(CurrentStar->MapDiameter, 2.0L));
        float DistanceSquared = pas::sqr(static_cast<pas::Extended>(Position.X)) + pas::sqr(static_cast<pas::Extended>(Position.Y));
        float TargetDistanceSquared = pas::sqr(static_cast<pas::Extended>(reinterpret_cast<TShip*>(OrderTarget)->Position.X)) + pas::sqr(static_cast<pas::Extended>(reinterpret_cast<TShip*>(OrderTarget)->Position.Y));
        if (DistanceSquared > BoundarySquared && TargetDistanceSquared > DistanceSquared) {
            return Result;
        }
        return Mode;
    }

    std::uint8_t TShip::NeedsEquipmentType(aConst::TItemType ItemType) {
        std::uint8_t Result = false;
        if ((pas::in_set<43, 44, 47, 47, 49, 49>(static_cast<std::uint8_t>(ItemType)) || pas::in_range(static_cast<std::uint8_t>(ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) && CountCarriedEquipmentByType(ItemType) <= 0) {
            return true;
        }
        return Result;
    }

    // Includes equipped artefacts.
    std::int32_t TShip::CalculateEquippedItemCostWithoutHull() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag != 0) {
                Result += Item->Cost;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
            if (Artefact->EquippedFlag != 0) {
                Result += Artefact->Cost;
            }
        }
        return Result;
    }

    // Skips inventory index zero; any weapon request counts all weapon types.
    std::int32_t TShip::CountCarriedEquipmentByType(aConst::TItemType ItemType) {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Result = 0;
        if (pas::in_range(static_cast<std::uint8_t>(ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Inventory, I);
                if (Item->ItemType == ItemType || pas::in_range(static_cast<std::uint8_t>(ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    ++Result;
                }
            }
        }
        return Result;
    }

    aItem::TWeapon* TShip::SelectBestUnequippedWeapon() {
        std::int32_t Index{};
        aItem::TEquipment* Candidate{};
        aItem::TWeapon* Weapon{};
        std::uint8_t CandidateProtected{};
        std::uint8_t BestProtected{};
        std::uint8_t PriceMode{};
        aItem::TWeapon* Result = nullptr;
        if (IsDocked()) {
            PriceMode = 3;
        } else {
            PriceMode = 0;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(Index); ) {
            Candidate = pas::list_at<aItem::TEquipment>(Inventory, Index);
            if (pas::in_range(Candidate->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && Candidate->EquippedFlag == 0) {
                if (Result == nullptr) {
                    Result = pas::checked_cast<aItem::TWeapon*>(Candidate);
                } else {
                    CandidateProtected = Candidate->NoDropFlag > 0 || Candidate->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Candidate->ScriptItem)->Name != u"";
                    BestProtected = Result->NoDropFlag > 0 || Result->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Result->ScriptItem)->Name != u"";
                    if (CandidateProtected && static_cast<std::uint8_t>(BestProtected ^ 1)) {
                        Result = pas::checked_cast<aItem::TWeapon*>(Candidate);
                    } else if (BestProtected == false || CandidateProtected != false) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(Candidate);
                        if (Weapon->Weight <= GetHull()->Weight * 0.25L) {
                            pas::Extended cpp_left = EvaluateItem(Result, PriceMode);
                            if (cpp_left < EvaluateItem(Weapon, PriceMode)) {
                                Result = Weapon;
                            }
                        }
                    }
                }
            }
        }
        return Result;
    }

    std::uint8_t TShip::HasLooseNonScriptItemsOrGoods() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag == 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"")) {
                return true;
            }
        }
        if (HasCargoGoods()) {
            return true;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
            if (Artefact->EquippedFlag == 0 && (Artefact->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Artefact->ScriptItem)->Name == u"")) {
                return true;
            }
        }
        return false;
    }

    // Preserves named script items; drops through the normal item/artefact helpers.
    void TShip::DropUnequippedItemsAndGoods() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        {
            const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
            if (cpp_first >= 1) {
                for (I = cpp_first; I >= 1; --I) {
                    Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (Item->EquippedFlag == 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"")) {
                        DropCarriedItemAsMovingLoot(Item);
                    }
                }
            }
        }
        if (HasCargoGoods()) {
            aShip::TShip_DropAllCargoGoods(this);
        }
        {
            const std::int32_t cpp_first_2 = pas::list_count(Artefacts) - 1;
            if (cpp_first_2 >= 0) {
                for (I = cpp_first_2; I >= 0; --I) {
                    Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                    if (Artefact->EquippedFlag == 0 && (Artefact->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Artefact->ScriptItem)->Name == u"")) {
                        DropCarriedArtefactAsMovingLoot(Artefact);
                    }
                }
            }
        }
    }

    // Always adds to the player's storage, even when Self is an NPC. Caller must detach the item from its previous owner. Negative Slot allocates a free slot.
    void TShip::AddItemToPlayerStorage(aItem::TItem* Item, pas::Object* Location, std::int32_t Slot) {
        aPlayer::PStorageEntry Entry{};
        pas::new_value(Entry);
        pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
        Entry->LocationOwner = Location;
        Entry->Item = Item;
        if (Slot >= 0) {
            if (aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(Location, Slot) >= 0) {
                aPlayer::GetPlayer()->ShiftStorageSlotsAtOrAfter(Location, Slot);
            }
            Entry->SlotIndex = Slot;
        } else {
            Entry->SlotIndex = -1;
            Entry->SlotIndex = aPlayer::GetPlayer()->FindNextStorageSlot(Entry->LocationOwner);
        }
        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
            aScript::RunItemConfigActionCode(Item, aConst::satOnMovingItemToStorage, nullptr, Item, Location, 0);
        }
        if (Item->ScriptItem != nullptr) {
            reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnMovingItemToStorage, nullptr, Item, Location, 0);
        }
        ScriptItemsAct(aConst::satOnMovingItemToStorage, Item, Location, 0);
    }

    // Transfers ownership; a successful goods/countable merge frees Item.
    void TShip::MergeItemIntoPlayerStorage(aItem::TItem* Item, pas::Object* Location, std::int32_t Slot) {
        std::int32_t Index{};
        std::int32_t Occupant{};
        aPlayer::PStorageEntry Entry{};
        if (pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr) {
            Index = aPlayer::GetPlayer()->FindMergeableStorageItemByLocation(Location, reinterpret_cast<aItem::TCountableItem*>(Item));
            if (Index >= 0) {
                Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index);
                aItem::TCountableItem_Merge(reinterpret_cast<aItem::TCountableItem*>(Entry->Item), Item);
                if (Slot >= 0) {
                    Occupant = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(Location, Slot);
                    if (Occupant >= 0 && Occupant != Index) {
                        aPlayer::GetPlayer()->ShiftStorageSlotsAtOrAfter(Location, Slot);
                    }
                    Entry->SlotIndex = Slot;
                }
                pas::free(Item);
                return;
            }
        } else if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
            Index = aPlayer::GetPlayer()->FindStorageGoodsByLocationAndType(Location, static_cast<std::uint8_t>(Item->ItemType));
            if (Index >= 0) {
                Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Quantity += reinterpret_cast<aItem::TGoods*>(Item)->Quantity;
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Weight += Item->Weight;
                {
                    std::int64_t cpp_step = System::Round(Item->Cost);
                    std::int32_t& cpp_target = pas::checked_cast<aItem::TGoods*>(Entry->Item)->Cost;
                    cpp_target += cpp_step;
                }
                if (Slot >= 0) {
                    Occupant = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(Location, Slot);
                    if (Occupant >= 0 && Occupant != Index) {
                        aPlayer::GetPlayer()->ShiftStorageSlotsAtOrAfter(Location, Slot);
                    }
                    Entry->SlotIndex = Slot;
                }
                pas::free(Item);
                return;
            }
        }
        AddItemToPlayerStorage(Item, Location, Slot);
    }

    // Positive quantities only; Cost is the total cost basis, not a unit price.
    void TShip::AddGoodsToPlayerStorage(std::uint8_t Good, std::int32_t Quantity, std::int32_t Cost, pas::Object* Location, std::int32_t Slot) {
        std::int32_t Index{};
        std::int32_t Occupant{};
        aPlayer::PStorageEntry Entry{};
        if (Quantity > 0) {
            Index = aPlayer::GetPlayer()->FindStorageGoodsByLocationAndType(Location, Good);
            if (Index >= 0) {
                Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, Index);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Quantity += Quantity;
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Weight += Quantity;
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Cost += Cost;
                if (Slot >= 0) {
                    Occupant = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(Location, Slot);
                    if (Occupant >= 0 && Occupant != Index) {
                        aPlayer::GetPlayer()->ShiftStorageSlotsAtOrAfter(Location, Slot);
                    }
                    Entry->SlotIndex = Slot;
                }
            } else {
                pas::new_value(Entry);
                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Entry));
                Entry->Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                pas::checked_cast<aItem::TGoods*>(Entry->Item)->Init(static_cast<aConst::TItemType>(Good), Quantity);
                Entry->Item->Cost = Cost;
                Entry->LocationOwner = Location;
                if (Slot >= 0) {
                    Occupant = aPlayer::GetPlayer()->FindStorageIndexByLocationAndSlot(Location, Slot);
                    if (Occupant >= 0) {
                        aPlayer::GetPlayer()->ShiftStorageSlotsAtOrAfter(Location, Slot);
                    }
                    Entry->SlotIndex = Slot;
                } else {
                    Entry->SlotIndex = aPlayer::GetPlayer()->FindNextStorageSlot(Entry->LocationOwner);
                    Entry->SlotIndex = aPlayer::GetPlayer()->FindNextStorageSlot(Entry->LocationOwner);
                }
                ScriptItemsAct(aConst::satOnMovingItemToStorage, Entry->Item, Location, 0);
            }
        }
    }

    // Uses the player's storage filters. Returns true for an eligible location even if nothing was moved.
    std::uint8_t TShip::StoreLooseInventoryAt(pas::Object* Location) {
        std::int32_t I{};
        aConst::TItemType Good{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        aItem::TGoods* Goods{};
        std::uint8_t Result = false;
        if (Location != nullptr) {
            if (!(pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) || pas::in_set<0, 4, 7, 7>(pas::checked_cast<aPlanet::TPlanet*>(Location)->OwnerId)) {
                {
                    const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
                    if (cpp_first >= 0) {
                        for (I = cpp_first; I >= 0; --I) {
                            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                            if (Item->NoDropFlag <= 0 && Item->EquippedFlag == 0 && Item->ItemType != aConst::t_Hull && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Item))) {
                                MergeItemIntoPlayerStorage(Item, Location, -1);
                                pas::list_delete(Inventory, I);
                            }
                        }
                    }
                }
                {
                    const std::int32_t cpp_first_2 = pas::list_count(Artefacts) - 1;
                    if (cpp_first_2 >= 0) {
                        for (I = cpp_first_2; I >= 0; --I) {
                            Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                            if (Artefact->NoDropFlag <= 0 && Artefact->EquippedFlag == 0 && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Artefact))) {
                                AddItemToPlayerStorage(Artefact, Location, -1);
                                pas::list_delete(Artefacts, I);
                            }
                        }
                    }
                }
                for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_Narcotics); cpp_range.next(Good); ) {
                    if (CargoGoods[Good].Count > 0 && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessHoldGoods(static_cast<std::uint8_t>(Good)))) {
                        Goods = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                        Goods->Init(Good, CargoGoods[Good].Count);
                        Goods->Cost = CargoGoods[Good].TotalCost;
                        MergeItemIntoPlayerStorage(Goods, Location, -1);
                        CargoGoods[Good].Count = 0;
                        CargoGoods[Good].TotalCost = 0;
                    }
                }
                return true;
            }
        }
        return Result;
    }

    // Transfers matching player storage entries into Self; returns location eligibility, not whether items were retrieved.
    std::uint8_t TShip::RetrieveStoredItems(pas::Object* Location) {
        std::int32_t I{};
        std::int32_t J{};
        aItem::TItem* Item{};
        aPlayer::PStorageEntry Entry{};
        aItem::TCountableItem* Stack{};
        std::uint8_t Result = false;
        if (Location != nullptr) {
            if (!(pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) || pas::in_set<0, 4, 7, 7>(pas::checked_cast<aPlanet::TPlanet*>(Location)->OwnerId)) {
                {
                    const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1;
                    if (cpp_first >= 0) {
                        for (I = cpp_first; I >= 0; --I) {
                            Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
                            if (Entry->LocationOwner != Location || Entry->Item->ItemType == aConst::t_Hull) {
                                continue;
                            }
                            if (!(static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessStoredItem(Entry->Item))) {
                                continue;
                            }
                            if (pas::class_cast_if<aItem::TEquipment*>(Entry->Item) != nullptr && pas::checked_cast<aItem::TEquipment*>(Entry->Item)->EquippedFlag != 0) {
                                pas::checked_cast<aItem::TEquipment*>(Entry->Item)->EquippedFlag = 0;
                            }
                            if (pas::class_cast_if<aItem::TArtefact*>(Entry->Item) != nullptr) {
                                pas::list_add(Artefacts, reinterpret_cast<void*>(Entry->Item));
                            } else if (pas::class_cast_if<aItem::TCountableItem*>(Entry->Item) != nullptr) {
                                Stack = nullptr;
                                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(J); ) {
                                    Item = pas::list_at<aItem::TItem>(Inventory, J);
                                    if (aItem::TCountableItem_CanMerge(pas::checked_cast<aItem::TCountableItem*>(Entry->Item), Item)) {
                                        Stack = pas::checked_cast<aItem::TCountableItem*>(Item);
                                        break;
                                    }
                                }
                                if (Stack != nullptr) {
                                    aItem::TCountableItem_Merge(Stack, pas::checked_cast<aItem::TCountableItem*>(Entry->Item));
                                    pas::free(Entry->Item);
                                } else {
                                    pas::list_add(Inventory, reinterpret_cast<void*>(Entry->Item));
                                }
                            } else if (pas::class_cast_if<aItem::TGoods*>(Entry->Item) != nullptr) {
                                CargoGoods[Entry->Item->ItemType].Count += reinterpret_cast<aItem::TGoods*>(Entry->Item)->Quantity;
                                CargoGoods[Entry->Item->ItemType].TotalCost += Entry->Item->Cost;
                                pas::free(Entry->Item);
                            } else if (pas::class_cast_if<aItem::TEquipment*>(Entry->Item) != nullptr) {
                                pas::list_add(Inventory, reinterpret_cast<void*>(Entry->Item));
                            }
                            pas::list_delete(aPlayer::GetPlayer()->StorageEntries, I);
                            pas::dispose(Entry);
                        }
                    }
                }
                return true;
            }
        }
        return Result;
    }

    void TShip::AutoEquipArtefacts() {
        static const pas::Set<0, 255> EnergyFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy}});
        static const pas::Set<0, 255> SplinterFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkSplinter}});
        std::int32_t RemainingSlots{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        // Caller-popped static link; remaining slots -4, ship -8.
        auto EquipType = [&](aConst::TItemType ArtefactType) -> void {
            std::int32_t I{};
            float Score{};
            float BestScore{};
            aItem::TEquipment* Candidate{};
            aItem::TEquipment* Best{};
            if (RemainingSlots == 0) {
                return;
            }
            while (RemainingSlots > 0) {
                Best = nullptr;
                BestScore = 0.0f;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->Artefacts) - 1); cpp_range.next(I); ) {
                    Candidate = pas::list_at<aItem::TEquipment>(this->Artefacts, I);
                    if (Candidate->EquippedFlag == 0 && (Candidate->ItemType == ArtefactType || pas::in_range(static_cast<std::uint8_t>(Candidate->ItemType), static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) && reinterpret_cast<aItem::TArtefactCustom*>(Candidate)->CountsAsItemType == ArtefactType) && (static_cast<std::uint8_t>(HasEquippedArtefactOfSameUseGroup(Candidate) ^ 1) || aGalaxy::Galaxy->AreDuplicateArtefactsEnabled())) {
                        Score = EvaluateItem(Candidate, 3);
                        if (Best == nullptr || Score > BestScore) {
                            Best = Candidate;
                            BestScore = Score;
                        }
                    }
                }
                if (Best == nullptr) {
                    break;
                }
                Best->Equip();
                --RemainingSlots;
            }
        };
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
            Item->Unequip();
        }
        RemainingSlots = GetSlotCountForItemType(aConst::t_Artefact);
        if (RemainingSlots <= 0) {
            return;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
            if (pas::in_set<8, 22, 26, 34, 36, 41>(static_cast<std::uint8_t>(Item->ItemType)) && (Item->NoDropFlag > 0 || Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name != u"")) {
                Item->Equip();
                --RemainingSlots;
                if (RemainingSlots <= 0) {
                    return;
                }
            }
        }
        std::int32_t EnergyCount = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(EnergyFlags)) & 0x0000007f;
        std::int32_t SplinterCount = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(SplinterFlags)) & 0x0000007f;
        std::uint8_t HasEnergy = EnergyCount > 0;
        std::uint8_t HasSplinter = SplinterCount > 0;
        std::uint8_t HasMissiles = false;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (pas::in_range(static_cast<std::uint8_t>(Weapons[I]->GetWeaponInfo()->ShotType), static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        HasMissiles = true;
                        break;
                    }
                }
            }
        }
        I = 4;
        while (RemainingSlots > 0 && I > 0) {
            EquipType(aConst::t_ArtefactAntigrav);
            EquipType(aConst::t_ArtefactSpeed);
            EquipType(aConst::t_ArtefactHull);
            EquipType(aConst::t_ArtMissileDef);
            EquipType(aConst::t_ArtEnergyDef);
            EquipType(aConst::t_ArtPDTurret);
            if (HasEnergy) {
                EquipType(aConst::t_ArtEnergyPulse);
            }
            if (HasSplinter) {
                EquipType(aConst::t_ArtSplinter);
            }
            if (HasMissiles) {
                EquipType(aConst::t_ArtFastRacks);
            }
            if (GetDefGenerator() != nullptr) {
                EquipType(aConst::t_ArtefactDef);
            }
            if (GetRepairRobot() != nullptr) {
                EquipType(aConst::t_ArtefactDroid);
            }
            if (HasSplinter) {
                EquipType(aConst::t_ArtDecelerate);
            }
            if (HasEnergy & (GetDefGenerator() != nullptr)) {
                EquipType(aConst::t_ArtDefToEnergy);
            }
            if (GetCargoHook() != nullptr && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTransport, aGalaxyStruct::stWarrior>(TypeId) ^ 1)) {
                EquipType(aConst::t_ArtefactHook);
            }
            if (!pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTransport, aGalaxyStruct::stWarrior>(TypeId)) {
                EquipType(aConst::t_ArtefactMiniExpl);
            }
            EquipType(aConst::t_ArtefactNano);
            EquipType(aConst::t_ArtefactPower);
            EquipType(aConst::t_ArtefactRadar);
            EquipType(aConst::t_ArtefactScaner);
            EquipType(aConst::t_ArtefactFuel);
            EquipType(aConst::t_ArtBio);
            if (GetSlotCount(aConst::sskWeapon) < 5) {
                EquipType(aConst::t_ArtDefToArms1);
            }
            if (GetSlotCount(aConst::sskDefGenerator) > 0 && GetSlotCount(aConst::sskWeapon) < 4) {
                EquipType(aConst::t_ArtDefToArms2);
            }
            if (GetSlotCount(aConst::sskWeapon) > WeaponCount) {
                EquipType(aConst::t_ArtWeaponToSpeed);
            }
            if (GetSlotCount(aConst::sskAfterburner) > 0 && pas::in_set<aGalaxyStruct::stRanger, aGalaxyStruct::stRanger, aGalaxyStruct::stPirate, aGalaxyStruct::stWarrior>(TypeId)) {
                EquipType(aConst::t_ArtForsage);
            }
            if (GetEngine() != nullptr && GetEngine()->JumpRange * 1.2L < aConst::HyperJumpArtefactRange) {
                EquipType(aConst::t_ArtGiperJump);
            }
            --I;
        }
        RefreshDerivedStats(true);
    }

    // Temporarily changes weapon installation and disables ChaoticRandom while evaluating equipment; restores them on the normal path.
    float TShip::CalculateItemEffectiveness(aItem::TItem* Item) {
        aItem::THull* CandidateHull{};
        std::int32_t WeaponIndex{};
        std::int32_t I{};
        float Damage{};
        float WeaponRange{};
        float OtherRange{};
        float MinRange{};
        float DamageValue{};
        aItem::TWeapon* Weapon{};
        std::uint8_t DamageBonusKind{};
        pas::Array<aItem::TWeapon*, 1, 5> SavedWeapons{};
        float Result = 0.0f;
        if (CurrentPlanet == nullptr && DockedTo == nullptr && pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr && static_cast<aItem::TEquipment*>(Item)->BrokenFlag != 0 && Item->ItemType != aConst::t_Engine) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr && static_cast<std::uint8_t>(aShip::TShip_CanUseEquipmentTech(this, reinterpret_cast<aItem::TEquipment*>(Item)) ^ 1)) {
            return Result;
        }
        if (CurrentPlanet == nullptr && DockedTo == nullptr && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && pas::in_range(static_cast<std::uint8_t>(reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->ShotType), static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && reinterpret_cast<aItem::TWeapon*>(Item)->Ammo <= 0) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr && reinterpret_cast<aItem::TEngine*>(Item)->Speed < 100 && !(pas::class_cast_if<aRuins::TRuins*>(this) != nullptr)) {
            return Result;
        }
        if (!pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                pas::Extended cpp_right = pas::real_max<float>(0.25f, reinterpret_cast<aItem::TEquipment*>(Item)->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(aGalaxyStruct::EmptyDamageFlags)));
                return pas::real_divide(pas::real_divide(5.0E+3L, std::max<std::int32_t>(1, Item->Weight)), cpp_right);
            }
            return pas::real_divide(Item->Cost, std::max<std::int32_t>(1, Item->Weight));
        }
        aItem::TEquipment* Equipment = pas::checked_cast<aItem::TEquipment*>(Item);
        for (I = 1; I <= 5; ++I) {
            SavedWeapons[I] = nullptr;
        }
        pas::Object* SavedTarget = nullptr;
        std::uint8_t TemporarilyUnequipped = Equipment->EquippedFlag != 0 && pas::in_range(static_cast<std::uint8_t>(Equipment->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon));
        if (TemporarilyUnequipped) {
            TemporarilyUnequipped = false;
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last) {
                    for (WeaponIndex = 1; WeaponIndex <= cpp_last; ++WeaponIndex) {
                        if (Weapons[WeaponIndex] == Item) {
                            for (I = 1; I <= 5; ++I) {
                                SavedWeapons[I] = Weapons[I];
                            }
                            SavedTarget = pas::checked_cast<aItem::TWeapon*>(Item)->Target;
                            UnequipSlot(static_cast<std::uint8_t>(Item->ItemType), WeaponIndex);
                            TemporarilyUnequipped = true;
                            break;
                        }
                    }
                }
            }
        }
        std::uint8_t SavedChaoticRandom = aGalaxy::Galaxy->CustomRules.ChaoticRandom;
        aGalaxy::Galaxy->CustomRules.ChaoticRandom = false;
        if (pas::in_range(static_cast<std::uint8_t>(Equipment->ItemType), static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            switch (Equipment->ItemType) {
                case aConst::t_Hull: {
                    CandidateHull = reinterpret_cast<aItem::THull*>(Equipment);
                    Result = EvaluateStatBonus(aConst::bonHull, aShip::TShip_CalculateHullArmor(this, CandidateHull));
                    if (GetHull() != CandidateHull) {
                        pas::Extended cpp_left = EvaluateStatBonus(aConst::bonMass, CalculateEquippedMass(nullptr));
                        std::int32_t cpp_left_3 = CalculateEquippedMass(nullptr);
                        std::int32_t cpp_left_2 = cpp_left_3 + CandidateHull->CalculateMass();
                        pas::Extended cpp_right_2 = System::Round(cpp_left - EvaluateStatBonus(aConst::bonMass, cpp_left_2 - GetHull()->CalculateMass()));
                        std::int32_t cpp_left_11 = CandidateHull->GetSlotCount(aConst::sskRadar);
                        pas::Extended cpp_left_10 = static_cast<long double>(Result) + EvaluateStatBonus(aConst::bonSlotRadar, cpp_left_11 - GetHull()->GetSlotCount(aConst::sskRadar));
                        std::int32_t cpp_left_12 = CandidateHull->GetSlotCount(aConst::sskScanner);
                        pas::Extended cpp_left_9 = cpp_left_10 + EvaluateStatBonus(aConst::bonSlotScaner, cpp_left_12 - GetHull()->GetSlotCount(aConst::sskScanner));
                        std::int32_t cpp_left_13 = CandidateHull->GetSlotCount(aConst::sskRepairRobot);
                        pas::Extended cpp_left_8 = cpp_left_9 + EvaluateStatBonus(aConst::bonSlotDroid, cpp_left_13 - GetHull()->GetSlotCount(aConst::sskRepairRobot));
                        std::int32_t cpp_left_14 = CandidateHull->GetSlotCount(aConst::sskCargoHook);
                        pas::Extended cpp_left_7 = cpp_left_8 + EvaluateStatBonus(aConst::bonSlotHook, cpp_left_14 - GetHull()->GetSlotCount(aConst::sskCargoHook));
                        std::int32_t cpp_left_15 = CandidateHull->GetSlotCount(aConst::sskDefGenerator);
                        pas::Extended cpp_left_6 = cpp_left_7 + EvaluateStatBonus(aConst::bonSlotDef, cpp_left_15 - GetHull()->GetSlotCount(aConst::sskDefGenerator));
                        std::int32_t cpp_left_16 = CandidateHull->GetSlotCount(aConst::sskWeapon);
                        pas::Extended cpp_left_5 = cpp_left_6 + EvaluateStatBonus(aConst::bonSlotWeapon, cpp_left_16 - GetHull()->GetSlotCount(aConst::sskWeapon));
                        std::int32_t cpp_left_17 = CandidateHull->GetSlotCount(aConst::sskArtefact);
                        pas::Extended cpp_left_4 = cpp_left_5 + EvaluateStatBonus(aConst::bonSlotArt, cpp_left_17 - GetHull()->GetSlotCount(aConst::sskArtefact));
                        std::int32_t cpp_left_18 = CandidateHull->GetSlotCount(aConst::sskAfterburner);
                        Result = cpp_left_4 + EvaluateStatBonus(aConst::bonSlotForsage, cpp_left_18 - GetHull()->GetSlotCount(aConst::sskAfterburner)) + cpp_right_2;
                    }
                    break;
                }
                case aConst::t_FuelTanks: {
                    Result = EvaluateStatBonus(aConst::bonFuel, TShip::GetItemFuelTankCapacity(Equipment));
                    break;
                }
                case aConst::t_Engine: {
                    pas::Extended cpp_left_19 = EvaluateStatBonus(aConst::bonSpeed, aShip::TShip_CalculateEngineSpeed(this, reinterpret_cast<aItem::TEngine*>(Equipment), InNormalSpace() || static_cast<std::uint8_t>(aShip::TShip_CanRepairEquipmentTech(this, Equipment) ^ 1)));
                    Result = cpp_left_19 + EvaluateStatBonus(aConst::bonJump, aShip::TShip_CalculateEngineJumpRange(this, reinterpret_cast<aItem::TEngine*>(Equipment)));
                    break;
                }
                case aConst::t_Radar: {
                    Result = EvaluateStatBonus(aConst::bonRadar, aShip::TShip_CalculateRadarRange(this, reinterpret_cast<aItem::TRadar*>(Equipment)));
                    break;
                }
                case aConst::t_Scaner: {
                    if (GetRadar() != nullptr) {
                        Result = EvaluateStatBonus(aConst::bonScan, aShip::TShip_CalculateScannerPower(this, reinterpret_cast<aItem::TScaner*>(Equipment)));
                    }
                    break;
                }
                case aConst::t_RepairRobot: {
                    Result = EvaluateStatBonus(aConst::bonDroid, aShip::TShip_CalculateRepairPoints(this, reinterpret_cast<aItem::TRepairRobot*>(Equipment)));
                    break;
                }
                case aConst::t_CargoHook: {
                    Result = EvaluateStatBonus(aConst::bonHook, aShip::TShip_CalculateCargoHookPower(this, reinterpret_cast<aItem::TCargoHook*>(Equipment)));
                    break;
                }
                case aConst::t_DefGenerator: {
                    Result = EvaluateStatBonus(aConst::bonDef, System::Round(1.0E+2L - aShip::TShip_CalculateDefGeneratorFactor(this, reinterpret_cast<aItem::TDefGenerator*>(Equipment)) * 1.0E+2L));
                    break;
                }
            }
        } else if (pas::in_range(static_cast<std::uint8_t>(Equipment->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Weapon = reinterpret_cast<aItem::TWeapon*>(Equipment);
            WeaponRange = aShip::TShip_GetWeaponRange(this, Weapon);
            Damage = EstimateWeaponDamageAgainstTypicalDefense(Weapon);
            Damage = EvaluateWeaponDamage(Weapon, true, Damage);
            MinRange = WeaponRange;
            {
                const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last_2) {
                    for (WeaponIndex = 1; WeaponIndex <= cpp_last_2; ++WeaponIndex) {
                        OtherRange = aShip::TShip_GetWeaponRange(this, Weapons[WeaponIndex]);
                        if (OtherRange < MinRange) {
                            MinRange = OtherRange;
                        }
                    }
                }
            }
            DamageBonusKind = aConst::WeaponDamageClasses[static_cast<std::int32_t>(aConst::ClassifyWeaponDamageFlags(Weapon->GetWeaponInfo()->DamageFlags)) & 0x0000007f].BonusKind;
            DamageValue = EvaluateStatBonus(static_cast<aConst::TEquipmentBonusKind>(DamageBonusKind), System::Round(Damage));
            Result = Result + DamageValue * (1.0L + EvaluateStatBonus(aConst::bonWRadius, System::Round(WeaponRange)) * 0.002L);
            Result = Result + EvaluateStatBonus(aConst::bonWRadius, System::Round((static_cast<long double>(WeaponRange) + MinRange) * 0.5L)) * 0.5L;
            if ((CurrentPlanet != nullptr || DockedTo != nullptr) && pas::in_range(static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType), static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                pas::Extended cpp_left_20 = aMyFunction::RemapClamped(Weapon->AmmoCapacity, 3.0E+1, 1.0E+2, 0.0, 0.5);
                Result = Result * (cpp_left_20 + aMyFunction::RemapClamped(Weapon->AmmoCapacity, 0.0, 3.0E+1, 0.0, 0.5));
            }
        } else {
            Result = 1.0f;
            aGalaxy::Galaxy->CustomRules.ChaoticRandom = SavedChaoticRandom;
            return Result;
        }
        Result = static_cast<long double>(Result) + aShip::TShip_GetEquipmentEvaluationSynergyBonus(this, Equipment);
        aGalaxy::Galaxy->CustomRules.ChaoticRandom = SavedChaoticRandom;
        if (Equipment->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Equipment->ScriptItem)->Name != u"" || Equipment->NoDropFlag > 0) {
            Result = pas::real_max<float>(Result, 0.0f) * 1.0E+1L;
        } else if (Equipment->BrokenFlag != 0 && pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr && static_cast<std::uint8_t>(reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->Availability) == 4) {
            Result = 0.0f;
        } else if (Equipment->BrokenFlag != 0 && static_cast<std::uint8_t>(aShip::TShip_CanRepairEquipmentTech(this, Equipment) ^ 1)) {
            Result = 0.0f;
        } else if (!aShip::TShip_CanUseEquipmentTech(this, Equipment)) {
            Result = 0.0f;
        }
        if (TemporarilyUnequipped) {
            EquipItem(Equipment);
            pas::checked_cast<aItem::TWeapon*>(Equipment)->Target = SavedTarget;
            for (I = 1; I <= 5; ++I) {
                Weapons[I] = SavedWeapons[I];
            }
        }
        return Result;
    }

    // Returns one for a non-weapon; adjusts weapon damage for galaxy technology and accuracy.
    float TShip::EstimateWeaponDamageAgainstTypicalDefense(aItem::TItem* Item) {
        std::int32_t MinimumDamage{};
        std::int32_t MaximumDamage{};
        std::int32_t Level{};
        float DefenseFactor{};
        float Armor{};
        float Result = 1.0f;
        std::uint8_t ItemKind = static_cast<std::uint8_t>(Item->ItemType);
        if (pas::in_range(ItemKind, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            if ((std::bit_cast<std::uint32_t>(reinterpret_cast<aItem::TWeapon*>(Item)->GetDamageFlags()) & pas::shl(1, static_cast<std::int32_t>(aGalaxyStruct::dkUndefendable))) != 0) {
                MinimumDamage = GetWeaponMinDamage(reinterpret_cast<aItem::TWeapon*>(Item));
                MaximumDamage = GetWeaponMaxDamage(reinterpret_cast<aItem::TWeapon*>(Item));
            } else {
                Level = std::min<std::int32_t>(aGalaxy::Galaxy->TechLevel + 1, 8);
                DefenseFactor = 1.0L - aConst::DefGeneratorLevelFactors[Level];
                DefenseFactor = 1.0L - static_cast<long double>(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 1.0, 8.0, 0.5, 1.2)) * DefenseFactor;
                Armor = static_cast<long double>(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 1.0, 8.0, 0.5, 1.5)) * aConst::HullLevelStats[Level].Armor;
                MinimumDamage = System::Round(static_cast<long double>(GetWeaponMinDamage(reinterpret_cast<aItem::TWeapon*>(Item))) * DefenseFactor - Armor);
                MaximumDamage = System::Round(static_cast<long double>(GetWeaponMaxDamage(reinterpret_cast<aItem::TWeapon*>(Item))) * DefenseFactor - Armor);
            }
            MinimumDamage = std::max<std::int32_t>(1, MinimumDamage);
            MaximumDamage = std::max<std::int32_t>(1, MaximumDamage);
            return aMyFunction::RemapClamped(GetEffectiveSkillLevel(psAccuracy, false) & 0x0000007f, 0.0, 6.0, (3 * MinimumDamage + MaximumDamage) / 4, (MinimumDamage + 3 * MaximumDamage) / 4);
        }
        return Result;
    }

    std::int32_t TShip_GetWeaponRange(TShip* Self, aItem::TWeapon* Weapon) {
        std::int32_t Range{};
        std::int32_t I{};
        std::int32_t TemplateRange{};
        aItem::PExtraSpecial Extra{};
        if (!pas::in_range(static_cast<std::uint8_t>(Weapon->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            return 0;
        }
        Range = Weapon->Range + Self->GetTotalStatBonus(aConst::bonWRadius);
        if (Weapon->ExtraSpecials != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Weapon->ExtraSpecials) - 1); cpp_range.next(I); ) {
                Extra = pas::list_at<aItem::TExtraSpecial>(Weapon->ExtraSpecials, I);
                Range += pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Extra->ModuleIndexPlusOne - 1].StatBonuses, aConst::bonWRadius * sizeof(std::int32_t))) * Extra->Count;
            }
        }
        if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
            TemplateRange = Weapon->GetWeaponInfo()->MissileRange;
            if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr && static_cast<aKling::TKling*>(Self)->KlingType == 0) {
                Range = std::max<std::int32_t>(Range, TemplateRange);
            } else if (pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr) {
                Range = std::max<std::int32_t>(Range, TemplateRange);
            } else if (aGalaxy::Galaxy->AreMaxRangeMissilesEnabled() || aPlayer::GetPlayer() == Self) {
                aShip::ClampMissileWeaponRange(Self, TemplateRange, Range);
            } else {
                Range = std::min<std::int32_t>(Self->GetRadarRange(), Range);
            }
        }
        return std::max<std::int32_t>(100, Range);
    }

    std::int32_t TShip_CalculateHullArmor(TShip* Self, aItem::THull* Hull) {
        if (Hull->ItemType != aConst::t_Hull) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonHull);
        if (Self->GetHull() != Hull) {
            Bonus -= aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonHull, Self->GetHull());
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonHull, Hull);
        }
        std::int32_t Value = Hull->Armor + Bonus;
        {
            std::int32_t cpp_right = aConst::HullArtefactArmor + aConst::HullArtefactBoostArmor * static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtefactHull, Hull, false));
            Value += Self->CountActiveArtefacts(aConst::t_ArtefactHull) * cpp_right;
        }
        return std::max<std::int32_t>(0, Value);
    }

    // Zero unless Item is fuel tanks.
    std::int32_t TShip::GetItemFuelTankCapacity(aItem::TItem* Item) {
        if (Item->ItemType != aConst::t_FuelTanks) {
            return 0;
        }
        return reinterpret_cast<aItem::TFuelTanks*>(Item)->Capacity;
    }

    std::int32_t TShip_CalculateEngineSpeed(TShip* Self, aItem::TEngine* Engine, std::uint8_t ApplyBrokenPenalty) {
        std::int32_t I{};
        if (Engine->ItemType != aConst::t_Engine) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonSpeed);
        if (Engine->EquippedFlag == 0 && Engine->SpecialModuleIndex != 0) {
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonSpeed, Engine);
        }
        std::int32_t Value = Engine->Speed;
        if (ApplyBrokenPenalty && Engine->BrokenFlag != 0) {
            Value = System::Round(Value * 0.6L);
        }
        {
            std::int32_t cpp_right = aConst::WeaponToSpeedArtefactBonus + aConst::WeaponToSpeedArtefactBoost * static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtWeaponToSpeed, Engine, false));
            Value += Self->CountActiveArtefacts(aConst::t_ArtWeaponToSpeed) * cpp_right;
        }
        Value = std::max<std::int32_t>(std::min<std::int32_t>(200, Engine->Speed), Value + Bonus);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtefactSpeed)); cpp_range.next(I); ) {
            Value = System::Round(Value * (aConst::SpeedArtefactFactor + static_cast<long double>(aConst::SpeedArtefactBoostFactor) * static_cast<std::int8_t>(Self->CanBoostArtefact(aConst::t_ArtefactSpeed, nullptr, false))));
        }
        return std::max<std::int32_t>(0, Value);
    }

    std::int32_t TShip_CalculateEngineJumpRange(TShip* Self, aItem::TEngine* Engine) {
        if (Engine->ItemType != aConst::t_Engine) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonJump);
        if (Engine->EquippedFlag == 0 && Engine->SpecialModuleIndex != 0) {
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonJump, Engine);
        }
        std::int32_t Value = Engine->JumpRange;
        if (!aShip::TShip_CanUseEquipmentTech(Self, Engine)) {
            Value = Value / 2;
        }
        if (Self->CountActiveArtefacts(aConst::t_ArtGiperJump) > 0) {
            Value = std::max<std::int64_t>(static_cast<std::int64_t>(Value), ([&] {
                pas::Extended cpp_right = aConst::HyperJumpArtefactRange + aConst::HyperJumpArtefactBoostRange * (Self->CanBoostArtefact(aConst::t_ArtGiperJump, Engine, false) & 127 & 0x0000007f);
                return System::Round(System::Sqrt(Self->CountActiveArtefacts(aConst::t_ArtGiperJump)) * cpp_right);
            }()));
        }
        return std::max<std::int32_t>(0, Value + Bonus);
    }

    std::int32_t TShip_CalculateRadarRange(TShip* Self, aItem::TRadar* Radar) {
        if (Radar->ItemType != aConst::t_Radar) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonRadar);
        if (Radar->EquippedFlag == 0 && Radar->SpecialModuleIndex != 0) {
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonRadar, Radar);
        }
        std::int32_t Value = Radar->Range;
        {
            std::int32_t cpp_right = aConst::RadarArtefactRange + aConst::RadarArtefactBoostRange * static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtefactRadar, Radar, false));
            Value += Self->CountActiveArtefacts(aConst::t_ArtefactRadar) * cpp_right;
        }
        return std::max<std::int32_t>(0, Value + Bonus);
    }

    std::int32_t TShip_CalculateScannerPower(TShip* Self, aItem::TScaner* Scanner) {
        if (Scanner->ItemType != aConst::t_Scaner) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonScan);
        if (Scanner->EquippedFlag == 0 && Scanner->SpecialModuleIndex != 0) {
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonScan, Scanner);
        }
        std::int32_t Value = Scanner->ScanPower;
        {
            std::int32_t cpp_right = aConst::ScannerArtefactPower + aConst::ScannerArtefactBoostPower * static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtefactScaner, Scanner, false));
            Value += Self->CountActiveArtefacts(aConst::t_ArtefactScaner) * cpp_right;
        }
        return std::max<std::int32_t>(0, Value + Bonus);
    }

    std::int32_t TShip_CalculateRepairPoints(TShip* Self, aItem::TRepairRobot* RepairRobot) {
        if (RepairRobot->ItemType != aConst::t_RepairRobot) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonDroid);
        if (RepairRobot->EquippedFlag == 0 && RepairRobot->SpecialModuleIndex != 0) {
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonDroid, RepairRobot);
        }
        std::int32_t Value = RepairRobot->RepairPoints;
        {
            std::int32_t cpp_right = aConst::DroidArtefactRepair + aConst::DroidArtefactBoostRepair * static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtefactDroid, RepairRobot, false));
            Value += Self->CountActiveArtefacts(aConst::t_ArtefactDroid) * cpp_right;
        }
        return std::max<std::int32_t>(0, Value + Bonus);
    }

    std::int32_t TShip_CalculateCargoHookPower(TShip* Self, aItem::TCargoHook* CargoHook) {
        if (CargoHook->ItemType != aConst::t_CargoHook) {
            return 0;
        }
        std::int32_t Bonus = Self->GetTotalStatBonus(aConst::bonHook);
        if (CargoHook->EquippedFlag == 0 && CargoHook->SpecialModuleIndex != 0) {
            Bonus += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonHook, CargoHook);
        }
        std::int32_t Value = CargoHook->PickupPower;
        {
            std::int32_t cpp_right = aConst::CargoHookArtefactPower + aConst::CargoHookArtefactBoostPower * static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtefactHook, CargoHook, false));
            Value += Self->CountActiveArtefacts(aConst::t_ArtefactHook) * cpp_right;
        }
        return std::max<std::int32_t>(0, Value + Bonus);
    }

    float TShip_CalculateDefGeneratorFactor(TShip* Self, aItem::TDefGenerator* DefGenerator) {
        std::int32_t I{};
        if (DefGenerator->ItemType != aConst::t_DefGenerator) {
            return 0.0f;
        }
        std::int32_t Percent = Self->GetTotalStatBonus(aConst::bonDef);
        if (DefGenerator->EquippedFlag == 0 && DefGenerator->SpecialModuleIndex != 0) {
            Percent += aShip::TShip_GetEquipmentStatBonus(Self, aConst::bonDef, DefGenerator);
        }
        float Factor = DefGenerator->DamageFactor;
        if (Percent != 0) {
            Factor = Factor - (1.0L - aItem::DefensePercentToDamageFactor(Percent));
        }
        {
            pas::Extended cpp_right = aConst::DefenseArtefactBonus + static_cast<long double>(aConst::DefenseArtefactBoost) * static_cast<std::int8_t>(Self->CanBoostArtefact(aConst::t_ArtefactDef, DefGenerator, false));
            Factor = Factor - Self->CountActiveArtefacts(aConst::t_ArtefactDef) * cpp_right;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtDefToEnergy)); cpp_range.next(I); ) {
            Factor = pas::real_min<pas::Extended>(1.0L, Factor + (1.0L - Factor) * (aConst::DefenseToEnergyPenalty + static_cast<long double>(aConst::DefenseToEnergyBoostPenalty) * static_cast<std::int8_t>(Self->CanBoostArtefact(aConst::t_ArtDefToEnergy, DefGenerator, false))));
        }
        if (Self->CountActiveArtefacts(aConst::t_ArtDefToArms1) > 0 && static_cast<std::uint8_t>(Self->CanBoostArtefact(aConst::t_ArtDefToArms1, DefGenerator, false) ^ 1)) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtDefToArms1)); cpp_range_2.next(I); ) {
                Factor = pas::real_min<pas::Extended>(1.0L, Factor + (1.0L - Factor) * aConst::DefenseToWeaponPenalty);
            }
        }
        return pas::real_min<float>(1.0f, pas::real_max<float>(0.01f, Factor));
    }

    float TShip_GetEquipmentEvaluationSynergyBonus(TShip* Self, aItem::TEquipment* Item) {
        float Positive{};
        float Nonpositive{};
        float DamageBonus{};
        aConst::PMicroModuleTemplate Module{};
        std::int32_t I{};
        std::int32_t ModuleIndex{};
        aConst::TItemType ItemType{};
        // Adds positive and nonpositive values to separate Single accumulators at ParentFrame-4/-8.
        auto AccumulateEquipmentBonus = [&](float Value) -> void {
            if (Value > 0.0L) {
                Positive = static_cast<long double>(Positive) + Value;
            } else {
                Nonpositive = static_cast<long double>(Nonpositive) + Value;
            }
        };
        float Result = 0.0f;
        if (Item->SpecialModuleIndex != 0) {
            ModuleIndex = Item->SpecialModuleIndex - 1;
            if (!aShip::TShip_IsMicroModuleRaciallyRestricted(Self, ModuleIndex)) {
                Positive = 0.0f;
                Nonpositive = 0.0f;
                ItemType = Item->ItemType;
                if (Item->ExtraSpecials == nullptr || pas::list_count(Item->ExtraSpecials) == 0) {
                    Module = &aConst::MicroModuleTemplates[ModuleIndex];
                    if (ItemType != aConst::t_Hull) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonHull, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonHull * sizeof(std::int32_t)))));
                    }
                    if (ItemType != aConst::t_Engine) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSpeed, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSpeed * sizeof(std::int32_t)))));
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonJump, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonJump * sizeof(std::int32_t)))));
                    }
                    if (Self->GetRadar() != nullptr && ItemType != aConst::t_Radar) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonRadar, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonRadar * sizeof(std::int32_t)))));
                    }
                    if (Self->GetScanner() != nullptr && ItemType != aConst::t_Scaner) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonScan, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonScan * sizeof(std::int32_t)))));
                    }
                    if (Self->GetRepairRobot() != nullptr && ItemType != aConst::t_RepairRobot) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonDroid, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonDroid * sizeof(std::int32_t)))));
                    }
                    if (Self->GetCargoHook() != nullptr && ItemType != aConst::t_CargoHook) {
                        // Native expression adds raw hook power to the evaluated bonus before subtracting its evaluation.
                        AccumulateEquipmentBonus(([&] {
                            pas::Extended cpp_left = ([&] {
                                pas::Extended cpp_left_2 = Self->EvaluateStatBonus(aConst::bonHook, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonHook * sizeof(std::int32_t))));
                                return cpp_left_2 + aShip::TShip_CalculateCargoHookPower(Self, Self->GetCargoHook());
                            }());
                            return cpp_left - Self->EvaluateStatBonus(aConst::bonHook, aShip::TShip_CalculateCargoHookPower(Self, Self->GetCargoHook()));
                        }()));
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonHookRadius, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonHookRadius * sizeof(std::int32_t)))));
                    }
                    if (Self->GetDefGenerator() != nullptr && ItemType != aConst::t_DefGenerator) {
                        AccumulateEquipmentBonus(([&] {
                            pas::Extended cpp_left_3 = Self->EvaluateStatBonus(aConst::bonDef, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonDef * sizeof(std::int32_t))) + static_cast<std::int32_t>(System::Round(1.0E+2L - aShip::TShip_CalculateDefGeneratorFactor(Self, Self->GetDefGenerator()) * 1.0E+2L)));
                            return cpp_left_3 - Self->EvaluateStatBonus(aConst::bonDef, System::Round(1.0E+2L - aShip::TShip_CalculateDefGeneratorFactor(Self, Self->GetDefGenerator()) * 1.0E+2L));
                        }()));
                    }
                    AccumulateEquipmentBonus(([&] {
                        pas::Extended cpp_left_4 = Self->EvaluateStatBonus(aConst::bonWRadius, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonWRadius * sizeof(std::int32_t))));
                        return cpp_left_4 * (Self->CountEquippedWeapons() & 0x0000007f);
                    }()));
                    if (!pas::in_range(static_cast<std::uint8_t>(ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
                            DamageBonus = 0.0f;
                            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Self->Weapons[I]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy)) {
                                DamageBonus = static_cast<long double>(DamageBonus) + Self->EvaluateStatBonus(aConst::bonWEnergy, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonWEnergy * sizeof(std::int32_t))));
                            }
                            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Self->Weapons[I]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter)) {
                                DamageBonus = static_cast<long double>(DamageBonus) + Self->EvaluateStatBonus(aConst::bonWSplinter, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonWSplinter * sizeof(std::int32_t))));
                            }
                            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Self->Weapons[I]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkMissile)) {
                                DamageBonus = static_cast<long double>(DamageBonus) + Self->EvaluateStatBonus(aConst::bonWMissile, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonWMissile * sizeof(std::int32_t))));
                            }
                            if (DamageBonus > 0.001L || DamageBonus < -0.001L) {
                                DamageBonus = Self->EvaluateWeaponDamage(Self->Weapons[I], false, DamageBonus);
                            }
                            AccumulateEquipmentBonus(DamageBonus);
                        }
                    }
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill1, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSkill1 * sizeof(std::int32_t)))));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill2, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSkill2 * sizeof(std::int32_t)))));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill3, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSkill3 * sizeof(std::int32_t)))));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill4, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSkill4 * sizeof(std::int32_t)))));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill5, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSkill5 * sizeof(std::int32_t)))));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill6, pas::load_unaligned<std::int32_t>(pas::byte_offset(&Module->StatBonuses, aConst::bonSkill6 * sizeof(std::int32_t)))));
                } else {
                    if (ItemType != aConst::t_Hull) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonHull, Item->GetStatBonus(aConst::bonHull)));
                    }
                    if (ItemType != aConst::t_Engine) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSpeed, Item->GetStatBonus(aConst::bonSpeed)));
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonJump, Item->GetStatBonus(aConst::bonJump)));
                    }
                    if (Self->GetRadar() != nullptr && ItemType != aConst::t_Radar) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonRadar, Item->GetStatBonus(aConst::bonRadar)));
                    }
                    if (Self->GetScanner() != nullptr && ItemType != aConst::t_Scaner) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonScan, Item->GetStatBonus(aConst::bonScan)));
                    }
                    if (Self->GetRepairRobot() != nullptr && ItemType != aConst::t_RepairRobot) {
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonDroid, Item->GetStatBonus(aConst::bonDroid)));
                    }
                    if (Self->GetCargoHook() != nullptr && ItemType != aConst::t_CargoHook) {
                        // Native expression adds raw hook power to the evaluated bonus before subtracting its evaluation.
                        {
                            pas::Extended cpp_left_6 = Self->EvaluateStatBonus(aConst::bonHook, Item->GetStatBonus(aConst::bonHook));
                            pas::Extended cpp_left_5 = cpp_left_6 + aShip::TShip_CalculateCargoHookPower(Self, Self->GetCargoHook());
                            AccumulateEquipmentBonus(cpp_left_5 - Self->EvaluateStatBonus(aConst::bonHook, aShip::TShip_CalculateCargoHookPower(Self, Self->GetCargoHook())));
                        }
                        AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonHookRadius, Item->GetStatBonus(aConst::bonHookRadius)));
                    }
                    if (Self->GetDefGenerator() != nullptr && ItemType != aConst::t_DefGenerator) {
                        std::int32_t cpp_right = System::Round(1.0E+2L - aShip::TShip_CalculateDefGeneratorFactor(Self, Self->GetDefGenerator()) * 1.0E+2L);
                        pas::Extended cpp_left_7 = Self->EvaluateStatBonus(aConst::bonDef, Item->GetStatBonus(aConst::bonDef) + cpp_right);
                        AccumulateEquipmentBonus(cpp_left_7 - Self->EvaluateStatBonus(aConst::bonDef, System::Round(1.0E+2L - aShip::TShip_CalculateDefGeneratorFactor(Self, Self->GetDefGenerator()) * 1.0E+2L)));
                    }
                    {
                        pas::Extended cpp_left_8 = Self->EvaluateStatBonus(aConst::bonWRadius, Item->GetStatBonus(aConst::bonWRadius));
                        AccumulateEquipmentBonus(cpp_left_8 * (Self->CountEquippedWeapons() & 0x0000007f));
                    }
                    if (!pas::in_range(static_cast<std::uint8_t>(ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Self->CountEquippedWeapons() & 0x0000007f); cpp_range_2.next(I); ) {
                            DamageBonus = 0.0f;
                            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Self->Weapons[I]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy)) {
                                DamageBonus = static_cast<long double>(DamageBonus) + Self->EvaluateStatBonus(aConst::bonWEnergy, Item->GetStatBonus(aConst::bonWEnergy));
                            }
                            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Self->Weapons[I]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter)) {
                                DamageBonus = static_cast<long double>(DamageBonus) + Self->EvaluateStatBonus(aConst::bonWSplinter, Item->GetStatBonus(aConst::bonWSplinter));
                            }
                            if (pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Self->Weapons[I]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkMissile)) {
                                DamageBonus = static_cast<long double>(DamageBonus) + Self->EvaluateStatBonus(aConst::bonWMissile, Item->GetStatBonus(aConst::bonWMissile));
                            }
                            if (DamageBonus > 0.001L || DamageBonus < -0.001L) {
                                DamageBonus = Self->EvaluateWeaponDamage(Self->Weapons[I], false, DamageBonus);
                            }
                            AccumulateEquipmentBonus(DamageBonus);
                        }
                    }
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill1, Item->GetStatBonus(aConst::bonSkill1)));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill2, Item->GetStatBonus(aConst::bonSkill2)));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill3, Item->GetStatBonus(aConst::bonSkill3)));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill4, Item->GetStatBonus(aConst::bonSkill4)));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill5, Item->GetStatBonus(aConst::bonSkill5)));
                    AccumulateEquipmentBonus(Self->EvaluateStatBonus(aConst::bonSkill6, Item->GetStatBonus(aConst::bonSkill6)));
                    AccumulateEquipmentBonus(Item->GetStatBonus(aConst::bonAIValue));
                }
                {
                    pas::Extended cpp_left_9 = Self->EvaluateStatBonus(aConst::bonMass, Self->CalculateEquippedMass(Item));
                    AccumulateEquipmentBonus(cpp_left_9 - Self->EvaluateStatBonus(aConst::bonMass, Self->CalculateEquippedMass(nullptr)));
                }
                if (Item->MicroModuleIndex != 0) {
                    Positive = static_cast<long double>(Positive) + System::Round(static_cast<long double>(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinEff * sizeof(std::int32_t)))) * Positive * 1.0E-4L);
                    Nonpositive = static_cast<long double>(Nonpositive) + System::Round(static_cast<long double>(pas::load_unaligned<std::int32_t>(pas::byte_offset(&aConst::MicroModuleTemplates[Item->MicroModuleIndex - 1].StatBonuses, aConst::bonExtraAkrinPenalty * sizeof(std::int32_t)))) * Nonpositive * 1.0E-4L);
                }
                return static_cast<long double>(Positive) + Nonpositive;
            }
        }
        return Result;
    }

    float TShip::GetWeaponArtefactDamageFactor(aItem::TWeapon* Weapon) {
        static const pas::Set<0, 255> EnergyDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy}});
        std::int32_t I{};
        float Result = 1.0f;
        std::uint32_t Flags = Weapon->GetWeaponInfo()->DamageFlags;
        if ((Flags & 1) != 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtEnergyPulse)); cpp_range.next(I); ) {
                Result = Result * (1.0L + (aConst::EnergyPulseArtefactFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtEnergyPulse, Weapon, false))) * aConst::EnergyPulseArtefactBoostFactor) * aConst::EnergyPulseArtefactChance);
            }
        }
        if (CountActiveArtefacts(aConst::t_ArtDefToEnergy) > 0 && (Flags & 1) != 0 && GetDefGenerator() != nullptr) {
            double cpp_arg = (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(EnergyDamageFlags)) & 0x0000007f) + 1;
            double cpp_arg_2 = aConst::DefenseToEnergyUpperFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtDefToEnergy, Weapon, false))) * aConst::DefenseToEnergyUpperBoost;
            double cpp_arg_3 = aConst::DefenseToEnergyMinimumFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtDefToEnergy, Weapon, false))) * aConst::DefenseToEnergyMinimumBoost;
            pas::Extended cpp_right = aMyFunction::RemapClamped(cpp_arg, 1.0, 5.0, cpp_arg_2, cpp_arg_3);
            pas::Extended cpp_left_2 = ((CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(EnergyDamageFlags)) & 0x0000007f) + 1) * cpp_right;
            double cpp_arg_4 = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(EnergyDamageFlags)) & 0x0000007f;
            double cpp_arg_5 = aConst::DefenseToEnergyUpperFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtDefToEnergy, Weapon, false))) * aConst::DefenseToEnergyUpperBoost;
            double cpp_arg_6 = aConst::DefenseToEnergyMinimumFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtDefToEnergy, Weapon, false))) * aConst::DefenseToEnergyMinimumBoost;
            pas::Extended cpp_right_2 = aMyFunction::RemapClamped(cpp_arg_4, 1.0, 5.0, cpp_arg_5, cpp_arg_6);
            pas::Extended cpp_left = cpp_left_2 - (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(EnergyDamageFlags)) & 0x0000007f) * cpp_right_2 - 1.0L;
            Result = Result * (1.0L + cpp_left * CountActiveArtefacts(aConst::t_ArtDefToEnergy));
        }
        if ((Flags & 2) != 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtSplinter)); cpp_range_2.next(I); ) {
                Result = Result * (aConst::SplinterArtefactFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtSplinter, Weapon, false))) * aConst::SplinterArtefactBoostFactor);
            }
        }
        return Result;
    }

    void TShip::UpdateSpeedTrackingMetrics() {
        SmoothedSpeed = (29 * SmoothedSpeed + Speed) / 30;
        if (EnemyShip != nullptr && EnemyShip->InNormalSpace() && EnemyShip->CurrentStar == CurrentStar) {
            SmoothedEnemySpeed = (29 * SmoothedEnemySpeed + EnemyShip->Speed) / 30;
        }
    }

    void TShip::RefreshEquipmentEvaluationMetrics() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        if (SmoothedWealth == 0) {
            SmoothedWealth = Wealth;
        } else {
            SmoothedWealth = (Wealth + SmoothedWealth) / 2;
        }
        if (SmoothedMoneyFraction == 0.0L) {
            SmoothedMoneyFraction = pas::real_divide(Money, std::max<std::int32_t>(1, Wealth));
        } else {
            SmoothedMoneyFraction = (pas::real_divide(Money, std::max<std::int32_t>(1, Wealth)) + 4.0L * SmoothedMoneyFraction) * 0.2L;
        }
        std::int32_t EquippedCount = 0;
        float Effectiveness = 0.0f;
        std::int32_t FreeCapacity = GetHull()->Weight;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->ItemType != aConst::t_Hull && Item->EquippedFlag != 0) {
                ++EquippedCount;
                Effectiveness = static_cast<long double>(Effectiveness) + CalculateItemEffectiveness(Item);
                FreeCapacity -= Item->Weight;
            }
        }
        Effectiveness = pas::real_divide(Effectiveness, std::max<std::int32_t>(1, EquippedCount));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
            if (Item->EquippedFlag != 0) {
                FreeCapacity -= Item->Weight;
            }
        }
        if (SmoothedEquipmentEffectiveness == 0.0L) {
            SmoothedEquipmentEffectiveness = Effectiveness;
        } else {
            SmoothedEquipmentEffectiveness = (4.0L * SmoothedEquipmentEffectiveness + Effectiveness) * 0.2L;
        }
        if (SmoothedFreeCapacityFraction == 0.0L) {
            SmoothedFreeCapacityFraction = pas::real_max<pas::Extended>(0.0L, pas::real_divide(FreeCapacity, std::max<std::int32_t>(1, GetHull()->Weight)));
        } else {
            SmoothedFreeCapacityFraction = (4.0L * SmoothedFreeCapacityFraction + pas::real_max<pas::Extended>(0.0L, pas::real_divide(FreeCapacity, std::max<std::int32_t>(1, GetHull()->Weight)))) * 0.2L;
        }
    }

    float TShip::EvaluateItem(aItem::TItem* Item, std::uint8_t PriceMode) {
        return AdjustItemEvaluation(Item, PriceMode, CalculateItemEffectiveness(Item));
    }

    // Price modes: 1 negated item cost, 3 resale value, 4 item cost; other modes omit the price term. Mode 0 also omits weight/fragility penalties; the supplied effectiveness is recomputed.
    float TShip::AdjustItemEvaluation(aItem::TItem* Item, std::uint8_t PriceMode, float Effectiveness) {
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        std::int32_t Price{};
        float FragilityScale = 1.0f;
        if (pas::is_one_of<aConst::t_FuelTanks, aConst::t_Radar, aConst::t_Scaner>(Item->ItemType)) {
            FragilityScale = FragilityScale * 0.5L;
        }
        float DesiredMoneyFraction = 0.2f;
        float HullValueScale = 1.0f;
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
        Effectiveness = CalculateItemEffectiveness(Item);
        switch (PriceMode) {
            case 4: Price = Item->Cost; break;
            case 3: Price = Item->CalculateResaleValue(GetEffectiveSkillLevel(psTrading, false)); break;
            case 1: Price = -Item->Cost; break;
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

    float TShip::EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannerFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> NoFlags = pas::constant_set<pas::Set<0, 255>>({});
        float Result = 0.0f;
        if (Value == 0) {
            return Result;
        }
        {
            aConst::TEquipmentBonusKind cpp_case = BonusKind;
            if (cpp_case == aConst::bonHull) {
                return Value * 200;
            } else if (cpp_case == aConst::bonFuel) {
                return Value * 2.5L;
            } else if (cpp_case == aConst::bonSpeed) {
                return Value;
            } else if (cpp_case == aConst::bonJump) {
                return Value * 25;
            } else if (cpp_case == aConst::bonRadar) {
                return Value * 0.05L;
            } else if (cpp_case == aConst::bonScan) {
                return Value * 5 + Value * 20 * (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f);
            } else if (cpp_case == aConst::bonDroid) {
                return pas::real_divide(Value * 10, pas::real_max<float>(0.1f, GetHull()->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(NoFlags))));
            } else if (cpp_case == aConst::bonHook) {
                return (pas::real_min<pas::Extended>(static_cast<pas::Extended>(Value), static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]) + Value * 0.1L) * 1.0L;
            } else if (cpp_case == aConst::bonDef) {
                return pas::real_divide(pas::real_divide(Value * 5 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                return Value * 10;
            } else if (cpp_case == aConst::bonWSplinter) {
                return Value * 10;
            } else if (cpp_case == aConst::bonWMissile) {
                return Value * 10 * (0.1L + static_cast<std::int8_t>(GetRadarRange() > 0) * 0.9L);
            } else if (cpp_case == aConst::bonWRadius) {
                return Value * pas::sqr(pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed), std::max<std::int32_t>(100, SmoothedSpeed)));
            } else if (cpp_case == aConst::bonHookRadius) {
                return Value * 0.1L;
            } else if (cpp_case == aConst::bonMass) {
                return aMyFunction::RemapClamped(Value, aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333) * 5.0E+3L;
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * 0.3L;
                } else if (GetRadar() != nullptr && Value < 0) {
                    return -SlotBonusEvaluationWeights[BonusKind] - SlotBonusEvaluationWeights[18] * (CountMissileWeapons() & 0x0000007f);
                } else if (GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotScaner) {
                if (GetSlotCount(aConst::sskScanner) == 0 && Value > 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * 0.3L;
                } else if (GetScanner() != nullptr && Value < 0) {
                    return -SlotBonusEvaluationWeights[BonusKind] - (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannerFlags)) & 0x0000007f) * 0.1L * SlotBonusEvaluationWeights[18];
                } else if (GetSlotCount(aConst::sskScanner) == 1 && Value < 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * 0.3L;
                } else if (GetRepairRobot() != nullptr && Value < 0) {
                    return -SlotBonusEvaluationWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotHook) {
                if (GetSlotCount(aConst::sskCargoHook) == 0 && Value > 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * 0.3L;
                } else if (GetCargoHook() != nullptr && Value < 0) {
                    return -SlotBonusEvaluationWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskCargoHook) == 1 && Value < 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * 0.3L;
                } else if (GetDefGenerator() != nullptr && Value < 0) {
                    return -SlotBonusEvaluationWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    return SlotBonusEvaluationWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - GetSlotCount(aConst::sskWeapon)) * SlotBonusEvaluationWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskWeapon)) * SlotBonusEvaluationWeights[BonusKind];
                }
                {
                    std::int32_t cpp_right = std::max<std::int32_t>(Value + GetSlotCount(aConst::sskWeapon), 1);
                    if ((CountEquippedWeapons() & 0x0000007f) > cpp_right) {
                        std::int32_t cpp_right_2 = std::max<std::int32_t>(1, Value + GetSlotCount(aConst::sskWeapon));
                        return Result - SlotBonusEvaluationWeights[BonusKind] * 0.6L * ((CountEquippedWeapons() & 0x0000007f) - cpp_right_2);
                    }
                }
                return Result;
            } else if (cpp_case == aConst::bonSlotArt) {
                if (GetSlotCount(aConst::sskArtefact) < aConst::DefaultHullSlotCounts[8] && Value > 0) {
                    Result = std::min<std::int32_t>(Value, aConst::DefaultHullSlotCounts[8] - GetSlotCount(aConst::sskArtefact)) * SlotBonusEvaluationWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -GetSlotCount(aConst::sskArtefact)) * SlotBonusEvaluationWeights[BonusKind];
                }
                if (Artefacts != nullptr) {
                    if (pas::list_count(Artefacts) > std::max<std::int32_t>(Value + GetSlotCount(aConst::sskArtefact), 0)) {
                        return -1.0E+3f;
                    }
                }
                return Result;
            } else if (cpp_case == aConst::bonSlotForsage) {
                if (GetSlotCount(aConst::sskAfterburner) == 0 && Value > 0) {
                    return SlotBonusEvaluationWeights[BonusKind];
                } else if (GetSlotCount(aConst::sskAfterburner) == 1 && Value < 0) {
                    return -SlotBonusEvaluationWeights[BonusKind];
                } else {
                    return Result;
                }
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(6 - (GetEffectiveSkillLevel(static_cast<TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f), Value) * SkillBonusEvaluationWeights[BonusKind];
                }
                if (Value > 0 && Value + (GetEffectiveSkillLevel(static_cast<TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) > 6) {
                    Result = Result + SkillBonusEvaluationWeights[BonusKind] * 0.05L * (Value + (GetEffectiveSkillLevel(static_cast<TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) - 6);
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(GetEffectiveSkillLevel(static_cast<TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f, -Value) * -SkillBonusEvaluationWeights[BonusKind];
                }
                if (Value < 0 && Value + (GetEffectiveSkillLevel(static_cast<TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) < 0) {
                    return Result + SkillBonusEvaluationWeights[BonusKind] * 0.03L * (Value + (GetEffectiveSkillLevel(static_cast<TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f));
                }
                return Result;
            } else {
                return 0.0f;
            }
        }
    }

    float TShip::EvaluateWeaponDamage(aItem::TWeapon* Weapon, std::uint8_t IncludeAdditiveBonuses, float BaseDamage) {
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
        if (pas::contains(Flags, aGalaxyStruct::dkDestruct)) {
            Result = Result * 1.05L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkDrain)) {
            Result = Result * 1.5L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkShock)) {
            Result = Result * (1.1L + (CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ShockFlags)) & 0x0000007f) * 0.05L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
            Result = Result * 1.1L;
        }
        if (pas::contains(Flags, aGalaxyStruct::dkMagnetic)) {
            Result = Result * 1.1L;
        }
        float StatusFactor = 1.0f;
        if (pas::contains(Flags, aGalaxyStruct::dkScanBonus)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkBonusToDamaged)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.1L);
        }
        if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
            StatusFactor = StatusFactor * (1.0L + ScannerFactor * 0.05L);
        }
        StatusFactor = StatusFactor - 1.0L;
        if (IncludeAdditiveBonuses) {
            if (pas::contains(Flags, aGalaxyStruct::dkDestruct)) {
                Result = Result + 1.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDecelerate)) {
                Result = Result + 2.0L;
            }
            if (CountActiveArtefacts(aConst::t_ArtDecelerate) > 0 && pas::contains(Flags, aGalaxyStruct::dkSplinter)) {
                Result = Result + 5.0L + 5 * ((CanBoostArtefact(aConst::t_ArtDecelerate, Weapon, false) || CountActiveArtefacts(aConst::t_ArtDecelerate) > 1) & 0x0000007f);
            }
            {
                std::int32_t cpp_left = CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(AcidFlags));
                Result = static_cast<long double>(Result) + cpp_left * Weapon->GetShotCount();
            }
            if (pas::contains(Flags, aGalaxyStruct::dkAcid)) {
                ShotTotal = 1;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, CountEquippedWeapons() & 0x0000007f); cpp_range.next(I); ) {
                    ShotTotal += Weapons[I]->GetShotCount();
                }
                Result = static_cast<long double>(Result) + ShotTotal * 2;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkMoreDrop)) {
                Result = Result + ScannerFactor * 5.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDropCargo)) {
                Result = Result + ScannerFactor * 5.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkReduceEngine)) {
                Result = Result + ScannerFactor * 1.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkBlockWeapon)) {
                Result = Result + ScannerFactor * 5.0L;
            }
            if (pas::contains(Flags, aGalaxyStruct::dkDroidBlock)) {
                Result = Result + ScannerFactor * 5.0L;
            }
        }
        float SpeedFactor = pas::real_divide(std::max<std::int32_t>(100, SmoothedEnemySpeed) * GetHull()->Weight, aConst::HullBaseSize * pas::real_max<pas::Extended>(1.0E+2L, static_cast<long double>(SmoothedSpeed) * aConst::EquipmentSizeFactors[1]));
        switch (static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType)) {
            case aGalaxyStruct::wstRocket: {
                Result = Result * 1.0L * Weapon->GetShotCount() * (1.0L + StatusFactor);
                break;
            }
            case aGalaxyStruct::wstMissile: {
                Result = Result * (1.0L + Weapon->GetWeaponInfo()->SecondaryDamageRadius * 0.5L * 0.01L + StatusFactor) * Weapon->GetShotCount();
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
        Result = static_cast<long double>(Result) * Weapon->GetAttackCount();
        return Result * 0.01L * (100 + aMyFunction::SeededRandomIntRange(-20, 20, Seed + Weapon->GetWeaponInfo()->TypeHash));
    }

    // Evaluates a temporary clone; restores NextItemId but leaves LoadedSaveVersion set to CurrentSaveVersion. ModuleIndex is zero-based.
    float TShip::EvaluateMicroModuleGain(aItem::TEquipment* Item, std::int32_t ModuleIndex) {
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        std::uint32_t SavedNextId = aGalaxy::Galaxy->NextItemId;
        aItem::TEquipment* CopyItem = reinterpret_cast<aItem::TEquipment*>(aItem::CreateItemByType(Item->ItemType));
        Item->SaveToBuffer(Buffer);
        Buffer->SetPosition(0);
        GlobalsV::LoadedSaveVersion = aConst::CurrentSaveVersion;
        CopyItem->LoadFromBuffer(Buffer, aGalaxy::Galaxy);
        aItem::ApplyMicroModule(ModuleIndex, CopyItem);
        float WithModule = EvaluateItem(CopyItem, 1);
        float Result = static_cast<long double>(WithModule) - EvaluateItem(Item, 1);
        pas::free(CopyItem);
        aGalaxy::Galaxy->NextItemId = SavedNextId;
        pas::free(Buffer);
        return Result;
    }

    // Consumes beneficial carried modules, preferring installed equipment; refreshes derived stats after each application.
    void TShip::AutoApplyMicroModules() {
        aItem::TEquipment* Candidate{};
        aItem::TEquipment* Module{};
        std::int32_t I{};
        std::int32_t J{};
        aItem::TEquipment* Best{};
        float Score{};
        float BestScore{};
        // Caller-popped static link; equipment -4, module item -8, ship -12.
        auto FitsMicroModuleCapacity = [&]() -> std::uint8_t {
            if (Candidate->ItemType != aConst::t_Hull) {
                return Candidate->Weight * (aConst::MicroModuleTemplates[pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1].SizePercent - 100) * 0.01L <= this->CargoFreeSpace + 1;
            }
            return -Candidate->Weight * (aConst::MicroModuleTemplates[pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1].SizePercent - 100) * 0.01L <= this->CargoFreeSpace + 1;
        };
        std::uint8_t Changed = true;
        while (Changed) {
            Changed = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Module = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (pas::class_cast_if<aItem::TMicroModule*>(Module) != nullptr) {
                    Best = nullptr;
                    BestScore = 0.0f;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range_2.next(J); ) {
                        Candidate = pas::list_at<aItem::TEquipment>(Inventory, J);
                        if (pas::checked_cast<aItem::TMicroModule*>(Module)->CanInstallOn(Candidate) && (Candidate->EquippedFlag != 0 || Candidate->ItemType == aConst::t_Hull) && FitsMicroModuleCapacity()) {
                            Score = EvaluateMicroModuleGain(Candidate, pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1);
                            if (Score > BestScore) {
                                BestScore = Score;
                                Best = Candidate;
                            }
                        }
                    }
                    if (Best != nullptr) {
                        Changed = true;
                        aItem::ApplyMicroModule(pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1, Best);
                        pas::list_delete(Inventory, pas::list_indexof(Inventory, reinterpret_cast<void*>(Module)));
                        pas::free(Module);
                        RefreshDerivedStats(true);
                        break;
                    }
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range_3.next(J); ) {
                        Candidate = pas::list_at<aItem::TEquipment>(Inventory, J);
                        if (pas::checked_cast<aItem::TMicroModule*>(Module)->CanInstallOn(Candidate) && Candidate->EquippedFlag == 0 && Candidate->ItemType != aConst::t_Hull && FitsMicroModuleCapacity()) {
                            Score = EvaluateMicroModuleGain(Candidate, pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1);
                            if (Score > BestScore) {
                                BestScore = Score;
                                Best = Candidate;
                            }
                        }
                    }
                    if (Best != nullptr) {
                        Changed = true;
                        aItem::ApplyMicroModule(pas::checked_cast<aItem::TMicroModule*>(Module)->MicroModuleIndex - 1, Best);
                        pas::list_delete(Inventory, pas::list_indexof(Inventory, reinterpret_cast<void*>(Module)));
                        pas::free(Module);
                        RefreshDerivedStats(true);
                        break;
                    }
                }
                if (Changed) {
                    break;
                }
            }
        }
    }

    void TShip::AutoEquipInventory() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TEquipment* Best{};
        float Score{};
        float BestScore{};
        std::uint8_t Kind{};
        std::uint8_t PriceMode{};
        std::uint8_t BestProtected{};
        std::uint8_t ItemProtected{};
        if (IsDocked() || CargoFreeSpace < 0) {
            PriceMode = 3;
        } else {
            PriceMode = 0;
        }
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(this) != nullptr) {
            PriceMode = 0;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                Item->Unequip();
            } else if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator)) && pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (Item->ItemType - 42) * sizeof(aItem::TEquipment*))) != Item) {
                Item->Unequip();
            }
        }
        for (I = 1; I <= 5; ++I) {
            Weapons[I] = nullptr;
        }
        WeaponCount = 0;
        {
            std::int32_t cpp_left = CountCarriedEquipmentByType(aConst::t_Weapon1);
            if (cpp_left < GetSlotCount(aConst::sskWeapon)) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range_2.next(I); ) {
                    Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                        EquipItem(pas::checked_cast<aItem::TWeapon*>(Item));
                    }
                }
            } else {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, GetSlotCount(aConst::sskWeapon)); cpp_range_3.next(I); ) {
                    EquipItem(SelectBestUnequippedWeapon());
                }
            }
        }
        for (Kind = static_cast<std::uint8_t>(43); Kind <= static_cast<std::uint8_t>(49); ++Kind) {
            if (pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (Kind - 42) * sizeof(aItem::TEquipment*))) != nullptr) {
                pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (Kind - 42) * sizeof(aItem::TEquipment*)))->Unequip();
            }
            if (GetSlotCountForItemType(Kind) > 0) {
                pas::store_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(this)->Slots, (Kind - 42) * sizeof(aItem::TEquipment*)), nullptr);
                Best = nullptr;
                BestScore = 0.0f;
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range_4.next(I); ) {
                    Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (static_cast<std::uint8_t>(Item->ItemType) == Kind) {
                        BestProtected = Best != nullptr && (Best->NoDropFlag > 0 || Best->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Best->ScriptItem)->Name != u"");
                        ItemProtected = Item->NoDropFlag > 0 || Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name != u"";
                        if (BestProtected) {
                            if (!ItemProtected) {
                                continue;
                            }
                        }
                        Score = EvaluateItem(Item, PriceMode);
                        if (Kind == 43 && Best != nullptr && ([&] {
                            std::int32_t cpp_left_2 = GetJumpDestinationDistance();
                            return cpp_left_2 <= pas::checked_cast<aItem::TFuelTanks*>(Best)->Fuel;
                        }()) && ([&] {
                            std::int32_t cpp_left_3 = GetJumpDestinationDistance();
                            return cpp_left_3 > pas::checked_cast<aItem::TFuelTanks*>(Item)->Fuel;
                        }())) {
                            continue;
                        }
                        if ((Score >= 0.0L || pas::in_range(Kind, 43, 44) || Kind == 48 && pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(TypeId) && GetSlotCount(aConst::sskCargoHook) > 0 || Item->NoDropFlag > 0 || Item->ScriptItem != nullptr || CargoFreeSpace >= 0 && PriceMode != 0 && EvaluateItem(Item, 0) >= 0.0L) && (Best == nullptr || Score > BestScore || Score == BestScore && Best->Weight > Item->Weight || ItemProtected && static_cast<std::uint8_t>(BestProtected ^ 1))) {
                            BestScore = Score;
                            Best = Item;
                        }
                    }
                }
                if (Best != nullptr) {
                    EquipItem(Best);
                }
            }
        }
        RefreshDerivedStats(true);
    }

    // Sells at a location or jettisons in space. May flag a long-stranded NPC for destruction when overload cannot be resolved.
    void TShip_DropCargoUntilNotOverloaded(TShip* Self) {
        std::int32_t Count{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        std::uint8_t Good{};
        double ItemValue{};
        double ArtefactValue{};
        double GoodsValue{};
        std::int32_t I{};
        aItem::TItem* Candidate{};
        std::uint8_t CanDrop = Self->InNormalSpace() && static_cast<std::uint8_t>(pas::in_range(Self->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) ^ 1);
        if (Self->CargoFreeSpace < 0 && Self->NoDrop && CanDrop) {
            if (Self->ScriptShip == nullptr) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning! Ship ", Self->GetFullName(u" "_wref.get()), u" is overloaded but cant drop anything with NoDrop flag"})));
            } else {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning! Ship ", Self->GetFullName(u" "_wref.get()), u" (", reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->Script->ScriptFileName, u" ", reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->State->Name, u") is overloaded but cant drop anything with NoDrop flag"})));
            }
        } else {
            while (Self->CargoFreeSpace < 0) {
                if (Self->NoDrop) {
                    Item = nullptr;
                } else {
                    Item = reinterpret_cast<aItem::TEquipment*>(Self->SelectLeastValuableInventoryItem());
                }
                if (Item != nullptr) {
                    ItemValue = pas::real_divide(Item->Cost, Item->Weight);
                } else {
                    ItemValue = 0.0;
                }
                if (Self->NoDrop) {
                    Artefact = nullptr;
                } else {
                    Artefact = Self->SelectLeastValuableArtefact();
                }
                if (Artefact != nullptr) {
                    ArtefactValue = pas::real_divide(Artefact->Cost, Artefact->Weight);
                    if (Artefact->EquippedFlag != 0) {
                        ArtefactValue = ArtefactValue * 1.0E+1L;
                    }
                } else {
                    ArtefactValue = 0.0;
                }
                if (Self->SelectCheapestCargoGood() < 255) {
                    Good = Self->SelectCheapestCargoGood();
                    Count = std::min<std::int32_t>(pas::abs(Self->CargoFreeSpace), Self->CargoGoods[Good].Count);
                    GoodsValue = aConst::GoodsMarket[Good].AveragePrice * Count;
                    if (!(Item != nullptr && static_cast<long double>(Item->Cost) < GoodsValue) && !(Artefact != nullptr && static_cast<long double>(Artefact->Cost) < GoodsValue)) {
                        if (CanDrop) {
                            aShip::TShip_DropGoodsIntoSpace(Self, Good, Count);
                        } else {
                            Self->SellGoodsToLocation(Good, Count);
                        }
                        continue;
                    }
                } else {
                    Count = 0;
                    GoodsValue = 0.0;
                    Good = 0;
                }
                if (Artefact != nullptr && ArtefactValue <= ItemValue) {
                    if (CanDrop) {
                        Self->DropCarriedArtefactAsMovingLoot(Artefact);
                    } else {
                        Self->LiquidateArtefact(Artefact);
                    }
                } else if (Item != nullptr && Item->EquippedFlag == 0 && (Count == 0 || static_cast<long double>(Item->Cost) < GoodsValue)) {
                    if (CanDrop && pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr && Item->Weight + Self->CargoFreeSpace > 0) {
                        Item = pas::checked_cast<aItem::TCountableItem*>(Item)->Split(-Self->CargoFreeSpace);
                        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Item));
                        Self->DropCarriedItemAsMovingLoot(Item);
                    } else if (CanDrop) {
                        Self->DropCarriedItemAsMovingLoot(Item);
                    } else {
                        aShip::TShip_LiquidateInventoryItem(Self, Item);
                    }
                } else if (Count > 0) {
                    if (CanDrop) {
                        aShip::TShip_DropGoodsIntoSpace(Self, Good, Count);
                    } else {
                        Self->SellGoodsToLocation(Good, Count);
                    }
                } else if (Artefact != nullptr) {
                    if (CanDrop) {
                        Self->DropCarriedArtefactAsMovingLoot(Artefact);
                    } else {
                        Self->LiquidateArtefact(Artefact);
                    }
                } else if (Item != nullptr && static_cast<std::uint8_t>(Self->IsEssentialInventoryItem(Item) ^ 1)) {
                    if (CanDrop) {
                        Self->DropCarriedItemAsMovingLoot(Item);
                    } else {
                        aShip::TShip_LiquidateInventoryItem(Self, Item);
                    }
                } else {
                    for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Self->Inventory) - 1); cpp_range.next(I); ) {
                        Candidate = pas::list_at<aItem::TItem>(Self->Inventory, I);
                        if (Candidate->NoDropFlag > 0) {
                            continue;
                        }
                        if (pas::in_range(Candidate->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && Candidate->Weight > Self->GetHull()->Weight * 0.2L) {
                            Item = reinterpret_cast<aItem::TEquipment*>(Candidate);
                            break;
                        }
                        if (Candidate->ItemType == aConst::t_CargoHook && Candidate->Weight > Self->GetHull()->Weight * 0.2L) {
                            Item = reinterpret_cast<aItem::TEquipment*>(Candidate);
                            break;
                        }
                    }
                    if (Item != nullptr && (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) || Item->ItemType == aConst::t_CargoHook)) {
                        if (CanDrop) {
                            Self->DropCarriedItemAsMovingLoot(Item);
                        } else {
                            aShip::TShip_LiquidateInventoryItem(Self, Item);
                        }
                    } else {
                        if (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) && Self->ConsecutiveDockedDays > 100) {
                            Self->DestroyQueued = true;
                        }
                        break;
                    }
                }
            }
        }
    }

    void TShip::DropRandomCheapItemsOnDestruction(std::int32_t Count) {
        std::int32_t Index{};
        std::int32_t I{};
        aItem::TItem* Item{};
        // Caller-popped static link; cycling inventory index -4, ship -8.
        auto SelectCheapItem = [&]() -> aItem::TItem* {
            std::int32_t Attempt{};
            aItem::TItem* Candidate{};
            double Value{};
            aItem::TItem* Result = nullptr;
            Index = aMyFunction::NextRandomIntRange(1, pas::list_count(this->Inventory) - 1, this->RandomState);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(this->Inventory) - 1); cpp_range.next(Attempt); ) {
                aMyFunction::IncrementWrapped(Index, 1, pas::list_count(this->Inventory) - 1);
                Candidate = pas::list_at<aItem::TItem>(this->Inventory, Index);
                if (Candidate->DestroyFlag > 0 || Candidate->NoDropFlag > 0) {
                    continue;
                }
                Value = pas::real_divide(Candidate->Cost, std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(static_cast<long double>(aGalaxy::Galaxy->GetDropValueModifier()) * aGalaxy::Galaxy->AverageRangerCapital) / 25));
                if (pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
                    Value = static_cast<long double>(KlingCheapDropValueFactors[pas::checked_cast<aKling::TKling*>(this)->KlingType]) * Value;
                }
                if (Candidate->Cost > 1000 && ([&] {
                    std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 100, this->RandomState);
                    return cpp_left > System::Round(1.0E+2L * System::Exp(2.0L - 2.0L * Value));
                }())) {
                    continue;
                }
                if (Result == nullptr || ([&] {
                    pas::Extended cpp_left_2 = aMyFunction::NextRandomFloatRange(0.5, 1.0, this->RandomState);
                    pas::Extended cpp_right = cpp_left_2 * Result->Cost;
                    return Candidate->Cost < cpp_right;
                }())) {
                    Result = Candidate;
                }
            }
            return Result;
        };
        if (NoDrop) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(I); ) {
            if (pas::list_count(Inventory) > 1) {
                Item = SelectCheapItem();
                if (Item != nullptr) {
                    DropCarriedItemAsMovingLoot(Item);
                }
            }
        }
    }

    void TShip::DropRandomValuableItemsOnDestruction(std::int32_t Count) {
        std::int32_t Index{};
        std::int32_t I{};
        aItem::TItem* Item{};
        // Caller-popped static link; cycling inventory index -4, ship -8.
        auto SelectValuableItem = [&]() -> aItem::TItem* {
            std::int32_t Attempt{};
            aItem::TItem* Candidate{};
            double Value{};
            aItem::TItem* Result = nullptr;
            if (aPlayer::GetPlayer() == nullptr) {
                return Result;
            }
            Index = aMyFunction::NextRandomIntRange(1, pas::list_count(this->Inventory) - 1, this->RandomState);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(this->Inventory) - 1); cpp_range.next(Attempt); ) {
                aMyFunction::IncrementWrapped(Index, 1, pas::list_count(this->Inventory) - 1);
                Candidate = pas::list_at<aItem::TItem>(this->Inventory, Index);
                if (Candidate->DestroyFlag > 0 || Candidate->NoDropFlag > 0) {
                    continue;
                }
                Value = pas::real_divide(Candidate->Cost, std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(static_cast<long double>(aGalaxy::Galaxy->GetDropValueModifier()) * aGalaxy::Galaxy->AverageRangerCapital) / 12));
                if (pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
                    Value = static_cast<long double>(KlingValuableDropValueFactors[pas::checked_cast<aKling::TKling*>(this)->KlingType]) * Value;
                }
                if (Candidate->Cost > 1000 && ([&] {
                    std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 100, this->RandomState);
                    return cpp_left > System::Round(1.0E+2L * System::Exp(0.3L - 0.3L * Value));
                }())) {
                    continue;
                }
                if (Result == nullptr || ([&] {
                    pas::Extended cpp_left_2 = aMyFunction::NextRandomFloatRange(0.5, 1.0, this->RandomState);
                    pas::Extended cpp_right = cpp_left_2 * Result->Cost;
                    return Candidate->Cost > cpp_right;
                }())) {
                    Result = Candidate;
                }
            }
            return Result;
        };
        if (NoDrop) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(I); ) {
            if (pas::list_count(Inventory) > 1) {
                Item = SelectValuableItem();
                if (Item != nullptr) {
                    DropCarriedItemAsMovingLoot(Item);
                }
            }
        }
    }

    // Requires a Dominator; used by dialogue and Script.DomikProgramm. Attempts at most 100 selections.
    void TShip::DropItemsForDominatorProgram(std::int32_t Count) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        double Value{};
        std::int64_t DropValueScale{};
        if (NoDrop) {
            return;
        }
        std::int32_t Attempts = 0;
        std::int32_t Dropped = 0;
        aItem::TEquipment* Cheapest = nullptr;
        while (true) {
            ++Attempts;
            if (Attempts > 100) {
                return;
            }
            I = aMyFunction::NextRandomIntRange(1, pas::list_count(Inventory) - 1, RandomState);
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->NoDropFlag > 0) {
                continue;
            }
            if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_Engine))) {
                continue;
            }
            if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && (WeaponCount <= 1 || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->ShotType == aGalaxyStruct::wstAreaDamage)) {
                continue;
            }
            DropValueScale = System::Round(static_cast<long double>(aGalaxy::Galaxy->GetDropValueModifier()) * aGalaxy::Galaxy->AverageRangerCapital) / 12;
            {
                pas::Extended cpp_right = pas::real_divide(Item->Cost, std::max<std::int64_t>(static_cast<std::int64_t>(1), DropValueScale));
                Value = DominatorProgramDropCostFactors[pas::checked_cast<aKling::TKling*>(this)->KlingType] * cpp_right;
            }
            if (Item->Cost > 1000 && ([&] {
                std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                return cpp_left > System::Round(System::Exp(0.3L - 0.3L * Value) * 1.0E+2L);
            }())) {
                continue;
            }
            if (Item->Cost > 1000 && ([&] {
                std::int32_t cpp_left_2 = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                return cpp_left_2 > System::Round(System::Exp(0.3L - 0.3L * Value * 0.5L) * 1.0E+2L);
            }())) {
                if (Cheapest == nullptr || Cheapest->Cost > Item->Cost) {
                    Cheapest = Item;
                    continue;
                } else {
                    DropCarriedItemAsMovingLoot(Cheapest);
                    Cheapest = nullptr;
                }
            } else {
                DropCarriedItemAsMovingLoot(Item);
                Cheapest = nullptr;
            }
            ++Dropped;
            if (Dropped >= Count) {
                break;
            }
        }
    }

    // Requires membership in Inventory; honors ship/item NoDrop and rejects the hull. True means accepted, even if a script consumes the drop.
    std::uint8_t TShip::DropCarriedItemAsMovingLoot(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (NoDrop || Item->NoDropFlag > 0) {
            return Result;
        }
        if (Item->ItemType == aConst::t_Hull) {
            return false;
        }
        Item->Position = Position;
        pas::list_delete(Inventory, pas::list_indexof(Inventory, reinterpret_cast<void*>(Item)));
        aShip::TShip_QueueMovingItemDrop(this, Item, 0);
        RefreshDerivedStats(true);
        return true;
    }

    // Requires membership in Artefacts; honors ship/item NoDrop.
    std::uint8_t TShip::DropCarriedArtefactAsMovingLoot(aItem::TArtefact* Item) {
        std::uint8_t Result = false;
        if (NoDrop || Item->NoDropFlag > 0) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr) {
            static_cast<aTranclucator::TTranclucator*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Item)->Ship)->OwnerShip = nullptr;
        }
        Item->Position = Position;
        pas::list_delete(Artefacts, pas::list_indexof(Artefacts, reinterpret_cast<void*>(Item)));
        aShip::TShip_QueueMovingItemDrop(this, Item, 0);
        RefreshDerivedStats(true);
        return true;
    }

    void TShip::DropGuaranteedDeathDropItems() {
        std::int32_t I{};
        aItem::TItem* Item{};
        if (NoDrop) {
            return;
        }
        std::int32_t Count = pas::list_count(GuaranteedDeathDropItems);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(GuaranteedDeathDropItems, I);
            Item->Position = Position;
            aShip::TShip_QueueMovingItemDrop(this, Item, 0);
        }
        pas::list_clear(GuaranteedDeathDropItems);
    }

    // Deterministic eligibility roll for an ordinary pirate without a script binding or player partnership.
    std::uint8_t TShip_CanDropTreasureMap(TShip* Self) {
        std::uint8_t Result = false;
        if (pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr && static_cast<aPirate::TPirate*>(Self)->PirateType == 0 && aPlayer::GetPlayer() != Self->PartnerShip && Self->ScriptShip == nullptr) {
            if (aMyFunction::SeededRandomIntRange(1, 100, Self->Seed) <= 15) {
                return true;
            }
        }
        return Result;
    }

    // Searches reachable uninhabited planets with accessible artefact/module loot; excludes maps already carried or stored by the player.
    aPlanet::TPlanet* TShip::SelectTreasureMapPlanet() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t Index{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        aPlanet::PPlanetSurfaceLootEntry Loot{};
        std::uint8_t AlreadyMapped{};
        aItem::TItem* Item{};
        aPlayer::PStorageEntry Storage{};
        aPlanet::TPlanet* Result = nullptr;
        std::int32_t Last = CurrentStar->StarDistances.length() - 1;
        Index = aMyFunction::SeededRandomIntRange(0, Last, Id + System::Trunc(static_cast<std::int32_t>(aGalaxy::Galaxy->GenerationSeed)));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Last); cpp_range.next(I); ) {
            aMyFunction::IncrementWrapped(Index, 0, Last);
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[Index].Star));
            if (Star->Constellation->Id == 20) {
                continue;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                if (static_cast<std::uint8_t>(pas::in_range(Planet->OwnerId, static_cast<std::int32_t>(aGalaxyStruct::oiUninhabited), static_cast<std::int32_t>(aGalaxyStruct::oiUninhabited)) ^ 1) || Planet->SurfaceLootEntries == nullptr) {
                    continue;
                }
                AlreadyMapped = false;
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range_3.next(K); ) {
                    Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Inventory, K);
                    if (pas::class_cast_if<aItem::TTreasureMap*>(Item) != nullptr && static_cast<aItem::TTreasureMap*>(Item)->TargetPlanet == Planet) {
                        AlreadyMapped = true;
                        break;
                    }
                }
                if (AlreadyMapped) {
                    continue;
                }
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_4.next(K); ) {
                    Storage = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, K);
                    Item = Storage->Item;
                    if (pas::class_cast_if<aItem::TTreasureMap*>(Item) != nullptr && static_cast<aItem::TTreasureMap*>(Item)->TargetPlanet == Planet) {
                        AlreadyMapped = true;
                        break;
                    }
                }
                if (AlreadyMapped) {
                    continue;
                }
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->SurfaceLootEntries) - 1); cpp_range_5.next(K); ) {
                    Loot = pas::list_at<aPlanet::TPlanetSurfaceLootEntry>(Planet->SurfaceLootEntries, K);
                    if (Loot->Item != nullptr && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessSurfaceLootItem(Loot->Item)) && (pas::class_cast_if<aItem::TArtefact*>(Loot->Item) != nullptr || pas::class_cast_if<aItem::TMicroModule*>(Loot->Item) != nullptr)) {
                        return Planet;
                    }
                }
            }
        }
        return Result;
    }

    std::uint8_t TShip_TryDropTreasureMap(TShip* Self) {
        aItem::TItem* Item{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t Result = false;
        if (aShip::TShip_CanDropTreasureMap(Self) && static_cast<std::uint8_t>(Self->NoDrop ^ 1)) {
            Planet = Self->SelectTreasureMapPlanet();
            if (Planet != nullptr) {
                Item = pas::construct_call<aItem::TTreasureMap>(aItem::TEquipment_Create);
                pas::checked_cast<aItem::TTreasureMap*>(Item)->Init(Planet, Self);
                pas::list_add(Self->Inventory, reinterpret_cast<void*>(Item));
                Self->DropCarriedItemAsMovingLoot(Item);
                return true;
            }
        }
        return Result;
    }

    // Player drops are placed immediately and may be consumed by scripts or stellar heat. Native artefact branch mistakenly deletes Inventory at the Artefacts index (); preserved here as observed behavior.
    std::uint8_t TShip::DropItemIntoStar(aItem::TItem* Item) {
        SE_Weapon::TWeaponSE* Effect{};
        aEFilmEnd::PEFilmEndEntry FilmEntry{};
        if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr && reinterpret_cast<aItem::TEquipment*>(Item)->EquippedFlag != 0) {
            UnequipItem(reinterpret_cast<aItem::TEquipment*>(Item));
        }
        if (aPlayer::GetPlayer() != this) {
            if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                return DropCarriedArtefactAsMovingLoot(reinterpret_cast<aItem::TArtefact*>(Item));
            }
            return DropCarriedItemAsMovingLoot(Item);
        }
        aGalaxy::TStar* Star = CurrentStar;
        if (pas::list_indexof(Inventory, reinterpret_cast<void*>(Item)) >= 0) {
            pas::list_delete(Inventory, pas::list_indexof(Inventory, reinterpret_cast<void*>(Item)));
        } else if (pas::list_indexof(Artefacts, reinterpret_cast<void*>(Item)) >= 0) {
            pas::list_delete(Inventory, pas::list_indexof(Artefacts, reinterpret_cast<void*>(Item)));
        }
        pas::list_add(Star->Items, reinterpret_cast<void*>(Item));
        double Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, CurrentStar->GenerationSeed * aGalaxy::Galaxy->CurrentTurn * Item->Id) * SystemImports::Pi, 1.8E+2L);
        if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
            pas::checked_cast<aItem::TWeapon*>(Item)->Target = nullptr;
        }
        Item->Position.X = aPlayer::GetPlayer()->Position.X + System::Sin(Angle) * 1.0E+2L;
        Item->Position.Y = aPlayer::GetPlayer()->Position.Y - System::Cos(Angle) * 1.0E+2L;
        std::int32_t ActionResult = ScriptItemsAct(aConst::satOnDropItemFixed, Item, nullptr, 0);
        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
            ActionResult = aScript::RunItemConfigActionCode(Item, aConst::satOnDropItemFixed, this, Item, nullptr, ActionResult);
        }
        if (Item->ScriptItem != nullptr) {
            ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnDropItemFixed, this, Item, nullptr, ActionResult);
        }
        if (ActionResult == 0) {
            {
                SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                EC_Struct::TPointF position = Item->Position;
                graphObject->SetPosition(position);
            }
            if (static_cast<long double>(Star->DamageRadius) * Star->DamageRadius > pas::sqr(static_cast<pas::Extended>(Item->Position.X)) + pas::sqr(static_cast<pas::Extended>(Item->Position.Y))) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                Effect->SetEndpoints(nullptr, Item->GetGraphObject());
                Effect->SetHit(0, 0, true, true);
                if (Globals::TrailingFilmEffects == nullptr) {
                    Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
                }
                FilmEntry = Globals::TrailingFilmEffects->AppendEntry();
                SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&FilmEntry->SceneObject), Effect);
                {
                    SE_Space::TObjectSE* graphObject_2 = Item->GetGraphObject();
                    pas::Var<SE_Space::TObjectSE*> relatedObject1 = pas::Var<SE_Space::TObjectSE*>(&FilmEntry->RelatedObject1);
                    SE_Space::RetainSpaceObject(relatedObject1, graphObject_2);
                }
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                pas::list_delete(Star->Items, pas::list_indexof(Star->Items, reinterpret_cast<void*>(Item)));
                pas::free(Item);
            }
        } else {
            pas::list_delete(Star->Items, pas::list_indexof(Star->Items, reinterpret_cast<void*>(Item)));
            if (ActionResult < 0) {
                pas::free(Item);
            }
        }
        return true;
    }

    void TShip::DropAllArtefactsOnDestruction() {
        aItem::TArtefact* Item{};
        std::int32_t I{};
        {
            const std::int32_t cpp_first = pas::list_count(Artefacts) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Item = pas::list_at<aItem::TArtefact>(Artefacts, I);
                    DropCarriedArtefactAsMovingLoot(Item);
                }
            }
        }
    }

    // Caller supplies a valid good and quantity; does not itself honor ship NoDrop.
    void TShip_DropGoodsIntoSpace(TShip* Self, std::uint8_t Good, std::int32_t Count) {
        aItem::TGoods* Goods = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
        Goods->Init(static_cast<aConst::TItemType>(Good), Count);
        Goods->Cost = System::Round(static_cast<long double>(Self->GetAverageCargoCost(Good)) * Count);
        Goods->Position = Self->Position;
        aShip::TShip_QueueMovingItemDrop(Self, Goods, 0);
        Self->ConsumeCargoGoods(Good, Count);
        Self->RefreshDerivedStats(true);
    }

    std::uint8_t TShip::JettisonCargoGoodsTowardTargetValue(std::int32_t TargetValue) {
        std::uint8_t Good{};
        std::int32_t Pass{};
        std::int32_t Quantity{};
        float Factor{};
        std::uint8_t Result = false;
        if (NoDrop) {
            return Result;
        }
        std::int32_t Value = 0;
        std::uint8_t Done = false;
        std::int32_t Drops = 0;
        for (Pass = 1; Pass <= 3; ++Pass) {
            for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                if (CargoGoods[Good].Count > 0) {
                    Factor = aMyFunction::RemapClamped(CargoGoods[Good].Count * aConst::GoodsMarket[Good].AveragePrice, TargetValue / 3, TargetValue * 3, 1.0, 7.0);
                    Quantity = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(pas::real_divide(CargoGoods[Good].Count, Factor)));
                    Value += Quantity * aConst::GoodsMarket[Good].AveragePrice;
                    aShip::TShip_DropGoodsIntoSpace(this, Good, Quantity);
                    ++Drops;
                    if (Value > TargetValue || Drops > 2) {
                        Done = true;
                        break;
                    }
                }
            }
            if (Done) {
                break;
            }
        }
        return Drops > 0;
    }

    void TShip_DropAllCargoGoods(TShip* Self) {
        std::uint8_t Good{};
        std::int32_t Quantity{};
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (Self->CargoGoods[Good].Count > 0) {
                Quantity = Self->CargoGoods[Good].Count;
                aShip::TShip_DropGoodsIntoSpace(Self, Good, Quantity);
            }
        }
    }

    // Caller detaches Item first. Script action 33 can suppress transfer or free Item; otherwise the moving-drop descriptor takes ownership.
    void TShip_QueueMovingItemDrop(TShip* Self, aItem::TItem* Item, std::uint8_t UseFlag) {
        aItem::TItem* OtherItem{};
        float Angle{};
        float Distance{};
        std::int32_t I{};
        std::int32_t Count{};
        if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
            pas::checked_cast<aItem::TWeapon*>(Item)->Target = nullptr;
            Self->CurrentStar->ClearCombatEventWeaponReferences(Item);
        }
        aGalaxy::PMovingDropItemEntry Drop = static_cast<aGalaxy::PMovingDropItemEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(aGalaxy::TMovingDropItemEntry))));
        Drop->Payload = Item;
        Drop->SourceShipId = Self->Id;
        Drop->InsertedIntoStar = false;
        Drop->UseFlag = UseFlag;
        std::int32_t Attempts = 0;
        std::uint8_t Retry = true;
        while (Retry) {
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::NextRandomIntRange(0, 360, Self->RandomState));
            Distance = aMyFunction::NextRandomIntRange(100, 200, Self->RandomState);
            Drop->Destination.X = System::Sin(Angle) * Distance + Item->Position.X;
            Drop->Destination.Y = Item->Position.Y - System::Cos(Angle) * Distance;
            Retry = false;
            Count = pas::list_count(Self->CurrentStar->Items);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 2); cpp_range.next(I); ) {
                OtherItem = pas::list_at<aItem::TItem>(Self->CurrentStar->Items, I);
                if (aMyFunction::PointDistanceSquared(Drop->Destination, OtherItem->Position) < 1.0E+2L) {
                    Retry = true;
                    break;
                }
            }
            if (Attempts > 15) {
                break;
            }
            ++Attempts;
        }
        if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr && reinterpret_cast<aItem::TEquipment*>(Item)->EquippedFlag != 0) {
            Self->UnequipItem(reinterpret_cast<aItem::TEquipment*>(Item));
        }
        std::int32_t ActionResult = Self->ScriptItemsAct(aConst::satOnDropItem, Item, nullptr, 0);
        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
            ActionResult = aScript::RunItemConfigActionCode(Item, aConst::satOnDropItem, Self, Item, nullptr, ActionResult);
        }
        if (Item->ScriptItem != nullptr) {
            ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aConst::satOnDropItem, Self, Item, nullptr, ActionResult);
        }
        if (ActionResult == 0) {
            pas::list_add(Self->CurrentStar->MovingDropItems, static_cast<void*>(Drop));
            if (!(pas::class_cast_if<aKling::TKling*>(Self) != nullptr && pas::class_cast_if<aItem::TProtoplasm*>(Item) != nullptr) && aPlayer::GetPlayer() != Self) {
                Self->AddRecentlyDroppedItem(Item);
            }
        } else {
            Drop->Payload = nullptr;
            EC_Mem::FreeEC(Drop);
            if (ActionResult < 0) {
                pas::free(Item);
            }
        }
    }

    // Considers protection, essential equipment and value per mass; can return the hull when no alternative qualifies.
    aItem::TItem* TShip::SelectLeastValuableInventoryItem() {
        std::int32_t I{};
        double Value{};
        aItem::TEquipment* Item{};
        aItem::TItem* Result = nullptr;
        aItem::TEquipment* Best = GetHull();
        double BestValue = 1.0E+8;
        while (Result != Best) {
            Result = Best;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (Item == Best) {
                    continue;
                }
                if (IsEssentialInventoryItem(Item) && static_cast<std::uint8_t>(IsEssentialInventoryItem(Best) ^ 1)) {
                    continue;
                }
                if (Item->EquippedFlag != 0 && Best->EquippedFlag == 0) {
                    continue;
                }
                if (Item->NoDropFlag > 0) {
                    continue;
                }
                Value = pas::real_divide(Item->Cost, Item->Weight);
                if (IsEssentialInventoryItem(Best) && static_cast<std::uint8_t>(IsEssentialInventoryItem(Item) ^ 1)) {
                    Best = Item;
                    BestValue = Value;
                } else if (Best->EquippedFlag != 0 && Item->EquippedFlag == 0) {
                    Best = Item;
                    BestValue = Value;
                } else if (!(Item->EquippedFlag != 0 && IsOptionalUtilityEquipment(Best) && static_cast<std::uint8_t>(IsOptionalUtilityEquipment(Item) ^ 1))) {
                    if (Value < BestValue) {
                        Best = Item;
                        BestValue = Value;
                    }
                }
            }
        }
        return Result;
    }

    // Native scan starts at index one; an artefact list with one element yields nil.
    aItem::TArtefact* TShip::SelectLeastValuableArtefact() {
        std::int32_t I{};
        double Value{};
        aItem::TArtefact* Item{};
        if (Artefacts == nullptr) {
            return nullptr;
        }
        if (pas::list_count(Artefacts) == 0) {
            return nullptr;
        }
        aItem::TArtefact* Result = nullptr;
        double BestValue = 0.0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TArtefact>(Artefacts, I);
            if (Item->NoDropFlag > 0) {
                continue;
            }
            Value = pas::real_divide(Item->Cost, Item->Weight);
            if (Item->EquippedFlag != 0) {
                Value = Value * 1.0E+1L;
            }
            if (Result == nullptr || Value < BestValue) {
                Result = Item;
                BestValue = Value;
            }
        }
        return Result;
    }

    // Returns 255 if no cargo qualifies.
    std::uint8_t TShip::SelectCheapestCargoGood() {
        double Value{};
        std::uint8_t Good{};
        double BestValue = 1.0E+5;
        std::uint8_t Result = 255;
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            if (CargoGoods[Good].Count != 0) {
                Value = aConst::GoodsMarket[Good].AveragePrice;
                if (Value < BestValue) {
                    BestValue = Value;
                    Result = Good;
                }
            }
        }
        return Result;
    }

    // Can sell equipment, artefacts and excess cargo; outside a market, excess goods are refunded at their cost basis.
    void TShip::OptimizeInventory() {
        std::int32_t Index{};
        aItem::TEquipment* Item{};
        aItem::TEquipment* Best{};
        std::uint8_t Done{};
        double CostPerWeight{};
        std::int32_t Count{};
        std::uint8_t Good{};
        aItem::TArtefact* Artefact{};
        // Caller-popped static link; ship -4. Hull capacity minus equipped and protected items.
        auto GetRetainedCapacity = [&]() -> std::int32_t {
            aItem::TEquipment* Entry{};
            std::int32_t I{};
            std::int32_t Result = GetHull()->Weight;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(this->Inventory) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<aItem::TEquipment>(this->Inventory, I);
                if (Entry->EquippedFlag != 0 || Entry->NoDropFlag > 0 || Entry->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Entry->ScriptItem)->Name != u"") {
                    Result -= Entry->Weight;
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->Artefacts) - 1); cpp_range_2.next(I); ) {
                Entry = pas::list_at<aItem::TEquipment>(this->Artefacts, I);
                if (Entry->EquippedFlag != 0 || Entry->NoDropFlag > 0 || Entry->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Entry->ScriptItem)->Name != u"") {
                    Result -= Entry->Weight;
                }
            }
            return Result;
        };
        AutoEquipInventory();
        while (([&] {
            std::int32_t cpp_left = GetDesiredCargoFreeSpace();
            return cpp_left > GetRetainedCapacity();
        }())) {
            CostPerWeight = 0.0;
            Best = nullptr;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(Index); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, Index);
                if (Item->EquippedFlag != 0 && static_cast<std::uint8_t>(IsEssentialInventoryItem(Item) ^ 1) && (Best == nullptr || pas::real_divide(Item->Cost, Item->Weight) < CostPerWeight || IsOptionalUtilityEquipment(Item) && static_cast<std::uint8_t>(IsOptionalUtilityEquipment(Best) ^ 1))) {
                    Best = Item;
                    CostPerWeight = pas::real_divide(Item->Cost, Item->Weight);
                }
            }
            if (Best == nullptr) {
                break;
            }
            aShip::TShip_LiquidateInventoryItem(this, Best);
            AutoEquipInventory();
        }
        AutoEquipArtefacts();
        while (pas::list_count(Artefacts) > 0 && ([&] {
            std::int32_t cpp_left_2 = GetDesiredCargoFreeSpace();
            return cpp_left_2 > GetRetainedCapacity();
        }())) {
            CostPerWeight = 0.0;
            Artefact = nullptr;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(Artefacts) - 1); cpp_range_2.next(Index); ) {
                Item = pas::list_at<aItem::TEquipment>(Artefacts, Index);
                if (Item->EquippedFlag != 0 && static_cast<std::uint8_t>(IsEssentialInventoryItem(Item) ^ 1) && (Artefact == nullptr || pas::real_divide(Item->Cost, Item->Weight) < CostPerWeight || IsOptionalUtilityEquipment(Item) && static_cast<std::uint8_t>(IsOptionalUtilityEquipment(Artefact) ^ 1))) {
                    Artefact = reinterpret_cast<aItem::TArtefact*>(Item);
                    CostPerWeight = pas::real_divide(Item->Cost, Item->Weight);
                }
            }
            if (Artefact == nullptr) {
                break;
            }
            LiquidateArtefact(Artefact);
            AutoEquipArtefacts();
        }
        do {
            Done = true;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range_3.next(Index); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, Index);
                if (Item->EquippedFlag == 0 && Item->NoDropFlag <= 0 && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"")) {
                    aShip::TShip_LiquidateInventoryItem(this, Item);
                    Done = false;
                    break;
                }
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_4.next(Index); ) {
                Artefact = pas::list_at<aItem::TArtefact>(Artefacts, Index);
                if (Artefact->EquippedFlag == 0 && Artefact->NoDropFlag <= 0 && (Artefact->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Artefact->ScriptItem)->Name == u"")) {
                    LiquidateArtefact(Artefact);
                    Done = false;
                    break;
                }
            }
        } while (!Done);
        if (CargoFreeSpace < 0) {
            for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                if (CargoGoods[Good].Count > 0) {
                    Count = std::min<std::int32_t>(-CargoFreeSpace, CargoGoods[Good].Count);
                    if (CurrentPlanet != nullptr || DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(DockedTo) != nullptr) {
                        SellGoodsToLocation(Good, Count);
                    } else {
                        SetMoney(Money + System::Round(static_cast<long double>(GetAverageCargoCost(Good)) * Count));
                        ConsumeCargoGoods(Good, Count);
                        RefreshDerivedStats(true);
                    }
                    if (CargoFreeSpace >= 0) {
                        break;
                    }
                }
            }
        }
    }

    // Normally credits resale value and frees Item; eligible NPC node stacks instead feed DepositCarriedNodes and automatic training.
    void TShip_LiquidateInventoryItem(TShip* Self, aItem::TItem* Item) {
        if (Item->ItemType == aConst::t_Protoplasm && aPlayer::GetPlayer() != Self && (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr && Self->DockedTo != nullptr && Self->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter) || Self->DaysSincePlayerSeen > 100 || pas::class_cast_if<aWarrior::TWarrior*>(Self) != nullptr && static_cast<aWarrior::TWarrior*>(Self)->WarriorType == aWarrior::wtFlagship)) {
            Self->DepositCarriedNodes();
            pas::checked_cast<aNormalShip::TNormalShip*>(Self)->TrainSkillsAutomatically();
        } else {
            Self->SetMoney(Self->Money + Item->CalculateResaleValue(Self->GetEffectiveSkillLevel(psTrading, false)));
            pas::list_delete(Self->Inventory, pas::list_indexof(Self->Inventory, reinterpret_cast<void*>(Item)));
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && Self->CurrentStar != nullptr) {
                Self->CurrentStar->ClearCombatEventWeaponReferences(Item);
            }
            pas::free(Item);
            Self->RefreshDerivedStats(true);
        }
    }

    // Removes and frees Item after crediting resale value.
    void TShip::LiquidateArtefact(aItem::TArtefact* Item) {
        SetMoney(Money + Item->CalculateResaleValue(GetEffectiveSkillLevel(psTrading, false)));
        pas::list_delete(Artefacts, pas::list_indexof(Artefacts, reinterpret_cast<void*>(Item)));
        pas::free(Item);
        RefreshDerivedStats(true);
    }

    // Returns whether AI should remain docked; false does not guarantee full repair. Does not charge Money.
    std::uint8_t TShip::RepairHullAtLocation() {
        std::int32_t FriendlyCount{};
        std::int32_t I{};
        TShip* Ship{};
        std::uint8_t Result = false;
        if (GetHull()->HullPoints == GetHull()->Weight) {
            return Result;
        }
        std::int32_t Repair = System::Trunc(pas::real_divide(GetHull()->Weight, 1.0E+1L));
        if (Repair + GetHull()->HullPoints > GetHull()->Weight) {
            GetHull()->HullPoints = GetHull()->Weight;
            return Result;
        }
        GetHull()->HullPoints += Repair;
        if (CurrentStar->Status.Battle != 0) {
            FriendlyCount = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
                if (Ship->InNormalSpace() && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && !(Ship->TypeId == aGalaxyStruct::stPirate && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), Ship->OwnerId)) && Ship->TypeId != aGalaxyStruct::stKling && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), Ship->OwnerId) == pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), OwnerId)) {
                    ++FriendlyCount;
                }
            }
            return pas::idiv(GetHull()->HullPoints * 5, GetHull()->Weight) < FriendlyCount;
        }
        return true;
    }

    void TShip::ReloadWeaponAmmo() {
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Weapons[I];
                    if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        Weapon->Ammo = Weapon->AmmoCapacity;
                    }
                }
            }
        }
    }

    void TShip::ApplyCombatItemDegradation(double BaseDurabilityDamage) {
        static const pas::Set<0, 255> WearableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TEquipment* Artefact{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->BrokenFlag == 0 && pas::contains(WearableTypes, static_cast<std::uint8_t>(Item->ItemType))) {
                if (GetDefGenerator() == Item) {
                    aShip::TShip_ApplyItemDegradation(this, Item, idkBattle, BaseDurabilityDamage * 1.3L);
                } else if (GetRepairRobot() == Item) {
                    aShip::TShip_ApplyItemDegradation(this, Item, idkBattle, BaseDurabilityDamage * 1.25L);
                } else if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    aShip::TShip_ApplyItemDegradation(this, Item, idkBattle, BaseDurabilityDamage * 1.15L);
                } else {
                    aShip::TShip_ApplyItemDegradation(this, Item, idkBattle, BaseDurabilityDamage);
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
            Artefact = pas::list_at<aItem::TEquipment>(Artefacts, I);
            if (Artefact->BrokenFlag == 0 && pas::contains(WearableTypes, static_cast<std::uint8_t>(Artefact->ItemType))) {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(1, 10, RandomState);
                if (cpp_case >= 1 && cpp_case <= 6) {
                    continue;
                } else if (cpp_case == 7) {
                    aShip::TShip_ApplyItemDegradation(this, Artefact, idkBattle, BaseDurabilityDamage * 0.3L);
                } else if (cpp_case == 8) {
                    aShip::TShip_ApplyItemDegradation(this, Artefact, idkBattle, BaseDurabilityDamage * 0.6L);
                } else if (cpp_case == 9) {
                    aShip::TShip_ApplyItemDegradation(this, Artefact, idkBattle, BaseDurabilityDamage * 0.9L);
                } else if (cpp_case == 10) {
                    aShip::TShip_ApplyItemDegradation(this, Artefact, idkBattle, BaseDurabilityDamage * 1.2L);
                }
            }
        }
    }

    void TShip::ApplyArtefactUseDegradation(double BaseDurabilityDamage) {
        static const pas::Set<0, 255> WearableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        std::int32_t I{};
        aItem::TEquipment* Item{};
        if (pas::list_count(Artefacts) == 0) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
            if (!pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(Item->ItemType)) {
                Item->Repair();
            }
            if (Item->BrokenFlag == 0 && Item->EquippedFlag != 0 && pas::contains(WearableTypes, static_cast<std::uint8_t>(Item->ItemType))) {
                aShip::TShip_ApplyItemDegradation(this, Item, idkUse, BaseDurabilityDamage);
            }
        }
    }

    void TShip::ApplyAfterburnerItemDegradation() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        {
            double afterburnerWear = GetAfterburnerWear();
            aItem::TEquipment* engine = GetEngine();
            aShip::TShip_ApplyItemDegradation(this, engine, idkAfterburner, afterburnerWear);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
            if (Item->ItemType == aConst::t_ArtForsage && Item->EquippedFlag != 0) {
                aShip::TShip_ApplyItemDegradation(this, Item, idkAfterburner, 3.0);
                break;
            }
        }
    }

    // True only when the item becomes newly broken. Nil is accepted; script actions 36..39 can modify the damage.
    std::uint8_t TShip_ApplyItemDegradation(TShip* Self, aItem::TEquipment* Item, TItemDegradationKind Kind, double DurabilityDamage) {
        std::int32_t AfterScript{};
        std::int32_t AttackCount{};
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() == Self && (aGalaxy::Galaxy->TechnicModEnabled == 1 || aGalaxy::Galaxy->SpecialSimulationMode != 0)) {
            return Result;
        }
        if (Item == nullptr) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr && static_cast<std::uint8_t>(pas::in_set<aConst::t_Artefact, aConst::t_Artefact, aConst::t_ArtefactHull, aConst::t_ArtefactAntigrav, aConst::t_ArtDefToEnergy, aConst::t_ArtGiperJump, aConst::t_ArtBio, aConst::t_ArtFastRacks>(Item->ItemType) ^ 1)) {
            return Result;
        }
        if (Item->ConditionPercent < -95.0L) {
            return Result;
        }
        if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && Kind == idkUse) {
            AttackCount = Self->GetAttackMultiplier();
            if (AttackCount > 1) {
                DurabilityDamage = pas::real_divide(DurabilityDamage, AttackCount);
            }
        }
        if (aPlayer::GetPlayer() == Self) {
            DurabilityDamage = static_cast<long double>(DurabilityDamage) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[3]].EquipmentWearFactor;
            if (Self->IsHealthEffectActive(6)) {
                DurabilityDamage = DurabilityDamage * 2.0L;
            }
            if (Self->IsHealthEffectActive(16)) {
                DurabilityDamage = DurabilityDamage * 0.4L;
            }
        }
        if (aShip::TShip_CanUseEquipmentTech(Self, Item) && Self->GetEffectiveSkillLevel(psTechnical, false) > 0) {
            DurabilityDamage = pas::real_divide(DurabilityDamage, 1.0L + (Self->GetEffectiveSkillLevel(psTechnical, false) & 0x0000007f) * 0.2L);
        }
        DurabilityDamage = static_cast<long double>(DurabilityDamage) * Item->GetFragilityFactor(static_cast<aGalaxyStruct::TDamageFlagSet>(aGalaxyStruct::EmptyDamageFlags));
        std::int32_t BeforeScript = System::Round(DurabilityDamage * 1.0E+3L);
        switch (Kind) {
            case idkBattle: {
                AfterScript = Self->ScriptItemsAct(aConst::satOnReduceEqBattle, Item, nullptr, BeforeScript);
                break;
            }
            case idkUse: {
                AfterScript = Self->ScriptItemsAct(aConst::satOnReduceEqUse, Item, nullptr, BeforeScript);
                break;
            }
            case idkForce: {
                AfterScript = Self->ScriptItemsAct(aConst::satOnReduceEqForce, Item, nullptr, BeforeScript);
                break;
            }
            case idkAfterburner: {
                AfterScript = Self->ScriptItemsAct(aConst::satOnReduceEqForsage, Item, nullptr, BeforeScript);
                break;
            }
            default: AfterScript = BeforeScript; break;
        }
        if (BeforeScript != AfterScript) {
            DurabilityDamage = AfterScript * 0.001L;
        }
        if (DurabilityDamage < 0.0L) {
            DurabilityDamage = 0.0;
        }
        double NewCondition = static_cast<long double>(Item->ConditionPercent) - DurabilityDamage;
        Item->ConditionPercent = NewCondition;
        if (Item->ConditionPercent < 0.0L && Item->BrokenFlag == 0) {
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                if (Self->TypeId == aGalaxyStruct::stWarrior && Self->UsableWeaponCount < 3) {
                    Item->ConditionPercent = 1.0;
                    return Result;
                }
                pas::checked_cast<aItem::TWeapon*>(Item)->Target = nullptr;
            }
            Result = true;
            Item->BrokenFlag = 1;
            if (aPlayer::GetPlayer() == Self && Item->EquippedFlag != 0) {
                switch (Kind) {
                    case idkBattle: {
                        auto& cpp_target = ([&] {
                            const pas::WideString& brokenInBattleText = Item->GetBrokenInBattleText();
                            std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                            return Globals::AddOrUpdatePlayerBubble(8, currentTurn, brokenInBattleText, u""_wref.get());
                        }())->Targets[0].ShipId;
                        cpp_target = Self->Id;
                        break;
                    }
                    case idkUse:
                    case idkAfterburner: {
                        auto& cpp_target_2 = ([&] {
                            const pas::WideString& brokenInUseText = Item->GetBrokenInUseText();
                            std::int32_t currentTurn_2 = aGalaxy::Galaxy->CurrentTurn;
                            return Globals::AddOrUpdatePlayerBubble(8, currentTurn_2, brokenInUseText, u""_wref.get());
                        }())->Targets[0].ShipId;
                        cpp_target_2 = Self->Id;
                        break;
                    }
                    case idkForce: {
                        auto& cpp_target_3 = ([&] {
                            const pas::WideString& brokenByForceText = Item->GetBrokenByForceText();
                            std::int32_t currentTurn_3 = aGalaxy::Galaxy->CurrentTurn;
                            return Globals::AddOrUpdatePlayerBubble(8, currentTurn_3, brokenByForceText, u""_wref.get());
                        }())->Targets[0].ShipId;
                        cpp_target_3 = Self->Id;
                        break;
                    }
                }
                aRanger::PlayerEquipmentBrokenThisTurn = true;
            }
            if (aPlayer::GetPlayer() == Self && pas::class_cast_if<aItem::TSatellite*>(Item) != nullptr && static_cast<aItem::TSatellite*>(Item)->TargetPlanet != nullptr) {
                {
                    auto& cpp_target_4 = ([&] {
                        const pas::WideString& brokenInUseText_2 = pas::checked_cast<aItem::TSatellite*>(Item)->GetBrokenInUseText_2();
                        std::int32_t currentTurn_4 = aGalaxy::Galaxy->CurrentTurn;
                        return Globals::AddOrUpdatePlayerBubble(8, currentTurn_4, brokenInUseText_2, u""_wref.get());
                    }())->Targets[0].ShipId;
                    cpp_target_4 = Self->Id;
                }
                aPlayer::GetPlayer()->RefreshStorageBubbles();
            }
            Self->RefreshDerivedStats(true);
        }
        if (aPlayer::GetPlayer() == Self) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (std::fabs(static_cast<long double>(Item->ConditionPercent) - NewCondition) > 1.0E-5L && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
        return Result;
    }

    std::uint8_t TShip::CanGenerateMicroModuleForLoadout() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() == this) {
            return true;
        }
        if (!pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate, aGalaxyStruct::stWarrior>(TypeId)) {
            return Result;
        }
        if (!(DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(DockedTo) != nullptr || CurrentPlanet != nullptr && pas::in_set<0, 4, 7, 7>(CurrentPlanet->OwnerId))) {
            return Result;
        }
        std::int32_t Equipped = 0;
        std::int32_t Specials = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (!(pas::class_cast_if<aItem::THull*>(Item) != nullptr) && Item->EquippedFlag != 0) {
                ++Equipped;
                if (Item->SpecialModuleIndex != 0) {
                    ++Specials;
                }
            }
        }
        return Specials * 100 < static_cast<long double>(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 1.0, 8.0, 0.0, 1.0E+2)) * Equipped;
    }

    // True for the player or when the current hull already has a special module.
    std::uint8_t TShip::CanGenerateSpecialHullModule() {
        return aPlayer::GetPlayer() == this || GetHull()->SpecialModuleIndex != 0;
    }

    void TShip::ImproveRandomEquipment(std::uint8_t ResolveOverload) {
        std::int32_t Index{};
        aItem::TEquipment* Item{};
        std::int32_t BestCost = 0;
        aItem::TEquipment* Best = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(Index); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, Index);
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L && aShip::TShip_IsEquipmentUsable(this, Item) && pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && (Item->EquippedFlag != 0 || Item->ItemType == aConst::t_Hull) && Item->CanImprove() && pas::in_set<0, 4, 7, 7>(Item->OwnerId) && (!(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) || reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability != aGalaxyStruct::waNotSoldAndNodeRepair) && BestCost < Item->Cost) {
                BestCost = Item->Cost;
                Best = Item;
            }
        }
        if (Best != nullptr) {
            if (aGalaxy::Galaxy->TechLevel > 6 && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->StrengthInBestRanger > 0.6L && (StrengthInBestRanger < 0.5L || aMyFunction::NextRandomUnitFloat(RandomState) < 0.4L)) {
                Best->Improve(aItem::ikMajor);
            } else {
                Best->Improve(aItem::ikAny);
            }
        }
        if (Best != nullptr && aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L && Best->MicroModuleIndex == 0) {
            aShip::TShip_GenerateAndApplyMicroModule(this, Best, ResolveOverload);
            return;
        }
        BestCost = 0;
        Best = nullptr;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range_2.next(Index); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, Index);
            if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L && aShip::TShip_IsEquipmentUsable(this, Item) && (Item->EquippedFlag != 0 || Item->ItemType == aConst::t_Hull) && Item->MicroModuleIndex == 0 && BestCost < Item->Cost) {
                BestCost = Item->Cost;
                Best = Item;
            }
        }
        if (Best != nullptr && aMyFunction::NextRandomUnitFloat(RandomState) < 0.9L) {
            aShip::TShip_GenerateAndApplyMicroModule(this, Best, ResolveOverload);
        }
    }

    // Selects up to three qualifying candidates within 51 attempts, applies the best positive gain, then auto-equips. Nil Item is accepted.
    void TShip_GenerateAndApplyMicroModule(TShip* Self, aItem::TEquipment* Item, std::uint8_t ResolveOverload) {
        std::int32_t ModuleIndex{};
        std::int32_t Priority{};
        float Gain{};
        // Caller-popped static link; equipment -4, zero-based module index -8, ship -12. Uses strict remaining-capacity comparison.
        auto FitsGeneratedModuleCapacity = [&]() -> std::uint8_t {
            if (Item->ItemType != aConst::t_Hull) {
                return (aConst::MicroModuleTemplates[ModuleIndex].SizePercent - 100) * Item->Weight * 0.01L < Self->CargoFreeSpace;
            }
            return (aConst::MicroModuleTemplates[ModuleIndex].SizePercent - 100) * -Item->Weight * 0.01L < Self->CargoFreeSpace;
        };
        // Caller-popped static link; module index -8, ship -12. Random rejection for matching installed modules.
        auto AcceptDuplicate = [&]() -> std::uint8_t {
            std::int32_t Index{};
            aItem::TEquipment* Entry{};
            std::uint8_t Result = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->Inventory) - 1); cpp_range.next(Index); ) {
                Entry = pas::list_at<aItem::TEquipment>(Self->Inventory, Index);
                if (Entry->EquippedFlag != 0 && ModuleIndex + 1 == Entry->MicroModuleIndex && aMyFunction::NextRandomIntRange(0, 100, Self->RandomState) > 65) {
                    return Result;
                }
            }
            return true;
        };
        if (Item == nullptr) {
            return;
        }
        std::int32_t Attempts = 0;
        std::int32_t Accepted = 0;
        std::int32_t BestModule = -1;
        float BestGain = 0.0f;
        do {
            Priority = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 7.0, 7.0E+1, 0.0));
            {
                std::uint32_t advanceRandomSeed = aMyFunction::AdvanceRandomSeed(Self->RandomState);
                pas::Object* self = Self;
                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                ModuleIndex = aGalaxy::TGalaxy::SelectMicroModuleForEquipment(Priority + Accepted * 5, std::min<std::int32_t>(Priority + 40 + Accepted * 20, 100), advanceRandomSeed, self, Item);
            }
            if (aItem::CanInstallMicroModule(ModuleIndex, Item) && FitsGeneratedModuleCapacity() && AcceptDuplicate()) {
                Gain = Self->EvaluateMicroModuleGain(Item, ModuleIndex);
                if (Gain > BestGain) {
                    BestGain = Gain;
                    BestModule = ModuleIndex;
                }
                ++Accepted;
            }
            ++Attempts;
        } while (!(Attempts > 50 || Accepted >= 3));
        if (BestModule >= 0) {
            aItem::ApplyMicroModule(BestModule, Item);
            Self->AutoEquipInventory();
            if (ResolveOverload) {
                aShip::TShip_DropCargoUntilNotOverloaded(Self);
            }
        }
    }

    // Adds a generated weapon, may improve/module it, then auto-equips and optimizes inventory.
    void TShip_GenerateExtraWeapon(TShip* Self) {
        std::int32_t MaximumLevel{};
        std::int32_t MinimumLevel = 3;
        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.05L) {
            MaximumLevel = aGalaxy::Galaxy->TechLevel + 1;
        } else {
            MaximumLevel = aGalaxy::Galaxy->TechLevel;
        }
        MaximumLevel = std::max<std::int32_t>(MinimumLevel, std::min<std::int32_t>(MaximumLevel, 8));
        MinimumLevel = std::max<std::int32_t>(1, std::min<std::int32_t>(MinimumLevel, MaximumLevel - 2));
        aConst::PWeaponInfo Info = aGalaxy::Galaxy->SelectWeaponInfo(Self->RandomState, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}}), MaximumLevel, MinimumLevel);
        aItem::TWeapon* Weapon = ([&] {
            std::uint8_t ownerId = Self->OwnerId;
            std::int32_t nextRandomIntRange = ([&] {
                std::int32_t round = System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[3]);
                std::int32_t round_2 = System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[5]);
                return aMyFunction::NextRandomIntRange(round_2, round, Self->RandomState);
            }());
            std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinimumLevel, MaximumLevel, Self->RandomState);
            return aItem::CreateGeneratedWeapon(Info, nextRandomIntRange, nextRandomIntRange_2, ownerId);
        }());
        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Weapon));
        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.5L) {
            if (aGalaxy::Galaxy->TechLevel > 6 && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->StrengthInBestRanger > 0.6L && (Self->StrengthInBestRanger < 0.5L || aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.4L)) {
                Weapon->Improve(aItem::ikMajor);
            } else {
                Weapon->Improve(aItem::ikAny);
            }
        }
        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.5L) {
            aShip::TShip_GenerateAndApplyMicroModule(Self, Weapon, true);
        }
        Self->AutoEquipInventory();
        Self->OptimizeInventory();
    }

    // Requires an installable target and no carried module of the same index.
    std::uint8_t TShip::NeedsMicroModule(std::int32_t ModuleIndexPlusOne) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::uint8_t Result = false;
        std::uint8_t Found = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (aItem::CanInstallMicroModule(ModuleIndexPlusOne - 1, Item)) {
                Found = true;
                break;
            }
        }
        if (Found) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (pas::class_cast_if<aItem::TMicroModule*>(Item) != nullptr && Item->MicroModuleIndex == ModuleIndexPlusOne) {
                    return Result;
                }
            }
            return true;
        }
        return Result;
    }

    // Skips inventory index 0.
    std::int32_t TShip::CountUnequippedDominatorEquipment() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Item->EquippedFlag == 0) {
                ++Result;
            }
        }
        return Result;
    }

    std::int32_t TShip::GetSatelliteLimit() {
        std::int32_t cpp_left = aConst::TechnicalSkillSatelliteLimits[GetEffectiveSkillLevel(psTechnical, false) & 0x0000007f];
        return cpp_left + GetTotalStatBonus(aConst::bonZonds);
    }

    // Includes protected/script-named items and indispensable installed equipment; nil is false.
    std::uint8_t TShip::IsEssentialInventoryItem(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (Item == nullptr) {
            return Result;
        }
        if (Item->NoDropFlag > 0 || Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name != u"") {
            return true;
        }
        if (reinterpret_cast<aItem::TEquipment*>(Item)->EquippedFlag == 0 && Item->ItemType != aConst::t_Hull) {
            return Result;
        }
        Result = true;
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_Engine))) {
            return Result;
        }
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && WeaponCount <= 1) {
            return Result;
        }
        if (Item->ItemType == aConst::t_CargoHook && pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(TypeId) && GetSlotCount(aConst::sskCargoHook) > 0) {
            return Result;
        }
        return false;
    }

    // Radar/scanner, or cargo hook on a ship other than a ranger or pirate. Nil is false.
    std::uint8_t TShip::IsOptionalUtilityEquipment(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (Item != nullptr) {
            if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_Radar), static_cast<std::int32_t>(aConst::t_Scaner))) {
                return true;
            } else if (Item->ItemType == aConst::t_CargoHook && static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(TypeId) ^ 1)) {
                return true;
            } else {
                return Result;
            }
        }
        return Result;
    }

    std::uint8_t TShip::NeedsEssentialEquipment() {
        std::uint8_t Result = true;
        if (Speed > 0 && WeaponCount > 0 && (static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(TypeId) ^ 1) || GetCargoHook() != nullptr || GetSlotCount(aConst::sskCargoHook) <= 0)) {
            return false;
        }
        return Result;
    }

    // Can subsidize equipment purchases; does not guarantee success.
    void TShip::RestoreEssentialEquipment() {
        std::int32_t Subsidy{};
        std::int32_t OriginalMoney{};
        if (NeedsEssentialEquipment()) {
            Subsidy = 1000;
            OriginalMoney = Money;
            while (NeedsEssentialEquipment() && Subsidy < 1000000) {
                aShip::TShip_BuyEquipmentAtLocation(this, true);
                if (!NeedsEssentialEquipment()) {
                    break;
                }
                SetMoney(Money + Subsidy);
                CalculateWealth();
                Subsidy = System::Round(Subsidy * 1.3L);
            }
            if (Money > OriginalMoney) {
                SetMoney(OriginalMoney);
            }
            CalculateWealth();
        }
    }

    void TShip_BuyEquipmentAtLocation(TShip* Self, std::uint8_t ForceGeneratedOffers) {
        aItem::TWeapon* ReplacementWeapon{};
        pas::Array<aItem::TWeapon*, 1, 5> SavedWeapons{};
        pas::Object* SavedTarget{};
        aItem::TEquipment* OfferItem{};
        aItem::TEquipment* OldItem{};
        pas::List* Offers{};
        float NewScore{};
        float OldScore{};
        float NewEffectiveness{};
        float OldEffectiveness{};
        std::uint8_t DifferentHullGraph{};
        // Caller-popped static link; ship -4. Selects the lowest evaluated weapon when all slots are occupied.
        auto SelectReplacedWeapon = [&]() -> aItem::TWeapon* {
            std::int32_t J{};
            aItem::TWeapon* Result = nullptr;
            if (Self->GetSlotCount(aConst::sskWeapon) > Self->WeaponCount) {
                return Result;
            }
            std::int32_t Best = 1;
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
                if (2 <= cpp_last) {
                    for (J = 2; J <= cpp_last; ++J) {
                        pas::Extended cpp_left = Self->EvaluateItem(Self->Weapons[Best], 3);
                        if (cpp_left > Self->EvaluateItem(Self->Weapons[J], 3)) {
                            Best = J;
                        }
                    }
                }
            }
            return Self->Weapons[Best];
        };
        // Caller-popped static link; ship -4, fallback weapon -8; saves five weapon pointers and target at -32.
        auto TemporarilyUnequipWeapon = [&](aItem::TWeapon* Weapon) -> void {
            std::int32_t J{};
            aItem::TWeapon* Selected = Weapon;
            if (Selected == nullptr) {
                Selected = ReplacementWeapon;
            }
            std::int32_t WeaponIndex = 0;
            for (J = 1; J <= 5; ++J) {
                SavedWeapons[J] = Self->Weapons[J];
                if (SavedWeapons[J] == Selected) {
                    WeaponIndex = J;
                }
            }
            if (WeaponIndex != 0) {
                SavedTarget = Selected->Target;
                Self->UnequipSlot(static_cast<std::uint8_t>(Selected->ItemType), WeaponIndex);
            }
        };
        // Caller-popped static link; ship -4, weapon -8; restores saved target and weapon pointers.
        auto RestoreWeapon = [&]() -> void {
            std::int32_t J{};
            Self->EquipItem(ReplacementWeapon);
            ReplacementWeapon->Target = SavedTarget;
            for (J = 1; J <= 5; ++J) {
                Self->Weapons[J] = SavedWeapons[J];
            }
        };
        std::uint8_t RestoreStock = false;
        std::uint8_t GeneratedBatch = false;
        std::uint8_t UseMoney = true;
        std::uint32_t SavedNextItemId = aGalaxy::Galaxy->NextItemId;
        std::uint8_t Bought = false;
        std::int32_t OriginalMoney = Self->Money;
        if (Self->CurrentPlanet != nullptr) {
            if (fEquipmentShop::TemporaryShopSlots != nullptr && fEquipmentShop::TemporaryShopPlanet == Self->CurrentPlanet) {
                fEquipmentShop::RestoreTemporaryShopStock();
                RestoreStock = true;
            }
            if (aGalaxy::Galaxy->IsAIShoppingEnabled() && static_cast<std::uint8_t>(ForceGeneratedOffers ^ 1) && (!(pas::class_cast_if<aWarrior::TWarrior*>(Self) != nullptr) || pas::checked_cast<aWarrior::TWarrior*>(Self)->WarriorType != aWarrior::wtFlagship)) {
                Offers = Self->CurrentPlanet->EquipmentShop;
            } else {
                Offers = aPlanet::TPlanet_BuildEquipmentOfferBatch(Self->CurrentPlanet, Self, ForceGeneratedOffers);
                GeneratedBatch = true;
            }
        } else if (Self->DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(Self->DockedTo) != nullptr) {
            if (fEquipmentShop::TemporaryShopSlots != nullptr && fEquipmentShop::TemporaryShopStation == Self->DockedTo) {
                fEquipmentShop::RestoreTemporaryShopStock();
                RestoreStock = true;
            }
            if (aGalaxy::Galaxy->IsAIShoppingEnabled() && static_cast<std::uint8_t>(ForceGeneratedOffers ^ 1) && (!(pas::class_cast_if<aWarrior::TWarrior*>(Self) != nullptr) || pas::checked_cast<aWarrior::TWarrior*>(Self)->WarriorType != aWarrior::wtFlagship)) {
                Offers = reinterpret_cast<aRuins::TRuins*>(Self->DockedTo)->EquipmentShop;
            } else {
                Offers = aRuins::TRuins_GenerateEquipmentOfferBatch(reinterpret_cast<aRuins::TRuins*>(Self->DockedTo), Self, ForceGeneratedOffers);
                GeneratedBatch = true;
            }
        } else if (pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr && aGalaxy::Galaxy->IsStationShopUpdateEnabled()) {
            if (fEquipmentShop::TemporaryShopSlots != nullptr && fEquipmentShop::TemporaryShopStation == Self) {
                fEquipmentShop::RestoreTemporaryShopStock();
                RestoreStock = true;
            }
            Offers = reinterpret_cast<aRuins::TRuins*>(Self)->EquipmentShop;
            UseMoney = false;
        } else {
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip != nullptr && Self->CurrentStar == nullptr && Self->CurrentPlanet == nullptr && Self->DockedTo == nullptr) {
                Self->CurrentStar = reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip->CurrentStar;
                Self->CurrentPlanet = reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip->CurrentPlanet;
                Self->DockedTo = reinterpret_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip->DockedTo;
                if (Self->CurrentPlanet != nullptr || Self->DockedTo != nullptr) {
                    aShip::TShip_BuyEquipmentAtLocation(Self, ForceGeneratedOffers);
                    Self->CurrentPlanet = nullptr;
                    Self->DockedTo = nullptr;
                }
                Self->CurrentStar = nullptr;
            }
            return;
        }
        Self->RefreshEquipmentEvaluationMetrics();
        std::int32_t I = pas::list_count(Self->Inventory);
        if (UseMoney) {
            while (I > 0) {
                --I;
                OldItem = pas::list_at<aItem::TEquipment>(Self->Inventory, I);
                if (OldItem->ItemType != aConst::t_Hull && OldItem->EquippedFlag != 0 && static_cast<std::uint8_t>(Self->IsEssentialInventoryItem(OldItem) ^ 1) && Self->EvaluateItem(OldItem, 2) <= 0.0L && (OldItem->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(OldItem->ScriptItem)->Name == u"") && OldItem->NoDropFlag <= 0) {
                    if (pas::class_cast_if<aItem::TWeapon*>(OldItem) != nullptr) {
                        TemporarilyUnequipWeapon(reinterpret_cast<aItem::TWeapon*>(OldItem));
                    } else {
                        Self->UnequipSlot(static_cast<std::uint8_t>(OldItem->ItemType), 0);
                    }
                    aShip::TShip_LiquidateInventoryItem(Self, OldItem);
                    I = pas::list_count(Self->Inventory);
                }
            }
        }
        ReplacementWeapon = SelectReplacedWeapon();
        std::int32_t FreeWeight = Self->CargoFreeSpace - std::max<std::int32_t>(0, Self->GetDesiredCargoFreeSpace() - Self->GetCargoGoodsWeight());
        float BestGain = 0.0f;
        aItem::TEquipment* BestItem = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Offers) - 1); cpp_range.next(I); ) {
            OfferItem = pas::list_at<aItem::TEquipment>(Offers, I);
            if (static_cast<std::uint8_t>(pas::in_range(OfferItem->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon)) ^ 1) || OfferItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(OfferItem->ScriptItem)->Name != u"" || OfferItem->NoDropFlag > 0 || static_cast<std::uint8_t>(pas::in_range(OfferItem->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_Engine)) ^ 1) && Self->GetSlotCountForItemType(static_cast<std::uint8_t>(OfferItem->ItemType)) == 0) {
                continue;
            }
            if (OfferItem->ItemType == aConst::t_Hull) {
                if (pas::class_cast_if<aRuins::TRuins*>(Self) != nullptr || Self->GetHull()->HullType != pas::checked_cast<aItem::THull*>(OfferItem)->HullType || Self->GetHull()->OwnerId != OfferItem->OwnerId || (pas::checked_cast<aItem::THull*>(OfferItem)->SpecialModuleIndex == 0) != (Self->GetHull()->SpecialModuleIndex == 0)) {
                    continue;
                }
                if (Self->GetHull()->SpecialModuleIndex != 0) {
                    Self->GetHull()->OwnerShip = nullptr;
                    DifferentHullGraph = ([&] {
                        pas::WideString cpp_string = pas::checked_cast<aItem::THull*>(OfferItem)->GetSpecialKindGraph();
                        pas::WideString cpp_string_2 = Self->GetHull()->GetSpecialKindGraph();
                        return cpp_string != cpp_string_2;
                    }());
                    Self->GetHull()->OwnerShip = Self;
                    if (DifferentHullGraph) {
                        continue;
                    }
                }
            }
            if (pas::class_cast_if<aItem::TWeapon*>(OfferItem) != nullptr) {
                OldItem = ReplacementWeapon;
            } else {
                OldItem = pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(Self)->Slots, (static_cast<std::uint8_t>(OfferItem->ItemType) - 42) * sizeof(aItem::TEquipment*)));
            }
            if (pas::in_range(OfferItem->ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_Engine)) && OldItem == nullptr && (static_cast<std::uint8_t>(UseMoney ^ 1) || Self->Money >= OfferItem->Cost)) {
                BestItem = OfferItem;
                break;
            }
            OldScore = 0.0f;
            OldEffectiveness = 0.0f;
            NewEffectiveness = 0.0f;
            if (OldItem != nullptr) {
                if (OfferItem->ItemType != aConst::t_Hull && FreeWeight + OldItem->Weight - OfferItem->Weight < 0 || OfferItem->ItemType == aConst::t_Hull && FreeWeight - OldItem->Weight + OfferItem->Weight < 0 || OldItem->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(OldItem->ScriptItem)->Name != u"" || OldItem->NoDropFlag > 0) {
                    continue;
                }
                if (pas::class_cast_if<aItem::TWeapon*>(OldItem) != nullptr) {
                    TemporarilyUnequipWeapon(nullptr);
                } else if (!(pas::class_cast_if<aItem::THull*>(OldItem) != nullptr)) {
                    Self->UnequipSlot(static_cast<std::uint8_t>(OldItem->ItemType), 0);
                }
                OldEffectiveness = Self->CalculateItemEffectiveness(OldItem);
                NewEffectiveness = Self->CalculateItemEffectiveness(OfferItem);
                if (Self->TypeId == aGalaxyStruct::stRanger) {
                    pas::Extended cpp_left = Self->AdjustItemEvaluation(OfferItem, 2, NewEffectiveness) * 1.2L;
                    if (cpp_left > Self->AdjustItemEvaluation(OldItem, 2, OldEffectiveness)) {
                        if (Self->Money < OfferItem->Cost) {
                            Self->EquipmentPriceSensitivity = 0.99L * Self->EquipmentPriceSensitivity + 0.01L;
                        } else {
                            Self->EquipmentPriceSensitivity = 0.99L * Self->EquipmentPriceSensitivity;
                        }
                    }
                }
                if (UseMoney && OldItem->CalculateResaleValue(Self->GetEffectiveSkillLevel(psTrading, false)) + Self->Money - OfferItem->Cost < 0) {
                    if (!(pas::class_cast_if<aItem::THull*>(OldItem) != nullptr)) {
                        if (pas::class_cast_if<aItem::TWeapon*>(OldItem) != nullptr) {
                            RestoreWeapon();
                        } else {
                            Self->EquipItem(OldItem);
                        }
                    }
                    continue;
                }
                OldScore = Self->AdjustItemEvaluation(OldItem, 3, OldEffectiveness);
            } else {
                if (FreeWeight - OfferItem->Weight < 0 || UseMoney && Self->Money - OfferItem->Cost < 0) {
                    continue;
                }
                NewEffectiveness = Self->CalculateItemEffectiveness(OfferItem);
            }
            if (NewEffectiveness < OldEffectiveness) {
                NewEffectiveness = NewEffectiveness - std::fabs(static_cast<pas::Extended>(NewEffectiveness)) * 0.1L;
            }
            NewScore = Self->AdjustItemEvaluation(OfferItem, 4, NewEffectiveness);
            if (OldItem != nullptr && !(pas::class_cast_if<aItem::THull*>(OldItem) != nullptr)) {
                if (pas::class_cast_if<aItem::TWeapon*>(OldItem) != nullptr) {
                    RestoreWeapon();
                } else {
                    Self->EquipItem(OldItem);
                }
            }
            if (static_cast<long double>(NewScore) - OldScore > BestGain) {
                BestGain = static_cast<long double>(NewScore) - OldScore;
                BestItem = OfferItem;
            }
        }
        if (BestItem != nullptr) {
            if (pas::class_cast_if<aItem::TWeapon*>(BestItem) != nullptr) {
                OldItem = ReplacementWeapon;
            } else {
                OldItem = pas::load_unaligned<aItem::TEquipment*>(pas::byte_offset(&reinterpret_cast<PShipEquipmentCacheView>(Self)->Slots, (static_cast<std::uint8_t>(BestItem->ItemType) - 42) * sizeof(aItem::TEquipment*)));
            }
            pas::list_delete(Offers, pas::list_indexof(Offers, reinterpret_cast<void*>(BestItem)));
            Bought = true;
            if (OldItem != nullptr) {
                if (pas::class_cast_if<aItem::TWeapon*>(OldItem) != nullptr) {
                    TemporarilyUnequipWeapon(nullptr);
                } else {
                    Self->UnequipSlot(static_cast<std::uint8_t>(OldItem->ItemType), 0);
                }
                if (UseMoney) {
                    Self->SetMoney(OldItem->CalculateResaleValue(Self->GetEffectiveSkillLevel(psTrading, false)) + Self->Money);
                }
                pas::list_delete(Self->Inventory, pas::list_indexof(Self->Inventory, reinterpret_cast<void*>(OldItem)));
                pas::free(OldItem);
            }
            if (UseMoney) {
                Self->SetMoney(Self->Money - BestItem->Cost);
            }
            if (pas::class_cast_if<aItem::THull*>(BestItem) != nullptr) {
                pas::list_insert(Self->Inventory, 0, reinterpret_cast<void*>(BestItem));
            } else {
                pas::list_add(Self->Inventory, reinterpret_cast<void*>(BestItem));
            }
            Self->EquipItem(BestItem);
            if (pas::class_cast_if<aItem::THull*>(BestItem) != nullptr) {
                Self->RefreshGraphicSize();
            }
        }
        if (RestoreStock) {
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            Globals::EquipmentShopScreen->ClearGoodsControls();
            Globals::EquipmentShopScreen->BuildGoodsControls();
            Globals::EquipmentShopScreen->UpdateScrollButtons();
        }
        if (GeneratedBatch) {
            pas::free(Offers);
        }
        if (GeneratedBatch && static_cast<std::uint8_t>(Bought ^ 1)) {
            aGalaxy::Galaxy->NextItemId = SavedNextItemId;
        }
        Self->OptimizeInventory();
        Self->RefreshDerivedStats(true);
        if (Self->Money > OriginalMoney) {
            aShip::TShip_BuyEquipmentAtLocation(Self, ForceGeneratedOffers);
        }
    }

    aItem::THull* TShip_CreateAndEquipHull(TShip* Self, std::uint16_t Capacity, std::uint8_t Level, std::uint8_t Owner, std::int32_t Series, std::uint8_t PirateBuilt) {
        aItem::THull* Item = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
        std::uint8_t Kind = Self->GetDefaultHullType();
        Item->Init(Capacity, Level, Owner, Kind, Series, PirateBuilt);
        pas::list_add(Self->Inventory, reinterpret_cast<void*>(Item));
        Self->EquipItem(Item);
        Item->OwnerShip = Self;
        aItem::THull* Result = Item;
        aShip::TShip_RefreshGraphic(Self);
        return Result;
    }

    // Normal ships use PilotRace rather than OwnerId; selects rarity 1..100 through the galaxy RNG.
    std::int32_t TShip::SelectRandomHullSeries() {
        if (pas::class_cast_if<aNormalShip::TNormalShip*>(this) != nullptr) {
            std::uint8_t defaultHullType = GetDefaultHullType();
            std::uint8_t raceToOwner = aConst::RaceToOwner(PilotRace);
            return aGalaxy::Galaxy->SelectHullSeries(raceToOwner, defaultHullType, 1, 100);
        }
        return aGalaxy::Galaxy->SelectHullSeries(OwnerId, GetDefaultHullType(), 1, 100);
    }

    aItem::TFuelTanks* TShip::CreateAndEquipFuelTanks(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TFuelTanks* Item = pas::construct_call<aItem::TFuelTanks>(aItem::TEquipment_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TEngine* TShip::CreateAndEquipEngine(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TEngine* Item = pas::construct_call<aItem::TEngine>(aItem::TEquipment_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TRadar* TShip::CreateAndEquipRadar(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TRadar* Item = pas::construct_call<aItem::TRadar>(aItem::TEquipment_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TScaner* TShip::CreateAndEquipScanner(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TScaner* Item = pas::construct_call<aItem::TScaner>(aItem::TEquipment_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TRepairRobot* TShip::CreateAndEquipRepairRobot(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TRepairRobot* Item = pas::construct_call<aItem::TRepairRobot>(aItem::TEquipment_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TCargoHook* TShip::CreateAndEquipCargoHook(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TCargoHook* Item = pas::construct_call<aItem::TCargoHook>(aItem::TCargoHook_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TDefGenerator* TShip::CreateAndEquipDefGenerator(std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TDefGenerator* Item = pas::construct_call<aItem::TDefGenerator>(aItem::TEquipment_Create);
        Item->Init(Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    aItem::TWeapon* TShip::CreateAndEquipWeapon(std::uint8_t ItemType, std::int32_t Weight, std::uint8_t Level, std::uint8_t Owner) {
        aItem::TWeapon* Item = pas::construct_call<aItem::TWeapon>(aItem::TEquipment_Create);
        Item->Init(static_cast<aConst::TItemType>(ItemType), Weight, Level, Owner);
        pas::list_add(Inventory, reinterpret_cast<void*>(Item));
        EquipItem(Item);
        return Item;
    }

    std::uint8_t TShip::ScanForCollectableItems() {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::uint8_t Result = false;
        if (aShip::TShip_IsEquipmentUsable(this, GetCargoHook())) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                if (aItem::CanCargoHookHandleItem(Item, this) && AcceptPickupItem(Item) && ShouldPickUpItem(Item) && CountOtherShipsTargetingItem(Item) <= 0 && AcceptPickupDistance(Item, aMyFunction::PointDistance(Position, Item->Position))) {
                    return true;
                }
            }
        }
        return Result;
    }

    void TShip::QueueItemsWithinPickupRange() {
        std::int32_t I{};
        aItem::TItem* Item{};
        if (aShip::TShip_IsEquipmentUsable(this, GetCargoHook()) && Speed > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                if (aShip::TShip_IsItemInPickupRange(this, Item) && static_cast<std::uint8_t>(IsRecentlyDroppedItem(Item) ^ 1) && AcceptPickupItem(Item) && ShouldPickUpItem(Item)) {
                    AddPickupTarget(Item, false);
                }
            }
        }
    }

    // May queue nearby pickups and issue/cancel a move order; true means a move order remains.
    std::uint8_t TShip::TryCollectBestFloatingItem(std::int32_t MaximumTravelTurns) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TItem* BestItem{};
        std::uint8_t Found{};
        double Distance{};
        double BestDistance{};
        aGalaxy::PMovingDropItemEntry Drop{};
        EC_Struct::TPointF ItemPosition{};
        std::uint8_t Result = false;
        if (aShip::TShip_IsEquipmentUsable(this, GetCargoHook()) && Speed >= 1) {
            Found = false;
            BestItem = nullptr;
            BestDistance = 1.0E+4;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                if (aItem::CanCargoHookHandleItem(Item, this) && static_cast<std::uint8_t>(IsRecentlyDroppedItem(Item) ^ 1) && AcceptPickupItem(Item) && ShouldPickUpItem(Item) && (CountOtherShipsTargetingItem(Item) < 2 || CanReachItemBeforeOtherShips(Item))) {
                    if (aShip::TShip_IsItemInPickupRange(this, Item)) {
                        AddPickupTarget(Item, false);
                    } else {
                        Distance = aMyFunction::PointDistance(Position, Item->Position);
                        if (MaximumTravelTurns >= pas::real_divide(Distance, Speed) && AcceptPickupDistance(Item, Distance) && (static_cast<std::uint8_t>(OrderAbsolute ^ 1) || Order == soMove) && (BestItem == nullptr || 10 * BestItem->Cost < Item->Cost && BestItem->Cost > Wealth * 0.01L || 1.3L * Distance < BestDistance && BestItem->Cost < 2 * Item->Cost)) {
                            Found = true;
                            BestItem = Item;
                            BestDistance = Distance;
                            ItemPosition = Item->Position;
                        }
                    }
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->MovingDropItems) - 1); cpp_range_2.next(I); ) {
                Drop = pas::list_at<aGalaxy::TMovingDropItemEntry>(CurrentStar->MovingDropItems, I);
                Item = reinterpret_cast<aItem::TItem*>(Drop->Payload);
                if (aItem::CanCargoHookHandleItem(Item, this) && static_cast<std::uint8_t>(IsRecentlyDroppedItem(Item) ^ 1) && AcceptPickupItem(Item) && ShouldPickUpItem(Item)) {
                    Distance = aMyFunction::PointDistance(Position, Drop->Destination);
                    if (MaximumTravelTurns >= pas::real_divide(Distance, Speed) && AcceptPickupDistance(Item, Distance) && (static_cast<std::uint8_t>(OrderAbsolute ^ 1) || Order == soMove) && (BestItem == nullptr || 10 * BestItem->Cost < Item->Cost && BestItem->Cost > Wealth * 0.01L || 1.3L * Distance < BestDistance && BestItem->Cost < 2 * Item->Cost)) {
                        Found = true;
                        BestItem = Item;
                        BestDistance = Distance;
                        ItemPosition = Drop->Destination;
                    }
                }
            }
            if (BestItem != nullptr) {
                OrderMove(GetPickupApproachPosition(ItemPosition), false);
            }
            if (static_cast<std::uint8_t>(Found ^ 1) && Order == soMove) {
                OrderNone(false);
            }
            if (Order == soMove) {
                return true;
            }
        }
        return Result;
    }

    std::int32_t TShip::GetReservedPickupWeight() {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Result = 0;
        if (PickupTargets != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PickupTargets) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(PickupTargets, I);
                Result += Item->Weight;
            }
        }
        return Result;
    }

    // Also checks hook eligibility.
    std::uint8_t TShip_IsItemInPickupRange(TShip* Self, aItem::TItem* Item) {
        return aItem::CanCargoHookHandleItem(Item, Self) && static_cast<long double>(aMyFunction::PointDistanceSquared(Self->Position, Item->Position)) <= Self->GetCargoHookRangeSquared();
    }

    // Base implementation returns false.
    std::uint8_t TShip::AcceptPickupItem(aItem::TItem* Item) {
        return false;
    }

    std::uint8_t TShip::ShouldPickUpItem(aItem::TItem* Item) {
        std::int32_t I{};
        std::int32_t EquippedWeight{};
        aItem::TEquipment* Other{};
        float EquippedValue{};
        std::int32_t LooseWeight{};
        std::int32_t LooseCost{};
        std::uint8_t Result = true;
        if (PickupTargets != nullptr && pas::list_indexof(PickupTargets, reinterpret_cast<void*>(Item)) >= 0 && CargoFreeSpace > 0 || CargoFreeSpace - GetReservedPickupWeight() >= Item->Weight && (aGalaxy::Galaxy->SpecialSimulationMode == 0 || pas::class_cast_if<aItem::TMicroModule*>(Item) != nullptr) || pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr && CargoFreeSpace - GetReservedPickupWeight() > 0 && aGalaxy::Galaxy->SpecialSimulationMode == 0 || pas::class_cast_if<aItem::TGoods*>(Item) != nullptr && CargoFreeSpace - GetReservedPickupWeight() > 0 && aGalaxy::Galaxy->SpecialSimulationMode == 0) {
            return Result;
        }
        Result = false;
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator)) && aShip::TShip_IsEquipmentUsable(this, pas::checked_cast<aItem::TEquipment*>(Item))) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Other = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (Item->ItemType == Other->ItemType && Other->Weight + CargoFreeSpace > Item->Weight && Other->EquippedFlag != 0) {
                    pas::Extended cpp_left = EvaluateItem(Item, 1);
                    if (cpp_left > EvaluateItem(Other, 1)) {
                        return true;
                    }
                }
            }
        } else if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && aShip::TShip_IsEquipmentUsable(this, pas::checked_cast<aItem::TEquipment*>(Item))) {
            EquippedWeight = 0;
            EquippedValue = 0.0f;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range_2.next(I); ) {
                Other = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (pas::in_range(Other->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && Other->EquippedFlag != 0) {
                    EquippedValue = static_cast<long double>(EquippedValue) + EvaluateItem(Other, 1);
                    EquippedWeight += Other->Weight;
                }
            }
            if (EquippedWeight + CargoFreeSpace >= Item->Weight && ([&] {
                pas::Extended cpp_left_3 = EvaluateItem(Item, 1);
                pas::Extended cpp_left_2 = pas::real_divide(cpp_left_3, std::max<std::int32_t>(1, Item->Weight));
                return cpp_left_2 > pas::real_divide(EquippedValue, std::max<std::int32_t>(1, EquippedWeight));
            }())) {
                return true;
            }
        }
        if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
            LooseWeight = 0;
            LooseCost = 0;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range_3.next(I); ) {
                Other = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (Other->EquippedFlag == 0) {
                    LooseWeight += Other->Weight;
                    LooseCost += Other->Cost;
                }
            }
            return LooseWeight + CargoFreeSpace >= Item->Weight && ([&] {
                pas::Extended cpp_left_4 = pas::real_divide(Item->Cost, std::max<std::int32_t>(1, Item->Weight));
                return cpp_left_4 > pas::real_divide(LooseCost, std::max<std::int32_t>(1, LooseWeight));
            }());
        }
        return Result;
    }

    // Base implementation returns true.
    std::uint8_t TShip::AcceptPickupDistance(aItem::TItem* Item, double Distance) {
        return true;
    }

    // Existing targets keep their position.
    void TShip::AddPickupTarget(aItem::TItem* Item, std::uint8_t Prioritize) {
        if (PickupTargets == nullptr) {
            PickupTargets = pas::make_object<pas::List>();
        }
        if (pas::list_indexof(PickupTargets, reinterpret_cast<void*>(Item)) < 0) {
            if (Prioritize) {
                pas::list_insert(PickupTargets, 0, reinterpret_cast<void*>(Item));
            } else {
                pas::list_add(PickupTargets, reinterpret_cast<void*>(Item));
            }
        }
    }

    void TShip::RemovePickupTarget(aItem::TItem* Item) {
        if (PickupTargets == nullptr) {
            return;
        }
        std::int32_t I = pas::list_indexof(PickupTargets, reinterpret_cast<void*>(Item));
        if (I >= 0) {
            pas::list_delete(PickupTargets, I);
            if (pas::list_count(PickupTargets) < 1) {
                pas::free(PickupTargets);
                PickupTargets = nullptr;
            }
        }
    }

    void TShip::ClearPickupTargets() {
        if (PickupTargets != nullptr) {
            pas::free(PickupTargets);
            PickupTargets = nullptr;
        }
    }

    // Frees the target list when it becomes empty.
    void TShip::RemoveInvalidPickupTargets() {
        std::int32_t I{};
        if (PickupTargets != nullptr) {
            I = 0;
            while (I < pas::list_count(PickupTargets)) {
                if (!aItem::CanCargoHookHandleItem(pas::list_at<aItem::TItem>(PickupTargets, I), this)) {
                    pas::list_delete(PickupTargets, I);
                } else {
                    ++I;
                }
            }
            if (pas::list_count(PickupTargets) < 1) {
                pas::free(PickupTargets);
                PickupTargets = nullptr;
            }
        }
    }

    // Adds missing eligible targets; removes eligible targets only if none were added.
    void TShip::TogglePickupTargets(std::uint8_t IgnoreRange) {
        std::int32_t I{};
        aItem::TItem* Item{};
        // Caller-popped static link; mode -1, ship -8.
        auto AcceptPickupTarget = [&](aItem::TItem* Item) -> std::uint8_t {
            std::uint8_t Result = false;
            if (IgnoreRange) {
                if (aItem::CanCargoHookHandleItem(Item, this)) {
                    return true;
                }
                return Result;
            } else if (aShip::TShip_IsItemInPickupRange(this, Item)) {
                return true;
            } else {
                return Result;
            }
        };
        std::uint8_t Added = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
            if (AcceptPickupTarget(Item) && static_cast<std::uint8_t>(HasPickupTarget(Item) ^ 1)) {
                AddPickupTarget(Item, false);
                Added = true;
            }
        }
        if (!Added) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                if (AcceptPickupTarget(Item)) {
                    RemovePickupTarget(Item);
                }
            }
        }
    }

    std::uint8_t TShip::HasPickupTarget(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (PickupTargets != nullptr) {
            if (pas::list_indexof(PickupTargets, reinterpret_cast<void*>(Item)) >= 0) {
                return true;
            }
        }
        return Result;
    }

    std::int32_t TShip::CountOtherShipsTargetingItem(aItem::TItem* Item) {
        std::int32_t I{};
        TShip* Ship{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
            if (Ship != this && Ship->PickupTargets != nullptr && pas::list_indexof(Ship->PickupTargets, reinterpret_cast<void*>(Item)) >= 0) {
                ++Count;
            }
        }
        return Count;
    }

    // Ties are allowed; requires positive speed.
    std::uint8_t TShip::CanReachItemBeforeOtherShips(aItem::TItem* Item) {
        std::int32_t I{};
        TShip* Ship{};
        if (Speed < 1) {
            return false;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
            if (Ship != this && Ship->InNormalSpace() && Ship->Speed >= 1) {
                if (Ship->GetPickupApproachPosition(Item->Position).X == Ship->OrderDestination.X && Ship->GetPickupApproachPosition(Item->Position).Y == Ship->OrderDestination.Y) {
                    pas::Extended cpp_left = pas::real_divide(aMyFunction::PointDistance(Ship->Position, Item->Position), Ship->Speed);
                    if (cpp_left < pas::real_divide(aMyFunction::PointDistance(Position, Item->Position), Speed)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    EC_Struct::TPointF TShip::GetPickupApproachPosition(EC_Struct::TPointF ItemPosition) {
        EC_Struct::TPointF Result{};
        std::int32_t Direction{};
        if (ItemPosition.X < Position.X) {
            Direction = -1;
        } else {
            Direction = 1;
        }
        Result.X = static_cast<long double>(ItemPosition.X) + GetCargoHookRange() / 2 * Direction;
        if (ItemPosition.Y < Position.Y) {
            Direction = -1;
        } else {
            Direction = 1;
        }
        Result.Y = static_cast<long double>(ItemPosition.Y) + GetCargoHookRange() / 2 * Direction;
        return Result;
    }

    aItem::TItem* TShip::GetCurrentPickupItem() {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TItem* Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
            if (PickupTargets != nullptr && pas::list_indexof(PickupTargets, reinterpret_cast<void*>(Item)) >= 0) {
                return Item;
            }
            if (Order == soMove && GetPickupApproachPosition(Item->Position).X == OrderDestination.X && GetPickupApproachPosition(Item->Position).Y == OrderDestination.Y) {
                return Item;
            }
        }
        return Result;
    }

    void TShip::ClearRecentlyDroppedItems() {
        if (RecentlyDroppedItemIds != nullptr) {
            pas::free(RecentlyDroppedItemIds);
            RecentlyDroppedItemIds = nullptr;
        }
    }

    void TShip::AddRecentlyDroppedItem(aItem::TItem* Item) {
        if (RecentlyDroppedItemIds == nullptr) {
            RecentlyDroppedItemIds = pas::make_object<pas::List>();
        }
        if (pas::list_indexof(RecentlyDroppedItemIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Item->Id)))) < 0) {
            pas::list_add(RecentlyDroppedItemIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Item->Id))));
        }
    }

    std::uint8_t TShip::IsRecentlyDroppedItem(aItem::TItem* Item) {
        std::uint8_t Result = false;
        if (RecentlyDroppedItemIds != nullptr && pas::list_indexof(RecentlyDroppedItemIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Item->Id)))) >= 0) {
            return true;
        }
        return Result;
    }

    // Except OrderJumpHole, new orders respect AbsoluteScriptOrder.
    void TShip::OrderNone(std::uint8_t OverrideScriptOrder) {
        if (AbsoluteScriptOrder > 0 && static_cast<std::uint8_t>(OverrideScriptOrder ^ 1)) {
            return;
        }
        OrderAbsolute = false;
        Order = soNone;
        OrderTarget = nullptr;
        ClearMovementPath();
    }

    void TShip::OrderMove(EC_Struct::TPointF Destination, std::uint8_t Absolute) {
        if (AbsoluteScriptOrder > 0) {
            return;
        }
        if (!HasPositiveSpeed()) {
            OrderNone(false);
            return;
        }
        Order = soMove;
        OrderDestination = Destination;
        OrderAbsolute = Absolute;
        OrderTarget = nullptr;
    }

    EC_Struct::TPointF TShip::GetJumpDeparturePoint(aGalaxy::TStar* Destination) {
        EC_Struct::TPointF Result{};
        pas::Extended cpp_right = aMyFunction::SeededRandomIntRange(-4, 4, (CurrentStar->GenerationSeed + Seed) * Destination->GenerationSeed);
        double Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(CurrentStar->Position, Destination->Position) + cpp_right);
        double Radius = pas::real_divide(CurrentStar->ComputeMapDiameter(), 2.0L);
        Result.X = System::Trunc(System::Sin(Angle) * Radius);
        Result.Y = System::Trunc(-System::Cos(Angle) * Radius);
        return Result;
    }

    // Point on the destination map boundary facing the current system.
    EC_Struct::TPointF TShip::GetArrivalPosition(aGalaxy::TStar* DestinationStar) {
        EC_Struct::TPointF Result{};
        double Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(DestinationStar->Position, CurrentStar->Position));
        double Radius = pas::real_divide(DestinationStar->ComputeMapDiameter(), 2.0L);
        Result.X = System::Trunc(System::Sin(Angle) * Radius);
        Result.Y = System::Trunc(-System::Cos(Angle) * Radius);
        return Result;
    }

    // Minimum two days; independent of equipment and fuel.
    std::int32_t TShip::CalculateJumpTravelDays(aGalaxy::TStar* Origin, aGalaxy::TStar* Destination) {
        return std::max<std::int64_t>(static_cast<std::int64_t>(2), System::Round(aMyFunction::PointDistance(Origin->Position, Destination->Position) * 0.1L) + 1);
    }

    void TShip::OrderJump(aGalaxy::TStar* Star, std::uint8_t Absolute) {
        if (AbsoluteScriptOrder > 0 || CurrentStar == Star) {
            return;
        }
        if (!HasPositiveSpeed()) {
            OrderNone(false);
            return;
        }
        Order = soJump;
        OrderTarget = Star;
        OrderAbsolute = Absolute;
        OrderDestination = GetJumpDeparturePoint(Star);
        OrderStateData = TShip::CalculateJumpTravelDays(CurrentStar, Star);
    }

    // Does not check AbsoluteScriptOrder.
    void TShip::OrderJumpHole(aGalaxy::THole* Hole, std::uint8_t Absolute) {
        if (!HasPositiveSpeed()) {
            OrderNone(false);
            return;
        }
        Order = soJumpHole;
        OrderTarget = Hole;
        OrderAbsolute = Absolute;
        if (CurrentStar == Hole->Star1) {
            OrderDestination = Hole->Position1;
            OrderStateData = 2;
        } else {
            OrderDestination = Hole->Position2;
            OrderStateData = 0x00010002;
        }
    }

    void TShip::OrderTeleport(aGalaxy::TStar* Star, EC_Struct::TPointF Destination, std::int32_t TransitionData, std::uint8_t Absolute) {
        if (AbsoluteScriptOrder > 0) {
            return;
        }
        Order = soTeleport;
        OrderTarget = Star;
        OrderAbsolute = Absolute;
        OrderDestination = Destination;
        OrderStateData = TransitionData;
    }

    // Location is a planet or dockable ship.
    void TShip::OrderLanding(pas::Object* Location, std::uint8_t Absolute) {
        if (AbsoluteScriptOrder > 0) {
            return;
        }
        if (Location == nullptr) {
            OrderNone(false);
            return;
        }
        if (!HasPositiveSpeed()) {
            OrderNone(false);
            return;
        }
        Order = soLand;
        OrderTarget = Location;
        OrderDestination = EC_Struct::MakePointF(0.0f, 0.0f);
        OrderAbsolute = Absolute;
    }

    void TShip::OrderTakeoff() {
        double Angle{};
        std::uint8_t Conflict{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t Attempt{};
        TShip* Ship{};
        float DY{};
        float DX{};
        if (AbsoluteScriptOrder > 0) {
            return;
        }
        if (!HasPositiveSpeed()) {
            OrderNone(false);
            return;
        }
        OrderNone(false);
        PlayerExtortionPactActive = false;
        if (CurrentPlanet != nullptr) {
            Order = soTakeoff;
            if (pas::class_cast_if<aNormalShip::TNormalShip*>(this) != nullptr) {
                pas::checked_cast<aNormalShip::TNormalShip*>(this)->LastDockedPlanet = CurrentPlanet;
            }
            Position = CurrentPlanet->GetPosition();
            Conflict = true;
            Attempt = 0;
            while (Conflict) {
                Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, CurrentPlanet->GenerationSeed * Seed * aGalaxy::Galaxy->CurrentTurn * (Attempt + 1)) * SystemImports::Pi, 1.8E+2L);
                {
                    pas::Extended cpp_right = System::Sin(Angle) * 4.0E+2L;
                    OrderDestination.X = System::Trunc(CurrentPlanet->GetPosition().X + cpp_right);
                }
                {
                    pas::Extended cpp_right_2 = -System::Cos(Angle) * 4.0E+2L;
                    OrderDestination.Y = System::Trunc(CurrentPlanet->GetPosition().Y + cpp_right_2);
                }
                DX = static_cast<long double>(OrderDestination.X) - Position.X;
                DY = static_cast<long double>(OrderDestination.Y) - Position.Y;
                if ((aGalaxy::Galaxy->CurrentTurn & 1) != 0) {
                    MovementDirection = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(0.0L - DY, -(0.0L + DX)));
                } else {
                    MovementDirection = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(0.0L + DY, -(0.0L - DX)));
                }
                {
                    pas::Extended cpp_right_3 = aMyFunction::SeededRandomIntRange(-5, 5, CurrentPlanet->GenerationSeed * Seed * aGalaxy::Galaxy->CurrentTurn * (Attempt + 3 + 1));
                    MovementDirection = aMyFunction::WrapHeadingDegrees(MovementDirection + cpp_right_3);
                }
                Conflict = false;
                Count = pas::list_count(CurrentStar->Ships);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                    Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
                    if (Ship != this && Ship->Order == soTakeoff && Ship->CurrentPlanet == CurrentPlanet && std::fabs(static_cast<pas::Extended>(aMyFunction::HeadingDifferenceDegrees(MovementDirection, Ship->MovementDirection))) < 2.0E+1L) {
                        Conflict = true;
                        break;
                    }
                }
                ++Attempt;
                if (Attempt > 5) {
                    break;
                }
            }
        } else if (DockedTo != nullptr) {
            Order = soTakeoff;
            if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr) {
                pas::checked_cast<aRanger::TRanger*>(this)->LastDockedNonPlanetLocation = DockedTo;
            }
            if (DockedTo->CurrentPlanet == nullptr) {
                Position = DockedTo->Position;
            } else {
                Position = DockedTo->CurrentPlanet->GetPosition();
            }
            Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, (DockedTo->Seed + Seed) * aGalaxy::Galaxy->CurrentTurn) * SystemImports::Pi, 1.8E+2L);
            OrderDestination.X = System::Trunc(Position.X + System::Sin(Angle) * 4.0E+2L);
            OrderDestination.Y = System::Trunc(Position.Y + -System::Cos(Angle) * 4.0E+2L);
            MovementDirection = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(OrderDestination.X) - Position.X), -(-(static_cast<long double>(OrderDestination.Y) - Position.Y))));
            {
                pas::Extended cpp_right_4 = aMyFunction::SeededRandomIntRange(-5, 5, (DockedTo->Seed + Seed + 234) * aGalaxy::Galaxy->CurrentTurn * 4);
                MovementDirection = aMyFunction::WrapHeadingDegrees(MovementDirection + cpp_right_4);
            }
        }
    }

    void TShip::OrderFollowShip(TShip* Ship, std::uint8_t FollowMode, std::uint8_t Absolute) {
        if (AbsoluteScriptOrder > 0) {
            return;
        }
        OrderNone(false);
        Order = soFollowShip;
        OrderTarget = Ship;
        OrderStateData = FollowMode;
        OrderAbsolute = Absolute;
    }

    // Ceiling of active path-node count times the star's MovementStepScale.
    std::int32_t TShip::GetMovementPathTurnCount() {
        return MathImports::Ceil(MovementPath->NodeCount * CurrentStar->MovementStepScale);
    }

    void TShip::PrepareTurnMovement(std::int32_t StartStepIndex, std::uint8_t RecordFilm) {
        double Distance{};
        double Angle{};
        aGalaxy::PJumpGateEntry Gate{};
        aEFilm::TEFilmObj* EffectFilm{};
        TShip* Owner{};
        aGalaxy::TStar* Star{};
        EC_Struct::TPointF Point{};
        EC_Struct::TPointF TargetPosition{};
        std::int32_t Countdown{};
        aPath::PSPathNode Node{};
        std::int32_t I{};
        std::int32_t J{};
        std::uint8_t K{};
        aItem::TItem* Item{};
        float PickupDistance{};
        aPath::PSPathNode PickupNode{};
        std::uint8_t CanLand{};
        SE_Space::TObjectSE* Effect{};
        // Caller-popped static link; step index -4, ship -8.
        auto InitializeFilm = [&](std::uint8_t Alpha) -> void {
            Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, this->FilmObject, this->Position);
            Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, this->FilmObject, aMyFunction::HeadingDegreesToByte(this->MovementDirection));
            Globals::PrimaryFilm->SetObjectAlpha(StartStepIndex, this->FilmObject, Alpha);
            Globals::PrimaryFilm->AttachObject(StartStepIndex, this->FilmObject);
            if (this->AuxiliaryFilmObject != nullptr) {
                Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, this->AuxiliaryFilmObject, this->Position);
                Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, this->AuxiliaryFilmObject, aMyFunction::HeadingDegreesToByte(this->MovementDirection));
                Globals::PrimaryFilm->SetObjectAlpha(StartStepIndex, this->AuxiliaryFilmObject, this->AuxiliaryFilmObject->SceneObject->GetAlpha());
                Globals::PrimaryFilm->AttachObject(StartStepIndex, this->AuxiliaryFilmObject);
            }
        };
        FilmObject = nullptr;
        AuxiliaryFilmObject = nullptr;
        PickupPathUpdatesAllowed = InNormalSpace() && Order != soTeleport;
        if (PickupPathUpdatesAllowed && (Order == soLand || Order == soJump || Order == soJumpHole)) {
            PickupPathUpdatesAllowed = false;
            if (PickupTargets != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PickupTargets) - 1); cpp_range.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(PickupTargets, I);
                    PickupNode = MovementPath->ActiveHead;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, MovementPath->NodeCount - 1); cpp_range_2.next(J); ) {
                        PickupDistance = aMyFunction::PointDistance(Item->Position, PickupNode->Position);
                        if (static_cast<long double>(PickupDistance) <= GetCargoHookRange()) {
                            PickupPathUpdatesAllowed = true;
                            break;
                        }
                        PickupNode = PickupNode->Next;
                    }
                    if (PickupPathUpdatesAllowed) {
                        break;
                    }
                }
            }
        }
        if (aPlayer::GetPlayer() == this && Order == soJump && InNormalSpace()) {
            if (GetFuelTanks()->Fuel < System::Round(aMyFunction::PointDistance(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Position, CurrentStar->Position))) {
                {
                    auto& cpp_target = ([&] {
                        const pas::WideString& formatText1 = ([&] {
                            auto name = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Name);
                            pas::WideString localizedText = aConst::LocalizedText(u"Items.FuelTanks.NoFuelJump"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
                        }());
                        std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                        return Globals::AddOrUpdatePlayerBubble(8, currentTurn, formatText1, u""_wref.get());
                    }())->Targets[0].ShipId;
                    cpp_target = Id;
                }
                OrderMove(OrderDestination, false);
            }
        }
        if (RecordFilm && (Order != soNone || static_cast<std::uint8_t>(IsOnPlanet() ^ 1)) && (Order != soNone || static_cast<std::uint8_t>(IsDockedToShip() ^ 1))) {
            FilmObject = Globals::PrimaryFilm->AddObject(Id, Graphic, 0, 0);
            if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Graphic) != nullptr) {
                if (AfterburnerActive) {
                    Globals::PrimaryFilm->SetShipSizeAndTailMode(StartStepIndex, FilmObject, Graphic->Size, 2);
                } else {
                    Globals::PrimaryFilm->SetShipSizeAndTailMode(StartStepIndex, FilmObject, Graphic->Size, 1);
                }
            } else if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Graphic) != nullptr && static_cast<SE_Ruins::TRuinsSE*>(Graphic)->HasTransitionImages) {
                Globals::PrimaryFilm->SetRuinsState(StartStepIndex, FilmObject, 1);
            }
            if (InterceptorGraphic != nullptr) {
                AuxiliaryFilmObject = Globals::PrimaryFilm->AddObject(Id, InterceptorGraphic, 0, 0);
            }
        }
        if (Order == soNone) {
            if (!InHyperspace) {
                if (RecordFilm && static_cast<std::uint8_t>(IsOnPlanet() ^ 1) && static_cast<std::uint8_t>(IsDockedToShip() ^ 1)) {
                    InitializeFilm(255);
                    if (aPlayer::GetPlayer() == this) {
                        Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                    }
                }
            }
        } else if (Order == soMove) {
            FilmAlpha = 255.0f;
            FilmAlphaStep = 0.0f;
            if (RecordFilm) {
                InitializeFilm(255);
                if (aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, OrderDestination, true);
                }
            }
        } else if (Order == soLand) {
            FilmAlpha = 5.1E+2f;
            FilmAlphaStep = 0.0f;
            if (RecordFilm) {
                InitializeFilm(255);
            }
            if (pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
                Point = pas::checked_cast<TShip*>(OrderTarget)->Position;
                TargetPosition = Point;
                if (MovementPath->ActiveHead == nullptr || PickupPathUpdatesAllowed) {
                    CanLand = false;
                } else {
                    CanLand = aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, EC_Struct::AddPointsF(Point, OrderDestination)) <= 0.0L;
                }
            } else {
                Point = pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->PredictPosition(CurrentStar->MovementStepCount);
                TargetPosition = pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->GetPosition();
                if (MovementPath->ActiveHead == nullptr || PickupPathUpdatesAllowed) {
                    CanLand = false;
                } else {
                    CanLand = static_cast<long double>(aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, Point)) <= pas::sqr(pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->GraphicRadius) + 1;
                }
            }
            OrderStateData = 0;
            if (MovementPath->ActiveHead != nullptr && static_cast<std::uint8_t>(PickupPathUpdatesAllowed ^ 1)) {
                if (CanLand) {
                    if (RecordFilm) {
                        FilmAlphaStep = -pas::real_divide(FilmAlpha, MovementPath->NodeCount);
                        Globals::PrimaryFilm->SetObjectAlpha(StartStepIndex, FilmObject, 255);
                        if (aPlayer::GetPlayer() == this) {
                            Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                        }
                    } else {
                        FilmAlpha = 0.0f;
                    }
                    AfterburnerActive = false;
                    OrderStateData = 1;
                }
            } else if (RecordFilm) {
                Globals::PrimaryFilm->SetObjectAlpha(StartStepIndex, FilmObject, 255);
                if (aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, TargetPosition, true);
                }
            }
        } else if (Order == soTeleport) {
            if (!InHyperspace) {
                if (RecordFilm) {
                    if (aPlayer::GetPlayer() == this) {
                        Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                    }
                    if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(FilmObject->SceneObject) != nullptr && reinterpret_cast<SE_Ruins::TRuinsSE*>(FilmObject->SceneObject)->HasTransitionImages) {
                        Globals::PrimaryFilm->SetRuinsState(StartStepIndex, FilmObject, 2);
                    } else {
                        Effect = pas::construct_call<SE_GAIEffect::TGAIEffectSE>(SE_GAIEffect::TGAIEffectSE_Create, u"Effect.TeleportOut"_wref.get(), ClassesImports::Point(0, 0));
                        EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                        {
                            std::int32_t round = System::Round(Position.Y);
                            std::int32_t round_2 = System::Round(Position.X);
                            Globals::PrimaryFilm->SetEffectImagePosition(StartStepIndex, EffectFilm, ClassesImports::Point(round_2, round));
                        }
                        FilmAlpha = 5.1E+2f;
                        if (OrderStateData <= 0 && OrderTarget == CurrentStar) {
                            Globals::PrimaryFilm->SetEffectDurationScale(StartStepIndex, EffectFilm, 0.5f);
                            FilmAlphaStep = -7.65f;
                        } else {
                            Globals::PrimaryFilm->SetEffectDurationScale(StartStepIndex, EffectFilm, 1.0f);
                            FilmAlphaStep = -3.825f;
                        }
                        Globals::PrimaryFilm->AttachObject(StartStepIndex + 1, EffectFilm);
                        Globals::PrimaryFilm->PlayObjectSound(StartStepIndex + 1, FilmObject, u"Sound.TeleportOut"_wref.get());
                    }
                    InitializeFilm(255);
                }
                AfterburnerActive = false;
                InHyperspace = OrderStateData > 0 || OrderTarget != CurrentStar;
                AbductedByPirateClan = AbductedByPirateClan && OrderTarget == CurrentStar;
                ClearMovementPath();
                RefreshDerivedStats(true);
                if (OrderTarget != CurrentStar) {
                    pas::list_add(aGalaxy::Galaxy->ShipsInTransit, reinterpret_cast<void*>(this));
                    CurrentStar->HandleObjectLeavingStar(this);
                } else {
                    TransitOriginStar = CurrentStar;
                }
            } else {
                --OrderStateData;
                if (OrderStateData <= 0) {
                    InHyperspace = false;
                    Position = OrderDestination;
                    OrderNone(true);
                    Graphic->SetPosition(Position);
                    if (RecordFilm) {
                        if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(FilmObject->SceneObject) != nullptr && reinterpret_cast<SE_Ruins::TRuinsSE*>(FilmObject->SceneObject)->HasTransitionImages) {
                            Globals::PrimaryFilm->SetRuinsState(StartStepIndex, FilmObject, 3);
                            InitializeFilm(255);
                        } else {
                            Effect = pas::construct_call<SE_GAIEffect::TGAIEffectSE>(SE_GAIEffect::TGAIEffectSE_Create, u"Effect.TeleportIn"_wref.get(), ClassesImports::Point(0, 0));
                            EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            {
                                std::int32_t round_3 = System::Round(Position.Y);
                                std::int32_t round_4 = System::Round(Position.X);
                                Globals::PrimaryFilm->SetEffectImagePosition(StartStepIndex, EffectFilm, ClassesImports::Point(round_4, round_3));
                            }
                            Globals::PrimaryFilm->SetEffectDurationScale(StartStepIndex, EffectFilm, 1.0f);
                            Globals::PrimaryFilm->AttachObject(StartStepIndex + 1, EffectFilm);
                            Globals::PrimaryFilm->PlayObjectSound(StartStepIndex + 1, FilmObject, u"Sound.TeleportIn"_wref.get());
                            FilmAlpha = -255.0f;
                            FilmAlphaStep = 3.825f;
                            InitializeFilm(0);
                        }
                        if (aPlayer::GetPlayer() == this) {
                            Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, true);
                        }
                        if (pas::class_cast_if<aRuins::TRuins*>(this) != nullptr && TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && TransitOriginStar == CurrentStar) {
                            reinterpret_cast<aRuins::TRuins*>(this)->ReportAbductionOutcome();
                        }
                    } else {
                        FilmAlpha = 255.0f;
                        FilmAlphaStep = 0.0f;
                    }
                }
            }
        } else if (Order == soJump) {
            if (!InHyperspace) {
                FilmAlpha = 5.1E+2f;
                FilmAlphaStep = 0.0f;
                AbductedByPirateClan = false;
                if (RecordFilm) {
                    InitializeFilm(255);
                }
                if (static_cast<std::uint8_t>(PickupPathUpdatesAllowed ^ 1) && MovementPath->ActiveHead == nullptr) {
                    OrderDestination = GetJumpDeparturePoint(reinterpret_cast<aGalaxy::TStar*>(OrderTarget));
                }
                if (static_cast<std::uint8_t>(PickupPathUpdatesAllowed ^ 1) && MovementPath->ActiveHead != nullptr && JumpDeparturePathCommitted) {
                    OrderDestination = EC_Struct::MakePointF(0.0f, 0.0f);
                    AfterburnerActive = false;
                    if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition && reinterpret_cast<aGalaxy::TStar*>(OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
                            if (CurrentStar->StarDistances[I].Distance > 40) {
                                break;
                            }
                            Star = CurrentStar->StarDistances[I].Star;
                            if (Star->Dominion != nullptr && (aPlayer::GetPlayer() != this || Star->IsConstellationVisible())) {
                                reinterpret_cast<aRuins::TRuins*>(Star->Dominion)->TryAbductDepartingShip(this);
                            }
                        }
                    }
                    if (RecordFilm) {
                        if (aPlayer::GetPlayer() == this) {
                            Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                        }
                        FilmAlphaStep = -pas::real_divide(5.1E+2L, MovementPath->NodeCount);
                        if (aGalaxy::PlayerStar == CurrentStar) {
                            Point = MovementPath->ActiveHead->Position;
                            Angle = MovementPath->ActiveHead->Heading;
                            Node = MovementPath->ActiveHead->Next;
                            while (Node != nullptr) {
                                if (std::fabs(static_cast<long double>(Angle) - Node->Heading) < 0.001L) {
                                    break;
                                }
                                Angle = Node->Heading;
                                Point = Node->Position;
                                Node = Node->Next;
                            }
                            Gate = aGalaxy::Galaxy->CreateJumpGate(AbductedByPirateClan);
                            if (GR_Main::GiResourceVariant() == 2) {
                                std::int32_t round_5 = System::Round(Graphic->Size.Y * 1.5L);
                                std::int32_t round_6 = System::Round(Graphic->Size.X * 1.5L);
                                Gate->Gate->SetSize(ClassesImports::Point(round_6, round_5));
                            } else {
                                std::int32_t round_7 = System::Round(pas::real_divide(Graphic->Size.Y * 1.5L * 1024.0L, 8.0E+2L));
                                std::int32_t round_8 = System::Round(pas::real_divide(Graphic->Size.X * 1.5L * 1024.0L, 8.0E+2L));
                                Gate->Gate->SetSize(ClassesImports::Point(round_8, round_7));
                            }
                            {
                                float cpp_arg = Point.X + System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * 1.8E+2L;
                                float cpp_arg_2 = Point.Y - System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * 1.8E+2L;
                                Gate->Gate->SetPosition(EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
                            }
                            Gate->Gate->SetAngle(aMyFunction::HeadingDegreesToByte(Angle) + 127);
                            EffectFilm = Globals::PrimaryFilm->AddObject(0u, Gate->Gate, 0, 0);
                            Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, EffectFilm, Gate->Gate->Position);
                            Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, EffectFilm, Gate->Gate->GetAngle());
                            Globals::PrimaryFilm->SetGateSize(StartStepIndex, EffectFilm, Gate->Gate->Size.X);
                            Globals::PrimaryFilm->SetGateState(StartStepIndex, EffectFilm, 0);
                            Globals::PrimaryFilm->OpenGate(StartStepIndex, EffectFilm);
                            if (!(pas::class_cast_if<aKling::TKling*>(this) != nullptr) && static_cast<std::uint8_t>(AbductedByPirateClan ^ 1)) {
                                std::int64_t cpp_right = System::Round(aMyFunction::PointDistance(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Position, CurrentStar->Position));
                                if (GetJumpRange() >= cpp_right) {
                                    if (pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Constellation->Id != 20 || pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->IsConstellationVisible()) {
                                        Globals::PrimaryFilm->SetObjectText(StartStepIndex, EffectFilm, pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Name);
                                    }
                                }
                            }
                            Globals::PrimaryFilm->AttachObject(StartStepIndex, EffectFilm);
                            if (Gate->Effect == nullptr) {
                                Globals::PrimaryFilm->PlayObjectSound(StartStepIndex + 1, EffectFilm, u"Sound.HyperJump"_wref.get());
                            } else {
                                Globals::PrimaryFilm->PlayObjectSound(StartStepIndex + 1, EffectFilm, u"Sound.HyperJumpAbducted"_wref.get());
                                if (GR_Main::GiResourceVariant() == 2) {
                                    std::int32_t round_9 = System::Round(Graphic->Size.Y * 1.5L);
                                    std::int32_t round_10 = System::Round(Graphic->Size.X * 1.5L);
                                    Gate->Effect->SetSize(ClassesImports::Point(round_10, round_9));
                                } else {
                                    std::int32_t round_11 = System::Round(pas::real_divide(Graphic->Size.Y * 1.5L * 1024.0L, 8.0E+2L));
                                    std::int32_t round_12 = System::Round(pas::real_divide(Graphic->Size.X * 1.5L * 1024.0L, 8.0E+2L));
                                    Gate->Effect->SetSize(ClassesImports::Point(round_12, round_11));
                                }
                                {
                                    float cpp_arg_3 = Point.X + System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * 1.8E+2L;
                                    float cpp_arg_4 = Point.Y - System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * 1.8E+2L;
                                    Gate->Effect->SetPosition(EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4));
                                }
                                Gate->Effect->SetAngle(aMyFunction::HeadingDegreesToByte(Angle) + 127);
                                EffectFilm = Globals::PrimaryFilm->AddObject(0u, Gate->Effect, 0, 0);
                                Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, EffectFilm, Gate->Effect->Position);
                                Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, EffectFilm, Gate->Effect->GetAngle());
                                Globals::PrimaryFilm->SetGateEffectSize(StartStepIndex, EffectFilm, Gate->Effect->Size.X);
                                Globals::PrimaryFilm->AttachObject(StartStepIndex, EffectFilm);
                            }
                        }
                    }
                } else if (RecordFilm && aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, OrderDestination, false);
                }
            } else {
                FilmAlpha = -255.0f;
                FilmAlphaStep = 0.0f;
                if (aKling::KellerShip != this && aKling::TerronShip != this) {
                    --OrderStateData;
                    if (static_cast<std::uint32_t>(OrderStateData) > 1 && static_cast<std::uint8_t>(AbductedByPirateClan ^ 1)) {
                        OrderStateData -= std::min<std::int64_t>(static_cast<std::int64_t>(([&] {
                            std::int32_t cpp_right_2 = (CanBoostArtefact(aConst::t_ArtGiperJump, nullptr, false) & 127) + 1;
                            return CountActiveArtefacts(aConst::t_ArtGiperJump) * cpp_right_2;
                        }())), static_cast<std::int64_t>(static_cast<std::uint32_t>(OrderStateData)));
                        OrderStateData = std::max<std::int64_t>(static_cast<std::int64_t>(1), static_cast<std::int64_t>(static_cast<std::uint32_t>(OrderStateData)));
                    }
                }
                if (OrderStateData <= 0) {
                    InHyperspace = false;
                    Distance = -(pas::real_divide(CurrentStar->ComputeMapDiameter(), 2.0L) + 0.0L);
                    if (AbductedByPirateClan) {
                        Angle = aMyFunction::PointBearingDegrees(CurrentStar->Position, TransitOriginStar->Position);
                    } else {
                        Angle = MovementDirection;
                    }
                    Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(static_cast<long double>(Angle) + pas::abs(static_cast<std::int32_t>(CurrentStar->GenerationSeed + Seed)) % 10 - 5.0L));
                    if (AbductedByPirateClan && CurrentStar->Dominion != nullptr && reinterpret_cast<TShip*>(CurrentStar->Dominion)->InNormalSpace() && reinterpret_cast<TShip*>(CurrentStar->Dominion)->CurrentStar == CurrentStar) {
                        Position.X = reinterpret_cast<TShip*>(CurrentStar->Dominion)->Position.X;
                        Position.Y = reinterpret_cast<TShip*>(CurrentStar->Dominion)->Position.Y;
                        MovementDirection = -aMyFunction::RadiansToHeadingDegrees(Angle);
                        if (RecordFilm && SimulationContext == 0) {
                            Effect = pas::construct_call<SE_GAIEffect::TGAIEffectSE>(SE_GAIEffect::TGAIEffectSE_Create, u"Effect.CBAbductEffect"_wref.get(), ClassesImports::Point(0, 0));
                            EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            {
                                std::int32_t round_13 = System::Round(Position.Y);
                                std::int32_t round_14 = System::Round(Position.X);
                                Globals::PrimaryFilm->SetEffectImagePosition(StartStepIndex, EffectFilm, ClassesImports::Point(round_14, round_13));
                            }
                            Globals::PrimaryFilm->SetEffectDurationScale(StartStepIndex, EffectFilm, 0.5f);
                            Globals::PrimaryFilm->AttachObject(StartStepIndex + 1, EffectFilm);
                            SimulationContext = 1;
                        }
                    } else {
                        Position.X = System::Sin(Angle) * Distance;
                        Position.Y = -System::Cos(Angle) * Distance;
                        MovementDirection = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-Position.X, -(-Position.Y)));
                    }
                    Graphic->SetPosition(Position);
                    Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
                    OrderNone(true);
                    if ((AbductedByPirateClan || aPlayer::GetPlayer() == this && aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) && GetEngine() != nullptr) {
                        GetEngine()->OutputPercent = 0;
                    }
                    Order = soMove;
                    AppendHyperspaceTransitionPath(-1.0f);
                    if (MovementPath->ActiveTail != nullptr) {
                        OrderDestination = MovementPath->ActiveTail->Position;
                    } else {
                        OrderDestination = Position;
                    }
                    if (RecordFilm) {
                        FilmAlphaStep = pas::real_divide(5.1E+2L, MovementPath->NodeCount);
                        InitializeFilm(0);
                        if (aPlayer::GetPlayer() == this) {
                            Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, true);
                        }
                        if (aGalaxy::PlayerStar == CurrentStar) {
                            Distance = aMyFunction::PointDistance(Position, OrderDestination);
                            Gate = aGalaxy::Galaxy->CreateJumpGate(AbductedByPirateClan || aPlayer::GetPlayer() == this && aPlayer::GetPlayer()->QueuedTravelTarget != nullptr);
                            if (GR_Main::GiResourceVariant() == 2) {
                                std::int32_t round_15 = System::Round(Graphic->Size.Y * 1.5L);
                                std::int32_t round_16 = System::Round(Graphic->Size.X * 1.5L);
                                Gate->Gate->SetSize(ClassesImports::Point(round_16, round_15));
                            } else {
                                std::int32_t round_17 = System::Round(pas::real_divide(Graphic->Size.Y * 1.5L * 1024.0L, 8.0E+2L));
                                std::int32_t round_18 = System::Round(pas::real_divide(Graphic->Size.X * 1.5L * 1024.0L, 8.0E+2L));
                                Gate->Gate->SetSize(ClassesImports::Point(round_18, round_17));
                            }
                            {
                                float cpp_arg_5 = OrderDestination.X + pas::real_divide(static_cast<long double>(Position.X) - OrderDestination.X, Distance) * 1.5E+2L;
                                float cpp_arg_6 = OrderDestination.Y + pas::real_divide(static_cast<long double>(Position.Y) - OrderDestination.Y, Distance) * 1.5E+2L;
                                Gate->Gate->SetPosition(EC_Struct::MakePointF(cpp_arg_5, cpp_arg_6));
                            }
                            Gate->Gate->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
                            EffectFilm = Globals::PrimaryFilm->AddObject(0u, Gate->Gate, 0, 0);
                            Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, EffectFilm, Gate->Gate->Position);
                            Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, EffectFilm, Gate->Gate->GetAngle());
                            Globals::PrimaryFilm->SetGateSize(StartStepIndex, EffectFilm, Gate->Gate->Size.X);
                            Globals::PrimaryFilm->SetGateState(StartStepIndex, EffectFilm, 0);
                            Globals::PrimaryFilm->OpenGate(StartStepIndex, EffectFilm);
                            if (TransitOriginStar != nullptr) {
                                if (static_cast<std::uint8_t>(AbductedByPirateClan ^ 1) && !(pas::class_cast_if<aKling::TKling*>(this) != nullptr)) {
                                    if (([&] {
                                        std::int64_t cpp_right_3 = System::Round(aMyFunction::PointDistance(TransitOriginStar->Position, CurrentStar->Position));
                                        return GetJumpRange() >= cpp_right_3;
                                    }()) && (TransitOriginStar->Constellation->Id != 20 || TransitOriginStar->IsConstellationVisible())) {
                                        Globals::PrimaryFilm->SetObjectText(StartStepIndex, EffectFilm, TransitOriginStar->Name);
                                    }
                                }
                            }
                            Globals::PrimaryFilm->AttachObject(StartStepIndex, EffectFilm);
                            Globals::PrimaryFilm->PlayObjectSound(StartStepIndex + 1, EffectFilm, u"Sound.HyperJump"_wref.get());
                            if (Gate->Effect != nullptr) {
                                if (GR_Main::GiResourceVariant() == 2) {
                                    std::int32_t round_19 = System::Round(Graphic->Size.Y * 1.5L);
                                    std::int32_t round_20 = System::Round(Graphic->Size.X * 1.5L);
                                    Gate->Effect->SetSize(ClassesImports::Point(round_20, round_19));
                                } else {
                                    std::int32_t round_21 = System::Round(pas::real_divide(Graphic->Size.Y * 1.5L * 1024.0L, 8.0E+2L));
                                    std::int32_t round_22 = System::Round(pas::real_divide(Graphic->Size.X * 1.5L * 1024.0L, 8.0E+2L));
                                    Gate->Effect->SetSize(ClassesImports::Point(round_22, round_21));
                                }
                                {
                                    float cpp_arg_7 = OrderDestination.X + pas::real_divide(static_cast<long double>(Position.X) - OrderDestination.X, Distance) * 1.5E+2L;
                                    float cpp_arg_8 = OrderDestination.Y + pas::real_divide(static_cast<long double>(Position.Y) - OrderDestination.Y, Distance) * 1.5E+2L;
                                    Gate->Effect->SetPosition(EC_Struct::MakePointF(cpp_arg_7, cpp_arg_8));
                                }
                                Gate->Effect->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
                                EffectFilm = Globals::PrimaryFilm->AddObject(0u, Gate->Effect, 0, 0);
                                Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, EffectFilm, Gate->Effect->Position);
                                Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, EffectFilm, Gate->Effect->GetAngle());
                                Globals::PrimaryFilm->SetGateEffectSize(StartStepIndex, EffectFilm, Gate->Effect->Size.X);
                                Globals::PrimaryFilm->AttachObject(StartStepIndex, EffectFilm);
                            }
                        }
                        if (aPlayer::GetPlayer() == this) {
                            Globals::PrimaryFilm->SetViewCenter(StartStepIndex, OrderDestination);
                            Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, OrderDestination, false);
                            for (K = static_cast<std::uint8_t>(0); K <= static_cast<std::uint8_t>(2); ++K) {
                                aPlayer::GetPlayer()->ChameleonDetected[K] = false;
                            }
                            aPlayer::GetPlayer()->ReportIdleSatellites(aPlayer::GetPlayer()->CurrentStar);
                            Achievements::TryAddAchievementProgress(u"JUMPER"_w, 1);
                        }
                    }
                    if (aPlayer::GetPlayer() == this && aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
                        aPlayer::GetPlayer()->QueuedTravelTarget = nullptr;
                    }
                }
            }
        } else if (Order == soJumpHole) {
            if (!InHyperspace) {
                FilmAlpha = 5.1E+2f;
                FilmAlphaStep = 0.0f;
                if (RecordFilm) {
                    InitializeFilm(255);
                }
                if (MovementPath->ActiveHead != nullptr && static_cast<std::uint8_t>(PickupPathUpdatesAllowed ^ 1)) {
                    if (aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, OrderDestination) <= 0.0L) {
                        AfterburnerActive = false;
                        if (RecordFilm) {
                            FilmAlphaStep = -pas::real_divide(FilmAlpha, MovementPath->NodeCount);
                            Globals::PrimaryFilm->SetObjectAlpha(StartStepIndex, FilmObject, 255);
                            if (aPlayer::GetPlayer() == this) {
                                Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                            }
                        } else {
                            FilmAlpha = 0.0f;
                        }
                    }
                } else if (RecordFilm) {
                    Globals::PrimaryFilm->SetObjectAlpha(StartStepIndex, FilmObject, 255);
                    if (aPlayer::GetPlayer() == this) {
                        Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, OrderDestination, true);
                    }
                }
            } else {
                FilmAlpha = 0.0f;
                FilmAlphaStep = 0.0f;
                Countdown = OrderStateData & 0x0000ffff;
                --Countdown;
                OrderStateData = pas::shl(pas::shr(OrderStateData, 16), 16) | Countdown;
                if (Countdown <= 0) {
                    InHyperspace = false;
                    if (pas::shr(OrderStateData, 16) == 0 && reinterpret_cast<aGalaxy::THole*>(OrderTarget)->Star1 != reinterpret_cast<aGalaxy::THole*>(OrderTarget)->Star2) {
                        Point = reinterpret_cast<aGalaxy::THole*>(OrderTarget)->Position2;
                    } else {
                        Point = reinterpret_cast<aGalaxy::THole*>(OrderTarget)->Position1;
                    }
                    Position = Point;
                    Angle = pas::real_divide(aMyFunction::SeededRandomIntRange(0, 360, (Seed + CurrentStar->GenerationSeed + 1) * aGalaxy::Galaxy->CurrentTurn) * SystemImports::Pi, 1.8E+2L);
                    OrderDestination.X = System::Trunc(Point.X + System::Sin(Angle) * 4.0E+2L);
                    OrderDestination.Y = System::Trunc(Point.Y + -System::Cos(Angle) * 4.0E+2L);
                    MovementDirection = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(OrderDestination.X) - Point.X), -(-(static_cast<long double>(OrderDestination.Y) - Point.Y))));
                    {
                        pas::Extended cpp_right_4 = aMyFunction::SeededRandomIntRange(-5, 5, (CurrentStar->GenerationSeed + aGalaxy::Galaxy->CurrentTurn) * Seed * 4);
                        MovementDirection = aMyFunction::WrapHeadingDegrees(MovementDirection + cpp_right_4);
                    }
                    OrderStateData = -65536;
                    OrderTarget = nullptr;
                    BuildOrderMovementPath(1000);
                    if (MovementPath->ActiveTail != nullptr) {
                        OrderDestination = MovementPath->ActiveTail->Position;
                    } else {
                        OrderDestination = Position;
                    }
                    if (RecordFilm) {
                        FilmAlphaStep = 1.275f;
                        InitializeFilm(0);
                        if (aPlayer::GetPlayer() == this) {
                            Globals::PrimaryFilm->SetViewCenter(StartStepIndex, Position);
                            Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                            for (K = static_cast<std::uint8_t>(0); K <= static_cast<std::uint8_t>(2); ++K) {
                                aPlayer::GetPlayer()->ChameleonDetected[K] = false;
                            }
                            aPlayer::GetPlayer()->ReportIdleSatellites(aPlayer::GetPlayer()->CurrentStar);
                        }
                    }
                }
            }
        } else if (Order == soTakeoff) {
            DockedTo = nullptr;
            CurrentPlanet = nullptr;
            if (RecordFilm) {
                FilmAlpha = 0.0f;
                FilmAlphaStep = 1.33875f;
                InitializeFilm(0);
                if (aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetViewCenter(StartStepIndex, Position);
                    Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, Position, false);
                }
            }
            if (aPlayer::GetPlayer() == this && aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
                FilmAlphaStep = 5.1f;
                OrderNone(true);
                MovementDirection = aMyFunction::PointBearingDegrees(CurrentStar->Position, aPlayer::GetPlayer()->QueuedTravelTarget->Position);
                AppendHyperspaceTransitionPath(-1.0f);
                if (MovementPath->ActiveTail != nullptr) {
                    OrderDestination = MovementPath->ActiveTail->Position;
                } else {
                    OrderDestination = Position;
                }
                Order = soJump;
                OrderTarget = pas::checked_cast<aPlayer::TPlayer*>(this)->QueuedTravelTarget;
                OrderStateData = std::max<std::int64_t>(static_cast<std::int64_t>(2), System::Round(TShip::CalculateJumpTravelDays(CurrentStar, pas::checked_cast<aPlayer::TPlayer*>(this)->QueuedTravelTarget) * 0.75L));
                OrderDestination.X = 0.0f;
                OrderDestination.Y = 0.0f;
                Graphic->SetPosition(Position);
                Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection));
                if (RecordFilm) {
                    Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, FilmObject, aMyFunction::HeadingDegreesToByte(MovementDirection));
                }
                Gate = aGalaxy::Galaxy->CreateJumpGate(true);
                if (GR_Main::GiResourceVariant() == 2) {
                    std::int32_t round_23 = System::Round(Graphic->Size.Y * 1.5L);
                    std::int32_t round_24 = System::Round(Graphic->Size.X * 1.5L);
                    Gate->Gate->SetSize(ClassesImports::Point(round_24, round_23));
                } else {
                    std::int32_t round_25 = System::Round(pas::real_divide(Graphic->Size.Y * 1.5L * 1024.0L, 8.0E+2L));
                    std::int32_t round_26 = System::Round(pas::real_divide(Graphic->Size.X * 1.5L * 1024.0L, 8.0E+2L));
                    Gate->Gate->SetSize(ClassesImports::Point(round_26, round_25));
                }
                {
                    float cpp_arg_9 = Position.X + System::Sin(aMyFunction::HeadingDegreesToRadians(MovementDirection)) * 1.8E+2L;
                    float cpp_arg_10 = Position.Y - System::Cos(aMyFunction::HeadingDegreesToRadians(MovementDirection)) * 1.8E+2L;
                    Gate->Gate->SetPosition(EC_Struct::MakePointF(cpp_arg_9, cpp_arg_10));
                }
                Gate->Gate->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection) + 127);
                EffectFilm = Globals::PrimaryFilm->AddObject(0u, Gate->Gate, 0, 0);
                Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, EffectFilm, Gate->Gate->Position);
                Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, EffectFilm, Gate->Gate->GetAngle());
                Globals::PrimaryFilm->SetGateSize(StartStepIndex, EffectFilm, Gate->Gate->Size.X);
                Globals::PrimaryFilm->SetGateState(StartStepIndex, EffectFilm, 0);
                Globals::PrimaryFilm->OpenGate(StartStepIndex, EffectFilm);
                Globals::PrimaryFilm->AttachObject(StartStepIndex + 1, EffectFilm);
                Globals::PrimaryFilm->PlayObjectSound(StartStepIndex + 1, EffectFilm, u"Sound.HyperJump"_wref.get());
                if (Gate->Effect != nullptr) {
                    if (GR_Main::GiResourceVariant() == 2) {
                        std::int32_t round_27 = System::Round(Graphic->Size.Y * 1.5L);
                        std::int32_t round_28 = System::Round(Graphic->Size.X * 1.5L);
                        Gate->Effect->SetSize(ClassesImports::Point(round_28, round_27));
                    } else {
                        std::int32_t round_29 = System::Round(pas::real_divide(Graphic->Size.Y * 1.5L * 1024.0L, 8.0E+2L));
                        std::int32_t round_30 = System::Round(pas::real_divide(Graphic->Size.X * 1.5L * 1024.0L, 8.0E+2L));
                        Gate->Effect->SetSize(ClassesImports::Point(round_30, round_29));
                    }
                    {
                        float cpp_arg_11 = Position.X + System::Sin(aMyFunction::HeadingDegreesToRadians(MovementDirection)) * 1.8E+2L;
                        float cpp_arg_12 = Position.Y - System::Cos(aMyFunction::HeadingDegreesToRadians(MovementDirection)) * 1.8E+2L;
                        Gate->Effect->SetPosition(EC_Struct::MakePointF(cpp_arg_11, cpp_arg_12));
                    }
                    Gate->Effect->SetAngle(aMyFunction::HeadingDegreesToByte(MovementDirection) + 127);
                    EffectFilm = Globals::PrimaryFilm->AddObject(0u, Gate->Effect, 0, 0);
                    Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, EffectFilm, Gate->Effect->Position);
                    Globals::PrimaryFilm->SetObjectAngle(StartStepIndex, EffectFilm, Gate->Effect->GetAngle());
                    Globals::PrimaryFilm->SetGateEffectSize(StartStepIndex, EffectFilm, Gate->Effect->Size.X);
                    Globals::PrimaryFilm->AttachObject(StartStepIndex + 1, EffectFilm);
                }
            }
        } else if (Order == soFollowShip) {
            FilmAlpha = 255.0f;
            FilmAlphaStep = 0.0f;
            if (RecordFilm) {
                InitializeFilm(255);
                if (aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetCameraAnchor(StartStepIndex, pas::checked_cast<TShip*>(OrderTarget)->Position, true);
                }
            }
            if (pas::class_cast_if<aTranclucator::TTranclucator*>(this) != nullptr) {
                if (pas::checked_cast<aTranclucator::TTranclucator*>(this)->CanFollowOwnerInCurrentStar()) {
                    Owner = pas::checked_cast<aTranclucator::TTranclucator*>(this)->OwnerShip;
                    if (Owner->CurrentPlanet != nullptr) {
                        TargetPosition = Owner->CurrentPlanet->PredictPosition(CurrentStar->MovementStepCount);
                    } else if (Owner->DockedTo != nullptr) {
                        TargetPosition = Owner->DockedTo->Position;
                    } else if (Owner->MovementPath->ActiveTail == nullptr) {
                        TargetPosition = Owner->Position;
                    } else {
                        TargetPosition = Owner->MovementPath->ActiveTail->Position;
                    }
                    if (MovementPath->ActiveTail == nullptr) {
                        Point = Position;
                    } else {
                        Point = MovementPath->ActiveTail->Position;
                    }
                    if (aMyFunction::PointDistanceSquared(Point, TargetPosition) < 25.0L) {
                        if (RecordFilm) {
                            if (MovementPath->NodeCount < 1) {
                                FilmAlpha = 0.0f;
                                FilmAlphaStep = 0.0f;
                            } else {
                                FilmAlphaStep = -pas::real_divide(2.0E+2L, MovementPath->NodeCount);
                            }
                        } else {
                            FilmAlpha = 0.0f;
                        }
                    }
                }
            }
        } else {
            pas::raise(pas::make_exception<pas::Exception>("Error in TShip.StepDayStart"_a));
        }
        if (RecordFilm && aPlayer::GetPlayer() == this) {
            Globals::PrimaryFilm->SetRadarCenter(StartStepIndex, Position);
        }
    }

    // The native result remains false.
    std::uint8_t TShip::ProcessMovementStep(std::int32_t StepIndex, std::uint8_t RecordFilm) {
        aPath::PSPathNode Node{};
        std::uint8_t Angle{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t HalfTurn{};
        aItem::TItem* Item{};
        float Distance{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* EffectFilm{};
        std::int32_t Fuel{};
        // Caller-popped static link; ship -4, node -8, recording flag -9, step index -16. Removes one path node and updates position, heading and film.
        auto AdvancePath = [&]() -> void {
            if (this->MovementPath != nullptr && this->MovementPath->ActiveHead != nullptr) {
                Node = this->MovementPath->ActiveHead;
                if (RecordFilm) {
                    Angle = aMyFunction::HeadingDegreesToByte(Node->Heading);
                    if (aMyFunction::HeadingDegreesToByte(this->MovementDirection) != Angle) {
                        Globals::PrimaryFilm->SetObjectAngle(StepIndex, this->FilmObject, Angle);
                    }
                }
                this->Position = Node->Position;
                this->MovementDirection = Node->Heading;
                this->MovementPath->RemoveNode(this->MovementPath->ActiveHead);
            }
            if (RecordFilm) {
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, this->FilmObject, this->Position);
                if (this->AuxiliaryFilmObject != nullptr) {
                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, this->AuxiliaryFilmObject, this->Position);
                }
                if (aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetRadarCenter(StepIndex, this->Position);
                }
                if (this->FilmAlphaStep != 0.0L) {
                    this->FilmAlpha = static_cast<long double>(this->FilmAlpha) + this->FilmAlphaStep;
                    if (this->FilmAlpha <= 0.0L) {
                        Globals::PrimaryFilm->SetObjectAlpha(StepIndex, this->FilmObject, 0);
                    } else if (this->FilmAlpha >= 255.0L) {
                        Globals::PrimaryFilm->SetObjectAlpha(StepIndex, this->FilmObject, 255);
                    } else {
                        Globals::PrimaryFilm->SetObjectAlpha(StepIndex, this->FilmObject, System::Round(this->FilmAlpha));
                    }
                }
            }
        };
        std::uint8_t Result = false;
        if (IsHullDestroyed()) {
            return Result;
        }
        PickupPathUpdatesAllowed = InNormalSpace() && Order != soTeleport;
        if (PickupPathUpdatesAllowed && (Order == soLand || Order == soJump || Order == soJumpHole)) {
            PickupPathUpdatesAllowed = false;
            if (PickupTargets != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PickupTargets) - 1); cpp_range.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(PickupTargets, I);
                    Node = MovementPath->ActiveHead;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, MovementPath->NodeCount - 1); cpp_range_2.next(J); ) {
                        Distance = aMyFunction::PointDistance(Item->Position, Node->Position);
                        if (static_cast<long double>(Distance) <= GetCargoHookRange()) {
                            PickupPathUpdatesAllowed = true;
                            break;
                        }
                        Node = Node->Next;
                    }
                    if (PickupPathUpdatesAllowed) {
                        break;
                    }
                }
            }
        }
        if (Order == soNone) {
            if (FilmObject != nullptr) {
                AdvancePath();
            }
        } else if (Order == soMove) {
            if (MovementPath->ActiveHead != nullptr) {
                AdvancePath();
                if (Position.X == OrderDestination.X && Position.Y == OrderDestination.Y) {
                    OrderNone(false);
                }
            } else {
                OrderNone(false);
            }
        } else if (Order == soLand) {
            if (MovementPath->ActiveHead != nullptr) {
                AdvancePath();
                if (RecordFilm && aPlayer::GetPlayer() == this && OrderStateData == 0) {
                    if (pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
                        Globals::PrimaryFilm->SetCameraAnchor(StepIndex, pas::checked_cast<TShip*>(OrderTarget)->Position, true);
                    } else {
                        Globals::PrimaryFilm->SetCameraAnchor(StepIndex, pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->GetPosition(), true);
                    }
                }
                if ((MovementPath->ActiveHead == nullptr || MovementPath->ActiveHead->Next == nullptr) && OrderStateData == 1) {
                    if (aPlayer::GetPlayer() == this) {
                        aPlayer::GetPlayer()->ProcessCareerActivityAndEminentProgress();
                    }
                    ClearPickupTargets();
                    if (pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
                        DockedTo = pas::checked_cast<TShip*>(OrderTarget);
                    } else {
                        CurrentPlanet = pas::checked_cast<aPlanet::TPlanet*>(OrderTarget);
                        AbductedByPirateClan = false;
                        if (aPlayer::GetPlayer() == this && static_cast<std::uint8_t>(CurrentPlanet->HasPlayerLanded ^ 1)) {
                            CurrentPlanet->HasPlayerLanded = true;
                            if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                                Achievements::TrySetAchievementProgress(u"EXPLORER"_w, aPlayer::GetPlayer()->AchievementStats->UninhabitedPlanetsVisited);
                                Achievements::TryAddAchievementProgress(u"EXPLORER"_w, 1);
                                ++aPlayer::GetPlayer()->AchievementStats->UninhabitedPlanetsVisited;
                            }
                        }
                    }
                    OrderNone(true);
                    AfterburnerActive = false;
                    RefreshTechKnowledgeAtLocation();
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range_3.next(I); ) {
                        Item = pas::list_at<aItem::TItem>(Inventory, I);
                        if (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr) {
                            pas::checked_cast<aItem::TEngine*>(Item)->OutputPercent = 100;
                        }
                    }
                    if (RecordFilm) {
                        Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                        if (AuxiliaryFilmObject != nullptr) {
                            Globals::PrimaryFilm->DetachObject(StepIndex, AuxiliaryFilmObject);
                        }
                    }
                }
            }
        } else if (Order == soTeleport) {
            if (FilmObject != nullptr) {
                AdvancePath();
                if (FilmAlphaStep < 0.0L && FilmAlpha <= 0.0L && (OrderStateData > 0 || OrderTarget != CurrentStar)) {
                    Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                    if (AuxiliaryFilmObject != nullptr) {
                        Globals::PrimaryFilm->DetachObject(StepIndex, AuxiliaryFilmObject);
                    }
                }
            }
            HalfTurn = CurrentStar->MovementStepCount / 2;
            if (OrderStateData <= 0 && OrderTarget == CurrentStar && StepIndex == HalfTurn) {
                Position = OrderDestination;
                if (RecordFilm) {
                    FilmAlpha = -255.0f;
                    FilmAlphaStep = 7.65f;
                    Effect = pas::construct_call<SE_GAIEffect::TGAIEffectSE>(SE_GAIEffect::TGAIEffectSE_Create, u"Effect.TeleportIn"_wref.get(), ClassesImports::Point(0, 0));
                    EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    {
                        std::int32_t round = System::Round(Position.Y);
                        std::int32_t round_2 = System::Round(Position.X);
                        Globals::PrimaryFilm->SetEffectImagePosition(StepIndex, EffectFilm, ClassesImports::Point(round_2, round));
                    }
                    Globals::PrimaryFilm->SetEffectDurationScale(StepIndex, EffectFilm, 0.5f);
                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                    Globals::PrimaryFilm->PlayObjectSound(StepIndex + 1, FilmObject, u"Sound.TeleportIn"_wref.get());
                } else {
                    FilmAlpha = 255.0f;
                    FilmAlphaStep = 0.0f;
                }
                OrderNone(true);
            }
        } else if (Order == soJump) {
            if (aPlayer::GetPlayer() == this && aPlayer::GetPlayer()->QueuedTravelTarget != nullptr && StepIndex >= 50) {
                FilmAlphaStep = -1.785f;
            }
            if (static_cast<std::uint8_t>(InHyperspace ^ 1) && MovementPath->ActiveHead != nullptr) {
                AdvancePath();
                if (OrderDestination.X == 0.0L && OrderDestination.Y == 0.0L && MovementPath->ActiveHead == nullptr) {
                    InHyperspace = true;
                    if (RecordFilm) {
                        Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                        if (AuxiliaryFilmObject != nullptr) {
                            Globals::PrimaryFilm->DetachObject(StepIndex, AuxiliaryFilmObject);
                        }
                    }
                    ClearMovementPath();
                    if ((aPlayer::GetPlayer() == this || TypeId != aGalaxyStruct::stRanger || PartnerShip == nullptr || reinterpret_cast<aGalaxy::TStar*>(OrderTarget)->Status.ControlFaction != aGalaxyStruct::sfDominators && reinterpret_cast<aGalaxy::TStar*>(OrderTarget)->Status.CustomFaction == u"") && (aPlayer::GetPlayer() != this || aGalaxy::Galaxy->AmmoModEnabled != 1 && aPlayer::GetPlayer()->QueuedTravelTarget == nullptr)) {
                        Fuel = GetFuelTanks()->Fuel - static_cast<std::int32_t>(System::Round(aMyFunction::PointDistance(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Position, CurrentStar->Position)));
                        GetFuelTanks()->Fuel = Fuel;
                        if (aPlayer::GetPlayer() == this) {
                            SysUtilsImports::Sleep(1u);
                            if (GetFuelTanks()->Fuel != Fuel && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                                GR_Main::CCInterface->SetTamperDetected(true);
                            }
                        }
                    }
                    if (GetFuelTanks()->Fuel < 0) {
                        GetFuelTanks()->Fuel = 0;
                    }
                    RefreshDerivedStats(true);
                    pas::list_add(aGalaxy::Galaxy->ShipsInTransit, reinterpret_cast<void*>(this));
                    CurrentStar->HandleObjectLeavingStar(this);
                }
            }
        } else if (Order == soJumpHole) {
            if (OrderStateData == -65536 && MovementPath->ActiveHead != nullptr) {
                AdvancePath();
            } else if (static_cast<std::uint8_t>(InHyperspace ^ 1) && MovementPath->ActiveHead != nullptr) {
                AdvancePath();
                if (RecordFilm && FilmAlphaStep == 0.0L && aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetCameraAnchor(StepIndex, OrderDestination, true);
                }
                if (Position.X == OrderDestination.X && Position.Y == OrderDestination.Y) {
                    if (pas::in_range(reinterpret_cast<aGalaxy::THole*>(OrderTarget)->HoleType, 1, 1)) {
                        reinterpret_cast<aGalaxy::THole*>(OrderTarget)->HoleType = 3;
                    }
                    if (reinterpret_cast<aGalaxy::THole*>(OrderTarget)->HoleType == 4 && aPlayer::GetPlayer() == this && (aKling::KellerShip == nullptr || aKling::KellerShip->InHyperspace) && aGalaxy::Galaxy->KellerMissionState == 5) {
                        reinterpret_cast<aGalaxy::THole*>(OrderTarget)->CreatedTurn = aGalaxy::Galaxy->CurrentTurn - 10 - 1;
                    }
                    InHyperspace = true;
                    AbductedByPirateClan = false;
                    if (RecordFilm) {
                        Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                        if (AuxiliaryFilmObject != nullptr) {
                            Globals::PrimaryFilm->DetachObject(StepIndex, AuxiliaryFilmObject);
                        }
                    }
                    ClearMovementPath();
                    pas::list_add(aGalaxy::Galaxy->ShipsInTransit, reinterpret_cast<void*>(this));
                    CurrentStar->HandleObjectLeavingStar(this);
                }
            }
        } else if (Order == soTakeoff) {
            if (MovementPath->ActiveHead != nullptr) {
                AdvancePath();
            }
        } else if (Order == soFollowShip) {
            if (MovementPath->ActiveHead != nullptr) {
                AdvancePath();
                if (RecordFilm && aPlayer::GetPlayer() == this) {
                    Globals::PrimaryFilm->SetCameraAnchor(StepIndex, pas::checked_cast<TShip*>(OrderTarget)->Position, true);
                }
            }
        } else {
            pas::raise(pas::make_exception<pas::Exception>("Error in TShip.StepDay"_a));
        }
        if (RecordFilm && AuxiliaryFilmObject != nullptr) {
            if (FilmAlphaStep < 0.0L && InterceptorPassesRemaining > 1) {
                InterceptorPassesRemaining = 1;
            }
            if (InterceptorPassesRemaining <= 0) {
                AuxiliaryFilmObject->SceneObject->SetAlpha(std::min<std::int64_t>(static_cast<std::int64_t>(255), std::max<std::int64_t>(static_cast<std::int64_t>(0), System::Round(static_cast<std::int32_t>(AuxiliaryFilmObject->SceneObject->GetAlpha()) - 5.1L))));
            } else if (InterceptorPassesRemaining == 1) {
                AuxiliaryFilmObject->SceneObject->SetAlpha(std::min<std::int64_t>(static_cast<std::int64_t>(255), std::max<std::int64_t>(static_cast<std::int64_t>(0), System::Round(static_cast<std::int32_t>(AuxiliaryFilmObject->SceneObject->GetAlpha()) - 1.275L))));
            } else {
                AuxiliaryFilmObject->SceneObject->SetAlpha(std::min<std::int64_t>(static_cast<std::int64_t>(255), std::max<std::int64_t>(static_cast<std::int64_t>(0), System::Round(static_cast<std::int32_t>(AuxiliaryFilmObject->SceneObject->GetAlpha()) + 2.55L))));
            }
            Globals::PrimaryFilm->SetObjectAlpha(StepIndex, AuxiliaryFilmObject, AuxiliaryFilmObject->SceneObject->GetAlpha());
        }
        return Result;
    }

    void TShip::ClearCompletedTakeoffOrHoleOrder(std::int32_t UnusedStepIndex, std::uint8_t UnusedRecordFilm) {
        if (IsHullDestroyed()) {
            return;
        }
        if (Order == soTakeoff) {
            OrderNone(true);
        } else if (Order == soJumpHole && OrderStateData == -65536) {
            OrderNone(true);
        }
    }

    // Tests landing, jump, hole and teleport completion conditions against the prepared path.
    std::uint8_t TShip::IsTravelCompletionPathReady() {
        std::uint8_t Result = true;
        do {
            if (Order == soJump) {
                if (!InNormalSpace()) {
                    break;
                }
                if (MovementPath->ActiveHead == nullptr) {
                    break;
                }
                if (JumpDeparturePathCommitted) {
                    return Result;
                }
            } else if (Order == soJumpHole) {
                if (!InNormalSpace()) {
                    break;
                }
                if (OrderStateData == -65536) {
                    break;
                }
                if (MovementPath->ActiveHead == nullptr) {
                    break;
                }
                if (aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, OrderDestination) <= 0.0L) {
                    return Result;
                }
            } else if (Order == soTeleport) {
                if (InNormalSpace()) {
                    return Result;
                }
            } else if (Order == soLand && pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
                if (MovementPath->ActiveHead == nullptr) {
                    break;
                }
                if (aMyFunction::PointDistanceSquared(EC_Struct::AddPointsF(pas::checked_cast<TShip*>(OrderTarget)->Position, OrderDestination), MovementPath->ActiveTail->Position) <= 0.0L) {
                    return Result;
                }
            } else if (Order == soLand) {
                if (MovementPath->ActiveHead == nullptr) {
                    break;
                }
                {
                    pas::Extended cpp_right = pas::sqr(pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->GraphicRadius);
                    if (aMyFunction::PointDistanceSquared(pas::checked_cast<aPlanet::TPlanet*>(OrderTarget)->PredictPosition(CurrentStar->MovementStepCount), MovementPath->ActiveTail->Position) < cpp_right) {
                        return Result;
                    }
                }
            }
        } while (!true);
        return false;
    }

    // Adjusts RepulsionPosition on Self and nearby following ships using their collision radii.
    void TShip::RepelFollowingShips() {
        std::int32_t I{};
        std::int32_t Count{};
        TShip* Other{};
        float Distance{};
        float Overlap{};
        float VectorLength{};
        float Angle{};
        float DeltaY{};
        float DeltaX{};
        float OffsetY{};
        float OffsetX{};
        float SelfWeight{};
        float OtherWeight{};
        float Fraction{};
        try {
            Count = pas::list_count(CurrentStar->Ships);
            OffsetX = 0.0f;
            OffsetY = 0.0f;
            SelfWeight = pas::real_max<pas::Extended>(0.01L, ([&] {
                pas::Extended cpp_right = pas::real_max<double>(0.01, aMyFunction::PointDistance(RepulsionPosition, Position));
                return pas::real_divide(CalculateSpeed(), cpp_right);
            }()));
            do {
                I = 0;
                while (I < Count) {
                    Other = pas::list_at<TShip>(CurrentStar->Ships, I);
                    if (this == Other || Other->Order != soFollowShip) {
                        ++I;
                        continue;
                    }
                    OtherWeight = pas::real_max<pas::Extended>(0.01L, ([&] {
                        pas::Extended cpp_right_2 = pas::real_max<double>(0.01, aMyFunction::PointDistance(Other->RepulsionPosition, Other->Position));
                        return pas::real_divide(Other->CalculateSpeed(), cpp_right_2);
                    }()));
                    {
                        pas::Extended cpp_left = aMyFunction::SeededRandomIntRange(95, 105, static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) * (Other->Seed + Seed));
                        Fraction = cpp_left * pas::real_divide(SelfWeight, static_cast<long double>(SelfWeight) + OtherWeight) * 0.01L;
                    }
                    Distance = aMyFunction::PointDistance(RepulsionPosition, Other->RepulsionPosition);
                    Overlap = static_cast<long double>(Distance) - CollisionRadius - Other->CollisionRadius - 5.0L;
                    if (Overlap < -0.01L) {
                        Overlap = -Overlap;
                        if (Distance <= 1.0L) {
                            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) * (Other->Seed + Seed)));
                            DeltaX = System::Sin(Angle) * Overlap;
                            DeltaY = -System::Cos(Angle) * Overlap;
                        } else {
                            Distance = pas::real_divide(1.0L, Distance) * Overlap;
                            DeltaX = (static_cast<long double>(Other->RepulsionPosition.X) - RepulsionPosition.X) * Distance;
                            DeltaY = (static_cast<long double>(Other->RepulsionPosition.Y) - RepulsionPosition.Y) * Distance;
                        }
                        VectorLength = System::Sqrt(static_cast<long double>(DeltaX) * DeltaX + static_cast<long double>(DeltaY) * DeltaY);
                        if (VectorLength < 0.01L) {
                            DeltaX = pas::real_divide(0.01L * DeltaX, VectorLength);
                            DeltaY = pas::real_divide(0.01L * DeltaY, VectorLength);
                        }
                        Other->RepulsionPosition.X = Other->RepulsionPosition.X + static_cast<long double>(DeltaX) * Fraction;
                        Other->RepulsionPosition.Y = Other->RepulsionPosition.Y + static_cast<long double>(DeltaY) * Fraction;
                        OffsetX = OffsetX - (1.0L - Fraction) * DeltaX;
                        OffsetY = OffsetY - (1.0L - Fraction) * DeltaY;
                    }
                    ++I;
                }
            } while (!(I >= Count));
            RepulsionPosition.X = static_cast<long double>(RepulsionPosition.X) + OffsetX;
            RepulsionPosition.Y = static_cast<long double>(RepulsionPosition.Y) + OffsetY;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TShip.RepulsionShip ", Name}))));
            } else {
                throw;
            }
        }
    }

    // Uses the current turn's step limit; rewrites OrderDestination to the resulting endpoint.
    void TShip::RebuildMovePath() {
        ClearMovementPath();
        AppendStarAvoidingPath(OrderDestination, CurrentStar->MovementStepCount);
        if (MovementPath->ActiveTail != nullptr) {
            OrderDestination = MovementPath->ActiveTail->Position;
        } else {
            OrderDestination = Position;
        }
    }

    // Clears the old path and uses aCalc, aGroup, a 999999-node limit.
    void TShip::BuildFullPathTo(EC_Struct::TPointF Destination) {
        ClearMovementPath();
        if (MovementSpeed < 0.001L) {
            return;
        }
        AppendStarAvoidingPath(Destination, 999999);
    }

    void TShip::BuildPlanetLandingPath() {
        EC_Struct::TPointF Destination{};
        ClearMovementPath();
        if (MovementSpeed < 0.001L) {
            return;
        }
        if (Order != soLand) {
            return;
        }
        aPlanet::TPlanet* Planet = pas::checked_cast<aPlanet::TPlanet*>(OrderTarget);
        std::int32_t Steps = 0;
        while (true) {
            Steps += 200;
            if (Steps > 10000) {
                break;
            }
            Destination = EC_Struct::AddPointsF(Planet->PredictPosition(Steps), OrderDestination);
            AppendStarAvoidingPath(Destination, Steps);
            if (MovementPath->ActiveTail == nullptr) {
                break;
            }
            if (aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, Destination) < static_cast<long double>(MovementSpeed) * MovementSpeed) {
                pas::store_unaligned<EC_Struct::TPointF>(&MovementPath->ActiveTail->Position, Destination);
                break;
            }
        }
    }

    // Clears the path, normalizes heading and handles the current order; may commit jump departure or adjust the destination.
    void TShip::BuildOrderMovementPath(std::int32_t MaximumNodes) {
        aPlanet::TPlanet* Planet{};
        aGalaxy::THole* Hole{};
        EC_Struct::TPointF Point{};
        TShip* Ship{};
        float Angle{};
        float Distance{};
        float Radius{};
        float OuterRadius{};
        float InnerRadius{};
        float UpperDistance{};
        float LowerDistance{};
        aPath::PSPathNode Node{};
        JumpDeparturePathCommitted = false;
        if (aGalaxy::PlayerStar == CurrentStar && aPlayer::GetPlayer() == this) {
            MovementSpeed = pas::real_max<double>(0.5, MovementSpeed);
        }
        MovementDirection = aMyFunction::WrapHeadingDegrees(MovementDirection);
        ClearMovementPath();
        if (MovementSpeed < 0.001L) {
            return;
        }
        std::int32_t Steps = CurrentStar->MovementStepCount;
        if (aKling::TerronShip == this && Order == soMove && aGalaxy::Galaxy->TerronToStarTurn == 0) {
            AppendOrbitalPath(MaximumNodes);
            return;
        }
        if (Order == soLand && pas::class_cast_if<TShip*>(OrderTarget) != nullptr) {
            Ship = pas::checked_cast<TShip*>(OrderTarget);
            Point = Ship->Position;
            if (aMyFunction::PointDistanceSquared(Point, Position) > pas::sqr(4.0E+2L)) {
                Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, Ship->Id + Seed));
                Distance = 2.0E+2f;
                Point.X = Point.X + System::Sin(Angle) * Distance;
                Point.Y = Point.Y + -System::Cos(Angle) * Distance;
                AppendStarAvoidingPathWithTurnPadding(Point, MaximumNodes);
            } else {
                AppendStarAvoidingPathWithTurnPadding(EC_Struct::AddPointsF(Point, OrderDestination), MaximumNodes);
                if (MovementPath->ActiveHead == nullptr) {
                    {
                        float randomIntRange = aMyFunction::RandomIntRange(-5, 5);
                        float randomIntRange_2 = aMyFunction::RandomIntRange(-5, 5);
                        OrderDestination = EC_Struct::MakePointF(randomIntRange, randomIntRange_2);
                    }
                    AppendStarAvoidingPathWithTurnPadding(EC_Struct::AddPointsF(Point, OrderDestination), MaximumNodes);
                }
            }
        } else if (Order == soLand) {
            Planet = pas::checked_cast<aPlanet::TPlanet*>(OrderTarget);
            OuterRadius = Planet->GraphicRadius + 150;
            InnerRadius = Planet->GraphicRadius + 50;
            if (aPlayer::GetPlayer() == this) {
                Point = EC_Struct::AddPointsF(Planet->PredictPosition(Steps), OrderDestination);
                if (aMyFunction::PointDistanceSquared(Position, Planet->GetPosition()) > pas::sqr(static_cast<pas::Extended>(OuterRadius))) {
                    if (aMyFunction::PointDistanceSquared(Point, Planet->PredictPosition(Steps)) <= pas::sqr(static_cast<pas::Extended>(InnerRadius))) {
                        Distance = aMyFunction::PointDistance(Point, Position);
                        Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(aMyFunction::PointBearingDegrees(Position, Point)));
                        do {
                            Distance = 0.5L * Distance;
                            Point = Position;
                            Point.X = Point.X + System::Sin(Angle) * Distance;
                            Point.Y = Point.Y - System::Cos(Angle) * Distance;
                        } while (!(aMyFunction::PointDistanceSquared(Point, Planet->PredictPosition(Steps)) > pas::sqr(static_cast<pas::Extended>(InnerRadius)) || Distance <= 3.0E+1L));
                        UpperDistance = Distance * 2.0L;
                        LowerDistance = Distance;
                        while (static_cast<long double>(UpperDistance) - LowerDistance > 1.0E+1L) {
                            Distance = (static_cast<long double>(UpperDistance) + LowerDistance) * 0.5L;
                            Point = Position;
                            Point.X = Point.X + System::Sin(Angle) * Distance;
                            Point.Y = Point.Y - System::Cos(Angle) * Distance;
                            if (aMyFunction::PointDistanceSquared(Point, Planet->PredictPosition(Steps)) > pas::sqr(static_cast<pas::Extended>(InnerRadius))) {
                                LowerDistance = Distance;
                            } else {
                                UpperDistance = Distance;
                            }
                        }
                        Distance = LowerDistance;
                        Point = Position;
                        Point.X = Point.X + System::Sin(Angle) * Distance;
                        Point.Y = Point.Y - System::Cos(Angle) * Distance;
                    }
                }
            } else {
                Point = Planet->PredictPosition(Steps);
                {
                    pas::Extended cpp_right = aMyFunction::SeededRandomIntRange(-20, 20, Seed + Planet->GenerationSeed);
                    Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(aMyFunction::PointBearingDegrees(Planet->GetPosition(), Position) + cpp_right));
                }
                if (aMyFunction::PointDistanceSquared(Planet->GetPosition(), Position) > pas::sqr(static_cast<pas::Extended>(OuterRadius))) {
                    Distance = InnerRadius;
                } else {
                    Distance = pas::imod(pas::abs(static_cast<std::int32_t>(Planet->GenerationSeed + Seed + CurrentStar->GenerationSeed)), Planet->GraphicRadius) * 0.7L;
                }
                Point.X = Point.X + System::Sin(Angle) * Distance;
                Point.Y = Point.Y + -System::Cos(Angle) * Distance;
            }
            if (Position.X != Point.X || Position.Y != Point.Y) {
                AppendStarAvoidingPathWithTurnPadding(Point, MaximumNodes);
            }
        } else if (Order == soJumpHole) {
            if (OrderStateData == -65536) {
                AppendStarAvoidingPathWithTurnPadding(OrderDestination, MaximumNodes);
            } else {
                Hole = pas::checked_cast<aGalaxy::THole*>(OrderTarget);
                if (pas::shr(OrderStateData, 16) == 0) {
                    Point = Hole->Position1;
                } else {
                    Point = Hole->Position2;
                }
                if (aMyFunction::PointDistanceSquared(Point, Position) > pas::sqr(2.0E+2L)) {
                    {
                        pas::Extended cpp_right_2 = aMyFunction::SeededRandomIntRange(-20, 20, Hole->Id + Seed);
                        Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(aMyFunction::PointBearingDegrees(Point, Position) + cpp_right_2));
                    }
                    Distance = 1.0E+2f;
                    Point.X = Point.X + System::Sin(Angle) * Distance;
                    Point.Y = Point.Y + -System::Cos(Angle) * Distance;
                    AppendStarAvoidingPathWithTurnPadding(Point, MaximumNodes);
                } else {
                    AppendStarAvoidingPathWithTurnPadding(OrderDestination, MaximumNodes);
                }
            }
        } else if (Order == soFollowShip) {
            Ship = pas::checked_cast<TShip*>(OrderTarget);
            Radius = CalculateFollowRadius();
            if (Ship->MovementPath->ActiveHead == nullptr) {
                AppendStarAvoidingPathWithTurnPadding(aMyFunction::PointBehindHeading(Ship->Position, Ship->MovementDirection, Radius, pas::abs(static_cast<std::int32_t>(Ship->Seed + Seed))), MaximumNodes);
                if (MovementPath->ActiveTail != nullptr) {
                    OrderDestination = MovementPath->ActiveTail->Position;
                } else {
                    OrderDestination = Position;
                }
            } else if (Ship->MovementPath->NodeCount <= Steps) {
                AppendStarAvoidingPathWithTurnPadding(aMyFunction::PointBehindHeading(Ship->MovementPath->ActiveTail->Position, Ship->MovementPath->ActiveTail->Heading, Radius, pas::abs(static_cast<std::int32_t>(Ship->Seed + Seed))), MaximumNodes);
                if (MovementPath->ActiveTail != nullptr) {
                    OrderDestination = MovementPath->ActiveTail->Position;
                } else {
                    OrderDestination = Position;
                }
            } else {
                Node = (static_cast<void>(Ship->MovementPath), aPath::TSPath::GetFollowingNode(Ship->MovementPath->ActiveHead, Steps - 1));
                AppendStarAvoidingPathWithTurnPadding(aMyFunction::PointBehindHeading(Node->Position, Node->Heading, Radius, pas::abs(static_cast<std::int32_t>(Ship->Seed + Seed))), MaximumNodes);
                if (MovementPath->ActiveTail != nullptr) {
                    OrderDestination = MovementPath->ActiveTail->Position;
                } else {
                    OrderDestination = Position;
                }
            }
        } else if (Order == soJump) {
            Radius = pas::real_divide(CurrentStar->ComputeMapDiameter(), 2.0L) - 4.0E+2L;
            Radius = static_cast<long double>(Radius) * Radius;
            if (static_cast<long double>(Position.X) * Position.X + static_cast<long double>(Position.Y) * Position.Y > Radius) {
                if (std::fabs(static_cast<pas::Extended>(([&] {
                    double pointBearingDegrees = aMyFunction::PointBearingDegrees(CurrentStar->Position, pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Position);
                    double pointBearingDegrees_2 = aMyFunction::PointBearingDegrees(EC_Struct::MakePointF(0.0f, 0.0f), Position);
                    return aMyFunction::HeadingDifferenceDegrees(pointBearingDegrees, pointBearingDegrees_2);
                }()))) <= 5.0L) {
                    Angle = std::fabs(static_cast<pas::Extended>(aMyFunction::HeadingDifferenceDegrees(MovementDirection, aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-Position.X, Position.Y)))));
                    if (Angle >= 125.0L && Speed > 200 || Angle >= 175.0L) {
                        if (Angle < 175.0L && Speed >= 200 && aGalaxy::PlayerStar == CurrentStar) {
                            Distance = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Position.X) * Position.X + static_cast<long double>(Position.Y) * Position.Y));
                            Point.X = Position.X + static_cast<long double>(Position.X) * Distance * 1.0E+4L;
                            Point.Y = Position.Y + static_cast<long double>(Position.Y) * Distance * 1.0E+4L;
                            AppendTurningPath(Point, false, MaximumNodes);
                        }
                        AppendHyperspaceTransitionPath(1.0f);
                        if (MovementPath->ActiveTail != nullptr) {
                            OrderDestination = MovementPath->ActiveTail->Position;
                        } else {
                            OrderDestination = Position;
                        }
                        JumpDeparturePathCommitted = true;
                    } else {
                        Distance = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Position.X) * Position.X + static_cast<long double>(Position.Y) * Position.Y));
                        OrderDestination.X = Position.X + static_cast<long double>(Position.X) * Distance * pas::real_max<pas::Extended>(2.5E+2L, Speed * 0.25L);
                        OrderDestination.Y = Position.Y + static_cast<long double>(Position.Y) * Distance * pas::real_max<pas::Extended>(2.5E+2L, Speed * 0.25L);
                        AppendStarAvoidingPathWithTurnPadding(OrderDestination, MaximumNodes);
                    }
                } else {
                    AppendStarAvoidingPathWithTurnPadding(OrderDestination, MaximumNodes);
                }
            } else {
                AppendStarAvoidingPathWithTurnPadding(OrderDestination, MaximumNodes);
            }
        } else if (Order == soMove || Order == soJump || Order == soTakeoff) {
            AppendStarAvoidingPathWithTurnPadding(OrderDestination, MaximumNodes);
        }
    }

    // Turning then straight movement; may pad the player's visible turn to 200 nodes.
    void TShip::AppendPathToWithTurnPadding(EC_Struct::TPointF Destination, std::int32_t MaximumNodes) {
        AppendTurningPath(Destination, false, MaximumNodes);
        AppendStraightPath(Destination, MaximumNodes);
        if (aGalaxy::PlayerStar == CurrentStar && MovementPath->NodeCount < 200 && (aPlayer::GetPlayer()->CurrentPlanet == nullptr || aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->Order == soTakeoff)) {
            MovementPath->ResampleBezierRange(MovementPath->ActiveHead, MovementPath->ActiveTail, 200);
        }
        float Step = MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale;
        if (MovementPath->ActiveHead != nullptr) {
            if (aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, Destination) < static_cast<long double>(Step) * Step) {
                pas::store_unaligned<EC_Struct::TPointF>(&MovementPath->ActiveTail->Position, Destination);
            }
        }
    }

    void TShip::AppendPathTo(EC_Struct::TPointF Destination, std::int32_t MaximumNodes) {
        AppendTurningPath(Destination, false, MaximumNodes);
        AppendStraightPath(Destination, MaximumNodes);
        float Step = MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale;
        if (MovementPath->ActiveHead != nullptr) {
            if (aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, Destination) < static_cast<long double>(Step) * Step) {
                pas::store_unaligned<EC_Struct::TPointF>(&MovementPath->ActiveTail->Position, Destination);
            }
        }
    }

    void TShip::AppendStarAvoidingPathWithTurnPadding(EC_Struct::TPointF Destination, std::int32_t MaximumNodes) {
        AppendStarAvoidingPath(Destination, MaximumNodes);
        if (aGalaxy::PlayerStar == CurrentStar && MovementPath->NodeCount < 200 && (aPlayer::GetPlayer()->CurrentPlanet == nullptr || aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->Order == soTakeoff)) {
            MovementPath->ResampleBezierRange(MovementPath->ActiveHead, MovementPath->ActiveTail, 200);
        }
        float Step = MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale;
        if (MovementPath->ActiveHead != nullptr) {
            if (aMyFunction::PointDistanceSquared(MovementPath->ActiveTail->Position, Destination) < static_cast<long double>(Step) * Step) {
                pas::store_unaligned<EC_Struct::TPointF>(&MovementPath->ActiveTail->Position, Destination);
            }
        }
    }

    // Appends up to 200 nodes around the system origin, with visible-turn padding.
    void TShip::AppendOrbitalPath(std::int32_t MaximumNodes) {
        aPath::PSPathNode Node{};
        std::int32_t I{};
        float Step{};
        EC_Struct::TPointF Point{};
        if (MaximumNodes > 200) {
            MaximumNodes = 200;
        }
        if (MovementPath->ActiveTail == nullptr) {
            Point = Position;
        } else {
            Point = MovementPath->ActiveTail->Position;
        }
        float Radius = System::Sqrt(static_cast<long double>(Point.X) * Point.X + static_cast<long double>(Point.Y) * Point.Y);
        float Angle = Math::ArcTan2(Point.X, -Point.Y);
        if (Radius == 0.0L) {
            Step = 0.0f;
        } else {
            Step = pas::real_divide(MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale, Radius);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, MaximumNodes - 1); cpp_range.next(I); ) {
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            Node->Position.X = System::Sin(Angle) * Radius;
            Node->Position.Y = -System::Cos(Angle) * Radius;
            Node->Heading = 0.0f;
            Angle = static_cast<long double>(Angle) + Step;
        }
        if (CurrentStar == aGalaxy::PlayerStar && MovementPath->NodeCount < 200) {
            MovementPath->ResampleBezierRange(MovementPath->ActiveHead, MovementPath->ActiveTail, 200);
        }
    }

    void TShip::AppendTurningPath(EC_Struct::TPointF Destination, std::uint8_t AvoidStar, std::int32_t MaximumNodes) {
        aPath::PSPathNode Node{};
        EC_Struct::TPointF Point{};
        EC_Struct::TPointF Output{};
        EC_Struct::TPointF Center{};
        double Angle{};
        double Difference{};
        double PreviousRadiusSquared{};
        float StartRadialAngle{};
        float EndRadialAngle{};
        float ArcStart{};
        float ArcEnd{};
        std::uint8_t TooClose{};
        if (MovementPath->ActiveTail == nullptr) {
            Point = Position;
            Angle = MovementDirection;
        } else {
            Point = MovementPath->ActiveTail->Position;
            Angle = MovementPath->ActiveTail->Heading;
        }
        if (Point.X == Destination.X && Point.Y == Destination.Y) {
            if (MovementPath->ActiveHead == nullptr && Order == soJumpHole) {
                while (MovementPath->NodeCount < MaximumNodes) {
                    MovementPath->AppendNode();
                    Node = MovementPath->ActiveTail;
                    {
                        pas::Extended cpp_left = System::Cos(aMyFunction::HeadingDegreesToRadians(Angle));
                        Output.X = Point.X + (cpp_left - System::Cos(aMyFunction::HeadingDegreesToRadians(Angle + pas::real_divide(360 * MovementPath->NodeCount, MaximumNodes)))) * 3.0E+1L;
                    }
                    {
                        pas::Extended cpp_left_2 = System::Sin(aMyFunction::HeadingDegreesToRadians(Angle));
                        Output.Y = Point.Y + (cpp_left_2 - System::Sin(aMyFunction::HeadingDegreesToRadians(Angle + pas::real_divide(360 * MovementPath->NodeCount, MaximumNodes)))) * 3.0E+1L;
                    }
                    pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Output);
                    Node->Heading = Angle + pas::real_divide(360 * MovementPath->NodeCount, MaximumNodes);
                }
            }
            return;
        }
        double TurnStep = MovementTurnRate * 2.0E+2L * CurrentStar->MovementStepScale;
        double Step = MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale;
        if (Order == soTakeoff || Order == soJumpHole && OrderStateData == -65536) {
            Step = pas::real_divide(Step, 2.0L);
            TurnStep = pas::real_divide(TurnStep, 2.0L);
        }
        // The native code compares squared distance with the unsquared step here.
        if (aMyFunction::PointDistanceSquared(Point, Destination) < Step) {
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Point);
            Node->Heading = Angle;
            return;
        }
        double TargetAngle = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(Point.X) - Destination.X), static_cast<long double>(Point.Y) - Destination.Y));
        double TurnSign = aMyFunction::HeadingDifferenceDegrees(Angle, TargetAngle);
        if (std::fabs(static_cast<pas::Extended>(TurnSign)) < TurnStep) {
            return;
        }
        if (AvoidStar) {
            StartRadialAngle = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Point.X, -Point.Y));
            EndRadialAngle = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Destination.X, -Destination.Y));
            if (aMyFunction::HeadingDifferenceDegrees(StartRadialAngle, EndRadialAngle) <= 0.0L) {
                ArcStart = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-Point.X, Point.Y));
                ArcEnd = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(static_cast<long double>(Destination.X) - Point.X, -(static_cast<long double>(Destination.Y) - Point.Y)));
                if (aMyFunction::HeadingWithinArc(ArcStart, Angle, ArcEnd)) {
                    TurnSign = 1.0;
                } else {
                    TurnSign = -1.0;
                }
            } else {
                ArcStart = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-Point.X, Point.Y));
                ArcEnd = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(static_cast<long double>(Destination.X) - Point.X, -(static_cast<long double>(Destination.Y) - Point.Y)));
                if (aMyFunction::HeadingWithinArc(ArcStart, Angle, ArcEnd)) {
                    TurnSign = -1.0;
                } else {
                    TurnSign = 1.0;
                }
            }
        }
        double MiddleRadiusSquared = -1.0;
        double RadiusSquared = -1.0;
        double TangentStep = aMyFunction::CalculateTangentArcOffset(Point, Destination, Angle, TurnStep);
        if (TangentStep < Step) {
            Step = TangentStep;
        }
        if (TurnSign > 0.0L) {
            TargetAngle = Angle + 9.0E+1L;
        } else {
            TargetAngle = Angle - 9.0E+1L;
        }
        do {
            {
                pas::Extended cpp_left_3 = pas::real_divide(Step, aMyFunction::HeadingDegreesToRadians(TurnStep));
                Center.X = Point.X + cpp_left_3 * System::Sin(aMyFunction::HeadingDegreesToRadians(TargetAngle));
            }
            {
                pas::Extended cpp_left_4 = pas::real_divide(Step, aMyFunction::HeadingDegreesToRadians(TurnStep));
                Center.Y = Point.Y - cpp_left_4 * System::Cos(aMyFunction::HeadingDegreesToRadians(TargetAngle));
            }
            TooClose = aMyFunction::PointDistanceSquared(Center, Destination) * 0.95L < aMyFunction::PointDistanceSquared(Center, Point);
            if (TooClose) {
                Step = Step * 0.9L;
            }
        } while (TooClose);
        while (Point.X != Destination.X || Point.Y != Destination.Y) {
            if (AvoidStar) {
                PreviousRadiusSquared = MiddleRadiusSquared;
                MiddleRadiusSquared = RadiusSquared;
                RadiusSquared = static_cast<long double>(Point.X) * Point.X + static_cast<long double>(Point.Y) * Point.Y;
                if (PreviousRadiusSquared != -1.0L && PreviousRadiusSquared < MiddleRadiusSquared && RadiusSquared < MiddleRadiusSquared) {
                    break;
                }
            }
            TargetAngle = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(Point.X) - Destination.X), static_cast<long double>(Point.Y) - Destination.Y));
            Difference = aMyFunction::HeadingDifferenceDegrees(Angle, TargetAngle);
            if (std::fabs(static_cast<pas::Extended>(Difference)) <= TurnStep) {
                break;
            }
            if (TurnSign > 0.0L) {
                Angle = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Angle) + TurnStep);
            } else {
                Angle = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Angle) - TurnStep);
            }
            Point.X = Point.X + System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * Step;
            Point.Y = Point.Y - System::Cos(aMyFunction::HeadingDegreesToRadians(Angle)) * Step;
            if ((static_cast<long double>(Point.X) - Destination.X) * (static_cast<long double>(Point.X) - Destination.X) + (static_cast<long double>(Point.Y) - Destination.Y) * (static_cast<long double>(Point.Y) - Destination.Y) <= static_cast<long double>(Step) * Step) {
                Point = Destination;
            }
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Point);
            Node->Heading = Angle;
            if (MovementPath->NodeCount >= MaximumNodes) {
                break;
            }
        }
    }

    void TShip::AppendStraightPath(EC_Struct::TPointF Destination, std::int32_t MaximumNodes) {
        std::uint8_t UseY{};
        double Slope{};
        double Scale{};
        double Origin{};
        EC_Struct::TPointF Output{};
        EC_Struct::TPointF Point{};
        aPath::PSPathNode Node{};
        if (MovementPath->ActiveTail == nullptr) {
            Point = Position;
        } else {
            Point = MovementPath->ActiveTail->Position;
        }
        if (Point.X == Destination.X && Point.Y == Destination.Y) {
            return;
        }
        double Step = MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale;
        if (Order == soTakeoff || Order == soJumpHole && OrderStateData == -65536) {
            Step = pas::real_min<pas::Extended>(2.0L, pas::real_divide(Step, 2.0L));
        }
        double Heading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(Point.X) - Destination.X), static_cast<long double>(Point.Y) - Destination.Y));
        if (std::fabs(static_cast<long double>(Point.X) - Destination.X) < std::fabs(static_cast<long double>(Point.Y) - Destination.Y)) {
            UseY = true;
        } else {
            UseY = false;
        }
        double Distance = System::Sqrt((static_cast<long double>(Point.X) - Destination.X) * (static_cast<long double>(Point.X) - Destination.X) + (static_cast<long double>(Point.Y) - Destination.Y) * (static_cast<long double>(Point.Y) - Destination.Y));
        if (UseY) {
            Slope = pas::real_divide(static_cast<long double>(Destination.X) - Point.X, static_cast<long double>(Destination.Y) - Point.Y);
            Scale = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
            if (static_cast<long double>(Destination.Y) - Point.Y < 0.0L) {
                Scale = -Scale;
            }
            Origin = Point.Y;
        } else {
            Slope = pas::real_divide(static_cast<long double>(Destination.Y) - Point.Y, static_cast<long double>(Destination.X) - Point.X);
            Scale = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
            if (static_cast<long double>(Destination.X) - Point.X < 0.0L) {
                Scale = -Scale;
            }
            Origin = Point.X;
        }
        double Travelled = Step;
        if (Travelled >= Distance) {
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Destination);
            Node->Heading = Heading;
            return;
        }
        while (Travelled < Distance) {
            if (UseY) {
                Output.Y = Origin + static_cast<long double>(Travelled) * Scale;
                Output.X = Point.X + (static_cast<long double>(Output.Y) - Point.Y) * Slope;
            } else {
                Output.X = Origin + static_cast<long double>(Travelled) * Scale;
                Output.Y = Point.Y + (static_cast<long double>(Output.X) - Point.X) * Slope;
            }
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Output);
            Node->Heading = Heading;
            if (MovementPath->NodeCount >= MaximumNodes) {
                break;
            }
            Travelled = static_cast<long double>(Travelled) + Step;
        }
    }

    // Positive/negative Direction selects the outgoing/incoming transition path.
    void TShip::AppendHyperspaceTransitionPath(float Direction) {
        EC_Struct::TPointF Point{};
        float Heading{};
        float Speed{};
        float Increment{};
        aPath::PSPathNode Node{};
        std::int32_t I = 0;
        if (MovementPath->ActiveTail == nullptr) {
            Point = Position;
            Heading = MovementDirection;
        } else {
            Point = MovementPath->ActiveTail->Position;
            Heading = MovementPath->ActiveTail->Heading;
            if (aGalaxy::PlayerStar == CurrentStar) {
                I = std::min<std::int32_t>(100, MovementPath->NodeCount % 200);
            }
        }
        std::int32_t Count = CurrentStar->MovementStepCount;
        float Minimum = pas::real_divide(1.0L, 2.0E+2L * CurrentStar->MovementStepScale);
        float Maximum = pas::real_divide(3.0E+2L, Count - I);
        if (Direction > 0.0L) {
            Speed = Minimum;
            Increment = pas::real_divide(static_cast<long double>(Maximum) - Minimum, Count - I);
        } else {
            Speed = Maximum;
            Increment = -pas::real_divide(static_cast<long double>(Maximum) - Minimum, Count - I);
        }
        float SinHeading = System::Sin(aMyFunction::HeadingDegreesToRadians(Heading));
        float CosHeading = System::Cos(aMyFunction::HeadingDegreesToRadians(Heading));
        // The native increment is applied once, before the loop.
        Speed = static_cast<long double>(Speed) + Increment;
        while (I < Count) {
            Point.X = Point.X + static_cast<long double>(SinHeading) * Speed;
            Point.Y = Point.Y - static_cast<long double>(CosHeading) * Speed;
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Point);
            Node->Heading = Heading;
            ++I;
        }
    }

    void TShip::AppendStarAvoidingPath(EC_Struct::TPointF Destination, std::int32_t MaximumNodes) {
        EC_Struct::TPointF Point{};
        EC_Struct::TPointF StartTangent{};
        EC_Struct::TPointF OtherStartTangent{};
        EC_Struct::TPointF EndTangent{};
        EC_Struct::TPointF OtherEndTangent{};
        EC_Struct::TPointF PreviousTangent{};
        if (MovementPath->ActiveTail == nullptr) {
            Point = Position;
        } else {
            Point = MovementPath->ActiveTail->Position;
        }
        double Radius = CurrentStar->SafeRadius;
        Point = aMyFunction::PushPointOutsideCircleBand(Point, Radius, 2.0f);
        if (!aMyFunction::SegmentCrossesOriginCircle(Point, Destination, Radius)) {
            if (MaximumNodes < 20) {
                AppendPathTo(Destination, MaximumNodes);
            } else {
                AppendPathToWithTurnPadding(Destination, MaximumNodes);
            }
            return;
        }
        aMyFunction::CircleTangentPoints(Point, Radius, StartTangent, OtherStartTangent);
        aMyFunction::CircleTangentPoints(Destination, Radius, EndTangent, OtherEndTangent);
        if (aMyFunction::PointDistanceSquared(StartTangent, OtherEndTangent) < aMyFunction::PointDistanceSquared(OtherStartTangent, EndTangent)) {
            EndTangent = OtherEndTangent;
        } else {
            StartTangent = OtherStartTangent;
        }
        if (static_cast<long double>(Point.X) * Point.X + static_cast<long double>(Point.Y) * Point.Y < Radius * 2.0L * (Radius * 2.0L)) {
            AppendTurningPath(StartTangent, true, MaximumNodes);
            if (MovementPath->NodeCount >= MaximumNodes) {
                return;
            }
            if (MovementPath->ActiveTail == nullptr) {
                Point = Position;
            } else {
                Point = MovementPath->ActiveTail->Position;
            }
            Point = aMyFunction::PushPointOutsideCircleBand(Point, Radius, 2.0f);
            if (!aMyFunction::SegmentCrossesOriginCircle(Point, Destination, Radius)) {
                AppendPathTo(Destination, MaximumNodes);
                return;
            }
            {
                std::int64_t cpp_left = System::Round(static_cast<long double>(Point.X) * Point.X + static_cast<long double>(Point.Y) * Point.Y);
                if (cpp_left < System::Round(static_cast<long double>(Radius) * Radius)) {
                    if (MaximumNodes < 20) {
                        AppendPathTo(Destination, MaximumNodes);
                    } else {
                        AppendPathToWithTurnPadding(Destination, MaximumNodes);
                    }
                    return;
                }
            }
            PreviousTangent = StartTangent;
            aMyFunction::CircleTangentPoints(Point, Radius, StartTangent, OtherStartTangent);
            aMyFunction::CircleTangentPoints(Destination, Radius, EndTangent, OtherEndTangent);
            if (aMyFunction::PointDistanceSquared(StartTangent, PreviousTangent) > aMyFunction::PointDistanceSquared(OtherStartTangent, PreviousTangent)) {
                StartTangent = OtherStartTangent;
            }
            if (aMyFunction::PointDistanceSquared(StartTangent, OtherEndTangent) < aMyFunction::PointDistanceSquared(StartTangent, EndTangent)) {
                EndTangent = OtherEndTangent;
            }
            AppendPathTo(StartTangent, MaximumNodes);
            if (MovementPath->NodeCount >= MaximumNodes) {
                return;
            }
        } else {
            AppendPathTo(StartTangent, MaximumNodes);
            if (MovementPath->NodeCount >= MaximumNodes) {
                return;
            }
        }
        AppendCircularDetour(EndTangent, MaximumNodes, Radius);
        if (MovementPath->NodeCount >= MaximumNodes) {
            return;
        }
        AppendStraightPath(Destination, MaximumNodes);
    }

    void TShip::AppendCircularDetour(EC_Struct::TPointF Destination, std::int32_t MaximumNodes, double Radius) {
        aPath::PSPathNode Node{};
        EC_Struct::TPointF Point{};
        double Heading{};
        if (MovementPath->ActiveTail == nullptr) {
            Point = Position;
        } else {
            Point = MovementPath->ActiveTail->Position;
        }
        double FromHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Point.X, -Point.Y));
        double ToHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Destination.X, -Destination.Y));
        double DistancePerStep = MovementSpeed * 2.0E+2L * CurrentStar->MovementStepScale;
        double Step = pas::real_divide(DistancePerStep * 1.8E+2L, 3.1415926L * Radius);
        double Difference = aMyFunction::HeadingDifferenceDegrees(FromHeading, ToHeading);
        if (Difference < 0.0L) {
            Step = -Step;
        }
        if (std::fabs(static_cast<pas::Extended>(Difference)) <= 5.0L) {
            AppendPathTo(Destination, MaximumNodes);
            return;
        }
        while (FromHeading != ToHeading) {
            FromHeading = static_cast<long double>(FromHeading) + Step;
            if (FromHeading < 0.0L) {
                FromHeading = 3.6E+2L + FromHeading;
            }
            if (FromHeading >= 3.6E+2L) {
                FromHeading = FromHeading - 3.6E+2L;
            }
            if (std::fabs(static_cast<pas::Extended>(aMyFunction::HeadingDifferenceDegrees(FromHeading, ToHeading))) < std::fabs(static_cast<pas::Extended>(Step))) {
                FromHeading = ToHeading;
            }
            Point.X = System::Sin(aMyFunction::HeadingDegreesToRadians(FromHeading)) * Radius;
            Point.Y = -System::Cos(aMyFunction::HeadingDegreesToRadians(FromHeading)) * Radius;
            if (Step < 0.0L) {
                Heading = FromHeading - 9.0E+1L;
            } else {
                Heading = FromHeading + 9.0E+1L;
            }
            if (Heading < 0.0L) {
                Heading = 3.6E+2L + Heading;
            }
            if (Heading >= 3.6E+2L) {
                Heading = Heading - 3.6E+2L;
            }
            MovementPath->AppendNode();
            Node = MovementPath->ActiveTail;
            pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Point);
            Node->Heading = Heading;
            if (MovementPath->NodeCount >= MaximumNodes) {
                break;
            }
        }
    }

    void TShip::ClearMovementPath() {
        MovementPath->Clear();
    }

    std::int32_t TShip::GetSlotCount(aConst::TShipSlotKind SlotKind) {
        std::int32_t Result = GetHull()->GetSlotCount(SlotKind);
        switch (SlotKind) {
            case aConst::sskRadar: Result += GetOwnStatBonus(aConst::bonSlotRadar); break;
            case aConst::sskScanner: Result += GetOwnStatBonus(aConst::bonSlotScaner); break;
            case aConst::sskRepairRobot: Result += GetOwnStatBonus(aConst::bonSlotDroid); break;
            case aConst::sskCargoHook: Result += GetOwnStatBonus(aConst::bonSlotHook); break;
            case aConst::sskDefGenerator: Result += GetOwnStatBonus(aConst::bonSlotDef); break;
            case aConst::sskWeapon: Result += GetOwnStatBonus(aConst::bonSlotWeapon); break;
            case aConst::sskArtefact: Result += GetOwnStatBonus(aConst::bonSlotArt); break;
            case aConst::sskAfterburner: Result += GetOwnStatBonus(aConst::bonSlotForsage); break;
        }
        Result = std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result));
        if (pas::list_count(Artefacts) > 0) {
            if (SlotKind == aConst::sskWeapon) {
                if (CountActiveArtefacts(aConst::t_ArtWeaponToSpeed) > 0 && static_cast<std::uint8_t>(CanBoostArtefact(aConst::t_ArtWeaponToSpeed, GetHull(), false) ^ 1)) {
                    Result = std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result - CountActiveArtefacts(aConst::t_ArtWeaponToSpeed)));
                }
                if (CountActiveArtefacts(aConst::t_ArtDefToArms1) > 0 && GetDefGenerator() != nullptr) {
                    Result = std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result + CountActiveArtefacts(aConst::t_ArtDefToArms1)));
                }
                if (CountActiveArtefacts(aConst::t_ArtDefToArms2) > 0 && GetHull()->GetSlotCount(aConst::sskDefGenerator) > 0) {
                    return std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result + 1 + CountActiveArtefacts(aConst::t_ArtDefToArms2)));
                }
                return Result;
            } else if (SlotKind == aConst::sskDefGenerator) {
                if (CountActiveArtefacts(aConst::t_ArtDefToArms2) > 0) {
                    return std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result - 1));
                }
                return Result;
            } else if (SlotKind == aConst::sskRepairRobot) {
                if (CountActiveArtefacts(aConst::t_ArtArtefactor) > 0) {
                    return std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result - 1));
                }
                return Result;
            } else if (SlotKind == aConst::sskArtefact && Result > 0 && CountActiveArtefacts(aConst::t_ArtArtefactor) > 0 && GetHull()->GetSlotCount(aConst::sskRepairRobot) > 0) {
                return std::min<std::int32_t>(aConst::DefaultHullSlotCounts[SlotKind], std::max<std::int32_t>(aConst::MinimumHullSlotCounts[SlotKind], Result + 3));
            } else {
                return Result;
            }
        }
        return Result;
    }

    std::int32_t TShip::GetSlotCountForItemType(std::uint8_t ItemType) {
        return GetSlotCount(aConst::ItemTypeToSlotKind(ItemType));
    }

    // Repairs duplicate/out-of-range equipped slots; unequips overflow and preserves slot flag bit 7.
    void TShip::ReassignActiveItemSlots(aConst::TItemType ItemType) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Slot{};
        aItem::TEquipment* Item{};
        aConst::TShipSlotKind ItemKind{};
        std::uint8_t NoFreeSlot{};
        std::uint32_t Used = 0u;
        std::int32_t Count = GetSlotCountForItemType(static_cast<std::uint8_t>(ItemType));
        aConst::TShipSlotKind Kind = aConst::ItemTypeToSlotKind(static_cast<std::uint8_t>(ItemType));
        if (Kind != aConst::sskArtefact) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (Item->EquippedFlag != 0) {
                    ItemKind = aConst::ItemTypeToSlotKind(static_cast<std::uint8_t>(Item->ItemType));
                    if (Kind == ItemKind) {
                        Slot = Item->AssignedSlotData & aItem::EquipmentSlotIndexMask;
                        if (Slot < 0 || Slot >= Count || (pas::shr(Used, Slot) & 1) == 1) {
                            NoFreeSlot = true;
                            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(J); ) {
                                if ((pas::shr(Used, J) & 1) == 0) {
                                    Item->AssignedSlotData = Item->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | J;
                                    Used |= pas::shl(1, J);
                                    NoFreeSlot = false;
                                    break;
                                }
                            }
                            if (NoFreeSlot) {
                                Item->Unequip();
                            }
                        } else {
                            Used |= pas::shl(1, Item->AssignedSlotData & aItem::EquipmentSlotIndexMask);
                        }
                    }
                }
            }
        } else {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_3.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
                if (Item->EquippedFlag != 0) {
                    ItemKind = aConst::ItemTypeToSlotKind(static_cast<std::uint8_t>(Item->ItemType));
                    if (Kind == ItemKind) {
                        Slot = Item->AssignedSlotData & aItem::EquipmentSlotIndexMask;
                        if (Slot < 0 || Slot >= Count || (pas::shr(Used, Slot) & 1) == 1) {
                            NoFreeSlot = true;
                            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(J); ) {
                                if ((pas::shr(Used, J) & 1) == 0) {
                                    Item->AssignedSlotData = Item->AssignedSlotData & aItem::EquipmentSecondaryFireFlag | J;
                                    Used |= pas::shl(1, J);
                                    NoFreeSlot = false;
                                    break;
                                }
                            }
                            if (NoFreeSlot) {
                                Item->Unequip();
                            }
                        } else {
                            Used |= pas::shl(1, Item->AssignedSlotData & aItem::EquipmentSlotIndexMask);
                        }
                    }
                }
            }
        }
    }

    void TShip::RefreshAssignedItemSlots() {
        ReassignActiveItemSlots(aConst::t_FuelTanks);
        ReassignActiveItemSlots(aConst::t_Engine);
        ReassignActiveItemSlots(aConst::t_Radar);
        ReassignActiveItemSlots(aConst::t_Scaner);
        ReassignActiveItemSlots(aConst::t_RepairRobot);
        ReassignActiveItemSlots(aConst::t_CargoHook);
        ReassignActiveItemSlots(aConst::t_DefGenerator);
        ReassignActiveItemSlots(aConst::t_Weapon1);
        ReassignActiveItemSlots(aConst::t_Artefact);
        RefreshInactiveItemSlotAssignments();
    }

    // Slot indices are zero-based.
    aItem::TEquipment* TShip::FindEquippedItemInSlot(std::uint8_t ItemType, std::int32_t SlotIndex) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aConst::TShipSlotKind Kind = aConst::ItemTypeToSlotKind(ItemType);
        if (Kind != aConst::sskArtefact) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (Item->EquippedFlag != 0 && aConst::ItemTypeToSlotKind(static_cast<std::uint8_t>(Item->ItemType)) == Kind && (static_cast<std::int32_t>(Item->AssignedSlotData) & aItem::EquipmentSlotIndexMask) == SlotIndex) {
                    return Item;
                }
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(Artefacts, I);
                if (Item->EquippedFlag != 0 && (static_cast<std::int32_t>(Item->AssignedSlotData) & aItem::EquipmentSlotIndexMask) == SlotIndex) {
                    return Item;
                }
            }
        }
        return nullptr;
    }

    void TShip::RefreshInactiveItemSlotAssignments() {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Count = pas::list_count(Inventory);
        {
            const std::int32_t cpp_first = Count - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Item = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (Item->EquippedFlag == 0 && CountUnequippedItemsInSlot(Item->AssignedSlotData & aItem::EquipmentSlotIndexMask) > 1) {
                        Item->AssignedSlotData = FindFreeUnequippedSlot() | Item->AssignedSlotData & aItem::EquipmentSecondaryFireFlag;
                    }
                }
            }
        }
    }

    std::int32_t TShip::CountUnequippedItemsInSlot(std::int32_t SlotIndex) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Found = 0;
        std::int32_t Count = pas::list_count(Inventory);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag == 0 && static_cast<std::int32_t>(Item->AssignedSlotData & aItem::EquipmentSlotIndexMask) == SlotIndex) {
                ++Found;
            }
        }
        return Found;
    }

    std::int32_t TShip::FindFreeUnequippedSlot() {
        std::int32_t SlotIndex = 0;
        while (CountUnequippedItemsInSlot(SlotIndex) > 0) {
            ++SlotIndex;
        }
        return SlotIndex;
    }

    void TShip::RepairDuplicateSatelliteTrajectoryIndices() {
        std::int32_t I{};
        std::int32_t J{};
        aItem::TItem* First{};
        aItem::TItem* Second{};
        std::int32_t Count = pas::list_count(Inventory);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            First = pas::list_at<aItem::TItem>(Inventory, I);
            if (First->ItemType == aConst::t_Satellite) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, Count - 1); cpp_range_2.next(J); ) {
                    Second = pas::list_at<aItem::TItem>(Inventory, J);
                    if (Second->ItemType == aConst::t_Satellite && ([&] {
                        std::int32_t cpp_left = pas::checked_cast<aItem::TSatellite*>(Second)->TrajectoryIndex;
                        return cpp_left == pas::checked_cast<aItem::TSatellite*>(First)->TrajectoryIndex;
                    }())) {
                        pas::checked_cast<aItem::TSatellite*>(Second)->TrajectoryIndex = FindFreeSatelliteTrajectoryIndex();
                    }
                }
            }
        }
    }

    // One past the largest carried satellite index, or zero.
    std::int32_t TShip::GetSatelliteTrajectoryIndexLimit() {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Result = 0;
        std::int32_t Count = pas::list_count(Inventory);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            if (Item->ItemType == aConst::t_Satellite) {
                Result = std::max<std::int32_t>(Result, pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex + 1);
            }
        }
        return Result;
    }

    std::int32_t TShip::FindFreeSatelliteTrajectoryIndex() {
        std::int32_t I{};
        aItem::TItem* Item{};
        std::int32_t Result = 0;
        std::int32_t Count = pas::list_count(Inventory);
        while (true) {
            I = 0;
            while (I < Count) {
                Item = pas::list_at<aItem::TItem>(Inventory, I);
                if (Item->ItemType == aConst::t_Satellite && pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex == Result) {
                    break;
                }
                ++I;
            }
            if (I >= Count) {
                break;
            }
            ++Result;
        }
        return Result;
    }

    aItem::TSatellite* TShip::FindSatelliteByTrajectoryIndex(std::int32_t Index) {
        std::int32_t I{};
        aItem::TItem* Item{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            if (Item->ItemType == aConst::t_Satellite && pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex == Index) {
                return reinterpret_cast<aItem::TSatellite*>(Item);
            }
        }
        return nullptr;
    }

    // Shifts indices at or above Index upward.
    void TShip::InsertSatelliteTrajectoryIndex(std::int32_t Index) {
        std::int32_t I{};
        aItem::TItem* Item{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            if (Item->ItemType == aConst::t_Satellite && pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex >= Index) {
                ++pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex;
            }
        }
    }

    // Does nothing if Index is occupied.
    void TShip::RemoveEmptySatelliteTrajectoryIndex(std::int32_t Index) {
        std::int32_t I{};
        aItem::TItem* Item{};
        if (FindSatelliteByTrajectoryIndex(Index) != nullptr) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            if (Item->ItemType == aConst::t_Satellite && pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex >= Index) {
                --pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex;
            }
        }
    }

    void TShip::CompactSatelliteTrajectoryIndices() {
        std::int32_t I = 0;
        while (true) {
            if (GetSatelliteTrajectoryIndexLimit() <= I) {
                break;
            }
            if (FindSatelliteByTrajectoryIndex(I) == nullptr) {
                RemoveEmptySatelliteTrajectoryIndex(I);
            } else {
                ++I;
            }
        }
    }

    // Uses and refreshes global hold-view state.
    void TShip::AssignSatelliteIndicesFromHoldOrder() {
        pas::DynArray<std::int32_t> Indices{};
        std::int32_t I{};
        std::int32_t Next{};
        std::int32_t Count{};
        aItem::TEquipment* Item{};
        fShip2::TPlayerHoldUnit* Entry{};
        if (pas::list_count(Inventory) > 0) {
            RepairDuplicateSatelliteTrajectoryIndices();
            RefreshAssignedItemSlots();
            fShip2::PlayerHoldShip = this;
            fShip2::RefreshPlayerHoldView(true);
            Indices.set_length(pas::list_count(Inventory));
            Count = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, GetSatelliteTrajectoryIndexLimit() - 1); cpp_range.next(I); ) {
                Item = FindSatelliteByTrajectoryIndex(I);
                if (Item != nullptr) {
                    Indices[Count] = I;
                    ++Count;
                }
            }
            Next = 0;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(fShip2::PlayerHoldEntries) - 1); cpp_range_2.next(I); ) {
                Entry = pas::list_at<fShip2::TPlayerHoldUnit>(fShip2::PlayerHoldEntries, I);
                if (Entry == nullptr) {
                    continue;
                }
                if (Entry->Kind != fShip2::phkEquipment) {
                    continue;
                }
                if (pas::list_indexof(Inventory, reinterpret_cast<void*>(Entry->Item)) < 0) {
                    continue;
                }
                Item = pas::checked_cast<aItem::TEquipment*>(Entry->Item);
                if (Item->ItemType == aConst::t_Satellite) {
                    if (Next >= Count) {
                        GR_Main::RaiseWideMessage(u"Satellite renom 1"_wref.get());
                    }
                    pas::checked_cast<aItem::TSatellite*>(Item)->TrajectoryIndex = Indices[Next];
                    ++Next;
                }
            }
            if (Count != Next) {
                GR_Main::RaiseWideMessage(u"Satellite renom 2"_wref.get());
            }
            Indices = nullptr;
        }
    }

    // Reorders the satellite entries in the global hold view.
    void TShip::ArrangeHoldSatellitesByTrajectoryIndex() {
        pas::DynArray<std::int32_t> Indices{};
        pas::DynArray<fShip2::TPlayerHoldUnit*> Entries{};
        std::int32_t I{};
        std::int32_t Next{};
        std::int32_t Count{};
        aItem::TEquipment* Item{};
        fShip2::TPlayerHoldUnit* Entry{};
        if (pas::list_count(Inventory) > 0) {
            RepairDuplicateSatelliteTrajectoryIndices();
            RefreshAssignedItemSlots();
            fShip2::PlayerHoldShip = this;
            fShip2::RefreshPlayerHoldView(true);
            Indices.set_length(pas::list_count(Inventory));
            Entries.set_length(pas::list_count(Inventory));
            Count = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(fShip2::PlayerHoldEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<fShip2::TPlayerHoldUnit>(fShip2::PlayerHoldEntries, I);
                if (Entry != nullptr && Entry->Kind == fShip2::phkEquipment && pas::list_indexof(Inventory, reinterpret_cast<void*>(Entry->Item)) >= 0) {
                    Item = pas::checked_cast<aItem::TEquipment*>(Entry->Item);
                    if (Item->ItemType == aConst::t_Satellite) {
                        Entries[Count] = pas::make_object<fShip2::TPlayerHoldUnit>();
                        Entries[Count]->Kind = Entry->Kind;
                        Entries[Count]->GoodsIndex = Entry->GoodsIndex;
                        Entries[Count]->ItemId = Entry->ItemId;
                        Entries[Count]->Item = Entry->Item;
                        Entries[Count]->Retained = Entry->Retained;
                        Indices[Count] = I;
                        ++Count;
                    }
                }
            }
            Next = 0;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, GetSatelliteTrajectoryIndexLimit() - 1); cpp_range_2.next(I); ) {
                Item = FindSatelliteByTrajectoryIndex(I);
                if (Item != nullptr) {
                    if (Next >= Count) {
                        GR_Main::RaiseWideMessage(u"Satellite renom 3"_wref.get());
                    }
                    Entry = pas::list_at<fShip2::TPlayerHoldUnit>(fShip2::PlayerHoldEntries, Indices[Next]);
                    Entry->Kind = Entries[Next]->Kind;
                    Entry->GoodsIndex = Entries[Next]->GoodsIndex;
                    Entry->ItemId = Item->Id;
                    Entry->Item = reinterpret_cast<aItem::TItem*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item)) + 0)));
                    Entry->Retained = Entries[Next]->Retained;
                    ++Next;
                }
            }
            if (Count != Next) {
                GR_Main::RaiseWideMessage(u"Satellite renom 4"_wref.get());
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
                Entries[I]->Item = nullptr;
                pas::free(Entries[I]);
            }
            Entries = nullptr;
            Indices = nullptr;
        }
    }

    // Requires minimum charge; records a galaxy event and consumes charge even if no Dominators respond.
    std::uint8_t TShip::UseDominatorTransmitter(aItem::TArtefactTransmitter* Artefact) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Selected{};
        std::int32_t Available{};
        std::int32_t Penalty{};
        aGalaxy::TStar* Star{};
        TShip* Ship{};
        float Strength{};
        float PowerFactor{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (Artefact->Power >= aConst::MinTransmitterPower) {
            Penalty = 0;
            {
                const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + std::max<std::int32_t>(aConst::TransmitterSameSystemPenaltyTurns, aConst::TransmitterAnySystemPenaltyTurns) < aGalaxy::Galaxy->CurrentTurn) {
                            break;
                        }
                        if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerUsesBeacon" && pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + aConst::TransmitterAnySystemPenaltyTurns < aGalaxy::Galaxy->CurrentTurn) {
                            Penalty += aConst::TransmitterAnySystemPenalty;
                        }
                        if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerUsesBeacon" && pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->GetData(0) == static_cast<std::int32_t>(CurrentStar->Id) && pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + aConst::TransmitterSameSystemPenaltyTurns < aGalaxy::Galaxy->CurrentTurn) {
                            Penalty += aConst::TransmitterSameSystemPenalty;
                        }
                    }
                }
            }
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerUsesBeacon"_w, nullptr);
            Event->AddData(CurrentStar->Id);
            Selected = 0;
            Strength = 0.0f;
            PowerFactor = aMyFunction::RemapClamped(Artefact->Power, aConst::MinTransmitterPower, aConst::MaxTransmitterPower, 1.0, 3.0);
            if (CurrentStar->Id != 71 && CurrentStar->Id != 72) {
                for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
                    Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[I].Star));
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators && Star->Status.Battle == 0 && Star->Status.CustomFaction == u"" && (Star->Status.DominatorSeries == aGalaxyStruct::dsKeller || aMyFunction::NextRandomUnitFloat(RandomState) >= 0.8L && PowerFactor >= 2.0L)) {
                        Available = 0;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                            Ship = pas::list_at<TShip>(Star->Ships, J);
                            if (Ship->InNormalSpace() && pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1) && pas::is_one_of<soNone, soMove>(Ship->Order) && pas::in_range(static_cast<aKling::TKling*>(Ship)->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktEquentor), static_cast<std::int32_t>(aGalaxyStruct::ktShtip))) {
                                ++Available;
                            }
                        }
                        if (Available <= 1) {
                            Available = 0;
                        } else {
                            Available = std::max<std::int32_t>(1, Available - aMyFunction::NextRandomIntRange(2, 5, RandomState));
                        }
                        if (Available > 0) {
                            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                                Ship = pas::list_at<TShip>(Star->Ships, J);
                                if (Ship->InNormalSpace() && pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1) && pas::is_one_of<soNone, soMove>(Ship->Order) && pas::in_range(static_cast<aKling::TKling*>(Ship)->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktEquentor), static_cast<std::int32_t>(aGalaxyStruct::ktShtip))) {
                                    if (Star->Status.DominatorSeries == aGalaxyStruct::dsKeller && 1.0L - Penalty * 0.01L > aMyFunction::NextRandomUnitFloat(RandomState) && aKling::KellerShip != nullptr && aGalaxy::Galaxy->KellerLeaveTurn == 0) {
                                        Ship->OrderJump(CurrentStar, true);
                                    }
                                    if (Star->Status.DominatorSeries == aGalaxyStruct::dsBlazer && 1.0L - Penalty * 0.01L > aMyFunction::NextRandomUnitFloat(RandomState) && aKling::BlazerShip != nullptr) {
                                        Ship->OrderJump(CurrentStar, true);
                                    }
                                    if (Star->Status.DominatorSeries == aGalaxyStruct::dsTerron && 1.0L - Penalty * 0.01L > aMyFunction::NextRandomUnitFloat(RandomState) && aKling::TerronShip != nullptr) {
                                        Ship->OrderJump(CurrentStar, true);
                                    }
                                    --Available;
                                    ++Selected;
                                    Strength = static_cast<long double>(Strength) + Ship->StrengthInBestRanger;
                                    if (Available <= 0) {
                                        break;
                                    }
                                }
                            }
                        }
                        if (Selected > static_cast<long double>(aMyFunction::NextRandomIntRange(10, 12, RandomState)) * PowerFactor || Selected > 8.0L * PowerFactor && Strength > 8.0L * PowerFactor) {
                            break;
                        }
                    }
                }
            }
            Artefact->Power = 0;
            return true;
        }
        return false;
    }

    // Custom items match by configuration name; generic artefacts also compare names.
    std::uint8_t TShip::HasMatchingArtefactOrCustomItem(aItem::TItem* Item) {
        std::int32_t I{};
        aItem::TEquipment* Other{};
        std::uint8_t Result = false;
        if (Item->ItemType == aConst::t_UselessItem) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
                Other = pas::list_at<aItem::TEquipment>(Inventory, I);
                if (Other->ItemType == aConst::t_UselessItem && Other->ConfigBlockName == reinterpret_cast<aItem::TEquipment*>(Item)->ConfigBlockName) {
                    return true;
                }
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Other = pas::list_at<aItem::TEquipment>(Artefacts, I);
                if (Item->ItemType == Other->ItemType && (static_cast<std::uint8_t>(pas::is_one_of<aConst::t_Artefact, aConst::t_Artefact2>(Other->ItemType) ^ 1) || Other->ConfigBlockName == reinterpret_cast<aItem::TEquipment*>(Item)->ConfigBlockName)) {
                    return true;
                }
            }
        }
        return Result;
    }

    // Uses custom SharedEffect types and excludes broken items. Activation exceptions can count some unequipped artefacts.
    std::int32_t TShip::CountActiveArtefacts(std::uint8_t ArtefactType) {
        std::int32_t I{};
        aItem::TArtefact* Item{};
        std::int32_t Result = 0;
        std::uint8_t AllActive = false;
        if (pas::in_set<14, 15, 20, 20, 39, 39>(ArtefactType) && ArtefactType != static_cast<std::uint8_t>(aConst::t_ArtArtefactor)) {
            AllActive = CountActiveArtefacts(aConst::t_ArtArtefactor) > 0 && (static_cast<std::uint8_t>(pas::is_one_of<ThreadCalc::tcpGalaxyRunning, ThreadCalc::tcpPlayerStarRunning>(aCalc::TurnCalculationPhase) ^ 1) || CountActiveArtefacts(aConst::t_ArtArtefactor) > 1);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TArtefact>(Artefacts, I);
            if (static_cast<std::uint8_t>(Item->GetEffectiveType()) == ArtefactType && Item->BrokenFlag == 0 && (Item->EquippedFlag | static_cast<std::uint8_t>(AllActive)) != 0) {
                ++Result;
            }
        }
        return Result;
    }

    // Uses custom SharedUse and ConfigBlockName. Includes Item itself if equipped, and does not exclude broken items.
    std::uint8_t TShip::HasEquippedArtefactOfSameUseGroup(aItem::TItem* Item) {
        std::int32_t I{};
        aItem::TArtefact* Artefact{};
        std::uint8_t Result = false;
        if (!(pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr)) {
            return Result;
        }
        aConst::TItemType Kind = Item->ItemType;
        if (pas::in_range(Kind, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) && reinterpret_cast<aItem::TArtefactCustom*>(Item)->SharedUse) {
            Kind = reinterpret_cast<aItem::TArtefactCustom*>(Item)->CountsAsItemType;
        }
        if (!pas::in_range(Kind, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2))) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                if (Artefact->EquippedFlag != 0) {
                    if (Artefact->ItemType == Kind) {
                        return true;
                    }
                    if (pas::in_range(Artefact->ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) && reinterpret_cast<aItem::TArtefactCustom*>(Artefact)->CountsAsItemType == Kind && reinterpret_cast<aItem::TArtefactCustom*>(Artefact)->SharedUse) {
                        return true;
                    }
                }
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Artefacts) - 1); cpp_range_2.next(I); ) {
                Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I);
                if (Artefact->EquippedFlag != 0 && pas::in_range(Artefact->ItemType, static_cast<std::int32_t>(aConst::t_Artefact), static_cast<std::int32_t>(aConst::t_Artefact2)) && Artefact->ConfigBlockName == reinterpret_cast<aItem::TEquipment*>(Item)->ConfigBlockName) {
                    return true;
                }
            }
        }
        return Result;
    }

    // Item=nil checks cached installed equipment. A supplied item need not be equipped; eligible equipment types depend on ArtefactType.
    std::uint8_t TShip::CanBoostArtefact(std::uint8_t ArtefactType, aItem::TEquipment* Item, std::uint8_t IgnoreArtefactAvailability) {
        // Nested in CanBoostArtefact with unused caller-popped static link. OwnerId=6 and empty CustomFaction; nil returns false.
        auto IsArtefactBoostEquipment = [&](aItem::TEquipment* Item) -> std::uint8_t {
            return Item != nullptr && Item->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) && Item->CustomFaction == u"";
        };
        std::uint8_t Result = false;
        if (static_cast<std::uint8_t>(IgnoreArtefactAvailability ^ 1) && CountActiveArtefacts(ArtefactType) <= 0) {
            return Result;
        }
        if (!pas::in_set<10, 19, 21, 22, 26, 34, 36, 36, 40, 41>(ArtefactType)) {
            return Result;
        }
        if (Item == nullptr) {
            switch (ArtefactType) {
                case aConst::t_ArtefactHull: Result = IsArtefactBoostEquipment(GetHull()); break;
                case aConst::t_ArtefactFuel: Result = IsArtefactBoostEquipment(GetFuelTanks()); break;
                case aConst::t_ArtefactSpeed: Result = IsArtefactBoostEquipment(GetEngine()); break;
                case aConst::t_ArtefactPower: {
                    Result = IsArtefactBoostEquipment(GetEngine()) || IsArtefactBoostEquipment(GetHull());
                    break;
                }
                case aConst::t_ArtefactRadar: Result = IsArtefactBoostEquipment(GetRadar()); break;
                case aConst::t_ArtefactScaner: Result = IsArtefactBoostEquipment(GetScanner()); break;
                case aConst::t_ArtefactDroid: Result = IsArtefactBoostEquipment(GetRepairRobot()); break;
                case aConst::t_ArtefactHook: Result = IsArtefactBoostEquipment(GetCargoHook()); break;
                case aConst::t_ArtefactDef: Result = IsArtefactBoostEquipment(GetDefGenerator()); break;
                case aConst::t_ArtefactAntigrav: Result = IsArtefactBoostEquipment(GetHull()); break;
                case aConst::t_ArtDefToEnergy: Result = IsArtefactBoostEquipment(GetDefGenerator()); break;
                case aConst::t_ArtGiperJump: Result = IsArtefactBoostEquipment(GetEngine()); break;
                case aConst::t_ArtDefToArms1: Result = IsArtefactBoostEquipment(GetDefGenerator()); break;
                case aConst::t_ArtForsage: Result = IsArtefactBoostEquipment(GetEngine()); break;
                case aConst::t_ArtWeaponToSpeed: {
                    Result = IsArtefactBoostEquipment(GetEngine()) || IsArtefactBoostEquipment(GetHull());
                    break;
                }
                case aConst::t_ArtEnergyDef: Result = IsArtefactBoostEquipment(GetHull()); break;
                case aConst::t_ArtMissileDef: Result = IsArtefactBoostEquipment(GetHull()); break;
                case aConst::t_ArtefactMiniExpl: Result = IsArtefactBoostEquipment(GetScanner()); break;
                case aConst::t_ArtPDTurret: Result = IsArtefactBoostEquipment(GetRadar()); break;
            }
            if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtefactNano)) {
                if (IsArtefactBoostEquipment(GetEngine()) || IsArtefactBoostEquipment(GetFuelTanks()) || IsArtefactBoostEquipment(GetRadar()) || IsArtefactBoostEquipment(GetScanner()) || IsArtefactBoostEquipment(GetRepairRobot()) || IsArtefactBoostEquipment(GetCargoHook()) || IsArtefactBoostEquipment(GetDefGenerator()) || IsArtefactBoostEquipment(Weapons[1]) || IsArtefactBoostEquipment(Weapons[2]) || IsArtefactBoostEquipment(Weapons[3]) || IsArtefactBoostEquipment(Weapons[4]) || IsArtefactBoostEquipment(Weapons[5])) {
                    Result = true;
                }
            }
            if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtDefToEnergy) || ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtEnergyPulse)) {
                if (IsArtefactBoostEquipment(Weapons[1]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[1]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy) || IsArtefactBoostEquipment(Weapons[2]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[2]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy) || IsArtefactBoostEquipment(Weapons[3]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[3]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy) || IsArtefactBoostEquipment(Weapons[4]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[4]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy) || IsArtefactBoostEquipment(Weapons[5]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[5]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy)) {
                    Result = true;
                }
            }
            if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtSplinter) || ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtDecelerate)) {
                if (IsArtefactBoostEquipment(Weapons[1]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[1]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter) || IsArtefactBoostEquipment(Weapons[2]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[2]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter) || IsArtefactBoostEquipment(Weapons[3]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[3]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter) || IsArtefactBoostEquipment(Weapons[4]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[4]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter) || IsArtefactBoostEquipment(Weapons[5]) && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&Weapons[5]->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter)) {
                    Result = true;
                }
            }
            if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtFastRacks)) {
                if (IsArtefactBoostEquipment(Weapons[1]) && pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapons[1]->GetWeaponInfo()->ShotType) || IsArtefactBoostEquipment(Weapons[2]) && pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapons[2]->GetWeaponInfo()->ShotType) || IsArtefactBoostEquipment(Weapons[3]) && pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapons[3]->GetWeaponInfo()->ShotType) || IsArtefactBoostEquipment(Weapons[4]) && pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapons[4]->GetWeaponInfo()->ShotType) || IsArtefactBoostEquipment(Weapons[5]) && pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(Weapons[5]->GetWeaponInfo()->ShotType)) {
                    return true;
                }
            }
            return Result;
        }
        aItem::TEquipment* Equipment = Item;
        if (!IsArtefactBoostEquipment(Equipment)) {
            return Result;
        }
        switch (ArtefactType) {
            case aConst::t_ArtefactHull: {
                if (!(pas::class_cast_if<aItem::THull*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactFuel: {
                if (!(pas::class_cast_if<aItem::TFuelTanks*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactSpeed: {
                if (!(pas::class_cast_if<aItem::TEngine*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactPower: {
                if (!(pas::class_cast_if<aItem::TEngine*>(Equipment) != nullptr || pas::class_cast_if<aItem::THull*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactRadar: {
                if (!(pas::class_cast_if<aItem::TRadar*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactScaner: {
                if (!(pas::class_cast_if<aItem::TScaner*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactDroid: {
                if (!(pas::class_cast_if<aItem::TRepairRobot*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactDef: {
                if (!(pas::class_cast_if<aItem::TDefGenerator*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactAntigrav: {
                if (!(pas::class_cast_if<aItem::THull*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactHook: {
                if (!(pas::class_cast_if<aItem::TCargoHook*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtGiperJump: {
                if (!(pas::class_cast_if<aItem::TEngine*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtDefToArms1: {
                if (!(pas::class_cast_if<aItem::TDefGenerator*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtForsage: {
                if (!(pas::class_cast_if<aItem::TEngine*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtEnergyDef: {
                if (!(pas::class_cast_if<aItem::THull*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtMissileDef: {
                if (!(pas::class_cast_if<aItem::THull*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtefactMiniExpl: {
                if (!(pas::class_cast_if<aItem::TScaner*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
            case aConst::t_ArtPDTurret: {
                if (!(pas::class_cast_if<aItem::TRadar*>(Equipment) != nullptr)) {
                    return Result;
                }
                break;
            }
        }
        if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtefactNano) && pas::class_cast_if<aItem::THull*>(Equipment) != nullptr) {
            return Result;
        }
        if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtWeaponToSpeed) && !(pas::class_cast_if<aItem::THull*>(Equipment) != nullptr || pas::class_cast_if<aItem::TEngine*>(Equipment) != nullptr)) {
            return Result;
        }
        if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtDefToEnergy) && !(pas::class_cast_if<aItem::TDefGenerator*>(Equipment) != nullptr || pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy))) {
            return Result;
        }
        if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtEnergyPulse) && !(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkEnergy))) {
            return Result;
        }
        if ((ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtSplinter) || ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtDecelerate)) && !(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr && pas::contains(pas::load_unaligned<aGalaxyStruct::TDamageFlagSet>(&reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->DamageFlags), aGalaxyStruct::dkSplinter))) {
            return Result;
        }
        if (ArtefactType == static_cast<std::uint8_t>(aConst::t_ArtFastRacks) && !(pas::class_cast_if<aItem::TWeapon*>(Equipment) != nullptr && pas::is_one_of<aGalaxyStruct::wstTorpedo, aGalaxyStruct::wstMissile, aGalaxyStruct::wstRocket>(reinterpret_cast<aItem::TWeapon*>(Equipment)->GetWeaponInfo()->ShotType))) {
            return Result;
        }
        if (!pas::in_range(Equipment->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            return Result;
        }
        return true;
    }

    // Prefers installed inventory equipment; selects at most one repairable item and may clear BrokenFlag.
    void TShip::ApplyNanoArtefactRepair() {
        aItem::TEquipment* Item{};
        std::int32_t I{};
        // Caller-popped static link; ship -4, selected-item output -8. At most 30 deterministic selections.
        auto SelectRepairable = [&](std::int32_t Count, std::uint8_t Equipped) -> void {
            static const pas::Set<0, 255> RepairableTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
            std::int32_t Attempt = 0;
            do {
                ++Attempt;
                Item = nullptr;
                if (Attempt > 30) {
                    return;
                }
                aShip::SelectOrdinal(aMyFunction::SeededRandomIntRange(1, Count, aGalaxy::Galaxy->GenerationSeed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) + static_cast<std::uint32_t>(Attempt)), Equipped, this, Item);
                if (Item == nullptr) {
                    return;
                }
            } while (!(pas::contains(RepairableTypes, static_cast<std::uint8_t>(Item->ItemType)) && Item->ConditionPercent <= 6.0E+1L && !(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair) && aShip::TShip_CanRepairEquipmentTech(this, Item)));
        };
        if (pas::list_count(Inventory) < 1) {
            GR_Main::RaiseWideMessage(pas::concat_wide({u"Not equipments in ship ", GetName()}));
        }
        std::int32_t EquippedCount = 0;
        std::int32_t UnequippedCount = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(Inventory, I);
            if (Item->EquippedFlag != 0) {
                ++EquippedCount;
            } else {
                ++UnequippedCount;
            }
        }
        Item = nullptr;
        if (EquippedCount > 0) {
            SelectRepairable(EquippedCount, true);
        }
        if (Item == nullptr && UnequippedCount > 0) {
            SelectRepairable(UnequippedCount, false);
        }
        if (Item != nullptr) {
            Item->ConditionPercent = Item->ConditionPercent + pas::real_divide(aConst::NanoArtefactRepair, aConst::OwnerInfo[Item->OwnerId].EquipmentDurabilityFactor);
            if (CanBoostArtefact(aConst::t_ArtefactNano, Item, false)) {
                Item->ConditionPercent = Item->ConditionPercent + pas::real_divide(aConst::NanoArtefactBoostRepair, aConst::OwnerInfo[Item->OwnerId].EquipmentDurabilityFactor);
            }
            if (Item->ConditionPercent > 1.0E+2L) {
                Item->ConditionPercent = 1.0E+2;
            }
            if (Item->ConditionPercent > 0.0L && Item->BrokenFlag != 0) {
                Item->BrokenFlag = 0;
                if (aPlayer::GetPlayer() == this) {
                    auto& cpp_target = ([&] {
                        const pas::WideString& formatText1 = ([&] {
                            pas::WideString displayName = Item->GetDisplayName();
                            pas::WideString localizedText = aConst::LocalizedText(u"Artefacts.ArtNano.RepairItem"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName));
                        }());
                        std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                        return Globals::AddOrUpdatePlayerBubble(2, currentTurn, formatText1, u""_wref.get());
                    }())->Targets[0].ShipId;
                    cpp_target = Id;
                }
            }
        }
    }

    void SelectOrdinal(std::int32_t Ordinal, std::uint8_t Equipped, TShip* Self, aItem::TEquipment*& Item) {
        std::int32_t Index{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Self->Inventory) - 1); cpp_range.next(Index); ) {
            Item = pas::list_at<aItem::TEquipment>(Self->Inventory, Index);
            if (Item->EquippedFlag == static_cast<std::uint8_t>(Equipped)) {
                ++Count;
            }
            if (Count == Ordinal) {
                return;
            }
        }
        Item = nullptr;
    }

    // Uses Self's radar with a 500-unit minimum and both NoTalk flags; does not test system membership.
    std::uint8_t TShip::CanContactShip(TShip* OtherShip) {
        return static_cast<long double>(std::max<std::int32_t>(([&] {
            std::int32_t cpp_left = GetRadarRange();
            return cpp_left * GetRadarRange();
        }()), 250000)) >= aMyFunction::PointDistanceSquared(Position, OtherShip->Position) && static_cast<std::uint8_t>(OtherShip->NoTalk ^ 1) && static_cast<std::uint8_t>(NoTalk ^ 1);
    }

    // Selects among at most ten contiguous variants using ship seed and turn; substitutes ship names and HomePlanet. Missing text returns an unavailable marker.
    pas::WideString TShip_LookupTalkText(TShip* Self, const pas::WideString& Path) {
        pas::WideString Result{};
        std::int32_t Count{};
        pas::WideString Key{};
        pas::Array<pas::WideString, 0, 9> Variants{};
        auto GetTalkContextPrefix = [&](TShip* Ship) -> pas::WideString {
            pas::WideString Result{};
            Result = u"Talk."_w;
            if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) {
                return pas::concat_wide({Result, u"PirateClan."});
            } else if (Ship->IsFemaleHumanPilot()) {
                return pas::concat_wide({Result, u"Female."});
            } else {
                return Result;
            }
        };
        if (pas::pos(u"Talk.", Path) == 1) {
            if (aPlayer::GetPlayer() != Self) {
                Key = EC_Str::ReplaceAllWideString(Path, u"Talk."_wref.get(), GetTalkContextPrefix(Self));
            } else if (Globals::TalkShip != nullptr) {
                Key = EC_Str::ReplaceAllWideString(Path, u"Talk."_wref.get(), GetTalkContextPrefix(Globals::TalkShip));
            } else {
                Key = Path;
            }
        } else {
            Key = Path;
        }
        Count = 0;
        Variants[Count] = aConst::LocalizedText(Key);
        if (Variants[Count] != u"") {
            ++Count;
        }
        std::int32_t I = 1;
        do {
            Variants[Count] = aConst::LocalizedText(pas::concat_wide({Key, pas::wide_int_to_str(Count)}));
            if (Variants[Count] != u"") {
                ++Count;
            }
            ++I;
        } while (!(I > 9));
        if (Count == 0) {
            return pas::concat_wide({u"String: ", aMyFunction::WrapTextInColor(Key, u"<color=255,240,100>"_w), u" is unavailable"});
        }
        if (Count == 1) {
            Result = Variants[0];
        } else {
            Count = aMyFunction::SeededRandomIntRange(0, Count - 1, (aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(Self->Seed)) / 10);
            Result = Variants[Count];
        }
        if (Self->HomePlanet != nullptr) {
            Result = aMyFunction::ReplaceColoredToken(Result, u"<HomePlanet>"_w, Self->HomePlanet->Name, u"<color=255,240,100>"_w);
        }
        Result = ([&] {
            pas::WideString name = Self->GetName();
            pas::WideString result = Result;
            return aMyFunction::ReplaceColoredToken(std::move(result), u"<Ship>"_w, std::move(name), u"<color=255,240,100>"_w);
        }());
        Result = ([&] {
            pas::WideString fullName = Self->GetFullName(u" "_wref.get());
            pas::WideString result_2 = Result;
            return aMyFunction::ReplaceColoredToken(std::move(result_2), u"<FullShip>"_w, std::move(fullName), u"<color=255,240,100>"_w);
        }());
        Result = EC_Str::ReplaceAllWideString(Result, u"<clr>"_wref.get(), u"<color=255,240,100>"_wref.get());
        Result = EC_Str::ReplaceAllWideString(Result, u"<clrEnd>"_wref.get(), u"</color>"_wref.get());
        if (aPlayer::GetPlayer() == Self && Globals::TalkShip != nullptr) {
            pas::WideString name_2 = Globals::TalkShip->GetName();
            pas::WideString result_3 = Result;
            return aMyFunction::ReplaceColoredToken(std::move(result_3), u"<TalkShip>"_w, std::move(name_2), u"<color=255,240,100>"_w);
        }
        return Result;
    }

    // Turn-worker/UI handshake; waits for conversation completion or shutdown. Requires an active visible-space turn.
    std::uint8_t TShip_OpenPlayerConversation(TShip* Self, std::uint8_t RespectChameleon) {
        std::uint8_t Result{};
        pas::Array<WindowsImports::THandle, 0, 1> Handles{};
        if (GR_Main::ExitScreenLoop || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1) || aPlayer::GetPlayer()->CurrentStar != Self->CurrentStar || pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase) || GlobalsV::CurrentScreenId != GlobalsV::screenStarMap || aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return false;
        }
        if ((static_cast<std::uint8_t>(RespectChameleon) & static_cast<std::uint8_t>(aPlayer::GetPlayer()->ChameleonActive)) != 0 && Self->IsPlayerChameleonEffectiveAgainstSelf() && (Self->ScriptShip == nullptr || static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1))) {
            return false;
        }
        Globals::TalkShip = Self;
        Globals::TalkPlanet = nullptr;
        Globals::TalkScripted = RespectChameleon;
        WindowsSdk::ResetEvent(Globals::TalkCompletedEvent);
        WindowsSdk::SetEvent(Globals::TalkRequestEvent);
        WindowsSdk::SetEvent(Globals::ScriptUiRequestEvent);
        Handles[0] = Globals::TalkCompletedEvent;
        Handles[1] = Globals::ScriptUiAbortEvent;
        if (WindowsSdk::WaitForMultipleObjects(2u, reinterpret_cast<WindowsSdk::PWOHandleArray>(&Handles[0]), 0, WindowsSdk::INFINITE) != WindowsSdk::WAIT_OBJECT_0) {
            Result = false;
            Globals::TalkScripted = false;
            WindowsSdk::ResetEvent(Globals::TalkRequestEvent);
            return Result;
        }
        WindowsSdk::ResetEvent(Globals::ScriptUiRequestEvent);
        Globals::TalkScripted = false;
        SysUtilsImports::Sleep(10u);
        return true;
    }

    // Returns the global dialogue response, or zero when conversation cannot open. Amount only replaces the global amount when positive.
    std::uint8_t TShip_ShowPlayerDialogue(TShip* Self, std::uint8_t Kind, const pas::WideString& Text, std::int32_t Amount) {
        Globals::TalkType = Kind;
        if (Amount > 0) {
            Globals::TalkAmount = Amount;
        }
        Globals::TalkResponse = 0;
        Globals::TalkText = Text;
        std::uint8_t Result = 0;
        if (aShip::TShip_OpenPlayerConversation(Self, true)) {
            return Globals::TalkResponse;
        }
        return Result;
    }

    void TShip::NotifyMoneyDemand(TShip* OtherShip, pas::WideString Response, std::int32_t Amount) {
        pas::WideString Header{};
        pas::WideString Request{};
        if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            return;
        }
        std::int32_t RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
        if (!(static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(OtherShip->Position, aPlayer::GetPlayer()->Position))) {
            return;
        }
        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", OtherShip->GetFullName(u" "_wref.get()), u"</color>"});
        Request = pas::concat_wide({u"- ", ([&] {
            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Money.Send"_wref.get());
            pas::WideString intToStr = pas::wide_int_to_str(Amount);
            return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText), u"<Money>"_w, std::move(intToStr), u"<color=255,240,100>"_w);
        }())});
        Response = pas::concat_wide({u"- ", Response});
        {
            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, u"\r\n", Request, u"\r\n", Response}), u""_wref.get());
            cpp_with->Targets[0].ShipId = Id;
            cpp_with->Targets[1].ShipId = OtherShip->Id;
        }
    }

    void TShip::NotifyCargoDemand(TShip* OtherShip, pas::WideString Response) {
        pas::WideString Header{};
        pas::WideString Request{};
        if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            return;
        }
        std::int32_t RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
        if (!(static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(OtherShip->Position, aPlayer::GetPlayer()->Position))) {
            return;
        }
        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", OtherShip->GetFullName(u" "_wref.get()), u"</color>"});
        Request = pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(aShip::TShip_LookupTalkText(this, u"Talk.Goods.Send"_wref.get()), pas::WideString())});
        Response = pas::concat_wide({u"- ", Response});
        {
            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, u"\r\n", Request, u"\r\n", Response}), u""_wref.get());
            cpp_with->Targets[0].ShipId = Id;
            cpp_with->Targets[1].ShipId = OtherShip->Id;
        }
    }

    void TShip::NotifyFearCargoDrop(TShip* OtherShip) {
        pas::WideString Header{};
        std::int32_t RadarSquared{};
        pas::WideString Request{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
            if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(OtherShip->Position, aPlayer::GetPlayer()->Position)) {
                if ((aPlayer::GetPlayer() != OtherShip || static_cast<std::uint8_t>(aPlayer::GetPlayer()->ChameleonActive ^ 1) || static_cast<std::uint8_t>(IsPlayerChameleonEffectiveAgainstSelf() ^ 1)) && static_cast<std::uint8_t>(NoTalk ^ 1) && static_cast<std::uint8_t>(OtherShip->NoTalk ^ 1)) {
                    Request = aShip::TShip_LookupVisibleTalkText(this, u"Talk.DropGoodsInFear.Drop"_wref.get(), OtherShip);
                    aMyFunction::ReplaceTextToken(Request, u"<ShipBad>"_w, OtherShip->GetName(), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Request, u"<FullShipBad>"_w, OtherShip->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Request, u"<Star>"_w, CurrentStar->Name, u"<color=255,240,100>"_w);
                    Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", u":"});
                    Request = pas::concat_wide({u"- ", Request});
                    {
                        Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, u"\r\n", Request}), u""_wref.get());
                        cpp_with->Targets[0].ShipId = Id;
                        cpp_with->Targets[1].ShipId = OtherShip->Id;
                    }
                }
            }
        }
    }

    void TShip::NotifyTruceOffer(TShip* OtherShip, pas::WideString Response, std::int32_t Amount) {
        pas::WideString Header{};
        pas::WideString Request{};
        if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            return;
        }
        std::int32_t RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
        if (!(static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(OtherShip->Position, aPlayer::GetPlayer()->Position))) {
            return;
        }
        if (NoTalk || OtherShip->NoTalk) {
            return;
        }
        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", OtherShip->GetFullName(u" "_wref.get()), u"</color>"});
        Request = pas::concat_wide({u"- ", ([&] {
            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(this, pas::concat_wide({u"Talk.Truce.", GetTypeNameKey(), u"Send"}));
            pas::WideString intToStr = pas::wide_int_to_str(Amount);
            return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText), u"<Money>"_w, std::move(intToStr), u"<color=255,240,100>"_w);
        }())});
        Response = pas::concat_wide({u"- ", Response});
        {
            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, u"\r\n", Request, u"\r\n", Response}), u""_wref.get());
            cpp_with->Targets[0].ShipId = Id;
            cpp_with->Targets[1].ShipId = OtherShip->Id;
        }
    }

    void TShip::NotifyAttackRequest(TShip* OtherShip, pas::WideString Response, TShip* Target) {
        pas::WideString Header{};
        pas::WideString Request{};
        if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            return;
        }
        std::int32_t RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
        if (!(static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(OtherShip->Position, aPlayer::GetPlayer()->Position))) {
            return;
        }
        if (NoTalk || OtherShip->NoTalk) {
            return;
        }
        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", OtherShip->GetFullName(u" "_wref.get()), u"</color>"});
        Request = pas::concat_wide({u"- ", ([&] {
            pas::WideString fullName = Target->GetFullName(u" "_wref.get());
            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(this, pas::concat_wide({u"Talk.Attack.", GetTypeNameKey(), u"Send"}));
            return aMyFunction::ReplaceColoredToken(std::move(lookupTalkText), u"<Target>"_w, std::move(fullName), u"<color=255,240,100>"_w);
        }())});
        Response = pas::concat_wide({u"- ", Response});
        {
            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, u"\r\n", Request, u"\r\n", Response}), u""_wref.get());
            cpp_with->Targets[0].ShipId = Id;
            cpp_with->Targets[1].ShipId = OtherShip->Id;
            cpp_with->Targets[2].ShipId = Target->Id;
        }
    }

    void TShip::NotifyPartnershipOffer(TShip* OtherShip, pas::WideString Response, std::int32_t Amount) {
        pas::WideString Header{};
        pas::WideString Request{};
        if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            return;
        }
        std::int32_t RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
        if (!(static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(OtherShip->Position, aPlayer::GetPlayer()->Position))) {
            return;
        }
        if (NoTalk || OtherShip->NoTalk) {
            return;
        }
        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", OtherShip->GetFullName(u" "_wref.get()), u"</color>"});
        Request = pas::concat_wide({u"- ", ([&] {
            pas::WideString intToStr = pas::wide_int_to_str(Amount);
            pas::WideString lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Partner.Send"_wref.get());
            return aMyFunction::FormatText1(std::move(lookupTalkText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
        }())});
        Response = pas::concat_wide({u"- ", Response});
        {
            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, u"\r\n", Request, u"\r\n", Response}), u""_wref.get());
            cpp_with->Targets[0].ShipId = Id;
            cpp_with->Targets[1].ShipId = OtherShip->Id;
        }
    }

    void TShip::NotifyPartnerBreak(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateBreak"_wref.get());
                    TShip* self = this;
                    aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerBreak, lookupTalkText, 0);
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateBreak"_wref.get())});
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.AnswerLiderBreak"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    void TShip::NotifyPartnershipExpired(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    std::int32_t cpp_left = aPlayer::GetPlayer()->GetEffectiveSkillLevel(psLeadership, false);
                    if (cpp_left > aPlayer::GetPlayer()->CountWingmen()) {
                        const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateTheEnd"_wref.get());
                        TShip* self = this;
                        aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerEnd, lookupTalkText, 0);
                    } else {
                        const pas::WideString& lookupTalkText_2 = aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateTheEndLowLeadership"_wref.get());
                        TShip* self_2 = this;
                        aShip::TShip_ShowPlayerDialogue(self_2, aGalaxyStruct::tkPartnerEnd, lookupTalkText_2, 0);
                    }
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        if (aPlayer::GetPlayer() != Leader || ([&] {
                            std::int32_t cpp_left_2 = aPlayer::GetPlayer()->GetEffectiveSkillLevel(psLeadership, false);
                            return cpp_left_2 > aPlayer::GetPlayer()->CountWingmen();
                        }())) {
                            Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateTheEnd"_wref.get())});
                        } else {
                            Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateTheEndLowLeadership"_wref.get())});
                        }
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.AnswerLiderTheEnd"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    void TShip::NotifyPartnerRebellion(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateRiot"_wref.get());
                    TShip* self = this;
                    aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerRiot, lookupTalkText, 0);
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.MateRiot"_wref.get())});
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Partner.AnswerLiderRiot"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    void TShip::ShowMessageToPlayer(pas::WideString Text) {
        if (static_cast<std::uint8_t>(NoTalk ^ 1) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->NoTalk ^ 1)) {
            aGalaxy::TurnsSinceLastShipMessage = 0u;
            {
                Globals::TMessagePlayer* cpp_with = ([&] {
                    const pas::WideString& cpp_arg = pas::concat_wide({aMyFunction::WrapTextInColor(GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w), u"\r\n", u" ", u"\r\n", Text});
                    std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                    return Globals::AddOrUpdatePlayerBubble(1, currentTurn, cpp_arg, u""_wref.get());
                }());
                cpp_with->Targets[0].ShipId = Id;
                cpp_with->Targets[1].ShipId = aPlayer::GetPlayer()->Id;
            }
        }
    }

    void TShip::NotifyPiratePartnerRelationBreak(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateBreakRelation"_wref.get());
                    TShip* self = this;
                    aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerBreak, lookupTalkText, 0);
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateBreakRelation"_wref.get())});
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.AnswerLiderBreak"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    void TShip::NotifyPiratePartnerRatingBreak(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateBreakRating"_wref.get());
                    TShip* self = this;
                    aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerBreak, lookupTalkText, 0);
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateBreakRating"_wref.get())});
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.AnswerLiderBreak"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    void TShip::NotifyPiratePartnershipExpired(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    std::int32_t cpp_left = aPlayer::GetPlayer()->GetEffectiveSkillLevel(psLeadership, false);
                    if (cpp_left > aPlayer::GetPlayer()->CountWingmen()) {
                        const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateTheEnd"_wref.get());
                        TShip* self = this;
                        aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerEnd, lookupTalkText, 0);
                    } else {
                        const pas::WideString& lookupTalkText_2 = aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateTheEndLowLeadership"_wref.get());
                        TShip* self_2 = this;
                        aShip::TShip_ShowPlayerDialogue(self_2, aGalaxyStruct::tkPartnerEnd, lookupTalkText_2, 0);
                    }
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        if (aPlayer::GetPlayer() != Leader || ([&] {
                            std::int32_t cpp_left_2 = aPlayer::GetPlayer()->GetEffectiveSkillLevel(psLeadership, false);
                            return cpp_left_2 > aPlayer::GetPlayer()->CountWingmen();
                        }())) {
                            Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateTheEnd"_wref.get())});
                        } else {
                            Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateTheEndLowLeadership"_wref.get())});
                        }
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.AnswerLiderTheEnd"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    void TShip::NotifyPiratePartnerRebellion(TShip* Leader) {
        pas::WideString Header{};
        pas::WideString Request{};
        pas::WideString Response{};
        std::int32_t RadarSquared{};
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar && aPlayer::GetPlayer()->InNormalSpace()) {
            if (aPlayer::GetPlayer() == Leader && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(Leader->NoTalk ^ 1)) {
                {
                    const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateRiot"_wref.get());
                    TShip* self = this;
                    aShip::TShip_ShowPlayerDialogue(self, aGalaxyStruct::tkPartnerRiot, lookupTalkText, 0);
                }
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShipTalkedWithPlayer, this, nullptr, 0);
            } else {
                RadarSquared = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Position, aPlayer::GetPlayer()->Position) || static_cast<long double>(RadarSquared) >= aMyFunction::PointDistanceSquared(Leader->Position, aPlayer::GetPlayer()->Position)) {
                    if (static_cast<std::uint8_t>(NoTalk ^ 1) && (static_cast<std::uint8_t>(Leader->NoTalk ^ 1) || aPlayer::GetPlayer() == Leader)) {
                        Header = pas::concat_wide({u"<color=255,240,100>", GetFullName(u" "_wref.get()), u"</color>", aShip::TShip_LookupTalkText(this, u"Talk.To"_wref.get()), u"<color=255,240,100>", Leader->GetFullName(u" "_wref.get()), u"</color>"});
                        Request = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.MateRiot"_wref.get())});
                        Response = pas::concat_wide({u"\r\n- ", aShip::TShip_LookupTalkText(this, u"Talk.Pirate.AnswerLiderRiot"_wref.get())});
                        if (Leader->NoTalk) {
                            Response = pas::WideString();
                        }
                        {
                            Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(1, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Header, Request, Response}), u""_wref.get());
                            cpp_with->Targets[0].ShipId = Id;
                            cpp_with->Targets[1].ShipId = Leader->Id;
                        }
                    }
                }
            }
        }
    }

    // Native base implementation returns false; argument role and intended operation remain unresolved.
    std::uint8_t TShip::UnknownVirtualC0(void* Argument) {
        return false;
    }

    // Payment/wealth and relation determine contract months; a player stimulant can double the result.
    std::int32_t TShip::CalculatePartnershipMonths(std::int32_t Amount, TShip* OtherShip) {
        if (Amount < pas::real_divide(Wealth, 45.0L)) {
            return 0;
        }
        double cpp_arg = pas::real_divide(Wealth, 45.0L);
        double cpp_arg_2 = pas::real_divide(Wealth, 8.0L);
        pas::Extended cpp_left = aMyFunction::RemapClamped(Amount, cpp_arg, cpp_arg_2, 8.0, 36.0);
        std::int32_t Result = System::Round(cpp_left * aMyFunction::RemapClamped(aShip::TShip_RelationToShip(this, OtherShip) & 0x0000007f, 5.0E+1, 1.0E+2, 0.7, 1.5));
        if (aPlayer::GetPlayer() == OtherShip && OtherShip->IsHealthEffectActive(20)) {
            return Result * 2;
        }
        return Result;
    }

    pas::WideString TShip_GetGreetingText(TShip* Self) {
        pas::WideString Result{};
        pas::WideString Key{};
        std::uint8_t Series{};
        std::uint8_t Chameleon = aPlayer::GetPlayer()->ChameleonActive;
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr) {
            Key = u"Talk.Tranclucator.Greeting"_w;
        } else if (Self->IsFemaleHumanPilot()) {
            if (aPlayer::GetPlayer() == Self->PartnerShip) {
                if (Chameleon) {
                    Key = u"ShipGreetings.Standart.FemalePartnerChameleon"_w;
                } else {
                    Key = u"ShipGreetings.Standart.FemalePartner"_w;
                }
            } else if (Chameleon) {
                if (aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer()) < aGalaxyStruct::rlNormal) {
                    Key = u"ShipGreetings.Standart.FemaleChameleonWar"_w;
                } else {
                    Key = u"ShipGreetings.Standart.FemaleChameleon"_w;
                }
            } else {
                Result = aNormalShip::TNormalShip_SelectSituationalMessage(pas::checked_cast<aNormalShip::TNormalShip*>(Self), false);
                if (Result != u"") {
                    return Result;
                }
                switch (static_cast<std::int32_t>(aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer())) & 0x0000007f) {
                    case 0: Key = u"ShipGreetings.Standart.FemaleWar"_w; break;
                    case 1: Key = u"ShipGreetings.Standart.FemaleBad"_w; break;
                    case 2: Key = u"ShipGreetings.Standart.FemaleNormal"_w; break;
                    case 3: Key = u"ShipGreetings.Standart.FemaleGood"_w; break;
                    case 4: Key = u"ShipGreetings.Standart.FemaleBest"_w; break;
                }
            }
        } else if (aPlayer::GetPlayer() == Self->PartnerShip) {
            if (Chameleon && Self->TypeId != aGalaxyStruct::stPirate) {
                Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"PartnerChameleon"});
            } else {
                Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"Partner"});
            }
        } else if (pas::class_cast_if<aNormalShip::TNormalShip*>(Self) != nullptr) {
            if (Chameleon && Self->TypeId == aGalaxyStruct::stPirate && aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer()) < aGalaxyStruct::rlNormal) {
                Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"ChameleonWar"});
            } else if (Chameleon && (Self->TypeId == aGalaxyStruct::stPirate || Self->IsPlayerChameleonEffectiveAgainstSelf())) {
                Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"Chameleon"});
            } else {
                if (Self->LiberationGroup != nullptr && aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer()) > aGalaxyStruct::rlHostile) {
                    Result = pas::checked_cast<aGroup::TGroup*>(Self->LiberationGroup)->GetShipGreeting(Self);
                    if (Result != u"") {
                        return Result;
                    }
                }
                Result = aNormalShip::TNormalShip_SelectSituationalMessage(pas::checked_cast<aNormalShip::TNormalShip*>(Self), false);
                if (Result != u"") {
                    return Result;
                }
                switch (static_cast<std::int32_t>(aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer())) & 0x0000007f) {
                    case 0: Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"War"}); break;
                    case 1: Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"Bad"}); break;
                    case 2: {
                        Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"Normal"});
                        break;
                    }
                    case 3: {
                        Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"Good"});
                        break;
                    }
                    case 4: {
                        Key = pas::concat_wide({u"ShipGreetings.Standart.", Self->GetTypeNameKey(), u"Best"});
                        break;
                    }
                }
            }
        } else if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr) {
            if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->HasProgram(aGalaxyStruct::prgIntercom) ^ 1) || static_cast<std::uint8_t>(aPlayer::GetPlayer()->CanResolveObjectWithScanner(Self) ^ 1)) {
                Key = u"ShipGreetings.Dominator.Rnd"_w;
            } else if (pas::checked_cast<aKling::TKling*>(Self)->ActiveProgramAppliedTurn > 0) {
                Key = u"ShipGreetings.Dominator.ProgrammRun"_w;
            } else {
                Series = static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Self)->DominatorSeries);
                switch (Series) {
                    case 0: {
                        Key = pas::concat_wide({u"ShipGreetings.Dominator.", aConst::DominatorSeriesNames[0]});
                        break;
                    }
                    case 1: {
                        Key = pas::concat_wide({u"ShipGreetings.Dominator.", aConst::DominatorSeriesNames[1]});
                        break;
                    }
                    case 2: {
                        Key = pas::concat_wide({u"ShipGreetings.Dominator.", aConst::DominatorSeriesNames[2]});
                        break;
                    }
                }
                if (Chameleon && static_cast<std::uint8_t>(aPlayer::GetPlayer()->ChameleonSeries) == Series && static_cast<std::uint8_t>(aPlayer::GetPlayer()->ChameleonDetected[Series] ^ 1)) {
                    Key = pas::concat_wide({Key, u"Chameleon"});
                }
            }
        } else {
            return u"no greeting"_w;
        }
        Result = aShip::TShip_LookupTalkText(Self, Key);
        if (pas::class_cast_if<aKling::TKling*>(Self) != nullptr && static_cast<aKling::TKling*>(Self)->ActiveProgramAppliedTurn > 0) {
            aMyFunction::ReplaceTextToken(Result, u"<Name>"_w, (static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(pas::checked_cast<aKling::TKling*>(Self)->ActiveProgramId)), u"<color=255,240,100>"_w);
        }
        return Result;
    }

    // Requires ScriptShip; clears EndState, applies state orders and refreshes completion/pickup state.
    void TShip::InitializeScriptStateOrders() {
        aScript::TScriptShip* Binding = pas::checked_cast<aScript::TScriptShip*>(ScriptShip);
        Binding->EndState = false;
        ApplyScriptStateOrders();
        UpdateScriptStateCompletionAndPickups();
    }

    // Requires ScriptShip; may issue travel orders and assign script-selected weapon targets.
    void TShip::ApplyScriptStateOrders() {
        aScript::TScriptShip* OtherBinding{};
        TShip* FollowTarget{};
        std::int32_t WeaponTotal{};
        std::int32_t WeaponIndex{};
        std::int32_t BindingIndex{};
        std::int32_t BindingCount{};
        std::int32_t GroupIndex{};
        std::int32_t GroupCount{};
        aItem::TWeapon* Weapon{};
        float Distance{};
        float BestDistance{};
        aScript::TScriptPlace* Place{};
        aScript::TScriptShip* Binding = pas::checked_cast<aScript::TScriptShip*>(ScriptShip);
        aScript::TScriptState* State = Binding->State;
        if (InHyperspace || aKling::BlazerShip == this && OrderAbsolute) {
            return;
        }
        if (State->StateKind == aScript::sskIdle) {
            OrderNone(false);
        } else if (State->StateKind == aScript::sskMoveToPlace) {
            Place = reinterpret_cast<aScript::TScriptPlace*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue)));
            if (Place->PlaceKind == aScript::spkDockedPlanet) {
                if (IsOnPlanet() && reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Place->TargetValue))) == CurrentPlanet) {
                    OrderNone(false);
                } else if (IsOnPlanet() || IsDockedToShip()) {
                    OrderTakeoff();
                } else {
                    OrderLanding(reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Place->TargetValue))), false);
                }
            } else if (Place->PlaceKind == aScript::spkScriptItem) {
                if (Place->TargetValue == 0) {
                    OrderNone(false);
                } else if (pas::list_indexof(CurrentStar->Items, reinterpret_cast<void*>(reinterpret_cast<aScript::TScriptItem*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Place->TargetValue)))->Item)) < 0) {
                    OrderNone(false);
                } else if (IsOnPlanet() || IsDockedToShip()) {
                    OrderTakeoff();
                } else {
                    OrderMove(Place->GetRandomPoint((static_cast<std::uint32_t>(static_cast<std::int32_t>(Seed) + 0) + CurrentStar->GenerationSeed) * aGalaxy::Galaxy->CurrentTurn), false);
                }
            } else if (IsOnPlanet() || IsDockedToShip()) {
                OrderTakeoff();
            } else {
                OrderMove(Place->GetRandomPoint((static_cast<std::uint32_t>(static_cast<std::int32_t>(Seed) + 0) + CurrentStar->GenerationSeed) * aGalaxy::Galaxy->CurrentTurn), false);
            }
        } else if (State->StateKind == aScript::sskFollowGroup) {
            if (IsOnPlanet() || IsDockedToShip()) {
                OrderTakeoff();
            } else {
                FollowTarget = FindScriptFollowTarget();
                if (FollowTarget == nullptr) {
                    OrderNone(false);
                } else {
                    OrderFollowShip(FollowTarget, 0, false);
                }
            }
        } else if (State->StateKind == aScript::sskJumpToStar) {
            // Native repeats the planet test; it does not test docking here.
            if (IsOnPlanet() || IsOnPlanet()) {
                OrderTakeoff();
            } else if (InNormalSpace()) {
                OrderJump(reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue))), false);
            }
        } else if (State->StateKind == aScript::sskLandOnPlanet) {
            if (IsOnPlanet()) {
                if (reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue))) == CurrentPlanet) {
                    OrderNone(false);
                } else {
                    OrderTakeoff();
                }
            } else if (IsDockedToShip()) {
                OrderTakeoff();
            } else {
                OrderLanding(reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue))), false);
            }
        }
        if (WeaponCount > 0 && InNormalSpace()) {
            if (State->StateKind != aScript::sskNormalAI && pas::class_cast_if<aKling::TKling*>(this) != nullptr) {
                const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last) {
                    for (WeaponIndex = 1; WeaponIndex <= cpp_last; ++WeaponIndex) {
                        reinterpret_cast<TShip*>(reinterpret_cast<std::uint8_t*>(this) + 0)->Weapons[WeaponIndex]->Target = nullptr;
                    }
                }
            }
            if (HasScriptControl() && !(pas::class_cast_if<aKling::TKling*>(this) != nullptr)) {
                AssignWeaponTargetsInStar();
            }
            if (State->EnemyGroupIndices != nullptr) {
                WeaponTotal = WeaponCount;
                BindingCount = pas::list_count(Binding->Script->Ships);
                GroupCount = State->EnemyGroupIndices.length() - 1 + 1;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, WeaponTotal); cpp_range.next(WeaponIndex); ) {
                    Weapon = reinterpret_cast<TShip*>(reinterpret_cast<std::uint8_t*>(this) + 0)->Weapons[WeaponIndex];
                    if (aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                        BestDistance = 1.0E+15f;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, BindingCount - 1); cpp_range_2.next(BindingIndex); ) {
                            OtherBinding = pas::list_at<aScript::TScriptShip>(Binding->Script->Ships, BindingIndex);
                            if (reinterpret_cast<TShip*>(reinterpret_cast<std::uint8_t*>(this) + 0)->CurrentStar == OtherBinding->Ship->CurrentStar && OtherBinding->Ship->InNormalSpace()) {
                                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, GroupCount - 1); cpp_range_3.next(GroupIndex); ) {
                                    if (State->EnemyGroupIndices[GroupIndex] == OtherBinding->GroupIndex) {
                                        break;
                                    }
                                }
                                if (GroupIndex < GroupCount) {
                                    Distance = aMyFunction::PointDistanceSquared(Position, OtherBinding->Ship->Position);
                                    if (static_cast<long double>(pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) >= Distance && Distance < BestDistance) {
                                        BestDistance = Distance;
                                        Weapon->Target = reinterpret_cast<pas::Object*>(reinterpret_cast<std::uint8_t*>(OtherBinding->Ship) + 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last_2) {
                for (WeaponIndex = 1; WeaponIndex <= cpp_last_2; ++WeaponIndex) {
                    reinterpret_cast<TShip*>(reinterpret_cast<std::uint8_t*>(this) + 0)->Weapons[WeaponIndex]->Target = nullptr;
                }
            }
        }
    }

    // Requires ScriptShip; updates EndState and queues state-requested pickups.
    void TShip::UpdateScriptStateCompletionAndPickups() {
        std::int32_t I{};
        aItem::TItem* Item{};
        aScript::TScriptShip* Binding = pas::checked_cast<aScript::TScriptShip*>(ScriptShip);
        aScript::TScriptState* State = Binding->State;
        if (State->StateKind == aScript::sskIdle) {
            Binding->EndState = true;
        } else if (State->StateKind == aScript::sskMoveToPlace) {
            Binding->EndState = reinterpret_cast<aScript::TScriptPlace*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue)))->ShipInPlace(this);
        } else if (State->StateKind == aScript::sskFollowGroup) {
            Binding->EndState = FindScriptFollowTarget() == nullptr;
        } else if (State->StateKind == aScript::sskJumpToStar) {
            if (InHyperspace) {
                Binding->EndState = false;
            } else {
                Binding->EndState = CurrentStar == reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue)));
            }
        } else if (State->StateKind == aScript::sskLandOnPlanet) {
            Binding->EndState = IsOnPlanet() && CurrentPlanet == reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(State->TargetValue)));
        }
        if (InNormalSpace() && aShip::TShip_IsEquipmentUsable(this, GetCargoHook())) {
            if (State->PickupItem != nullptr && State->PickupItem->Item != nullptr) {
                Item = State->PickupItem->Item;
                if (pas::list_indexof(CurrentStar->Items, reinterpret_cast<void*>(Item)) >= 0 && static_cast<long double>(GetCargoHookRangeSquared()) >= aMyFunction::PointDistanceSquared(Item->Position, Position)) {
                    AddPickupTarget(Item, false);
                }
            }
            if (State->PickUpNearbyItems) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                    if (ShouldPickUpItem(Item) && aShip::TShip_IsItemInPickupRange(this, Item)) {
                        AddPickupTarget(Item, false);
                    }
                }
            }
        }
    }

    // Requires ScriptShip; script execution can remove the binding.
    void TShip::ScriptNextDay() {
        aScript::TScriptShip* Binding = pas::checked_cast<aScript::TScriptShip*>(ScriptShip);
        std::int32_t Stage = 0;
        try {
            Stage = 1;
            UpdateScriptStateCompletionAndPickups();
            Stage = 2;
            aScript::TScript_RunShipState(Binding->Script, Binding);
            if (ScriptShip == nullptr) {
                return;
            }
            Stage = 3;
            ApplyScriptStateOrders();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                if (ScriptShip != nullptr) {
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TShip.ScriptNextDay ", GetFullName(u" "_wref.get()), u" Id = ", EC_Str::IntToWideString(Id), u" script name = ", reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->Script->ScriptFileName, u" state type = ", EC_Str::IntToWideString(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->State->StateKind), u" order = ", EC_Str::IntToWideString(Order), u" label = ", pas::wide_int_to_str(Stage)}))));
                } else {
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TShip.ScriptNextDay ", GetFullName(u" "_wref.get()), u" Id = ", EC_Str::IntToWideString(Id), u" not in script now", u" order = ", EC_Str::IntToWideString(Order), u" label = ", pas::wide_int_to_str(Stage)}))));
                }
            } else {
                throw;
            }
        }
    }

    // Requires ScriptShip; matches the state's group in the current system.
    TShip* TShip::FindScriptFollowTarget() {
        std::int32_t I{};
        aScript::TScriptShip* Other{};
        aScript::TScriptShip* Binding = pas::checked_cast<aScript::TScriptShip*>(ScriptShip);
        aScript::TScriptState* State = Binding->State;
        std::int32_t Count = pas::list_count(Binding->Script->Ships);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Other = pas::list_at<aScript::TScriptShip>(Binding->Script->Ships, I);
            if (State->TargetValue == static_cast<std::uint32_t>(Other->GroupIndex) && Other->Ship->CurrentStar == CurrentStar) {
                return Other->Ship;
            }
        }
        return nullptr;
    }

    // Source kind 0 bypasses diminishing returns.
    void TShip::GainExperience(std::int32_t Amount, std::uint8_t SourceKind) {
        std::int32_t Awarded = Amount;
        if (SourceKind == 0) {
            TotalExperience += Amount;
            FreeExperience += Amount;
        } else {
            if (aPlayer::GetPlayer() == this) {
                switch (SourceKind) {
                    case 1: {
                        Awarded = System::Round(pas::real_divide(Awarded, aPlayer::GetPlayer()->ExperienceByDominators * 1.0E-6L + 1.0L));
                        aPlayer::GetPlayer()->ExperienceByDominators += Awarded;
                        break;
                    }
                    case 2: {
                        Awarded = System::Round(pas::real_divide(Awarded, (aPlayer::GetPlayer()->ExperienceByPirates + aPlayer::GetPlayer()->ExperienceByNormals) * 1.0E-6L + 1.0L));
                        aPlayer::GetPlayer()->ExperienceByPirates += Awarded;
                        break;
                    }
                    case 3: {
                        Awarded = System::Round(pas::real_divide(Awarded, (aPlayer::GetPlayer()->ExperienceByPirates + aPlayer::GetPlayer()->ExperienceByNormals) * 1.0E-6L + 1.0L));
                        aPlayer::GetPlayer()->ExperienceByNormals += Awarded;
                        break;
                    }
                    case 4: {
                        Awarded = System::Round(pas::real_divide(Awarded, aPlayer::GetPlayer()->ExperienceByTraderCareer * 1.0E-6L + 1.0L));
                        aPlayer::GetPlayer()->ExperienceByTraderCareer += Awarded;
                        break;
                    }
                }
            } else {
                Awarded = System::Round(pas::real_divide(Awarded, 1.0L + TotalExperience * 1.0E-6L));
            }
            TotalExperience += Awarded;
            FreeExperience += Awarded;
        }
        if (aPlayer::GetPlayer() == this) {
            Achievements::TryAddAchievementProgress(u"OLDFAG"_w, Awarded);
        }
    }

    // Subtracts independently from total and free experience, capped at each current balance.
    void TShip::RemoveExperience(std::int32_t Amount) {
        std::int32_t Removed = std::min<std::int32_t>(Amount, TotalExperience);
        TotalExperience -= Removed;
        Removed = std::min<std::int32_t>(Amount, FreeExperience);
        FreeExperience -= Removed;
    }

    // Deposits every carried stack and awards experience.
    void TShip::DepositCarriedNodes() {
        std::int32_t I{};
        aItem::TItem* Item{};
        {
            const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
            if (cpp_first >= 1) {
                for (I = cpp_first; I >= 1; --I) {
                    Item = pas::list_at<aItem::TItem>(Inventory, I);
                    if (Item->ItemType == aConst::t_Protoplasm) {
                        NodeReserve += Item->Weight;
                        if (pas::class_cast_if<aWarrior::TWarrior*>(this) != nullptr && static_cast<aWarrior::TWarrior*>(this)->WarriorType == aWarrior::wtFlagship) {
                            GainExperience(System::Round(Item->Weight * 2.0L), 0);
                        } else {
                            GainExperience(Item->Weight, 0);
                        }
                        if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr) {
                            pas::checked_cast<aRanger::TRanger*>(this)->BaseNodes += Item->Weight;
                        }
                        pas::list_delete(Inventory, I);
                        pas::free(Item);
                    }
                }
            }
        }
        RefreshDerivedStats(true);
    }

    std::uint8_t TShip::TrainSkill(TPilotSkill Skill) {
        std::uint8_t Result{};
        std::int32_t Expected{};
        if (BaseSkills[Skill] < 6 && aConst::SkillTrainingCosts[BaseSkills[Skill] + 1][Skill] <= FreeExperience) {
            ++BaseSkills[Skill];
            Expected = FreeExperience;
            FreeExperience -= aConst::SkillTrainingCosts[BaseSkills[Skill]][Skill];
            if (Expected - aConst::SkillTrainingCosts[BaseSkills[Skill]][Skill] != FreeExperience && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
            Expected = FreeExperience;
            Result = true;
            if (Globals::GetInnermostScreenLoop() == Globals::ShipScreen) {
                SysUtilsImports::Sleep(1u);
                if (Expected != FreeExperience && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                    GR_Main::CCInterface->SetTamperDetected(true);
                }
            }
            return Result;
        }
        return false;
    }

    std::uint8_t TShip::CanTrainSkill(TPilotSkill Skill) {
        return BaseSkills[Skill] < 6 && aConst::SkillTrainingCosts[BaseSkills[Skill] + 1][Skill] <= FreeExperience;
    }

    std::uint8_t TShip::GetBaseSkillLevel(TPilotSkill Skill) {
        return BaseSkills[Skill];
    }

    // Clamps to 0..6; equipment bonuses still apply when status effects are ignored.
    std::uint8_t TShip::GetEffectiveSkillLevel(TPilotSkill Skill, std::uint8_t IgnoreStatusEffects) {
        std::int32_t Level = BaseSkills[Skill];
        std::uint8_t BonusKind = Skill + aConst::bonSkill1;
        if (!IgnoreStatusEffects) {
            if (IsHealthEffectActive(1)) {
                if (Skill == psAccuracy) {
                    Level -= 3;
                }
                if (Skill == psManeuverability) {
                    Level -= 3;
                }
                if (Skill == psTechnical) {
                    Level -= 3;
                }
                if (Skill == psTrading) {
                    Level -= 3;
                }
            }
            if (IsHealthEffectActive(3)) {
                if (Skill == psAccuracy) {
                    Level += 1;
                }
                if (Skill == psManeuverability) {
                    Level += 1;
                }
                if (Skill == psCharisma) {
                    Level -= 1;
                }
                if (Skill == psLeadership) {
                    Level += 2;
                }
            }
            if (IsHealthEffectActive(5)) {
                if (Skill == psAccuracy) {
                    Level -= 2;
                }
                if (Skill == psCharisma) {
                    Level += 3;
                }
            }
            if (IsHealthEffectActive(6)) {
                if (Skill == psAccuracy) {
                    Level -= 3;
                }
                if (Skill == psManeuverability) {
                    Level -= 3;
                }
                if (Skill == psTechnical) {
                    Level -= 2;
                }
            }
            if (IsHealthEffectActive(7)) {
                if (Skill == psAccuracy) {
                    Level -= 2;
                }
                if (Skill == psManeuverability) {
                    Level -= 5;
                }
                if (Skill == psTrading) {
                    Level -= 10;
                }
            }
            if (IsHealthEffectActive(8)) {
                if (Skill == psAccuracy) {
                    Level -= 2;
                }
                if (Skill == psManeuverability) {
                    Level -= 2;
                }
                if (Skill == psTechnical) {
                    Level -= 2;
                }
                if (Skill == psCharisma) {
                    Level -= 1;
                }
                if (Skill == psLeadership) {
                    Level -= 1;
                }
            }
            if (IsHealthEffectActive(9)) {
                if (Skill == psLeadership) {
                    Level += 3;
                }
                if (Skill == psTrading) {
                    Level += 3;
                }
            }
            if (IsHealthEffectActive(10)) {
                if (Skill == psTechnical) {
                    Level -= 10;
                }
            }
            if (IsHealthEffectActive(11)) {
                if (Skill == psAccuracy) {
                    Level -= 1;
                }
                if (Skill == psTechnical) {
                    Level -= 2;
                }
                if (Skill == psTrading) {
                    Level += 2;
                }
            }
            if (IsHealthEffectActive(12)) {
                if (Skill == psAccuracy) {
                    Level += 1;
                }
                if (Skill == psManeuverability) {
                    Level += 1;
                }
            }
            if (IsHealthEffectActive(13)) {
                if (Skill == psAccuracy) {
                    Level += 4;
                }
                if (Skill == psManeuverability) {
                    Level += 3;
                }
            }
            if (IsHealthEffectActive(14)) {
                if (Skill == psCharisma) {
                    Level -= 1;
                }
                if (Skill == psLeadership) {
                    Level -= 1;
                }
            }
            if (IsHealthEffectActive(15)) {
                if (Skill == psAccuracy) {
                    Level += 1;
                }
                if (Skill == psManeuverability) {
                    Level += 1;
                }
                if (Skill == psTechnical) {
                    Level += 1;
                }
                if (Skill == psTrading) {
                    Level += 1;
                }
            }
            if (IsHealthEffectActive(16)) {
                if (Skill == psTechnical) {
                    Level += 5;
                }
            }
            if (IsHealthEffectActive(17)) {
                if (Skill == psAccuracy) {
                    Level += 4;
                }
                if (Skill == psManeuverability) {
                    Level += 2;
                }
            }
            if (IsHealthEffectActive(19)) {
                if (Skill == psCharisma) {
                    Level += 10;
                }
            }
            if (IsHealthEffectActive(20)) {
                if (Skill == psCharisma) {
                    Level += 1;
                }
                if (Skill == psLeadership) {
                    Level += 4;
                }
            }
            if (IsHealthEffectActive(21)) {
                if (Skill == psCharisma) {
                    Level -= 3;
                }
            }
            if (IsHealthEffectActive(22)) {
                if (Skill == psTrading) {
                    Level += 8;
                }
            }
            if (IsHealthEffectActive(23)) {
                if (Skill == psAccuracy) {
                    Level -= 1;
                }
                if (Skill == psManeuverability) {
                    Level -= 1;
                }
            }
            if (IsHealthEffectActive(24)) {
                if (Skill == psCharisma) {
                    Level += 2;
                }
            }
            if (TypeId == aGalaxyStruct::stKling) {
                if (pas::checked_cast<aKling::TKling*>(this)->IsProgramActive(aGalaxyStruct::prgShock)) {
                    if (Skill == psAccuracy) {
                        Level -= 4;
                    }
                    if (Skill == psManeuverability) {
                        Level -= 4;
                    }
                    if (Skill == psTechnical) {
                        Level -= 4;
                    }
                }
            }
        }
        Level += GetTotalStatBonus(BonusKind);
        return std::max<std::int32_t>(0, std::min<std::int32_t>(Level, 6));
    }

    // Returns a category from 1 to 5 using StrengthInBestRanger.
    std::uint8_t TShip::GetRelativeStrengthCategory() {
        std::uint8_t Percent{};
        std::uint8_t Result = 0;
        if (StrengthInBestRanger < 1.0L) {
            Percent = System::Round(aMyFunction::RemapClamped(StrengthInBestRanger, 0.1, 1.0, 0.0, 5.0E+1));
        } else {
            Percent = System::Round(aMyFunction::RemapClamped(StrengthInBestRanger, 1.0, 1.0E+1, 5.0E+1, 1.0E+2));
        }
        {
            std::uint8_t cpp_case = Percent;
            if (cpp_case >= 0 && cpp_case <= 20) {
                return 1;
            } else if (cpp_case >= 21 && cpp_case <= 40) {
                return 2;
            } else if (cpp_case >= 41 && cpp_case <= 60) {
                return 3;
            } else if (cpp_case >= 61 && cpp_case <= 80) {
                return 4;
            } else if (cpp_case >= 81 && cpp_case <= 100) {
                return 5;
            } else {
                GR_Main::RaiseWideMessage(u"function TPlayer.StrengthInStandartCnt:TStandartCnt;"_wref.get());
                return Result;
            }
        }
    }

    // Categories 1..5 split rounded hull percentage at 20, 50, 70 and 90.
    std::uint8_t TShip::GetHullConditionCategory() {
        std::uint8_t Result = 0;
        {
            std::int32_t cpp_case = static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0, GetHull()->Weight, 0.0, 1.0E+2)));
            if (cpp_case >= 0 && cpp_case <= 20) {
                return 1;
            } else if (cpp_case >= 21 && cpp_case <= 50) {
                return 2;
            } else if (cpp_case >= 51 && cpp_case <= 70) {
                return 3;
            } else if (cpp_case >= 71 && cpp_case <= 90) {
                return 4;
            } else if (cpp_case >= 91 && cpp_case <= 100) {
                return 5;
            } else {
                GR_Main::RaiseWideMessage(u"function TPlayer.StructureInStandartCnt:TStandartCnt;"_wref.get());
                return Result;
            }
        }
    }

    // Zero for non-rangers; otherwise 1..5 from the rounded experience-rank percentile. Uses all galaxy rangers, including excluded entries.
    std::uint8_t TShip::GetRangerRatingBand() {
        std::uint8_t Result = 0;
        if (TypeId == aGalaxyStruct::stRanger) {
            std::int32_t cpp_case = static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(pas::list_count(aGalaxy::Galaxy->Rangers) - pas::checked_cast<aRanger::TRanger*>(this)->PlaceInRating, 0.0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1, 0.0, 1.0E+2)));
            if (cpp_case >= 0 && cpp_case <= 10) {
                return 1;
            } else if (cpp_case >= 11 && cpp_case <= 40) {
                return 2;
            } else if (cpp_case >= 41 && cpp_case <= 60) {
                return 3;
            } else if (cpp_case >= 61 && cpp_case <= 90) {
                return 4;
            } else if (cpp_case >= 91 && cpp_case <= 100) {
                return 5;
            } else {
                GR_Main::RaiseWideMessage(u"function TShip.RatingInStandartCnt:TStandartCnt;"_wref.get());
                return Result;
            }
        }
        return Result;
    }

    std::uint8_t TShip::HasActiveDisease() {
        std::int32_t I{};
        for (I = 1; I <= 12; ++I) {
            if (CaptainHealth[I].Progress == 1.0E+2L) {
                return true;
            }
        }
        return false;
    }

    std::int32_t TShip::CountActiveDiseases() {
        std::int32_t I{};
        std::int32_t Result = 0;
        for (I = 1; I <= 12; ++I) {
            if (CaptainHealth[I].Progress == 1.0E+2L) {
                ++Result;
            }
        }
        return Result;
    }

    std::uint8_t TShip::HasPresentDisease() {
        std::int32_t I{};
        for (I = 1; I <= 12; ++I) {
            if (CaptainHealth[I].Progress > 0.0L) {
                return true;
            }
        }
        return false;
    }

    std::int32_t TShip::CountPresentDiseases() {
        std::int32_t I{};
        std::int32_t Result = 0;
        for (I = 1; I <= 12; ++I) {
            if (CaptainHealth[I].Progress > 0.0L) {
                ++Result;
            }
        }
        return Result;
    }

    std::uint8_t TShip::HasActiveStimulant() {
        std::int32_t I{};
        for (I = 13; I <= 24; ++I) {
            if (CaptainHealth[I].Progress == 1.0E+2L) {
                return true;
            }
        }
        return false;
    }

    std::int32_t TShip::CountActiveStimulants() {
        std::int32_t I{};
        std::int32_t Result = 0;
        for (I = 13; I <= 24; ++I) {
            if (CaptainHealth[I].Progress == 1.0E+2L) {
                ++Result;
            }
        }
        return Result;
    }

    std::int32_t TShip::CountPresentDiseasesAndActiveStimulants() {
        return CountPresentDiseases() + CountActiveStimulants();
    }

    std::uint8_t TShip::HasDiseaseFromCurrentPlanet() {
        std::int32_t I{};
        for (I = 1; I <= 12; ++I) {
            if (CaptainHealth[I].Progress > 0.0L && CurrentPlanet != nullptr && aPlayer::GetPlayer() == this && ([&] {
                pas::WideString cpp_string = CurrentPlanet->GetFullName(u" "_w);
                const pas::WideString& cpp_string_ref = aPlayer::GetPlayer()->StatusEffectSourceNames[I];
                return cpp_string == cpp_string_ref;
            }())) {
                return true;
            }
        }
        return false;
    }

    std::uint8_t TShip::HasDiseaseFromCurrentDockedShip() {
        std::int32_t I{};
        for (I = 1; I <= 12; ++I) {
            if (CaptainHealth[I].Progress > 0.0L && DockedTo != nullptr && aPlayer::GetPlayer() == this && ([&] {
                pas::WideString cpp_string = DockedTo->GetName();
                const pas::WideString& cpp_string_ref = aPlayer::GetPlayer()->StatusEffectSourceNames[I];
                return cpp_string == cpp_string_ref;
            }())) {
                return true;
            }
        }
        return false;
    }

    // Captain effect is active only when Progress equals 100.
    std::uint8_t TShip::IsHealthEffectActive(std::int32_t Index) {
        return CaptainHealth[Index].Progress == 1.0E+2L;
    }

    void TShip::SimulateNpcHealthEffects() {
        std::int32_t Index{};
        std::int32_t I{};
        if (aPlayer::GetPlayer() == this) {
            return;
        }
        if (ScriptShip != nullptr && IsOnPlanet()) {
            RefuelAtLocation();
        }
        if (DaysSincePlayerSeen > 100 && pas::in_range(TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stWarrior) && CountPresentDiseasesAndActiveStimulants() < static_cast<std::int32_t>(Seed) % 3 + 1) {
            if (DockedTo != nullptr && DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMedicalBase)) {
                for (I = 1; I <= 12; ++I) {
                    if (CaptainHealth[I].Progress != 0.0L) {
                        CaptainHealth[I].Progress = 0.0;
                        CaptainHealth[I].ExpireTurn = aGalaxy::Galaxy->CurrentTurn;
                    }
                }
                Index = aMyFunction::NextRandomIntRange(13, 24, RandomState);
                if (CaptainHealth[Index].Progress > 0.0L) {
                    Index = aMyFunction::NextRandomIntRange(13, 24, RandomState);
                }
                if (CaptainHealth[Index].Progress <= 0.0L && pas::contains(aConst::CaptainHealthDefinitions[Index].AllowedOwners, aConst::RaceToOwner(PilotRace))) {
                    CaptainHealth[Index].Progress = 1.0E+2;
                    CaptainHealth[Index].AppliedTurn = aGalaxy::Galaxy->CurrentTurn;
                    CaptainHealth[Index].ExpireTurn = ([&] {
                        std::int64_t cpp_right = System::Round(([&] {
                            pas::Extended cpp_left = aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->GenerationSeed + Index + aGalaxy::Galaxy->CurrentTurn), 0.0, 1.0, 0.5, 3.0);
                            return cpp_left * aConst::CaptainHealthDefinitions[Index].Duration;
                        }()));
                        return aGalaxy::Galaxy->CurrentTurn + cpp_right;
                    }());
                    ++CaptainHealth[Index].ApplicationCount;
                }
            } else if (DaysSincePlayerSeen % 100 == 0 && aMyFunction::NextRandomUnitFloat(RandomState) <= 0.1L) {
                Index = aMyFunction::NextRandomIntRange(1, 24, RandomState);
                if (CaptainHealth[Index].Progress > 0.0L) {
                    Index = aMyFunction::NextRandomIntRange(1, 24, RandomState);
                }
                if (CaptainHealth[Index].Progress <= 0.0L && pas::contains(aConst::CaptainHealthDefinitions[Index].AllowedOwners, aConst::RaceToOwner(PilotRace))) {
                    CaptainHealth[Index].Progress = 1.0E+2;
                    CaptainHealth[Index].AppliedTurn = aGalaxy::Galaxy->CurrentTurn;
                    CaptainHealth[Index].ExpireTurn = ([&] {
                        std::int64_t cpp_right_2 = System::Round(([&] {
                            pas::Extended cpp_left_2 = aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->GenerationSeed + Index + aGalaxy::Galaxy->CurrentTurn), 0.0, 1.0, 0.5, 3.0);
                            return cpp_left_2 * aConst::CaptainHealthDefinitions[Index].Duration;
                        }()));
                        return aGalaxy::Galaxy->CurrentTurn + cpp_right_2;
                    }());
                    ++CaptainHealth[Index].ApplicationCount;
                }
            }
        }
        if (CountActiveDiseases() > 0) {
            for (Index = 1; Index <= 12; ++Index) {
                if (CaptainHealth[Index].Progress != 0.0L && CaptainHealth[Index].ExpireTurn <= aGalaxy::Galaxy->CurrentTurn) {
                    CaptainHealth[Index].Progress = 0.0;
                }
            }
        }
    }

    std::uint8_t TShip::HasRadiationSickness() {
        return RadiationHealth[1].Progress > 0.0L;
    }

    std::int32_t TShip::CalculateSpeed() {
        double MassFactor{};
        double OutputFactor{};
        double OldSpeed{};
        std::int32_t Result = 0;
        if (GetEngine() == nullptr) {
            return Result;
        }
        if (GetFuelTanks() == nullptr) {
            return Result;
        }
        if (CargoFreeSpace < 0) {
            return Result;
        }
        if (aPlayer::GetPlayer() == this) {
            if (!aShip::TShip_CanUseEquipmentTech(this, GetFuelTanks())) {
                return Result;
            }
            if (!aShip::TShip_CanUseEquipmentTech(this, GetEngine())) {
                return Result;
            }
        }
        if (aGalaxy::Galaxy->IsOldSpeedCalculationEnabled()) {
            MassFactor = aMyFunction::RemapClamped(CalculateMass(), aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333);
            if (GetEngine()->OutputPercent == 100) {
                OutputFactor = 1.0;
            } else {
                OutputFactor = aMyFunction::RemapClamped(GetEngine()->OutputPercent, 0.0, 1.0E+2, 0.5, 1.0);
            }
            OldSpeed = static_cast<long double>(GetEngine()->Speed) * MassFactor * OutputFactor;
            if (AfterburnerActive && GetEngine()->BrokenFlag == 0 && GetSlotCount(aConst::sskAfterburner) > 0) {
                OldSpeed = static_cast<long double>(OldSpeed) * aConst::AfterburnerSpeedFactor;
            }
            if (GetEngine()->BrokenFlag != 0) {
                OldSpeed = OldSpeed * 0.6L;
            }
            if (TypeId == aGalaxyStruct::stKling) {
                if (pas::checked_cast<aKling::TKling*>(this)->IsProgramActive(aGalaxyStruct::prgInsanity)) {
                    OldSpeed = OldSpeed * 0.5L;
                }
            } else {
                if (IsHealthEffectActive(17)) {
                    OldSpeed = OldSpeed * 1.3L;
                }
                if (pas::list_count(Artefacts) > 0) {
                    if (CountActiveArtefacts(aConst::t_ArtefactSpeed) > 0) {
                        pas::Extended cpp_arg = aConst::SpeedArtefactFactor + static_cast<long double>(aConst::SpeedArtefactBoostFactor) * static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtefactSpeed, nullptr, false));
                        pas::Extended countActiveArtefacts = CountActiveArtefacts(aConst::t_ArtefactSpeed);
                        OldSpeed = OldSpeed * Math::Power(cpp_arg, countActiveArtefacts);
                    }
                    if (CountActiveArtefacts(aConst::t_ArtWeaponToSpeed) > 0) {
                        std::int32_t cpp_right = aConst::WeaponToSpeedArtefactBonus + aConst::WeaponToSpeedArtefactBoost * static_cast<std::uint8_t>(CanBoostArtefact(aConst::t_ArtWeaponToSpeed, GetEngine(), false));
                        OldSpeed = static_cast<long double>(OldSpeed) + CountActiveArtefacts(aConst::t_ArtWeaponToSpeed) * cpp_right;
                    }
                }
            }
            OldSpeed = pas::real_max<pas::Extended>(167.0L, static_cast<long double>(OldSpeed) + GetTotalStatBonus(aConst::bonSpeed));
            return System::Round(OldSpeed);
        }
        double CombinedFactor = 0.0;
        double MassPenalty = aMyFunction::RemapClamped(CalculateMass(), aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333);
        MassPenalty = pas::sqr(System::Ln(MassPenalty));
        double EnginePenalty = pas::sqr(System::Ln(aMyFunction::RemapClamped(GetEngine()->OutputPercent, 0.0, 1.0E+2, 0.5, 1.0)));
        if (GetEngine()->BrokenFlag != 0) {
            EnginePenalty = EnginePenalty + pas::sqr(System::Ln(0.6L));
        }
        if (!aShip::TShip_CanUseEquipmentTech(this, GetEngine())) {
            EnginePenalty = EnginePenalty + pas::sqr(System::Ln(0.4L));
        }
        if (TypeId == aGalaxyStruct::stKling) {
            if (pas::checked_cast<aKling::TKling*>(this)->IsProgramActive(aGalaxyStruct::prgInsanity)) {
                CombinedFactor = CombinedFactor + pas::sqr(System::Ln(0.5L));
            }
        }
        CombinedFactor = System::Exp(-System::Sqrt(static_cast<long double>(CombinedFactor) + MassPenalty + EnginePenalty));
        if (AfterburnerActive && aShip::TShip_IsEquipmentUsable(this, GetEngine()) && GetSlotCount(aConst::sskAfterburner) > 0) {
            CombinedFactor = static_cast<long double>(CombinedFactor) * aConst::AfterburnerSpeedFactor;
        }
        if (IsHealthEffectActive(17)) {
            CombinedFactor = CombinedFactor * 1.3L;
        }
        double NewSpeed = aShip::TShip_CalculateEngineSpeed(this, GetEngine(), false);
        NewSpeed = static_cast<long double>(NewSpeed) * CombinedFactor;
        std::int32_t MinimumSpeed = std::min<std::int32_t>(200, GetEngine()->Speed);
        if (static_cast<long double>(NewSpeed) < MinimumSpeed) {
            NewSpeed = MinimumSpeed + (static_cast<long double>(NewSpeed) - MinimumSpeed) * 0.2L;
        } else if (NewSpeed > 2.0E+3L) {
            NewSpeed = 1.25E+3L + NewSpeed * 0.2L;
        } else if (NewSpeed > 1.5E+3L) {
            NewSpeed = 6.5E+2L + NewSpeed * 0.5L;
        } else if (NewSpeed > 1.0E+3L) {
            NewSpeed = 2.0E+2L + NewSpeed * 0.8L;
        }
        return System::Round(NewSpeed);
    }

    // Human portrait IDs 25..32 on normal NPC ships; excludes special simulation mode.
    std::uint8_t TShip::IsFemaleHumanPilot() {
        return PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiHuman) && pas::in_range(PortraitFaceId, 25, 32) && aPlayer::GetPlayer() != this && aPlayer::GetPlayer() != nullptr && pas::class_cast_if<aNormalShip::TNormalShip*>(this) != nullptr && aGalaxy::Galaxy->SpecialSimulationMode == 0;
    }

    // Native resource key RangerOldFag; deterministic ID/creation-turn selection, excluding female pilots and the player.
    std::uint8_t TShip::UsesVeteranHumanRangerAppearance() {
        return TypeId == aGalaxyStruct::stRanger && PilotRace == static_cast<std::uint8_t>(aGalaxyStruct::oiHuman) && static_cast<std::uint32_t>(Id) % 6 == 0 && CreationTurn < 666 && aPlayer::GetPlayer() != this && aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(IsFemaleHumanPilot() ^ 1) && aGalaxy::Galaxy->SpecialSimulationMode == 0;
    }

    TShip* TShip::SelectInterceptorTarget() {
        std::int32_t I{};
        TShip* Ship{};
        aItem::TInterceptorTargetingStrategy Strategy{};
        // Caller-popped static link; source ship at -4 for distance strategies.
        auto IsBetterInterceptorTarget = [&](TShip* Current, TShip* Candidate, aItem::TInterceptorTargetingStrategy Strategy) -> std::uint8_t {
            switch (Strategy) {
                case aItem::itsMostHullPoints: return Candidate->GetHull()->HullPoints > Current->GetHull()->HullPoints;
                case aItem::itsFewestHullPoints: {
                    return Candidate->GetHull()->HullPoints < Current->GetHull()->HullPoints;
                }
                case aItem::itsStrongestDefense: {
                    pas::Extended cpp_left = aMyFunction::RemapClamped(Candidate->GetEffectiveSkillLevel(psManeuverability, false) & 0x0000007f, 0.0, 6.0, 1.5, 0.5) * 5.0E+1L * Candidate->DefenseDamageFactor - Candidate->GetHull()->Armor;
                    return cpp_left < aMyFunction::RemapClamped(Current->GetEffectiveSkillLevel(psManeuverability, false) & 0x0000007f, 0.0, 6.0, 1.5, 0.5) * 5.0E+1L * Current->DefenseDamageFactor - Current->GetHull()->Armor;
                }
                case aItem::itsGreatestStrength: return Candidate->Strength > Current->Strength;
                case aItem::itsNearest: {
                    return aMyFunction::PointDistanceSquared(Candidate->Position, this->Position) < aMyFunction::PointDistanceSquared(Current->Position, this->Position);
                }
                case aItem::itsFarthest: {
                    return aMyFunction::PointDistanceSquared(Candidate->Position, this->Position) > aMyFunction::PointDistanceSquared(Current->Position, this->Position);
                }
                default: return false;
            }
        };
        if (static_cast<std::uint8_t>(GetHull()->InterceptorsEnabled ^ 1) || static_cast<std::uint8_t>(InNormalSpace() ^ 1)) {
            return nullptr;
        }
        TShip* Current = nullptr;
        if (aPlayer::GetPlayer() == this) {
            Strategy = GetHull()->InterceptorTargetingStrategy;
        } else {
            Strategy = aItem::itsMostHullPoints;
        }
        if (Strategy == aItem::itsManual) {
            return nullptr;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<TShip>(CurrentStar->Ships, I);
            if (Ship != this && Ship->InNormalSpace() && (aPlayer::GetPlayer() == this || aShip::TShip_GetRelationLevelToShip(this, Ship) <= aGalaxyStruct::rlHostile) && (aPlayer::GetPlayer() != this || aShip::TShip_GetRelationLevelToShip(Ship, this) <= aGalaxyStruct::rlHostile || EnemyShip == Ship || aRanger::PendingPlayerFollowTarget == Ship) && pas::sqr(static_cast<pas::Extended>(Ship->Position.X)) + pas::sqr(static_cast<pas::Extended>(Ship->Position.Y)) >= static_cast<long double>(CurrentStar->DamageRadius) * CurrentStar->DamageRadius && aMyFunction::PointDistanceSquared(Position, Ship->Position) <= 1.0E+6L && Ship->InterceptorPassesRemaining <= 0 && (Current == nullptr || IsBetterInterceptorTarget(Current, Ship, Strategy))) {
                Current = Ship;
            }
        }
        return Current;
    }

    // Clears an explicit target before checking energy; successful launch installs source/pass state and a graphic on the target.
    void TShip::LaunchInterceptors() {
        TShip* Target{};
        if (GetHull()->InterceptorTarget != nullptr) {
            Target = static_cast<TShip*>(GetHull()->InterceptorTarget);
        } else {
            Target = SelectInterceptorTarget();
        }
        if (Target == nullptr) {
            return;
        }
        GetHull()->InterceptorTarget = nullptr;
        if (GetHull()->Energy < GetInterceptorEnergyCost()) {
            return;
        }
        GetHull()->Energy -= GetInterceptorEnergyCost();
        Target->InterceptorSourceShip = this;
        Target->InterceptorPassesRemaining = GetInterceptorPassCount();
        if (Target->InterceptorGraphic == nullptr) {
            {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), u"Ruins.FighterSwarm"_wref.get(), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> interceptorGraphic = pas::Var<SE_Space::TObjectSE*>(&Target->InterceptorGraphic);
                SE_Space::RetainSpaceObject(interceptorGraphic, createSpaceObjectByName);
            }
            Target->InterceptorGraphic->SetPosition(Target->Position);
            Target->InterceptorGraphic->SetAngle(aMyFunction::HeadingDegreesToByte(Target->MovementDirection));
            Target->InterceptorGraphic->SetAlpha(0);
        }
    }

    // Clears source/pass state and releases the interceptor graphic.
    void TShip::ClearIncomingInterceptors() {
        InterceptorSourceShip = nullptr;
        InterceptorPassesRemaining = 0;
        if (InterceptorGraphic != nullptr) {
            InterceptorGraphic->DetachFromSpace();
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&InterceptorGraphic));
        }
    }

    // Counts ships referring to Self as interceptor source across all galaxy systems.
    std::int32_t TShip::CountActiveInterceptorTargets() {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                if (pas::list_at<TShip>(Star->Ships, J)->InterceptorSourceShip == this) {
                    ++Count;
                }
            }
        }
        return Count;
    }

    std::int32_t TShip::GetHullEnergyRegeneration() {
        pas::Extended cpp_left = aMyFunction::RemapClamped(GetEffectiveSkillLevel(psTechnical, false) & 0x0000007f, 0.0, 6.0, 1.0, 2.0);
        return System::Round(cpp_left * (aMyFunction::RemapClamped(GetHull()->HullPoints, 0.0, GetHull()->Weight, 0.0, 1.0) * 1.0E+1L));
    }

    std::int32_t TShip::GetInterceptorDamage() {
        return System::Round(aMyFunction::RemapClamped(GetEffectiveSkillLevel(psTechnical, false) & 0x0000007f, 0.0, 6.0, 1.0, 2.0) * 25.0L);
    }

    std::int32_t TShip::GetInterceptorEnergyCost() {
        return 5 * GetInterceptorPassCount() + 5;
    }

    // Hull override or five when zero.
    std::uint8_t TShip::GetInterceptorPassCount() {
        if (GetHull()->InterceptorPassCountOverride == 0) {
            return 5;
        }
        return GetHull()->InterceptorPassCountOverride;
    }

    std::uint8_t TShip::HasScriptControl() {
        return ScriptShip != nullptr && pas::checked_cast<aScript::TScriptShip*>(ScriptShip)->State->StateKind != aScript::sskNormalAI || AbsoluteScriptOrder > 0;
    }

    std::uint8_t TShip::HasNoUsableWeapons() {
        std::int32_t I{};
        std::uint8_t Result = true;
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    if (Weapons[I]->EquippedFlag != 0 && (static_cast<std::uint8_t>(pas::in_range(Weapons[I]->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) || Weapons[I]->Ammo > 0) && aShip::TShip_IsEquipmentUsable(this, Weapons[I])) {
                        Result = false;
                        break;
                    }
                }
            }
        }
        return Result;
    }

    std::int32_t TShip::GetOwnStatBonus(std::uint8_t BonusKind) {
        std::int32_t I{};
        PShipStatBonusEntry Bonus{};
        std::int32_t Result = 0;
        if (StatBonuses == nullptr || pas::list_count(StatBonuses) == 0) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StatBonuses) - 1); cpp_range.next(I); ) {
            Bonus = pas::list_at<TShipStatBonusEntry>(StatBonuses, I);
            if (Bonus->BonusKind == BonusKind) {
                Result = Bonus->BonusValue;
                break;
            }
        }
        return Result;
    }

    // Zero removes and frees the matching bonus entry; nonzero inserts or replaces it.
    void TShip::SetStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        std::int32_t I{};
        PShipStatBonusEntry Bonus = nullptr;
        if (StatBonuses == nullptr && Value == 0) {
            return;
        }
        if (StatBonuses == nullptr) {
            StatBonuses = pas::make_object<pas::List>();
        }
        std::uint8_t Found = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StatBonuses) - 1); cpp_range.next(I); ) {
            Bonus = pas::list_at<TShipStatBonusEntry>(StatBonuses, I);
            if (Bonus->BonusKind == static_cast<std::uint8_t>(BonusKind)) {
                Found = true;
                break;
            }
        }
        if (static_cast<std::uint8_t>(Found ^ 1) && Value == 0) {
            return;
        }
        if (Found) {
            if (Value == 0) {
                pas::dispose(Bonus);
                pas::list_delete(StatBonuses, I);
            } else {
                Bonus->BonusValue = Value;
            }
        } else {
            pas::new_value(Bonus);
            Bonus->BonusKind = static_cast<std::uint8_t>(BonusKind);
            Bonus->BonusValue = Value;
            pas::list_add(StatBonuses, static_cast<void*>(Bonus));
        }
    }

    // Returns the list index, or -1.
    std::int32_t TShip::FindCombatStatusEffect(TCombatStatusEffectType EffectType) {
        std::int32_t I{};
        std::int32_t Result = -1;
        if (CombatStatusEffects == nullptr) {
            return Result;
        }
        if (pas::list_count(CombatStatusEffects) == 0) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CombatStatusEffects) - 1); cpp_range.next(I); ) {
            if (pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->EffectType == EffectType) {
                return I;
            }
        }
        return Result;
    }

    // Scales by hull and existing strength. Always replaces the source ID, including clearing it for nil Source; no sign validation.
    void TShip::AddCombatStatusStrength(TCombatStatusEffectType EffectType, float Strength, TShip* Source) {
        PCombatStatusEffect Entry{};
        if (CombatStatusEffects == nullptr) {
            CombatStatusEffects = pas::make_object<pas::List>();
        }
        std::int32_t I = FindCombatStatusEffect(EffectType);
        if (I >= 0) {
            Entry = pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I);
            Entry->Strength = Entry->Strength + pas::real_divide(pas::real_divide(Strength, 1.0L + (pas::real_divide(GetHull()->Weight, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[1]) - 1.0L) * (aConst::CombatStatusHullFactors[EffectType] * 0.5L)), 1.0L + static_cast<long double>(aConst::CombatStatusAccumulationFactors[EffectType]) * Entry->Strength);
            if (Source != nullptr) {
                Entry->SourceShipId = Source->Id;
            } else {
                Entry->SourceShipId = 0;
            }
        } else {
            pas::new_value(Entry);
            Entry->EffectType = EffectType;
            Entry->Strength = pas::real_divide(Strength, 1.0L + (pas::real_divide(GetHull()->Weight, static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[1]) - 1.0L) * (aConst::CombatStatusHullFactors[EffectType] * 0.5L));
            if (Source != nullptr) {
                Entry->SourceShipId = Source->Id;
            } else {
                Entry->SourceShipId = 0;
            }
            pas::list_add(CombatStatusEffects, static_cast<void*>(Entry));
        }
    }

    // Removes only entries reduced below zero; exactly zero remains. Negative Amount increases strength.
    void TShip::ReduceCombatStatusStrength(TCombatStatusEffectType EffectType, float Amount) {
        if (CombatStatusEffects == nullptr) {
            return;
        }
        if (pas::list_count(CombatStatusEffects) == 0) {
            return;
        }
        std::int32_t I = FindCombatStatusEffect(EffectType);
        if (I < 0) {
            return;
        }
        PCombatStatusEffect Entry = pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I);
        Entry->Strength = static_cast<long double>(Entry->Strength) - Amount;
        if (Entry->Strength < 0.0L) {
            pas::list_delete(CombatStatusEffects, I);
            pas::dispose(Entry);
        }
    }

    // Frees entries but keeps the list.
    void TShip::ClearCombatStatusEffects() {
        PCombatStatusEffect Entry{};
        if (CombatStatusEffects == nullptr) {
            return;
        }
        while (pas::list_count(CombatStatusEffects) > 0) {
            Entry = pas::list_at<TCombatStatusEffect>(CombatStatusEffects, 0);
            pas::list_delete(CombatStatusEffects, 0);
            pas::dispose(Entry);
        }
    }

    // Daily decay; exactly zero remains until a subsequent reduction.
    void TShip::DecayCombatStatusEffects() {
        PCombatStatusEffect Entry{};
        float Amount{};
        if (CombatStatusEffects == nullptr) {
            return;
        }
        std::int32_t I = pas::list_count(CombatStatusEffects) - 1;
        while (I >= 0) {
            Entry = pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I);
            switch (Entry->EffectType) {
                case cseShock: Amount = aShip::TShip_GetShockStatusDecay(this, Entry->Strength); break;
                case cseAcid: Amount = GetAcidStatusDecay(Entry->Strength); break;
                case cseMagnetic: Amount = GetMagneticStatusDecay(Entry->Strength); break;
                case cseWeaponBlock: Amount = TShip::GetWeaponBlockStatusDecay(Entry->Strength); break;
                case cseDroidBlock: Amount = TShip::GetDroidBlockStatusDecay(Entry->Strength); break;
                case cseBWBuff: Amount = TShip::GetBWBuffStatusDecay(Entry->Strength); break;
                case cseBWRepairDebuff: Amount = TShip::GetBWRepairDebuffStatusDecay(Entry->Strength); break;
                default: Amount = 0.0f; break;
            }
            Entry->Strength = static_cast<long double>(Entry->Strength) - Amount;
            if (Entry->Strength < 0.0L) {
                pas::list_delete(CombatStatusEffects, I);
                pas::dispose(Entry);
            }
            --I;
        }
    }

    float TShip_GetShockStatusDecay(TShip* Self, float Strength) {
        std::int32_t I{};
        float Result = 0.1L * Strength + 5.0L;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->CountActiveArtefacts(aConst::t_ArtefactHull)); cpp_range.next(I); ) {
            Result = Result * (aConst::HullArtefactStatusDecayFactor + static_cast<long double>(static_cast<std::int8_t>(Self->CanBoostArtefact(aConst::t_ArtefactHull, nullptr, false))) * aConst::HullArtefactBoostStatusDecay);
        }
        if (Self->TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(Self)->KlingType == 0) {
            return Result * 2.0L;
        }
        return Result;
    }

    float TShip::GetAcidStatusDecay(float UnusedStrength) {
        std::int32_t I{};
        float Result = 0.2f;
        if (aShip::TShip_IsEquipmentUsable(this, GetRepairRobot())) {
            Result = 0.2L + Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, CountActiveArtefacts(aConst::t_ArtefactDroid)); cpp_range.next(I); ) {
            Result = Result * (aConst::DroidArtefactStatusDecayFactor + static_cast<long double>(static_cast<std::int8_t>(CanBoostArtefact(aConst::t_ArtefactDroid, nullptr, false))) * aConst::DroidArtefactBoostStatusDecay);
        }
        if (TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(this)->KlingType == 0) {
            return Result * 2.0L;
        }
        return Result;
    }

    float TShip::GetMagneticStatusDecay(float Strength) {
        float Result = 0.1L * Strength + 5.0L;
        if (TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(this)->KlingType == 0) {
            return Result * 2.0L;
        }
        return Result;
    }

    float TShip::GetWeaponBlockStatusDecay(float Strength) {
        return Strength + 1.0L;
    }

    float TShip::GetDroidBlockStatusDecay(float Strength) {
        return Strength + 1.0L;
    }

    float TShip::GetBWBuffStatusDecay(float Strength) {
        return 0.05L * Strength + 0.5L;
    }

    float TShip::GetBWRepairDebuffStatusDecay(float Strength) {
        return 0.01L * Strength + 25.0L;
    }

    // Requires non-nil Source; clears matching IDs without changing effect strengths.
    void TShip::ClearCombatStatusSourceReferences(TShip* Source) {
        std::int32_t I{};
        if (CombatStatusEffects == nullptr) {
            return;
        }
        if (pas::list_count(CombatStatusEffects) == 0) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CombatStatusEffects) - 1); cpp_range.next(I); ) {
            if (pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->SourceShipId == Source->Id) {
                pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->SourceShipId = 0;
            }
        }
    }

    // Zero when absent.
    float TShip::GetCombatStatusStrength(TCombatStatusEffectType EffectType) {
        float Result = 0.0f;
        std::int32_t I = FindCombatStatusEffect(EffectType);
        if (I >= 0) {
            return pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->Strength;
        }
        return Result;
    }

    // Zero when absent or unattributed.
    std::int32_t TShip::GetCombatStatusSourceId(TCombatStatusEffectType EffectType) {
        std::int32_t Result = 0;
        std::int32_t I = FindCombatStatusEffect(EffectType);
        if (I >= 0) {
            return pas::list_at<TCombatStatusEffect>(CombatStatusEffects, I)->SourceShipId;
        }
        return Result;
    }

    // Includes rounded-positive shock, acid, magnetic, BW buff and custom status entries; omits transient blocking effects.
    pas::WideString TShip::GetCombatStatusDescription(std::int32_t& Count, std::uint8_t ShowStrength) {
        pas::WideString Result{};
        std::int32_t Strength{};
        std::int32_t I{};
        PCustomShipInfo Info{};
        // Caller-popped static link; show-strength flag -1, strength -8, count output -12, string-result output +8.
        auto AppendStatusLine = [&](pas::WideString TextKey) -> void {
            if (Result != u"") {
                Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedText(TextKey)});
            } else {
                Result = aConst::LocalizedText(TextKey);
            }
            if (ShowStrength) {
                Result = pas::concat_wide({Result, u" (", pas::wide_int_to_str(Strength), u")"});
            }
            ++Count;
        };
        Count = 0;
        Result = pas::WideString();
        if (CombatStatusEffects != nullptr && pas::list_count(CombatStatusEffects) > 0) {
            Strength = System::Round(GetCombatStatusStrength(cseBWBuff));
            if (Strength >= 1) {
                AppendStatusLine(u"FormInfo.ISEBWBuff"_w);
            }
            Strength = System::Round(GetCombatStatusStrength(cseAcid));
            if (Strength >= 1) {
                AppendStatusLine(u"FormInfo.ISEAcid"_w);
            }
            Strength = System::Round(GetCombatStatusStrength(cseShock));
            if (Strength >= 1) {
                AppendStatusLine(u"FormInfo.ISECharged"_w);
            }
            Strength = System::Round(GetCombatStatusStrength(cseMagnetic));
            if (Strength >= 1) {
                AppendStatusLine(u"FormInfo.ISEMagnetic"_w);
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CustomShipInfos) - 1); cpp_range.next(I); ) {
            Info = pas::list_at<TCustomShipInfo>(CustomShipInfos, I);
            if (Info->StatusEffect && static_cast<std::uint8_t>(Info->DeleteQueued ^ 1)) {
                Strength = pas::load_unaligned<std::int32_t>(pas::byte_offset(&Info->Data, (1 - 1) * sizeof(std::int32_t)));
                if (Strength >= 1) {
                    AppendStatusLine(pas::concat_wide({u"ShipInfo.AddInfo.CustomInfos.", Info->TypeName, u".StatusEffect"}));
                }
            }
        }
        return Result;
    }

    // Native base implementation is a no-op.
    void TShip::UpdateAfterburnerState() {
    }

    void TShip_RefreshCurrentStanding(TShip* Self) {
        std::int32_t StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            Self->CurrentStanding = aGalaxyStruct::ssUnaligned;
        }
    }

    std::uint8_t TShip::HasScriptStateText() {
        return ScriptShip != nullptr && reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText != u"";
    }

    // Requires a nonempty faction not beginning with SubFaction. The native substring result is used as Boolean, so absence also returns true.
    std::uint8_t TShip::HasIndependentScriptFaction() {
        return ScriptShip != nullptr && reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText != u"" && EC_Str::FindTextOffsetW(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText, u"SubFaction"_wref.get(), 0) != 0;
    }

    // Requires a nonempty faction other than the exact SubFactionFixedStanding marker.
    std::uint8_t TShip::HasNamedScriptFaction() {
        return ScriptShip != nullptr && reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText != u"" && reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText != u"SubFactionFixedStanding";
    }

    // 0 normal, 1 independent faction, 2 fixed standing. The SubFaction substring test accepts absence as mode one.
    std::int32_t TShip::GetScriptStandingOverrideMode() {
        std::int32_t Result = aGalaxyStruct::ssmNormal;
        if (ScriptShip == nullptr) {
            return Result;
        } else if (reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText == u"") {
            return Result;
        } else if (EC_Str::FindTextOffsetW(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText, u"SubFaction"_wref.get(), 0) != 0) {
            return aGalaxyStruct::ssmCustomFaction;
        } else if (EC_Str::FindTextOffsetW(reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText, u"FixedStanding"_wref.get(), 0) >= 0) {
            return aGalaxyStruct::ssmFixed;
        } else {
            return Result;
        }
    }

    // Returns Param after script handlers modify it; object slots may carry event-specific integer values.
    std::int32_t TShip::ScriptItemsAct(std::uint8_t ActionType, pas::Object* Object1, pas::Object* Object2, std::int32_t Param) {
        std::int32_t Result{};
        std::int32_t I{};
        std::int32_t NewIndex{};
        aItem::TItem* Item{};
        PCustomShipInfo Info{};
        aScript::TScriptShip* Binding{};
        aScript::TScript* SavedScript{};
        std::int32_t Stage{};
        Stage = 0;
        if (ActionType == aConst::satOnLeavingForm && Globals::TalkScreen->ParentLoop == nullptr) {
            ThreadCalc::WaitForTurnCalculation();
        }
        Info = nullptr;
        try {
            SavedScript = aScript::CurrentScript;
            aScript::CurrentScript = nullptr;
            if (aPlayer::GetPlayer() == this) {
                Stage = 1;
                I = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
                while (I >= 0) {
                    Binding = pas::list_at<aScript::TScriptShip>(aPlayer::GetPlayer()->ScriptShipBindings, I);
                    Param = Binding->RunActionCode(ActionType, this, Object1, Object2, Param);
                    if (IsHullDestroyed() && ActionType != aConst::satOnDeath) {
                        aScript::CurrentScript = SavedScript;
                        return Param;
                    }
                    if (pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) <= I || pas::list_get(aPlayer::GetPlayer()->ScriptShipBindings, I) != Binding) {
                        NewIndex = pas::list_indexof(aPlayer::GetPlayer()->ScriptShipBindings, reinterpret_cast<void*>(Binding));
                        if (NewIndex < 0) {
                            I = std::min<std::int32_t>(I - 1, pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1);
                            continue;
                        }
                        I = NewIndex;
                    }
                    --I;
                }
            } else if (ScriptShip != nullptr) {
                Stage = 2;
                Param = reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->RunActionCode(ActionType, this, Object1, Object2, Param);
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CustomShipInfos) - 1); cpp_range.next(I); ) {
                Info = pas::list_at<TCustomShipInfo>(CustomShipInfos, I);
                Stage = 3;
                if (!Info->DeleteQueued) {
                    Stage = 4;
                    Param = aScript::RunCustomShipInfoActionCode(Info, ActionType, this, Object1, Object2, Param);
                    Stage = 5;
                    if (IsHullDestroyed() && ActionType != aConst::satOnDeath) {
                        aScript::CurrentScript = SavedScript;
                        return Param;
                    }
                    Stage = 6;
                }
            }
            Stage = 11;
            if (pas::list_count(Globals::ScriptActionTypeStack) <= 0) {
                const std::int32_t cpp_first = pas::list_count(CustomShipInfos) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Info = pas::list_at<TCustomShipInfo>(CustomShipInfos, I);
                        if (Info->DeleteQueued) {
                            pas::list_delete(CustomShipInfos, I);
                            pas::dispose(Info);
                        }
                    }
                }
            }
            Stage = 12;
            I = pas::list_count(Inventory) - 1;
            while (I >= 0) {
                Stage = 13;
                Item = pas::list_at<aItem::TItem>(Inventory, I);
                if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                    Param = aScript::RunItemConfigActionCode(Item, ActionType, this, Object1, Object2, Param);
                    if (IsHullDestroyed() && ActionType != aConst::satOnDeath) {
                        aScript::CurrentScript = SavedScript;
                        return Param;
                    }
                    if (pas::list_count(Inventory) <= I || pas::list_get(Inventory, I) != Item) {
                        NewIndex = pas::list_indexof(Inventory, reinterpret_cast<void*>(Item));
                        if (NewIndex < 0) {
                            I = std::min<std::int32_t>(I - 1, pas::list_count(Inventory) - 1);
                            continue;
                        }
                        I = NewIndex;
                    }
                }
                Stage = 14;
                if (Item->ScriptItem != nullptr) {
                    if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                        if (pas::in_set<1, 2, 10, 10>(ActionType) && Object2 != Item) {
                            --I;
                            continue;
                        }
                        if (pas::in_set<11, 11, 23, 23>(ActionType) && Object2 != nullptr && reinterpret_cast<aMissile::TMissile*>(Object2)->WeaponId != Item->Id) {
                            --I;
                            continue;
                        }
                    }
                    Param = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(ActionType, this, Object1, Object2, Param);
                    if (IsHullDestroyed() && ActionType != aConst::satOnDeath) {
                        aScript::CurrentScript = SavedScript;
                        return Param;
                    }
                    if (pas::list_count(Inventory) <= I || pas::list_get(Inventory, I) != Item) {
                        NewIndex = pas::list_indexof(Inventory, reinterpret_cast<void*>(Item));
                        if (NewIndex >= 0) {
                            I = NewIndex - 1;
                        } else {
                            I = std::min<std::int32_t>(I - 1, pas::list_count(Inventory) - 1);
                        }
                        continue;
                    }
                }
                --I;
            }
            Stage = 15;
            I = pas::list_count(Artefacts) - 1;
            while (I >= 0) {
                Stage = 16;
                Item = pas::list_at<aItem::TItem>(Artefacts, I);
                if (reinterpret_cast<aItem::TEquipment*>(Item)->EquippedFlag != 0) {
                    Param = aScript::RunItemConfigActionCode(Item, ActionType, this, Object1, Object2, Param);
                    if (IsHullDestroyed() && ActionType != aConst::satOnDeath) {
                        aScript::CurrentScript = SavedScript;
                        return Param;
                    }
                    if (pas::list_count(Artefacts) <= I || pas::list_get(Artefacts, I) != Item) {
                        NewIndex = pas::list_indexof(Artefacts, reinterpret_cast<void*>(Item));
                        if (NewIndex < 0) {
                            I = std::min<std::int32_t>(I - 1, pas::list_count(Artefacts) - 1);
                            continue;
                        }
                        I = NewIndex;
                    }
                }
                Stage = 17;
                if (Item->ScriptItem != nullptr) {
                    Param = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(ActionType, this, Object1, Object2, Param);
                    if (IsHullDestroyed() && ActionType != aConst::satOnDeath) {
                        aScript::CurrentScript = SavedScript;
                        return Param;
                    }
                    if (pas::list_count(Artefacts) <= I || pas::list_get(Artefacts, I) != Item) {
                        NewIndex = pas::list_indexof(Artefacts, reinterpret_cast<void*>(Item));
                        if (NewIndex >= 0) {
                            I = NewIndex - 1;
                        } else {
                            I = std::min<std::int32_t>(I - 1, pas::list_count(Artefacts) - 1);
                        }
                        continue;
                    }
                }
                --I;
            }
            aScript::CurrentScript = SavedScript;
            Result = Param;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TShip.ScriptItemsAct ship=", GetFullName(u" "_wref.get()), u" acttype=", aConst::ScriptActionTypeNames[ActionType], u" label = ", pas::wide_int_to_str(Stage)})));
                if (Stage == 4) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Info->TypeName));
                }
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({SysUtils::IntToStr(pas::list_count(Globals::ScriptActionTypeStack)), ",", SysUtils::IntToStr(pas::list_count(Globals::ScriptActionObject1Stack)), ",", SysUtils::IntToStr(pas::list_count(Globals::ScriptActionObject2Stack)), ",", SysUtils::IntToStr(pas::list_count(Globals::ScriptActionParamStack)), ",", SysUtils::IntToStr(pas::list_count(Globals::ScriptActionShipStack)), ",", SysUtils::IntToStr(pas::list_count(Globals::ScriptItemInfoContextStack)), ",", SysUtils::IntToStr(pas::list_count(Globals::ScriptItemContextStack))}));
                if (ScriptShip != nullptr) {
                    aScript::TScriptShip* cpp_with = pas::checked_cast<aScript::TScriptShip*>(ScriptShip);
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"script - ", cpp_with->Script->ScriptFileName})));
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"state #", SysUtils::IntToStr(pas::list_indexof(cpp_with->Script->States, reinterpret_cast<void*>(cpp_with->State)))}));
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"(", cpp_with->State->Name, u")"})));
                }
                throw;
            } else {
                throw;
            }
        }
        return Result;
    }

    // Base implementation always returns false.
    std::uint8_t TShip_CanDock(TShip* Self, TShip* Ship) {
        return false;
    }

    // Base implementation clears Response and returns false.
    std::uint8_t TShip::CheckDockingPermission(TShip* Ship, pas::WideString& Response) {
        Response = pas::WideString();
        return false;
    }

    std::uint8_t TShip_RelationToShip(TShip* Self, TShip* Ship) {
        if (Ship == Self) {
            return 100;
        }
        if (pas::class_cast_if<aTranclucator::TTranclucator*>(Self) != nullptr && static_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip != nullptr) {
            return aShip::TShip_RelationToShip(pas::checked_cast<aTranclucator::TTranclucator*>(Self)->OwnerShip, Ship);
        }
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            if (aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Self) {
                return 100;
            }
            return 0;
        }
        std::uint8_t Result = 0;
        if (Ship == Self->EnemyShip || Ship->EnemyShip == Self || (Self->CurrentStanding == aGalaxyStruct::ssDominator) != (Ship->CurrentStanding == aGalaxyStruct::ssDominator) || (Self->CurrentStanding == aGalaxyStruct::ssCustom) != (Ship->CurrentStanding == aGalaxyStruct::ssCustom)) {
            return Result;
        }
        if (Self->CurrentStanding == aGalaxyStruct::ssCustom) {
            if (Self->ScriptShip != nullptr && Ship->ScriptShip != nullptr && reinterpret_cast<aScript::TScriptShip*>(Self->ScriptShip)->StateText == reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText) {
                return 100;
            }
            return Result;
        }
        if (Ship != Self->TruceShip && Ship->TruceShip != Self) {
            if (Self->CurrentStanding == aGalaxyStruct::ssCoalitionMilitary && pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary) || Self->CurrentStanding == aGalaxyStruct::ssPirateMilitary && pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive)) {
                return Result;
            }
        }
        if (Self->CurrentStanding != aGalaxyStruct::ssDominator && Ship->TypeId == aGalaxyStruct::stKling && Ship->CurrentStanding == aGalaxyStruct::ssNeutral) {
            return 100;
        } else if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
            return Self->RelationToRanger(Ship);
        } else if (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip != nullptr) {
            return aShip::TShip_RelationToShip(Self, pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip);
        } else {
            return Self->RelationToNonRanger(Ship);
        }
    }

    void TShip::p_destroy() {
        aShip::TShip_Destroy(this);
    }

    void TShip::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aShip::TShip_ResolveLoadedReferences(this, Galaxy);
    }

    void TShip::virtual_TShip_NextDay() {
        aShip::TShip_NextDay(this);
    }

    void TShip::virtual_TShip_NextDayLogic() {
        aShip::TShip_NextDayLogic(this);
    }

    void TShip::virtual_TShip_RefreshCurrentStanding() {
        aShip::TShip_RefreshCurrentStanding(this);
    }

    std::uint8_t TShip::virtual_TShip_CanDock(aShip::TShip* Ship) {
        return aShip::TShip_CanDock(this, Ship);
    }

} // namespace aShip
