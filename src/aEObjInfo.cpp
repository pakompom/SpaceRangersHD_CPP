#include "layout/aEObjInfo.hpp"
#include "types/EC_Buf.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aMissile.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/aConst.hpp"
#include "units/aEObjInfo.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aShip.hpp"

namespace aEObjInfo {
    void TEObjInfo_Destroy(TEObjInfo* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TEObjInfo::Clear() {
        StarName = pas::WideString();
        StarRadius = 0;
        Planets = nullptr;
        Ships = nullptr;
        Items = nullptr;
        Asteroids = nullptr;
        Missiles = nullptr;
        CustomSystemInfos = nullptr;
    }

    void TEObjInfo::LoadFromStar(aGalaxy::TStar* Star) {
        static const pas::Set<0, 255> NoDamageFlags = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::dkEnergy, aGalaxyStruct::dkDroidBlock}}) - pas::constant_set<pas::Set<0, 255>>({
                {aGalaxyStruct::dkEnergy, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> WearableItemTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) -
            pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        std::int32_t Index{};
        std::int32_t StatusCount{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        aAsteroid::TAsteroid* Asteroid{};
        aMissile::TMissile* Missile{};
        aGalaxy::TCustomSystemInfo* CustomInfo{};
        aGalaxyStruct::TRelationLevel Relation{};
        std::int32_t Stage = 0;
        try {
            StarName = Star->Name;
            StarRadius = Star->Radius;
            Stage = 1;
            Planets.set_length(pas::list_count(Star->Planets));
            {
                const std::int32_t cpp_last = pas::list_count(Star->Planets) - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, Index);
                        Planets[Index].Id = Planet->Id;
                        Planets[Index].Name = Planet->Name;
                        Planets[Index].OwnerId = Planet->OwnerId;
                        Planets[Index].RaceId = Planet->RaceId;
                        Planets[Index].Population = Planet->Population;
                        Planets[Index].Economy = Planet->Economy;
                        Planets[Index].Government = Planet->Government;
                        Planets[Index].Relation = Planet->GetRelationLevelToShip(aPlayer::GetPlayer());
                        Planets[Index].UnexploredWater = Planet->WaterTiles - Planet->WaterExplored;
                        Planets[Index].UnexploredLand = Planet->LandTiles - Planet->LandExplored;
                        Planets[Index].UnexploredHills = Planet->HillTiles - Planet->HillExplored;
                        Planets[Index].TreasureHint = Planet->BuildNonCivilTreasureHintText();
                        Planets[Index].Faction = Planet->GetFactionResourceName();
                    }
                }
            }
            Stage = 2;
            Ships.set_length(pas::list_count(Star->Ships));
            {
                const std::int32_t cpp_last_2 = pas::list_count(Star->Ships) - 1;
                if (0 <= cpp_last_2) {
                    for (Index = 0; Index <= cpp_last_2; ++Index) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, Index);
                        Ships[Index].Id = Ship->Id;
                        Ships[Index].Name = Ship->Name;
                        Ships[Index].OutsideNormalSpace = !Ship->InNormalSpace();
                        Stage = 20;
                        if (aPlayer::GetPlayer() != Ship) {
                            Stage = 21;
                            Ships[Index].FullName = ([&] {
                                    pas::WideString fullName = Ship->GetFullName(u" "_wref.get());
                                    pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                                    return aMyFunction::WrapTextInColor(pas::view(std::move(fullName)),
                                        pas::view(std::move(infoNameColorTag)));
                                }());
                            if (Ship != nullptr && aPlayer::GetPlayer() == Ship->PartnerShip) {
                                Ships[Index].FullName = pas::concat_wide({Ships[Index].FullName, u"\r\n",
                                        ([&] {
                                            pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(
                                                u"FormInfo.Partner"_wref.get());
                                            pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                                            return aMyFunction::WrapTextInColor(
                                                pas::view(std::move(lookupLocalizedTextByKey)),
                                                pas::view(std::move(textHighlightColorTag)));
                                        }())});
                            }
                            if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship); kling != nullptr &&
                                kling->ActiveProgramAppliedTurn > 0 && pas::in_range(kling->ActiveProgramId,
                                    static_cast<std::int32_t>(aGalaxyStruct::prgShipwreck),
                                    static_cast<std::int32_t>(aGalaxyStruct::prgDisconnection))) {
                                Ships[Index].FullName = pas::concat_wide({Ships[Index].FullName, u"\r\n",
                                        ([&] {
                                            pas::WideString localizedText = aConst::LocalizedText(pas::concat_wide({
                                                u"Programms.",
                                                aConst::ProgramNames[
                                                    pas::checked_cast<aKling::TKling*>(Ship)->ActiveProgramId],
                                                u".AddToShipInfo"}));
                                            pas::WideString redColorTag = aMyFunction::RedColorTag;
                                            return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText)),
                                                pas::view(std::move(redColorTag)));
                                        }())});
                            }
                        } else {
                            Stage = 22;
                            Ships[Index].FullName = ([&] {
                                    pas::WideString fullName_2 = Ship->GetFullName(u" "_wref.get());
                                    pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                                    return aMyFunction::WrapTextInColor(pas::view(std::move(fullName_2)),
                                        pas::view(std::move(infoNameColorTag_2)));
                                }());
                        }
                        Stage = 23;
                        Ships[Index].OwnerId = Ship->OwnerId;
                        if (Ships[Index].OwnerId == aGalaxyStruct::oiDominator) {
                            Ships[Index].DominatorSeries = pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries;
                        }
                        if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Ship)) {
                            Ships[Index].TypeName = ranger->GetCharacterName();
                        } else {
                            Ships[Index].TypeName = Ship->GetLocalizedTypeName();
                        }
                        Ships[Index].Speed = Ship->CalculateSpeed();
                        Ships[Index].HullCapacity = Ship->GetHull()->Weight;
                        Ships[Index].HullPoints = Ship->GetHull()->HullPoints;
                        Ships[Index].HullFragility =
                            Ship->GetHull()->GetFragilityFactor(
                                static_cast<aGalaxyStruct::TDamageFlagSet>(NoDamageFlags));
                        if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || aPlayer::GetPlayer() == Ship) {
                            Ships[Index].ScannerResolved = true;
                        } else {
                            Ships[Index].ScannerResolved = false;
                        }
                        Stage = 24;
                        Ships[Index].RepairPoints = -1;
                        Ships[Index].DamageText = aMyFunction::WrapTextInColor(u"???"sv, u""sv);
                        Ships[Index].DefenseText = pas::concat_wide({
                                pas::wide_int_to_str(static_cast<std::int32_t>(Ship->GetDefensePercent())), u"%"});
                        if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || aPlayer::GetPlayer() == Ship ||
                            aPlayer::GetPlayer() == Ship->PartnerShip || Ship->TypeId ==
                                aGalaxyStruct::stTranclucator) {
                            Stage = 25;
                            Ships[Index].DefenseText = pas::concat_wide({Ships[Index].DefenseText, u" + ",
                                    aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Ship->GetArmor())),
                                        u""sv)});
                            if (aPlayer::GetPlayer()->HasScannerArtefact(Ship)) {
                                if (Ship->GetRepairRobot() != nullptr) {
                                    Ships[Index].RepairPoints = aShip::TShip_CalculateRepairPoints(Ship,
                                            Ship->GetRepairRobot());
                                } else {
                                    Ships[Index].RepairPoints = 0;
                                }
                                Ships[Index].DamageText = aMyFunction::WrapTextInColor(
                                        pas::view(aShip::TShip_GetWeaponDamageSummary(Ship)), u""sv);
                                Ships[Index].DefenseText = pas::concat_wide({Ship->GetManeuverabilitySummary(),
                                        Ships[Index].DefenseText});
                            }
                        }
                        if (!aPlayer::GetPlayer()->HasScannerArtefact(Ship)) {
                            Ships[Index].DamageText = pas::WideString();
                        }
                        Stage = 26;
                        Relation = aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer());
                        Ships[Index].Relation = Relation;
                        if (Relation != aGalaxyStruct::rlHostile) {
                            if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) {
                                if (Ship->RangerRelations != nullptr) {
                                    if (pas::list_count(Ship->RangerRelations) > 0) {
                                        Relation =
                                            aConst::RelationValueToLevel(
                                                static_cast<std::uint8_t>(
                                                    reinterpret_cast<std::uintptr_t>(pas::list_get(
                                                    Ship->RangerRelations,
                                                    pas::list_indexof(aGalaxy::Galaxy->Rangers,
                                                    reinterpret_cast<void*>(aPlayer::GetPlayer()))))));
                                        if (Relation != aGalaxyStruct::rlHostile) {
                                            Ships[Index].Relation = Relation;
                                        }
                                    }
                                }
                            }
                        }
                        Stage = 27;
                        if (aPlayer::GetPlayer() != Ship && !(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) &&
                            aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 &&
                            aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship)) {
                            Ships[Index].WinChance = aShip::TShip_GetWinChancePercent(aPlayer::GetPlayer(), Ship);
                        } else {
                            Ships[Index].WinChance = -1;
                        }
                        Stage = 28;
                        Ships[Index].PortraitImage = Ship->GetShipPortraitImagePath();
                        Stage = 29;
                        if (!aPlayer::GetPlayer()->HasScannerArtefact(Ship)) {
                            Ships[Index].CombatStatusText = Ship->GetCombatStatusDescription(StatusCount, false);
                        } else {
                            Ships[Index].CombatStatusText = Ship->GetCombatStatusDescription(StatusCount, true);
                        }
                        Ships[Index].CombatStatusCount = StatusCount;
                        Ships[Index].Faction = Ship->GetFactionNameKey();
                    }
                }
            }
            Stage = 3;
            Items.set_length(pas::list_count(Star->Items));
            {
                const std::int32_t cpp_last_3 = pas::list_count(Star->Items) - 1;
                if (0 <= cpp_last_3) {
                    for (Index = 0; Index <= cpp_last_3; ++Index) {
                        Item = pas::list_at<aItem::TItem>(Star->Items, Index);
                        Stage = 30;
                        Items[Index].Id = Item->Id;
                        Items[Index].ItemType = Item->ItemType;
                        Items[Index].Weight = Item->Weight;
                        Items[Index].Cost = Item->Cost;
                        if (pas::contains(WearableItemTypes, static_cast<std::uint8_t>(Item->ItemType))) {
                            Items[Index].ConditionPercent =
                                pas::checked_cast<aItem::TEquipment*>(Item)->ConditionPercent;
                            Items[Index].Fragility =
                                pas::checked_cast<aItem::TEquipment*>(
                                    Item)->GetFragilityFactor(
                                    static_cast<aGalaxyStruct::TDamageFlagSet>(NoDamageFlags));
                        } else {
                            Items[Index].ConditionPercent = 1.0E+2;
                            Items[Index].Fragility = 1.0;
                        }
                        if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                            Stage = 31;
                            Items[Index].ImagePath = pas::concat_wide({u"GI,",
                                    aItem::GetItemTypeBitmapPath(Item->ItemType)});
                            Items[Index].Name = aMyFunction::WrapTextInColor(
                                    pas::view(
                                        aConst::GoodsMarket[static_cast<std::uint8_t>(Item->ItemType)].DisplayName),
                                    pas::view(aMyFunction::InfoNameColorTag));
                            Items[Index].InfoText = aConst::LocalizedText(pas::concat_wide({u"Items.Goods.Text.",
                                    pas::wide_int_to_str(static_cast<std::uint8_t>(Item->ItemType) + 1)}));
                            Items[Index].OwnerId = aGalaxyStruct::oiUninhabited;
                        } else {
                            Stage = 32;
                            Items[Index].ImagePath = pas::concat_wide({u"GI,", Item->GetBitmapResourceName(), u"s"});
                            Items[Index].Name = ([&] {
                                    pas::WideString displayName = Item->GetDisplayName();
                                    pas::WideString infoNameColorTag_3 = aMyFunction::InfoNameColorTag;
                                    return aMyFunction::WrapTextInColor(pas::view(std::move(displayName)),
                                        pas::view(std::move(infoNameColorTag_3)));
                                }());
                            Items[Index].InfoText = Item->virtual_TItem_GetInfoText(aMyFunction::TextHighlightColorTag,
                                    nullptr);
                            Items[Index].OwnerId = Item->OwnerId;
                        }
                        if (aItem::TEquipment* equipment = pas::class_cast_if<aItem::TEquipment*>(Item)) {
                            Stage = 33;
                            Items[Index].DominatorSeries = equipment->DominatorSeries;
                        }
                        Items[Index].Faction = aItem::TItem_GetOwnerConfigName(Item);
                    }
                }
            }
            Stage = 4;
            Asteroids.set_length(pas::list_count(Star->Asteroids));
            {
                const std::int32_t cpp_last_4 = pas::list_count(Star->Asteroids) - 1;
                if (0 <= cpp_last_4) {
                    for (Index = 0; Index <= cpp_last_4; ++Index) {
                        Asteroid = pas::list_at<aAsteroid::TAsteroid>(Star->Asteroids, Index);
                        Asteroids[Index].Id = Asteroid->Id;
                        Asteroids[Index].Name = Asteroid->GetDisplayName();
                        Asteroids[Index].InfoText = Asteroid->GetInfoText();
                    }
                }
            }
            Stage = 5;
            Missiles.set_length(pas::list_count(Star->Missiles));
            {
                const std::int32_t cpp_last_5 = pas::list_count(Star->Missiles) - 1;
                if (0 <= cpp_last_5) {
                    for (Index = 0; Index <= cpp_last_5; ++Index) {
                        Missile = pas::list_at<aMissile::TMissile>(Star->Missiles, Index);
                        Missiles[Index].Id = Missile->Id;
                        Missiles[Index].Name = Missile->GetDisplayName();
                        Missiles[Index].InfoText = Missile->GetInfoText();
                    }
                }
            }
            Stage = 5;
            CustomSystemInfos.set_length(pas::list_count(Star->CustomSystemInfos));
            {
                const std::int32_t cpp_last_6 = pas::list_count(Star->CustomSystemInfos) - 1;
                if (0 <= cpp_last_6) {
                    for (Index = 0; Index <= cpp_last_6; ++Index) {
                        CustomInfo = pas::list_at<aGalaxy::TCustomSystemInfo>(Star->CustomSystemInfos, Index);
                        CustomSystemInfos[Index].Name = CustomInfo->Name;
                        CustomSystemInfos[Index].ImagePath = CustomInfo->Icon;
                        CustomSystemInfos[Index].Text = CustomInfo->Info;
                        CustomSystemInfos[Index].Distance = CustomInfo->Distance;
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({
                    "Error in procedure TEObjInfo.LoadFromStar, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    // Borrowed pointer into the snapshot array.
    PEPlanetInfo TEObjInfo::FindPlanet(std::uint32_t ObjectId) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Planets.length() - 1); cpp_range.next(Index); ) {
            if (Planets[Index].Id == ObjectId) {
                return &Planets[Index];
            }
        }
        return nullptr;
    }

    // Borrowed pointer into the snapshot array.
    PEShipInfo TEObjInfo::FindShip(std::uint32_t ObjectId) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Ships.length() - 1); cpp_range.next(Index); ) {
            if (Ships[Index].Id == ObjectId) {
                return &Ships[Index];
            }
        }
        return nullptr;
    }

    // Borrowed pointer into the snapshot array.
    PEItemInfo TEObjInfo::FindItem(std::uint32_t ObjectId) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Items.length() - 1); cpp_range.next(Index); ) {
            if (Items[Index].Id == ObjectId) {
                return &Items[Index];
            }
        }
        return nullptr;
    }

    // Borrowed pointer into the snapshot array.
    PEAsteroidInfo TEObjInfo::FindAsteroid(std::uint32_t ObjectId) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Asteroids.length() - 1); cpp_range.next(Index); ) {
            if (Asteroids[Index].Id == ObjectId) {
                return &Asteroids[Index];
            }
        }
        return nullptr;
    }

    // Borrowed pointer into the snapshot array.
    PEMissileInfo TEObjInfo::FindMissile(std::uint32_t ObjectId) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Missiles.length() - 1); cpp_range.next(Index); ) {
            if (Missiles[Index].Id == ObjectId) {
                return &Missiles[Index];
            }
        }
        return nullptr;
    }

    // Appends the current snapshot; doubles are serialized as singles.
    void TEObjInfo::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t Index{};
        Buffer->AddWideStringZ(StarName);
        Buffer->AddIntegerValue(StarRadius);
        Buffer->AddAnsiChar(Planets.length() - 1 + 1);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Planets.length() - 1); cpp_range.next(Index); ) {
            Buffer->AddDWord(Planets[Index].Id);
            Buffer->AddWideStringZ(Planets[Index].Name);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Planets[Index].OwnerId));
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Planets[Index].RaceId));
            Buffer->AddIntegerValue(Planets[Index].Population);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Planets[Index].Economy));
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Planets[Index].Government));
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Planets[Index].Relation));
            Buffer->AddIntegerValue(Planets[Index].UnexploredWater);
            Buffer->AddIntegerValue(Planets[Index].UnexploredLand);
            Buffer->AddIntegerValue(Planets[Index].UnexploredHills);
            Buffer->AddWideStringZ(Planets[Index].TreasureHint);
            Buffer->AddWideStringZ(Planets[Index].Faction);
        }
        Buffer->AddWideChar(Ships.length() - 1 + 1);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Ships.length() - 1); cpp_range_2.next(Index); ) {
            Buffer->AddDWord(Ships[Index].Id);
            Buffer->AddWideStringZ(Ships[Index].Name);
            Buffer->AddWideStringZ(Ships[Index].FullName);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Ships[Index].OwnerId));
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Ships[Index].DominatorSeries));
            Buffer->AddWideStringZ(Ships[Index].TypeName);
            Buffer->AddIntegerValue(Ships[Index].Speed);
            Buffer->AddIntegerValue(Ships[Index].HullCapacity);
            Buffer->AddIntegerValue(Ships[Index].HullPoints);
            Buffer->AddSingle(Ships[Index].HullFragility);
            Buffer->AddWideStringZ(Ships[Index].DefenseText);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Ships[Index].Relation));
            Buffer->AddIntegerValue(Ships[Index].WinChance);
            Buffer->AddWideStringZ(Ships[Index].PortraitImage);
            Buffer->AddIntegerValue(Ships[Index].RepairPoints);
            Buffer->AddWideStringZ(Ships[Index].DamageText);
            Buffer->AddBoolean(Ships[Index].ScannerResolved);
            Buffer->AddIntegerValue(Ships[Index].CombatStatusCount);
            Buffer->AddWideStringZ(Ships[Index].CombatStatusText);
            Buffer->AddWideStringZ(Ships[Index].Faction);
        }
        Buffer->AddWideChar(Items.length() - 1 + 1);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Items.length() - 1); cpp_range_3.next(Index); ) {
            Buffer->AddDWord(Items[Index].Id);
            Buffer->AddWideStringZ(Items[Index].Name);
            Buffer->AddWideStringZ(Items[Index].ImagePath);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Items[Index].ItemType));
            Buffer->AddWideStringZ(Items[Index].InfoText);
            Buffer->AddIntegerValue(Items[Index].Weight);
            Buffer->AddIntegerValue(Items[Index].Cost);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Items[Index].OwnerId));
            Buffer->AddSingle(Items[Index].ConditionPercent);
            Buffer->AddSingle(Items[Index].Fragility);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Items[Index].DominatorSeries));
            Buffer->AddWideStringZ(Items[Index].Faction);
        }
        Buffer->AddWideChar(Asteroids.length() - 1 + 1);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Asteroids.length() - 1); cpp_range_4.next(Index); ) {
            Buffer->AddDWord(Asteroids[Index].Id);
            Buffer->AddWideStringZ(Asteroids[Index].Name);
            Buffer->AddWideStringZ(Asteroids[Index].InfoText);
        }
        Buffer->AddWideChar(Missiles.length() - 1 + 1);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Missiles.length() - 1); cpp_range_5.next(Index); ) {
            Buffer->AddDWord(Missiles[Index].Id);
            Buffer->AddWideStringZ(Missiles[Index].Name);
            Buffer->AddWideStringZ(Missiles[Index].InfoText);
        }
        Buffer->AddWideChar(CustomSystemInfos.length() - 1 + 1);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0,
            CustomSystemInfos.length() - 1); cpp_range_6.next(Index); ) {
            Buffer->AddWideStringZ(CustomSystemInfos[Index].Name);
            Buffer->AddWideStringZ(CustomSystemInfos[Index].ImagePath);
            Buffer->AddWideStringZ(CustomSystemInfos[Index].Text);
            Buffer->AddIntegerValue(CustomSystemInfos[Index].Distance);
        }
    }

    // Legacy versions omit scanner, repair, faction or custom-system fields.
    void TEObjInfo::LoadFromBuffer(EC_Buf::TBufEC* Buffer, std::int32_t Version) {
        std::int32_t Index{};
        Clear();
        StarName = Buffer->ReadWideString();
        StarRadius = Buffer->GetInt32();
        if (Version <= 5) {
            Buffer->GetByte();
        }
        std::int32_t Count = Buffer->GetByte();
        Planets.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Planets[Index].Id = Buffer->GetUInt32();
            Planets[Index].Name = Buffer->ReadWideString();
            Planets[Index].OwnerId = static_cast<aGalaxyStruct::TOwnerId>(Buffer->GetByte());
            Planets[Index].RaceId = static_cast<aGalaxyStruct::TOwnerId>(Buffer->GetByte());
            Planets[Index].Population = Buffer->GetInt32();
            Planets[Index].Economy = static_cast<aGalaxyStruct::TPlanetEconomy>(Buffer->GetByte());
            Planets[Index].Government = static_cast<aGalaxyStruct::TPlanetGovernment>(Buffer->GetByte());
            Planets[Index].Relation = static_cast<aGalaxyStruct::TRelationLevel>(Buffer->GetByte());
            Planets[Index].UnexploredWater = Buffer->GetInt32();
            Planets[Index].UnexploredLand = Buffer->GetInt32();
            Planets[Index].UnexploredHills = Buffer->GetInt32();
            Planets[Index].TreasureHint = Buffer->ReadWideString();
            if (Version >= 6) {
                Planets[Index].Faction = Buffer->ReadWideString();
            } else {
                Planets[Index].Faction = u"None"_w;
            }
        }
        Count = Buffer->GetWord();
        Ships.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Ships[Index].Id = Buffer->GetUInt32();
            Ships[Index].Name = Buffer->ReadWideString();
            Ships[Index].FullName = Buffer->ReadWideString();
            Ships[Index].OwnerId = static_cast<aGalaxyStruct::TOwnerId>(Buffer->GetByte());
            Ships[Index].DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(Buffer->GetByte());
            Ships[Index].TypeName = Buffer->ReadWideString();
            Ships[Index].Speed = Buffer->GetInt32();
            Ships[Index].HullCapacity = Buffer->GetInt32();
            Ships[Index].HullPoints = Buffer->GetInt32();
            Ships[Index].HullFragility = Buffer->GetSingle();
            Ships[Index].DefenseText = Buffer->ReadWideString();
            Ships[Index].Relation = static_cast<aGalaxyStruct::TRelationLevel>(Buffer->GetByte());
            Ships[Index].WinChance = Buffer->GetInt32();
            Ships[Index].PortraitImage = Buffer->ReadWideString();
            if (Version >= 3) {
                Ships[Index].RepairPoints = Buffer->GetInt32();
                Ships[Index].DamageText = Buffer->ReadWideString();
            }
            if (Version >= 4) {
                Ships[Index].ScannerResolved = Buffer->GetBoolean();
            }
            Ships[Index].CombatStatusCount = Buffer->GetInt32();
            Ships[Index].CombatStatusText = Buffer->ReadWideString();
            if (Version >= 6) {
                Ships[Index].Faction = Buffer->ReadWideString();
            } else {
                Ships[Index].Faction = u"None"_w;
            }
        }
        Count = Buffer->GetWord();
        Items.set_length(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
            Items[Index].Id = Buffer->GetUInt32();
            Items[Index].Name = Buffer->ReadWideString();
            Items[Index].ImagePath = Buffer->ReadWideString();
            Items[Index].ItemType = static_cast<aConst::TItemType>(Buffer->GetByte());
            Items[Index].InfoText = Buffer->ReadWideString();
            Items[Index].Weight = Buffer->GetInt32();
            Items[Index].Cost = Buffer->GetInt32();
            Items[Index].OwnerId = static_cast<aGalaxyStruct::TOwnerId>(Buffer->GetByte());
            Items[Index].ConditionPercent = Buffer->GetSingle();
            Items[Index].Fragility = Buffer->GetSingle();
            Items[Index].DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(Buffer->GetByte());
            if (Version >= 6) {
                Items[Index].Faction = Buffer->ReadWideString();
            } else {
                Items[Index].Faction = u"None"_w;
            }
        }
        Count = Buffer->GetWord();
        Asteroids.set_length(Count);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(Index); ) {
            Asteroids[Index].Id = Buffer->GetUInt32();
            Asteroids[Index].Name = Buffer->ReadWideString();
            Asteroids[Index].InfoText = Buffer->ReadWideString();
        }
        Count = Buffer->GetWord();
        Missiles.set_length(Count);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(Index); ) {
            Missiles[Index].Id = Buffer->GetUInt32();
            Missiles[Index].Name = Buffer->ReadWideString();
            Missiles[Index].InfoText = Buffer->ReadWideString();
        }
        if (Version >= 5) {
            Count = Buffer->GetWord();
            CustomSystemInfos.set_length(Count);
            {
                const std::int32_t cpp_last = CustomSystemInfos.length() - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        CustomSystemInfos[Index].Name = Buffer->ReadWideString();
                        CustomSystemInfos[Index].ImagePath = Buffer->ReadWideString();
                        CustomSystemInfos[Index].Text = Buffer->ReadWideString();
                        CustomSystemInfos[Index].Distance = Buffer->GetInt32();
                    }
                }
            }
        } else {
            CustomSystemInfos = nullptr;
        }
    }

    void TEObjInfo::p_destroy() {
        aEObjInfo::TEObjInfo_Destroy(this);
    }

} // namespace aEObjInfo
