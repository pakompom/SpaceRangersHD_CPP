#include "layout/aTranclucator.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aScript.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aShip.hpp"
#include "units/aTranclucator.hpp"

namespace aTranclucator {
    pas::Array<std::int32_t, 22, 27> TranclucatorSkillBonusWeights = pas::Array<std::int32_t, 22, 27>{{100, 100, 80, 80, 60, 60}};

    // The dispatch table confines these reads to BonusKind=13..20.
    // Each reloads the unchanged byte at EBP-5; is the base biased by -13*4.
    pas::Array<std::int32_t, 13, 20> TranclucatorSlotBonusWeights = pas::Array<std::int32_t, 13, 20>{{100, 100, 200, 100, 200, 75, 10, 30}};

    void TTranclucator_Create(TTranclucator* Self) {
        aShip::TShip_Create(Self);
        Self->ClearCollectionPermissions();
        Self->ResetStoragePermissions();
        Self->AutoArrange = false;
        Self->StoreOnLanding = false;
        Self->ArtefactSize = 0;
    }

    void TTranclucator_Destroy(TTranclucator* Self) {
        aShip::TShip_Destroy(Self);
    }

    void TTranclucator::ClearCollectionPermissions() {
        TTranclucatorCollectionKind Kind{};
        for (auto cpp_range = pas::for_to<TTranclucatorCollectionKind>(tckOther, tckCountable); cpp_range.next(Kind); ) {
            CollectionPermissions[Kind] = false;
        }
    }

    void TTranclucator::SetCollectionPermission(TTranclucatorCollectionKind Kind, std::uint8_t Enabled) {
        CollectionPermissions[Kind] = Enabled;
    }

    std::uint8_t TTranclucator::GetCollectionPermission(TTranclucatorCollectionKind Kind) {
        return CollectionPermissions[Kind];
    }

    // Enables planets and disables stations.
    void TTranclucator::ResetStoragePermissions() {
        std::int32_t Kind{};
        for (Kind = 1; Kind <= 2; ++Kind) {
            StoragePermissions[Kind] = false;
        }
        SetStoragePermission(tskPlanet, true);
    }

    void TTranclucator::SetStoragePermission(TTranclucatorStorageKind Kind, std::uint8_t Enabled) {
        switch (Kind) {
            case tskPlanet: StoragePermissions[tskPlanet] = Enabled; break;
            case tskStation: StoragePermissions[tskStation] = Enabled; break;
        }
    }

    // Unknown kinds return false.
    std::uint8_t TTranclucator::GetStoragePermission(TTranclucatorStorageKind Kind) {
        std::uint8_t Result = false;
        switch (Kind) {
            case tskPlanet: return StoragePermissions[tskPlanet];
            case tskStation: return StoragePermissions[tskStation];
            default: return Result;
        }
    }

