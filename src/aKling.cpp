#include "layout/aKling.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Space.hpp"
#include "types/SystemImports.hpp"
#include "types/aAsteroid.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aMissile.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"

namespace aKling {
    static std::int32_t RandomInteger(std::int32_t BoundA, std::int32_t BoundB, TKling* Self);

    static std::int32_t RandomEquipmentSize(std::int32_t BaseSize, std::int32_t MinimumSizeIndex,
        std::int32_t MaximumSizeIndex, TKling* Self);

    static std::int32_t NonDominatorDistanceIndex(aGalaxy::TStar* Star);

    static std::int32_t OtherSeriesDistanceIndex(aGalaxy::TStar* Star, aGalaxyStruct::TDominatorSeries& Series);

    // Maximum-size index followed by minimum-size index.
    pas::Array<pas::Array<std::int32_t, 0, 1>, 0, 7> DominatorEquipmentSizeIndices = pas::Array<pas::Array<std::int32_t, 0, 1>, 0, 7>{{
        pas::Array<std::int32_t, 0, 1>{{1, 1}}, pas::Array<std::int32_t, 0, 1>{{1, 3}},
        pas::Array<std::int32_t, 0, 1>{{2, 4}}, pas::Array<std::int32_t, 0, 1>{{3, 4}},
        pas::Array<std::int32_t, 0, 1>{{3, 5}}, pas::Array<std::int32_t, 0, 1>{{4, 5}},
        pas::Array<std::int32_t, 0, 1>{{1, 1}}, pas::Array<std::int32_t, 0, 1>{{4, 5}}}};

    pas::Array<std::int32_t, 1, 7> DominatorWeaponDistributionByTier = pas::Array<std::int32_t, 1, 7>{{1, 1, 2, 2, 3, 3,
        4}};

