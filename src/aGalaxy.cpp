#include "layout/aGalaxy.hpp"
#include "types/Classes.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/FGInt.hpp"
#include "types/GI_GI.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/LocationClass.hpp"
#include "types/ParameterClass.hpp"
#include "types/ParameterDeltaClass.hpp"
#include "types/PathClass.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Sputnik.hpp"
#include "types/TextFieldClass.hpp"
#include "types/TextQuestInterface.hpp"
#include "types/Types.hpp"
#include "types/ValueListClass.hpp"
#include "types/aEFilm.hpp"
#include "types/aEObjInfo.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPirate.hpp"
#include "types/aTranclucator.hpp"
#include "types/aWarrior.hpp"
#include "types/ab_W.hpp"
#include "types/fFilmFile.hpp"
#include "types/fGoodsShop2.hpp"
#include "types/fGov.hpp"
#include "types/fHangar.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fScore.hpp"
#include "types/fShip2.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/CrcUnit.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/EC_Expression.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Garbage.hpp"
#include "units/SE_Gate.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/TextQuest.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aAsteroid.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aGroup.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMissile.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPath.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aRuins.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/aVector.hpp"
#include "units/ab_Ship.hpp"
#include "units/fEquipmentShop.hpp"

namespace aGalaxy {
    using PUInt64 = std::uint64_t*;

    #pragma pack(push, 1)
    struct TUInt64Words {
        std::uint32_t Low;
        std::uint32_t High;
    };
    #pragma pack(pop)

    using SelectMicroModule_TModuleOwnerMasks = pas::Array<aGalaxyStruct::TOwnerMask, 0, 2>;

    using SelectMicroModuleForEquipment_TModuleOwnerMasks = pas::Array<aGalaxyStruct::TOwnerMask, 0, 2>;

    #pragma pack(push, 1)
    struct TCollisionEntry {
        aShip::TShip* Ship;
        EC_Struct::TPointF Position;
        float DistanceSquared;
    };
    #pragma pack(pop)

    using PCollisionEntry = TCollisionEntry*;

    // Park-Miller state at ParentFrame-4; returns the updated state minus one.
    std::uint32_t NextStateXorMask(std::int32_t& Seed);

    void XorStateBytes(std::uint8_t* Data, std::uint32_t Count, std::int32_t& Seed);

    static_assert(sizeof(void*) != 4 || sizeof(aGalaxy::TUInt64Words) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxy::TUInt64Words, Low) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxy::TUInt64Words, High) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(aGalaxy::TCollisionEntry) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxy::TCollisionEntry, Ship) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxy::TCollisionEntry, Position) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxy::TCollisionEntry, DistanceSquared) == 12);

    // Reset by galaxy construction; meaning unresolved.
    std::int32_t ReservedMessageCounter = 0;

    // Reset by ship messages; incremented by Galaxy.NextDay and saved with galaxy state.
    std::uint32_t TurnsSinceLastShipMessage = 0u;

    // Startup size-check marker: positive means mismatch; nonpositive is accepted.
    std::int32_t ModuleSizeIntegrityStatus = 0;

    // Cached module CRC result.
    aGalaxy::TModuleCrcStatus ModuleCrcIntegrityStatus = aGalaxy::mcsUnchecked;

    // Cleared on mismatch while saving star 1; no native readers.
    std::int32_t ModuleCrcFailureValue = 0;

    aGalaxy::TGalaxy* Galaxy{};

    aGalaxy::TStar* PlayerStar{};

    // Incremented when a ship dialogue passes the shared message-delay threshold; only count <= 1 is accepted.
    std::uint8_t PlayerDialogueRequestCount{};

    // Borrowed TShip entries.
    pas::List* WingmenPendingLeadershipPenalty{};

    // Native initial camera-step limit.
    std::int32_t CameraSpeed = 10;

    // Native initial camera-step limit.
    std::int32_t FastCameraSpeed = 20;

    // May prepare movement or start black-hole entry; false without a player.
    std::uint8_t ShouldContinuePlayerTravel() {
        if (aPlayer::GetPlayer() == nullptr) {
            return false;
        }
        if (PlayerStar->KeepFilmRunning) {
            return true;
        }
        if (static_cast<std::uint8_t>(Globals::HasShownPlayerTip(18) ^ 1) && aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
            Globals::ShowPlayerTipOnce(18);
            return false;
        }
        if (aRanger::PlayerEquipmentBrokenThisTurn) {
            return false;
        }
        if (aRanger::PendingPlayerFollowTarget != nullptr && aPlayer::GetPlayer()->GetHullIntegrityPercent() < 25) {
            return false;
        }
        if (aRanger::PlayerAutomaticControl || aRanger::PendingPlayerFollowTarget != nullptr && static_cast<std::uint8_t>(PlayerStar->InterruptLongTravel ^ 1)) {
            return true;
        }
        aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::BaseMovementStepsPerTurn);
        if (static_cast<std::uint8_t>(PlayerStar->PlayerCombatOccurred ^ 1) && static_cast<std::uint8_t>(PlayerStar->InterruptLongTravel ^ 1) && aPlayer::GetPlayer()->GetMovementPathTurnCount() > 0 && (aPlayer::GetPlayer()->Order != aShip::soMove || aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, aPlayer::GetPlayer()->OrderDestination) > 1.96E+4L)) {
            if (aPlayer::GetPlayer()->Order == aShip::soFollowShip && static_cast<long double>(aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget)->Position)) < pas::sqr(aPlayer::GetPlayer()->Speed)) {
                return false;
            }
            return true;
        }
        return false;
    }

    // Uses distance divided by Speed + 1; zero for interrupted travel or unsupported orders.
    float EstimatePlayerTravelTurns() {
        EC_Struct::TPointF Destination{};
        float Result = 0.0f;
        if (aPlayer::GetPlayer() == nullptr) {
            return Result;
        }
        if (PlayerStar->PlayerCombatOccurred || PlayerStar->InterruptLongTravel) {
            return Result;
        }
        if (aPlayer::GetPlayer()->PickupTargets != nullptr) {
            return Result;
        }
        if (aRanger::PendingPlayerFollowTarget != nullptr) {
            Destination = aRanger::PendingPlayerFollowTarget->Position;
        } else if (aPlayer::GetPlayer()->Order == aShip::soMove) {
            Destination = aPlayer::GetPlayer()->OrderDestination;
        } else if (aPlayer::GetPlayer()->Order == aShip::soJump) {
            Destination = aPlayer::GetPlayer()->OrderDestination;
        } else if (aPlayer::GetPlayer()->Order == aShip::soJumpHole && aPlayer::GetPlayer()->OrderStateData != aGalaxyStruct::HoleExitOrderState) {
            Destination = aPlayer::GetPlayer()->OrderDestination;
        } else if (aPlayer::GetPlayer()->Order == aShip::soLand) {
            if (pas::class_cast_if<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                Destination = reinterpret_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget)->Position;
            } else {
                Destination = reinterpret_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->OrderTarget)->GetPosition();
            }
        } else if (aPlayer::GetPlayer()->Order == aShip::soTakeoff) {
            return Result;
        } else if (aPlayer::GetPlayer()->Order == aShip::soFollowShip) {
            Destination = reinterpret_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget)->Position;
        } else {
            return Result;
        }
        return pas::real_divide(aMyFunction::PointDistance(Destination, aPlayer::GetPlayer()->Position), aPlayer::GetPlayer()->Speed + 1);
    }

    // Object markup embeds the native pointer, not an ID. Ships/planets/loose items must be in the player's star. Suppress returns empty; otherwise supported objects require a player.
    pas::WideString GetLocalObjectLink(pas::Object* Obj, std::uint8_t Suppress) {
        if (Suppress) {
            return pas::WideString();
        }
        std::uint8_t Local = false;
        if (static_cast<std::uint8_t>(Local ^ 1) && pas::class_cast_if<aShip::TShip*>(Obj) != nullptr) {
            if (static_cast<aShip::TShip*>(Obj)->CurrentStar == aPlayer::GetPlayer()->CurrentStar) {
                Local = true;
            }
        }
        if (static_cast<std::uint8_t>(Local ^ 1) && pas::class_cast_if<aPlanet::TPlanet*>(Obj) != nullptr) {
            if (static_cast<aPlanet::TPlanet*>(Obj)->CurrentStar == aPlayer::GetPlayer()->CurrentStar) {
                Local = true;
            }
        }
        if (static_cast<std::uint8_t>(Local ^ 1) && pas::class_cast_if<aItem::TItem*>(Obj) != nullptr) {
            if (pas::list_indexof(aPlayer::GetPlayer()->CurrentStar->Items, reinterpret_cast<void*>(Obj)) >= 0) {
                Local = true;
            }
        }
        if (Local) {
            return static_cast<pas::WideString>(pas::concat_ansi({"<Object=", SysUtils::Int64ToStr(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Obj))), ",23,17,0>"}));
        }
        return pas::WideString();
    }

    double GameTurnToDateTime(std::int32_t Turn) {
        return Turn + 511341.5L;
    }

    pas::WideString FormatGameTurnDate(std::int32_t Turn) {
        pas::WideString MonthNumber{};
        pas::WideString MonthName{};
        MonthNumber = static_cast<pas::WideString>(SysUtilsImports::FormatDateTime("mm"_a, aGalaxy::GameTurnToDateTime(Turn - aGalaxyStruct::GalaxyWarmupTurns)));
        MonthName = aConst::LocalizedText(pas::concat_wide({u"Month.", MonthNumber}));
        return pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({SysUtilsImports::FormatDateTime("d"_a, aGalaxy::GameTurnToDateTime(Turn - aGalaxyStruct::GalaxyWarmupTurns)), " "})), MonthName, u" ", static_cast<pas::WideString>(SysUtilsImports::FormatDateTime("yyyy"_a, aGalaxy::GameTurnToDateTime(Turn - aGalaxyStruct::GalaxyWarmupTurns)))});
    }

    void TGalaxy_Create(TGalaxy* Self) {
        pas::WideString ModuleName{};
        Globals::TScriptTemplUnit* Template{};
        std::int32_t I{};
        pas::WideString DllSuffix{};
        pas::WideString LibraryPrefix{};
        EC_BlockPar::TBlockParEC* Block{};
        // Caller-popped static link; filename at ParentFrame-4. Updates the startup integrity marker.
        auto CheckModuleSize = [&](std::uint32_t EncodedSize) -> void {
            WindowsImports::THandle Handle = SysUtilsImports::FileOpen(static_cast<pas::AnsiString>(ModuleName), 0u);
            std::uint32_t Size = WindowsSdk::GetFileSize(Handle, nullptr);
            WindowsImports::CloseHandle(Handle);
            if (12345678 - Size != EncodedSize) {
                ModuleSizeIntegrityStatus = aMyFunction::RandomIntRange(996345752, 2014356243);
            } else if (ModuleSizeIntegrityStatus <= 0) {
                ModuleSizeIntegrityStatus = aMyFunction::RandomIntRange(-2021352435, -1235457467);
            }
        };
        // Nested in TGalaxy.Create; unused caller-popped static link. Accepts exactly Yes, yes, True, true, TRUE or 1; no trimming.
        auto ParseCheatsDisabledFlag = [&](const std::u16string_view& Value) -> std::uint8_t {
            return Value == u"Yes"sv || Value == u"yes"sv || Value == u"True"sv || Value == u"true"sv || Value == u"TRUE"sv || Value == u"1"sv;
        };
        EC_Struct::TObjectEx_Create(Self);
        Self->CheatsDisabled = false;
        Block = GR_Main::MainDataConfig->GetBlock(u"BV"sv);
        if (Block->CountParams(u"CheatsDisabled"_wref.get()) > 0) {
            if (ParseCheatsDisabledFlag(pas::view(EC_Str::TrimWideString(Block->GetParamByPathOrMarker(u"CheatsDisabled"_wref.get()))))) {
                Self->CheatsDisabled = true;
            }
        }
        // Preserve the native string construction and encoded module-size checks.
        DllSuffix = u"ll"_w;
        DllSuffix = pas::concat_wide({u".d", DllSuffix});
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"sotoenalm^_^aucah"_w), DllSuffix});
        if (WindowsSdk::GetModuleHandleW(ModuleName.pchar()) != 0) {
            CheckModuleSize(0x00bb554eu);
        }
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"sotoenalm^_^aupki"_w), DllSuffix});
        if (WindowsSdk::GetModuleHandleW(ModuleName.pchar()) != 0) {
            CheckModuleSize(0x00babda6u);
        }
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"zoloimba"_w), DllSuffix});
        CheckModuleSize(0x00bb734eu);
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"MhastorhinxaGrakmae"_w), DllSuffix});
        CheckModuleSize(0x00a4cd4eu);
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"ookogifa"_w), DllSuffix});
        CheckModuleSize(0x00b3f14eu);
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"xavriadeccomrie"_w), DllSuffix});
        CheckModuleSize(0x00b09064u);
        LibraryPrefix = u"ib"_w;
        LibraryPrefix = pas::concat_wide({u"l", LibraryPrefix});
        ModuleName = pas::concat_wide({LibraryPrefix, EC_Str::DecodeTextW(u"osgaga-10a"_w), DllSuffix});
        CheckModuleSize(0x00bb50ffu);
        ModuleName = pas::concat_wide({LibraryPrefix, EC_Str::DecodeTextW(u"vrokrablius-->0"_w), DllSuffix});
        CheckModuleSize(0x00b99523u);
        ModuleName = pas::concat_wide({LibraryPrefix, EC_Str::DecodeTextW(u"veohrablissufainlae"_w), DllSuffix});
        CheckModuleSize(0x00bb8916u);
        GlobalsV::GameEndReason = GlobalsV::gerDefault;
        Self->PlayerRangerIndex = -1;
        for (I = 0; I <= 8; ++I) {
            Globals::HangarScreen->ShipSlots[I].ShipId = 0;
        }
        Self->SaveCount = 0;
        Self->LoadCount = 0;
        if (!GlobalsV::MemorySnapshotActive) {
            GR_Main::CCInterface->Buffer->Clear();
        }
        SystemImports::Randomize();
        Self->GenerationSeed = aMyFunction::RandomIntRange(100000, SystemImports::MaxInt);
        Self->SetCheatPoints(0);
        Self->RandomState = Self->GenerationSeed;
        Self->AverageRangerCapital = 3000;
        Self->MaxRangerWealth = 3000;
        Self->AverageRangerStrength = 1.0f;
        Self->BestRangerStrength = 1.0f;
        Self->ConstellationCount = 20;
        Self->Constellations = pas::make_object<aMyFunction::TObjectList>();
        Self->Stars = pas::make_object<aMyFunction::TObjectList>();
        Self->Holes = pas::make_object<aMyFunction::TObjectList>();
        Self->StoredItems = pas::make_object<aMyFunction::TObjectList>();
        Self->Planets = pas::make_object<pas::List>();
        Self->Rangers = pas::make_object<pas::List>();
        Self->ShipsInTransit = pas::make_object<pas::List>();
        Self->Scripts = pas::make_object<pas::List>();
        Self->LiberationGroups = pas::make_object<pas::List>();
        Self->PlanetNews = pas::make_object<pas::List>();
        Self->CustomWeaponTypes = pas::make_object<pas::List>();
        if (Globals::PrimaryFilm != nullptr) {
            Globals::PrimaryFilm->Clear();
        }
        if (Globals::SecondaryFilm != nullptr) {
            Globals::SecondaryFilm->Clear();
        }
        Globals::ClearPersistentPlayerMessages();
        Self->JumpGates = pas::make_object<pas::List>();
        Self->NextConstellationId = 1u;
        Self->NextStarId = 1u;
        Self->NextHoleId = 1u;
        Self->NextPlanetId = 1u;
        Self->NextSputnikId = 1u;
        Self->NextAsteroidId = 1u;
        Self->NextShipId = 1u;
        Self->NextItemId = 1u;
        Self->NextMissileId = 1u;
        Globals::SharedScriptVariables->CopyFrom(Globals::GlobalScriptVariables, true);
        std::int32_t TemplateCount = pas::list_count(Globals::ScriptTemplates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TemplateCount - 1); cpp_range.next(I); ) {
            Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, I);
            Template->UseCount = 0;
            Template->LastTurn = 0;
            Template->ConditionCode->LinkAll(Globals::SharedScriptVariables, false);
            Template->ConditionCode->LinkAll(aScript::ScriptFunctionScope, false);
            Template->ConditionCode->ScriptFunLinked = true;
            Template->ActiveScriptIndex = -1;
        }
        Globals::PreviousFilmActivity = 0u;
        Globals::ShownPlayerTips = 0u;
        Self->EminentCareerShips[aGalaxyStruct::rcTrader] = nullptr;
        Self->EminentCareerShips[aGalaxyStruct::rcPirate] = nullptr;
        Self->EminentCareerShips[aGalaxyStruct::rcWarrior] = nullptr;
        ReservedMessageCounter = 0;
        TurnsSinceLastShipMessage = 0u;
        Self->IronWill = false;
        Self->DominatorModLevel = 0;
        Self->TechnicModEnabled = 0;
        Self->AmmoModEnabled = 0;
        Self->GodModEnabled = 0;
        Self->UltraScanModEnabled = 0;
        Self->StasisModEnabled = 0;
        Self->NextSpecialStationServiceTurn = 0;
        Self->GalaxyEvents = pas::make_object<aMyFunction::TObjectList>();
        Self->ScoreScreenDismissed = 0;
        Self->Destroying = false;
        Self->InterfaceStateOverrides = pas::make_object<aMyFunction::TObjectList>();
        Self->InterfaceTextOverrides = pas::make_object<aMyFunction::TObjectList>();
        Self->InterfaceImageOverrides = pas::make_object<aMyFunction::TObjectList>();
        Self->InterfacePositionOverrides = pas::make_object<aMyFunction::TObjectList>();
        Self->InterfaceSizeOverrides = pas::make_object<aMyFunction::TObjectList>();
        Self->LoadedShips = pas::make_object<pas::List>();
        Self->SpecialSimulationMode = 0;
    }

    void TGalaxy_Destroy(TGalaxy* Self) {
        EC_Struct::PPointF Point{};
        std::int32_t I{};
        std::int32_t J{};
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        PPlanetNewsEntry News{};
        aConst::PWeaponInfo WeaponInfo{};
        TStoredItem* Stored{};
        std::uint8_t ResourceFailure{};
        Self->Destroying = true;
        if (GR_Main::CCInterface->GetProtectedStateXorSeed() != 0) {
            Self->RestoreProtectedState();
        }
        if (Self->ConstellationOutlineJunctions != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->ConstellationOutlineJunctions) - 1); cpp_range.next(I); ) {
                Point = pas::list_at<EC_Struct::TPointF>(Self->ConstellationOutlineJunctions, I);
                pas::dispose(Point);
            }
            pas::list_clear(Self->ConstellationOutlineJunctions);
            pas::free(Self->ConstellationOutlineJunctions);
            Self->ConstellationOutlineJunctions = nullptr;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->StoredItems) - 1); cpp_range_2.next(I); ) {
            Stored = pas::list_at<TStoredItem>(Self->StoredItems, I);
            if (Stored->Item != nullptr) {
                pas::free(Stored->Item);
            }
            Stored->Item = nullptr;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Stars) - 1); cpp_range_3.next(I); ) {
            Star = pas::list_at<TStar>(Self->Stars, I);
            while (pas::list_count(Star->Ships) > 0) {
                pas::free(pas::list_at<pas::Object>(Star->Ships, 0));
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_4.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                while (pas::list_count(Planet->Warriors) > 0) {
                    pas::free(pas::list_at<pas::Object>(Planet->Warriors, 0));
                }
            }
        }
        if (Self->Scripts != nullptr) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Scripts) - 1); cpp_range_5.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Self->Scripts, I));
            }
            pas::free(Self->Scripts);
            Self->Scripts = nullptr;
        }
        if (Self->LiberationGroups != nullptr) {
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Self->LiberationGroups) - 1); cpp_range_6.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Self->LiberationGroups, I));
            }
            pas::free(Self->LiberationGroups);
            Self->LiberationGroups = nullptr;
        }
        fEquipmentShop::ClearTemporaryShopSlotGrid();
        pas::free(Self->Stars);
        Self->Stars = nullptr;
        pas::free(Self->Holes);
        Self->Holes = nullptr;
        pas::free(Self->StoredItems);
        Self->StoredItems = nullptr;
        pas::list_clear(Self->Planets);
        pas::free(Self->Planets);
        Self->Planets = nullptr;
        pas::list_clear(Self->Rangers);
        pas::free(Self->Rangers);
        Self->Rangers = nullptr;
        pas::list_clear(Self->ShipsInTransit);
        pas::free(Self->ShipsInTransit);
        Self->ShipsInTransit = nullptr;
        pas::free(Self->Constellations);
        Self->Constellations = nullptr;
        // The original repeats the now-empty liberation-group cleanup.
        if (Self->LiberationGroups != nullptr) {
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Self->LiberationGroups) - 1); cpp_range_7.next(I); ) {
                pas::free(pas::list_at<pas::Object>(Self->LiberationGroups, I));
            }
            pas::free(Self->LiberationGroups);
            Self->LiberationGroups = nullptr;
        }
        {
            const std::int32_t cpp_first = pas::list_count(Self->PlanetNews) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    News = pas::list_at<aGalaxyStruct::TPlanetNews>(Self->PlanetNews, I);
                    pas::list_delete(Self->PlanetNews, I);
                    pas::dispose(News);
                }
            }
        }
        pas::list_clear(Self->PlanetNews);
        pas::free(Self->PlanetNews);
        Self->PlanetNews = nullptr;
        Self->ClearJumpGates();
        pas::free(Self->JumpGates);
        Self->JumpGates = nullptr;
        aPlayer::SetPlayer(nullptr, Self);
        PlayerStar = nullptr;
        if (aKling::DominatorSpawnPlanet != nullptr) {
            pas::free(aKling::DominatorSpawnPlanet);
            aKling::DominatorSpawnPlanet = nullptr;
        }
        Globals::ClearPersistentPlayerMessages();
        if (Globals::PrimaryFilm != nullptr) {
            Globals::PrimaryFilm->Clear();
        }
        if (Globals::SecondaryFilm != nullptr) {
            Globals::SecondaryFilm->Clear();
        }
        Self->SpaceBackgroundEntries = nullptr;
        pas::free(Self->GalaxyEvents);
        Self->GalaxyEvents = nullptr;
        pas::free(Self->InterfaceStateOverrides);
        Self->InterfaceStateOverrides = nullptr;
        pas::free(Self->InterfaceTextOverrides);
        Self->InterfaceTextOverrides = nullptr;
        pas::free(Self->InterfaceImageOverrides);
        Self->InterfaceImageOverrides = nullptr;
        pas::free(Self->InterfacePositionOverrides);
        Self->InterfacePositionOverrides = nullptr;
        pas::free(Self->InterfaceSizeOverrides);
        Self->InterfaceSizeOverrides = nullptr;
        pas::free(Self->LoadedShips);
        Self->LoadedShips = nullptr;
        if (!GlobalsV::MemorySnapshotActive) {
            ResourceFailure = GR_Main::CCInterface->GetResourceChecksumFailed();
            GR_Main::CCInterface->Reset();
            GR_Main::CCInterface->SetResourceChecksumFailed(ResourceFailure);
        }
        Self->Destroying = false;
        aScript::ClearPendingScriptRequests();
        if (Self->CustomWeaponTypes != nullptr) {
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CustomWeaponTypes) - 1); cpp_range_8.next(I); ) {
                WeaponInfo = pas::list_at<aConst::TWeaponInfo>(Self->CustomWeaponTypes, I);
                pas::dispose(WeaponInfo);
            }
            pas::list_clear(Self->CustomWeaponTypes);
            pas::free(Self->CustomWeaponTypes);
            Self->CustomWeaponTypes = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TGalaxy::InitializeCampaignState() {
        aGalaxyStruct::TDominatorSeries I{};
        CurrentTurn = 0;
        PirateCount = 0;
        TransportCount = 0;
        Globals::StarMapWeaponPanelOpen = true;
        ChecksumScalarD0 = 0.0f;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(I); ) {
            DominatorResearch[I].Progress = 0.0f;
            switch (DifficultyLevels[2]) {
                case 0: DominatorResearch[I].Material = 200; break;
                case 1: DominatorResearch[I].Material = 100; break;
                case 2: DominatorResearch[I].Material = 70; break;
                case 3: DominatorResearch[I].Material = 30; break;
                default: DominatorResearch[I].Material = 0; break;
            }
        }
        ChecksumScalarEC = 0.0f;
        WarDeltaWin[1] = 0;
        WarDeltaWin[2] = 0;
        WarDeltaWin[0] = 0;
        RangerSpawnQuotas[aGalaxyStruct::oiMaloc] = 0;
        RangerSpawnQuotas[aGalaxyStruct::oiPeleng] = 0;
        RangerSpawnQuotas[aGalaxyStruct::oiHuman] = 0;
        RangerSpawnQuotas[aGalaxyStruct::oiFeyan] = 0;
        RangerSpawnQuotas[aGalaxyStruct::oiGaal] = 0;
        ComputeGlobalGoodsPriceBands();
        CampaignFlag183 = 0;
        GR_Main::CCInterface->SetEditableStateApplied(false);
        FinalizationNameEncoded = pas::WideString();
        SpecialSimulationMode = 0;
    }

    // Increments SaveCount and restores temporary shop stock before serialization.
    void TGalaxy::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        std::int32_t J{};
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aRanger::TRanger* Ranger{};
        aRanger::PPlayerOldQuest OldQuest{};
        PJumpGateEntry Gate{};
        TConstellation* Constellation{};
        Globals::TScriptTemplUnit* Template{};
        aScript::TScript* Script{};
        aGroup::TGroup* Group{};
        fEquipmentShop::TShopSlot* ShopSlot{};
        THole* Hole{};
        aGalaxyStruct::TRangerCareer Career{};
        PPlanetNewsEntry News{};
        aGalaxyStruct::TDominatorSeries Series{};
        std::uint8_t Difficulty{};
        TStoredItem* Stored{};
        aConst::PWeaponInfo WeaponInfo{};
        aGalaxyStruct::TOwnerId Race{};
        if (fEquipmentShop::TemporaryShopSlots != nullptr && aPlayer::GetPlayer()->CurrentPlanet != fEquipmentShop::TemporaryShopPlanet && aPlayer::GetPlayer()->DockedTo != fEquipmentShop::TemporaryShopStation) {
            fEquipmentShop::RestoreTemporaryShopStock();
        }
        TGalaxy::RunConfigOnSaveHandlers();
        Buffer->AddWideStringZ(GR_Main::SelectedMods);
        Buffer->AddIntegerValue(GenerationSeed);
        Buffer->AddDWord(RandomState);
        Buffer->AddIntegerValue(AverageRangerCapital);
        Buffer->AddIntegerValue(MaxRangerWealth);
        Buffer->AddSingle(AverageRangerStrength);
        Buffer->AddSingle(BestRangerStrength);
        Buffer->AddBoolean(GR_Main::CCInterface->GetTamperDetected());
        Buffer->AddBoolean(GR_Main::CCInterface->GetFlag0A());
        Buffer->AddIntegerValue(0);
        Buffer->AddIntegerValue(GetCheatPoints());
        ++SaveCount;
        Buffer->AddIntegerValue(SaveCount);
        Buffer->AddIntegerValue(LoadCount);
        std::int32_t Count = pas::list_count(CustomWeaponTypes);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            WeaponInfo = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, I);
            Buffer->AddWideStringZ(WeaponInfo->ConfigName);
            Buffer->AddAnsiChar(WeaponInfo->TechLevel);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(WeaponInfo->InventionIndex));
            Buffer->AddSingle(WeaponInfo->CostFactor);
            Buffer->AddIntegerValue(WeaponInfo->MinDamage);
            Buffer->AddIntegerValue(WeaponInfo->MaxDamage);
            Buffer->AddIntegerValue(WeaponInfo->AverageSize);
            Buffer->AddIntegerValue(WeaponInfo->AverageRange);
            Buffer->AddIntegerValue(WeaponInfo->ShotSpeedPercent);
            Buffer->AddIntegerValue(WeaponInfo->MissileRange);
            Buffer->AddIntegerValue(WeaponInfo->MissileMaxSpeed);
            Buffer->AddIntegerValue(WeaponInfo->MissileMinSpeed);
            Buffer->AddAnsiChar(WeaponInfo->MissileChanceToBeHit);
            Buffer->AddDWord(pas::load_unaligned<std::uint32_t>(&WeaponInfo->DamageFlags));
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(WeaponInfo->ShotType));
            Buffer->AddAnsiChar(WeaponInfo->ShotCount);
            Buffer->AddAnsiChar(WeaponInfo->AttackCount);
            Buffer->AddSingle(WeaponInfo->SecondaryDamageRadius);
            Buffer->AddSingle(WeaponInfo->MiningFactor);
            for (J = 1; J <= 8; ++J) {
                Buffer->AddSingle(pas::load_unaligned<float>(pas::byte_offset(&WeaponInfo->DamageScaleByLevel, (J - 1) * sizeof(float))));
            }
            if (WeaponInfo->PrimarySE == u"") {
                Buffer->AddBoolean(false);
            } else {
                Buffer->AddBoolean(true);
                Buffer->AddWideStringZ(WeaponInfo->PrimarySE);
            }
            if (WeaponInfo->SecondarySE == u"") {
                Buffer->AddBoolean(false);
            } else {
                Buffer->AddBoolean(true);
                Buffer->AddWideStringZ(WeaponInfo->SecondarySE);
            }
            if (WeaponInfo->AreaSE == u"") {
                Buffer->AddBoolean(false);
            } else {
                Buffer->AddBoolean(true);
                Buffer->AddWideStringZ(WeaponInfo->AreaSE);
            }
            Buffer->AddIntegerValue(WeaponInfo->DefaultPalette);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(WeaponInfo->Availability));
            Buffer->AddAnsiChar(WeaponInfo->ArcadeWeaponType);
        }
        Count = pas::list_count(Constellations);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            Constellation->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Stars);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            Star->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Holes);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            Hole = pas::list_at<THole>(Holes, I);
            Hole->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(StoredItems);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
            Stored = pas::list_at<TStoredItem>(StoredItems, I);
            Stored->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(JumpGates);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(I); ) {
            Gate = pas::list_at<TJumpGateEntry>(JumpGates, I);
            Buffer->AddSingle(Gate->Gate->Position.X);
            Buffer->AddSingle(Gate->Gate->Position.Y);
            Buffer->AddAnsiChar(Gate->Gate->GetAngle());
            Buffer->AddWideChar(Gate->Gate->Size.X);
            Buffer->AddWideStringZ(Gate->Gate->GetText());
        }
        Count = pas::list_count(Planets);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            Buffer->AddDWord(Planet->Id);
        }
        Count = pas::list_count(Rangers);
        Buffer->AddWord(Count);
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(Rangers, I);
            Buffer->AddDWord(Ranger->Id);
        }
        for (auto cpp_range_9 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range_9.next(Race); ) {
            Buffer->AddIntegerValue(RangerSpawnQuotas[Race]);
        }
        if (KellerTargetStar == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(KellerTargetStar->Id);
        }
        Buffer->AddIntegerValue(KellerMissionState);
        Count = 0;
        if (fEquipmentShop::TemporaryShopSlots != nullptr) {
            Count = pas::list_count(fEquipmentShop::TemporaryShopSlots);
        }
        Buffer->AddWideChar(Count);
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(I); ) {
            ShopSlot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I);
            ShopSlot->SaveToBuffer(Buffer);
        }
        Globals::SharedScriptVariables->SaveToBuffer(Buffer);
        Count = pas::list_count(Globals::ScriptTemplates);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(I); ) {
            Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, I);
            Buffer->AddWideStringZ(Template->Name);
            Buffer->AddWideChar(Template->UseCount);
            Buffer->AddIntegerValue(Template->LastTurn);
            Buffer->AddIntegerValue(Template->ActiveScriptIndex);
        }
        Count = pas::list_count(Scripts);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_12.next(I); ) {
            Script = pas::list_at<aScript::TScript>(Scripts, I);
            Script->SaveState(Buffer);
        }
        Count = pas::list_count(LiberationGroups);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_13.next(I); ) {
            Group = pas::list_at<aGroup::TGroup>(LiberationGroups, I);
            Group->Save(Buffer);
        }
        Buffer->AddAnsiChar('\000');
        Buffer->AddIntegerValue(0);
        Buffer->AddWideChar(PirateCount);
        Buffer->AddWideChar(PirateClanCount);
        Buffer->AddWideChar(TransportCount);
        Buffer->AddDWord(CurrentTurn);
        for (Difficulty = static_cast<std::uint8_t>(0); Difficulty <= static_cast<std::uint8_t>(7); ++Difficulty) {
            Buffer->AddAnsiChar(DifficultyLevels[Difficulty]);
        }
        Buffer->AddDWord(aPlayer::GetPlayer()->Id);
        if (aRanger::PendingPlayerFollowTarget == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(aRanger::PendingPlayerFollowTarget->Id);
        }
        if (aKling::BlazerShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(aKling::BlazerShip->Id);
        }
        if (aKling::KellerShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(aKling::KellerShip->Id);
        }
        if (aKling::TerronShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(aKling::TerronShip->Id);
        }
        Buffer->AddDWord(PlayerStar->Id);
        Buffer->AddDWord(aKling::PieceCreatorTargetStarId);
        for (auto cpp_range_14 = pas::for_to<aGalaxyStruct::TRangerCareer>(aGalaxyStruct::rcTrader, aGalaxyStruct::rcWarrior); cpp_range_14.next(Career); ) {
            if (EminentCareerShips[Career] == nullptr) {
                Buffer->AddDWord(0u);
            } else {
                Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(EminentCareerShips[Career])->Id);
            }
        }
        Count = pas::list_count(aRanger::PlayerOldQuests);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_15.next(I); ) {
            OldQuest = pas::list_at<aRanger::TPlayerOldQuest>(aRanger::PlayerOldQuests, I);
            if (OldQuest->Planet == nullptr) {
                Buffer->AddDWord(0u);
            } else {
                Buffer->AddDWord(OldQuest->Planet->Id);
            }
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(OldQuest->QuestType));
            Buffer->AddWideChar(OldQuest->QuestNumber);
            Buffer->AddWideStringZ(OldQuest->Description);
            Buffer->AddBoolean(OldQuest->Successful);
            Buffer->AddBoolean(OldQuest->Declined);
        }
        Count = pas::list_count(PlanetNews);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_16 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_16.next(I); ) {
            News = pas::list_at<aGalaxyStruct::TPlanetNews>(PlanetNews, I);
            Buffer->AddDWord(News->Id);
            Buffer->AddDWord(News->Turn);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(News->NewsType));
            Buffer->AddWideStringZ(News->Text);
        }
        Buffer->AddDWord(ReservedMessageCounter);
        Buffer->AddDWord(TurnsSinceLastShipMessage);
        Buffer->AddSingle(ChecksumScalarD0);
        for (auto cpp_range_17 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_17.next(Series); ) {
            Buffer->AddSingle(DominatorResearch[Series].Progress);
            Buffer->AddDWord(DominatorResearch[Series].Material);
        }
        Buffer->AddSingle(ChecksumScalarEC);
        Buffer->AddIntegerValue(WarDeltaWin[1]);
        Buffer->AddIntegerValue(WarDeltaWin[2]);
        Buffer->AddIntegerValue(WarDeltaWin[0]);
        Buffer->AddBuffer(GR_Main::CCInterface->Buffer);
        Buffer->AddIntegerValue(SpaceBackgroundEntries.length() - 1 + 1);
        {
            const std::int32_t cpp_last = SpaceBackgroundEntries.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Buffer->AddIntegerValue(SpaceBackgroundEntries[I].ImageIndex);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].OrbitCenter.X);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].OrbitCenter.Y);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].OrbitCenter.Z);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].Position.X);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].Position.Y);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].Position.Z);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].Unknown38.X);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].Unknown38.Y);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].Unknown38.Z);
                    Buffer->AddSingle(SpaceBackgroundEntries[I].OrbitStepDegrees);
                    Buffer->AddIntegerValue(SpaceBackgroundEntries[I].FrameIndex);
                }
            }
        }
        for (I = 0; I <= 8; ++I) {
            Buffer->AddDWord(Globals::HangarScreen->ShipSlots[I].ShipId);
        }
        Buffer->AddIntegerValue(GR_Main::CCInterface->GetValue10());
        Buffer->AddIntegerValue(GR_Main::CCInterface->GetIntegrityStatus());
        Buffer->AddIntegerValue(GR_Main::CCInterface->GetIntegrityError());
        Buffer->AddIntegerValue(GR_Main::CCInterface->GetIntegrityChecksum());
        Buffer->AddWideChar(pas::list_count(ShipsInTransit));
        for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, pas::list_count(ShipsInTransit) - 1); cpp_range_18.next(I); ) {
            Buffer->AddDWord(pas::list_at<aShip::TShip>(ShipsInTransit, I)->Id);
        }
        Buffer->AddIntegerValue(TerronWeaponLockTurn);
        Buffer->AddIntegerValue(TerronGrowLockTurn);
        Buffer->AddIntegerValue(TerronLandingLockTurn);
        Buffer->AddIntegerValue(TerronToStarTurn);
        Buffer->AddIntegerValue(KellerLeaveTurn);
        Buffer->AddDWord(KellerResearchTargetStarId);
        Buffer->AddDWord(BlazerLandingPlanetId);
        Buffer->AddIntegerValue(BlazerSelfDestructTurn);
        Buffer->AddIntegerValue(TerronSeriesResolvedTurn);
        Buffer->AddIntegerValue(KellerSeriesResolvedTurn);
        Buffer->AddIntegerValue(BlazerSeriesResolvedTurn);
        Buffer->AddIntegerValue(PirateWinTurn);
        Buffer->AddIntegerValue(PirateWinType);
        Buffer->AddIntegerValue(CoalitionDefeatedTurn);
        Buffer->AddBoolean(GraphDominatorSurfacesEnabled);
        Buffer->AddAnsiChar(SpaceEffectKind);
        Buffer->AddBoolean(IronWill);
        Buffer->AddAnsiChar(DominatorModLevel);
        Buffer->AddAnsiChar(TechnicModEnabled);
        Buffer->AddAnsiChar(AmmoModEnabled);
        Buffer->AddAnsiChar(GodModEnabled);
        Buffer->AddAnsiChar(UltraScanModEnabled);
        Buffer->AddAnsiChar(StasisModEnabled);
        Buffer->AddDWord(NextPlanetNewsId);
        Buffer->AddIntegerValue(NextSpecialStationServiceTurn);
        Count = pas::list_count(GalaxyEvents);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_19.next(I); ) {
            pas::list_at<aGalaxyEvent::TGalaxyEvent>(GalaxyEvents, I)->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(InterfaceStateOverrides);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_20 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_20.next(I); ) {
            pas::list_at<TInterfaceStateOverride>(InterfaceStateOverrides, I)->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(InterfaceTextOverrides);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_21 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_21.next(I); ) {
            pas::list_at<TInterfaceTextOverride>(InterfaceTextOverrides, I)->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(InterfaceImageOverrides);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_22 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_22.next(I); ) {
            pas::list_at<TInterfaceImageOverride>(InterfaceImageOverrides, I)->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(InterfacePositionOverrides);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_23 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_23.next(I); ) {
            pas::list_at<TInterfacePosOverride>(InterfacePositionOverrides, I)->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(InterfaceSizeOverrides);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_24 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_24.next(I); ) {
            pas::list_at<TInterfaceSizeOverride>(InterfaceSizeOverrides, I)->SaveToBuffer(Buffer);
        }
        Buffer->AddDWord(NextShipId);
        Buffer->AddDWord(NextItemId);
        Buffer->AddDWord(GenerationMachineHash);
        Buffer->AddBoolean(GR_Main::CCInterface->GetEditableStateApplied());
        Buffer->AddWideStringZ(FinalizationNameEncoded);
        Buffer->AddBoolean(CustomRules.Enabled);
        Buffer->AddAnsiChar(CustomRules.DominatorStrength);
        Buffer->AddAnsiChar(CustomRules.DominatorAggression);
        Buffer->AddAnsiChar(CustomRules.DominatorSpawn);
        Buffer->AddAnsiChar(CustomRules.PirateAggression);
        Buffer->AddAnsiChar(CustomRules.CoalitionAggression);
        Buffer->AddAnsiChar(CustomRules.AsteroidModifier);
        Buffer->AddAnsiChar(CustomRules.SunDamageModifier);
        Buffer->AddAnsiChar(CustomRules.ExtraInventions);
        Buffer->AddAnsiChar(CustomRules.AcrynModifier);
        Buffer->AddAnsiChar(CustomRules.NodeDropModifier);
        Buffer->AddAnsiChar(CustomRules.ArcadeDropValueModifier);
        Buffer->AddAnsiChar(CustomRules.DropValueModifier);
        Buffer->AddAnsiChar(CustomRules.AgriculturalPlanetWeight);
        Buffer->AddAnsiChar(CustomRules.MixedPlanetWeight);
        Buffer->AddAnsiChar(CustomRules.IndustrialPlanetWeight);
        Buffer->AddAnsiChar(CustomRules.ExtraRangers);
        Buffer->AddAnsiChar(CustomRules.ArcadeHitpointsModifier);
        Buffer->AddAnsiChar(CustomRules.ArcadeDamageModifier);
        Buffer->AddAnsiChar(CustomRules.AIJunkTolerance);
        Buffer->AddBoolean(CustomRules.ChaoticRandom);
        Buffer->AddBoolean(CustomRules.UnrestrictedEquipmentKnowledge);
        Buffer->AddBoolean(CustomRules.StationsNearStars);
        Buffer->AddBoolean(CustomRules.FullStationTargeting);
        Buffer->AddBoolean(CustomRules.SpecialShips);
        Buffer->AddBoolean(CustomRules.ZeroStartingExperience);
        Buffer->AddBoolean(CustomRules.ArcadeBattleRoyale);
        Buffer->AddBoolean(CustomRules.DominatorRacialWeapons);
        Buffer->AddBoolean(CustomRules.StartInCenter);
        Buffer->AddBoolean(CustomRules.MaxRangeMissiles);
        Buffer->AddBoolean(CustomRules.OldHyperspace);
        Buffer->AddBoolean(CustomRules.PirateNodes);
        Buffer->AddBoolean(CustomRules.AIUseShops);
        Buffer->AddBoolean(CustomRules.StationsUseShop);
        Buffer->AddBoolean(CustomRules.DuplicateArtefacts);
        Buffer->AddAnsiChar(CustomRules.HullGrowth);
        Buffer->AddBoolean(CustomRules.ArcadeEquipmentChange);
        Buffer->AddBoolean(CustomRules.OldSpeedCalculation);
        Buffer->AddBoolean(CustomRules.OldMissileBonuses);
        Buffer->AddBoolean(false);
        Buffer->AddBoolean(false);
        Buffer->AddBoolean(false);
        Buffer->AddBoolean(false);
        Buffer->AddBoolean(false);
        if (CampaignFlag183 != 0) {
            SaveEditableState();
        }
    }

    void TGalaxy_LoadFromBuffer(TGalaxy* Self, EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t Count{};
        std::int32_t TemplateIndex{};
        std::int32_t ShipCount{};
        float X{};
        float Y{};
        TStar* Star{};
        aShip::TShip* Ship{};
        aRanger::PPlayerOldQuest OldQuest{};
        PJumpGateEntry Gate{};
        TConstellation* Constellation{};
        Globals::TScriptTemplUnit* Template{};
        aScript::TScript* Script{};
        aGroup::TGroup* Group{};
        fEquipmentShop::TShopSlot* ShopSlot{};
        THole* Hole{};
        aGalaxyStruct::TRangerCareer Career{};
        PPlanetNewsEntry News{};
        EC_Expression::TVarArrayEC* Variables{};
        EC_Expression::TVarEC* Variable{};
        EC_Expression::TVarEC* Existing{};
        aGalaxyStruct::TDominatorSeries Series{};
        std::uint8_t Difficulty{};
        aPlanet::TPlanet* LoadedPlanet{};
        std::uint32_t SavedRandomState{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        TInterfaceStateOverride* StateOverride{};
        TInterfaceTextOverride* TextOverride{};
        TInterfaceImageOverride* ImageOverride{};
        TInterfacePosOverride* PositionOverride{};
        TInterfaceSizeOverride* SizeOverride{};
        pas::WideString TemplateName{};
        pas::WideString SavedMods{};
        TStoredItem* Stored{};
        aConst::PWeaponInfo WeaponInfo{};
        std::uint32_t Crc{};
        aGalaxyStruct::TOwnerId Race{};
        // Native no-op with an unused caller-popped static link.
        auto CompatibilityHook = [&]() -> void {
        };
        std::int32_t Stage = 0;
        try {
            if (GlobalsV::LoadedSaveVersion >= 101) {
                SavedMods = Buffer->ReadWideString();
                if (SavedMods != GR_Main::SelectedMods) {
                    GR_Main::LoadedSaveModSet = SavedMods;
                    GR_Main::AppendLogLineThreadSafe("Warning! Mod sets mismatch:"_a);
                    if (SavedMods != u"") {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u" - when this save was made you were using: ", SavedMods})));
                    } else {
                        GR_Main::AppendLogLineThreadSafe(" - when this save was made you were not using mods"_a);
                    }
                    if (GR_Main::SelectedMods != u"") {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u" - and now you are using: ", GR_Main::SelectedMods})));
                    } else {
                        GR_Main::AppendLogLineThreadSafe(" - and now you are not using any mods"_a);
                    }
                }
            }
            Self->GenerationSeed = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->RandomState = EC_Buf::TBufEC_GetUInt32(Buffer);
            SavedRandomState = Self->RandomState;
            Self->AverageRangerCapital = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->MaxRangerWealth = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->AverageRangerStrength = EC_Buf::TBufEC_GetSingle(Buffer);
            Self->BestRangerStrength = EC_Buf::TBufEC_GetSingle(Buffer);
            {
                std::uint8_t boolean = EC_Buf::TBufEC_GetBoolean(Buffer);
                GR_Main::TCCInterface* ccInterface = GR_Main::CCInterface;
                ccInterface->SetTamperDetected(boolean);
            }
            {
                std::uint8_t boolean_2 = EC_Buf::TBufEC_GetBoolean(Buffer);
                GR_Main::TCCInterface* ccInterface_2 = GR_Main::CCInterface;
                ccInterface_2->SetFlag0A(boolean_2);
            }
            if (GlobalsV::LoadedSaveVersion == 136) {
                GR_Main::CCInterface->SetFlag0A(false);
            }
            switch (GlobalsV::LoadedSaveVersion) {
                case 136:
                case 140:
                case 142: GR_Main::CCInterface->SetTamperDetected(false); break;
            }
            EC_Buf::TBufEC_GetInt32(Buffer);
            Self->SetCheatPoints(EC_Buf::TBufEC_GetInt32(Buffer));
            Self->SaveCount = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->LoadCount = EC_Buf::TBufEC_GetInt32(Buffer) + 1;
            if (GlobalsV::LoadedSaveVersion >= 127) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
            } else {
                Count = 0;
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                pas::new_value(WeaponInfo);
                pas::list_add(Self->CustomWeaponTypes, static_cast<void*>(WeaponInfo));
                WeaponInfo->ItemType = aConst::t_CustomWeapon;
                WeaponInfo->ConfigName = Buffer->ReadWideString();
                Crc = CrcUnit::InitCrc32();
                Crc = CrcUnit::UpdateCrc32(Crc, WeaponInfo->ConfigName.pchar(), WeaponInfo->ConfigName.length() * 2);
                WeaponInfo->TypeHash = CrcUnit::FinishCrc32(Crc);
                WeaponInfo->TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
                WeaponInfo->InventionIndex = static_cast<aGalaxyStruct::TPlanetInvention>(EC_Buf::TBufEC_GetByte(Buffer));
                WeaponInfo->CostFactor = EC_Buf::TBufEC_GetSingle(Buffer);
                WeaponInfo->MinDamage = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->MaxDamage = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->AverageSize = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->AverageRange = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->ShotSpeedPercent = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->MissileRange = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->MissileMaxSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->MissileMinSpeed = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->MissileChanceToBeHit = EC_Buf::TBufEC_GetByte(Buffer);
                {
                    std::uint32_t cpp_value = EC_Buf::TBufEC_GetUInt32(Buffer);
                    auto cpp_target = &WeaponInfo->DamageFlags;
                    pas::store_unaligned<std::uint32_t>(cpp_target, cpp_value);
                }
                WeaponInfo->ShotType = static_cast<aGalaxyStruct::TWeaponShotType>(EC_Buf::TBufEC_GetByte(Buffer));
                WeaponInfo->ShotCount = EC_Buf::TBufEC_GetByte(Buffer);
                if (GlobalsV::LoadedSaveVersion >= 132) {
                    WeaponInfo->AttackCount = EC_Buf::TBufEC_GetByte(Buffer);
                } else {
                    WeaponInfo->AttackCount = 1;
                }
                WeaponInfo->SecondaryDamageRadius = EC_Buf::TBufEC_GetSingle(Buffer);
                WeaponInfo->MiningFactor = EC_Buf::TBufEC_GetSingle(Buffer);
                for (J = 1; J <= 8; ++J) {
                    X = EC_Buf::TBufEC_GetSingle(Buffer);
                    pas::store_unaligned<float>(pas::byte_offset(&WeaponInfo->DamageScaleByLevel, (J - 1) * sizeof(float)), X);
                }
                if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                    WeaponInfo->PrimarySE = Buffer->ReadWideString();
                } else {
                    WeaponInfo->PrimarySE = pas::WideString();
                }
                if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                    WeaponInfo->SecondarySE = Buffer->ReadWideString();
                } else {
                    WeaponInfo->SecondarySE = pas::WideString();
                }
                if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                    WeaponInfo->AreaSE = Buffer->ReadWideString();
                } else {
                    WeaponInfo->AreaSE = pas::WideString();
                }
                WeaponInfo->DefaultPalette = EC_Buf::TBufEC_GetInt32(Buffer);
                WeaponInfo->Availability = static_cast<aGalaxyStruct::TWeaponAvailability>(EC_Buf::TBufEC_GetByte(Buffer));
                WeaponInfo->ArcadeWeaponType = static_cast<std::uint8_t>(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
            }
            Stage = 1;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 1 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Constellation = pas::construct_call<TConstellation>(TConstellation_Create);
                pas::list_add(Self->Constellations, reinterpret_cast<void*>(Constellation));
                Constellation->LoadFromBuffer(Buffer, Self);
            }
            Self->CanRecordAchievements();
            aPlanet::MainPiratePlanet = nullptr;
            Stage = 2;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 1 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
                Star = pas::construct_call<TStar>(TStar_Create);
                pas::list_add(Self->Stars, reinterpret_cast<void*>(Star));
                Star->LoadFromBuffer(Buffer, Self);
            }
            Stage = 3;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                Hole = pas::construct_call<THole>(THole_Create);
                pas::list_add(Self->Holes, reinterpret_cast<void*>(Hole));
                Hole->LoadFromBuffer(Buffer, Self);
            }
            if (GlobalsV::LoadedSaveVersion >= 122) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
                    Stored = pas::construct_call<TStoredItem>(TStoredItem_CreateEmpty);
                    pas::list_add(Self->StoredItems, reinterpret_cast<void*>(Stored));
                    Stored->LoadFromBuffer(Buffer, Self);
                }
            }
            Stage = 4;
            Self->ClearJumpGates();
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(I); ) {
                Gate = Self->CreateJumpGate(false);
                X = EC_Buf::TBufEC_GetSingle(Buffer);
                Y = EC_Buf::TBufEC_GetSingle(Buffer);
                Gate->Gate->SetPosition(EC_Struct::MakePointF(X, Y));
                {
                    std::uint8_t byte = EC_Buf::TBufEC_GetByte(Buffer);
                    SE_Space::TObjectSE* gate = Gate->Gate;
                    gate->SetAngle(byte);
                }
                TemplateIndex = EC_Buf::TBufEC_GetWord(Buffer);
                Gate->Gate->SetSize(ClassesImports::Point(TemplateIndex, TemplateIndex));
                {
                    const pas::WideString& readWideString = Buffer->ReadWideString();
                    SE_Space::TObjectSE* gate_2 = Gate->Gate;
                    gate_2->SetText(readWideString);
                }
            }
            Stage = 5;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 1 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
                void* uInt32 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                pas::List* planets = Self->Planets;
                pas::list_add(planets, uInt32);
            }
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Stars) - 1); cpp_range_8.next(I); ) {
                Star = pas::list_at<TStar>(Self->Stars, I);
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_9.next(J); ) {
                    LoadedPlanet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                    if (LoadedPlanet->Id <= static_cast<std::uint32_t>(Count) && static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Self->Planets, static_cast<std::int32_t>(LoadedPlanet->Id - 1)))) == LoadedPlanet->Id) {
                        pas::list_put(Self->Planets, static_cast<std::int32_t>(LoadedPlanet->Id - 1), reinterpret_cast<void*>(LoadedPlanet));
                    } else {
                        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Planets) - 1); cpp_range_10.next(K); ) {
                            if (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Self->Planets, K))) == LoadedPlanet->Id) {
                                pas::list_put(Self->Planets, K, reinterpret_cast<void*>(LoadedPlanet));
                                break;
                            }
                        }
                    }
                }
            }
            Stage = 6;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 1 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(I); ) {
                void* uInt32_2 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                pas::List* rangers = Self->Rangers;
                pas::list_add(rangers, uInt32_2);
            }
            if (GlobalsV::LoadedSaveVersion >= 133) {
                for (auto cpp_range_12 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range_12.next(Race); ) {
                    Self->RangerSpawnQuotas[Race] = EC_Buf::TBufEC_GetInt32(Buffer);
                }
            }
            if (GlobalsV::LoadedSaveVersion < 102) {
                Stage = 7;
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_13.next(I); ) {
                    EC_Buf::TBufEC_GetUInt32(Buffer);
                }
            }
            Self->KellerTargetStar = reinterpret_cast<TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            Self->KellerMissionState = EC_Buf::TBufEC_GetInt32(Buffer);
            Stage = 8;
            fEquipmentShop::ClearTemporaryShopSlotGrid();
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count > 0) {
                fEquipmentShop::TemporaryShopSlots = pas::make_object<pas::List>();
                for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_14.next(I); ) {
                    ShopSlot = pas::construct_call<fEquipmentShop::TShopSlot>(fEquipmentShop::TShopSlot_Create);
                    pas::list_add(fEquipmentShop::TemporaryShopSlots, reinterpret_cast<void*>(ShopSlot));
                    ShopSlot->LoadFromBuffer(Buffer, Self);
                }
            }
            Stage = 9;
            Variables = pas::construct_call<EC_Expression::TVarArrayEC>(EC_Expression::TVarArrayEC_Create);
            Variables->LoadFromBuffer(Buffer);
            for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, Variables->Count - 1); cpp_range_15.next(I); ) {
                Variable = EC_Expression::TVarArrayEC_GetItem(Variables, I);
                Existing = Globals::SharedScriptVariables->GetVarNE(Variable->Name);
                if (Existing != nullptr) {
                    Existing->AssignFrom(Variable, true);
                }
            }
            pas::free(Variables);
            Stage = 10;
            Count = pas::list_count(Globals::ScriptTemplates);
            for (auto cpp_range_16 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_16.next(I); ) {
                Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, I);
                Template->UseCount = 0;
                Template->LastTurn = 0;
            }
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_17 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_17.next(I); ) {
                TemplateName = Buffer->ReadWideString();
                TemplateIndex = Globals::FindScriptTemplateIndex(pas::view(TemplateName));
                if (TemplateIndex < 0) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning: Script not found - ", TemplateName})));
                    EC_Buf::TBufEC_GetWord(Buffer);
                    EC_Buf::TBufEC_GetInt32(Buffer);
                    EC_Buf::TBufEC_GetInt32(Buffer);
                } else {
                    Template = pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, TemplateIndex);
                    Template->UseCount = EC_Buf::TBufEC_GetWord(Buffer);
                    Template->LastTurn = EC_Buf::TBufEC_GetInt32(Buffer);
                    Template->ActiveScriptIndex = EC_Buf::TBufEC_GetInt32(Buffer);
                }
            }
            Stage = 11;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_18.next(I); ) {
                Script = pas::construct_call<aScript::TScript>(aScript::TScript_Create);
                pas::list_add(Self->Scripts, reinterpret_cast<void*>(Script));
                Script->LoadState(Buffer, Self);
                if (GlobalsV::LoadedSaveVersion == 146 && Script->ScriptFileName == u"Script.PC_part7") {
                    if (Script->InitCode->LocalVar->GetVar(u"player_traitor"_wref.get())->GetInt() == 1) {
                        Script->InitCode->LocalVar->GetVar(u"pirates_killed_init"_wref.get())->SetInt(1);
                    }
                }
            }
            Stage = 12;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_19.next(I); ) {
                Group = pas::construct_call<aGroup::TGroup>(aGroup::TGroup_Create);
                pas::list_add(Self->LiberationGroups, reinterpret_cast<void*>(Group));
                Group->Load(Buffer, Self);
            }
            Stage = 13;
            EC_Buf::TBufEC_GetByte(Buffer);
            EC_Buf::TBufEC_GetInt32(Buffer);
            Self->PirateCount = EC_Buf::TBufEC_GetWord(Buffer);
            Self->PirateClanCount = EC_Buf::TBufEC_GetWord(Buffer);
            Self->TransportCount = EC_Buf::TBufEC_GetWord(Buffer);
            Self->CurrentTurn = EC_Buf::TBufEC_GetUInt32(Buffer);
            for (Difficulty = static_cast<std::uint8_t>(0); Difficulty <= static_cast<std::uint8_t>(7); ++Difficulty) {
                Self->DifficultyLevels[Difficulty] = EC_Buf::TBufEC_GetByte(Buffer);
            }
            Self->PlayerRangerIndex = EC_Buf::TBufEC_GetUInt32(Buffer);
            aRanger::PendingPlayerFollowTarget = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            aKling::BlazerShip = reinterpret_cast<aKling::TKling*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            aKling::KellerShip = reinterpret_cast<aKling::TKling*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            aKling::TerronShip = reinterpret_cast<aKling::TKling*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            PlayerStar = reinterpret_cast<TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            aKling::PieceCreatorTargetStarId = EC_Buf::TBufEC_GetUInt32(Buffer);
            Stage = 14;
            for (auto cpp_range_20 = pas::for_to<aGalaxyStruct::TRangerCareer>(aGalaxyStruct::rcTrader, aGalaxyStruct::rcWarrior); cpp_range_20.next(Career); ) {
                Self->EminentCareerShips[Career] = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            }
            Stage = 15;
            Count = pas::list_count(Self->Rangers);
            for (auto cpp_range_21 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_21.next(I); ) {
                pas::list_put(Self->Rangers, I, reinterpret_cast<void*>(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Self->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Self->Rangers, I))), true)))));
            }
            Stage = 16;
            aPlayer::SetPlayer(pas::checked_cast<aPlayer::TPlayer*>(static_cast<pas::Object*>(Self->IdToShip(Self->PlayerRangerIndex, true))), Self);
            Stage = 17;
            Count = pas::list_count(Self->Constellations);
            for (auto cpp_range_22 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_22.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Self->Constellations, I);
                Constellation->ResolveLoadedReferences(Self);
            }
            Stage = 18;
            J = 4;
            Count = pas::list_count(Self->Stars);
            for (auto cpp_range_23 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_23.next(I); ) {
                Star = pas::list_at<TStar>(Self->Stars, I);
                Star->ResolveLoadedReferences(Self);
            }
            if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                if (aPlayer::GetPlayer()->CurrentPlanet != nullptr) {
                    fEquipmentShop::TemporaryShopPlanet = aPlayer::GetPlayer()->CurrentPlanet;
                } else if (aPlayer::GetPlayer()->DockedTo != nullptr && pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo) != nullptr) {
                    fEquipmentShop::TemporaryShopStation = reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo);
                } else {
                    fEquipmentShop::ClearTemporaryShopSlotGrid();
                }
            }
            Count = pas::list_count(Self->StoredItems);
            for (auto cpp_range_24 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_24.next(I); ) {
                Stored = pas::list_at<TStoredItem>(Self->StoredItems, I);
                reinterpret_cast<aItem::TItem*>(Stored->Item)->ResolveLoadedReferences(Self);
            }
            Stage = 19;
            Count = pas::list_count(Self->Holes);
            for (auto cpp_range_25 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_25.next(I); ) {
                Hole = pas::list_at<THole>(Self->Holes, I);
                Hole->ResolveLoadedReferences(Self);
            }
            Stage = 22;
            if (Self->KellerTargetStar != nullptr) {
                Self->KellerTargetStar = pas::checked_cast<TStar*>(static_cast<pas::Object*>(Self->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->KellerTargetStar)))));
            }
            Stage = 23;
            aRanger::PendingPlayerFollowTarget = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Self->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(aRanger::PendingPlayerFollowTarget)), true)));
            aKling::BlazerShip = pas::checked_cast<aKling::TKling*>(static_cast<pas::Object*>(Self->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(aKling::BlazerShip)), true)));
            aKling::KellerShip = pas::checked_cast<aKling::TKling*>(static_cast<pas::Object*>(Self->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(aKling::KellerShip)), true)));
            aKling::TerronShip = pas::checked_cast<aKling::TKling*>(static_cast<pas::Object*>(Self->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(aKling::TerronShip)), true)));
            PlayerStar = pas::checked_cast<TStar*>(static_cast<pas::Object*>(Self->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PlayerStar)))));
            Stage = 24;
            for (auto cpp_range_26 = pas::for_to<aGalaxyStruct::TRangerCareer>(aGalaxyStruct::rcTrader, aGalaxyStruct::rcWarrior); cpp_range_26.next(Career); ) {
                Self->EminentCareerShips[Career] = pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Self->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->EminentCareerShips[Career])), true)));
            }
            Stage = 25;
            if (aRanger::PlayerOldQuests != nullptr) {
                pas::free(aRanger::PlayerOldQuests);
                aRanger::PlayerOldQuests = nullptr;
            }
            aRanger::PlayerOldQuests = pas::make_object<pas::List>();
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            Stage = 26;
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err in PlayerQuests load"_a));
            }
            for (auto cpp_range_27 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_27.next(I); ) {
                pas::new_value(OldQuest);
                pas::list_add(aRanger::PlayerOldQuests, static_cast<void*>(OldQuest));
                OldQuest->Planet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                OldQuest->Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Self->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(OldQuest->Planet)), true)));
                OldQuest->QuestType = static_cast<aGalaxyStruct::TQuestType>(EC_Buf::TBufEC_GetByte(Buffer));
                OldQuest->QuestNumber = EC_Buf::TBufEC_GetWord(Buffer);
                OldQuest->Description = Buffer->ReadWideString();
                OldQuest->Successful = EC_Buf::TBufEC_GetBoolean(Buffer);
                OldQuest->Declined = EC_Buf::TBufEC_GetBoolean(Buffer);
            }
            Stage = 27;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_28 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_28.next(I); ) {
                pas::new_value(News);
                pas::list_add(Self->PlanetNews, static_cast<void*>(News));
                News->Id = EC_Buf::TBufEC_GetUInt32(Buffer);
                News->Turn = EC_Buf::TBufEC_GetUInt32(Buffer);
                News->NewsType = static_cast<aGalaxyStruct::TGalaxyNewsKind>(EC_Buf::TBufEC_GetByte(Buffer));
                News->Text = Buffer->ReadWideString();
            }
            Stage = 28;
            ReservedMessageCounter = EC_Buf::TBufEC_GetUInt32(Buffer);
            TurnsSinceLastShipMessage = EC_Buf::TBufEC_GetUInt32(Buffer);
            Self->ChecksumScalarD0 = EC_Buf::TBufEC_GetSingle(Buffer);
            for (auto cpp_range_29 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_29.next(Series); ) {
                Self->DominatorResearch[Series].Progress = EC_Buf::TBufEC_GetSingle(Buffer);
                Self->DominatorResearch[Series].Material = EC_Buf::TBufEC_GetUInt32(Buffer);
            }
            Self->ChecksumScalarEC = EC_Buf::TBufEC_GetSingle(Buffer);
            Stage = 29;
            if (GlobalsV::LoadedSaveVersion >= 62) {
                Self->WarDeltaWin[1] = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->WarDeltaWin[2] = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->WarDeltaWin[0] = EC_Buf::TBufEC_GetInt32(Buffer);
            } else {
                Self->WarDeltaWin[1] = EC_Buf::TBufEC_GetInt32(Buffer) * -1;
                Self->WarDeltaWin[2] = 0;
                Self->WarDeltaWin[0] = 0;
            }
            Stage = 30;
            Buffer->ReadLengthPrefixedBuffer(GR_Main::CCInterface->Buffer);
            GR_Main::CCInterface->Buffer->Clear();
            Stage = 31;
            Count = EC_Buf::TBufEC_GetInt32(Buffer);
            if (Count < 0 || Count > 1000000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            Self->SpaceBackgroundEntries.set_length(Count);
            {
                const std::int32_t cpp_last = Self->SpaceBackgroundEntries.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Self->SpaceBackgroundEntries[I].ImageIndex = EC_Buf::TBufEC_GetInt32(Buffer);
                        Self->SpaceBackgroundEntries[I].OrbitCenter.X = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].OrbitCenter.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].OrbitCenter.Z = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].Position.Z = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].Unknown38.X = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].Unknown38.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].Unknown38.Z = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].OrbitStepDegrees = EC_Buf::TBufEC_GetSingle(Buffer);
                        Self->SpaceBackgroundEntries[I].FrameIndex = EC_Buf::TBufEC_GetInt32(Buffer);
                    }
                }
            }
            Stage = 32;
            for (I = 0; I <= 8; ++I) {
                Globals::HangarScreen->ShipSlots[I].ShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
            }
            Stage = 33;
            {
                std::int32_t int32 = EC_Buf::TBufEC_GetInt32(Buffer);
                GR_Main::TCCInterface* ccInterface_3 = GR_Main::CCInterface;
                ccInterface_3->SetValue10(int32);
            }
            {
                std::int32_t int32_2 = EC_Buf::TBufEC_GetInt32(Buffer);
                GR_Main::TCCInterface* ccInterface_4 = GR_Main::CCInterface;
                ccInterface_4->SetIntegrityStatus(int32_2);
            }
            {
                std::int32_t int32_3 = EC_Buf::TBufEC_GetInt32(Buffer);
                GR_Main::TCCInterface* ccInterface_5 = GR_Main::CCInterface;
                ccInterface_5->SetIntegrityError(int32_3);
            }
            {
                std::uint32_t int32_4 = EC_Buf::TBufEC_GetInt32(Buffer);
                GR_Main::TCCInterface* ccInterface_6 = GR_Main::CCInterface;
                ccInterface_6->SetIntegrityChecksum(int32_4);
            }
            Stage = 34;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_30 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_30.next(I); ) {
                void* idToShip = Self->IdToShip(EC_Buf::TBufEC_GetUInt32(Buffer), true);
                pas::List* shipsInTransit = Self->ShipsInTransit;
                pas::list_add(shipsInTransit, idToShip);
            }
            Stage = 35;
            Self->TerronWeaponLockTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->TerronGrowLockTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->TerronLandingLockTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->TerronToStarTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->KellerLeaveTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 47) {
                Self->KellerResearchTargetStarId = EC_Buf::TBufEC_GetUInt32(Buffer);
            }
            Self->BlazerLandingPlanetId = EC_Buf::TBufEC_GetUInt32(Buffer);
            Self->BlazerSelfDestructTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->TerronSeriesResolvedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->KellerSeriesResolvedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->BlazerSeriesResolvedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->PirateWinTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->PirateWinType = EC_Buf::TBufEC_GetInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 46) {
                Self->CoalitionDefeatedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            }
            Self->GraphDominatorSurfacesEnabled = EC_Buf::TBufEC_GetBoolean(Buffer);
            Self->SpaceEffectKind = EC_Buf::TBufEC_GetByte(Buffer);
            Self->IronWill = EC_Buf::TBufEC_GetBoolean(Buffer);
            Stage = 36;
            if (GlobalsV::LoadedSaveVersion >= 71) {
                Self->DominatorModLevel = EC_Buf::TBufEC_GetByte(Buffer);
                Self->TechnicModEnabled = EC_Buf::TBufEC_GetByte(Buffer);
                Self->AmmoModEnabled = EC_Buf::TBufEC_GetByte(Buffer);
                Self->GodModEnabled = EC_Buf::TBufEC_GetByte(Buffer);
                Self->UltraScanModEnabled = EC_Buf::TBufEC_GetByte(Buffer);
                Self->StasisModEnabled = EC_Buf::TBufEC_GetByte(Buffer);
            } else {
                Self->DominatorModLevel = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->TechnicModEnabled = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->AmmoModEnabled = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->GodModEnabled = EC_Buf::TBufEC_GetInt32(Buffer);
                if (GlobalsV::LoadedSaveVersion >= 65) {
                    Self->UltraScanModEnabled = EC_Buf::TBufEC_GetInt32(Buffer);
                } else {
                    Self->UltraScanModEnabled = 0;
                }
                Self->StasisModEnabled = 0;
            }
            Stage = 37;
            Self->NextPlanetNewsId = EC_Buf::TBufEC_GetUInt32(Buffer);
            Self->NextSpecialStationServiceTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Stage = 38;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_31 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_31.next(I); ) {
                Event = pas::construct_call<aGalaxyEvent::TGalaxyEvent>(aGalaxyEvent::TGalaxyEvent_Create, pas::WideString());
                pas::list_add(Self->GalaxyEvents, reinterpret_cast<void*>(Event));
                Event->LoadFromBuffer(Buffer);
            }
            if (GlobalsV::LoadedSaveVersion >= 112) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_32 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_32.next(I); ) {
                    StateOverride = pas::construct_call<TInterfaceStateOverride>(TInterfaceStateOverride_Create);
                    pas::list_add(Self->InterfaceStateOverrides, reinterpret_cast<void*>(StateOverride));
                    StateOverride->LoadFromBuffer(Buffer);
                }
            }
            if (GlobalsV::LoadedSaveVersion >= 117) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_33 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_33.next(I); ) {
                    TextOverride = pas::construct_call<TInterfaceTextOverride>(TInterfaceTextOverride_Create);
                    pas::list_add(Self->InterfaceTextOverrides, reinterpret_cast<void*>(TextOverride));
                    TextOverride->LoadFromBuffer(Buffer);
                }
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_34 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_34.next(I); ) {
                    ImageOverride = pas::construct_call<TInterfaceImageOverride>(TInterfaceImageOverride_Create);
                    pas::list_add(Self->InterfaceImageOverrides, reinterpret_cast<void*>(ImageOverride));
                    ImageOverride->LoadFromBuffer(Buffer);
                }
            }
            if (GlobalsV::LoadedSaveVersion >= 119) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_35 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_35.next(I); ) {
                    PositionOverride = pas::construct_call<TInterfacePosOverride>(TInterfacePosOverride_Create);
                    pas::list_add(Self->InterfacePositionOverrides, reinterpret_cast<void*>(PositionOverride));
                    PositionOverride->LoadFromBuffer(Buffer);
                }
            }
            if (GlobalsV::LoadedSaveVersion >= 134) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                    pas::raise(pas::make_exception<pas::Abort>("Err"_a));
                }
                for (auto cpp_range_36 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_36.next(I); ) {
                    SizeOverride = pas::construct_call<TInterfaceSizeOverride>(TInterfaceSizeOverride_Create);
                    pas::list_add(Self->InterfaceSizeOverrides, reinterpret_cast<void*>(SizeOverride));
                    SizeOverride->LoadFromBuffer(Buffer);
                }
            }
            Stage = 39;
            if (GlobalsV::LoadedSaveVersion >= 70) {
                Self->NextShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
                Self->NextItemId = EC_Buf::TBufEC_GetUInt32(Buffer);
            }
            Stage = 40;
            if (GlobalsV::LoadedSaveVersion >= 56) {
                Self->GenerationMachineHash = EC_Buf::TBufEC_GetUInt32(Buffer);
            } else {
                Self->GenerationMachineHash = GR_Main::ComputeMachineFingerprintCRC();
            }
            Stage = 41;
            Count = pas::list_count(Self->Scripts);
            for (auto cpp_range_37 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_37.next(I); ) {
                Script = pas::list_at<aScript::TScript>(Self->Scripts, I);
                Script->ResolveLoadedReferences(Self);
            }
            Stage = 42;
            Count = pas::list_count(Self->LiberationGroups);
            for (auto cpp_range_38 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_38.next(I); ) {
                Group = pas::list_at<aGroup::TGroup>(Self->LiberationGroups, I);
                Group->ResolveLoadedReferences(Self);
            }
            Stage = 43;
            Count = pas::list_count(Self->Stars);
            for (auto cpp_range_39 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_39.next(I); ) {
                Star = pas::list_at<TStar>(Self->Stars, I);
                ShipCount = pas::list_count(Star->Ships);
                for (auto cpp_range_40 = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range_40.next(TemplateIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, TemplateIndex);
                    if (Ship->CurrentPlanet != nullptr && Ship->CurrentPlanet->CurrentStar != Star) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning! ", Ship->GetFullName(u" "_wref.get()), u" is in system ", Star->Name, u" while landed on planet ", Ship->CurrentPlanet->Name})));
                        GR_Main::AppendLogLineThreadSafe("clearing landing state"_a);
                        Ship->CurrentPlanet = nullptr;
                    }
                    if (Ship->TypeId == aGalaxyStruct::stWarrior) {
                        pas::list_add(Ship->HomePlanet->Warriors, reinterpret_cast<void*>(Ship));
                    }
                    if (Ship->TypeId == aGalaxyStruct::stRanger && pas::list_indexof(Self->Rangers, reinterpret_cast<void*>(Ship)) < 0) {
                        pas::checked_cast<aRanger::TRanger*>(Ship)->RegisterInGalaxyRelations();
                    }
                }
            }
            if (GlobalsV::LoadedSaveVersion < 121 && aPlanet::MainPiratePlanet != nullptr) {
                for (auto cpp_range_41 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Stars) - 1); cpp_range_41.next(I); ) {
                    Star = pas::list_at<TStar>(Self->Stars, I);
                    for (auto cpp_range_42 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_42.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                        if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship); pirate != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate && pirate->PirateType != 0) {
                            for (auto cpp_range_43 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Rangers) - 1); cpp_range_43.next(K); ) {
                                pas::list_put(Ship->RangerRelations, K, pas::list_get(aPlanet::MainPiratePlanet->RangerRelations, K));
                            }
                        }
                    }
                }
            }
            Stage = 44;
            Self->ComputeGlobalGoodsPriceBands();
            Self->InitializeConstellationDistanceTiers();
            Self->RebuildStarDistances();
            Self->UpdateConstellationMilitaryStats();
            Self->RefreshRangerWealthStats();
            Self->RefreshRangerRatingPlaces();
            Self->RefreshTechLevel();
            Stage = 45;
            if (aKling::BlazerShip != nullptr) {
                TGalaxy::CreateDominatorSpawnProxy(aKling::BlazerShip->CurrentStar);
            } else if (aKling::TerronShip != nullptr) {
                TGalaxy::CreateDominatorSpawnProxy(aKling::TerronShip->CurrentStar);
            } else if (aKling::KellerShip != nullptr) {
                TGalaxy::CreateDominatorSpawnProxy(aKling::KellerShip->CurrentStar);
            } else {
                TGalaxy::CreateDominatorSpawnProxy(nullptr);
            }
            Stage = 46;
            for (auto cpp_range_44 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Stars) - 1); cpp_range_44.next(I); ) {
                Star = pas::list_at<TStar>(Self->Stars, I);
                Star->MapDiameter = Star->ComputeMapDiameter();
            }
            Stage = 47;
            if ((Self->TerronToStarTurn & aGalaxyStruct::TerronTransformationFlag) != 0 && aKling::TerronShip != nullptr) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&aKling::TerronShip->CurrentStar->Graphic));
                {
                    SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Star"sv, u"Star.TerronAfter"_wref.get(), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&aKling::TerronShip->CurrentStar->Graphic);
                    SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
                }
            }
            Stage = 48;
            aPlayer::GetPlayer()->CurrentStar->RefreshMovementStepParameters();
            Self->RandomState = SavedRandomState;
            Stage = 49;
            if (GlobalsV::LoadedSaveVersion >= 61) {
                {
                    std::uint8_t boolean_3 = EC_Buf::TBufEC_GetBoolean(Buffer);
                    GR_Main::TCCInterface* ccInterface_7 = GR_Main::CCInterface;
                    ccInterface_7->SetEditableStateApplied(boolean_3);
                }
                Self->FinalizationNameEncoded = Buffer->ReadWideString();
            } else {
                GR_Main::CCInterface->SetEditableStateApplied(false);
                Self->FinalizationNameEncoded = pas::WideString();
            }
            Stage = 50;
            if (GlobalsV::LoadedSaveVersion >= 63) {
                Self->CustomRules.Enabled = EC_Buf::TBufEC_GetBoolean(Buffer);
                Self->CustomRules.DominatorStrength = EC_Buf::TBufEC_GetByte(Buffer);
                Self->CustomRules.DominatorAggression = EC_Buf::TBufEC_GetByte(Buffer);
                Self->CustomRules.DominatorSpawn = EC_Buf::TBufEC_GetByte(Buffer);
                Self->CustomRules.PirateAggression = EC_Buf::TBufEC_GetByte(Buffer);
                Self->CustomRules.CoalitionAggression = EC_Buf::TBufEC_GetByte(Buffer);
                Self->CustomRules.AsteroidModifier = EC_Buf::TBufEC_GetByte(Buffer);
                Self->CustomRules.SunDamageModifier = EC_Buf::TBufEC_GetByte(Buffer);
                if (GlobalsV::LoadedSaveVersion >= 64) {
                    Self->CustomRules.ExtraInventions = EC_Buf::TBufEC_GetByte(Buffer);
                } else {
                    Self->CustomRules.ExtraInventions = 0;
                }
                if (GlobalsV::LoadedSaveVersion >= 64) {
                    Self->CustomRules.AcrynModifier = EC_Buf::TBufEC_GetByte(Buffer);
                } else {
                    Self->CustomRules.AcrynModifier = 16;
                }
                if (GlobalsV::LoadedSaveVersion >= 66) {
                    Self->CustomRules.NodeDropModifier = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.ArcadeDropValueModifier = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.DropValueModifier = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.AgriculturalPlanetWeight = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.MixedPlanetWeight = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.IndustrialPlanetWeight = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.ExtraRangers = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.ArcadeHitpointsModifier = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.ArcadeDamageModifier = EC_Buf::TBufEC_GetByte(Buffer);
                    Self->CustomRules.AIJunkTolerance = EC_Buf::TBufEC_GetByte(Buffer);
                } else {
                    Self->CustomRules.NodeDropModifier = 8;
                    Self->CustomRules.ArcadeDropValueModifier = 8;
                    Self->CustomRules.DropValueModifier = 8;
                    Self->CustomRules.AgriculturalPlanetWeight = 1;
                    Self->CustomRules.MixedPlanetWeight = 1;
                    Self->CustomRules.IndustrialPlanetWeight = 1;
                    Self->CustomRules.ExtraRangers = 0;
                    Self->CustomRules.ArcadeHitpointsModifier = 8;
                    Self->CustomRules.ArcadeDamageModifier = 8;
                    Self->CustomRules.AIJunkTolerance = 7;
                }
                Self->CustomRules.ChaoticRandom = EC_Buf::TBufEC_GetBoolean(Buffer);
                Self->CustomRules.UnrestrictedEquipmentKnowledge = EC_Buf::TBufEC_GetBoolean(Buffer);
                Self->CustomRules.StationsNearStars = EC_Buf::TBufEC_GetBoolean(Buffer);
                Self->CustomRules.FullStationTargeting = EC_Buf::TBufEC_GetBoolean(Buffer);
                if (GlobalsV::LoadedSaveVersion >= 64) {
                    Self->CustomRules.SpecialShips = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.SpecialShips = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 66) {
                    Self->CustomRules.ZeroStartingExperience = EC_Buf::TBufEC_GetBoolean(Buffer);
                    if (GlobalsV::LoadedSaveVersion < 92) {
                        EC_Buf::TBufEC_GetBoolean(Buffer);
                    }
                    Self->CustomRules.ArcadeBattleRoyale = EC_Buf::TBufEC_GetBoolean(Buffer);
                    Self->CustomRules.DominatorRacialWeapons = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.ZeroStartingExperience = false;
                    Self->CustomRules.ArcadeBattleRoyale = false;
                    Self->CustomRules.DominatorRacialWeapons = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 72) {
                    Self->CustomRules.StartInCenter = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.StartInCenter = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 73) {
                    Self->CustomRules.MaxRangeMissiles = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.MaxRangeMissiles = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 75) {
                    Self->CustomRules.OldHyperspace = EC_Buf::TBufEC_GetBoolean(Buffer);
                    Self->CustomRules.PirateNodes = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.OldHyperspace = false;
                    Self->CustomRules.PirateNodes = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 84) {
                    Self->CustomRules.AIUseShops = EC_Buf::TBufEC_GetBoolean(Buffer);
                    Self->CustomRules.StationsUseShop = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.AIUseShops = false;
                    Self->CustomRules.StationsUseShop = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 136) {
                    Self->CustomRules.DuplicateArtefacts = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.DuplicateArtefacts = false;
                }
                if (GlobalsV::LoadedSaveVersion >= 156) {
                    Self->CustomRules.HullGrowth = EC_Buf::TBufEC_GetByte(Buffer);
                } else {
                    Self->CustomRules.HullGrowth = 0;
                }
                if (GlobalsV::LoadedSaveVersion >= 166) {
                    Self->CustomRules.ArcadeEquipmentChange = EC_Buf::TBufEC_GetBoolean(Buffer);
                    Self->CustomRules.OldSpeedCalculation = EC_Buf::TBufEC_GetBoolean(Buffer);
                    Self->CustomRules.OldMissileBonuses = EC_Buf::TBufEC_GetBoolean(Buffer);
                    EC_Buf::TBufEC_GetBoolean(Buffer);
                    EC_Buf::TBufEC_GetBoolean(Buffer);
                    EC_Buf::TBufEC_GetBoolean(Buffer);
                    EC_Buf::TBufEC_GetBoolean(Buffer);
                    EC_Buf::TBufEC_GetBoolean(Buffer);
                } else {
                    Self->CustomRules.ArcadeEquipmentChange = false;
                    Self->CustomRules.OldSpeedCalculation = false;
                    Self->CustomRules.OldMissileBonuses = false;
                }
            } else {
                Self->CustomRules.Enabled = true;
                switch (Self->DifficultyLevels[0]) {
                    case 0: Self->CustomRules.DominatorStrength = 0; break;
                    case 1: Self->CustomRules.DominatorStrength = 8; break;
                    case 2: Self->CustomRules.DominatorStrength = 16; break;
                    case 3: Self->CustomRules.DominatorStrength = 24; break;
                }
                Self->CustomRules.DominatorAggression = Self->CustomRules.DominatorStrength;
                Self->CustomRules.DominatorSpawn = Self->CustomRules.DominatorStrength;
                Self->CustomRules.PirateAggression = Self->CustomRules.DominatorStrength;
                Self->CustomRules.CoalitionAggression = 8;
                Self->CustomRules.AsteroidModifier = 8;
                Self->CustomRules.SunDamageModifier = 8;
                Self->CustomRules.ExtraInventions = 0;
                Self->CustomRules.AcrynModifier = 16;
                Self->CustomRules.NodeDropModifier = 8;
                Self->CustomRules.ArcadeDropValueModifier = 8;
                Self->CustomRules.DropValueModifier = 8;
                Self->CustomRules.AgriculturalPlanetWeight = 1;
                Self->CustomRules.MixedPlanetWeight = 1;
                Self->CustomRules.IndustrialPlanetWeight = 1;
                Self->CustomRules.ExtraRangers = 0;
                Self->CustomRules.ArcadeHitpointsModifier = 8;
                Self->CustomRules.ArcadeDamageModifier = 8;
                Self->CustomRules.AIJunkTolerance = 7;
                Self->CustomRules.ChaoticRandom = false;
                Self->CustomRules.UnrestrictedEquipmentKnowledge = false;
                Self->CustomRules.StationsNearStars = false;
                Self->CustomRules.FullStationTargeting = false;
                Self->CustomRules.SpecialShips = false;
                Self->CustomRules.ZeroStartingExperience = false;
                Self->CustomRules.ArcadeBattleRoyale = false;
                Self->CustomRules.DominatorRacialWeapons = false;
                Self->CustomRules.StartInCenter = false;
                Self->CustomRules.MaxRangeMissiles = false;
                Self->CustomRules.OldHyperspace = false;
                Self->CustomRules.PirateNodes = false;
                Self->CustomRules.AIUseShops = false;
                Self->CustomRules.StationsUseShop = false;
                Self->CustomRules.DuplicateArtefacts = false;
                Self->CustomRules.HullGrowth = 0;
                Self->CustomRules.ArcadeEquipmentChange = false;
                Self->CustomRules.OldSpeedCalculation = false;
                Self->CustomRules.OldMissileBonuses = false;
            }
            Stage = 51;
            CompatibilityHook();
            Event = aGalaxyEvent::AddGalaxyEvent(u"SaveLoaded"_w, Self);
            ShipCount = EC_Str::CountDelimitedPartsW(pas::view(GR_Main::SelectedMods), u","sv);
            for (auto cpp_range_45 = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range_45.next(I); ) {
                Event->AddTextData(EC_Str::ExtractDelimitedPartW(pas::view(GR_Main::SelectedMods), I, u","sv));
            }
            Event->AddData(GR_Main::ApplyEditableSaveOnLoad);
            Stage = 52;
            Galaxy = Self;
            for (auto cpp_range_46 = pas::for_to<std::int32_t>(0, pas::list_count(Self->LoadedShips) - 1); cpp_range_46.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->LoadedShips, I);
                Ship->RefreshDerivedStats(false);
                Ship->RefreshGraphicSize();
                aShip::TShip::DerivedStateCompatibilityHook();
            }
            Self->RefreshRangerStrengthStats();
            for (auto cpp_range_47 = pas::for_to<std::int32_t>(0, pas::list_count(Self->LoadedShips) - 1); cpp_range_47.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Self->LoadedShips, I);
                Ship->UpdateBestRangerRelativeRatings();
                Ship->UpdateAverageRangerRelativeStrength();
            }
            pas::list_clear(Self->LoadedShips);
            aPlayer::GetPlayer()->RefreshStorageBubbles();
            Self->PrimeIntegrityChecksum(101);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure ", EC_Str::DecodeTextW(u"TAGSallbanxuy..MLFovasdi"_w), u", label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    // Replaces the shared editable-save block, including player, holes and stars. Requires a player.
    void TGalaxy::SaveEditableState() {
        std::int32_t i{};
        pas::WideString Name{};
        TStar* Star{};
        THole* Hole{};
        GR_Main::EditableSaveBlock->Clear();
        {
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(FinalizationNameEncoded);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"FsignsarltiyzaazthikoEnoNiaemaex"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_2, decodeTextW);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(CurrentTurn);
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"ImDeary"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_3, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[0]));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"DeieffPhizroantle"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_4, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[1]));
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"DuiefsTvrnaSdlej"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_5, intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[2]));
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"DpiffeSscvn"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_6, intToStr_4);
        }
        {
            const pas::WideString& intToStr_5 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[3]));
            const pas::WideString& decodeTextW_7 = EC_Str::DecodeTextW(u"DqirfsRfejpyariSra"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_7, intToStr_5);
        }
        {
            const pas::WideString& intToStr_6 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[4]));
            const pas::WideString& decodeTextW_8 = EC_Str::DecodeTextW(u"DoitfdTgeecthv"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_8, intToStr_6);
        }
        {
            const pas::WideString& intToStr_7 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[5]));
            const pas::WideString& decodeTextW_9 = EC_Str::DecodeTextW(u"DpiFfsQvueeYsst"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_9, intToStr_7);
        }
        {
            const pas::WideString& intToStr_8 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[6]));
            const pas::WideString& decodeTextW_10 = EC_Str::DecodeTextW(u"DpiwfsHrojlee"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_10, intToStr_8);
        }
        {
            const pas::WideString& intToStr_9 = pas::wide_int_to_str(static_cast<std::int32_t>(DifficultyLevels[7]));
            const pas::WideString& decodeTextW_11 = EC_Str::DecodeTextW(u"DpiefdBkarlsaGndcVee"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_11, intToStr_9);
        }
        {
            const pas::WideString& intToStr_10 = pas::wide_int_to_str(WarDeltaWin[1]);
            const pas::WideString& decodeTextW_12 = EC_Str::DecodeTextW(u"KvlsiRnsgTshDdeHljtoaRWdifnG"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_12, intToStr_10);
        }
        {
            const pas::WideString& intToStr_11 = pas::wide_int_to_str(WarDeltaWin[2]);
            const pas::WideString& decodeTextW_13 = EC_Str::DecodeTextW(u"PuirreastTerswDVesltt4a6WHidns"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_13, intToStr_11);
        }
        {
            const pas::WideString& intToStr_12 = pas::wide_int_to_str(WarDeltaWin[0]);
            const pas::WideString& decodeTextW_14 = EC_Str::DecodeTextW(u"Npo6rdm2aSlfsHDeeyljt4asWCignI"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_14, intToStr_12);
        }
        {
            const pas::WideString& boolToWideString = EC_Str::BoolToWideString(aPlayer::GetPlayer()->DeclinePlanetBattleOffers);
            const pas::WideString& decodeTextW_15 = EC_Str::DecodeTextW(u"RlehjieScataPSB"_w);
            GR_Main::EditableSaveBlock->AddParam(decodeTextW_15, boolToWideString);
        }
        {
            EC_BlockPar::TBlockParEC* addBlockByPath = GR_Main::EditableSaveBlock->AddBlockByPath(EC_Str::DecodeTextW(u"Polearymeir"_w));
            aPlayer::TPlayer* player = aPlayer::GetPlayer();
            player->SaveToBlock(addBlockByPath);
        }
        EC_BlockPar::TBlockParEC* HoleBlock = GR_Main::EditableSaveBlock->AddBlockByPath(EC_Str::DecodeTextW(u"HroslaenLfirs4t"_w));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Holes) - 1); cpp_range.next(i); ) {
            Hole = pas::list_at<THole>(Holes, i);
            Name = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(Hole->Id)), EC_Str::DecodeTextW(u"Heo4lge5I6dY"_w)});
            Hole->SaveToBlock(HoleBlock->AddBlockByPath(Name));
        }
        {
            const pas::WideString& intToStr_13 = pas::wide_int_to_str(0);
            const pas::WideString& decodeTextW_16 = EC_Str::DecodeTextW(u"CEr2e4aftge4NgehwYHeohlsegs1"_w);
            HoleBlock->AddParam(decodeTextW_16, intToStr_13);
        }
        // 'StarList'
        {
            EC_BlockPar::TBlockParEC* cpp_with = GR_Main::EditableSaveBlock->AddBlockByPath(EC_Str::DecodeTextW(u"SatraproLaiAsot"_w));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_2.next(i); ) {
                Star = pas::list_at<TStar>(Stars, i);
                Name = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(Star->Id)), EC_Str::DecodeTextW(u"S5tTaersIed2"_w)});
                Star->SaveToBlock(cpp_with->AddBlockByPath(Name));
            }
        }
    }

    // Only runs when FinalizationNameEncoded is empty; consumes and clears the shared editable-save block. Requires a player.
    void TGalaxy::ApplyEditableState() {
        std::int32_t I{};
        pas::WideString Key{};
        TStar* Star{};
        THole* Hole{};
        float Angle{};
        float Radius{};
        EC_BlockPar::TBlockParEC* HoleBlock{};
        if (FinalizationNameEncoded == u"") {
            GR_Main::CCInterface->SetEditableStateApplied(true);
            FinalizationNameEncoded = EC_Str::EncodeTextW(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"FsignsarltiyzaazthikoEnoNiaemaex"_w))));
            if (FinalizationNameEncoded != u"") {
                SetCheatPoints(0);
            }
            {
                std::int32_t min = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DeieffPhizroantle"_w))))));
                DifficultyLevels[0] = std::max<std::int32_t>(0, min);
            }
            {
                std::int32_t min_2 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DuiefsTvrnaSdlej"_w))))));
                DifficultyLevels[1] = std::max<std::int32_t>(0, min_2);
            }
            {
                std::int32_t min_3 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DpiffeSscvn"_w))))));
                DifficultyLevels[2] = std::max<std::int32_t>(0, min_3);
            }
            {
                std::int32_t min_4 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DqirfsRfejpyariSra"_w))))));
                DifficultyLevels[3] = std::max<std::int32_t>(0, min_4);
            }
            {
                std::int32_t min_5 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DoitfdTgeecthv"_w))))));
                DifficultyLevels[4] = std::max<std::int32_t>(0, min_5);
            }
            {
                std::int32_t min_6 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DpiFfsQvueeYsst"_w))))));
                DifficultyLevels[5] = std::max<std::int32_t>(0, min_6);
            }
            {
                std::int32_t min_7 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DpiwfsHrojlee"_w))))));
                DifficultyLevels[6] = std::max<std::int32_t>(0, min_7);
            }
            {
                std::int32_t min_8 = std::min<std::int32_t>(9, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"DpiefdBkarlsaGndcVee"_w))))));
                DifficultyLevels[7] = std::max<std::int32_t>(0, min_8);
            }
            WarDeltaWin[1] = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"KvlsiRnsgTshDdeHljtoaRWdifnG"_w)))));
            WarDeltaWin[2] = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"PuirreastTerswDVesltt4a6WHidns"_w)))));
            WarDeltaWin[0] = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"Npo6rdm2aSlfsHDeeyljt4asWCignI"_w)))));
            aPlayer::GetPlayer()->DeclinePlanetBattleOffers = SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(GR_Main::EditableSaveBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"RlehjieScataPSB"_w))))) == "true";
            {
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::EditableSaveBlock->GetBlockByPath(EC_Str::DecodeTextW(u"Polearymeir"_w));
                aPlayer::TPlayer* player = aPlayer::GetPlayer();
                player->LoadFromBlock(blockByPath);
            }
            HoleBlock = GR_Main::EditableSaveBlock->GetBlockByPath(EC_Str::DecodeTextW(u"HroslaenLfirs4t"_w));
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Holes) - 1); cpp_range.next(I); ) {
                Hole = pas::list_at<THole>(Holes, I);
                Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(Hole->Id)), EC_Str::DecodeTextW(u"Heo4lge5I6dY"_w)});
                Hole->LoadFromBlock(HoleBlock->GetBlockByPath(Key));
            }
            // 'CreateNewHoles'
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SysUtils::StrToInt(static_cast<pas::AnsiString>(HoleBlock->GetParam(pas::view(EC_Str::DecodeTextW(u"CEr2e4aftge4NgehwYHeohlsegs1"_w))))) - 1); cpp_range_2.next(I); ) {
                Hole = pas::construct_call<THole>(THole_Create);
                Hole->InitializeGraphic(pas::WideString());
                reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->SetState(1);
                Hole->Star1 = aPlayer::GetPlayer()->CurrentStar;
                Hole->Star2 = aPlayer::GetPlayer()->CurrentStar;
                Hole->ArcadeMapName = pas::WideString();
                Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 359));
                Radius = aMyFunction::RandomIntRange(1000, 2000);
                {
                    float cpp_arg = System::Sin(Angle) * Radius;
                    float cpp_arg_2 = -System::Cos(Angle) * Radius;
                    Hole->Position1 = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
                }
                {
                    float cpp_arg_3 = System::Sin(Angle) * Radius;
                    float cpp_arg_4 = -System::Cos(Angle) * Radius;
                    Hole->Position2 = EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4);
                }
                Hole->CreatedTurn = CurrentTurn;
                Hole->HoleType = 1;
                pas::list_add(Holes, reinterpret_cast<void*>(Hole));
            }
            // 'StarList'
            {
                EC_BlockPar::TBlockParEC* cpp_with = GR_Main::EditableSaveBlock->GetBlockByPath(EC_Str::DecodeTextW(u"SatraproLaiAsot"_w));
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_3.next(I); ) {
                    Star = pas::list_at<TStar>(Stars, I);
                    Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(Star->Id)), EC_Str::DecodeTextW(u"S5tTaersIed2"_w)});
                    Star->LoadFromBlock(cpp_with->GetBlockByPath(Key));
                }
            }
            GR_Main::EditableSaveBlock->Clear();
        }
    }

    void TGalaxy::RunConfigOnStartHandlers() {
        EC_BlockPar::TBlockParEC* Handler{};
        std::int32_t I{};
        std::int32_t Count{};
        pas::WideString Text{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::MainDataConfig->GetBlock(u"BV"sv)->FindBlockByPath(u"OnStart"_wref.get());
        if (Block != nullptr) {
            Count = Block->GetBlockCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Handler = Block->GetBlockByIndex(I);
                Text = Handler->ConcatenateValues();
                aScript::ExecuteScriptText(Text, nullptr);
            }
        }
    }

    void TGalaxy::RunConfigOnLoadHandlers() {
        EC_BlockPar::TBlockParEC* Handler{};
        std::int32_t I{};
        std::int32_t Count{};
        pas::WideString Text{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::MainDataConfig->GetBlock(u"BV"sv)->FindBlockByPath(u"OnLoad"_wref.get());
        if (Block != nullptr) {
            Count = Block->GetBlockCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Handler = Block->GetBlockByIndex(I);
                Text = Handler->ConcatenateValues();
                aScript::ExecuteScriptText(Text, nullptr);
            }
        }
    }

    void TGalaxy::RunConfigOnSaveHandlers() {
        EC_BlockPar::TBlockParEC* Handler{};
        std::int32_t I{};
        std::int32_t Count{};
        pas::WideString Text{};
        EC_BlockPar::TBlockParEC* Block = GR_Main::MainDataConfig->GetBlock(u"BV"sv)->FindBlockByPath(u"OnSave"_wref.get());
        if (Block != nullptr) {
            Count = Block->GetBlockCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Handler = Block->GetBlockByIndex(I);
                Text = Handler->ConcatenateValues();
                aScript::ExecuteScriptText(Text, nullptr);
            }
        }
    }

    void TGalaxy::ReapplyInterfaceOverrides() {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(InterfaceStateOverrides) - 1); cpp_range.next(I); ) {
            pas::list_at<TInterfaceStateOverride>(InterfaceStateOverrides, I)->Reapply();
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(InterfaceTextOverrides) - 1); cpp_range_2.next(I); ) {
            pas::list_at<TInterfaceTextOverride>(InterfaceTextOverrides, I)->Reapply();
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(InterfaceImageOverrides) - 1); cpp_range_3.next(I); ) {
            pas::list_at<TInterfaceImageOverride>(InterfaceImageOverrides, I)->Reapply();
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(InterfacePositionOverrides) - 1); cpp_range_4.next(I); ) {
            pas::list_at<TInterfacePosOverride>(InterfacePositionOverrides, I)->Reapply();
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(InterfaceSizeOverrides) - 1); cpp_range_5.next(I); ) {
            pas::list_at<TInterfaceSizeOverride>(InterfaceSizeOverrides, I)->Reapply();
        }
    }

    void TGalaxy::BindScriptImports() {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Scripts) - 1); cpp_range.next(I); ) {
            pas::list_at<aScript::TScript>(Scripts, I)->BindImportedFunctions();
        }
    }

    // Processes off-screen stars; the player's star is simulated separately.
    void TGalaxy::NextDay() {
        std::int32_t I{};
        std::int32_t J{};
        float Ratio{};
        TStar* Star{};
        aShip::TShip* Ship{};
        aGroup::TGroup* Group{};
        aScript::TScript* Script{};
        Globals::TMessagePlayer* Bubble{};
        std::int32_t PreviousTechLevel{};
        pas::WideString Text{};
        std::int32_t Stage = 0;
        if (aPlayer::GetPlayer() != nullptr) {
            try {
                Ratio = pas::real_divide(TGalaxy::GetCoalitionToPirateSystemRatio(), aConst::GalaxyDifficultyTuning[DifficultyLevels[0]].CoalitionToPirateBalanceRatio);
                // Native applies bitwise NOT before comparison, rather than inequality.
                if (~PirateWinType == 3 && (Ratio > 1.25L && aMyFunction::NextRandomIntRange(1, 1000, RandomState) <= 3 || Ratio > 1.5L && aMyFunction::NextRandomIntRange(1, 1000, RandomState) <= 10 || Ratio > 2.0L && aMyFunction::NextRandomIntRange(1, 1000, RandomState) <= 30)) {
                    --WarDeltaWin[2];
                }
                if (Galaxy->CoalitionDefeatedTurn == 0 && Ratio < 0.8L && aMyFunction::NextRandomIntRange(1, 1000, RandomState) <= 3 || Ratio < 0.66L && aMyFunction::NextRandomIntRange(1, 1000, RandomState) <= 10 || Ratio < 0.5L && aMyFunction::NextRandomIntRange(1, 1000, RandomState) <= 30) {
                    ++WarDeltaWin[2];
                    --WarDeltaWin[0];
                }
                Stage = 1;
                if (pas::random(50, &System::RandSeed) == 0) {
                    GR_Main::CheckPlatformModules();
                }
                Stage = 13;
                if (pas::imod(CurrentTurn + static_cast<std::int32_t>(GenerationSeed), GetTurnsBetweenLiberationGroups()) == 0 && CurrentTurn >= aGalaxyStruct::GalaxyWarmupTurns || WarDeltaWin[0] < -5 || CountFactionStars(aGalaxyStruct::sfCoalition) < 5 && pas::list_count(LiberationGroups) == 0 || CountFactionStars(aGalaxyStruct::sfCoalition) == 1) {
                    Stage = 14;
                    if (pas::list_count(LiberationGroups) < 2) {
                        TryCreateLiberationGroup();
                    }
                }
                Stage = 15;
                {
                    const std::int32_t cpp_first = pas::list_count(LiberationGroups) - 1;
                    if (cpp_first >= 0) {
                        for (I = cpp_first; I >= 0; --I) {
                            Group = pas::list_at<aGroup::TGroup>(LiberationGroups, I);
                            Group->NextDay();
                        }
                    }
                }
                Stage = 16;
                if (WingmenPendingLeadershipPenalty == nullptr) {
                    WingmenPendingLeadershipPenalty = pas::make_object<pas::List>();
                } else {
                    pas::list_clear(WingmenPendingLeadershipPenalty);
                }
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
                    Star = pas::list_at<TStar>(Stars, I);
                    if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->CurrentStar != Star) {
                        Star->NextDay(false);
                    }
                }
                if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetHull()->HullPoints <= 0) {
                    return;
                }
                Stage = 2;
                I = 0;
                while (I < pas::list_count(Scripts)) {
                    Stage = 3;
                    Script = pas::list_at<aScript::TScript>(Scripts, I);
                    aScript::TScript_RunTurnCode(Script);
                    Stage = 4;
                    if (pas::list_count(Script->Ships) < 1) {
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Script->EtherIds->GetCount() - 1); cpp_range_2.next(J); ) {
                            Stage = 5;
                            Bubble = Globals::FindPlayerBubbleByKey(Script->EtherIds->GetTextAt(J), false);
                            if (Bubble != nullptr && Bubble->Kind == Globals::pmQuestActive) {
                                Bubble->Kind = Globals::pmQuestCancelled;
                                Bubble->WasRead = false;
                            }
                        }
                        Stage = 6;
                        pas::list_delete(Scripts, I);
                        try {
                            pas::free(Script);
                        } catch (...) {
                            GR_Main::AppendLogLineThreadSafe("Error Galaxy.Script.Free"_a);
                        }
                        Stage = 7;
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Globals::ScriptTemplates) - 1); cpp_range_3.next(J); ) {
                            if (pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, J)->ActiveScriptIndex == I) {
                                pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, J)->ActiveScriptIndex = -1;
                            } else if (pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, J)->ActiveScriptIndex > I) {
                                --pas::list_at<Globals::TScriptTemplUnit>(Globals::ScriptTemplates, J)->ActiveScriptIndex;
                            }
                        }
                    } else {
                        ++I;
                    }
                }
                ++CurrentTurn;
                Stage = 8;
                ProcessStationSpawning();
                Stage = 9;
                UpdateConstellationMilitaryStats();
                Stage = 10;
                PlayerDialogueRequestCount = 0;
                ++ReservedMessageCounter;
                ++TurnsSinceLastShipMessage;
                if (pas::imod(CurrentTurn, aGalaxyStruct::TurnsPerYear) == 0) {
                    ComputeGlobalGoodsPriceBands();
                }
                Stage = 11;
                if (aPlayer::GetPlayer() != nullptr) {
                    aPlayer::GetPlayer()->RebuildEquipmentCache();
                }
                Stage = 12;
                if ((CurrentTurn + static_cast<std::int32_t>(GenerationSeed)) % 7 == 0) {
                    ComputeRangerSpawnQuotas();
                }
                Stage = 17;
                if (aPlayer::GetPlayer() != nullptr) {
                    if (aPlayer::GetPlayer()->CurrentStar != nullptr) {
                        const std::int32_t cpp_first_2 = pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1;
                        if (cpp_first_2 >= 0) {
                            for (I = cpp_first_2; I >= 0; --I) {
                                Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
                                if (Ship->PartnerShip != nullptr && Ship->PartnershipDaysRemaining > 0 && pas::list_indexof(WingmenPendingLeadershipPenalty, reinterpret_cast<void*>(Ship)) < 0) {
                                    pas::list_add(WingmenPendingLeadershipPenalty, reinterpret_cast<void*>(Ship));
                                }
                            }
                        }
                    }
                }
                Stage = 18;
                TGalaxy::ApplyWingmanLeadershipPenalty();
                Stage = 19;
                PruneExpiredGalaxyEvents();
                Stage = 20;
                PreviousTechLevel = TechLevel;
                RefreshTechLevel();
                if (aPlayer::GetPlayer() != nullptr) {
                    if (aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
                        Text = pas::WideString();
                        if (TechLevel > PreviousTechLevel) {
                            Text = aConst::LocalizedText(u"Artefacts.ArtAnalyzer.TechLevelUp"_wref.get());
                        }
                        if (TechLevel < PreviousTechLevel) {
                            Text = aConst::LocalizedText(u"Artefacts.ArtAnalyzer.TechLevelDown"_wref.get());
                        }
                        if (Text != u"") {
                            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, Text, u""_wref.get());
                        }
                    }
                }
                Stage = 21;
                TryDispatchMilitaryBaseToEnemyStar();
                Stage = 22;
                RefreshRangerWealthStats();
                Stage = 23;
                RefreshRangerStrengthStats();
                Stage = 24;
                RefreshRangerRatingPlaces();
                Stage = 25;
                PrunePlanetNews();
                Stage = 26;
                SE_Garbage::CheckMemoryUsage();
            } catch (...) {
                auto cpp_exception_2 = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception_2)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TGalaxy.NextDay label = ", SysUtils::IntToStr(Stage)})));
                } else {
                    throw;
                }
            }
        }
    }

    void TGalaxy::CompleteDay(std::uint8_t UnusedRecordFilm) {
        THole* Hole{};
        aShip::TShip* Ship{};
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t Count{};
        float Angle{};
        float Radius{};
        TStar* Star{};
        aMissile::TMissile* Missile{};
        if (SpecialSimulationMode != 0) {
            return;
        }
        if (AreSpecialShipsEnabled()) {
            AssignSpecialStationService();
        }
        ProcessDominatorResearchProgress();
        ProcessBankDebtAndDeposits();
        ProcessRangerCenterNewYearEvent();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->AfterburnerActive && aPlayer::GetPlayer()->GetEngine() != nullptr && static_cast<long double>(aPlayer::GetPlayer()->GetEngine()->ConditionPercent) <= GlobalsV::AfterburnerStopCondition && TechnicModEnabled == 0) {
            aPlayer::GetPlayer()->AfterburnerActive = false;
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            PlayerStar->InterruptLongTravel = true;
        }
        if (aPlayer::GetPlayer() != nullptr) {
            i = 0;
            while (i < pas::list_count(Holes)) {
                Hole = pas::list_at<THole>(Holes, i);
                if (Hole->HoleType == 3 || Hole->HoleType == 4 && KellerMissionState == 5 && ScaleIntByTechLevel(1, 10) < CurrentTurn - Hole->CreatedTurn || Hole->HoleType == 1 && CurrentTurn - Hole->CreatedTurn > 200 || Hole->HoleType == 4 && aKling::KellerShip == nullptr) {
                    j = 0;
                    Count = pas::list_count(Hole->Star1->Ships);
                    while (j < Count) {
                        Ship = pas::list_at<aShip::TShip>(Hole->Star1->Ships, j);
                        if (Ship->Order == aShip::soJumpHole && Ship->OrderTarget == Hole) {
                            break;
                        }
                        ++j;
                    }
                    if (j >= Count) {
                        j = 0;
                        Count = pas::list_count(Hole->Star2->Ships);
                        while (j < Count) {
                            Ship = pas::list_at<aShip::TShip>(Hole->Star2->Ships, j);
                            if (Ship->Order == aShip::soJumpHole && Ship->OrderTarget == Hole) {
                                break;
                            }
                            ++j;
                        }
                        if (j >= Count) {
                            if (Hole->HoleType == 4) {
                                KellerMissionState = 0;
                            }
                            pas::list_delete(Holes, i);
                            pas::free(Hole);
                            --i;
                        }
                    }
                }
                ++i;
            }
        }
        if (aPlayer::GetPlayer() != nullptr && aMyFunction::NextRandomIntRange(0, aConst::GalaxyDifficultyTuning[DifficultyLevels[6]].RandomHoleSpawnRollMaximum, RandomState) == 0 && pas::list_count(Holes) <= 2 && CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns) {
            Hole = pas::construct_call<THole>(THole_Create);
            Hole->InitializeGraphic(pas::WideString());
            Hole->HoleType = 1;
            Hole->CreatedTurn = CurrentTurn;
            j = 0;
            while (true) {
                ++j;
                if (j > 100) {
                    break;
                }
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(Stars) - 1, RandomState);
                    aMyFunction::TObjectList* stars = Stars;
                    Hole->Star1 = pas::list_at<TStar>(stars, nextRandomIntRange);
                }
                if (Hole->Star1->Constellation->Id != 20 && aPlayer::GetPlayer()->CurrentStar != Hole->Star1 && Hole->Star1->IsConstellationVisible() && Hole->Star1->DaysSincePlayerVisit >= 30 && Hole->Star1->Status.ControlFaction != aGalaxyStruct::sfDominators && Hole->Star1->Status.CustomFaction == u"") {
                    break;
                }
            }
            if (j > 100) {
                pas::free(Hole);
            } else {
                j = 0;
                while (true) {
                    ++j;
                    if (j > 100) {
                        break;
                    }
                    {
                        std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(0, pas::list_count(Stars) - 1, RandomState);
                        aMyFunction::TObjectList* stars_2 = Stars;
                        Hole->Star2 = pas::list_at<TStar>(stars_2, nextRandomIntRange_2);
                    }
                    if (Hole->Star1 != Hole->Star2 && Hole->Star2->Constellation->Id != 20 && aPlayer::GetPlayer()->CurrentStar != Hole->Star2 && Hole->Star2->IsConstellationVisible() && Hole->Star2->DaysSincePlayerVisit >= 30) {
                        break;
                    }
                }
                if (j > 100) {
                    pas::free(Hole);
                } else {
                    pas::list_add(Holes, reinterpret_cast<void*>(Hole));
                    Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, (Hole->Star1->GenerationSeed + CurrentTurn) * GenerationSeed));
                    {
                        std::int32_t round = System::Round(Hole->Star1->MapDiameter * 0.5L * 0.9L);
                        std::int32_t round_2 = System::Round(Hole->Star1->MapDiameter * 0.5L * 0.7L);
                        Radius = aMyFunction::SeededRandomIntRange(round_2, round, (Hole->Star1->GenerationSeed + CurrentTurn + j) * GenerationSeed);
                    }
                    {
                        float cpp_arg = System::Sin(Angle) * Radius;
                        float cpp_arg_2 = -System::Cos(Angle) * Radius;
                        Hole->Position1 = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
                    }
                    Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, (Hole->Star2->GenerationSeed + CurrentTurn) * GenerationSeed));
                    {
                        std::int32_t round_3 = System::Round(Hole->Star2->MapDiameter * 0.5L * 0.9L);
                        std::int32_t round_4 = System::Round(Hole->Star2->MapDiameter * 0.5L * 0.7L);
                        Radius = aMyFunction::SeededRandomIntRange(round_4, round_3, (Hole->Star2->GenerationSeed + CurrentTurn + j) * GenerationSeed);
                    }
                    {
                        float cpp_arg_3 = System::Sin(Angle) * Radius;
                        float cpp_arg_4 = -System::Cos(Angle) * Radius;
                        Hole->Position2 = EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4);
                    }
                    if (CoalitionDefeatedTurn == 0) {
                        pas::WideString formatText2 = ([&] {
                            auto name = pas::borrow(Hole->Star1->Name);
                            auto name_2 = pas::borrow(Hole->Star2->Name);
                            pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.BlackHole.Create"_wref.get(), (Hole->Star1->GenerationSeed + CurrentTurn) * GenerationSeed);
                            pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Star1>"_w, name.get(), u"<Star2>"_w, name_2.get());
                        }());
                        TGalaxy* self = this;
                        self->AddPlanetNews(aGalaxyStruct::gnWormholeCreated, std::move(formatText2));
                    }
                }
            }
        }
        TGalaxy::ProcessPlayerSatelliteExploration();
        if (TerronSeriesResolvedTurn == 0) {
            if (aKling::TerronShip == nullptr) {
                TerronSeriesResolvedTurn = CurrentTurn;
                if (aPlayer::GetPlayer() != nullptr && TerronLandingLockTurn != 0) {
                    Achievements::TryUnlockAchievement(u"TERRONBATTLE"_w);
                }
            } else if (TerronToStarTurn != 0) {
                TerronSeriesResolvedTurn = CurrentTurn;
                if (aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryUnlockAchievement(u"TERRONSTAR"_w);
                }
            }
            if (TerronSeriesResolvedTurn != 0) {
                DominatorResearch[aGalaxyStruct::dsTerron].Progress = 1.0E+2f;
                if (CoalitionDefeatedTurn == 0) {
                    const pas::WideString& replaceColoredToken = ([&] {
                        auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString formatGameTurnDate = aGalaxy::FormatGameTurnDate(TerronSeriesResolvedTurn);
                        pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuinsRC.Win.AddNews"_wref.get());
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Date>"_w, std::move(formatGameTurnDate), textHighlightColorTag_2.get());
                    }());
                    std::int32_t terronSeriesResolvedTurn = TerronSeriesResolvedTurn;
                    Globals::AddOrUpdatePlayerBubble(Globals::pmQuestActive, terronSeriesResolvedTurn, replaceColoredToken, u"TerronWin"_wref.get());
                }
            }
        }
        if (KellerSeriesResolvedTurn == 0) {
            if (aKling::KellerShip == nullptr) {
                KellerSeriesResolvedTurn = CurrentTurn;
                if (aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryUnlockAchievement(u"KELLERDESTROY"_w);
                }
            } else if (KellerLeaveTurn != 0) {
                KellerSeriesResolvedTurn = CurrentTurn;
                if (aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryUnlockAchievement(u"KELLERRESEARCH"_w);
                }
            }
            if (KellerResearchTargetStarId != 0) {
                KellerSeriesResolvedTurn = CurrentTurn;
            }
            if (KellerSeriesResolvedTurn != 0) {
                DominatorResearch[aGalaxyStruct::dsKeller].Progress = 1.0E+2f;
                if (CoalitionDefeatedTurn == 0) {
                    const pas::WideString& replaceColoredToken_2 = ([&] {
                        auto textHighlightColorTag_3 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString formatGameTurnDate_2 = aGalaxy::FormatGameTurnDate(KellerSeriesResolvedTurn);
                        pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuinsRC.Win.AddNews"_wref.get());
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Date>"_w, std::move(formatGameTurnDate_2), textHighlightColorTag_3.get());
                    }());
                    std::int32_t kellerSeriesResolvedTurn = KellerSeriesResolvedTurn;
                    Globals::AddOrUpdatePlayerBubble(Globals::pmQuestActive, kellerSeriesResolvedTurn, replaceColoredToken_2, u"KellerWin"_wref.get());
                }
            }
        }
        if (BlazerSeriesResolvedTurn == 0) {
            if (aKling::BlazerShip == nullptr) {
                BlazerSeriesResolvedTurn = CurrentTurn;
                if (aPlayer::GetPlayer() != nullptr) {
                    if (BlazerSelfDestructTurn == 0) {
                        Achievements::TryUnlockAchievement(u"TERMINATOR"_w);
                    } else {
                        Achievements::TryUnlockAchievement(u"BLAZERPROGRAM"_w);
                    }
                }
            } else if (BlazerLandingPlanetId != 0) {
                BlazerSeriesResolvedTurn = CurrentTurn;
                if (aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryUnlockAchievement(u"BLAZERPIECE"_w);
                }
            }
            if (BlazerSeriesResolvedTurn != 0) {
                DominatorResearch[aGalaxyStruct::dsBlazer].Progress = 1.0E+2f;
                if (CoalitionDefeatedTurn == 0) {
                    const pas::WideString& replaceColoredToken_3 = ([&] {
                        auto textHighlightColorTag_4 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString formatGameTurnDate_3 = aGalaxy::FormatGameTurnDate(BlazerSeriesResolvedTurn);
                        pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuinsRC.Win.AddNews"_wref.get());
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_3), u"<Date>"_w, std::move(formatGameTurnDate_3), textHighlightColorTag_4.get());
                    }());
                    std::int32_t blazerSeriesResolvedTurn = BlazerSeriesResolvedTurn;
                    Globals::AddOrUpdatePlayerBubble(Globals::pmQuestActive, blazerSeriesResolvedTurn, replaceColoredToken_3, u"BlazerWin"_wref.get());
                }
                if (BlazerLandingPlanetId != 0 && aKling::BlazerShip != nullptr && aKling::BlazerShip->InNormalSpace()) {
                    aKling::BlazerShip->EnemyShip = nullptr;
                    Star = aKling::BlazerShip->CurrentStar;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range.next(j); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, j);
                        if (Ship->EnemyShip == aKling::BlazerShip) {
                            Ship->EnemyShip = nullptr;
                        }
                        if (Ship->TruceShip == aKling::BlazerShip) {
                            Ship->TruceShip = nullptr;
                        }
                        // Native code compares the order target with the galaxy instance.
                        if (Ship->OrderTarget == this) {
                            Ship->OrderNone(false);
                        }
                    }
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Missiles) - 1); cpp_range_2.next(j); ) {
                        Missile = pas::list_at<aMissile::TMissile>(Star->Missiles, j);
                        Missile->ClearReferencesTo(this);
                    }
                }
            }
        }
        ProcessCoalitionDefeat();
        if (CurrentTurn % 30 == 0) {
            TGalaxy::AppendIntegritySnapshot();
        }
    }

    void TGalaxy::TransferShipsInTransit() {
        std::int32_t I{};
        std::int32_t Effect{};
        aShip::TShip* Ship{};
        THole* Hole{};
        std::int32_t J = pas::list_count(ShipsInTransit);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, J - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(ShipsInTransit, I);
            if (Ship->OrderTarget != nullptr) {
                if (Ship == aPlayer::GetPlayer()) {
                    do {
                        Effect = aMyFunction::NextRandomIntRange(0, 2, RandomState);
                    } while (!(SpaceEffectKind != Effect));
                    SpaceEffectKind = Effect;
                }
                if (TStar* star = pas::class_cast_if<TStar*>(Ship->OrderTarget)) {
                    Ship->TransferToStar(star);
                } else {
                    Hole = pas::checked_cast<THole*>(Ship->OrderTarget);
                    if (pas::shr(Ship->OrderStateData, 16) == 0) {
                        Ship->TransferToStar(Hole->Star2);
                    } else {
                        Ship->TransferToStar(Hole->Star1);
                    }
                }
            }
        }
        pas::list_clear(ShipsInTransit);
        if (KellerMissionState == 1 && aKling::KellerShip != nullptr && KellerTargetStar != nullptr) {
            if (aKling::KellerShip->CurrentStar != KellerTargetStar) {
                aKling::KellerShip->CurrentStar->HandleObjectLeavingStar(aKling::KellerShip);
                aKling::KellerShip->TransferToStar(KellerTargetStar);
            }
            KellerMissionState = 2;
        }
    }

    // Refreshes every star's cache; called during generation and loading, not by Script.CoordX/CoordY.
    void TGalaxy::RebuildStarDistances() {
        std::int32_t I{};
        TStar* Star{};
        std::int32_t J = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, J - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            Star->RebuildStarDistances(this);
        }
    }

    void TGalaxy::RefreshAllShipDerivedState() {
        std::int32_t I{};
        std::int32_t K{};
        std::int32_t L{};
        TStar* Star{};
        aShip::TShip* Ship{};
        std::int32_t J = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, J - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            L = pas::list_count(Star->Ships);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, L - 1); cpp_range_2.next(K); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, K);
                Ship->RefreshDerivedStats(true);
                Ship->RefreshGraphicSize();
            }
        }
    }

    // Zero returns nil; an unknown nonzero ID raises.
    TConstellation* TGalaxy::IdToConstellation(std::uint32_t Id) {
        TConstellation* Item{};
        std::int32_t I{};
        TConstellation* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<TConstellation>(Constellations, I);
            if (Item->Id == Id) {
                return Item;
            }
        }
        pas::raise(pas::make_exception<pas::Exception>("function TGalaxy.IdToConstellation (id: Cardinal): TObject;"_a));
    }

    // Zero returns nil; an unknown nonzero ID raises.
    TStar* TGalaxy::IdToStar(std::uint32_t Id) {
        TStar* Item{};
        std::int32_t I{};
        TStar* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        if (static_cast<std::uint32_t>(pas::list_count(Stars)) >= Id && pas::list_at<TStar>(Stars, static_cast<std::int32_t>(Id - 1))->Id == Id) {
            return pas::list_at<TStar>(Stars, static_cast<std::int32_t>(Id - 1));
        }
        std::int32_t Count = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<TStar>(Stars, I);
            if (Item->Id == Id) {
                return Item;
            }
        }
        pas::raise(pas::make_exception<pas::Exception>("function TGalaxy.IdToStar (id: Cardinal): TObject;"_a));
    }

    // Zero returns nil; an unknown nonzero ID raises.
    THole* TGalaxy::IdToHole(std::uint32_t Id) {
        THole* Item{};
        std::int32_t I{};
        THole* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        std::int32_t Count = pas::list_count(Holes);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Item = pas::list_at<THole>(Holes, I);
            if (Item->Id == Id) {
                return Item;
            }
        }
        pas::raise(pas::make_exception<pas::Exception>("function TGalaxy.IdToHole (id: Cardinal): TObject;"_a));
    }

    // Zero always returns nil.
    void* TGalaxy::IdToPlanet(std::uint32_t Id, std::uint8_t RaiseIfMissing) {
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t PlanetCount{};
        void* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        if (static_cast<std::uint32_t>(pas::list_count(Planets)) >= Id && pas::list_at<aPlanet::TPlanet>(Planets, static_cast<std::int32_t>(Id - 1))->Id == Id) {
            return pas::list_get(Planets, static_cast<std::int32_t>(Id - 1));
        }
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            PlanetCount = pas::list_count(Star->Planets);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PlanetCount - 1); cpp_range_2.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                if (Planet->Id == Id) {
                    return Planet;
                }
            }
        }
        if (RaiseIfMissing) {
            pas::raise(pas::make_exception<pas::Exception>("function TGalaxy.IdToPlanet (id: Cardinal; exc: boolean = True): TObject;"_a));
        }
        return nullptr;
    }

    // Zero always returns nil. Includes docked ships and stored Tranclucators.
    void* TGalaxy::IdToShip(std::uint32_t Id, std::uint8_t RaiseIfMissing) {
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count{};
        std::int32_t K{};
        std::int32_t ArtefactCount{};
        aItem::TItem* Item{};
        aItem::TArtefact* Artefact{};
        aPlayer::PStorageEntry Storage{};
        void* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            Count = pas::list_count(Star->Ships);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (static_cast<std::uint32_t>(Ship->Id) == Id) {
                    return Ship;
                }
                ArtefactCount = pas::list_count(Ship->Artefacts);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ArtefactCount - 1); cpp_range_3.next(K); ) {
                    Artefact = pas::list_at<aItem::TArtefact>(Ship->Artefacts, K);
                    if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Artefact); artefactTranclucator != nullptr && static_cast<std::uint32_t>(pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->Id) == Id) {
                        return artefactTranclucator->Ship;
                    }
                }
            }
            Count = pas::list_count(Star->Planets);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                ArtefactCount = pas::list_count(Planet->Warriors);
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, ArtefactCount - 1); cpp_range_5.next(K); ) {
                    Ship = pas::list_at<aShip::TShip>(Planet->Warriors, K);
                    if (static_cast<std::uint32_t>(Ship->Id) == Id) {
                        return Ship;
                    }
                }
            }
            Count = pas::list_count(Star->Items);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(J); ) {
                Item = pas::list_at<aItem::TItem>(Star->Items, J);
                if (aItem::TArtefactTranclucator* artefactTranclucator_2 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item); artefactTranclucator_2 != nullptr && artefactTranclucator_2->Ship != nullptr && static_cast<std::uint32_t>(pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator_2->Ship))->Id) == Id) {
                    return artefactTranclucator_2->Ship;
                }
            }
            Count = pas::list_count(Star->MovingDropItems);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(J); ) {
                Item = reinterpret_cast<aItem::TItem*>(pas::list_at<TMovingDropItemEntry>(Star->MovingDropItems, J)->Payload);
                if (Item != nullptr && pas::class_cast_if<aItem::TArtefactTranclucator*>(Item) != nullptr && static_cast<aItem::TArtefactTranclucator*>(Item)->Ship != nullptr && static_cast<std::uint32_t>(pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(static_cast<aItem::TArtefactTranclucator*>(Item)->Ship))->Id) == Id) {
                    return static_cast<aItem::TArtefactTranclucator*>(Item)->Ship;
                }
            }
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_8.next(I); ) {
            Storage = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
            Item = Storage->Item;
            if (aItem::TArtefactTranclucator* artefactTranclucator_3 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item); artefactTranclucator_3 != nullptr && artefactTranclucator_3->Ship != nullptr && static_cast<std::uint32_t>(pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator_3->Ship))->Id) == Id) {
                return artefactTranclucator_3->Ship;
            }
        }
        if (RaiseIfMissing) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"function TGalaxy.IdToShip, id = ", SysUtils::Int64ToStr(Id)})));
        }
        return nullptr;
    }

    // Zero always returns nil. Includes inventories, shops, storage and moving drops.
    void* TGalaxy::IdToItem(std::uint32_t Id, std::uint8_t RaiseIfMissing) {
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        aPlayer::PStorageEntry Storage{};
        TStoredItem* Stored{};
        PMovingDropItemEntry Drop{};
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t ListCount{};
        std::int32_t k{};
        std::int32_t SubCount{};
        void* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(i); ) {
            Star = pas::list_at<TStar>(Stars, i);
            ListCount = pas::list_count(Star->Items);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ListCount - 1); cpp_range_2.next(j); ) {
                Item = pas::list_at<aItem::TItem>(Star->Items, j);
                if (static_cast<std::uint32_t>(Item->Id) == Id) {
                    return Item;
                }
                if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                    Result = static_cast<aShip::TShip*>(artefactTranclucator->Ship)->FindCarriedItemById(Id);
                    if (Result != nullptr) {
                        return Result;
                    }
                }
            }
            ListCount = pas::list_count(Star->MovingDropItems);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ListCount - 1); cpp_range_3.next(j); ) {
                Drop = pas::list_at<TMovingDropItemEntry>(Star->MovingDropItems, j);
                Item = pas::checked_cast<aItem::TItem*>(Drop->Payload);
                if (Item == nullptr) {
                    continue;
                }
                if (static_cast<std::uint32_t>(Item->Id) == Id) {
                    return Item;
                }
                if (aItem::TArtefactTranclucator* artefactTranclucator_2 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                    Result = static_cast<aShip::TShip*>(artefactTranclucator_2->Ship)->FindCarriedItemById(Id);
                    if (Result != nullptr) {
                        return Result;
                    }
                }
            }
            ListCount = pas::list_count(Star->Ships);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, ListCount - 1); cpp_range_4.next(j); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, j);
                Item = Ship->FindCarriedItemById(Id);
                if (Item != nullptr) {
                    return Item;
                }
                if (Ship->TypeId == aGalaxyStruct::stRanger && pas::class_cast_if<aPlayer::TPlayer*>(Ship) != nullptr) {
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(static_cast<aPlayer::TPlayer*>(Ship)->StorageEntries) - 1); cpp_range_5.next(k); ) {
                        Storage = pas::list_at<aPlayer::TStorageEntry>(static_cast<aPlayer::TPlayer*>(Ship)->StorageEntries, k);
                        Item = Storage->Item;
                        if (static_cast<std::uint32_t>(Item->Id) == Id) {
                            return Item;
                        }
                        if (aItem::TArtefactTranclucator* artefactTranclucator_3 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                            Result = static_cast<aShip::TShip*>(artefactTranclucator_3->Ship)->FindCarriedItemById(Id);
                            if (Result != nullptr) {
                                return Result;
                            }
                        }
                    }
                }
            }
            ListCount = pas::list_count(Star->Planets);
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, ListCount - 1); cpp_range_6.next(j); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, j);
                SubCount = pas::list_count(Planet->Warriors);
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_7.next(k); ) {
                    Ship = pas::list_at<aShip::TShip>(Planet->Warriors, k);
                    Item = Ship->FindCarriedItemById(Id);
                    if (Item != nullptr) {
                        return Item;
                    }
                }
                SubCount = pas::list_count(Planet->EquipmentShop);
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_8.next(k); ) {
                    Item = pas::list_at<aItem::TItem>(Planet->EquipmentShop, k);
                    if (static_cast<std::uint32_t>(Item->Id) == Id) {
                        return Item;
                    }
                    if (aItem::TArtefactTranclucator* artefactTranclucator_4 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                        Result = static_cast<aShip::TShip*>(artefactTranclucator_4->Ship)->FindCarriedItemById(Id);
                        if (Result != nullptr) {
                            return Result;
                        }
                    }
                }
                if (Planet->SurfaceLootEntries != nullptr) {
                    SubCount = pas::list_count(Planet->SurfaceLootEntries);
                    for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, SubCount - 1); cpp_range_9.next(k); ) {
                        if (pas::list_get(Planet->SurfaceLootEntries, k) == nullptr) {
                            continue;
                        }
                        Item = pas::list_at<aPlanet::TPlanetSurfaceLootEntry>(Planet->SurfaceLootEntries, k)->Item;
                        if (static_cast<std::uint32_t>(Item->Id) == Id) {
                            return Item;
                        }
                        if (aItem::TArtefactTranclucator* artefactTranclucator_5 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                            Result = static_cast<aShip::TShip*>(artefactTranclucator_5->Ship)->FindCarriedItemById(Id);
                            if (Result != nullptr) {
                                return Result;
                            }
                        }
                    }
                }
            }
        }
        if (fEquipmentShop::TemporaryShopSlots != nullptr) {
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_10.next(i); ) {
                if (pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, i)->Item != nullptr) {
                    if (static_cast<std::uint32_t>(pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, i)->Item->Id) == Id) {
                        return pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, i)->Item;
                    }
                }
            }
        }
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(StoredItems) - 1); cpp_range_11.next(k); ) {
            Stored = pas::list_at<TStoredItem>(StoredItems, k);
            if (Stored->Item == nullptr) {
                continue;
            }
            Item = pas::checked_cast<aItem::TItem*>(Stored->Item);
            if (static_cast<std::uint32_t>(Item->Id) == Id) {
                return Stored->Item;
            }
            if (aItem::TArtefactTranclucator* artefactTranclucator_6 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                Result = static_cast<aShip::TShip*>(artefactTranclucator_6->Ship)->FindCarriedItemById(Id);
                if (Result != nullptr) {
                    return Result;
                }
            }
        }
        if (RaiseIfMissing) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"function TGalaxy.IdToItem, id = ", SysUtils::Int64ToStr(Id)})));
        }
        return nullptr;
    }

    // Returns nil when absent.
    void* TGalaxy::IdToAsteroid(std::uint32_t Id) {
        TStar* Star{};
        aAsteroid::TAsteroid* Item{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count{};
        void* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            Count = pas::list_count(Star->Asteroids);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(J); ) {
                Item = pas::list_at<aAsteroid::TAsteroid>(Star->Asteroids, J);
                if (Item->Id == Id) {
                    return Item;
                }
            }
        }
        return Result;
    }

    // Returns nil when absent.
    void* TGalaxy::IdToMissile(std::uint32_t Id) {
        TStar* Star{};
        aMissile::TMissile* Item{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count{};
        void* Result = nullptr;
        if (Id == 0) {
            return Result;
        }
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            Count = pas::list_count(Star->Missiles);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(J); ) {
                Item = pas::list_at<aMissile::TMissile>(Star->Missiles, J);
                if (Item->Id == Id) {
                    return Item;
                }
            }
        }
        return Result;
    }

    std::uint8_t TGalaxy::ContainsShipReference(void* Ship) {
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* OtherShip{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t ItemCount{};
        std::int32_t K{};
        std::int32_t ChildCount{};
        aItem::TItem* Item{};
        std::uint8_t Result = true;
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            ItemCount = pas::list_count(Star->Ships);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range_2.next(J); ) {
                OtherShip = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (OtherShip == Ship) {
                    return Result;
                }
                ChildCount = pas::list_count(OtherShip->Artefacts);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ChildCount - 1); cpp_range_3.next(K); ) {
                    Item = pas::list_at<aItem::TItem>(OtherShip->Artefacts, K);
                    if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item); artefactTranclucator != nullptr && pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship)) == Ship) {
                        return Result;
                    }
                }
            }
            ItemCount = pas::list_count(Star->Planets);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range_4.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                ChildCount = pas::list_count(Planet->Warriors);
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, ChildCount - 1); cpp_range_5.next(K); ) {
                    OtherShip = pas::list_at<aShip::TShip>(Planet->Warriors, K);
                    if (OtherShip == Ship) {
                        return Result;
                    }
                }
            }
        }
        return false;
    }

    std::uint8_t TGalaxy::ContainsPlanetReference(void* Planet) {
        TStar* Star{};
        aPlanet::TPlanet* UnusedPlanet{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t PlanetCount{};
        std::uint8_t Result = true;
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            PlanetCount = pas::list_count(Star->Planets);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PlanetCount - 1); cpp_range_2.next(J); ) {
                UnusedPlanet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                if (UnusedPlanet == Planet) {
                    return Result;
                }
            }
        }
        return false;
    }

    void TGalaxy::ClearJumpGates() {
        std::int32_t I{};
        PJumpGateEntry Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(JumpGates) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TJumpGateEntry>(JumpGates, I);
            if (Entry->Gate != nullptr) {
                Entry->Gate->DetachFromSpace();
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->Gate));
            }
            if (Entry->Effect != nullptr) {
                Entry->Effect->DetachFromSpace();
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->Effect));
            }
            EC_Mem::FreeEC(Entry);
        }
        pas::list_clear(JumpGates);
    }

    // Registers an owned gate descriptor; returned storage is borrowed until ClearJumpGates.
    PJumpGateEntry TGalaxy::CreateJumpGate(std::uint8_t WithEffect) {
        PJumpGateEntry Entry{};
        Entry = static_cast<PJumpGateEntry>(EC_Mem::AllocClearEC(static_cast<std::int32_t>(sizeof(TJumpGateEntry))));
        pas::list_add(JumpGates, static_cast<void*>(Entry));
        Entry->UsedThisTurn = false;
        {
            SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Gate::TGateSE>(SE_Gate::TGateSE_Create, u"Gate"_w, ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> gate = pas::Var<SE_Space::TObjectSE*>(&Entry->Gate);
            SE_Space::RetainSpaceObject(gate, cpp_arg);
        }
        if (WithEffect) {
            SE_Space::TObjectSE* cpp_arg_2 = pas::construct_call<SE_Gate::TGateEffectSE>(SE_Gate::TGateEffectSE_Create, u"Effect.GateEffect"_w, ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> effect = pas::Var<SE_Space::TObjectSE*>(&Entry->Effect);
            SE_Space::RetainSpaceObject(effect, cpp_arg_2);
        } else {
            Entry->Effect = nullptr;
        }
        return Entry;
    }

    // Accepts either endpoint star; returns nil when absent.
    THole* TGalaxy::FindHoleInStarByKind(TStar* Star, std::int32_t HoleKind) {
        std::int32_t I{};
        THole* Hole{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Holes) - 1); cpp_range.next(I); ) {
            Hole = pas::list_at<THole>(Holes, I);
            if (Hole->HoleType == HoleKind && (Hole->Star1 == Star || Hole->Star2 == Star)) {
                return Hole;
            }
        }
        return nullptr;
    }

    // Visits loose items and ship equipment/artifacts; excludes shop stock and stored items.
    void TGalaxy::ReleaseItemGraphics() {
        TStar* Star{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        std::int32_t ShipCount{};
        std::int32_t ItemCount{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            ItemCount = pas::list_count(Star->Items);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range_2.next(K); ) {
                Item = pas::list_at<aItem::TItem>(Star->Items, K);
                if (Item->GraphObject != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                }
            }
            ShipCount = pas::list_count(Star->Ships);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range_3.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                ItemCount = pas::list_count(Ship->Inventory);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range_4.next(K); ) {
                    Item = pas::list_at<aItem::TItem>(Ship->Inventory, K);
                    if (Item->GraphObject != nullptr) {
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                    }
                }
                ItemCount = pas::list_count(Ship->Artefacts);
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, ItemCount - 1); cpp_range_5.next(K); ) {
                    Item = pas::list_at<aItem::TItem>(Ship->Artefacts, K);
                    if (Item->GraphObject != nullptr) {
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                    }
                }
            }
        }
    }

    // Replaces SpaceBackgroundEntries. Requires PlayerStar, a nonempty star list and differing minimum/maximum map diameters.
    void TGalaxy::GenerateSpaceBackground(std::int32_t BackgroundIndex) {
        static constexpr std::int32_t MinGroupCount = 4;
        static constexpr std::int32_t MaxNearGroupCount = 5;
        static constexpr std::int32_t MaxFarGroupCount = 6;
        std::int32_t EntryIndex{};
        std::int32_t Capacity{};
        std::int32_t I{};
        std::int32_t GroupSize{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t ImageKind{};
        double OrbitStep{};
        double Angle1{};
        double Angle2{};
        double Radius{};
        double Angle{};
        double DepthRange{};
        std::int32_t StarRadius{};
        float Density{};
        std::int32_t LayerIndex{};
        std::int32_t OffsetX{};
        std::int32_t OffsetY{};
        std::int32_t Quadrant{};
        pas::WideString Style{};
        EC_Struct::TVector3D Center{};
        pas::Array<std::int32_t, 0, 10> ImageKinds{};
        // Caller-popped static link; entry index -4, capacity -8, galaxy -12.
        auto AdvanceEntry = [&]() -> void {
            ++EntryIndex;
            if (EntryIndex + 1 > Capacity) {
                Capacity = EntryIndex + 100;
                this->SpaceBackgroundEntries.set_length(Capacity);
            }
        };
        std::int32_t MinRadius = pas::list_at<TStar>(Stars, 0)->MapDiameter;
        std::int32_t MaxRadius = MinRadius;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            StarRadius = pas::list_at<TStar>(Stars, I)->MapDiameter;
            MinRadius = std::min<std::int32_t>(MinRadius, StarRadius);
            MaxRadius = std::max<std::int32_t>(MaxRadius, StarRadius);
        }
        MinRadius = MinRadius / 2;
        MaxRadius = MaxRadius / 2;
        StarRadius = PlayerStar->MapDiameter / 2;
        // The native formula requires differing extrema; retain that assumption.
        float RadiusFraction = pas::real_divide(PlayerStar->MapDiameter / 2 - MinRadius, MaxRadius - MinRadius);
        if (GlobalsV::SpaceImage <= 1) {
            Density = 0.5f;
        } else {
            Density = 1.0f;
        }
        EntryIndex = 0;
        Capacity = 500;
        float NearDepth = aMyFunction::RemapClamped(PlayerStar->MapDiameter / 2, MinRadius, MaxRadius, 5.1, 7.1);
        float FarDepth = aMyFunction::RemapClamped(PlayerStar->MapDiameter / 2, MinRadius, MaxRadius, 7.0, 1.0E+1);
        float DepthScale = aMyFunction::RemapClamped(PlayerStar->MapDiameter / 2, MinRadius, MaxRadius, 1.0, 1.5);
        SpaceBackgroundEntries.set_length(Capacity);
        if (BackgroundIndex < 10) {
            Style = ([&] {
                const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(BackgroundIndex)}));
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"StyleGarbage"_wref.get());
                return blockByPath->GetParam(pas::view(cpp_arg));
            }());
        } else {
            Style = ([&] {
                const pas::WideString& intToStr = pas::wide_int_to_str(BackgroundIndex);
                EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(u"StyleGarbage"_wref.get());
                return blockByPath_2->GetParam(pas::view(intToStr));
            }());
        }
        std::int32_t ImageKindCount = EC_Str::CountDelimitedPartsW(pas::view(Style), u","sv);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ImageKindCount - 1); cpp_range_2.next(I); ) {
            ImageKinds[I] = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Style), I, u","sv)));
        }
        std::int32_t GroupCount = System::Round((RadiusFraction * pas::constant(static_cast<long double>(MaxNearGroupCount - MinGroupCount)) + pas::constant(static_cast<long double>(MinGroupCount))) * Density);
        Quadrant = 0;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, GroupCount - 1); cpp_range_3.next(I); ) {
            J = System::Round(aMyFunction::RandomFloatRange(0.0, 1.0) * (StarRadius * 0.6L));
            switch (Quadrant) {
                case 0: {
                    {
                        pas::Extended cpp_left = static_cast<long double>(aMyFunction::RandomFloatRange(0.1, 0.2)) * StarRadius;
                        Center.X = cpp_left * (aMyFunction::RandomIntRange(0, 1) * 2 - 1);
                    }
                    {
                        pas::Extended cpp_left_2 = static_cast<long double>(aMyFunction::RandomFloatRange(0.1, 0.2)) * StarRadius;
                        Center.Y = cpp_left_2 * (aMyFunction::RandomIntRange(0, 1) * 2 - 1);
                    }
                    Quadrant = aMyFunction::RandomIntRange(1, 4);
                    break;
                }
                case 1: {
                    Center.X = static_cast<long double>(aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius;
                    Center.Y = static_cast<long double>(-aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius + J;
                    break;
                }
                case 2: {
                    Center.X = static_cast<long double>(aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius - J;
                    Center.Y = static_cast<long double>(aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius;
                    break;
                }
                case 3: {
                    Center.X = static_cast<long double>(-aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius;
                    Center.Y = static_cast<long double>(aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius - J;
                    break;
                }
                case 4: {
                    Center.X = static_cast<long double>(-aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius + J;
                    Center.Y = static_cast<long double>(-aMyFunction::RandomFloatRange(0.6, 1.5)) * StarRadius;
                    break;
                }
            }
            aMyFunction::IncrementWrapped(Quadrant, 1, 4);
            Center.Z = aMyFunction::RandomFloatRange(0.9, 1.9);
            ImageKind = ImageKinds[aMyFunction::RandomIntRange(0, ImageKindCount - 1)];
            LayerIndex = 0;
            for (J = 0; J <= 5; ++J) {
                OffsetX = System::Round(aMyFunction::RandomIntRange(-100, 100));
                OffsetY = System::Round(aMyFunction::RandomIntRange(-100, 100));
                for (K = 0; K <= 1; ++K) {
                    ++LayerIndex;
                    SpaceBackgroundEntries[EntryIndex].ImageIndex = Globals::SelectSpaceImageTemplate(ImageKind + 5 - J);
                    pas::store_unaligned<EC_Struct::TVector3D>(&SpaceBackgroundEntries[EntryIndex].OrbitCenter, Center);
                    {
                        pas::Extended cpp_left_3 = static_cast<long double>(aMyFunction::RemapClamped(LayerIndex, 1.0, 8.0, 1.0, 5.0)) * OffsetX * DepthScale;
                        SpaceBackgroundEntries[EntryIndex].Position.X = cpp_left_3 + (static_cast<long double>(Center.X) + aMyFunction::RandomIntRange(-100, 100));
                    }
                    {
                        pas::Extended cpp_left_4 = static_cast<long double>(aMyFunction::RemapClamped(LayerIndex, 1.0, 1.0E+1, 1.0, 5.0)) * OffsetY * DepthScale;
                        SpaceBackgroundEntries[EntryIndex].Position.Y = cpp_left_4 + (static_cast<long double>(Center.Y) + aMyFunction::RandomIntRange(-100, 100));
                    }
                    SpaceBackgroundEntries[EntryIndex].Position.Z = aMyFunction::RemapClamped(LayerIndex, 1.0, 12.0, NearDepth, FarDepth);
                    pas::store_unaligned<EC_Struct::TVector3D>(&SpaceBackgroundEntries[EntryIndex].Unknown38, EC_Struct::MakeVector3D(0.0, 0.0, 0.0));
                    SpaceBackgroundEntries[EntryIndex].OrbitStepDegrees = 0.0;
                    SpaceBackgroundEntries[EntryIndex].FrameIndex = aMyFunction::RandomIntRange(0, 2000000000);
                    AdvanceEntry();
                }
            }
        }
        GroupCount = System::Round((RadiusFraction * pas::constant(static_cast<long double>(MaxFarGroupCount - MinGroupCount)) + pas::constant(static_cast<long double>(MinGroupCount))) * Density);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, GroupCount - 1); cpp_range_4.next(I); ) {
            do {
                J = System::Round(PlayerStar->MapDiameter * 0.8L);
                Center.X = aMyFunction::RandomIntRange(-J, J);
                Center.Y = aMyFunction::RandomIntRange(-J, J);
                Center.Z = FarDepth + static_cast<long double>(aMyFunction::RandomFloatRange(2.05, 3.0)) * DepthScale;
            } while (!(static_cast<long double>(Center.X) * Center.X + static_cast<long double>(Center.Y) * Center.Y > 25.0L));
            DepthRange = aMyFunction::RandomFloatRange(4.0, 1.0E+1);
            {
                pas::Extended cpp_left_5 = aMyFunction::RandomFloatRange(0.05, 0.1);
                OrbitStep = cpp_left_5 * (aMyFunction::RandomIntRange(0, 1) * 2 - 1);
            }
            Angle1 = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 360));
            Angle2 = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 360));
            GroupSize = aMyFunction::RandomIntRange(1, 2);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, GroupSize - 1); cpp_range_5.next(J); ) {
                SpaceBackgroundEntries[EntryIndex].Position.Z = static_cast<long double>(Center.Z) + aMyFunction::RandomFloatRange(0.0, DepthRange);
                SpaceBackgroundEntries[EntryIndex].ImageIndex = Globals::SelectSpaceImageTemplate(1000 + 5 - System::Round(pas::real_divide(static_cast<long double>(SpaceBackgroundEntries[EntryIndex].Position.Z) - Center.Z, DepthRange) * 5.0L));
                pas::store_unaligned<EC_Struct::TVector3D>(&SpaceBackgroundEntries[EntryIndex].OrbitCenter, Center);
                if (aMyFunction::RandomIntRange(0, 2) == 0) {
                    Radius = aMyFunction::RandomIntRange(100, 250);
                    Angle = Angle1 + pas::real_divide(aMyFunction::RandomIntRange(-1, 1) * aMyFunction::GamePi, 1.8E+2L);
                    SpaceBackgroundEntries[EntryIndex].Position.X = Center.X + System::Sin(Angle) * Radius;
                    SpaceBackgroundEntries[EntryIndex].Position.Y = Center.Y - System::Cos(Angle) * Radius;
                } else if (aMyFunction::RandomIntRange(0, 2) != 0) {
                    Radius = aMyFunction::RandomIntRange(100, 250);
                    Angle = Angle2 + pas::real_divide(aMyFunction::RandomIntRange(-3, 3) * aMyFunction::GamePi, 1.8E+2L);
                    SpaceBackgroundEntries[EntryIndex].Position.X = Center.X + System::Sin(Angle) * Radius;
                    SpaceBackgroundEntries[EntryIndex].Position.Y = Center.Y - System::Cos(Angle) * Radius;
                } else {
                    SpaceBackgroundEntries[EntryIndex].Position.X = static_cast<long double>(Center.X) + aMyFunction::RandomIntRange(-100, 100);
                    SpaceBackgroundEntries[EntryIndex].Position.Y = static_cast<long double>(Center.Y) + aMyFunction::RandomIntRange(-100, 100);
                }
                pas::store_unaligned<EC_Struct::TVector3D>(&SpaceBackgroundEntries[EntryIndex].Unknown38, EC_Struct::MakeVector3D(0.0, 0.0, 0.0));
                SpaceBackgroundEntries[EntryIndex].OrbitStepDegrees = static_cast<long double>(OrbitStep) + aMyFunction::RandomFloatRange(0.07, 0.1);
                SpaceBackgroundEntries[EntryIndex].FrameIndex = aMyFunction::RandomIntRange(0, 2000000000);
                AdvanceEntry();
            }
        }
        SpaceBackgroundEntries.set_length(EntryIndex);
    }

    void TGalaxy::EnableDominatorSurfaces() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        if (GraphDominatorSurfacesEnabled) {
            return;
        }
        GraphDominatorSurfacesEnabled = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                aShip::TShip_RefreshGraphic(pas::list_at<aShip::TShip>(Star->Ships, J));
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(K); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, K);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_4.next(J); ) {
                    aShip::TShip_RefreshGraphic(pas::list_at<aShip::TShip>(Planet->Warriors, J));
                }
            }
        }
    }

    void TGalaxy::DisableDominatorSurfaces() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        if (!GraphDominatorSurfacesEnabled) {
            return;
        }
        GraphDominatorSurfacesEnabled = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                aShip::TShip_RefreshGraphic(pas::list_at<aShip::TShip>(Star->Ships, J));
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(K); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, K);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_4.next(J); ) {
                    aShip::TShip_RefreshGraphic(pas::list_at<aShip::TShip>(Planet->Warriors, J));
                }
            }
        }
    }

    // Includes the active text quest; applying the same seed twice restores the state.
    void TGalaxy::XorProtectedState(std::int32_t Seed) {
        std::int32_t ExclusionCount{};
        pas::Array<std::uint32_t, 0, 10> Exclusions{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t L{};
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t Good{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* Ship{};
        aRanger::PQuest RangerQuest{};
        TextQuest::TTextQuest* Quest{};
        ParameterClass::TParameter* Parameter{};
        LocationClass::TLocation* Location{};
        ParameterDeltaClass::TParameterDelta* Change{};
        PathClass::TPath* Path{};
        aPlayer::PStorageEntry Storage{};
        auto XorStateUInt64 = [&](pas::Var<std::uint64_t> Value) -> void {
            pas::store_unaligned<std::uint32_t>(reinterpret_cast<System::PCardinal>(pas::byte_offset(Value.address, offsetof(TUInt64Words, Low))), pas::load_unaligned<std::uint32_t>(reinterpret_cast<System::PCardinal>(pas::byte_offset(Value.address, offsetof(TUInt64Words, Low)))) ^ aGalaxy::NextStateXorMask(Seed));
            pas::store_unaligned<std::uint32_t>(reinterpret_cast<System::PCardinal>(pas::byte_offset(Value.address, offsetof(TUInt64Words, High))), pas::load_unaligned<std::uint32_t>(reinterpret_cast<System::PCardinal>(pas::byte_offset(Value.address, offsetof(TUInt64Words, High)))) ^ aGalaxy::NextStateXorMask(Seed));
        };
        auto XorStateUInt32 = [&](pas::Var<std::uint32_t> Value) -> void {
            pas::store_unaligned<std::uint32_t>(Value.address, pas::load_unaligned<std::uint32_t>(Value.address) ^ aGalaxy::NextStateXorMask(Seed));
        };
        auto XorStateByte = [&](std::uint8_t& Value) -> void {
            Value = Value ^ static_cast<std::uint8_t>(aGalaxy::NextStateXorMask(Seed));
        };
        auto XorStateWords = [&](WindowsSdk::PWORD Data, std::uint32_t Count) -> void {
            std::int32_t I = 0;
            while (static_cast<std::uint32_t>(I) < Count) {
                pas::store_unaligned<std::uint16_t>(Data, static_cast<std::uint16_t>(pas::load_unaligned<std::uint16_t>(Data) ^ static_cast<std::uint16_t>(aGalaxy::NextStateXorMask(Seed))));
                Data = static_cast<WindowsSdk::PWORD>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Data)) + static_cast<std::int32_t>(sizeof(std::uint16_t))))));
                ++I;
            }
        };
        // Preserves the VMT pointer.
        auto XorStateObject = [&](pas::Object* Instance) -> void {
            aGalaxy::XorStateBytes(reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Instance) + 4), pas::instance_size(pas::class_type(Instance)) - 4, Seed);
        };
        // Preserves the VMT and one four-byte field.
        auto XorStateObjectExceptField = [&](pas::Object* Instance, void* ExcludedField) -> void {
            std::uint32_t Data = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Instance)) + 4;
            std::uint32_t Excluded = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ExcludedField));
            std::uint32_t Limit = Data + static_cast<std::uint32_t>(pas::instance_size(pas::class_type(Instance)) - 4);
            if (Excluded < Data || Excluded + 4 >= Limit) {
                GR_Main::RaiseWideMessage(u"-"_wref.get());
            }
            aGalaxy::XorStateBytes(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Data))), Excluded - Data, Seed);
            aGalaxy::XorStateBytes(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Excluded + 4))), Limit - (Excluded + 4), Seed);
        };
        // Preserves the VMT and sorted four-byte exclusions supplied by the parent frame.
        auto XorStateObjectExceptFields = [&](pas::Object* Instance) -> void {
            std::int32_t I{};
            std::uint32_t Data = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Instance)) + 4;
            std::uint32_t Limit = Data + static_cast<std::uint32_t>(pas::instance_size(pas::class_type(Instance)) - 4);
            if (ExclusionCount < 2) {
                GR_Main::RaiseWideMessage(u"-"_wref.get());
            }
            if (Exclusions[0] < Data || Exclusions[ExclusionCount - 1] + 4 >= Limit) {
                GR_Main::RaiseWideMessage(u"-"_wref.get());
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, ExclusionCount - 1); cpp_range.next(I); ) {
                if (Exclusions[I] < Data) {
                    GR_Main::RaiseWideMessage(u"-"_wref.get());
                }
                aGalaxy::XorStateBytes(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Data))), Exclusions[I] - Data, Seed);
                Data = Exclusions[I] + 4;
            }
            aGalaxy::XorStateBytes(reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Data))), Limit - Data, Seed);
        };
        aPlayer::TPlayer* Player = nullptr;
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&PendingEquipmentPurchasePrice)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&CurrentTurn)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&AverageRangerCapital)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&MaxRangerWealth)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&AverageRangerStrength)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&BestRangerStrength)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ChecksumScalarD0)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ChecksumScalarEC)));
        XorStateByte(*(&TechLevel));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            ExclusionCount = 0;
            Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Star->Planets));
            ++ExclusionCount;
            Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Star->Asteroids));
            ++ExclusionCount;
            Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Star->Ships));
            ++ExclusionCount;
            Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Star->Items));
            ++ExclusionCount;
            Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Star->MovingDropItems));
            ++ExclusionCount;
            XorStateObjectExceptFields(Star);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Items) - 1); cpp_range_2.next(J); ) {
                XorStateObject(pas::list_at<pas::Object>(Star->Items, J));
            }
            if (Star->MovingDropItems != nullptr) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->MovingDropItems) - 1); cpp_range_3.next(J); ) {
                    if (pas::class_cast_if<aItem::TItem*>(pas::list_at<TMovingDropItemEntry>(Star->MovingDropItems, J)->Payload) != nullptr) {
                        XorStateObject(pas::checked_cast<aItem::TItem*>(pas::list_at<TMovingDropItemEntry>(Star->MovingDropItems, J)->Payload));
                    }
                }
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Asteroids) - 1); cpp_range_4.next(J); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(Star->Asteroids, J);
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Asteroid->MineralCount)));
            }
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_5.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                XorStateObjectExceptField(Planet, &Planet->EquipmentShop);
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->EquipmentShop) - 1); cpp_range_6.next(K); ) {
                    XorStateObject(pas::list_at<pas::Object>(Planet->EquipmentShop, K));
                }
            }
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_7.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                ExclusionCount = 0;
                Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Ship->Inventory));
                ++ExclusionCount;
                Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&Ship->Artefacts));
                ++ExclusionCount;
                if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Ship)) {
                    Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&ruins->EquipmentShop));
                    ++ExclusionCount;
                }
                if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Ship)) {
                    Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&ranger->Quests));
                    ++ExclusionCount;
                }
                if (aPlayer::TPlayer* player = pas::class_cast_if<aPlayer::TPlayer*>(Ship)) {
                    Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&player->StorageEntries));
                    ++ExclusionCount;
                }
                if (aPlayer::TPlayer* player_2 = pas::class_cast_if<aPlayer::TPlayer*>(Ship)) {
                    Exclusions[ExclusionCount] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&player_2->Satellites));
                    ++ExclusionCount;
                }
                XorStateObjectExceptFields(Ship);
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range_8.next(K); ) {
                    XorStateObject(pas::list_at<pas::Object>(Ship->Inventory, K));
                }
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Artefacts) - 1); cpp_range_9.next(K); ) {
                    XorStateObject(pas::list_at<pas::Object>(Ship->Artefacts, K));
                }
                if (aRuins::TRuins* ruins_2 = pas::class_cast_if<aRuins::TRuins*>(Ship)) {
                    for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(ruins_2->EquipmentShop) - 1); cpp_range_10.next(K); ) {
                        XorStateObject(pas::list_at<pas::Object>(static_cast<pas::List*>(pas::checked_cast<aRuins::TRuins*>(Ship)->EquipmentShop), K));
                    }
                }
                if (aRanger::TRanger* ranger_2 = pas::class_cast_if<aRanger::TRanger*>(Ship)) {
                    if (ranger_2->Quests != nullptr) {
                        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(ranger_2->Quests) - 1); cpp_range_11.next(K); ) {
                            RangerQuest = pas::list_at<aRanger::TQuest>(pas::checked_cast<aRanger::TRanger*>(Ship)->Quests, K);
                            if (RangerQuest != nullptr) {
                                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&RangerQuest->DeadlineTurn)));
                                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&RangerQuest->RewardMoney)));
                            }
                        }
                    }
                }
                if (aPlayer::TPlayer* player_3 = pas::class_cast_if<aPlayer::TPlayer*>(Ship)) {
                    Player = player_3;
                    for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, pas::list_count(Player->StorageEntries) - 1); cpp_range_12.next(K); ) {
                        Storage = pas::list_at<aPlayer::TStorageEntry>(reinterpret_cast<aPlayer::TPlayer*>(Ship)->StorageEntries, K);
                        XorStateObject(Storage->Item);
                    }
                    for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, pas::list_count(reinterpret_cast<aPlayer::TPlayer*>(Ship)->Satellites) - 1); cpp_range_13.next(K); ) {
                        XorStateObject(pas::list_at<pas::Object>(static_cast<pas::List*>(reinterpret_cast<aPlayer::TPlayer*>(Ship)->Satellites), K));
                    }
                }
            }
        }
        if (Player != nullptr) {
            if (Player->IsOnPlanet() || Player->IsDockedToShip()) {
                if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                    for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_14.next(I); ) {
                        if (pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I)->Item != nullptr) {
                            XorStateObject(pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I)->Item);
                        }
                    }
                }
            }
        }
        if (Player != nullptr) {
            if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanetQuest) {
                XorStateUInt32(pas::Var<std::uint32_t>(&Globals::PlanetQuestScreen->MoneyLimitComplement));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::PlanetQuestScreen->DaysElapsed)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::PlanetQuestScreen->QuestId)));
                Quest = Globals::PlanetQuestScreen->Quest;
                if (Quest != nullptr) {
                    for (auto cpp_range_15 = pas::for_to<std::int32_t>(1, Quest->GetParameterCount()); cpp_range_15.next(J); ) {
                        Parameter = Quest->GetParameter(J);
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Parameter->MinValue)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Parameter->MaxValue)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Parameter->Value)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Parameter->CriticalOutcome)));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Parameter->Hidden));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Parameter->ShowWhenZero));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Parameter->CriticalAtMinimum));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Parameter->Enabled));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Parameter->IsMoney));
                    }
                    for (auto cpp_range_16 = pas::for_to<std::int32_t>(1, Quest->GetLocationCount()); cpp_range_16.next(J); ) {
                        Location = Quest->GetLocation(J);
                        XorStateWords(reinterpret_cast<WindowsSdk::PWORD>(Location->EventExpression->Text.pchar()), Location->EventExpression->Text.length());
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Location->Days)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Location->Id)));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Location->UseEventExpression));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Location->NextEventIndex)));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Location->IsEmpty));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Location->IsDeath));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Location->IsStart));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Location->IsSuccess));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Location->IsFailure));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Location->VisitLimit)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Location->VisitCount)));
                        for (auto cpp_range_17 = pas::for_to<std::int32_t>(1, Location->GetParameterChangeCount()); cpp_range_17.next(K); ) {
                            Change = Location->GetParameterChange(K);
                            XorStateWords(reinterpret_cast<WindowsSdk::PWORD>(Change->ExpressionText->Text.pchar()), Change->ExpressionText->Text.length());
                            for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, Change->ValueConstraint->Values.length() - 1); cpp_range_18.next(L); ) {
                                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->ValueConstraint->Values[L])));
                            }
                            for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, Change->MultipleConstraint->Values.length() - 1); cpp_range_19.next(L); ) {
                                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->MultipleConstraint->Values[L])));
                            }
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->MinValue)));
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->MaxValue)));
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->ChangeValue)));
                            XorStateByte(*reinterpret_cast<std::uint8_t*>(&Change->ChangeByPercent));
                            XorStateByte(*reinterpret_cast<std::uint8_t*>(&Change->SetValue));
                            XorStateByte(*reinterpret_cast<std::uint8_t*>(&Change->UseExpression));
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->VisibilityChange)));
                        }
                    }
                    for (auto cpp_range_20 = pas::for_to<std::int32_t>(1, Quest->GetPathCount()); cpp_range_20.next(J); ) {
                        Path = Quest->GetPath(J);
                        XorStateWords(reinterpret_cast<WindowsSdk::PWORD>(Path->Caption->Text.pchar()), Path->Caption->Text.length());
                        XorStateWords(reinterpret_cast<WindowsSdk::PWORD>(Path->ConditionExpression->Text.pchar()), Path->ConditionExpression->Text.length());
                        XorStateUInt64(pas::Var<std::uint64_t>(reinterpret_cast<PUInt64>(&Path->Priority)));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Path->IsAutomatic));
                        XorStateByte(*reinterpret_cast<std::uint8_t*>(&Path->AlwaysShow));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->Days)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->DisplayOrder)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->Id)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->TraversalLimit)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->TraversalCount)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->FromLocationId)));
                        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Path->ToLocationId)));
                        for (auto cpp_range_21 = pas::for_to<std::int32_t>(1, Path->GetParameterChangeCount()); cpp_range_21.next(K); ) {
                            Change = Path->GetParameterChange(K);
                            XorStateWords(reinterpret_cast<WindowsSdk::PWORD>(Change->ExpressionText->Text.pchar()), Change->ExpressionText->Text.length());
                            for (auto cpp_range_22 = pas::for_to<std::int32_t>(0, Change->ValueConstraint->Values.length() - 1); cpp_range_22.next(L); ) {
                                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->ValueConstraint->Values[L])));
                            }
                            for (auto cpp_range_23 = pas::for_to<std::int32_t>(0, Change->MultipleConstraint->Values.length() - 1); cpp_range_23.next(L); ) {
                                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->MultipleConstraint->Values[L])));
                            }
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->MinValue)));
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->MaxValue)));
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->ChangeValue)));
                            XorStateByte(*reinterpret_cast<std::uint8_t*>(&Change->ChangeByPercent));
                            XorStateByte(*reinterpret_cast<std::uint8_t*>(&Change->SetValue));
                            XorStateByte(*reinterpret_cast<std::uint8_t*>(&Change->UseExpression));
                            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Change->VisibilityChange)));
                        }
                    }
                }
            }
        }
        if (GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle && ab_Ship::PlayerArcadeShip != nullptr) {
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Health)));
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->MaxHealth)));
            for (auto cpp_range_24 = pas::for_to<std::int32_t>(0, 4); cpp_range_24.next(I); ) {
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].Ammo)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].MaxAmmo)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].RechargePerTick)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].AmmoCost)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].LastFireTick)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].FireIntervalTicks)));
                XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Weapons[I].Damage)));
            }
        }
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::ShipScreen->SelectedHoldKind)));
        XorStateByte(*(&Globals::ShipScreen->SelectedGoodsIndex));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::ShipScreen->SelectedGoodsQuantity)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::ShipScreen->SelectedGoodsCost)));
        if (GlobalsV::CurrentScreenId == GlobalsV::screenShip && pas::is_one_of<fShip2::phkEquipment, fShip2::phkArtefact>(Globals::ShipScreen->SelectedHoldKind) && Globals::ShipScreen->SelectedHoldItem != nullptr) {
            XorStateObject(Globals::ShipScreen->SelectedHoldItem);
        }
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GoodsShopScreen->PartnerCargoLimit)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GoodsShopScreen->PartnerMoneyLimit)));
        for (auto cpp_range_25 = pas::for_to<std::uint8_t>(0, 7); cpp_range_25.next(Good); ) {
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GoodsShopScreen->TradeRows[Good].Count)));
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GoodsShopScreen->TradeRows[Good].MaximumPrice)));
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GoodsShopScreen->TradeRows[Good].PurchasePrice)));
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GoodsShopScreen->TradeRows[Good].BaseSalePrice)));
        }
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GovernmentScreen->QuestOffer.DeadlineTurn)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GovernmentScreen->QuestOffer.RewardMoney)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GovernmentScreen->QuestNegotiationLevel)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GovernmentScreen->QuestRewardStep)));
        XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&Globals::GovernmentScreen->QuestDurationStep)));
        if (ab_Ship::PlayerArcadeShip != nullptr) {
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->Health)));
            XorStateUInt32(pas::Var<std::uint32_t>(reinterpret_cast<System::PCardinal>(&ab_Ship::PlayerArcadeShip->MaxHealth)));
        }
        aGalaxy::XorStateBytes(reinterpret_cast<std::uint8_t*>(&aConst::IntegrityDataBegin), static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&aConst::IntegrityDataEnd)) - static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&aConst::IntegrityDataBegin)), Seed);
    }

    std::uint32_t NextStateXorMask(std::int32_t& Seed) {
        {
            std::int32_t cpp_left = EC_Buf::SeedRngMultiplier * pas::imod(Seed, EC_Buf::SeedRngQuotient);
            Seed = cpp_left - EC_Buf::SeedRngRemainder * pas::idiv(Seed, EC_Buf::SeedRngQuotient);
        }
        if (Seed <= 0) {
            Seed += EC_Buf::SeedRngModulus;
        }
        return Seed - 1;
    }

    void XorStateBytes(std::uint8_t* Data, std::uint32_t Count, std::int32_t& Seed) {
        std::int32_t I = 0;
        while (static_cast<std::uint32_t>(I) < Count) {
            *Data = *Data ^ static_cast<std::uint8_t>(aGalaxy::NextStateXorMask(Seed));
            Data = static_cast<std::uint8_t*>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Data)) + static_cast<std::int32_t>(sizeof(std::uint8_t))))));
            ++I;
        }
    }

    // Already-obfuscated state is left unchanged.
    void TGalaxy::ObfuscateProtectedState() {
        if (GR_Main::CCInterface->GetProtectedStateXorSeed() == 0) {
            do {
                std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, 2000000000);
                GR_Main::TCCInterface* ccInterface = GR_Main::CCInterface;
                ccInterface->SetProtectedStateXorSeed(randomIntRange);
            } while (!(GR_Main::CCInterface->GetProtectedStateXorSeed() != 0));
            XorProtectedState(GR_Main::CCInterface->GetProtectedStateXorSeed());
        }
    }

    void TGalaxy::RestoreProtectedState() {
        if (GR_Main::CCInterface->GetProtectedStateXorSeed() != 0) {
            XorProtectedState(GR_Main::CCInterface->GetProtectedStateXorSeed());
            GR_Main::CCInterface->SetProtectedStateXorSeed(0);
        }
    }

    // An unconditional jump disables the checksum body; always returns zero in this binary.
    std::uint32_t TGalaxy::ComputeIntegrityChecksum(std::int32_t Mode) {
        return 0u;
    }

    void TGalaxy::PrimeIntegrityChecksum(std::int32_t StatusCode) {
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            aCalc::WaitForTurnCalculationUI();
            GR_Main::CCInterface->SetIntegrityChecksum(TGalaxy::ComputeIntegrityChecksum(0));
            GR_Main::CCInterface->SetIntegrityStatus(StatusCode);
        }
    }

    void TGalaxy::PrimeIntegrityChecksum1(std::int32_t StatusCode) {
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            aCalc::WaitForTurnCalculationUI();
            GR_Main::CCInterface->SetIntegrityChecksum1(TGalaxy::ComputeIntegrityChecksum(1));
            GR_Main::CCInterface->SetIntegrityStatus(StatusCode);
        }
    }

    void TGalaxy::PrimeIntegrityChecksum2(std::int32_t StatusCode) {
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            aCalc::WaitForTurnCalculationUI();
            GR_Main::CCInterface->SetIntegrityChecksum2(TGalaxy::ComputeIntegrityChecksum(2));
            GR_Main::CCInterface->SetIntegrityStatus(StatusCode);
        }
    }

    // A matching checksum clears the integrity status.
    void TGalaxy::CheckIntegrityChecksum(std::int32_t ErrorCode) {
        std::int32_t ResourceError{};
        aCalc::WaitForTurnCalculationUI();
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            if (GR_Main::CCInterface->GetResourceChecksumFailed()) {
                ResourceError = 0x001347cc;
                ResourceError += 0x000b3cb4;
                GR_Main::CCInterface->SetIntegrityError(ResourceError);
            } else if (GR_Main::CCInterface->GetIntegrityStatus() != 0) {
                if (TGalaxy::ComputeIntegrityChecksum(0) != GR_Main::CCInterface->GetIntegrityChecksum()) {
                    GR_Main::CCInterface->SetIntegrityError(ErrorCode);
                } else {
                    GR_Main::CCInterface->SetIntegrityStatus(0);
                }
            }
        }
    }

    // Channel zero; unlike CheckIntegrityChecksum, a matching checksum stores StatusCode rather than zero.
    void TGalaxy::CheckIntegrityChecksumAndSetStatus(std::int32_t StatusCode) {
        std::int32_t ResourceError{};
        aCalc::WaitForTurnCalculationUI();
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            if (GR_Main::CCInterface->GetResourceChecksumFailed()) {
                ResourceError = 0x001347cc;
                ResourceError += 0x000b3cb4;
                GR_Main::CCInterface->SetIntegrityError(ResourceError);
            } else if (GR_Main::CCInterface->GetIntegrityStatus() != 0) {
                if (TGalaxy::ComputeIntegrityChecksum(0) != GR_Main::CCInterface->GetIntegrityChecksum()) {
                    GR_Main::CCInterface->SetIntegrityError(StatusCode);
                } else {
                    GR_Main::CCInterface->SetIntegrityStatus(StatusCode);
                }
            }
        }
    }

    void TGalaxy::CheckIntegrityChecksum1(std::int32_t ErrorCode) {
        std::int32_t ResourceError{};
        aCalc::WaitForTurnCalculationUI();
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            if (GR_Main::CCInterface->GetResourceChecksumFailed()) {
                ResourceError = 0x001347cc;
                ResourceError += 0x000b3cb4;
                GR_Main::CCInterface->SetIntegrityError(ResourceError);
            } else if (GR_Main::CCInterface->GetIntegrityStatus() != 0) {
                if (TGalaxy::ComputeIntegrityChecksum(1) != GR_Main::CCInterface->GetIntegrityChecksum1()) {
                    GR_Main::CCInterface->SetIntegrityError(ErrorCode);
                } else {
                    GR_Main::CCInterface->SetIntegrityStatus(0);
                }
            }
        }
    }

    void TGalaxy::CheckIntegrityChecksum2(std::int32_t ErrorCode) {
        std::int32_t ResourceError{};
        aCalc::WaitForTurnCalculationUI();
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            if (GR_Main::CCInterface->GetResourceChecksumFailed()) {
                ResourceError = 0x001347cc;
                ResourceError += 0x000b3cb4;
                GR_Main::CCInterface->SetIntegrityError(ResourceError);
            } else if (GR_Main::CCInterface->GetIntegrityStatus() != 0) {
                if (TGalaxy::ComputeIntegrityChecksum(2) != GR_Main::CCInterface->GetIntegrityChecksum2()) {
                    GR_Main::CCInterface->SetIntegrityError(ErrorCode);
                } else {
                    GR_Main::CCInterface->SetIntegrityStatus(0);
                }
            }
        }
    }

    void TGalaxy::ClearIntegrityStatus() {
        if (GR_Main::CCInterface->GetIntegrityError() == 0) {
            GR_Main::CCInterface->SetIntegrityStatus(0);
        }
    }

    // An unconditional jump disables the snapshot body; no-op in this binary.
    void TGalaxy::AppendIntegritySnapshot() {
        FGInt::TFGInt Exponent{};
        FGInt::TFGInt Modulus{};
        pas::AnsiString Bytes{};
        return;
    }

    std::uint8_t TGalaxy::HasVisibleScoreModFlags() {
        std::uint8_t Result = true;
        if (DominatorModLevel == 0 && TechnicModEnabled == 0 && AmmoModEnabled == 0 && GodModEnabled == 0 && UltraScanModEnabled == 0 && StasisModEnabled == 0 && static_cast<std::uint8_t>(GR_Main::CCInterface->GetEditableStateApplied() ^ 1)) {
            return false;
        }
        return Result;
    }

    std::int32_t TGalaxy::GetCheatPoints() {
        std::int32_t cpp_left = GR_Main::CCInterface->GetEncodedCheatPoints();
        return cpp_left ^ GenerationSeed;
    }

    void TGalaxy::SetCheatPoints(std::int32_t Value) {
        GR_Main::CCInterface->SetEncodedCheatPoints(Value ^ GenerationSeed);
    }

    void THole_Create(THole* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (Galaxy != nullptr) {
            Self->Id = Galaxy->NextHoleId;
            ++Galaxy->NextHoleId;
        }
    }

    void THole_Destroy(THole* Self) {
        if (Self->Graphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Empty GraphKey chooses a seeded Hole template. Clears ArcadeMapName; replacing an existing Graphic does not release the old reference.
    void THole::InitializeGraphic(pas::WideString GraphKey) {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Key{};
        Key = std::move(GraphKey);
        if (Key == u"") {
            Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Hole"_wref.get());
            Key = pas::concat_wide({u"Hole.", Block->GetBlockNameByIndex(aMyFunction::SeededRandomIntRange(0, Block->GetBlockCount() - 1, Id + Galaxy->CurrentTurn))});
        }
        {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Hole"sv, Key, ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
        }
        Graphic->SetPosition(EC_Struct::MakePointF(0.0f, 0.0f));
        ArcadeMapName = pas::WideString();
    }

    // Requires both endpoint stars and Graphic.
    void THole::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddDWord(Id);
        Buffer->AddDWord(Star1->Id);
        Buffer->AddSingle(Position1.X);
        Buffer->AddSingle(Position1.Y);
        Buffer->AddDWord(Star2->Id);
        Buffer->AddSingle(Position2.X);
        Buffer->AddSingle(Position2.Y);
        Buffer->AddIntegerValue(CreatedTurn);
        Buffer->AddIntegerValue(HoleType);
        Buffer->AddWideStringZ(Graphic->GraphKey);
        Buffer->AddWideStringZ(ArcadeMapName);
    }

    // Leaves endpoint IDs unresolved until ResolveLoadedReferences.
    void THole::LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy) {
        Id = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Galaxy->NextHoleId <= Id) {
            Galaxy->NextHoleId = Id + 1;
        }
        Star1 = reinterpret_cast<TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        Position1.X = EC_Buf::TBufEC_GetSingle(Buffer);
        Position1.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        Star2 = reinterpret_cast<TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        Position2.X = EC_Buf::TBufEC_GetSingle(Buffer);
        Position2.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        CreatedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        HoleType = EC_Buf::TBufEC_GetInt32(Buffer);
        {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Hole"sv, Buffer->ReadWideString(), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
        }
        Graphic->SetPosition(EC_Struct::MakePointF(0.0f, 0.0f));
        ArcadeMapName = Buffer->ReadWideString();
    }

    void THole::ResolveLoadedReferences(TGalaxy* Galaxy) {
        Star1 = pas::checked_cast<TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Star1)))));
        Star2 = pas::checked_cast<TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Star2)))));
    }

    // Editable subset; writes remaining lifetime as CreatedTurn + 200 - Galaxy.CurrentTurn.
    void THole::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        {
            const pas::WideString& int64ToStr = pas::wide_int64_to_str(static_cast<std::int64_t>(Star1->Id));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Skt5adrs1tI2dx"_w);
            Block->AddParam(decodeTextW, int64ToStr);
        }
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Position1.X), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"S0tua4rw1gCjotoerwd4Xw"_w);
            Block->AddParam(decodeTextW_2, cpp_arg);
        }
        {
            const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Position1.Y), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"S3t4agrj1kCworour4ddYx"_w);
            Block->AddParam(decodeTextW_3, cpp_arg_2);
        }
        {
            const pas::WideString& int64ToStr_2 = pas::wide_int64_to_str(static_cast<std::int64_t>(Star2->Id));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"Sltkalru2tIrdd"_w);
            Block->AddParam(decodeTextW_4, int64ToStr_2);
        }
        {
            const pas::WideString& cpp_arg_3 = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Position2.X), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"Sstfawrr2tC4oyojr7dkX"_w);
            Block->AddParam(decodeTextW_5, cpp_arg_3);
        }
        {
            const pas::WideString& cpp_arg_4 = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Position2.Y), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"S2tga5rg2wCxobokrFdsYA"_w);
            Block->AddParam(decodeTextW_6, cpp_arg_4);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(CreatedTurn + 200 - Galaxy->CurrentTurn);
            const pas::WideString& decodeTextW_7 = EC_Str::DecodeTextW(u"TtuwrdngshT4oaC2l5ojsden"_w);
            Block->AddParam(decodeTextW_7, intToStr);
        }
        Block->AddParam(EC_Str::DecodeTextW(u"MpaypeNqazmveR"_w), ArcadeMapName);
    }

    // Updates endpoints, positions, remaining lifetime and ArcadeMapName; preserves Id, HoleType and Graphic.
    void THole::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        Star1 = Galaxy->IdToStar(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Skt5adrs1tI2dx"_w))))));
        Position1.X = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"S0tua4rw1gCjotoerwd4Xw"_w))));
        Position1.Y = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"S3t4agrj1kCworour4ddYx"_w))));
        Star2 = Galaxy->IdToStar(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Sltkalru2tIrdd"_w))))));
        Position2.X = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Sstfawrr2tC4oyojr7dkX"_w))));
        Position2.Y = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"S2tga5rg2wCxobokrFdsYA"_w))));
        CreatedTurn = Galaxy->CurrentTurn - 200 + SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"TtuwrdngshT4oaC2l5ojsden"_w)))));
        ArcadeMapName = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"MpaypeNqazmveR"_w)));
    }

    void TCustomSystemInfo_Create(TCustomSystemInfo* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TCustomSystemInfo_Destroy(TCustomSystemInfo* Self) {
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TCustomSystemInfo::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        Name = Buffer->ReadWideString();
        Icon = Buffer->ReadWideString();
        Info = Buffer->ReadWideString();
        TypeTag = Buffer->ReadWideString();
        Distance = EC_Buf::TBufEC_GetInt32(Buffer);
    }

    void TCustomSystemInfo::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(Name);
        Buffer->AddWideStringZ(Icon);
        Buffer->AddWideStringZ(Info);
        Buffer->AddWideStringZ(TypeTag);
        Buffer->AddIntegerValue(Distance);
    }

    void TStar_Create(TStar* Self) {
        pas::object_create(Self);
        if (Galaxy != nullptr) {
            Self->Id = Galaxy->NextStarId;
            ++Galaxy->NextStarId;
            Self->GenerationSeed = aMyFunction::NextRandomIntRange(100000, SystemImports::MaxInt, Galaxy->RandomState);
        }
        Self->RandomState = Self->GenerationSeed;
        Self->Planets = pas::make_object<aMyFunction::TObjectList>();
        Self->Asteroids = pas::make_object<aMyFunction::TObjectList>();
        Self->Ships = pas::make_object<aMyFunction::TObjectList>();
        Self->Items = pas::make_object<aMyFunction::TObjectList>();
        Self->MovingDropItems = pas::make_object<pas::List>();
        Self->Missiles = pas::make_object<aMyFunction::TObjectList>();
        Self->PlayerCombatOccurred = false;
        Self->InterruptLongTravel = false;
        Self->KeepFilmRunning = false;
        Self->Reserved64 = 0;
        Self->Dominion = nullptr;
        Self->Status.FactionStrengthCacheTurn = 0;
        Self->CustomSystemInfos = pas::make_object<aMyFunction::TObjectList>();
        Self->CombatEvents = pas::make_object<pas::List>();
        Self->PendingFilmObjectRemovals = pas::make_object<pas::List>();
        Self->ReferencedItems = pas::make_object<pas::List>();
        Self->PlayerFilmPath = nullptr;
        Self->RecordingTurnFilm = false;
    }

    void TStar_Destroy(TStar* Self) {
        std::int32_t I{};
        PMovingDropItemEntry Entry{};
        if (Self->Graphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->MovingDropItems) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TMovingDropItemEntry>(Self->MovingDropItems, I);
            if (Entry->Payload != nullptr) {
                pas::free(Entry->Payload);
            }
            Entry->Payload = nullptr;
            EC_Mem::FreeEC(Entry);
        }
        pas::free(Self->MovingDropItems);
        Self->MovingDropItems = nullptr;
        pas::free(Self->Items);
        Self->Items = nullptr;
        pas::free(Self->Missiles);
        Self->Missiles = nullptr;
        pas::free(Self->Ships);
        Self->Ships = nullptr;
        pas::free(Self->Planets);
        Self->Planets = nullptr;
        pas::free(Self->Asteroids);
        Self->Asteroids = nullptr;
        pas::free(Self->CustomSystemInfos);
        Self->CustomSystemInfos = nullptr;
        pas::free(Self->CombatEvents);
        Self->CombatEvents = nullptr;
        pas::free(Self->PendingFilmObjectRemovals);
        Self->PendingFilmObjectRemovals = nullptr;
        pas::free(Self->ReferencedItems);
        Self->ReferencedItems = nullptr;
        pas::object_destroy(Self);
    }

    void TStar::GenerateSystemContents(std::uint8_t TerronSystem) {
        std::int32_t I{};
        std::int32_t Variant{};
        std::int32_t Tries{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        std::int32_t TotalPlanets{};
        std::int32_t Inhabited{};
        pas::WideString Text{};
        EC_BlockPar::TBlockParEC* Definition{};
        std::int32_t NameIndex = ([&] {
            std::int32_t cpp_left = pas::list_indexof(Galaxy->Stars, reinterpret_cast<void*>(this));
            return pas::imod(cpp_left, GR_Main::LanguageDataConfig->GetBlock(u"Star"sv)->GetParamCount());
        }());
        Text = GR_Main::LanguageDataConfig->GetBlock(u"Star"sv)->GetParamValue(NameIndex);
        Name = EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv);
        if (TerronSystem) {
            Definition = GR_Main::GameDataConfig->GetBlockByPath(u"Star.Terron"_wref.get());
        } else if (pas::list_indexof(Galaxy->Stars, reinterpret_cast<void*>(this)) == 2) {
            Definition = GR_Main::GameDataConfig->GetBlockByPath(u"Star.04"_wref.get());
        } else {
            if (BackgroundImage < 10) {
                Text = ([&] {
                    const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(BackgroundImage)}));
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"StyleStar"_wref.get());
                    return blockByPath->GetParam(pas::view(cpp_arg));
                }());
            } else {
                Text = ([&] {
                    const pas::WideString& intToStr = pas::wide_int_to_str(BackgroundImage);
                    EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(u"StyleStar"_wref.get());
                    return blockByPath_2->GetParam(pas::view(intToStr));
                }());
            }
            Definition = GR_Main::GameDataConfig->GetBlockByPath(u"Star"_wref.get());
            Variant = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Definition->GetBlockCount() - 1); cpp_range.next(I); ) {
                if (EC_Str::FindTextPosW(Definition->GetBlockNameByIndex(I), Text) > 0) {
                    Variant += EC_Str::ExtractDigitsToIntW(pas::view(Definition->GetBlockByIndex(I)->GetParam(u"Priority"sv)));
                }
            }
            Variant = aMyFunction::RandomIntRange(0, Variant - 1);
            I = 0;
            while (I < Definition->GetBlockCount()) {
                if (EC_Str::FindTextPosW(Definition->GetBlockNameByIndex(I), Text) > 0) {
                    Variant -= EC_Str::ExtractDigitsToIntW(pas::view(Definition->GetBlockByIndex(I)->GetParam(u"Priority"sv)));
                    if (Variant < 0) {
                        break;
                    }
                }
                ++I;
            }
            if (I >= Definition->GetBlockCount()) {
                GR_Main::RaiseWideMessage(u"Star.Init"_wref.get());
            }
            Definition = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"Star.", Definition->GetBlockNameByIndex(I)}));
        }
        Radius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Definition->GetParam(u"Radius"sv)));
        SafeRadius = EC_Str::ExtractDecimalToSingleW(Definition->GetParam(u"SafeRadius"sv));
        DamageRadius = EC_Str::ExtractDecimalToSingleW(Definition->GetParam(u"DamageRadius"sv));
        SystemRadius = Radius;
        {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Star"sv, Definition->GetParam(u"SEGraph"sv), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
        }
        SystemProcessName = Definition->GetParam(u"SEProcess"sv);
        Graphic->SetPosition(EC_Struct::MakePointF(0.0f, 0.0f));
        if (pas::list_indexof(Galaxy->Stars, reinterpret_cast<void*>(this)) == 2) {
            TotalPlanets = 7;
            Inhabited = 0;
        } else if (pas::list_indexof(Galaxy->Stars, reinterpret_cast<void*>(this)) < 5) {
            TotalPlanets = 6;
            Inhabited = 3;
        } else if (pas::list_indexof(Galaxy->Stars, reinterpret_cast<void*>(this)) == 70) {
            TotalPlanets = 3;
            Inhabited = 10;
        } else if (pas::list_indexof(Galaxy->Stars, reinterpret_cast<void*>(this)) == 71) {
            TotalPlanets = 5;
            Inhabited = 11;
        } else {
            TotalPlanets = aMyFunction::NextRandomIntRange(3, 6, RandomState);
            Inhabited = System::Round(TotalPlanets / 2 + aMyFunction::NextRandomIntRange(0, 1, RandomState));
            if (Inhabited > pas::real_divide(2 * TotalPlanets, 3.0L)) {
                Inhabited = System::Round(pas::real_divide(2 * TotalPlanets, 3.0L));
            }
            if (Inhabited > 3) {
                Inhabited = 3;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, TotalPlanets); cpp_range_2.next(I); ) {
            Planet = pas::construct_call<aPlanet::TPlanet>(aPlanet::TPlanet_Create);
            Planet->InitGenerated(this, TotalPlanets, Inhabited);
            pas::list_add(Planets, reinterpret_cast<void*>(Planet));
            pas::list_add(Galaxy->Planets, reinterpret_cast<void*>(Planet));
        }
        Status.ControlFaction = aGalaxyStruct::sfCoalition;
        Status.PreviousControlFaction = Status.ControlFaction;
        Status.Battle = 0;
        Status.DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(aMyFunction::NextRandomIntRange(0, 2, RandomState));
        Flag80 = 0;
        LastDominatorPresenceTurn = 0;
        LastPiratePresenceTurn = 0;
        LastLiberationRewardsTurn = 0;
        LiberationRewardsPending = false;
        if (BackgroundImage < 10) {
            Text = ([&] {
                const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(BackgroundImage)}));
                EC_BlockPar::TBlockParEC* blockByPath_3 = GR_Main::GameDataConfig->GetBlockByPath(u"StyleAsteroid"_wref.get());
                return blockByPath_3->GetParam(pas::view(cpp_arg_2));
            }());
        } else {
            Text = ([&] {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(BackgroundImage);
                EC_BlockPar::TBlockParEC* blockByPath_4 = GR_Main::GameDataConfig->GetBlockByPath(u"StyleAsteroid"_wref.get());
                return blockByPath_4->GetParam(pas::view(intToStr_2));
            }());
        }
        I = 2 * aMyFunction::NextRandomIntRange(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) / 2 - 1, RandomState);
        std::int32_t Variants = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), I + 1, u","sv)));
        Text = EC_Str::ExtractDelimitedPartW(pas::view(Text), I, u","sv);
        pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(8, 10, RandomState);
        std::int32_t AsteroidCount = System::Round(cpp_left_2 * Galaxy->GetAsteroidModifier());
        if (Constellation->Id == 20) {
            AsteroidCount += 30;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, AsteroidCount - 1); cpp_range_3.next(I); ) {
            Asteroid = pas::construct_call<aAsteroid::TAsteroid>(aAsteroid::TAsteroid_Create);
            Tries = 10;
            Variant = 0;
            while (Tries > 0) {
                Variant = aMyFunction::NextRandomIntRange(0, Variants - 1, RandomState);
                J = 0;
                while (J < pas::list_count(Asteroids)) {
                    if (EC_Str::ExtractDigitsToIntW(pas::view(pas::list_at<aAsteroid::TAsteroid>(Asteroids, J)->GraphObject->GraphKey)) == Variant) {
                        break;
                    }
                    ++J;
                }
                if (J >= pas::list_count(Asteroids)) {
                    break;
                }
                --Tries;
            }
            if (Variant < 10) {
                Asteroid->Init(this, pas::concat_wide({u"Asteroid.", Text, u"0", pas::wide_int_to_str(Variant)}));
            } else {
                Asteroid->Init(this, pas::concat_wide({u"Asteroid.", Text, pas::wide_int_to_str(Variant)}));
            }
            pas::list_add(Asteroids, reinterpret_cast<void*>(Asteroid));
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 300); cpp_range_4.next(Variant); ) {
                Asteroid->IntegrateMotion(2.0E+1f);
            }
        }
        PlayerPresenceLevel = 0;
        DaysSincePlayerVisit = 100;
        DaysSinceLastNpcShipSpawn = 100;
        RefreshMapDiameterAndStats();
    }

    // Removes empty moving-drop descriptors; includes module-integrity checks.
    void TStar::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        pas::WideString FileName{};
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        PMovingDropItemEntry Drop{};
        aMissile::TMissile* Missile{};
        pas::WideString Extension{};
        pas::WideString Prefix{};
        // Caller-popped static link; filename at -4, star at -8. Updates global integrity status on mismatch.
        auto CheckModuleCRC = [&](std::uint32_t ExpectedCRC) -> void {
            EC_Buf::TBufEC* Data = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
            Data->LoadFromWideFilePath(FileName.pchar());
            Data->AddIntegerValue(426333);
            Data->AddIntegerValue(1052456);
            Data->AddIntegerValue(-346336);
            Data->AddIntegerValue(11111);
            std::int32_t Unused = 4;
            if (Data->ComputeCrc32() != ExpectedCRC) {
                ModuleCrcIntegrityStatus = mcsMismatch;
                if (this->Id == 1) {
                    ModuleCrcFailureValue = 0;
                }
            }
            pas::free(Data);
        };
        if (ModuleCrcIntegrityStatus == mcsUnchecked) {
            Extension = u"ll"_w;
            Extension = pas::concat_wide({u".d", Extension});
            FileName = pas::concat_wide({EC_Str::DecodeTextW(u"sotoenalm^_^aucah"_w), Extension});
            if (WindowsSdk::GetModuleHandleW(FileName.pchar()) != 0) {
                CheckModuleCRC(0xa5ea67a9u);
            }
            FileName = pas::concat_wide({EC_Str::DecodeTextW(u"sotoenalm^_^aupki"_w), Extension});
            if (WindowsSdk::GetModuleHandleW(FileName.pchar()) != 0) {
                CheckModuleCRC(0xfd0a392fu);
            }
            FileName = pas::concat_wide({EC_Str::DecodeTextW(u"zoloimba"_w), Extension});
            CheckModuleCRC(0x429862e3u);
            FileName = pas::concat_wide({EC_Str::DecodeTextW(u"MhastorhinxaGrakmae"_w), Extension});
            CheckModuleCRC(0xfaff5f87u);
            FileName = pas::concat_wide({EC_Str::DecodeTextW(u"ookogifa"_w), Extension});
            CheckModuleCRC(0xd027cdf5u);
            FileName = pas::concat_wide({EC_Str::DecodeTextW(u"xavriadeccomrie"_w), Extension});
            CheckModuleCRC(0xb7c65763u);
            Prefix = u"ib"_w;
            Prefix = pas::concat_wide({u"l", Prefix});
            FileName = pas::concat_wide({Prefix, EC_Str::DecodeTextW(u"osgaga-10a"_w), Extension});
            CheckModuleCRC(0x3c9cd24cu);
            FileName = pas::concat_wide({Prefix, EC_Str::DecodeTextW(u"vrokrablius-->0"_w), Extension});
            CheckModuleCRC(0xe1ca75c7u);
            FileName = pas::concat_wide({Prefix, EC_Str::DecodeTextW(u"veohrablissufainlae"_w), Extension});
            CheckModuleCRC(0xd1ed59c5u);
            if (ModuleCrcIntegrityStatus == mcsUnchecked) {
                ModuleCrcIntegrityStatus = mcsAccepted;
            }
        }
        Buffer->AddDWord(Id);
        Buffer->AddIntegerValue(GenerationSeed);
        Buffer->AddDWord(RandomState);
        Buffer->AddWideStringZ(Name);
        Buffer->AddSingle(Position.X);
        Buffer->AddSingle(Position.Y);
        Buffer->AddWideChar(SystemRadius);
        Buffer->AddAnsiChar(BackgroundImage);
        std::int32_t Count = pas::list_count(Planets);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            Planet->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Asteroids);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, I);
            Asteroid->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Ships);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (pas::class_cast_if<aPlayer::TPlayer*>(Ship) != nullptr) {
                Buffer->AddAnsiChar(255);
            } else {
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Ship->TypeId));
            }
            Ship->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Items);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Items, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        {
            const std::int32_t cpp_first = pas::list_count(MovingDropItems) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Drop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, I);
                    if (Drop->Payload == nullptr) {
                        pas::list_delete(MovingDropItems, I);
                        EC_Mem::FreeEC(Drop);
                    }
                }
            }
        }
        Count = pas::list_count(MovingDropItems);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
            Drop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, I);
            Buffer->AddSingle(Drop->Destination.X);
            Buffer->AddSingle(Drop->Destination.Y);
            Buffer->AddDWord(Drop->SourceShipId);
            Buffer->AddBoolean(Drop->DeployTranclucator);
            Item = pas::checked_cast<aItem::TItem*>(Drop->Payload);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        Count = pas::list_count(Missiles);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Missiles, I);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Missile->ItemType));
            Missile->SaveToBuffer(Buffer);
        }
        Buffer->AddDWord(Constellation->Id);
        Buffer->AddWideStringZ(SystemProcessName);
        Buffer->AddBoolean(Status.Battle);
        Buffer->AddAnsiChar(Status.ThreatLevel);
        Buffer->AddAnsiChar(Status.TrafficLevel);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Status.ControlFaction));
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Status.PreviousControlFaction));
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Status.DominatorSeries));
        Buffer->AddWideStringZ(Status.CustomFaction);
        Buffer->AddSingle(SafeRadius);
        Buffer->AddSingle(DamageRadius);
        Buffer->AddWideChar(Radius);
        Buffer->AddWideStringZ(Graphic->GraphKey);
        Buffer->AddBoolean(PlayerCombatOccurred);
        Buffer->AddAnsiChar(Flag80);
        Buffer->AddIntegerValue(DaysSincePlayerVisit);
        Buffer->AddIntegerValue(DaysSinceLastNpcShipSpawn);
        Buffer->AddIntegerValue(LastDominatorPresenceTurn);
        Buffer->AddIntegerValue(LastPiratePresenceTurn);
        Buffer->AddIntegerValue(LastLiberationRewardsTurn);
        Buffer->AddIntegerValue(PlayerPresenceLevel);
        Buffer->AddBoolean(NoComeKling);
        if (Dominion == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(reinterpret_cast<aShip::TShip*>(Dominion)->Id);
        }
        Buffer->AddWideStringZ(MapLabel);
        Buffer->AddWideChar(pas::list_count(CustomSystemInfos));
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(CustomSystemInfos) - 1); cpp_range_7.next(I); ) {
            pas::list_at<TCustomSystemInfo>(CustomSystemInfos, I)->SaveToBuffer(Buffer);
        }
    }

    // Appends owned objects; requires a fresh instance. References are resolved separately.
    void TStar::LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t Count{};
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        aGalaxyStruct::TShipType ShipType{};
        PMovingDropItemEntry Drop{};
        EC_BlockPar::TBlockParEC* Definition{};
        std::uint8_t Tag{};
        aMissile::TMissile* Missile{};
        TCustomSystemInfo* Info{};
        std::int32_t Stage = 0;
        try {
            Id = EC_Buf::TBufEC_GetUInt32(Buffer);
            if (Galaxy->NextStarId <= Id) {
                Galaxy->NextStarId = Id + 1;
            }
            GenerationSeed = EC_Buf::TBufEC_GetInt32(Buffer);
            RandomState = EC_Buf::TBufEC_GetUInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion < 158) {
                EC_Buf::TBufEC_GetBoolean(Buffer);
            }
            Name = Buffer->ReadWideString();
            Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            SystemRadius = EC_Buf::TBufEC_GetWord(Buffer);
            BackgroundImage = EC_Buf::TBufEC_GetByte(Buffer);
            Stage = 1;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            Stage = 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Planet = pas::construct_call<aPlanet::TPlanet>(aPlanet::TPlanet_Create);
                Planet->CurrentStar = this;
                pas::list_add(Planets, reinterpret_cast<void*>(Planet));
                aPlanet::TPlanet_LoadFromBuffer(Planet, Buffer, Galaxy);
            }
            Stage = 3;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            Stage = 4;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Asteroid = pas::construct_call<aAsteroid::TAsteroid>(aAsteroid::TAsteroid_Create);
                Asteroid->CurrentStar = this;
                pas::list_add(Asteroids, reinterpret_cast<void*>(Asteroid));
                Asteroid->LoadFromBuffer(Buffer, Galaxy);
            }
            Stage = 5;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            Stage = 6;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
                Tag = EC_Buf::TBufEC_GetByte(Buffer);
                if (Tag == 255) {
                    Ship = pas::construct_call<aPlayer::TPlayer>(aPlayer::TPlayer_Create);
                } else {
                    ShipType = static_cast<aGalaxyStruct::TShipType>(Tag);
                    Ship = aShip::CreateShipByType(ShipType);
                }
                pas::list_add(Ships, reinterpret_cast<void*>(Ship));
                Ship->CurrentStar = this;
                Ship->LoadFromBuffer(Buffer, Galaxy);
            }
            Stage = 7;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            Stage = 8;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
                pas::list_add(Items, reinterpret_cast<void*>(Item));
                Item->LoadFromBuffer(Buffer, Galaxy);
            }
            Stage = 9;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > aGalaxyStruct::MaxSavedListCount) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
                Drop = static_cast<PMovingDropItemEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TMovingDropItemEntry))));
                Drop->Destination.X = EC_Buf::TBufEC_GetSingle(Buffer);
                Drop->Destination.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                Drop->SourceShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
                Drop->InsertedIntoStar = false;
                Drop->DeployTranclucator = EC_Buf::TBufEC_GetBoolean(Buffer);
                Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
                Drop->Payload = Item;
                Item->LoadFromBuffer(Buffer, Galaxy);
                pas::list_add(MovingDropItems, static_cast<void*>(Drop));
            }
            Stage = 10;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (GlobalsV::LoadedSaveVersion <= 127) {
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(I); ) {
                    Missile = pas::construct_call<aMissile::TMissile>(aMissile::TMissile_Create);
                    pas::list_add(Missiles, reinterpret_cast<void*>(Missile));
                    Missile->LoadFromBuffer(Buffer, Galaxy);
                }
            } else {
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(I); ) {
                    if (aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)) == aConst::t_CustomWeapon) {
                        Missile = pas::construct_call<aMissile::TCustomMissile>(aMissile::TMissile_Create);
                    } else {
                        Missile = pas::construct_call<aMissile::TMissile>(aMissile::TMissile_Create);
                    }
                    pas::list_add(Missiles, reinterpret_cast<void*>(Missile));
                    Missile->LoadFromBuffer(Buffer, Galaxy);
                }
            }
            Stage = 11;
            Constellation = reinterpret_cast<TConstellation*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            SystemProcessName = Buffer->ReadWideString();
            if (GlobalsV::LoadedSaveVersion >= 141) {
                Status.Battle = EC_Buf::TBufEC_GetBoolean(Buffer);
            }
            Status.ThreatLevel = EC_Buf::TBufEC_GetByte(Buffer);
            Status.TrafficLevel = EC_Buf::TBufEC_GetByte(Buffer);
            Status.ControlFaction = static_cast<aGalaxyStruct::TStarFaction>(EC_Buf::TBufEC_GetByte(Buffer));
            if (GlobalsV::LoadedSaveVersion >= 53) {
                Status.PreviousControlFaction = static_cast<aGalaxyStruct::TStarFaction>(EC_Buf::TBufEC_GetByte(Buffer));
            } else {
                Status.PreviousControlFaction = Status.ControlFaction;
            }
            Status.DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(EC_Buf::TBufEC_GetByte(Buffer));
            Stage = 12;
            if (GlobalsV::LoadedSaveVersion >= 149) {
                Status.CustomFaction = Buffer->ReadWideString();
            } else {
                Status.CustomFaction = pas::WideString();
            }
            if (GlobalsV::LoadedSaveVersion == 149) {
                EC_Buf::TBufEC_GetByte(Buffer);
            }
            Stage = 13;
            SafeRadius = EC_Buf::TBufEC_GetSingle(Buffer);
            DamageRadius = EC_Buf::TBufEC_GetSingle(Buffer);
            Radius = EC_Buf::TBufEC_GetWord(Buffer);
            Stage = 14;
            if (GlobalsV::LoadedSaveVersion >= 154) {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Star"sv, Buffer->ReadWideString(), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
            } else {
                Definition = ([&] {
                    const pas::WideString& readWideString = Buffer->ReadWideString();
                    EC_BlockPar::TBlockParEC* gameDataConfig = GR_Main::GameDataConfig;
                    return gameDataConfig->GetBlockByPath(readWideString);
                }());
                {
                    SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Star"sv, Definition->GetParam(u"SEGraph"sv), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic_2 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                    SE_Space::RetainSpaceObject(graphic_2, createSpaceObjectByName_2);
                }
            }
            Graphic->SetPosition(EC_Struct::MakePointF(0.0f, 0.0f));
            Stage = 15;
            if (GlobalsV::LoadedSaveVersion <= 123) {
                EC_Buf::TBufEC_GetBoolean(Buffer);
            }
            PlayerCombatOccurred = EC_Buf::TBufEC_GetBoolean(Buffer);
            Flag80 = EC_Buf::TBufEC_GetByte(Buffer);
            DaysSincePlayerVisit = EC_Buf::TBufEC_GetInt32(Buffer);
            DaysSinceLastNpcShipSpawn = EC_Buf::TBufEC_GetInt32(Buffer);
            LastDominatorPresenceTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            LastPiratePresenceTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            LastLiberationRewardsTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            PlayerPresenceLevel = EC_Buf::TBufEC_GetInt32(Buffer);
            NoComeKling = EC_Buf::TBufEC_GetBoolean(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 105) {
                Dominion = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            }
            if (GlobalsV::LoadedSaveVersion >= 111) {
                MapLabel = Buffer->ReadWideString();
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(I); ) {
                    Info = pas::construct_call<TCustomSystemInfo>(TCustomSystemInfo_Create);
                    Info->LoadFromBuffer(Buffer);
                    pas::list_add(CustomSystemInfos, reinterpret_cast<void*>(Info));
                }
            }
            RefreshMovementStepParameters();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TStar.Load, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TStar::ResolveLoadedReferences(TGalaxy* Galaxy) {
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        std::int32_t I{};
        PMovingDropItemEntry Drop{};
        aMissile::TMissile* Missile{};
        std::int32_t Count = pas::list_count(Planets);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            Planet->ResolveLoadedReferences(Galaxy);
        }
        Count = pas::list_count(Ships);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            Ship->virtual_TShip_ResolveLoadedReferences(Galaxy);
        }
        Count = pas::list_count(Items);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Items, I);
            Item->ResolveLoadedReferences(Galaxy);
        }
        Count = pas::list_count(MovingDropItems);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            Drop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, I);
            pas::checked_cast<aItem::TItem*>(Drop->Payload)->ResolveLoadedReferences(Galaxy);
        }
        Count = pas::list_count(Missiles);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Missiles, I);
            Missile->ResolveLoadedReferences(Galaxy);
        }
        if (Dominion != nullptr) {
            Dominion = static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Dominion)), true));
        }
        Constellation = pas::checked_cast<TConstellation*>(static_cast<pas::Object*>(Galaxy->IdToConstellation(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Constellation)))));
    }

    // Editable subset of the system; excludes the player ship.
    void TStar::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        pas::WideString Key{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        Block->AddParam(EC_Str::DecodeTextW(u"Sgt3adr3Nsaym7ee"_w), Name);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(ComputeMapDiameter());
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"ImSkyasUDOiranma"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        Block->AddParam(u"X"_wref.get(), static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Position.X), SysUtils::DecimalSeparator)));
        Block->AddParam(u"Y"_wref.get(), static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Position.Y), SysUtils::DecimalSeparator)));
        Key = EC_Str::DecodeTextW(u"O3wHnfeWrss2"_w);
        switch (Status.ControlFaction) {
            case aGalaxyStruct::sfCoalition: Block->AddParam(Key, EC_Str::DecodeTextW(u"Ndo3rFm3awlfs"_w)); break;
            case aGalaxyStruct::sfPirates: Block->AddParam(Key, EC_Str::DecodeTextW(u"Pui4rfawtqeEs"_w)); break;
            case aGalaxyStruct::sfDominators: Block->AddParam(Key, EC_Str::DecodeTextW(u"Kzlwiqndgus"_w)); break;
        }
        Block->AddParam(EC_Str::DecodeTextW(u"D9o5meScewr3iwegs4"_w), aConst::DominatorSeriesNames[Status.DominatorSeries]);
        EC_BlockPar::TBlockParEC* ShipBlock = Block->AddBlockByPath(EC_Str::DecodeTextW(u"SahainpaLeikswt"_w));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Ship->Id))), EC_Str::DecodeTextW(u"S5heifphI4d"_w)});
            if (aPlayer::GetPlayer() != Ship) {
                Ship->SaveToBlock(ShipBlock->AddBlockByPath(Key));
            }
        }
        ShipBlock->AddParam(EC_Str::DecodeTextW(u"CorzeSafteetNgehwjRuuti5nrse"_w), u""_wref.get());
        // 'PlanetList'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->AddBlockByPath(EC_Str::DecodeTextW(u"PalkainrestaLuiksete"_w));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_2.next(I); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
                Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(Planet->Id)), EC_Str::DecodeTextW(u"PwlgaRneeZtfI6d3"_w)});
                Planet->SaveToBlock(cpp_with->AddBlockByPath(Key));
            }
            cpp_with->AddParam(EC_Str::DecodeTextW(u"CorzeSafteetNgehwjPoloaInuent"_w), u"0"_wref.get());
        }
        // 'Junk'
        {
            EC_BlockPar::TBlockParEC* cpp_with_2 = Block->AddBlockByPath(EC_Str::DecodeTextW(u"JoulnAk"_w));
            if (Items != nullptr) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_3.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(Items, I);
                    Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    {
                        EC_BlockPar::TBlockParEC* cpp_with_3 = cpp_with_2->AddBlockByPath(Key);
                        cpp_with_3->AddParam(u"X"_wref.get(), static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Item->Position.X), SysUtils::DecimalSeparator)));
                        cpp_with_3->AddParam(u"Y"_wref.get(), static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Item->Position.Y), SysUtils::DecimalSeparator)));
                    }
                    {
                        EC_BlockPar::TBlockParEC* blockByPath = cpp_with_2->GetBlockByPath(Key);
                        aItem::TItem* item = Item;
                        item->SaveToBlock(blockByPath);
                    }
                }
            }
            cpp_with_2->AddParam(EC_Str::DecodeTextW(u"Cur5erawtre3NregwgJou1nfk"_w), u""_wref.get());
        }
        Block->AddParam(EC_Str::DecodeTextW(u"CtrGefaEtdefNgeywuAksltkeuryoTirdesd"_w), u"0"_wref.get());
    }

    // Updates existing objects and can create stations, planets, items and asteroids. Moves star-link endpoints but does not rebuild distance caches.
    void TStar::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t I{};
        pas::WideString Key{};
        pas::WideString Value{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        aGalaxyStruct::TShipType StationType{};
        PConstellationStarLink Link{};
        aAsteroid::TAsteroid* Asteroid{};
        pas::WideString Style{};
        std::int32_t Part{};
        std::int32_t Variants{};
        std::int32_t Variant{};
        aItem::TItem* Item{};
        aConst::TItemType ItemType{};
        double Angle{};
        Name = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Sgt3adr3Nsaym7ee"_w)));
        float X = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"X"sv));
        float Y = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Y"sv));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->StarLinks) - 1); cpp_range.next(I); ) {
            Link = pas::list_at<TConstellationStarLink>(Constellation->StarLinks, I);
            if (Link->StartPoint.X == Position.X && Link->StartPoint.Y == Position.Y) {
                Link->StartPoint.X = X;
                Link->StartPoint.Y = Y;
            }
            if (Link->EndPoint.X == Position.X && Link->EndPoint.Y == Position.Y) {
                Link->EndPoint.X = X;
                Link->EndPoint.Y = Y;
            }
        }
        Position.X = X;
        Position.Y = Y;
        Key = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"O3wHnfeWrss2"_w)));
        // 'Normals'
        if (Key == EC_Str::DecodeTextW(u"Ndo3rFm3awlfs"_w)) {
            Status.ControlFaction = aGalaxyStruct::sfCoalition;
        } else if (Key == EC_Str::DecodeTextW(u"Pui4rfawtqeEs"_w)) {
            // 'Pirates'
            Status.ControlFaction = aGalaxyStruct::sfPirates;
        } else if (Key == EC_Str::DecodeTextW(u"Kzlwiqndgus"_w)) {
            Status.ControlFaction = aGalaxyStruct::sfDominators;
        }
        Key = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"D9o5meScewr3iwegs4"_w)));
        for (I = static_cast<std::int32_t>(aGalaxyStruct::dsBlazer); I <= static_cast<std::int32_t>(aGalaxyStruct::dsTerron); ++I) {
            if (Key == aConst::DominatorSeriesNames[static_cast<aGalaxyStruct::TDominatorSeries>(I)]) {
                Status.DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(I);
            }
        }
        // 'ShipList'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->GetBlockByPath(EC_Str::DecodeTextW(u"SahainpaLeikswt"_w));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_2.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, I);
                Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Ship->Id))), EC_Str::DecodeTextW(u"S5heifphI4d"_w)});
                if (aPlayer::GetPlayer() != Ship) {
                    Ship->LoadFromBlock(cpp_with->GetBlockByPath(Key));
                }
            }
            Key = cpp_with->GetParam(pas::view(EC_Str::DecodeTextW(u"CorzeSafteetNgehwjRuuti5nrse"_w)));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Key), u","sv) - 1); cpp_range_3.next(I); ) {
                Value = EC_Str::ExtractDelimitedPartW(pas::view(Key), I, u","sv);
                for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range_4.next(StationType); ) {
                    if (aConst::ShipTypeNames[StationType].Name == Value) {
                        aRuins::TRuins* cpp_arg = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                        TStar* self = this;
                        cpp_arg->Init(static_cast<aGalaxyStruct::TStationType>(StationType), self, pas::WideString());
                        break;
                    }
                }
            }
        }
        // 'PlanetList'
        {
            EC_BlockPar::TBlockParEC* cpp_with_2 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"PalkainrestaLuiksete"_w));
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_5.next(I); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
                Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(Planet->Id)), EC_Str::DecodeTextW(u"PwlgaRneeZtfI6d3"_w)});
                Planet->LoadFromBlock(cpp_with_2->GetBlockByPath(Key));
            }
            // 'CreateNewPlanet'
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, SysUtils::StrToInt(static_cast<pas::AnsiString>(cpp_with_2->GetParam(pas::view(EC_Str::DecodeTextW(u"CorzeSafteetNgehwjPoloaInuent"_w))))) - 1); cpp_range_6.next(I); ) {
                Planet = pas::construct_call<aPlanet::TPlanet>(aPlanet::TPlanet_Create);
                Planet->InitGeneratedUninhabited(this);
                pas::list_add(Planets, reinterpret_cast<void*>(Planet));
                pas::list_add(Galaxy->Planets, reinterpret_cast<void*>(Planet));
            }
        }
        // 'Junk'
        {
            EC_BlockPar::TBlockParEC* cpp_with_3 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"JoulnAk"_w));
            if (Items != nullptr) {
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_7.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(Items, I);
                    Key = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    {
                        EC_BlockPar::TBlockParEC* cpp_with_4 = cpp_with_3->GetBlockByPath(Key);
                        Item->Position.X = EC_Str::ExtractDecimalToSingleW(cpp_with_4->GetParam(u"X"sv));
                        Item->Position.Y = EC_Str::ExtractDecimalToSingleW(cpp_with_4->GetParam(u"Y"sv));
                    }
                    Item->virtual_TItem_LoadFromBlock(cpp_with_3->GetBlockByPath(Key));
                }
            }
            Key = cpp_with_3->GetParam(pas::view(EC_Str::DecodeTextW(u"Cur5erawtre3NregwgJou1nfk"_w)));
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Key), u","sv) - 1); cpp_range_8.next(I); ) {
                Value = EC_Str::ExtractDelimitedPartW(pas::view(Key), I, u","sv);
                for (auto cpp_range_9 = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_UselessCountableItem); cpp_range_9.next(ItemType); ) {
                    if (aConst::ItemTypeNames[ItemType] == Value) {
                        if (pas::in_set<aConst::t_Food, aConst::t_Narcotics, aConst::t_ArtefactHull, aConst::t_Satellite>(ItemType) && ItemType != aConst::t_Hull) {
                            Item = aItem::CreateDefaultItemByType(ItemType);
                            if (ItemType == aConst::t_Minerals) {
                                reinterpret_cast<aItem::TGoods*>(Item)->NaturalFlag = true;
                            }
                            if (aItem::TCountableItem* countableItem = pas::class_cast_if<aItem::TCountableItem*>(Item)) {
                                countableItem->DropFlag = 1;
                            }
                            if (Item != nullptr) {
                                pas::list_add(Items, reinterpret_cast<void*>(Item));
                            }
                            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 359));
                            Item->Position.X = System::Sin(Angle) * (3.0L * DamageRadius);
                            Item->Position.Y = System::Cos(Angle) * (3.0L * DamageRadius);
                        }
                        break;
                    }
                }
            }
        }
        Key = EC_Str::DecodeTextW(u"AfsBtfegrFodiDdf"_w);
        // 'CreateNewAsteroids'
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"CtrGefaEtdefNgeywuAksltkeuryoTirdesd"_w))))) - 1); cpp_range_10.next(I); ) {
            if (BackgroundImage < 10) {
                Style = ([&] {
                    const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(BackgroundImage)}));
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"Style", Key}));
                    return blockByPath->GetParam(pas::view(cpp_arg_2));
                }());
            } else {
                Style = ([&] {
                    const pas::WideString& intToStr = pas::wide_int_to_str(BackgroundImage);
                    EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"Style", Key}));
                    return blockByPath_2->GetParam(pas::view(intToStr));
                }());
            }
            Part = aMyFunction::NextRandomIntRange(0, EC_Str::CountDelimitedPartsW(pas::view(Style), u","sv) / 2 - 1, RandomState) * 2;
            Variants = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Style), Part + 1, u","sv)));
            Style = EC_Str::ExtractDelimitedPartW(pas::view(Style), Part, u","sv);
            Variant = aMyFunction::NextRandomIntRange(0, Variants - 1, RandomState);
            Asteroid = pas::construct_call<aAsteroid::TAsteroid>(aAsteroid::TAsteroid_Create);
            if (Variant < 10) {
                Asteroid->Init(this, pas::concat_wide({Key, u".", Style, u"0", pas::wide_int_to_str(Variant)}));
            } else {
                Asteroid->Init(this, pas::concat_wide({Key, u".", Style, pas::wide_int_to_str(Variant)}));
            }
            pas::list_add(Asteroids, reinterpret_cast<void*>(Asteroid));
        }
    }

    void TStar::PruneWeaponTargetsAfterTurn() {
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count = pas::list_count(Ships);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
                if (1 <= cpp_last) {
                    for (J = 1; J <= cpp_last; ++J) {
                        Weapon = Ship->Weapons[J];
                        if (Weapon->Target != nullptr) {
                            if (!Ship->InNormalSpace()) {
                                Weapon->Target = nullptr;
                            } else if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo <= 0) {
                                Weapon->Target = nullptr;
                            } else if (pas::class_cast_if<aItem::TItem*>(Weapon->Target) != nullptr) {
                                Weapon->Target = nullptr;
                            } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Weapon->Target) != nullptr) {
                                Weapon->Target = nullptr;
                            } else if (aMissile::TMissile* missile = pas::class_cast_if<aMissile::TMissile*>(Weapon->Target); missile != nullptr && ([&] {
                                pas::Extended cpp_right = pas::sqr(aShip::TShip_GetWeaponActionRange(Ship, Weapon));
                                return aMyFunction::PointDistanceSquared(Ship->Position, missile->Position) > cpp_right;
                            }())) {
                                Weapon->Target = nullptr;
                            } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Weapon->Target)) {
                                if (static_cast<std::uint8_t>(ship->InNormalSpace() ^ 1) || ([&] {
                                    pas::Extended cpp_right_2 = pas::sqr(aShip::TShip_GetWeaponActionRange(Ship, Weapon));
                                    return aMyFunction::PointDistanceSquared(Ship->Position, ship->Position) > cpp_right_2;
                                }())) {
                                    Weapon->Target = nullptr;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Recursively marks unmarked events sharing an attacker or target. Events contains PStarCombatEvent; Group must be nonzero.
    void TStar::MarkConnectedCombatEvents(pas::List* Events, pas::Object* Target, std::int32_t Group) {
        std::int32_t I{};
        PStarCombatEvent Event{};
        std::int32_t Count = pas::list_count(Events);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Event = pas::list_at<TStarCombatEvent>(Events, I);
            if (Event->CombatGroup == 0) {
                if (Event->Attacker == Target) {
                    Event->CombatGroup = Group;
                    MarkConnectedCombatEvents(Events, Event->Target, Group);
                } else if (Event->Target == Target) {
                    Event->CombatGroup = Group;
                    MarkConnectedCombatEvents(Events, Event->Attacker, Group);
                }
            }
        }
    }

    // Returns the sum of the new goods' Cost, not their quantity. Nonpositive Quantity creates no drops and returns zero.
    std::int32_t TStar::DropMinerals(std::int32_t Quantity, EC_Struct::TPointF Position, std::uint32_t Seed) {
        std::int32_t Result{};
        float Angle{};
        float AngleStep{};
        float Radius{};
        float Jitter{};
        std::int32_t DropCount{};
        std::int32_t DropQuantity{};
        std::int32_t I{};
        aItem::TGoods* Goods{};
        PMovingDropItemEntry Entry{};
        std::int32_t MaximumDrops = 4;
        try {
            Result = 0;
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, Seed));
            Seed = aMyFunction::StepRandomSeed(Seed);
            DropCount = 0;
            while (Quantity > 0) {
                if (Quantity < 10 || DropCount >= 3) {
                    DropQuantity = Quantity;
                } else {
                    DropQuantity = System::Round((aMyFunction::SeededRandomUnitFloat(Seed) * 0.2L + 0.55L) * Quantity);
                }
                Seed = aMyFunction::StepRandomSeed(Seed);
                Quantity -= DropQuantity;
                Goods = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                Goods->Init(aConst::t_Minerals, DropQuantity);
                Goods->NaturalFlag = true;
                Goods->Position = Position;
                Entry = static_cast<PMovingDropItemEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TMovingDropItemEntry))));
                Entry->Payload = Goods;
                Entry->SourceShipId = 0;
                Entry->InsertedIntoStar = false;
                Entry->DeployTranclucator = 0;
                pas::list_add(MovingDropItems, static_cast<void*>(Entry));
                ++DropCount;
                Result += Goods->Cost;
            }
            AngleStep = pas::constant(static_cast<float>(aMyFunction::GamePi));
            if (DropCount > 1) {
                AngleStep = pas::real_divide(aMyFunction::GameTwoPi, DropCount);
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, DropCount - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<TMovingDropItemEntry>(MovingDropItems, pas::list_count(MovingDropItems) - 1 - I);
                Radius = aMyFunction::SeededRandomIntRange(50, 150, Seed);
                if (PlayerStar == this && CurrentStepIndex > SimulationStepCount - 20) {
                    Radius = 5.0f;
                }
                Seed = aMyFunction::StepRandomSeed(Seed);
                Jitter = aMyFunction::SeededRandomUnitFloat(Seed) * 0.3L - 0.15L;
                Seed = aMyFunction::StepRandomSeed(Seed);
                {
                    pas::Extended cpp_right = System::Sin(aMyFunction::SeededRandomUnitFloat(Seed) * (static_cast<long double>(Angle) + Jitter)) * Radius;
                    Entry->Destination.X = pas::checked_cast<aItem::TItem*>(Entry->Payload)->Position.X + cpp_right;
                }
                Seed = aMyFunction::StepRandomSeed(Seed);
                {
                    pas::Extended cpp_right_2 = System::Cos(aMyFunction::SeededRandomUnitFloat(Seed) * (static_cast<long double>(Angle) + Jitter)) * Radius;
                    Entry->Destination.Y = pas::checked_cast<aItem::TItem*>(Entry->Payload)->Position.Y - cpp_right_2;
                }
                Seed = aMyFunction::StepRandomSeed(Seed);
                Angle = static_cast<long double>(Angle) + AngleStep;
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TStar.DropMineral ", Name}))));
            } else {
                throw;
            }
        }
        return Result;
    }

    // Requires a player. Achievement progress is independent of eligibility for a planet's reward or complaint.
    void TStar::ProcessPlayerAsteroidKill(std::int32_t MineralValue, EC_Struct::TPointF Position, std::uint32_t AsteroidId) {
        std::int32_t I{};
        std::int32_t MessageVariant{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TPlanet* NearestPlanet{};
        pas::Extended BestDistance{};
        pas::Extended Distance{};
        pas::WideString Text{};
        ++aPlayer::GetPlayer()->AchievementStats->AsteroidsDestroyed;
        Achievements::TrySetAchievementProgress(u"ASTEROID"_w, aPlayer::GetPlayer()->AchievementStats->AsteroidsDestroyed);
        if (Status.ControlFaction != aGalaxyStruct::sfCoalition) {
            return;
        }
        if (Status.CustomFaction != u"") {
            return;
        }
        if (Status.Battle != 0) {
            return;
        }
        BestDistance = 1.0E+20L;
        NearestPlanet = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            Distance = aMyFunction::PointDistanceSquared(Position, Planet->GetPosition());
            if (Distance < BestDistance) {
                BestDistance = Distance;
                NearestPlanet = Planet;
            }
        }
        if (NearestPlanet == nullptr) {
            return;
        }
        if (!pas::contains(aConst::PlanetOwnerMasks.Coalition, NearestPlanet->OwnerId)) {
            return;
        }
        if (NearestPlanet->IsMainPiratePlanet) {
            return;
        }
        std::int32_t Roll = aMyFunction::SeededRandomIntRange(1, 100, (Galaxy->GenerationSeed + NearestPlanet->GenerationSeed) * AsteroidId);
        if (Roll <= 70) {
            MessageVariant = aMyFunction::SeededRandomIntRange(1, 3, Galaxy->GenerationSeed + NearestPlanet->GenerationSeed + static_cast<std::uint32_t>(Galaxy->CurrentTurn));
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + MineralValue);
            Text = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"GalaxyNews.Star.Asteroid.Kill.", SysUtils::IntToStr(MessageVariant)})));
            aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(MineralValue), aMyFunction::TextHighlightColorTag);
            NearestPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 5);
        } else {
            Text = aConst::LocalizedColorText(pas::concat_wide({u"GalaxyNews.Star.Asteroid.Kill.", aConst::OwnerInfo[NearestPlanet->OwnerId].InternalName}));
            NearestPlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), -10);
        }
        {
            auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
            pas::WideString fullName = NearestPlanet->GetFullName(u" "_w);
            aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, std::move(fullName), textHighlightColorTag.get());
        }
        Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, Galaxy->CurrentTurn, Text, u"AsteroidKill"_wref.get());
    }

    // Clears weapon, missile and queued attack references; accepts any target class. Nil is a no-op.
    void TStar::ClearTargetReferences(pas::Object* Target) {
        aMissile::TMissile* Missile{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count{};
        PStarCombatEvent Event{};
        try {
            if (Target == nullptr) {
                return;
            }
            Count = pas::list_count(Ships);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, I);
                {
                    const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
                    if (1 <= cpp_last) {
                        for (J = 1; J <= cpp_last; ++J) {
                            Weapon = Ship->Weapons[J];
                            if (Weapon != nullptr && Weapon->Target == Target) {
                                Weapon->Target = nullptr;
                            }
                        }
                    }
                }
            }
            Count = pas::list_count(Missiles);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Missile = pas::list_at<aMissile::TMissile>(Missiles, I);
                Missile->ClearReferencesTo(Target);
            }
            Count = pas::list_count(CombatEvents);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
                Event = pas::list_at<TStarCombatEvent>(CombatEvents, I);
                if (Event->Target == Target) {
                    Event->Target = nullptr;
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TStar.NextDay.DelTarget ", Name}))));
            } else {
                throw;
            }
        }
    }

    // Clears attack, landing and combat-event references without removing or freeing Ship. Nil is a no-op.
    void TStar::ClearShipReferences(void* Ship) {
        aShip::TShip* TargetShip{};
        aShip::TShip* OtherShip{};
        aItem::TWeapon* Weapon{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count{};
        PStarCombatEvent Event{};
        try {
            if (Ship == nullptr) {
                return;
            }
            ClearTargetReferences(static_cast<pas::Object*>(Ship));
            TargetShip = static_cast<aShip::TShip*>(Ship);
            TargetShip->InterceptorPassesRemaining = 0;
            Count = pas::list_count(Ships);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                OtherShip = pas::list_at<aShip::TShip>(Ships, I);
                if (OtherShip->Order == aShip::soLand && OtherShip->OrderTarget == TargetShip) {
                    OtherShip->OrderNone(true);
                    if (RecordingTurnFilm) {
                        if (OtherShip->FilmObject != nullptr) {
                            OtherShip->FilmAlpha = 255.0f;
                            OtherShip->FilmAlphaStep = 0.0f;
                        }
                    }
                }
            }
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(TargetShip->WeaponCount);
                if (1 <= cpp_last) {
                    for (J = 1; J <= cpp_last; ++J) {
                        Weapon = TargetShip->Weapons[J];
                        if (Weapon != nullptr) {
                            Weapon->Target = nullptr;
                        }
                    }
                }
            }
            Count = pas::list_count(CombatEvents);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Event = pas::list_at<TStarCombatEvent>(CombatEvents, I);
                if (Event->Attacker == TargetShip) {
                    Event->Attacker = nullptr;
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TStar.NextDay.DelTargetShip ", Name}))));
            } else {
                throw;
            }
        }
    }

    // Clears targets, pickups, ReferencedItems and moving-drop payload references; does not remove Item from Items or free it.
    void TStar::ClearItemReferences(void* Item) {
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t Count{};
        try {
            if (Item == nullptr) {
                return;
            }
            ClearTargetReferences(static_cast<pas::Object*>(Item));
            Count = pas::list_count(Ships);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, I);
                Ship->RemovePickupTarget(static_cast<aItem::TItem*>(Item));
            }
            {
                const std::int32_t cpp_first = pas::list_count(ReferencedItems) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        if (pas::list_get(ReferencedItems, I) == Item) {
                            pas::list_delete(ReferencedItems, I);
                        }
                    }
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(MovingDropItems) - 1); cpp_range_2.next(I); ) {
                if (pas::list_at<TMovingDropItemEntry>(MovingDropItems, I)->Payload == Item) {
                    pas::list_at<TMovingDropItemEntry>(MovingDropItems, I)->Payload = nullptr;
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TStar.NextDay.DelTargetItem ", Name}))));
            } else {
                throw;
            }
        }
    }

    void TStar::ClearCombatEventWeaponReferences(void* Weapon) {
        std::int32_t I{};
        PStarCombatEvent Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CombatEvents) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TStarCombatEvent>(CombatEvents, I);
            if (Entry->Weapon == Weapon) {
                Entry->Weapon = nullptr;
            }
        }
    }

    // Daily preparation without movement/combat simulation; the caller tracks whether it has already run.
    void TStar::PrepareNextDay() {
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == this) {
            DaysSincePlayerVisit = 0;
            if (PlayerPresenceLevel < 90) {
                ++PlayerPresenceLevel;
            }
        } else {
            ++DaysSincePlayerVisit;
            if (PlayerPresenceLevel > 0) {
                --PlayerPresenceLevel;
            }
        }
        ++DaysSinceLastNpcShipSpawn;
        TryGenerateSystemNews();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            aPlanet::TPlanet_NextDay(Planet);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range_2.next(I); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, I);
            Asteroid->RespawnIfOutsideSystem();
        }
        {
            const std::int32_t cpp_first = pas::list_count(Ships) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Ship = pas::list_at<aShip::TShip>(Ships, I);
                    Ship->virtual_TShip_NextDay();
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_3.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship->Order == aShip::soLand && pas::class_cast_if<aShip::TShip*>(Ship->OrderTarget) != nullptr && static_cast<aShip::TShip*>(Ship->OrderTarget)->Order != aShip::soNone) {
                if (static_cast<aShip::TShip*>(Ship->OrderTarget)->Order != aShip::soTeleport) {
                    static_cast<aShip::TShip*>(Ship->OrderTarget)->OrderNone(false);
                } else {
                    Ship->OrderNone(false);
                }
            } else if (Ship->Order == aShip::soTakeoff && Ship->DockedTo != nullptr && Ship->DockedTo->Order != aShip::soNone && Ship->DockedTo->Order != aShip::soTeleport) {
                Ship->DockedTo->OrderNone(false);
            }
        }
    }

    // Updates followers and targets recursively for docked ships. Does not remove the object from Ships or free it; selected pursuers may receive a jump order.
    void TStar::HandleObjectLeavingStar(pas::Object* Obj) {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aShip::TShip* Target{};
        if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Obj)) {
            const std::int32_t cpp_last = static_cast<std::int32_t>(ship->WeaponCount);
            if (1 <= cpp_last) {
                for (J = 1; J <= cpp_last; ++J) {
                    ship->Weapons[J]->Target = nullptr;
                }
            }
        }
        std::int32_t Count = pas::list_count(Ships);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (!Ship->IsHullDestroyed()) {
                if (Ship->Order == aShip::soFollowShip && Ship->OrderTarget == Obj) {
                    if (aShip::TShip* ship_2 = pas::class_cast_if<aShip::TShip*>(Obj); ship_2 != nullptr && ship_2->Order == aShip::soJump) {
                        Target = ship_2;
                        if (Ship->TypeId == aGalaxyStruct::stKling && pas::contains(aConst::PlanetOwnerMasks.Coalition, Target->OwnerId) && Ship->GetHullIntegrityPercent() > 30 && Target->GetHullIntegrityPercent() > 10 && I > std::max<std::int32_t>(4, Count / 2) && ShipTypeCounts[aGalaxyStruct::stKling] > 7 && (pas::in_range(pas::checked_cast<aKling::TKling*>(Ship)->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktSmersh), static_cast<std::int32_t>(aGalaxyStruct::ktShtip)) || pas::in_range(static_cast<aKling::TKling*>(Ship)->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktEquantor), static_cast<std::int32_t>(aGalaxyStruct::ktUrgant)) && pas::in_set<5, 6>(I) && ShipTypeCounts[aGalaxyStruct::stKling] > 9) && aPlayer::GetPlayer() != nullptr && Target->InHyperspace && pas::class_cast_if<TStar*>(Target->OrderTarget) != nullptr && static_cast<TStar*>(Target->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfCoalition && static_cast<std::uint8_t>(aScript::IsStarProtectedByScript(static_cast<TStar*>(Target->OrderTarget)) ^ 1) && (Galaxy->CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns || aPlayer::GetPlayer()->CurrentStar != Target->OrderTarget) && Galaxy->CurrentTurn % 15 == 0) {
                            Ship->OrderJump(static_cast<TStar*>(Target->OrderTarget), true);
                        } else if (Ship->TypeId == aGalaxyStruct::stPirate && pas::checked_cast<aPirate::TPirate*>(Ship)->PirateType == 0 && pas::contains(aConst::PlanetOwnerMasks.Coalition, Target->OwnerId) && Ship->GetHullIntegrityPercent() > 90 && Target->GetHullIntegrityPercent() > 10 && aShip::TShip_ChanceToWin(Ship, Target) > 1.0L && aPlayer::GetPlayer() != nullptr && Target->InHyperspace && pas::class_cast_if<TStar*>(Target->OrderTarget) != nullptr && static_cast<TStar*>(Target->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfCoalition && static_cast<TStar*>(Target->OrderTarget)->Status.CustomFaction == u"" && static_cast<std::uint8_t>(aScript::IsStarProtectedByScript(static_cast<TStar*>(Target->OrderTarget)) ^ 1) && (Galaxy->CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns || aPlayer::GetPlayer()->CurrentStar != Target->OrderTarget) && Galaxy->CurrentTurn % 7 == 0) {
                            Ship->OrderJump(static_cast<TStar*>(Target->OrderTarget), true);
                        } else {
                            Ship->OrderNone(false);
                        }
                    } else {
                        Ship->OrderNone(false);
                    }
                }
                if (Ship->Order == aShip::soLand && Ship->OrderTarget == Obj) {
                    Ship->OrderNone(false);
                }
                if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(Ship); tranclucator != nullptr && tranclucator->OwnerShip == Obj) {
                    tranclucator->FollowOwner = false;
                }
                {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Ship->WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            if (Ship->Weapons[J]->Target == Obj) {
                                Ship->Weapons[J]->Target = nullptr;
                            }
                        }
                    }
                }
                if (Ship->DockedTo == Obj) {
                    HandleObjectLeavingStar(Ship);
                }
            }
        }
        if (aShip::TShip* ship_3 = pas::class_cast_if<aShip::TShip*>(Obj)) {
            ship_3->ClearPickupTargets();
        }
    }

    // Trims or clears planned movement paths; does not apply collision damage.
    void TStar::AvoidShipPathCollisions() {
        aShip::TShip* Ship{};
        PCollisionEntry Entry{};
        PCollisionEntry Other{};
        std::int32_t I{};
        std::int32_t J{};
        aPath::PSPathNode Node{};
        std::uint8_t Collides{};
        EC_Struct::TPointF PreviousPosition{};
        std::int32_t ShipCount = pas::list_count(Ships);
        if (ShipCount < 1) {
            return;
        }
        pas::List* Entries = pas::make_object<pas::List>();
        std::int32_t StationaryCount = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship); (!(kling != nullptr) || kling->KlingType != aGalaxyStruct::ktBoss || kling->DominatorSeries != aGalaxyStruct::dsTerron) && static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && Ship->Order != aShip::soTakeoff && static_cast<std::uint8_t>(Ship->IsTravelCompletionPathReady() ^ 1) && Ship->CurrentPlanet == nullptr && Ship->DockedTo == nullptr) {
                if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(Ship); (!(tranclucator != nullptr) || static_cast<std::uint8_t>(tranclucator->CanFollowOwnerInCurrentStar() ^ 1) || static_cast<long double>(pas::sqr(Ship->Speed)) <= aMyFunction::PointDistanceSquared(Ship->Position, tranclucator->OwnerShip->Position)) && !(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) && static_cast<std::uint8_t>(Ship->OrderAbsolute ^ 1)) {
                    Entry = static_cast<PCollisionEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TCollisionEntry))));
                    Entry->Ship = Ship;
                    Entry->DistanceSquared = 0.0f;
                    pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, Ship->Position);
                    if (Ship->MovementPath->ActiveTail != nullptr) {
                        Entry->DistanceSquared = aMyFunction::PointDistanceSquared(Entry->Position, Ship->MovementPath->ActiveTail->Position);
                        pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, Ship->MovementPath->ActiveTail->Position);
                    } else {
                        ++StationaryCount;
                    }
                    J = 0;
                    while (J < pas::list_count(Entries)) {
                        Other = pas::list_at<TCollisionEntry>(Entries, J);
                        if (Other->DistanceSquared > Entry->DistanceSquared) {
                            break;
                        }
                        ++J;
                    }
                    if (J >= pas::list_count(Entries)) {
                        pas::list_add(Entries, static_cast<void*>(Entry));
                    } else {
                        pas::list_insert(Entries, J, static_cast<void*>(Entry));
                    }
                }
            }
        }
        std::int32_t Count = pas::list_count(Entries);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(StationaryCount, Count - 1); cpp_range_2.next(I); ) {
            Entry = pas::list_at<TCollisionEntry>(Entries, I);
            Node = Entry->Ship->MovementPath->ActiveTail;
            while (Node != nullptr) {
                Collides = false;
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, I - 1); cpp_range_3.next(J); ) {
                    Other = pas::list_at<TCollisionEntry>(Entries, J);
                    if (Node->Prev == nullptr) {
                        PreviousPosition = Entry->Ship->Position;
                    } else {
                        PreviousPosition = Node->Prev->Position;
                    }
                    if (aMyFunction::PointDistanceSquared(Node->Position, Other->Position) < pas::sqr(static_cast<long double>(Entry->Ship->CollisionRadius) + Other->Ship->CollisionRadius) && aMyFunction::PointDistanceSquared(Node->Position, Other->Position) < aMyFunction::PointDistanceSquared(PreviousPosition, Other->Position)) {
                        Collides = true;
                        break;
                    }
                }
                if (!Collides) {
                    break;
                }
                Node = Node->Prev;
            }
            if (Node == nullptr) {
                pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, Entry->Ship->Position);
                Entry->Ship->ClearMovementPath();
            } else {
                pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, Node->Position);
                if (Node->Next != nullptr) {
                    Entry->Ship->MovementPath->RemoveNodeRange(Node->Next, Entry->Ship->MovementPath->ActiveTail);
                    Entry->Ship->MovementPath->ResampleBezierRange(Entry->Ship->MovementPath->ActiveHead, Entry->Ship->MovementPath->ActiveTail, aGalaxyStruct::BaseMovementStepsPerTurn);
                }
            }
        }
        Count = pas::list_count(Entries);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            EC_Mem::FreeEC(pas::list_get(Entries, I));
        }
        pas::free(Entries);
    }

    // Normal-space ships only; uses MovementStepCount.
    void TStar::RebuildShipMovementPaths() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship->InNormalSpace()) {
                Ship->BuildOrderMovementPath(MovementStepCount);
            }
        }
    }

    void TStar::OpenSpaceScene(GI_Panel::TPanelGI* MapPanel, GI_MessageLoop::TObjectGI* Minimap, GI_MessageLoop::TMessageLoopGI* Screen) {
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        THole* Hole{};
        aPlanet::TSputnik* Satellite{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        PJumpGateEntry Gate{};
        aMissile::TMissile* Missile{};
        if (aPlayer::GetPlayer() != nullptr) {
            Globals::SpaceProcess->RadarCenter = aPlayer::GetPlayer()->Position;
            Globals::SpaceProcess->RadarRange = aPlayer::GetPlayer()->GetRadarRange();
            Globals::SpaceProcess->ActionRange = aPlayer::GetPlayer()->GetRadarRange();
            Globals::SpaceProcess->ActionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
        } else {
            Globals::SpaceProcess->RadarCenter = EC_Struct::MakePointF(0.0f, 0.0f);
            Globals::SpaceProcess->RadarRange = 0;
            Globals::SpaceProcess->ActionRange = 0;
            Globals::SpaceProcess->ActionColor = 0u;
        }
        Globals::SpaceProcess->SystemRadius = ComputeMapDiameter() / 2;
        Globals::SpaceProcess->PopulateAmbientObjects(ComputeMapDiameter() / 2, BackgroundImage, GenerationSeed);
        Globals::SpaceProcess->OpenSpace(MapPanel, Screen);
        Globals::SpaceProcess->Space->MinimapScale = pas::real_divide(Minimap->ClientSize.X, ComputeMapDiameter());
        Globals::SpaceProcess->Space->AlphaShift = 0;
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->IsHealthEffectActive(aGalaxyStruct::heBlindness)) {
                Globals::SpaceProcess->Space->AlphaShift = 2;
            }
        }
        Globals::SpaceProcess->BindMinimap(Minimap);
        Graphic->AttachToSpace(Globals::SpaceProcess->Space);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            Planet->Graphic->Civilized = Planet->OwnerId != aGalaxyStruct::oiUninhabited;
            Planet->Graphic->SetMinimapOwner(Planet->OwnerId);
            if (Planet->CustomFaction != u"") {
                J = aConst::GetCustomFactionPlanetIconNumber(Planet->CustomFaction);
                if (J >= 0) {
                    Planet->Graphic->SetMinimapOwner(J + 1 + 7);
                }
            }
            {
                std::int32_t surfaceAnimationMask = Planet->GetSurfaceAnimationMask();
                SE_Planet::TPlanetSE* graphic = Planet->Graphic;
                graphic->SetSurfaceAnimationMask(surfaceAnimationMask);
            }
            Planet->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
            if (GlobalsV::SputnikShow) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Satellites) - 1); cpp_range_2.next(J); ) {
                    Satellite = pas::list_at<aPlanet::TSputnik>(Planet->Satellites, J);
                    Satellite->Graphic->OrbitCenter = Planet->GetPosition();
                    Satellite->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range_3.next(I); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, I);
            Asteroid->GraphObject->AttachToSpace(Globals::SpaceProcess->Space);
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_4.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship->InNormalSpace()) {
                Ship->Graphic->SetAlpha(255);
                if (SE_Ship2::TShip2SE* ship2SE = pas::class_cast_if<SE_Ship2::TShip2SE*>(Ship->Graphic)) {
                    SE_Ship2::TShip2SE* cpp_with = ship2SE;
                    if (GlobalsV::ShipTail == 2 || GlobalsV::ShipTail == 1 && aPlayer::GetPlayer() == Ship) {
                        cpp_with->SetTailMode(1);
                    } else {
                        cpp_with->SetTailMode(0);
                    }
                } else if (SE_Ruins::TRuinsSE* ruinsSE = pas::class_cast_if<SE_Ruins::TRuinsSE*>(Ship->Graphic); ruinsSE != nullptr && ruinsSE->HasTransitionImages) {
                    ruinsSE->SetState(1);
                }
                Ship->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
                if (Ship->InterceptorGraphic != nullptr) {
                    Ship->InterceptorGraphic->SetAlpha(255);
                    Ship->InterceptorGraphic->AttachToSpace(Globals::SpaceProcess->Space);
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Items) - 1); cpp_range_5.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Items, I);
            {
                SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                SE_Space::TSpaceSE* space = Globals::SpaceProcess->Space;
                graphObject->AttachToSpace(space);
            }
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->JumpGates) - 1); cpp_range_6.next(I); ) {
            Gate = pas::list_at<TJumpGateEntry>(Galaxy->JumpGates, I);
            reinterpret_cast<SE_Gate::TGateSE*>(Gate->Gate)->SetState(2);
            Gate->Gate->AttachToSpace(Globals::SpaceProcess->Space);
        }
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Holes) - 1); cpp_range_7.next(I); ) {
            Hole = pas::list_at<THole>(Galaxy->Holes, I);
            if (Hole->Star1 == this) {
                Hole->Graphic->SetPosition(Hole->Position1);
                reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->SetState(0);
                Hole->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
            } else if (Hole->Star2 == this) {
                Hole->Graphic->SetPosition(Hole->Position2);
                reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->SetState(0);
                Hole->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
            }
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Missiles) - 1); cpp_range_8.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Missiles, I);
            {
                SE_Space::TObjectSE* graphObject_2 = Missile->GetGraphObject();
                SE_Space::TSpaceSE* space_2 = Globals::SpaceProcess->Space;
                graphObject_2->AttachToSpace(space_2);
            }
        }
    }

    void TStar::RefreshSpaceObjectPositions() {
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TSputnik* Satellite{};
        aMissile::TMissile* Missile{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Planets)); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I - 1);
            Planet->Graphic->SetPosition(aMyFunction::PolarToPoint(Planet->Orbit));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Satellites) - 1); cpp_range_2.next(J); ) {
                Satellite = pas::list_at<aPlanet::TSputnik>(Planet->Satellites, J);
                Satellite->Graphic->OrbitCenter = Planet->GetPosition();
                Satellite->Graphic->UpdateOrbitDisplay();
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range_3.next(I); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, I);
            Asteroid->GraphObject->SetPosition(Asteroid->Position);
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(Ships)); cpp_range_4.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I - 1);
            Ship->Graphic->SetPosition(Ship->Position);
            Ship->Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(Ship->MovementDirection));
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, pas::list_count(Items)); cpp_range_5.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Items, I - 1);
            {
                SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                EC_Struct::TPointF position = Item->Position;
                graphObject->SetPosition(position);
            }
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Missiles) - 1); cpp_range_6.next(I); ) {
            Missile = pas::list_at<aMissile::TMissile>(Missiles, I);
            {
                SE_Space::TObjectSE* graphObject_2 = Missile->GetGraphObject();
                EC_Struct::TPointF position_2 = Missile->Position;
                graphObject_2->SetPosition(position_2);
            }
            {
                std::uint8_t headingDegreesToByte = aMyFunction::HeadingDegreesToByte(Missile->Direction);
                SE_Space::TObjectSE* graphObject_3 = Missile->GetGraphObject();
                graphObject_3->SetAngle(headingDegreesToByte);
            }
        }
    }

    void TStar::QueueSpaceImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        THole* Hole{};
        aItem::TItem* Item{};
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TSputnik* Satellite{};
        aAsteroid::TAsteroid* Asteroid{};
        Graphic->QueueImageLoad(PendingLoads, Owner);
        if (aKling::TerronShip != nullptr && aKling::TerronShip->CurrentStar == this && GlobalsV::AnimStar) {
            {
                GI_GAI::TgaiGI* cpp_with = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
                cpp_with->SetImagePath(u"Bm.Star.Terron_Transform_a"_wref.get());
                cpp_with->QueueImageLoad(PendingLoads);
                pas::free(cpp_with);
            }
            {
                GI_GAI::TgaiGI* cpp_with_2 = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
                cpp_with_2->SetImagePath(u"Bm.Star.TerronAfter_a"_wref.get());
                cpp_with_2->QueueImageLoad(PendingLoads);
                pas::free(cpp_with_2);
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Planets)); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I - 1);
            Planet->Graphic->QueueImageLoad(PendingLoads, Owner);
            if (GlobalsV::SputnikShow) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Satellites) - 1); cpp_range_2.next(J); ) {
                    Satellite = pas::list_at<aPlanet::TSputnik>(Planet->Satellites, J);
                    Satellite->Graphic->QueueImageLoad(PendingLoads, Owner);
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(Ships)); cpp_range_3.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I - 1);
            Ship->Graphic->QueueImageLoad(PendingLoads, Owner);
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(Items)); cpp_range_4.next(I); ) {
            Item = pas::list_at<aItem::TItem>(Items, I - 1);
            Item->GetGraphObject()->QueueImageLoad(PendingLoads, Owner);
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range_5.next(I); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, I);
            Asteroid->GraphObject->QueueImageLoad(PendingLoads, Owner);
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Holes) - 1); cpp_range_6.next(I); ) {
            Hole = pas::list_at<THole>(Galaxy->Holes, I);
            if (Hole->Star1 == this || Hole->Star2 == this) {
                Hole->Graphic->QueueImageLoad(PendingLoads, Owner);
            }
        }
    }

    void TStar::QueueHyperspaceShipImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        aShip::TShip* Ship{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Ships)); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I - 1);
            if (Ship->InHyperspace) {
                Ship->Graphic->QueueImageLoad(PendingLoads, Owner);
            }
        }
    }

    // Sets Size to 2000.
    pas::WideString TStar::GetBackgroundImagePath(std::int32_t& Size) {
        Size = 2000;
        if (BackgroundImage < 10) {
            return static_cast<pas::WideString>(pas::concat_ansi({"Bm.BGO.bg0", SysUtils::IntToStr(BackgroundImage)}));
        }
        return static_cast<pas::WideString>(pas::concat_ansi({"Bm.BGO.bg", SysUtils::IntToStr(BackgroundImage)}));
    }

    void TStar::ProcessItemScripts(std::int32_t TurnPhase) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aShip::TShip* Ship{};
        std::int32_t Stage{};
        Stage = 0;
        try {
            {
                const std::int32_t cpp_first = pas::list_count(Ships) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        if (pas::list_count(Ships) > I) {
                            Ship = pas::list_at<aShip::TShip>(Ships, I);
                            Stage = 1;
                            if (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (aPlayer::GetPlayer() == Ship || Galaxy->StasisModEnabled != 1)) {
                                Ship->ScriptItemsAct(aGalaxyStruct::satOnStep, nullptr, nullptr, TurnPhase);
                            }
                            Stage = 2;
                        }
                    }
                }
            }
            Stage = 3;
            if (Galaxy->StasisModEnabled == 1) {
                return;
            }
            {
                const std::int32_t cpp_first_2 = pas::list_count(Items) - 1;
                if (cpp_first_2 >= 0) {
                    for (I = cpp_first_2; I >= 0; --I) {
                        Stage = 4;
                        if (pas::list_count(Items) <= I) {
                            continue;
                        }
                        Item = pas::list_at<aItem::TItem>(Items, I);
                        if (Item->DestroyFlag > 0) {
                            continue;
                        }
                        Stage = 5;
                        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                            aScript::RunItemConfigActionCode(Item, aGalaxyStruct::satOnStep, nullptr, this, nullptr, TurnPhase);
                            if (pas::list_count(Items) <= I || pas::list_get(Items, I) != Item) {
                                continue;
                            }
                        }
                        Stage = 6;
                        if (Item->ScriptItem != nullptr) {
                            reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aGalaxyStruct::satOnStep, nullptr, this, nullptr, TurnPhase);
                        }
                        Stage = 7;
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Error in procedure TStar.ScriptShipsAndItemsAct label = ", SysUtils::IntToStr(Stage)}));
                throw;
            } else {
                throw;
            }
        }
    }

    // Uses polygon containment, not Star.Constellation; returns -1 when no polygon contains the star.
    std::int32_t TGalaxy::FindConstellationIndexForStar(TStar* Star) {
        std::int32_t I{};
        TConstellation* Constellation{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            if (Constellation->ContainsPoint(Star->Position)) {
                return I;
            }
        }
        return -1;
    }

    // Requires a player/home planet and generated outlines. Boss sectors are forced to tier three.
    void TGalaxy::InitializeConstellationDistanceTiers() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Pass{};
        TConstellation* Constellation{};
        TConstellation* Other{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            if (Constellation->SharesOutlineSegment(aPlayer::GetPlayer()->HomePlanet->CurrentStar->Constellation)) {
                Constellation->HomeDistanceTier = 0;
            } else {
                Constellation->HomeDistanceTier = 3;
            }
        }
        std::uint8_t Tier = 0;
        for (Pass = 1; Pass <= 2; ++Pass) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_2.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, I);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_3.next(J); ) {
                    Other = pas::list_at<TConstellation>(Constellations, J);
                    if (Other->SharesOutlineSegment(Constellation) && Other->HomeDistanceTier == 3 && Constellation->HomeDistanceTier == Tier) {
                        Other->HomeDistanceTier = Tier + 1;
                    }
                }
            }
            ++Tier;
        }
        if (aKling::BlazerShip != nullptr) {
            aKling::BlazerShip->CurrentStar->Constellation->HomeDistanceTier = 3;
        }
        if (aKling::KellerShip != nullptr) {
            aKling::KellerShip->CurrentStar->Constellation->HomeDistanceTier = 3;
        }
        if (aKling::TerronShip != nullptr) {
            aKling::TerronShip->CurrentStar->Constellation->HomeDistanceTier = 3;
        }
    }

    void TGalaxy::BuildConstellationOutlineJunctions() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        PMapLineSegment Segment{};
        PMapLineSegment First{};
        PMapLineSegment Next{};
        void* Temp{};
        EC_Struct::PPointF Point{};
        TConstellation* Constellation{};
        TConstellation* Other{};
        std::uint8_t Outer{};
        EC_Struct::TPointF TempPoint{};
        if (ConstellationOutlineJunctions != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ConstellationOutlineJunctions) - 1); cpp_range.next(I); ) {
                Point = pas::list_at<EC_Struct::TPointF>(ConstellationOutlineJunctions, I);
                pas::dispose(Point);
            }
            pas::list_clear(ConstellationOutlineJunctions);
        } else {
            ConstellationOutlineJunctions = pas::make_object<pas::List>();
        }
        pas::List* Edges = pas::make_object<pas::List>();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_2.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->OutlineSegments) - 1); cpp_range_3.next(J); ) {
                Segment = pas::list_at<TMapLineSegment>(Constellation->OutlineSegments, J);
                Outer = true;
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_4.next(K); ) {
                    if (K != I) {
                        Other = pas::list_at<TConstellation>(Constellations, K);
                        if (Other->HasOutlineSegment(Segment->StartPoint, Segment->EndPoint)) {
                            Outer = false;
                            break;
                        }
                    }
                }
                if (Outer) {
                    pas::get_mem_at(&Next, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
                    pas::store_unaligned<EC_Struct::TPointF>(&Next->StartPoint, Segment->StartPoint);
                    pas::store_unaligned<EC_Struct::TPointF>(&Next->EndPoint, Segment->EndPoint);
                    pas::list_add(Edges, static_cast<void*>(Next));
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Edges) - 2); cpp_range_5.next(I); ) {
            First = pas::list_at<TMapLineSegment>(Edges, I);
            J = I + 1;
            while (J < pas::list_count(Edges)) {
                Next = pas::list_at<TMapLineSegment>(Edges, J);
                if (aVector::PointsNearlyEqualF(First->EndPoint, Next->StartPoint)) {
                    break;
                }
                if (aVector::PointsNearlyEqualF(First->EndPoint, Next->EndPoint)) {
                    TempPoint = Next->StartPoint;
                    pas::store_unaligned<EC_Struct::TPointF>(&Next->StartPoint, Next->EndPoint);
                    pas::store_unaligned<EC_Struct::TPointF>(&Next->EndPoint, TempPoint);
                    break;
                }
                ++J;
            }
            if (J < pas::list_count(Edges)) {
                Temp = pas::list_get(Edges, I + 1);
                pas::list_put(Edges, I + 1, pas::list_get(Edges, J));
                pas::list_put(Edges, J, Temp);
            }
        }
        float Distance = 0.0f;
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Edges) - 1); cpp_range_6.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(Edges, I);
            Outer = false;
            K = 0;
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_7.next(J); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, J);
                if (Constellation->HasOutlineVertex(Segment->StartPoint)) {
                    ++K;
                    if (K > 1) {
                        Outer = true;
                        break;
                    }
                }
            }
            if (Distance > 0.0L || Outer != false) {
                pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                pas::store_unaligned<EC_Struct::TPointF>(Point, Segment->StartPoint);
                pas::list_add(ConstellationOutlineJunctions, static_cast<void*>(Point));
                Distance = 0.0f;
            }
            Distance = static_cast<long double>(aVector::PointDistanceF(Segment->StartPoint, Segment->EndPoint)) + Distance;
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Edges) - 1); cpp_range_8.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(Edges, I);
            pas::dispose(Segment);
        }
        pas::list_clear(Edges);
        pas::free(Edges);
    }

    std::uint8_t TGalaxy::ShouldKeepConstellationOutlineVertex(EC_Struct::TPointF Point) {
        std::int32_t I{};
        TConstellation* Constellation{};
        EC_Struct::PPointF Vertex{};
        std::int32_t Count = 0;
        if (ConstellationOutlineJunctions == nullptr) {
            if (aVector::ScalarsNearlyEqualF(Point.X, 0.0f)) {
                ++Count;
            }
            if (aVector::ScalarsNearlyEqualF(Point.Y, 0.0f)) {
                ++Count;
            }
            if (aVector::ScalarsNearlyEqualF(Point.X, aConst::GalaxySizeY)) {
                ++Count;
            }
            if (aVector::ScalarsNearlyEqualF(Point.Y, aConst::GalaxySizeY)) {
                ++Count;
            }
        } else {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ConstellationOutlineJunctions) - 1); cpp_range.next(I); ) {
                Vertex = pas::list_at<EC_Struct::TPointF>(ConstellationOutlineJunctions, I);
                if (aVector::PointsNearlyEqualF(pas::load_unaligned<EC_Struct::TPointF>(Vertex), Point)) {
                    return true;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_2.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            if (Constellation->HasOutlineVertex(Point)) {
                ++Count;
                if (Count > 2) {
                    break;
                }
            }
        }
        return Count > 2;
    }

    void TGalaxy::SimplifyConstellationOutline(std::int32_t ConstellationIndex) {
        std::int32_t I{};
        pas::List* Points{};
        PMapLineSegment Segment{};
        EC_Struct::PPointF First{};
        EC_Struct::PPointF Last{};
        aVector::TPolygon2D* Polygon{};
        TConstellation* Constellation = pas::list_at<TConstellation>(Constellations, ConstellationIndex);
        if (Constellation != nullptr) {
            Points = pas::make_object<pas::List>();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->OutlineSegments) - 1); cpp_range.next(I); ) {
                Segment = pas::list_at<TMapLineSegment>(Constellation->OutlineSegments, I);
                if (ShouldKeepConstellationOutlineVertex(Segment->StartPoint)) {
                    pas::get_mem_at(&First, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                    pas::store_unaligned<EC_Struct::TPointF>(First, Segment->StartPoint);
                    pas::list_add(Points, static_cast<void*>(First));
                }
            }
            Constellation->ClearOutlineSegmentsAndBounds();
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range_2.next(I); ) {
                First = pas::list_at<EC_Struct::TPointF>(Points, I);
                if (I == pas::list_count(Points) - 1) {
                    Last = pas::list_at<EC_Struct::TPointF>(Points, 0);
                } else {
                    Last = pas::list_at<EC_Struct::TPointF>(Points, I + 1);
                }
                pas::get_mem_at(&Segment, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
                pas::store_unaligned<EC_Struct::TPointF>(&Segment->StartPoint, pas::load_unaligned<EC_Struct::TPointF>(First));
                pas::store_unaligned<EC_Struct::TPointF>(&Segment->EndPoint, pas::load_unaligned<EC_Struct::TPointF>(Last));
                pas::list_add(Constellation->OutlineSegments, static_cast<void*>(Segment));
            }
            Constellation->RefreshOutlineBounds();
            Polygon = nullptr;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range_3.next(I); ) {
                First = pas::list_at<EC_Struct::TPointF>(Points, I);
                if (I == pas::list_count(Points) - 1) {
                    Last = pas::list_at<EC_Struct::TPointF>(Points, 0);
                } else {
                    Last = pas::list_at<EC_Struct::TPointF>(Points, I + 1);
                }
                if (Polygon == nullptr) {
                    Polygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_CreateTriangle, pas::load_unaligned<EC_Struct::TPointF>(First), pas::load_unaligned<EC_Struct::TPointF>(Last), Constellation->MapCenter);
                } else {
                    Polygon->Append(pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_CreateTriangle, pas::load_unaligned<EC_Struct::TPointF>(First), pas::load_unaligned<EC_Struct::TPointF>(Last), Constellation->MapCenter));
                }
            }
            Constellation->SetOutlinePolygon(Polygon);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range_4.next(I); ) {
                pas::dispose(pas::list_at<EC_Struct::TPointF>(Points, I));
            }
            pas::free(Points);
        }
    }

    // Attempts every constellation; false means at least one graph is disconnected.
    std::uint8_t TGalaxy::BuildConstellationStarGraphs() {
        std::int32_t I{};
        TConstellation* Constellation{};
        std::uint8_t Result = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            if (!Constellation->BuildStarGraph()) {
                Result = false;
            }
        }
        return Result;
    }

    // Requires at least eight constellations.
    void TGalaxy::BuildConstellationPolygonsAndAdjacency(aVector::TPolygon2D* WorkingPolygon) {
        std::int32_t I{};
        std::int32_t J{};
        TConstellation* Constellation{};
        TConstellation* Other{};
        PConstellationBoundaryRaySample Sample{};
        EC_Struct::TPointF PreviousPoint{};
        EC_Struct::TPointF Point{};
        float Step = 3.0f;
        aVector::TPolygon2D_ResetChainGroups(WorkingPolygon);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            aVector::TPolygon2D_AssignGroupAtPoint(WorkingPolygon, Constellation->MapCenter, I);
            Constellation->GenerateBoundaryRaySamples(128);
        }
        std::int32_t Steps = System::Trunc(pas::real_divide(pas::real_divide(aConst::GalaxySizeY, System::Sqrt(static_cast<std::uint32_t>(ConstellationCount))), Step) * 0.5L) + 1;
        for (I = 0; I <= 7; ++I) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            Constellation->OutlineGrowthStepsRemaining = Steps + 20;
        }
        Steps += 20;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(8, pas::list_count(Constellations) - 1); cpp_range_2.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            Constellation->OutlineGrowthStepsRemaining = Steps;
        }
        std::int32_t Pass = -1;
        do {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_3.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, I);
                if (static_cast<std::uint32_t>(Constellation->OutlineGrowthStepsRemaining) > 0) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->BoundaryRaySamples) - 1); cpp_range_4.next(J); ) {
                        Sample = pas::list_at<TConstellationBoundaryRaySample>(Constellation->BoundaryRaySamples, J);
                        if (!Sample->GrowthStopped) {
                            PreviousPoint = Sample->Position;
                            Point = EC_Struct::MakePointF(static_cast<long double>(Step) * Sample->Direction.X + PreviousPoint.X, static_cast<long double>(Step) * Sample->Direction.Y + PreviousPoint.Y);
                            if (Point.X < 0.0L || static_cast<long double>(aConst::GalaxySizeX * 1) <= Point.X || Point.Y < 0.0L || static_cast<long double>(aConst::GalaxySizeY * 1) <= Point.Y) {
                                Point = PreviousPoint;
                            }
                            pas::store_unaligned<EC_Struct::TPointF>(&Sample->Position, Point);
                            Sample->GrowthStopped = static_cast<std::uint8_t>(aVector::TPolygon2D_AssignGroupAtPoint(WorkingPolygon, Point, I) ^ 1);
                        }
                    }
                    --Constellation->OutlineGrowthStepsRemaining;
                }
            }
            ++Pass;
        } while (!(Pass >= Steps));
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_5.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            Constellation->SetOutlinePolygon(WorkingPolygon->ExtractFollowingGroup(I));
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_6.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(I + 1, pas::list_count(Constellations) - 1); cpp_range_7.next(J); ) {
                Other = pas::list_at<TConstellation>(Constellations, J);
                if (Constellation->SharesOutlineSegment(Other)) {
                    Constellation->AddAdjacentConstellation(Other);
                    Other->AddAdjacentConstellation(Constellation);
                }
            }
        }
    }

    void TGalaxy::GenerateGalaxyLayout(aGalaxyStruct::TOwnerId PlayerRace) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t FirstIndex{};
        std::int32_t SecondIndex{};
        std::int32_t N{};
        std::int32_t Attempts{};
        TStar* Star{};
        TStar* OtherStar{};
        TStar* SecondStar{};
        std::uint8_t InvalidLayout{};
        std::int32_t FuelRange{};
        std::int32_t NearestDistance{};
        std::int32_t Distance{};
        pas::DynArray<std::uint8_t> Reached{};
        TConstellation* Constellation{};
        TConstellation* OtherConstellation{};
        std::int32_t MinimumStarDistance{};
        std::int32_t ConstellationIndex{};
        WindowsSdk::TPoint BoundsSize{};
        double BestAreaPerStar{};
        double AreaPerStar{};
        aVector::TPolygon2D* WorkingPolygon{};
        aVector::TPolygon2D* Polygon{};
        std::uint8_t Occupied{};
        std::int32_t PlacementAttempts{};
        std::int32_t TotalLinks{};
        std::int32_t AxisLinks{};
        PConstellationStarLink Link{};
        float Coordinate{};
        std::int32_t HumanPosition{};
        pas::Array<std::uint8_t, 0, 7> RaceOrder{};
        pas::Array<std::uint8_t, 0, 4> CoalitionPositions{};
        WindowsSdk::TRect Bounds{};
        System::RandSeed = Galaxy->GenerationSeed;
        pas::list_clear(Constellations);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ConstellationCount); cpp_range.next(I); ) {
            Constellation = pas::construct_call<TConstellation>(TConstellation_Create);
            pas::list_add(Constellations, reinterpret_cast<void*>(Constellation));
        }
        std::int32_t MinimumConstellationDistance = System::Round(System::Sqrt(pas::real_divide(aConst::GalaxySizeY * aConst::GalaxySizeY, static_cast<std::uint32_t>(ConstellationCount))) * 0.75L);
        float MinimumArea = pas::real_divide(aConst::GalaxySizeX * 0.52L * aConst::GalaxySizeY, static_cast<std::uint32_t>(ConstellationCount));
        float MaximumArea = pas::real_divide(aConst::GalaxySizeX * 1 * 1.5L * (aConst::GalaxySizeY * 1), static_cast<std::uint32_t>(ConstellationCount));
        std::int32_t GenerationAttempts = 0;
        do {
            WorkingPolygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
            Polygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
            Polygon->SetRectangle(ClassesImports::Rect(0, 0, aConst::GalaxySizeX, aConst::GalaxySizeY));
            WorkingPolygon->Append(Polygon);
            Coordinate = 5.0f;
            while (static_cast<long double>(Coordinate) < aConst::GalaxySizeX * 1) {
                aVector::TPolygon2D_SplitChainByPoints(WorkingPolygon, EC_Struct::MakePointF(Coordinate, 0.0f), EC_Struct::MakePointF(Coordinate, aConst::GalaxySizeY));
                Coordinate = Coordinate + 5.0L;
            }
            Coordinate = 5.0f;
            while (static_cast<long double>(Coordinate) < aConst::GalaxySizeY * 1) {
                aVector::TPolygon2D_SplitChainByPoints(WorkingPolygon, EC_Struct::MakePointF(0.0f, Coordinate), EC_Struct::MakePointF(aConst::GalaxySizeX, Coordinate));
                Coordinate = Coordinate + 5.0L;
            }
            for (I = 0; I <= 7; ++I) {
                RaceOrder[I] = I;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(0, 3, Galaxy->RandomState)); cpp_range_2.next(K); ) {
                for (I = 0; I <= 7; ++I) {
                    RaceOrder[I] = aMyFunction::DecrementWrappedValue(RaceOrder[I], 0, 7);
                    RaceOrder[I] = aMyFunction::DecrementWrappedValue(RaceOrder[I], 0, 7);
                }
            }
            N = 0;
            for (I = 0; I <= 7; ++I) {
                if (pas::in_range(RaceOrder[I], 0, 4)) {
                    CoalitionPositions[N] = I;
                    ++N;
                    if (RaceOrder[I] == 2) {
                        HumanPosition = I;
                    }
                }
            }
            N = 0;
            do {
                do {
                    I = CoalitionPositions[aMyFunction::NextRandomIntRange(0, 4, Galaxy->RandomState)];
                    K = CoalitionPositions[aMyFunction::NextRandomIntRange(0, 4, Galaxy->RandomState)];
                } while (!(I != K));
                Attempts = RaceOrder[I];
                RaceOrder[I] = RaceOrder[K];
                RaceOrder[K] = Attempts;
                ++N;
            } while (!(N > 3 && PlayerRace == RaceOrder[HumanPosition]));
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[0]));
            Constellation->ResetGeneratedMapShape();
            Constellation->MapCenter.X = aMyFunction::RandomIntRange(0, 2) + MinimumConstellationDistance * 0.6L;
            Constellation->MapCenter.Y = aMyFunction::RandomIntRange(0, 2) + MinimumConstellationDistance * 0.6L;
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[1]));
            Constellation->ResetGeneratedMapShape();
            {
                pas::Extended cpp_left = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.X = cpp_left + (pas::real_divide(aConst::GalaxySizeX, 2.0L) - 1.0L);
            }
            Constellation->MapCenter.Y = aMyFunction::RandomIntRange(0, 2) + MinimumConstellationDistance * 0.6L;
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[2]));
            Constellation->ResetGeneratedMapShape();
            {
                pas::Extended cpp_left_2 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.X = cpp_left_2 + (aConst::GalaxySizeX - MinimumConstellationDistance * 0.6L - 2.0L);
            }
            Constellation->MapCenter.Y = aMyFunction::RandomIntRange(0, 2) + MinimumConstellationDistance * 0.6L;
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[3]));
            Constellation->ResetGeneratedMapShape();
            {
                pas::Extended cpp_left_3 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.X = cpp_left_3 + (aConst::GalaxySizeX - MinimumConstellationDistance * 0.4L - 2.0L);
            }
            {
                pas::Extended cpp_left_4 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.Y = cpp_left_4 + (pas::real_divide(aConst::GalaxySizeY, 2.0L) - 1.0L);
            }
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[4]));
            Constellation->ResetGeneratedMapShape();
            {
                pas::Extended cpp_left_5 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.X = cpp_left_5 + (aConst::GalaxySizeX - MinimumConstellationDistance * 0.6L - 2.0L);
            }
            {
                pas::Extended cpp_left_6 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.Y = cpp_left_6 + (aConst::GalaxySizeY - MinimumConstellationDistance * 0.6L - 2.0L);
            }
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[5]));
            Constellation->ResetGeneratedMapShape();
            {
                pas::Extended cpp_left_7 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.X = cpp_left_7 + (pas::real_divide(aConst::GalaxySizeX, 2.0L) - 1.0L);
            }
            {
                pas::Extended cpp_left_8 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.Y = cpp_left_8 + (aConst::GalaxySizeY - MinimumConstellationDistance * 0.6L - 2.0L);
            }
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[6]));
            Constellation->ResetGeneratedMapShape();
            Constellation->MapCenter.X = aMyFunction::RandomIntRange(0, 2) + MinimumConstellationDistance * 0.6L;
            {
                pas::Extended cpp_left_9 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.Y = cpp_left_9 + (aConst::GalaxySizeY - MinimumConstellationDistance * 0.6L - 2.0L);
            }
            Constellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[7]));
            Constellation->ResetGeneratedMapShape();
            Constellation->MapCenter.X = aMyFunction::RandomIntRange(0, 2) + MinimumConstellationDistance * 0.6L;
            {
                pas::Extended cpp_left_10 = aMyFunction::RandomIntRange(0, 2);
                Constellation->MapCenter.Y = cpp_left_10 + (pas::real_divide(aConst::GalaxySizeY, 2.0L) - 1.0L);
            }
            for (I = 0; I <= 7; ++I) {
                if (RaceOrder[I] == static_cast<std::int32_t>(PlayerRace)) {
                    break;
                }
            }
            OtherConstellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[I]));
            if (OtherConstellation->MapCenter.X < aConst::GalaxySizeX * 0.15L) {
                K = 1;
            } else {
                K = -1;
            }
            if (OtherConstellation->MapCenter.Y < aConst::GalaxySizeY * 0.2L) {
                Attempts = 1;
            } else {
                Attempts = -1;
            }
            if (Attempts == 1) {
                OtherConstellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[1]));
            } else {
                OtherConstellation = pas::list_at<TConstellation>(Constellations, static_cast<std::int32_t>(RaceOrder[5]));
            }
            Constellation = pas::list_at<TConstellation>(Constellations, pas::list_count(Constellations) - 1);
            Constellation->MapCenter = OtherConstellation->MapCenter;
            Constellation->MapCenter.X = static_cast<long double>(Constellation->MapCenter.X) + 6 * K;
            Constellation->MapCenter.Y = static_cast<long double>(Constellation->MapCenter.Y) - 4 * Attempts;
            OtherConstellation->MapCenter.X = static_cast<long double>(OtherConstellation->MapCenter.X) - 4 * K;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(8, pas::list_count(Constellations) - 2); cpp_range_3.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, I);
                Constellation->ResetGeneratedMapShape();
                Constellation->MapCenter.X = 0.0f;
                Constellation->MapCenter.Y = 0.0f;
                PlacementAttempts = 0;
                while (true) {
                    ++PlacementAttempts;
                    if (PlacementAttempts > 100) {
                        break;
                    }
                    Constellation->MapCenter.X = aMyFunction::RandomIntRange(0, aConst::GalaxySizeX - 1) + 1;
                    Constellation->MapCenter.Y = aMyFunction::RandomIntRange(0, aConst::GalaxySizeY - 1) + 1;
                    if (Constellation->MapCenter.X < MinimumConstellationDistance * 0.4L || Constellation->MapCenter.X > aConst::GalaxySizeX - MinimumConstellationDistance * 0.4L || Constellation->MapCenter.Y < MinimumConstellationDistance * 0.4L || Constellation->MapCenter.Y > aConst::GalaxySizeY - MinimumConstellationDistance * 0.4L) {
                        continue;
                    }
                    Occupied = false;
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, I); cpp_range_4.next(K); ) {
                        if (K == I) {
                            OtherConstellation = pas::list_at<TConstellation>(Constellations, pas::list_count(Constellations) - 1);
                        } else {
                            OtherConstellation = pas::list_at<TConstellation>(Constellations, K);
                        }
                        {
                            aVector::TPolygon2D* cpp_left_11 = aVector::TPolygon2D_FindContainingPolygon(WorkingPolygon, Constellation->MapCenter);
                            if (cpp_left_11 == aVector::TPolygon2D_FindContainingPolygon(WorkingPolygon, OtherConstellation->MapCenter)) {
                                Occupied = true;
                                break;
                            }
                        }
                    }
                    if (Occupied) {
                        continue;
                    }
                    NearestDistance = std::max<std::int32_t>(aConst::GalaxySizeX, aConst::GalaxySizeY);
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, I); cpp_range_5.next(K); ) {
                        if (K == I) {
                            OtherConstellation = pas::list_at<TConstellation>(Constellations, pas::list_count(Constellations) - 1);
                        } else {
                            OtherConstellation = pas::list_at<TConstellation>(Constellations, K);
                        }
                        Distance = System::Round(aMyFunction::PointDistance(Constellation->MapCenter, OtherConstellation->MapCenter));
                        if (Distance < NearestDistance) {
                            NearestDistance = Distance;
                        }
                    }
                    if (NearestDistance >= MinimumConstellationDistance && NearestDistance >= 7.0L) {
                        break;
                    }
                }
            }
            BuildConstellationPolygonsAndAdjacency(WorkingPolygon);
            pas::free(WorkingPolygon);
            InvalidLayout = false;
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_6.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, I);
                Constellation->NormalizeOutlineSegmentOrder();
            }
            BuildConstellationOutlineJunctions();
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_7.next(I); ) {
                SimplifyConstellationOutline(I);
            }
            TotalLinks = 0;
            AxisLinks = 0;
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_8.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, I);
                if (aVector::TPolygon2D_GetChainArea(Constellation->OutlinePolygons) < MinimumArea || aVector::TPolygon2D_GetChainArea(Constellation->OutlinePolygons) > MaximumArea) {
                    InvalidLayout = true;
                    break;
                }
                if (aVector::TPolygon2D_ChainSelfIntersects(Constellation->OutlinePolygons)) {
                    InvalidLayout = true;
                    break;
                }
                TotalLinks += pas::list_count(Constellation->StarLinks);
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->StarLinks) - 1); cpp_range_9.next(K); ) {
                    Link = pas::list_at<TConstellationStarLink>(Constellation->StarLinks, K);
                    if (Link->StartPoint.Y == Link->EndPoint.Y || Link->StartPoint.X == Link->EndPoint.X) {
                        ++AxisLinks;
                    }
                }
                for (auto cpp_range_10 = pas::for_to<std::int32_t>(I + 1, pas::list_count(Constellations) - 1); cpp_range_10.next(J); ) {
                    if (aVector::TPolygon2D_IntersectsChain(Constellation->OutlinePolygons, pas::list_at<TConstellation>(Constellations, J)->OutlinePolygons)) {
                        InvalidLayout = true;
                        break;
                    }
                }
                if (InvalidLayout) {
                    break;
                }
            }
            if (AxisLinks * 7 > TotalLinks) {
                InvalidLayout = true;
            }
            ++GenerationAttempts;
        } while (!(GenerationAttempts > 1000 || static_cast<std::uint8_t>(InvalidLayout ^ 1)));
        GenerationAttempts = 0;
        do {
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_11.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Constellations, I);
                Constellation->ClearStars();
            }
            for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_12.next(I); ) {
                Star = pas::list_at<TStar>(Galaxy->Stars, I);
                Star->Position.X = 0.0f;
                Star->Position.Y = 0.0f;
            }
            for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_13.next(I); ) {
                Star = pas::list_at<TStar>(Galaxy->Stars, I);
                if (I == 70 || I == 71) {
                    ConstellationIndex = pas::list_count(Constellations) - 1;
                } else {
                    if (I < 65) {
                        ConstellationIndex = I % 18;
                    } else {
                        ConstellationIndex = 18;
                        if (I > 68) {
                            ConstellationIndex = pas::imod(I, pas::list_count(Constellations) - 1);
                        }
                    }
                    if (I > pas::real_divide(pas::list_count(Galaxy->Stars), 1.46L) && pas::random_real(&System::RandSeed) < 0.6L) {
                        BestAreaPerStar = 0.0;
                        for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 2); cpp_range_14.next(K); ) {
                            if (I < 65 || K >= 8) {
                                Constellation = pas::list_at<TConstellation>(Constellations, K);
                                if (pas::list_count(Constellation->Stars) == 0) {
                                    AreaPerStar = Constellation->GetOutlineArea() * 2.0L;
                                } else {
                                    pas::Extended cpp_right = pas::list_count(Constellation->Stars);
                                    AreaPerStar = pas::real_divide(Constellation->GetOutlineArea(), cpp_right);
                                }
                                if (AreaPerStar > BestAreaPerStar) {
                                    ConstellationIndex = K;
                                    BestAreaPerStar = AreaPerStar;
                                }
                            }
                        }
                    }
                }
                Constellation = pas::list_at<TConstellation>(Constellations, ConstellationIndex);
                Constellation->AddStar(Star);
                Bounds = Constellation->OutlineBounds;
                BoundsSize = Constellation->OutlineBoundsSize;
                {
                    pas::Extended cpp_right_2 = pas::idiv(pas::list_count(Stars), pas::list_count(Constellations));
                    MinimumStarDistance = System::Round(System::Sqrt(pas::real_divide(Constellation->GetOutlineArea(), cpp_right_2)) * 0.5L);
                }
                FuelRange = aItem::CalculateGeneratedFuelCapacity(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1);
                Attempts = 0;
                while (true) {
                    // Separate tests release the conversion temporary between the X/Y bounds.
                    while (true) {
                        Star->Position.X = Bounds.Left + aMyFunction::RandomIntRange(0, System::Round(BoundsSize.X * 1.0L) - 1) + BoundsSize.X * 0.0L;
                        Star->Position.Y = Bounds.Top + aMyFunction::RandomIntRange(0, System::Round(BoundsSize.Y * 1.0L) - 1) + BoundsSize.Y * 0.0L;
                        if (Star->Position.X + 0.5L >= 8.0L) {
                            if (Star->Position.X + 0.5L <= aConst::GalaxySizeX - 8) {
                                if (Star->Position.Y + 0.5L >= 5.0L) {
                                    if (Star->Position.Y + 0.5L <= aConst::GalaxySizeY - 8) {
                                        if (Constellation->ContainsPoint(Star->Position)) {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ++Attempts;
                    if (Attempts > 100) {
                        break;
                    }
                    if (Constellation->IsPointNearOutline(Star->Position)) {
                        continue;
                    }
                    NearestDistance = SystemImports::MaxInt;
                    for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, I - 1); cpp_range_15.next(K); ) {
                        OtherStar = pas::list_at<TStar>(Galaxy->Stars, K);
                        Distance = System::Round(aMyFunction::PointDistance(Star->Position, OtherStar->Position));
                        if (NearestDistance > Distance) {
                            NearestDistance = Distance;
                        }
                    }
                    if (NearestDistance >= 4 && (NearestDistance <= FuelRange || PlayerRace != ConstellationIndex || ConstellationCount >= I) && (NearestDistance >= MinimumStarDistance || Attempts >= 20)) {
                        break;
                    }
                }
            }
            InvalidLayout = false;
            Reached.set_length(pas::list_count(Galaxy->Stars) + 1);
            for (auto cpp_range_16 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_16.next(K); ) {
                Reached[K] = false;
            }
            Reached[PlayerRace] = true;
            FuelRange = aItem::CalculateGeneratedFuelCapacity(System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]), 1);
            N = 0;
            for (auto cpp_range_17 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_17.next(FirstIndex); ) {
                OtherStar = pas::list_at<TStar>(Galaxy->Stars, FirstIndex);
                for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_18.next(SecondIndex); ) {
                    SecondStar = pas::list_at<TStar>(Galaxy->Stars, SecondIndex);
                    if (FirstIndex != SecondIndex && std::fabs(static_cast<long double>(OtherStar->Position.X) - SecondStar->Position.X) < 7.0L && std::fabs(static_cast<long double>(OtherStar->Position.Y) - SecondStar->Position.Y) < 2.0L) {
                        InvalidLayout = true;
                        break;
                    }
                    Distance = System::Round(aMyFunction::PointDistance(OtherStar->Position, SecondStar->Position));
                    if (Distance <= FuelRange && (Reached[FirstIndex] && static_cast<std::uint8_t>(Reached[SecondIndex] ^ 1) || Reached[SecondIndex] && static_cast<std::uint8_t>(Reached[FirstIndex] ^ 1))) {
                        Reached[FirstIndex] = true;
                        Reached[SecondIndex] = true;
                        ++N;
                        if (N > 2) {
                            break;
                        }
                    }
                }
                if (InvalidLayout) {
                    break;
                }
            }
            if (N < 3) {
                InvalidLayout = true;
            }
            if (!InvalidLayout) {
                if (!BuildConstellationStarGraphs()) {
                    InvalidLayout = true;
                }
            }
            ++GenerationAttempts;
        } while (!(GenerationAttempts > 50 || static_cast<std::uint8_t>(InvalidLayout ^ 1)));
        Reached.set_length(0);
    }

    // Requires constellation ID 20 and generated compatible outlines. Merges its visible outline into a neighbor and retains backups for RestoreHiddenForm.
    void TGalaxy::HideSpecialConstellation() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t SegmentIndex{};
        std::int32_t EarIndex{};
        std::int32_t LastIndex{};
        std::int32_t MiddleIndex{};
        TStar* Star{};
        std::uint8_t ContainsStar{};
        TConstellation* Current{};
        aVector::TPolygon2D* Polygon{};
        PMapLineSegment Segment{};
        pas::List* WorkX{};
        pas::List* WorkY{};
        float AreaAfterUnrestrictedTrim{};
        float AreaAfterStarSafeTrim{};
        float FullArea{};
        double Score{};
        double CrossEar{};
        double CrossMid{};
        float MidX{};
        float MidY{};
        float AX{};
        float BX{};
        float CX{};
        float AY{};
        float BY{};
        float CY{};
        float MaxX{};
        float MinX{};
        float MaxY{};
        float MinY{};
        float NewX{};
        float NewY{};
        float IntersectionX{};
        float Determinant{};
        EC_Struct::PPointF Point{};
        std::uint8_t Flag{};
        // Strict interior only; excludes edges and degenerate triangles. The caller-popped static link is unused.
        auto PointInsideTriangle = [&](float PointX, float PointY, float AX, float AY, float BX, float BY, float CX, float CY) -> std::uint8_t {
            std::uint8_t Result = false;
            if (((static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX)) * ((static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - PointY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - PointX)) > 0.0L && ((static_cast<long double>(CX) - BX) * (static_cast<long double>(CY) - AY) - (static_cast<long double>(CY) - BY) * (static_cast<long double>(CX) - AX)) * ((static_cast<long double>(CX) - BX) * (static_cast<long double>(CY) - PointY) - (static_cast<long double>(CY) - BY) * (static_cast<long double>(CX) - PointX)) > 0.0L && ((static_cast<long double>(AX) - CX) * (static_cast<long double>(AY) - BY) - (static_cast<long double>(AY) - CY) * (static_cast<long double>(AX) - BX)) * ((static_cast<long double>(AX) - CX) * (static_cast<long double>(AY) - PointY) - (static_cast<long double>(AY) - CY) * (static_cast<long double>(AX) - PointX)) > 0.0L) {
                return true;
            }
            return Result;
        };
        // Delphi's Pointer/Single hard casts preserve bits, including signed zero.
        // The temporary coordinate lists intentionally store those bits in pointer slots.
        pas::List* Neighbors = pas::make_object<pas::List>();
        pas::List* XList = pas::make_object<pas::List>();
        pas::List* YList = pas::make_object<pas::List>();
        pas::List* BestX = nullptr;
        pas::List* BestY = nullptr;
        TConstellation* Hidden = Galaxy->IdToConstellation(20u);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(I); ) {
            Current = pas::list_at<TConstellation>(Constellations, I);
            if (Current != Hidden) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Hidden->OutlineSegments) - 1); cpp_range_2.next(SegmentIndex); ) {
                    EC_Struct::TPointF endPoint = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint;
                    EC_Struct::TPointF startPoint = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint;
                    if (Current->HasOutlineSegment(startPoint, endPoint)) {
                        pas::list_add(Neighbors, reinterpret_cast<void*>(Current));
                        break;
                    }
                }
            }
        }
        double BestScore = -1.0;
        TConstellation* Candidate = nullptr;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Neighbors) - 1); cpp_range_3.next(I); ) {
            Current = pas::list_at<TConstellation>(Neighbors, I);
            pas::list_clear(XList);
            pas::list_clear(YList);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Hidden->OutlineSegments) - 1); cpp_range_4.next(SegmentIndex); ) {
                EC_Struct::TPointF endPoint_2 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint;
                EC_Struct::TPointF startPoint_2 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint;
                if (Current->HasOutlineSegment(startPoint_2, endPoint_2)) {
                    pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.X))));
                    pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.Y))));
                    pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.X))));
                    pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.Y))));
                }
            }
            MaxX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, 0))));
            MinX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, 0))));
            MaxY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, 0))));
            MinY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, 0))));
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, pas::list_count(XList) - 1); cpp_range_5.next(SegmentIndex); ) {
                MaxX = pas::real_max<float>(MaxX, std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, SegmentIndex)))));
                MinX = pas::real_min<float>(MinX, std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, SegmentIndex)))));
                MaxY = pas::real_max<float>(MaxY, std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, SegmentIndex)))));
                MinY = pas::real_min<float>(MinY, std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, SegmentIndex)))));
            }
            MidX = (static_cast<long double>(MaxX) + MinX) * 0.5L;
            MidY = (static_cast<long double>(MaxY) + MinY) * 0.5L;
            pas::list_clear(XList);
            pas::list_clear(YList);
            SegmentIndex = 0;
            while (([&] {
                EC_Struct::TPointF endPoint_3 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint;
                EC_Struct::TPointF startPoint_3 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint;
                return Current->HasOutlineSegment(startPoint_3, endPoint_3);
            }())) {
                ++SegmentIndex;
            }
            pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.X))));
            pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.Y))));
            pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.X))));
            pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.Y))));
            while (true) {
                MiddleIndex = pas::list_count(XList);
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Hidden->OutlineSegments) - 1); cpp_range_6.next(SegmentIndex); ) {
                    EC_Struct::TPointF endPoint_4 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint;
                    EC_Struct::TPointF startPoint_4 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint;
                    if (!Current->HasOutlineSegment(startPoint_4, endPoint_4)) {
                        if (([&] {
                            pas::Extended cpp_left = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.X;
                            return cpp_left == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 1))));
                        }()) && ([&] {
                            pas::Extended cpp_left_2 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.Y;
                            return cpp_left_2 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 1))));
                        }()) && !(([&] {
                            pas::Extended cpp_left_3 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.X;
                            return cpp_left_3 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 2))));
                        }()) && ([&] {
                            pas::Extended cpp_left_4 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.Y;
                            return cpp_left_4 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 2))));
                        }()))) {
                            pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.X))));
                            pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.Y))));
                        } else if (([&] {
                            pas::Extended cpp_left_5 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.X;
                            return cpp_left_5 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 1))));
                        }()) && ([&] {
                            pas::Extended cpp_left_6 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->EndPoint.Y;
                            return cpp_left_6 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 1))));
                        }()) && !(([&] {
                            pas::Extended cpp_left_7 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.X;
                            return cpp_left_7 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 2))));
                        }()) && ([&] {
                            pas::Extended cpp_left_8 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.Y;
                            return cpp_left_8 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 2))));
                        }()))) {
                            pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.X))));
                            pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, SegmentIndex)->StartPoint.Y))));
                        }
                        if (([&] {
                            void* cpp_left_9 = pas::list_get(XList, pas::list_count(XList) - 1);
                            return cpp_left_9 == pas::list_get(XList, 0);
                        }()) && ([&] {
                            void* cpp_left_10 = pas::list_get(YList, pas::list_count(YList) - 1);
                            return cpp_left_10 == pas::list_get(YList, 0);
                        }())) {
                            break;
                        }
                    }
                }
                if (([&] {
                    void* cpp_left_11 = pas::list_get(XList, pas::list_count(XList) - 1);
                    return cpp_left_11 == pas::list_get(XList, 0);
                }()) && ([&] {
                    void* cpp_left_12 = pas::list_get(YList, pas::list_count(YList) - 1);
                    return cpp_left_12 == pas::list_get(YList, 0);
                }())) {
                    pas::list_delete(XList, pas::list_count(XList) - 1);
                    pas::list_delete(YList, pas::list_count(YList) - 1);
                    break;
                }
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Current->OutlineSegments) - 1); cpp_range_7.next(SegmentIndex); ) {
                    EC_Struct::TPointF endPoint_5 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint;
                    EC_Struct::TPointF startPoint_5 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint;
                    if (!Hidden->HasOutlineSegment(startPoint_5, endPoint_5)) {
                        if (([&] {
                            pas::Extended cpp_left_13 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint.X;
                            return cpp_left_13 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 1))));
                        }()) && ([&] {
                            pas::Extended cpp_left_14 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint.Y;
                            return cpp_left_14 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 1))));
                        }()) && !(([&] {
                            pas::Extended cpp_left_15 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint.X;
                            return cpp_left_15 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 2))));
                        }()) && ([&] {
                            pas::Extended cpp_left_16 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint.Y;
                            return cpp_left_16 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 2))));
                        }()))) {
                            pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint.X))));
                            pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint.Y))));
                        } else if (([&] {
                            pas::Extended cpp_left_17 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint.X;
                            return cpp_left_17 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 1))));
                        }()) && ([&] {
                            pas::Extended cpp_left_18 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->EndPoint.Y;
                            return cpp_left_18 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 1))));
                        }()) && !(([&] {
                            pas::Extended cpp_left_19 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint.X;
                            return cpp_left_19 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(XList, pas::list_count(XList) - 2))));
                        }()) && ([&] {
                            pas::Extended cpp_left_20 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint.Y;
                            return cpp_left_20 == std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(YList, pas::list_count(XList) - 2))));
                        }()))) {
                            pas::list_add(XList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint.X))));
                            pas::list_add(YList, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(pas::list_at<TMapLineSegment>(Current->OutlineSegments, SegmentIndex)->StartPoint.Y))));
                        }
                        if (([&] {
                            void* cpp_left_21 = pas::list_get(XList, pas::list_count(XList) - 1);
                            return cpp_left_21 == pas::list_get(XList, 0);
                        }()) && ([&] {
                            void* cpp_left_22 = pas::list_get(YList, pas::list_count(YList) - 1);
                            return cpp_left_22 == pas::list_get(YList, 0);
                        }())) {
                            break;
                        }
                    }
                }
                if (([&] {
                    void* cpp_left_23 = pas::list_get(XList, pas::list_count(XList) - 1);
                    return cpp_left_23 == pas::list_get(XList, 0);
                }()) && ([&] {
                    void* cpp_left_24 = pas::list_get(YList, pas::list_count(YList) - 1);
                    return cpp_left_24 == pas::list_get(YList, 0);
                }())) {
                    pas::list_delete(XList, pas::list_count(XList) - 1);
                    pas::list_delete(YList, pas::list_count(YList) - 1);
                    break;
                }
                if (pas::list_count(XList) == MiddleIndex) {
                    break;
                }
            }
            Score = 0.0;
            WorkX = pas::make_object<pas::List>();
            WorkY = pas::make_object<pas::List>();
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(XList) - 1); cpp_range_8.next(EarIndex); ) {
                pas::list_add(WorkX, pas::list_get(XList, EarIndex));
                pas::list_add(WorkY, pas::list_get(YList, EarIndex));
            }
            FullArea = 0.0f;
            while (pas::list_count(WorkX) > 2) {
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_9.next(EarIndex); ) {
                    LastIndex = EarIndex + 2;
                    MiddleIndex = EarIndex + 1;
                    if (LastIndex >= pas::list_count(WorkX)) {
                        LastIndex -= pas::list_count(WorkX);
                    }
                    if (MiddleIndex >= pas::list_count(WorkX)) {
                        MiddleIndex -= pas::list_count(WorkX);
                    }
                    AX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, EarIndex))));
                    BX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, LastIndex))));
                    CX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, MiddleIndex))));
                    AY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, EarIndex))));
                    BY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, LastIndex))));
                    CY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, MiddleIndex))));
                    Flag = 0;
                    for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_10.next(J); ) {
                        if (J != EarIndex && J != LastIndex && J != MiddleIndex) {
                            {
                                float cpp_arg = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, J))));
                                float cpp_arg_2 = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, J))));
                                if (PointInsideTriangle(cpp_arg, cpp_arg_2, AX, AY, BX, BY, CX, CY)) {
                                    Flag = 1;
                                }
                            }
                            if (Flag == 1) {
                                break;
                            }
                        }
                    }
                    if (Flag == 1) {
                        continue;
                    }
                    CrossEar = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX);
                    CrossMid = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - MidY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - MidX);
                    if (static_cast<long double>(CrossEar) * CrossMid <= 0.0L || pas::list_count(WorkX) == 3) {
                        FullArea = FullArea + std::fabs(CrossEar * 0.5L);
                        pas::list_delete(WorkX, MiddleIndex);
                        pas::list_delete(WorkY, MiddleIndex);
                        break;
                    }
                }
            }
            pas::list_clear(WorkX);
            pas::list_clear(WorkY);
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(XList) - 1); cpp_range_11.next(EarIndex); ) {
                pas::list_add(WorkX, pas::list_get(XList, EarIndex));
                pas::list_add(WorkY, pas::list_get(YList, EarIndex));
            }
            AreaAfterUnrestrictedTrim = 0.0f;
            while (pas::list_count(WorkX) > 2) {
                Flag = 0;
                for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_12.next(EarIndex); ) {
                    LastIndex = EarIndex + 2;
                    MiddleIndex = EarIndex + 1;
                    if (LastIndex >= pas::list_count(WorkX)) {
                        LastIndex -= pas::list_count(WorkX);
                    }
                    if (MiddleIndex >= pas::list_count(WorkX)) {
                        MiddleIndex -= pas::list_count(WorkX);
                    }
                    AX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, EarIndex))));
                    BX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, LastIndex))));
                    CX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, MiddleIndex))));
                    AY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, EarIndex))));
                    BY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, LastIndex))));
                    CY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, MiddleIndex))));
                    CrossEar = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX);
                    CrossMid = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - MidY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - MidX);
                    if (static_cast<long double>(CrossEar) * CrossMid >= 0.0L && static_cast<std::uint8_t>(PointInsideTriangle(MidX, MidY, AX, AY, BX, BY, CX, CY) ^ 1)) {
                        pas::list_delete(WorkX, MiddleIndex);
                        pas::list_delete(WorkY, MiddleIndex);
                        Flag = 1;
                        break;
                    }
                }
                if (Flag != 0) {
                    break;
                }
            }
            while (pas::list_count(WorkX) > 2) {
                for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_13.next(EarIndex); ) {
                    LastIndex = EarIndex + 2;
                    MiddleIndex = EarIndex + 1;
                    if (LastIndex >= pas::list_count(WorkX)) {
                        LastIndex -= pas::list_count(WorkX);
                    }
                    if (MiddleIndex >= pas::list_count(WorkX)) {
                        MiddleIndex -= pas::list_count(WorkX);
                    }
                    AX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, EarIndex))));
                    BX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, LastIndex))));
                    CX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, MiddleIndex))));
                    AY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, EarIndex))));
                    BY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, LastIndex))));
                    CY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, MiddleIndex))));
                    Flag = 0;
                    for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_14.next(J); ) {
                        if (J != EarIndex && J != LastIndex && J != MiddleIndex) {
                            {
                                float cpp_arg_3 = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, J))));
                                float cpp_arg_4 = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, J))));
                                if (PointInsideTriangle(cpp_arg_3, cpp_arg_4, AX, AY, BX, BY, CX, CY)) {
                                    Flag = 1;
                                }
                            }
                            if (Flag == 1) {
                                break;
                            }
                        }
                    }
                    if (Flag == 1) {
                        continue;
                    }
                    CrossEar = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX);
                    CrossMid = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - MidY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - MidX);
                    if (static_cast<long double>(CrossEar) * CrossMid <= 0.0L || pas::list_count(WorkX) == 3) {
                        AreaAfterUnrestrictedTrim = AreaAfterUnrestrictedTrim + std::fabs(CrossEar * 0.5L);
                        pas::list_delete(WorkX, MiddleIndex);
                        pas::list_delete(WorkY, MiddleIndex);
                        break;
                    }
                }
            }
            pas::list_clear(WorkX);
            pas::list_clear(WorkY);
            for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, pas::list_count(XList) - 1); cpp_range_15.next(EarIndex); ) {
                pas::list_add(WorkX, pas::list_get(XList, EarIndex));
                pas::list_add(WorkY, pas::list_get(YList, EarIndex));
            }
            AreaAfterStarSafeTrim = 0.0f;
            while (pas::list_count(WorkX) > 2) {
                Flag = 0;
                for (auto cpp_range_16 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_16.next(EarIndex); ) {
                    LastIndex = EarIndex + 2;
                    MiddleIndex = EarIndex + 1;
                    if (LastIndex >= pas::list_count(WorkX)) {
                        LastIndex -= pas::list_count(WorkX);
                    }
                    if (MiddleIndex >= pas::list_count(WorkX)) {
                        MiddleIndex -= pas::list_count(WorkX);
                    }
                    AX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, EarIndex))));
                    BX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, LastIndex))));
                    CX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, MiddleIndex))));
                    AY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, EarIndex))));
                    BY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, LastIndex))));
                    CY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, MiddleIndex))));
                    CrossEar = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX);
                    CrossMid = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - MidY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - MidX);
                    if (static_cast<long double>(CrossEar) * CrossMid >= 0.0L && static_cast<std::uint8_t>(PointInsideTriangle(MidX, MidY, AX, AY, BX, BY, CX, CY) ^ 1)) {
                        ContainsStar = false;
                        for (auto cpp_range_17 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_17.next(J); ) {
                            Star = pas::list_at<TStar>(Stars, J);
                            ContainsStar = PointInsideTriangle(Star->Position.X, Star->Position.Y, AX, AY, BX, BY, CX, CY);
                            if (ContainsStar) {
                                break;
                            }
                        }
                        if (ContainsStar) {
                            continue;
                        }
                        pas::list_delete(WorkX, MiddleIndex);
                        pas::list_delete(WorkY, MiddleIndex);
                        Flag = 1;
                        break;
                    }
                }
                if (Flag != 0) {
                    break;
                }
            }
            while (pas::list_count(WorkX) > 2) {
                for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_18.next(EarIndex); ) {
                    LastIndex = EarIndex + 2;
                    MiddleIndex = EarIndex + 1;
                    if (LastIndex >= pas::list_count(WorkX)) {
                        LastIndex -= pas::list_count(WorkX);
                    }
                    if (MiddleIndex >= pas::list_count(WorkX)) {
                        MiddleIndex -= pas::list_count(WorkX);
                    }
                    AX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, EarIndex))));
                    BX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, LastIndex))));
                    CX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, MiddleIndex))));
                    AY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, EarIndex))));
                    BY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, LastIndex))));
                    CY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, MiddleIndex))));
                    Flag = 0;
                    for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, pas::list_count(WorkX) - 1); cpp_range_19.next(J); ) {
                        if (J != EarIndex && J != LastIndex && J != MiddleIndex) {
                            {
                                float cpp_arg_5 = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkX, J))));
                                float cpp_arg_6 = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(WorkY, J))));
                                if (PointInsideTriangle(cpp_arg_5, cpp_arg_6, AX, AY, BX, BY, CX, CY)) {
                                    Flag = 1;
                                }
                            }
                            if (Flag == 1) {
                                break;
                            }
                        }
                    }
                    if (Flag == 1) {
                        continue;
                    }
                    CrossEar = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX);
                    CrossMid = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - MidY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - MidX);
                    if (static_cast<long double>(CrossEar) * CrossMid <= 0.0L || pas::list_count(WorkX) == 3) {
                        AreaAfterStarSafeTrim = AreaAfterStarSafeTrim + std::fabs(CrossEar * 0.5L);
                        pas::list_delete(WorkX, MiddleIndex);
                        pas::list_delete(WorkY, MiddleIndex);
                        break;
                    }
                }
            }
            pas::list_clear(WorkX);
            pas::list_clear(WorkY);
            pas::free(WorkX);
            pas::free(WorkY);
            Score = pas::real_divide((static_cast<long double>(AreaAfterUnrestrictedTrim) - AreaAfterStarSafeTrim) * 0.5L + (static_cast<long double>(AreaAfterStarSafeTrim) - FullArea), FullArea);
            if (Score < BestScore || BestScore < 0.0L) {
                BestScore = Score;
                Candidate = Current;
                if (BestX != nullptr) {
                    pas::free(BestX);
                }
                if (BestY != nullptr) {
                    pas::free(BestY);
                }
                BestX = XList;
                BestY = YList;
                XList = pas::make_object<pas::List>();
                YList = pas::make_object<pas::List>();
            }
        }
        Current = Candidate;
        for (auto cpp_range_20 = pas::for_to<std::int32_t>(0, pas::list_count(BestX) - 1); cpp_range_20.next(EarIndex); ) {
            LastIndex = EarIndex + 2;
            MiddleIndex = EarIndex + 1;
            if (LastIndex >= pas::list_count(BestX)) {
                LastIndex -= pas::list_count(BestX);
            }
            if (MiddleIndex >= pas::list_count(BestX)) {
                MiddleIndex -= pas::list_count(BestX);
            }
            AX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(BestX, EarIndex))));
            BX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(BestX, LastIndex))));
            CX = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(BestX, MiddleIndex))));
            AY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(BestY, EarIndex))));
            BY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(BestY, LastIndex))));
            CY = std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(BestY, MiddleIndex))));
            CrossEar = (static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX);
            if (CrossEar != 0.0L) {
                for (auto cpp_range_21 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_21.next(I); ) {
                    ContainsStar = false;
                    Candidate = pas::list_at<TConstellation>(Constellations, I);
                    if (Candidate != Current && Candidate != Hidden && Candidate->HasOutlineVertex(EC_Struct::MakePointF(AX, AY)) && Candidate->HasOutlineVertex(EC_Struct::MakePointF(BX, BY)) && Candidate->HasOutlineVertex(EC_Struct::MakePointF(CX, CY))) {
                        for (auto cpp_range_22 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_22.next(J); ) {
                            Star = pas::list_at<TStar>(Stars, J);
                            if (((static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - CY) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - CX)) * ((static_cast<long double>(BX) - AX) * (static_cast<long double>(BY) - Star->Position.Y) - (static_cast<long double>(BY) - AY) * (static_cast<long double>(BX) - Star->Position.X)) >= 0.0L && ((static_cast<long double>(CX) - BX) * (static_cast<long double>(CY) - AY) - (static_cast<long double>(CY) - BY) * (static_cast<long double>(CX) - AX)) * ((static_cast<long double>(CX) - BX) * (static_cast<long double>(CY) - Star->Position.Y) - (static_cast<long double>(CY) - BY) * (static_cast<long double>(CX) - Star->Position.X)) >= 0.0L && ((static_cast<long double>(AX) - CX) * (static_cast<long double>(AY) - BY) - (static_cast<long double>(AY) - CY) * (static_cast<long double>(AX) - BX)) * ((static_cast<long double>(AX) - CX) * (static_cast<long double>(AY) - Star->Position.Y) - (static_cast<long double>(AY) - CY) * (static_cast<long double>(AX) - Star->Position.X)) >= 0.0L) {
                                ContainsStar = true;
                            }
                        }
                        if (ContainsStar) {
                            continue;
                        }
                        NewX = AX - (static_cast<long double>(BY) - CY);
                        NewY = AY + (static_cast<long double>(BX) - CX);
                        for (auto cpp_range_23 = pas::for_to<std::int32_t>(0, pas::list_count(Hidden->OutlineSegments) - 1); cpp_range_23.next(J); ) {
                            if (pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.X == AX && pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.Y == AY) {
                                continue;
                            }
                            if (pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.X == BX && pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.Y == BY) {
                                continue;
                            }
                            if (pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.X == AX && pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.Y == AY) {
                                continue;
                            }
                            if (pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.X == BX && pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.Y == BY) {
                                continue;
                            }
                            if (pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.X == CX && pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.Y == CY) {
                                NewX = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.X;
                                NewY = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.Y;
                                break;
                            }
                            if (pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.X == CX && pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->EndPoint.Y == CY) {
                                NewX = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.X;
                                NewY = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, J)->StartPoint.Y;
                                break;
                            }
                        }
                        Determinant = (static_cast<long double>(BY) - AY) * (static_cast<long double>(CX) - NewX) - (static_cast<long double>(CY) - NewY) * (static_cast<long double>(BX) - AX);
                        if (static_cast<long double>(Determinant) * Determinant > 0.01L) {
                            IntersectionX = pas::real_divide(static_cast<long double>(-BX) * AY * CX + static_cast<long double>(BX) * AY * NewX - static_cast<long double>(AX) * BY * NewX - static_cast<long double>(AX) * CX * NewY + static_cast<long double>(AX) * NewX * CY + static_cast<long double>(BX) * CX * NewY - static_cast<long double>(BX) * NewX * CY + static_cast<long double>(AX) * BY * CX, Determinant);
                            NewY = pas::real_divide(static_cast<long double>(NewY) * BX * AY - static_cast<long double>(CY) * BX * AY - static_cast<long double>(AY) * CX * NewY + static_cast<long double>(AY) * NewX * CY + static_cast<long double>(CY) * AX * BY + static_cast<long double>(BY) * CX * NewY - static_cast<long double>(BY) * NewX * CY - static_cast<long double>(NewY) * AX * BY, Determinant);
                            NewX = IntersectionX;
                        } else {
                            NewX = (static_cast<long double>(AX) + BX) * 0.5L;
                            NewY = (static_cast<long double>(AY) + BY) * 0.5L;
                        }
                        do {
                            for (auto cpp_range_24 = pas::for_to<std::int32_t>(0, pas::list_count(Candidate->OutlineSegments) - 1); cpp_range_24.next(J); ) {
                                if (pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->StartPoint.X == CX && pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->StartPoint.Y == CY) {
                                    pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->StartPoint.X = NewX;
                                    pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->StartPoint.Y = NewY;
                                }
                                if (pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->EndPoint.X == CX && pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->EndPoint.Y == CY) {
                                    pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->EndPoint.X = NewX;
                                    pas::list_at<TMapLineSegment>(Candidate->OutlineSegments, J)->EndPoint.Y = NewY;
                                }
                            }
                            Polygon = nullptr;
                            for (auto cpp_range_25 = pas::for_to<std::int32_t>(0, aVector::TPolygon2D_CountChain(Candidate->OutlinePolygons) - 1); cpp_range_25.next(J); ) {
                                if (J == 0) {
                                    Polygon = Candidate->OutlinePolygons;
                                } else {
                                    Polygon = Polygon->Next;
                                }
                                for (auto cpp_range_26 = pas::for_to<std::int32_t>(0, pas::list_count(Polygon->Points) - 1); cpp_range_26.next(K); ) {
                                    if (pas::list_at<EC_Struct::TPointF>(Polygon->Points, K)->X == CX && pas::list_at<EC_Struct::TPointF>(Polygon->Points, K)->Y == CY) {
                                        pas::list_at<EC_Struct::TPointF>(Polygon->Points, K)->X = NewX;
                                        pas::list_at<EC_Struct::TPointF>(Polygon->Points, K)->Y = NewY;
                                    }
                                }
                            }
                            if (Candidate == Hidden) {
                                Candidate = nullptr;
                            } else if (Candidate == Current) {
                                Candidate = Hidden;
                            } else {
                                Candidate = Current;
                            }
                        } while (!(Candidate == nullptr));
                        CX = NewX;
                        CY = NewY;
                        pas::list_put(BestX, MiddleIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(CX))));
                        pas::list_put(BestY, MiddleIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(CY))));
                    }
                }
            }
        }
        pas::list_clear(Neighbors);
        pas::list_clear(XList);
        pas::list_clear(YList);
        pas::list_clear(BestX);
        pas::list_clear(BestY);
        {
            const std::int32_t cpp_first = pas::list_count(Hidden->OutlineSegments) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    pas::get_mem_at(&Segment, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
                    Segment->StartPoint.X = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, I)->StartPoint.X;
                    Segment->StartPoint.Y = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, I)->StartPoint.Y;
                    Segment->EndPoint.X = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, I)->EndPoint.X;
                    Segment->EndPoint.Y = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, I)->EndPoint.Y;
                    pas::list_add(Hidden->HiddenOutlineSegmentsBackup, static_cast<void*>(Segment));
                }
            }
        }
        {
            const std::int32_t cpp_first_2 = pas::list_count(Current->OutlineSegments) - 1;
            if (cpp_first_2 >= 0) {
                for (I = cpp_first_2; I >= 0; --I) {
                    pas::get_mem_at(&Segment, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
                    Segment->StartPoint.X = pas::list_at<TMapLineSegment>(Current->OutlineSegments, I)->StartPoint.X;
                    Segment->StartPoint.Y = pas::list_at<TMapLineSegment>(Current->OutlineSegments, I)->StartPoint.Y;
                    Segment->EndPoint.X = pas::list_at<TMapLineSegment>(Current->OutlineSegments, I)->EndPoint.X;
                    Segment->EndPoint.Y = pas::list_at<TMapLineSegment>(Current->OutlineSegments, I)->EndPoint.Y;
                    pas::list_add(Current->HiddenOutlineSegmentsBackup, static_cast<void*>(Segment));
                }
            }
        }
        {
            const std::int32_t cpp_first_3 = pas::list_count(Hidden->OutlineSegments) - 1;
            if (cpp_first_3 >= 0) {
                for (I = cpp_first_3; I >= 0; --I) {
                    EC_Struct::TPointF endPoint_6 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, I)->EndPoint;
                    EC_Struct::TPointF startPoint_6 = pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, I)->StartPoint;
                    if (Current->HasOutlineSegment(startPoint_6, endPoint_6)) {
                        EarIndex = I;
                        pas::dispose(pas::list_at<TMapLineSegment>(Hidden->OutlineSegments, EarIndex));
                        pas::list_delete(Hidden->OutlineSegments, EarIndex);
                    }
                }
            }
        }
        void* SwapList = Hidden->HiddenOutlineSegmentsBackup;
        Hidden->HiddenOutlineSegmentsBackup = Hidden->OutlineSegments;
        Hidden->OutlineSegments = static_cast<pas::List*>(SwapList);
        {
            const std::int32_t cpp_first_4 = pas::list_count(Current->OutlineSegments) - 1;
            if (cpp_first_4 >= 0) {
                for (I = cpp_first_4; I >= 0; --I) {
                    EC_Struct::TPointF endPoint_7 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, I)->EndPoint;
                    EC_Struct::TPointF startPoint_7 = pas::list_at<TMapLineSegment>(Current->OutlineSegments, I)->StartPoint;
                    if (Hidden->HasOutlineSegment(startPoint_7, endPoint_7)) {
                        LastIndex = I;
                        pas::dispose(pas::list_at<TMapLineSegment>(Current->OutlineSegments, LastIndex));
                        pas::list_delete(Current->OutlineSegments, LastIndex);
                    }
                }
            }
        }
        SwapList = Hidden->HiddenOutlineSegmentsBackup;
        Hidden->HiddenOutlineSegmentsBackup = Hidden->OutlineSegments;
        Hidden->OutlineSegments = static_cast<pas::List*>(SwapList);
        SwapList = nullptr;
        while (pas::list_count(Current->OutlineSegments) > 0) {
            pas::list_add(XList, pas::list_get(Current->OutlineSegments, 0));
            pas::list_delete(Current->OutlineSegments, 0);
        }
        while (pas::list_count(Hidden->OutlineSegments) > 0) {
            pas::list_add(XList, pas::list_get(Hidden->OutlineSegments, 0));
            pas::list_delete(Hidden->OutlineSegments, 0);
        }
        pas::list_add(Current->OutlineSegments, pas::list_get(XList, 0));
        pas::list_delete(XList, 0);
        MiddleIndex = 0;
        {
            const std::int32_t cpp_first_5 = pas::list_count(XList) - 1;
            if (cpp_first_5 >= 0) {
                for (SegmentIndex = cpp_first_5; SegmentIndex >= 0; --SegmentIndex) {
                    if (([&] {
                        pas::Extended cpp_left_25 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.X;
                        return cpp_left_25 == pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_26 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.Y;
                        return cpp_left_26 == pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.Y;
                    }())) {
                        pas::dispose(pas::list_at<TMapLineSegment>(XList, SegmentIndex));
                        pas::list_delete(XList, SegmentIndex);
                    }
                }
            }
        }
        while (pas::list_count(XList) > 0) {
            const std::int32_t cpp_first_6 = pas::list_count(XList) - 1;
            if (cpp_first_6 >= 0) {
                for (SegmentIndex = cpp_first_6; SegmentIndex >= 0; --SegmentIndex) {
                    if (([&] {
                        pas::Extended cpp_left_27 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.X;
                        return cpp_left_27 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->StartPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_28 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.Y;
                        return cpp_left_28 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->StartPoint.Y;
                    }()) || ([&] {
                        pas::Extended cpp_left_29 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.X;
                        return cpp_left_29 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->EndPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_30 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.Y;
                        return cpp_left_30 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->EndPoint.Y;
                    }()) || ([&] {
                        pas::Extended cpp_left_31 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.X;
                        return cpp_left_31 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->StartPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_32 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.Y;
                        return cpp_left_32 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->StartPoint.Y;
                    }()) || ([&] {
                        pas::Extended cpp_left_33 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.X;
                        return cpp_left_33 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->EndPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_34 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.Y;
                        return cpp_left_34 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, MiddleIndex)->EndPoint.Y;
                    }())) {
                        pas::list_add(Current->OutlineSegments, pas::list_get(XList, SegmentIndex));
                        pas::list_delete(XList, SegmentIndex);
                        ++MiddleIndex;
                    } else if (([&] {
                        pas::Extended cpp_left_35 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.X;
                        return cpp_left_35 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->StartPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_36 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.Y;
                        return cpp_left_36 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->StartPoint.Y;
                    }()) || ([&] {
                        pas::Extended cpp_left_37 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.X;
                        return cpp_left_37 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->EndPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_38 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->StartPoint.Y;
                        return cpp_left_38 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->EndPoint.Y;
                    }()) || ([&] {
                        pas::Extended cpp_left_39 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.X;
                        return cpp_left_39 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->StartPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_40 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.Y;
                        return cpp_left_40 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->StartPoint.Y;
                    }()) || ([&] {
                        pas::Extended cpp_left_41 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.X;
                        return cpp_left_41 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->EndPoint.X;
                    }()) && ([&] {
                        pas::Extended cpp_left_42 = pas::list_at<TMapLineSegment>(XList, SegmentIndex)->EndPoint.Y;
                        return cpp_left_42 == pas::list_at<TMapLineSegment>(Current->OutlineSegments, 0)->EndPoint.Y;
                    }())) {
                        pas::list_insert(Current->OutlineSegments, 0, pas::list_get(XList, SegmentIndex));
                        pas::list_delete(XList, SegmentIndex);
                        ++MiddleIndex;
                    }
                }
            }
        }
        aVector::TPolygon2D* SourcePolygon = nullptr;
        for (auto cpp_range_27 = pas::for_to<std::int32_t>(0, aVector::TPolygon2D_CountChain(Hidden->OutlinePolygons) - 1); cpp_range_27.next(I); ) {
            Polygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
            if (I == 0) {
                Hidden->HiddenOutlinePolygonsBackup = Polygon;
                SourcePolygon = Hidden->OutlinePolygons;
            } else {
                Hidden->HiddenOutlinePolygonsBackup->Append(Polygon);
                SourcePolygon = SourcePolygon->Next;
            }
            for (auto cpp_range_28 = pas::for_to<std::int32_t>(0, pas::list_count(SourcePolygon->Points) - 1); cpp_range_28.next(J); ) {
                pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                Point->X = pas::list_at<EC_Struct::TPointF>(SourcePolygon->Points, J)->X;
                Point->Y = pas::list_at<EC_Struct::TPointF>(SourcePolygon->Points, J)->Y;
                pas::list_add(Polygon->Points, static_cast<void*>(Point));
            }
            Polygon->Extent.X = SourcePolygon->Extent.X;
            Polygon->Extent.Y = SourcePolygon->Extent.Y;
            Polygon->Bounds.Left = SourcePolygon->Bounds.Left;
            Polygon->Bounds.Top = SourcePolygon->Bounds.Top;
            Polygon->Bounds.Right = SourcePolygon->Bounds.Right;
            Polygon->Bounds.Bottom = SourcePolygon->Bounds.Bottom;
        }
        for (auto cpp_range_29 = pas::for_to<std::int32_t>(0, aVector::TPolygon2D_CountChain(Current->OutlinePolygons) - 1); cpp_range_29.next(I); ) {
            Polygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
            if (I == 0) {
                Current->HiddenOutlinePolygonsBackup = Polygon;
                SourcePolygon = Current->OutlinePolygons;
            } else {
                Current->HiddenOutlinePolygonsBackup->Append(Polygon);
                SourcePolygon = SourcePolygon->Next;
            }
            for (auto cpp_range_30 = pas::for_to<std::int32_t>(0, pas::list_count(SourcePolygon->Points) - 1); cpp_range_30.next(J); ) {
                pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                Point->X = pas::list_at<EC_Struct::TPointF>(SourcePolygon->Points, J)->X;
                Point->Y = pas::list_at<EC_Struct::TPointF>(SourcePolygon->Points, J)->Y;
                pas::list_add(Polygon->Points, static_cast<void*>(Point));
            }
            Polygon->Extent.X = SourcePolygon->Extent.X;
            Polygon->Extent.Y = SourcePolygon->Extent.Y;
            Polygon->Bounds.Left = SourcePolygon->Bounds.Left;
            Polygon->Bounds.Top = SourcePolygon->Bounds.Top;
            Polygon->Bounds.Right = SourcePolygon->Bounds.Right;
            Polygon->Bounds.Bottom = SourcePolygon->Bounds.Bottom;
        }
        Current->OutlinePolygons->Append(Hidden->OutlinePolygons);
        Hidden->OutlinePolygons = nullptr;
        pas::free(Neighbors);
        pas::free(XList);
        pas::free(YList);
        pas::free(BestX);
        pas::free(BestY);
    }

    void TConstellation::RestoreHiddenForm() {
        if (HiddenOutlinePolygonsBackup != nullptr) {
            pas::free(OutlinePolygons);
            OutlinePolygons = HiddenOutlinePolygonsBackup;
            HiddenOutlinePolygonsBackup = nullptr;
        }
        if (pas::list_count(HiddenOutlineSegmentsBackup) != 0) {
            pas::free(OutlineSegments);
            OutlineSegments = HiddenOutlineSegmentsBackup;
            HiddenOutlineSegmentsBackup = pas::make_object<pas::List>();
        }
    }

    std::int32_t TGalaxy::CountVisibleConstellationsWithBoundaryPoints(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint) {
        std::int32_t I{};
        TConstellation* Constellation{};
        std::int32_t Result = 0;
        std::int32_t Count = pas::list_count(Constellations);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            if (Constellation->Visible && Constellation->AreBothPointsOnOutline(FirstPoint, SecondPoint)) {
                ++Result;
            }
        }
        return Result;
    }

    void TConstellation_Create(TConstellation* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (Galaxy != nullptr) {
            Self->Id = Galaxy->NextConstellationId;
            ++Galaxy->NextConstellationId;
        }
        Self->MapCenter = EC_Struct::MakePointF(0.0f, 0.0f);
        Self->Stars = pas::make_object<pas::List>();
        Self->AdjacentConstellations = pas::make_object<pas::List>();
        Self->OutlineBounds = ClassesImports::Rect(0, 0, 0, 0);
        Self->OutlineBoundsSize = ClassesImports::Point(0, 0);
        Self->StarLinks = pas::make_object<pas::List>();
        Self->OutlinePolygons = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
        Self->BoundaryRaySamples = pas::make_object<pas::List>();
        Self->OutlineSegments = pas::make_object<pas::List>();
        Self->HiddenOutlineSegmentsBackup = pas::make_object<pas::List>();
    }

    void TConstellation_Destroy(TConstellation* Self) {
        std::int32_t I{};
        Self->ClearStarLinks();
        Self->ClearOutlineSegmentsAndBounds();
        Self->ClearBoundaryRaySamples();
        pas::free(Self->StarLinks);
        pas::free(Self->OutlineSegments);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->HiddenOutlineSegmentsBackup) - 1); cpp_range.next(I); ) {
            pas::dispose(pas::list_at<TMapLineSegment>(Self->HiddenOutlineSegmentsBackup, I));
        }
        pas::list_clear(Self->HiddenOutlineSegmentsBackup);
        pas::free(Self->HiddenOutlineSegmentsBackup);
        pas::free(Self->AdjacentConstellations);
        pas::free(Self->Stars);
        pas::free(Self->OutlinePolygons);
        pas::free(Self->HiddenOutlinePolygonsBackup);
        pas::free(Self->BoundaryRaySamples);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TConstellation::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t i{};
        std::int32_t j{};
        TStar* Star{};
        TConstellation* Constellation{};
        PMapLineSegment Segment{};
        EC_Struct::PPointF Point{};
        Buffer->AddDWord(Id);
        Buffer->AddBoolean(Visible);
        Buffer->AddWideChar(SerializedValue88);
        Buffer->AddSingle(MapCenter.X);
        Buffer->AddSingle(MapCenter.Y);
        Buffer->AddWideChar(pas::list_count(Stars));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(i); ) {
            Star = pas::list_at<TStar>(Stars, i);
            Buffer->AddDWord(Star->Id);
        }
        Buffer->AddWideChar(pas::list_count(AdjacentConstellations));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(AdjacentConstellations) - 1); cpp_range_2.next(i); ) {
            Constellation = pas::list_at<TConstellation>(AdjacentConstellations, i);
            Buffer->AddDWord(Constellation->Id);
        }
        Buffer->AddWideChar(pas::list_count(OutlineSegments));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range_3.next(i); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, i);
            Buffer->AddSingle(Segment->StartPoint.X);
            Buffer->AddSingle(Segment->StartPoint.Y);
            Buffer->AddSingle(Segment->EndPoint.X);
            Buffer->AddSingle(Segment->EndPoint.Y);
        }
        Buffer->AddWideChar(pas::list_count(HiddenOutlineSegmentsBackup));
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(HiddenOutlineSegmentsBackup) - 1); cpp_range_4.next(i); ) {
            Segment = pas::list_at<TMapLineSegment>(HiddenOutlineSegmentsBackup, i);
            Buffer->AddSingle(Segment->StartPoint.X);
            Buffer->AddSingle(Segment->StartPoint.Y);
            Buffer->AddSingle(Segment->EndPoint.X);
            Buffer->AddSingle(Segment->EndPoint.Y);
        }
        Buffer->AddIntegerValue(OutlineBounds.Left);
        Buffer->AddIntegerValue(OutlineBounds.Top);
        Buffer->AddIntegerValue(OutlineBounds.Right);
        Buffer->AddIntegerValue(OutlineBounds.Bottom);
        Buffer->AddIntegerValue(OutlineBoundsSize.X);
        Buffer->AddIntegerValue(OutlineBoundsSize.Y);
        Buffer->AddWideChar(pas::list_count(StarLinks));
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range_5.next(i); ) {
            Segment = pas::list_at<TMapLineSegment>(StarLinks, i);
            Buffer->AddSingle(Segment->StartPoint.X);
            Buffer->AddSingle(Segment->StartPoint.Y);
            Buffer->AddSingle(Segment->EndPoint.X);
            Buffer->AddSingle(Segment->EndPoint.Y);
        }
        Buffer->AddWideChar(aVector::TPolygon2D_CountChain(OutlinePolygons));
        aVector::TPolygon2D* Polygon = OutlinePolygons;
        while (Polygon != nullptr) {
            Buffer->AddWideChar(pas::list_count(Polygon->Points));
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Polygon->Points) - 1); cpp_range_6.next(j); ) {
                Point = pas::list_at<EC_Struct::TPointF>(Polygon->Points, j);
                Buffer->AddSingle(Point->X);
                Buffer->AddSingle(Point->Y);
            }
            Buffer->AddSingle(Polygon->Extent.X);
            Buffer->AddSingle(Polygon->Extent.Y);
            Buffer->AddSingle(Polygon->Bounds.Left);
            Buffer->AddSingle(Polygon->Bounds.Top);
            Buffer->AddSingle(Polygon->Bounds.Right);
            Buffer->AddSingle(Polygon->Bounds.Bottom);
            Polygon = Polygon->Next;
        }
        Buffer->AddWideChar(aVector::TPolygon2D_CountChain(HiddenOutlinePolygonsBackup));
        Polygon = HiddenOutlinePolygonsBackup;
        while (Polygon != nullptr) {
            Buffer->AddWideChar(pas::list_count(Polygon->Points));
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Polygon->Points) - 1); cpp_range_7.next(j); ) {
                Point = pas::list_at<EC_Struct::TPointF>(Polygon->Points, j);
                Buffer->AddSingle(Point->X);
                Buffer->AddSingle(Point->Y);
            }
            Buffer->AddSingle(Polygon->Extent.X);
            Buffer->AddSingle(Polygon->Extent.Y);
            Buffer->AddSingle(Polygon->Bounds.Left);
            Buffer->AddSingle(Polygon->Bounds.Top);
            Buffer->AddSingle(Polygon->Bounds.Right);
            Buffer->AddSingle(Polygon->Bounds.Bottom);
            Polygon = Polygon->Next;
        }
    }

    // Requires a fresh instance; saved object IDs remain unresolved.
    void TConstellation::LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy) {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t PointCount{};
        PMapLineSegment Segment{};
        aVector::TPolygon2D* Polygon{};
        EC_Struct::PPointF Point{};
        Id = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Galaxy->NextConstellationId <= Id) {
            Galaxy->NextConstellationId = Id + 1;
        }
        Visible = EC_Buf::TBufEC_GetBoolean(Buffer);
        SerializedValue88 = EC_Buf::TBufEC_GetWord(Buffer);
        MapCenter.X = EC_Buf::TBufEC_GetSingle(Buffer);
        MapCenter.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            void* uInt32 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            pas::List* stars = Stars;
            pas::list_add(stars, uInt32);
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
            void* uInt32_2 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            pas::List* adjacentConstellations = AdjacentConstellations;
            pas::list_add(adjacentConstellations, uInt32_2);
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(i); ) {
            pas::get_mem_at(&Segment, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
            Segment->StartPoint.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Segment->StartPoint.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            Segment->EndPoint.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Segment->EndPoint.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            pas::list_add(OutlineSegments, static_cast<void*>(Segment));
        }
        if (GlobalsV::LoadedSaveVersion >= 45) {
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(i); ) {
                pas::get_mem_at(&Segment, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
                Segment->StartPoint.X = EC_Buf::TBufEC_GetSingle(Buffer);
                Segment->StartPoint.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                Segment->EndPoint.X = EC_Buf::TBufEC_GetSingle(Buffer);
                Segment->EndPoint.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                pas::list_add(HiddenOutlineSegmentsBackup, static_cast<void*>(Segment));
            }
        }
        OutlineBounds.Left = EC_Buf::TBufEC_GetInt32(Buffer);
        OutlineBounds.Top = EC_Buf::TBufEC_GetInt32(Buffer);
        OutlineBounds.Right = EC_Buf::TBufEC_GetInt32(Buffer);
        OutlineBounds.Bottom = EC_Buf::TBufEC_GetInt32(Buffer);
        OutlineBoundsSize.X = EC_Buf::TBufEC_GetInt32(Buffer);
        OutlineBoundsSize.Y = EC_Buf::TBufEC_GetInt32(Buffer);
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(i); ) {
            pas::get_mem_at(&Segment, static_cast<std::int32_t>(sizeof(TMapLineSegment)));
            Segment->StartPoint.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Segment->StartPoint.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            Segment->EndPoint.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Segment->EndPoint.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            pas::list_add(StarLinks, static_cast<void*>(Segment));
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(i); ) {
            Polygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
            if (i == 0) {
                OutlinePolygons = Polygon;
            } else {
                OutlinePolygons->Append(Polygon);
            }
            PointCount = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, PointCount - 1); cpp_range_7.next(j); ) {
                pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                Point->X = EC_Buf::TBufEC_GetSingle(Buffer);
                Point->Y = EC_Buf::TBufEC_GetSingle(Buffer);
                pas::list_add(Polygon->Points, static_cast<void*>(Point));
            }
            Polygon->Extent.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Polygon->Extent.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            Polygon->Bounds.Left = EC_Buf::TBufEC_GetSingle(Buffer);
            Polygon->Bounds.Top = EC_Buf::TBufEC_GetSingle(Buffer);
            Polygon->Bounds.Right = EC_Buf::TBufEC_GetSingle(Buffer);
            Polygon->Bounds.Bottom = EC_Buf::TBufEC_GetSingle(Buffer);
        }
        if (GlobalsV::LoadedSaveVersion >= 45) {
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(i); ) {
                Polygon = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
                if (i == 0) {
                    HiddenOutlinePolygonsBackup = Polygon;
                } else {
                    HiddenOutlinePolygonsBackup->Append(Polygon);
                }
                PointCount = EC_Buf::TBufEC_GetWord(Buffer);
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, PointCount - 1); cpp_range_9.next(j); ) {
                    pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                    Point->X = EC_Buf::TBufEC_GetSingle(Buffer);
                    Point->Y = EC_Buf::TBufEC_GetSingle(Buffer);
                    pas::list_add(Polygon->Points, static_cast<void*>(Point));
                }
                Polygon->Extent.X = EC_Buf::TBufEC_GetSingle(Buffer);
                Polygon->Extent.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                Polygon->Bounds.Left = EC_Buf::TBufEC_GetSingle(Buffer);
                Polygon->Bounds.Top = EC_Buf::TBufEC_GetSingle(Buffer);
                Polygon->Bounds.Right = EC_Buf::TBufEC_GetSingle(Buffer);
                Polygon->Bounds.Bottom = EC_Buf::TBufEC_GetSingle(Buffer);
            }
        }
    }

    void TConstellation::ResolveLoadedReferences(TGalaxy* Galaxy) {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            pas::list_put(Stars, I, reinterpret_cast<void*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Stars, I))))));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(AdjacentConstellations) - 1); cpp_range_2.next(I); ) {
            pas::list_put(AdjacentConstellations, I, reinterpret_cast<void*>(Galaxy->IdToConstellation(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(AdjacentConstellations, I))))));
        }
    }

    void TConstellation::ClearStarLinks() {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range.next(I); ) {
            pas::dispose(pas::list_at<TConstellationStarLink>(StarLinks, I));
        }
        pas::list_clear(StarLinks);
    }

    void TConstellation::ClearBoundaryRaySamples() {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(BoundaryRaySamples) - 1); cpp_range.next(I); ) {
            pas::dispose(pas::list_at<TConstellationBoundaryRaySample>(BoundaryRaySamples, I));
        }
        pas::list_clear(BoundaryRaySamples);
    }

    // Requires a positive Count.
    void TConstellation::GenerateBoundaryRaySamples(std::int32_t Count) {
        std::int32_t I{};
        PConstellationBoundaryRaySample Sample{};
        ClearBoundaryRaySamples();
        float Step = pas::real_divide(pas::constant(2.0L * SystemImports::Pi), Count);
        float Angle = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            pas::get_mem_at(&Sample, static_cast<std::int32_t>(sizeof(TConstellationBoundaryRaySample)));
            {
                float cpp_arg = System::Cos(Angle) + MapCenter.X;
                float cpp_arg_2 = System::Sin(Angle) + MapCenter.Y;
                pas::store_unaligned<EC_Struct::TPointF>(&Sample->Position, EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
            }
            {
                float cos = System::Cos(Angle);
                float sin = System::Sin(Angle);
                pas::store_unaligned<EC_Struct::TPointF>(&Sample->Direction, EC_Struct::MakePointF(cos, sin));
            }
            Sample->Angle = Angle;
            Sample->GrowthStopped = false;
            pas::list_add(BoundaryRaySamples, static_cast<void*>(Sample));
            Angle = static_cast<long double>(Angle) + Step;
        }
    }

    // Takes ownership and frees the previous polygon chain; do not pass the current chain.
    void TConstellation::SetOutlinePolygon(aVector::TPolygon2D* Polygon) {
        if (OutlinePolygons != nullptr) {
            pas::free(OutlinePolygons);
        }
        OutlinePolygons = Polygon;
        RebuildOutlineSegments();
    }

    // Replaces the list without freeing its old segment records.
    void TConstellation::RebuildOutlineSegments() {
        if (OutlineSegments != nullptr) {
            pas::free(OutlineSegments);
        }
        OutlineSegments = aVector::TPolygon2D_ExtractBoundaryEdges(OutlinePolygons);
        RefreshOutlineBounds();
    }

    void TConstellation::ClearOutlineSegmentsAndBounds() {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            pas::dispose(pas::list_at<TMapLineSegment>(OutlineSegments, I));
        }
        pas::list_clear(OutlineSegments);
        OutlineBounds = ClassesImports::Rect(0, 0, 0, 0);
        OutlineBoundsSize = ClassesImports::Point(0, 0);
    }

    // Also sets Star.Constellation; does not remove earlier membership.
    void TConstellation::AddStar(TStar* Star) {
        pas::list_add(Stars, reinterpret_cast<void*>(Star));
        Star->Constellation = this;
    }

    // Suppresses duplicates; does not add the reciprocal relationship.
    void TConstellation::AddAdjacentConstellation(TConstellation* Constellation) {
        if (!HasAdjacentConstellation(Constellation)) {
            pas::list_add(AdjacentConstellations, reinterpret_cast<void*>(Constellation));
        }
    }

    // Self compares true; otherwise matches complete segment endpoints in either direction.
    std::uint8_t TConstellation::SharesOutlineSegment(TConstellation* Constellation) {
        std::int32_t I{};
        PMapLineSegment Segment{};
        if (this == Constellation) {
            return true;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            if (Constellation->HasOutlineSegment(Segment->StartPoint, Segment->EndPoint)) {
                return true;
            }
        }
        return false;
    }

    void TConstellation::ResetGeneratedMapShape() {
        ClearBoundaryRaySamples();
        ClearStarLinks();
        ClearOutlineSegmentsAndBounds();
        ClearStars();
        ClearAdjacentConstellations();
        pas::free(OutlinePolygons);
        OutlinePolygons = pas::construct_call<aVector::TPolygon2D>(aVector::TPolygon2D_Create);
    }

    // Clears only the borrowed list; leaves Star.Constellation unchanged.
    void TConstellation::ClearStars() {
        if (Stars != nullptr) {
            pas::list_clear(Stars);
        }
    }

    void TConstellation::ClearAdjacentConstellations() {
        if (AdjacentConstellations != nullptr) {
            pas::list_clear(AdjacentConstellations);
        }
    }

    float TConstellation::GetOutlineArea() {
        return aVector::TPolygon2D_GetChainArea(OutlinePolygons);
    }

    // Resumes equal-distance pairs using the input stars; zero clears both outputs and means no pair remains.
    std::int32_t TConstellation::FindNextClosestStarPair(TStar*& FirstStar, TStar*& SecondStar, std::int32_t MinimumDistance) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Distance{};
        TStar* A{};
        TStar* B{};
        std::int32_t BestDistance = MinimumDistance + 1;
        std::int32_t StartI = 0;
        std::int32_t StartJ = -1;
        if (FirstStar != nullptr) {
            StartI = FirstStar->ConstellationGraphIndex - 1;
        }
        if (SecondStar != nullptr) {
            StartJ = SecondStar->ConstellationGraphIndex - 1;
        }
        FirstStar = nullptr;
        SecondStar = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(StartI, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, pas::list_count(Stars) - 1); cpp_range_2.next(J); ) {
                if (I != StartI || I == StartI && J > StartJ) {
                    A = pas::list_at<TStar>(Stars, I);
                    B = pas::list_at<TStar>(Stars, J);
                    Distance = System::Round(aMyFunction::PointDistance(A->Position, B->Position));
                    if (Distance < BestDistance && Distance >= MinimumDistance) {
                        BestDistance = Distance;
                        FirstStar = A;
                        SecondStar = B;
                    }
                }
            }
        }
        if (FirstStar == nullptr) {
            BestDistance = System::Round(aConst::GalaxySizeY * System::Sqrt(2.0L));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_3.next(I); ) {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(I + 1, pas::list_count(Stars) - 1); cpp_range_4.next(J); ) {
                    A = pas::list_at<TStar>(Stars, I);
                    B = pas::list_at<TStar>(Stars, J);
                    Distance = System::Round(aMyFunction::PointDistance(A->Position, B->Position));
                    if (Distance < BestDistance && Distance > MinimumDistance) {
                        BestDistance = Distance;
                        FirstStar = A;
                        SecondStar = B;
                    }
                }
            }
        }
        if (FirstStar == nullptr) {
            return 0;
        }
        return BestDistance;
    }

    // Changes link traversal marks; returns false without checking when Stars.Count exceeds 100.
    std::uint8_t TConstellation::HasStarGraphCycle() {
        std::int32_t I{};
        PConstellationStarLink Link{};
        std::uint8_t Changed{};
        pas::Array<std::int32_t, 1, 100> Parents{};
        std::uint8_t Result = false;
        if (pas::list_count(Stars) > 100) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range.next(I); ) {
            Link = pas::list_at<TConstellationStarLink>(StarLinks, I);
            Link->TraversalMark = false;
        }
        while (true) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(Stars)); cpp_range_2.next(I); ) {
                Parents[I] = -1;
            }
            I = 0;
            Link = nullptr;
            while (I < pas::list_count(StarLinks)) {
                Link = pas::list_at<TConstellationStarLink>(StarLinks, I);
                if (!Link->TraversalMark) {
                    break;
                }
                ++I;
            }
            if (I == pas::list_count(StarLinks)) {
                break;
            }
            Parents[Link->StartStarIndex] = Link->EndStarIndex;
            Changed = true;
            while (Changed) {
                Changed = false;
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range_3.next(I); ) {
                    Link = pas::list_at<TConstellationStarLink>(StarLinks, I);
                    if (Parents[Link->StartStarIndex] != -1) {
                        Link->TraversalMark = true;
                        if (Parents[Link->EndStarIndex] == -1) {
                            Changed = true;
                            Parents[Link->EndStarIndex] = Link->StartStarIndex;
                        } else if (Parents[Link->EndStarIndex] != Link->StartStarIndex && Parents[Link->StartStarIndex] != Link->EndStarIndex) {
                            return true;
                        }
                    } else if (Parents[Link->EndStarIndex] != -1) {
                        Link->TraversalMark = true;
                        if (Parents[Link->StartStarIndex] == -1) {
                            Changed = true;
                            Parents[Link->StartStarIndex] = Link->EndStarIndex;
                        } else if (Parents[Link->StartStarIndex] != Link->EndStarIndex && Parents[Link->EndStarIndex] != Link->StartStarIndex) {
                            return true;
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Clears link traversal marks; returns false when Stars.Count exceeds 100.
    std::uint8_t TConstellation::IsStarGraphConnected() {
        std::int32_t I{};
        std::int32_t J{};
        PConstellationStarLink Link{};
        std::uint8_t Stable{};
        pas::Array<std::int32_t, 1, 100> Parents{};
        std::uint8_t Result = false;
        if (pas::list_count(Stars) > 100) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range.next(I); ) {
            Link = pas::list_at<TConstellationStarLink>(StarLinks, I);
            Link->TraversalMark = false;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(Stars)); cpp_range_2.next(I); ) {
            Parents[I] = -1;
        }
        Parents[1] = 0;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range_3.next(I); ) {
            Stable = true;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(StarLinks) - 1); cpp_range_4.next(J); ) {
                Link = pas::list_at<TConstellationStarLink>(StarLinks, J);
                if (Parents[Link->StartStarIndex] != -1 && Parents[Link->EndStarIndex] == -1) {
                    Parents[Link->EndStarIndex] = 0;
                    Stable = false;
                } else if (Parents[Link->EndStarIndex] != -1 && Parents[Link->StartStarIndex] == -1) {
                    Parents[Link->StartStarIndex] = 0;
                    Stable = false;
                }
            }
            if (Stable) {
                break;
            }
        }
        Result = true;
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, pas::list_count(Stars)); cpp_range_5.next(I); ) {
            if (Parents[I] == -1) {
                Result = false;
            }
        }
        return Result;
    }

    // Replaces StarLinks; result reports connectivity.
    std::uint8_t TConstellation::BuildStarGraph() {
        std::int32_t I{};
        TStar* Star{};
        TStar* FirstStar{};
        TStar* SecondStar{};
        PConstellationStarLink Link{};
        PMapLineSegment Segment{};
        EC_Struct::TPointF Intersection{};
        ClearStarLinks();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            Star->ConstellationGraphIndex = I + 1;
        }
        std::int32_t MinimumDistance = 0;
        FirstStar = nullptr;
        SecondStar = nullptr;
        while (true) {
            MinimumDistance = FindNextClosestStarPair(FirstStar, SecondStar, MinimumDistance);
            if (MinimumDistance == 0) {
                break;
            }
            pas::get_mem_at(&Link, static_cast<std::int32_t>(sizeof(TConstellationStarLink)));
            pas::store_unaligned<EC_Struct::TPointF>(&Link->StartPoint, FirstStar->Position);
            pas::store_unaligned<EC_Struct::TPointF>(&Link->EndPoint, SecondStar->Position);
            Link->StartStarIndex = FirstStar->ConstellationGraphIndex;
            Link->EndStarIndex = SecondStar->ConstellationGraphIndex;
            pas::list_add(StarLinks, static_cast<void*>(Link));
            if (HasStarGraphCycle()) {
                pas::list_delete(StarLinks, pas::list_count(StarLinks) - 1);
                pas::dispose(Link);
                continue;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range_2.next(I); ) {
                Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
                if (aVector::IntersectSegmentsF(Link->StartPoint, Link->EndPoint, Segment->StartPoint, Segment->EndPoint, Intersection)) {
                    pas::list_delete(StarLinks, pas::list_count(StarLinks) - 1);
                    pas::dispose(Link);
                    break;
                }
            }
        }
        return IsStarGraphConnected();
    }

    void TConstellation::ExpandOutlineBounds(EC_Struct::TPointF Point) {
        if (static_cast<long double>(OutlineBounds.Left) > Point.X) {
            OutlineBounds.Left = System::Round(Point.X);
        }
        if (static_cast<long double>(OutlineBounds.Top) > Point.Y) {
            OutlineBounds.Top = System::Round(Point.Y);
        }
        if (static_cast<long double>(OutlineBounds.Right) < Point.X) {
            OutlineBounds.Right = System::Round(Point.X);
        }
        if (static_cast<long double>(OutlineBounds.Bottom) < Point.Y) {
            OutlineBounds.Bottom = System::Round(Point.Y);
        }
    }

    void TConstellation::RefreshOutlineBounds() {
        std::int32_t I{};
        PMapLineSegment Segment{};
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&OutlineBounds, 0), ClassesImports::Point(0, 0));
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&OutlineBounds, 8), ClassesImports::Point(0, 0));
        if (pas::list_count(OutlineSegments) != 0) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, 0);
            OutlineBounds.Top = System::Round(Segment->StartPoint.Y);
            OutlineBounds.Left = System::Round(Segment->StartPoint.X);
            OutlineBounds.Bottom = System::Round(Segment->StartPoint.Y);
            OutlineBounds.Right = System::Round(Segment->StartPoint.X);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
                Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
                ExpandOutlineBounds(Segment->StartPoint);
                ExpandOutlineBounds(Segment->EndPoint);
            }
            OutlineBoundsSize = ClassesImports::Point(OutlineBounds.Right - OutlineBounds.Left, OutlineBounds.Bottom - OutlineBounds.Top);
        }
    }

    std::uint8_t TConstellation::ContainsPoint(EC_Struct::TPointF Point) {
        std::uint8_t Result = false;
        if (OutlinePolygons != nullptr) {
            return aVector::TPolygon2D_ChainContainsPoint(OutlinePolygons, Point) != false;
        }
        return Result;
    }

    std::uint8_t TConstellation::HasAdjacentConstellation(TConstellation* Constellation) {
        std::int32_t I{};
        std::uint8_t Result = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(AdjacentConstellations) - 1); cpp_range.next(I); ) {
            if (pas::list_get(AdjacentConstellations, I) == Constellation) {
                return Result;
            }
        }
        return false;
    }

    // Endpoint matching uses a tolerance and accepts either direction.
    std::uint8_t TConstellation::HasOutlineSegment(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint) {
        std::int32_t I{};
        PMapLineSegment Segment{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            if (aVector::SegmentsNearlyEqualF(FirstPoint, SecondPoint, Segment->StartPoint, Segment->EndPoint)) {
                return true;
            }
        }
        return false;
    }

    // The points may lie on different outline segments.
    std::uint8_t TConstellation::AreBothPointsOnOutline(EC_Struct::TPointF FirstPoint, EC_Struct::TPointF SecondPoint) {
        std::int32_t I{};
        PMapLineSegment Segment{};
        std::int32_t Classification{};
        std::uint8_t FoundFirst = false;
        std::uint8_t FoundSecond = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            Classification = aVector::ClassifyPointToSegment(Segment->StartPoint, Segment->EndPoint, FirstPoint);
            if (Classification >= 5) {
                FoundFirst = true;
            }
            Classification = aVector::ClassifyPointToSegment(Segment->StartPoint, Segment->EndPoint, SecondPoint);
            if (Classification >= 5) {
                FoundSecond = true;
            }
        }
        return FoundFirst && FoundSecond;
    }

    // Requires an outline yielding interior samples; the sample mean need not lie inside a concave outline.
    EC_Struct::TPointF TConstellation::CalculateLabelPosition() {
        EC_Struct::TPointF Result{};
        std::int32_t I{};
        PMapLineSegment Segment{};
        EC_Struct::TPointF MinPoint{};
        EC_Struct::TPointF MaxPoint{};
        float X{};
        MinPoint = EC_Struct::MakePointF(1.0E+20f, 1.0E+20f);
        MaxPoint = EC_Struct::MakePointF(-1.0E+20f, -1.0E+20f);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            MinPoint.X = pas::real_min<float>(MinPoint.X, Segment->StartPoint.X);
            MinPoint.Y = pas::real_min<float>(MinPoint.Y, Segment->StartPoint.Y);
            MaxPoint.X = pas::real_max<float>(MaxPoint.X, Segment->StartPoint.X);
            MaxPoint.Y = pas::real_max<float>(MaxPoint.Y, Segment->StartPoint.Y);
        }
        float StepX = pas::real_divide(static_cast<long double>(MaxPoint.X) - MinPoint.X, 1.0E+1L);
        float StepY = pas::real_divide(static_cast<long double>(MaxPoint.Y) - MinPoint.Y, 1.0E+1L);
        float SumX = 0.0f;
        float SumY = 0.0f;
        std::int32_t Count = 0;
        float Y = MinPoint.Y;
        while (Y < MaxPoint.Y) {
            X = MinPoint.X;
            while (X < MaxPoint.X) {
                if (ContainsPoint(EC_Struct::MakePointF(X, Y))) {
                    SumX = static_cast<long double>(SumX) + X;
                    SumY = static_cast<long double>(SumY) + Y;
                    ++Count;
                }
                X = static_cast<long double>(X) + StepX;
            }
            Y = static_cast<long double>(Y) + StepY;
        }
        {
            float cpp_arg = pas::real_divide(SumX, Count);
            float cpp_arg_2 = pas::real_divide(SumY, Count);
            Result = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
        }
        return Result;
    }

    std::uint8_t TConstellation::HasOutlineVertex(EC_Struct::TPointF Point) {
        std::int32_t I{};
        PMapLineSegment Segment{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            if (aVector::PointsNearlyEqualF(Segment->StartPoint, Point) || aVector::PointsNearlyEqualF(Segment->EndPoint, Point)) {
                return true;
            }
        }
        return false;
    }

    // Tests a distance of at most two map units.
    std::uint8_t TConstellation::IsPointNearOutline(EC_Struct::TPointF Point) {
        std::int32_t I{};
        PMapLineSegment Segment{};
        float Distance{};
        float Minimum = aConst::GalaxySizeX;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 1); cpp_range.next(I); ) {
            Segment = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            Distance = aVector::PointSegmentDistanceF(Segment->StartPoint, Segment->EndPoint, Point);
            if (Distance < Minimum) {
                Minimum = Distance;
            }
        }
        return Minimum <= 2.0L;
    }

    void TConstellation::NormalizeOutlineSegmentOrder() {
        std::int32_t I{};
        std::int32_t J{};
        PMapLineSegment First{};
        PMapLineSegment Next{};
        void* Temp{};
        EC_Struct::TPointF Point{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(OutlineSegments) - 2); cpp_range.next(I); ) {
            First = pas::list_at<TMapLineSegment>(OutlineSegments, I);
            J = I + 1;
            while (J < pas::list_count(OutlineSegments)) {
                Next = pas::list_at<TMapLineSegment>(OutlineSegments, J);
                if (aVector::PointsNearlyEqualF(First->EndPoint, Next->StartPoint)) {
                    break;
                }
                if (aVector::PointsNearlyEqualF(First->EndPoint, Next->EndPoint)) {
                    Point = Next->StartPoint;
                    pas::store_unaligned<EC_Struct::TPointF>(&Next->StartPoint, Next->EndPoint);
                    pas::store_unaligned<EC_Struct::TPointF>(&Next->EndPoint, Point);
                    break;
                }
                ++J;
            }
            if (J < pas::list_count(OutlineSegments)) {
                Temp = pas::list_get(OutlineSegments, I + 1);
                pas::list_put(OutlineSegments, I + 1, pas::list_get(OutlineSegments, J));
                pas::list_put(OutlineSegments, J, Temp);
            }
        }
    }

    // Localization key uses the current position in Galaxy.Constellations, not Id.
    pas::WideString TConstellation::GetName() {
        std::int32_t Index{};
        Index = pas::list_indexof(Galaxy->Constellations, reinterpret_cast<void*>(this));
        return aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Constellations.Name.", SysUtils::IntToStr(Index + 1)})));
    }

    // Uses member stars' cached population counts.
    std::uint8_t TConstellation::HasDominatorPresence() {
        std::int32_t I{};
        TStar* Star{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            if (Star->ShipTypeCounts[aGalaxyStruct::stKling] > 0) {
                return true;
            }
        }
        return false;
    }

    std::uint8_t TConstellation::HasPirateClanPresence() {
        std::int32_t I{};
        std::int32_t J{};
        TStar* Star{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship); pirate != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate && pirate->PirateType != 0) {
                    return true;
                }
            }
        }
        return false;
    }

    std::uint8_t TConstellation::HasBertorOfSeries(aGalaxyStruct::TDominatorSeries Series) {
        std::int32_t I{};
        std::int32_t J{};
        TStar* Star{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship); kling != nullptr && kling->DominatorSeries == Series && kling->KlingType == aGalaxyStruct::ktBertor) {
                    return true;
                }
            }
        }
        return false;
    }

    // Uses cached population counts.
    std::int32_t TConstellation::CountShipsByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask) {
        aGalaxyStruct::TShipType I{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range.next(I); ) {
            if (pas::contains(ShipTypeMask, I)) {
                Count += ShipTypeCounts[I];
            }
        }
        return Count;
    }

    std::int32_t TGalaxy::CountEligibleRangers() {
        std::int32_t Index{};
        aRanger::TRanger* Ranger{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range.next(Index); ) {
            Ranger = pas::list_at<aRanger::TRanger>(Rangers, Index);
            if (!Ranger->ExcludedFromRating) {
                ++Result;
            }
        }
        return Result;
    }

    void TGalaxy::RefreshRangerWealthStats() {
        std::int32_t I{};
        std::int32_t J{};
        aRanger::TRanger* Ranger{};
        aShip::TShip* Ship{};
        TStar* Star{};
        if (SpecialSimulationMode != 0) {
            AverageRangerCapital = 100000000;
            return;
        }
        WealthiestRanger = nullptr;
        MaxRangerWealth = 0;
        if (pas::list_count(Rangers) == 0) {
            return;
        }
        std::int64_t Total = 0;
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(Rangers, I);
            if (Ranger->ExcludedFromRating) {
                continue;
            }
            Total += Ranger->CalculateWealth();
            ++Count;
            if (MaxRangerWealth < Ranger->Wealth || WealthiestRanger == nullptr) {
                MaxRangerWealth = Ranger->Wealth;
                WealthiestRanger = Ranger;
            }
        }
        if (CoalitionDefeatedTurn != 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_2.next(I); ) {
                Star = pas::list_at<TStar>(Stars, I);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship); !(pirate != nullptr && pirate->PirateType == 0 && Ship->OwnerId == aGalaxyStruct::oiPirate)) {
                        continue;
                    }
                    Total += Ship->CalculateWealth();
                    ++Count;
                }
            }
        }
        Total = System::Round(pas::real_divide(Total, Count));
        if (Total > SystemImports::MaxInt) {
            AverageRangerCapital = SystemImports::MaxInt;
        } else {
            AverageRangerCapital = Total;
        }
    }

    void TGalaxy::RefreshRangerStrengthStats() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count{};
        aRanger::TRanger* Ranger{};
        float Total{};
        aShip::TShip* Ship{};
        TStar* Star{};
        BestRangerStrength = 0.0f;
        StrongestRanger = nullptr;
        if (pas::list_count(Rangers) != 0) {
            Total = 0.0f;
            Count = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range.next(I); ) {
                Ranger = pas::list_at<aRanger::TRanger>(Rangers, I);
                if (!Ranger->ExcludedFromRating) {
                    Total = static_cast<long double>(Total) + Ranger->Strength;
                    if (BestRangerStrength < Ranger->Strength || StrongestRanger == nullptr) {
                        BestRangerStrength = Ranger->Strength;
                        StrongestRanger = Ranger;
                    }
                    ++Count;
                }
            }
            if (CoalitionDefeatedTurn != 0) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_2.next(I); ) {
                    Star = pas::list_at<TStar>(Stars, I);
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                        if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship); pirate != nullptr && pirate->PirateType == 0 && Ship->OwnerId == aGalaxyStruct::oiPirate) {
                            Total = static_cast<long double>(Total) + Ship->Strength;
                            if (BestRangerStrength < Ship->Strength) {
                                BestRangerStrength = Ship->Strength;
                            }
                            ++Count;
                        }
                    }
                }
            }
            AverageRangerStrength = pas::real_divide(Total, Count);
        }
    }

    // Assigns one-based positions by descending TotalExperience, including excluded rangers; leaves the Rangers list order unchanged.
    void TGalaxy::RefreshRangerRatingPlaces() {
        std::int32_t I{};
        std::int32_t J{};
        aRanger::TRanger* First{};
        aRanger::TRanger* Second{};
        pas::DynArray<std::uint32_t> Sorted{};
        // Native RTTI: dynamic Cardinal array storing object addresses.
        std::uint32_t SwapFirst{};
        std::uint32_t SwapSecond{};
        Sorted.set_length(pas::list_count(Rangers));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range.next(I); ) {
            First = pas::list_at<aRanger::TRanger>(Rangers, I);
            Sorted[I] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(First));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range_2.next(I); ) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(I, pas::list_count(Rangers) - 1); cpp_range_3.next(J); ) {
                First = reinterpret_cast<aRanger::TRanger*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sorted[I])));
                Second = reinterpret_cast<aRanger::TRanger*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sorted[J])));
                if (First->TotalExperience < Second->TotalExperience) {
                    SwapFirst = Sorted[I];
                    SwapSecond = Sorted[J];
                    Sorted[J] = SwapFirst;
                    Sorted[I] = SwapSecond;
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range_4.next(I); ) {
            First = reinterpret_cast<aRanger::TRanger*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sorted[I])));
            First->PlaceInRating = I + 1;
        }
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(Destroying ^ 1)) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckFirstPlaceRatingAchievement();
        }
    }

    // Ignores ExcludedFromRating rangers; returns nil if none has positive strength.
    void* TGalaxy::FindStrongestRanger() {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        float Best = 0.0f;
        void* Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(Rangers, I);
            if (static_cast<std::uint8_t>(Ranger->ExcludedFromRating ^ 1) && Ranger->Strength > Best) {
                Result = Ranger;
                Best = Ranger->Strength;
            }
        }
        return Result;
    }

    // Ignores ExcludedFromRating rangers; returns nil if none has positive wealth.
    void* TGalaxy::FindWealthiestRanger() {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        float Best = 0.0f;
        void* Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Rangers) - 1); cpp_range.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(Rangers, I);
            if (static_cast<std::uint8_t>(Ranger->ExcludedFromRating ^ 1) && static_cast<long double>(Ranger->Wealth) > Best) {
                Result = Ranger;
                Best = Ranger->Wealth;
            }
        }
        return Result;
    }

    // Excludes stars with a custom faction.
    std::int32_t TGalaxy::CountFactionStars(aGalaxyStruct::TStarFaction Faction) {
        std::int32_t Index{};
        TStar* Star{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(Index); ) {
            Star = pas::list_at<TStar>(Stars, Index);
            if (Star->Status.CustomFaction == u"" && Star->Status.ControlFaction == Faction) {
                ++Result;
            }
        }
        return Result;
    }

    aGalaxyStruct::TPercent TGalaxy::GetFactionControlPercent(aGalaxyStruct::TStarFaction Faction) {
        return System::Round(pas::real_divide(CountFactionStars(Faction), pas::list_count(Stars)) * 1.0E+2L);
    }

    // Active Galaxy only. Fraction of Dominator systems in Series, multiplied by the number of unresolved series; not a percentage.
    float TGalaxy::GetDominatorSeriesControlShare(aGalaxyStruct::TDominatorSeries Series) {
        std::int32_t I{};
        TStar* Star{};
        std::int32_t DominatorStars = 0;
        std::int32_t SeriesStars = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators && Star->Status.CustomFaction == u"") {
                ++DominatorStars;
                if (Star->Status.DominatorSeries == Series) {
                    ++SeriesStars;
                }
            }
        }
        if (DominatorStars == 0) {
            DominatorStars = 1;
        }
        std::int32_t Unresolved = 0;
        if (Galaxy->KellerSeriesResolvedTurn == 0) {
            ++Unresolved;
        }
        if (Galaxy->TerronSeriesResolvedTurn == 0) {
            ++Unresolved;
        }
        if (Galaxy->BlazerSeriesResolvedTurn == 0) {
            ++Unresolved;
        }
        return pas::real_divide(Unresolved * SeriesStars, DominatorStars);
    }

    std::int32_t TGalaxy::CountStarsInBattle() {
        std::int32_t I{};
        TStar* Star{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            if (Star->Status.Battle != 0) {
                ++Result;
            }
        }
        return Result;
    }

    // Uses quest target-owner filters; leaves planets with an existing quest unchanged.
    void TGalaxy::AssignTextQuestsToPlanets() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Index{};
        std::int32_t QuestId{};
        aPlanet::TPlanet* Planet{};
        TextQuest::TTextQuest* Quest{};
        EC_CacheBuf::TCBufControlEC* Control{};
        EC_CacheBuf::TCBufEC* Buffer{};
        std::int32_t Count = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.PlanetQuest"_wref.get())->GetParamCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            if (EC_Str::IsIntegerTextW(pas::view(([&] {
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.PlanetQuest"_wref.get());
                std::int32_t i = I;
                return blockByPath->GetParamName(i);
            }())))) {
                QuestId = SysUtils::StrToInt(static_cast<pas::AnsiString>(([&] {
                    EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::LanguageDataConfig->GetBlockByPath(u"PlanetQuest.PlanetQuest"_wref.get());
                    std::int32_t i_2 = I;
                    return blockByPath_2->GetParamName(i_2);
                }())));
                Quest = pas::construct_call<TextQuest::TTextQuest>(TextQuest::TTextQuest_Create);
                Control = nullptr;
                {
                    std::exception_ptr cpp_error{};
                    try {
                        Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                        EC_Cache::TCacheEC::ResetControl(Control);
                        Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"PlanetQuest.", SysUtils::IntToStr(QuestId)})));
                        Buffer = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                        Quest->LoadFromReader(Buffer->Buffer, true);
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
                Index = aMyFunction::NextRandomIntRange(0, pas::list_count(Planets) - 1, RandomState);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_2.next(J); ) {
                    aMyFunction::IncrementWrapped(Index, 0, pas::list_count(Planets) - 1);
                    Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                    if (Planet->CurrentStar->Constellation->Id != 20 && Planet->TextQuestId <= -1 && Planet->Graphic->QuestEnabled && (Planet->OwnerId != aGalaxyStruct::oiDominator || aMyFunction::PointDistance(Planet->CurrentStar->Position, aPlayer::GetPlayer()->CurrentStar->Position) <= 8.0E+1L) && (Planet->LandTiles >= Planet->GetTotalSurfaceTileCount() * 0.2L || Planet->LandTiles >= Planet->GetTotalSurfaceTileCount() * 0.1L && J >= pas::list_count(Planets) * 0.7L) && (Planet->OwnerId == aGalaxyStruct::oiUninhabited && pas::contains(Quest->TargetRaces, TextQuest::qrUninhabited) || pas::contains(Quest->TargetRaces, TextQuest::qrMaloc) && Planet->RaceId == aGalaxyStruct::oiMaloc && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->TargetRaces, TextQuest::qrPeleng) && Planet->RaceId == aGalaxyStruct::oiPeleng && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->TargetRaces, TextQuest::qrHuman) && Planet->RaceId == aGalaxyStruct::oiHuman && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->TargetRaces, TextQuest::qrFeyan) && Planet->RaceId == aGalaxyStruct::oiFeyan && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->TargetRaces, TextQuest::qrGaal) && Planet->RaceId == aGalaxyStruct::oiGaal && Planet->OwnerId != aGalaxyStruct::oiUninhabited || Quest->TargetRaces == pas::constant_set<TextQuest::TQuestRaceSet>({}) && (pas::contains(Quest->IssuerRaces, TextQuest::qrMaloc) && Planet->RaceId == aGalaxyStruct::oiMaloc && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->IssuerRaces, TextQuest::qrPeleng) && Planet->RaceId == aGalaxyStruct::oiPeleng && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->IssuerRaces, TextQuest::qrHuman) && Planet->RaceId == aGalaxyStruct::oiHuman && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->IssuerRaces, TextQuest::qrFeyan) && Planet->RaceId == aGalaxyStruct::oiFeyan && Planet->OwnerId != aGalaxyStruct::oiUninhabited || pas::contains(Quest->IssuerRaces, TextQuest::qrGaal) && Planet->RaceId == aGalaxyStruct::oiGaal && Planet->OwnerId != aGalaxyStruct::oiUninhabited))) {
                        Planet->TextQuestId = QuestId;
                        break;
                    }
                }
                pas::free(Quest);
            }
        }
    }

    std::uint8_t TGalaxy::HasPlayerQuestHistory(aGalaxyStruct::TQuestType QuestType, std::uint16_t QuestNumber) {
        std::int32_t I{};
        aRanger::PPlayerOldQuest Quest{};
        {
            const std::int32_t cpp_first = pas::list_count(aRanger::PlayerOldQuests) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Quest = pas::list_at<aRanger::TPlayerOldQuest>(aRanger::PlayerOldQuests, I);
                    if (QuestType == Quest->QuestType && QuestNumber == Quest->QuestNumber) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // Delphi TDateTime; -1 selects CurrentTurn.
    double TGalaxy::TurnToDateTime(std::int32_t Turn) {
        if (Turn == -1) {
            Turn = CurrentTurn;
        }
        return Turn + 511341.5L - pas::constant(static_cast<long double>(aGalaxyStruct::GalaxyWarmupTurns));
    }

    // -1 selects CurrentTurn.
    pas::WideString TGalaxy::FormatTurnDate(std::int32_t Turn) {
        pas::WideString MonthNumber{};
        pas::WideString MonthName{};
        MonthNumber = static_cast<pas::WideString>(SysUtilsImports::FormatDateTime("mm"_a, TurnToDateTime(Turn)));
        MonthName = aConst::LocalizedText(pas::concat_wide({u"Month.", MonthNumber}));
        return pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({SysUtilsImports::FormatDateTime("d"_a, TurnToDateTime(Turn)), " "})), MonthName, u" ", static_cast<pas::WideString>(SysUtilsImports::FormatDateTime("yyyy"_a, TurnToDateTime(Turn)))});
    }

    // Adds a player bubble only after turn 300; news insertion still uses duplicate-text suppression.
    void TGalaxy::AddPlanetNewsWithPlayerBubble(aGalaxyStruct::TGalaxyNewsKind NewsType, pas::WideString Text) {
        if (CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns) {
            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, Text, u""_wref.get());
        }
        AddPlanetNews(NewsType, Text);
    }

    // Rejects empty text; identical existing text suppresses insertion regardless of NewsType.
    void TGalaxy::AddPlanetNews(aGalaxyStruct::TGalaxyNewsKind NewsType, pas::WideString Text) {
        PPlanetNewsEntry Entry{};
        std::int32_t Index{};
        if (Text == u"") {
            pas::raise(pas::make_exception<pas::Exception>("Error! \317\356\353\363\367\345\355\340 \357\363\361\362\340\377 \357\353\340\355\345\362\340\360\355\340\377 \355\356\342\356\361\362\374"_a));
        }
        if (PlanetNews != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlanetNews) - 1); cpp_range.next(Index); ) {
                Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(PlanetNews, Index);
                if (Entry->Text == Text) {
                    return;
                }
            }
        }
        ++NextPlanetNewsId;
        pas::new_value(Entry);
        Entry->Id = NextPlanetNewsId;
        Entry->Turn = CurrentTurn;
        Entry->NewsType = NewsType;
        Entry->Text = std::move(Text);
        pas::list_add(PlanetNews, static_cast<void*>(Entry));
    }

    std::int32_t TGalaxy::CountPlanetNewsByType(aGalaxyStruct::TGalaxyNewsKind NewsType) {
        PPlanetNewsEntry Entry{};
        std::int32_t Index{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PlanetNews) - 1); cpp_range.next(Index); ) {
            Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(PlanetNews, Index);
            if (Entry->NewsType == NewsType) {
                ++Result;
            }
        }
        return Result;
    }

    // Removes entries more than 30 days old.
    void TGalaxy::PrunePlanetNews() {
        std::int32_t I{};
        PPlanetNewsEntry News{};
        {
            const std::int32_t cpp_first = pas::list_count(PlanetNews) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    News = pas::list_at<aGalaxyStruct::TPlanetNews>(PlanetNews, I);
                    if (News->Turn < CurrentTurn - 30) {
                        pas::list_delete(PlanetNews, I);
                        pas::dispose(News);
                    }
                }
            }
        }
    }

    // Replaces the global spawn-planet pointer without freeing its previous value; does not register the proxy in star or galaxy planet lists.
    void TGalaxy::CreateDominatorSpawnProxy(TStar* Star) {
        aKling::DominatorSpawnPlanet = pas::construct_call<aPlanet::TPlanet>(aPlanet::TPlanet_Create);
        aKling::DominatorSpawnPlanet->InitDominatorSpawnProxy(pas::checked_cast<TStar*>(static_cast<pas::Object*>(Star)));
    }

    void TGalaxy::UpdateConstellationMilitaryStats() {
        std::int32_t I{};
        std::int32_t J{};
        TConstellation* Constellation{};
        TStar* Star{};
        aGalaxyStruct::TShipType Kind{};
        // The native cache indexes all fourteen ship types at $84..$B8.
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range.next(Kind); ) {
            ShipTypeCounts[Kind] = 0;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range_2.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Constellations, I);
            for (auto cpp_range_3 = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range_3.next(Kind); ) {
                Constellation->ShipTypeCounts[Kind] = 0;
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range_4.next(J); ) {
                Star = pas::list_at<TStar>(Constellation->Stars, J);
                Star->RefreshShipTypeCounts();
                for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range_5.next(Kind); ) {
                    Constellation->ShipTypeCounts[Kind] += Star->ShipTypeCounts[Kind];
                    ShipTypeCounts[Kind] += Star->ShipTypeCounts[Kind];
                }
            }
        }
    }

    // Updates TechLevel but normally returns zero. The special mode returns 8 without updating it.
    std::uint8_t TGalaxy::RefreshTechLevel() {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        if (SpecialSimulationMode != 0) {
            return 8;
        }
        std::uint8_t Result = 0;
        std::int32_t HighestCount = 0;
        std::int32_t PreviousCount = 0;
        std::int32_t HighestLevel = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            if (Planet->IsCoalitionOwned || Planet->OwnerId == aGalaxyStruct::oiPirate) {
                if (Planet->InventionLevels[aGalaxyStruct::piMainTech] > HighestLevel) {
                    if (Planet->InventionLevels[aGalaxyStruct::piMainTech] == HighestLevel + 1) {
                        PreviousCount = HighestCount;
                    } else {
                        PreviousCount = 0;
                    }
                    HighestCount = 1;
                    HighestLevel = Planet->InventionLevels[aGalaxyStruct::piMainTech];
                } else if (Planet->InventionLevels[aGalaxyStruct::piMainTech] == HighestLevel) {
                    ++HighestCount;
                } else if (Planet->InventionLevels[aGalaxyStruct::piMainTech] == HighestLevel - 1) {
                    ++PreviousCount;
                }
            }
        }
        if (HighestCount >= 5) {
            TechLevel = std::max<std::int32_t>(1, HighestLevel);
        } else if (HighestCount >= 2 || HighestCount + PreviousCount >= 5) {
            TechLevel = std::max<std::int32_t>(1, HighestLevel - 1);
        } else {
            TechLevel = std::max<std::int32_t>(1, HighestLevel - 2);
        }
        return Result;
    }

    // Cancels normal-space Dominator and hostile-pirate jumps from other stars, plus assigned Dominion relocations.
    void TGalaxy::CancelEnemyJumpsToStar(TStar* Star) {
        std::int32_t I{};
        std::int32_t J{};
        TStar* Other{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range.next(I); ) {
            Other = pas::list_at<TStar>(Stars, I);
            if (Other != Star) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Other->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Other->Ships, J);
                    if (Ship->InNormalSpace()) {
                        if ((pas::class_cast_if<aKling::TKling*>(Ship) != nullptr || Ship->CurrentStanding == aGalaxyStruct::ssPirateMilitary) && Ship->Order == aShip::soJump && Ship->OrderTarget == Star) {
                            Ship->OrderNone(false);
                        }
                        if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Ship); ruins != nullptr && Ship->TypeId == aGalaxyStruct::rstDominion && ruins->FlyToStar == Star) {
                            ruins->FlyToStar = nullptr;
                            ruins->FlyDate = 0;
                            Ship->OrderNone(false);
                        }
                    }
                }
            }
        }
    }

    // Nil Origin omits the origin-distance penalty. Searches active Galaxy; advances Self.RandomState.
    TStar* TGalaxy::SelectStarForLiberationAttack(TStar* Origin, aGalaxyStruct::TStarFaction FriendlyFaction) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Weight{};
        TStar* Star{};
        TStar* Other{};
        std::int32_t BestWeight = SystemImports::MaxInt;
        TStar* Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            if (Star->Status.ControlFaction != FriendlyFaction && Star->Status.Battle == 0 && Star->Constellation->Id != 20) {
                Weight = 0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(Galaxy->Stars) - 1); cpp_range_2.next(J); ) {
                    Other = pas::checked_cast<TStar*>(static_cast<pas::Object*>(Star->StarDistances[J].Star));
                    if (Other->Status.ControlFaction == FriendlyFaction && Other->Status.CustomFaction == u"") {
                        Weight += Star->StarDistances[J].Distance;
                    }
                }
                if (Origin != nullptr) {
                    Weight = System::Round(aMyFunction::RemapClamped(aMyFunction::PointDistance(Origin->Position, Star->Position), 1.0E+1, 1.0E+2, Weight, 100 * Weight));
                }
                Weight = System::Round(aMyFunction::NextRandomFloatRange(Weight, 2 * Weight, RandomState));
                if (Weight < BestWeight) {
                    BestWeight = Weight;
                    Result = Star;
                }
            }
        }
        return Result;
    }

    void TGalaxy::ComputeGlobalGoodsPriceBands() {
        std::uint8_t Good{};
        float PriceSpread{};
        for (Good = 0; Good <= 7; ++Good) {
            aConst::GoodsMarket[Good].MinPrice = ScaleGoodsPriceByGalaxyAge(aConst::GoodsMarketBase[Good].MinPrice);
            aConst::GoodsMarket[Good].AveragePrice = ScaleGoodsPriceByGalaxyAge(aConst::GoodsMarketBase[Good].AveragePrice);
            aConst::GoodsMarket[Good].MaxPrice = ScaleGoodsPriceByGalaxyAge(aConst::GoodsMarketBase[Good].MaxPrice);
            aConst::GoodsMarket[Good].BaseStock = ScaleGoodsStockByGalaxyAge(aConst::GoodsMarketBase[Good].BaseStock);
            PriceSpread = aConst::GoodsMarket[Good].AveragePrice - aConst::GoodsMarket[Good].MinPrice;
            aConst::GoodsMarket[Good].MinPrice += System::Round(static_cast<long double>(PriceSpread) * aConst::GalaxyDifficultyTuning[DifficultyLevels[1]].MarketPriceBandSqueeze);
            if (aConst::GoodsMarket[Good].MinPrice >= aConst::GoodsMarket[Good].AveragePrice - 1) {
                aConst::GoodsMarket[Good].MinPrice = aConst::GoodsMarket[Good].AveragePrice - 2;
            }
            aConst::GoodsMarket[Good].MaxPrice -= System::Round(static_cast<long double>(PriceSpread) * aConst::GalaxyDifficultyTuning[DifficultyLevels[1]].MarketPriceBandSqueeze);
            if (aConst::GoodsMarket[Good].MaxPrice <= aConst::GoodsMarket[Good].AveragePrice + 1) {
                aConst::GoodsMarket[Good].MaxPrice = aConst::GoodsMarket[Good].AveragePrice + 2;
            }
        }
    }

    // Maps the global minimum/maximum price band to 0..100 with clamping.
    aGalaxyStruct::TPercent TGalaxy::GetGoodsPricePercent(std::uint8_t GoodsType, std::int32_t Price) {
        return System::Round(aMyFunction::RemapClamped(Price, aConst::GoodsMarket[GoodsType].MinPrice, aConst::GoodsMarket[GoodsType].MaxPrice, 0.0, 1.0E+2));
    }

    std::int32_t TGalaxy::ScaleGoodsPriceByGalaxyAge(std::int32_t BaseValue) {
        return System::Round(aMyFunction::RemapClamped(CurrentTurn, aConst::GoodsInflationStartTurn, aConst::GoodsInflationEndTurn, static_cast<long double>(BaseValue) * aConst::GoodsInflationMin, static_cast<long double>(BaseValue) * aConst::GoodsInflationMax));
    }

    std::int32_t TGalaxy::ScaleGoodsStockByGalaxyAge(std::int32_t BaseValue) {
        return System::Round(aMyFunction::RemapClamped(CurrentTurn, aConst::GoodsInflationStartTurn, aConst::GoodsInflationEndTurn, static_cast<long double>(BaseValue) * aConst::GoodsStockMin, static_cast<long double>(BaseValue) * aConst::GoodsStockMax));
    }

    // Clamps TechLevel to 2..7, linearly interpolates the endpoints, then rounds.
    std::int32_t TGalaxy::ScaleIntByTechLevel(std::int32_t AtLevelTwo, std::int32_t AtLevelSeven) {
        return System::Round(aMyFunction::RemapClamped(TechLevel, 2.0, 7.0, AtLevelTwo, AtLevelSeven));
    }

    float TGalaxy::InterpolateSingleByTechLevel(float AtLevelTwo, float AtLevelSeven) {
        return aMyFunction::RemapClamped(TechLevel, 2.0, 7.0, AtLevelTwo, AtLevelSeven);
    }

    // Borrowed template. Uses the closest eligible technology when the interval has no match; falls back to the first built-in template when no availability matches.
    aConst::PWeaponInfo TGalaxy::SelectWeaponInfo(std::uint32_t Seed, aGalaxyStruct::TWeaponAvailabilityMask AvailabilityMask, std::uint8_t MaximumTechLevel, std::uint8_t MinimumTechLevel) {
        aConst::PWeaponInfo Result{};
        std::int32_t I{};
        aConst::PWeaponInfo Info{};
        std::int32_t Distance{};
        // Nested helper with caller-popped static link. Minimum/maximum bytes are at ParentFrame+8/+12.
        auto TechDistance = [&](std::int32_t TechLevel) -> std::int32_t {
            std::int32_t Result = 0;
            if (TechLevel > MaximumTechLevel) {
                Result = TechLevel - MaximumTechLevel;
            }
            if (TechLevel < MinimumTechLevel) {
                return std::max<std::int32_t>(Result, MinimumTechLevel - TechLevel);
            }
            return Result;
        };
        pas::List* Candidates = pas::make_object<pas::List>();
        std::int32_t NearestDistance = 0;
        aConst::PWeaponInfo Nearest = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}))); cpp_range.next(I); ) {
            Info = &aConst::WeaponInfos[static_cast<aConst::TItemType>(aConst::GetItemTypeFromMask(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}), I))];
            if (pas::contains(AvailabilityMask, Info->Availability)) {
                Distance = TechDistance(Info->TechLevel);
                if (Distance > 0) {
                    if (pas::list_count(Candidates) <= 0 && (Nearest == nullptr || Distance < NearestDistance)) {
                        Nearest = Info;
                        NearestDistance = Distance;
                    }
                } else {
                    pas::list_add(Candidates, static_cast<void*>(Info));
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CustomWeaponTypes) - 1); cpp_range_2.next(I); ) {
            Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, I);
            if (pas::contains(AvailabilityMask, Info->Availability)) {
                Distance = TechDistance(Info->TechLevel);
                if (Distance > 0) {
                    if (pas::list_count(Candidates) <= 0 && (Nearest == nullptr || Distance < NearestDistance)) {
                        Nearest = Info;
                        NearestDistance = Distance;
                    }
                } else {
                    pas::list_add(Candidates, pas::list_get(CustomWeaponTypes, I));
                }
            }
        }
        if (pas::list_count(Candidates) > 0) {
            Result = pas::list_at<aConst::TWeaponInfo>(Candidates, aMyFunction::SeededRandomIntRange(0, pas::list_count(Candidates) - 1, Seed));
        } else {
            Result = Nearest;
        }
        pas::free(Candidates);
        if (Result == nullptr) {
            return aConst::WeaponInfos.elements;
        }
        return Result;
    }

    // Zero-based index; Context may be a planet or ship, or nil. Relaxes the priority interval after repeated misses. No termination guarantee when every template fails the context filter.
    std::int32_t TGalaxy::SelectMicroModule(std::uint8_t MinimumPriority, std::uint8_t MaximumPriority, std::uint32_t Seed, pas::Object* Context) {
        std::int32_t Result{};
        std::int32_t ModuleIndex{};
        std::int32_t Attempts = 0;
        std::int32_t BestIndex = -1;
        std::int32_t BestPriority = 100;
        do {
            ++Attempts;
            ModuleIndex = aMyFunction::NextRandomIntRange(0, aConst::MicroModuleTemplateCount - 1, Seed);
            if (aConst::MicroModuleTemplates[ModuleIndex].SpecialOnly) {
                continue;
            }
            if (Context != nullptr) {
                if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Context); kling != nullptr && static_cast<std::uint8_t>(static_cast<aShip::TShip*>(kling)->HasScriptStateText() ^ 1)) {
                    if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask, kling->DominatorSeries)) {
                        continue;
                    }
                    if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask, aGalaxyStruct::oiDominator)) {
                        continue;
                    }
                    if (static_cast<std::uint8_t>(aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction ^ 1) && aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask != pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiDominator}}) && aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiDominator}, {aGalaxyStruct::oiPirate}}) != pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiDominator}, {aGalaxyStruct::oiPirate}})) {
                        continue;
                    }
                }
                if (aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction) {
                    if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(Context)) {
                        if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask, planet->OwnerId)) {
                            continue;
                        }
                    } else if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Context)) {
                        if (pas::contains(aConst::FactionStandingMasks[ruins->CurrentStar->Status.ControlFaction], ruins->CurrentStanding)) {
                            if (aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * pas::load_unaligned<aGalaxyStruct::TOwnerMask>(pas::byte_offset(&aConst::PlanetOwnerMasks, ruins->CurrentStar->Status.ControlFaction * sizeof(aGalaxyStruct::TOwnerMask))) == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                                continue;
                            }
                        } else {
                            if (pas::in_range(ruins->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssCoalitionMilitary), static_cast<std::int32_t>(aGalaxyStruct::ssNeutral))) {
                                if (aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * aConst::PlanetOwnerMasks.Coalition == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                                    continue;
                                }
                            }
                            if (pas::in_range(ruins->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPiratePassive), static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary))) {
                                if (aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * aConst::PlanetOwnerMasks.PirateClan == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                                    continue;
                                }
                            }
                        }
                    } else if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(Context)) {
                        if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask, aConst::RaceToOwner(normalShip->PilotRace))) {
                            continue;
                        }
                    }
                }
            }
            if (Context == nullptr && aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction) {
                continue;
            }
            if (pas::contains(pas::make_set<pas::Set<0, 255>>({{static_cast<std::int32_t>(MinimumPriority), static_cast<std::int32_t>(MaximumPriority)}}), aConst::MicroModuleTemplates[ModuleIndex].Priority)) {
                return ModuleIndex;
            }
            if (Attempts > 1000) {
                if (BestIndex >= 0) {
                    return BestIndex;
                }
                return ModuleIndex;
            }
            if (aConst::MicroModuleTemplates[ModuleIndex].Priority > MaximumPriority && (BestIndex < 0 || aConst::MicroModuleTemplates[ModuleIndex].Priority < BestPriority)) {
                BestIndex = ModuleIndex;
                BestPriority = aConst::MicroModuleTemplates[ModuleIndex].Priority;
            }
            if (Attempts % 99 == 0) {
                MinimumPriority = std::max<std::int32_t>(0, MinimumPriority - 10);
                MaximumPriority = std::min<std::int32_t>(100, MaximumPriority + 10);
                if (BestIndex >= 0 && MaximumPriority >= BestPriority) {
                    return BestIndex;
                }
            }
        } while (!false);
        return Result;
    }

    // Zero-based index. The final attempt-limit fallback can return an incompatible module; callers must check CanInstallMicroModule. Context rejection can bypass the attempt-limit check.
    std::int32_t TGalaxy::SelectMicroModuleForEquipment(std::uint8_t MinimumPriority, std::uint8_t MaximumPriority, std::uint32_t Seed, pas::Object* Context, void* Item) {
        std::int32_t Result{};
        std::int32_t ModuleIndex{};
        std::int32_t Attempts = 0;
        aItem::TEquipment* Equipment = static_cast<aItem::TEquipment*>(Item);
        std::int32_t BestIndex = -1;
        std::int32_t BestPriority = 100;
        do {
            ++Attempts;
            ModuleIndex = aMyFunction::NextRandomIntRange(0, aConst::MicroModuleTemplateCount - 1, Seed);
            if (aConst::MicroModuleTemplates[ModuleIndex].SpecialOnly) {
                continue;
            }
            if (Context != nullptr) {
                if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Context); kling != nullptr && static_cast<std::uint8_t>(static_cast<aShip::TShip*>(kling)->HasScriptStateText() ^ 1)) {
                    if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedDominatorSeriesMask, kling->DominatorSeries)) {
                        continue;
                    }
                    if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask, aGalaxyStruct::oiDominator)) {
                        continue;
                    }
                    if (static_cast<std::uint8_t>(aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction ^ 1) && aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask != pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiDominator}}) && aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiDominator}, {aGalaxyStruct::oiPirate}}) != pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiDominator}, {aGalaxyStruct::oiPirate}})) {
                        continue;
                    }
                }
                if (aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction) {
                    if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(Context)) {
                        if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask, planet->OwnerId)) {
                            continue;
                        }
                    } else if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Context)) {
                        if (pas::contains(aConst::FactionStandingMasks[ruins->CurrentStar->Status.ControlFaction], ruins->CurrentStanding)) {
                            if (aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * pas::load_unaligned<aGalaxyStruct::TOwnerMask>(pas::byte_offset(&aConst::PlanetOwnerMasks, ruins->CurrentStar->Status.ControlFaction * sizeof(aGalaxyStruct::TOwnerMask))) == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                                continue;
                            }
                        } else {
                            if (pas::in_range(ruins->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssCoalitionMilitary), static_cast<std::int32_t>(aGalaxyStruct::ssNeutral))) {
                                if (aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * aConst::PlanetOwnerMasks.Coalition == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                                    continue;
                                }
                            }
                            if (pas::in_range(ruins->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPiratePassive), static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary))) {
                                if (aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask * aConst::PlanetOwnerMasks.PirateClan == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                                    continue;
                                }
                            }
                        }
                    } else if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(Context)) {
                        if (!pas::contains(aConst::MicroModuleTemplates[ModuleIndex].AllowedHullOwnerMask, aConst::RaceToOwner(normalShip->PilotRace))) {
                            continue;
                        }
                    }
                }
            }
            if (Context == nullptr && aConst::MicroModuleTemplates[ModuleIndex].RacialRestriction) {
                continue;
            }
            if (pas::contains(pas::make_set<pas::Set<0, 255>>({{static_cast<std::int32_t>(MinimumPriority), static_cast<std::int32_t>(MaximumPriority)}}), aConst::MicroModuleTemplates[ModuleIndex].Priority) && (Equipment == nullptr || aItem::CanInstallMicroModule(ModuleIndex, Equipment))) {
                return ModuleIndex;
            }
            if (Attempts > 1000 && BestIndex >= 0 && (Equipment == nullptr || aItem::CanInstallMicroModule(BestIndex, Equipment))) {
                return BestIndex;
            }
            if (Attempts > 2000) {
                return ModuleIndex;
            }
            if (aConst::MicroModuleTemplates[ModuleIndex].Priority > MaximumPriority && (BestIndex < 0 || aConst::MicroModuleTemplates[ModuleIndex].Priority < BestPriority) && (Equipment == nullptr || aItem::CanInstallMicroModule(ModuleIndex, Equipment))) {
                BestIndex = ModuleIndex;
                BestPriority = aConst::MicroModuleTemplates[ModuleIndex].Priority;
            }
            if (Attempts % 99 == 0) {
                MinimumPriority = std::max<std::int32_t>(0, MinimumPriority - 10);
                MaximumPriority = std::min<std::int32_t>(100, MaximumPriority + 10);
                if (BestIndex >= 0 && MaximumPriority >= BestPriority) {
                    return BestIndex;
                }
            }
        } while (!false);
        return Result;
    }

    // Zero-based series index or -1; advances Self.RandomState.
    std::int32_t TGalaxy::SelectHullSeries(aGalaxyStruct::TOwnerId OwnerId, aGalaxyStruct::THullType HullType, std::uint8_t MinimumRarity, std::uint8_t MaximumRarity) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Temp{};
        pas::DynArray<std::int32_t> Indices{};
        Indices.set_length(aConst::HullSeriesCount);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::HullSeriesCount - 1); cpp_range.next(I); ) {
            Indices[I] = I;
            J = aMyFunction::NextRandomIntRange(0, aConst::HullSeriesCount - 1, RandomState);
            if (J < I) {
                Temp = Indices[J];
                Indices[J] = I;
                Indices[I] = Temp;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, aConst::HullSeriesCount - 1); cpp_range_2.next(I); ) {
            if (pas::contains(aConst::HullSeriesDefinitions[Indices[I]].AllowedOwners, OwnerId) && pas::contains(aConst::HullSeriesDefinitions[Indices[I]].AllowedShipTypes, HullType)) {
                if (([&] {
                    pas::Extended cpp_right_2 = aMyFunction::NextRandomIntRange(0, 20, RandomState);
                    pas::Extended cpp_right = aMyFunction::RemapClamped(TechLevel, 2.0, 8.0, 0.0, 8.0E+1) + cpp_right_2;
                    return aConst::HullSeriesDefinitions[Indices[I]].Year <= cpp_right;
                }()) && aConst::HullSeriesDefinitions[Indices[I]].ProbabilityWeight >= MinimumRarity && aConst::HullSeriesDefinitions[Indices[I]].ProbabilityWeight <= MaximumRarity) {
                    if (([&] {
                        pas::Extended cpp_left = aMyFunction::NextRandomUnitFloat(RandomState);
                        return cpp_left <= pas::real_divide(1.0L, aConst::HullSeriesDefinitions[Indices[I]].ProbabilityWeight);
                    }())) {
                        return Indices[I];
                    }
                }
            }
        }
        return -1;
    }

    // Invalid series values return false.
    std::uint8_t TGalaxy::IsDominatorSeriesUnresolved(aGalaxyStruct::TDominatorSeries Series) {
        switch (Series) {
            case aGalaxyStruct::dsBlazer: return BlazerSeriesResolvedTurn == 0;
            case aGalaxyStruct::dsKeller: return KellerSeriesResolvedTurn == 0;
            case aGalaxyStruct::dsTerron: return TerronSeriesResolvedTurn == 0;
            default: return false;
        }
    }

    // True if any selected series is unresolved; false for an empty set.
    std::uint8_t TGalaxy::HasUnresolvedDominatorSeries(TDominatorSeriesSet Series) {
        aGalaxyStruct::TDominatorSeries I{};
        std::uint8_t Result = false;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(I); ) {
            if (pas::contains(Series, I)) {
                switch (I) {
                    case aGalaxyStruct::dsBlazer: Result = BlazerSeriesResolvedTurn == 0; break;
                    case aGalaxyStruct::dsKeller: Result = KellerSeriesResolvedTurn == 0; break;
                    case aGalaxyStruct::dsTerron: Result = TerronSeriesResolvedTurn == 0; break;
                }
            }
            if (Result) {
                break;
            }
        }
        return Result;
    }

    // Advances deployed probes' terrain exploration and wear; idle completed planets still incur reduced wear.
    void TGalaxy::ProcessPlayerSatelliteExploration() {
        std::int32_t I{};
        std::int32_t Water{};
        std::int32_t Land{};
        std::int32_t Hill{};
        aItem::TSatellite* Satellite{};
        double Wear{};
        std::uint8_t Changed{};
        std::int32_t Amount{};
        if (aPlayer::GetPlayer() != nullptr) {
            Changed = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Satellites) - 1); cpp_range.next(I); ) {
                Satellite = pas::list_at<aItem::TSatellite>(aPlayer::GetPlayer()->Satellites, I);
                if (Satellite->BrokenFlag == 0 && Satellite->TargetPlanet != nullptr) {
                    aPlanet::TPlanet* cpp_with = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Satellite->TargetPlanet));
                    Water = cpp_with->WaterExplored;
                    Land = cpp_with->LandExplored;
                    Hill = cpp_with->HillExplored;
                    cpp_with->WaterExplored = std::min<std::int32_t>(cpp_with->WaterTiles, cpp_with->WaterExplored + Satellite->WaterExplorationRate);
                    cpp_with->LandExplored = std::min<std::int32_t>(cpp_with->LandTiles, cpp_with->LandExplored + Satellite->LandExplorationRate);
                    cpp_with->HillExplored = std::min<std::int32_t>(cpp_with->HillTiles, cpp_with->HillExplored + Satellite->HillExplorationRate);
                    Amount = cpp_with->WaterExplored - Water + (cpp_with->LandExplored - Land) + (cpp_with->HillExplored - Hill);
                    aPlayer::GetPlayer()->SatelliteTilesExplored += Amount;
                    Achievements::TryAddAchievementProgress(u"ARCHEOLOGY"_w, Amount);
                    if (Water < cpp_with->WaterExplored || Land < cpp_with->LandExplored || Hill < cpp_with->HillExplored) {
                        Wear = 1.0;
                        if (pas::list_count(aPlayer::GetPlayer()->Satellites) > aPlayer::GetPlayer()->GetSatelliteLimit()) {
                            Wear = Wear + (pas::list_count(aPlayer::GetPlayer()->Satellites) - aPlayer::GetPlayer()->GetSatelliteLimit()) * 1.0L;
                        }
                        Changed = true;
                    } else {
                        Wear = 0.1;
                    }
                    {
                        pas::Extended cpp_left = aMyFunction::NextRandomUnitFloat(cpp_with->RandomState);
                        double cpp_arg = cpp_left * Satellite->WearPerTurn * Wear;
                        aShip::TShip* player = aPlayer::GetPlayer();
                        aShip::TShip_ApplyItemDegradation(player, Satellite, aShip::idkUse, cpp_arg);
                    }
                }
            }
            if (Changed) {
                aPlayer::GetPlayer()->RefreshStorageBubbles();
            }
        }
    }

    // Requires a player. Counts deployed/player-storage probes, loose and carried probes in active Galaxy, and Self.StoredItems; excludes shop stock.
    std::int32_t TGalaxy::CountExistingSatellites() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        TStar* Star{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        TStoredItem* Stored{};
        std::int32_t Result = aPlayer::GetPlayer()->CountStoredItemUnits(nullptr, aConst::t_Satellite) + pas::list_count(aPlayer::GetPlayer()->Satellites);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Items) - 1); cpp_range_2.next(J); ) {
                Item = pas::list_at<aItem::TItem>(Star->Items, J);
                if (pas::class_cast_if<aItem::TSatellite*>(Item) != nullptr) {
                    ++Result;
                }
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range_4.next(K); ) {
                    Item = pas::list_at<aItem::TItem>(Ship->Inventory, K);
                    if (pas::class_cast_if<aItem::TSatellite*>(Item) != nullptr) {
                        ++Result;
                    }
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(StoredItems) - 1); cpp_range_5.next(I); ) {
            Stored = pas::list_at<TStoredItem>(StoredItems, I);
            if (Stored->Item != nullptr && pas::class_cast_if<aItem::TSatellite*>(Stored->Item) != nullptr) {
                ++Result;
            }
        }
        return Result;
    }

    std::int32_t TGalaxy::ComputeScaledMiniMoney(aGalaxyStruct::TOwnerId Owner) {
        std::int32_t Result = System::Round(AverageRangerCapital * 0.01L * aConst::OwnerInfo[Owner].FuelPriceFactor);
        if (Result > 250) {
            return System::Round((Result - 250) * 0.3L) + 250;
        }
        return Result;
    }

    std::int32_t TGalaxy::ComputeScaledSmallMoney(aGalaxyStruct::TOwnerId Owner) {
        std::int32_t Result = System::Round(AverageRangerCapital * 0.015384615384615384616L * aConst::OwnerInfo[Owner].FuelPriceFactor);
        if (Result > 1000) {
            return System::Round((Result - 1000) * 0.3L) + 1000;
        }
        return Result;
    }

    std::int32_t TGalaxy::ComputeScaledAverageMoney(aGalaxyStruct::TOwnerId Owner) {
        std::int32_t Result = System::Round(AverageRangerCapital * 0.025L * aConst::OwnerInfo[Owner].FuelPriceFactor);
        if (Result > 5000) {
            return System::Round((Result - 5000) * 0.3L) + 5000;
        }
        return Result;
    }

    std::int32_t TGalaxy::ComputeScaledBigMoney(aGalaxyStruct::TOwnerId Owner) {
        std::int32_t Result = System::Round(AverageRangerCapital * 0.04L * aConst::OwnerInfo[Owner].FuelPriceFactor);
        if (Result > 10000) {
            return System::Round((Result - 10000) * 0.3L) + 10000;
        }
        return Result;
    }

    std::int32_t TGalaxy::ComputeScaledHugeMoney(aGalaxyStruct::TOwnerId Owner) {
        std::int32_t Result = System::Round(AverageRangerCapital * 0.06666666666666666667L * aConst::OwnerInfo[Owner].FuelPriceFactor);
        if (Result > 25000) {
            return System::Round((Result - 25000) * 0.3L) + 25000;
        }
        return Result;
    }

    // Accepts Zero, Mini, Small, Average, Big and Huge; unknown tags raise. Uses active Galaxy for scaling.
    std::int32_t TGalaxy::ResolveMoneySizeTag(const std::u16string_view& Tag, aGalaxyStruct::TOwnerId Owner) {
        if (Tag == u"Zero"sv) {
            return 0;
        } else if (Tag == u"Mini"sv) {
            return Galaxy->ComputeScaledMiniMoney(Owner);
        } else if (Tag == u"Small"sv) {
            return Galaxy->ComputeScaledSmallMoney(Owner);
        } else if (Tag == u"Average"sv) {
            return Galaxy->ComputeScaledAverageMoney(Owner);
        } else if (Tag == u"Big"sv) {
            return Galaxy->ComputeScaledBigMoney(Owner);
        } else if (Tag == u"Huge"sv) {
            return Galaxy->ComputeScaledHugeMoney(Owner);
        } else {
            GR_Main::RaiseWideMessage(pas::concat_wide({u"Error! \u0423\u043a\u0430\u0437\u0430\u043d \u043d\u0435\u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u044b\u0439 \u0444\u043e\u0440\u043c\u0430\u0442 \u0440\u0430\u0437\u043c\u0435\u0440\u0430 \u0443 \u0432\u0435\u0449\u0438 ", Tag}));
            return -1;
        }
    }

    std::int32_t TGalaxy::GetMiniGoodsQuantity(std::uint8_t GoodsType) {
        return System::Round(aConst::GoodsMarket[GoodsType].BaseStock * 0.1L);
    }

    std::int32_t TGalaxy::GetSmallGoodsQuantity(std::uint8_t GoodsType) {
        return System::Round(aConst::GoodsMarket[GoodsType].BaseStock * 0.5L);
    }

    std::int32_t TGalaxy::GetAverageGoodsQuantity(std::uint8_t GoodsType) {
        std::int32_t Stock = aConst::GoodsMarket[GoodsType].BaseStock;
        return System::Round(Stock);
    }

    std::int32_t TGalaxy::GetBigGoodsQuantity(std::uint8_t GoodsType) {
        return System::Round(aConst::GoodsMarket[GoodsType].BaseStock * 1.5L);
    }

    std::int32_t TGalaxy::GetHugeGoodsQuantity(std::uint8_t GoodsType) {
        return System::Round(aConst::GoodsMarket[GoodsType].BaseStock * 2.0L);
    }

    // Sizes 0..5 select zero through huge; invalid sizes raise.
    std::int32_t TGalaxy::GetGoodsQuantityBySize(std::uint8_t Size, std::uint8_t GoodsType) {
        if (Size == 0) {
            return 0;
        } else if (Size == 1) {
            return TGalaxy::GetMiniGoodsQuantity(GoodsType);
        } else if (Size == 2) {
            return TGalaxy::GetSmallGoodsQuantity(GoodsType);
        } else if (Size == 3) {
            return TGalaxy::GetAverageGoodsQuantity(GoodsType);
        } else if (Size == 4) {
            return TGalaxy::GetBigGoodsQuantity(GoodsType);
        } else if (Size == 5) {
            return TGalaxy::GetHugeGoodsQuantity(GoodsType);
        } else {
            GR_Main::RaiseWideMessage(u"Error! \u0423\u043a\u0430\u0437\u0430\u043d \u043d\u0435\u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u044b\u0439 \u0444\u043e\u0440\u043c\u0430\u0442 \u043a\u043e\u043b\u0438\u0447\u0435\u0441\u0442\u0432\u0430 \u0442\u043e\u0432\u0430\u0440\u0430 "_wref.get());
            return -1;
        }
    }

    // Zero maps to zero; otherwise chooses levels 1..5, with ties favoring the larger level.
    std::uint8_t TGalaxy::ClassifyGoodsQuantity(std::int32_t Quantity, std::uint8_t GoodsType) {
        if (Quantity == 0) {
            return 0;
        }
        std::int32_t Distance1 = pas::abs(TGalaxy::GetGoodsQuantityBySize(1, GoodsType) - Quantity);
        std::int32_t Distance2 = pas::abs(TGalaxy::GetGoodsQuantityBySize(2, GoodsType) - Quantity);
        std::int32_t Distance3 = pas::abs(TGalaxy::GetGoodsQuantityBySize(3, GoodsType) - Quantity);
        std::int32_t Distance4 = pas::abs(TGalaxy::GetGoodsQuantityBySize(4, GoodsType) - Quantity);
        std::int32_t Distance5 = pas::abs(TGalaxy::GetGoodsQuantityBySize(5, GoodsType) - Quantity);
        if (Distance1 < Distance2) {
            return 1;
        } else if (Distance2 < Distance3) {
            return 2;
        } else if (Distance3 < Distance4) {
            return 3;
        } else if (Distance4 < Distance5) {
            return 4;
        } else {
            return 5;
        }
    }

    std::int32_t TGalaxy::GetMinimumGoodsPrice(std::uint8_t GoodsType) {
        return aConst::GoodsMarket[GoodsType].MinPrice;
    }

    std::int32_t TGalaxy::GetLowGoodsPrice(std::uint8_t GoodsType) {
        return (aConst::GoodsMarket[GoodsType].MinPrice + aConst::GoodsMarket[GoodsType].AveragePrice) / 2;
    }

    std::int32_t TGalaxy::GetAverageGoodsPrice(std::uint8_t GoodsType) {
        return aConst::GoodsMarket[GoodsType].AveragePrice;
    }

    std::int32_t TGalaxy::GetHighGoodsPrice(std::uint8_t GoodsType) {
        return (aConst::GoodsMarket[GoodsType].AveragePrice + aConst::GoodsMarket[GoodsType].MaxPrice) / 2;
    }

    std::int32_t TGalaxy::GetMaximumGoodsPrice(std::uint8_t GoodsType) {
        return aConst::GoodsMarket[GoodsType].MaxPrice;
    }

    // Levels 1..5 select minimum through maximum; invalid levels raise.
    std::int32_t TGalaxy::GetGoodsPriceByLevel(std::uint8_t Level, std::uint8_t GoodsType) {
        if (Level == 1) {
            return TGalaxy::GetMinimumGoodsPrice(GoodsType);
        } else if (Level == 2) {
            return TGalaxy::GetLowGoodsPrice(GoodsType);
        } else if (Level == 3) {
            return TGalaxy::GetAverageGoodsPrice(GoodsType);
        } else if (Level == 4) {
            return TGalaxy::GetHighGoodsPrice(GoodsType);
        } else if (Level == 5) {
            return TGalaxy::GetMaximumGoodsPrice(GoodsType);
        } else {
            GR_Main::RaiseWideMessage(u"Error! \u0423\u043a\u0430\u0437\u0430\u043d \u043d\u0435\u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u044b\u0439 \u0444\u043e\u0440\u043c\u0430\u0442 \u0441\u0442\u043e\u0438\u043c\u043e\u0441\u0442\u0438 \u0442\u043e\u0432\u0430\u0440\u0430 "_wref.get());
            return -1;
        }
    }

    // Zero maps to zero; otherwise chooses levels 1..5, with ties favoring the larger level.
    std::uint8_t TGalaxy::ClassifyGoodsPrice(std::int32_t Price, std::uint8_t GoodsType) {
        if (Price == 0) {
            return 0;
        }
        std::int32_t Distance1 = pas::abs(TGalaxy::GetGoodsPriceByLevel(1, GoodsType) - Price);
        std::int32_t Distance2 = pas::abs(TGalaxy::GetGoodsPriceByLevel(2, GoodsType) - Price);
        std::int32_t Distance3 = pas::abs(TGalaxy::GetGoodsPriceByLevel(3, GoodsType) - Price);
        std::int32_t Distance4 = pas::abs(TGalaxy::GetGoodsPriceByLevel(4, GoodsType) - Price);
        std::int32_t Distance5 = pas::abs(TGalaxy::GetGoodsPriceByLevel(5, GoodsType) - Price);
        if (Distance1 < Distance2) {
            return 1;
        } else if (Distance2 < Distance3) {
            return 2;
        } else if (Distance3 < Distance4) {
            return 3;
        } else if (Distance4 < Distance5) {
            return 4;
        } else {
            return 5;
        }
    }

    void TGalaxy::ProcessStationSpawning() {
        std::int32_t I{};
        std::int32_t Count{};
        TConstellation* Constellation{};
        aGalaxyStruct::TStationType Kind{};
        if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L) {
            return;
        }
        for (Kind = static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstRangerCenter); Kind <= static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstDominion); ++Kind) {
            Count = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Constellations) - 1); cpp_range.next(I); ) {
                Constellation = pas::list_at<TConstellation>(Galaxy->Constellations, I);
                if (Constellation->Visible && Constellation->ShipTypeCounts[Kind] > 0) {
                    ++Count;
                }
            }
            if (Count == 0 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.3L) {
                ReplenishStationType(Kind);
                return;
            }
        }
        ReplenishStationType((CurrentTurn + 100) % 7 + 6);
    }

    void TGalaxy::ReplenishStationType(aGalaxyStruct::TStationType StationType) {
        static const pas::Set<0, 255> StationMask = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::rstRangerCenter, aGalaxyStruct::rstDominion}});
        static const pas::Set<0, 255> MilitaryBaseMask = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::rstMilitaryBase}});
        static const pas::Set<0, 255> PirateBaseMask = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::rstPirateBase}});
        std::int32_t I{};
        std::int32_t J{};
        std::uint8_t Hostile{};
        std::uint8_t Assigned{};
        TConstellation* Constellation{};
        TStar* Star{};
        aRuins::TRuins* Station{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Constellations) - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<TConstellation>(Galaxy->Constellations, I);
            if (!(Constellation->Id != 20 && Constellation->ShipTypeCounts[StationType] <= 0 && Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationMask)) < pas::list_count(Constellation->Stars) && Constellation->ShipTypeCounts[aGalaxyStruct::stKling] <= 0)) {
                continue;
            }
            Hostile = false;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range_2.next(J); ) {
                Star = pas::list_at<TStar>(Constellation->Stars, J);
                if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators || Star->Status.CustomFaction != u"") {
                    Hostile = true;
                }
            }
            if (!Hostile) {
                {
                    std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(Constellation->Stars) - 1, RandomState);
                    pas::List* stars = Constellation->Stars;
                    Star = pas::list_at<TStar>(stars, nextRandomIntRange);
                }
                if (!(Star->Status.Battle == 0 && pas::contains(aConst::FactionStandingMasks[Star->Status.ControlFaction], aConst::StationDefaultStandings[StationType]) && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 70 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationMask)) <= 1 && (StationType != aGalaxyStruct::rstPirateBase || Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(MilitaryBaseMask)) <= 0) && (StationType != aGalaxyStruct::rstMilitaryBase || Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(PirateBaseMask)) <= 0))) {
                    continue;
                }
                if (StationType == aGalaxyStruct::rstMilitaryBase) {
                    Assigned = false;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range_3.next(J); ) {
                        if (TGalaxy::HasMilitaryBaseAssignedToStar(pas::list_at<TStar>(Constellation->Stars, J))) {
                            Assigned = true;
                            break;
                        }
                    }
                    if (Assigned) {
                        continue;
                    }
                }
                if (StationType == aGalaxyStruct::rstDominion) {
                    Assigned = false;
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range_4.next(J); ) {
                        if (pas::list_at<TStar>(Constellation->Stars, J)->Dominion != nullptr) {
                            Assigned = true;
                            break;
                        }
                    }
                    if (Assigned) {
                        continue;
                    }
                }
                Station = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                Station->Init(StationType, Star, pas::WideString());
                if (CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText3 = ([&] {
                        pas::WideString name = Station->GetName();
                        auto name_2 = pas::borrow(Star->Name);
                        pas::WideString name_3 = Star->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(pas::concat_wide({u"GalaxyNews.CreateNewObject.", aConst::ShipTypeNames[StationType].Name}), GenerationSeed * (Galaxy->CurrentTurn / 10));
                        pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Name>"_w, std::move(name), u"<Star>"_w, name_2.get(), u"<Sector>"_w, std::move(name_3));
                    }());
                    TGalaxy* galaxy = Galaxy;
                    galaxy->AddPlanetNewsWithPlayerBubble(aGalaxyStruct::gnStationCreated, std::move(formatText3));
                }
                return;
            }
        }
    }

    void TGalaxy::ProcessDominatorResearchProgress() {
        aGalaxyStruct::TDominatorSeries Series{};
        float Progress{};
        pas::WideString News{};
        if ((DominatorResearch[aGalaxyStruct::dsBlazer].Progress < 1.0E+2L || DominatorResearch[aGalaxyStruct::dsKeller].Progress < 1.0E+2L || DominatorResearch[aGalaxyStruct::dsTerron].Progress < 1.0E+2L) && ShipTypeCounts[aGalaxyStruct::rstScienceBase] > 0) {
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(Series); ) {
                if (DominatorResearch[Series].Progress < 1.0E+2L) {
                    Progress = static_cast<long double>(DominatorResearch[Series].Progress) + GetDominatorResearchRate(Series);
                    {
                        std::int32_t cpp_right = aMyFunction::NextRandomIntRange(2, 5, RandomState);
                        if (pas::imod(CurrentTurn, cpp_right) == 0) {
                            std::int32_t max = std::max<std::int32_t>(0, ([&] {
                                std::int32_t cpp_right_2 = aMyFunction::NextRandomIntRange(1, aConst::GalaxyDifficultyTuning[DifficultyLevels[2]].MaximumResearchMaterialConsumption, RandomState);
                                return DominatorResearch[Series].Material - cpp_right_2;
                            }()));
                            DominatorResearch[Series].Material = max;
                        }
                    }
                    if (Progress >= 1.0E+2L) {
                        switch (Series) {
                            case aGalaxyStruct::dsBlazer: News = u"Programms.LogicalNegation.GalaxyNews"_w; break;
                            case aGalaxyStruct::dsKeller: News = u"Programms.Dematerial.GalaxyNews"_w; break;
                            case aGalaxyStruct::dsTerron: News = u"Programms.Energotron.GalaxyNews"_w; break;
                        }
                        {
                            pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(News, GenerationSeed * (Galaxy->CurrentTurn / 10));
                            TGalaxy* galaxy = Galaxy;
                            galaxy->AddPlanetNewsWithPlayerBubble(aGalaxyStruct::gnDominatorResearchCompleted, std::move(pickLocalizedTextVariant));
                        }
                        ++aPlayer::GetPlayer()->AchievementStats->CompletedResearchPrograms;
                        aPlayer::GetPlayer()->AchievementStats->CheckScienceAchievement();
                    }
                    DominatorResearch[Series].Progress = pas::real_min<float>(1.0E+2f, Progress);
                }
            }
        }
    }

    // Every selected series must have at least 100 progress; the empty set returns true.
    std::uint8_t TGalaxy::IsDominatorResearchComplete(TDominatorSeriesSet Series) {
        aGalaxyStruct::TDominatorSeries I{};
        std::uint8_t Result = true;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(I); ) {
            if (pas::contains(Series, I) && DominatorResearch[I].Progress < 1.0E+2L) {
                Result = false;
                break;
            }
        }
        return Result;
    }

    // Percentage points per day.
    float TGalaxy::GetDominatorResearchRate(aGalaxyStruct::TDominatorSeries Series) {
        std::int32_t Efficiency = GetDominatorResearchEfficiency(Series);
        return static_cast<long double>(aMyFunction::RemapClamped(Efficiency, 0.0, 1.0E+2, 1.0E-5, aConst::GalaxyDifficultyTuning[DifficultyLevels[2]].MaximumDominatorResearchRate)) * aConst::DominatorResearchRateMultipliers[Series];
    }

    // Returns 20..100 percent.
    aGalaxyStruct::TPercent TGalaxy::GetDominatorResearchEfficiency(aGalaxyStruct::TDominatorSeries Series) {
        return System::Trunc(aMyFunction::RemapClamped(DominatorResearch[Series].Material, 0.0, 3.0E+2, 2.0E+1, 1.0E+2));
    }

    // One-based index over active Galaxy star/ship order. Uses Self's cached type count as an early gate; missing entries return nil.
    void* TGalaxy::FindStationByTypeAndIndex(std::int32_t Index, aGalaxyStruct::TStationType StationType) {
        TStar* Star{};
        aShip::TShip* Ship{};
        std::int32_t I{};
        std::int32_t J{};
        void* Result = nullptr;
        std::int32_t Number = 1;
        if (ShipTypeCounts[StationType] > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<TStar>(Galaxy->Stars, I);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship->TypeId == StationType) {
                        if (Number == Index) {
                            return Ship;
                        }
                        ++Number;
                    }
                }
            }
        }
        return Result;
    }

    // Eligible only at positive multiples of 365 accrued deposit days.
    void TGalaxy::TryAwardDepositPrize() {
        std::int32_t Deposit{};
        std::int32_t Chance{};
        std::int32_t Roll{};
        aRuins::TRuins* Station{};
        aItem::TItem* Item{};
        pas::WideString Text{};
        if (aPlayer::GetPlayer()->DepositAmount != 0 && aPlayer::GetPlayer()->DepositDayCount != 0 && pas::imod(aPlayer::GetPlayer()->DepositDayCount, aGalaxyStruct::TurnsPerYear) == 0) {
            Station = pas::checked_cast<aRuins::TRuins*>(static_cast<pas::Object*>(FindStationByTypeAndIndex(aMyFunction::SeededRandomIntRange(1, ShipTypeCounts[aGalaxyStruct::rstBusinessCenter], Galaxy->GenerationSeed + Galaxy->CurrentTurn / 33), static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstBusinessCenter))));
            if (Station != nullptr) {
                Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DepositPrizeLose"_wref.get(), Station->Seed * (Galaxy->CurrentTurn / 10));
                Chance = 30 + pas::idiv(aPlayer::GetPlayer()->DepositDayCount, aGalaxyStruct::TurnsPerYear) * 10;
                Roll = aMyFunction::SeededRandomIntRange(1, 100, Station->Seed + Galaxy->CurrentTurn / 7);
                if (Roll < Chance) {
                    Deposit = aPlayer::GetPlayer()->ComputeDepositAccruedValue();
                    {
                        std::int32_t round = System::Round(Deposit * 1.0L);
                        std::int32_t round_2 = System::Round(Deposit * 0.1L);
                        Item = Station->FindMostExpensiveShopItem(round_2, round);
                    }
                    if (Item != nullptr) {
                        pas::list_delete(Station->EquipmentShop, pas::list_indexof(Station->EquipmentShop, reinterpret_cast<void*>(Item)));
                        aPlayer::GetPlayer()->AddItemToPlayerStorage(Item, Station, -1);
                        aPlayer::GetPlayer()->RefreshStorageBubbles();
                        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DepositPrizeWin"_wref.get(), Station->Seed * (Galaxy->CurrentTurn / 10));
                        {
                            auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                            pas::WideString displayName = Item->GetDisplayName();
                            aMyFunction::ReplaceTextToken(Text, u"<Item>"_w, std::move(displayName), textHighlightColorTag.get());
                        }
                    }
                }
                {
                    auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString fullName = Station->GetFullName(u" "_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<BKName>"_w, std::move(fullName), textHighlightColorTag_2.get());
                }
                aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, Station->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
                Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, Text, u""_wref.get());
            }
        }
    }

    // Debt pauses deposit accrual. Both states are cleared when no business centers remain.
    void TGalaxy::ProcessBankDebtAndDeposits() {
        static const pas::Set<0, 255> AffectedShipTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htPirate}});
        static const pas::Set<0, 255> AffectedOwners = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate}});
        pas::WideString News{};
        std::int32_t OldDebt{};
        std::int32_t Penalty{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (ShipTypeCounts[aGalaxyStruct::rstBusinessCenter] > 0) {
            if (aPlayer::GetPlayer()->DepositAmount > 0 && aPlayer::GetPlayer()->DebtAmount == 0) {
                ++aPlayer::GetPlayer()->DepositDayCount;
                TryAwardDepositPrize();
            }
            if (aPlayer::GetPlayer()->DebtAmount > 0 && aPlayer::GetPlayer()->DebtDueTurn <= Galaxy->CurrentTurn) {
                ++aPlayer::GetPlayer()->DebtDefaultCount;
                if (aPlayer::GetPlayer()->DebtDefaultCount == 1) {
                    Achievements::TryAddAchievementProgress(u"CREDITOR"_w, 1);
                }
                OldDebt = aPlayer::GetPlayer()->DebtAmount;
                Penalty = aMyFunction::RoundAndTruncateToTens(pas::real_min<pas::Extended>(aPlayer::GetPlayer()->DebtAmount * 0.5L * aPlayer::GetPlayer()->DebtDefaultCount, static_cast<pas::Extended>(aPlayer::GetPlayer()->Wealth / 8)));
                aPlayer::GetPlayer()->DebtAmount = std::min<std::int32_t>(aGalaxyStruct::MaxMonetaryValue, aPlayer::GetPlayer()->DebtAmount + Penalty);
                {
                    std::int64_t cpp_right = System::Round(aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(Galaxy->CurrentTurn / 80), 0.0, 1.0, 0.7, 1.5) * 3.0E+2L);
                    aPlayer::GetPlayer()->DebtDueTurn = Galaxy->CurrentTurn + cpp_right;
                }
                if (aPlayer::GetPlayer()->DebtDefaultCount < 3) {
                    News = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DebtInfo"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                } else {
                    News = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DebtInfoContinue"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    aRanger::TRanger_ChangeGlobalRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmDecrease, 50, static_cast<aConst::THullShipTypeMask>(AffectedShipTypes), static_cast<aGalaxyStruct::TOwnerMask>(AffectedOwners));
                }
                aMyFunction::ReplaceTextToken(News, u"<OldMoney>"_w, pas::wide_int_to_str(OldDebt), aMyFunction::TextHighlightColorTag);
                aMyFunction::ReplaceTextToken(News, u"<Penalty>"_w, pas::wide_int_to_str(Penalty), aMyFunction::TextHighlightColorTag);
                aMyFunction::ReplaceTextToken(News, u"<NewMoney>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount), aMyFunction::TextHighlightColorTag);
                {
                    auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString formatTurnDate = Galaxy->FormatTurnDate(aPlayer::GetPlayer()->DebtDueTurn);
                    aMyFunction::ReplaceTextToken(News, u"<NewDate>"_w, std::move(formatTurnDate), textHighlightColorTag.get());
                }
                Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, News, u""_wref.get());
            }
        } else if (aPlayer::GetPlayer()->DebtAmount > 0 || aPlayer::GetPlayer()->DepositAmount > 0) {
            News = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DeadAllBKStart"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
            if (aPlayer::GetPlayer()->DebtAmount > 0) {
                News = pas::concat_wide({News, u"\r\n", ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount);
                    pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DeadAllBKDebt"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    pas::WideString textHighlightColorTag_2 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag_2), u"<Money>"_w, std::move(intToStr));
                }())});
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerDebtNullified"_w, nullptr);
                Event->AddData(aPlayer::GetPlayer()->DebtAmount);
            }
            aPlayer::GetPlayer()->DebtAmount = 0;
            aPlayer::GetPlayer()->DebtDueTurn = 0;
            aPlayer::GetPlayer()->DebtDefaultCount = 0;
            if (aPlayer::GetPlayer()->DepositAmount > 0) {
                News = pas::concat_wide({News, u"\r\n", ([&] {
                    pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->DepositAmount);
                    pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.BK.DeadAllBKDeposit"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    pas::WideString textHighlightColorTag_3 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_2), std::move(textHighlightColorTag_3), u"<Money>"_w, std::move(intToStr_2));
                }())});
            }
            aPlayer::GetPlayer()->DepositAmount = 0;
            aPlayer::GetPlayer()->DepositStartTurn = 0;
            aPlayer::GetPlayer()->DepositDayCount = 0;
            aPlayer::GetPlayer()->DepositInterestRate = 0.0f;
            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, News, u""_wref.get());
        }
    }

    // Also clears the player's deposited nodes when no ranger centers remain.
    void TGalaxy::ProcessRangerCenterNewYearEvent() {
        std::uint16_t Year{};
        std::uint16_t Month{};
        std::uint16_t Day{};
        pas::WideString Text{};
        aRuins::TRuins* Station{};
        aItem::TMicroModule* Item{};
        std::int32_t Minimum{};
        std::int32_t Maximum{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (aPlayer::GetPlayer() != nullptr) {
            if (ShipTypeCounts[aGalaxyStruct::rstRangerCenter] > 0) {
                if (Galaxy->CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns) {
                    SysUtilsImports::DecodeDate(aGalaxy::GameTurnToDateTime(Galaxy->CurrentTurn - aGalaxyStruct::GalaxyWarmupTurns), Year, Month, Day);
                    if (Day == 31 && Month == 12) {
                        Station = pas::checked_cast<aRuins::TRuins*>(static_cast<pas::Object*>(FindStationByTypeAndIndex(aMyFunction::SeededRandomIntRange(1, ShipTypeCounts[aGalaxyStruct::rstRangerCenter], Galaxy->GenerationSeed + Galaxy->CurrentTurn), static_cast<aGalaxyStruct::TStationType>(aGalaxyStruct::rstRangerCenter))));
                        if (Station != nullptr) {
                            Item = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                            Minimum = 70;
                            Minimum -= System::Round(aMyFunction::RemapClamped(Year, 3301.0, 3311.0, 0.0, 1.0E+1));
                            Minimum -= System::Round(aMyFunction::RemapClamped(aPlayer::GetPlayer()->PlaceInRating, 1.0, pas::list_count(Galaxy->Rangers), 1.0E+1, 0.0));
                            Minimum -= System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(aPlayer::GetPlayer()->Rank), 0.0, 7.0, 0.0, 1.0E+1));
                            Minimum -= Galaxy->ScaleIntByTechLevel(0, 20);
                            Minimum += aMyFunction::SeededRandomIntRange(-10, 10, Galaxy->GenerationSeed + Station->Seed);
                            Minimum = std::max<std::int32_t>(1, std::min<std::int32_t>(Minimum, 100));
                            Maximum = 100;
                            Item->Init(TGalaxy::SelectMicroModule(Minimum, Maximum, Galaxy->GenerationSeed + Station->Seed + Galaxy->CurrentTurn, Station));
                            Item->OwnerId = aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace);
                            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerReceivesMMOnNewYear"_w, nullptr);
                            Event->AddData(Item->Id);
                            Event->AddData(Item->MicroModuleIndex - 1);
                            aPlayer::GetPlayer()->AddItemToPlayerStorage(Item, Station, -1);
                            aPlayer::GetPlayer()->RefreshStorageBubbles();
                            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.RC.NewYear"_wref.get(), GenerationSeed * (Galaxy->CurrentTurn / 10));
                            {
                                auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                                pas::WideString fullName = Station->GetFullName(u" "_wref.get());
                                aMyFunction::ReplaceTextToken(Text, u"<RCName>"_w, std::move(fullName), textHighlightColorTag.get());
                            }
                            aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, Station->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
                            aMyFunction::ReplaceTextToken(Text, u"<Year>"_w, pas::wide_int_to_str(Year + 1), aMyFunction::TextHighlightColorTag);
                            {
                                auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                                pas::WideString displayName = Item->GetDisplayName();
                                aMyFunction::ReplaceTextToken(Text, u"<Item>"_w, std::move(displayName), textHighlightColorTag_2.get());
                            }
                            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, Text, u""_wref.get());
                        }
                    }
                }
            } else if (aPlayer::GetPlayer()->BaseNodes > 0) {
                Text = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->BaseNodes);
                    pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.RC.DeadBaseNod"_wref.get(), GenerationSeed * (Galaxy->CurrentTurn / 10));
                    pas::WideString textHighlightColorTag_3 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag_3), u"<Nod>"_w, std::move(intToStr));
                }());
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerNodesNullified"_w, nullptr);
                Event->AddData(aPlayer::GetPlayer()->BaseNodes);
                aPlayer::GetPlayer()->BaseNodes = 0;
                Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, Text, u""_wref.get());
            }
        }
    }

    // Returns BuildLiberationOrders on accepted groups; false for early rejection or disbanding.
    std::uint8_t TGalaxy::TryCreateLiberationGroup() {
        std::int32_t i{};
        std::int32_t Index{};
        std::int32_t j{};
        std::int32_t k{};
        std::int32_t ShipCount{};
        TStar* Star{};
        aShip::TShip* Ship{};
        aPlanet::TPlanet* Planet{};
        double GroupStrength{};
        double EnemyStrength{};
        std::uint8_t HasSubtypeOne{};
        std::uint8_t Result = false;
        std::uint8_t EmergencyControlPercent = 5;
        if (Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) > 90 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L && Galaxy->WarDeltaWin[0] > 3) {
            return Result;
        }
        if (Galaxy->WarDeltaWin[0] > 5 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.8L) {
            return Result;
        }
        TConstellation* Constellation = nullptr;
        Index = aMyFunction::NextRandomIntRange(0, pas::list_count(Constellations) - 1, RandomState);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Constellations) - 1); cpp_range.next(i); ) {
            aMyFunction::IncrementWrapped(Index, 0, pas::list_count(Constellations) - 1);
            Constellation = pas::list_at<TConstellation>(Constellations, Index);
            if (static_cast<std::uint8_t>(Constellation->HasDominatorPresence() ^ 1) && static_cast<std::uint8_t>(Constellation->HasPirateClanPresence() ^ 1)) {
                break;
            }
        }
        aGroup::TGroup* Group = pas::construct_call<aGroup::TGroup>(aGroup::TGroup_Create);
        pas::list_add(LiberationGroups, reinterpret_cast<void*>(Group));
        if (Group->SelectLiberationTarget()) {
            EnemyStrength = 0.0;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Group->TargetStar->Ships) - 1); cpp_range_2.next(i); ) {
                Ship = pas::list_at<aShip::TShip>(Group->TargetStar->Ships, i);
                if (Ship->IsOutsideStarSpace()) {
                    continue;
                }
                if (Group->TargetStar->Status.CustomFaction != u"") {
                    if (Ship->CurrentStanding != aGalaxyStruct::ssCustom) {
                        continue;
                    }
                } else if (Group->TargetStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                    if (Ship->CurrentStanding != aGalaxyStruct::ssDominator) {
                        continue;
                    }
                } else if (Group->TargetStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                    if (!pas::in_range(Ship->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPirateActive), static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary))) {
                        continue;
                    }
                }
                EnemyStrength = static_cast<long double>(EnemyStrength) + Ship->Strength;
            }
            ShipCount = 0;
            GroupStrength = 0.0;
            HasSubtypeOne = false;
            Index = aMyFunction::NextRandomIntRange(0, pas::list_count(Constellation->Stars) - 1, RandomState);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range_3.next(i); ) {
                aMyFunction::IncrementWrapped(Index, 0, pas::list_count(Constellation->Stars) - 1);
                Star = pas::list_at<TStar>(Constellation->Stars, Index);
                if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.Battle == 0 && Star->Status.CustomFaction == u"" && (static_cast<std::uint8_t>(Star->HasLiberationGroupOrder() ^ 1) || Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) <= EmergencyControlPercent)) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_4.next(j); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, j);
                        if (Planet->IsCoalitionOwned) {
                            std::int32_t cpp_first = aMyFunction::NextRandomIntRange(0, 1, Planet->RandomState);
                            std::int32_t cpp_last = pas::list_count(Planet->Warriors) - 1;
                            for (auto cpp_range_5 = pas::for_to<std::int32_t>(cpp_first, cpp_last); cpp_range_5.next(k); ) {
                                Ship = pas::list_at<aShip::TShip>(Planet->Warriors, k);
                                if (Ship->ScriptShip == nullptr && Ship->CurrentPlanet == Planet && pas::list_indexof(Star->Ships, reinterpret_cast<void*>(Ship)) < 0 && Ship->LiberationGroup == nullptr) {
                                    Ship->LiberationGroup = Group;
                                    pas::list_add(Star->Ships, reinterpret_cast<void*>(Ship));
                                    Group->AddShip(Ship);
                                    ++ShipCount;
                                    if (reinterpret_cast<aWarrior::TWarrior*>(Ship)->WarriorType == aWarrior::wtFlagship) {
                                        GroupStrength = 0.5L * Ship->Strength + GroupStrength;
                                        if (!HasSubtypeOne) {
                                            EnemyStrength = 1.2L * EnemyStrength;
                                        }
                                        HasSubtypeOne = true;
                                    } else {
                                        GroupStrength = static_cast<long double>(GroupStrength) + Ship->Strength;
                                    }
                                    if (ShipCount >= 5 && (GroupStrength >= EnemyStrength || ShipCount >= 30)) {
                                        return Group->BuildLiberationOrders();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (GroupStrength <= EnemyStrength && ShipCount < 30) {
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->Stars) - 1); cpp_range_6.next(i); ) {
                    aMyFunction::IncrementWrapped(Index, 0, pas::list_count(Constellation->Stars) - 1);
                    Star = pas::list_at<TStar>(Constellation->Stars, Index);
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && Star->Status.Battle == 0 && Star->Status.CustomFaction == u"" && (static_cast<std::uint8_t>(Star->HasLiberationGroupOrder() ^ 1) || Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) <= EmergencyControlPercent)) {
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_7.next(j); ) {
                            Ship = pas::list_at<aShip::TShip>(Star->Ships, j);
                            if (Ship->TypeId == aGalaxyStruct::stWarrior && Ship->HomePlanet->CurrentStar == Star && static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && Ship->ScriptShip == nullptr && pas::is_one_of<aShip::soNone, aShip::soMove>(Ship->Order) && Ship->AbsoluteScriptOrder == 0 && Ship->LiberationGroup == nullptr) {
                                Ship->LiberationGroup = Group;
                                Group->AddShip(Ship);
                                ++ShipCount;
                                if (reinterpret_cast<aWarrior::TWarrior*>(Ship)->WarriorType == aWarrior::wtFlagship) {
                                    GroupStrength = 0.5L * Ship->Strength + GroupStrength;
                                    if (!HasSubtypeOne) {
                                        EnemyStrength = 1.2L * EnemyStrength;
                                    }
                                    HasSubtypeOne = true;
                                } else {
                                    GroupStrength = static_cast<long double>(GroupStrength) + Ship->Strength;
                                }
                            }
                        }
                    }
                }
            }
            if (ShipCount >= 1 && Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) <= EmergencyControlPercent) {
                if (aPlayer::GetPlayer() != nullptr) {
                    GroupStrength = aPlayer::GetPlayer()->Strength * 2.0L + GroupStrength;
                }
                if (1.0E+1L * GroupStrength >= static_cast<long double>(aMyFunction::NextRandomIntRange(3, 10, RandomState)) * EnemyStrength || aMyFunction::NextRandomIntRange(0, 700, RandomState) == 0) {
                    return Group->BuildLiberationOrders();
                }
                Group->Disband();
            } else {
                if (ShipCount >= 5) {
                    return Group->BuildLiberationOrders();
                }
                Group->Disband();
            }
        }
        return false;
    }

    std::uint8_t TGalaxy::TryDispatchMilitaryBaseToEnemyStar() {
        std::int32_t I{};
        std::int32_t J{};
        TStar* Star{};
        aShip::TShip* Ship{};
        aWarrior::TWarrior* Warrior{};
        pas::WideString Text{};
        std::uint8_t Result = false;
        if (TGalaxy::FindMilitaryBaseInTransit() != nullptr || CurrentTurn < aGalaxyStruct::GalaxyWarmupTurns || CurrentTurn % 133 != 0) {
            return Result;
        }
        if (aMyFunction::SeededRandomUnitFloat(GenerationSeed * CurrentTurn + CountStarsInBattle()) < 0.5L) {
            return Result;
        }
        aRuins::TRuins* Station = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            if (Star->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase] != 0 && static_cast<std::uint8_t>(Star->Constellation->HasDominatorPresence() ^ 1) && aMyFunction::SeededRandomUnitFloat((GenerationSeed + I) * CurrentTurn * Star->GenerationSeed) >= 0.2L) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship->TypeId == aGalaxyStruct::rstMilitaryBase) {
                        Station = pas::checked_cast<aRuins::TRuins*>(Ship);
                        if (Station->FlyToStar != nullptr) {
                            Station = nullptr;
                        } else if (Station->HasScriptControl()) {
                            Station = nullptr;
                        }
                    }
                    if (Station != nullptr) {
                        break;
                    }
                }
                if (Station != nullptr) {
                    break;
                }
            }
        }
        if (Station == nullptr) {
            return Result;
        }
        std::int32_t Count = 0;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
            Count += pas::list_at<TStar>(Galaxy->Stars, I)->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase];
        }
        if (Count < 2) {
            return Result;
        }
        TStar* Target = nullptr;
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range_4.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            if (Star->IsConstellationVisible() && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfDominators && pas::list_count(Star->Ships) <= Star->ShipTypeCounts[aGalaxyStruct::stKling] && Star->ShipTypeCounts[aGalaxyStruct::stKling] >= 5 && static_cast<std::uint8_t>(TGalaxy::HasMilitaryBaseAssignedToStar(Star) ^ 1) && static_cast<std::uint8_t>(HasLiberationGroupTargetingStar(Star) ^ 1) && aMyFunction::SeededRandomUnitFloat((GenerationSeed + I) * CurrentTurn * Star->GenerationSeed) >= 0.7L && ([&] {
                pas::Extended cpp_right = ScaleIntByTechLevel(30, 60);
                return aMyFunction::PointDistance(Station->CurrentStar->Position, Star->Position) >= cpp_right;
            }())) {
                Target = Star;
                break;
            }
        }
        if (Target == nullptr) {
            return Result;
        }
        aPlanet::TPlanet* Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Station->CurrentStar->FindFastestResearchPlanet()));
        if (!Planet->IsCoalitionOwned) {
            return Result;
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, aMyFunction::SeededRandomIntRange(4, 6, CurrentTurn * Planet->GenerationSeed)); cpp_range_5.next(I); ) {
            if (I == 1 && Galaxy->RangerSpawnQuotas[Planet->RaceId] > 0) {
                Warrior = pas::checked_cast<aWarrior::TWarrior*>(static_cast<pas::Object*>(Planet->BuyFlagship(200)));
            } else {
                Warrior = pas::checked_cast<aWarrior::TWarrior*>(static_cast<pas::Object*>(Planet->BuyWarrior(200)));
            }
            Warrior->Position = Station->Position;
            Warrior->CurrentPlanet = nullptr;
            Warrior->DockedTo = Station;
            pas::list_add(Planet->CurrentStar->Ships, reinterpret_cast<void*>(Warrior));
        }
        std::int32_t cpp_right_2 = aMyFunction::SeededRandomIntRange(30, 40, CurrentTurn + Planet->GenerationSeed);
        std::int32_t Turn = CurrentTurn + cpp_right_2;
        Station->FlyToStar = Target;
        Station->FlyDate = Turn;
        Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.WBGoToEnemyStar.Create"_wref.get(), GenerationSeed * (Galaxy->CurrentTurn / 10));
        aMyFunction::ReplaceTextToken(Text, u"<WB>"_w, Station->Name, aMyFunction::TextHighlightColorTag);
        aMyFunction::ReplaceTextToken(Text, u"<WBStar>"_w, Station->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
        aMyFunction::ReplaceTextToken(Text, u"<StarEnemy>"_w, Target->Name, aMyFunction::TextHighlightColorTag);
        {
            auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
            pas::WideString name = Station->CurrentStar->Constellation->GetName();
            aMyFunction::ReplaceTextToken(Text, u"<WBSector>"_w, std::move(name), textHighlightColorTag.get());
        }
        {
            auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
            pas::WideString name_2 = Target->Constellation->GetName();
            aMyFunction::ReplaceTextToken(Text, u"<SectorEnemy>"_w, std::move(name_2), textHighlightColorTag_2.get());
        }
        {
            auto textHighlightColorTag_3 = pas::borrow(aMyFunction::TextHighlightColorTag);
            pas::WideString formatTurnDate = Galaxy->FormatTurnDate(Turn);
            aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, std::move(formatTurnDate), textHighlightColorTag_3.get());
        }
        Galaxy->AddPlanetNewsWithPlayerBubble(aGalaxyStruct::gnMilitaryBaseOperation, Text);
        return true;
    }

    // Searches active Galaxy; requires an assigned destination different from the current normal-space star.
    void* TGalaxy::FindMilitaryBaseInTransit() {
        std::int32_t I{};
        std::int32_t J{};
        TStar* Star{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (Ship->TypeId == aGalaxyStruct::rstMilitaryBase && pas::checked_cast<aRuins::TRuins*>(Ship)->FlyToStar != nullptr && (static_cast<std::uint8_t>(Ship->InNormalSpace() ^ 1) || static_cast<aRuins::TRuins*>(Ship)->FlyToStar != Ship->CurrentStar)) {
                    return Ship;
                }
            }
        }
        return nullptr;
    }

    // Searches active Galaxy; includes a base already at its assigned destination.
    std::uint8_t TGalaxy::HasMilitaryBaseAssignedToStar(TStar* Star) {
        std::int32_t I{};
        std::int32_t J{};
        TStar* SystemStar{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            SystemStar = pas::list_at<TStar>(Galaxy->Stars, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(SystemStar->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(SystemStar->Ships, J);
                if (Ship->TypeId == aGalaxyStruct::rstMilitaryBase && ([&] {
                    TStar* cpp_left = pas::checked_cast<aRuins::TRuins*>(Ship)->FlyToStar;
                    return cpp_left == pas::checked_cast<TStar*>(static_cast<pas::Object*>(Star));
                }())) {
                    return true;
                }
            }
        }
        return false;
    }

    // Tests the fourth order target of each liberation group, unlike TStar.HasLiberationGroupOrder.
    std::uint8_t TGalaxy::HasLiberationGroupTargetingStar(TStar* Star) {
        std::int32_t I{};
        aGroup::TGroup* Group{};
        {
            const std::int32_t cpp_first = pas::list_count(LiberationGroups) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Group = pas::list_at<aGroup::TGroup>(LiberationGroups, I);
                    if (pas::checked_cast<TStar*>(Group->Route[3].Target) == Star) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void TGalaxy::AssignSpecialStationService() {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t ShipCount{};
        TStar* Star{};
        aShip::TShip* Ship{};
        aRuins::TRuins* Station{};
        std::uint32_t Seed{};
        pas::Array<aRuins::TRuins*, 0, 2> Candidates{};
        if ((CurrentTurn & 15) != 0) {
            return;
        }
        if (NextSpecialStationServiceTurn == 0) {
            NextSpecialStationServiceTurn = aMyFunction::SeededRandomIntRange(0, aGalaxyStruct::TurnsPerYear, aPlayer::GetPlayer()->Id ^ GenerationSeed) + 2125;
        }
        if (CurrentTurn < NextSpecialStationServiceTurn) {
            return;
        }
        Seed = aPlayer::GetPlayer()->Id ^ GenerationSeed ^ Galaxy->CurrentTurn;
        {
            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(0, aGalaxyStruct::TurnsPerYear, Seed);
            NextSpecialStationServiceTurn = cpp_left + (NextSpecialStationServiceTurn + aGalaxyStruct::TurnsPerYear);
        }
        aRuins::TRuins* PirateActive = nullptr;
        aRuins::TRuins* PirateCandidate = nullptr;
        aRuins::TRuins* ScienceActive = nullptr;
        aRuins::TRuins* ScienceCandidate = nullptr;
        aRuins::TRuins* MilitaryActive = nullptr;
        aRuins::TRuins* MilitaryCandidate = nullptr;
        std::int32_t StarCount = pas::list_count(Stars);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(i); ) {
            Star = pas::list_at<TStar>(Stars, i);
            ShipCount = pas::list_count(Star->Ships);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ShipCount - 1); cpp_range_2.next(j); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, j);
                if (Ship->TypeId == aGalaxyStruct::rstPirateBase) {
                    Station = pas::checked_cast<aRuins::TRuins*>(Ship);
                    if (Station->SpecialServiceActive) {
                        PirateActive = Station;
                    } else if (Station->TypeNameOverrideKey == u"" && Station->ScriptShip == nullptr && Station->InNormalSpace() && (PirateCandidate == nullptr || aMyFunction::NextRandomIntRange(0, 100, Seed) < 50)) {
                        PirateCandidate = Station;
                    }
                } else if (Ship->TypeId == aGalaxyStruct::rstScienceBase) {
                    Station = pas::checked_cast<aRuins::TRuins*>(Ship);
                    if (Station->SpecialServiceActive) {
                        ScienceActive = Station;
                    } else if (Station->TypeNameOverrideKey == u"" && Station->ScriptShip == nullptr && Station->InNormalSpace() && (ScienceCandidate == nullptr || aMyFunction::NextRandomIntRange(0, 100, Seed) < 50)) {
                        ScienceCandidate = Station;
                    }
                } else if (Ship->TypeId == aGalaxyStruct::rstMilitaryBase) {
                    Station = pas::checked_cast<aRuins::TRuins*>(Ship);
                    if (Station->SpecialServiceActive) {
                        MilitaryActive = Station;
                    } else if (Station->TypeNameOverrideKey == u"" && Station->ScriptShip == nullptr && Station->InNormalSpace() && (MilitaryCandidate == nullptr || aMyFunction::NextRandomIntRange(0, 100, Seed) < 50)) {
                        MilitaryCandidate = Station;
                    }
                }
            }
        }
        std::int32_t Count = 0;
        if (PirateActive == nullptr && PirateCandidate != nullptr) {
            Candidates[Count] = PirateCandidate;
            ++Count;
        }
        if (ScienceActive == nullptr && ScienceCandidate != nullptr) {
            Candidates[Count] = ScienceCandidate;
            ++Count;
        }
        if (MilitaryActive == nullptr && MilitaryCandidate != nullptr) {
            Candidates[Count] = MilitaryCandidate;
            ++Count;
        }
        if (Count > 0) {
            Station = Candidates[aMyFunction::NextRandomIntRange(0, Count * 100 - 1, Seed) / 100];
            Station->SpecialServiceActive = true;
            AddPlanetNewsWithPlayerBubble(aGalaxyStruct::gnStationSpecialShip, ([&] {
                pas::WideString name = Station->GetName();
                auto name_2 = pas::borrow(Station->CurrentStar->Name);
                pas::WideString name_3 = Station->CurrentStar->Constellation->GetName();
                pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(pas::concat_wide({u"FormRuins.", Station->GetTypeNameKey(), u".SpecialShip.News"}), Galaxy->CurrentTurn / 10 * GenerationSeed);
                pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Name>"_w, std::move(name), u"<Star>"_w, name_2.get(), u"<Sector>"_w, std::move(name_3));
            }()));
        }
    }

    void TGalaxy::ApplyWingmanLeadershipPenalty() {
        std::int32_t I{};
        std::int32_t Excess{};
        aShip::TShip* Leader{};
        aShip::TShip* Ship{};
        if (WingmenPendingLeadershipPenalty != nullptr) {
            while (pas::list_count(WingmenPendingLeadershipPenalty) > 0) {
                Leader = pas::list_at<aShip::TShip>(WingmenPendingLeadershipPenalty, pas::list_count(WingmenPendingLeadershipPenalty) - 1)->PartnerShip;
                if (Leader == nullptr) {
                    pas::list_delete(WingmenPendingLeadershipPenalty, pas::list_count(WingmenPendingLeadershipPenalty) - 1);
                } else {
                    Excess = 0;
                    {
                        const std::int32_t cpp_first = pas::list_count(WingmenPendingLeadershipPenalty) - 1;
                        if (cpp_first >= 0) {
                            for (I = cpp_first; I >= 0; --I) {
                                if (pas::list_at<aShip::TShip>(WingmenPendingLeadershipPenalty, I)->PartnerShip == Leader) {
                                    ++Excess;
                                }
                            }
                        }
                    }
                    Excess -= Leader->GetEffectiveSkillLevel(aGalaxyStruct::psLeadership, false);
                    {
                        const std::int32_t cpp_first_2 = pas::list_count(WingmenPendingLeadershipPenalty) - 1;
                        if (cpp_first_2 >= 0) {
                            for (I = cpp_first_2; I >= 0; --I) {
                                if (pas::list_at<aShip::TShip>(WingmenPendingLeadershipPenalty, I)->PartnerShip == Leader) {
                                    if (Excess > 0) {
                                        Ship = pas::list_at<aShip::TShip>(WingmenPendingLeadershipPenalty, I);
                                        if (Ship->PartnershipDaysRemaining > 0) {
                                            Ship->PartnershipDaysRemaining = std::max<std::int32_t>(1, Ship->PartnershipDaysRemaining - (Excess + 1) / 2);
                                        }
                                    }
                                    pas::list_delete(WingmenPendingLeadershipPenalty, I);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Can complete pirate ending five, clear eminent ranger titles and publish defeat news.
    void TGalaxy::ProcessCoalitionDefeat() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        TStar* Star{};
        pas::WideString Text{};
        Globals::TMessagePlayer* Bubble{};
        std::uint8_t Contested{};
        float CoalitionStrength{};
        float PirateStrength{};
        if (aPlayer::GetPlayer() != nullptr && CountFactionStars(aGalaxyStruct::sfCoalition) <= 0 && aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate && PirateWinType != 3 && CoalitionDefeatedTurn == 0 && (aPlanet::MainPiratePlanet == nullptr || aPlayer::GetPlayer()->CurrentStar != aPlanet::MainPiratePlanet->CurrentStar)) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
                Star = pas::list_at<TStar>(Galaxy->Stars, I);
                Contested = false;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                    if (Ship->CurrentStanding == aGalaxyStruct::ssCoalitionMilitary) {
                        return;
                    }
                    if (!(pas::contains(aConst::PlanetOwnerMasks.Coalition, Ship->OwnerId) && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && Ship->OwnerId != aGalaxyStruct::oiPirate)) {
                        continue;
                    }
                    if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators || Star->Status.CustomFaction != u"") {
                        return;
                    }
                    if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                        Contested = true;
                        break;
                    }
                }
                if (Contested) {
                    CoalitionStrength = 0.0f;
                    PirateStrength = 0.0f;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                        if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                            CoalitionStrength = static_cast<long double>(CoalitionStrength) + Ship->Strength;
                        }
                        if (pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(Ship->CurrentStanding)) {
                            PirateStrength = static_cast<long double>(PirateStrength) + Ship->Strength;
                        }
                    }
                    if (3.0L * CoalitionStrength > PirateStrength) {
                        return;
                    }
                }
            }
            Bubble = Globals::FindPlayerBubbleByKey(u"BlazerWin"_wref.get(), false);
            if (Bubble != nullptr && Bubble->Kind == Globals::pmQuestActive) {
                Bubble->Kind = Globals::pmQuestSucceeded;
                Bubble->WasRead = false;
            }
            Bubble = Globals::FindPlayerBubbleByKey(u"TerronWin"_wref.get(), false);
            if (Bubble != nullptr && Bubble->Kind == Globals::pmQuestActive) {
                Bubble->Kind = Globals::pmQuestSucceeded;
                Bubble->WasRead = false;
            }
            Bubble = Globals::FindPlayerBubbleByKey(u"KellerWin"_wref.get(), false);
            if (Bubble != nullptr && Bubble->Kind == Globals::pmQuestActive) {
                Bubble->Kind = Globals::pmQuestSucceeded;
                Bubble->WasRead = false;
            }
            EminentCareerShips[aGalaxyStruct::rcTrader] = nullptr;
            EminentCareerShips[aGalaxyStruct::rcPirate] = nullptr;
            EminentCareerShips[aGalaxyStruct::rcWarrior] = nullptr;
            CoalitionDefeatedTurn = CurrentTurn;
            Galaxy->PirateWinTurn = Galaxy->CurrentTurn;
            Galaxy->PirateWinType = 5;
            Achievements::TryUnlockAchievement(u"PIRATEWIN"_w);
            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.CoalitionDefeated"_wref.get(), Galaxy->CurrentTurn / 23);
            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, CurrentTurn, Text, u""_wref.get())->NotificationSoundKind = 1;
            AddPlanetNews(aGalaxyStruct::gnCoalitionDefeated, Text);
        }
    }

    void TGalaxy::ComputeRangerSpawnQuotas() {
        aGalaxyStruct::TOwnerId Race{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        TStar* Star{};
        aPlanet::TPlanet* Planet{};
        aWarrior::TWarrior* Warrior{};
        pas::Array<aGalaxyStruct::TOwnerId, 0, 4> Order{};
        pas::Array<std::int32_t, 0, 4> Counts{};
        pas::Array<std::int32_t, 0, 4> Recruits{};
        pas::Array<std::int32_t, 0, 4> Sorted{};
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range.next(Race); ) {
            Counts[Race] = 0;
            Recruits[Race] = 0;
            RangerSpawnQuotas[Race] = 0;
            Order[Race] = Race;
        }
        std::int32_t Ratio = System::Round(pas::real_divide(18.0L, CustomRules.CoalitionAggression * 0.0625L + 0.5L));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Stars) - 1); cpp_range_2.next(I); ) {
            Star = pas::list_at<TStar>(Stars, I);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_3.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_4.next(K); ) {
                    Warrior = pas::list_at<aWarrior::TWarrior>(Planet->Warriors, K);
                    if (Warrior->TypeNameOverrideKey == u"") {
                        if (Warrior->WarriorType == aWarrior::wtFlagship) {
                            ++Recruits[Warrior->PilotRace];
                        } else {
                            ++Counts[Warrior->PilotRace];
                        }
                    }
                }
            }
        }
        std::int32_t Total = 0;
        for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range_5.next(Race); ) {
            Total += Counts[Race];
        }
        if (Total < 3) {
            return;
        }
        for (auto cpp_range_6 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range_6.next(Race); ) {
            I = pas::idiv(Counts[Race], Ratio);
            RangerSpawnQuotas[Race] = I - Recruits[Race];
            Counts[Race] -= I * Ratio;
            Total -= I * Ratio;
        }
        std::int32_t Extra = pas::idiv(Total, Ratio);
        if (Total - Extra * Ratio >= 3) {
            ++Extra;
        }
        for (auto cpp_range_7 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range_7.next(Race); ) {
            Sorted[Race] = Counts[Race];
        }
        for (I = 0; I <= 3; ++I) {
            const std::int32_t cpp_first = I + 1;
            if (cpp_first <= 4) {
                for (J = cpp_first; J <= 4; ++J) {
                    if (Sorted[I] < Sorted[J]) {
                        Race = Order[I];
                        K = Sorted[I];
                        Order[I] = Order[J];
                        Sorted[I] = Sorted[J];
                        Order[J] = Race;
                        Sorted[J] = K;
                    }
                }
            }
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, std::min<std::int32_t>(Extra - 0 - 1, 4)); cpp_range_8.next(I); ) {
            ++RangerSpawnQuotas[Order[I]];
        }
    }

    // Retains the most recent 1825 days.
    void TGalaxy::PruneExpiredGalaxyEvents() {
        while (pas::list_count(GalaxyEvents) > 0 && CurrentTurn - pas::list_at<aGalaxyEvent::TGalaxyEvent>(GalaxyEvents, 0)->Turn > 1825) {
            pas::free(pas::list_at<pas::Object>(GalaxyEvents, 0));
            pas::list_delete(GalaxyEvents, 0);
        }
    }

    // Uses the active Galaxy, not Self; denominator is max(pirate systems - 1, 1).
    float TGalaxy::GetCoalitionToPirateSystemRatio() {
        std::int32_t max = std::max<std::int32_t>(1, Galaxy->CountFactionStars(aGalaxyStruct::sfPirates) - 1);
        return pas::real_divide(Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition), max);
    }

    // With custom rules disabled, reads the active Galaxy difficulty array rather than Self.
    std::int32_t TGalaxy::GetEffectiveDifficultyLevel() {
        std::uint8_t I{};
        std::int32_t Result = 0;
        if (CustomRules.Enabled) {
            return CustomRules.DominatorStrength;
        }
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(7); ++I) {
            Result += Galaxy->DifficultyLevels[I];
        }
        return Result;
    }

    // Returns 0..9; tier boundaries are 6, 14, 22, and subsequent increments of eight.
    TDifficultyTier TGalaxy::GetDifficultyTierIndex() {
        std::int32_t Level = GetEffectiveDifficultyLevel();
        std::int32_t Bound = 6;
        std::int32_t Tier = 0;
        std::int32_t Step = 8;
        while (Level >= Bound) {
            Bound += Step;
            ++Tier;
        }
        return std::min<std::int32_t>(Tier, 9);
    }

    // A negative Level selects the effective difficulty; values above 24 extrapolate.
    float TGalaxy::InterpolateDifficulty(std::int32_t Level, float AtZero, float AtEight, float AtSixteen, float AtTwentyFour) {
        std::int32_t EffectiveLevel{};
        if (Level < 0) {
            EffectiveLevel = GetEffectiveDifficultyLevel();
        } else {
            EffectiveLevel = Level;
        }
        if (EffectiveLevel <= 0) {
            return AtZero;
        } else if (EffectiveLevel <= 8) {
            return aMyFunction::RemapClamped(EffectiveLevel, 0.0, 8.0, AtZero, AtEight);
        } else if (EffectiveLevel <= 16) {
            return aMyFunction::RemapClamped(EffectiveLevel, 8.0, 16.0, AtEight, AtSixteen);
        } else if (EffectiveLevel <= 24) {
            return aMyFunction::RemapClamped(EffectiveLevel, 16.0, 24.0, AtSixteen, AtTwentyFour);
        } else {
            return AtTwentyFour + pas::real_divide((static_cast<long double>(AtTwentyFour) - AtSixteen) * (EffectiveLevel - 24), 8.0L);
        }
    }

    // A negative Level selects the effective difficulty.
    float TGalaxy::ScaleDifficultyExponentially(std::int32_t Level, float BaseValue, float FactorPerEightLevels) {
        std::int32_t EffectiveLevel{};
        if (Level < 0) {
            EffectiveLevel = GetEffectiveDifficultyLevel();
        } else {
            EffectiveLevel = Level;
        }
        return BaseValue * System::Exp(System::Ln(FactorPerEightLevels) * EffectiveLevel * 0.125L);
    }

    float TGalaxy::GetDominatorBossHullScale() {
        return InterpolateDifficulty(-1, 0.8f, 1.0f, 1.2f, 1.5f);
    }

    float TGalaxy::GetDominatorKillExperienceScale() {
        return pas::real_divide(GetEffectiveDifficultyLevel() * 0.3L, 24.0L) + 0.9L;
    }

    std::int32_t TGalaxy::GetTurnsBetweenLiberationGroups() {
        if (CustomRules.Enabled) {
            return System::Round(pas::real_divide(InterpolateDifficulty(-1, 25.0f, 3.0E+1f, 5.0E+1f, 8.0E+1f), 0.5L + CustomRules.CoalitionAggression * 0.0625L));
        }
        return System::Round(InterpolateDifficulty(-1, 25.0f, 3.0E+1f, 5.0E+1f, 8.0E+1f));
    }

    std::int32_t TGalaxy::GetInitialDominatorControlPercent() {
        std::int32_t Level = GetEffectiveDifficultyLevel();
        return 45 + System::Round(Level * 1.25L);
    }

    std::int32_t TGalaxy::GetDominatorAggressionLevel() {
        if (CustomRules.Enabled) {
            return CustomRules.DominatorAggression;
        }
        return GetEffectiveDifficultyLevel();
    }

    std::int32_t TGalaxy::GetDominatorSpawnLevel() {
        if (CustomRules.Enabled) {
            return CustomRules.DominatorSpawn;
        }
        return GetEffectiveDifficultyLevel();
    }

    std::int32_t TGalaxy::GetPirateAggressionLevel() {
        if (CustomRules.Enabled) {
            return CustomRules.PirateAggression;
        }
        return 8 * Galaxy->DifficultyLevels[0];
    }

    // When enabled, seeded helpers ignore their supplied seed.
    std::uint8_t TGalaxy::IsChaoticRandomEnabled() {
        return CustomRules.Enabled && CustomRules.ChaoticRandom;
    }

    std::uint8_t TGalaxy::AreStationsNearStarsEnabled() {
        return CustomRules.Enabled && CustomRules.StationsNearStars;
    }

    std::uint8_t TGalaxy::IsFullStationTargetingEnabled() {
        return CustomRules.Enabled && CustomRules.FullStationTargeting;
    }

    std::uint8_t TGalaxy::IsEquipmentKnowledgeUnrestricted() {
        return CustomRules.Enabled && CustomRules.UnrestrictedEquipmentKnowledge;
    }

    float TGalaxy::GetAsteroidModifier() {
        float Result = 1.0f;
        if (CustomRules.Enabled) {
            return 0.5L + CustomRules.AsteroidModifier * 0.0625L;
        }
        return Result;
    }

    float TGalaxy::GetStarDamageDifficultyScale() {
        float Result = 1.0f;
        if (Galaxy->CustomRules.Enabled) {
            return CustomRules.SunDamageModifier * 0.0625L + 0.5L;
        }
        return Result;
    }

    std::uint8_t TGalaxy::AreSpecialShipsEnabled() {
        return CustomRules.Enabled && CustomRules.SpecialShips;
    }

    float TGalaxy::GetMicroModuleOfferRollThresholdPercent() {
        float Result = 3.0E+1f;
        if (CustomRules.Enabled) {
            return CustomRules.AcrynModifier;
        }
        return Result;
    }

    float TGalaxy::GetNodeDropModifier() {
        float Result = 1.0f;
        if (CustomRules.Enabled) {
            return 0.5L + CustomRules.NodeDropModifier * 0.0625L;
        }
        return Result;
    }

    float TGalaxy::GetArcadeDropValueModifier() {
        float Result = 1.0f;
        if (CustomRules.Enabled) {
            return 0.5L + CustomRules.ArcadeDropValueModifier * 0.0625L;
        }
        return Result;
    }

    float TGalaxy::GetDropValueModifier() {
        float Result = 1.0f;
        if (CustomRules.Enabled) {
            return CustomRules.DropValueModifier * 0.0625L + 0.5L;
        }
        return Result;
    }

    std::int32_t TGalaxy::GetAgriculturalPlanetWeight() {
        std::int32_t Result = 1;
        if (CustomRules.Enabled) {
            if (CustomRules.AgriculturalPlanetWeight + CustomRules.MixedPlanetWeight + CustomRules.IndustrialPlanetWeight == 0) {
                return 1;
            }
            return CustomRules.AgriculturalPlanetWeight;
        }
        return Result;
    }

    std::int32_t TGalaxy::GetMixedPlanetWeight() {
        std::int32_t Result = 1;
        if (CustomRules.Enabled) {
            if (CustomRules.AgriculturalPlanetWeight + CustomRules.MixedPlanetWeight + CustomRules.IndustrialPlanetWeight == 0) {
                return 1;
            }
            return CustomRules.MixedPlanetWeight;
        }
        return Result;
    }

    std::int32_t TGalaxy::GetIndustrialPlanetWeight() {
        std::int32_t Result = 1;
        if (CustomRules.Enabled) {
            if (CustomRules.AgriculturalPlanetWeight + CustomRules.MixedPlanetWeight + CustomRules.IndustrialPlanetWeight == 0) {
                return 1;
            }
            return CustomRules.IndustrialPlanetWeight;
        }
        return Result;
    }

    std::uint8_t TGalaxy::IsZeroStartingExperienceEnabled() {
        return CustomRules.Enabled && CustomRules.ZeroStartingExperience;
    }

    std::int32_t TGalaxy::GetExtraRangerCount() {
        std::int32_t Result = 0;
        if (CustomRules.Enabled) {
            return CustomRules.ExtraRangers;
        }
        return Result;
    }

    std::uint8_t TGalaxy::IsArcadeBattleRoyaleEnabled() {
        return CustomRules.Enabled && CustomRules.ArcadeBattleRoyale;
    }

    float TGalaxy::GetArcadeHitpointsModifier() {
        float Result = 1.0f;
        if (CustomRules.Enabled) {
            return CustomRules.ArcadeHitpointsModifier * 0.0625L + 0.5L;
        }
        return Result;
    }

    float TGalaxy::GetArcadeDamageModifier() {
        float Result = 1.0f;
        if (CustomRules.Enabled) {
            return CustomRules.ArcadeDamageModifier * 0.0625L + 0.5L;
        }
        return Result;
    }

    std::uint8_t TGalaxy::AreDominatorRacialWeaponsEnabled() {
        return CustomRules.Enabled && CustomRules.DominatorRacialWeapons;
    }

    std::int32_t TGalaxy::GetAIJunkToleranceLevel() {
        std::int32_t Result = 7;
        if (CustomRules.Enabled) {
            return CustomRules.AIJunkTolerance;
        }
        return Result;
    }

    std::uint8_t TGalaxy::AreMaxRangeMissilesEnabled() {
        return CustomRules.Enabled && CustomRules.MaxRangeMissiles;
    }

    std::uint8_t TGalaxy::IsOldHyperspaceEnabled() {
        return CustomRules.Enabled && CustomRules.OldHyperspace;
    }

    std::uint8_t TGalaxy::ArePirateNodesEnabled() {
        return CustomRules.Enabled && CustomRules.PirateNodes;
    }

    std::uint8_t TGalaxy::IsAIShoppingEnabled() {
        return CustomRules.Enabled && CustomRules.AIUseShops;
    }

    std::uint8_t TGalaxy::IsStationShopUpdateEnabled() {
        return CustomRules.Enabled && CustomRules.StationsUseShop;
    }

    std::uint8_t TGalaxy::AreDuplicateArtefactsEnabled() {
        return CustomRules.Enabled && CustomRules.DuplicateArtefacts;
    }

    std::uint8_t TGalaxy::GetHullGrowthMod() {
        if (CustomRules.Enabled) {
            return CustomRules.HullGrowth;
        }
        return 0;
    }

    std::uint8_t TGalaxy::IsArcadeEquipmentChangeEnabled() {
        return CustomRules.Enabled && CustomRules.ArcadeEquipmentChange;
    }

    std::uint8_t TGalaxy::IsOldSpeedCalculationEnabled() {
        return CustomRules.Enabled && CustomRules.OldSpeedCalculation;
    }

    std::uint8_t TGalaxy::AreOldMissileBonusesEnabled() {
        return CustomRules.Enabled && CustomRules.OldMissileBonuses;
    }

    // Changes the series only when exactly one series has eligible local forces; includes the bosses.
    void TStar::RefreshDominatorSeries() {
        pas::Extended Strength{};
        std::int32_t Blazers = CountDominatorForces(aGalaxyStruct::dsBlazer, false, false, Strength);
        std::int32_t Kellers = CountDominatorForces(aGalaxyStruct::dsKeller, false, false, Strength);
        std::int32_t Terrons = CountDominatorForces(aGalaxyStruct::dsTerron, false, false, Strength);
        if (aKling::BlazerShip != nullptr && aKling::BlazerShip->InNormalSpace() && aKling::BlazerShip->CurrentStar == this) {
            ++Blazers;
        }
        if (aKling::KellerShip != nullptr && aKling::KellerShip->InNormalSpace() && aKling::KellerShip->CurrentStar == this) {
            ++Kellers;
        }
        if (aKling::TerronShip != nullptr && aKling::TerronShip->InNormalSpace() && aKling::TerronShip->CurrentStar == this) {
            ++Terrons;
        }
        if (Blazers > 0 && Kellers == 0 && Terrons == 0) {
            Status.DominatorSeries = aGalaxyStruct::dsBlazer;
        }
        if (Blazers == 0 && Kellers > 0 && Terrons == 0) {
            Status.DominatorSeries = aGalaxyStruct::dsKeller;
        }
        if (Blazers == 0 && Kellers == 0 && Terrons > 0) {
            Status.DominatorSeries = aGalaxyStruct::dsTerron;
        }
    }

    void TStar::RefreshDerivedStats() {
        std::int32_t I{};
        std::int32_t Strength{};
        aShip::TShip* Ship{};
        Status.TrafficLevel = System::Round(aMyFunction::RemapClamped(pas::list_count(Ships), 3.0, 13.0, 0.0, 1.0E+2));
        if (pas::list_count(Ships) > 0) {
            Strength = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, I);
                Strength += Ship->GetStrengthScaledPirateStatus();
            }
            Status.ThreatLevel = System::Round(aMyFunction::RemapClamped(Strength, 0.0, 5.0E+2, 0.0, 1.0E+2));
        } else {
            Status.ThreatLevel = 0;
        }
        UpdateControlFaction();
        RefreshShipTypeCounts();
        RefreshDominatorSeries();
    }

    // Applies campaign-ending and liberation-contribution rules; not a raw ship-presence query.
    void TStar::GetControlPresence(std::uint8_t& PlayerPartyPresent, std::uint8_t& CoalitionPresent, std::uint8_t& DominatorsPresent, std::uint8_t& PiratesPresent, std::uint8_t& CustomPresent) {
        aShip::TShip* Ship{};
        std::uint8_t CoalitionLeaning{};
        std::uint8_t NeutralPresent{};
        std::uint8_t PirateLeaning{};
        std::int32_t Index{};
        std::int32_t DefenderIndex{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t CoalitionMilitaryPresent{};
        std::uint8_t PirateMilitaryPresent{};
        std::uint8_t PirateActivePresent{};
        std::uint8_t CoalitionActivePresent{};
        std::int32_t PirateKills{};
        std::int32_t CoalitionKills{};
        std::int32_t PirateLeaningKills{};
        std::int32_t CoalitionLeaningKills{};
        // Caller-popped static link; ship -4, Coalition output -8, faction flags -9..-11, other outputs +8..+16.
        auto AccumulateControlPresence = [&]() -> void {
            switch (Ship->CurrentStanding) {
                case aGalaxyStruct::ssDominator: DominatorsPresent = true; break;
                case aGalaxyStruct::ssCoalitionMilitary:
                case aGalaxyStruct::ssCoalitionActive: CoalitionPresent = true; break;
                case aGalaxyStruct::ssCoalitionPassive: CoalitionLeaning = true; break;
                case aGalaxyStruct::ssNeutral: NeutralPresent = true; break;
                case aGalaxyStruct::ssPiratePassive: PirateLeaning = true; break;
                case aGalaxyStruct::ssPirateActive:
                case aGalaxyStruct::ssPirateMilitary: PiratesPresent = true; break;
                case aGalaxyStruct::ssCustom: CustomPresent = true; break;
            }
        };
        CoalitionPresent = false;
        DominatorsPresent = false;
        PiratesPresent = false;
        CustomPresent = false;
        PlayerPartyPresent = false;
        PirateLeaning = false;
        NeutralPresent = false;
        CoalitionLeaning = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, Index);
            if (static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && (Ship->CurrentPlanet == nullptr || Ship->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited) && (Ship->DockedTo == nullptr || aPlayer::GetPlayer() == Ship) && (aPlayer::GetPlayer() != Ship || Ship->CurrentPlanet == nullptr && Ship->DockedTo == nullptr || Ship->ConsecutiveDockedDays <= 2 || GlobalsV::CurrentScreenId == GlobalsV::screenPlanetQuest)) {
                AccumulateControlPresence();
                if (aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->PartnerShip) {
                    PlayerPartyPresent = true;
                }
            }
        }
        if (static_cast<std::uint8_t>(CoalitionPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfCoalition) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_2.next(Index); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                if (Planet->IsCoalitionOwned && pas::list_count(Planet->Warriors) > 0) {
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_3.next(DefenderIndex); ) {
                        Ship = pas::list_at<aShip::TShip>(Planet->Warriors, DefenderIndex);
                        if (Ship->CurrentStar == this && static_cast<std::uint8_t>(Ship->InHyperspace ^ 1)) {
                            AccumulateControlPresence();
                        }
                    }
                }
            }
        }
        if (Status.ControlFaction == aGalaxyStruct::sfCoalition && static_cast<std::uint8_t>(CoalitionPresent ^ 1) && static_cast<std::uint8_t>(PiratesPresent ^ 1)) {
            if (NeutralPresent || CoalitionLeaning) {
                CoalitionPresent = true;
            } else if (DominatorsPresent && PirateLeaning) {
                CoalitionPresent = true;
            } else if (PirateLeaning) {
                PiratesPresent = true;
            }
        }
        if (Status.ControlFaction == aGalaxyStruct::sfPirates && static_cast<std::uint8_t>(CoalitionPresent ^ 1) && static_cast<std::uint8_t>(PiratesPresent ^ 1)) {
            if (NeutralPresent || PirateLeaning) {
                PiratesPresent = true;
            } else if (DominatorsPresent && CoalitionLeaning) {
                PiratesPresent = true;
            } else if (CoalitionLeaning) {
                CoalitionPresent = true;
            }
        }
        if (Status.ControlFaction == aGalaxyStruct::sfDominators || Status.CustomFaction != u"") {
            if (NeutralPresent) {
                CoalitionPresent = true;
            }
            if (PirateLeaning) {
                PiratesPresent = true;
            }
            if (CoalitionLeaning) {
                CoalitionPresent = true;
            }
        }
        if (Galaxy->CoalitionDefeatedTurn != 0 && Status.ControlFaction != aGalaxyStruct::sfCoalition) {
            PiratesPresent = PiratesPresent || CoalitionPresent;
            CoalitionPresent = false;
        }
        if (Galaxy->PirateWinType == 3 && Status.ControlFaction != aGalaxyStruct::sfPirates) {
            CoalitionPresent = PiratesPresent || CoalitionPresent;
            PiratesPresent = false;
        }
        if (static_cast<std::uint8_t>(DominatorsPresent ^ 1) && static_cast<std::uint8_t>(CoalitionPresent ^ 1) && static_cast<std::uint8_t>(PiratesPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfDominators && Status.CustomFaction == u"" && (Status.DominatorSeries == aGalaxyStruct::dsBlazer && Galaxy->BlazerSeriesResolvedTurn != 0 || Status.DominatorSeries == aGalaxyStruct::dsTerron && Galaxy->TerronSeriesResolvedTurn != 0)) {
            if (Galaxy->CoalitionDefeatedTurn == 0) {
                CoalitionPresent = true;
            } else {
                PiratesPresent = true;
            }
        }
        if (Status.ControlFaction == aGalaxyStruct::sfDominators && CoalitionPresent && PiratesPresent && static_cast<std::uint8_t>(DominatorsPresent ^ 1)) {
            CoalitionMilitaryPresent = false;
            PirateMilitaryPresent = false;
            PirateActivePresent = false;
            CoalitionActivePresent = false;
            PirateKills = 0;
            CoalitionKills = 0;
            PirateLeaningKills = 0;
            CoalitionLeaningKills = 0;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_4.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr && (aPlayer::GetPlayer() != Ship || Ship->CurrentPlanet == nullptr && Ship->DockedTo == nullptr || Ship->ConsecutiveDockedDays <= 2)) {
                    if (Ship->CurrentStanding == aGalaxyStruct::ssCoalitionMilitary) {
                        CoalitionMilitaryPresent = true;
                        CoalitionKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Dominator;
                    } else if (Ship->CurrentStanding == aGalaxyStruct::ssPirateMilitary) {
                        PirateMilitaryPresent = true;
                        PirateKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Dominator;
                    } else if (Ship->CurrentStanding != aGalaxyStruct::ssNeutral) {
                        if (pas::is_one_of<aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateActive>(Ship->CurrentStanding)) {
                            PirateKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Dominator;
                            if (Ship->CurrentStanding == aGalaxyStruct::ssPirateActive) {
                                PirateActivePresent = true;
                            } else {
                                PirateLeaningKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Dominator;
                            }
                        } else if (pas::is_one_of<aGalaxyStruct::ssCoalitionActive, aGalaxyStruct::ssCoalitionPassive>(Ship->CurrentStanding)) {
                            CoalitionKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Dominator;
                            if (Ship->CurrentStanding == aGalaxyStruct::ssCoalitionActive) {
                                CoalitionActivePresent = true;
                            } else {
                                CoalitionLeaningKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Dominator;
                            }
                        }
                    }
                }
            }
            if (CoalitionKills < PirateLeaningKills) {
                CoalitionPresent = false;
            } else if (PirateKills < CoalitionLeaningKills) {
                PiratesPresent = false;
            } else if (PirateMilitaryPresent && CoalitionMilitaryPresent) {
                return;
            } else if (PirateMilitaryPresent && CoalitionActivePresent) {
                return;
            } else if (CoalitionMilitaryPresent && PirateActivePresent) {
                return;
            } else if (CoalitionKills > PirateKills) {
                PiratesPresent = false;
            } else if (CoalitionKills < PirateKills) {
                CoalitionPresent = false;
            } else if (PlayerPartyPresent) {
                if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
                    CoalitionPresent = false;
                } else {
                    PiratesPresent = false;
                }
            } else {
                PiratesPresent = false;
            }
        }
    }

    // Can transfer planet ownership and emit capture news. Pirate ending 3 prevents new Pirate Clan captures.
    void TStar::UpdateControlFaction() {
        std::int32_t Index{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t CoalitionPresent{};
        std::uint8_t DominatorsPresent{};
        std::uint8_t PiratesPresent{};
        std::uint8_t CustomPresent{};
        std::uint8_t PlayerPartyPresent{};
        // Nested in UpdateControlFaction with unused caller-popped static link. Updates active Galaxy.WarDeltaWin; a losing streak below -1 is halved rather than incremented.
        auto RecordFactionVictory = [&](aGalaxyStruct::TStarFaction Faction) -> void {
            if (Galaxy->WarDeltaWin[Faction] >= -1) {
                ++Galaxy->WarDeltaWin[Faction];
            } else {
                Galaxy->WarDeltaWin[Faction] = Galaxy->WarDeltaWin[Faction] / 2;
            }
        };
        // Nested in UpdateControlFaction with unused caller-popped static link. Updates active Galaxy.WarDeltaWin; a winning streak above 1 is halved rather than decremented.
        auto RecordFactionDefeat = [&](aGalaxyStruct::TStarFaction Faction) -> void {
            if (Galaxy->WarDeltaWin[Faction] <= 1) {
                --Galaxy->WarDeltaWin[Faction];
            } else {
                Galaxy->WarDeltaWin[Faction] = Galaxy->WarDeltaWin[Faction] / 2;
            }
        };
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        PlayerPartyPresent = false;
        DominatorsPresent = false;
        std::uint8_t DominatorsCaptured = false;
        CoalitionPresent = false;
        std::uint8_t CoalitionCaptured = false;
        PiratesPresent = false;
        std::uint8_t PiratesCaptured = false;
        if (Constellation->Id == 20 && Galaxy->PirateWinType != 3 || Galaxy->KellerResearchTargetStarId == Id || NoComeKling || aScript::IsStarProtectedByScript(this)) {
            Status.Battle = 0;
            return;
        }
        GetControlPresence(PlayerPartyPresent, CoalitionPresent, DominatorsPresent, PiratesPresent, CustomPresent);
        if (static_cast<std::uint8_t>(Status.Battle) && CoalitionPresent && static_cast<std::uint8_t>(DominatorsPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfCoalition && Status.CustomFaction == u"" && Galaxy->CurrentTurn - 1 <= LastDominatorPresenceTurn && IsConstellationVisible() && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnDominatorAttackRepelled) < 2 && Galaxy->CoalitionDefeatedTurn == 0) {
            pas::WideString formatText1 = ([&] {
                auto name = pas::borrow(Name);
                pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Kling.Lost"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Star>"_w, name.get());
            }());
            TGalaxy* galaxy = Galaxy;
            galaxy->AddPlanetNews(aGalaxyStruct::gnDominatorAttackRepelled, std::move(formatText1));
        }
        if (static_cast<std::uint8_t>(Status.Battle) && CoalitionPresent && static_cast<std::uint8_t>(PiratesPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfCoalition && Status.CustomFaction == u"" && Galaxy->CurrentTurn - 1 <= LastPiratePresenceTurn && IsConstellationVisible() && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnPirateAttackRepelled) < 2 && Galaxy->CoalitionDefeatedTurn == 0) {
            pas::WideString formatText1_2 = ([&] {
                auto name_2 = pas::borrow(Name);
                pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Pirates.Lost"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                pas::WideString textHighlightColorTag_2 = aMyFunction::TextHighlightColorTag;
                return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_2), std::move(textHighlightColorTag_2), u"<Star>"_w, name_2.get());
            }());
            TGalaxy* galaxy_2 = Galaxy;
            galaxy_2->AddPlanetNews(aGalaxyStruct::gnPirateAttackRepelled, std::move(formatText1_2));
        }
        if (DominatorsPresent) {
            LastDominatorPresenceTurn = Galaxy->CurrentTurn;
        }
        if (PiratesPresent) {
            LastPiratePresenceTurn = Galaxy->CurrentTurn;
        }
        if (DominatorsPresent && CoalitionPresent) {
            if (IsConstellationVisible() && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnDominatorAttack) < 2 && Status.Battle == 0 && Status.ControlFaction == aGalaxyStruct::sfCoalition && Status.CustomFaction == u"" && Galaxy->CoalitionDefeatedTurn == 0) {
                pas::WideString formatText1_3 = ([&] {
                    auto name_3 = pas::borrow(Name);
                    pas::WideString pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Kling.Attack"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    pas::WideString textHighlightColorTag_3 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_3), std::move(textHighlightColorTag_3), u"<Star>"_w, name_3.get());
                }());
                TGalaxy* galaxy_3 = Galaxy;
                galaxy_3->AddPlanetNews(aGalaxyStruct::gnDominatorAttack, std::move(formatText1_3));
            }
            Status.Battle = 1;
            return;
        }
        if (PiratesPresent && CoalitionPresent) {
            if (IsConstellationVisible() && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnPirateAttack) < 2 && Status.Battle == 0 && Status.ControlFaction == aGalaxyStruct::sfCoalition && Status.CustomFaction == u"" && Galaxy->CoalitionDefeatedTurn == 0) {
                pas::WideString formatText1_4 = ([&] {
                    auto name_4 = pas::borrow(Name);
                    pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Pirates.Attack"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    pas::WideString textHighlightColorTag_4 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_4), std::move(textHighlightColorTag_4), u"<Star>"_w, name_4.get());
                }());
                TGalaxy* galaxy_4 = Galaxy;
                galaxy_4->AddPlanetNews(aGalaxyStruct::gnPirateAttack, std::move(formatText1_4));
            }
            Status.Battle = 1;
            return;
        }
        if (DominatorsPresent && PiratesPresent) {
            Status.Battle = 1;
            return;
        }
        if (CustomPresent && Status.CustomFaction == u"") {
            Status.Battle = 1;
            return;
        }
        if (Status.CustomFaction != u"") {
            Status.Battle = CoalitionPresent || PiratesPresent || DominatorsPresent;
            return;
        }
        if (static_cast<std::uint8_t>(DominatorsPresent ^ 1) && static_cast<std::uint8_t>(PiratesPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfCoalition && Status.Battle != 0) {
            if (PlayerPartyPresent && aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                ++aPlayer::GetPlayer()->AchievementStats->SystemsDefended;
                Achievements::TrySetAchievementProgress(u"DEFENDER"_w, aPlayer::GetPlayer()->AchievementStats->SystemsDefended);
            }
            Status.Battle = 0;
            return;
        }
        if (static_cast<std::uint8_t>(CoalitionPresent ^ 1) && static_cast<std::uint8_t>(PiratesPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfDominators && Status.Battle != 0) {
            Status.Battle = 0;
            return;
        }
        if (static_cast<std::uint8_t>(CoalitionPresent ^ 1) && static_cast<std::uint8_t>(DominatorsPresent ^ 1) && Status.ControlFaction == aGalaxyStruct::sfPirates && Status.Battle != 0) {
            if (PlayerPartyPresent && aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
                ++aPlayer::GetPlayer()->AchievementStats->SystemsDefended;
                Achievements::TrySetAchievementProgress(u"DEFENDER"_w, aPlayer::GetPlayer()->AchievementStats->SystemsDefended);
            }
            Status.Battle = 0;
            return;
        }
        if ((DominatorsPresent || PiratesPresent) && Status.ControlFaction == aGalaxyStruct::sfCoalition) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(Index); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                if (Planet->IsCoalitionOwned && aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * Planet->GenerationSeed * 2311) < 20) {
                    // Native tests the quotient, not the remainder: preserve the early-turn behavior.
                    if (Galaxy->CurrentTurn / 5 == 0) {
                        Planet->ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{6}}));
                    }
                    if (Galaxy->CurrentTurn / 7 == 0) {
                        Planet->ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{1}}));
                    }
                    if (Galaxy->CurrentTurn / 3 == 0) {
                        Planet->ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}}));
                    }
                    if (Galaxy->CurrentTurn / 5 == 0) {
                        Planet->ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{3}}));
                    }
                    if (Galaxy->CurrentTurn / 3 == 0) {
                        Planet->ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{5}}));
                    }
                    if (Galaxy->CurrentTurn / 11 == 0) {
                        Planet->ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{7}}));
                    }
                }
            }
        }
        if (DominatorsPresent && Status.ControlFaction != aGalaxyStruct::sfDominators) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_2.next(Index); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                    Planet->OwnerId = aGalaxyStruct::oiDominator;
                    Planet->UpdateOwnerFlags();
                    DominatorsCaptured = true;
                }
            }
            if (DominatorsCaptured) {
                if (Galaxy->CoalitionDefeatedTurn == 0) {
                    if (Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                        pas::WideString formatText2 = ([&] {
                            auto name_5 = pas::borrow(Name);
                            pas::WideString name_6 = Constellation->GetName();
                            pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.KlingTakeSystemFromNormals"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                            pas::WideString textHighlightColorTag_5 = aMyFunction::TextHighlightColorTag;
                            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_5), std::move(textHighlightColorTag_5), u"<Star>"_w, name_5.get(), u"<Sector>"_w, std::move(name_6));
                        }());
                        TGalaxy* galaxy_5 = Galaxy;
                        galaxy_5->AddPlanetNewsWithPlayerBubble(aGalaxyStruct::gnDominatorsTakeCoalitionSystem, std::move(formatText2));
                    } else if (Galaxy->CoalitionDefeatedTurn == 0) {
                        pas::WideString formatText2_2 = ([&] {
                            auto name_7 = pas::borrow(Name);
                            pas::WideString name_8 = Constellation->GetName();
                            pas::WideString pickLocalizedTextVariant_6 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.KlingTakeSystemFromPirateClan"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                            pas::WideString textHighlightColorTag_6 = aMyFunction::TextHighlightColorTag;
                            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_6), std::move(textHighlightColorTag_6), u"<Star>"_w, name_7.get(), u"<Sector>"_w, std::move(name_8));
                        }());
                        TGalaxy* galaxy_6 = Galaxy;
                        galaxy_6->AddPlanetNewsWithPlayerBubble(aGalaxyStruct::gnDominatorsTakePirateSystem, std::move(formatText2_2));
                    } else {
                        // Retained native branch, despite the outer zero test.
                        const pas::WideString& formatText2_3 = ([&] {
                            auto name_9 = pas::borrow(Name);
                            pas::WideString name_10 = Constellation->GetName();
                            pas::WideString pickLocalizedTextVariant_7 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.KlingTakeSystemFromPirateClanAlt"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                            pas::WideString textHighlightColorTag_7 = aMyFunction::TextHighlightColorTag;
                            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_7), std::move(textHighlightColorTag_7), u"<Star>"_w, name_9.get(), u"<Sector>"_w, std::move(name_10));
                        }());
                        std::int32_t currentTurn = Galaxy->CurrentTurn;
                        Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, currentTurn, formatText2_3, u""_wref.get());
                    }
                }
                RecordFactionDefeat(Status.ControlFaction);
                Status.PreviousControlFaction = Status.ControlFaction;
                Status.ControlFaction = aGalaxyStruct::sfDominators;
                Status.Battle = 0;
                RecordFactionVictory(aGalaxyStruct::sfDominators);
            }
        } else if (PiratesPresent && Galaxy->PirateWinType != 3 && Status.ControlFaction != aGalaxyStruct::sfPirates) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_3.next(Index); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                    pas::list_clear(Planet->Warriors);
                    Planet->OwnerId = aGalaxyStruct::oiPirate;
                    Planet->Government = aGalaxyStruct::pgAnarchy;
                    Planet->UpdateOwnerFlags();
                    PiratesCaptured = true;
                }
            }
            if (PiratesCaptured) {
                RecordFactionDefeat(Status.ControlFaction);
                Status.PreviousControlFaction = Status.ControlFaction;
                Status.ControlFaction = aGalaxyStruct::sfPirates;
                Status.Battle = 0;
                if (aPlanet::MainPiratePlanet == nullptr || aPlanet::MainPiratePlanet->CurrentStar != this) {
                    LiberationRewardsPending = true;
                }
                RecordFactionVictory(aGalaxyStruct::sfPirates);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_4.next(Index); ) {
                    if (pas::class_cast_if<aPirate::TPirate*>(pas::list_at<pas::Object>(Ships, Index)) != nullptr && static_cast<std::uint8_t>(pas::list_at<aShip::TShip>(Ships, Index)->HasScriptControl() ^ 1)) {
                        pas::list_at<aPirate::TPirate>(Ships, Index)->PrisonTermRemaining = 0u;
                    }
                }
                if (aPlayer::GetPlayer() != nullptr) {
                    static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckAllPirateSystemsAchievement();
                }
                if (aKling::PieceCreatorTargetStarId == Id && aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryUnlockAchievement(u"PIECECREATOR"_w);
                }
            }
        } else if (CoalitionPresent && Galaxy->CoalitionDefeatedTurn == 0 && Status.ControlFaction != aGalaxyStruct::sfCoalition) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_5.next(Index); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                    if (Planet->OwnerId == aGalaxyStruct::oiPirate) {
                        Planet->Government = static_cast<aGalaxyStruct::TPlanetGovernment>(aMyFunction::SeededRandomIntRange(0, 4, Planet->RandomState));
                    }
                    Planet->OwnerId = aConst::RaceToOwner(Planet->RaceId);
                    Planet->UpdateOwnerFlags();
                    Planet->InventionLevels[aGalaxyStruct::piMainTech] = std::max<std::int32_t>(static_cast<std::int32_t>(Planet->InventionLevels[aGalaxyStruct::piMainTech]), Galaxy->TechLevel - 2);
                    CoalitionCaptured = true;
                }
            }
            if (CoalitionCaptured) {
                RecordFactionDefeat(Status.ControlFaction);
                Status.PreviousControlFaction = Status.ControlFaction;
                Status.ControlFaction = aGalaxyStruct::sfCoalition;
                Status.Battle = 0;
                if (aPlanet::MainPiratePlanet == nullptr || aPlanet::MainPiratePlanet->CurrentStar != this) {
                    LiberationRewardsPending = true;
                }
                RecordFactionVictory(aGalaxyStruct::sfCoalition);
                for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_6.next(Index); ) {
                    if (pas::class_cast_if<aRanger::TRanger*>(pas::list_at<pas::Object>(Ships, Index)) != nullptr && static_cast<std::uint8_t>(pas::list_at<aShip::TShip>(Ships, Index)->HasScriptControl() ^ 1) && pas::list_at<aShip::TShip>(Ships, Index) != aPlayer::GetPlayer()) {
                        pas::list_at<aRanger::TRanger>(Ships, Index)->PrisonTermRemaining = 0;
                    }
                }
                if (aKling::PieceCreatorTargetStarId == Id && aPlayer::GetPlayer() != nullptr) {
                    Achievements::TryUnlockAchievement(u"PIECECREATOR"_w);
                }
            }
        }
    }

    // Reevaluates ownership after clearing a custom faction, resets liberation contributions and can clear NPC prison terms. No-op without a player.
    void TStar::ResetControlFaction() {
        aShip::TShip* Ship{};
        std::int32_t DominatorCount{};
        std::int32_t CoalitionCount{};
        std::int32_t CoalitionPassiveCount{};
        std::int32_t NeutralCount{};
        std::int32_t PiratePassiveCount{};
        std::int32_t PirateCount{};
        aPlanet::TPlanet* Planet{};
        std::int32_t I{};
        pas::Array<std::int32_t, 0, 2> SeriesCounts{};
        // Caller-popped static link; ship -4, standing counters -8..-28.
        auto CountShipStanding = [&]() -> void {
            switch (Ship->CurrentStanding) {
                case aGalaxyStruct::ssDominator: ++DominatorCount; break;
                case aGalaxyStruct::ssCoalitionMilitary:
                case aGalaxyStruct::ssCoalitionActive: ++CoalitionCount; break;
                case aGalaxyStruct::ssCoalitionPassive: ++CoalitionPassiveCount; break;
                case aGalaxyStruct::ssNeutral: ++NeutralCount; break;
                case aGalaxyStruct::ssPiratePassive: ++PiratePassiveCount; break;
                case aGalaxyStruct::ssPirateActive:
                case aGalaxyStruct::ssPirateMilitary: ++PirateCount; break;
            }
        };
        // Caller-popped static link; star -32. Also transfers planets and clears eligible NPC ranger prison terms.
        auto SetCoalition = [&]() -> void {
            std::int32_t J{};
            this->Status.ControlFaction = aGalaxyStruct::sfCoalition;
            this->Status.PreviousControlFaction = aGalaxyStruct::sfCoalition;
            this->Status.Battle = PirateCount + DominatorCount > 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->Planets) - 1); cpp_range.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(this->Planets, J);
                if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                    Planet->OwnerId = aConst::RaceToOwner(Planet->RaceId);
                    Planet->UpdateOwnerFlags();
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(this->Ships, J);
                if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Ship); ranger != nullptr && static_cast<std::uint8_t>(Ship->HasScriptControl() ^ 1) && aPlayer::GetPlayer() != Ship) {
                    ranger->PrisonTermRemaining = 0;
                }
            }
        };
        // Caller-popped static link; star -32. Also transfers planets and clears eligible NPC pirate prison terms.
        auto SetPirates = [&]() -> void {
            std::int32_t J{};
            this->Status.ControlFaction = aGalaxyStruct::sfPirates;
            this->Status.PreviousControlFaction = aGalaxyStruct::sfPirates;
            this->Status.Battle = CoalitionCount + DominatorCount > 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(this->Planets) - 1); cpp_range.next(J); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(this->Planets, J);
                if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                    Planet->OwnerId = aGalaxyStruct::oiPirate;
                    Planet->UpdateOwnerFlags();
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(this->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(this->Ships, J);
                if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship); pirate != nullptr && static_cast<std::uint8_t>(Ship->HasScriptControl() ^ 1)) {
                    pirate->PrisonTermRemaining = 0u;
                }
            }
        };
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        DominatorCount = 0;
        CoalitionCount = 0;
        PirateCount = 0;
        CoalitionPassiveCount = 0;
        PiratePassiveCount = 0;
        NeutralCount = 0;
        SeriesCounts[aGalaxyStruct::dsBlazer] = 0;
        SeriesCounts[aGalaxyStruct::dsTerron] = 0;
        SeriesCounts[aGalaxyStruct::dsKeller] = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && (Ship->CurrentPlanet == nullptr || Ship->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited) && (Ship->DockedTo == nullptr || aPlayer::GetPlayer() == Ship) && (aPlayer::GetPlayer() != Ship || Ship->CurrentPlanet == nullptr && Ship->DockedTo == nullptr || Ship->ConsecutiveDockedDays <= 2 || GlobalsV::CurrentScreenId == GlobalsV::screenPlanetQuest)) {
                CountShipStanding();
                if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship); kling != nullptr && Ship->CurrentStanding == aGalaxyStruct::ssDominator) {
                    aKling::TKling* cpp_with = kling;
                    ++SeriesCounts[cpp_with->DominatorSeries];
                    if (cpp_with->KlingType == aGalaxyStruct::ktBertor) {
                        SeriesCounts[cpp_with->DominatorSeries] += 3;
                    }
                    if (cpp_with->KlingType == aGalaxyStruct::ktBoss) {
                        SeriesCounts[cpp_with->DominatorSeries] += 10;
                    }
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(Ship)) {
                aNormalShip::TNormalShip* cpp_with_2 = normalShip;
                cpp_with_2->CurrentSystemKills.Normal = 0;
                cpp_with_2->CurrentSystemKills.Pirate = 0;
                cpp_with_2->CurrentSystemKills.Dominator = 0;
                cpp_with_2->CurrentSystemKills.Custom = 0;
                cpp_with_2->PendingLiberationCeremonyPlanet = nullptr;
                cpp_with_2->PendingLiberationContribution = 0;
            }
        }
        if (DominatorCount > CoalitionCount + PirateCount + CoalitionPassiveCount + PiratePassiveCount + NeutralCount) {
            Status.ControlFaction = aGalaxyStruct::sfDominators;
            Status.PreviousControlFaction = aGalaxyStruct::sfDominators;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_3.next(I); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
                if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                    Planet->OwnerId = aGalaxyStruct::oiDominator;
                    Planet->UpdateOwnerFlags();
                }
            }
            if (SeriesCounts[aGalaxyStruct::dsBlazer] >= std::max<std::int32_t>(SeriesCounts[aGalaxyStruct::dsTerron], SeriesCounts[aGalaxyStruct::dsKeller])) {
                Status.DominatorSeries = aGalaxyStruct::dsBlazer;
            } else if (SeriesCounts[aGalaxyStruct::dsTerron] >= std::max<std::int32_t>(SeriesCounts[aGalaxyStruct::dsBlazer], SeriesCounts[aGalaxyStruct::dsKeller])) {
                Status.DominatorSeries = aGalaxyStruct::dsTerron;
            } else {
                Status.DominatorSeries = aGalaxyStruct::dsKeller;
            }
            Status.Battle = CoalitionCount + PirateCount + CoalitionPassiveCount + PiratePassiveCount + NeutralCount > 0;
        } else if (Galaxy->CoalitionDefeatedTurn > 0) {
            SetPirates();
        } else if (Galaxy->PirateWinType == 3) {
            SetCoalition();
        } else if (CoalitionCount > PirateCount) {
            SetCoalition();
        } else if (PirateCount > CoalitionCount) {
            SetPirates();
        } else if (CoalitionPassiveCount > PiratePassiveCount) {
            SetCoalition();
        } else if (PiratePassiveCount > CoalitionPassiveCount) {
            SetPirates();
        } else {
            SetCoalition();
        }
    }

    void TStar::RefreshMapDiameterAndStats() {
        MapDiameter = ComputeMapDiameter();
        RefreshDerivedStats();
    }

    // 200 steps in the player's star, 50 elsewhere; MovementStepScale is the reciprocal.
    void TStar::RefreshMovementStepParameters() {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == this) {
            MovementStepCount = aGalaxyStruct::BaseMovementStepsPerTurn;
            MovementStepScale = pas::constant(1.0L / pas::constant(static_cast<long double>(aGalaxyStruct::BaseMovementStepsPerTurn)));
        } else {
            MovementStepCount = 50;
            MovementStepScale = 0.02L;
        }
    }

    // Uses the final planet-list entry when nonempty; otherwise SystemRadius. Does not update MapDiameter.
    std::int32_t TStar::ComputeMapDiameter() {
        aPlanet::TPlanet* Planet{};
        if (pas::list_count(Planets) > 0) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, pas::list_count(Planets) - 1);
            return System::Round(static_cast<long double>(Planet->Orbit.Radius) + Planet->Radius + 8.0E+2L) * 2;
        }
        return (SystemRadius + 800) * 2;
    }

    std::int32_t TStar::CountPlanetsByOwner(aGalaxyStruct::TOwnerId OwnerId) {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            if (Planet->OwnerId == OwnerId) {
                ++Result;
            }
        }
        return Result;
    }

    // Counts owners 0..5 and 7; includes Dominators.
    std::int32_t TStar::CountDistinctInhabitedPlanetOwners() {
        aGalaxyStruct::TOwnerId OwnerId{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiDominator); cpp_range.next(OwnerId); ) {
            if (CountPlanetsByOwner(OwnerId) > 0) {
                ++Result;
            }
        }
        if (CountPlanetsByOwner(aGalaxyStruct::oiPirate) > 0) {
            ++Result;
        }
        return Result;
    }

    // If every planet is uninhabited, returns the last planet; nil only for an empty list.
    void* TStar::FindFirstInhabitedPlanet() {
        std::int32_t Index{};
        void* Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(Index); ) {
            Result = pas::list_get(Planets, Index);
            if (pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Result))->OwnerId != aGalaxyStruct::oiUninhabited) {
                break;
            }
        }
        return Result;
    }

    // Returns nil when no inhabited planet exists; advances the star RNG.
    void* TStar::SelectRandomInhabitedPlanet() {
        std::int32_t I{};
        void* Result = nullptr;
        std::int32_t Remaining = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            if (pas::list_at<aPlanet::TPlanet>(Planets, I)->OwnerId != aGalaxyStruct::oiUninhabited) {
                ++Remaining;
            }
        }
        Remaining = aMyFunction::NextRandomIntRange(1, Remaining, RandomState);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_2.next(I); ) {
            if (pas::list_at<aPlanet::TPlanet>(Planets, I)->OwnerId != aGalaxyStruct::oiUninhabited) {
                --Remaining;
                if (Remaining == 0) {
                    return pas::list_get(Planets, I);
                }
            }
        }
        return Result;
    }

    // Excludes uninhabited planets; first entry wins equal progress rates.
    void* TStar::FindFastestResearchPlanet() {
        std::int32_t I{};
        float Rate{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TPlanet* Best = nullptr;
        float BestRate = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                Rate = Planet->CalculateInventionProgressRate();
                if (Rate > BestRate || Best == nullptr) {
                    BestRate = Rate;
                    Best = Planet;
                }
            }
        }
        return Best;
    }

    void TStar::RefreshShipTypeCounts() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        aGalaxyStruct::TShipType Kind{};
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range.next(Kind); ) {
            ShipTypeCounts[Kind] = 0;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_2.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Dominion == Ship || Ship->InNormalSpace() || pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Ship->InHyperspace) {
                ++ShipTypeCounts[Ship->TypeId];
            }
        }
    }

    std::int32_t TStar::CountEligibleRangersInSpace() {
        std::int32_t Index{};
        aShip::TShip* Ship{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, Index);
            if (static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr && static_cast<std::uint8_t>(static_cast<aRanger::TRanger*>(Ship)->ExcludedFromRating ^ 1)) {
                ++Result;
            }
        }
        return Result;
    }

    // Uses cached population counts.
    std::int32_t TStar::CountShipsByTypeMask(aGalaxyStruct::TShipTypeMask ShipTypeMask) {
        aGalaxyStruct::TShipType I{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation); cpp_range.next(I); ) {
            if (pas::contains(ShipTypeMask, I)) {
                Count += ShipTypeCounts[I];
            }
        }
        return Count;
    }

    // Excludes bosses, scripted-standing ships and Dominators with a positive ActiveProgramAppliedTurn. Strength is an Extended output.
    std::int32_t TStar::CountDominatorForces(aGalaxyStruct::TDominatorSeries Series, std::uint8_t ExcludeAbsoluteOrders, std::uint8_t OtherSeries, pas::Extended& Strength) {
        std::int32_t Result{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::int32_t OtherCount = 0;
        std::int32_t MatchCount = 0;
        float OtherStrength = 0.0f;
        float MatchStrength = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship->IsOutsideStarSpace()) {
                continue;
            }
            if (ExcludeAbsoluteOrders && Ship->OrderAbsolute) {
                continue;
            }
            if (aKling::BlazerShip == Ship || aKling::KellerShip == Ship || aKling::TerronShip == Ship) {
                continue;
            }
            if (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr)) {
                continue;
            }
            if (static_cast<aKling::TKling*>(Ship)->ActiveProgramAppliedTurn > 0) {
                continue;
            }
            if (Ship->CurrentStanding == aGalaxyStruct::ssCustom) {
                continue;
            }
            if (static_cast<aKling::TKling*>(Ship)->DominatorSeries == Series) {
                ++MatchCount;
                MatchStrength = static_cast<long double>(MatchStrength) + Ship->Strength;
            } else {
                ++OtherCount;
                OtherStrength = static_cast<long double>(OtherStrength) + Ship->Strength;
            }
        }
        if (OtherSeries) {
            Result = OtherCount;
            Strength = OtherStrength;
        } else {
            Result = MatchCount;
            Strength = MatchStrength;
        }
        return Result;
    }

    // Counts normal-space types 1..5; excludes bosses and scripted standing.
    std::int32_t TStar::CountStandardDominatorsOfLocalSeries() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && Ship->CurrentStanding != aGalaxyStruct::ssCustom && Ship != aKling::BlazerShip && Ship != aKling::KellerShip && Ship != aKling::TerronShip && pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && static_cast<aKling::TKling*>(Ship)->DominatorSeries == Status.DominatorSeries && pas::in_range(static_cast<aKling::TKling*>(Ship)->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktEquantor), static_cast<std::int32_t>(aGalaxyStruct::ktShtip))) {
                ++Result;
            }
        }
        return Result;
    }

    // Requires pirate ownership. IncludeIndependent also includes the pirate player.
    std::int32_t TStar::CountPirateForces(std::uint8_t ExcludeAbsoluteOrders, pas::Extended& Strength, std::uint8_t IncludeClanVariants, std::uint8_t IncludeIndependent) {
        std::int32_t Index{};
        aShip::TShip* Ship{};
        std::int32_t Count = 0;
        Strength = 0.0L;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, Index);
            if (Ship->IsOutsideStarSpace() || Ship->CurrentStanding == aGalaxyStruct::ssCustom || ExcludeAbsoluteOrders && Ship->OrderAbsolute) {
                continue;
            }
            if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship); pirate != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate) {
                if (static_cast<std::uint8_t>(IncludeClanVariants ^ 1) && pirate->PirateType != 0 || static_cast<std::uint8_t>(IncludeIndependent ^ 1) && pirate->PirateType == 0) {
                    continue;
                }
                ++Count;
                Strength = Strength + Ship->Strength;
            }
            if (aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate && IncludeIndependent) {
                ++Count;
                Strength = Strength + Ship->Strength;
            }
        }
        return Count;
    }

    std::int32_t TStar::CountCustomFactionForces(std::uint8_t ExcludeAbsoluteOrders, pas::WideString& Faction, pas::Extended& Strength) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        Faction = Status.CustomFaction;
        std::int32_t Result = 0;
        Strength = 0.0L;
        if (Faction != u"") {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, I);
                if (Ship->IsOutsideStarSpace()) {
                    continue;
                }
                if (Ship->CurrentStanding != aGalaxyStruct::ssCustom) {
                    continue;
                }
                if (Ship->ScriptShip == nullptr) {
                    continue;
                }
                if (reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText != Faction) {
                    continue;
                }
                if (ExcludeAbsoluteOrders && Ship->OrderAbsolute) {
                    continue;
                }
                ++Result;
                Strength = Strength + Ship->Strength;
            }
        }
        return Result;
    }

    // Faction is empty when counted ships do not share one nonempty faction tag.
    std::int32_t TStar::CountOtherCustomFactionForces(std::uint8_t ExcludeAbsoluteOrders, pas::WideString& Faction, pas::Extended& Strength) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        pas::WideString ShipFaction{};
        pas::WideString OwnFaction{};
        std::int32_t Count = 0;
        Strength = 0.0L;
        OwnFaction = Status.CustomFaction;
        Faction = pas::WideString();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship->IsOutsideStarSpace()) {
                continue;
            }
            if (Ship->CurrentStanding != aGalaxyStruct::ssCustom) {
                continue;
            }
            if (ExcludeAbsoluteOrders && Ship->OrderAbsolute) {
                continue;
            }
            if (Ship->ScriptShip == nullptr || reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText == u"") {
                Faction = pas::WideString();
            } else {
                ShipFaction = reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText;
                if (ShipFaction == OwnFaction) {
                    continue;
                }
                if (Count == 0) {
                    Faction = ShipFaction;
                } else if (Faction != ShipFaction) {
                    Faction = pas::WideString();
                }
            }
            ++Count;
            Strength = Strength + Ship->Strength;
        }
        return Count;
    }

    // Includes the pirate player; excludes scripted standing.
    std::int32_t TStar::CountPirateShips(std::uint8_t IncludeOutsideStarSpace) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if ((static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) || IncludeOutsideStarSpace != false) && Ship->CurrentStanding != aGalaxyStruct::ssCustom) {
                if (pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate) {
                    ++Count;
                }
                if (Ship == aPlayer::GetPlayer() && aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
                    ++Count;
                }
            }
        }
        return Count;
    }

    // Includes local garrison ships absent from Ships, avoiding duplicate list entries.
    std::int32_t TStar::CountForcesByOwnerGroups(pas::Extended& Strength, std::uint8_t IncludeCoalition, std::uint8_t IncludeDominators, std::uint8_t IncludePirates, std::uint8_t IncludeCustom) {
        aShip::TShip* Ship{};
        std::int32_t Count{};
        std::int32_t I{};
        std::int32_t J{};
        aPlanet::TPlanet* Planet{};
        auto AccumulateFactionForces = [&]() -> void {
            if (Ship->InHyperspace) {
                return;
            }
            if (Ship->CurrentPlanet != nullptr && Ship->CurrentPlanet->OwnerId == aGalaxyStruct::oiUninhabited) {
                return;
            }
            if (Ship->CurrentStanding == aGalaxyStruct::ssCustom) {
                if (IncludeCustom) {
                    Strength = Strength + Ship->Strength;
                    ++Count;
                }
                return;
            }
            if (static_cast<std::uint8_t>(IncludeCoalition ^ 1) && pas::contains(aConst::PlanetOwnerMasks.Coalition, Ship->OwnerId)) {
                return;
            }
            if (static_cast<std::uint8_t>(IncludeDominators ^ 1) && pas::contains(aConst::PlanetOwnerMasks.Dominators, Ship->OwnerId)) {
                return;
            }
            if (static_cast<std::uint8_t>(IncludePirates ^ 1) && pas::contains(aConst::PlanetOwnerMasks.PirateClan, Ship->OwnerId)) {
                return;
            }
            if (aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->IsOutsideStarSpace()) {
                return;
            }
            Strength = Strength + Ship->Strength;
            ++Count;
        };
        Count = 0;
        Strength = 0.0L;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            AccumulateFactionForces();
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_2.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Planets, I);
            if (Planet->Warriors != nullptr) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_3.next(J); ) {
                    Ship = pas::list_at<aShip::TShip>(Planet->Warriors, J);
                    if (Ship->CurrentStar == this && pas::list_indexof(Ships, reinterpret_cast<void*>(Ship)) < 0) {
                        AccumulateFactionForces();
                    }
                }
            }
        }
        return Count;
    }

    // Includes docked and hyperspace entries in Ships; excludes ExcludedFromRating. Byte count can wrap.
    std::uint8_t TStar::CountRatedRangersByCareerMask(aGalaxyStruct::TRangerCareerSet CareerMask) {
        std::int32_t Index{};
        pas::Object* Ship{};
        aRanger::TRanger* Ranger{};
        std::uint8_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<pas::Object>(Ships, Index);
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Ship)) {
                Ranger = ranger;
                if (static_cast<std::uint8_t>(Ranger->ExcludedFromRating ^ 1) && pas::contains(CareerMask, Ranger->GetDominantCareer())) {
                    ++Result;
                }
            }
        }
        return Result;
    }

    // Unlike CountRatedRangersByCareerMask, includes ExcludedFromRating entries.
    pas::WideString TStar::GetRangerNamesByCareerMask(aGalaxyStruct::TRangerCareerSet CareerMask) {
        pas::WideString Result{};
        std::int32_t Index{};
        pas::Object* Ship{};
        aRanger::TRanger* Ranger{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<pas::Object>(Ships, Index);
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Ship)) {
                Ranger = ranger;
                if (pas::contains(CareerMask, Ranger->GetDominantCareer())) {
                    if (Result != u"") {
                        Result = pas::concat_wide({Result, u",", u" ", Ranger->Name});
                    } else {
                        Result = Ranger->Name;
                    }
                }
            }
        }
        return Result;
    }

    std::uint8_t TStar::IsConstellationVisible() {
        return Constellation->Visible;
    }

    // Group 0 Coalition, 1 Dominators/custom, 2 pirates. Lazily refreshes all three once per active Galaxy.CurrentTurn.
    float TStar::GetCachedFactionStrength(aGalaxyStruct::TStarFaction FactionGroup) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        float Weight{};
        float Strength{};
        float Extra{};
        if (Galaxy->CurrentTurn == Status.FactionStrengthCacheTurn) {
            return Status.CachedFactionStrength[FactionGroup];
        }
        Status.FactionStrengthCacheTurn = Galaxy->CurrentTurn;
        float DominatorAndCustomStrength = 0.0f;
        float CoalitionStrength = 0.0f;
        float PirateStrength = 0.0f;
        float RelativeScale = pas::real_divide(1.0L, pas::real_max<float>(1.0f, Galaxy->AverageRangerStrength));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            Weight = 1.0f;
            if (Ship->InHyperspace && Ship->OrderTarget == this) {
                Weight = Weight - 0.25L;
            }
            if (Ship->InNormalSpace() && Ship->Order == aShip::soJump) {
                Weight = Weight - 0.5L;
            }
            if (Ship->DockedTo == nullptr || Ship->Order != aShip::soNone || !(pas::class_cast_if<aRuins::TRuins*>(Ship->DockedTo) != nullptr) || static_cast<aRuins::TRuins*>(Ship->DockedTo)->FlyToStar == nullptr || static_cast<aRuins::TRuins*>(Ship->DockedTo)->FlyToStar == this) {
                if (Ship->GetHull()->HullPoints < Ship->GetHull()->Weight * 0.25L) {
                    Weight = Weight - 0.5L;
                }
                Strength = pas::real_min<pas::Extended>(1.0E+1L, pas::real_max<pas::Extended>(0.1L, static_cast<long double>(Ship->Strength) * RelativeScale)) * Weight;
                if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship)) {
                    Strength = static_cast<long double>(aConst::DominatorShipDefinitions[kling->KlingType].FactionStrengthWeight) * Strength;
                }
                if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                    CoalitionStrength = static_cast<long double>(CoalitionStrength) + Strength;
                } else if (pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(Ship->CurrentStanding)) {
                    PirateStrength = static_cast<long double>(PirateStrength) + Strength;
                } else if (pas::is_one_of<aGalaxyStruct::ssDominator, aGalaxyStruct::ssCustom>(Ship->CurrentStanding)) {
                    DominatorAndCustomStrength = static_cast<long double>(DominatorAndCustomStrength) + Strength;
                }
                if (aNormalShip::TNormalShip* normalShip = pas::class_cast_if<aNormalShip::TNormalShip*>(Ship)) {
                    Extra = static_cast<long double>(pas::sqr(static_cast<std::int32_t>(normalShip->CurrentSystemKills.Pirate))) * Strength * 0.04L;
                    CoalitionStrength = static_cast<long double>(CoalitionStrength) + Extra;
                    Extra = static_cast<long double>(pas::sqr(static_cast<std::int32_t>(normalShip->CurrentSystemKills.Normal))) * Strength * 0.04L;
                    PirateStrength = static_cast<long double>(PirateStrength) + Extra;
                    Extra = static_cast<long double>(pas::sqr(static_cast<std::int32_t>(normalShip->CurrentSystemKills.Dominator))) * Strength * 0.04L;
                    if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                        CoalitionStrength = static_cast<long double>(CoalitionStrength) + Extra;
                    } else if (pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(Ship->CurrentStanding)) {
                        PirateStrength = static_cast<long double>(PirateStrength) + Extra;
                    }
                }
            }
        }
        // Native store these in TStar.
        Status.CachedFactionStrength[aGalaxyStruct::sfCoalition] = CoalitionStrength;
        Status.CachedFactionStrength[aGalaxyStruct::sfDominators] = DominatorAndCustomStrength;
        Status.CachedFactionStrength[aGalaxyStruct::sfPirates] = PirateStrength;
        return Status.CachedFactionStrength[FactionGroup];
    }

    // Sums StrengthInBestRanger over Ships, excluding the three bosses; no docking/hyperspace filter.
    float TStar::SumBestRangerRelativeStrength(aGalaxyStruct::TShipTypeMask ShipTypeMask) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        float Result = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship != aKling::BlazerShip && Ship != aKling::KellerShip && Ship != aKling::TerronShip && pas::contains(ShipTypeMask, Ship->TypeId)) {
                Result = static_cast<long double>(Result) + Ship->StrengthInBestRanger;
            }
        }
        return Result;
    }

    void TStar::RebuildStarDistances(TGalaxy* Galaxy) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Distance{};
        TStar* Star{};
        std::int32_t Count = pas::list_count(Galaxy->Stars);
        StarDistances = nullptr;
        StarDistances.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Star = pas::list_at<TStar>(Galaxy->Stars, I);
            StarDistances[I].Star = Star;
            StarDistances[I].Distance = System::Round(aMyFunction::PointDistance(Position, Star->Position));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 2); cpp_range_2.next(I); ) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(I, Count - 1); cpp_range_3.next(J); ) {
                if (StarDistances[J].Distance < StarDistances[I].Distance) {
                    Distance = StarDistances[J].Distance;
                    StarDistances[J].Distance = StarDistances[I].Distance;
                    StarDistances[I].Distance = Distance;
                    Star = pas::checked_cast<TStar*>(static_cast<pas::Object*>(StarDistances[J].Star));
                    StarDistances[J].Star = StarDistances[I].Star;
                    StarDistances[I].Star = Star;
                }
            }
        }
    }

    // Includes any TKling, standing eight, or scripted ship with nonempty faction not beginning with SubFaction. No docking/hyperspace filter; the substring test also accepts absence.
    std::uint8_t TStar::HasHostilePresenceForScriptBinding() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr || Ship->CurrentStanding == aGalaxyStruct::ssPirateMilitary || Ship->HasIndependentScriptFaction()) {
                return true;
            }
        }
        return false;
    }

    // Starts at distance-cache index one and excludes custom factions. Requires a current distance cache.
    TStar* TStar::FindNearestStarByFaction(aGalaxyStruct::TStarFaction Faction, std::uint8_t InBattle) {
        std::int32_t I{};
        TStar* Star{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::checked_cast<TStar*>(static_cast<pas::Object*>(StarDistances[I].Star));
            if (Star->Status.ControlFaction == Faction && Star->Status.Battle == static_cast<std::uint8_t>(InBattle) && Star->Status.CustomFaction == u"") {
                return Star;
            }
        }
        return nullptr;
    }

    EC_Struct::TPointF TStar::GetBoundaryPointTowardStar(TStar* Star) {
        EC_Struct::TPointF Result{};
        double Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(Position, Star->Position));
        double Radius = ComputeMapDiameter() * 0.4L;
        Result.X = System::Trunc(System::Sin(Angle) * Radius);
        Result.Y = System::Trunc(-System::Cos(Angle) * Radius);
        return Result;
    }

    // Searches every order target in active Galaxy.LiberationGroups.
    std::uint8_t TStar::HasLiberationGroupOrder() {
        std::int32_t I{};
        std::int32_t J{};
        aGroup::TGroup* Group{};
        aGroup::TGroupRouteOrder Order{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->LiberationGroups) - 1); cpp_range.next(I); ) {
            Group = pas::list_at<aGroup::TGroup>(Galaxy->LiberationGroups, I);
            {
                const std::int32_t cpp_last = Group->Route.length() - 1;
                if (0 <= cpp_last) {
                    for (J = 0; J <= cpp_last; ++J) {
                        Order = Group->Route[J];
                        if (pas::class_cast_if<TStar*>(Order.Target) != nullptr && Order.Target == this) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    // Requires a visible peaceful system, no custom faction and an undefeated Coalition.
    void TStar::TryGenerateSystemNews() {
        std::int32_t Chance{};
        pas::WideString Names{};
        if (IsConstellationVisible() && pas::list_count(Galaxy->PlanetNews) < aConst::MaxPlanetNews && Status.ControlFaction != aGalaxyStruct::sfDominators && Status.Battle == 0 && Status.CustomFaction == u"" && Galaxy->CoalitionDefeatedTurn <= 0 && Constellation->Id != 20) {
            Chance = System::Round(aMyFunction::RemapClamped(pas::list_count(Galaxy->PlanetNews), 0.0, aConst::MaxPlanetNews, 95.0, 5.0));
            if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2111) < Chance && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnTransportActivity) == 0 && ShipTypeCounts[aGalaxyStruct::stTransport] > 9) {
                pas::WideString formatText1 = ([&] {
                    auto name = pas::borrow(Name);
                    pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Transport.Many"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Star>"_w, name.get());
                }());
                TGalaxy* galaxy = Galaxy;
                galaxy->AddPlanetNews(aGalaxyStruct::gnTransportActivity, std::move(formatText1));
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2211) < Chance && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnTransportActivity) == 0 && ShipTypeCounts[aGalaxyStruct::stTransport] > 9) {
                pas::WideString formatText1_2 = ([&] {
                    auto name_2 = pas::borrow(Name);
                    pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Transport.Many1"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                    pas::WideString textHighlightColorTag_2 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_2), std::move(textHighlightColorTag_2), u"<Star>"_w, name_2.get());
                }());
                TGalaxy* galaxy_2 = Galaxy;
                galaxy_2->AddPlanetNews(aGalaxyStruct::gnTransportActivity, std::move(formatText1_2));
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2311) < Chance && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnManyPirates) < 1 && DaysSincePlayerVisit > 30 && ShipTypeCounts[aGalaxyStruct::stKling] == 0 && ShipTypeCounts[aGalaxyStruct::stPirate] > 4 && Status.ControlFaction != aGalaxyStruct::sfPirates) {
                Names = pas::wide_int_to_str(([&] {
                    std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                    return cpp_left + ShipTypeCounts[aGalaxyStruct::stPirate];
                }()));
                {
                    pas::WideString formatText2 = ([&] {
                        auto name_3 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Pirates.Many"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_3 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_3), std::move(textHighlightColorTag_3), u"<Star>"_w, name_3.get(), u"<AttackCount>"_w, Names);
                    }());
                    TGalaxy* galaxy_3 = Galaxy;
                    galaxy_3->AddPlanetNews(aGalaxyStruct::gnManyPirates, std::move(formatText2));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2411) < Chance && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnSomePirates) < 1 && DaysSincePlayerVisit > 30 && ShipTypeCounts[aGalaxyStruct::stKling] == 0 && ShipTypeCounts[aGalaxyStruct::stPirate] > 2 && Status.ControlFaction != aGalaxyStruct::sfPirates) {
                Names = pas::wide_int_to_str(([&] {
                    std::int32_t cpp_left_2 = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                    return cpp_left_2 + ShipTypeCounts[aGalaxyStruct::stPirate];
                }()));
                {
                    pas::WideString formatText2_2 = ([&] {
                        auto name_4 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Pirates.Some"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_4 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_4), std::move(textHighlightColorTag_4), u"<Star>"_w, name_4.get(), u"<AttackCount>"_w, Names);
                    }());
                    TGalaxy* galaxy_4 = Galaxy;
                    galaxy_4->AddPlanetNews(aGalaxyStruct::gnSomePirates, std::move(formatText2_2));
                }
            } else if (aMyFunction::SeededRandomIntRange(50, 100, Galaxy->CurrentTurn * GenerationSeed * 2511) < Chance && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnNoPirates) < 1 && DaysSincePlayerVisit > 30 && ShipTypeCounts[aGalaxyStruct::stKling] == 0 && ShipTypeCounts[aGalaxyStruct::stPirate] == 0 && Status.ControlFaction != aGalaxyStruct::sfPirates) {
                Names = pas::wide_int_to_str(([&] {
                    std::int32_t cpp_left_3 = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                    return cpp_left_3 + ShipTypeCounts[aGalaxyStruct::stPirate];
                }()));
                {
                    pas::WideString formatText2_3 = ([&] {
                        auto name_5 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Pirates.None"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_5 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_5), std::move(textHighlightColorTag_5), u"<Star>"_w, name_5.get(), u"<AttackCount>"_w, Names);
                    }());
                    TGalaxy* galaxy_5 = Galaxy;
                    galaxy_5->AddPlanetNews(aGalaxyStruct::gnNoPirates, std::move(formatText2_3));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2611) < 90 && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnManyRangers) == 0 && ShipTypeCounts[aGalaxyStruct::stRanger] >= 4 && CountRatedRangersByCareerMask(pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}})) > 4) {
                Names = GetRangerNamesByCareerMask(pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcTrader}}));
                {
                    pas::WideString formatText2_4 = ([&] {
                        auto name_6 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_6 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Rangers.ManyTrader"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_6 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_6), std::move(textHighlightColorTag_6), u"<Star>"_w, name_6.get(), u"<Names>"_w, Names);
                    }());
                    TGalaxy* galaxy_6 = Galaxy;
                    galaxy_6->AddPlanetNews(aGalaxyStruct::gnManyRangers, std::move(formatText2_4));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2711) < 90 && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnManyRangers) == 0 && ShipTypeCounts[aGalaxyStruct::stRanger] >= 4 && CountRatedRangersByCareerMask(pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcPirate}})) > 4) {
                Names = GetRangerNamesByCareerMask(pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcPirate}}));
                {
                    pas::WideString formatText2_5 = ([&] {
                        auto name_7 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_7 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Rangers.ManyPirate"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_7 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_7), std::move(textHighlightColorTag_7), u"<Star>"_w, name_7.get(), u"<Names>"_w, Names);
                    }());
                    TGalaxy* galaxy_7 = Galaxy;
                    galaxy_7->AddPlanetNews(aGalaxyStruct::gnManyRangers, std::move(formatText2_5));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 2811) < 90 && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnManyRangers) == 0 && ShipTypeCounts[aGalaxyStruct::stKling] == 0 && ShipTypeCounts[aGalaxyStruct::stRanger] >= 4 && CountRatedRangersByCareerMask(pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcWarrior}})) > 4) {
                Names = GetRangerNamesByCareerMask(pas::constant_set<aGalaxyStruct::TRangerCareerSet>({{aGalaxyStruct::rcWarrior}}));
                {
                    pas::WideString formatText2_6 = ([&] {
                        auto name_8 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_8 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Rangers.ManyWarrior"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_8 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_8), std::move(textHighlightColorTag_8), u"<Star>"_w, name_8.get(), u"<Names>"_w, Names);
                    }());
                    TGalaxy* galaxy_8 = Galaxy;
                    galaxy_8->AddPlanetNews(aGalaxyStruct::gnManyRangers, std::move(formatText2_6));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 3011) < 100 && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnEminentRangerLocation) == 0 && Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader] != nullptr && pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader])->InHyperspace && pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader])->CurrentStar == this) {
                Names = pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcTrader])->Name;
                {
                    pas::WideString formatText2_7 = ([&] {
                        auto name_9 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_9 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Rangers.BestTrader"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_9 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_9), std::move(textHighlightColorTag_9), u"<Star>"_w, name_9.get(), u"<Name>"_w, Names);
                    }());
                    TGalaxy* galaxy_9 = Galaxy;
                    galaxy_9->AddPlanetNews(aGalaxyStruct::gnEminentRangerLocation, std::move(formatText2_7));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 3111) < 100 && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnEminentRangerLocation) == 0 && Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate] != nullptr && pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate])->InHyperspace && pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate])->CurrentStar == this) {
                Names = pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcPirate])->Name;
                {
                    pas::WideString formatText2_8 = ([&] {
                        auto name_10 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_10 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Rangers.BestPirate"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_10 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_10), std::move(textHighlightColorTag_10), u"<Star>"_w, name_10.get(), u"<Name>"_w, Names);
                    }());
                    TGalaxy* galaxy_10 = Galaxy;
                    galaxy_10->AddPlanetNews(aGalaxyStruct::gnEminentRangerLocation, std::move(formatText2_8));
                }
            } else if (aMyFunction::SeededRandomIntRange(0, 100, Galaxy->CurrentTurn * GenerationSeed * 3211) < 100 && Galaxy->CountPlanetNewsByType(aGalaxyStruct::gnEminentRangerLocation) == 0 && Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior] != nullptr && pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior])->InHyperspace && pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior])->CurrentStar == this) {
                Names = pas::checked_cast<aRanger::TRanger*>(Galaxy->EminentCareerShips[aGalaxyStruct::rcWarrior])->Name;
                {
                    pas::WideString formatText2_9 = ([&] {
                        auto name_11 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_11 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Star.Rangers.BestWarrior"_wref.get(), Galaxy->CurrentTurn / 10 * GenerationSeed);
                        pas::WideString textHighlightColorTag_11 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_11), std::move(textHighlightColorTag_11), u"<Star>"_w, name_11.get(), u"<Name>"_w, Names);
                    }());
                    TGalaxy* galaxy_11 = Galaxy;
                    galaxy_11->AddPlanetNews(aGalaxyStruct::gnEminentRangerLocation, std::move(formatText2_9));
                }
            }
        }
    }

    // Constant arguments preserve evaluation order; computed arguments stay at their call sites.
    void CreateFilmEffect(const pas::WideString& GraphKey, std::int32_t ShotVisual, SE_Space::TObjectSE*& Effect, aEFilm::TEFilmObj*& EffectFilm) {
        Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, GraphKey, ClassesImports::Point(0, 0), ShotVisual, -1);
        EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
    }

    void TStar::NextDay(std::uint8_t RecordFilm) {
        // Local order and unused Reserved slots preserve the native DCC32 frame.
        aItem::TItem* Item{};
        aShip::TShip* Ship{};
        std::int32_t Quantity{};
        std::int32_t i{};
        std::int32_t StepIndex{};
        std::int32_t PathStep{};
        std::int32_t Index{};
        std::int32_t AttackRound{};
        std::int32_t ArtefactIndex{};
        std::int32_t Count{};
        std::int32_t EntryIndex{};
        std::int32_t EntryCount{};
        std::int32_t DestinationShipCount{};
        std::int32_t CandidateIndex{};
        std::int32_t CandidateCount{};
        std::int32_t CombatGroup{};
        std::int32_t WorkCount{};
        std::int32_t NodeIndex{};
        std::int32_t ClosestNodeIndex{};
        std::int32_t PulledItemCount{};
        std::int32_t MineralValue{};
        std::int32_t AttackCount{};
        std::int32_t ShotEndMargin{};
        float WorkValue{};
        float WorkScale{};
        float WearMultiplier{};
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* OwnerShip{};
        aShip::TShip* GroupLeader{};
        aShip::TShip* HitShip{};
        aShip::TShip* NearestShip{};
        aItem::TItem* OtherItem{};
        aItem::TItem* NearestItem{};
        aItem::TWeapon* Weapon{};
        PMovingDropItemEntry MovingDrop{};
        PStarCombatEvent CombatEvent{};
        PStarCombatEvent ExtraAttack{};
        PStarCombatEvent QueuedAttack{};
        std::uint32_t DamageColor{};
        std::uint32_t Damage{};
        std::int32_t DrainedDamage{};
        aEFilm::TEFilmObj* ObjectFilm{};
        float Distance{};
        float Angle{};
        float WorkX{};
        float WorkY{};
        float ImpactX{};
        float ImpactY{};
        float ClosestDistance{};
        EC_Struct::TPointF Point{};
        EC_Struct::TPointF Delta{};
        aPath::PSPathNode Node{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* EffectFilm{};
        PJumpGateEntry GateEntry{};
        void* Target{};
        void* StoredTranclucator{};
        aTranclucator::TTranclucator* Tranclucator{};
        THole* Hole{};
        std::int8_t CanPull{};
        std::int8_t StationDestroyed{};
        aMissile::TMissile* Missile{};
        aMissile::TMissile* InterceptedMissile{};
        std::uint8_t BertorBoost{};
        std::int32_t RemainingAmmo{};
        std::int32_t PickupIndex{};
        std::int32_t PickupWeight{};
        void* DeathEvent{};
        pas::WideString FilmText{};
        std::int32_t NearestMissileDistance{};
        std::int32_t MissileDistance{};
        std::int32_t PointDefenseRangeSquared{};
        std::int32_t BestMissilePriority{};
        std::int32_t MissilePriority{};
        std::int32_t ActionResult{};
        aGalaxyStruct::TDamageFlagSet HitFlags{};
        // Caller-popped static link; item -4, ship -8, star -12, film flag -13. Transfers, merges or consumes the completed pickup; may free the item.
        auto CompleteItemPickup = [&]() -> void {
            aGalaxyStruct::TDominatorSeries Series{};
            std::int32_t ResearchCount{};
            std::int32_t Index{};
            if (Item->DestroyFlag > 0) {
                return;
            }
            aShip::TShip_ApplyItemDegradation(Ship, Ship->GetCargoHook(), aShip::idkUse, 3.0);
            pas::list_delete(this->Items, pas::list_indexof(this->Items, reinterpret_cast<void*>(Item)));
            ClearItemReferences(Item);
            if (aItem::TArtefact* artefact = pas::class_cast_if<aItem::TArtefact*>(Item)) {
                static_cast<aItem::TEquipment*>(artefact)->EquippedFlag = 0;
                pas::list_add(Ship->Artefacts, reinterpret_cast<void*>(Item));
                if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
                    pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship))->OwnerShip = Ship;
                }
            } else if (aItem::TCountableItem* countableItem = pas::class_cast_if<aItem::TCountableItem*>(Item)) {
                countableItem->DropFlag = 0;
                if (!RecordFilm) {
                    Item->ReleaseGraphObject();
                } else {
                    pas::list_add(this->PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                }
                pas::checked_cast<aItem::TEquipment*>(Item)->EquippedFlag = 0;
                Quantity = pas::list_count(Ship->Inventory);
                i = 0;
                while (i < Quantity) {
                    {
                        pas::Object* cpp_arg = pas::list_at<pas::Object>(Ship->Inventory, i);
                        aItem::TCountableItem* cpp_arg_2 = pas::checked_cast<aItem::TCountableItem*>(Item);
                        if (aItem::TCountableItem_CanMerge(cpp_arg_2, cpp_arg)) {
                            break;
                        }
                    }
                    ++i;
                }
                if (i < Quantity) {
                    aItem::TCountableItem_Merge(pas::list_at<aItem::TCountableItem>(Ship->Inventory, i), Item);
                    Item->DestroyFlag = 1;
                } else {
                    Item->DestroyFlag = 0;
                    pas::list_add(Ship->Inventory, reinterpret_cast<void*>(Item));
                }
            } else if (aItem::TEquipment* equipment = pas::class_cast_if<aItem::TEquipment*>(Item)) {
                equipment->EquippedFlag = 0;
                if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Ship); ruins != nullptr && pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                    pas::list_add(ruins->EquipmentShop, reinterpret_cast<void*>(Item));
                } else if (aWarrior::TWarrior* warrior = pas::class_cast_if<aWarrior::TWarrior*>(Ship); warrior != nullptr && warrior->WarriorType == aWarrior::wtFlagship && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && static_cast<aItem::TUselessItem*>(Item)->IsDominatorRemains()) {
                    Series = static_cast<aItem::TEquipment*>(Item)->DominatorSeries;
                    if (Galaxy->DominatorResearch[Series].Progress < 1.0E+2L && Galaxy->IsDominatorSeriesUnresolved(Series)) {
                        Ship->SetMoney(System::Round(Item->Cost * 3.0L) + Ship->Money);
                    } else {
                        Ship->SetMoney(System::Round(Item->Cost * 2.0L) + Ship->Money);
                    }
                    if (Galaxy->DominatorResearch[Series].Progress < 1.0E+2L && Galaxy->IsDominatorSeriesUnresolved(Series)) {
                        Galaxy->DominatorResearch[Series].Material += Item->Weight;
                    } else {
                        ResearchCount = 0;
                        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(Series); ) {
                            if (Galaxy->DominatorResearch[Series].Progress < 1.0E+2L && Galaxy->IsDominatorSeriesUnresolved(Series)) {
                                ++ResearchCount;
                            }
                        }
                        if (ResearchCount > 0) {
                            for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_2.next(Series); ) {
                                if (Galaxy->DominatorResearch[Series].Progress < 1.0E+2L && Galaxy->IsDominatorSeriesUnresolved(Series)) {
                                    Galaxy->DominatorResearch[Series].Material += pas::idiv(Item->Weight, ResearchCount);
                                }
                            }
                        }
                    }
                } else {
                    pas::list_add(Ship->Inventory, reinterpret_cast<void*>(Item));
                }
            } else if (aItem::TGoods* goods = pas::class_cast_if<aItem::TGoods*>(Item)) {
                if (aRuins::TRuins* ruins_2 = pas::class_cast_if<aRuins::TRuins*>(Ship)) {
                    std::int32_t& cpp_target = ([&] {
                        auto cpp_index = goods->ItemType;
                        auto* cpp_array = &ruins_2->ShopGoods;
                        return &(*cpp_array)[cpp_index];
                    }())->Count;
                    std::int32_t cpp_step = goods->Quantity;
                    cpp_target += cpp_step;
                } else {
                    {
                        std::int32_t cpp_step_2 = goods->Quantity;
                        std::int32_t& cpp_target_2 = Ship->CargoGoods[goods->ItemType].Count;
                        cpp_target_2 += cpp_step_2;
                    }
                    {
                        std::int32_t cpp_step_3 = pas::checked_cast<aItem::TGoods*>(Item)->Cost;
                        std::int32_t& cpp_target_3 = Ship->CargoGoods[pas::checked_cast<aItem::TGoods*>(Item)->ItemType].TotalCost;
                        cpp_target_3 += cpp_step_3;
                    }
                }
            }
            Ship->RefreshDerivedStats(true);
            std::int32_t PickupResult = Ship->ScriptItemsAct(aGalaxyStruct::satOnItemPickUp, Item, nullptr, 0);
            if (PickupResult != 0) {
                if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                    Index = pas::list_indexof(Ship->Artefacts, reinterpret_cast<void*>(Item));
                    if (Index >= 0) {
                        pas::list_delete(Ship->Artefacts, Index);
                    }
                } else {
                    Index = pas::list_indexof(Ship->Inventory, reinterpret_cast<void*>(Item));
                    if (Index >= 0) {
                        pas::list_delete(Ship->Inventory, Index);
                    }
                }
            }
            if (Ship->CargoFreeSpace < 0 && aPlayer::GetPlayer() != Ship) {
                Ship->AutoEquipInventory();
                Ship->ClearUnequippedWeaponTargets();
                aShip::TShip_DropCargoUntilNotOverloaded(Ship);
                Ship->AutoEquipInventory();
                Ship->ClearUnequippedWeaponTargets();
                if (Ship->CargoFreeSpace <= 0) {
                    Ship->ClearPickupTargets();
                }
                Ship->RefreshDerivedStats(true);
            } else if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && (aRanger::PlayerAutomaticControl || aPlayer::GetPlayer() != Ship && Ship->TypeId != aGalaxyStruct::stTranclucator)) {
                Ship->AutoEquipInventory();
                Ship->ClearUnequippedWeaponTargets();
                Ship->RefreshDerivedStats(true);
            }
            if (aPlayer::GetPlayer() == Ship) {
                this->InterruptLongTravel = true;
            }
            if (RecordFilm) {
                Globals::PrimaryFilm->PlayPickupSound(StepIndex, Item->FilmObject);
                Globals::PrimaryFilm->DetachObject(StepIndex, Item->FilmObject);
            }
            if (PickupResult < 0 || pas::class_cast_if<aItem::TGoods*>(Item) != nullptr || pas::class_cast_if<aWarrior::TWarrior*>(Ship) != nullptr && static_cast<aWarrior::TWarrior*>(Ship)->WarriorType == aWarrior::wtFlagship && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && static_cast<aItem::TUselessItem*>(Item)->IsDominatorRemains()) {
                if (RecordFilm) {
                    Globals::PrimaryFilm->ReleaseObject(StepIndex, Item->FilmObject);
                }
                pas::free(Item);
            } else if (pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr && Item->DestroyFlag > 0) {
                pas::free(Item);
            } else if (RecordFilm && Item->GraphObject != nullptr) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                Globals::PrimaryFilm->ReleaseObject(StepIndex, Item->FilmObject);
            }
            if (Ship->Speed <= 0) {
                Ship->MovementPath->Clear();
            }
        };
        std::int32_t Stage = 0;
        try {
            RefreshMovementStepParameters();
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == this) {
                DaysSincePlayerVisit = 0;
                aPlayer::GetPlayer()->BombKillsThisTurn = 0;
                if (PlayerPresenceLevel < 90) {
                    ++PlayerPresenceLevel;
                }
            } else {
                ++DaysSincePlayerVisit;
                if (PlayerPresenceLevel > 0) {
                    --PlayerPresenceLevel;
                }
            }
            if (!Globals::PlayerStarDayPrepared) {
                ++DaysSinceLastNpcShipSpawn;
            }
            RecordingTurnFilm = RecordFilm;
            PlayerCombatOccurred = false;
            InterruptLongTravel = false;
            KeepFilmRunning = false;
            StepIndex = 0;
            CurrentStepIndex = 0;
            Stage = 1;
            if (!Globals::PlayerStarDayPrepared) {
                TryGenerateSystemNews();
            }
            Stage = 2;
            if (static_cast<std::uint8_t>(Globals::PlayerStarDayPrepared ^ 1) && Galaxy->StasisModEnabled != 1) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range.next(Index); ) {
                    Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, Index);
                    Asteroid->RespawnIfOutsideSystem();
                }
            }
            Stage = 3;
            if (WingmenPendingLeadershipPenalty == nullptr) {
                WingmenPendingLeadershipPenalty = pas::make_object<pas::List>();
            }
            if (!Globals::PlayerStarDayPrepared) {
                {
                    const std::int32_t cpp_first = pas::list_count(Ships) - 1;
                    if (cpp_first >= 0) {
                        for (Index = cpp_first; Index >= 0; --Index) {
                            Ship = pas::list_at<aShip::TShip>(Ships, Index);
                            Ship->virtual_TShip_RefreshCurrentStanding();
                        }
                    }
                }
                {
                    const std::int32_t cpp_first_2 = pas::list_count(Ships) - 1;
                    if (cpp_first_2 >= 0) {
                        for (Index = cpp_first_2; Index >= 0; --Index) {
                            Ship = pas::list_at<aShip::TShip>(Ships, Index);
                            if (Galaxy->StasisModEnabled != 1 || aPlayer::GetPlayer() == Ship) {
                                Ship->virtual_TShip_NextDay();
                            }
                            if (Ship->PartnerShip != nullptr && (Ship->PartnershipDaysRemaining > 0 && pas::list_indexof(WingmenPendingLeadershipPenalty, reinterpret_cast<void*>(Ship)) < 0)) {
                                pas::list_add(WingmenPendingLeadershipPenalty, reinterpret_cast<void*>(Ship));
                            }
                        }
                    }
                }
            }
            Stage = 4;
            ProcessItemScripts(0);
            Stage = 5;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_2.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->Order == aShip::soLand && (pas::class_cast_if<aShip::TShip*>(Ship->OrderTarget) != nullptr && static_cast<aShip::TShip*>(Ship->OrderTarget)->Order != aShip::soNone)) {
                    if (static_cast<aShip::TShip*>(Ship->OrderTarget)->Order != aShip::soTeleport) {
                        static_cast<aShip::TShip*>(Ship->OrderTarget)->OrderNone(false);
                    } else {
                        Ship->OrderNone(false);
                    }
                } else if (Ship->Order == aShip::soTakeoff && (Ship->DockedTo != nullptr && (Ship->DockedTo->Order != aShip::soNone && Ship->DockedTo->Order != aShip::soTeleport))) {
                    Ship->DockedTo->OrderNone(false);
                }
            }
            Stage = 6;
            if (RecordFilm) {
                Globals::PrimaryFilm->SystemProcessName = SystemProcessName;
                Globals::PrimaryFilm->MapDiameter = ComputeMapDiameter();
                Globals::PrimaryFilm->StarGenerationSeed = GenerationSeed;
                Globals::PrimaryFilm->BackgroundImage = static_cast<std::uint32_t>(BackgroundImage);
                Globals::PrimaryFilm->Turn = Galaxy->CurrentTurn;
                Globals::PrimaryFilm->RadarRange = 0;
                Stage = 60;
                if (aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar())) {
                    Globals::PrimaryFilm->RadarRange = aPlayer::GetPlayer()->GetRadarRange();
                }
                Stage = 61;
                ObjectFilm = Globals::PrimaryFilm->AddObject(static_cast<std::int32_t>(Id), Graphic, 0, 0);
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, ObjectFilm, EC_Struct::MakePointF(0.0f, 0.0f));
                Globals::PrimaryFilm->AttachObject(StepIndex, ObjectFilm);
                Stage = 62;
                pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::PrimaryFilm->ObjectInfo)->LoadFromStar(this);
            }
            Stage = 7;
            if (Galaxy->KellerMissionState == 2 && (Galaxy->KellerTargetStar == this && (aKling::KellerShip != nullptr && (aKling::KellerShip->CurrentStar == this && Galaxy->StasisModEnabled != 1)))) {
                aKling::KellerShip->OpenKellerMissionHole();
            }
            if (Galaxy->KellerMissionState == 4 && (aKling::KellerShip != nullptr && (aKling::KellerShip->CurrentStar == this && (aKling::KellerShip->InHyperspace != 0 && Galaxy->StasisModEnabled != 1)))) {
                Hole = Galaxy->FindHoleInStarByKind(this, 4);
                if (Hole == nullptr) {
                    pas::raise(pas::make_exception<pas::Exception>("Hole not found"_a));
                }
                aKling::KellerShip->Order = aShip::soJump;
                aKling::KellerShip->OrderTarget = aKling::KellerShip->CurrentStar;
                aKling::KellerShip->OrderAbsolute = false;
                aKling::KellerShip->OrderDestination = EC_Struct::MakePointF(0.0f, 0.0f);
                aKling::KellerShip->OrderStateData = 2;
                Galaxy->KellerMissionState = 5;
            }
            Stage = 8;
            Count = pas::list_count(Ships);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && (Galaxy->StasisModEnabled != 1 || aPlayer::GetPlayer() == Ship) && (Ship->TypeId != aGalaxyStruct::stKling || (pas::checked_cast<aKling::TKling*>(Ship)->ActiveProgramAppliedTurn <= 0 || static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::prgWeaponBlocking, aGalaxyStruct::prgDisconnection>(static_cast<aKling::TKling*>(Ship)->ActiveProgramId) ^ 1)))) {
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, Ship->GetAttackMultiplier()); cpp_range_4.next(AttackRound); ) {
                        const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
                        if (1 <= cpp_last) {
                            for (EntryIndex = 1; EntryIndex <= cpp_last; ++EntryIndex) {
                                Weapon = Ship->Weapons[EntryIndex];
                                if (Weapon->Target != nullptr) {
                                    if (pas::class_cast_if<aItem::TItem*>(Weapon->Target) != nullptr || (pas::class_cast_if<aAsteroid::TAsteroid*>(Weapon->Target) != nullptr || (pas::class_cast_if<aMissile::TMissile*>(Weapon->Target) != nullptr || pas::class_cast_if<aShip::TShip*>(Weapon->Target) != nullptr && static_cast<aShip::TShip*>(Weapon->Target)->InNormalSpace()))) {
                                        CombatEvent = static_cast<PStarCombatEvent>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TStarCombatEvent))));
                                        CombatEvent->Attacker = Ship;
                                        CombatEvent->Target = Weapon->Target;
                                        CombatEvent->Weapon = Weapon;
                                        CombatEvent->CombatGroup = 0;
                                        if (MovementStepCount == aGalaxyStruct::BaseMovementStepsPerTurn) {
                                            ShotEndMargin = 30;
                                        } else {
                                            ShotEndMargin = 1;
                                        }
                                        CombatEvent->StepIndex = System::Round(static_cast<long double>(Weapon->GetShotDelayFactor()) * (MovementStepCount - ShotEndMargin));
                                        if (aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == CombatEvent->Target) {
                                            PlayerCombatOccurred = true;
                                        }
                                        Quantity = pas::list_count(CombatEvents);
                                        i = 0;
                                        while (i < Quantity) {
                                            QueuedAttack = pas::list_at<TStarCombatEvent>(CombatEvents, i);
                                            if (CombatEvent->StepIndex < QueuedAttack->StepIndex) {
                                                break;
                                            }
                                            ++i;
                                        }
                                        if (i >= Quantity) {
                                            pas::list_add(CombatEvents, static_cast<void*>(CombatEvent));
                                            if (Weapon->GetAttackCount() > 1 && static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1)) {
                                                for (auto cpp_range_5 = pas::for_to<std::int32_t>(2, Weapon->GetAttackCount()); cpp_range_5.next(CandidateIndex); ) {
                                                    ExtraAttack = static_cast<PStarCombatEvent>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TStarCombatEvent))));
                                                    ExtraAttack->Attacker = Ship;
                                                    ExtraAttack->Target = Weapon->Target;
                                                    ExtraAttack->Weapon = Weapon;
                                                    ExtraAttack->CombatGroup = 0;
                                                    ExtraAttack->StepIndex = CombatEvent->StepIndex;
                                                    pas::list_add(CombatEvents, static_cast<void*>(ExtraAttack));
                                                }
                                            }
                                        } else {
                                            pas::list_insert(CombatEvents, i, static_cast<void*>(CombatEvent));
                                            if (Weapon->GetAttackCount() > 1 && static_cast<std::uint8_t>(pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) ^ 1)) {
                                                for (auto cpp_range_6 = pas::for_to<std::int32_t>(2, Weapon->GetAttackCount()); cpp_range_6.next(CandidateIndex); ) {
                                                    ExtraAttack = static_cast<PStarCombatEvent>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TStarCombatEvent))));
                                                    ExtraAttack->Attacker = Ship;
                                                    ExtraAttack->Target = Weapon->Target;
                                                    ExtraAttack->Weapon = Weapon;
                                                    ExtraAttack->CombatGroup = 0;
                                                    ExtraAttack->StepIndex = CombatEvent->StepIndex;
                                                    pas::list_insert(CombatEvents, i, static_cast<void*>(ExtraAttack));
                                                }
                                            }
                                        }
                                        if (Ship->TypeId != aGalaxyStruct::stKling) {
                                            if (aPlayer::GetPlayer() == Ship && Ship->IsHealthEffectActive(aGalaxyStruct::heBitterPelenosia)) {
                                                WearMultiplier = 3.0f;
                                            } else {
                                                WearMultiplier = 1.0f;
                                            }
                                            {
                                                double cpp_arg = aMyFunction::NextRandomUnitFloat(Ship->RandomState) * 2.0L * WearMultiplier;
                                                aShip::TShip* ship = Ship;
                                                aShip::TShip_ApplyItemDegradation(ship, Weapon, aShip::idkUse, cpp_arg);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Stage = 9;
            if (RecordFilm) {
                CombatGroup = 0;
                Count = pas::list_count(CombatEvents);
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(Index); ) {
                    CombatEvent = pas::list_at<TStarCombatEvent>(CombatEvents, Index);
                    if (CombatEvent->CombatGroup == 0) {
                        ++CombatGroup;
                        CombatEvent->CombatGroup = CombatGroup;
                        MarkConnectedCombatEvents(CombatEvents, reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), CombatGroup);
                        MarkConnectedCombatEvents(CombatEvents, CombatEvent->Target, CombatGroup);
                    }
                }
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, CombatGroup); cpp_range_8.next(Index); ) {
                    Quantity = 0;
                    for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(EntryIndex); ) {
                        CombatEvent = pas::list_at<TStarCombatEvent>(CombatEvents, EntryIndex);
                        if (CombatEvent->CombatGroup == Index) {
                            ++Quantity;
                        }
                    }
                    WorkValue = 5.0f;
                    WorkScale = pas::real_divide(1.7E+2L, Quantity);
                    for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(EntryIndex); ) {
                        CombatEvent = pas::list_at<TStarCombatEvent>(CombatEvents, EntryIndex);
                        if (CombatEvent->CombatGroup == Index) {
                            CombatEvent->StepIndex = System::Round(WorkValue);
                            WorkValue = static_cast<long double>(WorkValue) + WorkScale;
                        }
                    }
                }
            }
            Stage = 10;
            Count = pas::list_count(Ships);
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (!Ship->InHyperspace) {
                    if (Ship->Order != aShip::soFollowShip) {
                        Ship->BuildOrderMovementPath(MovementStepCount);
                    } else {
                        Ship->ClearMovementPath();
                        Ship->OrderDestination = Ship->Position;
                        if (Ship->GetEffectiveFollowMode() == aShip::fmFollowNear) {
                            Angle = aMyFunction::HeadingDegreesToRadians(pas::abs(static_cast<std::int32_t>(static_cast<std::uint32_t>(Galaxy->CurrentTurn) * (Ship->Seed * pas::checked_cast<aShip::TShip*>(Ship->OrderTarget)->Seed))) % 360);
                            WorkCount = Ship->CalculateFollowRadius();
                            Ship->RepulsionPosition.X = System::Sin(Angle) * WorkCount;
                            Ship->RepulsionPosition.Y = System::Cos(Angle) * -WorkCount;
                        }
                    }
                }
            }
            Stage = 11;
            Count = MovementStepCount;
            SimulationStepCount = static_cast<std::uint32_t>(Count);
            EntryCount = pas::list_count(Ships);
            for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_12.next(PathStep); ) {
                for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_13.next(EntryIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Ships, EntryIndex);
                    if (Ship->Order == aShip::soFollowShip) {
                        OwnerShip = pas::checked_cast<aShip::TShip*>(Ship->OrderTarget);
                        PickupWeight = 0;
                        if (aPlayer::GetPlayer() == OwnerShip && (OwnerShip->PickupTargets != nullptr && OwnerShip->InNormalSpace())) {
                            for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(OwnerShip->PickupTargets) - 1); cpp_range_14.next(PickupIndex); ) {
                                if (aShip::TShip_IsItemInPickupRange(OwnerShip, pas::list_at<aItem::TItem>(OwnerShip->PickupTargets, PickupIndex))) {
                                    PickupWeight += pas::list_at<aItem::TItem>(OwnerShip->PickupTargets, PickupIndex)->Weight;
                                }
                            }
                        }
                        if (OwnerShip->Order != aShip::soFollowShip || OwnerShip->CargoFreeSpace < PickupWeight) {
                            if (OwnerShip->IsOnPlanet() && OwnerShip->Order == aShip::soNone) {
                                if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(Ship); tranclucator != nullptr && (tranclucator->CanFollowOwnerInCurrentStar() && tranclucator->OwnerShip == OwnerShip)) {
                                    Point = OwnerShip->CurrentPlanet->PredictPosition(MovementStepCount);
                                } else {
                                    Point = OwnerShip->CurrentPlanet->GetPosition();
                                }
                            } else if (OwnerShip->IsDockedToShip() && OwnerShip->Order == aShip::soNone) {
                                Point = OwnerShip->DockedTo->Position;
                            } else if (OwnerShip->MovementPath->ActiveTail == nullptr || OwnerShip->CargoFreeSpace < PickupWeight) {
                                Point = OwnerShip->Position;
                            } else {
                                Point = OwnerShip->MovementPath->ActiveTail->Position;
                            }
                        } else {
                            Point = OwnerShip->OrderDestination;
                        }
                        if (Galaxy->StasisModEnabled == 1 && aPlayer::GetPlayer() == Ship) {
                            Point = OwnerShip->Position;
                        }
                        if (Ship->GetEffectiveFollowMode() == aShip::fmFollowNear && (pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr && static_cast<aTranclucator::TTranclucator*>(Ship)->CanFollowOwnerInCurrentStar())) {
                            Ship->OrderDestination = Point;
                            WorkValue = 0.0f;
                        } else {
                            WorkCount = Ship->CalculateFollowRadius();
                            WorkScale = Ship->MovementSpeed * 2.0E+2L * MovementStepScale;
                            if (Ship->GetEffectiveFollowMode() == aShip::fmFollowNear) {
                                Point.X = static_cast<long double>(Point.X) + Ship->RepulsionPosition.X;
                                Point.Y = static_cast<long double>(Point.Y) + Ship->RepulsionPosition.Y;
                                Distance = aMyFunction::PointDistance(Ship->OrderDestination, Point);
                                WorkValue = 0.0L - Distance;
                            } else {
                                Distance = aMyFunction::PointDistance(Ship->OrderDestination, Point);
                                WorkValue = static_cast<long double>(WorkCount) - Distance;
                            }
                            if (Distance != 0.0L) {
                                if (WorkValue <= 0.0L) {
                                    WorkValue = pas::real_divide(pas::real_min<float>(-WorkValue, WorkScale), Distance);
                                    Delta.X = (static_cast<long double>(Point.X) - Ship->OrderDestination.X) * WorkValue;
                                    Delta.Y = (static_cast<long double>(Point.Y) - Ship->OrderDestination.Y) * WorkValue;
                                } else {
                                    WorkValue = pas::real_divide(pas::real_min<float>(WorkValue, WorkScale), Distance);
                                    Delta.X = (static_cast<long double>(Ship->OrderDestination.X) - Point.X) * WorkValue;
                                    Delta.Y = (static_cast<long double>(Ship->OrderDestination.Y) - Point.Y) * WorkValue;
                                }
                                Distance = pas::real_divide(MapDiameter, 2.0L);
                                WorkScale = static_cast<long double>(Ship->OrderDestination.X) * Ship->OrderDestination.X + static_cast<long double>(Ship->OrderDestination.Y) * Ship->OrderDestination.Y;
                                if (pas::sqr(0.7L * Distance) < WorkScale && (static_cast<long double>(Delta.X) * Ship->OrderDestination.X + static_cast<long double>(Delta.Y) * Ship->OrderDestination.Y > 0.0L && (reinterpret_cast<aShip::TShip*>(Ship->OrderTarget)->Order == aShip::soFollowShip && static_cast<long double>(reinterpret_cast<aShip::TShip*>(Ship->OrderTarget)->OrderDestination.X) * reinterpret_cast<aShip::TShip*>(Ship->OrderTarget)->OrderDestination.X + static_cast<long double>(reinterpret_cast<aShip::TShip*>(Ship->OrderTarget)->OrderDestination.Y) * reinterpret_cast<aShip::TShip*>(Ship->OrderTarget)->OrderDestination.Y <= WorkScale))) {
                                    Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::RemapClamped(System::Sqrt(WorkScale) - 0.7L * Distance, 0.0, 0.5L * Distance, 0.0, 45.0));
                                    if (static_cast<long double>(Delta.X) * Ship->OrderDestination.Y - static_cast<long double>(Delta.Y) * Ship->OrderDestination.X > 0.0L) {
                                        Angle = -Angle;
                                    }
                                    Point = Delta;
                                    WorkX = System::Sin(Angle);
                                    WorkY = System::Cos(Angle);
                                    Delta.X = static_cast<long double>(Point.X) * WorkY - static_cast<long double>(Point.Y) * WorkX;
                                    Delta.Y = static_cast<long double>(Point.X) * WorkX + static_cast<long double>(Point.Y) * WorkY;
                                }
                                Ship->OrderDestination.X = static_cast<long double>(Ship->OrderDestination.X) + Delta.X;
                                Ship->OrderDestination.Y = static_cast<long double>(Ship->OrderDestination.Y) + Delta.Y;
                            }
                        }
                    }
                }
            }
            Stage = 12;
            Stage = 13;
            if (RecordFilm) {
                for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_15.next(EntryIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Ships, EntryIndex);
                    if (Ship->Order == aShip::soFollowShip) {
                        Ship->RepulsionPosition = Ship->Position;
                    }
                }
                for (auto cpp_range_16 = pas::for_to<std::int32_t>(1, Count - 1); cpp_range_16.next(PathStep); ) {
                    for (auto cpp_range_17 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_17.next(EntryIndex); ) {
                        Ship = pas::list_at<aShip::TShip>(Ships, EntryIndex);
                        if (pas::is_one_of<aShip::soMove, aShip::soFollowShip>(Ship->Order) && ((!(pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr) || static_cast<aTranclucator::TTranclucator*>(Ship)->FollowOwner == 0) && (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) || (static_cast<aKling::TKling*>(Ship)->KlingType != aGalaxyStruct::ktBoss || static_cast<aKling::TKling*>(Ship)->DominatorSeries != aGalaxyStruct::dsTerron)))) {
                            WorkScale = Ship->MovementSpeed;
                            if (Galaxy->StasisModEnabled == 1 && aPlayer::GetPlayer() != Ship) {
                                WorkScale = 0.0f;
                            }
                            Distance = aMyFunction::PointDistanceSquared(Ship->RepulsionPosition, Ship->OrderDestination);
                            if (Distance != 0.0L) {
                                if (pas::sqr(static_cast<pas::Extended>(WorkScale)) >= Distance) {
                                    Ship->RepulsionPosition = Ship->OrderDestination;
                                } else {
                                    Distance = pas::real_divide(1.0L, System::Sqrt(Distance)) * WorkScale;
                                    Ship->RepulsionPosition.X = (static_cast<long double>(Ship->OrderDestination.X) - Ship->RepulsionPosition.X) * Distance + Ship->RepulsionPosition.X;
                                    Ship->RepulsionPosition.Y = (static_cast<long double>(Ship->OrderDestination.Y) - Ship->RepulsionPosition.Y) * Distance + Ship->RepulsionPosition.Y;
                                }
                                if (Galaxy->StasisModEnabled != 1) {
                                    Ship->RepelFollowingShips();
                                }
                            }
                        }
                    }
                }
                for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_18.next(EntryIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Ships, EntryIndex);
                    if (Ship->Order == aShip::soFollowShip && (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr) || static_cast<aTranclucator::TTranclucator*>(Ship)->FollowOwner == 0) && (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) || (static_cast<aKling::TKling*>(Ship)->KlingType != aGalaxyStruct::ktBoss || static_cast<aKling::TKling*>(Ship)->DominatorSeries != aGalaxyStruct::dsTerron))) {
                        Ship->OrderDestination = Ship->RepulsionPosition;
                    }
                }
            }
            Stage = 14;
            Count = pas::list_count(Ships);
            for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_19.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->Order == aShip::soFollowShip) {
                    if (pas::sqr(static_cast<pas::Extended>(Ship->Position.X)) + pas::sqr(static_cast<pas::Extended>(Ship->Position.Y)) > pas::sqr(static_cast<pas::Extended>(SafeRadius)) && pas::sqr(static_cast<pas::Extended>(Ship->OrderDestination.X)) + pas::sqr(static_cast<pas::Extended>(Ship->OrderDestination.Y)) < pas::sqr(static_cast<pas::Extended>(SafeRadius))) {
                        aMyFunction::RayIntersectsOriginCircle(Ship->Position, Ship->OrderDestination, Point, SafeRadius);
                        WorkValue = aMyFunction::PointDistance(Point, Ship->OrderDestination);
                        WorkScale = aMyFunction::HeadingDegreesToRadians(pas::real_divide(WorkValue * 3.6E+2L, pas::constant(2.0L * SystemImports::Pi) * SafeRadius));
                        WorkValue = Math::ArcTan2(Point.X, -Point.Y);
                        if (aMyFunction::HeadingDifferenceDegrees(aMyFunction::RadiansToHeadingDegrees(WorkValue), aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(static_cast<long double>(Ship->Position.X) - Point.X, -(static_cast<long double>(Ship->Position.Y) - Point.Y)))) < 0.0L) {
                            WorkValue = static_cast<long double>(WorkValue) + WorkScale;
                        } else {
                            WorkValue = static_cast<long double>(WorkValue) - WorkScale;
                        }
                        Ship->OrderDestination.X = System::Sin(WorkValue) * (SafeRadius + 0.1L);
                        Ship->OrderDestination.Y = -System::Cos(WorkValue) * (SafeRadius + 0.1L);
                    }
                    Ship->RebuildMovePath();
                }
            }
            Stage = 15;
            Count = pas::list_count(Ships);
            for (auto cpp_range_20 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_20.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->Order == aShip::soJump && Ship->TypeId == aGalaxyStruct::stRanger) {
                    if (Ship->MovementPath->ActiveHead != nullptr) {
                        if (static_cast<long double>(pas::sqr(Ship->Speed + 100)) < aMyFunction::PointDistanceSquared(Ship->MovementPath->ActiveHead->Position, Ship->MovementPath->ActiveTail->Position)) {
                            GroupLeader = pas::checked_cast<aRanger::TRanger*>(Ship)->PartnerShip;
                            if (GroupLeader == nullptr) {
                                GroupLeader = Ship;
                            }
                            for (auto cpp_range_21 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_21.next(EntryIndex); ) {
                                OwnerShip = pas::list_at<aShip::TShip>(Ships, EntryIndex);
                                if (OwnerShip->Order == aShip::soJump && OwnerShip->OrderTarget == Ship->OrderTarget && OwnerShip->TypeId == aGalaxyStruct::stRanger && Ship != OwnerShip) {
                                    if (pas::checked_cast<aRanger::TRanger*>(OwnerShip)->PartnerShip == GroupLeader || OwnerShip == GroupLeader) {
                                        if (OwnerShip->MovementPath->ActiveHead == nullptr || static_cast<long double>(pas::sqr(OwnerShip->Speed + 100)) >= aMyFunction::PointDistanceSquared(OwnerShip->MovementPath->ActiveHead->Position, OwnerShip->MovementPath->ActiveTail->Position)) {
                                            if ((OwnerShip->PickupTargets == nullptr || pas::list_count(OwnerShip->PickupTargets) <= 0) && aMyFunction::PointDistanceSquared(Ship->Position, OwnerShip->Position) <= 6.4E+5L) {
                                                Angle = std::fabs(static_cast<pas::Extended>(aMyFunction::HeadingDifferenceDegrees(OwnerShip->MovementDirection, aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-Ship->Position.X, Ship->Position.Y)))));
                                                if (Angle >= 9.0E+1L && OwnerShip->Speed >= 200 || Angle >= 175.0L) {
                                                    OwnerShip->ClearMovementPath();
                                                    if (Angle < 175.0L && (OwnerShip->Speed >= 200 && OwnerShip->CurrentStar == PlayerStar)) {
                                                        Distance = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Ship->Position.X) * Ship->Position.X + static_cast<long double>(Ship->Position.Y) * Ship->Position.Y));
                                                        Point.X = static_cast<long double>(Ship->Position.X) * Distance * 1.0E+4L + OwnerShip->Position.X;
                                                        Point.Y = static_cast<long double>(Ship->Position.Y) * Distance * 1.0E+4L + OwnerShip->Position.Y;
                                                        OwnerShip->AppendTurningPath(Point, false, aGalaxyStruct::BaseMovementStepsPerTurn);
                                                    }
                                                    OwnerShip->AppendHyperspaceTransitionPath(1.0f);
                                                    if (OwnerShip->MovementPath->ActiveTail != nullptr) {
                                                        OwnerShip->OrderDestination = OwnerShip->MovementPath->ActiveTail->Position;
                                                    } else {
                                                        OwnerShip->OrderDestination = OwnerShip->Position;
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
            }
            Stage = 16;
            if (PlayerStar == this) {
                AvoidShipPathCollisions();
            }
            Stage = 17;
            if (RecordFilm) {
                EntryCount = pas::list_count(Galaxy->JumpGates);
                for (auto cpp_range_22 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_22.next(Index); ) {
                    GateEntry = pas::list_at<TJumpGateEntry>(Galaxy->JumpGates, Index);
                    GateEntry->UsedThisTurn = true;
                    ObjectFilm = Globals::PrimaryFilm->AddObject(0u, GateEntry->Gate, 0, 0);
                    GateEntry->GateFilmId = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ObjectFilm));
                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, ObjectFilm, GateEntry->Gate->Position);
                    Globals::PrimaryFilm->SetObjectAngle(StepIndex, ObjectFilm, GateEntry->Gate->GetAngle());
                    Globals::PrimaryFilm->SetGateSize(StepIndex, ObjectFilm, GateEntry->Gate->Size.X);
                    Globals::PrimaryFilm->SetGateState(StepIndex, ObjectFilm, 2);
                    Globals::PrimaryFilm->CloseGate(StepIndex, ObjectFilm);
                    Globals::PrimaryFilm->SetObjectText(StepIndex, ObjectFilm, GateEntry->Gate->GetText());
                    Globals::PrimaryFilm->AttachObject(StepIndex, ObjectFilm);
                    if (GateEntry->Effect != nullptr && GateEntry->Effect->IsAttachedToSpace()) {
                        ObjectFilm = Globals::PrimaryFilm->AddObject(0u, GateEntry->Effect, 0, 0);
                        GateEntry->EffectFilmId = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ObjectFilm));
                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, ObjectFilm, GateEntry->Effect->Position);
                        Globals::PrimaryFilm->SetObjectAngle(StepIndex, ObjectFilm, GateEntry->Effect->GetAngle());
                        Globals::PrimaryFilm->SetGateSize(StepIndex, ObjectFilm, GateEntry->Effect->Size.X);
                        Globals::PrimaryFilm->AttachObject(StepIndex, ObjectFilm);
                    }
                }
            }
            Stage = 18;
            if (RecordFilm) {
                for (auto cpp_range_23 = pas::for_to<std::int32_t>(0, pas::list_count(Galaxy->Holes) - 1); cpp_range_23.next(Index); ) {
                    Hole = pas::list_at<THole>(Galaxy->Holes, Index);
                    if (Hole->Star1 == this) {
                        EffectFilm = Globals::PrimaryFilm->AddObject(Hole->Id, Hole->Graphic, 0, 0);
                        Hole->FilmObjectId = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(EffectFilm));
                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Hole->Position1);
                        if (Galaxy->CurrentTurn == Hole->CreatedTurn) {
                            Globals::PrimaryFilm->SetHoleState(StepIndex, EffectFilm, 1);
                        } else {
                            Globals::PrimaryFilm->SetHoleState(StepIndex, EffectFilm, 0);
                        }
                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                    } else if (Hole->Star2 == this) {
                        EffectFilm = Globals::PrimaryFilm->AddObject(Hole->Id, Hole->Graphic, 0, 0);
                        Hole->FilmObjectId = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(EffectFilm));
                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Hole->Position2);
                        if (Galaxy->CurrentTurn == Hole->CreatedTurn) {
                            Globals::PrimaryFilm->SetHoleState(StepIndex, EffectFilm, 1);
                        } else {
                            Globals::PrimaryFilm->SetHoleState(StepIndex, EffectFilm, 0);
                        }
                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                    }
                }
            }
            Stage = 19;
            for (auto cpp_range_24 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_24.next(Index); ) {
                Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                Planet->InitializeFilmState(StepIndex, RecordFilm);
            }
            Stage = 20;
            for (auto cpp_range_25 = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range_25.next(Index); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, Index);
                Asteroid->PrepareTurnMovement(StepIndex, RecordFilm);
            }
            Stage = 21;
            for (auto cpp_range_26 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_26.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Galaxy->StasisModEnabled != 1 || aPlayer::GetPlayer() == Ship) {
                    if (Ship->GetHull()->InterceptorsEnabled) {
                        Ship->LaunchInterceptors();
                    }
                    Ship->GetHull()->InterceptorTarget = nullptr;
                    if (Ship->AfterburnerActive) {
                        if (aShip::TShip_IsEquipmentUsable(Ship, Ship->GetEngine()) && Ship->GetSlotCount(aConst::sskAfterburner) > 0) {
                            Ship->ApplyAfterburnerItemDegradation();
                        }
                    }
                }
            }
            Stage = 22;
            for (auto cpp_range_27 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_27.next(Index); ) {
                pas::list_at<aShip::TShip>(Ships, Index)->PrepareTurnMovement(StepIndex, RecordFilm);
            }
            Stage = 23;
            for (auto cpp_range_28 = pas::for_to<std::int32_t>(0, pas::list_count(Missiles) - 1); cpp_range_28.next(Index); ) {
                Missile = static_cast<aMissile::TMissile*>(pas::load_unaligned<void*>(pas::byte_offset(pas::list_data(Missiles), Index * sizeof(void*))));
                Missile->PrepareTurnMovement(StepIndex, RecordFilm, false);
            }
            Stage = 24;
            if (RecordFilm) {
                Count = pas::list_count(Items);
                for (auto cpp_range_29 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_29.next(Index); ) {
                    Item = pas::list_at<aItem::TItem>(Items, Index);
                    {
                        SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                        std::uint32_t id = Item->Id;
                        aEFilm::TEFilm* primaryFilm = Globals::PrimaryFilm;
                        Item->FilmObject = primaryFilm->AddObject(id, graphObject, 0, 0);
                    }
                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, Item->FilmObject, Item->Position);
                    Globals::PrimaryFilm->AttachObject(StepIndex, Item->FilmObject);
                }
            }
            Stage = 25;
            Count = MovementStepCount;
            Globals::PrimaryFilm->AdvanceObjects(StepIndex);
            ++StepIndex;
            CurrentStepIndex = StepIndex;
            Stage = 26;
            if (RecordFilm) {
                if (static_cast<std::uint8_t>(PlayerCombatOccurred ^ 1) || aPlayer::GetPlayer()->MovementPath->ActiveTail != nullptr && !(static_cast<long double>(aPlayer::GetPlayer()->Speed * aPlayer::GetPlayer()->Speed + 100) >= aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, aPlayer::GetPlayer()->MovementPath->ActiveTail->Position)) || aPlayer::GetPlayer()->Order == aShip::soLand && aPlayer::GetPlayer()->FilmAlphaStep != 0.0L) {
                    PlayerFilmPath = nullptr;
                } else {
                    PlayerFilmPath = pas::construct_call<aPath::TSPath>(aPath::TSPath_Create);
                    PlayerFilmPath->AppendWaypoint(aPlayer::GetPlayer()->Position, StepIndex);
                }
            }
            Stage = 27;
            EntryIndex = 0;
            while (pas::list_count(Items) > EntryIndex) {
                Item = pas::list_at<aItem::TItem>(Items, EntryIndex);
                if (static_cast<long double>(DamageRadius) * DamageRadius > pas::sqr(static_cast<pas::Extended>(Item->Position.X)) + pas::sqr(static_cast<pas::Extended>(Item->Position.Y))) {
                    ClearItemReferences(Item);
                    if (RecordFilm) {
                        aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                        Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, nullptr, Item->FilmObject);
                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, true, true);
                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                        pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                    }
                    pas::list_delete(Items, EntryIndex);
                    pas::free(Item);
                } else {
                    ++EntryIndex;
                }
            }
            Stage = 28;
            NearestShip = nullptr;
            NearestItem = nullptr;
            WorkValue = 1.0E+20f;
            for (auto cpp_range_30 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_30.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->PickupTargets != nullptr) {
                    if (Ship->GetCargoHook() == nullptr) {
                        Ship->ClearPickupTargets();
                    } else {
                        Item = pas::list_at<aItem::TItem>(Ship->PickupTargets, 0);
                        WorkScale = aMyFunction::PointDistanceSquared(Ship->Position, Item->Position);
                        if (WorkScale < WorkValue) {
                            WorkValue = WorkScale;
                            NearestShip = Ship;
                            NearestItem = Item;
                        }
                    }
                }
            }
            Stage = 29;
            PlayerCombatOccurred = false;
            for (auto cpp_range_31 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_31.next(PathStep); ) {
                EntryIndex = 0;
                Stage = 2900;
                if (Galaxy->StasisModEnabled != 1) {
                    while (pas::list_count(Missiles) > EntryIndex) {
                        Missile = static_cast<aMissile::TMissile*>(pas::load_unaligned<void*>(pas::byte_offset(pas::list_data(Missiles), EntryIndex * sizeof(void*))));
                        if (Missile->DestroyQueued) {
                            ++EntryIndex;
                            continue;
                        }
                        Stage = 2901;
                        Target = Missile->StepDay(StepIndex, RecordFilm);
                        if (Target == nullptr) {
                            ++EntryIndex;
                            continue;
                        }
                        HitShip = nullptr;
                        Stage = 2902;
                        if (aPlayer::GetPlayer() == Missile->OwnerShip && (pas::class_cast_if<aItem::TGoods*>(static_cast<pas::Object*>(Target)) != nullptr && pas::checked_cast<aItem::TGoods*>(static_cast<pas::Object*>(Target))->NaturalFlag != 0)) {
                            Item = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Target));
                            if (aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnMissileHittingObject, Item, Missile, 1) == 0) {
                                ++EntryIndex;
                                continue;
                            }
                            if (RecordFilm) {
                                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Missile->GetWeaponInfo()->AreaSE, ClassesImports::Point(0, 0), Missile->GetShotVisual(), -1);
                                EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                                Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Item->Position);
                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                Globals::PrimaryFilm->DetachObject(StepIndex, Missile->FilmObject);
                                pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Missile->FilmObject));
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Missile->Graphic));
                                Globals::PrimaryFilm->DetachObject(StepIndex, Item->FilmObject);
                            }
                            ClearItemReferences(Item);
                            Quantity = pas::checked_cast<aItem::TGoods*>(Item)->Quantity;
                            if (Quantity >= 5) {
                                DropMinerals(System::Trunc(pas::real_divide(Quantity * 0.8L, Missile->GetWeaponInfo()->MiningFactor)), Item->Position, GenerationSeed * static_cast<std::uint32_t>(Item->Id));
                            }
                            Point = Item->Position;
                            if (RecordFilm) {
                                pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                                pas::list_delete(Items, pas::list_indexof(Items, reinterpret_cast<void*>(Item)));
                                pas::free(Item);
                            } else {
                                pas::list_delete(Items, pas::list_indexof(Items, reinterpret_cast<void*>(Item)));
                                pas::free(Item);
                            }
                        } else if (pas::class_cast_if<aItem::TItem*>(static_cast<pas::Object*>(Target)) != nullptr) {
                            Stage = 2903;
                            Item = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Target));
                            if (Missile->OwnerShip != nullptr && Missile->OwnerShip->ScriptItemsAct(aGalaxyStruct::satOnMissileHittingObject, Item, Missile, 1) == 0) {
                                ++EntryIndex;
                                continue;
                            } else {
                                ActionResult = 0;
                                if (Item->DestroyFlag == 0) {
                                    Item->DestroyFlag = 1;
                                }
                                if (Item->ScriptItem != nullptr) {
                                    ActionResult = reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->RunActionCode(aGalaxyStruct::satOnItemHit, Missile->OwnerShip, Missile, this, ActionResult);
                                }
                                if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) {
                                    ActionResult = aScript::RunItemConfigActionCode(Item, aGalaxyStruct::satOnItemHit, Missile->OwnerShip, Missile, this, ActionResult);
                                }
                                if (Item->DestroyFlag < 0 && Missile->Target != Item) {
                                    ++EntryIndex;
                                    ++Item->DestroyFlag;
                                    continue;
                                } else {
                                    if (RecordFilm) {
                                        Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Missile->GetWeaponInfo()->AreaSE, ClassesImports::Point(0, 0), Missile->GetShotVisual(), -1);
                                        EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Item->Position);
                                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, Item->ItemType == aConst::t_ArtefactBomb && Item->DestroyFlag >= 0, true);
                                        if ((Item->ItemType == aConst::t_ArtefactBomb || pas::class_cast_if<aItem::TCistern*>(Item) != nullptr && static_cast<aItem::TCistern*>(Item)->Fuel > 0) && Item->DestroyFlag > 0) {
                                            Globals::PrimaryFilm->SetDestructionEffect(StepIndex, EffectFilm, 1);
                                        }
                                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                        Globals::PrimaryFilm->DetachObject(StepIndex, Missile->FilmObject);
                                        pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Missile->FilmObject));
                                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Missile->Graphic));
                                    }
                                    Stage = 2904;
                                    if (Item->DestroyFlag >= 0) {
                                        if (RecordFilm) {
                                            Globals::PrimaryFilm->DetachObject(StepIndex, Item->FilmObject);
                                        }
                                        for (auto cpp_range_32 = pas::for_to<std::int32_t>(0, pas::list_count(MovingDropItems) - 1); cpp_range_32.next(i); ) {
                                            MovingDrop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, i);
                                            if (MovingDrop->Payload == Item) {
                                                MovingDrop->Payload = nullptr;
                                            }
                                        }
                                        ClearItemReferences(Item);
                                    }
                                    Stage = 2905;
                                    if (Item->ItemType == aConst::t_ArtefactBomb || pas::class_cast_if<aItem::TCistern*>(Item) != nullptr && static_cast<aItem::TCistern*>(Item)->Fuel > 0 || ActionResult != 0) {
                                        CandidateCount = pas::list_count(Ships);
                                        for (auto cpp_range_33 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_33.next(CandidateIndex); ) {
                                            Ship = pas::list_at<aShip::TShip>(Ships, CandidateIndex);
                                            if (Ship->InNormalSpace() && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (aPlayer::GetPlayer() != Ship || Galaxy->GodModEnabled != 1 && Galaxy->SpecialSimulationMode == 0))) {
                                                Distance = aMyFunction::PointDistanceSquared(Ship->Position, Item->Position);
                                                if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) >= Distance) {
                                                    Damage = aShip::TShip_ApplyExplosionDamage(Ship, Missile->OwnerShip, Item, ActionResult, Missile);
                                                    if (Ship->IsHullDestroyed() && (aPlayer::GetPlayer() != nullptr && (Item->ItemType == aConst::t_ArtefactBomb && aPlayer::GetPlayer() == Missile->OwnerShip))) {
                                                        ++aPlayer::GetPlayer()->BombKillsThisTurn;
                                                    }
                                                    DamageColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                                                    if (RecordFilm) {
                                                        aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                                        Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, Ship->IsHullDestroyed(), true);
                                                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                                    }
                                                }
                                            }
                                        }
                                        CandidateCount = pas::list_count(Items);
                                        for (auto cpp_range_34 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_34.next(CandidateIndex); ) {
                                            OtherItem = pas::list_at<aItem::TItem>(Items, CandidateIndex);
                                            if (OtherItem != Item) {
                                                Distance = aMyFunction::PointDistanceSquared(OtherItem->Position, Item->Position);
                                                if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) >= Distance) {
                                                    pas::Extended cpp_right = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                                                    if (2.0E+1L - pas::real_divide(2.0E+1L * Distance, aConst::ItemExplosionRadiusSquared) >= cpp_right) {
                                                        if (OtherItem->DestroyFlag < 0) {
                                                            ++OtherItem->DestroyFlag;
                                                        } else {
                                                            pas::list_add(ReferencedItems, reinterpret_cast<void*>(OtherItem));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    } else if (aItem::TUselessItem* uselessItem = pas::class_cast_if<aItem::TUselessItem*>(Item); uselessItem != nullptr && static_cast<aItem::TEquipment*>(uselessItem)->ConfigBlockName == u"ExampleAsteroid") {
                                        std::uint32_t cpp_arg_2 = GenerationSeed * static_cast<std::uint32_t>(Item->Id);
                                        std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(20, 30, GenerationSeed * static_cast<std::uint32_t>(Item->Id));
                                        EC_Struct::TPointF position = Item->Position;
                                        TStar* self = this;
                                        self->DropMinerals(seededRandomIntRange, position, cpp_arg_2);
                                    }
                                    Point = Item->Position;
                                    Stage = 2906;
                                    if (Item->DestroyFlag >= 0) {
                                        if (RecordFilm) {
                                            pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                                        }
                                        pas::list_delete(Items, pas::list_indexof(Items, reinterpret_cast<void*>(Item)));
                                        pas::free(Item);
                                    } else {
                                        ++Item->DestroyFlag;
                                    }
                                }
                            }
                        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(static_cast<pas::Object*>(Target)) != nullptr) {
                            Stage = 2907;
                            Asteroid = pas::checked_cast<aAsteroid::TAsteroid*>(static_cast<pas::Object*>(Target));
                            if (Missile->OwnerShip != nullptr && Missile->OwnerShip->ScriptItemsAct(aGalaxyStruct::satOnMissileHittingObject, Asteroid, Missile, 1) == 0) {
                                ++EntryIndex;
                                continue;
                            } else {
                                if (RecordFilm) {
                                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Missile->GetWeaponInfo()->AreaSE, ClassesImports::Point(0, 0), Missile->GetShotVisual(), -1);
                                    EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Asteroid->Position);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                    Globals::PrimaryFilm->DetachObject(StepIndex, Missile->FilmObject);
                                    pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Missile->FilmObject));
                                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Missile->Graphic));
                                }
                                ClearTargetReferences(Asteroid);
                                Stage = 2908;
                                MineralValue = DropMinerals(System::Trunc(pas::real_divide(Asteroid->MineralCount, Missile->GetWeaponInfo()->MiningFactor)), Asteroid->Position, GenerationSeed * Asteroid->Id);
                                if (aPlayer::GetPlayer() == Missile->OwnerShip) {
                                    ProcessPlayerAsteroidKill(MineralValue, Asteroid->Position, Asteroid->Id);
                                }
                                Asteroid->Respawn();
                                Point = Asteroid->Position;
                            }
                        } else if (pas::class_cast_if<aShip::TShip*>(static_cast<pas::Object*>(Target)) != nullptr) {
                            if (!pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Target))->IsHullDestroyed()) {
                                Stage = 29090;
                                HitShip = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Target));
                                if (aPlayer::GetPlayer() == HitShip) {
                                    PlayerCombatOccurred = true;
                                }
                                Stage = 29091;
                                DrainedDamage = 0;
                                Damage = aShip::TShip_ApplyMissileHit(HitShip, Missile, DamageColor, HitFlags);
                                Stage = 29092;
                                if (pas::contains(HitFlags, aGalaxyStruct::dkDrain) && static_cast<std::int32_t>(Damage) > 0) {
                                    DrainedDamage += static_cast<std::int32_t>(Damage);
                                }
                                if (RecordFilm) {
                                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, Missile->GetWeaponInfo()->AreaSE, ClassesImports::Point(0, 0), Missile->GetShotVisual(), -1);
                                    EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, HitShip->Position);
                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, nullptr, HitShip->FilmObject);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, HitShip->IsHullDestroyed(), true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                    Globals::PrimaryFilm->DetachObject(StepIndex, Missile->FilmObject);
                                    pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Missile->FilmObject));
                                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Missile->Graphic));
                                }
                                Stage = 29093;
                                Point = HitShip->Position;
                                if (pas::in_range(Missile->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstMissile))) {
                                    Stage = 29094;
                                    CandidateCount = pas::list_count(Ships);
                                    for (auto cpp_range_35 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_35.next(CandidateIndex); ) {
                                        Ship = pas::list_at<aShip::TShip>(Ships, CandidateIndex);
                                        if (Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && Ship != Target) {
                                            if (aMyFunction::PointDistanceSquared(Point, Ship->Position) < Math::Power(Missile->GetWeaponInfo()->SecondaryDamageRadius, 2.0L)) {
                                                Stage = 29095;
                                                Damage = aShip::TShip_ApplyMissileHit(Ship, Missile, DamageColor, HitFlags);
                                                if (pas::contains(HitFlags, aGalaxyStruct::dkDrain) && static_cast<std::int32_t>(Damage) > 0) {
                                                    DrainedDamage += static_cast<std::int32_t>(Damage);
                                                }
                                                Stage = 29096;
                                                Ship->RefreshDerivedStats(true);
                                                if (RecordFilm) {
                                                    aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, Ship->IsHullDestroyed(), true);
                                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                                }
                                            }
                                        }
                                    }
                                }
                                Stage = 29097;
                                if (DrainedDamage > 0 && (Missile->OwnerShip != nullptr && (Missile->OwnerShip->InNormalSpace() && Missile->OwnerShip->CurrentStar == this))) {
                                    aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Missile->OwnerShip->FilmObject, Missile->OwnerShip->FilmObject);
                                    if (aPlayer::GetPlayer() != Missile->OwnerShip) {
                                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, aConst::OwnerToFilmColor(Missile->OwnerShip->OwnerId), -DrainedDamage, false, true);
                                    } else {
                                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, aConst::OwnerToFilmColor(aConst::RaceToOwner(Missile->OwnerShip->PilotRace)), -DrainedDamage, false, true);
                                    }
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                }
                            }
                        }
                        Stage = 29098;
                        pas::free(Missile);
                    }
                }
                EntryIndex = 0;
                Stage = 2910;
                while (pas::list_count(Missiles) > EntryIndex) {
                    Missile = static_cast<aMissile::TMissile*>(pas::load_unaligned<void*>(pas::byte_offset(pas::list_data(Missiles), EntryIndex * sizeof(void*))));
                    if (Missile->DestroyQueued) {
                        if (RecordFilm) {
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Missile->Graphic));
                            pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Missile->FilmObject));
                        }
                        pas::free(Missile);
                    } else {
                        ++EntryIndex;
                    }
                }
                if (Galaxy->StasisModEnabled != 1 && (PathStep & 3) == 0) {
                    EntryCount = pas::list_count(Asteroids);
                    for (auto cpp_range_36 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_36.next(EntryIndex); ) {
                        Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, EntryIndex);
                        if (static_cast<long double>(Asteroid->Position.X) * Asteroid->Position.X + static_cast<long double>(Asteroid->Position.Y) * Asteroid->Position.Y < pas::sqr(Radius * 0.7L)) {
                            if (RecordFilm) {
                                aGalaxy::CreateFilmEffect(u"Weapon.Asteroid"_wref.get(), 0, Effect, EffectFilm);
                                Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Asteroid->Position);
                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                            }
                            ClearTargetReferences(Asteroid);
                            Asteroid->Respawn();
                        } else {
                            Planet = nullptr;
                            Quantity = pas::list_count(Planets);
                            for (auto cpp_range_37 = pas::for_to<std::int32_t>(0, Quantity - 1); cpp_range_37.next(i); ) {
                                Planet = pas::list_at<aPlanet::TPlanet>(Planets, i);
                                Point = Planet->GetPosition();
                                WorkX = Point.X;
                                WorkY = Point.Y;
                                ImpactX = Asteroid->Position.X;
                                ImpactY = Asteroid->Position.Y;
                                if (Planet->GraphicRadius * Planet->GraphicRadius >= (static_cast<long double>(WorkX) - ImpactX) * (static_cast<long double>(WorkX) - ImpactX) + (static_cast<long double>(WorkY) - ImpactY) * (static_cast<long double>(WorkY) - ImpactY)) {
                                    break;
                                }
                            }
                            if (pas::list_count(Planets) > i) {
                                aPlanet::TPlanet::HandleAsteroidImpact(Asteroid);
                                if (RecordFilm) {
                                    aGalaxy::CreateFilmEffect(u"Weapon.Asteroid"_wref.get(), 0, Effect, EffectFilm);
                                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Asteroid->Position);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                }
                                if (pas::list_count(Items) < 8) {
                                    Quantity = System::Trunc(pas::real_divide(Asteroid->MineralCount, 5.0L));
                                    Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                                    pas::checked_cast<aItem::TGoods*>(Item)->Init(aConst::t_Minerals, Quantity);
                                    pas::checked_cast<aItem::TGoods*>(Item)->NaturalFlag = true;
                                    Item->Position = Asteroid->Position;
                                    MovingDrop = static_cast<PMovingDropItemEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TMovingDropItemEntry))));
                                    MovingDrop->Payload = Item;
                                    MovingDrop->SourceShipId = 0;
                                    MovingDrop->InsertedIntoStar = false;
                                    MovingDrop->DeployTranclucator = 0;
                                    Distance = aMyFunction::SeededRandomIntRange(50, 150, GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn) * static_cast<std::uint32_t>(Item->Id));
                                    if (PlayerStar == this && Count - 20 < PathStep) {
                                        Distance = 5.0f;
                                    }
                                    {
                                        pas::Extended cpp_arg_3 = static_cast<long double>(Asteroid->Position.X) - Planet->GetPosition().X;
                                        pas::Extended cpp_arg_4 = -(static_cast<long double>(Asteroid->Position.Y) - Planet->GetPosition().Y);
                                        pas::Extended cpp_right_2 = Math::ArcTan2(cpp_arg_3, cpp_arg_4);
                                        Angle = aMyFunction::SeededRandomUnitFloat(GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn) * static_cast<std::uint32_t>(Item->Id)) * 1.2L - 0.6L + cpp_right_2;
                                    }
                                    MovingDrop->Destination.X = System::Sin(Angle) * Distance + Item->Position.X;
                                    MovingDrop->Destination.Y = Item->Position.Y - System::Cos(Angle) * Distance;
                                    pas::list_add(MovingDropItems, static_cast<void*>(MovingDrop));
                                }
                                ClearTargetReferences(Asteroid);
                                Asteroid->Respawn();
                            } else {
                                Ship = nullptr;
                                Quantity = pas::list_count(Ships);
                                for (auto cpp_range_38 = pas::for_to<std::int32_t>(0, Quantity - 1); cpp_range_38.next(i); ) {
                                    Ship = pas::list_at<aShip::TShip>(Ships, i);
                                    if (Ship->InNormalSpace() && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (aMyFunction::PointDistanceSquared(Asteroid->Position, Ship->Position) <= 2.5E+3L && (aPlayer::GetPlayer() == Ship || (Ship->ScriptShip == nullptr || Ship->HasScriptStateText()))))) {
                                        break;
                                    }
                                }
                                if (pas::list_count(Ships) <= i) {
                                    continue;
                                }
                                Damage = Ship->ApplyAsteroidImpactDamage(Asteroid, DamageColor);
                                if (RecordFilm) {
                                    aGalaxy::CreateFilmEffect(u"Weapon.Asteroid"_wref.get(), 0, Effect, EffectFilm);
                                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Asteroid->Position);
                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, nullptr, Ship->FilmObject);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, Ship->IsHullDestroyed(), true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                }
                                Quantity = System::Trunc(pas::real_divide(Asteroid->MineralCount, 4.0L));
                                Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 360, Asteroid->Id * GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn)));
                                CandidateCount = 0;
                                while (Quantity > 0) {
                                    if (Quantity < 10 || CandidateCount >= 3) {
                                        WorkCount = Quantity;
                                    } else {
                                        WorkCount = System::Round((aMyFunction::SeededRandomUnitFloat(GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn) * Asteroid->Id) * 0.2L + 0.55L) * Quantity);
                                    }
                                    Quantity -= WorkCount;
                                    Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                                    pas::checked_cast<aItem::TGoods*>(Item)->Init(aConst::t_Minerals, WorkCount);
                                    pas::checked_cast<aItem::TGoods*>(Item)->NaturalFlag = true;
                                    Item->Position = Asteroid->Position;
                                    MovingDrop = static_cast<PMovingDropItemEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TMovingDropItemEntry))));
                                    MovingDrop->Payload = Item;
                                    MovingDrop->SourceShipId = 0;
                                    MovingDrop->InsertedIntoStar = false;
                                    MovingDrop->DeployTranclucator = 0;
                                    pas::list_add(MovingDropItems, static_cast<void*>(MovingDrop));
                                    ++CandidateCount;
                                }
                                WorkValue = 3.1415925f;
                                if (CandidateCount > 1) {
                                    WorkValue = pas::real_divide(aMyFunction::GameTwoPi, CandidateCount);
                                }
                                for (auto cpp_range_39 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_39.next(CandidateIndex); ) {
                                    MovingDrop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, pas::list_count(MovingDropItems) - 1 - CandidateIndex);
                                    Distance = aMyFunction::SeededRandomIntRange(50, 150, static_cast<std::uint32_t>(pas::checked_cast<aItem::TItem*>(MovingDrop->Payload)->Id) * (GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn)));
                                    if (PlayerStar == this && Count - 20 < PathStep) {
                                        Distance = 5.0f;
                                    }
                                    WorkScale = aMyFunction::SeededRandomUnitFloat(static_cast<std::uint32_t>(pas::checked_cast<aItem::TItem*>(MovingDrop->Payload)->Id) * (GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn))) * 0.3L - 0.15L;
                                    {
                                        pas::Extended cpp_right_3 = System::Sin(aMyFunction::SeededRandomUnitFloat(static_cast<std::uint32_t>(pas::checked_cast<aItem::TItem*>(MovingDrop->Payload)->Id) * (GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn))) * (static_cast<long double>(Angle) + WorkScale) * 113.0L) * Distance;
                                        MovingDrop->Destination.X = pas::checked_cast<aItem::TItem*>(MovingDrop->Payload)->Position.X + cpp_right_3;
                                    }
                                    {
                                        pas::Extended cpp_right_4 = System::Cos(aMyFunction::SeededRandomUnitFloat(static_cast<std::uint32_t>(pas::checked_cast<aItem::TItem*>(MovingDrop->Payload)->Id) * (GenerationSeed * static_cast<std::uint32_t>(Galaxy->CurrentTurn))) * (static_cast<long double>(Angle) + WorkScale) * 517.0L) * Distance;
                                        MovingDrop->Destination.Y = pas::checked_cast<aItem::TItem*>(MovingDrop->Payload)->Position.Y - cpp_right_4;
                                    }
                                    Angle = static_cast<long double>(Angle) + WorkValue;
                                }
                                ClearTargetReferences(Asteroid);
                                Asteroid->Respawn();
                            }
                        }
                    }
                }
                Stage = 29300;
                EntryCount = pas::list_count(CombatEvents);
                for (auto cpp_range_40 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_40.next(EntryIndex); ) {
                    Stage = 2930;
                    CombatEvent = pas::list_at<TStarCombatEvent>(CombatEvents, EntryIndex);
                    if (CombatEvent->StepIndex == PathStep && (CombatEvent->Attacker != nullptr && (CombatEvent->Target != nullptr && (static_cast<std::uint8_t>(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->IsHullDestroyed() ^ 1) && (static_cast<std::uint8_t>(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->DestroyQueued ^ 1) && (CombatEvent->Weapon != nullptr && reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->EquippedFlag != 0)))))) {
                        if (!(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->GetCombatStatusStrength(aShip::cseWeaponBlock) <= 0.01L)) {
                            {
                                pas::Extended cpp_right_5 = reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->GetCombatStatusStrength(aShip::cseWeaponBlock);
                                if (aMyFunction::NextRandomUnitFloat(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->RandomState) < cpp_right_5) {
                                    reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->ReduceCombatStatusStrength(aShip::cseWeaponBlock, 1.0f);
                                    continue;
                                }
                            }
                            reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->ReduceCombatStatusStrength(aShip::cseWeaponBlock, 1.0f);
                        }
                        if (pas::in_range(reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                            Stage = 2931;
                            if (reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->Ammo > 0) {
                                if (pas::class_cast_if<aShip::TShip*>(CombatEvent->Target) != nullptr) {
                                    Point = pas::checked_cast<aShip::TShip*>(CombatEvent->Target)->Position;
                                } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(CombatEvent->Target) != nullptr) {
                                    Point = pas::checked_cast<aAsteroid::TAsteroid*>(CombatEvent->Target)->Position;
                                } else if (pas::class_cast_if<aItem::TItem*>(CombatEvent->Target) != nullptr) {
                                    Point = pas::checked_cast<aItem::TItem*>(CombatEvent->Target)->Position;
                                } else if (pas::class_cast_if<aMissile::TMissile*>(CombatEvent->Target) != nullptr) {
                                    Point = pas::checked_cast<aMissile::TMissile*>(CombatEvent->Target)->Position;
                                } else {
                                    Point = EC_Struct::MakePointF(1.0E+6f, 1.0E+6f);
                                }
                                if (static_cast<long double>(pas::sqr(aShip::TShip_GetWeaponActionRange(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)) + 200)) > aMyFunction::PointDistanceSquared(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->Position, Point)) {
                                    Quantity = 1;
                                    AttackCount = reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->GetAttackCount();
                                    if (pas::in_range(reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstMissile), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                                        Quantity = reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->GetShotCount();
                                    }
                                    BertorBoost = pas::class_cast_if<aKling::TKling*>(CombatEvent->Attacker) != nullptr && pas::checked_cast<aKling::TKling*>(CombatEvent->Attacker)->DominatorSeries == aGalaxyStruct::dsBlazer && pas::checked_cast<aKling::TKling*>(CombatEvent->Attacker)->HasNearbyBertorAura();
                                    for (auto cpp_range_41 = pas::for_to<std::int32_t>(1, ([&] {
                                        std::int32_t cpp_right_6 = (reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->CanBoostArtefact(aConst::t_ArtFastRacks, reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), false) & 127) + 1;
                                        return ([&] {
                                            std::int32_t cpp_right_7 = reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->CountActiveArtefacts(aConst::t_ArtFastRacks);
                                            return reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->GetAttackCount() * cpp_right_7;
                                        }()) * cpp_right_6;
                                    }())); cpp_range_41.next(ArtefactIndex); ) {
                                        if (([&] {
                                            pas::Extended cpp_left = aMyFunction::NextRandomUnitFloat(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->RandomState);
                                            return cpp_left <= aConst::ExtraMissileChance;
                                        }()) && pas::class_cast_if<aShip::TShip*>(CombatEvent->Target) != nullptr) {
                                            ++AttackCount;
                                        }
                                    }
                                    if (reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->Ammo < AttackCount) {
                                        AttackCount = std::max<std::int32_t>(1, reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->Ammo);
                                    }
                                    Quantity *= AttackCount;
                                    if (reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->TypeId != aGalaxyStruct::stKling && (aPlayer::GetPlayer() != CombatEvent->Attacker || Galaxy->AmmoModEnabled != 1)) {
                                        RemainingAmmo = reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->Ammo - AttackCount;
                                        reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->Ammo = RemainingAmmo;
                                        if (aPlayer::GetPlayer() == CombatEvent->Attacker) {
                                            SysUtilsImports::Sleep(1u);
                                            if (reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)->Ammo != RemainingAmmo && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                                                GR_Main::CCInterface->SetTamperDetected(true);
                                            }
                                        }
                                    }
                                    for (auto cpp_range_42 = pas::for_to<std::int32_t>(0, Quantity - 1); cpp_range_42.next(i); ) {
                                        if (pas::class_cast_if<aItem::TCustomWeapon*>(CombatEvent->Weapon) != nullptr) {
                                            Missile = pas::construct_call<aMissile::TCustomMissile>(aMissile::TMissile_Create);
                                            reinterpret_cast<aMissile::TCustomMissile*>(Missile)->InitializeShot_2(this, reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), CombatEvent->Target, i + (Quantity % 2 == 0));
                                        } else {
                                            Missile = pas::construct_call<aMissile::TMissile>(aMissile::TMissile_Create);
                                            Missile->InitializeShot(this, reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), CombatEvent->Target, i + (Quantity % 2 == 0));
                                        }
                                        if (BertorBoost) {
                                            if (RecordingTurnFilm && pas::checked_cast<aKling::TKling*>(CombatEvent->Attacker)->AuraEffectShownThisTurn == 0) {
                                                aGalaxy::CreateFilmEffect(u"Weapon.AuraEffect"_wref.get(), 0, Effect, EffectFilm);
                                                Globals::PrimaryFilm->SetWeaponEndpoints(CurrentStepIndex, EffectFilm, reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->FilmObject, reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->FilmObject);
                                                Globals::PrimaryFilm->SetWeaponHit(CurrentStepIndex, EffectFilm, 0, 0, false, true);
                                                Globals::PrimaryFilm->AttachObject(CurrentStepIndex, EffectFilm);
                                                pas::checked_cast<aKling::TKling*>(CombatEvent->Attacker)->AuraEffectShownThisTurn = true;
                                            }
                                            Missile->MinDamage = static_cast<std::uint32_t>(System::Round(Missile->MinDamage * 1.25L));
                                            Missile->MaxDamage = static_cast<std::uint32_t>(System::Round(Missile->MaxDamage * 1.25L));
                                        }
                                        reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->ScriptItemsAct(aGalaxyStruct::satOnMissileShot, Missile, reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), 0);
                                        Missile->PrepareTurnMovement(StepIndex, RecordFilm, true);
                                        if (RecordFilm) {
                                            Globals::PrimaryFilm->DetachObject(0, Missile->FilmObject);
                                        }
                                    }
                                }
                            }
                        } else if (pas::class_cast_if<aMissile::TMissile*>(CombatEvent->Target) != nullptr) {
                            Stage = 2932;
                            {
                                pas::Extended cpp_right_8 = 1.3L * pas::sqr(aShip::TShip_GetWeaponRange(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)));
                                if (aMyFunction::PointDistanceSquared(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->Position, pas::checked_cast<aMissile::TMissile*>(CombatEvent->Target)->Position) < cpp_right_8) {
                                    aShip::TShip_FireWeaponAtMissile(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), CombatEvent->Target, RecordFilm);
                                }
                            }
                        } else if (pas::class_cast_if<aShip::TShip*>(CombatEvent->Target) != nullptr) {
                            Stage = 2933;
                            {
                                pas::Extended cpp_right_9 = 1.3L * pas::sqr(aShip::TShip_GetWeaponRange(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)));
                                if (aMyFunction::PointDistanceSquared(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->Position, pas::checked_cast<aShip::TShip*>(CombatEvent->Target)->Position) < cpp_right_9) {
                                    aShip::TShip_FireWeaponAtShip(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), reinterpret_cast<aShip::TShip*>(CombatEvent->Target), RecordFilm);
                                }
                            }
                        } else if (pas::class_cast_if<aItem::TItem*>(CombatEvent->Target) != nullptr) {
                            Stage = 2934;
                            {
                                pas::Extended cpp_right_10 = 1.3L * pas::sqr(aShip::TShip_GetWeaponRange(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)));
                                if (aMyFunction::PointDistanceSquared(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->Position, pas::checked_cast<aItem::TItem*>(CombatEvent->Target)->Position) < cpp_right_10) {
                                    reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->FireWeaponAtItem(reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), reinterpret_cast<aItem::TItem*>(CombatEvent->Target), RecordFilm);
                                }
                            }
                        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(CombatEvent->Target) != nullptr) {
                            Stage = 2935;
                            {
                                pas::Extended cpp_right_11 = 1.3L * pas::sqr(aShip::TShip_GetWeaponRange(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker), reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon)));
                                if (aMyFunction::PointDistanceSquared(reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->Position, pas::checked_cast<aAsteroid::TAsteroid*>(CombatEvent->Target)->Position) < cpp_right_11) {
                                    reinterpret_cast<aShip::TShip*>(CombatEvent->Attacker)->FireWeaponAtAsteroid(reinterpret_cast<aItem::TWeapon*>(CombatEvent->Weapon), CombatEvent->Target, RecordFilm);
                                }
                            }
                        }
                        continue;
                    }
                }
                Stage = 2940;
                EntryCount = pas::list_count(MovingDropItems);
                for (auto cpp_range_43 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_43.next(EntryIndex); ) {
                    MovingDrop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, EntryIndex);
                    if (MovingDrop->Payload != nullptr) {
                        if (pas::class_cast_if<aItem::TItem*>(MovingDrop->Payload) != nullptr) {
                            Item = pas::checked_cast<aItem::TItem*>(MovingDrop->Payload);
                            if (!MovingDrop->InsertedIntoStar) {
                                if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item); artefactTranclucator != nullptr && MovingDrop->DeployTranclucator != 0) {
                                    Tranclucator = pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator->Ship));
                                    artefactTranclucator->Ship = nullptr;
                                    Tranclucator->CurrentStar = this;
                                    pas::list_add(Ships, reinterpret_cast<void*>(Tranclucator));
                                    Tranclucator->Position = Item->Position;
                                    Tranclucator->MovementDirection = 0.0;
                                    pas::free(Item);
                                    MovingDrop->Payload = Tranclucator;
                                    MovingDrop->InsertedIntoStar = true;
                                    if (RecordFilm) {
                                        Tranclucator->FilmObject = Globals::PrimaryFilm->AddObject(Tranclucator->Id, Tranclucator->Graphic, 0, 0);
                                        Globals::PrimaryFilm->DetachObject(0, Tranclucator->FilmObject);
                                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, Tranclucator->FilmObject, Position);
                                        Globals::PrimaryFilm->SetObjectAngle(StepIndex, Tranclucator->FilmObject, 0);
                                        Globals::PrimaryFilm->SetObjectAlpha(StepIndex, Tranclucator->FilmObject, 255);
                                        Globals::PrimaryFilm->AttachObject(StepIndex, Tranclucator->FilmObject);
                                    }
                                } else {
                                    pas::list_add(Items, reinterpret_cast<void*>(Item));
                                    MovingDrop->InsertedIntoStar = true;
                                    if (RecordFilm) {
                                        {
                                            SE_Space::TObjectSE* graphObject_2 = Item->GetGraphObject();
                                            std::uint32_t id_2 = Item->Id;
                                            aEFilm::TEFilm* primaryFilm_2 = Globals::PrimaryFilm;
                                            Item->FilmObject = primaryFilm_2->AddObject(id_2, graphObject_2, 0, 0);
                                        }
                                        Globals::PrimaryFilm->DetachObject(0, Item->FilmObject);
                                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, Item->FilmObject, Item->Position);
                                        Globals::PrimaryFilm->AttachObject(StepIndex, Item->FilmObject);
                                    }
                                }
                            } else {
                                Item->Position.X = pas::real_divide(static_cast<long double>(MovingDrop->Destination.X) - Item->Position.X, Count - PathStep) + Item->Position.X;
                                Item->Position.Y = pas::real_divide(static_cast<long double>(MovingDrop->Destination.Y) - Item->Position.Y, Count - PathStep) + Item->Position.Y;
                                if (RecordFilm) {
                                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, Item->FilmObject, Item->Position);
                                }
                                if (static_cast<long double>(DamageRadius) * DamageRadius > pas::sqr(static_cast<pas::Extended>(Item->Position.X)) + pas::sqr(static_cast<pas::Extended>(Item->Position.Y))) {
                                    if (RecordFilm) {
                                        aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                        Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, nullptr, Item->FilmObject);
                                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, true, true);
                                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                                        pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                                    }
                                    pas::list_delete(Items, pas::list_indexof(Items, reinterpret_cast<void*>(Item)));
                                    pas::free(Item);
                                    MovingDrop->Payload = nullptr;
                                }
                            }
                        } else {
                            HitShip = pas::checked_cast<aShip::TShip*>(MovingDrop->Payload);
                            HitShip->Position.X = pas::real_divide(static_cast<long double>(MovingDrop->Destination.X) - HitShip->Position.X, Count - PathStep) + HitShip->Position.X;
                            HitShip->Position.Y = pas::real_divide(static_cast<long double>(MovingDrop->Destination.Y) - HitShip->Position.Y, Count - PathStep) + HitShip->Position.Y;
                            if (RecordFilm) {
                                Globals::PrimaryFilm->SetObjectPosition(StepIndex, HitShip->FilmObject, HitShip->Position);
                            }
                        }
                    }
                }
                Stage = 2950;
                if (Galaxy->StasisModEnabled != 1) {
                    for (auto cpp_range_44 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_44.next(Index); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                        Planet->AdvanceOrbitStep(StepIndex, RecordFilm);
                    }
                }
                Stage = 2951;
                if (Galaxy->StasisModEnabled != 1) {
                    for (auto cpp_range_45 = pas::for_to<std::int32_t>(0, pas::list_count(Asteroids) - 1); cpp_range_45.next(Index); ) {
                        Asteroid = pas::list_at<aAsteroid::TAsteroid>(Asteroids, Index);
                        Asteroid->AdvanceOrbitStep(StepIndex, RecordFilm);
                    }
                }
                Stage = 2952;
                if (pas::imod(StepIndex, Count / 11) == 0) {
                    if (pas::idiv(StepIndex, Count / 11) >= 1) {
                        if (pas::idiv(StepIndex, Count / 11) <= 10) {
                            ProcessItemScripts(pas::idiv(StepIndex, Count / 11));
                        }
                    }
                }
                Stage = 2960;
                if (pas::imod(PathStep + 1, static_cast<std::int32_t>(static_cast<std::uint32_t>(Count) >> 3)) == 0) {
                    for (auto cpp_range_46 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_46.next(Index); ) {
                        Ship = pas::list_at<aShip::TShip>(Ships, Index);
                        if (Ship->InNormalSpace() && (aPlayer::GetPlayer() != Ship || static_cast<std::uint8_t>(GlobalsV::CurrentScreenId) == 16)) {
                            for (auto cpp_range_47 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->Inventory) - 1); cpp_range_47.next(i); ) {
                                Item = pas::list_at<aItem::TItem>(Ship->Inventory, i);
                                if (Item->DestroyFlag > 0) {
                                    while (Ship->DockedTo != nullptr) {
                                        Ship = Ship->DockedTo;
                                    }
                                    Ship->DestroyQueued = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                Stage = 2961;
                Index = 0;
                while (pas::list_count(Items) > Index) {
                    Item = pas::list_at<aItem::TItem>(Items, Index);
                    if (Item->DestroyFlag > 0) {
                        pas::list_delete(Items, Index);
                        --Index;
                        if (RecordFilm) {
                            aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Item->FilmObject, Item->FilmObject);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, true, true);
                            if (Item->DestroyFlag == 1) {
                                Globals::PrimaryFilm->SetDestructionEffect(StepIndex, EffectFilm, 3);
                            } else {
                                Globals::PrimaryFilm->SetDestructionEffect(StepIndex, EffectFilm, 1);
                            }
                            Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                        }
                        ClearItemReferences(Item);
                        if (Item->DestroyFlag > 1 || (Item->ItemType == aConst::t_ArtefactBomb || pas::class_cast_if<aItem::TCistern*>(Item) != nullptr && static_cast<aItem::TCistern*>(Item)->Fuel > 0)) {
                            CandidateCount = pas::list_count(Ships);
                            for (auto cpp_range_48 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_48.next(CandidateIndex); ) {
                                Ship = pas::list_at<aShip::TShip>(Ships, CandidateIndex);
                                if (Ship->InNormalSpace() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1)) {
                                    Distance = aMyFunction::PointDistanceSquared(Ship->Position, Item->Position);
                                    if (static_cast<long double>(pas::sqr(aConst::BombDamageRadius)) >= Distance && (aPlayer::GetPlayer() != Ship || Galaxy->GodModEnabled != 1 && Galaxy->SpecialSimulationMode == 0)) {
                                        Damage = aShip::TShip_ApplyExplosionDamage(Ship, nullptr, Item, 0, nullptr);
                                        DamageColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                                        if (RecordFilm) {
                                            aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                            if (Item->DestroyFlag > 0) {
                                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, Ship->IsHullDestroyed(), true);
                                            } else {
                                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, Ship->IsHullDestroyed(), true);
                                            }
                                            Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                        }
                                    }
                                }
                            }
                            CandidateCount = pas::list_count(Items);
                            for (auto cpp_range_49 = pas::for_to<std::int32_t>(0, CandidateCount - 1); cpp_range_49.next(CandidateIndex); ) {
                                OtherItem = pas::list_at<aItem::TItem>(Items, CandidateIndex);
                                Distance = aMyFunction::PointDistanceSquared(OtherItem->Position, Item->Position);
                                if (static_cast<long double>(aConst::ItemExplosionRadiusSquared) >= Distance) {
                                    pas::Extended cpp_right_12 = aMyFunction::NextRandomIntRange(1, 100, RandomState);
                                    if (2.0E+1L - pas::real_divide(2.0E+1L * Distance, aConst::ItemExplosionRadiusSquared) >= cpp_right_12) {
                                        if (OtherItem->DestroyFlag < 0) {
                                            ++OtherItem->DestroyFlag;
                                        } else {
                                            pas::list_add(ReferencedItems, reinterpret_cast<void*>(OtherItem));
                                        }
                                    }
                                }
                            }
                        }
                        if (RecordFilm) {
                            pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                        }
                        pas::free(Item);
                    }
                    ++Index;
                }
                Index = 0;
                Stage = 2970;
                while (pas::list_count(Ships) > Index) {
                    Ship = pas::list_at<aShip::TShip>(Ships, Index);
                    if (Ship->IsHullDestroyed()) {
                        ++Index;
                        continue;
                    }
                    Stage = 2971;
                    if (Ship->DestroyQueued && (static_cast<std::uint32_t>(Count) >> 2 == static_cast<std::uint32_t>(PathStep) && (aPlayer::GetPlayer() != Ship || static_cast<std::uint8_t>(GlobalsV::CurrentScreenId) == 16))) {
                        Ship->ScriptItemsAct(aGalaxyStruct::satOnDeath, nullptr, nullptr, 0);
                        Ship->GetHull()->HullPoints = 0;
                        if (aPlayer::GetPlayer() != nullptr) {
                            aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), Ship);
                        }
                        Ship->RefreshDerivedStats(true);
                        if (RecordFilm && Ship->FilmObject != nullptr) {
                            if (Ship->InNormalSpace()) {
                                aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, Ship->IsHullDestroyed(), true);
                                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                            } else {
                                Globals::PrimaryFilm->DetachObject(StepIndex, Ship->FilmObject);
                            }
                        }
                        ClearShipReferences(Ship);
                        ++Index;
                        continue;
                    }
                    Stage = 2972;
                    if (pas::imod(StepIndex, Count / 5) == 0 && pas::idiv(StepIndex, Count / 5) >= 1) {
                        if (pas::idiv(StepIndex, Count / 5) <= 3 && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (Ship->InNormalSpace() && (aPlayer::GetPlayer() != Ship || Galaxy->GodModEnabled != 1 && Galaxy->SpecialSimulationMode == 0)) && (aPlayer::GetPlayer() == Ship || Galaxy->StasisModEnabled != 1))) {
                            Distance = pas::sqr(static_cast<pas::Extended>(Ship->Position.X)) + pas::sqr(static_cast<pas::Extended>(Ship->Position.Y));
                            if (static_cast<long double>(DamageRadius) * DamageRadius > Distance) {
                                if (aPlayer::GetPlayer() == Ship) {
                                    PlayerCombatOccurred = true;
                                }
                                Damage = Ship->ApplyStarHeatDamage();
                                if (RecordFilm && static_cast<std::int32_t>(Damage) > 0) {
                                    aGalaxy::CreateFilmEffect(u"Weapon.Star"_wref.get(), 0, Effect, EffectFilm);
                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255), Damage, Ship->IsHullDestroyed(), true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                }
                                Ship->InterceptorPassesRemaining = 0;
                                if (Ship->IsHullDestroyed()) {
                                    ++Index;
                                    continue;
                                }
                            }
                        }
                    }
                    Stage = 29731;
                    if (pas::imod(StepIndex, Count / 5) == 0 && pas::idiv(StepIndex, Count / 5) >= 1) {
                        if (pas::idiv(StepIndex, Count / 5) <= 3 && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (Ship->InNormalSpace() && (aPlayer::GetPlayer() != Ship || Galaxy->GodModEnabled != 1 && Galaxy->SpecialSimulationMode == 0)) && ((aPlayer::GetPlayer() == Ship || Galaxy->StasisModEnabled != 1) && Ship->InterceptorPassesRemaining > 0))) {
                            if (aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->InterceptorSourceShip) {
                                PlayerCombatOccurred = true;
                                if (aPlayer::GetPlayer() == Ship->InterceptorSourceShip) {
                                    Globals::PrimaryFilm->AddCameraEvent(StepIndex, aPlayer::GetPlayer()->Position, Ship->Position, 1);
                                }
                                if (aPlayer::GetPlayer() == Ship && Ship->InterceptorSourceShip != nullptr) {
                                    Globals::PrimaryFilm->AddCameraEvent(StepIndex, aPlayer::GetPlayer()->Position, Ship->InterceptorSourceShip->Position, 1);
                                }
                            }
                            Damage = Ship->ApplyInterceptorDamage(DamageColor);
                            if (RecordFilm) {
                                aGalaxy::CreateFilmEffect(u"Weapon.Star"_wref.get(), 0, Effect, EffectFilm);
                                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, Ship->IsHullDestroyed(), true);
                                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                            }
                            if (Ship->IsHullDestroyed()) {
                                ++Index;
                                continue;
                            }
                        }
                    }
                    Stage = 29732;
                    if (Galaxy->StasisModEnabled != 1 && (4 * (Count / 5) == StepIndex && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (Ship->InNormalSpace() && Ship->InterceptorSourceShip != nullptr)))) {
                        if (Ship->InterceptorSourceShip->GetHull()->Energy >= 3) {
                            Ship->InterceptorSourceShip->GetHull()->Energy = Ship->InterceptorSourceShip->GetHull()->Energy - 3;
                            --Ship->InterceptorPassesRemaining;
                        } else {
                            Ship->InterceptorSourceShip->GetHull()->Energy = 0;
                            Ship->InterceptorPassesRemaining = 0;
                        }
                        if (Ship->InterceptorPassesRemaining <= 0) {
                            Ship->InterceptorSourceShip = nullptr;
                        }
                    }
                    Stage = 29733;
                    if (4 * (Count / 5) == StepIndex) {
                        if (System::Round(Ship->GetCombatStatusStrength(aShip::cseShock)) >= 1 && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (Ship->InNormalSpace() && (aPlayer::GetPlayer() != Ship || Galaxy->GodModEnabled != 1 && Galaxy->SpecialSimulationMode == 0)) && (aPlayer::GetPlayer() == Ship || Galaxy->StasisModEnabled != 1))) {
                            Damage = Ship->ApplyShockStatusDamage(DamageColor);
                            if (RecordFilm) {
                                aGalaxy::CreateFilmEffect(u"Weapon.Shock"_wref.get(), 0, Effect, EffectFilm);
                                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, Ship->IsHullDestroyed(), true);
                                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                            }
                        }
                    }
                    if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship)) {
                        if (kling->ShouldKamikaze() && static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && Ship->InNormalSpace()) {
                            if (Ship->OrderTarget != nullptr) {
                                if (aShip::TShip* ship_2 = pas::class_cast_if<aShip::TShip*>(Ship->OrderTarget)) {
                                    if (!ship_2->IsHullDestroyed()) {
                                        if (ship_2->InNormalSpace()) {
                                            if (aMyFunction::PointDistance(Ship->Position, ship_2->Position) <= 1.0E+2L) {
                                                Ship->GetHull()->HullPoints = 0;
                                                ClearShipReferences(Ship);
                                                Ship->ScriptItemsAct(aGalaxyStruct::satOnDeath, Ship, Ship, 0);
                                                if (aPlayer::GetPlayer() != nullptr) {
                                                    aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), Ship);
                                                }
                                                for (auto cpp_range_50 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_50.next(CandidateIndex); ) {
                                                    OwnerShip = pas::list_at<aShip::TShip>(Ships, CandidateIndex);
                                                    if (static_cast<std::uint8_t>(OwnerShip->IsHullDestroyed() ^ 1) && (OwnerShip->InNormalSpace() && (aMyFunction::PointDistanceSquared(Ship->Position, OwnerShip->Position) <= 2.25E+4L && (aPlayer::GetPlayer() != OwnerShip || Galaxy->GodModEnabled != 1 && Galaxy->SpecialSimulationMode == 0)))) {
                                                        if (aKling::TKling* kling_2 = pas::class_cast_if<aKling::TKling*>(OwnerShip); !(kling_2 != nullptr) || ([&] {
                                                            aGalaxyStruct::TDominatorSeries cpp_left_2 = kling_2->DominatorSeries;
                                                            return cpp_left_2 != pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries;
                                                        }())) {
                                                            Damage = aShip::TShip_ApplyExplosionDamage(OwnerShip, nullptr, Ship, 0, nullptr);
                                                            DamageColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                                                            if (RecordFilm) {
                                                                aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                                                                Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, OwnerShip->FilmObject, OwnerShip->FilmObject);
                                                                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, DamageColor, Damage, OwnerShip->IsHullDestroyed(), true);
                                                                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                                            }
                                                        }
                                                    }
                                                }
                                                if (RecordFilm) {
                                                    Globals::PrimaryFilm->SetObjectAlpha(StepIndex, Ship->FilmObject, 0);
                                                    aGalaxy::CreateFilmEffect(u"Weapon.Kamikaze"_wref.get(), 0, Effect, EffectFilm);
                                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, true, true);
                                                    Globals::PrimaryFilm->SetDestructionEffect(StepIndex, EffectFilm, 5);
                                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Stage = 29734;
                    if (pas::imod(StepIndex, Count / 9) == 0 && (pas::class_cast_if<aKling::TKling*>(Ship) != nullptr && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (Ship->InNormalSpace() && Galaxy->StasisModEnabled != 1)))) {
                        if (RecordFilm && (static_cast<aKling::TKling*>(Ship)->KlingType == aGalaxyStruct::ktBertor && pas::in_set<1, 1, 3, 3>(pas::idiv(StepIndex, Count / 9)))) {
                            Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.RadialEffect"_wref.get(), ClassesImports::Point(0, 0), static_cast<std::int32_t>(pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries), -1);
                            EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                            Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                        }
                        if (pas::idiv(StepIndex, Count / 9) == 3 && pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries == aGalaxyStruct::dsTerron) {
                            if (static_cast<aKling::TKling*>(Ship)->HasNearbyBertorAura()) {
                                Damage = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(Ship->GetHull()->Weight * 0.05L));
                                Ship->GetHull()->HullPoints = std::min<std::int32_t>(Ship->GetHull()->Weight, static_cast<std::int32_t>(Damage) + Ship->GetHull()->HullPoints);
                                if (RecordFilm) {
                                    aGalaxy::CreateFilmEffect(u"Weapon.AuraEffect"_wref.get(), 2, Effect, EffectFilm);
                                    Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, aConst::OwnerToFilmColor(Ship->OwnerId), -Damage, false, true);
                                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                }
                            }
                        }
                        if (pas::idiv(StepIndex, Count / 9) == 3 && (pas::checked_cast<aKling::TKling*>(Ship)->KlingType == aGalaxyStruct::ktBertor && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && Ship->InNormalSpace()))) {
                            for (auto cpp_range_51 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_51.next(CandidateIndex); ) {
                                OwnerShip = pas::list_at<aShip::TShip>(Ships, CandidateIndex);
                                if (static_cast<std::uint8_t>(OwnerShip->IsHullDestroyed() ^ 1) && OwnerShip->InNormalSpace() && Ship != OwnerShip && (!(pas::class_cast_if<aKling::TKling*>(OwnerShip) != nullptr) || static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::ktBoss, aGalaxyStruct::ktBertor>(static_cast<aKling::TKling*>(OwnerShip)->KlingType) ^ 1)) && aMyFunction::PointDistance(Ship->Position, OwnerShip->Position) <= 5.0E+2L) {
                                    for (auto cpp_range_52 = pas::for_to<std::int32_t>(0, pas::list_count(OwnerShip->Inventory) - 1); cpp_range_52.next(EntryIndex); ) {
                                        Item = pas::list_at<aItem::TItem>(OwnerShip->Inventory, EntryIndex);
                                        if (Item->OwnerId == aGalaxyStruct::oiDominator) {
                                            if (([&] {
                                                aGalaxyStruct::TDominatorSeries cpp_left_3 = pas::checked_cast<aItem::TEquipment*>(Item)->DominatorSeries;
                                                return cpp_left_3 != pas::checked_cast<aKling::TKling*>(Ship)->DominatorSeries;
                                            }()) && (pas::in_set<aConst::t_FuelTanks, aConst::t_CustomWeapon, aConst::t_Satellite, aConst::t_Satellite>(Item->ItemType) && static_cast<aItem::TEquipment*>(Item) != nullptr)) {
                                                if (static_cast<aItem::TEquipment*>(Item)->EquippedFlag != 0) {
                                                    if (static_cast<aItem::TEquipment*>(Item)->BrokenFlag == 0) {
                                                        if (static_cast<aItem::TEquipment*>(Item)->CustomFaction == u"") {
                                                            double nextRandomIntRange = aMyFunction::NextRandomIntRange(5, 10, Ship->RandomState);
                                                            aItem::TEquipment* item = reinterpret_cast<aItem::TEquipment*>(Item);
                                                            aShip::TShip_ApplyItemDegradation(OwnerShip, item, aShip::idkForce, nextRandomIntRange);
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
                    Stage = 29735;
                    if (pas::imod(StepIndex, pas::idiv(Count, aConst::PointDefensePassCount + 2)) == 0) {
                        if (pas::idiv(StepIndex, pas::idiv(Count, aConst::PointDefensePassCount + 2)) >= 1) {
                            if (pas::idiv(StepIndex, pas::idiv(Count, aConst::PointDefensePassCount + 2)) <= aConst::PointDefensePassCount && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (Ship->InNormalSpace() && (Ship->CountActiveArtefacts(aConst::t_ArtPDTurret) > 0 && (aPlayer::GetPlayer() == Ship || Galaxy->StasisModEnabled != 1))))) {
                                for (auto cpp_range_53 = pas::for_to<std::int32_t>(1, Ship->CountActiveArtefacts(aConst::t_ArtPDTurret)); cpp_range_53.next(ArtefactIndex); ) {
                                    CandidateIndex = 0;
                                    InterceptedMissile = nullptr;
                                    NearestMissileDistance = 0;
                                    BestMissilePriority = -1;
                                    MissilePriority = 0;
                                    PointDefenseRangeSquared = (aConst::PointDefenseBaseRange + aConst::PointDefenseBonusRange) * (aConst::PointDefenseBaseRange + aConst::PointDefenseBonusRange * (Ship->CanBoostArtefact(aConst::t_ArtPDTurret, nullptr, false) & 127));
                                    while (pas::list_count(Missiles) > CandidateIndex) {
                                        Missile = pas::list_at<aMissile::TMissile>(Missiles, CandidateIndex);
                                        ++CandidateIndex;
                                        if (Missile->OwnerShip != Ship && (!(pas::class_cast_if<aTranclucator::TTranclucator*>(Ship) != nullptr) || (static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == nullptr || (Missile->OwnerShip == nullptr || static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip != Missile->OwnerShip && (!(pas::class_cast_if<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Missile->OwnerShip)) != nullptr) || static_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip != reinterpret_cast<aTranclucator::TTranclucator*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Missile->OwnerShip)))))->OwnerShip))))) {
                                            MissileDistance = System::Round(aMyFunction::PointDistanceSquared(Ship->Position, Missile->Position));
                                            if (MissileDistance <= PointDefenseRangeSquared) {
                                                if (Missile->Target == Ship) {
                                                    MissilePriority = 3;
                                                } else if (Missile->Target != nullptr && pas::class_cast_if<aShip::TShip*>(Missile->Target) != nullptr && static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Missile->Target)) != static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Missile->OwnerShip)) && aShip::TShip_GetRelationLevelToShip(pas::checked_cast<aShip::TShip*>(Missile->Target), Ship) > aGalaxyStruct::rlHostile && aShip::TShip_GetRelationLevelToShip(Ship, pas::checked_cast<aShip::TShip*>(Missile->Target)) > aGalaxyStruct::rlHostile) {
                                                    MissilePriority = 2;
                                                } else if (Missile->OwnerShip != nullptr && (aShip::TShip_GetRelationLevelToShip(Missile->OwnerShip, Ship) <= aGalaxyStruct::rlHostile || aShip::TShip_GetRelationLevelToShip(Ship, Missile->OwnerShip) <= aGalaxyStruct::rlHostile)) {
                                                    MissilePriority = 1;
                                                } else {
                                                    continue;
                                                }
                                                if (MissilePriority >= BestMissilePriority && (MissilePriority != BestMissilePriority || MissileDistance <= NearestMissileDistance)) {
                                                    NearestMissileDistance = MissileDistance;
                                                    BestMissilePriority = MissilePriority;
                                                    InterceptedMissile = Missile;
                                                }
                                            }
                                        }
                                    }
                                    if (InterceptedMissile != nullptr) {
                                        if (RecordFilm) {
                                            aGalaxy::CreateFilmEffect(u"Weapon.PDTurret"_wref.get(), 0, Effect, EffectFilm);
                                            Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, InterceptedMissile->FilmObject);
                                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                                            Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                            aGalaxy::CreateFilmEffect(u"Weapon.Asteroid"_wref.get(), 0, Effect, EffectFilm);
                                            Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, InterceptedMissile->Position);
                                            Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                                            Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                                            Globals::PrimaryFilm->DetachObject(StepIndex, InterceptedMissile->FilmObject);
                                            pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(InterceptedMissile->FilmObject));
                                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&InterceptedMissile->Graphic));
                                        }
                                        pas::free(InterceptedMissile);
                                    }
                                }
                            }
                        }
                    }
                    Stage = 2974;
                    CanPull = 1;
                    if (aPlayer::GetPlayer() == Ship && Ship->PickupTargets != nullptr && Ship->GetCargoHook() != nullptr && Ship->MovementPath->NodeCount != 0) {
                        CanPull = 0;
                        for (auto cpp_range_54 = pas::for_to<std::int32_t>(0, pas::list_count(Ship->PickupTargets) - 1); cpp_range_54.next(CandidateIndex); ) {
                            Target = pas::list_get(Ship->PickupTargets, CandidateIndex);
                            ClosestNodeIndex = 0;
                            if (pas::list_indexof(Items, Target) >= 0) {
                                Item = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Target));
                                Node = Ship->MovementPath->ActiveHead;
                                ClosestDistance = aMyFunction::PointDistance(Item->Position, Node->Position);
                                if (static_cast<long double>(Ship->GetCargoHookRange()) < ClosestDistance) {
                                    continue;
                                }
                                ClosestNodeIndex = 0;
                                Node = Node->Next;
                                for (auto cpp_range_55 = pas::for_to<std::int32_t>(1, Ship->MovementPath->NodeCount - 1); cpp_range_55.next(NodeIndex); ) {
                                    Distance = aMyFunction::PointDistance(Item->Position, Node->Position);
                                    if (static_cast<long double>(Ship->GetCargoHookRange()) >= Distance) {
                                        if (Distance >= ClosestDistance) {
                                            break;
                                        }
                                        if (Distance < ClosestDistance) {
                                            ClosestDistance = Distance;
                                            ClosestNodeIndex = NodeIndex;
                                            break;
                                        }
                                        Node = Node->Next;
                                    }
                                }
                            }
                            if (ClosestNodeIndex == 0) {
                                CanPull = 1;
                                break;
                            }
                        }
                    }
                    Stage = 2975;
                    if (!(Galaxy->StasisModEnabled == 1 && aPlayer::GetPlayer() != Ship || (Ship->PickupTargets == nullptr || (Ship->GetCargoHook() == nullptr || CanPull == 0)))) {
                        CanPull = 0;
                        PulledItemCount = 0;
                        {
                            const std::int32_t cpp_first_3 = pas::list_count(Ship->PickupTargets) - 1;
                            if (cpp_first_3 >= 0) {
                                for (CandidateIndex = cpp_first_3; CandidateIndex >= 0; --CandidateIndex) {
                                    Target = pas::list_get(Ship->PickupTargets, CandidateIndex);
                                    if (pas::list_indexof(Items, Target) >= 0) {
                                        Item = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(Target));
                                        Distance = aMyFunction::PointDistance(Item->Position, Ship->Position);
                                        if (static_cast<long double>(Ship->GetCargoHookRange()) < Distance) {
                                            continue;
                                        }
                                        if (!Ship->PickupPathUpdatesAllowed) {
                                            continue;
                                        }
                                        if (Distance < 5.0L) {
                                            CompleteItemPickup();
                                        } else {
                                            if (Ship != NearestShip && Item == NearestItem) {
                                                pas::Extended cpp_left_4 = Distance - (2.5L - aMyFunction::RemapClamped(Distance, 0.0, Ship->GetCargoHookRange(), 1.0, 2.0));
                                                Distance = cpp_left_4 - aMyFunction::SeededRandomFloatRange(Ship->Id + static_cast<std::int32_t>(System::Trunc(Galaxy->CurrentTurn)), 0.1, 0.3);
                                            } else {
                                                double cargoHookRange = Ship->GetCargoHookRange();
                                                double cargoHookMaxPullSpeed = Ship->GetCargoHookMaxPullSpeed();
                                                double cargoHookMinPullSpeed = Ship->GetCargoHookMinPullSpeed();
                                                pas::Extended cpp_left_5 = static_cast<long double>(Distance) - aMyFunction::RemapClamped(Distance, 0.0, cargoHookRange, cargoHookMaxPullSpeed, cargoHookMinPullSpeed);
                                                Distance = cpp_left_5 - aMyFunction::SeededRandomFloatRange(Ship->Id + static_cast<std::int32_t>(System::Trunc(Galaxy->CurrentTurn)), 0.1, 0.3);
                                            }
                                            if (Item->DestroyFlag > 0) {
                                                Distance = pas::real_max<pas::Extended>(aMyFunction::PointDistance(Item->Position, Ship->Position) * 0.99L, static_cast<pas::Extended>(Distance));
                                            }
                                            if (Distance < 1.0L) {
                                                Distance = 1.0f;
                                            }
                                            Angle = Math::ArcTan2(static_cast<long double>(Item->Position.X) - Ship->Position.X, -(static_cast<long double>(Item->Position.Y) - Ship->Position.Y));
                                            {
                                                float cpp_arg_5 = System::Sin(Angle) * Distance + Ship->Position.X;
                                                float cpp_arg_6 = Ship->Position.Y - System::Cos(Angle) * Distance;
                                                Item->Position = EC_Struct::MakePointF(cpp_arg_5, cpp_arg_6);
                                            }
                                            if (!(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr)) {
                                                float cpp_arg_7 = System::Sin(Angle) * 0.01L + Ship->Position.X;
                                                float cpp_arg_8 = Ship->Position.Y - System::Cos(Angle) * 0.01L;
                                                Ship->Position = EC_Struct::MakePointF(cpp_arg_7, cpp_arg_8);
                                            }
                                            if (RecordFilm) {
                                                Globals::PrimaryFilm->SetObjectPosition(StepIndex, Item->FilmObject, Item->Position);
                                                Globals::PrimaryFilm->SetObjectPosition(StepIndex, Ship->FilmObject, Ship->Position);
                                            }
                                            ++PulledItemCount;
                                            if (Distance < 5.0L) {
                                                CompleteItemPickup();
                                            }
                                        }
                                    } else {
                                        Ship->RemovePickupTarget(static_cast<aItem::TItem*>(Target));
                                    }
                                    CanPull = 1;
                                    break;
                                }
                            }
                        }
                        if (CanPull != 0) {
                            ++Index;
                            continue;
                        }
                    }
                    Stage = 2976;
                    if (Galaxy->StasisModEnabled != 1 && Ship->CargoFreeSpace < 0 && aPlayer::GetPlayer() != Ship) {
                        Ship->AutoEquipInventory();
                        aShip::TShip_DropCargoUntilNotOverloaded(Ship);
                        Ship->AutoEquipInventory();
                        if (Ship->CargoFreeSpace <= 0) {
                            Ship->ClearPickupTargets();
                        }
                    }
                    Stage = 2977;
                    if (Galaxy->StasisModEnabled != 1 || aPlayer::GetPlayer() == Ship) {
                        if (!Ship->ProcessMovementStep(StepIndex, RecordFilm)) {
                            ++Index;
                        }
                    } else {
                        ++Index;
                    }
                    Stage = 2978;
                    continue;
                }
                Stage = 2980;
                if (RecordFilm && static_cast<std::uint32_t>(Count) >> 2 == static_cast<std::uint32_t>(StepIndex)) {
                    EntryIndex = 0;
                    while (pas::list_count(Galaxy->Holes) > EntryIndex) {
                        Hole = pas::list_at<THole>(Galaxy->Holes, EntryIndex);
                        if (Hole->Star1 != this && Hole->Star2 != this) {
                            ++EntryIndex;
                        } else {
                            if (Hole->HoleType == 1 && Galaxy->CurrentTurn - Hole->CreatedTurn > 200 || (Hole->HoleType == 3 || Hole->HoleType == 4 && (Galaxy->KellerMissionState == 5 && Galaxy->ScaleIntByTechLevel(1, 10) < Galaxy->CurrentTurn - Hole->CreatedTurn) || Hole->HoleType == 4 && aKling::KellerShip == nullptr)) {
                                Index = 0;
                                DestinationShipCount = pas::list_count(Hole->Star1->Ships);
                                while (Index < DestinationShipCount) {
                                    Ship = pas::list_at<aShip::TShip>(Hole->Star1->Ships, Index);
                                    if (Ship->Order == aShip::soJumpHole && Ship->OrderTarget == Hole) {
                                        break;
                                    }
                                    ++Index;
                                }
                                if (Index >= DestinationShipCount) {
                                    Index = 0;
                                    DestinationShipCount = pas::list_count(Hole->Star2->Ships);
                                    while (Index < DestinationShipCount) {
                                        Ship = pas::list_at<aShip::TShip>(Hole->Star2->Ships, Index);
                                        if (Ship->Order == aShip::soJumpHole && Ship->OrderTarget == Hole) {
                                            break;
                                        }
                                        ++Index;
                                    }
                                    if (Index >= DestinationShipCount) {
                                        if (Hole->FilmObjectId != 0) {
                                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Hole->Graphic));
                                            pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Hole->FilmObjectId))));
                                            Globals::PrimaryFilm->SetHoleState(StepIndex, static_cast<aEFilm::TEFilmObj*>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Hole->FilmObjectId)))), 2);
                                        }
                                        if (Hole->HoleType == 4) {
                                            Galaxy->KellerMissionState = 0;
                                        }
                                        pas::list_delete(Galaxy->Holes, EntryIndex);
                                        pas::free(Hole);
                                        --EntryIndex;
                                    }
                                }
                            }
                            ++EntryIndex;
                        }
                    }
                }
                Stage = 2990;
                if (RecordFilm) {
                    Globals::PrimaryFilm->AdvanceObjects(StepIndex);
                }
                ++StepIndex;
                CurrentStepIndex = StepIndex;
                Stage = 2999;
                for (auto cpp_range_56 = pas::for_to<std::int32_t>(0, pas::list_count(ReferencedItems) - 1); cpp_range_56.next(Index); ) {
                    Item = pas::list_at<aItem::TItem>(ReferencedItems, Index);
                    if (pas::list_indexof(Items, reinterpret_cast<void*>(Item)) >= 0) {
                        Item->DestroyFlag = std::max<std::int32_t>(1, Item->DestroyFlag);
                    }
                }
                pas::list_clear(ReferencedItems);
            }
            Stage = 29999;
            ProcessItemScripts(11);
            EntryIndex = 0;
            while (pas::list_count(Items) > EntryIndex) {
                Item = pas::list_at<aItem::TItem>(Items, EntryIndex);
                if (static_cast<long double>(DamageRadius) * DamageRadius > pas::sqr(static_cast<pas::Extended>(Item->Position.X)) + pas::sqr(static_cast<pas::Extended>(Item->Position.Y))) {
                    ClearItemReferences(Item);
                    if (RecordFilm) {
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                        pas::list_add(PendingFilmObjectRemovals, reinterpret_cast<void*>(Item->FilmObject));
                    }
                    pas::list_delete(Items, EntryIndex);
                    pas::free(Item);
                } else {
                    ++EntryIndex;
                }
            }
            for (auto cpp_range_57 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_57.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->DestroyQueued && (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && (aPlayer::GetPlayer() != Ship || static_cast<std::uint8_t>(GlobalsV::CurrentScreenId) == 16))) {
                    Ship->ScriptItemsAct(aGalaxyStruct::satOnDeath, nullptr, nullptr, 0);
                    Ship->GetHull()->HullPoints = 0;
                    if (Ship->IsHullDestroyed() && aPlayer::GetPlayer() != nullptr) {
                        aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), Ship);
                    }
                    Ship->RefreshDerivedStats(true);
                    if (RecordFilm && (Ship->FilmObject != nullptr && (Ship->CurrentPlanet == nullptr && Ship->DockedTo == nullptr))) {
                        aGalaxy::CreateFilmEffect(u"Weapon.NoGraph"_wref.get(), 0, Effect, EffectFilm);
                        Globals::PrimaryFilm->SetWeaponEndpoints(StepIndex, EffectFilm, Ship->FilmObject, Ship->FilmObject);
                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, Ship->IsHullDestroyed(), true);
                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                    }
                    ClearShipReferences(Ship);
                }
            }
            Stage = 30;
            if (RecordFilm && PlayerFilmPath != nullptr) {
                WorkCount = 0;
                PlayerFilmPath->AppendWaypoint(aPlayer::GetPlayer()->Position, StepIndex);
                Node = PlayerFilmPath->ActiveHead;
                Point = Node->Position;
                PathStep = System::Round(Node->Heading);
                Node = Node->Next;
                while (Node != nullptr && !(PathStep < System::Round(Node->Heading))) {
                    Node = Node->Next;
                }
                while (Node != nullptr) {
                    EntryCount = static_cast<std::int32_t>(System::Round(Node->Heading)) - PathStep + 1;
                    WorkValue = aMyFunction::PointDistance(Point, Node->Position);
                    if (WorkValue > 2.0E+2L || WorkValue > 0.0L && PlayerFilmPath->ActiveTail == Node) {
                        Delta.X = pas::real_divide(static_cast<long double>(Node->Position.X) - Point.X, WorkValue);
                        Delta.Y = pas::real_divide(static_cast<long double>(Node->Position.Y) - Point.Y, WorkValue);
                        WorkValue = pas::real_divide(WorkValue, EntryCount);
                        if (WorkCount == 0 || PlayerFilmPath->ActiveTail == Node) {
                            if (static_cast<long double>(FastCameraSpeed) < WorkValue) {
                                WorkValue = FastCameraSpeed;
                            }
                            if (static_cast<long double>(CameraSpeed) < WorkValue) {
                                WorkCount = 1;
                            }
                        } else if (static_cast<long double>(CameraSpeed) < WorkValue) {
                            WorkValue = CameraSpeed;
                        }
                        Delta.X = static_cast<long double>(Delta.X) * WorkValue;
                        Delta.Y = static_cast<long double>(Delta.Y) * WorkValue;
                        for (auto cpp_range_58 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_58.next(EntryIndex); ) {
                            Point.X = static_cast<long double>(Point.X) + Delta.X;
                            Point.Y = static_cast<long double>(Point.Y) + Delta.Y;
                            ++PathStep;
                        }
                    }
                    Node = Node->Next;
                    while (Node != nullptr && !(PathStep < System::Round(Node->Heading))) {
                        Node = Node->Next;
                    }
                }
                pas::free(PlayerFilmPath);
                PlayerFilmPath = nullptr;
            }
            Stage = 31;
            for (auto cpp_range_59 = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range_59.next(Index); ) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                Ship->ClearCompletedTakeoffOrHoleOrder(StepIndex, RecordFilm);
                if (Ship->InterceptorGraphic != nullptr && Ship->InterceptorPassesRemaining == 0 || static_cast<std::uint8_t>(Ship->InNormalSpace() ^ 1) && Ship->InterceptorPassesRemaining > 0) {
                    if (Ship->AuxiliaryFilmObject != nullptr) {
                        Globals::PrimaryFilm->DetachObject(StepIndex, Ship->AuxiliaryFilmObject);
                    }
                    Ship->InterceptorSourceShip = nullptr;
                    Ship->InterceptorPassesRemaining = 0;
                }
            }
            if (aPlayer::GetPlayer() != nullptr) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckTranclucatorFleetAchievement();
            }
            Stage = 32;
            EntryCount = pas::list_count(MovingDropItems);
            for (auto cpp_range_60 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_60.next(EntryIndex); ) {
                MovingDrop = pas::list_at<TMovingDropItemEntry>(MovingDropItems, EntryIndex);
                if (static_cast<std::uint8_t>(MovingDrop->InsertedIntoStar ^ 1) && MovingDrop->Payload != nullptr) {
                    Item = pas::checked_cast<aItem::TItem*>(MovingDrop->Payload);
                    if (aItem::TArtefactTranclucator* artefactTranclucator_2 = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item); artefactTranclucator_2 != nullptr && MovingDrop->DeployTranclucator != 0) {
                        Tranclucator = pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(artefactTranclucator_2->Ship));
                        artefactTranclucator_2->Ship = nullptr;
                        Tranclucator->CurrentStar = this;
                        pas::list_add(Ships, reinterpret_cast<void*>(Tranclucator));
                        Tranclucator->Position = MovingDrop->Destination;
                        Tranclucator->MovementDirection = 0.0;
                        pas::free(Item);
                        if (RecordFilm) {
                            Tranclucator->FilmObject = Globals::PrimaryFilm->AddObject(Tranclucator->Id, Tranclucator->Graphic, 0, 0);
                            Globals::PrimaryFilm->DetachObject(0, Tranclucator->FilmObject);
                            Globals::PrimaryFilm->SetObjectPosition(StepIndex, Tranclucator->FilmObject, Position);
                            Globals::PrimaryFilm->SetObjectAngle(StepIndex, Tranclucator->FilmObject, 0);
                            Globals::PrimaryFilm->SetObjectAlpha(StepIndex, Tranclucator->FilmObject, 255);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Tranclucator->FilmObject);
                        }
                    } else {
                        Item->Position = MovingDrop->Destination;
                        pas::list_add(Items, reinterpret_cast<void*>(Item));
                        MovingDrop->InsertedIntoStar = true;
                        if (RecordFilm) {
                            {
                                SE_Space::TObjectSE* graphObject_3 = Item->GetGraphObject();
                                std::uint32_t id_3 = Item->Id;
                                aEFilm::TEFilm* primaryFilm_3 = Globals::PrimaryFilm;
                                Item->FilmObject = primaryFilm_3->AddObject(id_3, graphObject_3, 0, 0);
                            }
                            Globals::PrimaryFilm->DetachObject(0, Item->FilmObject);
                            Globals::PrimaryFilm->SetObjectPosition(StepIndex, Item->FilmObject, Position);
                            Globals::PrimaryFilm->AttachObject(StepIndex, Item->FilmObject);
                        }
                    }
                }
                EC_Mem::FreeEC(MovingDrop);
            }
            pas::list_clear(MovingDropItems);
            Stage = 33;
            if (RecordFilm) {
                ++StepIndex;
                CurrentStepIndex = StepIndex;
                Globals::PrimaryFilm->BeginTrailingEffects(StepIndex);
                ++StepIndex;
                CurrentStepIndex = StepIndex;
            }
            if (RecordFilm) {
                Globals::PrimaryFilm->ReleaseWeaponEffects(StepIndex);
            }
            Stage = 34;
            Count = pas::list_count(CombatEvents);
            for (auto cpp_range_61 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_61.next(Index); ) {
                CombatEvent = pas::list_at<TStarCombatEvent>(CombatEvents, Index);
                EC_Mem::FreeEC(CombatEvent);
            }
            pas::list_clear(CombatEvents);
            Stage = 35;
            if (static_cast<std::uint8_t>(RecordFilm ^ 1) && (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar != this)) {
                Index = 0;
                while (pas::list_count(Ships) > Index) {
                    Ship = pas::list_at<aShip::TShip>(Ships, Index);
                    if (Ship->IsHullDestroyed() && pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
                        aShip::TShip_TryRelocateUnseenShip(Ship);
                    }
                    ++Index;
                }
            }
            Stage = 36;
            Index = 0;
            while (pas::list_count(Ships) > Index) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->DockedTo != nullptr && Ship->DockedTo->IsHullDestroyed()) {
                    Ship->ScriptItemsAct(aGalaxyStruct::satOnDeath, nullptr, nullptr, 0);
                    ClearShipReferences(Ship);
                    Ship->DockedTo = nullptr;
                    Ship->GetHull()->HullPoints = 0;
                    if (Ship->IsHullDestroyed()) {
                        if (aPlayer::GetPlayer() != nullptr) {
                            aRanger::TRanger_ProcessShipDestructionQuests(aPlayer::GetPlayer(), Ship);
                            if (aPlayer::GetPlayer() == Ship) {
                                Globals::ScoreScreen->RecordPlayerResult(false);
                                DeathEvent = aGalaxyEvent::AddGalaxyEvent(u"PlayerDeath"_w, nullptr);
                                static_cast<aGalaxyEvent::TGalaxyEvent*>(DeathEvent)->AddTextData(u"StationDestroyed"_w);
                            }
                        }
                    }
                    Index = 0;
                    if (RecordFilm && Ship->FilmObject == nullptr) {
                        Ship->FilmObject = Globals::PrimaryFilm->AddObject(Ship->Id, Ship->Graphic, 0, 0);
                        Globals::PrimaryFilm->DetachObject(0, Ship->FilmObject);
                    }
                } else {
                    ++Index;
                }
            }
            Stage = 37;
            Index = 0;
            StationDestroyed = 0;
            while (pas::list_count(Ships) > Index) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (Ship->IsHullDestroyed()) {
                    if (RecordFilm) {
                        EntryCount = pas::list_count(Ship->Inventory);
                        for (auto cpp_range_62 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_62.next(EntryIndex); ) {
                            Item = pas::list_at<aItem::TItem>(Ship->Inventory, EntryIndex);
                            if (Globals::PrimaryFilm->ContainsObject(Item->FilmObject)) {
                                Globals::PrimaryFilm->ReleaseObject(StepIndex, Item->FilmObject);
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                            }
                        }
                        EntryCount = pas::list_count(Ship->Artefacts);
                        for (auto cpp_range_63 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_63.next(EntryIndex); ) {
                            Item = pas::list_at<aItem::TItem>(Ship->Artefacts, EntryIndex);
                            if (Globals::PrimaryFilm->ContainsObject(Item->FilmObject)) {
                                Globals::PrimaryFilm->ReleaseObject(StepIndex, Item->FilmObject);
                                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Item->GraphObject));
                            }
                        }
                        if (Ship->FilmObject == nullptr) {
                            Ship->FilmObject = Globals::PrimaryFilm->AddObject(Ship->Id, Ship->Graphic, 0, 0);
                            Globals::PrimaryFilm->DetachObject(0, Ship->FilmObject);
                        }
                        if (Ship->FilmObject != nullptr) {
                            Globals::PrimaryFilm->ReleaseObject(StepIndex, Ship->FilmObject);
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Ship->Graphic));
                        }
                        if (Ship->AuxiliaryFilmObject != nullptr) {
                            Globals::PrimaryFilm->ReleaseObject(StepIndex, Ship->AuxiliaryFilmObject);
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Ship->InterceptorGraphic));
                        }
                    }
                    if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) {
                        StationDestroyed = 1;
                    }
                    if (aPlayer::GetPlayer() == Ship) {
                        Globals::ScoreScreen->RecordPlayerResult(false);
                        while ((static_cast<std::uint8_t>(GlobalsV::CurrentScreenId) == 19 || static_cast<std::uint8_t>(GlobalsV::CurrentScreenId) == 21) && Galaxy->ScoreScreenDismissed == 0) {
                            SysUtilsImports::Sleep(1u);
                        }
                        Galaxy->ScoreScreenDismissed = 0;
                    }
                    pas::list_delete(Ships, Index);
                    pas::free(Ship);
                } else {
                    if (!Ship->InNormalSpace()) {
                        Ship->InterceptorPassesRemaining = 0;
                    }
                    if (Ship->InterceptorPassesRemaining <= 0 && Ship->InterceptorGraphic != nullptr) {
                        if (RecordFilm && Ship->AuxiliaryFilmObject != nullptr) {
                            Globals::PrimaryFilm->ReleaseObject(StepIndex, Ship->AuxiliaryFilmObject);
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Ship->InterceptorGraphic));
                        }
                        Ship->ClearIncomingInterceptors();
                    }
                    ++Index;
                }
            }
            if (StationDestroyed != 0 && aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->RefreshStorageBubbles();
            }
            Stage = 38;
            Index = 0;
            while (pas::list_count(Ships) > Index) {
                Ship = pas::list_at<aShip::TShip>(Ships, Index);
                if (aTranclucator::TTranclucator* tranclucator_2 = pas::class_cast_if<aTranclucator::TTranclucator*>(Ship); tranclucator_2 != nullptr && (tranclucator_2->FollowOwner != 0 && tranclucator_2->OwnerShip != nullptr)) {
                    OwnerShip = tranclucator_2->OwnerShip;
                    if (OwnerShip->CurrentPlanet != nullptr && aMyFunction::PointDistanceSquared(Ship->Position, OwnerShip->CurrentPlanet->GetPosition()) < 25.0L || (OwnerShip->DockedTo != nullptr && aMyFunction::PointDistanceSquared(Ship->Position, OwnerShip->DockedTo->Position) < 25.0L || OwnerShip->InNormalSpace() && aMyFunction::PointDistanceSquared(Ship->Position, OwnerShip->Position) < 25.0L)) {
                        HandleObjectLeavingStar(Ship);
                        Ship->EnemyShip = nullptr;
                        Ship->TruceShip = nullptr;
                        Ship->PartnerShip = nullptr;
                        Ship->OrderNone(false);
                        Ship->AfterburnerActive = false;
                        {
                            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(Ship->WeaponCount);
                            if (1 <= cpp_last_2) {
                                for (EntryIndex = 1; EntryIndex <= cpp_last_2; ++EntryIndex) {
                                    Ship->Weapons[EntryIndex]->Target = nullptr;
                                }
                            }
                        }
                        if (RecordFilm) {
                            Globals::PrimaryFilm->DetachObject(StepIndex, Ship->FilmObject);
                        }
                        pas::list_delete(Ships, Index);
                        Ship->CurrentStar = nullptr;
                        if (Ship->ScriptShip != nullptr) {
                            pas::checked_cast<aScript::TScriptShip*>(Ship->ScriptShip)->Script->UnbindShip(Ship);
                        }
                        StoredTranclucator = pas::construct_call<aItem::TArtefactTranclucator>(aItem::TArtefact_Create);
                        static_cast<aItem::TArtefactTranclucator*>(StoredTranclucator)->InitTranclucator(Ship->GetHull()->OwnerId, OwnerShip, reinterpret_cast<aTranclucator::TTranclucator*>(Ship));
                        if (Ship->GetEngine() != nullptr) {
                            Ship->GetEngine()->OutputPercent = 100;
                        }
                        reinterpret_cast<aTranclucator::TTranclucator*>(Ship)->TransferUnequippedCargo(OwnerShip);
                        pas::list_add(OwnerShip->Artefacts, StoredTranclucator);
                        OwnerShip->RefreshDerivedStats(true);
                        reinterpret_cast<aTranclucator::TTranclucator*>(Ship)->FollowOwner = false;
                        Ship->ScriptItemsAct(aGalaxyStruct::satOnTrancPacking, static_cast<pas::Object*>(StoredTranclucator), OwnerShip, 0);
                        OwnerShip->ScriptItemsAct(aGalaxyStruct::satOnTrancPacking, static_cast<pas::Object*>(StoredTranclucator), OwnerShip, 0);
                    } else {
                        ++Index;
                    }
                } else {
                    ++Index;
                }
            }
            Stage = 39;
            if (RecordFilm) {
                Index = 0;
                while (pas::list_count(Galaxy->JumpGates) > Index) {
                    GateEntry = pas::list_at<TJumpGateEntry>(Galaxy->JumpGates, Index);
                    if (GateEntry->UsedThisTurn) {
                        Globals::PrimaryFilm->ReleaseObject(StepIndex, static_cast<aEFilm::TEFilmObj*>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GateEntry->GateFilmId)))));
                        if (GateEntry->EffectFilmId != 0) {
                            Globals::PrimaryFilm->ReleaseObject(StepIndex, static_cast<aEFilm::TEFilmObj*>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(GateEntry->EffectFilmId)))));
                        }
                        pas::list_delete(Galaxy->JumpGates, Index);
                        if (GateEntry->Gate != nullptr) {
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&GateEntry->Gate));
                        }
                        if (GateEntry->Effect != nullptr) {
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&GateEntry->Effect));
                        }
                        EC_Mem::FreeEC(GateEntry);
                    } else {
                        ++Index;
                    }
                }
            }
            PruneWeaponTargetsAfterTurn();
            Stage = 40;
            if (RecordFilm) {
                EntryCount = pas::list_count(PendingFilmObjectRemovals);
                for (auto cpp_range_64 = pas::for_to<std::int32_t>(0, EntryCount - 1); cpp_range_64.next(EntryIndex); ) {
                    EffectFilm = pas::list_at<aEFilm::TEFilmObj>(PendingFilmObjectRemovals, EntryIndex);
                    Globals::PrimaryFilm->ReleaseObject(StepIndex, EffectFilm);
                }
                pas::list_clear(PendingFilmObjectRemovals);
            }
            pas::list_clear(ReferencedItems);
            Globals::PrimaryFilm->PlayerCombatRecorded = RecordFilm && PlayerCombatOccurred;
            {
                const std::int32_t cpp_first_4 = pas::list_count(Ships) - 1;
                if (cpp_first_4 >= 0) {
                    for (Index = cpp_first_4; Index >= 0; --Index) {
                        Ship = pas::list_at<aShip::TShip>(Ships, Index);
                        Ship->virtual_TShip_RefreshCurrentStanding();
                    }
                }
            }
            RefreshDerivedStats();
            Stage = 41;
            if (static_cast<std::uint8_t>(Globals::PlayerStarDayPrepared ^ 1) && Galaxy->StasisModEnabled != 1) {
                for (auto cpp_range_65 = pas::for_to<std::int32_t>(0, pas::list_count(Planets) - 1); cpp_range_65.next(Index); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Planets, Index);
                    aPlanet::TPlanet_NextDay(Planet);
                }
            }
            {
                const std::int32_t cpp_first_5 = pas::list_count(Ships) - 1;
                if (cpp_first_5 >= 0) {
                    for (Index = cpp_first_5; Index >= 0; --Index) {
                        Ship = pas::list_at<aShip::TShip>(Ships, Index);
                        Ship->virtual_TShip_RefreshCurrentStanding();
                    }
                }
            }
            if (LiberationRewardsPending && aPlayer::GetPlayer() != nullptr) {
                aNormalShip::ProcessSystemLiberationRewards(aPlayer::GetPlayer(), this);
                LastLiberationRewardsTurn = static_cast<std::uint32_t>(Galaxy->CurrentTurn);
                LiberationRewardsPending = false;
            }
            Stage = 42;
            if (RecordFilm) {
                if (PlayerCombatOccurred) {
                    Globals::PrimaryFilm->InitialActivity = 0;
                    Globals::PrimaryFilm->FinalActivity = 0;
                } else {
                    Globals::PrimaryFilm->InitialActivity = Globals::PreviousFilmActivity;
                    Globals::PrimaryFilm->FinalActivity = Globals::PrimaryFilm->InitialActivity;
                    WorkValue = aGalaxy::EstimatePlayerTravelTurns();
                    if (Globals::PreviousFilmActivity == 0) {
                        if (WorkValue >= 1.0L) {
                            Globals::PrimaryFilm->FinalActivity = 1;
                        }
                    } else if (Globals::PreviousFilmActivity == 1) {
                        if (WorkValue >= 2.0L) {
                            Globals::PrimaryFilm->FinalActivity = 2;
                        }
                    } else if (Globals::PreviousFilmActivity == 2 && WorkValue <= 2.0L) {
                        Globals::PrimaryFilm->FinalActivity = 1;
                    }
                    Globals::PreviousFilmActivity = Globals::PrimaryFilm->FinalActivity;
                }
                Globals::FilmHistory->AddFilm(Globals::PrimaryFilm);
            }
            Stage = 43;
            if (Galaxy->TerronToStarTurn > 0 && Galaxy->TerronToStarTurn < aGalaxyStruct::TerronTransformationFlag) {
                if (aKling::TerronShip != nullptr) {
                    if (aKling::TerronShip->CurrentStar == this) {
                        if (aMyFunction::PointDistanceSquared(EC_Struct::MakePointF(-1.0E+2f, -1.0E+2f), aKling::TerronShip->Position) < 25.0L) {
                            Galaxy->TerronToStarTurn = Galaxy->CurrentTurn | aGalaxyStruct::TerronTransformationFlag;
                            *(&Graphic->GraphKey) = u"Star.TerronAfter"_w;
                            {
                                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.", Graphic->GraphKey}));
                                SE_Space::TObjectSE* graphic = Graphic;
                                graphic->LoadTemplate(blockByPath);
                            }
                            if (aPlayer::GetPlayer() != nullptr) {
                                if (aPlayer::GetPlayer()->CurrentStar != this) {
                                    aKling::TerronShip->Order = aShip::soJump;
                                    aKling::TerronShip->OrderTarget = this;
                                    aKling::TerronShip->InHyperspace = true;
                                    aKling::TerronShip->OrderStateData = 2;
                                    aKling::TerronShip->Position = EC_Struct::MakePointF(0.0f, 0.0f);
                                }
                            }
                        }
                    }
                }
            }
            Stage = 44;
            if (aPlayer::GetPlayer() != nullptr && (aPlayer::GetPlayer()->CurrentStar == this && (aRanger::PendingPlayerFollowTarget != nullptr && (aRanger::PendingPlayerFollowTarget->CurrentStar != this || static_cast<std::uint8_t>(aRanger::PendingPlayerFollowTarget->InNormalSpace() ^ 1))))) {
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderNone(false);
            }
            RecordingTurnFilm = false;
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == this) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckBomberAchievement(aPlayer::GetPlayer()->BombKillsThisTurn);
            }
            aShip::SimulationContext = 0;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TStar.NextDay ", Name, u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    // Adds a kind-five player notification using localized text.
    void TGalaxy::ShowLocalizedWarning(pas::WideString TextKey) {
        Globals::AddOrUpdatePlayerBubble(Globals::pmQuestCancelled, 0, aConst::LocalizedColorText(TextKey), u""_wref.get());
    }

    void TInterfaceStateOverride_Create(TInterfaceStateOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    // Restores the original control value when the control still exists.
    void TInterfaceStateOverride_Destroy(TInterfaceStateOverride* Self) {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(Self->FormName));
        if (Form != nullptr) {
            Control = Form->FindControlByPath(Self->ControlPath);
            if (Control != nullptr) {
                Control->SetActive(Self->OriginalState > 0);
                if (Self->OriginalState > 1 && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                    static_cast<GI_GraphButton::TGraphButtonGI*>(Control)->SetDisabled(Self->OriginalState == 2);
                }
            }
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Captures the original value before applying the override.
    void TInterfaceStateOverride::Initialize(pas::WideString FormName, pas::WideString ControlPath, std::uint8_t State) {
        this->FormName = std::move(FormName);
        this->ControlPath = std::move(ControlPath);
        this->State = State;
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(this->FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", this->FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(this->ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceStateOverride: object not found - ", this->ControlPath, u" on form ", this->FormName})));
        } else {
            OriginalState = Control->Active;
            Control->SetActive(this->State > 0);
            if (OriginalState > 0 && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                OriginalState = 3 - static_cast<GI_GraphButton::TGraphButtonGI*>(Control)->Disabled;
            }
            if (this->State > 1 && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                static_cast<GI_GraphButton::TGraphButtonGI*>(Control)->SetDisabled(this->State == 2);
            }
        }
    }

    void TInterfaceStateOverride::SetState(std::uint8_t State) {
        this->State = State;
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceStateOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            Control->SetActive(this->State > 0);
            if (this->State > 1 && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                static_cast<GI_GraphButton::TGraphButtonGI*>(Control)->SetDisabled(this->State == 2);
            }
        }
    }

    std::uint8_t TInterfaceStateOverride::GetState() {
        return State;
    }

    void TInterfaceStateOverride::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(FormName);
        Buffer->AddWideStringZ(ControlPath);
        Buffer->AddAnsiChar(State);
        Buffer->AddAnsiChar(OriginalState);
    }

    // Loads both values and immediately reapplies the override.
    void TInterfaceStateOverride::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        FormName = Buffer->ReadWideString();
        ControlPath = Buffer->ReadWideString();
        if (GlobalsV::LoadedSaveVersion >= 160) {
            State = EC_Buf::TBufEC_GetByte(Buffer);
            OriginalState = EC_Buf::TBufEC_GetByte(Buffer);
        } else {
            State = EC_Buf::TBufEC_GetBoolean(Buffer);
            OriginalState = EC_Buf::TBufEC_GetBoolean(Buffer);
        }
        Reapply();
    }

    void TInterfaceStateOverride::Reapply() {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceStateOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            Control->SetActive(State > 0);
            if (State > 1 && pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Control) != nullptr) {
                static_cast<GI_GraphButton::TGraphButtonGI*>(Control)->SetDisabled(State == 2);
            }
        }
    }

    void TInterfaceTextOverride_Create(TInterfaceTextOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    // Restores the original control value when the control still exists.
    void TInterfaceTextOverride_Destroy(TInterfaceTextOverride* Self) {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(Self->FormName));
        if (Form != nullptr) {
            Control = Form->FindControlByPath(Self->ControlPath);
            if (Control != nullptr) {
                if (GI_Label::TLabelGI* labelGI = pas::class_cast_if<GI_Label::TLabelGI*>(Control); !(labelGI != nullptr)) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not a label - ", Self->ControlPath})));
                } else {
                    labelGI->SetText(Self->OriginalText);
                }
            }
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Captures the original value before applying the override.
    void TInterfaceTextOverride::Initialize(pas::WideString FormName, pas::WideString ControlPath, pas::WideString Text) {
        this->FormName = std::move(FormName);
        this->ControlPath = std::move(ControlPath);
        this->Text = std::move(Text);
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(this->FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", this->FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(this->ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceTextOverride: object not found - ", this->ControlPath, u" on form ", this->FormName})));
        } else {
            if (!(pas::class_cast_if<GI_Label::TLabelGI*>(Control) != nullptr)) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not a label - ", this->ControlPath}))));
            }
            OriginalText = static_cast<GI_Label::TLabelGI*>(Control)->GetText();
            static_cast<GI_Label::TLabelGI*>(Control)->SetText(this->Text);
        }
    }

    void TInterfaceTextOverride::SetText(pas::WideString Text) {
        this->Text = std::move(Text);
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceTextOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            if (!(pas::class_cast_if<GI_Label::TLabelGI*>(Control) != nullptr)) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not a label - ", ControlPath}))));
            }
            static_cast<GI_Label::TLabelGI*>(Control)->SetText(this->Text);
        }
    }

    pas::WideString TInterfaceTextOverride::GetText() {
        return Text;
    }

    void TInterfaceTextOverride::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(FormName);
        Buffer->AddWideStringZ(ControlPath);
        Buffer->AddWideStringZ(Text);
        Buffer->AddWideStringZ(OriginalText);
    }

    // Loads both values and immediately reapplies the override.
    void TInterfaceTextOverride::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        FormName = Buffer->ReadWideString();
        ControlPath = Buffer->ReadWideString();
        Text = Buffer->ReadWideString();
        OriginalText = Buffer->ReadWideString();
        Reapply();
    }

    void TInterfaceTextOverride::Reapply() {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceTextOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            if (!(pas::class_cast_if<GI_Label::TLabelGI*>(Control) != nullptr)) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not a label - ", ControlPath}))));
            }
            static_cast<GI_Label::TLabelGI*>(Control)->SetText(Text);
        }
    }

    void TInterfaceImageOverride_Create(TInterfaceImageOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    // Restores the original value when the control still exists; an empty original style is not restored.
    void TInterfaceImageOverride_Destroy(TInterfaceImageOverride* Self) {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(Self->FormName));
        if (Form != nullptr) {
            Control = Form->FindControlByPath(Self->ControlPath);
            if (Control != nullptr) {
                if (EC_Str::CountDelimitedPartsW(pas::view(Self->ImagePath), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(Self->ImagePath), 0, u":"sv) == u"Style") {
                    if (Self->OriginalImagePath != u"") {
                        Control->SetConfigPath(Self->OriginalImagePath);
                    }
                } else if (GI_GAI::TgaiGI* gaiGI = pas::class_cast_if<GI_GAI::TgaiGI*>(Control)) {
                    // Native tests the replacement path before restoring the original.
                    if (EC_Str::CountDelimitedPartsW(pas::view(Self->ImagePath), u"|"sv) < 2) {
                        gaiGI->SetImagePath(Self->OriginalImagePath);
                    } else {
                        gaiGI->SetFirstFrameImagePath(EC_Str::ExtractDelimitedPartW(pas::view(Self->OriginalImagePath), 1, u"|"sv));
                        reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SetImagePath(EC_Str::ExtractDelimitedPartW(pas::view(Self->OriginalImagePath), 0, u"|"sv));
                    }
                    reinterpret_cast<GI_GAI::TgaiGI*>(Control)->PrimeImageCaches();
                    reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SequenceIndex = 0;
                    reinterpret_cast<GI_GAI::TgaiGI*>(Control)->UpdateAutoGeometry();
                    reinterpret_cast<GI_GAI::TgaiGI*>(Control)->RestartPlayback();
                } else if (GI_GI::TgiGI* giGI = pas::class_cast_if<GI_GI::TgiGI*>(Control)) {
                    giGI->SetImagePath(Self->OriginalImagePath);
                } else if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
                    imageGI->SetImagePath(Self->OriginalImagePath);
                } else {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not an image - ", Self->ControlPath})));
                }
            }
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Captures the original value before applying the override.
    void TInterfaceImageOverride::Initialize(pas::WideString FormName, pas::WideString ControlPath, pas::WideString ImagePath) {
        this->FormName = std::move(FormName);
        this->ControlPath = std::move(ControlPath);
        this->ImagePath = std::move(ImagePath);
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(this->FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", this->FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(this->ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceImageOverride: object not found - ", this->ControlPath, u" on form ", this->FormName, u" (", this->ImagePath, u")"})));
        } else if (EC_Str::CountDelimitedPartsW(pas::view(this->ImagePath), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(this->ImagePath), 0, u":"sv) == u"Style") {
            OriginalImagePath = Control->ConfigPath;
            Control->SetConfigPath(EC_Str::ExtractDelimitedPartW(pas::view(this->ImagePath), 1, u":"sv));
        } else if (GI_GAI::TgaiGI* gaiGI = pas::class_cast_if<GI_GAI::TgaiGI*>(Control)) {
            OriginalImagePath = gaiGI->GetImagePath();
            if (gaiGI->GetFirstFrameImagePath() != u"") {
                OriginalImagePath = pas::concat_wide({OriginalImagePath, u"|", gaiGI->GetFirstFrameImagePath()});
            }
            if (EC_Str::CountDelimitedPartsW(pas::view(this->ImagePath), u"|"sv) < 2) {
                gaiGI->SetImagePath(this->ImagePath);
            } else {
                gaiGI->SetFirstFrameImagePath(EC_Str::ExtractDelimitedPartW(pas::view(this->ImagePath), 1, u"|"sv));
                reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SetImagePath(EC_Str::ExtractDelimitedPartW(pas::view(this->ImagePath), 0, u"|"sv));
            }
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->PrimeImageCaches();
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SequenceIndex = 0;
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->UpdateAutoGeometry();
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->RestartPlayback();
        } else if (GI_GI::TgiGI* giGI = pas::class_cast_if<GI_GI::TgiGI*>(Control)) {
            OriginalImagePath = giGI->GetImagePath();
            giGI->SetImagePath(this->ImagePath);
        } else if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
            OriginalImagePath = imageGI->GetImagePath();
            imageGI->SetImagePath(this->ImagePath);
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not an image - ", this->ControlPath}))));
        }
    }

    // Also accepts Style:name for any control. Do not change between image and style modes: they share one original-value slot.
    void TInterfaceImageOverride::SetImagePath(pas::WideString ImagePath) {
        this->ImagePath = std::move(ImagePath);
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceImageOverride: object not found - ", ControlPath, u" on form ", FormName, u" (", this->ImagePath, u")"})));
        } else if (EC_Str::CountDelimitedPartsW(pas::view(this->ImagePath), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(this->ImagePath), 0, u":"sv) == u"Style") {
            Control->SetConfigPath(EC_Str::ExtractDelimitedPartW(pas::view(this->ImagePath), 1, u":"sv));
        } else if (GI_GAI::TgaiGI* gaiGI = pas::class_cast_if<GI_GAI::TgaiGI*>(Control)) {
            if (EC_Str::CountDelimitedPartsW(pas::view(this->ImagePath), u"|"sv) < 2) {
                gaiGI->SetImagePath(this->ImagePath);
            } else {
                gaiGI->SetFirstFrameImagePath(EC_Str::ExtractDelimitedPartW(pas::view(OriginalImagePath), 1, u"|"sv));
                reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SetImagePath(EC_Str::ExtractDelimitedPartW(pas::view(OriginalImagePath), 0, u"|"sv));
            }
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->PrimeImageCaches();
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SequenceIndex = 0;
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->UpdateAutoGeometry();
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->RestartPlayback();
        } else if (GI_GI::TgiGI* giGI = pas::class_cast_if<GI_GI::TgiGI*>(Control)) {
            giGI->SetImagePath(this->ImagePath);
        } else if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
            imageGI->SetImagePath(this->ImagePath);
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not an image - ", ControlPath}))));
        }
    }

    pas::WideString TInterfaceImageOverride::GetImagePath() {
        return ImagePath;
    }

    void TInterfaceImageOverride::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(FormName);
        Buffer->AddWideStringZ(ControlPath);
        Buffer->AddWideStringZ(ImagePath);
        Buffer->AddWideStringZ(OriginalImagePath);
    }

    // Loads both values and immediately reapplies the override.
    void TInterfaceImageOverride::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        FormName = Buffer->ReadWideString();
        ControlPath = Buffer->ReadWideString();
        ImagePath = Buffer->ReadWideString();
        OriginalImagePath = Buffer->ReadWideString();
        Reapply();
    }

    void TInterfaceImageOverride::Reapply() {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceImageOverride: object not found - ", ControlPath, u" on form ", FormName, u" (", ImagePath, u")"})));
        } else if (EC_Str::CountDelimitedPartsW(pas::view(ImagePath), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 0, u":"sv) == u"Style") {
            Control->SetConfigPath(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 1, u":"sv));
        } else if (GI_GAI::TgaiGI* gaiGI = pas::class_cast_if<GI_GAI::TgaiGI*>(Control)) {
            if (EC_Str::CountDelimitedPartsW(pas::view(ImagePath), u"|"sv) < 2) {
                gaiGI->SetImagePath(ImagePath);
            } else {
                gaiGI->SetFirstFrameImagePath(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 1, u"|"sv));
                reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SetImagePath(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 0, u"|"sv));
            }
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->PrimeImageCaches();
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->SequenceIndex = 0;
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->UpdateAutoGeometry();
            reinterpret_cast<GI_GAI::TgaiGI*>(Control)->RestartPlayback();
        } else if (GI_GI::TgiGI* giGI = pas::class_cast_if<GI_GI::TgiGI*>(Control)) {
            giGI->SetImagePath(ImagePath);
        } else if (GI_Image::TImageGI* imageGI = pas::class_cast_if<GI_Image::TImageGI*>(Control)) {
            imageGI->SetImagePath(ImagePath);
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Object is not an image - ", ControlPath}))));
        }
    }

    void TInterfacePosOverride_Create(TInterfacePosOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    // Restores the original control value when the control still exists.
    void TInterfacePosOverride_Destroy(TInterfacePosOverride* Self) {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(Self->FormName));
        if (Form != nullptr) {
            Control = Form->FindControlByPath(Self->ControlPath);
            if (Control != nullptr) {
                Control->SetPosition(ClassesImports::Point(Self->OriginalPosition.X, Self->OriginalPosition.Y));
                Control->SetDepth(Self->OriginalDepth);
            }
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Captures the original value before applying the override.
    void TInterfacePosOverride::Initialize(pas::WideString FormName, pas::WideString ControlPath, std::int32_t DeltaX, std::int32_t DeltaY, std::int32_t DeltaDepth) {
        this->FormName = std::move(FormName);
        this->ControlPath = std::move(ControlPath);
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(this->FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", this->FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(this->ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfacePosOverride: object not found - ", this->ControlPath, u" on form ", this->FormName})));
        } else {
            OriginalPosition.X = Control->LocalPosition.X;
            OriginalPosition.Y = Control->LocalPosition.Y;
            OriginalDepth = Control->Depth;
            Position.X = DeltaX + OriginalPosition.X;
            Position.Y = DeltaY + OriginalPosition.Y;
            Depth = static_cast<long double>(OriginalDepth) + DeltaDepth;
            Control->SetPosition(ClassesImports::Point(Position.X, Position.Y));
            Control->SetDepth(Depth);
        }
    }

    // Offsets are relative to the captured original position and depth, not the current control values.
    void TInterfacePosOverride::SetPosition(std::int32_t DeltaX, std::int32_t DeltaY, std::int32_t DeltaDepth) {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfacePosOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            Position.X = DeltaX + OriginalPosition.X;
            Position.Y = DeltaY + OriginalPosition.Y;
            Depth = static_cast<long double>(OriginalDepth) + DeltaDepth;
            Control->SetPosition(ClassesImports::Point(Position.X, Position.Y));
            Control->SetDepth(Depth);
        }
    }

    void TInterfacePosOverride::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(FormName);
        Buffer->AddWideStringZ(ControlPath);
        Buffer->AddIntegerValue(Position.X);
        Buffer->AddIntegerValue(Position.Y);
        Buffer->AddDouble(Depth);
        Buffer->AddIntegerValue(OriginalPosition.X);
        Buffer->AddIntegerValue(OriginalPosition.Y);
        Buffer->AddDouble(OriginalDepth);
    }

    // Loads both values and immediately reapplies the override.
    void TInterfacePosOverride::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        FormName = Buffer->ReadWideString();
        ControlPath = Buffer->ReadWideString();
        Position.X = EC_Buf::TBufEC_GetInt32(Buffer);
        Position.Y = EC_Buf::TBufEC_GetInt32(Buffer);
        Depth = Buffer->GetDouble();
        OriginalPosition.X = EC_Buf::TBufEC_GetInt32(Buffer);
        OriginalPosition.Y = EC_Buf::TBufEC_GetInt32(Buffer);
        OriginalDepth = Buffer->GetDouble();
        Reapply();
    }

    void TInterfacePosOverride::Reapply() {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfacePosOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            Control->SetPosition(ClassesImports::Point(Position.X, Position.Y));
            Control->SetDepth(Depth);
        }
    }

    void TInterfaceSizeOverride_Create(TInterfaceSizeOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    // Restores the original control value when the control still exists.
    void TInterfaceSizeOverride_Destroy(TInterfaceSizeOverride* Self) {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(Self->FormName));
        if (Form != nullptr) {
            Control = Form->FindControlByPath(Self->ControlPath);
            if (Control != nullptr) {
                Control->SetSize(ClassesImports::Point(Self->OriginalSize.X, Self->OriginalSize.Y));
            }
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Captures the original value before applying the override.
    void TInterfaceSizeOverride::Initialize(pas::WideString FormName, pas::WideString ControlPath, std::int32_t Width, std::int32_t Height) {
        this->FormName = std::move(FormName);
        this->ControlPath = std::move(ControlPath);
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(this->FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", this->FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(this->ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceSizeOverride: object not found - ", this->ControlPath, u" on form ", this->FormName})));
        } else {
            OriginalSize.X = Control->ClientSize.X;
            OriginalSize.Y = Control->ClientSize.Y;
            if (Width > 0) {
                Size.X = Width;
            } else {
                Size.X = OriginalSize.X;
            }
            if (Height > 0) {
                Size.Y = Height;
            } else {
                Size.Y = OriginalSize.Y;
            }
            Control->SetSize(ClassesImports::Point(Size.X, Size.Y));
        }
    }

    // Nonpositive dimensions restore the corresponding original dimension.
    void TInterfaceSizeOverride::SetSize(std::int32_t Width, std::int32_t Height) {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceSizeOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            if (Width > 0) {
                Size.X = Width;
            } else {
                Size.X = OriginalSize.X;
            }
            if (Height > 0) {
                Size.Y = Height;
            } else {
                Size.Y = OriginalSize.Y;
            }
            Control->SetSize(ClassesImports::Point(Size.X, Size.Y));
        }
    }

    void TInterfaceSizeOverride::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(FormName);
        Buffer->AddWideStringZ(ControlPath);
        Buffer->AddIntegerValue(Size.X);
        Buffer->AddIntegerValue(Size.Y);
        Buffer->AddIntegerValue(OriginalSize.X);
        Buffer->AddIntegerValue(OriginalSize.Y);
    }

    // Loads both values and immediately reapplies the override.
    void TInterfaceSizeOverride::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        FormName = Buffer->ReadWideString();
        ControlPath = Buffer->ReadWideString();
        Size.X = EC_Buf::TBufEC_GetInt32(Buffer);
        Size.Y = EC_Buf::TBufEC_GetInt32(Buffer);
        OriginalSize.X = EC_Buf::TBufEC_GetInt32(Buffer);
        OriginalSize.Y = EC_Buf::TBufEC_GetInt32(Buffer);
        Reapply();
    }

    void TInterfaceSizeOverride::Reapply() {
        GI_MessageLoop::TMessageLoopGI* Form = Globals::FindMessageLoop(pas::view(FormName));
        if (Form == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"ML not found - ", FormName}))));
        }
        GI_MessageLoop::TObjectGI* Control = Form->FindControlByPath(ControlPath);
        if (Control == nullptr) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"TInterfaceSizeOverride: object not found - ", ControlPath, u" on form ", FormName})));
        } else {
            Control->SetSize(ClassesImports::Point(Size.X, Size.Y));
        }
    }

    void TStoredItem_CreateEmpty(TStoredItem* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Name = pas::WideString();
        Self->Item = nullptr;
    }

    void TStoredItem_Create(TStoredItem* Self, pas::WideString Name, pas::Object* Item) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Name = std::move(Name);
        Self->Item = Item;
    }

    void TStoredItem_Destroy(TStoredItem* Self) {
        if (Self->Item != nullptr) {
            pas::free(Self->Item);
        }
        Self->Item = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Requires a non-nil Item.
    void TStoredItem::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(Name);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(reinterpret_cast<aItem::TItem*>(Item)->ItemType));
        reinterpret_cast<aItem::TItem*>(Item)->SaveToBuffer(Buffer);
    }

    // Creates the owned item; overwrites a previous Item without freeing it.
    void TStoredItem::LoadFromBuffer(EC_Buf::TBufEC* Buffer, TGalaxy* Galaxy) {
        Name = Buffer->ReadWideString();
        Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
        reinterpret_cast<aItem::TItem*>(Item)->LoadFromBuffer(Buffer, Galaxy);
    }

    // Takes ownership. Replacing a name frees the previously stored item; detach Item from its old container first.
    void TGalaxy::StoreItem(pas::WideString Name, pas::Object* Item) {
        TStoredItem* Entry{};
        std::int32_t Middle{};
        if (aItem::THull* hull = pas::class_cast_if<aItem::THull*>(Item)) {
            hull->OwnerShip = nullptr;
            hull->InterceptorTarget = nullptr;
        }
        if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item)) {
            weapon->Target = nullptr;
            weapon->LoadedTargetKind = aItem::wtkNone;
        }
        if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Item)) {
            static_cast<aTranclucator::TTranclucator*>(artefactTranclucator->Ship)->OwnerShip = nullptr;
        }
        if (aItem::TSatellite* satellite = pas::class_cast_if<aItem::TSatellite*>(Item)) {
            satellite->TargetPlanet = nullptr;
        }
        if (pas::list_count(StoredItems) < 1) {
            Entry = pas::construct_call<TStoredItem>(TStoredItem_Create, Name, Item);
            pas::list_add(StoredItems, reinterpret_cast<void*>(Entry));
            return;
        }
        std::int32_t LowIndex = 0;
        Entry = pas::list_at<TStoredItem>(StoredItems, 0);
        std::int32_t Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
        if (Comparison == 0) {
            pas::free(Entry->Item);
            Entry->Item = Item;
            return;
        }
        if (Comparison < 0) {
            Entry = pas::construct_call<TStoredItem>(TStoredItem_Create, Name, Item);
            pas::list_insert(StoredItems, 0, reinterpret_cast<void*>(Entry));
            return;
        }
        std::int32_t HighIndex = pas::list_count(StoredItems) - 1;
        Entry = pas::list_at<TStoredItem>(StoredItems, HighIndex);
        Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
        if (Comparison == 0) {
            pas::free(Entry->Item);
            Entry->Item = Item;
            return;
        }
        if (Comparison > 0) {
            Entry = pas::construct_call<TStoredItem>(TStoredItem_Create, Name, Item);
            pas::list_add(StoredItems, reinterpret_cast<void*>(Entry));
        } else {
            while (true) {
                if (HighIndex - LowIndex < 2) {
                    Entry = pas::construct_call<TStoredItem>(TStoredItem_Create, Name, Item);
                    pas::list_insert(StoredItems, HighIndex, reinterpret_cast<void*>(Entry));
                    return;
                }
                Middle = (LowIndex + HighIndex) / 2;
                Entry = pas::list_at<TStoredItem>(StoredItems, Middle);
                Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
                if (Comparison == 0) {
                    pas::free(Entry->Item);
                    Entry->Item = Item;
                    return;
                }
                if (Comparison < 0) {
                    HighIndex = Middle;
                } else {
                    LowIndex = Middle;
                }
            }
        }
    }

    // Returns nil when absent. Remove detaches the item and frees its named entry, transferring ownership to the caller.
    pas::Object* TGalaxy::GetStoredItem(pas::WideString Name, std::uint8_t Remove) {
        TStoredItem* Entry{};
        std::int32_t Index{};
        std::int32_t Comparison{};
        // Nested in GetStoredItem; caller-popped static link. Entry -4, Remove -5, Self -12, Index -16.
        auto TakeEntry = [&]() -> pas::Object* {
            pas::Object* Result = Entry->Item;
            if (Remove) {
                Entry->Item = nullptr;
                pas::free(Entry);
                pas::list_delete(this->StoredItems, Index);
            }
            return Result;
        };
        pas::Object* Result = nullptr;
        if (pas::list_count(StoredItems) < 1) {
            return Result;
        }
        Index = 0;
        Entry = pas::list_at<TStoredItem>(StoredItems, Index);
        if (Entry->Name == Name) {
            return TakeEntry();
        }
        Index = pas::list_count(StoredItems) - 1;
        Entry = pas::list_at<TStoredItem>(StoredItems, Index);
        if (Entry->Name == Name) {
            return TakeEntry();
        }
        std::int32_t LowIndex = 0;
        std::int32_t HighIndex = pas::list_count(StoredItems) - 1;
        while (true) {
            if (HighIndex - LowIndex < 2) {
                Result = nullptr;
                break;
            }
            Index = (LowIndex + HighIndex) / 2;
            Entry = pas::list_at<TStoredItem>(StoredItems, Index);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Entry->Name.pchar());
            if (Comparison == 0) {
                Result = TakeEntry();
                break;
            }
            if (Comparison < 0) {
                HighIndex = Index;
            } else {
                LowIndex = Index;
            }
        }
        return Result;
    }

    // Inserts a new custom template in the case-insensitive sorted pool.
    aConst::PWeaponInfo TGalaxy::GetOrCreateCustomWeaponInfo(pas::WideString Name) {
        aConst::PWeaponInfo Result{};
        aConst::PWeaponInfo Info{};
        std::int32_t Middle{};
        auto Allocate = [&]() -> void {
            pas::new_value(Result);
            Result->ConfigName = Name;
            Result->ItemType = aConst::t_CustomWeapon;
            std::uint32_t State = CrcUnit::InitCrc32();
            State = CrcUnit::UpdateCrc32(State, Result->ConfigName.pchar(), Result->ConfigName.length() * 2);
            Result->TypeHash = CrcUnit::FinishCrc32(State);
        };
        if (pas::list_count(CustomWeaponTypes) < 1) {
            Allocate();
            pas::list_add(CustomWeaponTypes, static_cast<void*>(Result));
            return Result;
        }
        std::int32_t LowIndex = 0;
        Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, 0);
        std::int32_t Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Info->ConfigName.pchar());
        if (Comparison == 0) {
            return Info;
        }
        if (Comparison < 0) {
            Allocate();
            pas::list_insert(CustomWeaponTypes, 0, static_cast<void*>(Result));
            return Result;
        }
        std::int32_t HighIndex = pas::list_count(CustomWeaponTypes) - 1;
        Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, HighIndex);
        Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Info->ConfigName.pchar());
        if (Comparison == 0) {
            return Info;
        }
        if (Comparison > 0) {
            Allocate();
            pas::list_add(CustomWeaponTypes, static_cast<void*>(Result));
            return Result;
        }
        while (true) {
            if (HighIndex - LowIndex < 2) {
                Allocate();
                pas::list_insert(CustomWeaponTypes, HighIndex, static_cast<void*>(Result));
                return Result;
            }
            Middle = (LowIndex + HighIndex) / 2;
            Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Info->ConfigName.pchar());
            if (Comparison == 0) {
                return Info;
            }
            if (Comparison < 0) {
                HighIndex = Middle;
            } else {
                LowIndex = Middle;
            }
        }
        return Result;
    }

    // Binary search of CustomWeaponTypes; raises if absent.
    aConst::PWeaponInfo TGalaxy::RequireCustomWeaponInfo(pas::WideString Name) {
        aConst::PWeaponInfo Result{};
        aConst::PWeaponInfo Info{};
        std::int32_t Middle{};
        if (pas::list_count(CustomWeaponTypes) < 1) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Cant find custom weapon info = ", Name}))));
        }
        std::int32_t LowIndex = 0;
        Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, 0);
        std::int32_t Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Info->ConfigName.pchar());
        if (Comparison == 0) {
            return Info;
        }
        if (Comparison < 0) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Cant find custom weapon info = ", Name}))));
        }
        std::int32_t HighIndex = pas::list_count(CustomWeaponTypes) - 1;
        Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, HighIndex);
        Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Info->ConfigName.pchar());
        if (Comparison == 0) {
            return Info;
        }
        if (Comparison > 0) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Cant find custom weapon info = ", Name}))));
        }
        while (true) {
            if (HighIndex - LowIndex < 2) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Cant find custom weapon info = ", Name}))));
            }
            Middle = (LowIndex + HighIndex) / 2;
            Info = pas::list_at<aConst::TWeaponInfo>(CustomWeaponTypes, Middle);
            Comparison = EC_Expression::CompareScriptNames(Name.pchar(), Info->ConfigName.pchar());
            if (Comparison == 0) {
                Result = Info;
                break;
            }
            if (Comparison < 0) {
                HighIndex = Middle;
            } else {
                LowIndex = Middle;
            }
        }
        return Result;
    }

    // Checks special simulation, cheat points and all protected integrity flags.
    std::uint8_t TGalaxy::CanRecordAchievements() {
        if (SpecialSimulationMode != 0) {
            return false;
        }
        // These unused calculations are present in the original eligibility check.
        std::int32_t ReservedMode = 4;
        std::int32_t ReservedLimit = aGalaxyStruct::TurnsPerYear;
        ReservedLimit += 107000;
        ReservedLimit = pas::shl(ReservedLimit, 1);
        return GetCheatPoints() == 0 && GR_Main::CCInterface->GetIntegrityError() == 0 && static_cast<std::uint8_t>(GR_Main::CCInterface->GetFlag0A() ^ 1) && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1) && static_cast<std::uint8_t>(GR_Main::CCInterface->GetEditableStateApplied() ^ 1) && static_cast<std::uint8_t>(GR_Main::CCInterface->GetResourceChecksumFailed() ^ 1);
    }

    void TInterfaceStateOverride::p_destroy() {
        aGalaxy::TInterfaceStateOverride_Destroy(this);
    }

    void TInterfaceTextOverride::p_destroy() {
        aGalaxy::TInterfaceTextOverride_Destroy(this);
    }

    void TInterfaceImageOverride::p_destroy() {
        aGalaxy::TInterfaceImageOverride_Destroy(this);
    }

    void TInterfacePosOverride::p_destroy() {
        aGalaxy::TInterfacePosOverride_Destroy(this);
    }

    void TInterfaceSizeOverride::p_destroy() {
        aGalaxy::TInterfaceSizeOverride_Destroy(this);
    }

    void TStoredItem::p_destroy() {
        aGalaxy::TStoredItem_Destroy(this);
    }

    void TGalaxy::p_destroy() {
        aGalaxy::TGalaxy_Destroy(this);
    }

    void TConstellation::p_destroy() {
        aGalaxy::TConstellation_Destroy(this);
    }

    void THole::p_destroy() {
        aGalaxy::THole_Destroy(this);
    }

    void TCustomSystemInfo::p_destroy() {
        aGalaxy::TCustomSystemInfo_Destroy(this);
    }

    void TStar::p_destroy() {
        aGalaxy::TStar_Destroy(this);
    }

} // namespace aGalaxy