    void TTranclucator::Init(aShip::TShip* AOwnerShip, std::uint8_t Faction, std::uint8_t BasicEquipment) {
        std::uint8_t WeaponType{};
        std::int32_t MaximumHullSize{};
        auto RandomHullLevel = [&]() -> std::int32_t {
            return aMyFunction::NextRandomIntRange(1, System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 1.0, 8.0)), this->RandomState);
        };
        auto RandomEquipmentLevel = [&]() -> std::int32_t {
            return aMyFunction::NextRandomIntRange(1, System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 1.0, 4.0)), this->RandomState);
        };
        auto RandomEquipmentSize = [&](std::int32_t BaseSize) -> std::int32_t {
            std::int32_t round = System::Round(static_cast<long double>(BaseSize) * aConst::EquipmentSizeFactors[4]);
            std::int32_t round_2 = System::Round(static_cast<long double>(BaseSize) * aConst::EquipmentSizeFactors[3]);
            return aMyFunction::NextRandomIntRange(round_2, round, this->RandomState);
        };
        TypeId = aGalaxyStruct::stTranclucator;
        if (AOwnerShip != nullptr) {
            OwnerShip = AOwnerShip;
            CurrentStar = AOwnerShip->CurrentStar;
            pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        } else {
            OwnerShip = nullptr;
            CurrentStar = nullptr;
        }
        OwnerId = Faction;
        HomePlanet = nullptr;
        CurrentPlanet = nullptr;
        Position = EC_Struct::MakePointF(0.0f, 0.0f);
        MovementDirection = 0.0;
        Name = pas::WideString();
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        if (BasicEquipment) {
            {
                std::uint8_t ownerId = OwnerId;
                pas::Extended cpp_left = aMyFunction::NextRandomIntRange(200, 300, RandomState);
                std::uint16_t round = System::Round(cpp_left * aConst::HullCapacityScale);
                aShip::TShip* self = this;
                aShip::TShip_CreateAndEquipHull(self, round, 1, ownerId, -1, false);
            }
            CreateAndEquipFuelTanks(10, 1, OwnerId);
            {
                std::uint8_t ownerId_2 = OwnerId;
                std::int32_t randomEquipmentSize = RandomEquipmentSize(aConst::EngineBaseSize);
                aShip::TShip* self_2 = this;
                self_2->CreateAndEquipEngine(randomEquipmentSize, 2, ownerId_2);
            }
            WeaponType = aMyFunction::NextRandomIntRange(0, 2, RandomState) + 50;
            {
                std::uint8_t ownerId_3 = OwnerId;
                std::int32_t randomEquipmentSize_2 = RandomEquipmentSize(aConst::WeaponInfos[WeaponType].AverageSize);
                aShip::TShip* self_3 = this;
                self_3->CreateAndEquipWeapon(WeaponType, randomEquipmentSize_2, 1, ownerId_3);
            }
        } else {
            MaximumHullSize = System::Round(static_cast<long double>(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 3.0E+2, 8.0E+2)) * aConst::HullCapacityScale);
            {
                std::uint8_t ownerId_4 = OwnerId;
                std::uint16_t nextRandomIntRange = aMyFunction::NextRandomIntRange(System::Round(aConst::HullCapacityScale * 2.0E+2L), MaximumHullSize, RandomState);
                std::uint8_t randomHullLevel = RandomHullLevel();
                aShip::TShip* self_4 = this;
                aShip::TShip_CreateAndEquipHull(self_4, nextRandomIntRange, randomHullLevel, ownerId_4, -1, false);
            }
            {
                std::uint8_t ownerId_5 = OwnerId;
                std::int32_t randomEquipmentSize_3 = RandomEquipmentSize(aConst::EngineBaseSize);
                std::uint8_t randomEquipmentLevel = RandomEquipmentLevel();
                aShip::TShip* self_5 = this;
                self_5->CreateAndEquipEngine(randomEquipmentSize_3, randomEquipmentLevel, ownerId_5);
            }
            CreateAndEquipFuelTanks(10, 1, OwnerId);
            {
                std::uint8_t ownerId_6 = OwnerId;
                std::int32_t randomEquipmentSize_4 = RandomEquipmentSize(aConst::DefGeneratorBaseSize);
                std::uint8_t randomEquipmentLevel_2 = RandomEquipmentLevel();
                aShip::TShip* self_6 = this;
                self_6->CreateAndEquipDefGenerator(randomEquipmentSize_4, randomEquipmentLevel_2, ownerId_6);
            }
            {
                std::uint8_t ownerId_7 = OwnerId;
                std::int32_t randomEquipmentSize_5 = RandomEquipmentSize(aConst::RepairRobotBaseSize);
                std::uint8_t randomEquipmentLevel_3 = RandomEquipmentLevel();
                aShip::TShip* self_7 = this;
                self_7->CreateAndEquipRepairRobot(randomEquipmentSize_5, randomEquipmentLevel_3, ownerId_7);
            }
            {
                std::uint8_t ownerId_8 = OwnerId;
                std::int32_t randomEquipmentSize_6 = RandomEquipmentSize(aConst::CargoHookBaseSize);
                std::uint8_t randomEquipmentLevel_4 = RandomEquipmentLevel();
                aShip::TShip* self_8 = this;
                self_8->CreateAndEquipCargoHook(randomEquipmentSize_6, randomEquipmentLevel_4, ownerId_8);
            }
            WeaponType = aMyFunction::NextRandomIntRange(0, 2, RandomState) + 50;
            {
                std::uint8_t ownerId_9 = OwnerId;
                std::int32_t randomEquipmentSize_7 = RandomEquipmentSize(aConst::WeaponInfos[WeaponType].AverageSize);
                aShip::TShip* self_9 = this;
                self_9->CreateAndEquipWeapon(WeaponType, randomEquipmentSize_7, 1, ownerId_9);
            }
            BaseSkills[0] = aMyFunction::NextRandomIntRange(0, System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 6.0)), RandomState);
            BaseSkills[1] = aMyFunction::NextRandomIntRange(0, System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 6.0)), RandomState);
            BaseSkills[2] = aMyFunction::NextRandomIntRange(0, System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 6.0)), RandomState);
        }
        TechKnowledge = 8;
        if (GetCargoFreeSpace() < 0) {
            GetHull()->Weight = GetHull()->Weight + pas::abs(GetCargoFreeSpace());
        }
        aShip::TShip_RefreshGraphicSize(this);
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
    }

    void TTranclucator::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        TTranclucatorCollectionKind Kind{};
        std::int32_t StorageKind{};
        aShip::TShip::SaveToBuffer(Buffer);
        if (OwnerShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(OwnerShip->Id);
        }
        Buffer->AddBoolean(FollowOwner);
        Buffer->AddBoolean(SeekItems);
        Buffer->AddBoolean(AutoArrange);
        Buffer->AddIntegerValue(ArtefactSize);
        if (ArtefactSystemName == u"") {
            Buffer->AddBoolean(false);
        } else {
            Buffer->AddBoolean(true);
            Buffer->AddWideStringZ(ArtefactSystemName);
        }
        for (auto cpp_range = pas::for_to<TTranclucatorCollectionKind>(tckOther, tckCountable); cpp_range.next(Kind); ) {
            Buffer->AddBoolean(CollectionPermissions[Kind]);
        }
        for (StorageKind = 1; StorageKind <= 2; ++StorageKind) {
            Buffer->AddBoolean(StoragePermissions[StorageKind]);
        }
        Buffer->AddBoolean(StoreOnLanding);
    }

    void TTranclucator::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t OldItemCount{};
        TTranclucatorCollectionKind Kind{};
        std::int32_t StorageKind{};
        auto ReadOldPermission = [&](std::uint8_t ItemType) -> void {
            std::uint8_t Enabled = EC_Buf::TBufEC_GetBoolean(Buffer);
            switch (ItemType) {
                case 0: this->CollectionPermissions[tckGoods] = Enabled; break;
                case 8: this->CollectionPermissions[tckArtefact] = Enabled; break;
                case 43: this->CollectionPermissions[tckEquipment] = Enabled; break;
                case 69: this->CollectionPermissions[tckCountable] = Enabled; break;
                case 70: this->CollectionPermissions[tckUseless] = Enabled; break;
                case 71: this->CollectionPermissions[tckMicroModule] = Enabled; break;
            }
        };
        aShip::TShip::LoadFromBuffer(Buffer, Galaxy);
        OwnerShip = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        FollowOwner = EC_Buf::TBufEC_GetBoolean(Buffer);
        SeekItems = EC_Buf::TBufEC_GetBoolean(Buffer);
        AutoArrange = EC_Buf::TBufEC_GetBoolean(Buffer);
        ArtefactSize = EC_Buf::TBufEC_GetInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion < 129) {
            EC_Buf::TBufEC_GetByte(Buffer);
        }
        if (GlobalsV::LoadedSaveVersion >= 86 && EC_Buf::TBufEC_GetBoolean(Buffer)) {
            ArtefactSystemName = Buffer->ReadWideString();
        }
        if (GlobalsV::LoadedSaveVersion >= 131) {
            for (auto cpp_range = pas::for_to<TTranclucatorCollectionKind>(tckOther, tckCountable); cpp_range.next(Kind); ) {
                CollectionPermissions[Kind] = EC_Buf::TBufEC_GetBoolean(Buffer);
            }
        } else {
            if (GlobalsV::LoadedSaveVersion < 78) {
                OldItemCount = 68;
            } else if (GlobalsV::LoadedSaveVersion < 96) {
                OldItemCount = 72;
            } else if (GlobalsV::LoadedSaveVersion < 127) {
                OldItemCount = 73;
            } else {
                OldItemCount = 74;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, OldItemCount - 1); cpp_range_2.next(I); ) {
                ReadOldPermission(static_cast<std::uint8_t>(aItem::MigrateSavedItemType(I)));
            }
        }
        for (StorageKind = 1; StorageKind <= 2; ++StorageKind) {
            StoragePermissions[StorageKind] = EC_Buf::TBufEC_GetBoolean(Buffer);
        }
        StoreOnLanding = EC_Buf::TBufEC_GetBoolean(Buffer);
    }

    void TTranclucator_ResolveLoadedReferences(TTranclucator* Self, aGalaxy::TGalaxy* Galaxy) {
        Self->OwnerShip = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Self->OwnerShip)), true)));
        aShip::TShip_ResolveLoadedReferences(Self, Galaxy);
    }

    void TTranclucator_NextDay(TTranclucator* Self) {
        aShip::TShip_NextDay(Self);
        if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
            aShip::TShip_ScriptNextDay(Self);
            if (Self->ScriptShip != nullptr) {
                return;
            }
        }
        Self->virtual_TShip_NextDayLogic();
        if (Self->ScriptShip != nullptr && static_cast<std::uint8_t>(Self->HasScriptControl() ^ 1)) {
            aShip::TShip_ScriptNextDay(Self);
        }
    }

    // Zero-byte pointer additions retain the native evaluation order of IndexOf.
    void TTranclucator_NextDayLogic(TTranclucator* Self) {
        std::int32_t Stage = 1;
        try {
            if (Self->GetEngine() == nullptr || Self->GetFuelTanks() == nullptr) {
                Self->EquipEssentialInventory();
            }
            Stage = 2;
            if (Self->AutoArrange || Self->OwnerShip == nullptr) {
                Self->AutoEquipInventory();
                Self->AutoEquipArtefacts();
            }
            Self->virtual_TShip_RepairBrokenEquipmentAtLocation();
            Stage = 3;
            if (Self->IsOnPlanet() || Self->IsDockedToShip()) {
                Stage = 4;
                if (Self->StoreOnLanding) {
                    Stage = 5;
                    if (!Self->ConvertToStoredArtefact()) {
                        Self->UnloadCargoForPlayerOwner();
                    }
                } else {
                    Stage = 6;
                    Self->UnloadCargoForPlayerOwner();
                    if (Self->CargoFreeSpace < 0) {
                        Stage = 7;
                        aShip::TShip_DropCargoUntilNotOverloaded(Self);
                        if (Self->CargoFreeSpace < 0) {
                            Self->ConvertToStoredArtefact();
                        } else {
                            Self->OrderTakeoff();
                        }
                    }
                }
                Stage = 8;
            } else if (Self->InNormalSpace()) {
                Stage = 9;
                Self->virtual_TShip_AssignWeaponTargetsInStar();
                Stage = 10;
                if (Self->CargoFreeSpace < 0) {
                    Stage = 11;
                    aShip::TShip_DropCargoUntilNotOverloaded(Self);
                } else if (!(Self->SeekItems && Self->CargoFreeSpace > 0 && aTranclucator::TTranclucator_TryCollectPreferredFloatingLoot(Self, 50))) {
                    if (!(Self->SeekItems && Self->TryLandForStorage())) {
                        if (Self->FollowOwner && Self->OwnerShip != nullptr) {
                            Stage = 12;
                            Self->OrderFollowShip(Self->OwnerShip, 0, false);
                        } else {
                            Stage = 13;
                            if (!Self->OrderAbsolute) {
                                Self->UpdateFreeFlightOrder();
                            }
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TTranclucator.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    pas::WideString TTranclucator::GetName() {
        return GetFullName(u" "_wref.get());
    }

    pas::WideString TTranclucator::GetFullName(const pas::WideString& Separator) {
        pas::WideString Path{};
        pas::WideString DisplayName{};
        pas::WideString TypeName{};
        if (Name != u"") {
            DisplayName = Name;
        } else {
            DisplayName = pas::concat_wide({GR_Main::LookupLocalizedTextByKey(u"Artefacts.ArtTranclucator.Name"_wref.get()), u"-", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id)))});
        }
        if (TypeNameOverrideKey == u"") {
            return DisplayName;
        }
        Path = pas::concat_wide({u"ShipType.", aConst::ShipTypeNames[aGalaxyStruct::stTranclucator].Name, u".", TypeNameOverrideKey});
        if (GR_Main::LanguageDataConfig->CountParamsByPath(Path) > 0) {
            TypeName = aConst::LocalizedText(Path);
        } else {
            TypeName = aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey}));
        }
        if (TypeName != u"") {
            return pas::concat_wide({TypeName, Separator, Name});
        }
        return Name;
    }

    std::uint8_t TTranclucator::GetGreetingShipCategory() {
        return aGalaxyStruct::gscTransport;
    }

    // Always rcWarrior.
    aGalaxyStruct::TRangerCareer TTranclucator::GetDominantCareer() {
        return aGalaxyStruct::rcWarrior;
    }

    aGalaxy::TStar* TTranclucator::GetHomeStar() {
        return nullptr;
    }

    std::uint8_t TTranclucator::GetStrengthScaledPirateStatus() {
        return 100;
    }

    std::int32_t TTranclucator::GetDesiredCargoFreeSpace() {
        return 0;
    }

    // Checks FollowOwner, owner presence, shared star and owner hyperspace state; does not require docking.
    std::uint8_t TTranclucator::CanFollowOwnerInCurrentStar() {
        return FollowOwner && OwnerShip != nullptr && OwnerShip->CurrentStar == CurrentStar && static_cast<std::uint8_t>(OwnerShip->InHyperspace ^ 1);
    }

    // Fills installed fuel tanks without charging Money.
    void TTranclucator::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr) {
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    void TTranclucator_RepairBrokenEquipmentAtLocation(TTranclucator* Self) {
        if (Self->GetEngine() != nullptr && (Self->GetEngine()->BrokenFlag != 0 || Self->GetEngine()->ConditionPercent < 1.0L)) {
            Self->GetEngine()->ConditionPercent = 1.0;
            Self->GetEngine()->BrokenFlag = 0;
        }
        if (Self->GetFuelTanks() != nullptr && (Self->GetFuelTanks()->BrokenFlag != 0 || Self->GetFuelTanks()->ConditionPercent < 1.0L)) {
            Self->GetFuelTanks()->ConditionPercent = 1.0;
            Self->GetFuelTanks()->BrokenFlag = 0;
        }
    }

    // Moves unequipped inventory and artefacts plus all goods; refreshes Self and the destination player's storage bubbles.
    void TTranclucator::TransferUnequippedCargo(aShip::TShip* Destination) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        std::uint8_t Good{};
        // Nested helper; caller-popped static link, destination at ParentFrame-4.
        auto AddGoods = [&](std::uint8_t Good, std::int32_t Quantity, std::int32_t Cost) -> void {
            if (Quantity > 0) {
                Destination->CargoGoods[Good].Count += Quantity;
                Destination->CargoGoods[Good].TotalCost += Cost;
            }
        };
        {
            const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Item = pas::list_at<aItem::TEquipment>(Inventory, I + 0);
                    if (Item->EquippedFlag == 0) {
                        pas::list_delete(Inventory, pas::list_indexof(Inventory, static_cast<void*>(reinterpret_cast<std::uint8_t*>(Item) + 0)));
                        pas::list_add(Destination->Inventory, reinterpret_cast<void*>(Item));
                    }
                }
            }
        }
        {
            const std::int32_t cpp_first_2 = pas::list_count(Artefacts) - 1;
            if (cpp_first_2 >= 0) {
                for (I = cpp_first_2; I >= 0; --I) {
                    Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I + 0);
                    if (Artefact->EquippedFlag == 0) {
                        pas::list_delete(Artefacts, pas::list_indexof(Artefacts, static_cast<void*>(reinterpret_cast<std::uint8_t*>(Artefact) + 0)));
                        pas::list_add(Destination->Artefacts, reinterpret_cast<void*>(Artefact));
                    }
                }
            }
        }
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            aShip::TCargoGoodsEntry& cpp_with = CargoGoods[Good];
            AddGoods(Good, cpp_with.Count, cpp_with.TotalCost);
            cpp_with.Count = 0;
            cpp_with.TotalCost = 0;
            cpp_with.PurchasedCount = 0;
            cpp_with.PurchasedTotalCost = 0;
        }
        RefreshDerivedStats(true);
        if (aPlayer::GetPlayer() == Destination) {
            aPlayer::GetPlayer()->RefreshStorageBubbles();
        }
    }

    // The +0 index/pointer expressions below preserve native DCC32 argument scheduling.
    void TTranclucator::StoreUnequippedCargoAt(pas::Object* Location) {
        std::uint8_t Good{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TArtefact* Artefact{};
        if (!(pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) || pas::in_set<0, 4, 7, 7>(pas::checked_cast<aPlanet::TPlanet*>(Location)->OwnerId)) {
            {
                const std::int32_t cpp_first = pas::list_count(Inventory) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Item = pas::list_at<aItem::TEquipment>(Inventory, I + 0);
                        if (Item->EquippedFlag == 0) {
                            aPlayer::GetPlayer()->AddItemToPlayerStorage(Item, Location, -1);
                            pas::list_delete(Inventory, pas::list_indexof(Inventory, static_cast<void*>(reinterpret_cast<std::uint8_t*>(Item) + 0)));
                        }
                    }
                }
            }
            {
                const std::int32_t cpp_first_2 = pas::list_count(Artefacts) - 1;
                if (cpp_first_2 >= 0) {
                    for (I = cpp_first_2; I >= 0; --I) {
                        Artefact = pas::list_at<aItem::TArtefact>(Artefacts, I + 0);
                        if (Artefact->EquippedFlag == 0) {
                            aPlayer::GetPlayer()->AddItemToPlayerStorage(Artefact, Location, -1);
                            pas::list_delete(Artefacts, pas::list_indexof(Artefacts, static_cast<void*>(reinterpret_cast<std::uint8_t*>(Artefact) + 0)));
                        }
                    }
                }
            }
            for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                aShip::TCargoGoodsEntry& cpp_with = CargoGoods[Good];
                aPlayer::GetPlayer()->AddGoodsToPlayerStorage(Good, cpp_with.Count, cpp_with.TotalCost, Location, -1);
                cpp_with.Count = 0;
                cpp_with.TotalCost = 0;
                cpp_with.PurchasedCount = 0;
                cpp_with.PurchasedTotalCost = 0;
            }
            RefreshDerivedStats(true);
            aPlayer::GetPlayer()->RefreshStorageBubbles();
        }
    }

    std::uint8_t TTranclucator::UnloadCargoForPlayerOwner() {
        std::uint8_t Result = false;
        if (OwnerShip != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer() == OwnerShip) {
            if (IsOnPlanet()) {
                StoreUnequippedCargoAt(CurrentPlanet);
                if (CargoFreeSpace >= 0) {
                    OrderTakeoff();
                }
                return true;
            } else if (IsDockedToShip()) {
                StoreUnequippedCargoAt(DockedTo);
                if (CargoFreeSpace >= 0 && DockedTo->InNormalSpace()) {
                    OrderTakeoff();
                } else {
                    OrderNone(false);
                }
                return true;
            } else {
                return Result;
            }
        }
        return Result;
    }

    std::uint8_t TTranclucator::TryLandForStorage() {
        std::int32_t I{};
        pas::Object* Location{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        double Distance{};
        double BestDistance{};
        std::uint8_t Result = false;
        if (OwnerShip != nullptr && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer() == OwnerShip && HasLooseNonScriptItemsOrGoods()) {
            BestDistance = 1.0E+4;
            Location = nullptr;
            if (GetStoragePermission(tskPlanet)) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Planets) - 1); cpp_range.next(I); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId) && Planet->GetRelationLevelToShip(aPlayer::GetPlayer()) >= aGalaxyStruct::rlNormal) {
                        Distance = aMyFunction::PointDistance(Position, Planet->GetPosition());
                        if (BestDistance >= Distance) {
                            Location = Planet;
                            BestDistance = Distance;
                        }
                    }
                }
            }
            if (GetStoragePermission(tskStation)) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr && aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer()) >= aGalaxyStruct::rlNormal && Ship->virtual_TShip_CanDock(this)) {
                        Distance = aMyFunction::PointDistance(Position, Ship->Position);
                        if (BestDistance >= Distance) {
                            Location = Ship;
                            BestDistance = Distance;
                        }
                    }
                }
            }
            if (Location != nullptr) {
                OrderLanding(Location, true);
                return true;
            }
        }
        return Result;
    }

    std::uint8_t TTranclucator::ConvertToStoredArtefact() {
        std::int32_t I{};
        std::int32_t Index{};
        pas::Object* Location{};
        aItem::TArtefactTranclucator* Artefact{};
        std::uint8_t Result = false;
        StoreOnLanding = false;
        SeekItems = false;
        FollowOwner = false;
        if (IsOnPlanet()) {
            Location = CurrentPlanet;
        } else if (IsDockedToShip()) {
            Location = DockedTo;
        } else {
            return Result;
        }
        if (OwnerShip != nullptr && (!(pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) || pas::in_set<0, 4, 7, 7>(pas::checked_cast<aPlanet::TPlanet*>(Location)->OwnerId))) {
            EnemyShip = nullptr;
            TruceShip = nullptr;
            PartnerShip = nullptr;
            OrderNone(false);
            AfterburnerActive = false;
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapons[I]->Target = nullptr;
                    }
                }
            }
            if (ScriptShip != nullptr) {
                pas::checked_cast<aScript::TScriptShip*>(ScriptShip)->Script->UnbindShip(this);
            }
            if (CurrentStar != nullptr) {
                Index = pas::list_indexof(CurrentStar->Ships, reinterpret_cast<void*>(this));
                if (Index >= 0) {
                    pas::list_delete(CurrentStar->Ships, Index);
                }
            }
            CurrentStar = nullptr;
            DockedTo = nullptr;
            CurrentPlanet = nullptr;
            if (GetEngine() != nullptr) {
                GetEngine()->OutputPercent = 100;
            }
            StoreUnequippedCargoAt(Location);
            Artefact = pas::construct_call<aItem::TArtefactTranclucator>(aItem::TArtefact_Create);
            Artefact->InitTranclucator(GetHull()->OwnerId, OwnerShip, this);
            OwnerShip->AddItemToPlayerStorage(Artefact, Location, -1);
            aPlayer::GetPlayer()->RefreshStorageBubbles();
            OwnerShip->RefreshDerivedStats(true);
            ScriptItemsAct(0x0000002e, Artefact, Location, 0);
            aPlayer::GetPlayer()->ScriptItemsAct(0x0000002e, Artefact, Location, 0);
            return true;
        }
        return Result;
    }

    void TTranclucator::UpdateFreeFlightOrder() {
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
            OrderFollowShip(EnemyShip, 1, false);
        } else if (OwnerShip != nullptr && OwnerShip->CurrentStar == CurrentStar && OwnerShip->InNormalSpace()) {
            if (OwnerShip->Order == aShip::soFollowShip && OwnerShip->EnemyShip == OwnerShip->OrderTarget && OwnerShip->EnemyShip != this) {
                OrderFollowShip(OwnerShip->EnemyShip, 1, false);
            } else {
                OrderFollowShip(OwnerShip, 0, false);
            }
        } else if (OwnerShip != nullptr && OwnerShip->CurrentStar == CurrentStar && OwnerShip->CurrentPlanet != nullptr) {
            OrderMove(OwnerShip->CurrentPlanet->GetPosition(), false);
        } else {
            OrderRandomFreeFlightMove();
        }
    }

    void TTranclucator::BuildReachablePlanetQueue() {
    }

    std::uint8_t TTranclucator_CanQueueReachablePlanet(TTranclucator* Self, aPlanet::TPlanet* Planet) {
        return false;
    }

    // Returns whether a move order is active; nearby pickups can be queued even when the result is false.
    std::uint8_t TTranclucator_TryCollectPreferredFloatingLoot(TTranclucator* Self, std::int32_t MaxTravelDays) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TItem* TargetItem{};
        std::uint8_t HaveTarget{};
        double Distance{};
        double BestDistance{};
        std::uint8_t Result = false;
        if (aShip::TShip_IsEquipmentUsable(Self, Self->GetCargoHook()) && Self->Speed >= 1) {
            HaveTarget = false;
            TargetItem = nullptr;
            BestDistance = 1.0E+4;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Items) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TItem>(Self->CurrentStar->Items, I);
                if (aShip::TShip_CalculateCargoHookPower(Self, Self->GetCargoHook()) >= Item->Weight && (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") && !(pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr)) {
                    if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                        if (!Self->CollectionPermissions[tckGoods]) {
                            continue;
                        }
                    } else if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                        if (!Self->CollectionPermissions[tckArtefact]) {
                            continue;
                        }
                    } else if (pas::class_cast_if<aItem::TMicroModule*>(Item) != nullptr) {
                        if (!Self->CollectionPermissions[tckMicroModule]) {
                            continue;
                        }
                    } else if (pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr) {
                        if (!Self->CollectionPermissions[tckCountable]) {
                            continue;
                        }
                    } else if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                        if (!Self->CollectionPermissions[tckUseless]) {
                            continue;
                        }
                    } else if (pas::in_range(static_cast<std::uint8_t>(Item->ItemType), static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                        if (!Self->CollectionPermissions[tckEquipment]) {
                            continue;
                        }
                    } else if (!Self->CollectionPermissions[tckOther]) {
                        continue;
                    }
                    if (Self->CountOtherShipsTargetingItem(Item) <= 0 && Self->CargoFreeSpace - Self->GetReservedPickupWeight() >= Item->Weight) {
                        if (aShip::TShip_IsItemInPickupRange(Self, Item)) {
                            Self->AddPickupTarget(Item, false);
                        } else {
                            Distance = aMyFunction::PointDistance(Self->Position, Item->Position);
                            if (MaxTravelDays >= pas::real_divide(Distance, Self->Speed) && BestDistance >= Distance) {
                                TargetItem = Item;
                                HaveTarget = true;
                                BestDistance = Distance;
                            }
                        }
                    }
                }
            }
            if (TargetItem != nullptr) {
                Self->OrderMove(Self->GetPickupApproachPosition(TargetItem->Position), true);
            }
            if (static_cast<std::uint8_t>(HaveTarget ^ 1) && Self->Order == aShip::soMove) {
                Self->OrderNone(false);
            }
            if (Self->Order == aShip::soMove) {
                return true;
            }
        }
        return Result;
    }

    void TTranclucator::EquipEssentialInventory() {
        std::int32_t I{};
        aItem::TItem* Item{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Inventory, I);
            switch (static_cast<std::uint8_t>(Item->ItemType)) {
                case aConst::t_FuelTanks: {
                    if (GetFuelTanks() == nullptr) {
                        EquipItem(pas::checked_cast<aItem::TFuelTanks*>(Item));
                    } else if (GetFuelTanks()->Weight > Item->Weight) {
                        UnequipSlot(static_cast<std::uint8_t>(GetFuelTanks()->ItemType), 0);
                        EquipItem(pas::checked_cast<aItem::TFuelTanks*>(Item));
                    }
                    break;
                }
                case aConst::t_Engine: {
                    if (GetEngine() == nullptr) {
                        EquipItem(pas::checked_cast<aItem::TEngine*>(Item));
                    } else {
                        pas::Extended cpp_left = CalculateItemEffectiveness(Item);
                        if (cpp_left > CalculateItemEffectiveness(GetEngine())) {
                            UnequipSlot(static_cast<std::uint8_t>(GetEngine()->ItemType), 0);
                            EquipItem(pas::checked_cast<aItem::TEngine*>(Item));
                        }
                    }
                    break;
                }
            }
        }
        RefreshDerivedStats(true);
    }

    void TTranclucator_AssignWeaponTargetsInStar(TTranclucator* Self) {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        double Distance{};
        aAsteroid::TAsteroid* Asteroid{};
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(Self->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = Self->Weapons[I];
                    Weapon->Target = nullptr;
                }
            }
        }
        std::int32_t AssignedCount = 0;
        if (Self->OwnerShip != nullptr) {
            // The native code repeats the owner guard before following its enemy.
            if (Self->OwnerShip != nullptr && Self->OwnerShip->EnemyShip != nullptr && Self->OwnerShip->EnemyShip != Self && Self->OwnerShip->EnemyShip->CurrentStar == Self->CurrentStar && Self->OwnerShip->EnemyShip->InNormalSpace()) {
                const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last_2) {
                    for (J = 1; J <= cpp_last_2; ++J) {
                        Weapon = Self->Weapons[J];
                        if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && ([&] {
                            pas::Extended cpp_right = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                            return aMyFunction::PointDistanceSquared(Self->Position, Self->OwnerShip->EnemyShip->Position) <= cpp_right;
                        }())) {
                            Weapon->Target = Self->OwnerShip->EnemyShip;
                            ++AssignedCount;
                            if (Self->WeaponCount == AssignedCount) {
                                return;
                            }
                        }
                    }
                }
            }
            if (Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar && Self->EnemyShip->InNormalSpace()) {
                const std::int32_t cpp_last_3 = static_cast<std::int32_t>(Self->WeaponCount);
                if (1 <= cpp_last_3) {
                    for (J = 1; J <= cpp_last_3; ++J) {
                        Weapon = Self->Weapons[J];
                        if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && ([&] {
                            pas::Extended cpp_right_2 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                            return aMyFunction::PointDistanceSquared(Self->Position, Self->EnemyShip->Position) <= cpp_right_2;
                        }())) {
                            Weapon->Target = Self->EnemyShip;
                            ++AssignedCount;
                            if (Self->WeaponCount == AssignedCount) {
                                return;
                            }
                        }
                    }
                }
            }
            if (Self->CurrentStar->Status.Battle != 0) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                    Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                    if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiDominator) && Ship->InNormalSpace()) {
                        Distance = aMyFunction::PointDistance(Self->Position, Ship->Position);
                        {
                            const std::int32_t cpp_last_4 = static_cast<std::int32_t>(Self->WeaponCount);
                            if (1 <= cpp_last_4) {
                                for (J = 1; J <= cpp_last_4; ++J) {
                                    Weapon = Self->Weapons[J];
                                    if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && static_cast<long double>(aShip::TShip_GetWeaponRange(Self, Weapon)) >= Distance) {
                                        Weapon->Target = Ship;
                                        ++AssignedCount;
                                        if (Self->WeaponCount == AssignedCount) {
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if (Ship->InNormalSpace() && Ship != Self && Ship != Self->OwnerShip && (aShip::TShip_RelationToShip(Self, Ship) < 10 || Ship == Self->EnemyShip || Ship->EnemyShip == Self)) {
                    const std::int32_t cpp_last_5 = static_cast<std::int32_t>(Self->WeaponCount);
                    if (1 <= cpp_last_5) {
                        for (J = 1; J <= cpp_last_5; ++J) {
                            Weapon = Self->Weapons[J];
                            if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && ([&] {
                                pas::Extended cpp_right_3 = pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon));
                                return aMyFunction::PointDistanceSquared(Self->Position, Ship->Position) <= cpp_right_3;
                            }())) {
                                Weapon->Target = Ship;
                                ++AssignedCount;
                                if (Self->WeaponCount == AssignedCount) {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
            if (aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Asteroids) - 1); cpp_range_3.next(I); ) {
                    Asteroid = pas::list_at<aAsteroid::TAsteroid>(Self->CurrentStar->Asteroids, I);
                    Distance = aMyFunction::PointDistanceSquared(Self->Position, Asteroid->Position);
                    if (Distance <= 1.0E+6L) {
                        const std::int32_t cpp_last_6 = static_cast<std::int32_t>(Self->WeaponCount);
                        if (1 <= cpp_last_6) {
                            for (J = 1; J <= cpp_last_6; ++J) {
                                Weapon = Self->Weapons[J];
                                if (static_cast<std::uint8_t>(pas::in_range(static_cast<std::uint8_t>(Weapon->GetWeaponInfo()->ShotType), static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1) && Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(Self, Weapon) && static_cast<long double>(pas::sqr(aShip::TShip_GetWeaponRange(Self, Weapon))) >= Distance) {
                                    Weapon->Target = Asteroid;
                                    ++AssignedCount;
                                    if (Self->WeaponCount == AssignedCount) {
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

    void TTranclucator::SelectEnemyShipInStar() {
        EnemyShip = nullptr;
    }

    void TTranclucator::EngageEnemyShip() {
    }

    std::uint8_t TTranclucator::RelationToNonRanger(aShip::TShip* Ship) {
        if (pas::is_one_of<aGalaxyStruct::stKling, aGalaxyStruct::stTranclucator>(Ship->TypeId)) {
            return 50;
        }
        return 100;
    }

    std::uint8_t TTranclucator::RelationToRanger(void* Ranger) {
        return 100;
    }

    void TTranclucator::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
    }

    void TTranclucator::ReactToAttack(aShip::TShip* Attacker) {
        if (OwnerShip == nullptr || OwnerShip != Attacker && (!(pas::class_cast_if<TTranclucator*>(Attacker) != nullptr) || reinterpret_cast<TTranclucator*>(Attacker)->OwnerShip != OwnerShip)) {
            EnemyShip = Attacker;
        }
    }

    std::uint8_t TTranclucator_RecomputeFearState(TTranclucator* Self) {
        return false;
    }

    std::uint8_t TTranclucator_AcceptsRansomDemandFrom(TTranclucator* Self, aShip::TShip* Ship) {
        return false;
    }

    std::uint8_t TTranclucator_TrustsAttackRequester(TTranclucator* Self, aShip::TShip* Ship) {
        return Ship == Self->OwnerShip;
    }

    std::uint8_t TTranclucator_EvaluateAllyRelationAndStrength(TTranclucator* Self, aShip::TShip* Ship) {
        return Ship == Self->OwnerShip;
    }

    void TTranclucator::ProcessCombatDialogue() {
    }

    void TTranclucator::ReactToExtortionDemand(void* Ranger) {
    }

    std::uint8_t TTranclucator_BuildMoneyExtortionResponse(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TTranclucator_BuildCargoExtortionResponse(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TTranclucator::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t OfferedAmount) {
        std::uint8_t Result = false;
        Response = u"Talk not supporting"_w;
        return Result;
    }

    std::uint8_t TTranclucator_BuildAttackRequestResponse(TTranclucator* Self, aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        std::uint8_t Result = true;
        Response = aShip::TShip_LookupVisibleTalkText(Self, u"Talk.Tranclucator.Attack.Ok"_wref.get(), Requester);
        Self->SetJointAttackTarget(Requester, Target);
        Self->FollowOwner = false;
        Self->SeekItems = false;
        return Result;
    }

    std::uint8_t TTranclucator_AcceptPartnershipOffer(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TTranclucator_BuildPartnershipOfferResponse(TTranclucator* Self, aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    void TTranclucator_RefreshCurrentStanding(TTranclucator* Self) {
        std::int32_t StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            if (Self->OwnerShip == nullptr) {
                Self->CurrentStanding = aGalaxyStruct::ssUnaligned;
            } else {
                Self->OwnerShip->virtual_TShip_RefreshCurrentStanding();
                if (Self->OwnerShip->CurrentStar == Self->CurrentStar || pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssPirateMilitary>(Self->OwnerShip->CurrentStanding)) {
                    Self->CurrentStanding = Self->OwnerShip->CurrentStanding;
                } else if (pas::is_one_of<aGalaxyStruct::ssCoalitionActive, aGalaxyStruct::ssCoalitionPassive>(Self->OwnerShip->CurrentStanding)) {
                    if (pas::in_set<0, 1>(static_cast<std::uint8_t>(Self->CurrentStar->Status.ControlFaction))) {
                        Self->CurrentStanding = aGalaxyStruct::ssCoalitionActive;
                    } else {
                        Self->CurrentStanding = aGalaxyStruct::ssNeutral;
                    }
                } else if (pas::is_one_of<aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateActive>(Self->OwnerShip->CurrentStanding)) {
                    if (pas::in_set<1, 2>(static_cast<std::uint8_t>(Self->CurrentStar->Status.ControlFaction))) {
                        Self->CurrentStanding = aGalaxyStruct::ssPirateActive;
                    } else {
                        Self->CurrentStanding = aGalaxyStruct::ssNeutral;
                    }
                } else {
                    Self->CurrentStanding = Self->OwnerShip->CurrentStanding;
                }
            }
        }
    }

    float TTranclucator_EvaluateStatBonus(TTranclucator* Self, aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        static const pas::Set<0, 255> ScannableDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        float Result = 0.0f;
        if (Value == 0) {
            return Result;
        }
        {
            aConst::TEquipmentBonusKind cpp_case = BonusKind;
            if (cpp_case == aConst::bonHull) {
                return Value * 200;
            } else if (cpp_case == aConst::bonFuel) {
                return 0.0f;
            } else if (cpp_case == aConst::bonSpeed) {
                return Value;
            } else if (cpp_case == aConst::bonJump) {
                return 0.0f;
            } else if (cpp_case == aConst::bonRadar) {
                return 0.0f;
            } else if (cpp_case == aConst::bonScan) {
                return Value * 20 * (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannableDamageFlags)) & 0x0000007f);
            } else if (cpp_case == aConst::bonDroid) {
                return pas::real_divide(Value * 10, pas::real_max<float>(0.1f, Self->GetHull()->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({}))));
            } else if (cpp_case == aConst::bonHook) {
                return (Value * 0.1L + pas::real_min<pas::Extended>(static_cast<pas::Extended>(Value), static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5])) * 1.0L;
            } else if (cpp_case == aConst::bonDef) {
                return pas::real_divide(pas::real_divide(Value * 5 * 100, std::max<std::int32_t>(5, 100 - Value)) * 45.0L, std::max<std::int32_t>(5, 45 - Value));
            } else if (cpp_case == aConst::bonWEnergy) {
                return Value * 10;
            } else if (cpp_case == aConst::bonWSplinter) {
                return Value * 10;
            } else if (cpp_case == aConst::bonWMissile) {
                return ((aShip::TShip_GetRadarRange(Self) > 0) * 0.9L + 0.1L) * (Value * 10);
            } else if (cpp_case == aConst::bonWRadius) {
                return pas::sqr(pas::real_divide(std::max<std::int32_t>(100, Self->SmoothedEnemySpeed), std::max<std::int32_t>(100, Self->SmoothedSpeed))) * Value;
            } else if (cpp_case == aConst::bonHookRadius) {
                return Value * 0.1L;
            } else if (cpp_case == aConst::bonMass) {
                return aMyFunction::RemapClamped(Value, aConst::HullMassEvaluationStart, aConst::HullMassEvaluationEnd, 1.0, 0.333) * 5.0E+3L;
            } else if (cpp_case == aConst::bonSlotRadar) {
                if (Self->GetSlotCount(aConst::sskRadar) == 0 && Value > 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetRadar() != nullptr && Value < 0) {
                    return -TranclucatorSlotBonusWeights[BonusKind] - TranclucatorSlotBonusWeights[18] * (Self->CountMissileWeapons() & 0x0000007f);
                } else if (Self->GetSlotCount(aConst::sskRadar) == 1 && Value < 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotScaner) {
                if (Self->GetSlotCount(aConst::sskScanner) == 0 && Value > 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetScanner() != nullptr && Value < 0) {
                    return -TranclucatorSlotBonusWeights[BonusKind] - (Self->CountWeaponsByDamageFlags(static_cast<aGalaxyStruct::TDamageFlagSet>(ScannableDamageFlags)) & 0x0000007f) * 0.1L * TranclucatorSlotBonusWeights[18];
                } else if (Self->GetSlotCount(aConst::sskScanner) == 1 && Value < 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotDroid) {
                if (Self->GetSlotCount(aConst::sskRepairRobot) == 0 && Value > 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetRepairRobot() != nullptr && Value < 0) {
                    return -TranclucatorSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskRepairRobot) == 1 && Value < 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotHook) {
                if (Self->GetSlotCount(aConst::sskCargoHook) == 0 && Value > 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetCargoHook() != nullptr && Value < 0) {
                    return -TranclucatorSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskCargoHook) == 1 && Value < 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotDef) {
                if (Self->GetSlotCount(aConst::sskDefGenerator) == 0 && Value > 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * 0.3L;
                } else if (Self->GetDefGenerator() != nullptr && Value < 0) {
                    return -TranclucatorSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskDefGenerator) == 1 && Value < 0) {
                    return TranclucatorSlotBonusWeights[BonusKind] * -0.3L;
                } else {
                    return Result;
                }
            } else if (cpp_case == aConst::bonSlotWeapon) {
                if (Self->GetSlotCount(aConst::sskWeapon) < 5 && Value > 0) {
                    Result = std::min<std::int32_t>(Value, 5 - Self->GetSlotCount(aConst::sskWeapon)) * TranclucatorSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -Self->GetSlotCount(aConst::sskWeapon)) * TranclucatorSlotBonusWeights[BonusKind];
                }
                {
                    std::int32_t cpp_right = std::max<std::int32_t>(Value + Self->GetSlotCount(aConst::sskWeapon), 1);
                    if ((Self->CountEquippedWeapons() & 0x0000007f) > cpp_right) {
                        std::int32_t cpp_right_2 = std::max<std::int32_t>(1, Value + Self->GetSlotCount(aConst::sskWeapon));
                        return Result - ((Self->CountEquippedWeapons() & 0x0000007f) - cpp_right_2) * (TranclucatorSlotBonusWeights[BonusKind] * 0.6L);
                    }
                }
                return Result;
            } else if (cpp_case == aConst::bonSlotArt) {
                if (Self->GetSlotCount(aConst::sskArtefact) < aConst::DefaultHullSlotCounts[8] && Value > 0) {
                    Result = std::min<std::int32_t>(Value, aConst::DefaultHullSlotCounts[8] - Self->GetSlotCount(aConst::sskArtefact)) * TranclucatorSlotBonusWeights[BonusKind];
                }
                if (Value < 0) {
                    Result = std::max<std::int32_t>(Value, -Self->GetSlotCount(aConst::sskArtefact)) * TranclucatorSlotBonusWeights[BonusKind];
                }
                if (Self->Artefacts != nullptr && pas::list_count(Self->Artefacts) > std::max<std::int32_t>(Value + Self->GetSlotCount(aConst::sskArtefact), 0)) {
                    return -1.0E+3f;
                }
                return Result;
            } else if (cpp_case == aConst::bonSlotForsage) {
                if (Self->GetSlotCount(aConst::sskAfterburner) == 0 && Value > 0) {
                    return TranclucatorSlotBonusWeights[BonusKind];
                } else if (Self->GetSlotCount(aConst::sskAfterburner) == 1 && Value < 0) {
                    return -TranclucatorSlotBonusWeights[BonusKind];
                } else {
                    return Result;
                }
            } else if (cpp_case >= aConst::bonSkill1 && cpp_case <= aConst::bonSkill6) {
                if (Value > 0) {
                    Result = std::min<std::int32_t>(6 - (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f), Value) * TranclucatorSkillBonusWeights[BonusKind];
                }
                if (Value > 0 && Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) > 6) {
                    Result = (Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) - 6) * (TranclucatorSkillBonusWeights[BonusKind] * 0.05L) + Result;
                }
                if (Value < 0) {
                    Result = std::min<std::int32_t>(Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f, -Value) * -TranclucatorSkillBonusWeights[BonusKind];
                }
                if (Value < 0 && Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f) < 0) {
                    return (Value + (Self->GetEffectiveSkillLevel(static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[BonusKind - 22]), false) & 0x0000007f)) * (TranclucatorSkillBonusWeights[BonusKind] * 0.03L) + Result;
                }
                return Result;
            } else {
                return 0.0f;
            }
        }
    }

    void TTranclucator::p_destroy() {
        aTranclucator::TTranclucator_Destroy(this);
    }

    void TTranclucator::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aTranclucator::TTranclucator_ResolveLoadedReferences(this, Galaxy);
    }

    void TTranclucator::virtual_TShip_NextDay() {
        aTranclucator::TTranclucator_NextDay(this);
    }

    void TTranclucator::virtual_TShip_NextDayLogic() {
        aTranclucator::TTranclucator_NextDayLogic(this);
    }

    void TTranclucator::virtual_TShip_RepairBrokenEquipmentAtLocation() {
        aTranclucator::TTranclucator_RepairBrokenEquipmentAtLocation(this);
    }

    std::uint8_t TTranclucator::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aTranclucator::TTranclucator_CanQueueReachablePlanet(this, Planet);
    }

    void TTranclucator::virtual_TShip_AssignWeaponTargetsInStar() {
        aTranclucator::TTranclucator_AssignWeaponTargetsInStar(this);
    }

    std::uint8_t TTranclucator::virtual_TShip_RecomputeFearState() {
        return aTranclucator::TTranclucator_RecomputeFearState(this);
    }

    std::uint8_t TTranclucator::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aTranclucator::TTranclucator_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TTranclucator::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aTranclucator::TTranclucator_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TTranclucator::virtual_TShip_EvaluateAllyRelationAndStrength(aShip::TShip* Ship) {
        return aTranclucator::TTranclucator_EvaluateAllyRelationAndStrength(this, Ship);
    }

    std::uint8_t TTranclucator::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t DemandedAmount) {
        return aTranclucator::TTranclucator_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TTranclucator::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aTranclucator::TTranclucator_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TTranclucator::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response, aShip::TShip* Target) {
        return aTranclucator::TTranclucator_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TTranclucator::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aTranclucator::TTranclucator_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TTranclucator::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response, std::int32_t PaymentAmount) {
        return aTranclucator::TTranclucator_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    void TTranclucator::virtual_TShip_RefreshCurrentStanding() {
        aTranclucator::TTranclucator_RefreshCurrentStanding(this);
    }

    float TTranclucator::virtual_TShip_EvaluateStatBonus(aConst::TEquipmentBonusKind BonusKind, std::int32_t Value) {
        return aTranclucator::TTranclucator_EvaluateStatBonus(this, BonusKind, Value);
    }

} // namespace aTranclucator