    pas::Array<pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>, 1, 4> DominatorWeaponWeights = pas::Array<pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>, 1, 4>{{
        pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>{{
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 5, 30, 5, 25, 5}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 30, 0, 0, 0, 0, 25, 5, 5, 0, 30, 0, 5}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 10, 0, 20, 30, 0, 0, 0, 0, 0, 0, 40}},
            pas::Array<std::int32_t, 50, 64>{{0, 20, 0, 20, 0, 30, 30, 0, 0, 0, 0, 0, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{25, 0, 50, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{15, 0, 50, 0, 25, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0}}}},
        pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>{{
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 5, 30, 5, 25, 5}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 30, 0, 0, 0, 0, 25, 5, 5, 0, 30, 0, 5}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 10, 0, 20, 30, 0, 0, 0, 0, 0, 0, 40}},
            pas::Array<std::int32_t, 50, 64>{{0, 20, 0, 20, 0, 30, 30, 0, 0, 0, 0, 0, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{5, 0, 30, 0, 10, 0, 0, 0, 0, 5, 50, 0, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 10, 0, 40, 0, 15, 30, 5, 0, 0, 0, 0, 0, 0}}}},
        pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>{{
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 5, 30, 5, 25, 5}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 30, 0, 0, 0, 0, 25, 5, 5, 0, 30, 0, 5}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 5, 0, 5, 20, 0, 10, 10, 0, 0, 0, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 5, 0, 5, 0, 30, 20, 0, 15, 0, 0, 25, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{5, 0, 30, 0, 10, 0, 0, 0, 0, 5, 50, 0, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 10, 0, 40, 0, 15, 30, 5, 0, 0, 0, 0, 0, 0}}}},
        pas::Array<pas::Array<std::int32_t, 50, 64>, 0, 7>{{
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 30, 15, 35, 10}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 10, 0, 0, 0, 0, 25, 10, 0, 10, 30, 0, 15}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 5, 0, 5, 20, 0, 10, 10, 0, 0, 0, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 5, 0, 5, 0, 30, 20, 0, 15, 0, 0, 25, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{5, 0, 30, 0, 10, 0, 0, 0, 0, 5, 50, 0, 0, 0, 0}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 25, 50}},
            pas::Array<std::int32_t, 50, 64>{{0, 0, 10, 0, 40, 0, 15, 30, 5, 0, 0, 0, 0, 0, 0}}}}}};

    // Control threshold and paired equipment/weapon generation bounds.
    pas::Array<pas::Array<std::int32_t, 1, 19>, 1, 7> DominatorGenerationTuning = pas::Array<pas::Array<std::int32_t, 1, 19>, 1, 7>{{
        pas::Array<std::int32_t, 1, 19>{{65, 1, 3, 1, 3, 1, 3, 1, 3, 1, 5, 1, 2, 5, 25, 5, 20, 0, 10}},
        pas::Array<std::int32_t, 1, 19>{{75, 1, 5, 1, 5, 1, 5, 1, 5, 2, 6, 1, 3, 15, 50, 10, 40, 0, 20}},
        pas::Array<std::int32_t, 1, 19>{{85, 2, 7, 2, 7, 2, 7, 2, 7, 3, 7, 2, 4, 30, 75, 20, 60, 15, 40}},
        pas::Array<std::int32_t, 1, 19>{{95, 3, 8, 3, 8, 3, 8, 3, 8, 4, 8, 2, 5, 45, 95, 30, 80, 30, 80}},
        pas::Array<std::int32_t, 1, 19>{{95, 4, 8, 4, 8, 4, 8, 4, 8, 6, 8, 3, 5, 50, 95, 50, 95, 40, 95}},
        pas::Array<std::int32_t, 1, 19>{{95, 6, 8, 6, 8, 6, 8, 6, 8, 7, 8, 4, 5, 75, 95, 75, 95, 70, 95}},
        pas::Array<std::int32_t, 1, 19>{{95, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 5, 95, 95, 95, 95, 95, 95}}}};

    aKling::TKling* BlazerShip{};

    aKling::TKling* KellerShip{};

    aKling::TKling* TerronShip{};

    // PIECECREATOR target selected during new-game generation and persisted with the galaxy.
    std::uint32_t PieceCreatorTargetStarId{};

    aPlanet::TPlanet* DominatorSpawnPlanet{};

    // Native ANSI exception text contains UTF-8 bytes; ordinary Russian literals
    // compile to Windows-1251. Text: Клинг выпустился со скоростью 0
    const pas::WideString DominatorZeroSpeedError = u"\u0420\u0459\u0420\u00bb\u0420\u0451\u0420\u0405\u0420\u0456 \u0420\u0406\u0421\u2039\u0420\u0457\u0421\u0453\u0421\u0403\u0421\u201a\u0420\u0451\u0420\u00bb\u0421\u0403\u0421\u040f \u0421\u0403\u0420\u0455 \u0421\u0403\u0420\u0454\u0420\u0455\u0421\u0402\u0420\u0455\u0421\u0403\u0421\u201a\u0421\u040a\u0421\u040b 0"_w;

    void TKling_Destroy(TKling* Self) {
        std::int32_t I{};
        aRanger::TRanger* Ranger{};
        if (Self->KlingType == aGalaxyStruct::ktBoss) {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Rangers) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    if (Ranger->LastDockedNonPlanetLocation == Self) {
                        Ranger->LastDockedNonPlanetLocation = nullptr;
                    }
                }
            }
        }
        aShip::TShip_Destroy(Self);
    }

    void TKling::InitBlazer(aGalaxy::TStar* Star) {
        TypeId = aGalaxyStruct::stKling;
        OwnerId = aGalaxyStruct::oiDominator;
        KlingType = aGalaxyStruct::ktBoss;
        DominatorSeries = aGalaxyStruct::dsBlazer;
        SetMoney(SystemImports::MaxInt);
        {
            double cpp_left_2 = aMyFunction::NextRandomFloatRange(0.8, 1.2, RandomState);
            double cpp_left = static_cast<float>(cpp_left_2 * aConst::DominatorShipDefinitions[KlingType].BaseNodeReserve);
            NodeReserve = pas::real_round(static_cast<float>(cpp_left * aGalaxy::Galaxy->GetNodeDropModifier()));
        }
        Position.X = 0.0f;
        Position.Y = 0.0f;
        CurrentStar = Star;
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        CurrentStar->Status.DominatorSeries = DominatorSeries;
        HomePlanet = nullptr;
        CurrentPlanet = nullptr;
        ++CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling];
        Name = aConst::DominatorShipDefinitions[KlingType].DisplayNames[DominatorSeries];
        RefreshCombatSkills();
        ActiveProgramAppliedTurn = 0;
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        {
            double cpp_right = aMyFunction::NextRandomFloatRange(0.9, 1.1, RandomState);
            std::uint16_t roundAndTruncateToHundreds = aMyFunction::RoundAndTruncateToHundreds(
                static_cast<float>(static_cast<float>(static_cast<float>(aConst::HullCapacityScale * 6.0E+3) * aGalaxy::Galaxy->GetDominatorBossHullScale()) * cpp_right));
            aShip::TShip* self = this;
            aShip::TShip_CreateAndEquipHull(self, roundAndTruncateToHundreds, 8, aGalaxyStruct::oiDominator, -1, false);
        }
        CreateAndEquipFuelTanks(100, 8, aGalaxyStruct::oiDominator);
        CreateAndEquipEngine(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1])), 8,
            aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_TorpedoTube,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_TorpedoTube].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_TorpedoTube,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_TorpedoTube].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_IMHO9000,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_IMHO9000].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_Disintegrator,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Disintegrator].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        if (aGalaxy::Galaxy->GetDifficultyTierIndex() > 0) {
            CreateAndEquipWeapon(aConst::t_Turbogravitron,
                pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Turbogravitron].AverageSize) *
                    aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        }
        CreateAndEquipDefGenerator(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[5])), 8,
            aGalaxyStruct::oiDominator);
        CreateAndEquipRepairRobot(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[5])),
            pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->GetEffectiveDifficultyLevel(), 0.0, 24.0, 4.0,
                8.0)), OwnerId);
        SetInventoryDominatorOwner();
        if (GetDefGenerator() != nullptr) {
            GetDefGenerator()->DamageFactor = pas::real_max<double>(0.14,
                    static_cast<float>(0.7 - static_cast<float>(aGalaxy::Galaxy->GetEffectiveDifficultyLevel() * 0x1.47ae147ae147ap-7)));
            GetHull()->Armor = pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 8.0)) + 11;
        }
        ImproveStandardEquipment();
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
        if (Speed == 0) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(DominatorZeroSpeedError)));
        }
    }

    void TKling::InitKeller(aGalaxy::TStar* Star) {
        TypeId = aGalaxyStruct::stKling;
        OwnerId = aGalaxyStruct::oiDominator;
        KlingType = aGalaxyStruct::ktBoss;
        DominatorSeries = aGalaxyStruct::dsKeller;
        SetMoney(SystemImports::MaxInt);
        {
            double cpp_left = aMyFunction::NextRandomFloatRange(0.8, 1.2, RandomState);
            NodeReserve = pas::real_round(static_cast<float>(cpp_left * aConst::DominatorShipDefinitions[KlingType].BaseNodeReserve));
        }
        Position.X = 0.0f;
        Position.Y = 0.0f;
        CurrentStar = Star;
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        CurrentStar->Status.DominatorSeries = DominatorSeries;
        HomePlanet = nullptr;
        CurrentPlanet = nullptr;
        ++CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling];
        Name = aConst::DominatorShipDefinitions[KlingType].DisplayNames[DominatorSeries];
        RefreshCombatSkills();
        ActiveProgramAppliedTurn = 0;
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        {
            double cpp_right = aMyFunction::NextRandomFloatRange(0.9, 1.1, RandomState);
            std::uint16_t roundAndTruncateToHundreds = aMyFunction::RoundAndTruncateToHundreds(
                static_cast<float>(static_cast<float>(static_cast<float>(aConst::HullCapacityScale * 4.0E+3) * aGalaxy::Galaxy->GetDominatorBossHullScale()) * cpp_right));
            aShip::TShip* self = this;
            aShip::TShip_CreateAndEquipHull(self, roundAndTruncateToHundreds, 8, aGalaxyStruct::oiDominator, -1, false);
        }
        CreateAndEquipFuelTanks(100, 8, aGalaxyStruct::oiDominator);
        CreateAndEquipEngine(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1])), 8,
            aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_TorpedoTube,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_TorpedoTube].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_Vertix,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Vertix].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_IMHO9000,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_IMHO9000].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_Multiresonator,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Multiresonator].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        if (aGalaxy::Galaxy->GetDifficultyTierIndex() > 0) {
            CreateAndEquipWeapon(aConst::t_AtomicVision,
                pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_AtomicVision].AverageSize) *
                    aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        }
        CreateAndEquipDefGenerator(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[5])), 8,
            aGalaxyStruct::oiDominator);
        {
            std::uint8_t& cpp_target = CreateAndEquipRepairRobot(
                pas::real_round(static_cast<float>(static_cast<double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[5])),
                pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->GetEffectiveDifficultyLevel(), 0.0, 24.0,
                    4.0, 8.0)), OwnerId)->RepairPoints;
            std::int32_t cpp_step = aGalaxy::Galaxy->GetEffectiveDifficultyLevel();
            cpp_target += cpp_step;
        }
        SetInventoryDominatorOwner();
        if (GetDefGenerator() != nullptr && GetDefGenerator()->HasStandardStats()) {
            switch (aGalaxy::Galaxy->GetDifficultyTierIndex()) {
                case 0: break;
                case 1: GetDefGenerator()->Improve(aItem::ikMinor); break;
                case 2: GetDefGenerator()->Improve(aItem::ikMedium); break;
                default: GetDefGenerator()->Improve(aItem::ikMajor); break;
            }
        }
        ImproveStandardEquipment();
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
        if (Speed == 0) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(DominatorZeroSpeedError)));
        }
    }

    void TKling::InitTerron(aGalaxy::TStar* Star) {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        TypeId = aGalaxyStruct::stKling;
        OwnerId = aGalaxyStruct::oiDominator;
        KlingType = aGalaxyStruct::ktBoss;
        DominatorSeries = aGalaxyStruct::dsTerron;
        SetMoney(SystemImports::MaxInt);
        {
            double cpp_left = aMyFunction::NextRandomFloatRange(0.8, 1.2, RandomState);
            NodeReserve = pas::real_round(static_cast<float>(cpp_left * aConst::DominatorShipDefinitions[KlingType].BaseNodeReserve));
        }
        Position.X = 1.0E+3f;
        Position.Y = 0.0f;
        CurrentStar = Star;
        CurrentStar->Name = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"Star.",
                aConst::DominatorSeriesNames[DominatorSeries]}));
        {
            const std::int32_t cpp_last = pas::list_count(Star->Planets) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, I);
                    if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate,
                        aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                        Planet->OwnerId = aGalaxyStruct::oiMaloc;
                        Planet->RaceId = aGalaxyStruct::oiMaloc;
                    }
                }
            }
        }
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
        CurrentStar->Status.DominatorSeries = DominatorSeries;
        HomePlanet = nullptr;
        CurrentPlanet = nullptr;
        ++CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling];
        Name = aConst::DominatorShipDefinitions[KlingType].DisplayNames[DominatorSeries];
        RefreshCombatSkills();
        ActiveProgramAppliedTurn = 0;
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        {
            std::int8_t& cpp_target = ([&] {
                double cpp_right = aMyFunction::NextRandomFloatRange(0.9, 1.1, RandomState);
                std::uint16_t roundAndTruncateToHundreds = aMyFunction::RoundAndTruncateToHundreds(
                    static_cast<float>(static_cast<float>(static_cast<float>(aConst::HullCapacityScale * 1.0E+4) * aGalaxy::Galaxy->GetDominatorBossHullScale()) * cpp_right));
                aShip::TShip* self = this;
                return aShip::TShip_CreateAndEquipHull(self, roundAndTruncateToHundreds, 8, aGalaxyStruct::oiDominator,
                    -1, false);
            }())->Armor;
            std::int32_t cpp_step = aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 4;
            cpp_target += cpp_step;
        }
        CreateAndEquipFuelTanks(100, 8, aGalaxyStruct::oiDominator);
        CreateAndEquipEngine(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1])), 8,
            aGalaxyStruct::oiDominator);
        aItem::TWeapon* Weapon = CreateAndEquipWeapon(aConst::t_IMHO9000,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_IMHO9000].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        Weapon->Range += 5 * aGalaxy::Galaxy->GetEffectiveDifficultyLevel();
        Weapon->MaxDamage += aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 2;
        CreateAndEquipWeapon(aConst::t_MissileLauncher,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_MissileLauncher].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        CreateAndEquipWeapon(aConst::t_MissileLauncher,
            pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_MissileLauncher].AverageSize) *
                aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        Weapon = CreateAndEquipWeapon(aConst::t_Multiresonator,
                pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_Multiresonator].AverageSize) *
                    aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        Weapon->Range += 5 * aGalaxy::Galaxy->GetEffectiveDifficultyLevel();
        Weapon->MaxDamage += aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 2;
        if (aGalaxy::Galaxy->GetDifficultyTierIndex() > 1) {
            CreateAndEquipWeapon(aConst::t_TorpedoTube,
                pas::real_round(static_cast<float>(static_cast<double>(aConst::WeaponInfos[aConst::t_TorpedoTube].AverageSize) *
                    aConst::EquipmentSizeFactors[5])), 8, aGalaxyStruct::oiDominator);
        }
        CreateAndEquipDefGenerator(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[5])), 8,
            aGalaxyStruct::oiDominator);
        CreateAndEquipRepairRobot(
            pas::real_round(static_cast<float>(static_cast<double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[5])),
            pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->GetEffectiveDifficultyLevel(), 0.0, 24.0, 4.0,
                8.0)), OwnerId);
        SetInventoryDominatorOwner();
        if (GetDefGenerator() != nullptr && GetDefGenerator()->HasStandardStats()) {
            switch (aGalaxy::Galaxy->GetDifficultyTierIndex()) {
                case 0: break;
                case 1: GetDefGenerator()->Improve(aItem::ikMinor); break;
                case 2: GetDefGenerator()->Improve(aItem::ikMedium); break;
                default: GetDefGenerator()->Improve(aItem::ikMajor); break;
            }
        }
        GetHull()->Armor = pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 8.0)) + 11;
        ImproveStandardEquipment();
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
        if (Speed == 0) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(DominatorZeroSpeedError)));
        }
    }

    void TKling::InitializeDominator(aGalaxyStruct::TKlingType Kind, aPlanet::TPlanet* Planet,
        aGalaxyStruct::TDominatorSeries Series) {
        TypeId = aGalaxyStruct::stKling;
        OwnerId = aGalaxyStruct::oiDominator;
        KlingType = Kind;
        DominatorSeries = Series;
        SetMoney(pas::real_round(static_cast<float>(static_cast<double>(aGalaxy::Galaxy->MaxRangerWealth) *
            aConst::DominatorShipDefinitions[Kind].InitialWealthScale)));
        {
            double cpp_left = static_cast<float>(aMyFunction::NextRandomUnitFloat(RandomState) + 0.5);
            NodeReserve = pas::real_round(static_cast<float>(cpp_left * aConst::DominatorShipDefinitions[Kind].BaseNodeReserve));
        }
        if (KlingType != aGalaxyStruct::ktBoss) {
            CurrentStar = Planet->CurrentStar;
            pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(this));
            HomePlanet = nullptr;
            CurrentPlanet = Planet;
        }
        ++CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling];
        Name = pas::WideString();
        if (GR_Main::ModShipNameConfig != nullptr && GR_Main::ModShipNameConfig->CountBlocks(u"Dominator"_wref.get()) >
                0) {
            Name = pas::concat_wide({
                    ([&] {
                        std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0,
                            GR_Main::ModShipNameConfig->GetBlock(u"Dominator"sv)->GetParamCount() - 1, RandomState);
                        EC_BlockPar::TBlockParEC* block = GR_Main::ModShipNameConfig->GetBlock(u"Dominator"sv);
                        return block->GetParamValue(nextRandomIntRange);
                    }()), u" ", u"-",
                    pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1)), u"-"});
        }
        if (GetName().length() == 0) {
            Name = pas::concat_wide({
                    ([&] {
                        std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(0,
                            GR_Main::LanguageDataConfig->GetBlock(
                                u"ShipName"sv)->GetBlock(u"Kling"sv)->GetParamCount() - 1, RandomState);
                        EC_BlockPar::TBlockParEC* block_2 = GR_Main::LanguageDataConfig->GetBlock(
                            u"ShipName"sv)->GetBlock(u"Kling"sv);
                        return block_2->GetParamValue(nextRandomIntRange_2);
                    }()), u" ", u"-",
                    pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Id) % 100 + 1)), u"-"});
        }
        ActiveProgramAppliedTurn = 0;
        RefreshCombatSkills();
    }

    // Initializes type/series and location through, then builds the generated loadout. Series occupies one four-byte stack slot.
    void TKling::InitGenerated(aGalaxyStruct::TKlingType Kind, aPlanet::TPlanet* Planet,
        aGalaxyStruct::TDominatorSeries Series) {
        std::int32_t ControlPercent{};
        std::int32_t MinimumControl{};
        std::int32_t MiddleControl{};
        std::int32_t Tier{};
        std::int32_t MaximumControl{};
        std::int32_t MaximumSizeIndex{};
        std::int32_t MinimumSizeIndex{};
        std::int32_t Roll{};
        std::int32_t I{};
        std::int32_t WeightSum{};
        std::int32_t Attempts{};
        aItem::TEquipment* Equipment{};
        aConst::TItemType WeaponType{};
        std::uint8_t Chosen{};
        std::uint8_t Accepted{};
        auto SizeForKind = [&](std::int32_t BaseSize) -> std::int32_t {
            return aKling::RandomEquipmentSize(BaseSize, DominatorEquipmentSizeIndices[this->KlingType][1],
                DominatorEquipmentSizeIndices[this->KlingType][0], this);
        };
        auto RandomTuning = [&](std::int32_t MinimumColumn, std::int32_t MaximumColumn) -> std::int32_t {
            std::int32_t A = pas::real_round(aMyFunction::RemapClamped(ControlPercent, MinimumControl, MiddleControl,
                DominatorGenerationTuning[Tier][MaximumColumn], DominatorGenerationTuning[Tier][MinimumColumn]));
            std::int32_t B = pas::real_round(aMyFunction::RemapClamped(ControlPercent, MiddleControl, MaximumControl,
                DominatorGenerationTuning[Tier][MaximumColumn], DominatorGenerationTuning[Tier][MinimumColumn]));
            A = std::min<std::int32_t>(A, B);
            return aKling::RandomInteger(A, B, this);
        };
        auto InterpolatedTuning = [&](std::int32_t MinimumColumn, std::int32_t MaximumColumn) -> std::int32_t {
            return pas::real_round(aMyFunction::RemapClamped(ControlPercent, MinimumControl, MaximumControl,
                DominatorGenerationTuning[Tier][MaximumColumn], DominatorGenerationTuning[Tier][MinimumColumn]));
        };
        InitializeDominator(Kind, Planet, Series);
        ControlPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
        std::int32_t Rating = pas::real_round(static_cast<float>(static_cast<double>(125 *
            aGalaxy::Galaxy->GetEffectiveDifficultyLevel()) + aMyFunction::RemapClamped(aGalaxy::Galaxy->CurrentTurn,
                pas::constant(static_cast<double>(aGalaxyStruct::GalaxyWarmupTurns)), 2.22E+4, 0.0, 3.0E+3)));
        std::int32_t WarRating = -150 * aGalaxy::Galaxy->WarDeltaWin[1];
        std::int32_t DistanceRating = 0;
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (CurrentStar->StarDistances[I].Star->Status.ControlFaction != aGalaxyStruct::sfDominators ||
                        CurrentStar->StarDistances[I].Star->Status.CustomFaction != u"") {
                        DistanceRating = 40 * I;
                        break;
                    }
                }
            }
        }
        if (WarRating > 0) {
            Rating = Rating + WarRating + DistanceRating / 4;
        } else if (WarRating + DistanceRating < 0) {
            Rating = Rating + WarRating + DistanceRating;
        } else {
            Rating += (WarRating + DistanceRating) / 4;
        }
        Rating = std::max<std::int32_t>(0, Rating);
        Tier = Rating / 1000 + 1;
        if (aMyFunction::NextRandomIntRange(0, 1000, RandomState) < Rating % 1000) {
            ++Tier;
        }
        if (Kind == aGalaxyStruct::ktBertor) {
            ++Tier;
        }
        if (Kind == aGalaxyStruct::ktKlig) {
            --Tier;
        }
        Tier = std::max<std::int32_t>(1, std::min<std::int32_t>(Tier, 7));
        if (aGalaxy::Galaxy->CurrentTurn >= 666) {
            if (aGalaxy::Galaxy->DominatorModLevel == 1) {
                Tier = std::max<std::int32_t>(Tier, 5);
            } else if (aGalaxy::Galaxy->DominatorModLevel == 2) {
                Tier = std::max<std::int32_t>(Tier, 6);
            } else if (aGalaxy::Galaxy->DominatorModLevel == 3) {
                Tier = std::max<std::int32_t>(Tier, 7);
            }
        }
        MinimumControl = 1;
        MaximumControl = DominatorGenerationTuning[Tier][1];
        MiddleControl = pas::real_round(aMyFunction::RemapClamped(aGalaxy::Galaxy->CurrentTurn,
                pas::constant(static_cast<double>(aGalaxyStruct::GalaxyWarmupTurns)), 1.125E+4, MaximumControl / 4,
                3 * MaximumControl / 4));
        ChameleonActive = false;
        GraphDominator = aGalaxy::Galaxy->GraphDominatorSurfacesEnabled;
        {
            std::uint8_t randomTuning = RandomTuning(2, 3);
            double cpp_left = aKling::RandomInteger(aConst::DominatorShipDefinitions[KlingType].MinimumHullSize,
                aConst::DominatorShipDefinitions[KlingType].MaximumHullSize, this);
            std::uint16_t round = pas::real_round(static_cast<float>(cpp_left * aConst::HullCapacityScale));
            aShip::TShip* self = this;
            aShip::TShip_CreateAndEquipHull(self, round, randomTuning, aGalaxyStruct::oiDominator, -1, false);
        }
        {
            std::uint8_t randomTuning_2 = RandomTuning(10, 11);
            std::int32_t sizeForKind = SizeForKind(aConst::EngineBaseSize);
            aShip::TShip* self_2 = this;
            self_2->CreateAndEquipEngine(sizeForKind, randomTuning_2, aGalaxyStruct::oiDominator);
        }
        {
            std::int32_t cpp_left_2 = aKling::RandomInteger(1, 100, this);
            if (cpp_left_2 <= InterpolatedTuning(14, 15)) {
                std::uint8_t randomTuning_3 = RandomTuning(4, 5);
                std::int32_t sizeForKind_2 = SizeForKind(aConst::RepairRobotBaseSize);
                aShip::TShip* self_3 = this;
                self_3->CreateAndEquipRepairRobot(sizeForKind_2, randomTuning_3, aGalaxyStruct::oiDominator);
            }
        }
        {
            std::int32_t cpp_left_3 = aKling::RandomInteger(1, 100, this);
            if (cpp_left_3 <= InterpolatedTuning(16, 17)) {
                std::uint8_t randomTuning_4 = RandomTuning(6, 7);
                std::int32_t sizeForKind_3 = SizeForKind(aConst::DefGeneratorBaseSize);
                aShip::TShip* self_4 = this;
                self_4->CreateAndEquipDefGenerator(sizeForKind_3, randomTuning_4, aGalaxyStruct::oiDominator);
            }
        }
        std::int32_t TechLevel = aGalaxy::Galaxy->TechLevel;
        {
            std::uint8_t randomInteger = aKling::RandomInteger(1, TechLevel, this);
            std::int32_t sizeForKind_4 = SizeForKind(aConst::FuelTanksBaseSize);
            aShip::TShip* self_5 = this;
            self_5->CreateAndEquipFuelTanks(sizeForKind_4, randomInteger, aGalaxyStruct::oiDominator);
        }
        {
            std::uint8_t randomInteger_2 = aKling::RandomInteger(1, TechLevel, this);
            std::int32_t sizeForKind_5 = SizeForKind(aConst::RadarBaseSize);
            aShip::TShip* self_6 = this;
            self_6->CreateAndEquipRadar(sizeForKind_5, randomInteger_2, aGalaxyStruct::oiDominator);
        }
        {
            std::uint8_t randomInteger_3 = aKling::RandomInteger(1, TechLevel, this);
            std::int32_t sizeForKind_6 = SizeForKind(aConst::ScannerBaseSize);
            aShip::TShip* self_7 = this;
            self_7->CreateAndEquipScanner(sizeForKind_6, randomInteger_3, aGalaxyStruct::oiDominator);
        }
        {
            std::int32_t SizeForKind_2 = SizeForKind(aConst::CargoHookBaseSize);
            std::int32_t min_2 = std::min<std::int32_t>(TechLevel, 7);
            std::uint8_t randomInteger_4 = aKling::RandomInteger(1, min_2, this);
            aShip::TShip* self_8 = this;
            self_8->CreateAndEquipCargoHook(SizeForKind_2, randomInteger_4, aGalaxyStruct::oiDominator);
        }
        std::int32_t Distribution = DominatorWeaponDistributionByTier[Tier];
        std::int32_t WeaponCount = RandomTuning(12, 13);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, WeaponCount); cpp_range.next(I); ) {
            Accepted = false;
            Attempts = 0;
            do {
                ++Attempts;
                if (Attempts > 1000) {
                    break;
                }
                WeaponType = aConst::t_IndustrialLaser;
                Roll = aKling::RandomInteger(1, 100, this);
                Chosen = false;
                WeightSum = 0;
                while (WeaponType <= aConst::t_TorpedoTube && static_cast<std::uint8_t>(Chosen ^ 1)) {
                    WeightSum += DominatorWeaponWeights[Distribution][KlingType][WeaponType];
                    if (Roll <= WeightSum) {
                        MaximumSizeIndex = DominatorEquipmentSizeIndices[KlingType][0];
                        MinimumSizeIndex = DominatorEquipmentSizeIndices[KlingType][1];
                        if (aConst::WeaponInfos[WeaponType].ShotType == aGalaxyStruct::wstAreaDamage) {
                            MaximumSizeIndex = 2;
                            MinimumSizeIndex = 1;
                        }
                        Accepted = !aGalaxy::Galaxy->AreDominatorRacialWeaponsEnabled() || (DominatorSeries !=
                                    aGalaxyStruct::dsBlazer || !pas::is_one_of<aConst::t_IMHO9000,
                                    aConst::t_Vertix>(WeaponType)) && (DominatorSeries != aGalaxyStruct::dsTerron ||
                                    !pas::is_one_of<aConst::t_Vertix, aConst::t_TorpedoTube>(WeaponType)) &&
                                    (DominatorSeries != aGalaxyStruct::dsKeller || !pas::is_one_of<aConst::t_IMHO9000,
                                    aConst::t_TorpedoTube>(WeaponType));
                        if (Accepted) {
                            std::uint8_t randomTuning_5 = RandomTuning(8, 9);
                            std::int32_t randomEquipmentSize = aKling::RandomEquipmentSize(
                                aConst::WeaponInfos[WeaponType].AverageSize, MaximumSizeIndex, MinimumSizeIndex, this);
                            aShip::TShip* self_9 = this;
                            self_9->CreateAndEquipWeapon(WeaponType, randomEquipmentSize, randomTuning_5,
                                aGalaxyStruct::oiDominator);
                        }
                        Chosen = true;
                    }
                    pas::inc(WeaponType);
                }
            } while (!Accepted);
        }
        Roll = GetCargoFreeSpace();
        if (Roll < 0) {
            GetHull()->Weight += pas::abs(Roll);
            if (GetEngine() != nullptr) {
                GetEngine()->Improve(aItem::ikAny);
            }
        }
        GetHull()->HullPoints = GetHull()->Weight;
        RefreshGraphicSize();
        RefreshDerivedStats(true);
        if (GetScanner() != nullptr && aKling::RandomInteger(1, 100, this) > ControlPercent) {
            GetScanner()->Improve(aItem::ikAny);
        }
        if (GetRadar() != nullptr && aKling::RandomInteger(1, 100, this) > ControlPercent) {
            GetRadar()->Improve(aItem::ikAny);
        }
        if (GetCargoHook() != nullptr && aKling::RandomInteger(1, 100, this) > ControlPercent) {
            GetCargoHook()->Improve(aItem::ikAny);
        }
        if (GetFuelTanks() != nullptr && aKling::RandomInteger(1, 100, this) > ControlPercent) {
            GetFuelTanks()->Improve(aItem::ikAny);
        }
        std::int32_t ImprovementChance = RandomTuning(18, 19);
        if (ImprovementChance > 0) {
            switch (DominatorSeries) {
                case aGalaxyStruct::dsBlazer: {
                    for (I = 1; I <= 5; ++I) {
                        if (Weapons[I] != nullptr && aKling::RandomInteger(1, 100, this) <= ImprovementChance) {
                            Weapons[I]->Improve(aItem::ikAny);
                        }
                    }
                    break;
                }
                case aGalaxyStruct::dsKeller: {
                    if (GetDefGenerator() != nullptr && aKling::RandomInteger(1, 100, this) <= ImprovementChance) {
                        GetDefGenerator()->Improve(aItem::ikAny);
                    }
                    break;
                }
                case aGalaxyStruct::dsTerron: {
                    if (GetRepairRobot() != nullptr && aKling::RandomInteger(1, 100, this) <= ImprovementChance) {
                        GetRepairRobot()->Improve(aItem::ikAny);
                    }
                    break;
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = pas::list_count(Inventory) - 1;
            if (1 <= cpp_last_2) {
                for (I = 1; I <= cpp_last_2; ++I) {
                    Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
                    Equipment->ConditionPercent = aMyFunction::NextRandomIntRange(5, 100, RandomState);
                }
            }
        }
        SetInventoryDominatorOwner();
        RefreshDerivedStats(true);
        this->virtual_TShip_RefreshCurrentStanding();
    }

    static std::int32_t RandomInteger(std::int32_t BoundA, std::int32_t BoundB, TKling* Self) {
        return aMyFunction::NextRandomIntRange(BoundA, BoundB, Self->RandomState);
    }

    static std::int32_t RandomEquipmentSize(std::int32_t BaseSize, std::int32_t MinimumSizeIndex,
        std::int32_t MaximumSizeIndex, TKling* Self) {
        return aKling::RandomInteger(
            pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(BaseSize) * aConst::EquipmentSizeFactors[MinimumSizeIndex]) * 0x1.ccccccccccccbp-1)),
            pas::real_round(static_cast<float>(static_cast<double>(BaseSize) * aConst::EquipmentSizeFactors[MaximumSizeIndex])), Self);
    }

    void TKling::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aShip::TShip::SaveToBuffer(Buffer);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(KlingType));
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(DominatorSeries));
        Buffer->AddIntegerValue(ActiveProgramAppliedTurn);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(ActiveProgramId));
    }

    void TKling::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aShip::TShip::LoadFromBuffer(Buffer, Galaxy);
        KlingType = static_cast<aGalaxyStruct::TKlingType>(Buffer->GetByte());
        DominatorSeries = static_cast<aGalaxyStruct::TDominatorSeries>(Buffer->GetByte());
        ActiveProgramAppliedTurn = Buffer->GetInt32();
        ActiveProgramId = static_cast<aGalaxyStruct::TProgramIndex>(Buffer->GetByte());
        if (GlobalsV::LoadedSaveVersion <= 147) {
            ClearRecentlyDroppedItems();
        }
    }

    void TKling_NextDay(TKling* Self) {
        try {
            if ((static_cast<std::int32_t>(Self->Seed) + aGalaxy::Galaxy->CurrentTurn) % 100 == 0) {
                Self->CalculateStrength();
            }
            Self->AuraEffectShownThisTurn = false;
            aShip::TShip_NextDay(Self);
            if (Self->ScriptShip != nullptr && Self->HasScriptControl()) {
                Self->ScriptNextDay();
                if (Self->ScriptShip != nullptr && Self->KlingType != aGalaxyStruct::ktBoss) {
                    return;
                }
            }
            if (Self->KlingType == aGalaxyStruct::ktBoss) {
                if (Self == BlazerShip) {
                    Self->BlazerNextDayLogic();
                } else if (Self == KellerShip) {
                    Self->KellerNextDayLogic();
                } else if (Self == TerronShip) {
                    Self->TerronNextDayLogic();
                } else {
                    Self->virtual_TShip_NextDayLogic();
                }
            } else if (Self->KlingType == aGalaxyStruct::ktBertor) {
                Self->MiniBossNextDayLogic();
            } else {
                Self->virtual_TShip_NextDayLogic();
            }
            if (Self->ScriptShip != nullptr && !Self->HasScriptControl()) {
                Self->ScriptNextDay();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TKling.NextDay ", Self->GetFullName(u" "_wref.get())}))));
            } else {
                throw;
            }
        }
    }

    void TKling_NextDayLogic(TKling* Self) {
        aShip::TShip* Ship{};
        std::int32_t Stage = 0;
        try {
            if (Self->CurrentPlanet != nullptr) {
                Stage = 1;
                Self->RepairBrokenEquipmentAtLocation();
                Self->RefuelAtLocation();
                Self->RefreshDerivedStats(true);
                Self->OrderTakeoff();
            } else if (BlazerShip != nullptr && Self->DockedTo == BlazerShip || KellerShip != nullptr &&
                    Self->DockedTo == KellerShip) {
                Self->OrderTakeoff();
            } else if (TerronShip != nullptr && Self->DockedTo == TerronShip) {
                Self->DestroyQueued = true;
            } else if (Self->InNormalSpace()) {
                Stage = 2;
                if (Self->IsProgramActive(aGalaxyStruct::prgDisconnection)) {
                    return;
                }
                Self->AssignWeaponTargetsInStar();
                if (Self->IsProgramActive(aGalaxyStruct::prgInsanity)) {
                    Self->MoveToRandomPatrolPoint();
                    return;
                }
                if (Self->DominatorSeries == aGalaxyStruct::dsTerron && TerronShip != nullptr &&
                    !Self->HasIndependentScriptFaction() && aGalaxy::Galaxy->TerronToStarTurn >=
                        aGalaxyStruct::TerronTransformationFlag) {
                    if (Self->CurrentStar != TerronShip->CurrentStar) {
                        Self->OrderJump(TerronShip->CurrentStar, false);
                    } else {
                        Self->OrderLanding(TerronShip, false);
                    }
                } else {
                    Stage = 3;
                    if (Self->Order == aShip::soFollowShip && Self->OrderTarget != nullptr &&
                        pas::class_cast_if<aShip::TShip*>(Self->OrderTarget) != nullptr && Self->OrderTarget !=
                            Self->EnemyShip && !pas::is_one_of<aShip::soMove,
                            aShip::soJump>(static_cast<aShip::TShip*>(Self->OrderTarget)->Order)) {
                        Self->OrderNone(false);
                    }
                    if (Self->PartnerShip != nullptr && Self->PartnerShip->CurrentStar != Self->CurrentStar) {
                        Self->OrderJump(Self->PartnerShip->CurrentStar, true);
                    } else if (Self->PartnerShip != nullptr && Self->PartnerShip->InNormalSpace() &&
                        Self->PartnerShip->Order == aShip::soJump && Self->PartnerShip->EstimateOrderTravelTurns() <=
                            3) {
                        Self->OrderFollowShip(Self->PartnerShip, aShip::fmFollowNear, true);
                    } else if ((static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1) ||
                        !(pas::class_cast_if<aShip::TShip*>(Self->OrderTarget) != nullptr)) &&
                        (!(pas::class_cast_if<aGalaxy::TStar*>(Self->OrderTarget) != nullptr) ||
                            Self->EstimateOrderTravelTurns() >= 3)) {
                        Stage = 4;
                        Self->SelectEnemyShipInStar();
                        Stage = 5;
                        Self->EngageEnemyShip();
                        Stage = 6;
                        Self->UpdateAfterburnerState();
                        Stage = 7;
                        if (Self->Order == aShip::soNone && Self->HasHullDamageOrBrokenEquippedItems()) {
                            Self->LandOnRandomFriendlyPlanet(false);
                        }
                        Stage = 8;
                        if (Self->Order == aShip::soNone && Self->CurrentStar->Id ==
                                aGalaxy::Galaxy->KellerResearchTargetStarId && KellerShip != nullptr &&
                            aMyFunction::NextRandomIntRange(0, 99, Self->RandomState) < 20 &&
                            !Self->HasIndependentScriptFaction()) {
                            Self->LandOnRandomFriendlyPlanet(false);
                        }
                        Stage = 9;
                        if (Self->Order == aShip::soNone && KellerShip != nullptr && Self->CurrentStar ==
                                KellerShip->CurrentStar && KellerShip->InNormalSpace() &&
                            aMyFunction::NextRandomIntRange(0, 99, Self->RandomState) < 20) {
                            Self->OrderMove(KellerShip->Position, false);
                        }
                        Stage = 10;
                        if (pas::in_range(Self->Order, static_cast<std::int32_t>(aShip::soNone),
                            static_cast<std::int32_t>(aShip::soMove))) {
                            Stage = 11;
                            Ship = Self->SelectBertorLeader();
                            if (Ship != nullptr) {
                                if (System::Sqrt(static_cast<float>(pas::sqr(static_cast<double>(Ship->Position.X)) +
                                    pas::sqr(static_cast<double>(Ship->Position.Y)))) > std::min<std::int32_t>(
                                        8 * Ship->Speed, 2 * Self->Speed)) {
                                    Stage = 12;
                                    Self->OrderFollowShip(Ship, aShip::fmMinWeaponRange, false);
                                }
                            }
                        }
                        Stage = 13;
                        if (Self->Order == aShip::soNone) {
                            Self->MoveToRandomPatrolPoint();
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TKling.NextDayLogic ", Self->GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    void TKling::MiniBossNextDayLogic() {
        std::int32_t Stage = 0;
        try {
            if (CurrentPlanet != nullptr) {
                Stage = 1;
                RepairBrokenEquipmentAtLocation();
                RefuelAtLocation();
                RefreshDerivedStats(true);
                OrderTakeoff();
                return;
            } else if (TerronShip != nullptr && DockedTo == TerronShip) {
                DestroyQueued = true;
                return;
            } else if (DockedTo != nullptr) {
                OrderTakeoff();
                return;
            } else {
                if (!InNormalSpace()) {
                    return;
                }
                Stage = 2;
                AssignWeaponTargetsInStar();
                if (DominatorSeries == aGalaxyStruct::dsTerron && TerronShip != nullptr &&
                    aGalaxy::Galaxy->TerronToStarTurn >= aGalaxyStruct::TerronTransformationFlag) {
                    if (CurrentStar != TerronShip->CurrentStar) {
                        OrderJump(TerronShip->CurrentStar, false);
                    } else {
                        OrderLanding(TerronShip, false);
                    }
                    return;
                } else {
                    Stage = 3;
                    if (OrderAbsolute && pas::class_cast_if<aShip::TShip*>(OrderTarget) != nullptr ||
                        pas::class_cast_if<aGalaxy::TStar*>(OrderTarget) != nullptr && EstimateOrderTravelTurns() < 3) {
                        return;
                    }
                    Stage = 4;
                    SelectEnemyShipInStar();
                    EngageEnemyShip();
                    if (Order == aShip::soNone && HasHullDamageOrBrokenEquippedItems()) {
                        LandOnRandomFriendlyPlanet(false);
                    }
                    if (Order == aShip::soNone && CurrentStar->Id == aGalaxy::Galaxy->KellerResearchTargetStarId &&
                        KellerShip != nullptr && aMyFunction::NextRandomIntRange(0, 99, RandomState) < 20) {
                        LandOnRandomFriendlyPlanet(false);
                    }
                    if (Order == aShip::soNone && KellerShip != nullptr && CurrentStar == KellerShip->CurrentStar &&
                        KellerShip->InNormalSpace() && aMyFunction::NextRandomIntRange(0, 99, RandomState) < 20) {
                        OrderMove(KellerShip->Position, false);
                    }
                    Stage = 5;
                    if (pas::in_range(Order, static_cast<std::int32_t>(aShip::soNone),
                        static_cast<std::int32_t>(aShip::soMove)) && (CurrentStar->Id !=
                            aGalaxy::Galaxy->KellerResearchTargetStarId || KellerShip == nullptr)) {
                        if (pas::imod(aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(Seed),
                            200 - 20 * pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 8.0))) == 0) {
                            RelocateBertorWithinConstellation();
                        }
                    }
                    if (Order == aShip::soNone) {
                        MoveToRandomPatrolPoint();
                    }
                    Stage = 6;
                    if (Order != aShip::soJump) {
                        if (SpawnEscortShips(aGalaxyStruct::ktKlig, 5) > 0) {
                            OrderNone(false);
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TKling.MiniBossNextDayLogic ", GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    void TKling::BlazerNextDayLogic() {
        std::int32_t Stage = 0;
        try {
            if (CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] == pas::list_count(CurrentStar->Ships)) {
                Stage = 1;
                RepairBrokenEquipmentAtLocation();
                GetHull()->HullPoints = GetHull()->Weight;
                RefuelAtLocation();
                RefreshDerivedStats(true);
            } else if (aPlayer::GetPlayer()->CurrentStar != CurrentStar) {
                Stage = 2;
                RepairBrokenEquipmentAtLocation();
                GetHull()->HullPoints = std::max<std::int32_t>(GetHull()->HullPoints, GetHull()->Weight / 4);
                RefuelAtLocation();
            }
            if (InNormalSpace()) {
                Stage = 3;
                if (aGalaxy::Galaxy->BlazerLandingPlanetId > 0) {
                    Stage = 4;
                    OrderLanding(
                        static_cast<pas::Object*>(aGalaxy::Galaxy->IdToPlanet(aGalaxy::Galaxy->BlazerLandingPlanetId,
                            true)), false);
                } else {
                    Stage = 5;
                    CoordinateSeriesInvasions(aGalaxyStruct::dsBlazer);
                    if (!OrderAbsolute) {
                        Stage = 6;
                        if (DaysSincePlayerSeen > 3) {
                            if (pas::imod(aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(Seed),
                                10 - pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetEffectiveDifficultyLevel() / 8.0))) == 0 &&
                                CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] >= 1) {
                                RetreatToReinforcedStar();
                            }
                        }
                        if (Order == aShip::soNone) {
                            MoveToRandomPatrolPoint();
                        }
                    }
                    Stage = 7;
                    AssignWeaponTargetsInStar();
                    if (static_cast<std::uint8_t>(RetreatIfHullCritical() ^ 1) && aPlayer::GetPlayer()->CurrentStar ==
                            CurrentStar) {
                        SelectEnemyShipInStar();
                        EngageEnemyShip();
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TKling.BlazerNextDayLogic ", GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    void TKling::KellerNextDayLogic() {
        std::int32_t IntervalBonus{};
        aGalaxy::THole* Hole{};
        std::int32_t Stage = 0;
        try {
            RepairBrokenEquipmentAtLocation();
            RefuelAtLocation();
            if (aPlayer::GetPlayer()->CurrentStar != CurrentStar) {
                GetHull()->HullPoints = std::max<std::int32_t>(GetHull()->HullPoints, GetHull()->Weight / 2);
            }
            Stage = 1;
            SelectKellerMission();
            SelectKellerReinforcementMission();
            if (InNormalSpace()) {
                CoordinateSeriesInvasions(aGalaxyStruct::dsKeller);
                if (aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfDominators) < 2 && aPlayer::GetPlayer() !=
                        nullptr && (aPlayer::GetPlayer()->IsOutsideStarSpace() || aPlayer::GetPlayer()->CurrentStar !=
                            CurrentStar)) {
                    IntervalBonus = 30;
                } else {
                    IntervalBonus = 1;
                }
                if (aGalaxy::Galaxy->KellerResearchTargetStarId != 0 && aGalaxy::Galaxy->KellerResearchTargetStarId !=
                        CurrentStar->Id || pas::imod(aGalaxy::Galaxy->CurrentTurn,
                        aGalaxy::Galaxy->ScaleIntByTechLevel(5, IntervalBonus + 30)) == 0 || GetHull()->HullPoints <
                        1300 || aPlayer::GetPlayer() != nullptr && (aPlayer::GetPlayer()->CurrentStar == CurrentStar ||
                            aPlayer::GetPlayer()->OrderTarget == CurrentStar) && aGalaxy::Galaxy->TechLevel < 7 &&
                        aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) < 70) {
                    Stage = 2;
                    Hole = aGalaxy::Galaxy->FindHoleInStarByKind(CurrentStar, 4);
                    if (Hole != nullptr) {
                        if (Hole->CreatedTurn + 3 <= aGalaxy::Galaxy->CurrentTurn &&
                            (aGalaxy::Galaxy->KellerResearchTargetStarId == 0 || GetHull()->HullPoints < 700) ||
                            aGalaxy::Galaxy->KellerResearchTargetStarId != 0 &&
                                aGalaxy::Galaxy->KellerResearchTargetStarId != CurrentStar->Id) {
                            OrderJumpHole(Hole, false);
                            aGalaxy::Galaxy->KellerMissionState = 4;
                        }
                    }
                }
                Stage = 3;
                if (Order == aShip::soNone) {
                    MoveNearKellerMissionHole();
                }
                AssignWeaponTargetsInStar();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TKling.KellerNextDayLogic ", GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    void TKling::TerronNextDayLogic() {
        std::int32_t Stage = 0;
        try {
            RepairBrokenEquipmentAtLocation();
            RefuelAtLocation();
            if (aPlayer::GetPlayer()->CurrentStar != CurrentStar) {
                GetHull()->HullPoints = std::max<std::int32_t>(GetHull()->HullPoints, GetHull()->Weight / 4);
            }
            if (InNormalSpace()) {
                if (aGalaxy::Galaxy->TerronToStarTurn > 0) {
                    OrderMove(EC_Struct::MakePointF(-1.0E+2f, -1.0E+2f), false);
                } else {
                    Stage = 1;
                    CoordinateSeriesInvasions(aGalaxyStruct::dsTerron);
                    OrderMove(EC_Struct::MakePointF(0.0f, 0.0f), false);
                    Stage = 2;
                    if (aGalaxy::Galaxy->TerronWeaponLockTurn == 0) {
                        AssignWeaponTargetsInStar();
                    }
                    if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
                        SelectEnemyShipInStar();
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({
                    static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({
                    u"Error in procedure TKling.TerronNextDayLogic ", GetFullName(u" "_wref.get()), u" label = ",
                    pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    aShip::TShip* TKling::SelectBertorLeader() {
        aShip::TShip* Ship{};
        std::int32_t I{};
        aShip::TShip* Result = nullptr;
        std::uint32_t Count = 0u;
        std::uint8_t PlayerIsBertor = aPlayer::GetPlayer()->CurrentStar == CurrentStar &&
            aPlayer::GetPlayer()->InNormalSpace() && IsPlayerCamouflageEffective(aPlayer::GetPlayer()) &&
            aPlayer::GetPlayer()->ChameleonActive && pas::in_range(aPlayer::GetPlayer()->ChameleonVisualType,
                static_cast<std::int32_t>(aGalaxyStruct::ktBertor), static_cast<std::int32_t>(aGalaxyStruct::ktBertor));
        if (PlayerIsBertor) {
            Result = aPlayer::GetPlayer();
            ++Count;
        }
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship->InNormalSpace() && pas::class_cast_if<TKling*>(Ship) != nullptr && pas::in_range(
                            static_cast<TKling*>(Ship)->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktBertor),
                            static_cast<std::int32_t>(aGalaxyStruct::ktBertor)) &&
                        static_cast<TKling*>(Ship)->DominatorSeries == DominatorSeries) {
                        if (Ship == PartnerShip) {
                            return Ship;
                        }
                        if (Count == 0) {
                            Result = Ship;
                        } else {
                            Result = nullptr;
                        }
                        ++Count;
                    }
                }
            }
        }
        if (Count > 1) {
            Count = pas::imod(Seed + CurrentStar->GenerationSeed, Count);
            if (PlayerIsBertor) {
                if (Count == 0) {
                    return aPlayer::GetPlayer();
                }
                --Count;
            }
            {
                const std::int32_t cpp_last_2 = pas::list_count(CurrentStar->Ships) - 1;
                if (0 <= cpp_last_2) {
                    for (I = 0; I <= cpp_last_2; ++I) {
                        Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                        if (Ship->InNormalSpace() && pas::class_cast_if<TKling*>(Ship) != nullptr && pas::in_range(
                                static_cast<TKling*>(Ship)->KlingType,
                                static_cast<std::int32_t>(aGalaxyStruct::ktBertor),
                                static_cast<std::int32_t>(aGalaxyStruct::ktBertor)) &&
                            static_cast<TKling*>(Ship)->DominatorSeries == DominatorSeries) {
                            if (Count == 0) {
                                return Ship;
                            }
                            --Count;
                        }
                    }
                }
            }
        }
        return Result;
    }

    // Requires a live enemy in the same star and KlingType=ktKlig. Existing kamikaze mode bypasses the proximity/strength test.
    std::uint8_t TKling::ShouldKamikaze() {
        std::uint8_t Result = false;
        if (KlingType != aGalaxyStruct::ktKlig) {
            return Result;
        }
        if (EnemyShip == nullptr) {
            return Result;
        }
        if (!EnemyShip->InNormalSpace()) {
            return Result;
        }
        if (EnemyShip->CurrentStar != CurrentStar) {
            return Result;
        }
        if (EnemyShip->IsHullDestroyed()) {
            return Result;
        }
        if (AfterburnerActive) {
            return true;
        }
        if (aMyFunction::PointDistance(Position, EnemyShip->Position) > static_cast<float>(Speed * 1.5)) {
            return Result;
        }
        if (aShip::TShip_ChanceToWin(this, EnemyShip) <= 0.2) {
            return true;
        }
        return Result;
    }

    std::uint8_t TKling::LandOnRandomFriendlyPlanet(std::uint8_t OverrideScriptOrder) {
        BuildReachablePlanetQueue();
        if (pas::list_count(PlanetQueue) > 0) {
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(PlanetQueue) - 1,
                    RandomState);
                pas::List* planetQueue = PlanetQueue;
                pas::Object* cpp_arg = pas::list_at<pas::Object>(planetQueue, nextRandomIntRange);
                aShip::TShip* self = this;
                self->OrderLanding(cpp_arg, OverrideScriptOrder);
            }
            return true;
        }
        return false;
    }

    void TKling::BuildReachablePlanetQueue() {
        std::int32_t I{};
        aPlanet::TPlanet* Planet{};
        ClearPlanetQueue();
        PlanetQueue = pas::make_object<pas::List>();
        if (Speed != 0 && CurrentStar->Status.CustomFaction == u"") {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Planets) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, I);
                    if (Planet->OwnerId == aGalaxyStruct::oiDominator || Planet->CurrentStar->Id ==
                            aGalaxy::Galaxy->KellerResearchTargetStarId && Planet->OwnerId !=
                                aGalaxyStruct::oiUninhabited && KellerShip != nullptr) {
                        pas::list_add(PlanetQueue, reinterpret_cast<void*>(Planet));
                    }
                }
            }
        }
    }

    std::uint8_t TKling::RetreatToReinforcedStar() {
        static const pas::Set<0, 255> DominatorShipMask = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::stKling}});
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        std::uint8_t Result = false;
        pas::List* Stars = pas::make_object<pas::List>();
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                    if (Star != CurrentStar && Star->Constellation->Id != 20 &&
                        !aScript::IsStarProtectedByScript(Star) && (Star->ShipTypeCounts[aGalaxyStruct::stKling] >=
                            10 ||
                            Star->SumBestRangerRelativeStrength(
                                static_cast<aGalaxyStruct::TShipTypeMask>(DominatorShipMask)) >=
                                aConst::DominatorRetreatStrengthByTier[Star->Constellation->HomeDistanceTier]) &&
                        Star->ShipTypeCounts[aGalaxyStruct::stKling] >= 6 && (KellerShip == nullptr ||
                            !KellerShip->InNormalSpace() || KellerShip->CurrentStar != Star) && (TerronShip ==
                            nullptr || !TerronShip->InNormalSpace() || TerronShip->CurrentStar != Star) &&
                        Star->Status.ControlFaction == aGalaxyStruct::sfDominators && Star->Status.DominatorSeries ==
                            DominatorSeries && Star->Status.CustomFaction == u"" && Star->Status.Battle == 0) {
                        pas::list_add(Stars, reinterpret_cast<void*>(Star));
                    }
                }
            }
        }
        if (pas::list_count(Stars) > 2) {
            I = pas::list_indexof(Stars, reinterpret_cast<void*>(TransitOriginStar));
            if (I > 0) {
                pas::list_delete(Stars, I);
            }
        }
        if (pas::list_count(Stars) > 0) {
            {
                aGalaxy::TStar* cpp_arg = pas::list_at<aGalaxy::TStar>(Stars,
                    aMyFunction::NextRandomIntRange(0, pas::list_count(Stars) - 1, RandomState));
                aShip::TShip* self = this;
                self->OrderJump(cpp_arg, true);
            }
            Result = true;
        }
        pas::free(Stars);
        return Result;
    }

    std::uint8_t TKling::RetreatIfHullCritical() {
        std::uint8_t Result = false;
        if (GetHull()->HullPoints <= GetHull()->Weight / 3) {
            Result = true;
            if (Order != aShip::soJump && static_cast<std::uint8_t>(RetreatToReinforcedStar() ^ 1)) {
                return false;
            }
        }
        return Result;
    }

    aGalaxy::TStar* TKling::FindKellerAttackTarget() {
        static const pas::Set<0, 255> CoalitionShipMask = pas::constant_set<pas::Set<0, 255>>({
            {aGalaxyStruct::stRanger, aGalaxyStruct::stTranclucator}});
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Score{};
        std::int32_t Index{};
        aGalaxy::TStar* Star{};
        aGalaxy::TStar* Neighbor{};
        std::int32_t BestScore = SystemImports::MaxInt;
        aGalaxy::TStar* BestStar = nullptr;
        Index = aMyFunction::SeededRandomIntRange(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1,
                static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) + aGalaxy::Galaxy->GenerationSeed);
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    aMyFunction::IncrementWrapped(Index, 0, pas::list_count(aGalaxy::Galaxy->Stars) - 1);
                    Star =
                        pas::checked_cast<aGalaxy::TStar*>(
                            static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                    if (Star->Constellation->Id != 20 && (BlazerShip == nullptr || BlazerShip->CurrentStar != Star) &&
                        (TerronShip == nullptr || TerronShip->CurrentStar != Star) && (Star->Status.ControlFaction !=
                            aGalaxyStruct::sfDominators || Star->Status.Battle != 0)) {
                        if (aGalaxy::Galaxy->CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns ||
                            aMyFunction::PointDistanceSquared(Star->Position,
                                aPlayer::GetPlayer()->CurrentStar->Position) >= pas::sqr(static_cast<float>(static_cast<float>((static_cast<float>(1.0 -
                                    static_cast<float>(aGalaxy::Galaxy->CurrentTurn /
                                    pas::constant(static_cast<double>(aGalaxyStruct::GalaxyWarmupTurns))))) * 7.0E+1) +
                                    35.0))) {
                            if ((Star->Status.Battle == 0 || Star->Status.DominatorSeries != aGalaxyStruct::dsKeller ||
                                Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 6) &&
                                !aScript::IsStarProtectedByScript(Star)) {
                                Score = 0;
                                {
                                    const std::int32_t cpp_last_2 = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
                                    if (1 <= cpp_last_2) {
                                        for (J = 1; J <= cpp_last_2; ++J) {
                                            Neighbor =
                                                pas::checked_cast<aGalaxy::TStar*>(
                                                    static_cast<pas::Object*>(Star->StarDistances[J].Star));
                                            if (Neighbor->Status.ControlFaction == aGalaxyStruct::sfCoalition &&
                                                Neighbor->Status.CustomFaction == u"") {
                                                Score += Star->StarDistances[J].Distance;
                                            } else if (Neighbor->Status.Battle != 0) {
                                                Score += 2 * Star->StarDistances[J].Distance;
                                            }
                                        }
                                    }
                                }
                                if (Star->Status.Battle != 0 && Star->Status.DominatorSeries !=
                                        aGalaxyStruct::dsKeller) {
                                    Score = pas::real_round(2 * Score);
                                }
                                Score = pas::real_round(static_cast<float>(static_cast<double>(Score) * aMyFunction::RemapClamped(
                                            Star->CountShipsByTypeMask(
                                            static_cast<aGalaxyStruct::TShipTypeMask>(CoalitionShipMask)), 1.0, 1.0E+1,
                                            2.0, 1.0E+1)));
                                Score = pas::real_round(static_cast<float>(static_cast<double>(Score) * aMyFunction::NextRandomFloatRange(
                                            1.0, 3.0, RandomState)));
                                if (aPlayer::GetPlayer()->HomePlanet->CurrentStar == Star &&
                                    aGalaxy::Galaxy->CurrentTurn < static_cast<float>(static_cast<float>(aGalaxy::Galaxy->InterpolateDifficulty(-1, 1.2f,
                                            1.0f, 0.7f, 0.5f) * 8.0E+2) +
                                            pas::constant(static_cast<double>(aGalaxyStruct::GalaxyWarmupTurns)))) {
                                    Score = SystemImports::MaxInt - 1;
                                }
                                if (Score < BestScore) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                    }
                }
            }
        }
        return BestStar;
    }

    aGalaxy::TStar* TKling::FindKellerReinforcementTarget() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Score{};
        std::int32_t Index{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        std::int32_t BestScore = SystemImports::MaxInt;
        aGalaxy::TStar* BestStar = nullptr;
        Index = aMyFunction::SeededRandomIntRange(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1,
                static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) + aGalaxy::Galaxy->GenerationSeed);
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    aMyFunction::IncrementWrapped(Index, 0, pas::list_count(aGalaxy::Galaxy->Stars) - 1);
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, Index);
                    if (Star->Constellation->Id != 20 && (BlazerShip == nullptr || BlazerShip->CurrentStar != Star) &&
                        (TerronShip == nullptr || TerronShip->CurrentStar != Star) && Star->Status.ControlFaction ==
                            aGalaxyStruct::sfDominators && (Star->Status.DominatorSeries != aGalaxyStruct::dsKeller ||
                            Star->Status.CustomFaction != u"") && !aScript::IsStarProtectedByScript(Star)) {
                        Score = 0;
                        {
                            const std::int32_t cpp_last_2 = pas::list_count(Star->Ships) - 1;
                            if (0 <= cpp_last_2) {
                                for (J = 0; J <= cpp_last_2; ++J) {
                                    Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                                    if (static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) &&
                                        pas::class_cast_if<TKling*>(Ship) != nullptr) {
                                        if (static_cast<TKling*>(Ship)->DominatorSeries == aGalaxyStruct::dsKeller) {
                                            --Score;
                                        } else {
                                            ++Score;
                                        }
                                    }
                                }
                            }
                        }
                        if (Score >= 0 && Score < BestScore) {
                            BestScore = Score;
                            BestStar = Star;
                        }
                    }
                }
            }
        }
        return BestStar;
    }

    void TKling::SelectKellerMission() {
        pas::WideString Text{};
        if (aGalaxy::Galaxy->KellerMissionState == 0 && aGalaxy::Galaxy->KellerLeaveTurn == 0) {
            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(60, 80, RandomState);
            std::int32_t cpp_right = cpp_left + aGalaxy::Galaxy->ScaleIntByTechLevel(20, 0);
            bool cpp_condition = pas::imod(aGalaxy::Galaxy->CurrentTurn, cpp_right) == 0 ||
                aGalaxy::Galaxy->CurrentTurn % 230 == 0;
            if (!cpp_condition) {
                std::int32_t cpp_right_2 = aMyFunction::NextRandomIntRange(5, 10, RandomState);
                cpp_condition = pas::imod(aGalaxy::Galaxy->CurrentTurn, cpp_right_2) == 0 &&
                        aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfDominators) < 2;
            }
            if (cpp_condition || aGalaxy::Galaxy->KellerResearchTargetStarId != 0) {
                if (aGalaxy::Galaxy->KellerResearchTargetStarId == 0) {
                    aGalaxy::Galaxy->KellerTargetStar = FindKellerAttackTarget();
                } else {
                    aGalaxy::Galaxy->KellerTargetStar =
                        aGalaxy::Galaxy->IdToStar(aGalaxy::Galaxy->KellerResearchTargetStarId);
                }
                if (aGalaxy::Galaxy->KellerTargetStar != nullptr) {
                    aGalaxy::Galaxy->KellerMissionState = 1;
                    if (aPlayer::GetPlayer() != nullptr &&
                        aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
                        Text = ([&] {
                                auto name = pas::borrow(aGalaxy::Galaxy->KellerTargetStar->Name);
                                pas::WideString localizedColorText = aConst::LocalizedColorText(
                                    u"Artefacts.ArtAnalyzer.KellerHole"_wref.get());
                                pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                                return aMyFunction::FormatText1(std::move(localizedColorText),
                                    std::move(textHighlightColorTag), u"<Star>"_w, name.get());
                            }());
                        if (Text != u"") {
                            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, Text,
                                u""_wref.get());
                        }
                    }
                }
            }
        }
    }

    void TKling::SelectKellerReinforcementMission() {
        bool cpp_condition = aGalaxy::Galaxy->KellerMissionState == 0 && aGalaxy::Galaxy->KellerResearchTargetStarId ==
                0 && aGalaxy::Galaxy->KellerLeaveTurn == 0;
        if (cpp_condition) {
            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(0, 1000, RandomState) + 1000;
            cpp_condition = cpp_left <= aGalaxy::Galaxy->CurrentTurn;
        }
        if (cpp_condition) {
            std::int32_t cpp_left_2 = aMyFunction::NextRandomIntRange(0, 10000, RandomState);
            std::int32_t cpp_left_3 = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
            if (cpp_left_2 < cpp_left_3 * (100 -
                        aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators))) {
                double cpp_left_4 = aMyFunction::NextRandomIntRange(0, 1000, RandomState);
                if (cpp_left_4 > aMyFunction::RemapClamped(
                        aGalaxy::TGalaxy::GetDominatorSeriesControlShare(aGalaxyStruct::dsKeller), 0.7, 1.2, 0.0,
                        1.0E+3)) {
                    aGalaxy::Galaxy->KellerTargetStar = TKling::FindKellerReinforcementTarget();
                    if (aGalaxy::Galaxy->KellerTargetStar != nullptr) {
                        aGalaxy::Galaxy->KellerMissionState = 1;
                    }
                }
            }
        }
    }

    // Advances mission state 2 to 3, creates the type-4 hole and sends Keller through it with generated reinforcements.
    void TKling::OpenKellerMissionHole() {
        TKling* Ship{};
        aGalaxy::THole* Hole{};
        float Angle{};
        float Radius{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t Threshold{};
        std::int32_t RandomMaximum{};
        aGalaxyStruct::TDominatorSeries Series{};
        auto SpawnReinforcement = [&](std::int32_t OrderData) -> void {
            Ship =
                pas::checked_cast<TKling*>(
                    static_cast<pas::Object*>(DominatorSpawnPlanet->SpawnWeightedDominatorShip()));
            Ship->Order = aShip::soJumpHole;
            Ship->OrderTarget = Hole;
            if (OrderData == 0) {
                Ship->OrderStateData = DominatorSpawnPlanet->CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] |
                        0x00010000;
            } else {
                Ship->OrderStateData = OrderData;
            }
            Ship->InHyperspace = true;
            Ship->CurrentPlanet = nullptr;
        };
        if (aGalaxy::Galaxy->KellerMissionState == 2) {
            aGalaxy::Galaxy->KellerMissionState = 3;
            {
                const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Holes) - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        Hole = pas::list_at<aGalaxy::THole>(aGalaxy::Galaxy->Holes, I);
                        if (Hole->HoleType == 0) {
                            Hole->HoleType = 3;
                        }
                    }
                }
            }
            Hole = pas::construct_call<aGalaxy::THole>(aGalaxy::THole_Create);
            Hole->InitializeGraphic(pas::WideString());
            reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->SetState(1);
            Hole->Star1 = aGalaxy::Galaxy->KellerTargetStar;
            Hole->Star2 = Hole->Star1;
            Hole->HoleType = 4;
            Hole->CreatedTurn = aGalaxy::Galaxy->CurrentTurn;
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::NextRandomIntRange(0, 359, RandomState));
            Radius = aMyFunction::NextRandomIntRange(2000, 3000, RandomState);
            Hole->Position1.X = static_cast<float>(System::Sin(Angle) * Radius);
            Hole->Position1.Y = static_cast<float>(System::Cos(Angle) * Radius);
            Hole->Position2.X = 1.0E+5f;
            Hole->Position2.Y = 1.0E+5f;
            pas::list_add(aGalaxy::Galaxy->Holes, reinterpret_cast<void*>(Hole));
            KellerShip->OrderNone(false);
            KellerShip->Order = aShip::soJumpHole;
            KellerShip->OrderTarget = Hole;
            KellerShip->OrderStateData = 0x00010003;
            KellerShip->InHyperspace = true;
            DominatorSpawnPlanet->CurrentStar = Hole->Star1;
            Series = Hole->Star1->Status.DominatorSeries;
            Hole->Star1->Status.DominatorSeries = aGalaxyStruct::dsKeller;
            Threshold = pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetDominatorAggressionLevel() * 1.25)) + 60;
            RandomMaximum = pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetDominatorAggressionLevel() * 0.125)) + 1;
            {
                std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, RandomMaximum, RandomState);
                Count = cpp_left + pas::real_round(aMyFunction::RemapClamped(
                            aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators), 0.0, Threshold,
                            12.0, 2.0));
            }
            if (aGalaxy::Galaxy->CurrentTurn >= 666) {
                if (aGalaxy::Galaxy->DominatorModLevel == 1) {
                    Count = 15;
                } else if (aGalaxy::Galaxy->DominatorModLevel == 2) {
                    Count = 17;
                } else if (aGalaxy::Galaxy->DominatorModLevel == 3) {
                    Count = 19;
                }
            }
            DominatorSpawnPlanet->GenerationSeed = Seed;
            DominatorSpawnPlanet->RandomState = RandomState;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(I); ) {
                SpawnReinforcement(I + 1);
            }
            Hole->Star1->Status.DominatorSeries = Series;
        }
    }

    std::uint8_t TKling::RelocateBertorWithinConstellation() {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        std::uint8_t Result = false;
        aGalaxy::TConstellation* Constellation = CurrentStar->Constellation;
        if (Constellation->Id == 20) {
            return Result;
        }
        std::int32_t EscortCount = 0;
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (!Ship->InNormalSpace()) {
                        continue;
                    }
                    if (TKling* kling = pas::class_cast_if<TKling*>(Ship); !(kling != nullptr)) {
                        if (!IsPlayerCamouflageEffective(Ship)) {
                            return Result;
                        }
                    } else {
                        if (kling->DominatorSeries != DominatorSeries) {
                            return Result;
                        }
                        if (pas::in_range(kling->KlingType, static_cast<std::int32_t>(aGalaxyStruct::ktEquantor),
                            static_cast<std::int32_t>(aGalaxyStruct::ktShtip)) && Ship != this) {
                            ++EscortCount;
                        }
                    }
                }
            }
        }
        if (EscortCount < 2) {
            return Result;
        }
        pas::List* Stars = pas::make_object<pas::List>();
        {
            const std::int32_t cpp_last_2 = pas::list_count(Constellation->Stars) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Star = pas::list_at<aGalaxy::TStar>(Constellation->Stars, I);
                    if (Star != CurrentStar && !aScript::IsStarProtectedByScript(Star) &&
                        Star->ShipTypeCounts[aGalaxyStruct::stKling] >= 6 && (KellerShip == nullptr ||
                            !KellerShip->InNormalSpace() || KellerShip->CurrentStar != Star) && (TerronShip ==
                            nullptr || !TerronShip->InNormalSpace() || TerronShip->CurrentStar != Star) &&
                        (BlazerShip == nullptr || !BlazerShip->InNormalSpace() || BlazerShip->CurrentStar != Star) &&
                        pas::in_range(Star->Status.ControlFaction,
                            static_cast<std::int32_t>(aGalaxyStruct::sfDominators),
                            static_cast<std::int32_t>(aGalaxyStruct::sfDominators)) && Star->Status.DominatorSeries ==
                            DominatorSeries && Star->Status.Battle == 0 && Star->Status.CustomFaction == u"") {
                        pas::list_add(Stars, reinterpret_cast<void*>(Star));
                    }
                }
            }
        }
        if (pas::list_count(Stars) > 2) {
            I = pas::list_indexof(Stars, reinterpret_cast<void*>(TransitOriginStar));
            if (I >= 0) {
                pas::list_delete(Stars, I);
            }
        }
        if (pas::list_count(Stars) > 0) {
            {
                aGalaxy::TStar* cpp_arg = pas::list_at<aGalaxy::TStar>(Stars,
                    aMyFunction::NextRandomIntRange(0, pas::list_count(Stars) - 1, RandomState));
                aShip::TShip* self = this;
                self->OrderJump(cpp_arg, true);
            }
            Result = true;
        }
        pas::free(Stars);
        return Result;
    }

    std::int32_t TKling::SpawnEscortShips(aGalaxyStruct::TKlingType Kind, std::int32_t DesiredCount) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::int32_t Result = 0;
        if (DesiredCount < 1) {
            return Result;
        }
        std::int32_t EscortCount = 0;
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (pas::list_at<aShip::TShip>(CurrentStar->Ships, I)->PartnerShip == this) {
                        ++EscortCount;
                    }
                }
            }
        }
        if (TransitOriginStar != nullptr) {
            const std::int32_t cpp_last_2 = pas::list_count(TransitOriginStar->Ships) - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    if (pas::list_at<aShip::TShip>(TransitOriginStar->Ships, I)->PartnerShip == this) {
                        ++EscortCount;
                    }
                }
            }
        }
        if (aMyFunction::NextRandomIntRange(0, 2 * DesiredCount, RandomState) < DesiredCount - EscortCount) {
            return Result;
        }
        DominatorSpawnPlanet->CurrentStar = CurrentStar;
        aGalaxyStruct::TDominatorSeries Series = CurrentStar->Status.DominatorSeries;
        CurrentStar->Status.DominatorSeries = DominatorSeries;
        std::int32_t SpawnTimer = CurrentStar->DaysSinceLastNpcShipSpawn;
        DominatorSpawnPlanet->GenerationSeed = Seed;
        DominatorSpawnPlanet->RandomState = Seed;
        while (Result + EscortCount < DesiredCount) {
            Ship = static_cast<aShip::TShip*>(DominatorSpawnPlanet->SpawnDominatorShip(Kind));
            if (Ship == nullptr) {
                break;
            }
            ++Result;
            Ship->PartnerShip = this;
            Ship->Position.X = Position.X;
            Ship->Position.Y = Position.Y;
            Ship->CurrentPlanet = nullptr;
            Ship->DockedTo = this;
            Ship->OrderTakeoff();
            Ship->MovementDirection = aMyFunction::PointBearingDegrees(Ship->Position, Ship->OrderDestination);
        }
        CurrentStar->Status.DominatorSeries = Series;
        CurrentStar->DaysSinceLastNpcShipSpawn = SpawnTimer;
        return Result;
    }

    void TKling::CoordinateSeriesInvasions(aGalaxyStruct::TDominatorSeries Series) {
        aGalaxy::TStar* TargetStar{};
        std::int32_t TargetCount{};
        std::int32_t NonDominatorCount{};
        std::int32_t OtherSeriesCount{};
        std::int32_t Action{};
        aGalaxy::TStar* Origin{};
        std::int32_t NearbyRange{};
        std::int32_t OriginCount{};
        double TargetStrength{};
        double NonDominatorStrength{};
        double OtherSeriesStrength{};
        double OriginStrength{};
        std::int32_t SendCount{};
        std::int32_t SendIndex{};
        std::int32_t Sent{};
        aShip::TShip* Ship{};
        std::int32_t MinimumPopulation{};
        std::int32_t Attempts{};
        std::int32_t NearbyIndex{};
        double Strength{};
        auto ChooseAction = [&]() -> void {
            auto TargetIsNotTerron = [&]() -> std::uint8_t {
                std::uint8_t Result = true;
                if (TerronShip != nullptr && TerronShip->CurrentStar == TargetStar) {
                    return false;
                }
                return Result;
            };
            // Actions 1/3 reinforce; 2/7 rebalance; 4/6 invade; 5 supports an existing foothold.
            if (Series == aGalaxyStruct::dsTerron && TerronShip->CurrentStar == TargetStar && (TargetCount <
                    NonDominatorCount || TargetCount < OtherSeriesCount)) {
                Action = 1;
            } else if (TargetStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                if (TargetStar->Status.DominatorSeries == Series && TargetStar->Status.CustomFaction == u"") {
                    if (aKling::OtherSeriesDistanceIndex(Origin, Series) > NearbyRange) {
                        if (aKling::OtherSeriesDistanceIndex(Origin, Series) > aKling::OtherSeriesDistanceIndex(
                                TargetStar, Series) && OtherSeriesCount == 0 && TargetCount > 0 && OriginCount >
                                static_cast<float>(TargetCount * 0x1.999999999999bp-1)) {
                            Action = 7;
                        }
                    }
                    if (OtherSeriesCount == 0 && TargetCount > 0 && OriginCount > 4 * TargetCount) {
                        Action = 2;
                    }
                    if (TargetCount < NonDominatorCount || TargetStrength < NonDominatorStrength) {
                        Action = 3;
                    }
                    {
                        bool cpp_condition = TargetCount < OtherSeriesCount || TargetStrength < OtherSeriesStrength;
                        if (cpp_condition) {
                            std::int32_t cpp_left = aMyFunction::NextRandomIntRange(0, 1000, this->RandomState) + 1000;
                            cpp_condition = cpp_left < aGalaxy::Galaxy->CurrentTurn;
                        }
                        if (cpp_condition) {
                            double cpp_left_2 = aMyFunction::NextRandomIntRange(0, 1000, this->RandomState);
                            if (cpp_left_2 > static_cast<float>(aGalaxy::TGalaxy::GetDominatorSeriesControlShare(Series) * 1.0E+3)) {
                                Action = 3;
                            }
                        }
                    }
                } else {
                    if (TargetCount > 0 && (TargetCount < OtherSeriesCount || TargetStrength < OtherSeriesStrength)) {
                        Action = 5;
                    }
                    {
                        bool cpp_condition_2 = Series != aGalaxyStruct::dsKeller && TargetIsNotTerron() &&
                            TargetCount == 0 && (OriginCount > 3 * OtherSeriesCount ||
                                aKling::NonDominatorDistanceIndex(Origin) > NearbyRange && OriginCount >
                                    static_cast<float>(OtherSeriesCount * 0x1.3333333333334p-1));
                        if (cpp_condition_2) {
                            std::int32_t cpp_left_3 = aMyFunction::NextRandomIntRange(0, 1000, this->RandomState) +
                                1000;
                            cpp_condition_2 = cpp_left_3 < aGalaxy::Galaxy->CurrentTurn;
                        }
                        if (cpp_condition_2) {
                            double cpp_left_4 = aMyFunction::NextRandomIntRange(0, 1400, this->RandomState);
                            if (cpp_left_4 > static_cast<float>(aGalaxy::TGalaxy::GetDominatorSeriesControlShare(Series) * 1.0E+3)) {
                                Action = 4;
                            }
                        }
                    }
                }
            } else {
                if (TargetCount > 0 && (TargetCount < NonDominatorCount || TargetStrength < NonDominatorStrength)) {
                    Action = 5;
                }
                if (Series != aGalaxyStruct::dsKeller && TargetCount == 0 && OtherSeriesCount == 0 &&
                    (NonDominatorCount < OriginCount || NonDominatorStrength < OriginStrength)) {
                    Action = 6;
                }
            }
        };
        auto SendShips = [&]() -> void {
            pas::WideString Text{};
            if (Action == 2) {
                SendCount = OriginCount / 4;
            } else if (Action == 7) {
                SendCount = OriginCount / 6;
            } else {
                SendCount = OriginCount - OriginCount / 4;
            }
            SendIndex = 0;
            Sent = 0;
            // Native <= deliberately permits one more ship than SendCount.
            while (SendIndex < pas::list_count(Origin->Ships) && Sent <= SendCount && SendCount > 0) {
                Ship = pas::list_at<aShip::TShip>(Origin->Ships, SendIndex);
                ++SendIndex;
                if (TKling* kling = pas::class_cast_if<TKling*>(Ship); kling != nullptr && kling->DominatorSeries ==
                        Series && static_cast<std::uint8_t>(Ship->OrderAbsolute ^ 1) && !Ship->IsOutsideStarSpace() &&
                    Ship != BlazerShip && Ship != KellerShip && Ship != TerronShip && pas::in_range(kling->KlingType,
                        static_cast<std::int32_t>(aGalaxyStruct::ktEquantor),
                        static_cast<std::int32_t>(aGalaxyStruct::ktShtip)) && kling->ActiveProgramAppliedTurn <= 0) {
                    Ship->OrderJump(TargetStar, true);
                    ++Sent;
                }
            }
            if (Action == 6 && Sent > 0 && TargetStar->Status.ControlFaction != aGalaxyStruct::sfDominators &&
                TargetStar->Status.CustomFaction == u"") {
                if (aPlayer::GetPlayer() != nullptr &&
                    aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
                    Text = ([&] {
                            auto name = pas::borrow(TargetStar->Name);
                            pas::WideString localizedText = aConst::LocalizedText(
                                u"Artefacts.ArtAnalyzer.AttackDomik"_wref.get());
                            pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                            return aMyFunction::FormatText1(std::move(localizedText), std::move(textHighlightColorTag),
                                u"<Star>"_w, name.get());
                        }());
                    if (Text != u"") {
                        Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, Text,
                            u""_wref.get());
                    }
                }
            }
        };
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        std::uint8_t ControlPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
        std::int32_t BaseChance = pas::real_round(aGalaxy::Galaxy->ScaleDifficultyExponentially(
            aGalaxy::Galaxy->GetDominatorAggressionLevel(), 1.0E+1f, 2.0f));
        std::int32_t ControlThreshold = pas::real_round(static_cast<float>(5 * aGalaxy::Galaxy->GetDominatorAggressionLevel() / 8.0)) + 80;
        std::int32_t Chance = pas::real_round(aMyFunction::RemapClamped(ControlPercent, 1.0, ControlThreshold,
            BaseChance, 0.0));
        Chance = pas::real_round(static_cast<float>(static_cast<double>(Chance) * aMyFunction::RemapClamped(
                    aGalaxy::Galaxy->WarDeltaWin[1], 0.0, 1.0E+1, 1.0, 0.3)));
        if (aGalaxy::Galaxy->CurrentTurn >= 666) {
            if (aGalaxy::Galaxy->DominatorModLevel == 1) {
                Chance = 400;
            } else if (aGalaxy::Galaxy->DominatorModLevel == 2) {
                Chance = 600;
            } else if (aGalaxy::Galaxy->DominatorModLevel == 3) {
                Chance = 800;
            }
        }
        if (Chance == 0) {
            return;
        }
        switch (Series) {
            case aGalaxyStruct::dsKeller: NearbyRange = 9; break;
            case aGalaxyStruct::dsBlazer: NearbyRange = 8; break;
            case aGalaxyStruct::dsTerron: NearbyRange = 7; break;
        }
        switch (Series) {
            case aGalaxyStruct::dsKeller: MinimumPopulation = 11; break;
            case aGalaxyStruct::dsBlazer: MinimumPopulation = 9; break;
            case aGalaxyStruct::dsTerron: MinimumPopulation = 7; break;
        }
        std::int32_t I = 0;
        while (I <= pas::list_count(aGalaxy::Galaxy->Stars) - 1) {
            Origin = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            ++I;
            if (Origin->Status.ControlFaction == aGalaxyStruct::sfDominators && Origin->Status.Battle == 0 &&
                Origin->Status.DominatorSeries == Series && Origin->Status.CustomFaction == u"") {
                if (aMyFunction::NextRandomIntRange(1, 1000, aGalaxy::Galaxy->RandomState) <= Chance &&
                    Origin->CountForcesByOwnerGroups(Strength, true, false, true, true) <= 0 &&
                    Origin->CountDominatorForces(Series, false, true, Strength) <= 0) {
                    OriginCount = Origin->CountDominatorForces(Series, true, false, OriginStrength);
                    if (OriginCount >= MinimumPopulation) {
                        Action = 0;
                        Attempts = NearbyRange;
                        NearbyIndex = aMyFunction::NextRandomIntRange(1, NearbyRange, aGalaxy::Galaxy->RandomState);
                        while (Attempts > 0 && Action == 0) {
                            TargetStar =
                                pas::checked_cast<aGalaxy::TStar*>(
                                    static_cast<pas::Object*>(Origin->StarDistances[NearbyIndex].Star));
                            NearbyIndex = aMyFunction::IncrementWrapped(NearbyIndex, 1, NearbyRange);
                            --Attempts;
                            if (TargetStar->Constellation->Id != 20 && TargetStar != Origin &&
                                !aScript::IsStarProtectedByScript(TargetStar)) {
                                if (aGalaxy::Galaxy->CurrentTurn > aGalaxyStruct::GalaxyWarmupTurns ||
                                    aPlayer::GetPlayer()->CurrentStar != TargetStar &&
                                        aMyFunction::PointDistanceSquared(TargetStar->Position,
                                            aPlayer::GetPlayer()->CurrentStar->Position) >= pas::sqr(static_cast<float>(static_cast<float>((static_cast<float>(1.0 -
                                            static_cast<float>(aGalaxy::Galaxy->CurrentTurn /
                                            pas::constant(static_cast<double>(aGalaxyStruct::GalaxyWarmupTurns))))) *
                                            7.0E+1) + 35.0))) {
                                    if (BlazerShip == nullptr || aGalaxy::Galaxy->BlazerLandingPlanetId == 0 ||
                                        BlazerShip->CurrentStar != TargetStar) {
                                        TargetCount = TargetStar->CountDominatorForces(Series, false, false,
                                                TargetStrength);
                                        OtherSeriesCount = TargetStar->CountDominatorForces(Series, false, true,
                                                OtherSeriesStrength);
                                        NonDominatorCount = TargetStar->CountForcesByOwnerGroups(NonDominatorStrength,
                                                true, false, true, true);
                                        ChooseAction();
                                        {
                                            bool cpp_condition = Action != 6 && Action != 5 && Action != 3;
                                            if (!cpp_condition) {
                                                std::int32_t cpp_left = aMyFunction::NextRandomIntRange(1, 60,
                                                    RandomState);
                                                cpp_condition = cpp_left > TargetStar->PlayerPresenceLevel;
                                            }
                                            if (cpp_condition) {
                                                if (Action != 0) {
                                                    SendShips();
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
    }

    static std::int32_t NonDominatorDistanceIndex(aGalaxy::TStar* Star) {
        std::int32_t I{};
        std::int32_t Result = 1000;
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (Star->StarDistances[I].Star->Status.ControlFaction != aGalaxyStruct::sfDominators &&
                        Star->StarDistances[I].Star->Status.CustomFaction == u"") {
                        return I;
                    }
                }
            }
        }
        return Result;
    }

    static std::int32_t OtherSeriesDistanceIndex(aGalaxy::TStar* Star, aGalaxyStruct::TDominatorSeries& Series) {
        std::int32_t I{};
        aGalaxy::TStar* OtherStar{};
        std::int32_t Result = 1000;
        {
            const std::int32_t cpp_last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    OtherStar = Star->StarDistances[I].Star;
                    if (OtherStar->Status.ControlFaction != aGalaxyStruct::sfDominators || Series !=
                            OtherStar->Status.DominatorSeries || OtherStar->Status.CustomFaction != u"") {
                        return I;
                    }
                }
            }
        }
        return Result;
    }

    void TKling::MoveToRandomPatrolPoint() {
        aMyFunction::TPolarPoint Polar{};
        aPlanet::TPlanet* Planet = pas::list_at<aPlanet::TPlanet>(CurrentStar->Planets, 0);
        Polar = Planet->Orbit;
        Polar.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
        if (aMyFunction::NextRandomIntRange(0, 50, RandomState) == 0 && 4 * aGalaxy::Galaxy->GetAIJunkToleranceLevel() <
                pas::list_count(CurrentStar->Items)) {
            Polar.Radius = static_cast<float>(CurrentStar->ComputeMapDiameter() / 2.0);
        }
        OrderMove(aMyFunction::PolarToPoint(Polar), false);
    }

    void TKling::MoveNearKellerMissionHole() {
        EC_Struct::TPointF Destination{};
        float Angle{};
        float Radius{};
        aGalaxy::THole* Hole = aGalaxy::Galaxy->FindHoleInStarByKind(CurrentStar, 4);
        if (Hole != nullptr) {
            if (Hole->Star1 == CurrentStar) {
                Destination = Hole->Position1;
            } else {
                Destination = Hole->Position2;
            }
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::NextRandomIntRange(0, 359, RandomState));
            Radius = aMyFunction::NextRandomIntRange(100, 150, RandomState);
            Destination.X = static_cast<float>(Destination.X + static_cast<float>(System::Sin(Angle) * Radius));
            Destination.Y = static_cast<float>(Destination.Y - static_cast<float>(System::Cos(Angle) * Radius));
            OrderMove(Destination, false);
        }
    }

    void TKling::RepairBrokenEquipmentAtLocation() {
        std::int32_t I{};
        aItem::TEquipment* Equipment{};
        if (CurrentPlanet != nullptr) {
            GetHull()->HullPoints = GetHull()->Weight;
        }
        {
            const std::int32_t cpp_last = pas::list_count(Inventory) - 1;
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (Equipment->BrokenFlag != 0 || Equipment->ConditionPercent < 1.0E+1) {
                        Equipment->Repair();
                    }
                }
            }
        }
    }

    aGalaxy::TStar* TKling::GetHomeStar() {
        return nullptr;
    }

    pas::WideString TKling::GetName() {
        return Name;
    }

    pas::WideString TKling::GetFullName(const pas::WideString& Separator) {
        pas::WideString Path{};
        pas::WideString Text{};
        if (TypeNameOverrideKey != u"") {
            Path = pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[DominatorSeries], u".",
                    TypeNameOverrideKey});
            if (GR_Main::LanguageDataConfig->CountParamsByPath(Path) > 0) {
                Text = aConst::LocalizedText(Path);
            } else {
                Text = aConst::LocalizedText(pas::concat_wide({u"ShipType.TypeName.", TypeNameOverrideKey}));
            }
            if (Text != u"") {
                return pas::concat_wide({Text, Separator, Name});
            }
            return Name;
        } else if (KlingType == aGalaxyStruct::ktBoss) {
            return aConst::DominatorShipDefinitions[KlingType].DisplayNames[DominatorSeries];
        } else {
            return pas::concat_wide({aConst::DominatorShipDefinitions[KlingType].DisplayNames[DominatorSeries],
                Separator, Name});
        }
    }

    aGalaxyStruct::TGreetingShipCategory TKling::GetGreetingShipCategory() {
        return aGalaxyStruct::gscKling;
    }

    // Always rcWarrior.
    aGalaxyStruct::TRangerCareer TKling::GetDominantCareer() {
        return aGalaxyStruct::rcWarrior;
    }

    aGalaxyStruct::TPercent TKling::GetStrengthScaledPirateStatus() {
        return 100;
    }

    std::int32_t TKling::GetDesiredCargoFreeSpace() {
        return 0;
    }

    // Fills installed fuel tanks without charging Money.
    void TKling::RefuelAtLocation() {
        if (GetFuelTanks() != nullptr) {
            GetFuelTanks()->Fuel = GetFuelTanks()->Capacity;
        }
    }

    void TKling::SetInventoryDominatorOwner() {
        std::int32_t I{};
        aItem::TEquipment* Equipment{};
        {
            const std::int32_t cpp_last = pas::list_count(Inventory) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
                    Equipment->OwnerId = aGalaxyStruct::oiDominator;
                    Equipment->DominatorSeries = DominatorSeries;
                }
            }
        }
    }

    void TKling::ImproveStandardEquipment() {
        std::int32_t I{};
        aItem::TEquipment* Equipment{};
        {
            const std::int32_t cpp_last = pas::list_count(Inventory) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Equipment = pas::list_at<aItem::TEquipment>(Inventory, I);
                    if (Equipment->HasStandardStats() && aMyFunction::NextRandomUnitFloat(RandomState) < 0.25) {
                        switch (aGalaxy::Galaxy->GetDifficultyTierIndex()) {
                            case 0: {
                                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.8) {
                                    Equipment->Improve(aItem::ikMinor);
                                } else {
                                    Equipment->Improve(aItem::ikAny);
                                }
                                break;
                            }
                            case 1: {
                                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.6) {
                                    Equipment->Improve(aItem::ikMinor);
                                } else {
                                    Equipment->Improve(aItem::ikAny);
                                }
                                break;
                            }
                            case 2: {
                                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.6) {
                                    Equipment->Improve(aItem::ikMedium);
                                } else {
                                    Equipment->Improve(aItem::ikAny);
                                }
                                break;
                            }
                            case 3: {
                                if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.6) {
                                    Equipment->Improve(aItem::ikMajor);
                                } else {
                                    Equipment->Improve(aItem::ikAny);
                                }
                                break;
                            }
                            default: Equipment->Improve(aItem::ikMajor); break;
                        }
                    }
                }
            }
        }
    }

    std::uint8_t TKling::RelationToNonRanger(aShip::TShip* Ship) {
        std::uint8_t Result{};
        if (TKling* kling = pas::class_cast_if<TKling*>(Ship)) {
            if (kling->DominatorSeries == DominatorSeries) {
                Result = 100;
            } else {
                Result = 40;
            }
        } else {
            Result = 0;
        }
        if (CurrentStanding == aGalaxyStruct::ssNeutral) {
            return 100;
        }
        return Result;
    }

    std::uint8_t TKling::RelationToRanger(void* Ranger) {
        std::uint8_t Result = 0;
        if (CurrentStanding == aGalaxyStruct::ssNeutral) {
            return 100;
        }
        return Result;
    }

    void TKling::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
    }

    void TKling::ReactToAttack(aShip::TShip* Attacker) {
        if (Attacker->OwnerId != aGalaxyStruct::oiDominator) {
            EnemyShip = Attacker;
        } else if (pas::checked_cast<TKling*>(Attacker)->DominatorSeries != DominatorSeries) {
            EnemyShip = Attacker;
        }
    }

    std::uint8_t TKling::AcceptsAppealFrom(aShip::TShip* Ship) {
        return Ship->OwnerId == OwnerId;
    }

    void TKling::AssignWeaponTargetsInStar() {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        aItem::TWeapon* Weapon{};
        aItem::TItem* Item{};
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
        std::int32_t Assigned = 0;
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace()) {
            const std::int32_t cpp_last_2 = static_cast<std::int32_t>(WeaponCount);
            if (1 <= cpp_last_2) {
                for (J = 1; J <= cpp_last_2; ++J) {
                    Weapon = Weapons[J];
                    if (Weapon->Target == nullptr && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                        if (static_cast<double>(aMyFunction::PointDistanceSquared(Position, EnemyShip->Position)) <=
                            pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                            Weapon->Target = EnemyShip;
                            ++Assigned;
                            if (Assigned == WeaponCount && !pas::in_set<aGalaxyStruct::ktBoss, aGalaxyStruct::ktUrgant,
                                    aGalaxyStruct::ktBertor, aGalaxyStruct::ktBertor>(KlingType)) {
                                return;
                            }
                        }
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_3 = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last_3) {
                for (I = 0; I <= cpp_last_3; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (Ship->InNormalSpace() && Ship != this && (aShip::TShip_RelationToShip(this, Ship) <= 50 ||
                            IsProgramActive(aGalaxyStruct::prgInsanity)) &&
                        static_cast<std::uint8_t>(IsPlayerCamouflageEffective(Ship) ^ 1)) {
                        const std::int32_t cpp_last_4 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_4) {
                            for (J = 1; J <= cpp_last_4; ++J) {
                                Weapon = Weapons[J];
                                if ((Weapon->Target == nullptr || Weapon->GetWeaponInfo()->ShotType ==
                                        aGalaxyStruct::wstAreaDamage) && aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                    if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                        Ship->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                        if (Weapon->Target == nullptr) {
                                            Weapon->Target = Ship;
                                            ++Assigned;
                                            if (Assigned == WeaponCount && !pas::in_set<aGalaxyStruct::ktBoss,
                                                    aGalaxyStruct::ktUrgant, aGalaxyStruct::ktBertor,
                                                    aGalaxyStruct::ktBertor>(KlingType)) {
                                                return;
                                            }
                                        } else if (reinterpret_cast<aShip::TShip*>(
                                            Weapon->Target)->GetHull()->HullPoints < Ship->GetHull()->HullPoints) {
                                            Weapon->Target = Ship;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (Assigned >= WeaponCount) {
            return;
        }
        {
            const std::int32_t cpp_last_5 = pas::list_count(CurrentStar->Missiles) - 1;
            if (0 <= cpp_last_5) {
                for (I = 0; I <= cpp_last_5; ++I) {
                    Missile = pas::list_at<aMissile::TMissile>(CurrentStar->Missiles, I);
                    if (Missile->Target == this && Missile->OwnerShip != this) {
                        const std::int32_t cpp_last_6 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_6) {
                            for (J = 1; J <= cpp_last_6; ++J) {
                                Weapon = Weapons[J];
                                if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                    static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                    static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Target == nullptr &&
                                    aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                    if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                        Missile->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                        Weapon->Target = Missile;
                                        ++Assigned;
                                        if (Assigned == WeaponCount) {
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
        {
            const std::int32_t cpp_last_7 = pas::list_count(CurrentStar->Items) - 1;
            if (0 <= cpp_last_7) {
                for (I = 0; I <= cpp_last_7; ++I) {
                    Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                    if ((Item->ScriptItem == nullptr ||
                        reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") &&
                        CanSafelyDetonateItem(Item)) {
                        const std::int32_t cpp_last_8 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_8) {
                            for (J = 1; J <= cpp_last_8; ++J) {
                                Weapon = Weapons[J];
                                if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                    static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo),
                                    static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Target == nullptr &&
                                    aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                    if (static_cast<double>(aMyFunction::PointDistanceSquared(Position,
                                        Item->Position)) <= pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) {
                                        Weapon->Target = Item;
                                        ++Assigned;
                                        if (Assigned == WeaponCount) {
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
        if (aPlayer::GetPlayer()->CurrentStar == CurrentStar) {
            const std::int32_t cpp_last_9 = pas::list_count(CurrentStar->Asteroids) - 1;
            if (0 <= cpp_last_9) {
                for (I = 0; I <= cpp_last_9; ++I) {
                    Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                    Distance = aMyFunction::PointDistanceSquared(Position, Asteroid->Position);
                    if (Distance <= pas::constant(static_cast<double>(aGalaxyStruct::AsteroidTargetRangeSquared))) {
                        const std::int32_t cpp_last_10 = static_cast<std::int32_t>(WeaponCount);
                        if (1 <= cpp_last_10) {
                            for (J = 1; J <= cpp_last_10; ++J) {
                                Weapon = Weapons[J];
                                // Native asteroid targeting can overwrite an existing assignment.
                                if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType,
                                    static_cast<std::int32_t>(aGalaxyStruct::wstAreaDamage),
                                    static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) &&
                                    aShip::TShip_IsEquipmentUsable(this, Weapon)) {
                                    if (static_cast<double>(pas::sqr(aShip::TShip_GetWeaponRange(this, Weapon))) >=
                                        Distance) {
                                        Weapon->Target = Asteroid;
                                        ++Assigned;
                                        if (Assigned == WeaponCount) {
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
    }

    // Marks this series as aware of the player's camouflage and reports a matching active disguise.
    void TKling::DetectAttackingPlayer(aShip::TShip* Attacker) {
        if (aPlayer::GetPlayer() == Attacker && Attacker->CurrentStar == CurrentStar &&
            static_cast<std::uint8_t>(
                reinterpret_cast<aPlayer::TPlayer*>(Attacker)->ChameleonDetected[DominatorSeries] ^ 1) &&
            aPlayer::GetPlayer()->ChameleonLogic[DominatorSeries] < 2 && !HasIndependentScriptFaction()) {
            reinterpret_cast<aPlayer::TPlayer*>(Attacker)->ChameleonDetected[DominatorSeries] = true;
            if (reinterpret_cast<aPlayer::TPlayer*>(Attacker)->ChameleonActive &&
                reinterpret_cast<aPlayer::TPlayer*>(Attacker)->ChameleonSeries == DominatorSeries) {
                const pas::WideString& localizedText = aConst::LocalizedText(
                    u"ShipInfo.AddInfo.Chameleon.Detect"_wref.get());
                std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, currentTurn, localizedText, u""_wref.get());
            }
        }
    }

    // Can mark the player's camouflage as detected by this Dominator series. Returns false for non-player ships.
    std::uint8_t TKling::IsPlayerCamouflageEffective(aShip::TShip* Ship) {
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer() == Ship && !HasIndependentScriptFaction() &&
            !Ship->IsOutsideStarSpace() &&
            (static_cast<std::uint8_t>(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ChameleonDetected[DominatorSeries] ^
                1) || aPlayer::GetPlayer()->ChameleonLogic[DominatorSeries] >= 2)) {
            if ((static_cast<std::uint8_t>(reinterpret_cast<aPlayer::TPlayer*>(Ship)->ChameleonActive ^ 1) ||
                reinterpret_cast<aPlayer::TPlayer*>(Ship)->ChameleonSeries != DominatorSeries) &&
                aPlayer::GetPlayer()->ChameleonLogic[DominatorSeries] == 0) {
                reinterpret_cast<aPlayer::TPlayer*>(Ship)->ChameleonDetected[DominatorSeries] = true;
                return Result;
            }
            return true;
        }
        return Result;
    }

    void TKling::SelectEnemyShipInStar() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        double Distance{};
        double BestDistance{};
        if (EnemyShip == nullptr || EnemyShip->CurrentStar != CurrentStar || static_cast<double>(Speed * Speed) <
                aMyFunction::PointDistanceSquared(Position, EnemyShip->Position) || !EnemyShip->InNormalSpace() &&
                (!EnemyShip->IsDocked() || EnemyShip->ConsecutiveDockedDays >= 2)) {
            if (UsableWeaponCount != 0) {
                EnemyShip = nullptr;
                BestDistance = 1.0E+5;
                {
                    const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
                    if (0 <= cpp_last) {
                        for (I = 0; I <= cpp_last; ++I) {
                            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                            if (Ship->InNormalSpace()) {
                                if (HasIndependentScriptFaction()) {
                                    if (Ship->HasIndependentScriptFaction() &&
                                        reinterpret_cast<aScript::TScriptShip*>(ScriptShip)->StateText ==
                                            reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText) {
                                        continue;
                                    }
                                } else if (Ship->OwnerId == aGalaxyStruct::oiDominator &&
                                    pas::checked_cast<TKling*>(Ship)->DominatorSeries == DominatorSeries &&
                                    Ship->CurrentStanding != aGalaxyStruct::ssCustom ||
                                    IsPlayerCamouflageEffective(Ship)) {
                                    continue;
                                }
                                if (EnemyShip == nullptr || !pas::in_range(Ship->TypeId,
                                        static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter),
                                        static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) || pas::in_range(
                                        EnemyShip->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter),
                                        static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                                    Distance = aMyFunction::PointDistance(Position, Ship->Position);
                                    if (static_cast<float>(static_cast<double>(aMyFunction::NextRandomFloatRange(0.3, 3.0, RandomState)) *
                                        BestDistance) > Distance) {
                                        EnemyShip = Ship;
                                        BestDistance = Distance;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void TKling::EngageEnemyShip() {
        if (Order == aShip::soFollowShip) {
            OrderNone(false);
        }
        if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar) {
            if (EnemyShip->InNormalSpace()) {
                if (ShouldKamikaze()) {
                    OrderFollowShip(EnemyShip, aShip::fmKamikaze, false);
                } else {
                    OrderFollowShip(EnemyShip, aShip::fmMinWeaponRange, false);
                }
            } else if (EnemyShip->CurrentPlanet != nullptr) {
                OrderMove(EnemyShip->CurrentPlanet->GetPosition(), false);
            }
        }
    }

    void TKling::UpdateAfterburnerState() {
        AfterburnerActive = ShouldKamikaze() && GetSlotCount(aConst::sskAfterburner) > 0 && GetEngine() != nullptr &&
                GetEngine()->ConditionPercent > 1.0E+1;
        RefreshDerivedStats(true);
    }

    void TKling::ProcessCombatDialogue() {
    }

    void TKling::ReactToExtortionDemand(void* Ranger) {
    }

    std::uint8_t TKling_BuildMoneyExtortionResponse(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t DemandedAmount) {
        return false;
    }

    std::uint8_t TKling_BuildCargoExtortionResponse(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response) {
        return false;
    }

    std::uint8_t TKling::BuildTrucePaymentResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t OfferedAmount) {
        return false;
    }

    std::uint8_t TKling_BuildAttackRequestResponse(TKling* Self, aShip::TShip* Requester, pas::WideString& Response,
        aShip::TShip* Target) {
        return false;
    }

    std::uint8_t TKling_AcceptPartnershipOffer(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    std::uint8_t TKling_BuildPartnershipOfferResponse(TKling* Self, aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        std::uint8_t Result = false;
        Response = u"Not supporting"_w;
        return Result;
    }

    void TKling::RefreshCombatSkills() {
        std::uint8_t Percent{};
        std::int32_t Lower{};
        std::int32_t Upper{};
        std::int32_t Maximum{};
        std::int32_t Minimum{};
        std::int32_t Threshold{};
        std::int32_t StartPercent{};
        std::int32_t MidPercent{};
        if (KlingType == aGalaxyStruct::ktBoss) {
            BaseSkills[aGalaxyStruct::psAccuracy] = 6;
            BaseSkills[aGalaxyStruct::psManeuverability] = 6;
            BaseSkills[aGalaxyStruct::psLeadership] = 6;
        } else {
            FreeExperience = aGalaxy::Galaxy->CurrentTurn;
            Minimum = 0;
            Maximum = pas::real_round(aGalaxy::Galaxy->InterpolateDifficulty(-1, 2.0f, 3.0f, 4.0f, 6.0f));
            Maximum = std::min<std::int32_t>(Maximum, 6);
            Threshold = pas::real_round(static_cast<float>(aGalaxy::Galaxy->GetEffectiveDifficultyLevel() * 1.25)) + 65;
            if (aGalaxy::Galaxy->CurrentTurn >= 666) {
                if (aGalaxy::Galaxy->DominatorModLevel == 1) {
                    Maximum = 6;
                    Minimum = 1;
                    Threshold = 100;
                } else if (aGalaxy::Galaxy->DominatorModLevel == 2) {
                    Maximum = 6;
                    Minimum = 3;
                    Threshold = 100;
                } else if (aGalaxy::Galaxy->DominatorModLevel == 3) {
                    Maximum = 6;
                    Minimum = 5;
                    Threshold = 100;
                }
            }
            StartPercent = 1;
            MidPercent = Threshold / 2;
            Percent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
            Lower = pas::real_round(aMyFunction::RemapClamped(Percent, StartPercent, MidPercent, Maximum, Minimum));
            Upper = pas::real_round(aMyFunction::RemapClamped(Percent, MidPercent, Threshold, Maximum, Minimum));
            Lower = std::min<std::int32_t>(Lower, Upper);
            BaseSkills[aGalaxyStruct::psAccuracy] = aMyFunction::NextRandomIntRange(Lower, Upper, RandomState);
            BaseSkills[aGalaxyStruct::psManeuverability] = aMyFunction::NextRandomIntRange(Lower, Upper, RandomState);
            BaseSkills[aGalaxyStruct::psTechnical] = 6;
        }
        TechKnowledge = 8;
    }

    // Bosses have a minimum calculated speed of 350.
    std::int32_t TKling::CalculateSpeed() {
        std::int32_t Speed = aShip::TShip::CalculateSpeed();
        if (KlingType == aGalaxyStruct::ktBoss) {
            Speed = std::max<std::int32_t>(350, Speed);
        }
        return Speed;
    }

    std::uint8_t TKling::HasNearbyBertorAura() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        std::uint8_t Result = false;
        if (KlingType == aGalaxyStruct::ktBoss) {
            return Result;
        }
        if (KlingType == aGalaxyStruct::ktBertor) {
            return Result;
        }
        if (HasIndependentScriptFaction()) {
            return Result;
        }
        {
            const std::int32_t cpp_last = pas::list_count(CurrentStar->Ships) - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                    if (TKling* kling = pas::class_cast_if<TKling*>(Ship); kling != nullptr && kling->KlingType ==
                            aGalaxyStruct::ktBertor && kling->DominatorSeries == DominatorSeries && Ship != this &&
                        Ship->InNormalSpace() && !Ship->IsHullDestroyed() && aMyFunction::PointDistance(Position,
                            Ship->Position) <= 5.0E+2) {
                        return true;
                    }
                }
            }
        }
        return Result;
    }

    void TKling_RefreshCurrentStanding(TKling* Self) {
        aGalaxyStruct::TScriptStandingOverrideMode StandingMode = Self->GetScriptStandingOverrideMode();
        if (StandingMode == aGalaxyStruct::ssmCustomFaction) {
            Self->CurrentStanding = aGalaxyStruct::ssCustom;
        } else if (StandingMode != aGalaxyStruct::ssmFixed) {
            if (Self == BlazerShip && aGalaxy::Galaxy->BlazerLandingPlanetId != 0) {
                Self->CurrentStanding = aGalaxyStruct::ssNeutral;
            } else {
                Self->CurrentStanding = aGalaxyStruct::ssDominator;
            }
        }
    }

    void TKling::p_destroy() {
        aKling::TKling_Destroy(this);
    }

    void TKling::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aKling::TKling_ResolveLoadedReferences(this, Galaxy);
    }

    void TKling::virtual_TShip_NextDay() {
        aKling::TKling_NextDay(this);
    }

    void TKling::virtual_TShip_NextDayLogic() {
        aKling::TKling_NextDayLogic(this);
    }

    std::uint8_t TKling::virtual_TShip_CanQueueReachablePlanet(aPlanet::TPlanet* Planet) {
        return aKling::TKling_CanQueueReachablePlanet(this, Planet);
    }

    std::uint8_t TKling::virtual_TShip_RecomputeFearState() {
        return aKling::TKling_RecomputeFearState(this);
    }

    std::uint8_t TKling::virtual_TShip_AcceptsRansomDemandFrom(aShip::TShip* Ship) {
        return aKling::TKling_AcceptsRansomDemandFrom(this, Ship);
    }

    std::uint8_t TKling::virtual_TShip_TrustsAttackRequester(aShip::TShip* Ship) {
        return aKling::TKling_TrustsAttackRequester(this, Ship);
    }

    std::uint8_t TKling::virtual_TShip_BuildMoneyExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t DemandedAmount) {
        return aKling::TKling_BuildMoneyExtortionResponse(this, OtherShip, Response, DemandedAmount);
    }

    std::uint8_t TKling::virtual_TShip_BuildCargoExtortionResponse(aShip::TShip* OtherShip, pas::WideString& Response) {
        return aKling::TKling_BuildCargoExtortionResponse(this, OtherShip, Response);
    }

    std::uint8_t TKling::virtual_TShip_BuildAttackRequestResponse(aShip::TShip* Requester, pas::WideString& Response,
        aShip::TShip* Target) {
        return aKling::TKling_BuildAttackRequestResponse(this, Requester, Response, Target);
    }

    std::uint8_t TKling::virtual_TShip_AcceptPartnershipOffer(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        return aKling::TKling_AcceptPartnershipOffer(this, OtherShip, Response, PaymentAmount);
    }

    std::uint8_t TKling::virtual_TShip_BuildPartnershipOfferResponse(aShip::TShip* OtherShip, pas::WideString& Response,
        std::int32_t PaymentAmount) {
        return aKling::TKling_BuildPartnershipOfferResponse(this, OtherShip, Response, PaymentAmount);
    }

    void TKling::virtual_TShip_RefreshCurrentStanding() {
        aKling::TKling_RefreshCurrentStanding(this);
    }

} // namespace aKling
